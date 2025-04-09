#include "IRVisitor.h"

antlrcpp::Any IRVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{
    this->visit(ctx->expr());
    infosVariable infosV = _variables[ctx->ID()->getText()];
    // std::cout << "    movl %eax, -"<<infosV.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosV.location), "0");
    _cfg->current_bb->add_IRInstr(instr);
    return 0;
}

antlrcpp::Any IRVisitor::visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx)
{
    if(ctx->expr() != nullptr) {
        this->visit(ctx->expr());
        infosVariable infosV = _variables[ctx->ID()->getText()];
        // std::cout << "    movl %eax, -"<<infosV.location<<"(%rbp)\n" ;
        IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosV.location), "0");
        _cfg->current_bb->add_IRInstr(instr);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitExprID(ifccParser::ExprIDContext *ctx)
{
    infosVariable infosV = _variables[ctx->ID()->getText()];
    // std::cout << "    movl -"<<inffosV.location<<"(%rbp), %eax\n" ;
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
    infosGauche.location = (_variables.size() + 1);
    _variables["!tempIR" + to_string(current_temp)] = infosGauche;
    current_temp++;

    // std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1);
    _variables["!tempIR" + to_string(current_temp)] = infosDroite;
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
    infosGauche.location = (_variables.size() + 1);
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;

    // std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1);
    _variables["!temp" + to_string(current_temp)] = infosDroite;
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
    infosGauche.location = (_variables.size() + 1);
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    // std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1);
    _variables["!temp" + to_string(current_temp)] = infosDroite;
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
    infosGauche.location = (_variables.size() + 1);
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    // std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1);
    _variables["!temp" + to_string(current_temp)] = infosDroite;
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
    infosGauche.location = (_variables.size() + 1);
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    //std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1);
    _variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    //std::cout << "    andl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    IRInstr *instrXor = new IRInstrAndBit( _cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
    _cfg->current_bb->add_IRInstr(instrXor);

    return 0;
 }
 

 antlrcpp::Any IRVisitor::visitExprOrBit(ifccParser::ExprOrBitContext *ctx){
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1);
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    //std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1);
    _variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    //std::cout << "    orl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
 
    IRInstr *instrXor = new IRInstrOrBit( _cfg->current_bb, "0", to_string(infosGauche.location), to_string(infosDroite.location));
    _cfg->current_bb->add_IRInstr(instrXor);

    return 0;
 }

 antlrcpp::Any IRVisitor::visitExprXorBit(ifccParser::ExprXorBitContext *ctx){
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1);
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    //std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    IRInstr * instr = new IRInstrAffect(_cfg->current_bb, to_string(infosGauche.location), "0");
    _cfg->current_bb->add_IRInstr(instr);

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1);
    _variables["!temp" + to_string(current_temp)] = infosDroite;
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
    infos.location = (_variables.size() + 1);
    string test_var_name = "test" + to_string(current_temp);
    _variables[test_var_name] = infos;
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
    infos.location = (_variables.size() + 1);
    string test_var_name = "test" + to_string(current_temp);
    _variables[test_var_name] = infos;
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


antlrcpp::Any IRVisitor::visitBreak(ifccParser::BreakContext *ctx){

    // TO DO : Faire la gestion d'erreur autre part ??!
    if (_cfg->stack_break_destinations.empty()) {
        std::cerr << "Erreur : break en dehors d’une boucle !" << std::endl;
        exit(1);
    }
    
    BasicBlock* destinationBreak = _cfg->stack_break_destinations.top();
    
    string jump_label;
    jump_label = destinationBreak->label;

    IRInstr * instr = new IRInstrJump(_cfg->current_bb, jump_label);
    _cfg->current_bb->add_IRInstr(instr);
    
    return 0;
}

// TO DO : Ne pas faire 2 fonctions pour visitCOntinue et visitBreak mais les merge
antlrcpp::Any IRVisitor::visitContinue(ifccParser::ContinueContext *ctx){

    // TO DO : Faire la gestion d'erreur autre part ??!
    if (_cfg->stack_boucle_test_block_for_continue.empty()) {
        std::cerr << "Erreur : Continue en dehors d’une boucle !" << std::endl;
        exit(1);
    }

    BasicBlock* destinationContinue = _cfg->stack_boucle_test_block_for_continue.top();

    string jump_label;
    jump_label = destinationContinue->label;

    IRInstr * instr = new IRInstrJump(_cfg->current_bb, jump_label);
    _cfg->current_bb->add_IRInstr(instr);

    return 0;
}

