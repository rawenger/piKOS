/*
 * printk_config.h - config file for printf library used to implement printk
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

#include "peripherals/mini_uart.h"
#include "peripherals/pl011_uart.h"
#include "pikos_config.h"

#ifdef USE_MUART
#define _putchar(c)     do { muart_send((c)); } while (0)
#else
#define _putchar(c)     do { uart_send(PIKOS_LOG_LINE, (c)); } while (0)
#endif

/* Disable floating point printing */
#define PRINTK_DISABLE_SUPPORT_FLOAT
#define PRINTK_DISABLE_SUPPORT_EXPONENTIAL
