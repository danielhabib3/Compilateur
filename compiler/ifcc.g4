grammar ifcc;

axiom : prog EOF ;

prog : type 'main' '(' ')' '{' block return_stmt '}' ;

block : (instruction)* ;

instruction : declaration | expr ';'| func_call ';' ;

declaration : type affectationDeclaration (',' affectationDeclaration )* ';' ;

functiondeclaration : type declarationfunction '(' (expr (',' expr)*)? ')' ((block) | ';') ;

affectationDeclaration : ID ('=' expr)? ;

affectation : ID '=' expr ';' ;

func_call : ID '(' (expr (',' expr)*)? ')' ;

return_stmt: RETURN expr ';' ;

expr : CONST                                                # exprConst
     | CHAR                                                 # exprChar
     | ID                                                   # exprID
     | '(' expr ')'                                         # exprParenthesis
     | OP=('-' | '!') expr                                  # exprUnary
     | expr OP=('*' | '/' | '%') expr                       # exprMulDivMod
     | expr OP=('+' | '-') expr                             # exprAddSub
     | expr OP=('<' | '>') expr                             # exprCompSupInf
     | expr OP=('!=' | '==') expr                           # exprCompEqual
     | expr '&' expr                                        # exprAndBit
     | expr '^' expr                                        # exprXorBit
     | expr '|' expr                                        # exprOrBit       
     | ID '=' expr                                          # exprAssign
    ;
      

type : 'int' ;


RETURN : 'return' ;
ID : [a-zA-Z_][a-zA-Z_0-9]* ;

CONST : [0-9]+ ;
CHAR : '\'' . '\'' ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
