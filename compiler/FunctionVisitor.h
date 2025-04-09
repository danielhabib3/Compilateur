#ifndef FUNCTION_VISITOR_H
#define FUNCTION_VISITOR_H

#include <map>
#include <string>
#include "generated/ifccBaseVisitor.h"

using namespace std;

enum FUNCTION_MESSAGE_TYPE {
    FUNC_ERROR,
    FUNC_WARNING
};

enum FunctionState {
    DECLARED,
    DEFINED
};

struct FunctionInfo {
    int line;
    int column;
    bool used = false;
    FunctionState state;
};

class FunctionVisitor : public ifccBaseVisitor {
public:
    virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext *ctx) override;
    virtual antlrcpp::Any visitFunction_declaration(ifccParser::Function_declarationContext *ctx) override;
    virtual antlrcpp::Any visitFunction_definition(ifccParser::Function_definitionContext *ctx) override;
    void checkMainFunction();

    map<string, FunctionInfo> _functions;
    map<string, FUNCTION_MESSAGE_TYPE> _functionMessages;
};

#endif // FUNCTION_VISITOR_H
