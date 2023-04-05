/*
 * bcm2835int.h
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *  Copyright (C) 2023 Ryan Wenger
 *
 * Many of the definitions in this file come from Circle
 *      file: include/circle/bcm2835int.h
 *      Copyright (C) 2014-2020  R. Stange <rsta2@o2online.de>
 *
 * The IRQ list is taken from Linux and is:
 *	Copyright (C) 2010 Broadcom
 *	Copyright (C) 2003 ARM Limited
 *	Copyright (C) 2000 Deep Blue Solutions Ltd.
 *	Licensed under GPL2
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

#if RASPPI >= 4
	#include "bsp/bcm2711int.h"
#else

// IRQs
#define ARM_IRQS_PER_REG	32
#define ARM_IRQS_BASIC_REG	8
#if RASPPI >= 2
#define ARM_IRQS_LOCAL_REG	12
#else
#define ARM_IRQS_LOCAL_REG	0
#endif

#define ARM_IRQ1_BASE		0
#define ARM_IRQBASIC_BASE	(ARM_IRQ2_BASE + ARM_IRQS_PER_REG)
#define ARM_IRQLOCAL_BASE	(ARM_IRQBASIC_BASE + ARM_IRQS_BASIC_REG)

#define ARM_IRQ_TIMER0		(1 << 0U)
#define ARM_IRQ_TIMER1		(1 << 1U)
#define ARM_IRQ_TIMER2		(1 << 2U)
#define ARM_IRQ_TIMER3		(1 << 3U)
#define ARM_IRQ_CODEC0		(1 << 4U)
#define ARM_IRQ_CODEC1		(1 << 5U)
#define ARM_IRQ_CODEC2		(1 << 6U)
#define ARM_IRQ_JPEG		(1 << 7U)
#define ARM_IRQ_ISP		(1 << 8U)
#define ARM_IRQ_USB		(1 << 9U)
#define ARM_IRQ_3D		(1 << 10U)
#define ARM_IRQ_TRANSPOSER	(1 << 11U)
#define ARM_IRQ_MULTICORESYNC0	(1 << 12U)
#define ARM_IRQ_MULTICORESYNC1	(1 << 13U)
#define ARM_IRQ_MULTICORESYNC2	(1 << 14U)
#define ARM_IRQ_MULTICORESYNC3	(1 << 15U)
#define ARM_IRQ_DMA0		(1 << 16U)
#define ARM_IRQ_DMA1		(1 << 17U)
#define ARM_IRQ_DMA2		(1 << 18U)
#define ARM_IRQ_DMA3		(1 << 19U)
#define ARM_IRQ_DMA4		(1 << 20U)
#define ARM_IRQ_DMA5		(1 << 21U)
#define ARM_IRQ_DMA6		(1 << 22U)
#define ARM_IRQ_DMA7		(1 << 23U)
#define ARM_IRQ_DMA8		(1 << 24U)
#define ARM_IRQ_DMA9		(1 << 25U)
#define ARM_IRQ_DMA10		(1 << 26U)
#define ARM_IRQ_DMA11		(1 << 27U)
#define ARM_IRQ_DMA_SHARED	(1 << 28U)
#define ARM_IRQ_AUX		(1 << 29U)
#define ARM_IRQ_ARM		(1 << 30U)
#define ARM_IRQ_VPUDMA		(1 << 31U)

#define ARM_IRQ2_BASE		(ARM_IRQ1_BASE + ARM_IRQS_PER_REG)
#define ARM_IRQ_HOSTPORT	(1 << 0U)
#define ARM_IRQ_VIDEOSCALER	(1 << 1U)
#define ARM_IRQ_CCP2TX		(1 << 2U)
#define ARM_IRQ_SDC		(1 << 3U)
#define ARM_IRQ_DSI0		(1 << 4U)
#define ARM_IRQ_AVE		(1 << 5U)
#define ARM_IRQ_CAM0		(1 << 6U)
#define ARM_IRQ_CAM1		(1 << 7U)
#define ARM_IRQ_HDMI0		(1 << 8U)
#define ARM_IRQ_HDMI1		(1 << 9U)
#define ARM_IRQ_PIXELVALVE1	(1 << 10U)
#define ARM_IRQ_I2CSPISLV	(1 << 11U)
#define ARM_IRQ_DSI1		(1 << 12U)
#define ARM_IRQ_PWA0		(1 << 13U)
#define ARM_IRQ_PWA1		(1 << 14U)
#define ARM_IRQ_CPR		(1 << 15U)
#define ARM_IRQ_SMI		(1 << 16U)
#define ARM_IRQ_GPIO0		(1 << 17U)
#define ARM_IRQ_GPIO1		(1 << 18U)
#define ARM_IRQ_GPIO2		(1 << 19U)
#define ARM_IRQ_GPIO3		(1 << 20U)
#define ARM_IRQ_I2C		(1 << 21U)
#define ARM_IRQ_SPI		(1 << 22U)
#define ARM_IRQ_I2SPCM		(1 << 23U)
#define ARM_IRQ_SDIO		(1 << 24U)
#define ARM_IRQ_UART		(1 << 25U)
#define ARM_IRQ_SLIMBUS		(1 << 26U)
#define ARM_IRQ_VEC		(1 << 27U)
#define ARM_IRQ_CPG		(1 << 28U)
#define ARM_IRQ_RNG		(1 << 29U)
#define ARM_IRQ_ARASANSDIO	(1 << 30U)
#define ARM_IRQ_AVSPMON		(1 << 31U)

#define ARM_IRQ_ARM_TIMER	(ARM_IRQBASIC_BASE + 0)
#define ARM_IRQ_ARM_MAILBOX	(ARM_IRQBASIC_BASE + 1)
#define ARM_IRQ_ARM_DOORBELL_0	(ARM_IRQBASIC_BASE + 2)
#define ARM_IRQ_ARM_DOORBELL_1	(ARM_IRQBASIC_BASE + 3)
#define ARM_IRQ_VPU0_HALTED	(ARM_IRQBASIC_BASE + 4)
#define ARM_IRQ_VPU1_HALTED	(ARM_IRQBASIC_BASE + 5)
#define ARM_IRQ_ILLEGAL_TYPE0	(ARM_IRQBASIC_BASE + 6)
#define ARM_IRQ_ILLEGAL_TYPE1	(ARM_IRQBASIC_BASE + 7)

#if RASPPI >= 2
#define ARM_IRQLOCAL0_CNTPS	(ARM_IRQLOCAL_BASE + 0)
#define ARM_IRQLOCAL0_CNTPNS	(ARM_IRQLOCAL_BASE + 1)
#define ARM_IRQLOCAL0_CNTHP	(ARM_IRQLOCAL_BASE + 2)
#define ARM_IRQLOCAL0_CNTV	(ARM_IRQLOCAL_BASE + 3)
#define ARM_IRQLOCAL0_MAILBOX0	(ARM_IRQLOCAL_BASE + 4)
#define ARM_IRQLOCAL0_MAILBOX1	(ARM_IRQLOCAL_BASE + 5)
#define ARM_IRQLOCAL0_MAILBOX2	(ARM_IRQLOCAL_BASE + 6)
#define ARM_IRQLOCAL0_MAILBOX3	(ARM_IRQLOCAL_BASE + 7)
#define ARM_IRQLOCAL0_GPU	(ARM_IRQLOCAL_BASE + 8)		// cascaded GPU interrupts
#define ARM_IRQLOCAL0_PMU 	(ARM_IRQLOCAL_BASE + 9)
#define ARM_IRQLOCAL0_AXI_IDLE	(ARM_IRQLOCAL_BASE + 10)	// on core 0 only
#define ARM_IRQLOCAL0_LOCALTIMER (ARM_IRQLOCAL_BASE + 11)
#endif

#define IRQ_LINES		(ARM_IRQS_PER_REG * 2 + ARM_IRQS_BASIC_REG + ARM_IRQS_LOCAL_REG)

// FIQs
#define ARM_FIQ_TIMER0		0
#define ARM_FIQ_TIMER1		1
#define ARM_FIQ_TIMER2		2
#define ARM_FIQ_TIMER3		3
#define ARM_FIQ_CODEC0		4
#define ARM_FIQ_CODEC1		5
#define ARM_FIQ_CODEC2		6
#define ARM_FIQ_JPEG		7
#define ARM_FIQ_ISP		8
#define ARM_FIQ_USB		9
#define ARM_FIQ_3D		10
#define ARM_FIQ_TRANSPOSER	11
#define ARM_FIQ_MULTICORESYNC0	12
#define ARM_FIQ_MULTICORESYNC1	13
#define ARM_FIQ_MULTICORESYNC2	14
#define ARM_FIQ_MULTICORESYNC3	15
#define ARM_FIQ_DMA0		16
#define ARM_FIQ_DMA1		17
#define ARM_FIQ_DMA2		18
#define ARM_FIQ_DMA3		19
#define ARM_FIQ_DMA4		20
#define ARM_FIQ_DMA5		21
#define ARM_FIQ_DMA6		22
#define ARM_FIQ_DMA7		23
#define ARM_FIQ_DMA8		24
#define ARM_FIQ_DMA9		25
#define ARM_FIQ_DMA10		26
#define ARM_FIQ_DMA11		27
#define ARM_FIQ_DMA_SHARED	28
#define ARM_FIQ_AUX		29
#define ARM_FIQ_ARM		30
#define ARM_FIQ_VPUDMA		31
#define ARM_FIQ_HOSTPORT	32
#define ARM_FIQ_VIDEOSCALER	33
#define ARM_FIQ_CCP2TX		34
#define ARM_FIQ_SDC		35
#define ARM_FIQ_DSI0		36
#define ARM_FIQ_AVE		37
#define ARM_FIQ_CAM0		38
#define ARM_FIQ_CAM1		39
#define ARM_FIQ_HDMI0		40
#define ARM_FIQ_HDMI1		41
#define ARM_FIQ_PIXELVALVE1	42
#define ARM_FIQ_I2CSPISLV	43
#define ARM_FIQ_DSI1		44
#define ARM_FIQ_PWA0		45
#define ARM_FIQ_PWA1		46
#define ARM_FIQ_CPR		47
#define ARM_FIQ_SMI		48
#define ARM_FIQ_GPIO0		49
#define ARM_FIQ_GPIO1		50
#define ARM_FIQ_GPIO2		51
#define ARM_FIQ_GPIO3		52
#define ARM_FIQ_I2C		53
#define ARM_FIQ_SPI		54
#define ARM_FIQ_I2SPCM		55
#define ARM_FIQ_SDIO		56
#define ARM_FIQ_UART		57
#define ARM_FIQ_SLIMBUS		58
#define ARM_FIQ_VEC		59
#define ARM_FIQ_CPG		60
#define ARM_FIQ_RNG		61
#define ARM_FIQ_ARASANSDIO	62
#define ARM_FIQ_AVSPMON		63
#define ARM_FIQ_ARM_TIMER	64
#define ARM_FIQ_ARM_MAILBOX	65
#define ARM_FIQ_ARM_DOORBELL_0	66
#define ARM_FIQ_ARM_DOORBELL_1	67
#define ARM_FIQ_VPU0_HALTED	68
#define ARM_FIQ_VPU1_HALTED	69
#define ARM_FIQ_ILLEGAL_TYPE0	70
#define ARM_FIQ_ILLEGAL_TYPE1	71

#define ARM_MAX_FIQ		71

#endif
