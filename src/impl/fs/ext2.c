#include "fs/ext2.h"
#include "fs/fs.h"
#include "memory/pmm.h"
#include "macros.h"
#include "array.h"
#include "utils.h"

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

void read_test(const char* path, file_system_t* fs)
{
    //For testing purpose write contents of root directory
    inode_t inode = read_inode(fs, 2);
    
    if(!(inode.type & 0x4000))
        return;

    uint8_t* buffer = malloc(4096);
    for(int i = 0; i < 12; i++)
    {
        if(inode.DBP[i] == 0)
            continue;

        read_block(fs->partition, inode.DBP[i], 4096, buffer);
        directory_entry_t* dir_entry = (directory_entry_t*)buffer;
        while(1)
        {
            if(dir_entry->inode == 0)
                break;

            printf("\t- %s\n", dir_entry->name);
            dir_entry = (directory_entry_t*)(((uint8_t*)dir_entry) + dir_entry->size);
        }
    }
    free(buffer, 4096);
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

            fs->read = &read_test;
            ///TODO: proper fs->read 
            ///TODO: proper fs->write

            parts_found++;
        }
    }
    free(sblock, 0x400);
    return parts_found;
}