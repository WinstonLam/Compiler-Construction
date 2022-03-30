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

    if (NODE_TYPE(CAST_EXPR(arg_node)) == T_int ||
        NODE_TYPE(CAST_EXPR(arg_node)) == T_float ) {
      if(CAST_EXPR(arg_node) >= 1) {
        arg_node = TBmakeBool(TRUE);
      } else {
        arg_node = TBmakeBool(FALSE);
      }
    }
  }

  if (CAST_TYPE(arg_node) == T_int) {
    if(CAST_EXPR(arg_node) == TRUE) {
      arg_node = TBmakeNum(1);
    } else {
      arg_node = TBmakeNum(0);
    }
  }

  if (CAST_TYPE(arg_node) == T_float){
    if(CAST_EXPR(arg_node) == TRUE) {
      arg_node = TBmakeFloat(1.0);
    } else {
      arg_node = TBmakeFloat(0.0);
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
