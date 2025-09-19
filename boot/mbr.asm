; Master Boot Record
BITS 16
ORG 0x7C00

start:
    mov si, msg1

.print_char:
    lodsb
    or al, al
    jz done 
    mov ah, 0x0E
    mov bh, 0x00
    mov bl, 0x07
    int 0x10
    jmp .print_char

done:
  hlt

msg1 db "Hello, world !", 0

times 510-($-$$) db 0
dw 0xAA55

