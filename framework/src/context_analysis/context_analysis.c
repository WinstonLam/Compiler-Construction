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
 * Helper Functions
 */

// // Typeprinter that given an enum type returns a *char
// static char *TypePrinter(type types)
// {
//   char *typename;
//   switch (types)
//   {
//     case T_void:
//       strcpy(typename, "_void");
//       break;
//     case T_bool:
//       strcpy(typename, "_bool");
//       break;
//     case T_int:
//       strcpy(typename, "_int");
//       break;
//     case T_float:
//       strcpy(typename, "_float");
//       break;
//     case T_unknown:
//       DBUG_ASSERT(0, "unknown type detected!");
//       break;
//   }
//   return typename;
// }


// // function to rename variables with different types but same name
// static void RenameCheck(info *arg_info, node *entry) {
//   DBUG_ENTER("RenameCheck");
//   // if symboltable is empty then no rename is needed
//   if (INFO_SYMBOLTABLE(arg_info) == NULL) {
//     return;
//   }
//   // traverse through the symbol table till the end,
//   // if variable is already in the table assert error. if
//   // variables have same name but different types then rename.
//   node *temp = INFO_SYMBOLTABLE(arg_info);
//   while (SYMBOLENTRY_NEXT(temp) != NULL) {
//     if (STReq(SYMBOLENTRY_NAME(temp), SYMBOLENTRY_NAME(entry)) &&
//         SYMBOLENTRY_TYPE(temp) == SYMBOLENTRY_TYPE(entry))
//         {
//         CTInote("test");
//         DBUG_PRINT( "Multiple variable declaration of %s", SYMBOLENTRY_NAME(entry));
//         }
//     // if variables have the same name, but different types then preform a rename
//     // where the name will be extended with the type as such: int i -> i_int
//     else if (STReq(SYMBOLENTRY_NAME(temp), SYMBOLENTRY_NAME(entry)))
//         {
//         // store the current names in temp values and free the current names of both variables later
//         char *currname_temp = STRcpy(SYMBOLENTRY_NAME(temp));
//         char *currname_entry = STRcpy(SYMBOLENTRY_NAME(entry));

//         // store the type adjusted names for both variables
//         // use the typeprinter to get a *char based on the type then STRcat
//         // both *char to a new one
//         SYMBOLENTRY_NAME(temp) = STRcat(currname_temp, TypePrinter(SYMBOLENTRY_TYPE(temp)));
//         SYMBOLENTRY_NAME(entry) = STRcat(currname_entry, TypePrinter(SYMBOLENTRY_TYPE(entry)));

//         MEMfree(currname_temp);
//         MEMfree(currname_entry);
//         }
//       temp = SYMBOLENTRY_NEXT(temp);
//     }
// }

// function that given an node puts it in the symboltable and returns 
static info *InsertEntry (info *arg_info, node *entry) {
  DBUG_ENTER("InsertEntry");
  // if symboltable is empty then change table pointer to current node
  if (INFO_SYMBOLTABLE(arg_info) == NULL) {
      INFO_SYMBOLTABLE(arg_info) = entry;
      DBUG_PRINT( "\nAdded variable %s to symbol table\n\n", SYMBOLENTRY_NAME(entry));
      DBUG_RETURN(arg_info);
  }

  // traverse through the symbol table till the end,
  // if variable is already in the table assert error.
  node *temp = INFO_SYMBOLTABLE(arg_info);
  while (SYMBOLENTRY_NEXT(temp) != NULL) {
    if (STReq(SYMBOLENTRY_NAME(temp), SYMBOLENTRY_NAME(entry)))
        {
        DBUG_PRINT( "Multiple variable declaration of %s", SYMBOLENTRY_NAME(entry));
        break; // hoe moet ik error returnen
        }
    temp = SYMBOLENTRY_NEXT(temp);
    }
  // insert entry at end of symboltable
  SYMBOLENTRY_NEXT(temp) = entry;
  DBUG_PRINT( "\nAdded variable %s to symbol table\n\n", SYMBOLENTRY_NAME(entry));
  DBUG_RETURN(arg_info);
}

/*
 * Traversal Functions
 */

// node *CAprogram (node *arg_node, info *arg_info) 
// {
//   DBUG_ENTER("CAprogram");

// }
node *CAglobdecl (node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAglobdecl");

  // create new node to add to symboltable
  node *new = TBmakeSymbolentry(GLOBDECL_TYPE(arg_node), STRcpy(GLOBDECL_NAME(arg_node)), NULL);
  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, new);

  DBUG_RETURN( arg_node);
}


node *CAfundef (node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAfundef");

  // create new node to add to symboltable
  node *new = TBmakeSymbolentry(FUNDEF_TYPE(arg_node),STRcpy(FUNDEF_NAME(arg_node)), NULL);

  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, new);

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

node *CAfunbody(node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAfunbody");

  TRAVopt(FUNBODY_VARDECLS(arg_node), arg_info);
  TRAVopt(FUNBODY_STMTS(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}


node *CAvardecl(node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAvardecl");

  // create new node to add to symboltable
  node *new = TBmakeSymbolentry(VARDECL_TYPE(arg_node), STRcpy(VARDECL_NAME(arg_node)), NULL);
  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, new);

  DBUG_RETURN( arg_node);
}


node *CAfor(node *arg_node, info *arg_info)
{
  DBUG_ENTER("CAfor");

  // create new node to add to symboltable
  node *new = TBmakeSymbolentry(T_int ,STRcpy(FOR_LOOPVAR(arg_node)), NULL);

  // use the InsertEntry function to insert the new node into the symboltable
  InsertEntry(arg_info, new);

  // store the innermost function scope symboltable in place to first traverse into the forbody.
  node *uppertable = INFO_SYMBOLTABLE( arg_info);
  // set the symbol table to for loop initializing value for the forloop block

  // traverse into the funbody to create lower level scope symboltables for the body
  TRAVopt(FOR_BLOCK(arg_node),arg_info);

  // reset global scope symboltable
  INFO_SYMBOLTABLE(arg_info) = uppertable;
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

  if (syntaxtree != NULL) {
    CTInote("entries are: %s", SYMBOLENTRY_NAME(PROGRAM_SYMBOLENTRY(syntaxtree)));
  }
  TRAVpop();          // Pop current traversal

  arg_info = FreeInfo( arg_info);

  DBUG_RETURN( syntaxtree);
}

