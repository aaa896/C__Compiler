    .global main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $-8 , %rsp
    movl $1 , -4(%rbp)
    movl -4(%rbp) , %edi
    movl $2 , %esi
    movl $3 , %edx
    movl $4 , %ecx
    movl $5 , %r8d
    movl $6 , %r9d
    pushq $8
    pushq $7
    call fun
    addq $16 , %rsp
    movl %eax , -8(%rbp)
    movl -8(%rbp) , %r10d
    movl %r10d , -4(%rbp)
    movl -4(%rbp) , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    movl $0 , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    .global fun
fun:
    pushq %rbp
    movq %rsp, %rbp
    subq $-68 , %rsp
    movl %edi , -8(%rbp)
    movl %esi , -16(%rbp)
    movl %edx , -24(%rbp)
    movl %ecx , -32(%rbp)
    movl %r8d , -40(%rbp)
    movl %r9d , -48(%rbp)
    movl 16(%rbp) , %r10d
    movl %r10d , -56(%rbp)
    movl 24(%rbp) , %r10d
    movl %r10d , -64(%rbp)
    movl -8(%rbp) , %r10d
    movl %r10d , -68(%rbp)
    movl -64(%rbp) , %r10d
    addl %r10d , -68(%rbp)
    movl -68(%rbp) , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    movl $0 , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
