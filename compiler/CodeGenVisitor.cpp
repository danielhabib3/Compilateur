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
    for(int i = ctx->value().size()-1; i > -1 ; i--)
    {
        infosVariable infosVGauche;
        if(i == 0)
        {
            infosVGauche = _variables[ctx->ID()->getText()];
        }
        else
        {
            infosVGauche = _variables[ctx->value(i-1)->ID()->getText()];
        }

        if(ctx->value(i)->ID() != nullptr)
        {
            infosVariable infosVDroite = _variables[ctx->value(i)->ID()->getText()];
            std::cout << "    movl -"<<infosVDroite.location<<"(%rbp), %eax\n" ;
            std::cout << "    movl %eax, -"<<infosVGauche.location<<"(%rbp)\n" ;
        }
        else
        {
            // c'est forcément pour i = ctx->value().size()-1
            int valDroite = stoi(ctx->value(i)->CONST()->getText());
            std::cout << "    movl $"<<valDroite<<", -"<<infosVGauche.location<<"(%rbp)\n" ;
        }
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx)
{
    if(ctx->value().size() > 0) {
        for(int i = ctx->value().size()-1; i > -1 ; i--)
        {
            infosVariable infosVGauche;
            if(i == 0)
            {
                infosVGauche = _variables[ctx->ID()->getText()];
            }
            else
            {
                infosVGauche = _variables[ctx->value(i-1)->ID()->getText()];
            }

            if(ctx->value(i)->ID() != nullptr)
            {
                infosVariable infosVDroite = _variables[ctx->value(i)->ID()->getText()];
                std::cout << "    movl -"<<infosVDroite.location<<"(%rbp), %eax\n" ;
                std::cout << "    movl %eax, -"<<infosVGauche.location<<"(%rbp)\n" ;
            }
            else
            {
                // c'est forcément pour i = ctx->value().size()-1
                int valDroite = stoi(ctx->value(i)->CONST()->getText());
                std::cout << "    movl $"<<valDroite<<", -"<<infosVGauche.location<<"(%rbp)\n" ;
            }
        }
    }
    return 0;
}

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
