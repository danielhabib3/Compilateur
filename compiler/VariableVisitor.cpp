#include "VariableVisitor.h"


antlrcpp::Any VariableVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) 
{
    for(int i = 0; i < ctx->affectationDeclaration().size(); i++)
    {
        pair<antlr4::tree::TerminalNode *, ValueContext *> affectationDeclaration = this->visit(ctx->affectationDeclaration(i));
        antlr4::tree::TerminalNode * ID = affectationDeclaration.first;
        ValueContext * value = affectationDeclaration.second;
        string variableName = ID->getText();
        string type = ctx->type()->getText();
        int line = ctx->getStart()->getLine();
        int column = ctx->getStart()->getCharPositionInLine();
        if(_variables.find(variableName) != _variables.end())
        {
            infosVariable infosDeclaredVariable = _variables[variableName];
            _variableErrorsWarnings["Error : Multiple Declarations : " + std::to_string(line) + ":" + std::to_string(column) + " : Variable \"" + variableName + "\" already declared in " + std::to_string(infosDeclaredVariable.line) + ":" + std::to_string(infosDeclaredVariable.column)] = ERROR;
        }
        else
        {
            infosVariable infos;
            infos.location = (_variables.size() + 1)*4;
            if(type == "int")
            {
                infos.type = INT;
            }
            else
            {
                _variableErrorsWarnings["Error : Unknown type : " + std::to_string(line) + ":" + std::to_string(column) + " : " + type] = ERROR;
            }
            infos.set = 0;
            infos.line = ctx->getStart()->getLine();
            infos.column = ctx->getStart()->getCharPositionInLine();
            infos.nbUse = 0;
            _variables[variableName] = infos;
        }

        this->visit(value);
    }
    return 0;
}

antlrcpp::Any VariableVisitor::visitAffectation(ifccParser::AffectationContext *ctx) 
{
    string variableGaucheName = ctx->ID()->getText();
    if(_variables.find(variableGaucheName) == _variables.end())
    {
        int line = ctx->getStart()->getLine();
        int column = ctx->getStart()->getCharPositionInLine();
        _variableErrorsWarnings["Error : Unknown variable : " + std::to_string(line) + ":" + std::to_string(column) + " : Variable " + variableGaucheName + " not declared"] = ERROR;
    }
    else
    {
        infosVariable infos = _variables[variableGaucheName];
        infos.set = 1;
        _variables[variableGaucheName] = infos;
    }

    this->visit(ctx->value());

    return 0;
}

antlrcpp::Any VariableVisitor::visitValue(ifccParser::ValueContext *ctx) 
{
    if(ctx->ID() != nullptr)
    {
        string variableName = ctx->ID()->getText();
        int line = ctx->getStart()->getLine();
        int column = ctx->getStart()->getCharPositionInLine();
        if(_variables.find(variableName) == _variables.end())
        {
            _variableErrorsWarnings["Error : Unknown variable : " + std::to_string(line) + ":" + std::to_string(column) + " : Variable \"" + variableName + "\" not declared"] = ERROR;
        }
        else
        {
            infosVariable infos = _variables[variableName];
            infos.nbUse++;
            _variables[variableName] = infos;
            if(infos.set == 0) {
                _variableErrorsWarnings["Warning : Using variable not initialized : " + std::to_string(line) + ":" + std::to_string(column) + " : Using variable \"" + variableName + "\" not initialized, declared at " + std::to_string(infos.line) + ":" + std::to_string(infos.column)] = WARNING;
            }
        }
    }
    return 0;
}

antlrcpp::Any VariableVisitor::visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx)
{
    return (pair<antlr4::tree::TerminalNode *, ValueContext *>(ctx->ID(), ctx->value()));
}
