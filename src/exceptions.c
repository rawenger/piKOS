//
// Created by ryan on 2/25/23.
//

#include "exceptions.h"
#include "peripherals/mini_uart.h"
#include "mmio.h"

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
			muart_send_str("Unkown\r\n");
	}
#endif
	// TRAP here to read out members of `context` because
	// I don't feel like writing a routine to dump them
	while (1)
		;
}

// so far this ONLY handles UART Tx/Rx, so need to fix that LOL
void irq_handler()
{
	// clear pending interrupt
	mmio_write32(UART0_ICR, mmio_read32(UART0_MIS));
	// that's all for now
}

