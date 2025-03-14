
// Generated from ifcc.g4 by ANTLR 4.13.2


#include "ifccVisitor.h"

#include "ifccParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct IfccParserStaticData final {
  IfccParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  IfccParserStaticData(const IfccParserStaticData&) = delete;
  IfccParserStaticData(IfccParserStaticData&&) = delete;
  IfccParserStaticData& operator=(const IfccParserStaticData&) = delete;
  IfccParserStaticData& operator=(IfccParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag ifccParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<IfccParserStaticData> ifccParserStaticData = nullptr;

void ifccParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (ifccParserStaticData != nullptr) {
    return;
  }
#else
  assert(ifccParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<IfccParserStaticData>(
    std::vector<std::string>{
      "axiom", "prog", "block", "instruction", "declaration", "affectationDeclaration", 
      "affectation", "return_stmt", "value", "type"
    },
    std::vector<std::string>{
      "", "'main'", "'('", "')'", "'{'", "'}'", "','", "';'", "'='", "'int'", 
      "'return'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "RETURN", "ID", "CONST", "COMMENT", 
      "DIRECTIVE", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,15,72,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,7,
  	7,7,2,8,7,8,2,9,7,9,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	2,5,2,34,8,2,10,2,12,2,37,9,2,1,3,1,3,3,3,41,8,3,1,4,1,4,1,4,1,4,5,4,
  	47,8,4,10,4,12,4,50,9,4,1,4,1,4,1,5,1,5,1,5,3,5,57,8,5,1,6,1,6,1,6,1,
  	6,1,6,1,7,1,7,1,7,1,7,1,8,1,8,1,9,1,9,1,9,0,0,10,0,2,4,6,8,10,12,14,16,
  	18,0,1,1,0,11,12,65,0,20,1,0,0,0,2,23,1,0,0,0,4,35,1,0,0,0,6,40,1,0,0,
  	0,8,42,1,0,0,0,10,53,1,0,0,0,12,58,1,0,0,0,14,63,1,0,0,0,16,67,1,0,0,
  	0,18,69,1,0,0,0,20,21,3,2,1,0,21,22,5,0,0,1,22,1,1,0,0,0,23,24,3,18,9,
  	0,24,25,5,1,0,0,25,26,5,2,0,0,26,27,5,3,0,0,27,28,5,4,0,0,28,29,3,4,2,
  	0,29,30,3,14,7,0,30,31,5,5,0,0,31,3,1,0,0,0,32,34,3,6,3,0,33,32,1,0,0,
  	0,34,37,1,0,0,0,35,33,1,0,0,0,35,36,1,0,0,0,36,5,1,0,0,0,37,35,1,0,0,
  	0,38,41,3,8,4,0,39,41,3,12,6,0,40,38,1,0,0,0,40,39,1,0,0,0,41,7,1,0,0,
  	0,42,43,3,18,9,0,43,48,3,10,5,0,44,45,5,6,0,0,45,47,3,10,5,0,46,44,1,
  	0,0,0,47,50,1,0,0,0,48,46,1,0,0,0,48,49,1,0,0,0,49,51,1,0,0,0,50,48,1,
  	0,0,0,51,52,5,7,0,0,52,9,1,0,0,0,53,56,5,11,0,0,54,55,5,8,0,0,55,57,3,
  	16,8,0,56,54,1,0,0,0,56,57,1,0,0,0,57,11,1,0,0,0,58,59,5,11,0,0,59,60,
  	5,8,0,0,60,61,3,16,8,0,61,62,5,7,0,0,62,13,1,0,0,0,63,64,5,10,0,0,64,
  	65,3,16,8,0,65,66,5,7,0,0,66,15,1,0,0,0,67,68,7,0,0,0,68,17,1,0,0,0,69,
  	70,5,9,0,0,70,19,1,0,0,0,4,35,40,48,56
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  ifccParserStaticData = std::move(staticData);
}

}

ifccParser::ifccParser(TokenStream *input) : ifccParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

ifccParser::ifccParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  ifccParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *ifccParserStaticData->atn, ifccParserStaticData->decisionToDFA, ifccParserStaticData->sharedContextCache, options);
}

ifccParser::~ifccParser() {
  delete _interpreter;
}

const atn::ATN& ifccParser::getATN() const {
  return *ifccParserStaticData->atn;
}

std::string ifccParser::getGrammarFileName() const {
  return "ifcc.g4";
}

const std::vector<std::string>& ifccParser::getRuleNames() const {
  return ifccParserStaticData->ruleNames;
}

