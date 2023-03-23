/*
 * utils.h
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
#include "types.h"
#include "printk.h"

#define BIT(bit)			(1 << (bit))

#define enable_irq()                    do { asm volatile ("\tmsr daifclr, #2\n"); } while (0)
#define disable_irq()                   do { asm volatile ("\tmsr daifset, #2\n"); } while (0)

void delay (unsigned long);

extern void
memset(void *dest, int val, size_t nbytes)
	__attribute__((nonnull (1)));

extern void memcpy(void *restrict dest, const void *restrict src, size_t nbytes)
	__attribute__((nonnull (1, 2)));

extern void memmove(void *dest, const void *src, size_t nbytes)
	__attribute__((nonnull (1, 2)));
