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
#include "free.h"


/*
 * Traversal functions
 */




node *SRbinop( node *arg_node, info *arg_info)
{

  DBUG_ENTER("SRbinop");

 // Check if Bin op is multiplication
 if (BINOP_OP( arg_node) == BO_mul) {
   

  if (NUM_VALUE(BINOP_LEFT( arg_node)) == 2) {
    
    node *left = TBmakeNum(NUM_VALUE(BINOP_LEFT( arg_node)));
    node *right = TBmakeNum(NUM_VALUE(BINOP_RIGHT( arg_node)));;
    node *new = TBmakeBinop (BO_add, left, right);
   
    arg_node = FREEdoFreeTree( arg_node);
    arg_node = new;  
  }
  
  
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
