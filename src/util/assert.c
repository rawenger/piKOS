/* assert.c
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *  Copyright (C) 2023 Ryan Wenger
 *
 */

#include "printk.h"
#include "types.h"
#include "assert.h"
#include "util/utils.h"

_Noreturn void assertion_failed(const char *expr, const char *file, int line)
{
	disable_irq();
	printk("Assertion failed: `%s' at %s:%d\r\n", expr, file, line);
	while (1)
		;
}