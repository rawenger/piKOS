/*
 * gpio.c
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

#include "peripherals/gpio.h"
#include "util/utils.h"
#include "assert.h"
#include "mmio.h"

#define GPIO_PINS       54

typedef enum {
	/* order here matters! */
	NOPUPD = 0,
	PullDown = 1,
	PullUp = 2
} GPIOPinPUPD;

//typedef enum {
//	Input = 0,
//	Output = 1,
//	AF0 = 4,
//	AF1 = 5,
//	AF2 = 6,
//	AF3 = 7,
//	AF4 = 3,
//	AF5 = 2
//} GPIOPinAFMode;
static const u8 GPIOPinAFMode[] = {4,5,6,7,3,2};

static void set_mode(struct GPIOPin *pin, GPIOPinMode mode);
static void set_pupd(struct GPIOPin *pin, GPIOPinPUPD pull_mode);
static void set_fselect(struct GPIOPin *pin, u8 func);
static void reset_pin(struct GPIOPin *pin);

void GPIOPin_init(struct GPIOPin *pin, u32 pin_num, GPIOPinMode mode)
{
	assert(pin != NULL);
	assert(pin_num < GPIO_PINS);
	pin->pin_num = pin_num;
	pin->reg_offset = (pin_num >> 5) << 2;
	pin->reg_mask = 1 << (pin_num & 31);

	reset_pin(pin);
	set_mode(pin, mode);
}

void GPIOPin_write(struct GPIOPin *pin, BOOL val)
{
	assert(pin != NULL);

	uintptr screg = pin->reg_offset + (val ? ARM_GPIO_GPSET0 : ARM_GPIO_GPCLR0);
	vmmio_write32(screg, pin->reg_mask);
}

BOOL GPIOPin_read(struct GPIOPin *pin)
{
	assert(pin != NULL);

	return (vmmio_read32(ARM_GPIO_GPLEV0 + pin->reg_offset) & pin->reg_mask)
		? TRUE : FALSE;
}

void set_mode(struct GPIOPin *pin, GPIOPinMode mode)
{
	assert(mode < GPIO_LASTMODE);
	pin->mode = mode;

	if (mode >= GPIO_AF0 && mode <= GPIO_AF5) {
		set_pupd(pin, NOPUPD);
		set_fselect(pin, GPIOPinAFMode[mode - GPIO_AF0]);
		return;
	}

	if (mode == GPIO_Output)
		set_pupd(pin, NOPUPD);

	set_fselect(pin, mode == GPIO_Output ? GPIO_Output : GPIO_Input);

	switch (mode) {
		case GPIO_Input:
			set_pupd(pin, NOPUPD);
			break;
		case GPIO_Output:
			GPIOPin_write(pin, FALSE);
			break;
		case GPIO_InputPullDown:
			set_pupd(pin, PullDown);
			break;
		case GPIO_InputPullUp:
			set_pupd(pin, PullUp);
			break;
		default:
			break;
	}
}

static void set_pupd(struct GPIOPin *pin, GPIOPinPUPD pull_mode)
{
	assert(pull_mode <= PullUp);
#if RASPPI == 3
	uintptr clk_reg = ARM_GPIO_GPPUDCLK0 + pin->reg_offset;
	vmmio_write32(ARM_GPIO_GPPUD, pull_mode);
	delay(150); // need to wait at least 1us
	vmmio_write32(clk_reg, pin->reg_mask);
	delay(150);
	vmmio_write32(ARM_GPIO_GPPUD, 0);
	vmmio_write32(clk_reg, 0);
#elif RASPPI == 4
	uintptr mode_reg = ARM_GPIO_GPPUPPDN0 + ((pin->pin_num >> 4) << 2);
	u32 shift = (pin->pin_num & 15) << 1;

	/* The bit values of pull-up and pull-down are swapped on the Pi 4
	 * i.e: PullDown = 2 and PullUp = 1
	 */
	if (pull_mode == PullUp)
		pull_mode = PullDown;
	else if (pull_mode == PullDown)
		pull_mode = PullUp;

	u32 mask = vmmio_read32(mode_reg);
	mask &= ~(3 << shift);
	mask |= pull_mode << shift;
	vmmio_write32(mode_reg, mask);
#endif
}

static void set_fselect(struct GPIOPin *pin, u8 func)
{
	assert(0 <= func && func <= 7);
	uintptr fsel = ARM_GPIO_GPFSEL0 + (pin->pin_num / 10) * 4;
	u32 shift = (pin->pin_num % 10) * 3;
	u32 mask = vmmio_read32(fsel);
	mask &= ~(7 << shift);
	mask |= func << shift;
	vmmio_write32(fsel, mask);
}

static void reset_pin(struct GPIOPin *pin)
{
	uintptr fsel = ARM_GPIO_GPFSEL0 + (pin->pin_num / 10) * 4;
	u32 shift = (pin->pin_num % 10) * 3;
	u32 mask = vmmio_read32(fsel);
	mask &= ~(7 << shift);
	vmmio_write32(fsel, mask);
}