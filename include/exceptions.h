/*
 * exceptions.h
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

#define E_SYNC          0
#define E_ERROR         1
#define E_FIQ           2
#define E_32BIT         3
#define E_BADIRQ        4
#define E_TODO          5

/* Disable saving of the floating point/SIMD registers on exception entry
 * if we are coming from the kernel. Currently, the memset routine
 * from ARM only uses q0, so we always save that when taking an exception;
 * otherwise, uncomment the following #define to save *all* the VFP registers.
 * All of the C code in the kernel is compiled with -mgeneral-regs-only.
 */
//#define SAVE_VFP_REGS_FROM_EL1

#ifndef __ASSEMBLER__
#ifdef DEBUG
struct ExceptionContext {
	void *sp;
	void *lr;
	void *sp_el0;
	void *elr_el1;
	void *spsr_el1;
	unsigned long esr_el1;
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

_Noreturn void InvalidExceptionHandler(int type, int currentEL, struct ExceptionContext *context);
/* Actual handler called by the stub in the hardware vector table */
void irq_handler(void);
#endif // #ifndef __ASSEMBLER__


