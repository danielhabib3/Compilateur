#include "IR.h"

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
        o << ".globl " << entry_label << "\n";
        o << entry_label << ":\n";
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
    o << "    addl " << op1 << ", " << dest << "\n";
}

void IRInstrSub::gen_asm(ostream &o) {
    o << "    movl " << dest << ", " << op2 << "\n";
    o << "    movl " << op1 << ", " << dest << "\n";
    o << "    subl " << op2 << ", " << dest << "\n";
}

void IRInstrMul::gen_asm(ostream &o) {
    o << "    imull " << op1 << ", " << dest << "\n";
}

void IRInstrDiv::gen_asm(ostream &o) {
    o << "    movl " << dest << ", " << op2 << "\n";
    o << "    movl " << op1 << ", " << dest << "\n";
    o << "    cltd\n";
    o << "    idivl " << op2 << "\n";
}
 

void IRInstrMod::gen_asm(ostream &o) {
    o << "    movl " << dest << ", " << op2 << "\n";
    o << "    movl " << op1 << ", " << dest << "\n";
    o << "    cltd\n";
    o << "    idivl " << op2 << "\n";
    o << "    movl %edx, %eax\n" ;  
}

void IRInstrCmpEQ::gen_asm(ostream &o) {
    o << "    cmpl " << dest << ", " << op1 << "\n";
    o << "    sete %al\n";
    o << "    movzbl %al, " << dest << "\n";
}

void IRInstrCmpNE::gen_asm(ostream &o) {
    o << "    cmpl " << dest << ", " << op1 << "\n";
    o << "    setne %al\n";
    o << "    movzbl %al, " << dest << "\n";
}
void IRInstrCmpLT::gen_asm(ostream &o) {
    o << "    cmpl " << dest << ", " << op1 << "\n";
    o << "    setl %al\n";
    o << "    movzbl %al, " << dest << "\n";
}

void IRInstrXorBit::gen_asm(ostream &o) {
    o << "    xorl " << op2 << ", " << op1 << "\n";
    o << "    movl " << op1 << ", " << dest << "\n";
}

void IRInstrNotUnary::gen_asm(ostream &o){
    o << "    cmpl $0, " << op1 << "\n";
    o << "    sete %al\n";
    o << "    movzbl %al, " << dest << "\n";	

}

void IRInstrSubUnary::gen_asm(ostream &o){
    o << "    negl " << op1 << "\n";
}

void IRInstrCmpGT::gen_asm(ostream &o) {
    o << "    cmpl " << dest << ", " << op1 << "\n";
    o << "    setg %al\n";
    o << "    movzbl %al, " << dest << "\n";
}

void IRInstrAndBit::gen_asm(ostream &o) {
    o << "    andl " << op2 << ", " << op1 << "\n";
    o << "    movl " << op1 << ", " << dest << "\n";
}

void IRInstrOrBit::gen_asm(ostream &o) {
    o << "    orl " << op2 << ", " << op1 << "\n";
    o << "    movl " << op1 << ", " << dest << "\n";
}

void IRInstrFunc_Call::gen_asm(std::ostream &o) {
    std::vector<std::string> argRegisters = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};

    for (size_t i = 0; i < args.size(); ++i) {
        if (i >= argRegisters.size()) {
            std::cerr << "Error: function call with more than 6 arguments not supported yet." << std::endl;
            exit(1);
        }
        o << "    movl " << args[i] << ", " << argRegisters[i] << std::endl;
    }

    o << "    call " << func_name << std::endl;

    if (dest != "") {
        o << "    movl %eax, " << dest << std::endl;
    }
}



