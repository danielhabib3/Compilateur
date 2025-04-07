grammar ifcc;

axiom : prog EOF ;

prog : type 'main' '(' ')' block;

block : '{' (instruction)* '}' ;

instruction : declaration | affectation | return_stmt | block | test | boucle_while ;

test : IF '(' expr ')' block (ELSE block)? ;

boucle_while : WHILE '(' expr ')' block ;

declaration : type affectationDeclaration (',' affectationDeclaration )* ';' ;

affectationDeclaration : ID ('=' expr)? ;

affectation : ID '=' expr ';' ;

return_stmt: RETURN expr ';' ;

expr : CONST                                                # exprConst
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
    ;
      

type : 'int' ;

WHILE : 'while' ;
IF: 'if' ;
ELSE : 'else' ;
RETURN : 'return' ;
ID : [a-zA-Z_][a-zA-Z_0-9]* ;

CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
