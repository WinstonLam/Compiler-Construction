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

#include "memory.h"
#include "ctinfo.h"
#include "free.h"
#include "str.h"


/*
 * INFO structure
 */

struct INFO {
  node *scope;
  node *symboltable;
};

/*
 * INFO macros
 */

#define INFO_SCOPE(n)  ((n)->scope)
#define INFO_SYMBOLTABLE(n)  ((n)->symboltable)

/*
 * INFO functions
 */

static info *MakeInfo(void)
{
  info *result;

  DBUG_ENTER( "MakeInfo");

  result = (info *)MEMmalloc(sizeof(info));

  INFO_SCOPE( result) = NULL;
  INFO_SYMBOLTABLE( result) = NULL;

  DBUG_RETURN( result);
}

static info *FreeInfo( info *info)
{
  DBUG_ENTER ("FreeInfo");

  info = MEMfree( info);

  DBUG_RETURN( info);
}

// this function will return a given node in a symboltable
static node *GetNode(char *entry, info *arg_info, node *arg_node)
{
    // traverse through the symbol table untill node is found
    DBUG_ENTER("GetNode");
    node *temp = INFO_SYMBOLTABLE(arg_info);
    while (temp) {
        if (STReq(SYMBOLENTRY_NAME(temp),entry)) {
            return temp;
        }
        temp = SYMBOLENTRY_NEXT(temp);
    }
    if (!temp) {
        CTIerrorLine(NODE_LINE(arg_node),"Use of undeclared variable %s", entry);
    }
    DBUG_RETURN(temp);
    
}

// this function checks if a certain vardecl of given name is already declared at given scope
static bool IsDeclared(char *name, info *arg_info)
{
  node *temp = INFO_SYMBOLTABLE(arg_info);
  while (temp) {
    if (STReq(SYMBOLENTRY_NAME(temp), name))
        {
        return TRUE;
        }
        temp = SYMBOLENTRY_NEXT(temp);
    }
  return FALSE;
}
/*
 * Traversal functions
 */

node *FOprogram (node *arg_node, info *arg_info)
{
  DBUG_ENTER("FOprogram");

  // set global scope symbol table to info struct
  INFO_SCOPE(arg_info) = PROGRAM_DECLS(arg_node);
  // set symboltable to global scope
  INFO_SYMBOLTABLE(arg_info) = PROGRAM_SYMBOLENTRY(arg_node);

  TRAVdo(PROGRAM_DECLS(arg_node),arg_info);

  DBUG_RETURN( arg_node);
}

node *FOfundef (node *arg_node, info *arg_info)
{
    DBUG_ENTER("FOfundef");

    // store the global scope in place to first traverse into the funbody.
    node *globalscope = INFO_SCOPE( arg_info);
    // store the global symboltable in place to first traverse into the funbody.
    node *globalsymboltable = INFO_SYMBOLTABLE( arg_info);

    // set the scope level to that of the fundef for one scope deeper.
    INFO_SCOPE(arg_info) = FUNBODY_STMTS(FUNDEF_FUNBODY(arg_node));
    INFO_SYMBOLTABLE(arg_info) = FUNDEF_SYMBOLENTRY(arg_node);

    // traverse into the funbody to create lower level scope symboltables for the body
    TRAVopt(FUNDEF_FUNBODY(arg_node),arg_info);

    // reset global scope and symboltable
    INFO_SCOPE( arg_info) = globalscope;
    INFO_SYMBOLTABLE( arg_info) = globalsymboltable;

    DBUG_RETURN( arg_node);
}


node *FOfor (node *arg_node, info *arg_info)
{
  DBUG_ENTER("FOfor");

  // if the variable has not yet been declared we need to lift it out of the forloop
  if (!IsDeclared) {
    // lift for loop variables into the scope of the stored scope
    node *stored_var = GetNode(FOR_LOOPVAR(arg_node),arg_info,arg_node);
  }
  
  node *loopvar = TBmakeStmts(TBmakeVardecl(STRcpy(SYMBOLENTRY_NAME(stored_var)), SYMBOLENTRY_TYPE(stored_var), NULL, ), NULL);
  INFO_SCOPE(arg_info) = TBmakeStmts(loopvar, INFO_SCOPE(arg_info));



  // traverse into the for loop body
  FOR_BLOCK(arg_node) = TRAVopt(FOR_BLOCK(arg_node),arg_info);

  DBUG_RETURN( arg_node);
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
