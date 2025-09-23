; Master Boot Record 
BITS 16 
ORG 0x7C00 
 
start: 
    ; Initialize segments
    cli
    xor ax, ax 
    mov ds, ax 
    mov es, ax 
    mov ss, ax
    mov sp, 0x7C00      ; Stack just below MBR
    sti

    mov si, msg1 
 
.print_char: 
    lodsb 
    or al, al 
    jz .read_loader 
    mov ah, 0x0E 
    mov bh, 0 
    mov bl, 0x07 
    int 0x10 
    jmp .print_char 

; This segment loads the loader.asm at 0x7E00, 512 bits after mbr (0x7C00)
.read_loader: 
    mov bx, 0x7E00      ; Load address
    mov ah, 0x02        ; Read sectors function
    mov al, 0x01        ; Read 1 sector (the loader)
    mov ch, 0x00        ; Cylinder 0
    mov cl, 0x02        ; Sector 2 (loader is second sector)
    mov dh, 0x00        ; Head 0
    mov dl, 0x80        ; Drive 0x80 (first hard disk)
 
    int 0x13 
    jc .disk_error      ; Jump if carry flag set (error)
   
    ; Jump to loaded code
    jmp 0x0000:0x7E00 

.disk_error:
    mov si, error_msg
.print_error:
    lodsb
    or al, al
    jz .hang
    mov ah, 0x0E
    mov bl, 0x0C        ; Red color for error
    int 0x10
    jmp .print_error

.hang:
    hlt
    jmp .hang
 
msg1 db "Hello, world! Loading...", 0x0D, 0x0A, 0
error_msg db "Disk read error!", 0x0D, 0x0A, 0

; Pad to 510
times 510-($-$$) db 0 
dw 0xAA55 ; Signature
