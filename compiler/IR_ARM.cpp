#include "IR.h"

bool is_mem(const string& s) {
    return s[0] != '$' && s[0] != '0';
}

string to_arm_reg(const string& s) {
    if (s[0] == '$') {
        return "#" + s.substr(1);  // Immediate values are prefixed with #
    } else if (s[0] == '0') {
        return "w0";  // ARM register
    } else {
        return "w1";  // Temporary register
    }
}

string to_arm_mem(const string& s) {
    int num = stoi(s) * 4;  // Convert memory offset to byte offset (ARM uses bytes)
    return "[sp, #-" + to_string(num) + "]";  // ARM memory addressing format
}

void BasicBlock::gen_asm(ostream &o) {
    for (IRInstr* instr : instrs) {
        instr->gen_asm(o);
    }

    if (exit_true == nullptr) {
        // End of program, epilogue will be called
    } else if (exit_false == nullptr) {
        o << "    b " << exit_true->label << "\n";
    } else {
        o << "    ldr w1, " << to_arm_mem(test_var_name) << "\n";
        o << "    cmp w1, #0\n";
        o << "    beq " << exit_false->label << "\n";
        o << "    b " << exit_true->label << "\n";
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
    o << ".globl _main\n";
    o << "_main:\n";
    #else
    o << ".globl main\n";
    o << "main:\n";
    #endif

    o << "    stp x29, x30, [sp, #-16]!\n";
    o << "    mov x29, sp\n";
}

void CFG::gen_asm_epilogue(ostream &o) {
    o << "    ldp x29, x30, [sp], #16\n";
    o << "    ret\n";
}

void IRInstrAffect::gen_asm(ostream &o) {
    if (is_mem(dest) && is_mem(op1)) {
        o << "    ldr w1, " << to_arm_mem(op1) << "\n";
        o << "    str w1, " << to_arm_mem(dest) << "\n";
    } else if (is_mem(dest)) {
        o << "    mov w1, " << to_arm_reg(op1) << "\n";
        o << "    str w1, " << to_arm_mem(dest) << "\n";
    } else if (is_mem(op1)) {
        o << "    ldr " << to_arm_reg(dest) << ", " << to_arm_mem(op1) << "\n";
    } else {
        o << "    mov " << to_arm_reg(dest) << ", " << to_arm_reg(op1) << "\n";
    }
}

void IRInstrAdd::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(dest) << "\n";
    o << "    add w0, w0, " << to_arm_reg(op1) << "\n";
    o << "    str w0, " << to_arm_mem(dest) << "\n";
}

void IRInstrSub::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(op1) << "\n";
    o << "    sub w0, w0, " << to_arm_reg(op2) << "\n";
    o << "    str w0, " << to_arm_mem(dest) << "\n";
}

void IRInstrMul::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(op1) << "\n";
    o << "    mul w0, w0, " << to_arm_reg(op2) << "\n";
    o << "    str w0, " << to_arm_mem(dest) << "\n";
}

void IRInstrDiv::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(op1) << "\n";
    o << "    ldr w1, " << to_arm_mem(op2) << "\n";
    o << "    sdiv w0, w0, w1" << "\n";
    o << "    str w0, " << to_arm_mem(dest) << "\n";
}

void IRInstrMod::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(op1) << "\n";
    o << "    ldr w1, " << to_arm_mem(op2) << "\n";
    // ARM does not support direct modulo, would need to call a library function
    // o << "\tbl __aeabi_idivmod\n";  // Call division/mod function
    o << "    str w1, " << to_arm_mem(dest) << "\n";  // Storing remainder in destination
}

void IRInstrCmpEQ::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(op1) << "\n";
    o << "    cmp w0, " << to_arm_reg(op2) << "\n";
    o << "    cset w0, eq\n";
    o << "    str w0, " << to_arm_mem(dest) << "\n";
}

void IRInstrCmpNE::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(op1) << "\n";
    o << "    cmp w0, " << to_arm_reg(op2) << "\n";
    o << "    cset w0, ne\n";
    o << "    str w0, " << to_arm_mem(dest) << "\n";
}

void IRInstrCmpLT::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(op1) << "\n";
    o << "    cmp w0, " << to_arm_reg(op2) << "\n";
    o << "    cset w0, lt\n";
    o << "    str w0, " << to_arm_mem(dest) << "\n";
}

void IRInstrCmpGT::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(op1) << "\n";
    o << "    cmp w0, " << to_arm_reg(op2) << "\n";
    o << "    cset w0, gt\n";
    o << "    str w0, " << to_arm_mem(dest) << "\n";
}

void IRInstrXorBit::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(op1) << "\n";
    o << "    eor w0, w0, " << to_arm_reg(op2) << "\n";
    o << "    str w0, " << to_arm_mem(dest) << "\n";
}

void IRInstrNotUnary::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(op1) << "\n";
    o << "    cmp w0, #0\n";
    o << "    cset w0, eq\n";
    o << "    str w0, " << to_arm_mem(dest) << "\n";
}

void IRInstrSubUnary::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(op1) << "\n";
    o << "    rsb w0, w0, #0\n";  // Reverse subtract to negate the value
    o << "    str w0, " << to_arm_mem(dest) << "\n";
}

void IRInstrAndBit::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(op1) << "\n";
    o << "    and w0, w0, " << to_arm_reg(op2) << "\n";
    o << "    str w0, " << to_arm_mem(dest) << "\n";
}

void IRInstrOrBit::gen_asm(ostream &o) {
    o << "    ldr w0, " << to_arm_mem(op1) << "\n";
    o << "    orr w0, w0, " << to_arm_reg(op2) << "\n";
    o << "    str w0, " << to_arm_mem(dest) << "\n";
}