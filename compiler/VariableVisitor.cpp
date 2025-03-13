#include "VariableVisitor.h"

map<string, infosVariable> _variables = {};
list<string> _variableErrors = {};

antlrcpp::Any VariableVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) 
{
    for(int i = 0; i < ctx->ID().size(); i++)
    {
        string variableName = ctx->ID(i)->getText();
        string type = ctx->type()->getText();
        int line = ctx->getStart()->getLine();
        int column = ctx->getStart()->getCharPositionInLine();
        if(_variables.find(variableName) != _variables.end())
        {
            infosVariable infosDeclaredVariable = _variables[variableName];
            _variableErrors.push_back("Error : Multiple Declarations : " + std::to_string(line) + ":" + std::to_string(column) + " : Variable " + variableName + " already declared in " + std::to_string(infosDeclaredVariable.line) + ":" + std::to_string(infosDeclaredVariable.column));
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
                _variableErrors.push_back("Error : Unknown type : " + std::to_string(line) + ":" + std::to_string(column) + " : " + type);
            }
            infos.set = 0;
            infos.line = ctx->getStart()->getLine();
            infos.column = ctx->getStart()->getCharPositionInLine();
            infos.nbUse = 0;
            _variables[variableName] = infos;
        }
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
        _variableErrors.push_back("Error : Unknown variable : " + std::to_string(line) + ":" + std::to_string(column) + " : Variable " + variableGaucheName + " not declared");
    }
    else
    {
        infosVariable infos = _variables[variableGaucheName];
        infos.set = 1;
        _variables[variableGaucheName] = infos;
    }

    if(ctx->value()->ID() != nullptr)
    {
        string variableDroiteName = ctx->value()->ID()->getText();
        if(_variables.find(variableDroiteName) == _variables.end())
        {
            int line = ctx->getStart()->getLine();
            int column = ctx->getStart()->getCharPositionInLine();
            _variableErrors.push_back("Error : Unknown variable : " + std::to_string(line) + ":" + std::to_string(column) + " : Variable " + variableDroiteName + " not declared");
        }
        else
        {
            infosVariable infos = _variables[variableDroiteName];
            infos.nbUse++;
            _variables[variableDroiteName] = infos;
        }
    }
    return 0;
}

antlrcpp::Any VariableVisitor::visitValue(ifccParser::ValueContext *ctx) 
{
    if(ctx->ID() != nullptr)
    {
        string variableName = ctx->ID()->getText();
        if(_variables.find(variableName) == _variables.end())
        {
            int line = ctx->getStart()->getLine();
            int column = ctx->getStart()->getCharPositionInLine();
            _variableErrors.push_back("Error : Unknown variable : " + std::to_string(line) + ":" + std::to_string(column) + " : Variable " + variableName + " not declared");
        }
        else
        {
            infosVariable infos = _variables[variableName];
            infos.nbUse++;
            _variables[variableName] = infos;
        }
    }
    return 0;
}