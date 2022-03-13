%{


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

%token BRACKET_L BRACKET_R COMMA SEMICOLON
%token MINUS PLUS STAR SLASH PERCENT LE LT GE GT EQ NE OR AND
%token TRUEVAL FALSEVAL LET EXTERN TYPE_BOOL TYPE_INT TYPE_FLOAT TYPE_VOID TYPE
%token FACTORIAL EXPORT

%token <cint> NUM
%token <cflt> FLOAT
%token <id> ID

// type/basictype een goede waarde hebben (het zijn geen node pointers)

%type <ctype> type basictype
%type <node> program declarations declaration
%type <node> globdecl
%type <node> fundefs fundef param
%type <node> funbody 
%type <node> vardecl
%type <node> globdef
%type <node> constant expr
%type <node> intval boolval floatval
%type <node> stmts stmt //assign varlet
// %type <cbinop> binop
// %type <cmonop> monop


%%

program: declarations
        {
          TBmakeProgram($1);
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

globdecl: EXTERN type ID
      {
        $$ = TBmakeGlobdecl( $2, $3, NULL);
      }
      ;
fundefs: fundef fundefs
      {
        $$ = TBmakeFundefs($1, $2);
      }
      | fundef
      {
        $$ = TBmakeFundefs($1, NULL);
      }
fundef: type ID BRACKET_L funbody param BRACKET_R // funbody is op NULL gezet voor testen.
      {
        $$ = TBmakeFundef($1, STRcpy($2), $4, $5);
      }
      ;
funbody: vardecl fundefs stmts
      {
        $$ = TBmakeFunbody(NULL, $1, $2);
      }
vardecl: type ID LET expr
      {
        $$ = TBmakeVardecl(STRcpy($2), $1, NULL, $4, NULL); // wat moeten dims en next hier zijn?
      }
/* funheader: basictype ID param // zit fun header er uberhaupt in als je kijkt naar hoe TBmakefundef beschreven is
      {
        $$ = $1, $3;
      }
      ; */
param: type ID
      {
        $$ = TBmakeParam(STRcpy($2), $1, NULL, NULL); // wat moeten dims en next hier zijn?
      }
globdef: EXPORT type ID LET expr
      {
        $$ = TBmakeGlobdef($2, STRcpy($3), $5, NULL);
      }

type: basictype
      {
        $$ = $1;
      }
      ;

basictype: TYPE_BOOL
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
expr: constant
      {
         $$ = $1;
       }
     /* | varlet ID
       {
         $$ = TBmakeVar( STRcpy( $2), $1, 0);
       }
     | BRACKET_L expr binop expr BRACKET_R
       {
         $$ = TBmakeBinop( $3, $2, $4);
       }
     ; */

// fundef: type ID BRACKET_L BRACKET_R SEMICOLON {};

// vardef: type ID ';'
//       { // Generate error if type == T_void here or do it in type checking.
//       }
//     ;

 stmt: 
        {
          $$ = NULL;
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

// assign: varlet LET expr SEMICOLON
//         {
//           $$ = TBmakeAssign( $1, $3);
//         }
//         ;

// varlet: varlet ID indices
//         {
//           $$ = TBmakeVarlet( STRcpy( $2), $1, 0);
//         }
//         ;

// indices: NUM
//         {
//           $$ = TBmakeNum( $1);
//         }
//         ;




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

// binop: PLUS      { $$ = BO_add; }
//      | MINUS     { $$ = BO_sub; }
//      | STAR      { $$ = BO_mul; }
//      | SLASH     { $$ = BO_div; }
//      | PERCENT   { $$ = BO_mod; }
//      | LE        { $$ = BO_le; }
//      | LT        { $$ = BO_lt; }
//      | GE        { $$ = BO_ge; }
//      | GT        { $$ = BO_gt; }
//      | EQ        { $$ = BO_eq; }
//      | OR        { $$ = BO_or; }
//      | AND       { $$ = BO_and; }
//      ;

// monop: FACTORIAL { $$ = MO_fact; }
//      ;
%%
static int yyerror (char *error)
{
  CTIabort("line %d, col %d\nError parsing source code: %s\n",
            global.line, global.col, error);
  return( 0);
}

node *YYparseTree( void)
{
  DBUG_ENTER("YYparseTree");

  yyparse();

  DBUG_RETURN( parseresult);
}

