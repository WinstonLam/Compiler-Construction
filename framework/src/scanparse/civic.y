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
fundefs: fundef fundefs
      {
        $$ = TBmakeFundefs($1, $2);
      }
      | fundef
      {
        $$ = $1;
      }
      ;

fundef: type ID BRACKET_L param BRACKET_R BRACES_L funbody BRACES_R
      {
        $$ = TBmakeFundef($1, STRcpy($2), $7, $4);
      }
      ;

funbody: vardecl fundefs stmts
      {
        $$ = TBmakeFunbody(NULL, $1, $2);
      }
      ;

vardecl: type ID LET expr
      {
        $$ = TBmakeVardecl(STRcpy($2), $1, NULL, $4, NULL); // Dims en Next zijn voor nu nog NULL
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

exprs: expr exprs
      {
        TBmakeExprs($1, $2);
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
      | expr binop expr
       {
         $$ = TBmakeBinop( $2, $1, $3);
       }
      | monop expr
       {
         $$ = TBmakeMonop($1, $2);
       }
      | BRACKET_L type BRACKET_R expr
      {
        $$ = TBmakeCast($2, $4);
      }
      | ID BRACKET_L BRACKET_R
      {
        $$ = TBmakeFuncall(STRcpy($1), NULL, NULL); // Wat moet decl hier zijn?
      }
      | ID BRACKET_L stmts BRACKET_R
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
          $$= $1;
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
        | ID BRACKET_L stmts BRACKET_R SEMICOLON
        {
          $$ = TBmakeExprstmt($3);
        }
        ;

ifelse: IF BRACKET_L expr BRACKET_R block
        {
          $$ = TBmakeIfelse($3, $5, NULL);
        }
        | IF BRACKET_L expr BRACKET_R block ELSE block
        {
          $$ = TBmakeIfelse($3, $5, $7);
        }
        ;

while: WHILE BRACKET_L expr BRACKET_R block
        {
          $$ = TBmakeWhile($3,$5);
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

varlet: constant
      {
         $$ = $1;     //Hoe moeten deze opgesteld worden wat zijn de decl en indices?
      }
      ;

block: BRACES_L stmts BRACES_R
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

 binop: PLUS      { $$ = BO_add; }
      | MINUS     { $$ = BO_sub; }
      | STAR      { $$ = BO_mul; }
      | SLASH     { $$ = BO_div; }
      | PERCENT   { $$ = BO_mod; }
      | LE        { $$ = BO_le; }
      | LT        { $$ = BO_lt; }
      | GE        { $$ = BO_ge; }
      | GT        { $$ = BO_gt; }
      | EQ        { $$ = BO_eq; }
      | OR        { $$ = BO_or; }
      | AND       { $$ = BO_and; }
      ;
monop: FACTORIAL  { $$ = MO_not; }
      | MINUS     { $$ = MO_neg; }
      ;
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

