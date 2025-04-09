#ifndef IR_H
#define IR_H

#include <vector>
#include <string>
#include <iostream>

// Declarations from the parser -- replace with your own
#include "tree/ParseTree.h" // Ensure this header file defines tree::ParseTree
#include "VariableVisitor.h"

using namespace std;

class BasicBlock;
class CFG;


//! The class for one 3-address instruction
class IRInstr {
   public:
	/**  constructor */
	IRInstr(BasicBlock* bb_) {
		this->bb = bb_;
	};
	
	/** Actual code generation */
	virtual void gen_asm(ostream &o) = 0; /**< x86 assembly code generation for this IR instruction */

 protected:
	BasicBlock* bb; /**< The BB this instruction belongs to, which provides a pointer to the CFG this instruction belong to */
	// if you subclass IRInstr, each IRInstr subclass has its parameters and the previous (very important) comment becomes useless: it would be a better design. 
};

/**  The class for a basic block */

/* A few important comments.
	 IRInstr has no jump instructions.
	 cmp_* instructions behaves as an arithmetic two-operand instruction (add or mult),
	  returning a boolean value (as an int)

	 Assembly jumps are generated as follows:
	 BasicBlock::gen_asm() first calls IRInstr::gen_asm() on all its instructions, and then 
		    if  exit_true  is a  nullptr, 
           	 the epilogue is generated
        else if exit_false is a nullptr, 
          an unconditional jmp to the exit_true branch is generated
		else (we have two successors, hence a branch)
          an instruction comparing the value of test_var_name to true is generated,
					followed by a conditional branch to the exit_false branch,
					followed by an unconditional branch to the exit_true branch
	 The attribute test_var_name itself is defined when converting 
  the if, while, etc of the AST  to IR.

Possible optimization:
     a cmp_* comparison instructions, if it is the last instruction of its block, 
       generates an actual assembly comparison 
       followed by a conditional jump to the exit_false branch
*/

class BasicBlock {
 public:
	BasicBlock(CFG* cfg, string entry_label, BasicBlock* s, BasicBlock* exit_t, BasicBlock* exit_f, string test_var_name = "") {
		this->cfg = cfg;
		this->exit_true = exit_t;
		this->exit_false = exit_f;
		this->source = s;
		this->label = entry_label;
		this->test_var_name = test_var_name;
		this->instrs = vector<IRInstr*>();
	};
	void gen_asm(ostream &o); /**< x86 assembly code generation for this basic block (very simple) */

	void add_IRInstr(IRInstr* instr) {
		instrs.push_back(instr);
	};

	// No encapsulation whatsoever here. Feel free to do better.
	BasicBlock* source; /**< pointer to the unique predecessor of this basic block */
	BasicBlock* exit_true;  /**< pointer to the next basic block, true branch. If nullptr, return from procedure */ 
	BasicBlock* exit_false; /**< pointer to the next basic block, false branch. If null_ptr, the basic block ends with an unconditional jump */
	string label; /**< label of the BB, also will be the label in the generated code */
	CFG* cfg; /** < the CFG where this block belongs */
	vector<IRInstr*> instrs; /** < the instructions themselves. */
  	string test_var_name;  /** < when generating IR code for an if(expr) or while(expr) etc,
													 store here the name of the variable that holds the value of expr */
 protected:

 
};




/** The class for the control flow graph, also includes the symbol table */

/* A few important comments:
	 The entry block is the one with the same label as the AST function name.
	   (it could be the first of bbs, or it could be defined by an attribute value)
	 The exit block is the one with both exit pointers equal to nullptr.
     (again it could be identified in a more explicit way)

 */
class CFG {
 public:
	CFG() {
		bbs = vector<BasicBlock*>();
	};

	CFG(string entry_label) {
		this->entry_label = entry_label;
		bbs = vector<BasicBlock*>();
		bbs.push_back(new BasicBlock(this, entry_label, nullptr, nullptr, nullptr));
		current_bb = bbs[0];
	};

	string get_entry_label() {
		return this->entry_label;
	};

	// destructor
	~CFG() {
		for (auto bb : bbs) {
			delete bb;
		}
	};

	
	void add_bb(BasicBlock* bb) {
		bbs.push_back(bb);
	}; 

	// getter
	vector<BasicBlock*> get_bbs() {
		return bbs;
	}

	// x86 code generation: could be encapsulated in a processor class in a retargetable compiler
	void gen_asm(ostream& o);
	void gen_asm_prologue(ostream& o);
	void gen_asm_epilogue(ostream& o);

	BasicBlock* current_bb; /**< The current basic block being built */



 protected:
	
	std::string entry_label; /**< label of the entry block */
	vector <BasicBlock*> bbs; /**< all the basic blocks of this CFG*/
};

class IRInstrAffect : public IRInstr {
	public:
	   IRInstrAffect(BasicBlock* bb_, string dest, string op1) : IRInstr(bb_), dest(dest), op1(op1) {};
	   void gen_asm(ostream &o);
	protected:
	   string dest; // partie gauche de l'affectation
	   string op1; // partie droite de l'affectation
};
   
