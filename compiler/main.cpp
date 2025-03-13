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

  // warnings pour les variables déclarées et pas utilisées
  for(auto it = _variables.begin(); it != _variables.end(); it++)
  {
    if(it->second.nbUse == 0)
    {
      cerr << "warning: variable declared ";
      if(it->second.set == 0)
        cerr << "and not initialized ";
      else
        cerr << "and initialized ";
      cerr << "but not used: " << it->first << " at " << it->second.line << ":" << it->second.column << endl;
    }
  }
  

  if(_variableErrors.size() > 0)
  {
    cerr << "error: syntax error during variable analysis" << endl;
    exit(1);
  }
  
  CodeGenVisitor cgv;
  cgv.visit(tree);

  return 0;
}
