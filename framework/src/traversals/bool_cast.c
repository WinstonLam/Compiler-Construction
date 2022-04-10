/*****************************************************************************
 *
 * Module: bool_cast
 *
 * Prefix: BC
 *
 * Description:
 *
 * This module implements a traversal of the abstract syntax tree that
 * enables the conversion of casting booleans to other types and vice versa
 *
 *****************************************************************************/


#include "bool_cast.h"

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

node *BCcast (node *arg_node, info *arg_info)
{
  DBUG_ENTER("BEbinop");

    if (CAST_TYPE(arg_node) == T_bool) {
      if (NODE_TYPE(CAST_EXPR(arg_node)) == N_num ) {
          arg_node = TBmakeCondexpr(TBmakeBinop(BO_ne, CAST_EXPR(arg_node), TBmakeNum(0)), TBmakeBool(TRUE), TBmakeBool(FALSE));
          DBUG_RETURN( arg_node);
      }
      if (NODE_TYPE(CAST_EXPR(arg_node)) == N_float ) {
          arg_node = TBmakeCondexpr(TBmakeBinop(BO_ne, CAST_EXPR(arg_node), TBmakeNum(0)), TBmakeBool(TRUE), TBmakeBool(FALSE));
          DBUG_RETURN( arg_node);
      }
    }

  if (CAST_TYPE(arg_node) == T_int) {
    if (NODE_TYPE(CAST_EXPR(arg_node)) == N_bool) {
      arg_node = TBmakeCondexpr(TBmakeBinop(BO_ne,CAST_EXPR(arg_node), TBmakeBool(TRUE)) , TBmakeNum(1), TBmakeNum(0));
      DBUG_RETURN( arg_node);
    }
  }

  if (CAST_TYPE(arg_node) == T_float) {
    if (NODE_TYPE(CAST_EXPR(arg_node)) == N_bool) {
      arg_node = TBmakeCondexpr(TBmakeBinop(BO_ne,CAST_EXPR(arg_node), TBmakeBool(TRUE)) , TBmakeFloat(1.0), TBmakeFloat(0.0));
      DBUG_RETURN( arg_node);
    }
  }

  DBUG_RETURN( arg_node);
}

/*
 * Traversal start function
 */

node *BEdoBoolCast( node *syntaxtree)
{
  DBUG_ENTER("BEdoBoolCast");

  TRAVpush( TR_bc);
  syntaxtree = TRAVdo( syntaxtree, NULL);
  TRAVpop();

  DBUG_RETURN( syntaxtree);
}
