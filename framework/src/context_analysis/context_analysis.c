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
 * Traversal functions
 */

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
