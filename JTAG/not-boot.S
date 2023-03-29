/*
 * not-boot.S - dummy "kernel" to be used when JTAG debugging.
 *  Build and load this onto the SD card as the kernel image. On reset,
 *  we load the *real* kernel into RAM over JTAG, but we still need
 *  something for the bootstrap to load before the debugger connects.
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

        .section .text.boot
        .globl _start
_start:
        b _start