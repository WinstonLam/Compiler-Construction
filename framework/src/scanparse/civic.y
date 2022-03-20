%{
#define YYDEBUG 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include "types.h"
#include "tree_basic.h"
#include "str.h"
#include "dbug.h"
#include "ctinfo.h"
#include "free.h"
#include "globals.h"

static node *parseresult = NULL;
extern int yylex();
static int yyerror( char *errname);

%}

%union {
 nodetype            nodetype;
 char               *id;
 int                 cint;
 float               cflt;
 binop               cbinop;
 monop               cmonop;
 type                ctype;
 node               *node;
}

%token BRACKET_L BRACKET_R COMMA SEMICOLON BRACES_L BRACES_R
%token MINUS PLUS STAR SLASH PERCENT LE LT GE GT EQ NE OR AND
%token TRUEVAL FALSEVAL LET EXTERN TYPE_BOOL TYPE_INT TYPE_FLOAT TYPE_VOID TYPE
%token FACTORIAL EXPORT WHILE FOR IF RETURN ELSE DO

%token <cint> NUM
%token <cflt> FLOAT
%token <id> ID

%type <ctype> type
%type <node> program declarations declaration
%type <node> globdecl
%type <node> fundefs fundef param
%type <node> funbody
%type <node> vardecl
%type <node> globdef
%type <node> constant exprs expr
%type <node> intval boolval floatval
%type <node> stmts stmt
%type <node> block
%type <node> assign varlet for dowhile while return exprstmts ifelse

//assign varlet
%type <cbinop> binop
%type <cmonop> monop

%precedence CAST
%precedence THEN
%precedence ELSE

%left MINUS PLUS
%right UMINUS FACTORIAL
%left STAR SLASH PERCENT
%left LE LT GE GT
%left NE
%right EQ
%left OR AND


%start program

%%

program: declarations
        {
          parseresult = TBmakeProgram($1);
        }
        ;

declarations: declaration declarations
      {
        $$ = TBmakeDecls($1, $2);
      }
    | declaration
      {
        $$ = TBmakeDecls($1, NULL);
      }
      ;

declaration: globdecl
        {
          $$ = $1;
        }
        | fundef
        {
          $$ = $1;
        }
        | globdef
        {
          $$ = $1;
        }
        ;

globdecl: EXTERN type ID SEMICOLON
      {
        $$ = TBmakeGlobdecl( $2, STRcpy($3), NULL);
      }
      ;

fundef: type ID BRACKET_L param BRACKET_R BRACES_L funbody BRACES_R
      {
        $$ = TBmakeFundef($1, STRcpy($2), $7, $4);
      }
      | type ID BRACKET_L BRACKET_R BRACES_L funbody BRACES_R
      {
        $$ = TBmakeFundef($1, STRcpy($2), $6, NULL);
      }
      ;

funbody: vardecl stmts
      {
        $$ = TBmakeFunbody($1, NULL, $2);
      }
      | vardecl
      {
        $$ = TBmakeFunbody($1, NULL, NULL);
      }
      | stmts
      {
        $$ = TBmakeFunbody(NULL, NULL, $1);
      }
      |
      {
        $$ = TBmakeFunbody(NULL, NULL, NULL);
      }
      ;

vardecl: type ID LET expr SEMICOLON vardecl
      {
        $$ = TBmakeVardecl(STRcpy($2), $1, NULL, $4, $6); // Dims en Next zijn voor nu nog NULL
      }
      | type ID LET expr SEMICOLON
      {
        $$ = TBmakeVardecl(STRcpy($2), $1, NULL, $4, NULL); // Dims en Next zijn voor nu nog NULL
      }
      | type ID SEMICOLON
      {
        $$ = TBmakeVardecl(STRcpy($2), $1, NULL, NULL, NULL); // Dims en Next zijn voor nu nog NULL
      }
      ;

param: type ID
      {
        $$ = TBmakeParam(STRcpy($2), $1, NULL, NULL); // Dims en Next zijn voor nu nog NULL
      }
      ;

globdef: EXPORT type ID LET expr
      {
        $$ = TBmakeGlobdef($2, STRcpy($3), $5, NULL);
      }

type: TYPE_BOOL
        {
          $$ = T_bool;
        }
      | TYPE_INT
        {
          $$ = T_int;
        }
      | TYPE_FLOAT
        {
          $$ = T_float;
        }
      | TYPE_VOID
        {
          $$ = T_void;
        }
        ;

constant: intval
           {
             $$ = $1;
           }
         | boolval
           {
             $$ = $1;
           }
         | floatval
           {
             $$ = $1;
           }
         ;

exprs: expr COMMA exprs
      {
        TBmakeExprs($1, $3);
      }
      | expr
      {
        TBmakeExprs($1, NULL);
      }
      ;

