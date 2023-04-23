%{
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "ast.h"

#define YYDEBUG 1

Block *programBlock;

int yylex();
extern int yylineno;
void yyerror(const char *s) { printf("ERROR: %s on line %d\n", s, yylineno); }
%}

%define parse.error verbose
%define parse.trace

%union {
	Node *node;
	Block *block;
	Expression *expression;
	Statement *statement;
	Identifier *identifier;
	String *string_;
	Condition *condition;
	LambdaExpression *lambda_expression;
	LambdaArgs *lambda_args;
	Return *return_;
	VariableDefinition *variable_definition;
	std::vector<VariableDefinition*> *variable_definition_vector;
    	std::vector<Expression*> *expression_vector;
	List *list;
	Vector *vector;
	Integer *integer;
	std::string *string;
	int token;
}

%token <string> T_VARIABLE_DEFINITION T_DATATYPE T_INTEGER T_IDENTIFIER T_IF T_ELSE T_SPECIAL_FUNCTION_SETUP T_SPECIAL_FUNCTION_LEAD T_SPECIAL_FUNCTION_FOLLOW T_SPECIAL_FUNCTION_SCORE T_LIBRARY_FUNCTION_PRINT T_LIBRARY_FUNCTION_ALL T_LIBRARY_FUNCTION_EXISTS T_LIBRARY_FUNCTION_COUNTIF T_STRING T_RETURN
%token <token> T_OPEN_BRACKETS T_CLOSE_BRACKETS T_OPEN_SQUIGGLY  T_CLOSE_SQUIGGLY T_COLON T_COMMA T_EQUALS T_OPEN_SQUARE T_CLOSE_SQUARE T_CONDITION_EQUALS T_CONDITION_NOT_EQUALS T_ARROW

%type <identifier> identifier condition_identifier
%type <string_> string
%type <condition> condition
%type <expression> numeric expression special_function
%type <block> program statements block functionblock
%type <statement> statement variable_definition function_definition if library_function
%type <token> operator
%type <variable_definition_vector> function_definition_args
%type <expression_vector> function_call_args
%type <list> list
%type <vector> list_items
%type <integer> list_item
%type <lambda_args> lambda_args
%type <lambda_expression> lambda_expression
%type <return_> return_

%start program

%%

program: { if (yylineno == 1) { yyerror("empty input file"); } }
     	| statements { programBlock = $1; };

statements: statement { $$ = new Block(); $$->statements.push_back($<statement>1); }
 			| statements statement { $1->statements.push_back($<statement>2); };

statement: variable_definition | function_definition | if | library_function | expression { $$ = new ExpressionStatement(*$1); };

variable_definition: T_VARIABLE_DEFINITION identifier { $$ = new VariableDefinition(*$2); }
					| T_VARIABLE_DEFINITION identifier T_EQUALS expression { $$ = new VariableDefinition(*$2, $4); }

function_definition: identifier T_OPEN_BRACKETS T_CLOSE_BRACKETS functionblock { $$ = new FunctionDefinition(*$1, *$4); }
				| identifier T_OPEN_BRACKETS function_definition_args T_CLOSE_BRACKETS functionblock { $$ = new FunctionDefinition(*$1, *$3, *$5); delete $3; }

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

string: T_STRING { $$ = new String(*$1); delete $1; }

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
     | string { $<string_>$ = $1; }
     | special_function {}
     | identifier T_EQUALS identifier { $$ = new Assignment(*$1, $3); }
     | identifier T_EQUALS expression { $$ = new Assignment(*$1, $3); }
     | identifier T_OPEN_BRACKETS T_CLOSE_BRACKETS { $$ = new FunctionCall(*$1); }
     | identifier T_OPEN_BRACKETS function_call_args T_CLOSE_BRACKETS { $$ = new FunctionCall(*$1, *$3); delete $3;}

block : T_OPEN_SQUIGGLY statements T_CLOSE_SQUIGGLY { $$ = $2; }
      | T_OPEN_SQUIGGLY T_CLOSE_SQUIGGLY { $$ = new Block(); }

functionblock : T_OPEN_SQUIGGLY statements T_CLOSE_SQUIGGLY { $$ = $2; }
	 	| T_OPEN_SQUIGGLY statements return_ T_CLOSE_SQUIGGLY { $2->statements.push_back($<statement>3);  $$ = $2; }
      	| T_OPEN_SQUIGGLY T_CLOSE_SQUIGGLY { $$ = new Block(); }
      	| T_OPEN_SQUIGGLY return_ T_CLOSE_SQUIGGLY { $$ = new Block(); $$->statements.push_back($<statement>2); }


special_function: {}

library_function: T_LIBRARY_FUNCTION_PRINT T_OPEN_BRACKETS function_call_args T_CLOSE_BRACKETS { $$ = new LibraryFunction(0, *$3); }
			| T_LIBRARY_FUNCTION_COUNTIF T_OPEN_BRACKETS lambda_args T_CLOSE_BRACKETS { $$ = new LibraryFunction(1, $3); }
			| T_LIBRARY_FUNCTION_ALL T_OPEN_BRACKETS lambda_args T_CLOSE_BRACKETS { $$ = new LibraryFunction(2, $3); }
			| T_LIBRARY_FUNCTION_EXISTS T_OPEN_BRACKETS lambda_args T_CLOSE_BRACKETS { $$ = new LibraryFunction(3, $3); }


lambda_args: identifier T_COMMA lambda_expression { $$ = new LambdaArgs(*$1, $3); }

lambda_expression: identifier T_ARROW condition { $$ = new LambdaExpression(*$1, $3); };

return_: T_RETURN expression { $$ = new Return($2); }

%%