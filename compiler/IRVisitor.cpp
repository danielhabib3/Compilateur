#include "IRVisitor.h"

infosVariable getInfosVariable(Block* currentBlock, string varName)
{
    Block* block = currentBlock;
    infosVariable varInfo;
    while(block != nullptr)
    {
        if(block->_variables.find(varName) != block->_variables.end())
        {
            varInfo = block->_variables[varName];
            break;
        }
        block = block->parent;
    }
    return varInfo;
}

antlrcpp::Any IRVisitor::visitBlock(ifccParser::BlockContext *ctx)
{
    if(currentBlock == nullptr) 
    {
        currentBlock = _rootBlock;
    }
    else 
    {
        Block* tempBlock = currentBlock->notVisitedChildren[0];
        currentBlock->notVisitedChildren.erase(currentBlock->notVisitedChildren.begin());
        currentBlock = tempBlock;
    }
    for (auto instruction : ctx->instruction()) {
        this->visit(instruction); // Visiter chaque instruction
    }
    if(currentBlock->parent != nullptr)
    {
        currentBlock = currentBlock->parent;
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitExprAffectation(ifccParser::ExprAffectationContext *ctx)
{
    this->visit(ctx->expr());
    infosVariable infosV = getInfosVariable(currentBlock, ctx->ID()->getText());
    // std::cout << "    movl %eax, -"<<infosV.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosV.location), "0");
    _cfg->current_bb->add_IRInstr(instr);
    return 0;
}

antlrcpp::Any IRVisitor::visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx)
{
    if(ctx->expr() != nullptr) {
        this->visit(ctx->expr());
        infosVariable infosV = getInfosVariable(currentBlock, ctx->ID()->getText());
        // std::cout << "    movl %eax, -"<<infosV.location<<"(%rbp)\n" ;
        IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosV.location), "0");
        _cfg->current_bb->add_IRInstr(instr);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitExprID(ifccParser::ExprIDContext *ctx)
{
    infosVariable infosV = getInfosVariable(currentBlock, ctx->ID()->getText());
    // std::cout << "    movl -"<<infosV.location<<"(%rbp), %eax\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "0", to_string(infosV.location));
    _cfg->current_bb->add_IRInstr(instr);
    return 0;
}

antlrcpp::Any IRVisitor::visitExprConst(ifccParser::ExprConstContext *ctx)
{
    int val = stoi(ctx->CONST()->getText());
    // std::cout << "    movl $"<<val<<", %eax\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "0", "$"+to_string(val));
    _cfg->current_bb->add_IRInstr(instr);
    return 0;
}

antlrcpp::Any IRVisitor::visitExprAddSub(ifccParser::ExprAddSubContext *ctx)
{
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!tempIR" + to_string(current_temp)] = infosGauche;
    current_temp++;

    // std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!tempIR" + to_string(current_temp)] = infosDroite;
    current_temp++;
    
    if(ctx->OP->getText() == "+")
    {
        // std::cout << "    addl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        IRInstr * instrAdd = new IRInstrAdd(_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrAdd);
    }
    else
    {
        // std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        // std::cout << "    subl -"<<infosDroite.location<<"(%rbp), %eax\n" ;
        IRInstr * instrSub = new IRInstrSub(_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrSub);
    }

    return 0;
}

antlrcpp::Any IRVisitor::visitExprMulDivMod(ifccParser::ExprMulDivModContext *ctx)
{
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;

    // std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    if(ctx->OP->getText() == "*")
    {
        // std::cout << "    imull -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        IRInstr * instrMul = new IRInstrMul(_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrMul);
    }
    else if(ctx->OP->getText() == "/")
    {
        // std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        // std::cout << "    cltd\n" ;
        // std::cout << "    idivl -"<<infosDroite.location<<"(%rbp)\n" ;
        IRInstr * instrDiv = new IRInstrDiv(_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrDiv);
    }
    else if(ctx->OP->getText() == "%")
    {
        // std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        // std::cout << "    cltd\n" ;
        // std::cout << "    idivl -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl %edx, %eax\n" ;  
        IRInstr * instrMod = new IRInstrMod(_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
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
        IRInstr * instrSubUnary = new IRInstrSubUnary(_cfg->current_bb, "0", "0");
        _cfg->current_bb->add_IRInstr(instrSubUnary);
    }
    else if(ctx->OP->getText() == "!")
    {
        // std::cout << "    cmpl $0, %eax\n" ;
        // std::cout << "    sete %al\n" ;
        // std::cout << "    movzbl %al, %eax\n" ;
        IRInstr * instrNotUnary = new IRInstrNotUnary(_cfg->current_bb, "0", "0");
        _cfg->current_bb->add_IRInstr(instrNotUnary);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitExprCompSupInf(ifccParser::ExprCompSupInfContext *ctx){

    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    // std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    if(ctx->OP->getText() == ">")
    {
        // std::cout << "    cmpl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
        // std::cout << "    setg %al\n";
        // std::cout << "    movzbl %al, %eax\n";
        IRInstr * instrCmpGT = new IRInstrCmpGT(_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrCmpGT);
    }
    else if(ctx->OP->getText() == "<")
    {
    
        // std::cout << "    cmpl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
        // std::cout << "    setl %al\n";  // Utilisation de setl (set less) pour <
        // std::cout << "    movzbl %al, %eax\n";
        IRInstr * instrCmpLT = new IRInstrCmpLT(_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrCmpLT);  
    }

    return antlrcpp::Any();
}

antlrcpp::Any IRVisitor::visitExprCompEqual(ifccParser::ExprCompEqualContext *ctx){

    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    // std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    
    if(ctx->OP->getText() == "==")
    {
        // std::cout << "    cmpl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
        // std::cout << "    sete %al\n" ;
        // std::cout << "    movzbl %al, %eax\n" ;
        IRInstr * instrCmpEQ = new IRInstrCmpEQ(_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrCmpEQ);
    }
    else
    {
        // std::cout << "    cmpl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
        // std::cout << "    setne %al\n" ;
        // std::cout << "    movzbl %al, %eax\n" ;
        IRInstr * instrCmpNE = new IRInstrCmpNE(_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrCmpNE);
    }

    return antlrcpp::Any();
}

 antlrcpp::Any IRVisitor::visitExprAndBit(ifccParser::ExprAndBitContext *ctx){
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    //std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    //std::cout << "    andl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    IRInstr *instrXor = new IRInstrAndBit( _cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
    _cfg->current_bb->add_IRInstr(instrXor);

    return 0;
 }
 

 antlrcpp::Any IRVisitor::visitExprOrBit(ifccParser::ExprOrBitContext *ctx){
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    //std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    //std::cout << "    orl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    IRInstr *instrXor = new IRInstrOrBit( _cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
    _cfg->current_bb->add_IRInstr(instrXor);

    return 0;
 }

 antlrcpp::Any IRVisitor::visitExprXorBit(ifccParser::ExprXorBitContext *ctx){
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    //std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    //std::cout << "    xorl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    IRInstr *instrXor = new IRInstrXorBit( _cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
    _cfg->current_bb->add_IRInstr(instrXor);

    return 0;
 }

 antlrcpp::Any IRVisitor::visitTest(ifccParser::TestContext *ctx)
 {

    BasicBlock* test_bb = new BasicBlock(_cfg, ".test" + to_string(current_test), nullptr, nullptr, TEST_IF);
    _cfg->current_bb->exit_true = test_bb;
    _cfg->current_bb = test_bb;

    BasicBlock* source_bb = _cfg->current_bb;

    string test_true_bb_label = ".true" + to_string(current_test);
    BasicBlock* test_true_bb = new BasicBlock(_cfg, test_true_bb_label, nullptr, nullptr);

    BasicBlock* test_false_bb;

    string test_endif_bb_label = ".endif" + to_string(current_test);
    BasicBlock* test_endif_bb = new BasicBlock(_cfg, test_endif_bb_label, nullptr, nullptr);
    

    _cfg->current_bb->exit_true = test_true_bb;

    // Quand il y a un if le exit_true et exit_false ne sont pas nullptr
    // si il y a un if sans else le exit_false pointe vers le endif sinon il pointe vers le bloc false
    if(ctx->ELSE() != nullptr)
    {
        string test_false_bb_label = ".false" + to_string(current_test);
        test_false_bb = new BasicBlock(_cfg, test_false_bb_label, nullptr, nullptr);


        _cfg->current_bb->exit_false = test_false_bb;
    }
    else
    {
        _cfg->current_bb->exit_false = test_endif_bb;
    }

    this->visit(ctx->expr());
    infosVariable infos;
    infos.location = this->next_free_location;
    this->next_free_location++;
    string test_var_name = "test" + to_string(current_temp);
    currentBlock->_variables[test_var_name] = infos;
    current_temp++;
    _cfg->current_bb->test_var_name = test_var_name;
    _cfg->current_bb->test_var_location = infos.location;

    //std::cout << "    movl %eax, -"<<infos.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infos.location), "0");
    _cfg->current_bb->add_IRInstr(instr);


    _cfg->add_bb(test_bb);
    _cfg->add_bb(test_true_bb);
    if(ctx->ELSE() != nullptr) {
        _cfg->add_bb(test_false_bb);
    }
    _cfg->add_bb(test_endif_bb);

    current_test++;

    _cfg->current_bb = test_true_bb;
    this->visit(ctx->block(0));
    _cfg->current_bb->exit_true = test_endif_bb;

    if(ctx->ELSE() != nullptr) {
        _cfg->current_bb = test_false_bb;
        this->visit(ctx->block(1));
        _cfg->current_bb->exit_true = test_endif_bb;
    }
    _cfg->current_bb = test_endif_bb;
    source_bb->endif = test_endif_bb;

    
    return 0;
 }



 antlrcpp::Any IRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
 {
    this->visit(ctx->expr());
    // std::cout << "    jmp ." << this->_cfg->current_bb->label << "_out\n";
    IRInstr * instr = new IRInstrReturn(_cfg->current_bb);
    _cfg->current_bb->add_IRInstr(instr);
    return 0;
 }

 antlrcpp::Any IRVisitor::visitBoucle_while(ifccParser::Boucle_whileContext *ctx)
 {
    BasicBlock* test_bb = new BasicBlock(_cfg, ".test" + to_string(current_test), nullptr, nullptr, TEST_WHILE);
    _cfg->stack_boucle_test_block_for_continue.push(test_bb); // On push le basicblock test_bb qui sera la destination en cas de continue trouvé dans le body

    _cfg->current_bb->exit_true = test_bb;

    _cfg->current_bb = test_bb;

    string body_bb_label = ".body" + to_string(current_test);
    BasicBlock* body_bb = new BasicBlock(_cfg, body_bb_label, nullptr, nullptr);

    string test_endwhile_bb_label = ".endwhile" + to_string(current_test);
    BasicBlock* test_endwhile_bb = new BasicBlock(_cfg, test_endwhile_bb_label, nullptr, nullptr);
    _cfg->stack_break_destinations.push(test_endwhile_bb); // On push le basicblock endwhile qui sera la destination en cas de break trouvé dans le body

    _cfg->current_bb->exit_true = body_bb;

    _cfg->current_bb->exit_false = test_endwhile_bb;

    this->visit(ctx->expr());
    infosVariable infos;
    infos.location = this->next_free_location;
    this->next_free_location++;
    string test_var_name = "test" + to_string(current_temp);
    currentBlock->_variables[test_var_name] = infos;
    current_temp++;
    _cfg->current_bb->test_var_name = test_var_name;
    _cfg->current_bb->test_var_location = infos.location;

    //std::cout << "    movl %eax, -"<<infos.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infos.location), "0");
    _cfg->current_bb->add_IRInstr(instr);


    _cfg->add_bb(test_bb);
    _cfg->add_bb(body_bb);
    _cfg->add_bb(test_endwhile_bb);

    current_test++;

    _cfg->current_bb = body_bb;
    this->visit(ctx->block());

    _cfg->stack_break_destinations.pop(); // On pop le basicblock endwhile une fois qu'on a fini le body
    _cfg->stack_boucle_test_block_for_continue.pop(); // On pop le basicblock test une fois qu'on a fini le body

    _cfg->current_bb->exit_true = test_bb;

    _cfg->current_bb = test_endwhile_bb;
    return 0;
 }

 antlrcpp::Any IRVisitor::visitAffectationDeclarationTable(ifccParser::AffectationDeclarationTableContext *ctx)
 {
    infosVariable infosV = getInfosVariable(currentBlock, ctx->ID()->getText());
    int nbAffectation = ctx->expr().size() > infosV.size ? infosV.size : ctx->expr().size();
    for(int i = 0; i < nbAffectation; i++) 
    {
        this->visit(ctx->expr(i));
        
        // std::cout << "    movl %eax, -"<<infosV.location<<"(%rbp)\n" ;
        IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosV.location + i), "0");
        _cfg->current_bb->add_IRInstr(instr);
    }
    if(ctx->expr().size() < infosV.size)
    {
        for(int i = ctx->expr().size(); i < infosV.size; i++)
        {
            // std::cout << "    movl $0, -"<<infosV.location<<"(%rbp)\n" ;
            IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosV.location + i), "$0");
            _cfg->current_bb->add_IRInstr(instr);
        }
    }
    return 0;
 }

 antlrcpp::Any IRVisitor::visitExprTable(ifccParser::ExprTableContext *ctx)
 {
    infosVariable infosV = getInfosVariable(currentBlock, ctx->ID()->getText());
    // std::cout << "    movl -"<<infosV.location<<"(%rbp), %eax\n" ;

    this->visit(ctx->expr());
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, "0", to_string(infosV.location), 4, 2);
    _cfg->current_bb->add_IRInstr(instr);
    return 0;
 }

 antlrcpp::Any IRVisitor::visitExprAffectationTable(ifccParser::ExprAffectationTableContext *ctx)
 {
    infosVariable infosV = getInfosVariable(currentBlock, ctx->ID()->getText());

    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;


    IRInstr * instr1 = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr1);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    IRInstr * instr2 = new IRInstrAffect(_cfg->current_bb, to_string(infosDroite.location), "0");
    _cfg->current_bb->add_IRInstr(instr2);

    IRInstr * instr3 = new IRInstrAffect(_cfg->current_bb, "0", to_string(infosGauche.location));
    _cfg->current_bb->add_IRInstr(instr3);

    IRInstr * instr4 = new IRInstrAffect(_cfg->current_bb, "-1", to_string(infosDroite.location));
    _cfg->current_bb->add_IRInstr(instr4);

    IRInstr * instr5 = new IRInstrAffect(_cfg->current_bb, to_string(infosV.location), "-1", 4, 1);
    _cfg->current_bb->add_IRInstr(instr5);

    return 0;
 }


