#include "FunctionVisitor.h"

antlrcpp::Any FunctionVisitor::visitFunction_declaration(ifccParser::Function_declarationContext *ctx) {
    string functionName = ctx->ID(0)->getText();
    int line = ctx->getStart()->getLine();
    int column = ctx->getStart()->getCharPositionInLine();
    vector<string> paramNames;
    for (size_t i = 1; i < ctx->ID().size(); ++i) {
        string paramName = ctx->ID(i) ? ctx->ID(i)->getText() : "param" + to_string(i);
        paramNames.push_back(paramName);
    }

    if (_functions.find(functionName) != _functions.end()) {
        // If already declared or defined, emit warning for multiple declarations
        _functionMessages["Warning : Multiple Declarations : " + to_string(line) + ":" + to_string(column) +
                          " : Function \"" + functionName + "\" already declared or defined at " +
                          to_string(_functions[functionName].line) + ":" + to_string(_functions[functionName].column)] = FUNC_WARNING;
    } else {
        FunctionInfo info;
        info.line = line;
        info.column = column;
        info.state = DECLARED;
        info.name = functionName;
        info.paramNames = paramNames;
        info.paramCount = paramNames.size();
        _functions[functionName] = info;
    }

    return visitChildren(ctx);
}

antlrcpp::Any FunctionVisitor::visitFunction_definition(ifccParser::Function_definitionContext *ctx) {
    string functionName = ctx->ID(0)->getText();
    int line = ctx->getStart()->getLine();
    int column = ctx->getStart()->getCharPositionInLine();
    vector<string> paramNames;
    for (size_t i = 1; i < ctx->ID().size(); ++i) {
        string paramName = ctx->ID(i) ? ctx->ID(i)->getText() : "param" + to_string(i);
        paramNames.push_back(paramName);
    }

    if (_functions.find(functionName) != _functions.end()) {
        if (_functions[functionName].state == DEFINED) {
            _functionMessages["Error : Multiple Definitions : " + to_string(line) + ":" + to_string(column) +
                              " : Function \"" + functionName + "\" already defined at " +
                              to_string(_functions[functionName].line) + ":" + to_string(_functions[functionName].column)] = FUNC_ERROR;
        } 
        else {
         // Définition d'une fonction déjà déclarée

        // Vérifie que le nombre de paramètres est identique
        if (_functions[functionName].paramCount != (int)paramNames.size()) {
            _functionMessages["Error : Signature mismatch : " + std::to_string(line) + ":" + std::to_string(column) + 
                " : Function \"" + functionName + "\" has different number of parameters than previous declaration at " +
                std::to_string(_functions[functionName].line) + ":" + std::to_string(_functions[functionName].column)] = FUNC_ERROR;
        }

        // Vérifie que les noms sont les mêmes (optionnel mais informatif)
        for (size_t i = 0; i < paramNames.size(); ++i) {
            if (_functions[functionName].paramNames[i] != paramNames[i]) {
                _functionMessages["Warning : Parameter name mismatch : " + std::to_string(line) + ":" + std::to_string(column) + 
                    " : Parameter " + std::to_string(i+1) + " of function \"" + functionName + "\" has different name than declaration (\"" + 
                    paramNames[i] + "\" vs \"" + _functions[functionName].paramNames[i] + "\")"] = FUNC_WARNING;
            }
        }
            _functions[functionName].line = line;
            _functions[functionName].column = column;
            _functions[functionName].state = DEFINED;
        }
    } 
    else {
        // Définition d'une fonction non déclarée
        FunctionInfo info;
        info.line = line;
        info.column = column;
        info.state = DEFINED;
        info.name = functionName;
        info.paramNames = paramNames;
        info.paramCount = paramNames.size();
        _functions[functionName] = info;
    }

    return visitChildren(ctx);
}

antlrcpp::Any FunctionVisitor::visitFunction_call(ifccParser::Function_callContext *ctx) {
    string functionName = ctx->ID()->getText();
    int line = ctx->getStart()->getLine();
    int column = ctx->getStart()->getCharPositionInLine();

    if (_functions.find(functionName) == _functions.end()) {
        _functionMessages["Error : Undeclared Function : " + to_string(line) + ":" + to_string(column) +
                          " : Function \"" + functionName + "\" called but not declared"] = FUNC_ERROR;
    } else {
        _functions[functionName].used = true;
    }

    return visitChildren(ctx);
}

void FunctionVisitor::checkMainFunction() {
    if (_functions.find("main") == _functions.end()) {
        _functionMessages["Error : Missing main function : Function \"main\" is not defined"] = FUNC_ERROR;
    } else if (_functions["main"].state != DEFINED) {
        _functionMessages["Error : Incomplete main function : Function \"main\" is declared but not defined"] = FUNC_ERROR;
    }
}
