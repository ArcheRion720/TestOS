#pragma once
#define PIC_MASTER_CMD 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_CMD 0xA0
#define PIC_SLAVE_DATA 0xA1
#define PIC_CMD_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8
#define LINE_STATUS(com) com + 5

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define RTC_REGISTER_A 0x0A
#define RTC_REGISTER_B 0x0B
#define RTC_REGISTER_C 0x0C

#include <stdint.h>

void io_wait(void);

void irq_unmask(uint8_t irq);
void send_eoi(uint8_t irq);

uint8_t inport8(uint16_t portId);
uint16_t inport16(uint16_t portId);
uint32_t inport32(uint16_t portId);

void outport8(uint16_t portId, uint8_t value);
void outport16(uint16_t portId, uint16_t value);
void outport32(uint16_t portId, uint32_t value);

uint8_t init_serial(uint16_t com);
uint8_t in_serial(uint16_t com);
void out_serial(uint16_t com, uint8_t value);
void out_serial_str(uint16_t com, const char* str, uint32_t length);

uintptr_t read_cr3();