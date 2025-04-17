#include "IR.h"

bool is_literal(const string& s) {
    return s[0] == '$';
}

bool is_w0(const string& s) {
    return s[0] == '0';
}

bool is_mem(const string& s) {
    return ! is_literal(s) && ! is_w0(s) && s[0] != '-';
}

bool is_reg(const string& s) {
    return ! is_literal(s) && (is_w0(s) || s[0] == '-');
}

string to_arm(const string& s) {
    if (s[0] == '$') {
        return "#" + s.substr(1);  // Immediate values are prefixed with #
    } else if (s[0] == '0') {
        return "w0";  // ARM register
    } else if (s[0] == '-') {
        string num = s.substr(1);
        int num_int = stoi(num);
        return "w" + to_string(num_int);
    } else {
        string num = s.substr(1);
        int num_int = stoi(s) * 4;  // Convert memory offset to byte offset (ARM uses bytes)
        return "[sp, #-" + to_string(num_int) + "]";  // ARM memory addressing format
    }
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
        o << "    ldr w1, " << to_arm(test_var_name) << "\n";
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

    o << "    sub  sp, sp, #64\n";
    o << "    str  wzr, [sp, #12]\n";

//    o << "    stp  x29, x30, [sp, #-16]!\n";
//    o << "    mov  x29, sp\n";
}

void CFG::gen_asm_epilogue(ostream &o) {
    o << "    add	sp, sp, #64\n";

    //    o << "    ldp  x29, x30, [sp], #16\n";

    o << "    ret\n";
}

void IRInstrAffect::gen_asm(ostream &o) {
    o << ";Call to Assignment with op1: " << op1 << ", dest: " << dest << "\n";
    if (is_mem(dest) && is_mem(op1)) {
        o << "    ldr  w1, " << to_arm(op1) << "\n";
        o << "    str  w1, " << to_arm(dest) << "\n";
    } else if (is_mem(dest)) {
        o << "    mov  w1, " << to_arm(op1) << "\n";
        o << "    str  w1, " << to_arm(dest) << "\n";
    } else if (is_mem(op1)) {
        o << "    ldr  " << to_arm(dest) << ", " << to_arm(op1) << "\n";
    } else {
        o << "    mov  " << to_arm(dest) << ", " << to_arm(op1) << "\n";
    }
}

void IRInstrAdd::gen_asm(ostream &o) {
    o << ";Call to Add with op1: " << op1 << ", op2: " << op2 << ", dest: " << dest << "\n";
    if (is_mem(op1))
        o << "    ldr  w0, " << to_arm(op1) << "\n";
    else
        o << "    mov  w0, " << to_arm(op1) << "\n";
    if (is_mem(op2))
        o << "    ldr  w1, " << to_arm(op2) << "\n";
    else
        o << "    mov  w1, " << to_arm(op2) << "\n";
    o << "    add  " << to_arm(dest) << ", w0, w1" << "\n";
}

void IRInstrSub::gen_asm(ostream &o) {
    o << ";Call to Sub with op1: " << op1 << ", op2: " << op2 << ", dest: " << dest << "\n";
    if (is_mem(op1))
        o << "    ldr  w0, " << to_arm(op1) << "\n";
    else
        o << "    mov  w0, " << to_arm(op1) << "\n";
    if (is_mem(op2))
        o << "    ldr  w1, " << to_arm(op2) << "\n";
    else
        o << "    mov  w1, " << to_arm(op2) << "\n";
    o << "    sub  " << to_arm(dest) << ", w0, w1" << "\n";
}

void IRInstrMul::gen_asm(ostream &o) {
    o << ";Call to Mult with op1: " << op1 << ", op2: " << op2 << ", dest: " << dest << "\n";
    if (is_mem(op1))
        o << "    ldr  w0, " << to_arm(op1) << "\n";
    else
        o << "    mov  w0, " << to_arm(op1) << "\n";
    if (is_mem(op2))
        o << "    ldr  w1, " << to_arm(op2) << "\n";
    else
        o << "    mov  w1, " << to_arm(op2) << "\n";
    o << "    mul  " << to_arm(dest) << ", w0, w1" << "\n";
}

