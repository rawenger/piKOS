/*
 * mini_uart.c - UART1 device driver for debug piKOS line
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *  Copyright (C) 2023 Ryan Wenger
 *
 * The muart_init function comes pretty much as-is from
 *      https://github.com/AlMazyr/raspberry-pi-os/blob/lesson01/src/lesson01/src/mini_uart.c
 *      Licensed under MIT
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

#include "util/utils.h"
#include "peripherals/mini_uart.h"
#include "mmio.h"

#define BAUD_RATE_REG_CALC(br)	((SYSTEM_CLOCK_FREQ / (8 * (br))) - 1)

#define BAUD_RATE_115200	BAUD_RATE_REG_CALC(115200ul)

void muart_send(char c)
{
	while ( !(vmmio_read32(MUART_LSR_REG) & (1 << 5)) ) ;

	vmmio_write32(MUART_IO_REG, c);
}

void muart_send_str(char* str)
{
	while (*str)
		muart_send(*str++);
}


__attribute__((unused))
char muart_recv(void)
{
	while( !(vmmio_read32(MUART_LSR_REG) & 1) ) ;
	return vmmio_read32(MUART_IO_REG) & 0xFF;
}

void muart_init(void)
{
	unsigned int selector;

	selector = vmmio_read32(ARM_GPIO_GPFSEL1);
	selector &= ~(7<<12);                   // clean gpio14
	selector |= 2<<12;                      // set alt5 for gpio14
	selector &= ~(7<<15);                   // clean gpio15
	selector |= 2<<15;                      // set alt5 for gpio15
	vmmio_write32(ARM_GPIO_GPFSEL1, selector);

	vmmio_write32(ARM_GPIO_GPPUD, 0);
	delay(150);
	vmmio_write32(ARM_GPIO_GPPUDCLK0, (1 << 14) | (1 << 15));
	delay(150);
	vmmio_write32(ARM_GPIO_GPPUDCLK0, 0);

	vmmio_write32(MUART_EN, 1);                   //Enable mini uart (this also enables access to it registers)
	vmmio_write32(MUART_CR_REG, 0);               //Disable auto flow control and disable receiver and transmitter (for now)
	vmmio_write32(MUART_IER_REG, 0);                //Disable receive and transmit interrupts
	vmmio_write32(MUART_LCR_REG, 3);                //Enable 8 bit mode
	vmmio_write32(MUART_MCR_REG, 0);                //Set RTS line to be always high
	vmmio_write32(MUART_BD_REG, BAUD_RATE_115200); //Set baud rate to 115200

	vmmio_write32(MUART_CR_REG, 0x3); // enable transmitter and receiver
}