antlrcpp::Any IRVisitor::visitSwitch_case(ifccParser::Switch_caseContext *ctx) {

    // Expression du switch qui sera à comparer avec les cases
    this->visit(ctx->expr(0));

    infosVariable switch_var;
    switch_var.location = (_variables.size() + 1);
    string switch_var_name = "switch" + to_string(current_temp++);
    _variables[switch_var_name] = switch_var;

    IRInstr *store_switch = new IRInstrAffect(_cfg->current_bb, to_string(switch_var.location), "0");
    _cfg->current_bb->add_IRInstr(store_switch);

    BasicBlock* switch_end_bb = new BasicBlock(_cfg, ".endswitch" + to_string(current_test), nullptr, nullptr);
    _cfg->stack_break_destinations.push(switch_end_bb);

    vector<BasicBlock*> case_blocks;
    vector<ifccParser::ExprContext*> case_exprs;
    BasicBlock* default_block = nullptr;

    int num_cases = ctx->expr().size();
    int block_index = 0;

    // Création des blocs pour chaque case
    for (int i = 1; i <= num_cases; i++) {
        string label = ".case" + to_string(current_test) + "_" + to_string(i); 
        BasicBlock* case_bb = new BasicBlock(_cfg, label, nullptr, nullptr);
        _cfg->add_bb(case_bb);
        case_blocks.push_back(case_bb);

        case_exprs.push_back(ctx->expr(i));
    }

    // Bloc default s’il existe
    if (ctx->DEFAULT() != nullptr) {
        default_block = new BasicBlock(_cfg, ".default" + to_string(current_test), nullptr, nullptr);
        _cfg->add_bb(default_block);
    }

    // Création des blocs de test
    vector<BasicBlock*> test_blocks;

    for (int i = 0; i < num_cases; i++) {
        BasicBlock* cmp_bb = new BasicBlock(_cfg, ".cmp" + to_string(current_test) + "_" + to_string(i), nullptr, nullptr);
        _cfg->add_bb(cmp_bb);
        test_blocks.push_back(cmp_bb);
    }

    // Le bloc actuel pointe vers le 1er test
    _cfg->current_bb->exit_true = test_blocks[0];

    // Génération des tests
    for (int i = 0; i < num_cases; i++) {
        BasicBlock* cmp_bb = test_blocks[i];

        // Évalue l'expression du case[i]
        _cfg->current_bb = cmp_bb;
        this->visit(case_exprs[i]);

        string case_val_name = "case_val_" + to_string(current_temp++);
        infosVariable case_var;
        case_var.location = (_variables.size() + 1);
        _variables[case_val_name] = case_var;

        IRInstr *store_case = new IRInstrAffect(cmp_bb, to_string(case_var.location), "0");
        cmp_bb->add_IRInstr(store_case);

        // Crée un nouveau temp pour le résultat de la comparaison
        string cmp_result_name = "cmp_result_" + to_string(current_temp++);
        infosVariable cmp_var;
        cmp_var.location = (_variables.size() + 1);
        _variables[cmp_result_name] = cmp_var;

        // Génère l’instruction de comparaison (switch == case)
        IRInstr *cmp = new IRInstrCmpEQ(cmp_bb, to_string(cmp_var.location), to_string(switch_var.location), to_string(case_var.location));
        cmp_bb->add_IRInstr(cmp);

        cmp_bb->test_var_name = cmp_result_name;
        cmp_bb->test_var_location = cmp_var.location;
        cmp_bb->test_type = TEST_SWITCH;

        cmp_bb->exit_true = case_blocks[i];
        
        if (i + 1 < num_cases) {
            cmp_bb->exit_false = test_blocks[i + 1]; // Aller au test suivant
        } else if (default_block != nullptr) {
            cmp_bb->exit_false = default_block; // Sinon, aller au bloc default s’il existe
        } else {
            cmp_bb->exit_false = switch_end_bb; // Sinon, aller directement à la fin du switch
        }
    }

    // Génération des blocs de chaque case
    for (int i = 0; i < case_blocks.size(); i++) {
        _cfg->current_bb = case_blocks[i];
        this->visit(ctx->block(block_index++));

        if (_cfg->current_bb->exit_true == nullptr)
            _cfg->current_bb->exit_true = switch_end_bb;
    }

    // Bloc default
    if (default_block != nullptr) {
        _cfg->current_bb = default_block;
        this->visit(ctx->block(block_index++));

        if (_cfg->current_bb->exit_true == nullptr)
            _cfg->current_bb->exit_true = switch_end_bb;
    }

    _cfg->current_bb = switch_end_bb;
    _cfg->add_bb(switch_end_bb);

    _cfg->stack_break_destinations.pop();
    current_test++;

    return 0;
}




