    .global main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $0 , %rsp
    jmp .Lend
    movl $1 , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.Lend:
    movl $3 , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    movl $0 , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
