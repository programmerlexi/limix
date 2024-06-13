[bits 64]
[extern irq_handler]

%macro irq_enter 0
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
%endmacro

%macro irq_exit 0
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
  pop rbx
  pop rax
  pop rbp
%endmacro

%macro irq 1
  irq_%+%1:
    irq_enter
    push %1
    call irq_common
    pop rax
    irq_exit
    iretq
  [global irq_%+%1]
%endmacro

irq 0
irq 1
irq 2
irq 3
irq 4
irq 5
irq 6
irq 7
irq 8
irq 9
irq 10
irq 11
irq 12
irq 13
irq 14
irq 15

irq_common:
  mov rdi, [rsp+8]
  call irq_handler
  ret

