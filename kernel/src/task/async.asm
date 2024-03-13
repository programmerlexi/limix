exec_first_switch:
  push rbp
  mov rbp, rsp

  pushf

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

exec_switch:
  push rbp
  mov rbp, rsp

  pushf

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

  popf

  pop rbp
  ret

[global exec_first_switch]
[global exec_switch]
