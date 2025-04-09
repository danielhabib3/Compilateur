#pragma once
#include "generated/ifccBaseVisitor.h"
#include "generated/ifccParser.h"
#include "tree/ParseTree.h" // Ensure this header file defines tree::ParseTree
#include "IR.h"
#include <map>
#include <string>
#include "VariableVisitor.h"

class IRVisitor : public ifccBaseVisitor {
    public:
        IRVisitor(antlr4::tree::ParseTree* ast) {
            _cfg = new CFG();
            _ast = ast;
            
		    _cfg->add_bb(new BasicBlock(_cfg, "main", nullptr, nullptr));
            _cfg->current_bb = _cfg->get_bbs()[0];

            current_temp = 0;
            current_test = 0;

            currentBlock = nullptr;
        };

        ~IRVisitor() {
            delete _cfg;
        }

        CFG* getCFG() {
            return _cfg;
        }


        void setVariableErrorsWarnings(map<string, ErrorType> variableErrorsWarnings) {
            _variableErrorsWarnings = variableErrorsWarnings;
        }


        void setRootBlock(Block* rootBlock) {
            _rootBlock = rootBlock;
        }


        map<string, ErrorType> getVariableErrorsWarnings() {
            return _variableErrorsWarnings;
        }

        int getNextFreeLocation() const {
            return next_free_location;
        }

        void setNextFreeLocation(int location) {
            next_free_location = location;
        }

        // virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;
        virtual antlrcpp::Any visitExprAffectation(ifccParser::ExprAffectationContext *ctx) override ;
        virtual antlrcpp::Any visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx) override ;
        virtual antlrcpp::Any visitExprID(ifccParser::ExprIDContext *ctx) override ;
        virtual antlrcpp::Any visitExprConst(ifccParser::ExprConstContext *ctx) override ;
        virtual antlrcpp::Any visitExprAddSub(ifccParser::ExprAddSubContext *ctx) override ;
        virtual antlrcpp::Any visitExprMulDivMod(ifccParser::ExprMulDivModContext *ctx) override ;
        virtual antlrcpp::Any visitExprUnary(ifccParser::ExprUnaryContext *ctx) override ;
        virtual antlrcpp::Any visitExprCompSupInf(ifccParser::ExprCompSupInfContext *ctx) override ;
        virtual antlrcpp::Any visitExprCompEqual(ifccParser::ExprCompEqualContext *ctx) override ;
        virtual antlrcpp::Any visitExprAndBit(ifccParser::ExprAndBitContext *ctx) override ;
        virtual antlrcpp::Any visitExprOrBit(ifccParser::ExprOrBitContext *ctx) override ;
        virtual antlrcpp::Any visitExprXorBit(ifccParser::ExprXorBitContext *ctx) override ;
        virtual antlrcpp::Any visitTest(ifccParser::TestContext *ctx) override ;
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override ;
        virtual antlrcpp::Any visitBoucle_while(ifccParser::Boucle_whileContext *ctx) override ;
        virtual antlrcpp::Any visitAffectationDeclarationTable(ifccParser::AffectationDeclarationTableContext *ctx) override ;
        virtual antlrcpp::Any visitExprTable(ifccParser::ExprTableContext *ctx) override ;
        virtual antlrcpp::Any visitExprAffectationTable(ifccParser::ExprAffectationTableContext *ctx) override ;
        // virtual antlrcpp::Any visitExprPostfixIncDec(ifccParser::ExprPostfixIncDecContext *ctx) override ;
        // virtual antlrcpp::Any visitExprPrefixIncDec(ifccParser::ExprPrefixIncDecContext *ctx) override ;
        virtual antlrcpp::Any visitExprAffectationComposee(ifccParser::ExprAffectationComposeeContext *ctx) override ;
    
    protected:
        CFG* _cfg;
        antlr4::tree::ParseTree* _ast;
        // map<string, infosVariable> _variables;
        Block* currentBlock;
        Block* _rootBlock;
        map<string, ErrorType> _variableErrorsWarnings;
        int current_temp;
        int current_test;
        int next_free_location;
};
