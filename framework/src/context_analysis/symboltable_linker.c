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
#include "context_analysis.h"
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
  node *parenttable;
  node *symboltable;
};

/*
 * INFO macros
 */

#define INFO_SYMBOLTABLE(n)  ((n)->symboltable)
#define INFO_PARENTTABLE(n) ((n)->parenttable)

/*
 * INFO functions
 */

static info *MakeInfo(void)
{
  info *tables;

  DBUG_ENTER( "MakeInfo");

  tables = (info *)MEMmalloc(sizeof(info));

  INFO_SYMBOLTABLE( tables) = NULL;
  INFO_PARENTTABLE( tables) = NULL;

  DBUG_RETURN( tables);
}

static info *FreeInfo( info *info)
{
  DBUG_ENTER ("FreeInfo");

  info = MEMfree( info);

  DBUG_RETURN( info);
}

// this function will return a given node in a symboltable
bool IsLocal(char *entry, node *symboltable)
{
    node *temp = symboltable;
    while (temp) {
        if (STReq(SYMBOLENTRY_NAME(temp),entry)) {
            return TRUE;
        }
        temp = SYMBOLENTRY_NEXT(temp);
    }
    return FALSE;
}


int GetParamcount (node *entry) {
    node *temp = SYMBOLENTRY_PARAMS(entry);
    int count = 0;
    while (temp) {
        count++;
        temp = PARAM_NEXT(temp);
    }
    return count;
}

int GetVardeclcount(node *entry) {
    node *temp = FUNDEF_TABLELINK(entry);

    int count = 0;
    int param = 0;
    if (temp) {
        param = GetParamcount(temp);
    }

    while (temp) {
        count++;
        temp = SYMBOLENTRY_NEXT(temp);
    }


    return count - param;
}

/*
 * Traversal Functions
 */


node *SLprogram(node *arg_node, info *arg_info)
{
    DBUG_ENTER("SLprogram");

    // Set global scope symbol table to info struct
    INFO_SYMBOLTABLE(arg_info) = PROGRAM_SYMBOLENTRY(arg_node);
    DBUG_PRINT("SLsymboltable global",("%s",SYMBOLENTRY_NAME(INFO_SYMBOLTABLE(arg_info))));
    TRAVdo(PROGRAM_DECLS(arg_node),arg_info);

    DBUG_RETURN( arg_node);

}

node *SLfundef (node *arg_node, info *arg_info)
{
    DBUG_ENTER("SLfundef");

    // create the link for the fundef node by getting it's symboltable
    // entry using the GetNode function.
    FUNDEF_TABLELINK(arg_node) = GetNode(FUNDEF_NAME(arg_node), INFO_SYMBOLTABLE( arg_info), arg_node, INFO_PARENTTABLE(arg_info));

    // store the global scope symboltable in place to first traverse into the funbody.
    node *globaltable = INFO_SYMBOLTABLE( arg_info);
    // set the symbol table to that of the fundef for one scope deeper to go with its symbol table.
    INFO_PARENTTABLE(arg_info) = globaltable;
    INFO_SYMBOLTABLE(arg_info) = FUNDEF_SYMBOLENTRY(arg_node);

    // traverse into the funbody to create lower level scope symboltables for the body
    TRAVopt(FUNDEF_FUNBODY(arg_node),arg_info);

    // reset global scope symboltable
    INFO_SYMBOLTABLE( arg_info) = globaltable;
     INFO_PARENTTABLE(arg_info) = NULL;

    DBUG_RETURN( arg_node);
}

node *SLvardecl(node *arg_node, info *arg_info)
{
    DBUG_ENTER("SLvardecl");

    if (VARDECL_INIT(arg_node))
    {
        VARDECL_INIT(arg_node) = TRAVopt(VARDECL_INIT(arg_node), arg_info);
    }
    // create the link for the vardecl node by getting it's symboltable
    // entry using the GetNode function.
    VARDECL_TABLELINK(arg_node) = GetNode((VARDECL_NAME(arg_node)), INFO_SYMBOLTABLE(arg_info), arg_node, INFO_PARENTTABLE(arg_info));

    // search for next vardecl if there.
    TRAVopt(VARDECL_NEXT(arg_node), arg_info);

    DBUG_RETURN( arg_node);
}

node *SLglobdef (node *arg_node, info *arg_info)
{
    DBUG_ENTER("SLglobdef");

    // create the link for the globdef node by getting it's symboltable
    // entry using the GetNode function.
    GLOBDEF_TABLELINK(arg_node) = GetNode(GLOBDEF_NAME(arg_node), INFO_SYMBOLTABLE( arg_info), arg_node, INFO_PARENTTABLE(arg_info));

    DBUG_RETURN( arg_node);
}

node *SLvarlet(node *arg_node, info *arg_info)
{
    DBUG_ENTER("SLvarlet");

    VARLET_TABLELINK(arg_node) = GetNode((VARLET_NAME(arg_node)),INFO_SYMBOLTABLE(arg_info), arg_node, INFO_PARENTTABLE(arg_info));

    DBUG_RETURN( arg_node);
}


node *SLvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("SLvar");

    VAR_TABLELINK(arg_node) = GetNode((VAR_NAME(arg_node)),INFO_SYMBOLTABLE(arg_info), arg_node, INFO_PARENTTABLE(arg_info));

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
