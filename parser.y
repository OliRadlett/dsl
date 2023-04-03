%{
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "ast.h"

Block *programBlock;

int yylex();
void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

%define parse.trace

%union {
	Node *node;
	Block *block;
	Expression *expression;
	Statement *statement;
	Identifier *identifier;
	VariableDefinition *variable_definition;
	std::string *string;
    int token;
}

%token <string> T_VARIABLE_DEFINITION T_DATATYPE T_SETUP_IDENTIFIER T_INTEGER T_IDENTIFIER
%token <token> T_OPEN_BRACKETS T_CLOSE_BRACKETS T_OPEN_SQUIGGLY  T_CLOSE_SQUIGGLY T_COLON T_COMMA T_EQUALS T_OPEN_SQUARE T_CLOSE_SQUARE

%type <identifier> identifier
%type <expression> numeric expression
%type <block> program statements block
%type <statement> statement variable_definition function_definition

%start program

%%

program: statements { programBlock = $1; };

statements: statement { $$ = new Block(); $$->statements.push_back($<statement>1); }
 			| statements statement { $1->statements.push_back($<statement>2); };

statement: variable_definition | function_definition;

variable_definition: T_VARIABLE_DEFINITION identifier { $$ = new VariableDefinition(*$2); }
					| T_VARIABLE_DEFINITION identifier T_EQUALS expression { $$ = new VariableDefinition(*$2); }

function_definition: identifier T_OPEN_BRACKETS T_CLOSE_BRACKETS block { $$ = new FunctionDefinition(*$1, *$4); }

identifier: T_IDENTIFIER { $$ = new Identifier(*$1); delete $1; }

numeric : T_INTEGER { $$ = new Integer(atol($1->c_str())); delete $1; }

expression: identifier { $<identifier>$ = $1; }
     | numeric

block : T_OPEN_SQUIGGLY statements T_CLOSE_SQUIGGLY { $$ = $2; }
      | T_OPEN_SQUIGGLY T_CLOSE_SQUIGGLY { $$ = new Block(); }
      ;


%%