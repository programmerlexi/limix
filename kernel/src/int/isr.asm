[bits 64]

[extern fault_handler]

%macro isr_err 1
  isr_%+%1:
    pop rsi
    mov rdi, %1
    jmp isr_common
  [global isr_%+%1]
%endmacro

%macro isr_noerr 1
  isr_%+%1:
    mov rsi, 0
    mov rdi, %1
    jmp isr_common
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

isr_common:
  mov rdx, [rsp]
  call fault_handler
  iretq