void IRInstrDiv::gen_asm(ostream &o) {
    o << ";Call to Div with op1: " << op1 << ", op2: " << op2 << ", dest: " << dest << "\n";
    if (is_mem(op1))
        o << "    ldr  w0, " << to_arm(op1) << "\n";
    else
        o << "    mov  w0, " << to_arm(op1) << "\n";
    if (is_mem(op2))
        o << "    ldr  w1, " << to_arm(op2) << "\n";
    else
        o << "    mov  w1, " << to_arm(op2) << "\n";
    o << "    sdiv " << to_arm(dest) << ", w0, w1" << "\n";
}

void IRInstrMod::gen_asm(ostream &o) {
    o << ";Call to Mod with op1: " << op1 << ", op2: " << op2 << ", dest: " << dest << "\n";
    if (is_mem(op1))
        o << "    ldr  w0, " << to_arm(op1) << "\n";
    else
        o << "    mov  w0, " << to_arm(op1) << "\n";
    if (is_mem(op2))
        o << "    ldr  w1, " << to_arm(op2) << "\n";
    else
        o << "    mov  w1, " << to_arm(op2) << "\n";
    o << "    sdiv w2, w0, w1" << "\n";
    o << "    mul  w2, w2, w1" << "\n";
    o << "    subs " << to_arm(dest) << ", w0, w2" << "\n";
}

void IRInstrCmpEQ::gen_asm(ostream &o) {
//    o << "    ldr  w0, " << to_arm(op1) << "\n";
//    o << "    cmp  w0, " << to_arm(op2) << "\n";
//    o << "    cset w0, eq\n";
//    o << "    str  w0, " << to_arm(dest) << "\n";
}

void IRInstrCmpNE::gen_asm(ostream &o) {
//    o << "    ldr  w0, " << to_arm(op1) << "\n";
//    o << "    cmp  w0, " << to_arm(op2) << "\n";
//    o << "    cset w0, ne\n";
//    o << "    str  w0, " << to_arm(dest) << "\n";
}

void IRInstrCmpLT::gen_asm(ostream &o) {
//    o << "    ldr  w0, " << to_arm(op1) << "\n";
//    o << "    cmp  w0, " << to_arm(op2) << "\n";
//    o << "    cset w0, lt\n";
//    o << "    str  w0, " << to_arm(dest) << "\n";
}

void IRInstrCmpGT::gen_asm(ostream &o) {
//    o << "    ldr  w0, " << to_arm(op1) << "\n";
//    o << "    cmp  w0, " << to_arm(op2) << "\n";
//    o << "    cset w0, gt\n";
//    o << "    str  w0, " << to_arm(dest) << "\n";
}

void IRInstrXorBit::gen_asm(ostream &o) {
//    o << "    ldr  w0, " << to_arm(op1) << "\n";
//    o << "    eor  w0, w0, " << to_arm(op2) << "\n";
//    o << "    str  w0, " << to_arm(dest) << "\n";
}

void IRInstrNotUnary::gen_asm(ostream &o) {
//    o << "    ldr  w0, " << to_arm(op1) << "\n";
//    o << "    cmp  w0, #0\n";
//    o << "    cset w0, eq\n";
//    o << "    str  w0, " << to_arm(dest) << "\n";
}

void IRInstrSubUnary::gen_asm(ostream &o) {
//    o << "    ldr  w0, " << to_arm(op1) << "\n";
//    o << "    rsb  w0, w0, #0\n";  // Reverse subtract to negate the value
//    o << "    str  w0, " << to_arm(dest) << "\n";
}

void IRInstrAndBit::gen_asm(ostream &o) {
//    o << "    ldr  w0, " << to_arm(op1) << "\n";
//    o << "    and  w0, w0, " << to_arm(op2) << "\n";
//    o << "    str  w0, " << to_arm(dest) << "\n";
}

void IRInstrOrBit::gen_asm(ostream &o) {
//    o << "    ldr  w0, " << to_arm(op1) << "\n";
//    o << "    orr  w0, w0, " << to_arm(op2) << "\n";
//    o << "    str  w0, " << to_arm(dest) << "\n";
}

void IRInstrReturn::gen_asm(ostream &o) {}

void IRInstrFunc_Call::gen_asm(std::ostream &o) {
//    o << "    call " << func_name << "\n";
}

void IRInstrJump::gen_asm(std::ostream &o) {
//    o << "    jmp " << jump_label << "\n";
}

void IRInstrCmp::gen_asm(ostream &o) {
}

void IRInstrJmpEQ::gen_asm(ostream &o) {
}

void IRInstrLabel::gen_asm(ostream &o) {
}


