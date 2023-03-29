/*
 * vectors.S - exception handling assembly routines
 *   + vtable layout
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

        .macro vtentry handler
        .align 7
        b \handler
        .endm

        .macro invalid_exception eType
        mrs     x6, CurrentEL // grab this first for Reasons
        lsr     x6, x6, #2
#ifdef DEBUG
        /* Save some important debug info to pass into
         * our exception handler.
         * NOTE: DO NOT CHANGE THIS ORDER WITHOUT UPDATING
         * THE "ExceptionContext" STRUCT IN 'exceptions.h'!!
         */
        mrs     x0, esr_el1
        mrs     x1, spsr_el1
        mrs     x2, elr_el1
        mrs     x3, sp_el0
        mov     x4, lr // (ra)
        mov     x5, sp // (including the stack itself)
        stp     x1, x0, [sp, #-16]!
        stp     x3, x2, [sp, #-16]!
        stp     x5, x4, [sp, #-16]!
        mov     x2, sp
#else
        mov     x2, xzr
#endif
        /* call _Noreturn void InvalidExceptionHandler(int type, int currentEL, struct ExceptionContext *context) */
        mov     w1, w6
        mov     w0, \eType
        b       InvalidExceptionHandler

        .endm

        /* If from_kern is 1 then we are being entered
         * from the kernel and need to save sp, rather
         * than sp_el0. We also don't plan to use the
         * VFP registers in the kernel (as of this writing),
         * so don't need to save off those.
         *
         * We set up the stack to look like this:
         *
         * +------------+
         * |    x0      | <-- 0x7CF0 (-784)
         * |  (sp_el0)  | ** Blank unless being called from EL0 (i.e: from_kern = 0)
         *      ...
         * |    x27     | <-- 0x7DD0 (-560)
         * |    x28     |
         * |    q0      | <-- 0x7DE0 (-544)
         *      ...
         * |    q30     |
         * |    q31     |
         * |  elr_el1   | <-- 0x7FE0 (-32)
         * |  spsr_el1  |
         * |    x29     | <-- 0x7FF0 (-16)
         * |    x30     |
         * +------------+ <-- 0x8000 (W.L.O.G.)
         */
        .macro save_state from_kern
        stp     x29, x30, [sp, #-16]!
        mrs     x30, spsr_el1 // save calling stack
        mrs     x29, elr_el1 // save exception return address
        stp     x29, x30, [sp, #-16]!

        /* Save off all our GP and VFP registers */
        /* VFP's first because machine-mandated 16-byte alignment
         * makes it easier to align the odd number of GP regs
         * at the top of the stack.
         */
#ifndef SAVE_VFP_REGS_FROM_EL1
        .if \from_kern == 0
#else
        .if 1
#endif
        stp     q30, q31, [sp, #-32]!
        stp     q28, q29, [sp, #-32]!
        stp     q26, q27, [sp, #-32]!
        stp     q24, q25, [sp, #-32]!
        stp     q22, q23, [sp, #-32]!
        stp     q20, q21, [sp, #-32]!
        stp     q18, q19, [sp, #-32]!
        stp     q16, q17, [sp, #-32]!
        stp     q14, q15, [sp, #-32]!
        stp     q12, q13, [sp, #-32]!
        stp     q10, q11, [sp, #-32]!
        stp     q8, q9, [sp, #-32]!
        stp     q6, q7, [sp, #-32]!
        stp     q4, q5, [sp, #-32]!
        stp     q2, q3, [sp, #-32]!
        stp     q0, q1, [sp, #-32]!
        .endif

        stp     x27, x28, [sp, #-16]!
        stp     x25, x26, [sp, #-16]!
        stp     x23, x24, [sp, #-16]!
        stp     x21, x22, [sp, #-16]!
        stp     x19, x20, [sp, #-16]!
        stp     x17, x18, [sp, #-16]!
        stp     x15, x16, [sp, #-16]!
        stp     x13, x14, [sp, #-16]!
        stp     x11, x12, [sp, #-16]!
        stp     x9, x10, [sp, #-16]!
        stp     x7, x8, [sp, #-16]!
        stp     x5, x6, [sp, #-16]!
        stp     x3, x4, [sp, #-16]!
        stp     x1, x2, [sp, #-16]!

        .if \from_kern == 0
        mrs     x1, sp_el0
        stp     x0, x1, [sp, #-16]!
        .else
        str     x0, [sp, #-16]!
        .endif
        .endm

        /* restore register state after handling IRQ/Exception */
        .macro restore_state to_kern
        .if \to_kern == 0
        ldp     x0, x1, [sp], #16
        msr     sp_el0, x1
        .else
        ldr     x0, [sp], #16
        .endif

        /* restore GP regs (except x29 & x30) first since we saved them second */
        ldp     x1, x2, [sp], #16
        ldp     x3, x4, [sp], #16
        ldp     x5, x6, [sp], #16
        ldp     x7, x8, [sp], #16
        ldp     x9, x10, [sp], #16
        ldp     x11, x12, [sp], #16
        ldp     x13, x14, [sp], #16
        ldp     x15, x16, [sp], #16
        ldp     x17, x18, [sp], #16
        ldp     x19, x20, [sp], #16
        ldp     x21, x22, [sp], #16
        ldp     x23, x24, [sp], #16
        ldp     x25, x26, [sp], #16
        ldp     x27, x28, [sp], #16

        /* VFP regs */
#ifndef SAVE_VFP_REGS_FROM_EL1
        .if \to_kern == 0
#else
        .if 1
#endif
        ldp     q0, q1, [sp], #32
        ldp     q2, q3, [sp], #32
        ldp     q4, q5, [sp], #32
        ldp     q6, q7, [sp], #32
        ldp     q8, q9, [sp], #32
        ldp     q10, q11, [sp], #32
        ldp     q12, q13, [sp], #32
        ldp     q14, q15, [sp], #32
        ldp     q16, q17, [sp], #32
        ldp     q18, q19, [sp], #32
        ldp     q20, q21, [sp], #32
        ldp     q22, q23, [sp], #32
        ldp     q24, q25, [sp], #32
        ldp     q26, q27, [sp], #32
        ldp     q28, q29, [sp], #32
        ldp     q30, q31, [sp], #32
        .endif

        /* el1 stack & return address */
        ldp     x29, x30, [sp], #16
        msr     elr_el1, x29
        msr     spsr_el1, x30

        /* and finally, x29 and x30 */
        ldp     x29, x30, [sp], #16
        .endm

        .text
        .align 11
        .globl VectorTable
VectorTable:
        /* from EL1t with shared EL0 stack */
        vtentry         SyncStub
        vtentry         IRQStubEL1
        vtentry         FIQStub
        vtentry         ErrorStub

        /* from EL1h with non-EL0 stack */
        vtentry         SyncStub
        vtentry         IRQStubEL1
        vtentry         FIQStub
        vtentry         ErrorStub

        /* from 64-bit EL0 */
        vtentry         SyscallStub
        vtentry         IRQStubEL0
        vtentry         FIQStub
        vtentry         ErrorStub

        /* from 32-bit EL0 (shouldn't happen) */
        vtentry         AARCH32
        vtentry         AARCH32
        vtentry         AARCH32
        vtentry         AARCH32

/* IRQ's that occur while in kernelspace */
        .globl IRQStubEL1
IRQStubEL1:
        save_state 1
        bl irq_handler
        restore_state 1
        eret

/* IRQ's that occur while in userspace */
        .globl IRQStubEL0
IRQStubEL0:
        b IRQStubEL0


/* System call exceptions */
        .globl SyscallStub
SyscallStub: /// TODO
        invalid_exception E_TODO


/* Unallowed exceptions */
        .globl SyncStub
SyncStub:
        invalid_exception E_SYNC

        .globl ErrorStub
ErrorStub: // TODO this will eventually need to be implemented to handle e.g. page faults
        invalid_exception E_ERROR

        .globl FIQStub
FIQStub:
        invalid_exception E_FIQ

        .globl AARCH32
AARCH32:
        invalid_exception E_32BIT

        .globl BadIRQStub
BadIRQStub:
        invalid_exception E_BADIRQ




