/*
 * uart0.c
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

#include "mmio.h"
#include "exceptions.h"
#include "util/utils.h"
#include "peripherals/uart0.h"
#include "peripherals/gpio.h"
#include "peripherals/mini_uart.h"

#define UART0_CLOCK	        (48000000UL)

#define BRD(br)		        ((UART0_CLOCK) / (16.0 * (br)))
#define BRD_115200	        (BRD(115200ul))
#define IBRD_115200	        ((unsigned int)(BRD_115200))
#define FBRD_115200	        ((unsigned int)(((BRD_115200-IBRD_115200)*64)+0.5))

void uart0_init()
{
	struct GPIOPin Tx, Rx;
	GPIOPin_init(&Tx, 14, GPIO_AF0);
	GPIOPin_init(&Rx, 15, GPIO_AF0);

	vmmio_write32(UART0_CR, 0); // disable
	vmmio_write32(UART0_IBRD, IBRD_115200); // setup br 115200
	vmmio_write32(UART0_FBRD, FBRD_115200);
	vmmio_write32(UART0_LCRH, LCRH_WLEN8_MASK | LCRH_FEN_MASK); // set 8-bit word, enable fifo

	// enable hardware flow control
	vmmio_write32(UART0_CR, CR_CTSEN_MASK | CR_RTSEN_MASK);

	// enable Tx & Rx interrupts
	vmmio_write32(UART0_IMSC, IMSC_RXIM | IMSC_TXIM);
	// set Rx & Tx FIFO interrupt trigger levels to 1/8 full
	vmmio_write32(UART0_IFLS, (IFLS_IFSEL_1_8 << IFLS_RXIFSEL_SHIFT) | (IFLS_IFSEL_1_8 << IFLS_TXIFSEL_SHIFT));

	// enable tx,rx
	vmmio_write32(UART0_CR, CR_UART_EN_MASK | CR_TXE_MASK | CR_RXE_MASK);
}

__attribute__((optimize(2)))
void uart0_irq_handler(void)
{
	/* purposely don't buffer this! we will do that in a separate kernel thread (watch_keyboard) */
	static char console_read_char;

	u32 int_type = vmmio_read32(UART0_MIS);
	vmmio_write32(UART0_ICR, int_type);

	if (int_type & MIS_RXMIS) {
		console_read_char = vmmio_read32(UART0_DR);
#ifdef DEBUG
		printk("Received character: '%c'\r\n", console_read_char);
#endif
		call_KOS_handler(ConsoleReadInt);
	}
	if (int_type & MIS_TXMIS) {
		/* console_write() is what actually accesses the DR here */
		call_KOS_handler(ConsoleWriteInt);
	}

#ifdef DEBUG
	if (int_type & ~(MIS_TXMIS | MIS_RXMIS))
		printk("other interrupt occurred in uart0\r\n");
#endif
}

void uart0_send(char c)
{
	while (vmmio_read32(UART0_FR) & FR_TXFF_MASK)
		;
	vmmio_write32(UART0_DR, c);
}

char uart0_recv(void)
{
	while (vmmio_read32(UART0_FR) & FR_RXFE_MASK)
		;
	return vmmio_read32(UART0_DR);
}
