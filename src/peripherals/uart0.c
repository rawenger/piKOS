#include "mmio.h"
#include "util/utils.h"
#include "peripherals/uart0.h"

#define UART0_CLOCK	        (48000000UL)

#define BRD(br)		        ((UART0_CLOCK) / (16.0 * (br)))
#define BRD_115200	        (BRD(115200ul))
#define IBRD_115200	        ((unsigned int)(BRD_115200))
#define FBRD_115200	        ((unsigned int)(((BRD_115200-IBRD_115200)*64)+0.5))

void uart0_init()
{
	uint32_t mask;

	mask = mmio_read32(ARM_GPIO_GPFSEL1);
	mask &= ~(7 << 12); // reset gpio14
	mask &= ~(7 << 15); // reset gpio15
	mask &= ~(4 << 12); // set AF0 for gpio14
	mask &= ~(4 << 15); // set AF0 for gpio15
	mmio_write32(ARM_GPIO_GPFSEL1, mask);

	mmio_write32(ARM_GPIO_GPPUD,0);
	delay(150);
	mmio_write32(ARM_GPIO_GPPUDCLK0, BIT(14) | BIT(15));
	delay(150);
	mmio_write32(ARM_GPIO_GPPUDCLK0, 0);

	mmio_write32(UART0_CR, 0); // disable
	mmio_write32(UART0_IBRD, IBRD_115200); // setup br 115200
	mmio_write32(UART0_FBRD, FBRD_115200);
	mmio_write32(UART0_LCRH, LCRH_WLEN8_MASK | LCRH_FEN_MASK); // set 8-bit word, enable fifo

	// enable hardware flow control
	mmio_write32(UART0_CR, CR_CTSEN_MASK | CR_RTSEN_MASK);
	// enable DMA for Tx and Rx
	// We will eventually use this to mimic the behavior of KOS's ConsoleRead/ConsoleWrite
	// functions, which perform non-blocking I/O to the kernel's console device
	mmio_write32(UART0_DMACR, DMACR_RXDMAE | DMACR_TXDMAE);

	// enable Tx and Rx interrupts
	mmio_write32(UART0_IMSC, IMSC_TXIM | IMSC_RXIM);

	// enable tx,rx
	mmio_write32(UART0_CR, CR_UART_EN_MASK | CR_TXE_MASK | CR_RXE_MASK);
}

void uart0_send(char c)
{
	while (mmio_read32(UART0_FR) & FR_TXFF_MASK)
		;

	mmio_write32(UART0_DR, c);
}


char uart0_recv()
{
	while (mmio_read32(UART0_FR) & FR_RXFE_MASK)
		;

	return mmio_read32(UART0_DR) & 0xFF;
}

void uart0_send_str(const char *str)
{
	while (*str)
		uart0_send(*str++);
}
