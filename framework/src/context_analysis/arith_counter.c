/*****************************************************************************
 *
 * Module: arith_counter
 *
 * Prefix: AC
 *
 * Description:
 *
 * This module implements a traversal of the abstract syntax tree that 
 * that counts the number of occurrences of each of the five arithmetic
 * operatorsand prints the result at the end of the traversal.
 *
 *****************************************************************************/


#include "arith_counter.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"

#include "memory.h"
#include "ctinfo.h"
#include "check.h"


/*
 * INFO structure
 */

struct INFO {
  int add;
  int sub;
  int mul;
  int div;
  int mod;
};


/*
 * INFO macros
 */

#define INFO_ADD(n)  ((n)->add)
#define INFO_SUB(n)  ((n)->sub)
#define INFO_MUL(n)  ((n)->mul)
#define INFO_DIV(n)  ((n)->div)
#define INFO_MOD(n)  ((n)->mod)

/*
 * INFO functions
 */

static info *MakeInfo(void)     // zou deze functie niet een struct moeten returnen?
{
  info *result;

  DBUG_ENTER( "MakeInfo");

  result = (info *)MEMmalloc(sizeof(info));

  INFO_ADD( result) = 0;
  INFO_SUB( result) = 0;
  INFO_MUL( result) = 0;
  INFO_DIV( result) = 0;
  INFO_MOD( result) = 0;

  DBUG_RETURN( result);     // moet ik hier een struct returnen?
}

static info *FreeInfo( info *info)
{
  DBUG_ENTER ("FreeInfo");

  info = MEMfree( info);

  DBUG_RETURN( info);
}


/*
 * Traversal functions
 */

node *ACbinop (node *arg_node, info *arg_info)
{
 
  DBUG_ENTER("ACbinop");

  

  if (CHKbinop( arg_node, arg_info)) {
  
      if (BINOP_OP( arg_node) == BO_add) {
        INFO_ADD(arg_info) += 1;       
    } else if (BINOP_OP( arg_node) == BO_sub) {
        INFO_SUB(arg_info) += 1; 
    } else if (BINOP_OP( arg_node) == BO_mul) {
        INFO_MUL(arg_info) += 1;
    } else if (BINOP_OP( arg_node) == BO_div) {
        INFO_DIV(arg_info) += 1;
    } else if (BINOP_OP( arg_node) == BO_mod) {
        INFO_MOD(arg_info) += 1;
    } 

  }
  DBUG_RETURN( arg_node);
}

/*
 * Traversal start function
 */

node *ACdoArithCounter( node *syntaxtree)
{
  info *arg_info;

  DBUG_ENTER("ACdoArithCounter");

  arg_info = MakeInfo();

  TRAVpush( TR_ac);
  syntaxtree = TRAVdo( syntaxtree, arg_info);
  TRAVpop();
  
  MODULE_ADD(syntaxtree) = INFO_ADD(arg_info);
  MODULE_SUB(syntaxtree) = INFO_SUB(arg_info);
  MODULE_MUL(syntaxtree) = INFO_MUL(arg_info);
  MODULE_DIV(syntaxtree) = INFO_DIV(arg_info);
  MODULE_MOD(syntaxtree) = INFO_MOD(arg_info); 
  
  arg_info = FreeInfo( arg_info);

  DBUG_RETURN( syntaxtree);
}
