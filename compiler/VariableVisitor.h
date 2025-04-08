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
    int set;  // que pour les variables
    int size;  // que pour les tableaux
    bool isArray; // true si c'est un tableau, false sinon
    // Constructor
    Variable()
    {
        isArray = false;
    }
} infosVariable;



class VariableVisitor : public ifccBaseVisitor {
    public:
        // constructor
        VariableVisitor() {
            _variables = {};
            _variableErrorsWarnings = {};
            next_free_location = 1;
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

        int getNextFreeLocation() const {
            return next_free_location;
        }

        void setNextFreeLocation(int location) {
            next_free_location = location;
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
        virtual antlrcpp::Any visitExprAffectation(ifccParser::ExprAffectationContext *ctx) override;
        virtual antlrcpp::Any visitExprID(ifccParser::ExprIDContext *ctx) override;
        virtual antlrcpp::Any visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx) override;
        virtual antlrcpp::Any visitDeclarationTable(ifccParser::DeclarationTableContext *ctx) override ;
        virtual antlrcpp::Any visitAffectationDeclarationTable(ifccParser::AffectationDeclarationTableContext *ctx) override ;
        virtual antlrcpp::Any visitExprTable(ifccParser::ExprTableContext *ctx) override;
        virtual antlrcpp::Any visitExprAffectationTable(ifccParser::ExprAffectationTableContext *ctx) override;
    
        private:
            map<string, infosVariable> _variables;
            map<string, ErrorType> _variableErrorsWarnings;
            int next_free_location;
        
};