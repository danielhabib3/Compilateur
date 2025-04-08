#include "IR.h"
#include <functional>


string to_x86(string s) {
    if(s[0] != '$') {
        if(s[0] == '0') {
            s = "%eax";
        } else if(s[0] == '-') {
            string num = s.substr(1);
            int num_int = stoi(num);
            switch (num_int) {
                case 1:
                    s = "%ecx";
                    break;
            }
        } else {
            int num = stoi(s) * 4;
            s = "-" + to_string(num) + "(%rbp)";
        }
    }
    return s;
}

bool BasicBlock::has_return_instr() {
    for (IRInstr* instr : instrs) {
        if (dynamic_cast<IRInstrReturn*>(instr)) {
            return true;
        }
    }
    return false;
}

void BasicBlock::gen_asm(ostream &o) {
    if(!this->already_generated) {
        if (this->label != "main") {
            o << this->label + ":\n";
        }
        this->already_generated = true;

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
            if(this->test_type == TEST_IF) {
                o << "    cmpl $0, -" + to_string(test_var_location*4) + "(%rbp)\n";
                o << "    je " + exit_false->label + "\n";
                exit_true->gen_asm(o);
                if (!exit_true->has_return_instr()) {
                    o << "    jmp " + endif->label + "\n";
                }
                if(exit_false->label != endif->label) {
                    exit_false->gen_asm(o);
                    if (!exit_true->has_return_instr()) {
                        o << "    jmp " + endif->label + "\n";
                    }
                }
                // on genere le endif que s'il n'y a pas de return dans le if et le else
                // ou si le else n'est pas le endif
                if(!(exit_true->has_return_instr() && exit_false->label != endif->label && exit_false->has_return_instr())) {
                    endif->gen_asm(o);
                }
            }
            else if(this->test_type == TEST_WHILE) {
                o << "    cmpl $0, -" + to_string(test_var_location*4) + "(%rbp)\n";
                o << "    je " + exit_false->label + "\n";
                exit_true->gen_asm(o);
                if (!exit_true->has_return_instr()) {
                    o << "    jmp " + this->label + "\n";
                }
                exit_false->gen_asm(o);
            }
        } else if(exit_true != nullptr && exit_false == nullptr && !return_found && exit_true->label.rfind(".endif", 0) != 0) {
            exit_true->gen_asm(o);
        }
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
            if (bb->exit_true && count(current_path.begin(), current_path.end(), bb->exit_true) < 2) {
                dfs(bb->exit_true);
            }
            if (bb->exit_false  && count(current_path.begin(), current_path.end(), bb->exit_false) < 2) {
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
    if(offsetAppliedTo == 0) {
        string new_dest = to_x86(dest);
        string new_op1 = to_x86(op1);
        o << "    movl " << new_op1 << ", " << new_dest << "\n";
    } else {
        if(offsetAppliedTo == 1) {
            // dest c'est un tableau

            // cltq
	        // movl	-32(%rbp,%rax,4), %eax

            string new_op1 = to_x86(op1);
            string dest_location = to_string(stoi(dest)*4);
            o << "    cltq\n";
            o << "    neg %rax\n";
            o << "    movl " + new_op1 + ", -" + dest_location + "(%rbp, %rax, " + to_string(sizeOfType) + ")\n";
        } else if(offsetAppliedTo == 2) {
            // op1 c'est un tableau
            string new_dest = to_x86(dest);
            string op1_location = to_string(stoi(op1)*4);
            o << "    cltq\n";
            o << "    neg %rax\n";
            o << "    movl -" + op1_location + "(%rbp, %rax, " + to_string(sizeOfType) + "), " + new_dest + "\n";
        }
    }
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
