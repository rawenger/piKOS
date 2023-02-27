/* MMIO.h - MMIO address mappings for Raspberry Pi 4B
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 */

#pragma once
#include "types.h"

#if (RASPPI == 4)
#define MMIO_BASE       (0xFE000000)
#define SYSTEM_CLOCK_FREQ	500000000UL
#elif (RASPPI == 3)
#define MMIO_BASE       (0x3F000000)
#define SYSTEM_CLOCK_FREQ	250000000UL
#endif

// The offsets for each register.
#define GPIO_BASE       (MMIO_BASE + 0x200000)

#if RASPPI <= 3
// Controls actuation of pull up/down to ALL GPIO pins.
#define GPIO_GPPUD		(ARM_GPIO_BASE + 0x94)
// Controls actuation of pull up/down for specific GPIO pin.
#define GPIO_GPPUDCLK0	        (ARM_GPIO_BASE + 0x98)
#else
#define GPIO_GPPINMUXSD	        (ARM_GPIO_BASE + 0xD0)
#define GPIO_GPPUPPDN0	        (ARM_GPIO_BASE + 0xE4)
#define GPIO_GPPUPPDN1	        (ARM_GPIO_BASE + 0xE8)
#define GPIO_GPPUPPDN2	        (ARM_GPIO_BASE + 0xEC)
#define GPIO_GPPUPPDN3          (ARM_GPIO_BASE + 0xF0)
#endif

// The base address for the PL011 UART.
#define UART0_BASE      (GPIO_BASE + 0x1000) // for raspi4 0xFE201000, raspi2 & 3 0x3F201000, and 0x20201000 for raspi1

// The offsets for reach register for the UART. (see ARMv8 spec sheet for A-profile)
#define UART0_DR        (UART0_BASE + 0x00)     // Data Register
#define DR_OE_MASK	        (1 << 11)
#define DR_BE_MASK	        (1 << 10)
#define DR_PE_MASK	        (1 << 9)
#define DR_FE_MASK	        (1 << 8)

#define UART0_RSRECR    (UART0_BASE + 0x04)     // Receive Status Register / Error Clear Register

#define UART0_FR        (UART0_BASE + 0x18)     // Flag Register
#define FR_TXFE_MASK	        (1 << 7)
#define FR_RXFF_MASK	        (1 << 6)
#define FR_TXFF_MASK	        (1 << 5)
#define FR_RXFE_MASK	        (1 << 4)
#define FR_BUSY_MASK	        (1 << 3)

#define UART0_ILPR      (UART0_BASE + 0x20)     // IrDA Low-Power Counter Register

#define UART0_IBRD      (UART0_BASE + 0x24)     // Integer Baud Rate Register
#define UART0_FBRD      (UART0_BASE + 0x28)     //  	Fractional Baud Rate Register

#define UART0_LCRH      (UART0_BASE + 0x2C)     // Line Control Register
#define LCRH_SPS_MASK	        (1 << 7)
#define LCRH_WLEN8_MASK	        (3 << 5)
#define LCRH_WLEN7_MASK	        (2 << 5)
#define LCRH_WLEN6_MASK	        (1 << 5)
#define LCRH_WLEN5_MASK	        (0 << 5)
#define LCRH_FEN_MASK	        (1 << 4)
#define LCRH_STP2_MASK	        (1 << 3)
#define LCRH_EPS_MASK	        (1 << 2)
#define LCRH_PEN_MASK	        (1 << 1)
#define LCRH_BRK_MASK	        (1 << 0)

#define UART0_CR        (UART0_BASE + 0x30)     // Control Register
#define CR_CTSEN_MASK           (1 << 15)
#define CR_RTSEN_MASK	        (1 << 14)
#define CR_OUT2_MASK	        (1 << 13)
#define CR_OUT1_MASK	        (1 << 12)
#define CR_RTS_MASK             (1 << 11)
#define CR_DTR_MASK             (1 << 10)
#define CR_RXE_MASK	        (1 << 9)
#define CR_TXE_MASK	        (1 << 8)
#define CR_LBE_MASK	        (1 << 7)
#define CR_UART_EN_MASK	        (1 << 0)

#define UART0_IFLS      (UART0_BASE + 0x34)     // Interrupt FIFO Level Select Register
#define IFLS_RXIFSEL_SHIFT	3
#define IFLS_RXIFSEL_MASK	(7 << IFLS_RXIFSEL_SHIFT)
#define IFLS_TXIFSEL_SHIFT	0
#define IFLS_TXIFSEL_MASK	(7 << IFLS_TXIFSEL_SHIFT)
#define IFLS_IFSEL_1_8		0
#define IFLS_IFSEL_1_4		1
#define IFLS_IFSEL_1_2		2
#define IFLS_IFSEL_3_4		3
#define IFLS_IFSEL_7_8		4

