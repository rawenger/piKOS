#include "mmio.h"
#include "util/utils.h"
#include "peripherals/uart0.h"
#include "peripherals/mini_uart.h"
#include "exceptions.h"

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

	// enable Tx & Rx interrupts
	mmio_write32(UART0_IMSC, IMSC_RXIM | IMSC_TXIM);
	// set Rx & Tx FIFO interrupt trigger levels to 1/8 full
	mmio_write32(UART0_IFLS, (IFLS_IFSEL_1_8 << IFLS_RXIFSEL_SHIFT) | (IFLS_IFSEL_1_8 << IFLS_TXIFSEL_SHIFT));

	// enable tx,rx
	mmio_write32(UART0_CR, CR_UART_EN_MASK | CR_TXE_MASK | CR_RXE_MASK);
}

/* purposely don't buffer this! we will do that in a separate kernel thread (watch_keyboard) */
static char console_read_char;

__attribute__((optimize(3)))
void uart0_irq_handler(void)
{
	uint32_t int_type = mmio_read32(UART0_MIS);
	mmio_write32(UART0_ICR, int_type);

	if (int_type & MIS_RXMIS) {
		console_read_char = mmio_read32(UART0_DR);
//#ifdef DEBUG
//		printk("Received character: '%c'\r\n", console_read_char);
//#endif
		call_KOS_handler(ConsoleReadInt);
	}
	if (int_type & MIS_TXMIS) {
		/* console_write() is what actually accesses the DR here */
		call_KOS_handler(ConsoleWriteInt);
	}

#ifdef DEBUG
	if (int_type & ~(MIS_TXMIS | MIS_RXMIS))
	printk("other interrupt occurred in uart0\r\n");
#endif
}

void dump_console_buf(void)
{
	console_buf[15] = '\0';
	printk("console_buf: %s\r\n", console_buf);
}
