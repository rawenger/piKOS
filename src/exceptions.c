/*
 * exceptions.c
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

#include "exceptions.h"
#include "peripherals/mini_uart.h"
#include "mmio.h"
#include "printk.h"

extern void uart0_irq_handler(void);

static int_handler_t KOS_handlers[KOS_IRQ_NUM] = {NULL};

void call_KOS_handler(IntType which)
{
	if (KOS_handlers[which])
		(*KOS_handlers[which]) ();
}

__attribute__((optimize(0))) // prevent compiler optimizing out `context` parameter
_Noreturn void InvalidExceptionHandler(int type, int currentEL, struct ExceptionContext *context)
{
#ifdef DEBUG
	muart_send_str("Invalid exception: ");
	switch (type) {
		case E_SYNC:
			muart_send_str("E_SYNC\r\n");
			break;
		case E_FIQ:
			muart_send_str("E_FIQ\r\n");
			break;
		case E_32BIT:
			muart_send_str("E_32BIT\r\n");
			break;
		case E_BADIRQ:
			muart_send_str("E_BADIRQ\r\n");
			break;
		case E_TODO:
			muart_send_str("E_TODO\r\n");
			break;
		default:
			muart_send_str("Unknown\r\n");
	}
#endif
	// TRAP here to read out members of `context` because
	// I don't feel like writing a routine to dump them
	while (1)
		;
}

__attribute__((optimize(2)))
void irq_handler(void)
{
	/* We have 3 registers for the pending IRQ's, but for our purposes
	 * the only IRQ's we *should* be dealing with are timer interrupts
	 * and console read/write interrupts (a.k.a. UART0 interrupts).
	 * Just to be safe, let's still just check everything
	 */
	u32 pending[3] = {
		vmmio_read32(ARM_IC_IRQ_PENDING_1),
		vmmio_read32(ARM_IC_IRQ_PENDING_2),
		vmmio_read32(ARM_IC_IRQ_BASIC_PENDING) & 0xFF,
	};

	for (unsigned reg = 0; reg < 3; reg++) {
		u32 pend = pending[reg];

		if (!pend)
			continue;

		/* Note: although __builtin_ctz(0) returns 0 on x86(_64),
		 * testing on aarch64 Linux with native GCC shows that it
		 * returns 32 there, which is what we want here.
		 */
		u32 irq_n = __builtin_ctz(pend);
		if (reg == 1 && irq_n == __builtin_ctz(ARM_IRQ_UART)) /* UART */ {
			uart0_irq_handler();
			return;
		}
		vmmio_write32(ARM_IC_IRQ_PENDING_1, 0);
		vmmio_write32(ARM_IC_IRQ_PENDING_2, 0);
		vmmio_write32(ARM_IC_IRQ_BASIC_PENDING, 0);
#ifdef DEBUG
		printk("IRQ #%d asserted on line %d\r\n", irq_n, reg);
#endif

	}
}



void register_isr(IntType which, int_handler_t handler)
{
	if (which >= KOS_IRQ_NUM)
		return;

	KOS_handlers[which] = handler;
}