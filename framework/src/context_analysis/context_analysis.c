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
#include "lookup_table.h"
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

/*
 * Traversal functions
 */

/*  Whenever we encounter a fundef, update the symboltable
*   pointer inside the info struct to point to the current fundef
*   to which we will start appending nodes
*/

static int InsertEntry (info *arg_info, node *entry) {
  // if symboltable is empty then change table pointer to current node
  if (INFO_SYMBOLTABLE(arg_info) == NULL) {
    INFO_SYMBOLTABLE(arg_info) = entry;
  } else {
    // if there is something in the symbol table already then set the
    // next pointer of the tail node to this current node
    node *temp = INFO_SYMBOLTABLE(arg_info);
    while (SYMBOLENTRY_NEXT(temp) != NULL) {
      if(SYMBOLENTRY_NAME(temp) == SYMBOLENTRY_NAME(entry) ){
        return 1;
      }
      temp = SYMBOLENTRY_NEXT(temp);
    }
    SYMBOLENTRY_NEXT(temp) = entry;
  }
  return 0;
}

node *CAfundef (node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAfundef");

  // create new node to add to symboltable
  node *new = TBmakeSymbolentry(FUNDEF_TYPE(arg_node),FUNDEF_NAME(arg_node), NULL);

  // use the InsertEntry function to insert the new node into the symboltable
  if (InsertEntry(arg_info, new) == 1) {
    CTInote( "Multiple declarations of: %s", SYMBOLENTRY_NAME(new));
    DBUG_RETURN( arg_node);
  };

  // store the global scope symboltable in place to first traverse into the funbody.
  node *globaltable = INFO_SYMBOLTABLE( arg_info);
  // set the symbol table to NULL for one scope deeper to start with fresh symbol table.
  INFO_SYMBOLTABLE(arg_info) = NULL;

  // traverse into the funbody to create lower level scope symboltables for the body
  TRAVopt(FUNDEF_FUNBODY(arg_node),arg_info);
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

  // create new node to add to symboltable
  node *new = TBmakeSymbolentry(VARDECL_TYPE(arg_node), VARDECL_NAME(arg_node), NULL);

  // use the InsertEntry function to insert the new node into the symboltable
  if (InsertEntry(arg_info, new) == 1) {
    CTInote( "Multiple declarations of: %s", SYMBOLENTRY_NAME(new));
    DBUG_RETURN( arg_node);
  };

  DBUG_RETURN( arg_node);
}

node *CAfor(node *arg_node, info *arg_info)
{
  // DBUG_ENTER("CAfor");

  // // create new node to add to symboltable
  // node *new = TBmakeSymbolentry(T_int ,FOR_LOOPVAR(arg_node), NULL);

  // // store the global scope symboltable in place to first traverse into the forbody.
  // node *globaltable = INFO_SYMBOLTABLE( arg_info);
  // // set the symbol table to for loop initializing value for the forloop block


  // // traverse into the funbody to create lower level scope symboltables for the body
  // TRAVopt(FOR_BLOCK(arg_node),arg_info);
  // // link these lower level scope symboltables to their corresponding node
  // node *localtable = INFO_SYMBOLTABLE( arg_info);
  // FOR_SYMBOLENTRY(arg_node) = localtable;

  // // reset global scope symboltable
  // INFO_SYMBOLTABLE(arg_info) = globaltable;
  // DBUG_RETURN( arg_node);

}

/*
 * Traversal start function
 */

node *CAdoContextAnalysis( node *syntaxtree)
{
  DBUG_ENTER("CAdoContextAnalysis");

  info *arg_info;

  arg_info = MakeInfo();

  TRAVpush( TR_ca);   // Push traversal "ca" as defined in ast.xml

  syntaxtree = TRAVdo( syntaxtree, NULL);   // Initiate ast traversal

  TRAVpop();          // Pop current traversal

  arg_info = FreeInfo( arg_info);

  DBUG_RETURN( syntaxtree);
}
