%option noyywrap
%{
	#include <string>
	#include "ast.h"
	#include "parser.h"
	#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
	#define TOKEN(t) (yylval.token = t)
%}

%%
[ \t\n];
"var" { return TOKEN(T_VARIABLE_DEFINITION); }
"rank" { return TOKEN(T_DATATYPE); }
"suit" { return TOKEN(T_DATATYPE); }
"setup" { return TOKEN(T_SETUP_IDENTIFIER); }
"if" { return TOKEN(T_IF); }
"else" { return TOKEN(T_ELSE); }
"==" { return TOKEN(T_CONDITION_EQUALS); }
"!=" { return TOKEN(T_CONDITION_NOT_EQUALS); }
"(" { return TOKEN(T_OPEN_BRACKETS); }
")" { return TOKEN(T_CLOSE_BRACKETS); }
"{" { return TOKEN(T_OPEN_SQUIGGLY); }
"}" { return TOKEN(T_CLOSE_SQUIGGLY); }
":" { return TOKEN(T_COLON); }
"," { return TOKEN(T_COMMA); }
"=" { return TOKEN(T_EQUALS); }
"[" { return TOKEN(T_OPEN_SQUARE); }
"]" { return TOKEN(T_CLOSE_SQUARE); }
[0-9]+ { SAVE_TOKEN; return T_INTEGER; }
[a-zA-Z]+ { SAVE_TOKEN; return T_IDENTIFIER; }

%%