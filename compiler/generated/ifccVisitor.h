
// Generated from ifcc.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "ifccParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by ifccParser.
 */
class  ifccVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by ifccParser.
   */
    virtual std::any visitAxiom(ifccParser::AxiomContext *context) = 0;

    virtual std::any visitProg(ifccParser::ProgContext *context) = 0;

    virtual std::any visitBlock(ifccParser::BlockContext *context) = 0;

    virtual std::any visitInstruction(ifccParser::InstructionContext *context) = 0;

    virtual std::any visitDeclaration(ifccParser::DeclarationContext *context) = 0;

    virtual std::any visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *context) = 0;

    virtual std::any visitAffectation(ifccParser::AffectationContext *context) = 0;

    virtual std::any visitReturn_stmt(ifccParser::Return_stmtContext *context) = 0;

    virtual std::any visitValue(ifccParser::ValueContext *context) = 0;

    virtual std::any visitType(ifccParser::TypeContext *context) = 0;


};

