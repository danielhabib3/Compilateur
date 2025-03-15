.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    movl $10, -4(%rbp)
    movl $3, %eax
    popq %rbp
    ret
