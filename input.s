    .global main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $0 , %rsp
    movl $0 , %r11d
    cmpl $0,%r11d
    je .Llabel.0
    movl $1 , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    jmp .Llabel.1
.Llabel.0:
.Llabel.1:
    movl $0 , %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU-stack,"",@progbits
