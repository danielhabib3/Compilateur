#include "IRVisitor.h"

antlrcpp::Any IRVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{
    this->visit(ctx->expr());
    infosVariable infosV = _variables[ctx->ID()->getText()];
    // std::cout << "    movl %eax, -"<<infosV.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosV.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);
    return 0;
}

antlrcpp::Any IRVisitor::visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx)
{
    if(ctx->expr() != nullptr) {
        this->visit(ctx->expr());
        infosVariable infosV = _variables[ctx->ID()->getText()];
        // std::cout << "    movl %eax, -"<<infosV.location<<"(%rbp)\n" ;
        IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosV.location)+"(%rbp)", "%eax");
        _cfg->current_bb->add_IRInstr(instr);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitExprID(ifccParser::ExprIDContext *ctx)
{
    infosVariable infosV = _variables[ctx->ID()->getText()];
    // std::cout << "    movl -"<<infosV.location<<"(%rbp), %eax\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "%eax", "-"+to_string(infosV.location)+"(%rbp)");
    _cfg->current_bb->add_IRInstr(instr);
    return 0;
}

antlrcpp::Any IRVisitor::visitExprConst(ifccParser::ExprConstContext *ctx)
{
    int val = stoi(ctx->CONST()->getText());
    // std::cout << "    movl $"<<val<<", %eax\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "%eax", "$"+to_string(val));
    _cfg->current_bb->add_IRInstr(instr);
    return 0;
}

