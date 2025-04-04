#include "IR.h"
#include <functional>


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
    bool return_found = false;
    for (IRInstr* instr : instrs) {
        instr->gen_asm(o);
        if (dynamic_cast<IRInstrReturn*>(instr)) {
            return_found = true;
            break;
        }
    }

    // if  exit_true  is a  nullptr, 
    //     the epilogue is generated
    // else if exit_false is a nullptr, 
    //     an unconditional jmp to the exit_true branch is generated
    // else (we have two successors, hence a branch)
    //         an instruction comparing the value of test_var_name to true is generated,
    //         followed by a conditional branch to the exit_false branch,
    //         followed by an unconditional branch to the exit_true branch
    
    if(exit_true != nullptr && exit_false != nullptr && !return_found) {
        o << "    cmpl $0, -" + to_string(test_var_location*4) + "(%rbp)\n";
        o << "    je " + exit_false->label + "\n";
        exit_true->gen_asm(o);
        o << "    jmp " + endif->label + "\n";
        if(exit_false->label != endif->label) {
            o << exit_false->label << ":\n";
            exit_false->gen_asm(o);
        }
        o << endif->label << ":\n";
        endif->gen_asm(o);
    }

}

void CFG::gen_asm(ostream &o) {
    gen_asm_prologue(o);
    bbs[0]->gen_asm(o);
    bool return_missing = check_return_stmt();
    if (return_missing) {
        o << "    movl $0, %eax\n";
    }
    o << "." << bbs[0]->label << "_out:\n";
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

bool CFG::check_return_stmt() {
    BasicBlock* bb_start = bbs[0];
    BasicBlock* bb_end = nullptr;
    for (BasicBlock* bb : bbs) {
        if (bb->exit_true == nullptr && bb->exit_false == nullptr) {
            bb_end = bb;
            break;
        }
    }

    vector<vector<BasicBlock*>> paths;
    vector<BasicBlock*> current_path;

    function<void(BasicBlock*)> dfs = [&](BasicBlock* bb) {
        current_path.push_back(bb);

        if (bb == bb_end) {
            paths.push_back(current_path);
        } else {
            if (bb->exit_true) {
                dfs(bb->exit_true);
            }
            if (bb->exit_false) {
                dfs(bb->exit_false);
            }
        }

        current_path.pop_back();
    };

    dfs(bb_start);

    // // Print paths for debugging
    // for (const auto& path : paths) {
    //     std::cout << "Path: ";
    //     for (const auto& block : path) {
    //         std::cout << block->label << " ";
    //     }
    //     std::cout << "\n";
    // }

    bool return_missing = false;
    for (const auto& path : paths) {
        bool return_found = false;
        for (const auto& block : path) {
            for (const auto& instr : block->instrs) {
                if (dynamic_cast<IRInstrReturn*>(instr)) {
                    return_found = true;
                    break;
                }
            }
            if (return_found) {
                break;
            }
        }
        if (!return_found) {
            return_missing = true;
            break;
        }
    }
    return return_missing;
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

void IRInstrReturn::gen_asm(ostream &o)
{
    o << "    jmp ." << this->bb->cfg->bbs[0]->label << "_out\n";
}
