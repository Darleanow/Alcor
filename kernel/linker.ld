ENTRY(_start)

SECTIONS
{
    /* Kernel loading address in memory */
    . = 0x30000;

    /* Executable code */
    .text : {
        *(.text*)
    }

    /* Read-only data (const, strings) */
    .rodata : {
        *(.rodata*)
    }

    /* Initialized data */
    .data : {
        *(.data*)
    }

    /* Uninitialized data (bss + COMMON) */
    .bss : {
        *(COMMON)
        *(.bss*)
    }
}
