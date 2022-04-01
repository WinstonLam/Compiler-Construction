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
  node *init;
};

/*
 * INFO macros
 */
#define INFO_INIT(n)  ((n)->init)
/*
 * INFO functions
 */

static info *MakeInfo(void)
{
  info *init_func;

  DBUG_ENTER( "MakeInfo");

  init_func = (info *)MEMmalloc(sizeof(info));

  INFO_INIT( init_func) = NULL;

  DBUG_RETURN( init_func);
}

static info *FreeInfo( info *info)
{
  DBUG_ENTER ("FreeInfo");

  info = MEMfree( info);

  DBUG_RETURN( info);
}


static void AddNode(node *entry, info *arg_info){
  node *body = FUNBODY_STMTS(FUNDEF_FUNBODY(INFO_INIT(arg_info)));
  while (body) {
        if (!STMTS_NEXT(body)) {
          STMTS_NEXT(body) = entry;
          return;
        }
        body = STMTS_NEXT(body);
    }
  FUNBODY_STMTS(FUNDEF_FUNBODY(INFO_INIT(arg_info))) = entry;
}
/*
 * Traversal functions
 */

node *GTdecls (node *arg_node, info *arg_info)
{
  DBUG_ENTER("GTdecls");

  // Initialize an empty init function
  char *init = "__init";
  INFO_INIT(arg_info) = TBmakeFundef(T_void, init, NULL, TBmakeFunbody(NULL, NULL, NULL), NULL);

  TRAVdo(DECLS_DECL(arg_node),arg_info);

  DBUG_RETURN( arg_node);
}

node *GTglobdef (node *arg_node, info *arg_info)
{
  DBUG_ENTER("GTglobdef");
   // voeg een node toe aan de struct info als die initialized is
  if (GLOBDEF_INIT(arg_node) != NULL) { 

    AddNode(TBmakeStmts(TBmakeVarlet(GLOBDEF_NAME(arg_node), GLOBDEF_INIT(arg_node), NULL), NULL), arg_info);
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
  syntaxtree = TRAVdo( syntaxtree, NULL);
  TRAVpop();

  arg_info = FreeInfo( arg_info);

  DBUG_RETURN( syntaxtree);
}
