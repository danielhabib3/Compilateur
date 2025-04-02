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
    for (IRInstr* instr : instrs) {
        instr->gen_asm(o);
    }

    // if  exit_true  is a  nullptr, 
    //     the epilogue is generated
    // else if exit_false is a nullptr, 
    //     an unconditional jmp to the exit_true branch is generated
    // else (we have two successors, hence a branch)
    //         an instruction comparing the value of test_var_name to true is generated,
    //         followed by a conditional branch to the exit_false branch,
    //         followed by an unconditional branch to the exit_true branch
    
    if(exit_true != nullptr && exit_false != nullptr) {
        o << "    cmpl $0, -" + to_string(test_var_location*4) + "(%rbp)\n";
        o << "    je " + exit_false->label + "\n";
        exit_true->gen_asm(o);
        o << "    jmp " + exit_true->exit_true->label + "\n";
        o << exit_false->label << ":\n";
        exit_false->gen_asm(o);
        o << exit_true->exit_true->label << ":\n";
        exit_true->exit_true->gen_asm(o);
    }

}

void CFG::gen_asm(ostream &o) {
    gen_asm_prologue(o);
    bbs[0]->gen_asm(o);
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
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    o << "    movl " << new_op1 << ", " << new_dest << "\n";
}

void IRInstrAdd::gen_asm(ostream &o) {
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    o << "    addl " << new_op1 << ", " << new_dest << "\n";
}

void IRInstrSub::gen_asm(ostream &o) {
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    string new_op2 = to_x86(op2);
    o << "    movl " << new_dest << ", " << new_op2 << "\n";
    o << "    movl " << new_op1 << ", " << new_dest << "\n";
    o << "    subl " << new_op2 << ", " << new_dest << "\n";
}

void IRInstrMul::gen_asm(ostream &o) {
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    o << "    imull " << new_op1 << ", " << new_dest << "\n";
}

void IRInstrDiv::gen_asm(ostream &o) {
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    string new_op2 = to_x86(op2);
    o << "    movl " << new_dest << ", " << new_op2 << "\n";
    o << "    movl " << new_op1 << ", " << new_dest << "\n";
    o << "    cltd\n";
    o << "    idivl " << new_op2 << "\n";
}

void IRInstrMod::gen_asm(ostream &o) {
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    string new_op2 = to_x86(op2);
    o << "    movl " << new_dest << ", " << new_op2 << "\n";
    o << "    movl " << new_op1 << ", " << new_dest << "\n";
    o << "    cltd\n";
    o << "    idivl " << new_op2 << "\n";
    o << "    movl %edx, " << new_dest << "\n";
}

void IRInstrCmpEQ::gen_asm(ostream &o) {
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    o << "    cmpl " << new_dest << ", " << new_op1 << "\n";
    o << "    sete %al\n";
    o << "    movzbl %al, " << new_dest << "\n";
}

void IRInstrCmpNE::gen_asm(ostream &o) {
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    o << "    cmpl " << new_dest << ", " << new_op1 << "\n";
    o << "    setne %al\n";
    o << "    movzbl %al, " << new_dest << "\n";
}

void IRInstrCmpLT::gen_asm(ostream &o) {
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    o << "    cmpl " << new_dest << ", " << new_op1 << "\n";
    o << "    setl %al\n";
    o << "    movzbl %al, " << new_dest << "\n";
}

void IRInstrXorBit::gen_asm(ostream &o) {
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    o << "    xorl " << new_op1 << ", " << new_dest << "\n";
}

void IRInstrNotUnary::gen_asm(ostream &o) {
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    o << "    cmpl $0, " << new_op1 << "\n";
    o << "    sete %al\n";
    o << "    movzbl %al, " << new_dest << "\n";
}

void IRInstrSubUnary::gen_asm(ostream &o) {
    string new_op1 = to_x86(op1);
    o << "    negl " << new_op1 << "\n";
}

void IRInstrCmpGT::gen_asm(ostream &o) {
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    o << "    cmpl " << new_dest << ", " << new_op1 << "\n";
    o << "    setg %al\n";
    o << "    movzbl %al, " << new_dest << "\n";
}

void IRInstrAndBit::gen_asm(ostream &o) {
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    o << "    andl " << new_op1 << ", " << new_dest << "\n";
}

void IRInstrOrBit::gen_asm(ostream &o) {
    string new_dest = to_x86(dest);
    string new_op1 = to_x86(op1);
    o << "    orl " << new_op1 << ", " << new_dest << "\n";
}
