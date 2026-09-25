    .global main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $52 , %rsp
    movl $1 , -4(%rbp)
    movl $10 , -8(%rbp)
.Lloop_start.0:
    cmpl $32,-8(%rbp)
    movl $0 , -20(%rbp)
    setl -20(%rbp)
    movl $0 , %r11d
    cmpl -20(%rbp),%r11d
    je .Lloop_end.0
    movl $0 , -12(%rbp)
.Lloop_start.1:
    cmpl $3,-12(%rbp)
    movl $0 , -24(%rbp)
    setl -24(%rbp)
    movl $0 , %r11d
    cmpl -24(%rbp),%r11d
    je .Lloop_end.1
    movl -8(%rbp) , %r10d
    movl %r10d , -28(%rbp)
    movl -12(%rbp) , %r10d
    addl %r10d , -28(%rbp)
    movl -28(%rbp) , %r10d
    movl %r10d , -8(%rbp)
    movl -12(%rbp) , %r10d
    movl %r10d , -32(%rbp)
    addl $1 , -32(%rbp)
    movl -32(%rbp) , %r10d
    movl %r10d , -12(%rbp)
    jmp .Lloop_start.1
.Lloop_end.1:
    movl $7 , -36(%rbp)
    negl -36(%rbp)
    movl -36(%rbp) , %r10d
    movl %r10d , -40(%rbp)
    notl -40(%rbp)
    movl -40(%rbp) , %r10d
    movl %r10d , -16(%rbp)
    movl -8(%rbp) , %r10d
    movl %r10d , -44(%rbp)
    movl -16(%rbp) , %r10d
    movl -44(%rbp) , %r11d
    imull %r10d , %r11d
    movl %r11d , -44(%rbp)
    movl -44(%rbp) , %r10d
    movl %r10d , -8(%rbp)
    movl -8(%rbp) , %r10d
    movl %r10d , -48(%rbp)
    addl $98 , -48(%rbp)
    movl -48(%rbp) , %r10d
    movl %r10d , -8(%rbp)
    movl -8(%rbp) , %r10d
    movl %r10d , -4(%rbp)
    movl -8(%rbp) , %r10d
    movl %r10d , -52(%rbp)
    addl $1 , -52(%rbp)
    movl -52(%rbp) , %r10d
    movl %r10d , -8(%rbp)
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
