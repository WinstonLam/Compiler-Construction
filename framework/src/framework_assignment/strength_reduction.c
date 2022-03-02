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
#include "copy_node.h"
#include "copy_info.h"
#include "traverse.h"
#include "free.h"
#include "ctinfo.h"


/*
 * Traversal functions
 */

node *SRbinop (node *arg_node, info *arg_info) {
  DBUG_ENTER("SRbinop");
  SRbinopZ(arg_node, arg_info);
  SRbinopR(arg_node, arg_info);
  SRbinopL(arg_node, arg_info);
  DBUG_RETURN( arg_node);
}

// This function performs if NUM is either 0 or 1.
node *SRbinopZ (node *arg_node, info *arg_info) {
  DBUG_ENTER("SRbinopZ");
  // Check if Bin op is multiplication
  if (BINOP_OP( arg_node) == BO_mul) {
    
    // Check if left child is num, if so then duplicate right child.
   if (NODE_TYPE(BINOP_LEFT( arg_node)) == N_num) {

     // Check if right child is var, if so then make new var nodes.
      if (NODE_TYPE(BINOP_LEFT( arg_node)) == N_var) {

        // If left is 0 then return 0.
        if (NUM_VALUE(BINOP_LEFT( arg_node)) == 0) {
          // Free old binop and assign the new one.
          FREEdoFreeNode( arg_node);
          arg_node = TBmakeNum( 0);
        }
        // If left is 1 then return right.
        else if (NUM_VALUE(BINOP_LEFT( arg_node)) == 1) {
          node* new =  TBmakeVar(STRcpy(VAR_NAME(BINOP_RIGHT( arg_node))));
          FREEdoFreeNode( arg_node);
          arg_node = new;
        }
      }
    }
    // Check if right child is num, if so then duplicate right child.
    else if (NODE_TYPE(BINOP_RIGHT( arg_node)) == N_num) {

      // Check if right child is var, if so then make new var node.
      if (NODE_TYPE(BINOP_RIGHT( arg_node)) == N_var) {

        // If right is 0 then return 0.
        if (NUM_VALUE(BINOP_RIGHT( arg_node)) == 0) {
          // Free old binop and assign the new one.
          FREEdoFreeNode( arg_node);
          arg_node = TBmakeNum( 0);
        }
        // If right is 1 then return left.
        else if (NUM_VALUE(BINOP_RIGHT( arg_node)) == 1) {
          node* new =  TBmakeVar(STRcpy(VAR_NAME(BINOP_LEFT( arg_node))));
          FREEdoFreeNode( arg_node);
          arg_node = new;
        }
      }
    }
  } 
  DBUG_RETURN( arg_node);
}

// This function performs if NUM is either 2 or 3 for right child.
node *SRbinopR( node *arg_node, info *arg_info)
{

  DBUG_ENTER("SRbinopR");

 // Check if Bin op is multiplication
 if (BINOP_OP( arg_node) == BO_mul) {
 
   // Check if left child is num, if so then duplicate right child.
   if (NODE_TYPE(BINOP_LEFT( arg_node)) == N_num) {

     // If left is 2 than double right node twice.
      if (NUM_VALUE(BINOP_LEFT( arg_node)) == 2) {
      
        // Check if right child is var, if so then make new var nodes.
        if (NODE_TYPE(BINOP_RIGHT( arg_node)) == N_var) {
        
            // store name value of right node.
            node *left = TBmakeVar(STRcpy(VAR_NAME(BINOP_RIGHT( arg_node))));
            node *right = TBmakeVar(STRcpy(VAR_NAME(BINOP_RIGHT( arg_node))));

            // Free old binop and assign the new one.
            FREEdoFreeNode( arg_node);
            arg_node = TBmakeBinop(BO_add, left, right);
          }
      }
      //  If left is 3 than make new binop for right node.
      else if ((NUM_VALUE(BINOP_LEFT( arg_node)) == 3)) {
            
            // Check if right child is var, if so then make new var nodes.
            if (NODE_TYPE(BINOP_RIGHT( arg_node)) == N_var) {

            node *left = TBmakeVar(STRcpy(VAR_NAME(BINOP_RIGHT( arg_node))));
            node *right = TBmakeBinop(BO_add, TBmakeVar(STRcpy(VAR_NAME(BINOP_RIGHT( arg_node)))), 
            TBmakeVar(STRcpy(VAR_NAME(BINOP_RIGHT( arg_node)))));

            // Free old binop and assign the new one.
            FREEdoFreeNode( arg_node);
            arg_node = TBmakeBinop (BO_add, left, right);
            }
        }
    }
  }
  DBUG_RETURN( arg_node);
}

// This function performs if NUM is either 2 or 3 for left child.
node *SRbinopL( node *arg_node, info *arg_info)
{

  DBUG_ENTER("SRbinopL");
  
  if (NODE_TYPE(BINOP_RIGHT( arg_node)) == N_num) {
      
      // If right is 2 than double right node twice.
      if (NUM_VALUE(BINOP_RIGHT( arg_node)) == 2) {

        // Check if left child is var, if so then make new var nodes.
        if (NODE_TYPE(BINOP_LEFT( arg_node)) == N_var) {

            node *left = TBmakeVar(STRcpy(VAR_NAME(BINOP_RIGHT( arg_node))));
            node *right = TBmakeVar(STRcpy(VAR_NAME(BINOP_RIGHT( arg_node))));
            
            // Free old binop and assign the new one.
            FREEdoFreeNode( arg_node);
            arg_node = TBmakeBinop (BO_add, left, right);
          }
       
      }
      //  If right is 3 than make new binop for left node.
      else if ((NUM_VALUE(BINOP_RIGHT( arg_node)) == 3)) {
            
            // Check if left child is var, if so then make new var nodes.
            if (NODE_TYPE(BINOP_LEFT( arg_node)) == N_var) {

            node *left = TBmakeVar(STRcpy(VAR_NAME(BINOP_LEFT( arg_node))));
            node *right = TBmakeBinop(BO_add, TBmakeVar(STRcpy(VAR_NAME(BINOP_LEFT( arg_node)))), 
            TBmakeVar(STRcpy(VAR_NAME(BINOP_LEFT( arg_node)))));

            // Free old binop and assign the new one.
            FREEdoFreeNode( arg_node);
            arg_node = TBmakeBinop (BO_add, left, right);
            }
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
