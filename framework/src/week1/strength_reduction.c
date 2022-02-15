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


#include "strength_redcution.h"

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

   /* 1. verander de multiplication met for loop? 
    * 2. hoe worden de variabelen in opt_sub aangepast?
    * 
    * 3. mogelijke oplossing: 
    * for (int i = 0, BINOP_RIGHT( arg_node) ) {
    *   BINOP_LEFT( arg_node) += BINOP_LEFT( arg_node)
    */
   
    }

  DBUG_RETURN( arg_node);
}


node *SRvarlet( node *arg_node, info *arg_info)
{
  DBUG_ENTER("SRvarlet");

  
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
