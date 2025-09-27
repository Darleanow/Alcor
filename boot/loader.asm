	;     Second stage bootloader
	[BITS 16]
	ORG   0x7E00

start_loader:
	call detect_memory_e820
	call load_kernel
	cli
	lgdt [gdtr]
	call enable_a20
	call enter_protected_mode
	jmp  0x08:protected_mode_entry

	; Detect memory using BIOS E820

detect_memory_e820:
	mov edi, 0x8000; Memory map buffer
	mov dword [0x7FF0], 0; Entry counter
	xor ebx, ebx; Continuation value
	mov bp, 0; Entry count

.loop:
	mov eax, 0xE820
	mov ecx, 24
	mov edx, 0x534D4150; "SMAP"
	int 0x15

	jc  .fallback; Error - use fallback
	cmp eax, 0x534D4150
	jne .fallback
	cmp ecx, 20
	jb  .skip_entry

	inc bp
	add edi, 24

	test ebx, ebx
	jz   .done
	cmp  bp, 20; Max 20 entries
	jae  .done
	jmp  .loop

.skip_entry:
	test ebx, ebx
	jz   .done
	jmp  .loop

.fallback:
	;   Create fallback memory map
	mov edi, 0x8000
	mov bp, 2

	;   Entry 1: 0-639KB
	mov dword [edi], 0
	mov dword [edi+4], 0
	mov dword [edi+8], 0x9FC00
	mov dword [edi+12], 0
	mov dword [edi+16], 1
	mov dword [edi+20], 0

	;   Entry 2: 1MB-32MB
	add edi, 24
	mov dword [edi], 0x100000
	mov dword [edi+4], 0
	mov dword [edi+8], 0x1F00000
	mov dword [edi+12], 0
	mov dword [edi+16], 1
	mov dword [edi+20], 0

.done:
	mov [0x7FF0], bp
	mov word [0x7FF2], 0; Zero-extend to dword
	ret

	; Load kernel from disk

load_kernel:
	mov ax, 0x3000
	mov es, ax
	xor bx, bx

	mov ah, 0x02
	mov al, 0x20; 32 sectors
	mov ch, 0x00
	mov cl, 0x03
	mov dh, 0x00
	mov dl, 0x80
	int 0x13
	ret

	; Enable A20 line

enable_a20:
	in  al, 0x92
	or  al, 2
	out 0x92, al
	ret

	; Enter protected mode

enter_protected_mode:
	mov eax, cr0
	or  eax, 1
	mov cr0, eax
	ret

	; Global Descriptor Table

gdt_start:
	dq 0x0000000000000000; Null
	dq 0x00CF9A000000FFFF; Kernel code
	dq 0x00CF92000000FFFF; Kernel data
	dq 0x00CFFA000000FFFF; User code
	dq 0x00CFF2000000FFFF; User data

gdt_end:

gdtr:
	dw gdt_end - gdt_start - 1
	dd gdt_start

	;     Protected mode entry
	[BITS 32]

protected_mode_entry:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, 0x90000

	call clear_screen
	jmp  0x08:0x30000; Jump to kernel

clear_screen:
	mov edi, 0xB8000
	mov eax, 0x07200720
	mov ecx, 1000
	rep stosd
	ret

times 512-($-$$) db 0
