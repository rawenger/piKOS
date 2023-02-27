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

volatile int busy;

static void enable_irq_controller()
{
//	mmio_write32(ARM_IC_ENABLE_IRQS_1, ARM_IRQ_TIMER1);
	mmio_write32(ARM_IC_ENABLE_IRQS_2, ARM_IRQ_UART);
}

_Noreturn void kernel_main(void)
{
        busy = 0;
	uart0_init();
	enable_irq_controller();
	enable_irq();

#ifdef DEBUG
	muart_init(); // miniUART
#endif

	uart0_send_str("Hello, world!\r\n");

	uint64_t el;
	asm volatile ("\tmrs %0, CurrentEL\n"
		      "\tlsr %0, %0, #2\n"
		      : "=r" (el));
	uart0_send_str("Running in EL ");
	uart0_send('0' + el);
	uart0_send_str("\r\n");

	char c;
	while (1) {
		c = uart0_recv();
		uart0_send(c);
	}
}

