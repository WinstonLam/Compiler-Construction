/*****************************************************************************
 *
 * Module: type_checker
 *
 * Prefix: TC
 *
 * Description:
 *
 * This module implements a type checker for the semantic analysis of the compiler.
 * It will traverse down the syntax tree and store the corresponding types to each symbol.
 *
 *****************************************************************************/



#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "type_checker.h"

#include "memory.h"

node *TCfundef(node* arg_node, info *arg_info)
{
  DBUG_ENTER("TCfundef");

  // Traverse into the funbody
  TRAVopt(FUNDEF_FUNBODY(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}

node *TCfunbody(node* arg_node, info *arg_info)
{
  DBUG_ENTER("TCfunbody");

  switch (FUNDEF_TYPE(arg_node))
  {
    case T_assi:
      /* code */
      break;

    default:
      break;
  }
}

/*
 * Traversal start function
 */

node *TCdoTypeChecking( node *syntaxtree)
{
  DBUG_ENTER("CAdoContextAnalysis");

  info *arg_info;

  TRAVpush( TR_ca);   // Push traversal "ca" as defined in ast.xml

  syntaxtree = TRAVdo( syntaxtree, NULL);   // Initiate ast traversal

  TRAVpop();          // Pop current traversal

  DBUG_RETURN( syntaxtree);
}
