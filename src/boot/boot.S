/*
 * boot.S - initial entry point of piKOS
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *  Copyright (C) 2023 Ryan Wenger
 *
 * This file contains sligthly modified code from U-Boot
 *  armv8_switch_to_el1_m macro
 *  defined in arch/arm/include/asm/macro.h
 *  Copyright (C) 2009 Jean-Christophe PLAGNIOL-VILLARD <plagnioj@jcrosoft.com>
 *
 * (In particular, the modifications configure and enable the MMU for stage 1 EL1&0 translation
 *  while still in EL2, thus leaving us with no need to have any kind of "bootstrap"/
 *  identity mapping for enabling address translation from within EL1 itself.)
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

#include "util/memorymap.h"

	.macro armv8_switch_to_el1_m, xreg1, xreg2, return_symbol

	/* Initialize Generic Timers */
	mrs	\xreg1, cnthctl_el2
	orr	\xreg1, \xreg1, #0x3	/* Enable EL1 access to timers */
	msr	cnthctl_el2, \xreg1
	msr	cntvoff_el2, xzr

	/* Initilize MPID/MPIDR registers */
	mrs	\xreg1, midr_el1
	mrs	\xreg2, mpidr_el1
	msr	vpidr_el2, \xreg1
	msr	vmpidr_el2, \xreg2

	/* Disable coprocessor traps */
	mov	\xreg1, #0x33ff
	msr	cptr_el2, \xreg1	/* Disable coprocessor traps to EL2 */
	msr	hstr_el2, xzr		/* Disable coprocessor traps to EL2 */
	mov	\xreg1, #3 << 20
	msr	cpacr_el1, \xreg1	/* Enable FP/SIMD at EL1 */

	/* Initialize HCR_EL2 */
	mov	\xreg1, #(1 << 31)		/* 64bit EL1 */
	msr	hcr_el2, \xreg1

	/* SCTLR_EL1 initialization
	 *
	 * setting RES1 bits (29,28,23,22,20,11) to 1
	 * and RES0 bits (31,30,27,21,17,13,10,6) +
	 * UCI,EE,EOE,WXN,nTWE,nTWI,UCT,DZE,I,UMA,SED,ITD,
	 * CP15BEN,SA0,SA,C,A,M to 0
	 */
	mov	\xreg1, #0x0800
	movk	\xreg1, #0x30d0, lsl #16
	msr	sctlr_el1, \xreg1

	/* configure page table and turn on MMU */
	bl      EL2_MMU_bootstrap

	/* Return to the EL1_SP1 mode from EL2 */
	mov	\xreg1, #0x3c4
	msr	spsr_el2, \xreg1	/* EL1_SP0 | D | A | I | F */
	//adr	\xreg1, 1f
	ldr     \xreg1, =\return_symbol
	msr	elr_el2, \xreg1
	eret

	.endm


.section .text.boot, "x"

        .globl _start
_start: /* Entered from armstub8 while in EL2 after boot */
	mrs     x0, CurrentEL /* If we're in EL1, something's wrong */
	cmp     x0, #4  // value stored in bits[3:2]
	beq     _start

        /* Set up EL1 exception stack */
        ldr     x0, =EXCEPTION_STACK_BASE_VM
        msr     sp_el1, x0

        /* Set up core 0 EL2 stack, just with physical memory (needed for the MMU initialization) */
        ldr     x0, =KERN_STACK_BASE_PHYS
        mov     sp, x0

        /* Set up EL2 exception vector table */
        // TODO: current code in VectorTable assumes we have MMU turned on (and is linked at a virtual address anyway)
        //ldr     x0, =VectorTable
        //msr     vbar_el2, x0

        armv8_switch_to_el1_m x0, x1, EL1_entry

        .section .text
EL1_entry:
        /* set up EL1 exception vectors */
        ldr     x0, =VectorTable
        msr     vbar_el1, x0

        /* set up core 0 main kernel stack */
        ldr     x0, =KERN_STACK_BASE_VM
        mov     sp, x0

        /* Zero out the .bss segment */
	ldr     x0, =bss_begin
	mov     x1, xzr
        ldr     x2, =bss_end
	sub	x2, x2, x0
	bl 	memset

	/* enter the Kernel Proper! */
        b       kernel_main

	// don't wake up other cores, KOS is NOT that advanced LOL

#ifdef MULTICORE
	.globl	_start_secondary
_start_secondary: // entered by non-primary cores in EL2 after boot
        b _start_secondary
#endif