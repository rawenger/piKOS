//
// Created by ryan on 2/25/23.
//
#include "util/memorymap.h"

void memzero(uintptr_t p, size_t n)
{
	__asm__("\tstr xzr, [x0], #8\n"
	        "\tsubs x1, x1, #8\n"
	        "\tb.gt memzero\n");
}

//__attribute__((optimize(0)))
void delay(uint64_t cycles)
{
	__asm__ volatile ("\tsubs x0, x0, #1\n"
	        "\tbne delay\n");
}