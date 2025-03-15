.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    movl $0, -8(%rbp)
    movl -4(%rbp), %eax
    popq %rbp
    ret
