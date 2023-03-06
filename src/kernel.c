/*
 * kernel.c
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


#include <stdint.h>

#include "mmio.h"
#include "types.h"
#include "util/utils.h"
#include "peripherals/uart0.h"
#include "peripherals/mini_uart.h"

static void irq_init()
{
	mmio_write32(ARM_IC_FIQ_CONTROL, 0); // completely disable FIQ's -- Linux does not use them so neither will we
	mmio_write32(ARM_IC_DISABLE_IRQS_1, -1);
	mmio_write32(ARM_IC_DISABLE_IRQS_2, -1);
	mmio_write32(ARM_IC_DISABLE_BASIC_IRQS, -1);
	mmio_write32(ARM_LOCAL_TIMER_INT_CONTROL0, 0);

//	mmio_write32(ARM_IC_ENABLE_IRQS_1, ARM_IRQ_TIMER1);
	mmio_write32(ARM_IC_ENABLE_IRQS_2, ARM_IRQ_UART);

	enable_irq();
}

_Noreturn void kernel_main(void)
{
	delay(1000);

#ifdef DEBUG
	muart_init(); // miniUART
	muart_send_str("miniUART initialized\r\n");
#endif

	uart0_init();

	irq_init();

	uint64_t el;
	asm volatile ("\tmrs %0, CurrentEL\n"
		      "\tlsr %0, %0, #2\n"
		      : "=r" (el));
	muart_send_str("Running in EL ");
	muart_send('0' + el);
	muart_send_str("\r\n");

	char c;
	muart_send_str("here\r\n");
	//asm volatile ("\nsvc #0\t");
	while (1) {
//		c = uart0_recv();
//		uart0_send(c);
	}
}

