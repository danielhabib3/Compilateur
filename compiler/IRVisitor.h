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
            _ast = ast;
            current_test = 0;
        };



        vector<CFG*> getCfgs() const {
            return _cfgs;
        }


        void setVariableErrorsWarnings(map<string, ErrorType> variableErrorsWarnings) {
            _variableErrorsWarnings = variableErrorsWarnings;
        }


        void setRootBlocks(vector<Block*> rootBlocks) {
            _rootBlocks = rootBlocks;
        }


        map<string, ErrorType> getVariableErrorsWarnings() {
            return _variableErrorsWarnings;
        }

        vector<int> getNextFreeLocations() const {
            return next_free_locations;
        }

        void setNextFreeLocations(vector<int> locations) {
            next_free_locations = locations;
        }
        


        // virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;
        virtual antlrcpp::Any visitExprAffectation(ifccParser::ExprAffectationContext *ctx) override ;
        virtual antlrcpp::Any visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx) override ;
        virtual antlrcpp::Any visitExprID(ifccParser::ExprIDContext *ctx) override ;
        virtual antlrcpp::Any visitExprChar(ifccParser::ExprCharContext *ctx) override ;
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
        // virtual antlrcpp::Any visitBreak(ifccParser::BreakContext *ctx) override;
        // virtual antlrcpp::Any visitContinue(ifccParser::ContinueContext *ctx) override;
        // virtual antlrcpp::Any visitSwitch_case(ifccParser::Switch_caseContext *ctx) override;
        virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext *ctx) override ;
        virtual antlrcpp::Any visitFunction_definition(ifccParser::Function_definitionContext *ctx) override ;


    protected:
        CFG* current_cfg; // current CFG
        vector<CFG*> _cfgs;
        antlr4::tree::ParseTree* _ast;
        // map<string, infosVariable> _variables;
        Block* currentBlock;
        Block* current_rootBlock;
        vector<Block*> _rootBlocks;
        map<string, ErrorType> _variableErrorsWarnings;
        int current_temp;
        int current_test;
        vector<int> next_free_locations;

};
