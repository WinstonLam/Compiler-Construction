/*****************************************************************************
 *
 * Module: strength_reductuon
 *
 * Prefix: SR
 *
 * Description:
 *
 * This module implements a traversal of the abstract syntax tree that 
 * performs strength reduction, meaning that it will replace the * binary op.
 * with +. For example 2*k -> k+k.
 *
 *****************************************************************************/


#include "strength_reduction.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"

#include "str.h"
#include "memory.h"


/*
 * Traversal functions
 */

node *SRbinop( node *arg_node, info *arg_info)
{

  DBUG_ENTER("SRbinop");

 // Check if Bin op is multiplication
 if (BINOP_OP( arg_node) == BO_mul) {
   
   // check for 0 in either left or right
   if (BINOP_LEFT( arg_node) == 0 ||
   BINOP_RIGHT( arg_node) == 0) {
     arg_node = FREEdoFreeTree( arg_node);
     arg_node = TBmakeNum( 0);
   }
  
    for (int i = 0, BINOP_RIGHT( arg_node)) {
      BINOP_LEFT( arg_node) += BINOP_LEFT( arg_node)
    
    }

  DBUG_RETURN( arg_node);
}

/*
 * Traversal start function
 */

node *SRdoStrengthReduction( node *syntaxtree)
{
  DBUG_ENTER("SRdoStrengthReduction");

  TRAVpush( TR_sr);   // Push traversal "sr" as defined in ast.xml

  syntaxtree = TRAVdo( syntaxtree, NULL);   // Initiate ast traversal

  TRAVpop();          // Pop current traversal

  DBUG_RETURN( syntaxtree);
}