class IRInstrAdd : public IRInstr {
	public:
		IRInstrAdd(BasicBlock* bb_, string dest, string op1, string op2) : IRInstr(bb_), dest(dest), op1(op1), op2(op2) {};
		void gen_asm(ostream &o);
	protected:
		string dest;
		string op1; // opérande à gauche 
		string op2; // opérande à droite
};

class IRInstrSub : public IRInstr {
	public:
		IRInstrSub(BasicBlock* bb_, string dest, string op1, string op2) : IRInstr(bb_), dest(dest), op1(op1), op2(op2) {};
		void gen_asm(ostream &o);
	protected:
		string dest;
		string op1;
		string op2;
};

class IRInstrMul : public IRInstr {
public:
	IRInstrMul(BasicBlock* bb_, string dest, string op1, string op2) : IRInstr(bb_), dest(dest), op1(op1), op2(op2) {};
	void gen_asm(ostream &o);
protected:
	string dest;
	string op1;
	string op2;
};

class IRInstrMod : public IRInstr {
public:
	IRInstrMod(BasicBlock* bb_, string dest, string op1, string op2) : IRInstr(bb_), dest(dest), op1(op1), op2(op2) {};
	void gen_asm(ostream &o);
protected:
	string dest;
	string op1;
	string op2;
};

class IRInstrDiv : public IRInstr {
public:
	IRInstrDiv(BasicBlock* bb_, string dest, string op1, string op2) : IRInstr(bb_), dest(dest), op1(op1), op2(op2) {};
	void gen_asm(ostream &o);
protected:
	string dest;
	string op1;
	string op2;
};

class IRInstrCmpEQ : public IRInstr {
public:
	IRInstrCmpEQ(BasicBlock* bb_, string dest, string op1, string op2) : IRInstr(bb_), dest(dest), op1(op1), op2(op2) {};
	void gen_asm(ostream &o);
protected:
	string dest;
	string op1;
	string op2;
};

class IRInstrCmpNE : public IRInstr {
public:
	IRInstrCmpNE(BasicBlock* bb_, string dest, string op1, string op2) : IRInstr(bb_), dest(dest), op1(op1), op2(op2) {};
	void gen_asm(ostream &o);
protected:
	string dest;
	string op1;
	string op2;
};

class IRInstrCmpLT : public IRInstr {
public:
	IRInstrCmpLT(BasicBlock* bb_, string dest, string op1, string op2) : IRInstr(bb_), dest(dest), op1(op1), op2(op2) {};
	void gen_asm(ostream &o);
protected:
	string dest;
	string op1;
	string op2;
};

class IRInstrCmpGT : public IRInstr {
public:
	IRInstrCmpGT(BasicBlock* bb_, string dest, string op1, string op2) : IRInstr(bb_), dest(dest), op1(op1), op2(op2) {};
	void gen_asm(ostream &o);
protected:
	string dest;
	string op1;
	string op2;
};

class IRInstrAndBit : public IRInstr {
public:
	IRInstrAndBit(BasicBlock* bb_, string dest, string op1, string op2) : IRInstr(bb_), dest(dest), op1(op1), op2(op2) {};
	void gen_asm(ostream &o);
protected:
	string dest;
	string op1;
	string op2;
};

class IRInstrOrBit : public IRInstr {
public:
	IRInstrOrBit(BasicBlock* bb_, string dest, string op1, string op2) : IRInstr(bb_), dest(dest), op1(op1), op2(op2) {};
	void gen_asm(ostream &o);
protected:
	string dest;
	string op1;
	string op2;
};

class IRInstrXorBit : public IRInstr {
public:
	IRInstrXorBit(BasicBlock* bb_, string dest, string op1, string op2) : IRInstr(bb_), dest(dest), op1(op1), op2(op2) {};
	void gen_asm(ostream &o);
protected:
	string dest;
	string op1;
	string op2;
};

class IRInstrNotUnary : public IRInstr {
public:
	IRInstrNotUnary(BasicBlock* bb_, string dest, string op1) : IRInstr(bb_), dest(dest), op1(op1) {};
	void gen_asm(ostream &o);
protected:
	string dest;
	string op1;
};

class IRInstrSubUnary : public IRInstr {
public:
	IRInstrSubUnary(BasicBlock* bb_, string dest, string op1) : IRInstr(bb_), dest(dest), op1(op1) {};
	void gen_asm(ostream &o);
protected:
	string dest;
	string op1;
};

class IRInstrFunc_Call : public IRInstr {
	public:
		IRInstrFunc_Call(BasicBlock* bb_, std::string func_name, std::vector<std::string> args, std::string dest) : IRInstr(bb_), func_name(func_name), args(args), dest(dest){};
		void gen_asm(ostream &o);
	protected:
		std::string func_name;
		std::vector<std::string> args;
		std::string dest;
	
	};


		
		
#endif