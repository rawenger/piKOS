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

#include "pikos_config.h"
#include "mmio.h"
#include "exceptions.h"
#include "util/utils.h"
#include "peripherals/pl011_uart.h"
#include "peripherals/gpio.h"
#include "peripherals/mini_uart.h"

#define UART0_CLOCK	        (48000000UL)

#define BRD(br)		        ((UART0_CLOCK) / (16.0 * (br)))
#define BRD_115200	        (BRD(115200ul))
#define IBRD_115200	        ((unsigned int)(BRD_115200))
#define FBRD_115200	        ((unsigned int)(((BRD_115200-IBRD_115200)*64)+0.5))

#define UART0_GPIO_TX           14, GPIO_AF0
#define UART0_GPIO_RX           15, GPIO_AF0
/*
 * Note: UART0 also supports Tx, Rx = 32, 33 on AF3 or Tx, Rx = 36, 37 on AF2
 */
#define UART1_GPIO_TX           0, GPIO_AF4
#define UART1_GPIO_RX           1, GPIO_AF4
#define UART2_GPIO_TX           4, GPIO_AF4
#define UART2_GPIO_RX           5, GPIO_AF4
#define UART3_GPIO_TX           8, GPIO_AF4
#define UART3_GPIO_RX           9, GPIO_AF4
#define UART4_GPIO_TX           12, GPIO_AF4
#define UART4_GPIO_RX           13, GPIO_AF4

struct UART_GPIO {
	struct pin {
		int GPIO_num;
		int AF_mode;
	} Tx;
	struct pin Rx;
};

static struct UART_GPIO GPIOs[] = {
	{.Tx = {14, GPIO_AF0}, .Rx = {15, GPIO_AF0}},
	{}, // <-- RESERVED
#if RASPPI == 4
        {.Tx = {0, GPIO_AF4},  .Rx = {1, GPIO_AF4}},
	{.Tx = {4, GPIO_AF4},  .Rx = {5, GPIO_AF4}},
	{.Tx = {8, GPIO_AF4},  .Rx = {9, GPIO_AF4}},
	{.Tx = {12, GPIO_AF4}, .Rx = {13, GPIO_AF4}},
#endif
};

void uart_init(uart_device dev, int enable_irq)
{
#ifdef USE_MUART
	if (dev == MUART) {
		muart_init();
		return;
	}
#endif
	struct GPIOPin Tx, Rx;
	u64 uart_num = ((uintptr) dev - (uintptr) UART0) / 512;
	GPIOPin_init(&Tx, GPIOs[uart_num].Tx.GPIO_num, GPIOs[uart_num].Tx.AF_mode);
	GPIOPin_init(&Rx, GPIOs[uart_num].Rx.GPIO_num, GPIOs[uart_num].Rx.AF_mode);

	ASSIGN_STRUCT(dev->CR, {0}); // disable

	// setup baud rate
	dev->IBRD = IBRD_115200;
	dev->FBRD = FBRD_115200;

	// set 8-bit word, enable FIFO
	ASSIGN_STRUCT(dev->LCRH, {0});
	dev->LCRH.WLEN = UART_LCRH_WLEN8;
	dev->LCRH.FEN = 1;

	// enable hardware flow control
	dev->CR.CTSEN = 1;
	dev->CR.RTSEN = 1;

	// enable Tx & Rx interrupts
	ASSIGN_STRUCT(dev->IMSC, {0});
	dev->IMSC.RXIM = dev->IMSC.TXIM = 1;

	// set Rx & Tx FIFO interrupt trigger levels to 1/8 full
	ASSIGN_STRUCT(dev->IFLS, {0});
	dev->IFLS.RXIFSEL = UART_IFSEL_1_8;
	dev->IFLS.TXIFSEL = UART_IFSEL_1_8;

	// enable tx,rx
	dev->CR.EN = dev->CR.TXE = dev->CR.RXE = 1;
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

void uart_send(uart_device dev, char c)
{
	while (dev->FR.TXFF)
		;
	dev->DR = c;
}

char uart_recv(uart_device dev)
{
	while (dev->FR.RXFE)
		;
	return dev->DR;
}
