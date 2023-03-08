//
// Created by ryan on 2/25/23.
//

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
_Noreturn void InvalidExceptionHandler(int type, struct ExceptionContext *context)
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

__attribute__((optimize(3)))
void irq_handler(void)
{
	/* We have 3 registers for the pending IRQ's, but for our purposes
	 * the only IRQ's we *should* be dealing with are timer interrupts
	 * and console read/write interrupts (a.k.a. UART0 interrupts).
	 * Just to be safe, let's still just check everything
	 */
	uint32_t pending[3] = {
		mmio_read32(ARM_IC_IRQ_PENDING_1),
		mmio_read32(ARM_IC_IRQ_PENDING_2),
		mmio_read32(ARM_IC_IRQ_BASIC_PENDING) & 0xFF,
	};

	for (unsigned reg = 0; reg < 3; reg++) {
		uint32_t pend = pending[reg];

		if (!pend)
			continue;

		/* Note: although __builtin_ctz(0) returns 0 on x86(_64),
		 * testing on aarch64 Linux with native GCC shows that it
		 * returns 32 there, which is what we want here.
		 */
		uint32_t irq_n = __builtin_ctz(pend);
		if (reg == 1 && irq_n == __builtin_ctz(ARM_IRQ_UART)) /* UART */ {
			uart0_irq_handler();
			return;
		}
		mmio_write32(ARM_IC_IRQ_PENDING_1, 0);
		mmio_write32(ARM_IC_IRQ_PENDING_2, 0);
		mmio_write32(ARM_IC_IRQ_BASIC_PENDING, 0);
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