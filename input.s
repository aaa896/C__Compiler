    .global main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $20 , %rsp
    movl $4 , -4(%rbp)
    movl -4(%rbp) , %r10d
    movl %r10d , -8(%rbp)
    addl $1 , -8(%rbp)
    movl -8(%rbp) , %r10d
    movl %r10d , -4(%rbp)
.Lloop_start.0:
    movl -4(%rbp) , %r10d
    movl %r10d , -12(%rbp)
    addl $1 , -12(%rbp)
    movl -12(%rbp) , %r10d
    movl %r10d , -4(%rbp)
    cmpl $10,-4(%rbp)
    movl $0 , -16(%rbp)
    sete -16(%rbp)
    movl $0 , %r11d
    cmpl -16(%rbp),%r11d
    je .Llabel.0
    jmp .Lloop_start.0
    jmp .Llabel.1
.Llabel.0:
.Llabel.1:
    cmpl $10,-4(%rbp)
    movl $0 , -20(%rbp)
    setl -20(%rbp)
    movl $0 , %r11d
    cmpl -20(%rbp),%r11d
    je .Lloop_end.0
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
