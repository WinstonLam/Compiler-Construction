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

node *ACnum (node *arg_node, info *arg_info)
{
  DBUG_ENTER("ACnum");

  if (BINOP_OP( arg_node) == BO_add) {
      INFO_ADD(arg_info) += 1;       // wat doet arg_info hier?
  } else if (BINOP_OP( arg_node) == BO_sub) {
      INFO_SUB(arg_info) += 1; 
  } else if (BINOP_OP( arg_node) == BO_mul) {
      INFO_MUL(arg_info) += 1;
  } else if (BINOP_OP( arg_node) == BO_div) {
      INFO_DIV(arg_info) += 1;
  } else if (BINOP_OP( arg_node) == BO_mod) {
      INFO_MOD(arg_info) += 1;
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

  TRAVpush( TR_si);
  syntaxtree = TRAVdo( syntaxtree, arg_info);
  TRAVpop();

  CTInote( "Number of addition artihmetics: %d\n", INFO_ADD( arg_info));
  CTInote( "Number of subtraction artihmetics: %d\n", INFO_SUB( arg_info));
  CTInote( "Number of multiplication artihmetics: %d\n", INFO_MUL( arg_info));
  CTInote( "Number of division artihmetics: %d\n", INFO_DIV( arg_info));
  CTInote( "Number of modulo artihmetics: %d", INFO_MOD( arg_info));

  arg_info = FreeInfo( arg_info);

  DBUG_RETURN( syntaxtree);
}
