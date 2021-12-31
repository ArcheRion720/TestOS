#pragma once
#include "storage/drive.h"

struct ext2_super_block
{
    uint32_t total_inodes;
    uint32_t total_blocks;
    uint32_t super_blocks;
    uint32_t unallocated_blocks;
    uint32_t unallocated_inodes;
    uint32_t block_super_block;
    uint32_t log2_block_size;
    uint32_t log2_fragment_size;
    uint32_t blocks_per_group;
    uint32_t fragments_per_group;
    uint32_t inodes_per_group;
    uint32_t last_mount_time;
    uint32_t last_write_time;
    uint16_t fsck;
    uint16_t fsck_limit;
    uint16_t signature;
    uint16_t state;
    uint16_t error_behaviour;
    uint16_t minor_version;
    uint32_t fsck_last;
    uint32_t fsck_inteval;
    uint32_t os_id;
    uint32_t major_version;
    uint16_t user_id;
    uint16_t group_id;

    //Extended
    uint32_t first_non_rsv_inode;
    uint16_t size_inode;
    uint16_t backup_group;
    uint32_t optional_features;
    uint32_t required_features;
    uint32_t must_support_features;
    uint8_t file_system_id[16];
    uint8_t volume_name[16];
    uint8_t path_volume[64];
    uint32_t compression;
    uint8_t preallocate_file_blocks;
    uint8_t preallocate_dir_blocks;
    uint16_t unused;
    uint8_t journal_id[16];
    uint32_t journal_inode;
    uint32_t journal_device;
    uint32_t orphan_inode_list;
} __attribute__((packed));
typedef struct ext2_super_block ext2_super_block_t;

struct block_group_desc
{
    uint32_t block_bitmap;
    uint32_t inode_bitmap;
    uint32_t inode_table;
    uint16_t free_blocks;
    uint16_t free_inodes;
    uint16_t num_dirs;
    uint8_t unused[14];
} __attribute__((packed));
typedef struct block_group_desc block_group_desc_t;

struct inode
{
    //uint16_t type : 4;
    //uint16_t permissions : 12;
    uint16_t type;
    uint16_t user_id;
    uint32_t lower_size;
    uint32_t last_access_time;
    uint32_t creation_time;
    uint32_t last_modification_time;
    uint32_t deletion_time;
    uint16_t group_id;
    uint16_t hard_links_count; //directory entries
    uint32_t used_sectors;
    uint32_t flags;
    uint32_t os_specific1;
    uint32_t DBP[12];
    uint32_t SIBP;
    uint32_t DIBP;
    uint32_t TIBP;
    uint32_t generation_number;
    uint32_t file_ACL;
    union
    {
        uint32_t upper_size;
        uint32_t directory_ACL;
    };
    uint32_t fragment_address;
    uint32_t os_specific2;
} __attribute__ ((packed));
typedef struct inode inode_t;

struct directory_entry
{
    uint32_t inode;
    uint16_t size;
    union
    {
        uint16_t name_length;
        struct
        {
            uint8_t name_length_low;
            uint8_t type;
        };
    };
    uint8_t name[];
} __attribute__ ((packed));
typedef struct directory_entry directory_entry_t;

struct ext2_payload
{
    ext2_super_block_t superblock;
    uint32_t block_size;
    uint32_t groups;
    block_group_desc_t descs[];
};
typedef struct ext2_payload ext2_fs_t;

uint8_t discover_ext2_fs(drive_t* drive);
ext2_super_block_t read_super_block(partition_t* part);