#define UART0_IMSC      (UART0_BASE + 0x38)     // Interrupt Mask Set/Clear Register
#define IMSC_MASK       (0x7FF)
#define IMSC_OEIM       (1 << 10)
#define IMSC_BEIM       (1 << 9)
#define IMSC_PEIM       (1 << 8)
#define IMSC_FEIM       (1 << 7)
#define IMSC_RTIM       (1 << 6)
#define IMSC_TXIM       (1 << 5)
#define IMSC_RXIM       (1 << 4)
#define IMSC_DSRMIM     (1 << 3)
#define IMSC_DCDMIM     (1 << 2)
#define IMSC_CTSMIM     (1 << 1)
#define IMSC_RIMIM      (1 << 0)

#define UART0_RIS       (UART0_BASE + 0x3C)     // Raw Interrupt Status Register

#define UART0_MIS       (UART0_BASE + 0x40)     // Masked Interrupt Status Register

#define UART0_ICR       (UART0_BASE + 0x44)     // Interrupt Clear Register
#define ICR_MASK        (0x7FF)
#define ICR_OEIC        (1 << 10)
#define ICR_BEIC        (1 << 9)
#define ICR_PEIC        (1 << 8)
#define ICR_FEIC        (1 << 7)
#define ICR_RTIC        (1 << 6)
#define ICR_TXIC        (1 << 5)
#define ICR_RXIC        (1 << 4)
#define ICR_DSRMIC	(1 << 3)
#define ICR_DCDMIC	(1 << 2)
#define ICR_CTSMIC	(1 << 1)
#define ICR_RIMIC       (1 << 0)

#define UART0_DMACR     (UART0_BASE + 0x48)     // DMA Control Register
#define DMACR_DMAONERR  (1 << 2)
#define DMACR_TXDMAE    (1 << 1)
#define DMACR_RXDMAE    (1 << 0)


// miniUART defines (see BCM2835 datasheet)
// TODO: go through datasheet and add in #define's for all the
//   register bits. Haven't done that yet so the driver currently
//   uses hardcoded hexadecimal literals.
#define AUX_MU_BASE     (MMIO_BASE + 0x00215000U)
#define MUART_EN        (AUX_MU_BASE + 0x4)
#define MUART_IO_REG    (AUX_MU_BASE + 0x40)
#define MUART_IER_REG   (AUX_MU_BASE + 0x44)
#define MUART_IIR_REG   (AUX_MU_BASE + 0x48)
#define MUART_LCR_REG   (AUX_MU_BASE + 0x4C)
#define MUART_MCR_REG   (AUX_MU_BASE + 0x50)
#define MUART_LSR_REG   (AUX_MU_BASE + 0x54)
#define MUART_MSR_REG   (AUX_MU_BASE + 0x58)
#define MUART_SCRATCH   (AUX_MU_BASE + 0x5C)
#define MUART_CR_REG    (AUX_MU_BASE + 0x60)
#define MUART_ST_REG    (AUX_MU_BASE + 0x64)
#define MUART_BD_REG    (AUX_MU_BASE + 0x68)

/* These last 4 are "reserved for test purposes," according to ARM */
#define UART0_ITCR      (UART0_BASE + 0x80)
#define UART0_ITIP      (UART0_BASE + 0x84)
#define UART0_ITOP      (UART0_BASE + 0x88)
#define UART0_TDR       (UART0_BASE + 0x8C)

// The offsets for Mailbox registers
#define MBOX_BASE       (MMIO_BASE + 0xB880)
#define MBOX_READ       (MBOX_BASE + 0x00)
#define MBOX_STATUS     (MBOX_BASE + 0x18)
#define MBOX_WRITE      (MBOX_BASE + 0x20)


#include "peripherals/bcm2835.h"
#include "peripherals/bcm2835int.h"
#if (RASPPI == 4)
#include "peripherals/bcm2711.h"
#include "peripherals/bcm2711int.h"
#endif

static inline void mmio_write32(uintptr_t reg, uint32_t data)
{
        *(volatile uint32_t *) reg = data;
}

static inline uint32_t mmio_read32(uintptr_t reg)
{
	return *(volatile uint32_t *) reg;
}