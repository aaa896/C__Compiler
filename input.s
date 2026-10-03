    .global main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $44 , %rsp
    movl $1 , -16(%rbp)
    addl $1 , -16(%rbp)
    movl -16(%rbp) , %r10d
    movl %r10d , -4(%rbp)
    movl $1 , -8(%rbp)
.Lloop_start.0:
    cmpl $3,-4(%rbp)
    movl $0 , -20(%rbp)
    setl -20(%rbp)
    movl $0 , %r11d
    cmpl -20(%rbp),%r11d
    je .Lloop_end.0
    cmpl $2,-4(%rbp)
    movl $0 , -24(%rbp)
    sete -24(%rbp)
    movl $0 , %r11d
    cmpl -24(%rbp),%r11d
    jne .Lbody.2
    cmpl $0,-4(%rbp)
    movl $0 , -28(%rbp)
    sete -28(%rbp)
    movl $0 , %r11d
    cmpl -28(%rbp),%r11d
    jne .Lbody.8
    jmp .Lbody.7
.Lbody.2:
    cmpl $1,-8(%rbp)
    movl $0 , -32(%rbp)
    sete -32(%rbp)
    movl $0 , %r11d
    cmpl -32(%rbp),%r11d
    jne .Lbody.4
    jmp .Lbody.6
.Lbody.4:
    movl $0 , -12(%rbp)
.Lloop_start.5:
    cmpl $32,-12(%rbp)
    movl $0 , -36(%rbp)
    setl -36(%rbp)
    movl $0 , %r11d
    cmpl -36(%rbp),%r11d
    je .Lloop_end.5
    movl -4(%rbp) , %r10d
    movl %r10d , -40(%rbp)
    addl $4 , -40(%rbp)
    movl -40(%rbp) , %r10d
    movl %r10d , -4(%rbp)
    movl -12(%rbp) , %r10d
    movl %r10d , -44(%rbp)
    addl $1 , -44(%rbp)
    movl -44(%rbp) , %r10d
    movl %r10d , -12(%rbp)
    jmp .Lloop_start.5
.Lloop_end.5:
    jmp .Lloop_start.0
.Lbody.6:
    jmp .Lbreak.3
.Lbreak.3:
    jmp .Lbreak.1
.Lbody.7:
    movl $23 , -4(%rbp)
.Lbody.8:
    movl $2 , -4(%rbp)
    jmp .Lbreak.1
.Lbreak.1:
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
