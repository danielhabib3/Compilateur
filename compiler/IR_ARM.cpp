#include "IR.h"

void BasicBlock::gen_asm(ostream &o) {
    for (IRInstr* instr : instrs) {
        instr->gen_asm(o);
    }

    if (exit_true == nullptr) {
        // Fin du programme, épilogue sera appelé
    } else if (exit_false == nullptr) {
        o << "\tb " << exit_true->label << "\n";
    } else {
        o << "\tcmp " << test_var_name << ", #0\n";
        o << "\tbeq " << exit_false->label << "\n";
        o << "\tb " << exit_true->label << "\n";
    }
}

void CFG::gen_asm(ostream &o) {
    gen_asm_prologue(o);
    for (BasicBlock* bb : bbs) {
        bb->gen_asm(o);
    }
    gen_asm_epilogue(o);
}

void CFG::gen_asm_prologue(ostream &o) {
    #ifdef __APPLE__
    o << ".globl _main\n_main:\n";
    #else
    o << ".globl main\nmain:\n";
    #endif
    o << "\tpush {fp, lr}\n";
    o << "\tadd fp, sp, #4\n";
}

void CFG::gen_asm_epilogue(ostream &o) {
    o << "\tpop {fp, pc}\n";
}

void IRInstrAffect::gen_asm(ostream &o) {
    o << "\tmov " << dest << ", " << op1 << "\n";
}

void IRInstrAdd::gen_asm(ostream &o) {
    o << "\tadd " << dest << ", " << dest << ", " << op1 << "\n";
}

void IRInstrSub::gen_asm(ostream &o) {
    o << "\tsub " << dest << ", " << op1 << ", " << op2 << "\n";
}

void IRInstrMul::gen_asm(ostream &o) {
    o << "\tmul " << dest << ", " << op1 << ", " << dest << "\n";
}

void IRInstrDiv::gen_asm(ostream &o) {
    o << "\tmov r0, " << op1 << "\n";
    o << "\tmov r1, " << op2 << "\n";
    o << "\tbl __aeabi_idiv\n";
    o << "\tmov " << dest << ", r0\n";
}

void IRInstrMod::gen_asm(ostream &o) {
    o << "\tmov r0, " << op1 << "\n";
    o << "\tmov r1, " << op2 << "\n";
    o << "\tbl __aeabi_idivmod\n";
    o << "\tmov " << dest << ", r1\n";
}

void IRInstrCmpEQ::gen_asm(ostream &o) {
    o << "\tcmp " << op1 << ", " << dest << "\n";
    o << "\tmoveq " << dest << ", #1\n";
    o << "\tmovne " << dest << ", #0\n";
}

void IRInstrCmpNE::gen_asm(ostream &o) {
    o << "\tcmp " << op1 << ", " << dest << "\n";
    o << "\tmovne " << dest << ", #1\n";
    o << "\tmoveq " << dest << ", #0\n";
}

void IRInstrCmpLT::gen_asm(ostream &o) {
    o << "\tcmp " << op1 << ", " << dest << "\n";
    o << "\tmovlt " << dest << ", #1\n";
    o << "\tmovge " << dest << ", #0\n";
}

void IRInstrCmpGT::gen_asm(ostream &o) {
    o << "\tcmp " << op1 << ", " << dest << "\n";
    o << "\tmovgt " << dest << ", #1\n";
    o << "\tmovle " << dest << ", #0\n";
}

void IRInstrXorBit::gen_asm(ostream &o) {
    o << "\teor " << dest << ", " << op1 << ", " << op2 << "\n";
}

void IRInstrNotUnary::gen_asm(ostream &o){
    o << "\tcmp " << op1 << ", #0\n";
    o << "\tmoveq " << dest << ", #1\n";
    o << "\tmovne " << dest << ", #0\n";	
}

void IRInstrSubUnary::gen_asm(ostream &o){
    o << "\trsb " << op1 << ", " << op1 << ", #0\n";
}

void IRInstrAndBit::gen_asm(ostream &o) {
    o << "\tand " << dest << ", " << op1 << ", " << op2 << "\n";
}

void IRInstrOrBit::gen_asm(ostream &o) {
    o << "\torr " << dest << ", " << op1 << ", " << op2 << "\n";
}