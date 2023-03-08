#pragma once
#include "types.h"
#include "printk.h"

#define BIT(bit)			(1 << (bit))

#define enable_irq()                    do { asm volatile ("\tmsr daifclr, #2\n"); } while (0)
#define disable_irq()                   do { asm volatile ("\tmsr daifset, #2\n"); } while (0)

void delay (unsigned long);

extern void memset(void *dest, int val, size_t nbytes)
	__attribute__((nonnull (1)));

extern void memcpy(void *restrict dest, const void *restrict src, size_t nbytes)
	__attribute__((nonnull (1, 2)));

extern void memmove(void *dest, const void *src, size_t nbytes)
	__attribute__((nonnull (1, 2)));
