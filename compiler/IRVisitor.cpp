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
    // le cas ou on visit le block d'une définition de fonction
    if(currentBlock == nullptr)
        return (ifccParser::BlockContext *) ctx;

    // si c'est block normal
    Block* tempBlock = currentBlock->notVisitedChildren[0];
    currentBlock->notVisitedChildren.erase(currentBlock->notVisitedChildren.begin());
    currentBlock = tempBlock;
    

    for (int i = 0; i < ctx->instruction().size(); i++) {
        this->visit(ctx->instruction(i)); // Visiter chaque instruction
        // si break on break pour ne pas generer les instruction du bb
        if(ctx->instruction(i)->break_() != nullptr) {
            break;
        }
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
    IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, to_string(infosV.location), "0");
    current_cfg->current_bb->add_IRInstr(instr);
    return 0;
}

antlrcpp::Any IRVisitor::visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx)
{
    if(ctx->expr() != nullptr) {
        this->visit(ctx->expr());
        infosVariable infosV = getInfosVariable(currentBlock, ctx->ID()->getText());
        // std::cout << "    movl %eax, -"<<infosV.location<<"(%rbp)\n" ;
        IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, to_string(infosV.location), "0");
        current_cfg->current_bb->add_IRInstr(instr);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitExprID(ifccParser::ExprIDContext *ctx)
{
    infosVariable infosV = getInfosVariable(currentBlock, ctx->ID()->getText());
    // std::cout << "    movl -"<<infosV.location<<"(%rbp), %eax\n" ;
    IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, "0", to_string(infosV.location));
    current_cfg->current_bb->add_IRInstr(instr);
    return 0;
}

antlrcpp::Any IRVisitor::visitExprChar(ifccParser::ExprCharContext *ctx)
{
    string raw = ctx->CHAR()->getText(); // e.g., "'x'"
    char c = raw[1]; // assumes format is valid, like 'x'
    int asciiValue = static_cast<int>(c);

    IRInstr* instr = new IRInstrAffect(current_cfg->current_bb, "0", "$" + to_string(asciiValue));
    current_cfg->current_bb->add_IRInstr(instr);

    return 0;
}


