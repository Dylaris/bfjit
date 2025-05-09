format ELF64

;; void printchar(char *ptr);
public printchar

printchar:
    mov rax, 1      ; write
    mov rsi, rdi    ; ptr
    mov rdi, 1      ; fd
    mov rdx, 1      ; size
    syscall
    ret

;; char readchar(void);
public readchar
readchar:
    mov rax, 0      ; read
    mov rdi, 0      ; fd
    mov rsi, buffer ; buf
    mov rdx, 1      ; size
    syscall
    xor rax, rax
    mov al, [buffer]
    ret

buffer: db 0
