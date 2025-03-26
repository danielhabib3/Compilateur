#include "IR.h"

void BasicBlock::gen_asm(ostream &o) {
    o << label << ":\n"; 
    for (IRInstr* instr : instrs) {
        instr->gen_asm(o);
    }
    
    if (exit_true == nullptr) {
        o << "\tleave\n\tret\n";
    } else if (exit_false == nullptr) {
        o << "\tjmp " << exit_true->label << "\n";
    } else {
        o << "\tcmp $0, " << test_var_name << "\n";
        o << "\tje " << exit_false->label << "\n";
        o << "\tjmp " << exit_true->label << "\n";
    }
}

void CFG::gen_asm(ostream &o) {
    gen_asm_prologue(o);
    for (BasicBlock* bb : bbs) {
        o << bb->label << ":\n";
        bb->gen_asm(o);
    }
    gen_asm_epilogue(o);
}

void CFG::gen_asm_prologue(ostream &o) {
    #ifdef __APPLE__
    o << ".globl _main\n" ;
    o << " _main: \n" ;
    #else
    o << ".globl main\n" ;
    o << " main: \n" ;
    #endif
    o << "    pushq %rbp\n";
    o << "    movq %rsp, %rbp\n";
}

void CFG::gen_asm_epilogue(ostream &o) {
    o << "    popq %rbp\n";
    o << "    ret\n";
}

void IRInstrAffect::gen_asm(ostream &o) {
    o << "    movl " << op1 << ", " << dest << "\n";
}

void IRInstrAdd::gen_asm(ostream &o) {
    o << "    addl " << op2 << ", " << op1 << "\n";
    o << "    movl " << op1 << ", " << dest << "\n";
}

void IRInstrSub::gen_asm(ostream &o) {
    o << "    subl " << op2 << ", " << op1 << "\n";
    o << "    movl " << op1 << ", " << dest << "\n";
}

void IRInstrMul::gen_asm(ostream &o) {
    o << "    imull " << op2 << ", " << op1 << "\n";
    o << "    movl " << op1 << ", " << dest << "\n";
}

void IRInstrDiv::gen_asm(ostream &o) {
    o << "    movl " << op1 << ", %eax\n";
    o << "    cltd\n";
    o << "    idivl " << op2 << "\n";
    o << "    movl %eax, " << dest << "\n";
}
 

void IRIntrMod::gen_asm(ostream &o) {
    o << "    movl " << op1 << ", %eax\n";
    o << "    cltd\n";
    o << "    idivl " << op2 << "\n";
    o << "    movl %edx, " << dest << "\n";
    o << "    movl %edx, %eax\n" ;  
}

void IRInstrCmpEQ::gen_asm(ostream &o) {
    o << "    cmpl " << op1 << ", " << op2 << "\n";
    o << "    sete %al\n";
    o << "    movzbl %al, " << dest << "\n";
}

void IRInstrCmpNE::gen_asm(ostream &o) {
    o << "    cmpl " << op1 << ", " << op2 << "\n";
    o << "    setne %al\n";
    o << "    movzbl %al, " << dest << "\n";
}
void IRInstrCmpLT::gen_asm(ostream &o) {
    o << "    cmpl " << op1 << ", " << op2 << "\n";
    o << "    setl %al\n";
    o << "    movzbl %al, " << dest << "\n";
}

void IRInstrXorBit::gen_asm(ostream &o) {
    o << "    xorl " << op2 << ", " << op1 << "\n";
    o << "    movl " << op1 << ", " << dest << "\n";
}

void IRInstrNotUnary::gen_asm(ostream &o){

}
void IRInstrSubUnary::gen_asm(ostream &o){

}

