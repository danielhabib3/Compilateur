grammar ifcc;

axiom : prog EOF ;

prog : (function_definition | function_declaration)+ ;

function_definition : type ID '(' (type ID (',' type ID)*)? ')' block;

function_declaration : type ID '(' (type (ID)? (',' type (ID)?)*)? ')' ';' ;

block : '{' (instruction)* '}' ;

instruction : declaration | declarationTable | return_stmt | block | test | switch_case | boucle_while | break | continue | expr ';' | ';' ;

switch_case : SWITCH '(' expr ')' '{' (CASE expr ':' (block | instruction)*)* (DEFAULT ':' (block | instruction)*)? '}' ;

test : IF '(' expr ')' block (ELSE block)? ;

boucle_while : WHILE '(' expr ')' block ;

declarationTable : type affectationDeclarationTable (',' affectationDeclarationTable )* ';' ;

affectationDeclarationTable : ID'['CONST']' ('=' '{' (expr (',' expr)*)? '}')? ;

break : (BREAK ';') ;

continue : (CONTINUE ';') ;

declaration : type affectationDeclaration (',' affectationDeclaration )* ';' ;

affectationDeclaration : ID ('=' expr)? ;

return_stmt: RETURN expr ';' ;

function_call : ID '(' (expr (',' expr)*)? ')' ; 


expr : CONST                                                # exprConst
     | CHAR                                                 # exprChar
     | ID                                                   # exprID
     | OP=('++' | '--') ID                                  # exprPrefixIncDec                                             
     | ID '[' expr ']'                                      # exprTable
     | ID OP=('++' | '--')                                  # exprPostfixIncDec
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
     | ID '[' expr ']' '=' expr                             # exprAffectationTable
     | ID '=' expr                                          # exprAffectation
     | ID OP=('+=' | '-=') expr                             # exprAffectationComposee
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
CHAR : '\'' . '\'' ;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);