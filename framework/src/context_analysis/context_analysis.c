/*****************************************************************************
 *
 * Module: context_analysis
 *
 * Prefix: CA
 *
 * Description:
 *
 * This module implements a context analysis of the abstract syntax tree that
 * stores any variable found into the symbol table.
 *
 *****************************************************************************/


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
  node *symboltable;
  int forcounter;
};

/*
 * INFO macros
 */

#define INFO_SYMBOLTABLE(n)  ((n)->symboltable)
#define INFO_FORCOUNTER(n)  ((n)->forcounter)

/*
 * INFO functions
 */

static info *MakeInfo(void)
{
  info *tables;

  DBUG_ENTER( "MakeInfo");

  tables = (info *)MEMmalloc(sizeof(info));

  INFO_SYMBOLTABLE( tables) = NULL;
  INFO_FORCOUNTER( tables) = 0;

  DBUG_RETURN( tables);
}

static info *FreeInfo( info *info)
{
  DBUG_ENTER ("FreeInfo");

  info = MEMfree( info);

  DBUG_RETURN( info);
}

/*
 * Helper Functions
 */

// function to rename variables with different types but same name
static char *RenameCheck(info *arg_info, node *entry) {
  DBUG_ENTER("RenameCheck");
  // if symboltable is empty then no rename is needed
  if (INFO_SYMBOLTABLE(arg_info) == NULL) {
    return SYMBOLENTRY_NAME(entry);
  }
  // traverse through the symbol table till the end,
  // if variable is already in the table rename the variable.
  node *temp = INFO_SYMBOLTABLE(arg_info);

  while (temp) {
    if (STReq(SYMBOLENTRY_NAME(temp), SYMBOLENTRY_NAME(entry)))
        {
        // rename the name of the entry node adjusted with the var_count
        char *old= SYMBOLENTRY_NAME(entry); 
        char *n = STRitoa(INFO_FORCOUNTER(arg_info));
        // create var count string pointer that will be concatenated with multiple
        // occurances in nested variables.
        // variables wil be renamed as such: i - > _ 1 i -> _1i
        char *new = STRcat("_",n);
        SYMBOLENTRY_NAME(entry) = STRcat(new, SYMBOLENTRY_NAME(entry));
        MEMfree(old);
        MEMfree(new);
        MEMfree(n);
        // if variable has been renamed increment the counter
        INFO_FORCOUNTER(arg_info) += 1;
        break;
        }
      temp = SYMBOLENTRY_NEXT(temp);
    }
  return SYMBOLENTRY_NAME(entry);
}

// function that given an node puts it in the symboltable
static info *InsertEntry (info *arg_info, node *entry, node *arg_node) {
  DBUG_ENTER("InsertEntry");

  // if symboltable is empty then change table pointer to current node
  if (INFO_SYMBOLTABLE(arg_info) == NULL) {
      INFO_SYMBOLTABLE(arg_info) = entry;
      DBUG_RETURN(arg_info);
  }
  // traverse through the symbol table till the end,
  // if variable is already in the table assert error.
  node *curr = NULL;
  node *temp = INFO_SYMBOLTABLE(arg_info);
  while (temp) {
    if (STReq(SYMBOLENTRY_NAME(temp), SYMBOLENTRY_NAME(entry)))
        {
        CTIerrorLine(NODE_LINE(arg_node),"Multiple variable declaration of %s", SYMBOLENTRY_NAME(entry));
        }
        curr = temp;
        temp = SYMBOLENTRY_NEXT(temp);
    }

  // insert entry at end of symboltable
  SYMBOLENTRY_NEXT(curr) = entry;
  DBUG_RETURN(arg_info);
}

/*
 * Traversal Functions
 */

node *CAglobdef (node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAglobdef");

  // create new node to add to symboltable
  node *new = TBmakeSymbolentry(GLOBDEF_TYPE(arg_node), STRcpy(GLOBDEF_NAME(arg_node)), NULL);
  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, new, arg_node);

  DBUG_RETURN( arg_node);
}


node *CAfundef (node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAfundef");

  // create new node to add to symboltable
  node *new = TBmakeSymbolentry(FUNDEF_TYPE(arg_node),STRcpy(FUNDEF_NAME(arg_node)), NULL);

  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, new, arg_node);

  // store the global scope symboltable in place to first traverse into the funbody.
  node *globaltable = INFO_SYMBOLTABLE( arg_info);
  // set the symbol table to NULL for one scope deeper to start with fresh symbol table.
  INFO_SYMBOLTABLE(arg_info) = NULL;

  // traverse into the funbody to create lower level scope symboltables for the body
  TRAVopt(FUNDEF_FUNBODY(arg_node),arg_info);
  // traverse into the parameters and link these to fundef scope
  TRAVopt(FUNDEF_PARAMS(arg_node),arg_info);
  
  // link these lower level scope symboltables to their corresponding node
  node *localtable = INFO_SYMBOLTABLE( arg_info);
  FUNDEF_SYMBOLENTRY(arg_node) = localtable;

  // reset global scope symboltable
  INFO_SYMBOLTABLE(arg_info) = globaltable;
  DBUG_RETURN( arg_node);
}


node *CAvardecl(node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAvardecl");

  if VARDECL_INIT(arg_node) {
    TRAVopt(VARDECL_INIT(arg_node),arg_info);
  }
   // create new node to add to symboltable
  node *new = TBmakeSymbolentry(VARDECL_TYPE(arg_node), STRcpy(VARDECL_NAME(arg_node)), NULL);
  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, new, arg_node);

  // search for next vardecl if there.
  TRAVopt(VARDECL_NEXT(arg_node), arg_info);
  DBUG_RETURN( arg_node);
}


node *CAfor(node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAfor");

  // create new node to add to symboltable
  node *new = TBmakeSymbolentry(T_int ,STRcpy(FOR_LOOPVAR(arg_node)), NULL);

  // if instance variable is reused in the nested for loop then rename
  RenameCheck(arg_info, new);
  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, new, arg_node);

  // traverse into the funbody to create lower level scope symboltables for the body
  TRAVopt(FOR_BLOCK(arg_node),arg_info);

  // reset forloop variabel counter to 0
  INFO_FORCOUNTER(arg_info) = 0;

  DBUG_RETURN( arg_node);

}

node *CAparam(node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAparam");

  // create new node to add to symboltable
  node *new = TBmakeSymbolentry(PARAM_TYPE(arg_node), STRcpy(PARAM_NAME(arg_node)), NULL);

  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, new, arg_node);

  // search for next param if there.  
  TRAVopt(PARAM_NEXT(arg_node), arg_info);
  DBUG_RETURN( arg_node);

}
/*
 * Traversal start function
 */

node *CAdoContextAnalysis( node *syntaxtree)
{
  info *arg_info;

  DBUG_ENTER("CAdoContextAnalysis");

  arg_info = MakeInfo();

  TRAVpush( TR_ca);   // Push traversal "ca" as defined in ast.xml
  syntaxtree = TRAVdo( syntaxtree, arg_info);   // Initiate ast traversal

  // // add symbol table to the program scope
   PROGRAM_SYMBOLENTRY(syntaxtree) = INFO_SYMBOLTABLE(arg_info);

  TRAVpop();          // Pop current traversal

  arg_info = FreeInfo( arg_info);

  DBUG_RETURN( syntaxtree);
}

