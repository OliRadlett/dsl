%option noyywrap
%{
	#include "parser.h"
%}

%%

"var" { return variable_definition; }
"rank" { return datatype; }
"suit" {return datatype; }
"setup" { return setup_identifier; }
"(" { return open_brackets; }
")" { return close_brackets; }
"{" { return open_squiggly; }
"}" { return close_squiggly; }
":" { return colon; }
"," { return comma; }
"=" { return equals; }
"[" { return open_square; }
"]" { return close_square; }
[0-9]+ { yylval.i_val = atoi(yytext); return integer; }
[a-zA-Z] { yylval.c_val = yytext[0]; return character; } //UNKNOWN
[a-zA-Z]+ { yylval.s_val = strdup(yytext); return identifier; }

%%