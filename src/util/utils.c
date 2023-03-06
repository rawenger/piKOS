//
// Created by ryan on 2/25/23.
//
#include "util/memorymap.h"
#include "util/utils.h"

__attribute__((optimize(0)))
void delay(uint64_t cycles)
{
	while (cycles--)
		;
}