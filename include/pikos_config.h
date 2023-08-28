/*
 * kos_config.h - modifiable define's for various parameters of piKOS
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

/*
 * Pi 1-3 boards have two UARTs: the PL011 (UART0), and the mini-UART (mUART). Unfortunately,
 *   both devices are connected to the same GPIO pins, so there is no way to run jumper wires
 *   to both simultaneously. This means we have to choose between either a serial tty or a serial debug
 *   line--not both. QEMU, of course, doesn't care about physical GPIO pins, and can support both the mini
 *   and PL011 UARTs at the same time. Further, note that use of the mini UART device as a TTY
 *   backend is not supported.
 *
 * The Pi 4 also has this problem, except there are 4 additional PL011 UART's--and they're wired
 *   to different GPIO pins, so we can physically connect up to 5 UART devices simultaneously, allowing
 *   for both a debug line and a tty.
 */

// LOG() and printk() macros are handled via PIKOS_LOG_LINE

#if RASPPI == 3
  #ifdef QEMU
    #define PIKOS_LOG_LINE  MUART
    #define USE_MUART
  #else
    #define PIKOS_LOG_LINE  NULL
  #endif
  #define PIKOS_TTY0      UART0
  #define PIKOS_TTY1      NULL
  #define PIKOS_TTY2      NULL
  #define PIKOS_TTY3      NULL
  #define PIKOS_TTY4      NULL
#elif RASPPI == 4
  #ifdef QEMU
    #error "QEMU does not support Raspberry Pi 4"
  #endif
  #define PIKOS_LOG_LINE  UART0
  #define PIKOS_TTY0      UART1
  #define PIKOS_TTY1      UART2
  #define PIKOS_TTY2      UART3
  #define PIKOS_TTY3      UART4
  #define PIKOS_TTY4      NULL
#endif
