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

    this->visit( ctx->block() );
    this->visit( ctx->return_stmt() );

    std::cout<< "    popq %rbp\n" ;
    
    std::cout << "    ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{
    infosVariable infosVGauche = _variables[ctx->ID()->getText()];
    if(ctx->value()->ID() != nullptr)
    {
        infosVariable infosVDroite = _variables[ctx->value()->ID()->getText()];
        std::cout << "    movl -"<<infosVDroite.location<<"(%rbp), -"<<infosVGauche.location<<"(%rbp)\n" ;
    }
    else
    {
        int valDroite = stoi(ctx->value()->CONST()->getText());
        std::cout << "    movl $"<<valDroite<<", -"<<infosVGauche.location<<"(%rbp)\n" ;
    }

    return 0;
}

// anltrcpp::Any CodeGenVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx)
// {

// }


antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    if(ctx->value()->ID() != nullptr)
    {
        infosVariable infosV = _variables[ctx->value()->ID()->getText()];
        std::cout << "    movl -"<<infosV.location<<"(%rbp), %eax\n" ;
    }
    else
    {
        int retval = stoi(ctx->value()->CONST()->getText());
        std::cout << "    movl $"<<retval<<", %eax\n" ;
    }
    
    return 0;
}
