#pragma once
#include "generated/ifccBaseVisitor.h"
#include "generated/ifccParser.h"
#include "IR.h"

class IRVisitor : public ifccBaseVisitor {
    public:
        IRVisitor(tree::ParseTree* ast) {
            _cfg = CFG(ast);
        };

        CFG* getCFG() {
            return _cfg;
        }

        virtual antlrcpp::Any visitAxiom(ifccParser::AxiomContext *ctx) override;
    
    protected:
        CFG* _cfg;
};
