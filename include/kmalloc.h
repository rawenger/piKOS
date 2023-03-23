/* kmalloc.h - kernel heap allocator
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *      Copyright (C) 2023 Ryan Wenger
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

