#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "VariableVisitor.h"


class  CodeGenVisitor : public ifccBaseVisitor {
	public:
                // getter
                map<string, infosVariable> getVariables() {
                    return _variables;
                }

                // setter
                void setVariables(map<string, infosVariable> variables) {
                    _variables = variables;
                }

                virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override ;
                virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override ;
                virtual antlrcpp::Any visitAffectation(ifccParser::AffectationContext *ctx) override ;
        
        private:
                map<string, infosVariable> _variables;

};