// antlrcpp::Any IRVisitor::visitExprPostfixIncDec(ifccParser::ExprPostfixIncDecContext *ctx){

//     string id = ctx->ID()->getText();
//     infosVariable infosV = _variables[id];

//     return 0;
// }

// antlrcpp::Any IRVisitor::visitExprPrefixIncDec(ifccParser::ExprPrefixIncDecContext *ctx){
 
//     string id = ctx->ID()->getText();
//     infosVariable infosV = _variables[id];

//     if(ctx->OP->getText() == "++")
//     {
//         IRInstr* IRInstrPreInc = new IRInstrPreInc(_cfg->current_bb, "0", to_string(infosV.location));
//         _cfg->current_bb->add_IRInstr(IRInstrPreInc);
//     }
//     else
//     {
//         IRInstr * IRInstrPreDec = new IRInstrPreDec(_cfg->current_bb, "0", to_string(infosV.location));
//         _cfg->current_bb->add_IRInstr(IRInstrPreDec);
//     }

//     return 0;
// }

antlrcpp::Any IRVisitor::visitExprAffectationComposee(ifccParser::ExprAffectationComposeeContext *ctx)
{
    string id = ctx->ID()->getText();
    infosVariable infosV = getInfosVariable(currentBlock, id);

    // On Charge la valeur actuelle de la variable dans %eax
    IRInstr *instrLoad = new IRInstrAffect(_cfg->current_bb, "0", to_string(infosV.location));
    _cfg->current_bb->add_IRInstr(instrLoad);

    // On Stocke la valeur actuelle temporairement
    infosVariable infosGauche;
    infosGauche.location = this->next_free_location++;
    string tempVarGauche = "!temp" + to_string(current_temp++);
    currentBlock->_variables[tempVarGauche] = infosGauche;

    IRInstr *instrStoreLeft = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instrStoreLeft);

    // On visite la nouvelle expression qui sera à ajotuer à la valeur actuelle (la partie droite de += ou -=)
    this->visit(ctx->expr());

    // On Stocke cette nouvelle valeur dans une autre temporaire
    infosVariable infosDroite;
    infosDroite.location = this->next_free_location++;
    string tempVarDroite = "!temp" + to_string(current_temp++);
    currentBlock->_variables[tempVarDroite] = infosDroite;

    IRInstr *instrStoreRight = new IRInstrAffect(_cfg->current_bb, to_string(infosDroite.location), "0");
    _cfg->current_bb->add_IRInstr(instrStoreRight);

    // Effectuer l'opération += ou -=
    if(ctx->OP->getText() == "+=")
    {
        IRInstr *instrAdd = new IRInstrAdd(_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrAdd);
    }
    else
    {
        IRInstr *instrSub = new IRInstrSub(_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        _cfg->current_bb->add_IRInstr(instrSub);
    }

    // Affecter le résultat à la variable d'origine
    IRInstr *instrAffect = new IRInstrAffect(_cfg->current_bb, to_string(infosV.location), "0");
    _cfg->current_bb->add_IRInstr(instrAffect);

    return 0;
}