expr: constant
      {
         $$ = $1;
      }
      | expr PLUS expr
       {
         $$ = TBmakeBinop( BO_add, $1, $3);
       }
      | expr MINUS expr
       {
         $$ = TBmakeBinop( BO_sub, $1, $3);
       }
      | expr STAR expr
       {
         $$ = TBmakeBinop( BO_mul, $1, $3);
       }
       | expr SLASH expr
       {
         $$ = TBmakeBinop( BO_div, $1, $3);
       }
       | expr PERCENT expr
       {
         $$ = TBmakeBinop( BO_mod, $1, $3);
       }
       | expr LE expr
       {
         $$ = TBmakeBinop( BO_le, $1, $3);
       }
       | expr LT expr
       {
         $$ = TBmakeBinop( BO_lt, $1, $3);
       }
       | expr GE expr
       {
         $$ = TBmakeBinop( BO_ge, $1, $3);
       }
       | expr GT expr
       {
         $$ = TBmakeBinop( BO_gt, $1, $3);
       }
       | expr EQ expr
       {
         $$ = TBmakeBinop( BO_eq, $1, $3);
       }
       | expr NE expr
       {
         $$ = TBmakeBinop( BO_ne, $1, $3);
       }
       | expr OR expr
       {
         $$ = TBmakeBinop( BO_or, $1, $3);
       }
       | expr AND expr
       {
         $$ = TBmakeBinop( BO_and, $1, $3);
       }
       | FACTORIAL expr
       {
         $$ = TBmakeMonop( MO_not, $2);
       }
       | MINUS expr %prec UMINUS
       {
         $$ = TBmakeMonop( MO_neg, $2);
       }
      | BRACKET_L type BRACKET_R expr %prec CAST
      {
        $$ = TBmakeCast($2, $4);
      }
      | ID BRACKET_L BRACKET_R
      {
        $$ = TBmakeFuncall(STRcpy($1), NULL, NULL); // Wat moet decl hier zijn?
      }
      | ID BRACKET_L exprs BRACKET_R
      {
        $$ = TBmakeFuncall(STRcpy($1), NULL, $3);
      }
      | ID
      {
        $$ = TBmakeVar(STRcpy($1), NULL, NULL); //TBmakeVar wat moeten hier de decl en indices zijn?
      }
     ;

 stmt: assign
        {
           $$ = $1;
        }
        | exprstmts
        {
          $$ = $1;
        }
        | ifelse
        {
          $$ = $1;
        }
        | while
        {
          $$ = $1;
        }
        | dowhile
        {
          $$ = $1;
        }
        | for
        {
          $$ = $1;
        }
        | return
        {
          $$ = $1;
        }
        ;

assign: varlet LET expr SEMICOLON
      {
        $$ = TBmakeAssign($1, $3);
      }
      ;

exprstmts: ID BRACKET_L BRACKET_R SEMICOLON
        {
          $$ = TBmakeExprstmt(NULL);
        }
        | ID BRACKET_L expr BRACKET_R SEMICOLON
        {
          $$ = TBmakeExprstmt($3);
        }
        | ID BRACKET_L exprs BRACKET_R SEMICOLON
        {
          $$ = TBmakeExprstmt($3);
        }
        ;

ifelse: IF BRACKET_L expr BRACKET_R block ELSE block
        {
          $$ = TBmakeIfelse($3, $5, $7);
        }
        |IF BRACKET_L expr BRACKET_R block %prec THEN
        {
          $$ = TBmakeIfelse($3, $5, NULL);
        }
        ;

while: WHILE BRACKET_L expr BRACKET_R block
        {
          $$ = TBmakeWhile($3, $5);
        }
        ;

dowhile: DO block WHILE BRACKET_L expr BRACKET_R SEMICOLON
        {
          $$ = TBmakeDowhile($5, $2);
        }
        ;

for: FOR BRACKET_L TYPE_INT ID LET expr COMMA expr BRACKET_R block
        {
          $$ =  TBmakeFor($4, $6, $8, NULL, $10);
        }
        | FOR BRACKET_L TYPE_INT ID LET expr COMMA expr COMMA expr BRACKET_R block
        {
          $$ = TBmakeFor($4, $6, $8, $10, $12);
        }
        ;

return: RETURN SEMICOLON
        {
          $$ = TBmakeReturn(NULL);
        }
        | RETURN expr SEMICOLON
        {
          $$ = TBmakeReturn($2);
        }
        ;

varlet: ID
      {
         $$ = TBmakeVarlet(STRcpy($1), NULL, NULL);     //Hoe moeten deze opgesteld worden wat zijn de decl en indices?
      }
      ;

block: BRACES_L stmts BRACES_R
      {
        $$ = $2;
      }
      | BRACES_L stmt BRACES_R
      {
        $$ = $2;
      }
      | stmt
      {
        $$ = $1;
      }
      ;

stmts: stmt stmts
         {
           $$ = TBmakeStmts( $1, $2);
         }
       | stmt
         {
           $$ = TBmakeStmts( $1, NULL);
         }
      |
        {
          $$ = NULL;
        }
         ;

 floatval: FLOAT
            {
              $$ = TBmakeFloat( $1);
            }
          ;

 intval: NUM
         {
           $$ = TBmakeNum( $1);
         }
       ;

 boolval: TRUEVAL
          {
            $$ = TBmakeBool( TRUE);
          }
        | FALSEVAL
          {
            $$ = TBmakeBool( FALSE);
          }
        ;

// Extension work
// fundefs: fundef fundefs
//       {
//         $$ = TBmakeFundefs($1, $2);
//       }
//       | fundef
//       {
//         $$ = $1;
//       }
//       ;

%%
static int yyerror (char *error)
{
  CTIabort("line %d, col %d\nError parsing source code: %s\n",
            global.line, global.col, error);
  return( 0);
}

node *YYparseTree( void)
{
  yydebug = 1;
  DBUG_ENTER("YYparseTree");

  yyparse();

  DBUG_RETURN( parseresult);
}

