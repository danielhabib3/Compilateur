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

typedef struct FunctionInfo {
    int line;
    int column;
    bool used = false;
    FunctionState state;
    std::string name;                          // Function name
    std::vector<std::string> paramNames;       // List of parameter names
    int paramCount = 0;
} infosFunction;

class FunctionVisitor : public ifccBaseVisitor {
public:
    // constructor
    FunctionVisitor() {
        _functions = {};
        _functionMessages = {};
    };

    // getter
    map<string, infosFunction> getFunctions() {
        return _functions;
    }

    map<string, FUNCTION_MESSAGE_TYPE> getFunctionMessages() {
        return _functionMessages;
    }

    int countErrors() {
        int count = 0;
        for(auto it = _functionMessages.begin(); it != _functionMessages.end(); it++)
        {
            if(it->second == FUNC_ERROR)
            {
                count++;
            }
        }
        return count;
    }

    virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext *ctx) override;
    virtual antlrcpp::Any visitFunction_declaration(ifccParser::Function_declarationContext *ctx) override;
    virtual antlrcpp::Any visitFunction_definition(ifccParser::Function_definitionContext *ctx) override;
    virtual antlrcpp::Any visitExprFunctionCall(ifccParser::ExprFunctionCallContext *ctx) override;   
    void checkMainFunction();

private:
    map<string, FunctionInfo> _functions;
    map<string, FUNCTION_MESSAGE_TYPE> _functionMessages;
};

#endif // FUNCTION_VISITOR_H
