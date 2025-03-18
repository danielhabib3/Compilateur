#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"
#include "VariableVisitor.h"

#include "CodeGenVisitor.h"

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

  VariableVisitor vv;
  vv.visit(tree);

  // cout << "Variable analysis done checking errors .." << endl;

  map<string, infosVariable> _variables = vv.getVariables();
  map<string, ErrorType> _variableErrorsWarnings = vv.getVariableErrorsWarnings();

  // warnings pour les variables déclarées et pas utilisées
  for(auto it = _variables.begin(); it != _variables.end(); it++)
  {
    if(it->second.nbUse == 0)
    {
      string warning = "warning: variable declared ";
      if(it->second.set == 0)
        warning += "and not initialized ";
      else
        warning += "and initialized ";
      warning += "but not used: " + it->first + " at " + to_string(it->second.line) + ":" + to_string(it->second.column) + "\n";
      _variableErrorsWarnings[warning] = WARNING;
    }
  }

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

  
  CodeGenVisitor cgv;
  cgv.setVariables(vv.getVariables());
  cgv.visit(tree);

  return 0;
}
