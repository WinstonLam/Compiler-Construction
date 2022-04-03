/*****************************************************************************
 *
 * Module: for_optimsation
 *
 * Prefix: FO
 *
 * Description:
 *
 * This module implements a traversal of the abstract syntax tree that
 * optimises the for loops by replacing the for loop with a while loop.
 *
 *****************************************************************************/


#include "for_optimisation.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "copy.h"
#include "memory.h"
#include "ctinfo.h"
#include "free.h"
#include "str.h"


/*
 * INFO structure
 */

struct INFO {
  node *scope;
};

/*
 * INFO macros
 */

#define INFO_SCOPE(n)  ((n)->scope)


/*
 * INFO functions
 */

static info *MakeInfo(void)
{
  info *result;

  DBUG_ENTER( "MakeInfo");

  result = (info *)MEMmalloc(sizeof(info));

  INFO_SCOPE( result) = NULL;

  DBUG_RETURN( result);
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

node *FOprogram (node *arg_node, info *arg_info)
{
  DBUG_ENTER("FOprogram");

  // set global scope symbol table to info struct
  INFO_SCOPE(arg_info) = PROGRAM_DECLS(arg_node);
  TRAVdo(PROGRAM_DECLS(arg_node),arg_info);

  DBUG_RETURN( arg_node);
}

node *FOfundef (node *arg_node, info *arg_info)
{
    DBUG_ENTER("FOfundef");

    // set the scope level to that of the fundef for one scope deeper.
    INFO_SCOPE(arg_info) = FUNBODY_STMTS(FUNDEF_FUNBODY(arg_node));

    // traverse into the funbody to search for for loops
    TRAVopt(FUNDEF_FUNBODY(arg_node),arg_info);
    
    DBUG_RETURN( arg_node);
}


node *FOfor (node *arg_node, info *arg_info)
{
  DBUG_ENTER("FOfor");

  // make a new while with the values of the for loop
  node *new = TBmakeWhile(TBmakeBinop(BO_lt, TBmakeVar(STRcpy(FOR_LOOPVAR(arg_node)), NULL, NULL ), COPYdoCopy(FOR_STOP(arg_node))), COPYdoCopy(FOR_BLOCK(arg_node)));

  // after while has been made the for loop is no longer needed
  FREEdoFreeTree(arg_node);

  //  traverse into the new while loop body
  WHILE_BLOCK(new) = TRAVopt(WHILE_BLOCK(new),arg_info);

  DBUG_RETURN( new);
}
/*
 * Traversal start function
 */

node *FOdoForOptimisation( node *syntaxtree)
{
   info *arg_info;

    DBUG_ENTER("FOdoForOptimisation");

    arg_info = MakeInfo();

    TRAVpush( TR_fo);   // Push traversal "ca" as defined in ast.xml
    syntaxtree = TRAVdo( syntaxtree, arg_info);   // Initiate ast traversal
    TRAVpop();          // Pop current traversal

    arg_info = FreeInfo( arg_info);

    DBUG_RETURN( syntaxtree);
}
