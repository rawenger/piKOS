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

    .bss : {
        bss_begin = .;

        *(.bss*)
        *(COMMON)

        bss_end = .;
    }
}