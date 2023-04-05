/*
 * gpio.h
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

#pragma once

#include "types.h"

/* This GPIO driver supports initializing Pins, configuring their input/output mode and
 * AF mode, and reading/writing to them. It does not (yet) support interrupts.
 */

typedef enum {
	GPIO_Input,
	GPIO_Output,
	GPIO_InputPullUp,
	GPIO_InputPullDown,
	GPIO_AF0,
	GPIO_AF1,
	GPIO_AF2,
	GPIO_AF3,
	GPIO_AF4,
	GPIO_AF5,

	GPIO_LASTMODE
} GPIOPinMode;

struct GPIOPin {
	u32 pin_num;
	u32 reg_offset;
	u32 reg_mask;
	GPIOPinMode mode;
};

void GPIOPin_init(struct GPIOPin *pin, u32 pin_num, GPIOPinMode mode);
void GPIOPin_write(struct GPIOPin *pin, BOOL val);
BOOL GPIOPin_read(struct GPIOPin *pin);