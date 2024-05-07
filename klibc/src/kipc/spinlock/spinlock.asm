spinlock:
  lock bts dword [rdi], 0
  jc .spin_with_pause
  ret

.spin_with_pause:
  pause
  test dword [rdi], 1
  jnz .spin_with_pause
  jmp spinlock

[global spinlock]