const dfa::Vocabulary& ifccParser::getVocabulary() const {
  return ifccParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView ifccParser::getSerializedATN() const {
  return ifccParserStaticData->serializedATN;
}


//----------------- AxiomContext ------------------------------------------------------------------

ifccParser::AxiomContext::AxiomContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ifccParser::ProgContext* ifccParser::AxiomContext::prog() {
  return getRuleContext<ifccParser::ProgContext>(0);
}

tree::TerminalNode* ifccParser::AxiomContext::EOF() {
  return getToken(ifccParser::EOF, 0);
}


size_t ifccParser::AxiomContext::getRuleIndex() const {
  return ifccParser::RuleAxiom;
}


std::any ifccParser::AxiomContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitAxiom(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::AxiomContext* ifccParser::axiom() {
  AxiomContext *_localctx = _tracker.createInstance<AxiomContext>(_ctx, getState());
  enterRule(_localctx, 0, ifccParser::RuleAxiom);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(20);
    prog();
    setState(21);
    match(ifccParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ProgContext ------------------------------------------------------------------

ifccParser::ProgContext::ProgContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ifccParser::TypeContext* ifccParser::ProgContext::type() {
  return getRuleContext<ifccParser::TypeContext>(0);
}

ifccParser::BlockContext* ifccParser::ProgContext::block() {
  return getRuleContext<ifccParser::BlockContext>(0);
}

ifccParser::Return_stmtContext* ifccParser::ProgContext::return_stmt() {
  return getRuleContext<ifccParser::Return_stmtContext>(0);
}


size_t ifccParser::ProgContext::getRuleIndex() const {
  return ifccParser::RuleProg;
}


std::any ifccParser::ProgContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitProg(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::ProgContext* ifccParser::prog() {
  ProgContext *_localctx = _tracker.createInstance<ProgContext>(_ctx, getState());
  enterRule(_localctx, 2, ifccParser::RuleProg);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(23);
    type();
    setState(24);
    match(ifccParser::T__0);
    setState(25);
    match(ifccParser::T__1);
    setState(26);
    match(ifccParser::T__2);
    setState(27);
    match(ifccParser::T__3);
    setState(28);
    block();
    setState(29);
    return_stmt();
    setState(30);
    match(ifccParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockContext ------------------------------------------------------------------

ifccParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<ifccParser::InstructionContext *> ifccParser::BlockContext::instruction() {
  return getRuleContexts<ifccParser::InstructionContext>();
}

ifccParser::InstructionContext* ifccParser::BlockContext::instruction(size_t i) {
  return getRuleContext<ifccParser::InstructionContext>(i);
}


size_t ifccParser::BlockContext::getRuleIndex() const {
  return ifccParser::RuleBlock;
}


std::any ifccParser::BlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitBlock(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::BlockContext* ifccParser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 4, ifccParser::RuleBlock);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(35);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == ifccParser::T__8

    || _la == ifccParser::ID) {
      setState(32);
      instruction();
      setState(37);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InstructionContext ------------------------------------------------------------------

ifccParser::InstructionContext::InstructionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ifccParser::DeclarationContext* ifccParser::InstructionContext::declaration() {
  return getRuleContext<ifccParser::DeclarationContext>(0);
}

ifccParser::AffectationContext* ifccParser::InstructionContext::affectation() {
  return getRuleContext<ifccParser::AffectationContext>(0);
}


size_t ifccParser::InstructionContext::getRuleIndex() const {
  return ifccParser::RuleInstruction;
}


std::any ifccParser::InstructionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitInstruction(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::InstructionContext* ifccParser::instruction() {
  InstructionContext *_localctx = _tracker.createInstance<InstructionContext>(_ctx, getState());
  enterRule(_localctx, 6, ifccParser::RuleInstruction);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(40);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case ifccParser::T__8: {
        enterOuterAlt(_localctx, 1);
        setState(38);
        declaration();
        break;
      }

      case ifccParser::ID: {
        enterOuterAlt(_localctx, 2);
        setState(39);
        affectation();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DeclarationContext ------------------------------------------------------------------

ifccParser::DeclarationContext::DeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ifccParser::TypeContext* ifccParser::DeclarationContext::type() {
  return getRuleContext<ifccParser::TypeContext>(0);
}

std::vector<ifccParser::AffectationDeclarationContext *> ifccParser::DeclarationContext::affectationDeclaration() {
  return getRuleContexts<ifccParser::AffectationDeclarationContext>();
}

ifccParser::AffectationDeclarationContext* ifccParser::DeclarationContext::affectationDeclaration(size_t i) {
  return getRuleContext<ifccParser::AffectationDeclarationContext>(i);
}


size_t ifccParser::DeclarationContext::getRuleIndex() const {
  return ifccParser::RuleDeclaration;
}


std::any ifccParser::DeclarationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitDeclaration(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::DeclarationContext* ifccParser::declaration() {
  DeclarationContext *_localctx = _tracker.createInstance<DeclarationContext>(_ctx, getState());
  enterRule(_localctx, 8, ifccParser::RuleDeclaration);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(42);
    type();
    setState(43);
    affectationDeclaration();
    setState(48);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == ifccParser::T__5) {
      setState(44);
      match(ifccParser::T__5);
      setState(45);
      affectationDeclaration();
      setState(50);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(51);
    match(ifccParser::T__6);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AffectationDeclarationContext ------------------------------------------------------------------

ifccParser::AffectationDeclarationContext::AffectationDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ifccParser::AffectationDeclarationContext::ID() {
  return getToken(ifccParser::ID, 0);
}

ifccParser::ValueContext* ifccParser::AffectationDeclarationContext::value() {
  return getRuleContext<ifccParser::ValueContext>(0);
}


size_t ifccParser::AffectationDeclarationContext::getRuleIndex() const {
  return ifccParser::RuleAffectationDeclaration;
}


std::any ifccParser::AffectationDeclarationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitAffectationDeclaration(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::AffectationDeclarationContext* ifccParser::affectationDeclaration() {
  AffectationDeclarationContext *_localctx = _tracker.createInstance<AffectationDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 10, ifccParser::RuleAffectationDeclaration);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(53);
    match(ifccParser::ID);
    setState(56);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ifccParser::T__7) {
      setState(54);
      match(ifccParser::T__7);
      setState(55);
      value();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AffectationContext ------------------------------------------------------------------

ifccParser::AffectationContext::AffectationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ifccParser::AffectationContext::ID() {
  return getToken(ifccParser::ID, 0);
}

ifccParser::ValueContext* ifccParser::AffectationContext::value() {
  return getRuleContext<ifccParser::ValueContext>(0);
}


size_t ifccParser::AffectationContext::getRuleIndex() const {
  return ifccParser::RuleAffectation;
}


std::any ifccParser::AffectationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitAffectation(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::AffectationContext* ifccParser::affectation() {
  AffectationContext *_localctx = _tracker.createInstance<AffectationContext>(_ctx, getState());
  enterRule(_localctx, 12, ifccParser::RuleAffectation);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(58);
    match(ifccParser::ID);
    setState(59);
    match(ifccParser::T__7);
    setState(60);
    value();
    setState(61);
    match(ifccParser::T__6);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Return_stmtContext ------------------------------------------------------------------

ifccParser::Return_stmtContext::Return_stmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ifccParser::Return_stmtContext::RETURN() {
  return getToken(ifccParser::RETURN, 0);
}

ifccParser::ValueContext* ifccParser::Return_stmtContext::value() {
  return getRuleContext<ifccParser::ValueContext>(0);
}


size_t ifccParser::Return_stmtContext::getRuleIndex() const {
  return ifccParser::RuleReturn_stmt;
}


std::any ifccParser::Return_stmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitReturn_stmt(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::Return_stmtContext* ifccParser::return_stmt() {
  Return_stmtContext *_localctx = _tracker.createInstance<Return_stmtContext>(_ctx, getState());
  enterRule(_localctx, 14, ifccParser::RuleReturn_stmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(63);
    match(ifccParser::RETURN);
    setState(64);
    value();
    setState(65);
    match(ifccParser::T__6);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ValueContext ------------------------------------------------------------------

ifccParser::ValueContext::ValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ifccParser::ValueContext::CONST() {
  return getToken(ifccParser::CONST, 0);
}

tree::TerminalNode* ifccParser::ValueContext::ID() {
  return getToken(ifccParser::ID, 0);
}


size_t ifccParser::ValueContext::getRuleIndex() const {
  return ifccParser::RuleValue;
}


std::any ifccParser::ValueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitValue(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::ValueContext* ifccParser::value() {
  ValueContext *_localctx = _tracker.createInstance<ValueContext>(_ctx, getState());
  enterRule(_localctx, 16, ifccParser::RuleValue);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(67);
    _la = _input->LA(1);
    if (!(_la == ifccParser::ID

    || _la == ifccParser::CONST)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypeContext ------------------------------------------------------------------

ifccParser::TypeContext::TypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t ifccParser::TypeContext::getRuleIndex() const {
  return ifccParser::RuleType;
}


std::any ifccParser::TypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitType(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::TypeContext* ifccParser::type() {
  TypeContext *_localctx = _tracker.createInstance<TypeContext>(_ctx, getState());
  enterRule(_localctx, 18, ifccParser::RuleType);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(69);
    match(ifccParser::T__8);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void ifccParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  ifccParserInitialize();
#else
  ::antlr4::internal::call_once(ifccParserOnceFlag, ifccParserInitialize);
#endif
}
