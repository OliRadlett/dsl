%{
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "ast.h"

#define YYDEBUG 1

Block *programBlock;

int yylex();
void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

%define parse.error verbose
%define parse.trace

%union {
	Node *node;
	Block *block;
	Expression *expression;
	Statement *statement;
	Identifier *identifier;
	Condition *condition;
	VariableDefinition *variable_definition;
	std::vector<VariableDefinition*> *variable_definition_vector;
    	std::vector<Expression*> *expression_vector;
	List *list;
	Vector *vector;
	Integer *integer;
	std::string *string;
	int token;
}

%token <string> T_VARIABLE_DEFINITION T_DATATYPE T_SETUP_IDENTIFIER T_INTEGER T_IDENTIFIER T_IF T_ELSE
%token <token> T_OPEN_BRACKETS T_CLOSE_BRACKETS T_OPEN_SQUIGGLY  T_CLOSE_SQUIGGLY T_COLON T_COMMA T_EQUALS T_OPEN_SQUARE T_CLOSE_SQUARE T_CONDITION_EQUALS T_CONDITION_NOT_EQUALS

%type <identifier> identifier condition_identifier
%type <condition> condition
%type <expression> numeric expression
%type <block> program statements block
%type <statement> statement variable_definition function_definition if
%type <token> operator
%type <variable_definition_vector> function_definition_args
%type <expression_vector> function_call_args
%type <list> list
%type <vector> list_items
%type <integer> list_item

%start program

%%

program: statements { programBlock = $1; };

statements: statement { $$ = new Block(); $$->statements.push_back($<statement>1); }
 			| statements statement { $1->statements.push_back($<statement>2); };

statement: variable_definition | function_definition | if | expression { $$ = new ExpressionStatement(*$1); };

variable_definition: T_VARIABLE_DEFINITION identifier { $$ = new VariableDefinition(*$2); }
					| T_VARIABLE_DEFINITION identifier T_EQUALS expression { $$ = new VariableDefinition(*$2, $4); }

function_definition: identifier T_OPEN_BRACKETS T_CLOSE_BRACKETS block { $$ = new FunctionDefinition(*$1, *$4); }
				| identifier T_OPEN_BRACKETS function_definition_args T_CLOSE_BRACKETS block { $$ = new FunctionDefinition(*$1, *$3, *$5); delete $3; }

// This is how lists were supposed to work. <variable_definition> casts it and thats why my one wasn't working
function_definition_args: { $$ = new VariableList(); }
			          | variable_definition { $$ = new VariableList(); $$->push_back($<variable_definition>1); }
			          | function_definition_args T_COMMA variable_definition { $1->push_back($<variable_definition>3); }

if: T_IF T_OPEN_BRACKETS condition T_CLOSE_BRACKETS block { $$ = new IfStatement($3, *$5); }

condition_identifier: identifier | numeric

condition: condition_identifier operator condition_identifier { $$ = new Condition(*$1, *$3, $2); }

operator: T_CONDITION_EQUALS { $$ = 0; } | T_CONDITION_NOT_EQUALS { $$ = 1; }

identifier: T_IDENTIFIER { $$ = new Identifier(*$1); delete $1; }

numeric: T_INTEGER { $$ = new Integer(atol($1->c_str())); delete $1; }


list: T_OPEN_SQUARE T_CLOSE_SQUARE { $$ = new List(); }
    | T_OPEN_SQUARE list_items T_CLOSE_SQUARE { $$ = new List($2); }

list_items: list_item { $$ = new Vector(*$1); }
         | list_items T_COMMA list_item { $$ = $1; $$ ->Push(*$3); }

// Make the compiler happy
// Might be able to replace this rule with the numeric type
list_item: T_INTEGER { $$ = new Integer(atol($1->c_str())); delete $1; }

function_call_args: { $$ = new ExpressionList(); }
				| expression { $$ = new ExpressionList(); $$->push_back($1); }
          		| function_call_args T_COMMA expression { $1->push_back($3); }

expression: identifier { $<identifier>$ = $1; }
     | numeric
     | list
     | identifier T_EQUALS identifier { $$ = new Assignment(*$1, $3); }
     | identifier T_EQUALS expression { $$ = new Assignment(*$1, $3); }
     | identifier T_OPEN_BRACKETS T_CLOSE_BRACKETS { $$ = new FunctionCall(*$1); }
     | identifier T_OPEN_BRACKETS function_call_args T_CLOSE_BRACKETS { $$ = new FunctionCall(*$1, *$3); delete $3;}

block : T_OPEN_SQUIGGLY statements T_CLOSE_SQUIGGLY { $$ = $2; }
      | T_OPEN_SQUIGGLY T_CLOSE_SQUIGGLY { $$ = new Block(); }


%%