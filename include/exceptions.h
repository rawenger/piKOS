//
// Created by ryan on 2/25/23.
//

#pragma once

#define E_SYNC          0
#define E_ERROR         1
#define E_FIQ           2
#define E_32BIT         3
#define E_BADIRQ        4
#define E_TODO          5

#ifndef __ASSEMBLER__
#ifdef DEBUG
struct ExceptionContext {
	void *sp;
	void *lr;
	void *sp_el0;
	void *elr_el1;
	void *spsr_el1;
	void *esr_el1;
};

#else // #ifdef DEBUG
struct ExceptionContext {};
#endif // #ifdef DEBUG

// for KOS programmer-implemented ISR's
typedef void (*int_handler_t) (void);
typedef enum {
	TimerInt, DiskInt, ConsoleWriteInt, ConsoleReadInt,
	NetworkSendInt, NetworkRecvInt
} IntType;
#define KOS_IRQ_NUM             6 /* number of enum entries above */

/* Invokes the handler registered by KOS for the given interrupt type.
 * If no handler is registered, does nothing
 */
void call_KOS_handler(IntType which);

/* This is a replacement for the interruptHandler() routine from KOS`exception.c,
 * since it just doesn't make sense to determine the IRQ type in our actual handler,
 * only to convert it to an enum value and to pass back into another function which
 * must again determine the type. Instead, we just have KOS set up a jump table for these
 * different types, which also allows us to do some special intermediate handling on UART
 * interrupts in order to prevent (or at least reduce) dropped data.
 */
extern void register_isr(IntType which, int_handler_t handler);

_Noreturn void InvalidExceptionHandler(int type, struct ExceptionContext *context);
/* Actual handler called by the stub in the hardware vector table */
void irq_handler(void);
#endif // #ifndef __ASSEMBLER__


