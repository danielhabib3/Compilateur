#pragma once
#include "generated/ifccBaseVisitor.h"
#include "generated/ifccParser.h"
#include "IR.h"

class IRVisitor : public ifccBaseVisitor {
    public:
        IRVisitor() {
            _cfg = nullptr;
        };

        CFG* getCFG() {
            return _cfg;
        }

        void setCFG(CFG* cfg) {
            _cfg = cfg;
        }

        virtual antlrcpp::Any visitAxiom(ifccParser::AxiomContext *ctx) override;
    
    protected:
        CFG* _cfg;
};
