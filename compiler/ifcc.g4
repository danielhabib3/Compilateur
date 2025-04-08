grammar ifcc;

axiom : prog EOF ;

prog : (function_definition | function_declaration)+ ;

function_definition : type ID '('  (type ID (',' type ID)*)? ')' block;

function_declaration : type ID '(' (type (ID)? (',' type (ID)?)*)? ')' ';' ;

block : '{' (instruction)* '}' ;

instruction : declaration | affectation | return_stmt | block | test | switch_case | boucle_while | break | continue | (expr ';') | ';' ;

switch_case : SWITCH '(' expr ')' '{' (CASE expr ':' (block | instruction)*)* (DEFAULT ':' (block | instruction)*)? '}' ;

test : IF '(' expr ')' block (ELSE block)? ;

boucle_while : WHILE '(' expr ')' block ;

break : (BREAK ';') ;

continue : (CONTINUE ';') ;

declaration : type affectationDeclaration (',' affectationDeclaration )* ';' ;

affectationDeclaration : ID ('=' expr)? ;

affectation : ID '=' expr ';' ;

return_stmt: RETURN expr ';' ;

function_call : ID '(' (expr (',' expr)*)? ')' ;

expr : CONST                                                # exprConst
     | ID                                                   # exprID
     | function_call                                        # exprFunctionCall
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
