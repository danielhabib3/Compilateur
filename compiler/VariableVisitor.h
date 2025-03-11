#pragma once
#include "generated/ifccBaseVisitor.h"
#include <map>
#include <string>
#include <list>

using namespace std;

typedef struct Variable {
    int location;
    int type;
} infosVariable;

map<string, infosVariable> variables;
list<string> variableErrors;

class VariableVisitor : public ifccBaseVisitor {
    public:
        // constructor
        VariableVisitor() { 
            variables = new map<string, infosVariable>();
         };
        // destructor
        ~VariableVisitor() {
            delete variables;
        };

        virtual antlrcpp::Any visitDeclaration(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitAffectation(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitValue(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitAutre_declaration(ifccParser::ProgContext *ctx) override;
        
};