#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"
#include "VariableVisitor.h"
#include "FunctionVisitor.h"
#include "IR.h"
#include "IRVisitor.h"


using namespace antlr4;
using namespace std;

int main(int argn, const char **argv)
{
  stringstream in;
  if (argn==2)
  {
     ifstream lecture(argv[1]);
     if( !lecture.good() )
     {
         cerr<<"error: cannot read file: " << argv[1] << endl ;
         exit(1);
     }
     in << lecture.rdbuf();
  }
  else
  {
      cerr << "usage: ifcc path/to/file.c" << endl ;
      exit(1);
  }
  
  ANTLRInputStream input(in.str());

  ifccLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  tokens.fill();

  ifccParser parser(&tokens);
  tree::ParseTree* tree = parser.axiom();

  if(parser.getNumberOfSyntaxErrors() != 0)
  {
    cerr << "error: syntax error during parsing" << endl;
    exit(1);
  }


  
  // FunctionVisitor fv;
  // fv.visit(tree);
  // fv.checkMainFunction();
  // map<string, FUNCTION_MESSAGE_TYPE> funcErrorsWarnings = fv.getFunctionMessages();
  // bool hasFunctionError = false;
  
  // for (const auto& [msg, type] : funcErrorsWarnings) {
  //     cerr << msg << endl;
  //     if (type == FUNC_ERROR) {
  //         hasFunctionError = true;
  //     }
  // }
  
  // if (hasFunctionError) {
  //     cerr << "error: syntax error during functions analysis" << endl;
  //     exit(1);
  // }
  

  VariableVisitor vv;
  vv.visit(tree);
  ofstream fichier_bloc("fichier_bloc.dot");
  vector<Block*> _rootBlocks = vv.getRootBlocks();
  fichier_bloc << "digraph G {\n";
  int funcNameCounter = 0;
  for(auto _rootBlock : _rootBlocks)
  {
    _rootBlock->affiche_bloc(fichier_bloc, "Function_" + to_string(funcNameCounter++));
  }
  fichier_bloc << "}\n";

  // cout << "Variable analysis done checking errors .." << endl;

  map<string, ErrorType> _variableErrorsWarnings = vv.getVariableErrorsWarnings();

  // // warnings pour les variables déclarées et pas utilisées
  // for(auto it = _rootBlock->begin(); it != _rootBlock->end(); it++)
  // {
  //   if(it->second.nbUse == 0)
  //   {
  //     string warning = "warning: variable declared ";
  //     if(it->second.set == 0)
  //       warning += "and not initialized ";
  //     else
  //       warning += "and initialized ";
  //     warning += "but not used: " + it->first + " at " + to_string(it->second.line) + ":" + to_string(it->second.column) + "\n";
  //     _variableErrorsWarnings[warning] = WARNING;
  //   }
  // }

  vv.setVariableErrorsWarnings(_variableErrorsWarnings);
  
  // afficher les erreurs et/ou les warnings
  for(auto it = _variableErrorsWarnings.begin(); it != _variableErrorsWarnings.end(); it++)
  {
    cerr << it->first << endl;
  }

  if(vv.countErrors() > 0)
  {
    cerr << "error: syntax error during variable analysis" << endl;
    exit(1);
  }
  else
  {
    // cout << "Variable analysis done without errors" << endl;
  }

  int nbErrorsWarnings = vv.getVariableErrorsWarnings().size();
  
  IRVisitor irv(tree);
  irv.setNextFreeLocations(vv.getNextFreeLocations());
  irv.setRootBlocks(vv.getRootBlocks());
  irv.setVariableErrorsWarnings(vv.getVariableErrorsWarnings());
  irv.visit(tree);

  _variableErrorsWarnings = irv.getVariableErrorsWarnings();
  // afficher les erreurs et/ou les warnings à partir de nbErrorsWarnings
  int currentIndex = 0;
  for(auto it = _variableErrorsWarnings.begin(); it != _variableErrorsWarnings.end(); it++, currentIndex++)
  {
    if(currentIndex >= nbErrorsWarnings)
    {
      cerr << it->first << endl;
    }
  }

  if(_variableErrorsWarnings.size() > nbErrorsWarnings)
  {
    cerr << "error: syntax error during IR generation" << endl;
    exit(1);
  }
  else
  {
    // cout << "IR generation done without errors" << endl;
  }

  vector<CFG*> cfgs = irv.getCfgs();
  for(int i = 0; i < cfgs.size(); i++)
  {
    cfgs[i]->set_next_free_location(vv.getNextFreeLocations()[i]);
  }
  ofstream fichier("fichier.dot");
  fichier << "digraph G {\n";
  for(auto cfg : cfgs)
  {
    cfg->affiche_cfg(fichier, cfg->get_entry_label());
  }
  fichier << "}\n";
  for(auto cfg : cfgs)
  {
    cfg->gen_asm(cout);
  }

  // cout << "IR generation done without errors" << endl;

  for(auto cfg : cfgs)
  {
    delete cfg;
  }

  for(auto _rootBlock : _rootBlocks)
  {
    delete _rootBlock;
  }

  return 0;
}
