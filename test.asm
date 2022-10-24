; x86 emulator test
; Copyright (c) 2022, Kaimakan71.
; SPDX-License-Identifier: BSD-2-Clause
[bits 16]
[org 0x7c00]

; Exclusive OR register-register expect all registers 0
xor ax, ax
xor cx, cx
xor dx, dx
xor bx, bx

; Move data at address to register
; MOV AX/L, RMW/B expect AX=0000
mov al, [0x0000]
mov ax, [0x0000]

; Move immediate data to register
; MOV RX/L/H, IB/W expect AX=3 BX=C CX=6 DX=9
mov al, 1
mov ah, 2
mov ax, 3
mov cl, 4
mov ch, 5
mov cx, 6
mov dl, 7
mov dh, 8
mov dx, 9
mov bl, 10
mov bh, 11
mov bx, 12

; Flag tests expect flag changes only
cli ; CLI expect !IF
sti ; STI expect IF
cld ; CLD expect !DF
std ; STD expect DF
clc ; CLC expect !CF

; Pushing expect no change
push ax
push cx
push dx
push bx
push bp
push si
push di

; Popping expect same register values as the mov tests
pop di
pop si
pop bp
pop bp
pop dx
pop cx
pop ax

; Forward jump test
jmptest:
	jmp fwd ; JMP expect jump to fwd

; Halt test
halt:
	hlt ; HLT expect halt

; Backward jump test
fwd:
	jmp halt ; JMP expect jump to halt

times 510-($-$$) db 0
dw 0xaa55