[bits  32]
extern isr_handler
extern irq_handler

	;      Macro for ISRs without error code
	%macro ISR_NOERR 1
	global isr%1

isr%1:
	cli  ; Disable interruptions
	push byte 0; Push dummy error code
	push byte %1; Push interrupt number
	jmp  isr_common_stub; Jump to common handler
%endmacro

	;      Macro for ISRs with error code
	%macro ISR_ERR 1
	global isr%1

isr%1:
	cli  ; Disable interruptions
	push byte %1; Push interrupt number (error code already pushed)
	jmp  isr_common_stub; Jump to common handler
%endmacro

	;      Macro for IRQs (remapped to 32+)
	%macro IRQ 2
	global isr%1

isr%1:
	cli  ; Disable interruptions
	push byte 0; Push dummy error code
	push byte %1; Push interrupt number
	jmp  irq_common_stub; Jump to IRQ handler
%endmacro

	; ---- Exceptions 0-31 ----
	ISR_NOERR 0     ; Division By Zero
	ISR_NOERR 1     ; Debug
	ISR_NOERR 2     ; Non Maskable Interrupt
	ISR_NOERR 3     ; Breakpoint
	ISR_NOERR 4     ; Into Detected Overflow
	ISR_NOERR 5     ; Out of Bounds
	ISR_NOERR 6     ; Invalid Opcode
	ISR_NOERR 7     ; No Coprocessor
	ISR_ERR   8     ; Double Fault (with error code)
	ISR_NOERR 9     ; Coprocessor Segment Overrun
	ISR_ERR   10    ; Bad TSS (with error code)
	ISR_ERR   11    ; Segment Not Present (with error code)
	ISR_ERR   12    ; Stack Fault (with error code)
	ISR_ERR   13    ; General Protection Fault (with error code)
	ISR_ERR   14    ; Page Fault (with error code)
	ISR_NOERR 15    ; Unknown Interrupt
	ISR_NOERR 16    ; Coprocessor Fault
	ISR_ERR   17    ; Alignment Check (with error code)
	ISR_NOERR 18    ; Machine Check
	ISR_NOERR 19    ; SIMD Floating-Point Exception
	ISR_NOERR 20    ; Reserved
	ISR_NOERR 21    ; Reserved
	ISR_NOERR 22    ; Reserved
	ISR_NOERR 23    ; Reserved
	ISR_NOERR 24    ; Reserved
	ISR_NOERR 25    ; Reserved
	ISR_NOERR 26    ; Reserved
	ISR_NOERR 27    ; Reserved
	ISR_NOERR 28    ; Reserved
	ISR_NOERR 29    ; Reserved
	ISR_NOERR 30    ; Reserved
	ISR_NOERR 31    ; Reserved

	;   ---- IRQ remapped 32-47 ----
	IRQ 32, 0; Timer
	IRQ 33, 1; Keyboard
	IRQ 34, 2; Cascade (never raised)
	IRQ 35, 3; COM2
	IRQ 36, 4; COM1
	IRQ 37, 5; LPT2
	IRQ 38, 6; Floppy
	IRQ 39, 7; LPT1
	IRQ 40, 8; CMOS real-time clock
	IRQ 41, 9; Free for peripherals
	IRQ 42, 10; Free for peripherals
	IRQ 43, 11; Free for peripherals
	IRQ 44, 12; PS2 Mouse
	IRQ 45, 13; FPU / Coprocessor / Inter-processor
	IRQ 46, 14; Primary ATA Hard Disk
	IRQ 47, 15; Secondary ATA Hard Disk

	; Common handler for exceptions

isr_common_stub:
	pusha ; Save all general-purpose registers

	mov  ax, ds; Save the data segment
	push eax

	mov ax, 0x10; Load kernel data segment
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call isr_handler; Call C handler

	pop ebx; Restore original data segment
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx

	popa ; Restore all general-purpose registers
	add  esp, 8; Clean up interrupt number and error code
	sti  ; Re-enable interruptions
	iret ; Return from interrupt

	; Common handler for IRQs

irq_common_stub:
	pusha ; Save all general-purpose registers

	mov  ax, ds; Save the data segment
	push eax

	mov ax, 0x10; Load kernel data segment
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call irq_handler; Call C handler

	pop ebx; Restore original data segment
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx

	popa ; Restore all general-purpose registers
	add  esp, 8; Clean up interrupt number and error code
	sti  ; Re-enable interruptions
	iret ; Return from interrupt
