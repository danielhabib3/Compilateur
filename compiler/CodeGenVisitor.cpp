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

antlrcpp::Any CodeGenVisitor::visitExprAddSub(ifccParser::ExprAddSubContext *ctx)
{
    this->visit(ctx->expr(0));

    infosVariable infosGauche;
    infosGauche.location = (_variables.size() + 1)*4;
    _variables["temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1)*4;
    _variables["temp" + to_string(current_temp)] = infosDroite;
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
    _variables["temp" + to_string(current_temp)] = infosGauche;
    current_temp++;
    std::cout << "    movl %eax, -"<<infosGauche.location<<"(%rbp)\n" ;

    this->visit(ctx->expr(1));

    infosVariable infosDroite;
    infosDroite.location = (_variables.size() + 1)*4;
    _variables["temp" + to_string(current_temp)] = infosDroite;
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
    return 0;
}
