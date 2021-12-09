#pragma once

struct ext2_super_block
{
    uint32_t total_inodes;
    uint32_t total_blocks;
    uint32_t super_blocks;
    uint32_t unallocated_blocks;
    uint32_t unallocated_inodes;
    uint32_t lba_super_block;
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
};

typedef struct ext2_super_block ext2_super_block_t;

struct ext2_extended_block
{
    ext2_super_block_t base;
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
};

typedef struct ext2_extended_block ext2_extended_block_t;
