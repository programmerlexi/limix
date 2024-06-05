_syscall:
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

  cld

  mov rdi, rsp

  call syscall

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
  iretq

[global _syscall]
[extern syscall]
