#include "IR.h"

string to_x86(string s) {
    if(s[0] != '$') {
        if(s[0] == '0') {
            s = "%eax";
        } else {
            int num = stoi(s) * 4;
            s = "-" + to_string(num) + "(%rbp)";
        }
    }
    return s;
}

void BasicBlock::gen_asm(ostream &o) {
    // o << label << ":\n";
    for (IRInstr* instr : instrs) {
        instr->gen_asm(o);
    }

    if (exit_true == nullptr) {
        // o << "\tleave\n\tret\n";
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
        // o << bb->label << ":\n";
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
    dest = to_x86(dest);
    op1 = to_x86(op1);
    o << "    movl " << op1 << ", " << dest << "\n";
}

void IRInstrAdd::gen_asm(ostream &o) {
    dest = to_x86(dest);
    op1 = to_x86(op1);
    o << "    addl " << op1 << ", " << dest << "\n";
}

void IRInstrSub::gen_asm(ostream &o) {
    dest = to_x86(dest);
    op1 = to_x86(op1);
    op2 = to_x86(op2);
    o << "    movl " << dest << ", " << op2 << "\n";
    o << "    movl " << op1 << ", " << dest << "\n";
    o << "    subl " << op2 << ", " << dest << "\n";
}

void IRInstrMul::gen_asm(ostream &o) {
    dest = to_x86(dest);
    op1 = to_x86(op1);
    o << "    imull " << op1 << ", " << dest << "\n";
}

void IRInstrDiv::gen_asm(ostream &o) {
    dest = to_x86(dest);
    op1 = to_x86(op1);
    op2 = to_x86(op2);
    o << "    movl " << dest << ", " << op2 << "\n";
    o << "    movl " << op1 << ", " << dest << "\n";
    o << "    cltd\n";
    o << "    idivl " << op2 << "\n";
}


void IRInstrMod::gen_asm(ostream &o) {
    dest = to_x86(dest);
    op1 = to_x86(op1);
    op2 = to_x86(op2);
    o << "    movl " << dest << ", " << op2 << "\n";
    o << "    movl " << op1 << ", " << dest << "\n";
    o << "    cltd\n";
    o << "    idivl " << op2 << "\n";
    o << "    movl %edx, %eax\n" ;
}

void IRInstrCmpEQ::gen_asm(ostream &o) {
    dest = to_x86(dest);
    op1 = to_x86(op1);
    o << "    cmpl " << dest << ", " << op1 << "\n";
    o << "    sete %al\n";
    o << "    movzbl %al, " << dest << "\n";
}

void IRInstrCmpNE::gen_asm(ostream &o) {
    dest = to_x86(dest);
    op1 = to_x86(op1);
    o << "    cmpl " << dest << ", " << op1 << "\n";
    o << "    setne %al\n";
    o << "    movzbl %al, " << dest << "\n";
}
void IRInstrCmpLT::gen_asm(ostream &o) {
    dest = to_x86(dest);
    op1 = to_x86(op1);
    o << "    cmpl " << dest << ", " << op1 << "\n";
    o << "    setl %al\n";
    o << "    movzbl %al, " << dest << "\n";
}

void IRInstrXorBit::gen_asm(ostream &o) {
    dest = to_x86(dest);
    op1 = to_x86(op1);
    op2 = to_x86(op2);
    o << "    xorl " << op1 << ", " << dest << "\n";
}

void IRInstrNotUnary::gen_asm(ostream &o){
    dest = to_x86(dest);
    op1 = to_x86(op1);
    o << "    cmpl $0, " << op1 << "\n";
    o << "    sete %al\n";
    o << "    movzbl %al, " << dest << "\n";

}

void IRInstrSubUnary::gen_asm(ostream &o){
    op1 = to_x86(op1);
    o << "    negl " << op1 << "\n";
}

void IRInstrCmpGT::gen_asm(ostream &o) {
    dest = to_x86(dest);
    op1 = to_x86(op1);
    o << "    cmpl " << dest << ", " << op1 << "\n";
    o << "    setg %al\n";
    o << "    movzbl %al, " << dest << "\n";
}

void IRInstrAndBit::gen_asm(ostream &o) {
    dest = to_x86(dest);
    op1 = to_x86(op1);
    op2 = to_x86(op2);
    o << "    andl " << op1 << ", " << dest << "\n";
}

void IRInstrOrBit::gen_asm(ostream &o) {
    dest = to_x86(dest);
    op1 = to_x86(op1);
    op2 = to_x86(op2);
    o << "    orl " << op1 << ", " << dest << "\n";
}