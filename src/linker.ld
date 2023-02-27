ENTRY(_start)

SECTIONS
{
    .init : {
        *(.init)
    }

    .text :  {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    . = ALIGN(0x8);
    .bss : {
        bss_begin = .;

        *(.bss*)
        *(COMMON)

        bss_end = .;
    }
}