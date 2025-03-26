#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
    #ifdef __APPLE__
    std::cout<< ".globl _main\n" ;
    std::cout<< " _main: \n" ;
    #else
    std::cout<< ".globl main\n" ;
    std::cout<< " main: \n" ;
    #endif

    std::cout<< "    pushq %rbp\n" ;
    std::cout<< "    movq %rsp, %rbp\n" ;

    this->visitChildren(ctx);

    std::cout<< "    popq %rbp\n" ;
    
    std::cout << "    ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{
    this->visit(ctx->expr());
    infosVariable infosV = _variables[ctx->ID()->getText()];
    std::cout << "    movl %eax, -"<<infosV.location<<"(%rbp)\n" ;
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx)
{
    if(ctx->expr() != nullptr) {
        this->visit(ctx->expr());
        infosVariable infosV = _variables[ctx->ID()->getText()];
        std::cout << "    movl %eax, -"<<infosV.location<<"(%rbp)\n" ;
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprID(ifccParser::ExprIDContext *ctx)
{
    infosVariable infosV = _variables[ctx->ID()->getText()];
    std::cout << "    movl -"<<infosV.location<<"(%rbp), %eax\n" ;
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprConst(ifccParser::ExprConstContext *ctx)
{
    int val = stoi(ctx->CONST()->getText());
    std::cout << "    movl $"<<val<<", %eax\n" ;
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprChar(ifccParser::ExprCharContext *ctx)
{
    char c = ctx->CHAR()->getText()[1]; // extract the character between the quotes
    int ascii = (int)c;
    std::cout << "    movl $" << ascii << ", %eax\n";
    return nullptr;
}

antlrcpp::Any CodeGenVisitor::visitExprAddSub(ifccParser::ExprAddSubContext *ctx)
{
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;
    
    if(ctx->OP->getText() == "+")
    {
        std::cout << "    addl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
    }
    else
    {
        std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        std::cout << "    subl -"<<infosDroite.location<<"(%rbp), %eax\n" ;
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprMulDivMod(ifccParser::ExprMulDivModContext *ctx)
{
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosDroite;
    current_temp++;

    if(ctx->OP->getText() == "*")
    {
        std::cout << "    imull -"<<infosGauche.location<<"(%rbp), %eax\n" ;
    }
    else if(ctx->OP->getText() == "/")
    {
        std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        std::cout << "    cltd\n" ;
        std::cout << "    idivl -"<<infosDroite.location<<"(%rbp)\n" ;
    }
    else if(ctx->OP->getText() == "%")
    {
        std::cout << "    movl %eax, -"<<infosDroite.location<<"(%rbp)\n" ;
        std::cout << "    movl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
        std::cout << "    cltd\n" ;
        std::cout << "    idivl -"<<infosDroite.location<<"(%rbp)\n" ;
        std::cout << "    movl %edx, %eax\n" ;  
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprUnary(ifccParser::ExprUnaryContext *ctx)
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

antlrcpp::Any CodeGenVisitor::visitExprCompSupInf(ifccParser::ExprCompSupInfContext *ctx)
{

    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;

    this->visit(ctx->expr(1));

    if(ctx->OP->getText() == ">")
    {
        std::cout << "    cmpl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
        std::cout << "  setg %al\n";
        std::cout << "  movzbl %al, %eax\n";
    }
    else if(ctx->OP->getText() == "<")
    {
    
        std::cout << "    cmpl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
        std::cout << "  setl %al\n";  // Utilisation de setl (set less) pour <
        std::cout << "  movzbl %al, %eax\n";        
    }

    return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitExprCompEqual(ifccParser::ExprCompEqualContext *ctx)
{
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;

    this->visit(ctx->expr(1));

    std::cout << "    cmpl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;
    if(ctx->OP->getText() == "==")
        std::cout << "    sete %al\n" ;
    else
        std::cout << "    setne %al\n" ;
    std::cout << "    movzbl %al, %eax\n" ;

    return antlrcpp::Any();
}

antlrcpp::Any CodeGenVisitor::visitExprAndBit(ifccParser::ExprAndBitContext *ctx)
{
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;

    this->visit(ctx->expr(1));

    std::cout << "    andl -"<<infosGauche.location<<"(%rbp), %eax\n" ;

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprOrBit(ifccParser::ExprOrBitContext *ctx)
{
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;

    this->visit(ctx->expr(1));

    std::cout << "    orl -"<<infosGauche.location<<"(%rbp), %eax\n" ;
    
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprXorBit(ifccParser::ExprXorBitContext *ctx)
{
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["!temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;

    this->visit(ctx->expr(1));

    std::cout << "    xorl -"<<infosGauche.location<<"(%rbp), %eax\n" ;


    return 0;
}
