[org 0x7C00]

KERNEL_LOCATION  equ 0x1000         
KERNEL_SECTORS   equ 4            
REALMODE_STACK   equ 0x7000
PM_STACK         equ 0x9000         

    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, REALMODE_STACK
    sti                            

    mov [BOOT_DISK], dl

reset_disk:
    mov ah, 0x00
    mov dl, [BOOT_DISK]
    int 0x13
    jc reset_disk               

read_kernel:
    mov bx, KERNEL_LOCATION
    mov ah, 0x02          
    mov al, KERNEL_SECTORS
    mov ch, 0x00           
    mov dh, 0x00           
    mov cl, 0x02            
    mov dl, [BOOT_DISK]
    int 0x13
    jc .read_fail
    cmp ah, 0
    jne .read_fail
    jmp short after_read

.read_fail:
    mov si, 3
.retry:
    mov ah, 0x00
    mov dl, [BOOT_DISK]
    int 0x13
    jc .retry_reset_fail
    
    mov bx, KERNEL_LOCATION
    mov ah, 0x02
    mov al, KERNEL_SECTORS
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02
    mov dl, [BOOT_DISK]
    int 0x13
    jnc after_read
    
    dec si
    jnz .retry
    jmp .give_up

.retry_reset_fail:
    dec si
    jnz .retry

.give_up:
    mov si, disk_err_msg
    call puts
    hlt
    jmp $

after_read:
    mov ah, 0x0
    mov ax, 0x0003
    int 0x10

enable_a20:
    ; Try BIOS method first
    mov ax, 0x2401
    int 0x15
    jnc a20_done          
    
    ; Fall back to keyboard controller method
    call a20_wait
    mov al, 0xAD
    out 0x64, al           
    call a20_wait
    
    mov al, 0xD0
    out 0x64, al         
    call a20_wait2
    in al, 0x60
    push eax
    
    call a20_wait
    mov al, 0xD1
    out 0x64, al         
    call a20_wait
    
    pop eax
    or al, 2           
    out 0x60, al          
    
    call a20_wait
    mov al, 0xAE         
    out 0x64, al
    call a20_wait
    
    ; Test A20
    call check_a20
    jc a20_done             
    
    ; Final fallback: fast A20 method (carefully implemented for VMware)
    in al, 0x92
    test al, 2
    jnz a20_done         
    or al, 2
    and al, ~1             
    out 0x92, al

a20_done:
    cli                    

    lgdt [GDT_descriptor]
    mov eax, cr0
    or eax, 1             
    mov cr0, eax

    jmp 0x08:protected_mode


a20_wait:
    in al, 0x64
    test al, 2
    jnz a20_wait
    ret

a20_wait2:
    in al, 0x64
    test al, 1
    jz a20_wait2
    ret

check_a20:
    pushf
    push ds
    push es
    push di
    push si
    
    cli
    xor ax, ax
    mov es, ax
    mov di, 0x0500
    mov ax, 0xFFFF
    mov ds, ax
    mov si, 0x0510
    
    mov al, [es:di]
    push ax
    mov al, [ds:si]
    push ax
    
    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF
    
    cmp byte [es:di], 0xFF
    pop ax
    mov [ds:si], al
    pop ax
    mov [es:di], al
    
    je .a20_disabled
    clc                
    jmp .done
    
.a20_disabled:
    stc                    
    
.done:
    pop si
    pop di
    pop es
    pop ds
    popf
    ret

disk_err_msg db 'Disk error!', 0

BOOT_DISK db 0

puts:
    pusha
    mov ah, 0x0E
    mov bh, 0x00
.puts_loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .puts_loop
.done:
    popa
    ret

align 8
GDT_start:
GDT_null:                           dq 0

GDT_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

GDT_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

GDT_end:

GDT_descriptor:
    dw GDT_end - GDT_start - 1
    dd GDT_start

[bits 32]
protected_mode:
    mov ax, 0x10      
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov esp, PM_STACK

    jmp 0x08:KERNEL_LOCATION

times 510-($-$$) db 0
dw 0xAA55