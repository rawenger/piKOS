/*
 * memorymap.h - Kernel memory layout
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *      Copyright (c) 2023 Ryan Wenger
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

#ifndef KILOBYTE
#define KILOBYTE        (0x400UL)
#endif
#ifndef MEGABYTE
#define MEGABYTE	(KILOBYTE * KILOBYTE)
#endif
#ifndef GIGABYTE
#define GIGABYTE	(MEGABYTE * KILOBYTE)
#endif

/* All of this is drawn out in Notability, just waiting to copy diagram
 *  into ASCII here until it's all actually finalized.
 */

#define CORES			1 // must be a power of 2

#define KERNEL_IMG_MAX_SIZE     (2 * MEGABYTE) // can't be larger than MMU level 2 block descriptor

#define PAGE_SHIFT              12      // 4K page size
#define PAGE_SIZE		(1UL << PAGE_SHIFT)
#define PAGESIZE                PAGE_SIZE
#define KERN_PGDIR_SIZE         (5 * PAGESIZE)
#define KERN_VM_BASE            (0xFFFFUL << 48)

#define STACK_SIZE              (128 * KILOBYTE) // kernel + exception stacks
#define EXCEPTION_STACK_SIZE    (STACK_SIZE / 4)
#define KERN_STACK_SIZE         (3 * EXCEPTION_STACK_SIZE)

#define PAGETABLE_START_PHYS    (PAGESIZE) // could start at physical 0x0, but that just feels wrong...
#define KERN_IMG_START_PHYS	0x80000UL // _start gets loaded here
#define KERN_IMG_END_PHYS       (KERN_IMG_START_PHYS + KERNEL_IMG_MAX_SIZE)
// very top of kernel phys address space
#define KERN_STACK_BASE_PHYS	(192 * MEGABYTE) // bottom/high addr, used by some EL2 setup.
// TODO: update once MMU code working
#define KERN_STACK_BASE_VM      (KERN_VM_BASE + KERNEL_IMG_MAX_SIZE + STACK_SIZE) // bottom/initial sp
//#define MEM_KERNEL_STACK_END    (KERN_STACK_TOP_PHYS - 128 * KILOBYTE) // bottom/low addr
#define EXCEPTION_STACK_BASE_PHYS        (KERN_STACK_BASE_PHYS - KERN_STACK_SIZE)
//#define MEM_EXCEPTION_STACK_END	(EXCEPTION_STACK_TOP_PHYS + EXCEPTION_STACK_SIZE * (CORES-1))

// TODO: update once MMU code working
#define EXCEPTION_STACK_BASE_VM         (KERN_STACK_BASE_VM - KERN_STACK_SIZE) // bottom/initial sp

// TODO: update these once VM layout finalized
#define KERN_HEAP_START		(KERN_STACK_BASE_PHYS - STACK_SIZE - KERN_IMG_END_PHYS)
#define KERN_HEAP_MAXSIZE       (KERN_HEAP_START + 4 * MEGABYTE) // this can be revised

#ifndef __ASSEMBLER__

#include "types.h"

#endif

