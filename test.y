%{
#include <stdio.h>
#include <stdlib.h>
#include "main.h"

int yylex();
void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

%union {
	int i_val;
	double d_val;
	char c_val;
	char* s_val;
	int il_val[100];
	int expression/* unsure*/
}

%token variable_definition
%token <s_val> identifier
%token datatype
%token colon
%token equals
%token comma
%token open_square
%token close_square
%token <i_val> integer
%token <c_val> character
%token setup_identifier
%token open_brackets
%token close_brackets
%token open_squiggly
%token close_squiggly

%type <expression> variable_def
%type <il_val> list

%%

program: statements;

statements: statement | statements statement;

statement: setup | variable_def;

setup: setup_identifier function {}

/*simplify and break down + params + change statements to only support expressions (not setup)*/
function: open_brackets close_brackets open_squiggly statements close_squiggly {
	//printf("FUNCTION\n");
}

list: open_square list_internals close_square

list_internals: integer | integer comma list_internals

// TODO make this better
variable_def: variable_definition identifier colon datatype equals integer {
	integer_init($2, $6);
} | variable_definition identifier colon datatype equals list {
	list_init($2, $6);
}

//identifier_and_type: identifier colon datatype;

/* TODO string rule */
//data: integer | character /* | list  */;

/* Not sure I will support the n-dimensional lists this rule allows for (this  also doesn't work) */
//list: open_square data close_square;

%%