#include "IRVisitor.h"

antlrcpp::Any IRVisitor::visitExprAddSub(ifccParser::ExprAddSubContext *ctx)
{
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!tempIR" + to_string(current_temp)] = infosGauche;
    current_temp++;

    // std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    Instr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1)*4;
    _variables["!tempIR" + to_string(current_temp)] = infosDroite;
    current_temp++;
    
    if(ctx->OP->getText() == "+")
    {
        // std::cout << "    addl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        Instr * instrAdd = new IRInstrAdd(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrAdd);
    }
    else
    {
        // std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        // std::cout << "    subl -"<<infosDroite.location<<"(%rbp), %eax\n" ;
        Instr * instrSub = new IRInstrSub(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", "-"+to_string(infosDroite.location)+"(%rbp)");
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
    Instr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    if(ctx->OP->getText() == "*")
    {
        // std::cout << "    imull -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        Instr * instrMul = new IRInstrMul(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrMul);
    }
    else if(ctx->OP->getText() == "/")
    {
        // std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        // std::cout << "    cltd\n" ;
        // std::cout << "    idivl -"<<infosDroite.location<<"(%rbp)\n" ;
        Instr * instrDiv = new IRInstrDiv(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", "-"+to_string(infosDroite.location)+"(%rbp)");
        _cfg->current_bb->add_IRInstr(instrDiv);
    }
    else if(ctx->OP->getText() == "%")
    {
        // std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        // std::cout << "    cltd\n" ;
        // std::cout << "    idivl -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl %edx, %eax\n" ;  
        Instr * instrMod = new IRInstrMod(_cfg->current_bb, "%eax", "-"+to_string(infosGauche.location)+"(%rbp)", "-"+to_string(infosDroite.location)+"(%rbp)");
        _cfg->current_bb->add_IRInstr(instrMod);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitExprUnary(ifccParser::ExprUnaryContext *ctx)
{
    this->visit(ctx->expr());
    if(ctx->OP->getText() == "-")
        std::cout << "    negl %eax\n" ;
    else if(ctx->OP->getText() == "!")
    {
        std::cout << "    cmpl $0, %eax\n" ;
        std::cout << "    sete %al\n" ;
        std::cout << "    movzbl %al, %eax\n" ;
    }
    return 0;
}

antlrcpp::Any visitExprCompSupInf(ifccParser::ExprCompSupInfContext *ctx){

    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;

    Instr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    BasicBlock* bb = _cfg->current_bb ; 
    string dest = "%eax";
    string op1 = "-"+to_string(infosGauche.location)+"(%rbp)"  ;
    string op2 = "-"+to_string(infosDroite.location)+"(%rbp)" ;

    if(ctx->OP->getText() == ">")
    { 
        Instr * instrCmpGT = new IRInstrCmpGT(bb,dest,op1,op2);
        _cfg->current_bb->add_IRInstr(instrCmpGT);
    }
    else if(ctx->OP->getText() == "<")
    {
    
        Instr * instrCmpLT = new IRInstrCmpGT(bb,dest,op1,op2);
        _cfg->current_bb->add_IRInstr(instrCmpLT);       
    }

    return antlrcpp::Any();
}

antlrcpp::Any visitExprCompEqual(ifccParser::ExprCompEqualContext *ctx){

    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;

    Instr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    BasicBlock* bb = _cfg->current_bb ; 
    string dest = "%eax";
    string op1 = "-"+to_string(infosGauche.location)+"(%rbp)"  ;
    string op2 = "-"+to_string(infosDroite.location)+"(%rbp)" ;


    if(ctx->OP->getText() == "=="){
        Instr * instrCmpEqual = new IRInstrCmpEQ(bb,dest,op1,op2);
        _cfg->current_bb->add_IRInstr(instrCmpEqual);
    }
    else{
        Instr * instrCmpNEqual = new IRInstrCmpNE(bb,dest,op1,op2);
        _cfg->current_bb->add_IRInstr(instrCmpNEqual);
    }
  
    return antlrcpp::Any();
}

 antlrcpp::Any visitExprAndBit(ifccParser::ExprAndBitContext *ctx){
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    //std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    Instr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    //std::cout << "    andl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    Instr *instrXor = new IRInstrAndBit( _cfg->current_bb, "%eax", "-" + to_string(infosGauche.location) + "(%rbp)", "-" + to_string(infosDroite.location) + "(%rbp)");
    _cfg->current_bb->add_IRInstr(instrXor);

    return 0;
 }
 

 antlrcpp::Any visitExprOrBit(ifccParser::ExprOrBitContext *ctx){
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    //std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    Instr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    //std::cout << "    orl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    Instr *instrXor = new IRInstrOrBit( _cfg->current_bb, "%eax", "-" + to_string(infosGauche.location) + "(%rbp)", "-" + to_string(infosDroite.location) + "(%rbp)");
    _cfg->current_bb->add_IRInstr(instrXor);

    return 0;
 }

 antlrcpp::Any visitExprXorBit(ifccParser::ExprXorBitContext *ctx){
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    //std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    Instr * instr = new IRInstrAffect(_cfg->current_bb, "-"+to_string(infosGauche.location)+"(%rbp)", "%eax");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    //std::cout << "    xorl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    Instr *instrXor = new IRInstrXorBit( _cfg->current_bb, "%eax", "-" + to_string(infosGauche.location) + "(%rbp)", "-" + to_string(infosDroite.location) + "(%rbp)");
    _cfg->current_bb->add_IRInstr(instrXor);

    return 0;
 }