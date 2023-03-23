/*
 * asmdef.h - Aarch64 assembler macros
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *
 * Code from
 * https://github.com/ARM-software/optimized-routines/blob/3f91f739a96e726d4a298f9d9ccafe970a557615/string/aarch64/asmdefs.h
 *
 * Macros for asm code.  AArch64 version.
 *
 * Copyright (c) 2019-2023, Arm Limited.
 * SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception
 *
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

/* Branch Target Identitication support.  */
#define BTI_C		hint	34
#define BTI_J		hint	36
/* Return address signing support (pac-ret).  */
#define PACIASP		hint	25; .cfi_window_save
#define AUTIASP		hint	29; .cfi_window_save

/* GNU_PROPERTY_AARCH64_* macros from elf.h.  */
#define FEATURE_1_AND 0xc0000000
#define FEATURE_1_BTI 1
#define FEATURE_1_PAC 2

/* Add a NT_GNU_PROPERTY_TYPE_0 note.  */
#define GNU_PROPERTY(type, value)	\
  .section .note.gnu.property, "a";	\
  .p2align 3;				\
  .word 4;				\
  .word 16;				\
  .word 5;				\
  .asciz "GNU";				\
  .word type;				\
  .word 4;				\
  .word value;				\
  .word 0;				\
  .text

/* If set then the GNU Property Note section will be added to
   mark objects to support BTI and PAC-RET.  */
#ifndef WANT_GNU_PROPERTY
#define WANT_GNU_PROPERTY 1
#endif

#if WANT_GNU_PROPERTY
/* Add property note with supported features to all asm files.  */
GNU_PROPERTY (FEATURE_1_AND, FEATURE_1_BTI|FEATURE_1_PAC)
#endif

#define ENTRY_ALIGN(name, alignment)	\
  .global name;		\
  .type name,%function;	\
  .align alignment;		\
  name:			\
  .cfi_startproc;	\
  BTI_C;

#define ENTRY(name)	ENTRY_ALIGN(name, 6)

#define ENTRY_ALIAS(name)	\
  .global name;		\
  .type name,%function;	\
  name:

#define END(name)	\
  .cfi_endproc;		\
  .size name, .-name;

#define L(l) .L ## l

#ifdef __ILP32__
  /* Sanitize padding bits of pointer arguments as per aapcs64 */
#define PTR_ARG(n)  mov w##n, w##n
#else
#define PTR_ARG(n)
#endif

#ifdef __ILP32__
  /* Sanitize padding bits of size arguments as per aapcs64 */
#define SIZE_ARG(n)  mov w##n, w##n
#else
#define SIZE_ARG(n)
#endif

/* Compiler supports SVE instructions  */
#ifndef HAVE_SVE
# if __aarch64__ && (__GNUC__ >= 8 || __clang_major__ >= 5)
#   define HAVE_SVE 1
# else
#   define HAVE_SVE 0
# endif
#endif

