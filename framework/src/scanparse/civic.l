%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "node_basic.h"
#include "str.h"
#include "free.h"
#include "civic.tab.h"
#include "globals.h"
#include "ctinfo.h"


#define FILTER(token) \
  global.col += yyleng;  \
  return( token);

#define LINEBUF_SIZE 256
#define YY_NO_UNPUT
#define YY_NO_INPUT


%}

%p 6000 /* set positions to 6000 */
%e 2000 /* set nodes to 1100 */
%option noyywrap
%option nounput



%%

"("                        { FILTER( BRACKET_L); }
")"                        { FILTER( BRACKET_R); }
"{"                        { FILTER( BRACES_L); }
"}"                        { FILTER( BRACES_R); }
","                        { FILTER( COMMA); }
";"                        { FILTER( SEMICOLON); }

"-"                        { FILTER( MINUS); }
"+"                        { FILTER( PLUS);  }
"*"                        { FILTER( STAR);  }
"/"                        { FILTER( SLASH);  }
"%"                        { FILTER( PERCENT);  }

"!"                        { FILTER( FACTORIAL); }

"<="                       { FILTER( LE);    }
"<"                        { FILTER( LT);    }
">="                       { FILTER( GE);    }
">"                        { FILTER( GT);    }
"=="                       { FILTER( EQ);    }
"!="                       { FILTER( NE);    }
"&&"                       { FILTER( AND);    }
"||"                       { FILTER( OR);    }

"="                        { FILTER( LET);    }

"true"                     { FILTER( TRUEVAL); }
"false"                    { FILTER( FALSEVAL); }

"extern"                   { FILTER( EXTERN); }
"bool"                     { FILTER( TYPE_BOOL); }
"int"                      { FILTER( TYPE_INT); }
"float"                    { FILTER( TYPE_FLOAT); }
"void"                     { FILTER( TYPE_VOID); }

"export"                   { FILTER( EXPORT); }
"while"                    { FILTER( WHILE); }
"for"                      { FILTER( FOR); }
"do"                       { FILTER( DO); }
"return"                   { FILTER( RETURN); }
"if"                       { FILTER( IF); }
"else"                      { FILTER( ELSE); }


[A-Za-z][A-Za-z0-9_]*      { yylval.id = STRcpy(yytext);
                             FILTER( ID);
                           }

[0-9]+                     { if (strtol(yytext, 0, 10) > 2147483649) {
                              CTIerror("Integer %s out of range", yytext);
                           }
                             yylval.cint=atoi(yytext);
                             FILTER( NUM);
                           }

[0-9]+\.[0-9]+([eE][+-][0-9]+)?  { yylval.cflt = atof(yytext);
                                   FILTER( FLOAT);
                                  }

\n*                       { global.line += 1;
                             global.col = 0;
                             yyless(1);
                           }

"//".*                                    { /* none */ } 
"/*"((\*+[^/*])|([^*]))*\**"*/"           { /* none */ }
[ \t\n]+                                  { /* none */ }


[ \t]                      { global.col += yyleng;
                           }


.                          {
                            CTIerror("Unknown token %s found\n", STRcpy(yytext));
                            }
