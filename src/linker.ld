/*
 * linker.ld - piKOS linker script. Need I say more?
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

ENTRY(_start)

SECTIONS
{
    . = (0xFFFF << 48) | 0x80000; /* KERN_VM_BASE + kern.img base paddr */

    .text.boot : {
        *(.text.boot)
    }

    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    . = ALIGN(8);
    .bss : {
        bss_begin = .;

        *(.bss*)
        *(COMMON)

        bss_end = .;
    }

/* we may end up using this later, but for now it's unused */
/*    . = ALIGN(0x1000);*/
/*    pg_root = .;*/
/*    .pgdesc_area : {*/
/*        . += (5 * 0x1000);*/
/*    }*/
/**/

    kern_img_end = .;
}