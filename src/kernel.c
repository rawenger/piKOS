/*
 * kernel.c
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

#include <stdint.h>
#include <assert.h>

#include "mmio.h"
#include "types.h"
#include "kmalloc.h"
#include "util/utils.h"
#include "peripherals/uart0.h"
#include "peripherals/mini_uart.h"
#include "util/memorymap.h"
#include "vm_kernel.h"
#include "peripherals/systimer.h"
#include "peripherals/led.h"

/* from PlutoniumBob@raspi-forum */
// Power Manager
#define ARM_PM_RSTC     (ARM_PM_BASE + 0x1C)
#define ARM_PM_WDOG     (ARM_PM_BASE + 0x24)
#define   ARM_PM_PASSWD   (0x5A << 24)
#define   PM_RSTC_WRCFG_FULL_RESET 0x20

static void SystemReboot(void)
{
        printk("System rebooting...");
        vmmio_write32(ARM_PM_WDOG, ARM_PM_PASSWD | 1); // one second timeout
        vmmio_write32(ARM_PM_RSTC, ARM_PM_PASSWD | PM_RSTC_WRCFG_FULL_RESET);
}

static void irq_init()
{
	vmmio_write32(ARM_IC_FIQ_CONTROL, 0); // completely disable FIQ's -- Linux does not use them so neither will we
	vmmio_write32(ARM_IC_DISABLE_IRQS_1, -1);
	vmmio_write32(ARM_IC_DISABLE_IRQS_2, -1);
	vmmio_write32(ARM_IC_DISABLE_BASIC_IRQS, -1);
	vmmio_write32(ARM_LOCAL_TIMER_INT_CONTROL0, 0);

//	vmmio_write32(ARM_IC_ENABLE_IRQS_1, ARM_IRQ_TIMER1);
	vmmio_write32(ARM_IC_ENABLE_IRQS_2, ARM_IRQ_UART);

	enable_irq();
}

/* Initialize libraries, software layer stuff. Don't put driver code in here! */
// TODO: rename this LOL
static void init_stuff(void)
{
	// commented out until kernel heap layout set up
//	init_kmalloc();
}

extern void *kern_img_end;
extern void get_read(void);

_Noreturn void kernel_main(void)
{
	delay(1000);
#ifdef DEBUG
//	muart_init();
	uart0_init();
	printk("entered kernel\r\n");
	delay(1000);
#endif
        timer_init();
	irq_init();

	u64 el;
	asm volatile ("\tmrs %0, CurrentEL\n"
		      "\tlsr %0, %0, #2\n"
		      : "=r" (el));
	assert(el > 0);
	printk("Running in EL %lu\r\n", el);

	void *start = (void *) (KERN_VM_BASE | KERN_IMG_START_PHYS);

	printk("kernel image vm_start: %p\r\n"
	       "kernel image vm_end: %p\r\n"
	       "image size: 0x%lx\r\n",
	       start, &kern_img_end, (void *) &kern_img_end - (void *) start);

        delay(50000000);
//        delay(500000000);
        disable_irq();
        muart_init();
        muart_send_str("PL011 read:\r\n");
        get_read();
	while (1) {
//                muart_send(muart_recv());
//		uart0_send(uart0_recv());
	}
}