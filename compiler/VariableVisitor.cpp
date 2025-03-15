#include "VariableVisitor.h"


antlrcpp::Any VariableVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) 
{
    // cout << "Declaration : Visiting declaration" << endl;
    for(int i = 0; i < ctx->affectationDeclaration().size(); i++)
    {
        // cout << "Declaration : Visiting affectationDeclaration : " << i << endl;
        pair<antlr4::tree::TerminalNode *, vector<ifccParser::ValueContext *>> affectationDeclaration = any_cast<pair<antlr4::tree::TerminalNode*, vector<ifccParser::ValueContext*>>>(this->visit(ctx->affectationDeclaration(i)));
        antlr4::tree::TerminalNode * ID = affectationDeclaration.first;
        vector<ifccParser::ValueContext *> values = affectationDeclaration.second;
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

        for(int j = 0; j < values.size(); j++)
        {
            string variableGaucheName;
            if(j == 0) {
                variableGaucheName = ID->getText();
            } else {
                if(values[j-1]->ID() != nullptr)
                    variableGaucheName = values[j-1]->ID()->getText();
                else
                    _variableErrorsWarnings["Error : Left operand isn't a variable : " + to_string(line) + ":" + to_string(column) + " : Left opreand is " + values[j-1]->CONST()->getText() + " not a variable"] = ERROR;
            }

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
        }
        

        // cout << "Declaration : Visit Declaration end before visiting value" << endl;
        if(ctx->affectationDeclaration(i)->value().size() != 0)
            this->visit(ctx->affectationDeclaration(i)->value(ctx->affectationDeclaration(i)->value().size()-1));
        // cout << "Declaration : Visit Declaration end after visiting value" << endl;

    }
    return 0;
}

antlrcpp::Any VariableVisitor::visitAffectation(ifccParser::AffectationContext *ctx) 
{
    // cout << "Visiting affectation" << endl;
    int line = ctx->getStart()->getLine();
    int column = ctx->getStart()->getCharPositionInLine();
    for(int i = 0; i < ctx->value().size(); i++)
    {
        string variableGaucheName;
        if(i == 0) {
            variableGaucheName = ctx->ID()->getText();
        } else {
            if(ctx->value(i-1)->ID() != nullptr)
                variableGaucheName = ctx->value(i-1)->ID()->getText();
            else
                _variableErrorsWarnings["Error : Left operand isn't a variable : " + to_string(line) + ":" + to_string(column) + " : Left opreand is " + ctx->value(i-1)->CONST()->getText() + " not a variable"] = ERROR;
        }

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
    }

    // visiter la dernière valeur qui ne sera pas affectée
    this->visit(ctx->value(ctx->value().size()-1));

    return 0;
}

antlrcpp::Any VariableVisitor::visitValue(ifccParser::ValueContext *ctx) 
{
    // cout << "Visiting value" << endl;
    if(ctx->ID() != nullptr)
    {
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
    }
    return 0;
}

antlrcpp::Any VariableVisitor::visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx)
{
    // cout << "Visiting affectationDeclaration" << endl;
    return (pair<antlr4::tree::TerminalNode *, vector<ifccParser::ValueContext *>>) make_pair(ctx->ID(), ctx->value());
}
