.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    movl $2, -4(%rbp)
    movl -4(%rbp), %eax
    movl %eax, -8(%rbp)
    movl -8(%rbp), %eax
    popq %rbp
    ret
