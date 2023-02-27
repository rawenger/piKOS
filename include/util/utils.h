#pragma once
#include "types.h"

#define BIT(bit)			(1 << (bit))

#define enable_irq()                    do { asm volatile ("\tmsr daifclr, #2\n"); } while (0)
#define disable_irq()                    do { asm volatile ("\tmsr daifset, #2\n"); } while (0)

extern void delay ( unsigned long);

void memzero(uintptr_t p, size_t n);

