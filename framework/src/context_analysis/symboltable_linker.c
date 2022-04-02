/*****************************************************************************
 *
 * Module: symboltable_linker
 *
 * Prefix: SL
 *
 * Description:
 *
 * This module implements a symboltable linker of the abstract syntax tree that
 * links any variable found to its stored symbol table entry.
 *
 *****************************************************************************/


#include "symboltable_linker.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "ctinfo.h"
#include "str.h"
#include "string.h"
#include "memory.h"

/*
 * INFO structure
 */

struct INFO {
  node *symboltable;
};

/*
 * INFO macros
 */

#define INFO_SYMBOLTABLE(n)  ((n)->symboltable)

/*
 * INFO functions
 */

static info *MakeInfo(void)
{
  info *tables;

  DBUG_ENTER( "MakeInfo");

  tables = (info *)MEMmalloc(sizeof(info));

  INFO_SYMBOLTABLE( tables) = NULL;

  DBUG_RETURN( tables);
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


/*
 * Traversal Functions
 */


node *SLprogram(node *arg_node, info *arg_info) 
{
    DBUG_ENTER("SLprogram");

    // Set global scope symbol table to info struct
    INFO_SYMBOLTABLE(arg_info) = PROGRAM_SYMBOLENTRY(arg_node);
    TRAVdo(PROGRAM_DECLS(arg_node),arg_info);

    DBUG_RETURN( arg_node);

}

node *SLfundef (node *arg_node, info *arg_info)
{
    DBUG_ENTER("SLfundef");

    // create the link for the fundef node by getting it's symboltable
    // entry using the GetNode function.
    FUNDEF_TABLELINK(arg_node) = GetNode(FUNDEF_NAME(arg_node), arg_info, arg_node);

    // store the global scope symboltable in place to first traverse into the funbody.
    node *globaltable = INFO_SYMBOLTABLE( arg_info);
    // set the symbol table to that of the fundef for one scope deeper to go with its symbol table.
    INFO_SYMBOLTABLE(arg_info) = FUNDEF_SYMBOLENTRY(arg_node);

    // traverse into the funbody to create lower level scope symboltables for the body
    TRAVopt(FUNDEF_FUNBODY(arg_node),arg_info);

    // reset global scope symboltable
    INFO_SYMBOLTABLE( arg_info) = globaltable;

    DBUG_RETURN( arg_node);
}

node *SLvardecl(node *arg_node, info *arg_info)
{
    DBUG_ENTER("SLvardecl");

    // create the link for the vardecl node by getting it's symboltable
    // entry using the GetNode function.
    VARDECL_TABLELINK(arg_node) = GetNode((VARDECL_NAME(arg_node)), arg_info, arg_node);

    // search for next vardecl if there.
    TRAVopt(VARDECL_NEXT(arg_node), arg_info);

    DBUG_RETURN( arg_node);
}

node *SLfor(node *arg_node, info *arg_info)
{
    DBUG_ENTER("SLfor");

    FOR_TABLELINK(arg_node) = GetNode((FOR_LOOPVAR(arg_node)), arg_info, arg_node);

    TRAVopt(FOR_BLOCK(arg_node),arg_info);

    DBUG_RETURN( arg_node);
}

node *SLparam(node *arg_node, info *arg_info)
{
    DBUG_ENTER("SLparam");

    PARAM_TABLELINK(arg_node) = GetNode((PARAM_NAME(arg_node)), arg_info, arg_node);

    TRAVopt(PARAM_NEXT(arg_node), arg_info);

    DBUG_RETURN( arg_node);
}
/*
 * Traversal start function
 */

node *SLdoSymboltableLinker( node *syntaxtree)
{
    info *arg_info;

    DBUG_ENTER("SLdoSymboltableLinker");

    arg_info = MakeInfo();

    TRAVpush( TR_sl);   // Push traversal "ca" as defined in ast.xml
    syntaxtree = TRAVdo( syntaxtree, arg_info);   // Initiate ast traversal
    TRAVpop();          // Pop current traversal

    arg_info = FreeInfo( arg_info);

    DBUG_RETURN( syntaxtree);
}
