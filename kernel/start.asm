	[BITS  32]
	global _start
	extern kernel_main

	section .text

_start:
	;   Set up data segments
	mov ax, 0x10; Kernel data selector
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	;   Set up stack
	mov esp, stack_top

	; Disable interrupts during initialization
	cli

	;    Pass memory map parameters to kernel
	push dword [0x7FF0]; Entry count
	push dword 0x8000; Memory map address
	call kernel_main

	; If kernel returns, halt system
	cli

.hang:
	hlt
	jmp .hang

section .bss
resb    16384; 16KB stack

stack_top:
