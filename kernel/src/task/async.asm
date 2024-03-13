exec_switch:
  push rsp
  mov rbp, rsp

  push rdx
  push rcx
  push rbx
  push r12
  push r13
  push r14
  push r15

  mov [rdi], rsp
  mov rsp, [rsi]

  pop r15
  pop r14
  pop r13
  pop r12
  pop rbx
  pop rsi
  pop rdi

  pop rbp
  ret

[global exec_switch]
