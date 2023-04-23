%option noyywrap
%{
	#include <string>
	#include "ast.h"
	#include "parser.h"
	#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
	#define TOKEN(t) (yylval.token = t)
%}

%%
\n { yylineno++; }
[ \t\r\n]+ {}
"//" { while (1) { char c = yyinput(); if (c == '\n') break; else if (c == 0) { break; } } }
"var" { return TOKEN(T_VARIABLE_DEFINITION); }
"rank" { return TOKEN(T_DATATYPE); }
"suit" { return TOKEN(T_DATATYPE); }
"setup" { return TOKEN(T_SPECIAL_FUNCTION_SETUP); }
"lead" { return TOKEN(T_SPECIAL_FUNCTION_LEAD); }
"follow" { return TOKEN(T_SPECIAL_FUNCTION_FOLLOW); }
"score" { return TOKEN(T_SPECIAL_FUNCTION_SCORE); }
"print" { return TOKEN(T_LIBRARY_FUNCTION_PRINT); }
"all" { return TOKEN(T_LIBRARY_FUNCTION_ALL); }
"exists" { return TOKEN(T_LIBRARY_FUNCTION_EXISTS); }
"countif" { return TOKEN(T_LIBRARY_FUNCTION_COUNTIF); }
"true" { return TOKEN(T_TRUE); }
"false" { return TOKEN(T_FALSE); }
"if" { return TOKEN(T_IF); }
"else" { return TOKEN(T_ELSE); }
"return" { return TOKEN(T_RETURN); }
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
"->" { return TOKEN(T_ARROW); }
\"[^\"\n]*\" { SAVE_TOKEN; return T_STRING; }
[0-9]+ { SAVE_TOKEN; return T_INTEGER; }
[a-zA-Z]+ { SAVE_TOKEN; return T_IDENTIFIER; }

%%