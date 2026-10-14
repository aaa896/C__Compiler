    .global main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16 , %rsp
    movl $3 , -4(%rbp)
    movl $2 , -4(%rbp)
.Lloop_start.0:
    cmpl $20,-4(%rbp)
    movl $0 , -8(%rbp)
    setl -8(%rbp)
    movl $0 , %r11d
    cmpl -8(%rbp),%r11d
    je .Lloop_end.0
    movl -4(%rbp) , %r10d
    movl %r10d , -12(%rbp)
    addl $4 , -12(%rbp)
    movl -12(%rbp) , %r10d
    movl %r10d , -4(%rbp)
    movl -4(%rbp) , %r10d
    movl %r10d , -16(%rbp)
    addl $1 , -16(%rbp)
    movl -16(%rbp) , %r10d
    movl %r10d , -4(%rbp)
    jmp .Lloop_start.0
.Lloop_end.0:
    movl -4(%rbp) , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    movl $0 , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
