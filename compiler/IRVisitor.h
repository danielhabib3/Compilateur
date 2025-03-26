#pragma once
#include "generated/ifccBaseVisitor.h"
#include "generated/ifccParser.h"
#include "IR.h"
#include <map>
#include <string>
#include "VariableVisitor.h"

class IRVisitor : public ifccBaseVisitor {
    public:
        IRVisitor(tree::ParseTree* ast) {
            _cfg = CFG();
            _ast = ast;
            
		    _cfg->add_bb(new BasicBlock(_cfg, "main", nullptr, nullptr, nullptr));
            _cfg->current_bb = _cfg->get_bbs()[0];

            current_temp = 0;
        };

        CFG* getCFG() {
            return _cfg;
        }

        void setVariables(map<string, infosVariable> variables) {
            _variables = variables;
        }

        // virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        // virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;
        virtual antlrcpp::Any visitExprAddSub(ifccParser::ExprAddSubContext *ctx) override ;
        virtual antlrcpp::Any visitExprMulDivMod(ifccParser::ExprMulDivModContext *ctx) override ;
        virtual antlrcpp::Any visitExprUnary(ifccParser::ExprUnaryContext *ctx) override ;
        virtual antlrcpp::Any visitExprCompSupInf(ifccParser::ExprCompSupInfContext *ctx) override ;
        virtual antlrcpp::Any visitExprCompEqual(ifccParser::ExprCompEqualContext *ctx) override ;
        virtual antlrcpp::Any visitExprAndBit(ifccParser::ExprAndBitContext *ctx) override ;
        virtual antlrcpp::Any visitExprOrBit(ifccParser::ExprOrBitContext *ctx) override ;
        virtual antlrcpp::Any visitExprXorBit(ifccParser::ExprXorBitContext *ctx) override ;
    
    protected:
        CFG* _cfg;
        tree::ParseTree* _ast;
        map<string, infosVariable> _variables;
        int current_temp;
};
