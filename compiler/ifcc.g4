grammar ifcc;

axiom : prog EOF ;

prog : type 'main' '(' ')' '{' block return_stmt '}' ;

block : (instruction)* ;

instruction : declaration | affectation ;

declaration : type affectationDeclaration (',' affectationDeclaration )* ';' ;

affectationDeclaration : ID ('=' expr)? ;

affectation : ID '=' expr ';' ;

return_stmt: RETURN expr ';' ;

expr : CONST                            # exprConst
     | ID                               # exprID
     | '(' expr ')'                     # exprParenthesis
     | expr OP=('*' | '/' | '%') expr   # exprMulDivMod
     | expr OP=('+' | '-') expr         # exprAddSub               
     | OP=('+' | '-') expr              # exprUnary
    ;
      

type : 'int' ;


RETURN : 'return' ;
ID : [a-zA-Z_][a-zA-Z_0-9]* ;

CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
