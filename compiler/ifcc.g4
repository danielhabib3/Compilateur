grammar ifcc;

axiom : prog EOF ;

prog : type 'main' '(' ')' '{' block return_stmt '}' ;

block : (instruction)* ;

instruction : declaration | affectation ;

declaration : type affectationDeclaration (',' affectationDeclaration )* ';' ;

affectationDeclaration : ID ('=' value)? ;

affectation : ID '=' value ('=' value)* ';' ;

return_stmt: RETURN value ';' ;
value : CONST | ID ;

type : 'int' ;


RETURN : 'return' ;
ID : [a-zA-Z_][a-zA-Z_0-9]* ;

CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
