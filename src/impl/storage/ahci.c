#include "storage/ahci.h"
#include "storage/drive.h"
#include "storage/gpt.h"
#include "memory/vmm.h"
#include "pci.h"
#include "utils.h"
#include "intdt.h"
#include "hal.h"

hba_mem_t* ABAR;

uint8_t get_port_type(volatile hba_port_t* port)
{
    uint32_t ssts = port->ssts;
    uint8_t ipm = (ssts >> 8) & 0xF;
    uint8_t det = ssts & 0xF;

    if(det != HBA_PORT_DET_PRESENT)
        return AHCI_DEV_NULL;
    if(ipm != HBA_PORT_IPM_ACTIVE)
        return AHCI_DEV_NULL;

    switch(port->sig)
    {
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        default: 
            return AHCI_DEV_SATA;
    }
}

int __attribute__((noinline)) find_command_slot(volatile hba_port_t* port)
{
    uint32_t slots = (port->sact | port->ci);
    for(uint8_t i = 0; i < 32; i++)
    {
        if((slots & (1 << i)) == 0)
            return i;
    }

    //do something - error
    return -1;
}

uint8_t ahci_cmd(volatile hba_port_t* port, uint64_t lba, uint32_t count, uint16_t* buffer, uint8_t write)
{
    port->is = (uint32_t) -1;
    int spin = 0;
    int slot = find_command_slot(port);
    if(slot == -1)
        return 0;

    uint64_t cmd_addr = ((uint64_t)port->clb) | (((uint64_t)port->clbu) << 32);
    volatile hba_cmd_header_t* cmd_header = (hba_cmd_header_t*)HIGHER_HALF(cmd_addr);
    cmd_header += slot;
    cmd_header->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);
    cmd_header->w = write;
    cmd_header->c = 1;
    cmd_header->prdtl = (uint16_t)((count - 1) >> 4) + 1;

    uint64_t tbl_addr = ((uint64_t)cmd_header->ctba) | (((uint64_t)cmd_header->ctbau) << 32);
    volatile hba_cmd_tbl_t* cmd_tbl = (hba_cmd_tbl_t*)HIGHER_HALF(tbl_addr);
    memset(cmd_tbl,0,sizeof(hba_cmd_tbl_t) + (cmd_header->prdtl-1)*sizeof(hba_prdt_entry_t));

    //uintptr_t buf = ((uintptr_t)buffer) - 0xffffffff80000000;
    //uintptr_t buf = ((uintptr_t)buffer) -   0xffff800000000000;

    //printf("Buffer in addr: %ixq\n", buffer);
    //printf("Buffer addr: %ixq\n", buf);

    int i = 0;
    for(; i < cmd_header->prdtl - 1; i++)
    {
        cmd_tbl->prdt_entry[i].dba = (uint32_t)buffer;
        cmd_tbl->prdt_entry[i].dbc = 0x2000 - 1;
        cmd_tbl->prdt_entry[i].i = 1;
        buffer += 4096;
        count -= 16;
    }
    cmd_tbl->prdt_entry[i].dba = (uint32_t)buffer;
    cmd_tbl->prdt_entry[i].dbc = (count << 9) - 1;
    cmd_tbl->prdt_entry[i].i = 1;

    volatile fis_reg_h2d_t* cmd_fis = (fis_reg_h2d_t*)(&cmd_tbl->cfis);
    cmd_fis->fis_type = FIS_TYPE_REG_H2D;
    cmd_fis->c = 1;

    if(write == 0)
        cmd_fis->command = ATA_CMD_READ_DMA_EX;
    else
        cmd_fis->command = ATA_CMD_WRITE_DMA_EX;

    cmd_fis->lba0 = (uint8_t)lba;
    cmd_fis->lba1 = (uint8_t)(lba >> 8);
    cmd_fis->lba2 = (uint8_t)(lba >> 16);
    cmd_fis->lba3 = (uint8_t)(lba >> 24);
    cmd_fis->lba4 = (uint8_t)(lba >> 30);
    cmd_fis->lba5 = (uint8_t)(lba >> 40);

    cmd_fis->countl = count & 0xFF;
    cmd_fis->counth = (count >> 8) & 0xFF;

    cmd_fis->device = 1 << 6;

    while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }

    if(spin == 1000000)
    {
        //AHCI hung
        return 0;
    }
    port->ci = 1 << slot;

    for(;;)
    {
        if((port->ci & (1 << slot)) == 0)
            break;
        
        if(port->is & HBA_PxIS_TFES)
            return 0;
    }

    if(port->is & HBA_PxIS_TFES)
        return 0;

    while(port->ci != 0);
    return 1;
}

uint8_t ahci_write_drive(drive_t* drive, uint64_t lba, uint64_t count, uint8_t* buffer)
{
    volatile hba_port_t* port = &ABAR->ports[drive->port];
    return ahci_cmd(port, lba, count, (uint16_t*)buffer, 1);
}

uint8_t ahci_read_drive(drive_t* drive, uint64_t lba, uint64_t count, uint8_t* buffer)
{
    volatile hba_port_t* port = &ABAR->ports[drive->port];
    return ahci_cmd(port, lba, count, (uint16_t*)buffer, 0);
}

uint8_t detect_partition_table(drive_t* drive)
{
    gpt_t* gpt = (gpt_t*)drive->buffer;
    if(!drive->read(drive, 0x0, 1, gpt))
    {
        drive->flags &= ~DRIVE_DEV_PART_TABLE;
        return 0;
    }

    if(gpt->pmbr.boot_signature != 0xAA55)
    {
        drive->flags &= ~DRIVE_DEV_PART_TABLE;
        printf("Invalid MBR signature at AHCI port %iu\n", drive->port);
        return 0;
    }

    for(int i = 0; i < 4; i++)
    {
        if(gpt->pmbr.partitions[i].type == 0xEE)
        {
            drive->flags |= (DRIVE_DEV_PART_TABLE | DRIVE_DEV_GPT);
            printf("Found GPT partition table at AHCI port %iu\n", drive->port);
            return 1;
        }
    }

    printf("Found MBR partition table at AHCI port %iu\n", drive->port);
    drive->flags |= DRIVE_DEV_PART_TABLE;
    drive->flags &= ~DRIVE_DEV_GPT;

    return 2;
}

void probe_ports()
{
    uint32_t pi = ABAR->pi;
    for(int i = 0; i < 32; i++)
    {
        if(pi & (1 << i))
        {
            hba_port_t* port = &ABAR->ports[i];
            port->ie = 0;
            port->cmd |= 1;

            uint8_t type = get_port_type(port);
            if(type == AHCI_DEV_SATA)
            {
                printf("Found SATA device at port %iu\n", i);
                drive_t drive;
                str_cpy("SATA\0", &drive.label, 5);

                drive.flags = 1;
                drive.type = 0x53415441;

                drive.port = i;
                drive.read = &ahci_read_drive;
                drive.write = &ahci_write_drive;
                drive.buffer = (uint8_t*)malloc_page();

                detect_partition_table(&drive);
                find_partitions(&drive);
                register_drive(drive);
            }
        }
    }
}

void init_ahci()
{
    pci_entry_t* ahci_controller = pci_get_function(0x1, 0x6, 1);
    if(ahci_controller == 0x0)
    {
        printf("No AHCI controller found\n");
        return;
    }

    uintptr_t base = (uintptr_t)HIGHER_HALF(ahci_controller->header.bar[5]);
    ABAR = (hba_mem_t*)base;

    probe_ports();
}