

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