.globl main
 main: 
    pushq %rbp
    movq %rsp, %rbp
    movl $2, %eax
    popq %rbp
    ret
