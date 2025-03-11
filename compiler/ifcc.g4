grammar ifcc;

axiom : prog EOF ;

prog : type 'main' '(' ')' '{' block return_stmt '}' ;

block : instruction autre_instruction | ;

instruction : declaration | affectation ;

autre_instruction : instruction autre_instruction | ;

declaration : type ID autre_declaration ';' ;
autre_declaration : ',' ID autre_declaration | ;

affectation : ID '=' CONST ';' | ID '=' ID ';' ;

return_stmt: RETURN value ';' ;
value : CONST | ID ;

type : 'int' ;

ID : [a-zA-Z_][a-zA-Z_0-9]* ;
RETURN : 'return' ;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
