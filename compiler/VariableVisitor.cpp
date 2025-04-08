#include "VariableVisitor.h"


antlrcpp::Any VariableVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) 
{
    // cout << "Declaration : Visiting declaration" << endl;
    for(int i = 0; i < ctx->affectationDeclaration().size(); i++)
    {
        // cout << "Declaration : Visiting affectationDeclaration : " << i << endl;
        ifccParser::AffectationDeclarationContext * affectationDeclaration = any_cast<ifccParser::AffectationDeclarationContext *>(this->visit(ctx->affectationDeclaration(i)));
        antlr4::tree::TerminalNode * ID = affectationDeclaration->ID();
        ifccParser::ExprContext * expression = affectationDeclaration->expr();
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
            infos.location = this->next_free_location;
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
            this->next_free_location++;
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

antlrcpp::Any VariableVisitor::visitExprAffectation(ifccParser::ExprAffectationContext *ctx) 
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
    return (ifccParser::AffectationDeclarationContext *) ctx;
}

antlrcpp::Any VariableVisitor::visitDeclarationTable(ifccParser::DeclarationTableContext *ctx)
{
    // cout << "Declaration : Visiting declaration" << endl;
    for(int i = 0; i < ctx->affectationDeclarationTable().size(); i++)
    {
        // cout << "Declaration : Visiting affectationDeclarationTable : " << i << endl;
        ifccParser::AffectationDeclarationTableContext * affectationDeclarationTable = any_cast<ifccParser::AffectationDeclarationTableContext *>(this->visit(ctx->affectationDeclarationTable(i)));
        antlr4::tree::TerminalNode * ID = affectationDeclarationTable->ID();
        vector<ifccParser::ExprContext *> expressions = affectationDeclarationTable->expr();
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
            infos.location = this->next_free_location;
            infos.isArray = true;
            infos.size = stoi(affectationDeclarationTable->CONST()->getText());
            if(type == "int")
            {
                infos.type = INT;
            }
            else
            {
                _variableErrorsWarnings["Error : Unknown type : " + to_string(line) + ":" + to_string(column) + " : " + type] = ERROR;
            }
            infos.line = ctx->getStart()->getLine();
            infos.column = ctx->getStart()->getCharPositionInLine();
            infos.nbUse = 0;
            _variables[variableName] = infos;
            this->next_free_location += infos.size;
        }

        infosVariable infos = _variables[variableName];
        if(infos.size < expressions.size())
        {
            _variableErrorsWarnings["Warning : Array size too small : " + to_string(line) + ":" + to_string(column) + " : Array \"" + variableName + "\" size is " + to_string(infos.size) + ", but " + to_string(expressions.size()) + " values are given"] = WARNING;
        }
        
        // cout << "Declaration : Visit Declaration end before visiting expr" << endl;
        for(int j = 0; j < expressions.size(); j++)
        {
            // cout << "Declaration : Visiting expr : " << j << endl;
            this->visit(expressions[j]);
        }
        // cout << "Declaration : Visit Declaration end after visiting expr" << endl;

    }
    return 0;
}

antlrcpp::Any VariableVisitor::visitAffectationDeclarationTable(ifccParser::AffectationDeclarationTableContext *ctx)
{
    return (ifccParser::AffectationDeclarationTableContext *) ctx;
}

antlrcpp::Any VariableVisitor::visitExprTable(ifccParser::ExprTableContext *ctx)
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
    }
    this->visit(ctx->expr());
    return 0;
}

antlrcpp::Any VariableVisitor::visitExprAffectationTable(ifccParser::ExprAffectationTableContext *ctx)
{
    // cout << "Visiting affectation" << endl;
    int line = ctx->getStart()->getLine();
    int column = ctx->getStart()->getCharPositionInLine();

    string variableGaucheName = ctx->ID()->getText();

    if(_variables.find(variableGaucheName) == _variables.end())
    {
        _variableErrorsWarnings["Error : Unknown variable : " + to_string(line) + ":" + to_string(column) + " : Variable " + variableGaucheName + " not declared"] = ERROR;
    }
    

    // visiter la dernière valeur qui ne sera pas affectée
    this->visit(ctx->expr(0));
    this->visit(ctx->expr(1));
    return 0;
}
