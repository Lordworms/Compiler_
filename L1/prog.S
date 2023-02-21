.text
  .globl go
go:
pushq %rbx
pushq %rbp
pushq %r12
pushq %r13
pushq %r14
pushq %r15
call _main
popq %rbx
popq %rbp
popq %r12
popq %r13
popq %r14
popq %r15
retq


_encode:
movq %rdi,%r10
_NNNNNNNNNNN_global_1:
jmp _NNNNNNNNNNN_global_0

_NNNNNNNNNNN_global_0:
movq %r10,%r10
salq $1,%r10
movq %r10,%r10
movq %r10,%r10
addq $1,%r10
movq %r10,%r10
movq %r10,%rax
retq


_decode:
movq %rdi,%r10
_NNNNNNNNNNN_global_3:
jmp _NNNNNNNNNNN_global_2

_NNNNNNNNNNN_global_2:
movq %r10,%r10
sarq $1,%r10
movq %r10,%rax
retq


_main:
subq $56, %rsp
_NNNNNNNNNNN_global_9:
jmp _NNNNNNNNNNN_global_4

_NNNNNNNNNNN_global_8:
movq 40(%rsp),%r10
movq %r10,%rdi
call print
movq %rax,%r10
addq $56, %rsp
retq
_NNNNNNNNNNN_global_10:
movq 8(%rsp),%r10
movq %r10,%r10
addq $2,%r10
movq %r10,%r10
movq %r10,8(%rsp)
movq $1,%r10
movq %r10,0(%rsp)
movq $1,%r10
movq 16(%rsp),%r10
movq %r10,%r10
subq $2,%r10
movq %r10,%r10
movq %r10,16(%rsp)
movq 8(%rsp),%r10
movq 32(%rsp),%r11
cmpq %r11, %r10
jl _NNNNNNNNNNN_global_6

jmp _NNNNNNNNNNN_global_8

_NNNNNNNNNNN_global_6:
movq 0(%rsp),%r10
movq %r10,%rdi
movq $_NNNNNNNNNNN_decode_0,-8(%rsp)
subq $8, %rsp
jmp _decode
_NNNNNNNNNNN_decode_0:
movq %rax,%r10
movq %r10,%r11
movq %r11,%r10
addq $1,%r10
movq %r10,%r8
movq $1,%r10
movq $0,%r10
addq %r11,%r10
movq %r10,%r10
imulq $8,%r10
movq %r10,%r10
addq $16,%r10
movq 40(%rsp),%r9
movq %r9,%r9
addq %r10,%r9
movq 0(%r9),%r10
movq %r10,%rax
movq $1,%r10
movq $0,%r10
addq %r8,%r10
movq %r10,%r10
imulq $8,%r10
movq %r10,%r9
addq $16,%r9
movq 40(%rsp),%r10
movq %r10,%r10
addq %r9,%r10
movq 0(%r10),%r10
movq %r10,%r9
cmpq %rax, %r9
jl _NNNNNNNNNNN_global_11

jmp _NNNNNNNNNNN_global_7

_NNNNNNNNNNN_global_7:
movq 0(%rsp),%r10
movq %r10,%r10
addq $2,%r10
movq %r10,%r10
movq %r10,0(%rsp)
movq 0(%rsp),%r10
movq 16(%rsp),%r11
cmpq %r11, %r10
jl _NNNNNNNNNNN_global_6

jmp _NNNNNNNNNNN_global_10

_NNNNNNNNNNN_global_11:
movq $1,%r10
movq $0,%r10
addq %r8,%r10
movq %r10,%r10
imulq $8,%r10
movq %r10,%r8
addq $16,%r8
movq 40(%rsp),%r10
movq %r10,%r10
addq %r8,%r10
movq %rax,0(%r10)
movq $1,%r10
movq $0,%r10
addq %r11,%r10
movq %r10,%r10
imulq $8,%r10
movq %r10,%r10
addq $16,%r10
movq 40(%rsp),%r11
movq %r11,%r11
addq %r10,%r11
movq %r9,0(%r11)
jmp _NNNNNNNNNNN_global_7

_NNNNNNNNNNN_global_5:
movq $1,%r10
movq 48(%rsp),%r10
movq %r10,%rdi
movq $_NNNNNNNNNNN_decode_1,-8(%rsp)
subq $8, %rsp
jmp _decode
_NNNNNNNNNNN_decode_1:
movq %rax,%r11
movq $0,%r10
addq %r11,%r10
movq %r10,%r10
imulq $8,%r10
movq %r10,%r10
addq $16,%r10
movq 40(%rsp),%r11
movq %r11,%r11
movq %r11,24(%rsp)
movq 24(%rsp),%r11
addq %r10,%r11
movq %r11,24(%rsp)
call input
movq %rax,%r10
movq 24(%rsp),%r11
movq %r10,0(%r11)
movq 48(%rsp),%r10
movq %r10,%r10
addq $2,%r10
movq %r10,%r10
movq %r10,48(%rsp)
movq 32(%rsp),%r11
movq 48(%rsp),%r10
cmpq %r11, %r10
jl _NNNNNNNNNNN_global_5

jmp _NNNNNNNNNNN_global_6

_NNNNNNNNNNN_global_4:
call input
movq %rax,%r10
movq %r10,%r10
movq %r10,32(%rsp)
movq 32(%rsp),%r10
movq %r10,%r10
subq $2,%r10
movq %r10,%r10
movq %r10,16(%rsp)
movq 32(%rsp),%r10
movq %r10,%r10
sarq $1,%r10
movq %r10,%r10
movq %r10,%r10
salq $1,%r10
movq %r10,%r10
movq %r10,%r10
addq $1,%r10
movq %r10,%r10
movq %r10,%r10
addq $2,%r10
movq %r10,%r10
movq %r10,%rdi
movq $1,%rsi
call allocate
movq %rax,%r10
movq %r10,%r10
movq %r10,40(%rsp)
movq 40(%rsp),%r10
movq %r10,%r10
addq $8,%r10
movq 32(%rsp),%r11
movq %r11,0(%r10)
movq $1,%r10
movq %r10,0(%rsp)
movq $1,%r10
movq $1,%r10
movq %r10,48(%rsp)
movq $1,%r10
movq %r10,8(%rsp)
jmp _NNNNNNNNNNN_global_5



