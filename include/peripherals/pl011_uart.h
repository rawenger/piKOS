/*
 * uart0.h
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *  Copyright (C) 2023 Ryan Wenger
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "types.h"
#include "assert.h"
#include "mmio.h"

typedef struct _s_uart_dev {
	volatile u32 DR;                        // data register (offset 0x00)
	volatile u32 RSRECR;                    // recv status / error clear register (offset 0x04)
	u64 RESERVED1[2];                       // offsets 0x08 to 0x14
	volatile struct {
		u8 FR_RESERVED: 3,
		   BUSY: 1,
		   RXFE: 1,
		   TXFF: 1,
		   RXFF: 1,
		   TXFE: 1;
	} __attribute__((packed)) FR;           // flag register (offset 0x18)
	u8 RESERVED2[7];
	volatile u32 ILPR;                      // IrDA Low-power counter register (offset 0x20)
	volatile u32 IBRD;                      // Integer Baud Rate Register (offset 0x24)
	volatile u32 FBRD;                      // Fractional Baud Rate Register (offset 0x28)
	volatile struct {
		u8 BRK: 1;
		u8 PEN: 1;
		u8 EPS: 1;
		u8 STP2: 1;
		u8 FEN: 1;
		enum {
			UART_LCRH_WLEN5 = 0,
			UART_LCRH_WLEN6,
			UART_LCRH_WLEN7,
			UART_LCRH_WLEN8
		} WLEN: 2;
		u8 SPS: 1;
	} __attribute__((packed)) LCRH;         // Line control register (offset 0x2C)
	u8 RESERVED3[3];
	volatile struct {
		u16 EN: 1, // enable
		    CR_RESERVED: 6,
		    LBE: 1,
		    TXE: 1,
		    RXE: 1,
		    DTR: 1,
		    RTS: 1,
		    OUT1: 1,
		    OUT2: 1,
		    RTSEN: 1,
		    CTSEN: 1;
	} __attribute__((packed)) CR;           // Control Register (offset 0x30)
	u16 RESERVED4;
	volatile struct IFLS_Struct{
		enum UART_IFSEL {
			UART_IFSEL_1_8 = 0,
			UART_IFSEL_1_4,
			UART_IFSEL_1_2,
			UART_IFSEL_3_4,
			UART_IFSEL_7_8
		} TXIFSEL : 3;
		enum UART_IFSEL RXIFSEL : 3;
		// u8 IFLS_RESERVED: 2;
	} __attribute__((packed)) IFLS;         // Interrupt FIFO Level Select (offset 0x34)
	u8 RESERVED5[3];
	volatile struct {
		u16 RIMIM: 1,
		    CTSMIM: 1,
		    DCDMIM: 1,
		    DSRMIM: 1,
		    RXIM: 1,
		    TXIM: 1,
		    RTIM: 1,
		    FEIM: 1,
		    PEIM: 1,
		    BEIM: 1,
		    OEIM: 1,
		    IMSC_RESERVED: 5;
	} __attribute__((packed)) IMSC;         // Interrupt mask set/clear register (offset 0x38)
	u16 RESERVED6;
	// Note/to-do: define bit fields for this
	volatile u32 RIS;                       // Raw interrupt status register (offset 0x3C)
	volatile struct {
		u16 RIMMIS: 1,
		    CTSMMIS: 1,
		    DCDMMIS: 1,
		    DSRMMIS: 1,
		    RXMIS: 1,
		    TXMIS: 1,
		    RTMIS: 1,
		    FEMIS: 1,
		    PEMIS: 1,
		    BEMIS: 1,
		    OEMIS: 1,
		    MIS_RESERVED: 5;
	} __attribute__((packed)) MIS;          // Masked interrupt status register (offset 0x40)
	u16 RESERVED7;
	volatile struct {
		u16 RIMIC: 1,
		    CTSMIC: 1,
		    DCDMIC: 1,
		    DSRMIC: 1,
		    RXIC: 1,
		    TXIC: 1,
		    RTIC: 1,
		    FEIC: 1,
		    PEIC: 1,
		    BEIC: 1,
		    OEIC: 1,
		    ICR_RESERVED: 5;
	} __attribute__((packed)) ICR;          // Interrupt clear register (offset 0x44)
	u16 RESERVED8;
	volatile struct {
		u8 RXDMAE: 1,
		   TXDMAE: 1,
		   DMAONERR: 1,
		   DMACR_RESERVED: 5;
	} __attribute__((packed)) DMACR;        // DMA Control Register (offset 0x48)
} *uart_device;

static_assert(offsetof(struct _s_uart_dev, DR)          == 0x00);
static_assert(offsetof(struct _s_uart_dev, RSRECR)      == 0x04);
static_assert(offsetof(struct _s_uart_dev, FR)          == 0x18);
static_assert(offsetof(struct _s_uart_dev, ILPR)        == 0x20);
static_assert(offsetof(struct _s_uart_dev, IBRD)        == 0x24);
static_assert(offsetof(struct _s_uart_dev, FBRD)        == 0x28);
static_assert(offsetof(struct _s_uart_dev, LCRH)        == 0x2C);
static_assert(offsetof(struct _s_uart_dev, CR)          == 0x30);
static_assert(offsetof(struct _s_uart_dev, IFLS)        == 0x34);
static_assert(offsetof(struct _s_uart_dev, IMSC)        == 0x38);
static_assert(offsetof(struct _s_uart_dev, RIS)         == 0x3C);
static_assert(offsetof(struct _s_uart_dev, MIS)         == 0x40);
static_assert(offsetof(struct _s_uart_dev, ICR)         == 0x44);
static_assert(offsetof(struct _s_uart_dev, DMACR)       == 0x48);

#define ASSIGN_STRUCT(field, val)      do { (field) = (typeof(field)) val; } while (0)

#define MUART   ((uart_device) MUART_BASE) // don't use as a "regular" UART device! just here to simplify configuration
#define UART0   ((uart_device) (UART0_BASE | MMIO_VM_OFFSET))
#if RASPPI == 4
#define UART1   ((uart_device) (UART1_BASE | MMIO_VM_OFFSET))
#define UART2   ((uart_device) (UART2_BASE | MMIO_VM_OFFSET))
#define UART3   ((uart_device) (UART3_BASE | MMIO_VM_OFFSET))
#define UART4   ((uart_device) (UART4_BASE | MMIO_VM_OFFSET))
#endif

void uart_init(uart_device dev, int enable_irq);

void uart0_irq_handler(void);

__attribute__((unused))
void uart_send(uart_device dev, char c);

__attribute__((unused))
char uart_recv(uart_device dev);

inline void uart0_send(char c)
{ uart_send(UART0, (c)); }
