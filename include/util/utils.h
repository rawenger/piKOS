#pragma once
#include "types.h"

#define BIT(bit)			(1 << (bit))

#define enable_irq()                    do { asm volatile ("\tmsr daifclr, #2\n"); } while (0)
#define disable_irq()                   do { asm volatile ("\tmsr daifset, #2\n"); } while (0)

void delay (unsigned long);

extern void __memset_aarch64(void *p, int val, size_t n); //NOLINT(bugprone-reserved-identifier)
#define memset(p, val, n)               __memset_aarch64(p, val, n)

extern void __memcpy_aarch64(void *restrict dest, const void *restrict src, size_t count); //NOLINT(bugprone-reserved-identifier)
#define memcpy(dest, src, count)        __memcpy_aarch64(dest, src, count)
extern void __memmove_aarch64(void *dest, const void *src, size_t count); //NOLINT(bugprone-reserved-identifier)
#define memmove(dest, src, count)       __memmove_aarch64(dest, src, count)