antlrcpp::Any IRVisitor::visitExprConst(ifccParser::ExprConstContext *ctx)
{
    int val = stoi(ctx->CONST()->getText());
    // std::cout << "    movl $"<<val<<", %eax\n" ;
    IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, "0", "$"+to_string(val));
    current_cfg->current_bb->add_IRInstr(instr);
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
    IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, to_string(infosGauche.location), "0");
    current_cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!tempIR" + to_string(current_temp)] = infosDroite;
    current_temp++;
    
    if(ctx->OP->getText() == "+")
    {
        // std::cout << "    addl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        IRInstr * instrAdd = new IRInstrAdd(current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        current_cfg->current_bb->add_IRInstr(instrAdd);
    }
    else
    {
        // std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        // std::cout << "    subl -"<<infosDroite.location<<"(%rbp), %eax\n" ;
        IRInstr * instrSub = new IRInstrSub(current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        current_cfg->current_bb->add_IRInstr(instrSub);
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
    IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, to_string(infosGauche.location), "0");
    current_cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    if(ctx->OP->getText() == "*")
    {
        // std::cout << "    imull -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        IRInstr * instrMul = new IRInstrMul(current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        current_cfg->current_bb->add_IRInstr(instrMul);
    }
    else if(ctx->OP->getText() == "/")
    {
        // std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        // std::cout << "    cltd\n" ;
        // std::cout << "    idivl -"<<infosDroite.location<<"(%rbp)\n" ;
        IRInstr * instrDiv = new IRInstrDiv(current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        current_cfg->current_bb->add_IRInstr(instrDiv);
    }
    else if(ctx->OP->getText() == "%")
    {
        // std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        // std::cout << "    cltd\n" ;
        // std::cout << "    idivl -"<<infosDroite.location<<"(%rbp)\n" ;
        // std::cout << "    movl %edx, %eax\n" ;  
        IRInstr * instrMod = new IRInstrMod(current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        current_cfg->current_bb->add_IRInstr(instrMod);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitExprUnary(ifccParser::ExprUnaryContext *ctx)
{
    this->visit(ctx->expr());
    if(ctx->OP->getText() == "-") 
    {
        // std::cout << "    negl %eax\n" ;
        IRInstr * instrSubUnary = new IRInstrSubUnary(current_cfg->current_bb, "0", "0");
        current_cfg->current_bb->add_IRInstr(instrSubUnary);
    }
    else if(ctx->OP->getText() == "!")
    {
        // std::cout << "    cmpl $0, %eax\n" ;
        // std::cout << "    sete %al\n" ;
        // std::cout << "    movzbl %al, %eax\n" ;
        IRInstr * instrNotUnary = new IRInstrNotUnary(current_cfg->current_bb, "0", "0");
        current_cfg->current_bb->add_IRInstr(instrNotUnary);
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
    IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, to_string(infosGauche.location), "0");
    current_cfg->current_bb->add_IRInstr(instr);

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
        IRInstr * instrCmpGT = new IRInstrCmpGT(current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        current_cfg->current_bb->add_IRInstr(instrCmpGT);
    }
    else if(ctx->OP->getText() == "<")
    {
    
        // std::cout << "    cmpl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
        // std::cout << "    setl %al\n";  // Utilisation de setl (set less) pour <
        // std::cout << "    movzbl %al, %eax\n";
        IRInstr * instrCmpLT = new IRInstrCmpLT(current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        current_cfg->current_bb->add_IRInstr(instrCmpLT);  
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
    IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, to_string(infosGauche.location), "0");
    current_cfg->current_bb->add_IRInstr(instr);

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
        IRInstr * instrCmpEQ = new IRInstrCmpEQ(current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        current_cfg->current_bb->add_IRInstr(instrCmpEQ);
    }
    else
    {
        // std::cout << "    cmpl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
        // std::cout << "    setne %al\n" ;
        // std::cout << "    movzbl %al, %eax\n" ;
        IRInstr * instrCmpNE = new IRInstrCmpNE(current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        current_cfg->current_bb->add_IRInstr(instrCmpNE);
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
    IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, to_string(infosGauche.location), "0");
    current_cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    //std::cout << "    andl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    IRInstr *instrXor = new IRInstrAndBit( current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
    current_cfg->current_bb->add_IRInstr(instrXor);

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
    IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, to_string(infosGauche.location), "0");
    current_cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    //std::cout << "    orl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    IRInstr *instrXor = new IRInstrOrBit( current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
    current_cfg->current_bb->add_IRInstr(instrXor);

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
    IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, to_string(infosGauche.location), "0");
    current_cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    //std::cout << "    xorl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    IRInstr *instrXor = new IRInstrXorBit( current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
    current_cfg->current_bb->add_IRInstr(instrXor);

    return 0;
 }

 antlrcpp::Any IRVisitor::visitTest(ifccParser::TestContext *ctx)
 {

    BasicBlock* test_bb = new BasicBlock(current_cfg, ".test" + to_string(current_test), nullptr, nullptr, TEST_IF);
    current_cfg->current_bb->exit_true = test_bb;
    current_cfg->current_bb = test_bb;

    BasicBlock* source_bb = current_cfg->current_bb;

    string test_true_bb_label = ".true" + to_string(current_test);
    BasicBlock* test_true_bb = new BasicBlock(current_cfg, test_true_bb_label, nullptr, nullptr);

    BasicBlock* test_false_bb;

    string test_endif_bb_label = ".endif" + to_string(current_test);
    BasicBlock* test_endif_bb = new BasicBlock(current_cfg, test_endif_bb_label, nullptr, nullptr);
    

    current_cfg->current_bb->exit_true = test_true_bb;

    // Quand il y a un if le exit_true et exit_false ne sont pas nullptr
    // si il y a un if sans else le exit_false pointe vers le endif sinon il pointe vers le bloc false
    if(ctx->ELSE() != nullptr)
    {
        string test_false_bb_label = ".false" + to_string(current_test);
        test_false_bb = new BasicBlock(current_cfg, test_false_bb_label, nullptr, nullptr);


        current_cfg->current_bb->exit_false = test_false_bb;
    }
    else
    {
        current_cfg->current_bb->exit_false = test_endif_bb;
    }

    this->visit(ctx->expr());
    infosVariable infos;
    infos.location = this->next_free_location;
    this->next_free_location++;
    string test_var_name = "test" + to_string(current_temp);
    currentBlock->_variables[test_var_name] = infos;
    current_temp++;
    current_cfg->current_bb->test_var_name = test_var_name;
    current_cfg->current_bb->test_var_location = infos.location;

    //std::cout << "    movl %eax, -"<<infos.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, to_string(infos.location), "0");
    current_cfg->current_bb->add_IRInstr(instr);


    current_cfg->add_bb(test_bb);
    current_cfg->add_bb(test_true_bb);
    if(ctx->ELSE() != nullptr) {
        current_cfg->add_bb(test_false_bb);
    }
    current_cfg->add_bb(test_endif_bb);

    current_test++;

    current_cfg->current_bb = test_true_bb;
    this->visit(ctx->block(0));
    current_cfg->current_bb->exit_true = test_endif_bb;

    if(ctx->ELSE() != nullptr) {
        current_cfg->current_bb = test_false_bb;
        this->visit(ctx->block(1));
        current_cfg->current_bb->exit_true = test_endif_bb;
    }
    current_cfg->current_bb = test_endif_bb;
    source_bb->endif = test_endif_bb;

    
    return 0;
 }



 antlrcpp::Any IRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
 {
    this->visit(ctx->expr());
    // std::cout << "    jmp ." << this->current_cfg->current_bb->label << "_out\n";
    IRInstr * instr = new IRInstrReturn(current_cfg->current_bb);
    current_cfg->current_bb->add_IRInstr(instr);
    return 0;
 }

 antlrcpp::Any IRVisitor::visitBoucle_while(ifccParser::Boucle_whileContext *ctx)
 {
    BasicBlock* test_bb = new BasicBlock(current_cfg, ".test" + to_string(current_test), nullptr, nullptr, TEST_WHILE);
    current_cfg->stack_boucle_test_block_for_continue.push(test_bb); // On push le basicblock test_bb qui sera la destination en cas de continue trouvé dans le body

    current_cfg->current_bb->exit_true = test_bb;

    current_cfg->current_bb = test_bb;

    string body_bb_label = ".body" + to_string(current_test);
    BasicBlock* body_bb = new BasicBlock(current_cfg, body_bb_label, nullptr, nullptr);

    string test_endwhile_bb_label = ".endwhile" + to_string(current_test);
    BasicBlock* test_endwhile_bb = new BasicBlock(current_cfg, test_endwhile_bb_label, nullptr, nullptr);
    current_cfg->stack_break_destinations.push(test_endwhile_bb); // On push le basicblock endwhile qui sera la destination en cas de break trouvé dans le body

    current_cfg->current_bb->exit_true = body_bb;

    current_cfg->current_bb->exit_false = test_endwhile_bb;

    this->visit(ctx->expr());
    infosVariable infos;
    infos.location = this->next_free_location;
    this->next_free_location++;
    string test_var_name = "test" + to_string(current_temp);
    currentBlock->_variables[test_var_name] = infos;
    current_temp++;
    current_cfg->current_bb->test_var_name = test_var_name;
    current_cfg->current_bb->test_var_location = infos.location;

    //std::cout << "    movl %eax, -"<<infos.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, to_string(infos.location), "0");
    current_cfg->current_bb->add_IRInstr(instr);


    current_cfg->add_bb(test_bb);
    current_cfg->add_bb(body_bb);
    current_cfg->add_bb(test_endwhile_bb);

    current_test++;

    current_cfg->current_bb = body_bb;
    this->visit(ctx->block());

    current_cfg->stack_break_destinations.pop(); // On pop le basicblock endwhile une fois qu'on a fini le body
    current_cfg->stack_boucle_test_block_for_continue.pop(); // On pop le basicblock test une fois qu'on a fini le body

    current_cfg->current_bb->exit_true = test_bb;

    current_cfg->current_bb = test_endwhile_bb;
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
        IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, to_string(infosV.location + i), "0");
        current_cfg->current_bb->add_IRInstr(instr);
    }
    if(ctx->expr().size() < infosV.size)
    {
        for(int i = ctx->expr().size(); i < infosV.size; i++)
        {
            // std::cout << "    movl $0, -"<<infosV.location<<"(%rbp)\n" ;
            IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, to_string(infosV.location + i), "$0");
            current_cfg->current_bb->add_IRInstr(instr);
        }
    }
    return 0;
 }

 antlrcpp::Any IRVisitor::visitExprTable(ifccParser::ExprTableContext *ctx)
 {
    infosVariable infosV = getInfosVariable(currentBlock, ctx->ID()->getText());
    // std::cout << "    movl -"<<infosV.location<<"(%rbp), %eax\n" ;

    this->visit(ctx->expr());
    IRInstr * instr = new IRInstrAffect(current_cfg->current_bb, "0", to_string(infosV.location), 4, 2);
    current_cfg->current_bb->add_IRInstr(instr);
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


    IRInstr * instr1 = new IRInstrAffect(current_cfg->current_bb, to_string(infosGauche.location), "0");
    current_cfg->current_bb->add_IRInstr(instr1);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = this->next_free_location;
    this->next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    IRInstr * instr2 = new IRInstrAffect(current_cfg->current_bb, to_string(infosDroite.location), "0");
    current_cfg->current_bb->add_IRInstr(instr2);

    IRInstr * instr3 = new IRInstrAffect(current_cfg->current_bb, "0", to_string(infosGauche.location));
    current_cfg->current_bb->add_IRInstr(instr3);

    IRInstr * instr4 = new IRInstrAffect(current_cfg->current_bb, "-1", to_string(infosDroite.location));
    current_cfg->current_bb->add_IRInstr(instr4);

    IRInstr * instr5 = new IRInstrAffect(current_cfg->current_bb, to_string(infosV.location), "-1", 4, 1);
    current_cfg->current_bb->add_IRInstr(instr5);

    return 0;
 }


 antlrcpp::Any IRVisitor::visitExprPostfixIncDec(ifccParser::ExprPostfixIncDecContext *ctx)
 {
     string id = ctx->ID()->getText();
     infosVariable infosV = getInfosVariable(currentBlock, id);
 
     // On met dans eax
     IRInstr *instrLoad = new IRInstrAffect(_cfg->current_bb, "0", to_string(infosV.location));
     _cfg->current_bb->add_IRInstr(instrLoad);

    // Sauvegarde la valeur actuelle dans un temp car postfix
    infosVariable tempVar;
    tempVar.location = next_free_location++;
    currentBlock->_variables["!temp" + to_string(current_temp++)] = tempVar;

    IRInstr *instrSave = new IRInstrAffect(_cfg->current_bb, to_string(tempVar.location), "0");
    _cfg->current_bb->add_IRInstr(instrSave);
 

    string valeur;
    string op = ctx->OP->getText();
    if (op == "++") {
    valeur = "1";
    } else {
        valeur = "-1";
    }
     
     // Effectuer inc / dec
     IRInstr *instrAdd = new IRInstrAdd(_cfg->current_bb, to_string(infosV.location), "$" + valeur,  "0");
     _cfg->current_bb->add_IRInstr(instrAdd);
 
     // Restaure ancienne valeur car postfix
     IRInstr *instrRestore = new IRInstrAffect(_cfg->current_bb, "0", to_string(tempVar.location));
     _cfg->current_bb->add_IRInstr(instrRestore);
 
     return 0;
 }


antlrcpp::Any IRVisitor::visitExprPrefixIncDec(ifccParser::ExprPrefixIncDecContext *ctx)
{
    string id = ctx->ID()->getText();
    infosVariable infosV = getInfosVariable(currentBlock, id);

    IRInstr *instrLoad = new IRInstrAffect(_cfg->current_bb, "0", to_string(infosV.location));
    _cfg->current_bb->add_IRInstr(instrLoad);

    string op = ctx->OP->getText();

    string valeur;
    if (op == "++") {
        valeur = "1";
    } else {
        valeur = "-1";
    }

    IRInstr *instrAdd = new IRInstrAdd(_cfg->current_bb, "0", "$" + valeur,"0");
    _cfg->current_bb->add_IRInstr(instrAdd);

    IRInstr *instrStore = new IRInstrAffect(_cfg->current_bb, to_string(infosV.location), "0");
    _cfg->current_bb->add_IRInstr(instrStore);

    return 0;
}

antlrcpp::Any IRVisitor::visitExprAffectationComposee(ifccParser::ExprAffectationComposeeContext *ctx)
{
    string id = ctx->ID()->getText();
    infosVariable infosV = getInfosVariable(currentBlock, id);

    // On Charge la valeur actuelle de la variable dans %eax
    IRInstr *instrLoad = new IRInstrAffect(current_cfg->current_bb, "0", to_string(infosV.location));
    current_cfg->current_bb->add_IRInstr(instrLoad);

    // On Stocke la valeur actuelle temporairement
    infosVariable infosGauche;
    infosGauche.location = this->next_free_location++;
    string tempVarGauche = "!temp" + to_string(current_temp++);
    currentBlock->_variables[tempVarGauche] = infosGauche;

    IRInstr *instrStoreLeft = new IRInstrAffect(current_cfg->current_bb, to_string(infosGauche.location), "0");
    current_cfg->current_bb->add_IRInstr(instrStoreLeft);

    // On visite la nouvelle expression qui sera à ajotuer à la valeur actuelle (la partie droite de += ou -=)
    this->visit(ctx->expr());

    // On Stocke cette nouvelle valeur dans une autre temporaire
    infosVariable infosDroite;
    infosDroite.location = this->next_free_location++;
    string tempVarDroite = "!temp" + to_string(current_temp++);
    currentBlock->_variables[tempVarDroite] = infosDroite;

    IRInstr *instrStoreRight = new IRInstrAffect(current_cfg->current_bb, to_string(infosDroite.location), "0");
    current_cfg->current_bb->add_IRInstr(instrStoreRight);

    // Effectuer l'opération += ou -=
    if(ctx->OP->getText() == "+=")
    {
        IRInstr *instrAdd = new IRInstrAdd(current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        current_cfg->current_bb->add_IRInstr(instrAdd);
    }
    else
    {
        IRInstr *instrSub = new IRInstrSub(current_cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
        current_cfg->current_bb->add_IRInstr(instrSub);
    }

    // Affecter le résultat à la variable d'origine
    IRInstr *instrAffect = new IRInstrAffect(current_cfg->current_bb, to_string(infosV.location), "0");
    current_cfg->current_bb->add_IRInstr(instrAffect);

    return 0;
}

// antlrcpp::Any IRVisitor::visitBreak(ifccParser::BreakContext *ctx){

//     // TO DO : Faire la gestion d'erreur autre part ??!
//     if (current_cfg->stack_break_destinations.empty()) {
//         std::cerr << "Erreur : break en dehors d’une boucle !" << std::endl;
//         exit(1);
//     }
    
//     BasicBlock* destinationBreak = current_cfg->stack_break_destinations.top();
    
//     string jump_label;
//     jump_label = destinationBreak->label;

//     IRInstr * instr = new IRInstrJump(current_cfg->current_bb, jump_label);
//     current_cfg->current_bb->add_IRInstr(instr);
    
//     return 0;
// }

// antlrcpp::Any IRVisitor::visitContinue(ifccParser::ContinueContext *ctx)
// {

//     int line = ctx->getStart()->getLine();
//     int column = ctx->getStart()->getCharPositionInLine();
//     // TO DO : Faire la gestion d'erreur autre part ??!
//     if (current_cfg->stack_boucle_test_block_for_continue.empty()) {
//         std::cerr << "Erreur : Continue en dehors d’une boucle !" << std::endl;
//         exit(1);
//     }

//     BasicBlock* destinationContinue = current_cfg->stack_boucle_test_block_for_continue.top();

//     string jump_label;
//     jump_label = destinationContinue->label;

//     IRInstr * instr = new IRInstrJump(current_cfg->current_bb, jump_label);
//     current_cfg->current_bb->add_IRInstr(instr);

//     return 0;
// }

// antlrcpp::Any IRVisitor::visitSwitch_case(ifccParser::Switch_caseContext *ctx) {

//     // Expression du switch qui sera à comparer avec les cases
//     this->visit(ctx->expr(0));

//     infosVariable switch_var;
//     switch_var.location = (_variables.size() + 1);
//     string switch_var_name = "switch" + to_string(current_temp++);
//     _variables[switch_var_name] = switch_var;

//     IRInstr *store_switch = new IRInstrAffect(current_cfg->current_bb, to_string(switch_var.location), "0");
//     current_cfg->current_bb->add_IRInstr(store_switch);

//     BasicBlock* switch_end_bb = new BasicBlock(current_cfg, ".endswitch" + to_string(current_test), nullptr, nullptr);
//     current_cfg->stack_break_destinations.push(switch_end_bb);

//     vector<BasicBlock*> case_blocks;
//     vector<ifccParser::ExprContext*> case_exprs;
//     BasicBlock* default_block = nullptr;

//     int num_cases = ctx->expr().size();
//     int block_index = 0;

//     // Création des blocs pour chaque case
//     for (int i = 1; i <= num_cases; i++) {
//         string label = ".case" + to_string(current_test) + "_" + to_string(i); 
//         BasicBlock* case_bb = new BasicBlock(current_cfg, label, nullptr, nullptr);
//         current_cfg->add_bb(case_bb);
//         case_blocks.push_back(case_bb);

//         case_exprs.push_back(ctx->expr(i));
//     }

//     // Bloc default s’il existe
//     if (ctx->DEFAULT() != nullptr) {
//         default_block = new BasicBlock(current_cfg, ".default" + to_string(current_test), nullptr, nullptr);
//         current_cfg->add_bb(default_block);
//     }

//     // Création des blocs de test
//     vector<BasicBlock*> test_blocks;

//     for (int i = 0; i < num_cases; i++) {
//         BasicBlock* cmp_bb = new BasicBlock(current_cfg, ".cmp" + to_string(current_test) + "_" + to_string(i), nullptr, nullptr);
//         current_cfg->add_bb(cmp_bb);
//         test_blocks.push_back(cmp_bb);
//     }

//     // Le bloc actuel pointe vers le 1er test
//     current_cfg->current_bb->exit_true = test_blocks[0];

//     // Génération des tests
//     for (int i = 0; i < num_cases; i++) {
//         BasicBlock* cmp_bb = test_blocks[i];

//         // Évalue l'expression du case[i]
//         current_cfg->current_bb = cmp_bb;
//         this->visit(case_exprs[i]);

//         string case_val_name = "case_val_" + to_string(current_temp++);
//         infosVariable case_var;
//         case_var.location = (_variables.size() + 1);
//         _variables[case_val_name] = case_var;

//         IRInstr *store_case = new IRInstrAffect(cmp_bb, to_string(case_var.location), "0");
//         cmp_bb->add_IRInstr(store_case);

//         // Crée un nouveau temp pour le résultat de la comparaison
//         string cmp_result_name = "cmp_result_" + to_string(current_temp++);
//         infosVariable cmp_var;
//         cmp_var.location = (_variables.size() + 1);
//         _variables[cmp_result_name] = cmp_var;

//         // Génère l’instruction de comparaison (switch == case)
//         IRInstr *cmp = new IRInstrCmpEQ(cmp_bb, to_string(cmp_var.location), to_string(switch_var.location), to_string(case_var.location));
//         cmp_bb->add_IRInstr(cmp);

//         cmp_bb->test_var_name = cmp_result_name;
//         cmp_bb->test_var_location = cmp_var.location;
//         cmp_bb->test_type = TEST_SWITCH;

//         cmp_bb->exit_true = case_blocks[i];
        
//         if (i + 1 < num_cases) {
//             cmp_bb->exit_false = test_blocks[i + 1]; // Aller au test suivant
//         } else if (default_block != nullptr) {
//             cmp_bb->exit_false = default_block; // Sinon, aller au bloc default s’il existe
//         } else {
//             cmp_bb->exit_false = switch_end_bb; // Sinon, aller directement à la fin du switch
//         }
//     }

//     // Génération des blocs de chaque case
//     for (int i = 0; i < case_blocks.size(); i++) {
//         current_cfg->current_bb = case_blocks[i];
//         this->visit(ctx->block(block_index++));

//         if (current_cfg->current_bb->exit_true == nullptr)
//             current_cfg->current_bb->exit_true = switch_end_bb;
//     }

//     // Bloc default
//     if (default_block != nullptr) {
//         current_cfg->current_bb = default_block;
//         this->visit(ctx->block(block_index++));

//         if (current_cfg->current_bb->exit_true == nullptr)
//             current_cfg->current_bb->exit_true = switch_end_bb;
//     }

//     current_cfg->current_bb = switch_end_bb;
//     current_cfg->add_bb(switch_end_bb);

//     current_cfg->stack_break_destinations.pop();
//     current_test++;

//     return 0;
// }

antlrcpp::Any IRVisitor::visitFunction_call(ifccParser::Function_callContext *ctx) {
    string func_name = ctx->ID()->getText();

    for (size_t i = 0; i < ctx->expr().size(); ++i) {
        this->visit(ctx->expr(i));
        IRInstr * instrParam = new IRInstrAffect(current_cfg->current_bb, "-"+to_string(i+1), "0");
        current_cfg->current_bb->add_IRInstr(instrParam);
    }

    IRInstr * instrClearEax = new IRInstrAffect(current_cfg->current_bb, "0", "$0");
    current_cfg->current_bb->add_IRInstr(instrClearEax);
    IRInstr *instr = new IRInstrFunc_Call(current_cfg->current_bb, func_name);
    current_cfg->current_bb->add_IRInstr(instr);


    return 0;
}



antlrcpp::Any IRVisitor::visitFunction_definition(ifccParser::Function_definitionContext *ctx) {

    string funcName = ctx->ID(0)->getText(); 

    current_cfg = new CFG(funcName);
    current_cfg->add_bb(new BasicBlock(current_cfg, funcName, nullptr, nullptr));
    current_cfg->current_bb = current_cfg->get_bbs()[0];

    current_temp = 0;

    currentBlock = nullptr;

    //__________________________________________________________________________
    ifccParser::BlockContext* block_ctx = any_cast<ifccParser::BlockContext*>(this->visit(ctx->block()));

    if(currentBlock == nullptr) 
    {
        currentBlock = _rootBlocks[_cfgs.size()];
    }
    else 
    {
        Block* tempBlock = currentBlock->notVisitedChildren[0];
        currentBlock->notVisitedChildren.erase(currentBlock->notVisitedChildren.begin());
        currentBlock = tempBlock;
    }

    for(size_t i = 1; i < ctx->ID().size(); ++i) {
        string id = ctx->ID(i)->getText();
        infosVariable infosParam = getInfosVariable(currentBlock, id);
        IRInstr * instrParam = new IRInstrAffect(current_cfg->current_bb, to_string(infosParam.location), "-"+to_string(i));
        current_cfg->current_bb->add_IRInstr(instrParam);
    }

    for (int i = 0; i < block_ctx->instruction().size(); i++) {
        this->visit(block_ctx->instruction(i)); // Visiter chaque instruction
        // si break on break pour ne pas generer les instruction du bb
        if(block_ctx->instruction(i)->break_() != nullptr) {
            break;
        }
    }
    if(currentBlock->parent != nullptr)
    {
        currentBlock = currentBlock->parent;
    }
    //__________________________________________________________________________

    _cfgs.push_back(current_cfg);

    return 0;
}