antlrcpp::Any IRVisitor::visitExprAddSub(ifccParser::ExprAddSubContext *ctx)
{
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!tempIR" + to_string(current_temp)] = infosGauche;
    current_temp++;

    // std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1)*4;
    _variables["!tempIR" + to_string(current_temp)] = infosDroite;
    current_temp++;
    
    if(ctx->OP->getText() == "+")
    {
        // std::cout << "    addl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        IRInstr * instrAdd = new IRInstrAdd(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrAdd);
    }
    else
    {
        // std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        // std::cout << "    subl -"<<infosDroite.location<<"(%rbp), %eax\n" ;
        IRInstr * instrSub = new IRInstrSub(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", "-"+to_string(infosDroite.location)+"(%rbp)");
        _cfg->current_bb->add_IRInstr(instrSub);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprMulDivMod(ifccParser::ExprMulDivModContext *ctx)
{
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;

    // std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    if(ctx->OP->getText() == "*")
    {
        // std::cout << "    imull -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        IRInstr * instrMul = new IRInstrMul(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrMul);
    }
    else if(ctx->OP->getText() == "/")
    {
        // std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        // std::cout << "    cltd\n" ;
        // std::cout << "    idivl -"<<infosDroite.location<<"(%rbp)\n" ;
        IRInstr * instrDiv = new IRInstrDiv(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", "-"+to_string(infosDroite.location)+"(%rbp)");
        _cfg->current_bb->add_IRInstr(instrDiv);
    }
    else if(ctx->OP->getText() == "%")
    {
        // std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        // std::cout << "    cltd\n" ;
        // std::cout << "    idivl -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl %edx, %eax\n" ;  
        IRInstr * instrMod = new IRInstrMod(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", "-"+to_string(infosDroite.location)+"(%rbp)");
        _cfg->current_bb->add_IRInstr(instrMod);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitExprUnary(ifccParser::ExprUnaryContext *ctx)
{
    this->visit(ctx->expr());
    if(ctx->OP->getText() == "-") 
    {
        // std::cout << "    negl %eax\n" ;
        IRInstr * instrSubUnary = new IRInstrSubUnary(_cfg->current_bb, "%eax", "%eax");
        _cfg->current_bb->add_IRInstr(instrSubUnary);
    }
    else if(ctx->OP->getText() == "!")
    {
        // std::cout << "    cmpl $0, %eax\n" ;
        // std::cout << "    sete %al\n" ;
        // std::cout << "    movzbl %al, %eax\n" ;
        IRInstr * instrNotUnary = new IRInstrNotUnary(_cfg->current_bb, "%eax", "%eax");
        _cfg->current_bb->add_IRInstr(instrNotUnary);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitExprCompSupInf(ifccParser::ExprCompSupInfContext *ctx){

    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    // std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    if(ctx->OP->getText() == ">")
    {
        // std::cout << "    cmpl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
        // std::cout << "    setg %al\n";
        // std::cout << "    movzbl %al, %eax\n";
        IRInstr * instrCmpGT = new IRInstrCmpGT(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
        _cfg->current_bb->add_IRInstr(instrCmpGT);
    }
    else if(ctx->OP->getText() == "<")
    {
    
        // std::cout << "    cmpl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
        // std::cout << "    setl %al\n";  // Utilisation de setl (set less) pour <
        // std::cout << "    movzbl %al, %eax\n";
        IRInstr * instrCmpLT = new IRInstrCmpLT(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
        _cfg->current_bb->add_IRInstr(instrCmpLT);  
    }

    return antlrcpp::Any();
}

antlrcpp::Any IRVisitor::visitExprCompEqual(ifccParser::ExprCompEqualContext *ctx){

    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    // std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    
    if(ctx->OP->getText() == "==")
    {
        // std::cout << "    cmpl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
        // std::cout << "    sete %al\n" ;
        // std::cout << "    movzbl %al, %eax\n" ;
        IRInstr * instrCmpEQ = new IRInstrCmpEQ(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
        _cfg->current_bb->add_IRInstr(instrCmpEQ);
    }
    else
    {
        // std::cout << "    cmpl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
        // std::cout << "    setne %al\n" ;
        // std::cout << "    movzbl %al, %eax\n" ;
        IRInstr * instrCmpNE = new IRInstrCmpNE(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
        _cfg->current_bb->add_IRInstr(instrCmpNE);
    }

    return antlrcpp::Any();
}

 antlrcpp::Any IRVisitor::visitExprAndBit(ifccParser::ExprAndBitContext *ctx){
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    //std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    //std::cout << "    andl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    IRInstr *instrXor = new IRInstrAndBit( _cfg->current_bb, "%eax", "-" + to_string(infosGauche.location) + "(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instrXor);

    return 0;
 }
 

 antlrcpp::Any IRVisitor::visitExprOrBit(ifccParser::ExprOrBitContext *ctx){
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    //std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    //std::cout << "    orl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    IRInstr *instrXor = new IRInstrOrBit( _cfg->current_bb, "%eax", "-" + to_string(infosGauche.location) + "(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instrXor);

    return 0;
 }

 antlrcpp::Any IRVisitor::visitExprXorBit(ifccParser::ExprXorBitContext *ctx){
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    //std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    //std::cout << "    xorl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    IRInstr *instrXor = new IRInstrXorBit( _cfg->current_bb, "%eax", "-" + to_string(infosGauche.location) + "(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instrXor);

    return 0;
 }
 
 antlrcpp::Any IRVisitor::visitFunction_call(ifccParser::Function_callContext *ctx) {
    string func_name = ctx->ID()->getText();
    vector<string> argVars;

    for (size_t i = 0; i < ctx->expr().size(); ++i) {
        std::any res = this->visit(ctx->expr(i));
        string argName = std::any_cast<string>(res);
        argVars.push_back(argName);
    }

    string tempVar = "!temp" + to_string(current_temp++);
    infosVariable infos;
    infos.location = (_variables.size() + 1) * 4;
    _variables[tempVar] = infos;

    IRInstr *instr = new IRInstrFunc_Call(_cfg->current_bb, func_name, argVars, tempVar);
    _cfg->current_bb->add_IRInstr(instr);

    return tempVar;
}



antlrcpp::Any IRVisitor::visitFunction_definition(ifccParser::Function_definitionContext *ctx) {
    string funcName = ctx->ID(0)->getText(); 

    map<string, infosVariable> previous_variables = _variables;
    _cfg = new CFG(funcName);
    _variables.clear();

    BasicBlock* entryBB = new BasicBlock(_cfg, "entry");
    _cfg->current_bb = entryBB;
    _cfg->add_basic_block(entryBB);

    _cfg->current_bb->add_IRInstr(new IRInstrFunc_Def(_cfg->current_bb, funcName));

    if (ctx->block()) {
        visit(ctx->block());
    }

    return 0;
}
