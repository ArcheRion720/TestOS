#include "fs/ext2.h"
#include "memory/pmm.h"
#include "utils.h"
#include "string.h"

void read_block(partition_t* part, uint64_t block, uint64_t block_size, uint8_t* buffer)
{
    uint64_t sectors = (block_size >> 9);
    part->drive->read(part->drive, part->lba_start + (block * sectors), sectors, buffer);
}

inode_t read_inode(file_system_t* fs, uint32_t index)
{
    inode_t result;
    ext2_fs_t* ext2 = (ext2_fs_t*)fs->metadata;
    uint64_t inode_size = 128;

    if(ext2->superblock.major_version >= 1)
        inode_size = ext2->superblock.size_inode;

    uint64_t group = (index - 1) / ext2->superblock.inodes_per_group;
    uint64_t gridx = (index - 1) % ext2->superblock.inodes_per_group;
    uint64_t block = (gridx * inode_size) / ext2->block_size;

    ///FIXME: Breaks itself if inode is not in first block of group

    block_group_desc_t desc = ext2->descs[group];

    uint8_t* buffer = malloc(ext2->block_size);
    uint8_t* ptr = buffer;
    read_block(fs->partition, desc.inode_table + block, ext2->block_size, buffer);

    ptr += gridx * inode_size;
    result = *((inode_t*)ptr);

    free(buffer, ext2->block_size);

    return result;
}

bool_t query_subnode(inode_t* inode, string_t search, file_system_t* fs, uint32_t* pindex)
{
    if(!(inode->type & 0x4000))
        return false;

    uint8_t* buffer = malloc(4096);
    for(int i = 0; i < 12; i++)
    {
        if(inode->DBP[i] == 0)
            continue;

        read_block(fs->partition, inode->DBP[i], 4096, buffer);
        directory_entry_t* dir_entry = (directory_entry_t*)buffer;
        while(1)
        {
            if(dir_entry->inode == 0)
                break;

            string_t str;
            str.data = dir_entry->name;
            str.length = dir_entry->name_length;

            if(str_cmp(str, search))
            {
                if(pindex)
                {
                    *pindex = dir_entry->inode;
                }
                free(buffer, 4096);
                return true;
            }

            dir_entry = (directory_entry_t*)(((uint8_t*)dir_entry) + dir_entry->size);
        }
    }
    free(buffer, 4096);
    return false;
}

void ext2_list_directory(inode_t* directory, file_system_t* fs)
{
    if(!(directory->type & 0x4000))
        return;

    uint8_t* buffer = malloc(4096);
    for(uint32_t i = 0; i < 12; i++)
    {
        if(directory->DBP[i] == 0)
            continue;

        read_block(fs->partition, directory->DBP[i], 4096, buffer);
        directory_entry_t* dir_entry = (directory_entry_t*)buffer;
        while(1)
        {
            if(dir_entry->inode == 0)
                break;

            string_t str;
            str.data = dir_entry->name;
            str.length = dir_entry->name_length;

            if(dir_entry->type == 1)
            {
                printf("\t- %iu %s. [File]\n", read_inode(fs, dir_entry->inode).lower_size, str.data, str.length, dir_entry->inode);
            }
            else if(dir_entry->type == 2)
            {
                printf("\t- %iu %s. [%iu]\n", read_inode(fs, dir_entry->inode).lower_size, str.data, str.length, dir_entry->inode);
            }
            dir_entry = (directory_entry_t*)(((uint8_t*)dir_entry) + dir_entry->size);
        }
    }
    free(buffer, 4096);
}

void ext2_list(string_t path, file_system_t* fs)
{
    //sanity check
    if(!str_starts(path, '/'))
    {
        return;
    }

    inode_t inode;
    string_split_t split;
    uint32_t inode_index;

    //Root directory
    if(path.length == 1)
    {
        inode = read_inode(fs, 2);
        printf("Contents of '/':\n");
        ext2_list_directory(&inode, fs);
        return;
    }

    //uint8_t assert_dir = str_ends(path, '/');

    split = splitstr(path, '/');
    while(1)
    {
        if(split.left.length == 0)
        {
            if(split.right.length == 0)
                break;
            
            inode = read_inode(fs, 2);
            split = splitstr(split.right, '/');
            continue;
        }

        if(query_subnode(&inode, split.left, fs, &inode_index))
        {
            inode = read_inode(fs, inode_index);
        }
        else
        {
            printf("Directory '%s.' not found!\n", split.left, split.left.length);
            return;
        }

        split = splitstr(split.right, '/');
    }

    printf("Contents of '%s.':\n", path.data, path.length);
    ext2_list_directory(&inode, fs);
}

uint8_t discover_ext2_fs(drive_t* drive)
{
    uint8_t parts_found = 0;
    ext2_super_block_t* sblock = malloc(0x400);
    for(uint8_t i = 0; i < drive->partition_count; i++)
    {
        if(drive->partitions[i].flags & PART_RESERVED)
            continue;

        drive->read(drive, drive->partitions[i].lba_start + 0x2, 2, (uint8_t*)sblock);
        if(sblock->signature == 0xEF53)
        {
            drive->partitions[i].file_system = FS_EXT2;
            
            file_system_t* fs = register_filesystem();
            fs->partition = &drive->partitions[i];
            
            uint32_t groups = ALIGN_UP(sblock->total_blocks, sblock->blocks_per_group) / sblock->blocks_per_group;
            fs->metadata = malloc
                        (
                            sizeof(ext2_super_block_t) +
                            sizeof(uint32_t) +
                            sizeof(uint32_t) +
                            (sizeof(block_group_desc_t) * groups)
                        );

            ext2_fs_t* efs = (ext2_fs_t*)fs->metadata;
            efs->superblock = *sblock;
            efs->groups = groups;
            efs->block_size = (1024 << sblock->log2_block_size);

            ///TODO: Consider group descs spanning multiple blocks
            block_group_desc_t* buffer = malloc(1024 << sblock->log2_block_size);
            ///TODO: Consider read block according to size of block
            read_block(&drive->partitions[i], 1, 1024 << sblock->log2_block_size, (uint8_t*)buffer);
            for(uint32_t i = 0; i < groups; i++)
            {
                efs->descs[i] = buffer[i];
            }
            free(buffer, 1024 << sblock->log2_block_size);

            fs->list = &ext2_list;
            ///TODO: proper fs->read 
            ///TODO: proper fs->write

            parts_found++;
        }
    }
    free(sblock, 0x400);
    return parts_found;
}