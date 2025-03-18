#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "VariableVisitor.h"


class  CodeGenVisitor : public ifccBaseVisitor {
	public:
        // constructor
        CodeGenVisitor() {
            current_temp = 0;
        };

        // getter
        map<string, infosVariable> getVariables() {
            return _variables;
        }

        // setter
        void setVariables(map<string, infosVariable> variables) {
            _variables = variables;
        }

        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override ;
        virtual antlrcpp::Any visitAffectation(ifccParser::AffectationContext *ctx) override ;
        virtual antlrcpp::Any visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx) override ;
        virtual antlrcpp::Any visitExprID(ifccParser::ExprIDContext *ctx) override ;
        virtual antlrcpp::Any visitExprConst(ifccParser::ExprConstContext *ctx) override ;
        virtual antlrcpp::Any visitExprAddSub(ifccParser::ExprAddSubContext *ctx) override ;
        virtual antlrcpp::Any visitExprMulDiv(ifccParser::ExprMulDivContext *ctx) override ;
        virtual antlrcpp::Any visitExprUnary(ifccParser::ExprUnaryContext *ctx) override ;
        
        private:
                map<string, infosVariable> _variables;
                int current_temp;

};

