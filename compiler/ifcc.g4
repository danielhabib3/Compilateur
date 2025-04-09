grammar ifcc;

axiom : prog EOF ;

prog : type 'main' '(' ')' '{' block return_stmt '}' ;

block : (instruction)* ;

instruction : declaration | affectation ;

declaration : type affectationDeclaration (',' affectationDeclaration )* ';' ;

affectationDeclaration : ID ('=' expr)? ;

return_stmt: RETURN expr ';' ;

function_call : ID '(' (expr (',' expr)*)? ')' ;

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
     | ID '[' expr ']' '=' expr                             # exprAffectationTable
     | ID '=' expr                                          # exprAffectation
     | ID OP=('+=' | '-=') expr                             # exprAffectationComposee
     | ID OP=('++' | '--')                                  # exprPostfixIncDec
     | OP=('++' | '--') ID                                  # exprPrefixIncDec
     ;

      

type : 'int' ;

WHILE : 'while' ;
IF: 'if' ;
ELSE : 'else' ;
RETURN : 'return' ;
BREAK : 'break' ;
CONTINUE : 'continue' ;
SWITCH : 'switch' ;
CASE : 'case' ;
DEFAULT :'default' ;
ID : [a-zA-Z_][a-zA-Z_0-9]* ;

CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);