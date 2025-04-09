#include "FunctionVisitor.h"

antlrcpp::Any FunctionVisitor::visitFunction_declaration(ifccParser::Function_declarationContext *ctx) {
    string functionName = ctx->ID()->getText();
    int line = ctx->getStart()->getLine();
    int column = ctx->getStart()->getCharPositionInLine();

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
        _functions[functionName] = info;
    }

    return visitChildren(ctx);
}

antlrcpp::Any FunctionVisitor::visitFunction_definition(ifccParser::Function_definitionContext *ctx) {
    string functionName = ctx->ID()->getText();
    int line = ctx->getStart()->getLine();
    int column = ctx->getStart()->getCharPositionInLine();

    if (_functions.find(functionName) != _functions.end()) {
        if (_functions[functionName].state == DEFINED) {
            _functionMessages["Error : Multiple Definitions : " + to_string(line) + ":" + to_string(column) +
                              " : Function \"" + functionName + "\" already defined at " +
                              to_string(_functions[functionName].line) + ":" + to_string(_functions[functionName].column)] = FUNC_ERROR;
        } else {
            // It's okay to define a previously declared function
            _functions[functionName].line = line;
            _functions[functionName].column = column;
            _functions[functionName].state = DEFINED;
        }
    } else {
        FunctionInfo info;
        info.line = line;
        info.column = column;
        info.state = DEFINED;
        _functions[functionName] = info;
    }

    return visitChildren(ctx);
}

antlrcpp::Any FunctionVisitor::visitFunc_call(ifccParser::Func_callContext *ctx) {
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
