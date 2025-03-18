#pragma once
#include "generated/ifccBaseVisitor.h"
#include "generated/ifccParser.h"
#include <map>
#include <string>
#include <list>

using namespace std;

enum Types { INT };

enum ErrorType { WARNING, ERROR };

const string Etiquettes[] = { "INT" };

typedef struct Variable {
    int location;
    int type;
    int line;
    int column;
    int nbUse;
    int set;
} infosVariable;



class VariableVisitor : public ifccBaseVisitor {
    public:
        // constructor
        VariableVisitor() {
            _variables = {};
            _variableErrorsWarnings = {};
        };

        // getter
        map<string, infosVariable> getVariables() {
            return _variables;
        }

        map<string, ErrorType> getVariableErrorsWarnings() {
            return _variableErrorsWarnings;
        }

        // setter
        void setVariables(map<string, infosVariable> variables) {
            _variables = variables;
        }
        
        void setVariableErrorsWarnings(map<string, ErrorType> variableErrors) {
            _variableErrorsWarnings = variableErrors;
        }

        int countErrors() {
            int count = 0;
            for(auto it = _variableErrorsWarnings.begin(); it != _variableErrorsWarnings.end(); it++)
            {
                if(it->second == ERROR)
                {
                    count++;
                }
            }
            return count;
        }

        virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override;
        virtual antlrcpp::Any visitAffectation(ifccParser::AffectationContext *ctx) override;
        virtual antlrcpp::Any visitExprID(ifccParser::ExprIDContext *ctx) override;
        virtual antlrcpp::Any visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx) override;
    
        private:
            map<string, infosVariable> _variables;
            map<string, ErrorType> _variableErrorsWarnings;
        
};