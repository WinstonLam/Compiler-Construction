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

#include "str.h"
#include "memory.h"


/*
 * INFO structure
 */

struct INFO {
  lut_t *lut;
};

struct entry{
  char *name;
  char *type;
  int level;
};

/*
 * INFO macros
 */

#define INFO_LUT(n)  ((n)->lut)

/*
 * INFO functions
 */

static info *MakeSymboltable(void)
{
  
  info *symboltable;

  DBUG_ENTER( "MakeSymboltable");

  symboltable = (info *)MEMmalloc(sizeof(info));


  INFO_LUT(symboltable) = LUTgenerateLut();


  DBUG_RETURN( symboltable);
}

static info *FreeSymboltable( info *info)
{
  DBUG_ENTER ("FreeSymboltable");

  INFO_LUT( info) = LUTremoveLut( INFO_LUT( info));
  info = MEMfree (info);

  DBUG_RETURN (info);
}
/*
 * Traversal functions
 */
// This function creates a new entry given the type name and level
static struct value *MakeEntry (char *name, char *type, long level) {

  struct entry *new = (struct entry *)MEMmalloc(sizeof(struct entry));
  new->name = name;
  new->type = type;
  new->level = level;

  return new;
}

// This function renames a given variable by adding the level to it
static char* rename(char *name, long level)
{
  char *lvl;
  sprintf(lvl, "%d", level);

  // is het mallocen van een nieuwe char pointer nodig?
  // char *new_name = (char*)malloc(1+strlen(name)+strlen(lvl));
  char *new_name;
  strcpy(new_name, name);
  strcat(new_name, lvl);

  return new_name;
}

node *CAvardecl(node* arg_node, info *arg_info)
{
DBUG_ENTER("CAvardecl");
  
  lut_t *lut = INFO_LUT(arg_info);
  
  // Get the name adjusted for level with the rename function.
  char *leveled_name = rename(VARDECL_ID(arg_node), 0);

  void **lookup = LUTsearchInLutS(lut, leveled_name);

  // Search if arg_node is already in the lut.
  if (lookup == NULL) {

    // If not in LUT create a new value node to insert.
    struct entry *new = Makenode(VARDECL_ID(arg_node), VARDECL_TYPE(arg_node), 0 );

    // Insert the new value node into the LUT.
    LUTinsertIntoLutS(lut, leveled_name, new);
}

/*
 * Traversal start function
 */

node *CAdoContextAnalysis( node *syntaxtree)
{
  DBUG_ENTER("CAdoContextAnalysis");

  TRAVpush( TR_ca);   // Push traversal "ca" as defined in ast.xml

  syntaxtree = TRAVdo( syntaxtree, NULL);   // Initiate ast traversal

  TRAVpop();          // Pop current traversal

  DBUG_RETURN( syntaxtree);
}
