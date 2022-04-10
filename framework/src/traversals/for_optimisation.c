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
  int var_count;
};

/*
 * INFO macros
 */

#define INFO_SCOPE(n)  ((n)->scope)
#define INFO_VAR_COUNT(n)  ((n)->var_count)


/*
 * INFO functions
 */

static info *MakeInfo(void)
{
  info *result;

  DBUG_ENTER( "MakeInfo");

  result = (info *)MEMmalloc(sizeof(info));

  INFO_SCOPE( result) = NULL;
  INFO_VAR_COUNT( result) = 0;

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
    if (FUNDEF_FUNBODY(arg_node)) {
       INFO_SCOPE(arg_info) = FUNBODY_STMTS(FUNDEF_FUNBODY(arg_node));
    }

    // traverse into the funbody to search for for loops
    TRAVopt(FUNDEF_FUNBODY(arg_node),arg_info);
    
    DBUG_RETURN( arg_node);
}


node *FOfor (node *arg_node, info *arg_info)
{
  DBUG_ENTER("FOfor");
  binop op = BO_add;
  if (FOR_STEP(arg_node)) {
    if (FOR_STEP(arg_node) < 0) {
      op = BO_sub;
    }
  }

  char *name =  FOR_LOOPVAR(arg_node);

  node *block = FOR_BLOCK(arg_node);
  if (block) {
    if (NODE_TYPE(block) == N_stmts) {
      node *temp = block;
      while(STMTS_NEXT(temp)) {
        temp = STMTS_NEXT(temp);
      }

      STMTS_NEXT(temp) = TBmakeStmts(TBmakeAssign(TBmakeVarlet(STRcpy(name), NULL, NULL), TBmakeBinop(op, TBmakeVar(STRcpy(name), NULL, NULL), TBmakeNum(1))), NULL);
    } else {
      block = TBmakeStmts(block, TBmakeAssign(TBmakeVarlet(STRcpy(name), NULL, NULL), TBmakeBinop(op, TBmakeVar(STRcpy(name), NULL, NULL), TBmakeNum(1))));
    }
  } else {
    block = TBmakeStmts(TBmakeAssign(TBmakeVarlet(STRcpy(name), NULL, NULL), TBmakeBinop(op, TBmakeVar(STRcpy(name), NULL, NULL), TBmakeNum(1))), NULL);
  }


  // make a new while with the values of the for loop
  node *new = TBmakeWhile(TBmakeBinop(BO_lt, TBmakeVarlet(STRcpy(name), NULL, NULL ), COPYdoCopy(FOR_STOP(arg_node))), COPYdoCopy(block));

  //  traverse into the new while loop body
  TRAVopt(WHILE_BLOCK(new),arg_info);

    // after while has been made the for loop is no longer needed
  FREEdoFreeTree(arg_node);

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

    TRAVpush( TR_fo);   // Push traversal "fo" as defined in ast.xml
    syntaxtree = TRAVdo( syntaxtree, arg_info);   // Initiate ast traversal
    TRAVpop();          // Pop current traversal

    arg_info = FreeInfo( arg_info);

    DBUG_RETURN( syntaxtree);
}
