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
#include "copy.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "free.h"
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
  node *scope;
  char *funname;
  char *forname_old;
  char *forname_new;
  int forcounter;
  int offset;
};

/*
 * INFO macros
 */

#define INFO_SYMBOLTABLE(n)  ((n)->symboltable)
#define INFO_FORCOUNTER(n)  ((n)->forcounter)
#define INFO_FUNNAME(n)  ((n)->funname)
#define INFO_FORNAME_OLD(n)  ((n)->forname_old)
#define INFO_FORNAME_NEW(n)  ((n)->forname_new)
#define INFO_SCOPE(n)  ((n)->scope)
#define INFO_OFFSET(n)  ((n)->offset)

/*
 * INFO functions
 */

static info *MakeInfo(void)
{
  info *tables;

  DBUG_ENTER( "MakeInfo");

  tables = (info *)MEMmalloc(sizeof(info));

  INFO_SYMBOLTABLE( tables) = NULL;
  INFO_SCOPE( tables) = NULL;
  INFO_FUNNAME( tables) = NULL;
  INFO_FORNAME_OLD( tables) = NULL;
  INFO_FORNAME_NEW( tables) = NULL;
  INFO_FORCOUNTER( tables) = 0;
  INFO_OFFSET( tables) = 0;

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
node *RenameCheck(info *arg_info, node *entry) {
  DBUG_ENTER("RenameCheck");

  SYMBOLENTRY_NAME(entry) = STRcatn(3, SYMBOLENTRY_NAME(entry), "_", STRitoa(INFO_FORCOUNTER(arg_info)));
  // if variable has been renamed increment the counter
  INFO_FORCOUNTER(arg_info) ++;
  DBUG_RETURN(entry);

}

// function that given an node puts it in the symboltable
info *InsertEntry (info *arg_info, node *entry, node *arg_node) {
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

  // increment offset counter
  int offset = INFO_OFFSET(arg_info) ++;
  // create new node to add to symboltable
  node *new = TBmakeSymbolentry(GLOBDEF_TYPE(arg_node), STRcpy(GLOBDEF_NAME(arg_node)), offset, NULL, NULL);
  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, new, arg_node);

  DBUG_RETURN( arg_node);
}


node *CAfundef (node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAfundef");

  // store the global scope symboltable in place to first traverse into the funbody.
  node *globaltable = INFO_SYMBOLTABLE( arg_info);
  // set the symbol table to NULL for one scope deeper to start with fresh symbol table.
  INFO_SYMBOLTABLE(arg_info) = NULL;
  
  if (FUNDEF_FUNBODY(arg_node)) {
      INFO_SCOPE(arg_info) = FUNBODY_VARDECLS(FUNDEF_FUNBODY(arg_node)); 
  }

  INFO_FUNNAME(arg_info) = STRcpy(FUNDEF_NAME(arg_node));
   // traverse into the parameters and link these to fundef scope
  TRAVopt(FUNDEF_PARAMS(arg_node),arg_info);
  // traverse into the funbody to create lower level scope symboltables for the body
  TRAVopt(FUNDEF_FUNBODY(arg_node),arg_info);

  // link these lower level scope symboltables to their corresponding node
  node *localtable = INFO_SYMBOLTABLE( arg_info);
  FUNDEF_SYMBOLENTRY(arg_node) = localtable;

  // add new made vardecl to the body of the current scope
  if (FUNDEF_FUNBODY(arg_node)) {
      FUNBODY_VARDECLS(FUNDEF_FUNBODY(arg_node)) = COPYdoCopy(INFO_SCOPE(arg_info));
  }
  // reset the scope to the global scope
  INFO_SYMBOLTABLE(arg_info) = globaltable;

  // increment offset counter
  int offset = INFO_OFFSET(arg_info) ++;
  // create new node to add to symboltable
  node *new = TBmakeSymbolentry(FUNDEF_TYPE(arg_node),STRcpy(FUNDEF_NAME(arg_node)), offset, NULL, COPYdoCopy(FUNDEF_PARAMS(arg_node)));
 
  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, new, arg_node);

  DBUG_RETURN(arg_node);
  }

node *CAvar(node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAvar");

  if (STReq(VAR_NAME(arg_node),INFO_FORNAME_OLD(arg_info))) {
    // if the variable is a for loop variable then rename it
    VAR_NAME(arg_node) = STRcpy(INFO_FORNAME_NEW(arg_info));
  }

  DBUG_RETURN(arg_node);
}

node *CAvardecl(node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAvardecl");

  if VARDECL_INIT(arg_node) {
    TRAVopt(VARDECL_INIT(arg_node),arg_info);
  }
  // increment offset counter
  int offset = INFO_OFFSET(arg_info) ++;
   // create new node to add to symboltable
  node *new = TBmakeSymbolentry(VARDECL_TYPE(arg_node), STRcpy(VARDECL_NAME(arg_node)), offset, NULL, NULL);
  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, new, arg_node);

  // search for next vardecl if there.
  TRAVopt(VARDECL_NEXT(arg_node), arg_info);
  DBUG_RETURN( arg_node);
}

node *CAparam(node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAparam");

  // increment offset counter
  int offset = INFO_OFFSET(arg_info) ++;
  // create new node to add to symboltable 
  node *new = TBmakeSymbolentry(PARAM_TYPE(arg_node), STRcpy(PARAM_NAME(arg_node)), offset, NULL, NULL);
  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, new, arg_node);
  CTInote("inserted param %s", PARAM_NAME(arg_node));

  // search for next param if there.
  TRAVopt(PARAM_NEXT(arg_node), arg_info);
  DBUG_RETURN( arg_node);
}

node *CAfor(node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAfor");

  // increment offset counter
  int offset = INFO_OFFSET(arg_info) ++;
  // create new node to add to symboltable
  node *new = TBmakeSymbolentry(T_int ,STRcpy(FOR_LOOPVAR(arg_node)), offset, NULL, NULL);

  // if instance variable is reused in the nested for loop then rename
  node *renamed = RenameCheck(arg_info, COPYdoCopy(new));

  CTInote("renamed to: %s", SYMBOLENTRY_NAME(renamed));
  INFO_FORNAME_OLD(arg_info) = STRcpy(FOR_LOOPVAR(arg_node));
  INFO_FORNAME_NEW(arg_info) = STRcpy(SYMBOLENTRY_NAME(renamed));

  // create new vardecl in the current scope
  if (INFO_SCOPE(arg_info)) {
    INFO_SCOPE(arg_info) =  TBmakeVardecl(STRcpy(SYMBOLENTRY_NAME(renamed)), T_int, NULL, FOR_START(arg_node), INFO_SCOPE(arg_info));
  } else {
    INFO_SCOPE(arg_info) = TBmakeVardecl(STRcpy(SYMBOLENTRY_NAME(renamed)), T_int, NULL, FOR_START(arg_node), NULL);
  }

  // change the for loop var to declared vardecl
  FOR_LOOPVAR(arg_node) = STRcpy(SYMBOLENTRY_NAME(renamed));
  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, COPYdoCopy(renamed), arg_node);

  FREEdoFreeTree(new);
  FREEdoFreeTree(renamed);

  // traverse into the funbody to create lower level scope symboltables for the body
  TRAVopt(FOR_BLOCK(arg_node),arg_info);


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

