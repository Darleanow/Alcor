; Loader, switches from real mode to protected mode and loads kernel
[BITS 16]
ORG 0x7E00                 ; This code will be loaded at physical address 0x7E00

start_loader:
    call load_kernel       ; Load the kernel from disk into memory

    cli                    ; Disable interrupts while switching to protected mode
    lgdt [gdtr]            ; Load address/size of our Global Descriptor Table

    ; Enable the A20 line (allow access beyond 1 MB)
    in  al, 0x92
    or  al, 2
    out 0x92, al

    ; Set the PE (Protection Enable) bit in CR0 to enter protected mode
    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    ; Far jump to 32-bit code segment (selector 0x08) to flush pipeline
    jmp 0x08:protected_mode_entry


; Load the kernel sectors from disk into memory
load_kernel:
    mov ax, 0x3000         ; ES = 0x3000 → destination segment = 0x3000:0x0000
    mov es, ax
    xor bx, bx             ; BX = 0 → offset inside ES segment

    mov ah, 0x02           ; BIOS INT 13h function 02h: Read sectors
    mov al, 0x10           ; Read 16 sectors
    mov ch, 0x00           ; Cylinder 0
    mov cl, 0x03           ; Starting at sector 3 (sectors 1–2 already used)
    mov dh, 0x00           ; Head 0
    mov dl, 0x80           ; First hard disk

    int 0x13               ; BIOS disk service
    ret


; Global Descriptor Table (GDT)
gdt_start:
    dq 0x0000000000000000  ; Null descriptor (mandatory first entry)
    dq 0x00CF9A000000FFFF  ; Code segment: base=0, limit=4 GB, present, 32-bit
    dq 0x00CF92000000FFFF  ; Data segment: base=0, limit=4 GB, read/write
gdt_end:

; GDTR structure for lgdt: [size-1][base]
gdtr:
    dw gdt_end - gdt_start - 1   ; Size of GDT minus 1
    dd gdt_start                 ; Linear address of GDT


; 32-bit protected mode code
[BITS 32]
protected_mode_entry:
    mov ax, 0x10            ; Data segment selector (second descriptor in GDT)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000         ; Set up a stack in high memory

    call clear_screen        ; Clear text mode screen
    jmp 0x30000              ; Jump to the kernel loaded at physical 0x30000


; Simple routine to clear the VGA text screen
clear_screen:
    mov edi, 0xB8000         ; Start of VGA text buffer
    mov eax, 0x07200720      ; Two characters with attribute 0x07 (space + attribute)
    mov ecx, 1000            ; 80×25 / 2 dwords = 1000 doublewords
    rep stosd                ; Fill screen with spaces
    ret


; Pad the sector to 512 bytes (standard boot sector size)
times 512-($-$$) db 0

