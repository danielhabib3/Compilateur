#pragma once
#include "generated/ifccBaseVisitor.h"
#include <map>
#include <string>
#include <list>

using namespace std;

enum Types { INT };

const string Etiquettes[] = { "INT" };

typedef struct Variable {
    int location;
    int type;
    int line;
    int column;
    int nbUse;
    int set;
} infosVariable;

extern map<string, infosVariable> _variables;
extern list<string> _variableErrors;

class VariableVisitor : public ifccBaseVisitor {
    public:
        virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override;
        virtual antlrcpp::Any visitAffectation(ifccParser::AffectationContext *ctx) override;
        virtual antlrcpp::Any visitValue(ifccParser::ValueContext *ctx) override;
    
        
};