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
#else
struct ExceptionContext {};
#endif

_Noreturn void InvalidExceptionHandler(int type, struct ExceptionContext *context);
#endif


