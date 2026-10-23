    .global main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $20 , %rsp
    call foo
    movl %eax , -12(%rbp)
    movl -12(%rbp) , %r10d
    movl %r10d , -4(%rbp)
    cmpl $0,-4(%rbp)
    movl $0 , -16(%rbp)
    setg -16(%rbp)
    movl $0 , %r11d
    cmpl -16(%rbp),%r11d
    je .Llabel.0
    movl $3 , -8(%rbp)
    movl -4(%rbp) , %r10d
    movl %r10d , -20(%rbp)
    movl -8(%rbp) , %r10d
    addl %r10d , -20(%rbp)
    movl -20(%rbp) , %r10d
    movl %r10d , -4(%rbp)
    jmp .Llabel.1
.Llabel.0:
.Llabel.1:
    addq $20 , %rsp
    movl -4(%rbp) , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    addq $20 , %rsp
    movl $0 , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    .global foo
foo:
    pushq %rbp
    movq %rsp, %rbp
    subq $0 , %rsp
    addq $0 , %rsp
    movl $4 , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    addq $0 , %rsp
    movl $0 , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
