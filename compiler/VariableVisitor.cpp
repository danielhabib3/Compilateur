#include "VariableVisitor.h"


antlrcpp::Any VariableVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) 
{
    // cout << "Declaration : Visiting declaration" << endl;
    for(int i = 0; i < ctx->affectationDeclaration().size(); i++)
    {
        // cout << "Declaration : Visiting affectationDeclaration : " << i << endl;
        pair<antlr4::tree::TerminalNode *, ifccParser::ExprContext *> affectationDeclaration = any_cast<pair<antlr4::tree::TerminalNode*, ifccParser::ExprContext*>>(this->visit(ctx->affectationDeclaration(i)));
        antlr4::tree::TerminalNode * ID = affectationDeclaration.first;
        ifccParser::ExprContext * expression = affectationDeclaration.second;
        string variableName = ID->getText();
        string type = ctx->type()->getText();
        int line = ctx->getStart()->getLine();
        int column = ctx->getStart()->getCharPositionInLine();
        if(_variables.find(variableName) != _variables.end())
        {
            infosVariable infosDeclaredVariable = _variables[variableName];
            _variableErrorsWarnings["Error : Multiple Declarations : " + to_string(line) + ":" + to_string(column) + " : Variable \"" + variableName + "\" already declared in " + to_string(infosDeclaredVariable.line) + ":" + to_string(infosDeclaredVariable.column)] = ERROR;
        }
        else
        {
            // cout << "Declaration : Adding variable : " << variableName << endl;
            infosVariable infos;
            infos.location = (_variables.size() + 1)*4;
            if(type == "int")
            {
                infos.type = INT;
            }
            else
            {
                _variableErrorsWarnings["Error : Unknown type : " + to_string(line) + ":" + to_string(column) + " : " + type] = ERROR;
            }
            infos.set = 0;
            infos.line = ctx->getStart()->getLine();
            infos.column = ctx->getStart()->getCharPositionInLine();
            infos.nbUse = 0;
            _variables[variableName] = infos;
        }

        // cout << "Declaration : Visit Declaration end before visiting expr" << endl;
        if(expression != nullptr) {
            infosVariable infos = _variables[variableName];
            infos.set = 1;
            _variables[variableName] = infos;
            this->visit(expression);
        }
        // cout << "Declaration : Visit Declaration end after visiting expr" << endl;

    }
    return 0;
}

antlrcpp::Any VariableVisitor::visitAffectation(ifccParser::AffectationContext *ctx) 
{
    // cout << "Visiting affectation" << endl;
    int line = ctx->getStart()->getLine();
    int column = ctx->getStart()->getCharPositionInLine();

    string variableGaucheName = ctx->ID()->getText();

    if(_variables.find(variableGaucheName) == _variables.end())
    {
        _variableErrorsWarnings["Error : Unknown variable : " + to_string(line) + ":" + to_string(column) + " : Variable " + variableGaucheName + " not declared"] = ERROR;
    }
    else
    {
        infosVariable infos = _variables[variableGaucheName];
        infos.set = 1;
        _variables[variableGaucheName] = infos;
    }
    

    // visiter la dernière valeur qui ne sera pas affectée
    this->visit(ctx->expr());

    return 0;
}

antlrcpp::Any VariableVisitor::visitExprID(ifccParser::ExprIDContext *ctx) 
{
    // cout << "Visiting expr" << endl;
    string variableName = ctx->ID()->getText();
    int line = ctx->getStart()->getLine();
    int column = ctx->getStart()->getCharPositionInLine();
    if(_variables.find(variableName) == _variables.end())
    {
        _variableErrorsWarnings["Error : Unknown variable : " + to_string(line) + ":" + to_string(column) + " : Variable \"" + variableName + "\" not declared"] = ERROR;
    }
    else
    {
        infosVariable infos = _variables[variableName];
        infos.nbUse++;
        _variables[variableName] = infos;
        if(infos.set == 0) {
            _variableErrorsWarnings["Warning : Using variable not initialized : " + to_string(line) + ":" + to_string(column) + " : Using variable \"" + variableName + "\" not initialized, declared at " + to_string(infos.line) + ":" + to_string(infos.column)] = WARNING;
        }
    }
    return 0;
}

antlrcpp::Any VariableVisitor::visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx)
{
    // cout << "Visiting affectationDeclaration" << endl;
    return (pair<antlr4::tree::TerminalNode *, ifccParser::ExprContext *>) make_pair(ctx->ID(), ctx->expr());
}

