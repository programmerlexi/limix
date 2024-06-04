[bits 64]

[extern fault_handler]

discard: dq 0

%macro isr_enter 0
  push rbp
  mov rbp, rsp
  
  push rax
  push rbx
  push rcx
  push rdx

  push rsi
  push rdi

  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  pushfq
%endmacro

%macro isr_exit 0
  popfq
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rdi
  pop rsi
  pop rdx
  pop rcx
  mov rax, [rbp]
  mov rbx, [rbp+8]
  mov [rbp+8], rax
  mov [rbp], rbx
  pop rbx
  pop rax
  pop rbp
  pop rbp
%endmacro

%macro isr_err 1
  isr_%+%1:
    isr_enter
    push %1
    call isr_common
    pop rax
    isr_exit
    iretq
  [global isr_%+%1]
%endmacro

%macro isr_noerr 1
  isr_%+%1:
    push 0
    isr_enter
    push %1
    call isr_common
    pop rax
    isr_exit
    iretq
  [global isr_%+%1]
%endmacro

isr_noerr 0
isr_noerr 1
isr_noerr 2
isr_noerr 3
isr_noerr 4
isr_noerr 5
isr_noerr 6
isr_noerr 7
isr_err    8
isr_noerr 9
isr_err    10
isr_err    11
isr_err    12
isr_err    13
isr_err    14
isr_noerr 15
isr_noerr 16
isr_err    17
isr_noerr 18
isr_noerr 19
isr_noerr 20
isr_noerr 21
isr_noerr 22
isr_noerr 23
isr_noerr 24
isr_noerr 25
isr_noerr 26
isr_noerr 27
isr_noerr 28
isr_noerr 29
isr_err    30
isr_noerr 31
isr_noerr 32

isr_common:
  mov rdi, [rsp+8]
  mov rsi, [rbp+8]
  mov rdx, [rbp+16]
  mov rcx, [rbp+40]
  call fault_handler
  ret

_panic:
  cli
  push rbp
  mov rbp, rsp
  push rax
  push rbx
  push rcx
  push rdx

  push rsi
  push rdi

  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  mov rdi, rsp
  call panic_handle
  .hcf:
  hlt
  jmp .hcf
  [global _panic]
  [extern panic_handle]
