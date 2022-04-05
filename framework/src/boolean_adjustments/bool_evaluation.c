/*****************************************************************************
 *
 * Module: bool_evaluation
 *
 * Prefix: BE
 *
 * Description:
 *
 * This module implements a traversal of the abstract syntax tree that
 * applies short circuit Boolean evaluation.
 *
 *****************************************************************************/


#include "bool_evaluation.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"

#include "memory.h"
#include "ctinfo.h"
#include "free.h"
#include "str.h"



/*
 * Traversal functions
 */

node *BEbinop (node *arg_node, info *arg_info)
{
  DBUG_ENTER("BEbinop");

  BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);
  BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);

  if (BINOP_OP(arg_node) == BO_or) {
    if (NODE_TYPE(BINOP_LEFT(arg_node)) == N_bool) {
        arg_node = TBmakeCondexpr(BINOP_LEFT(arg_node), BINOP_LEFT(arg_node), BINOP_RIGHT(arg_node));
    }
  }
  if (BINOP_OP(arg_node) == BO_and) {
     if (NODE_TYPE(BINOP_LEFT(arg_node)) == N_bool) {
        arg_node = TBmakeCondexpr(BINOP_LEFT(arg_node), BINOP_RIGHT(arg_node), BINOP_RIGHT(arg_node));
      }
    }


  DBUG_RETURN( arg_node);
}

/*
 * Traversal start function
 */

node *BEdoBoolEvaluation( node *syntaxtree)
{
  DBUG_ENTER("BEdoBoolEvaluation");

  TRAVpush( TR_be);
  syntaxtree = TRAVdo( syntaxtree, NULL);
  TRAVpop();

  DBUG_RETURN( syntaxtree);
}
