/*
 * kmalloc.h - kernel heap allocator
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *   Copyright (C) 2023 Ryan Wenger
 *
 * This code was modified from one of my own class projects in undergrad,
 *   and is Copyright (c) 2022-2023 Ryan Wenger
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

/* need to call this before we can use any heap allocators */
void init_kmalloc(void);

/* These behave pretty much the way their stdlib counterparts do from a user's perspective */
void  kfree(void *ptr);

__attribute__((malloc, malloc (kfree, 1)))
void *kmalloc(size_t size);

__attribute__((malloc, malloc (kfree, 1)))
void *kcalloc(size_t count, size_t size);

void *krealloc(void *ptr, size_t size);

