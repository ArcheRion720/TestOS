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

#define COM_INTERRUPT(com)      ((com) + 1)
#define COM_FIFO(com)           ((com) + 2)
#define COM_LINE_CONTROL(com)   ((com) + 3)
#define COM_LINE_STATUS(com)    ((com) + 5)
#define COM_MODEM(com)          ((com) + 4)

#define COM_BAUD_LSB(com)       ((com))
#define COM_BAUD_MSB(com)       ((com) + 1)

#define COM_DLAB_BIT            0x80

#define COM_LINE_READY   (1 << 0)
#define COM_LINE_OVERRUN (1 << 1)
#define COM_LINE_PARITY  (1 << 2)
#define COM_LINE_FRAMING (1 << 3)
#define COM_LINE_BREAK   (1 << 4)
#define COM_LINE_BUFEMPT (1 << 5)
#define COM_LINE_FREE    (1 << 6)
#define COM_LINE_ERRORIM (1 << 7)

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

void clear_irq(uint8_t irq);
void mask_irq(uint8_t irq);

uintptr_t read_cr3();