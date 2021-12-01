#pragma once

#include <stdint.h>
#include <stddef.h>

//AHCI
#define AHCI_DEV_NULL   0
#define AHCI_DEV_SATA   1

#define HBA_PORT_IPM_ACTIVE     1
#define HBA_PORT_DET_PRESENT    3

#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000
#define HBA_PxIS_TFES   (1 << 30)

//SATA
#define	SATA_SIG_ATA    0x00000101
#define	SATA_SIG_ATAPI	0xEB140101
#define	SATA_SIG_SEMB	0xC33C0101
#define	SATA_SIG_PM	    0x96690101

#define ATA_DEV_BUSY    0x80
#define ATA_DEV_DRQ     0x08

#define ATA_CMD_READ_DMA_EX     0x25
#define ATA_CMD_WRITE_DMA_EX    0x35

typedef enum 
{
	FIS_TYPE_REG_H2D	= 0x27,
	FIS_TYPE_REG_D2H	= 0x34,
	FIS_TYPE_DMA_ACT	= 0x39,
	FIS_TYPE_DMA_SETUP	= 0x41,
	FIS_TYPE_DATA		= 0x46,
	FIS_TYPE_BIST		= 0x58,
	FIS_TYPE_PIO_SETUP	= 0x5F,
	FIS_TYPE_DEV_BITS	= 0xA1,
} FIS_TYPE;

struct fis_reg_h2d
{
	uint8_t fis_type;
	uint8_t pmport : 4;
	uint8_t rsv0 : 3;
	uint8_t c : 1;
	uint8_t command;
	uint8_t featurel;
	uint8_t lba0;
	uint8_t lba1;
	uint8_t lba2;
	uint8_t device;
	uint8_t lba3;
	uint8_t lba4;
	uint8_t lba5;
	uint8_t featureh;
	uint8_t countl;
	uint8_t counth;
	uint8_t icc;
	uint8_t control;
	uint8_t reserved[4];
} __attribute__((packed)) ;
typedef struct fis_reg_h2d fis_reg_h2d_t;

struct fis_reg_d2h
{
	uint8_t fis_type;
	uint8_t pmport : 4;
	uint8_t rsv0 : 2;
	uint8_t i : 1;
	uint8_t rsv1 : 1;
	uint8_t status;
	uint8_t error;
	uint8_t lba0;
	uint8_t lba1;
	uint8_t lba2;
	uint8_t device;
	uint8_t lba3;
	uint8_t lba4;
	uint8_t lba5;
	uint8_t rsv2;
	uint8_t countl;
	uint8_t counth;
	uint8_t rsv3[6];
};
typedef struct fis_reg_d2h fis_reg_d2h_t;

struct fis_data 
{
    uint8_t fis_type;
	uint8_t pmport:4;
	uint8_t rsv0:4;
	uint8_t rsv1[2];

	uint32_t data[1];
};
typedef struct fis_data fis_data_t;

struct fis_dma_setup
{
	uint8_t fis_type;
	uint8_t pmport : 4;
	uint8_t rsv0 : 1;
	uint8_t d : 1;
	uint8_t i : 1;
	uint8_t a : 1;
	uint8_t rsv1[2];
	uint64_t dma_buffer_id;
	uint32_t rsv2;
	uint32_t dma_buffer_offset;
	uint32_t dma_count;
	uint32_t rsv3;
};
typedef struct fis_dma_setup fis_dma_setup_t;

volatile struct hba_port
{
	volatile uint32_t clb;
	volatile uint32_t clbu;
	volatile uint32_t fb;
	volatile uint32_t fbu;
	volatile uint32_t is;
	volatile uint32_t ie;
	volatile uint32_t cmd;
	uint32_t rsv0;
	volatile uint32_t tfd;
	volatile uint32_t sig;
	volatile uint32_t ssts;
	volatile uint32_t sctl;
	volatile uint32_t serr;
	volatile uint32_t sact;
	volatile uint32_t ci;
	volatile uint32_t sntf;
	volatile uint32_t fbs;
	uint32_t rsv1[11];
	volatile uint32_t vendor[4];
};
typedef volatile struct hba_port hba_port_t;

volatile struct hba_mem
{
	volatile uint32_t cap;
	volatile uint32_t ghc;
	volatile uint32_t is;
	volatile uint32_t pi;
	volatile uint32_t version;
	volatile uint32_t ccc_ctl;
	volatile uint32_t ccc_pts;
	volatile uint32_t em_loc;
	volatile uint32_t em_ctl;
	volatile uint32_t cap2;
	volatile uint32_t bohc;
	
	uint8_t reserved[116];
	volatile uint8_t vendor[96];

	volatile hba_port_t ports[1];
};
typedef volatile struct hba_mem hba_mem_t;

volatile struct hba_cmd_header
{
	uint8_t cfl : 5;
	uint8_t a : 1;
	uint8_t w : 1;
	uint8_t p : 1;
	uint8_t r : 1;
	uint8_t b : 1;
	uint8_t c : 1;
	uint8_t rsv0 : 1;
	uint8_t pmport : 4;
	volatile uint16_t prdtl;
	uint32_t prdbc;
	uint32_t ctba;
    uint32_t ctbau;
	uint32_t rsv1[4];
};
typedef volatile struct hba_cmd_header hba_cmd_header_t;

struct hba_prdt_entry
{
	uint32_t dba;
	uint32_t dbau;
	uint32_t rsv0;
	uint32_t dbc : 22;
	uint32_t rsv1 : 9;
	uint32_t i : 1;
};
typedef struct hba_prdt_entry hba_prdt_entry_t;

struct hba_cmd_tbl
{
	uint8_t cfis[64];
	uint8_t acmd[16];
	uint8_t rsv[48];

	hba_prdt_entry_t prdt_entry[1];
};
typedef struct hba_cmd_tbl hba_cmd_tbl_t;

void init_ahci();