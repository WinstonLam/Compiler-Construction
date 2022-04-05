/*****************************************************************************
 *
 * Module: globvar_transformation
 *
 * Prefix: GT
 *
 * Description:
 *
 * This module implements a traversal of the abstract syntax tree that
 * turns variable initialisations into regular assignments by placing them
 * into a __init function
 *
 *****************************************************************************/


#include "globvar_transformation.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"

#include "memory.h"
#include "ctinfo.h"
#include "free.h"
#include "str.h"


/*
 * INFO structure
 */

struct INFO {
  node *start;
  node *end;
};

/*
 * INFO macros
 */
#define INFO_START(n)  ((n)->start)
#define INFO_END(n)  ((n)->end)
/*
 * INFO functions
 */

static info *MakeInfo(void)
{
  info *result;

  DBUG_ENTER( "MakeInfo");

  result = (info *)MEMmalloc(sizeof(info));

  INFO_START(result) = NULL;
  INFO_END(result) = NULL;

  DBUG_RETURN(result);
}

static info *FreeInfo( info *info)
{
  DBUG_ENTER ("FreeInfo");

  info = MEMfree( info);

  DBUG_RETURN( info);
}


void AddNode(node *entry, info *arg_info)
{
  if (INFO_START(arg_info) != NULL)
  {
    STMTS_NEXT(INFO_END(arg_info)) = entry;
    INFO_END(arg_info) = entry;
  } 
  else 
  {
    INFO_START(arg_info) = entry;
    INFO_END(arg_info) = INFO_START(arg_info);
  }
}
/*
 * Traversal functions
 */

node *GTprogram (node *arg_node, info *arg_info)
{
  DBUG_ENTER("GTprogram");

  // Initialize an empty init function.

  PROGRAM_DECLS(arg_node) = TRAVopt(PROGRAM_DECLS(arg_node), arg_info);

  if (INFO_START(arg_info) != NULL) {
    node *init = TBmakeFundef(T_void, STRcpy("__init"), NULL, TBmakeFunbody(NULL, NULL, INFO_START(arg_info)), NULL);
    PROGRAM_DECLS(arg_node) =  TBmakeDecls(init, PROGRAM_DECLS(arg_node));
  }


  DBUG_RETURN( arg_node);
}

node *GTglobdef (node *arg_node, info *arg_info)
{
  DBUG_ENTER("GTglobdef");
  DBUG_PRINT("Test", GLOBDEF_NAME(arg_node));
   // voeg een node toe aan de struct info als die initialized is
  if (GLOBDEF_INIT(arg_node)) {
 
    AddNode(TBmakeStmts(TBmakeAssign(TBmakeVarlet(STRcpy(GLOBDEF_NAME(arg_node)), NULL, NULL), GLOBDEF_INIT(arg_node)), NULL), arg_info);
    GLOBDEF_INIT(arg_node) = NULL;
  }
  DBUG_RETURN( arg_node);
  
}

/*
 * Traversal start function
 */

node *GTdoGlobvarTransformation( node *syntaxtree)
{
  info *arg_info;

  DBUG_ENTER("GTdoGlobvarTransformation");

  arg_info = MakeInfo();

  TRAVpush( TR_gt);
  syntaxtree = TRAVdo( syntaxtree, arg_info);
  TRAVpop();

  arg_info = FreeInfo( arg_info);

  DBUG_RETURN( syntaxtree);
}
