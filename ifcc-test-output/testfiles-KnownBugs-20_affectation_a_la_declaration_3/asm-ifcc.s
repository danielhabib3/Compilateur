.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    movl $0, -4(%rbp)
    movl $1, -8(%rbp)
    movl -8(%rbp), %eax
    popq %rbp
    ret
