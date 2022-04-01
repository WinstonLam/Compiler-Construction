/*****************************************************************************
 *
 * Module: type_checker
 *
 * Prefix: TC
 *
 * Description:
 *
 * This module implements a type checker for the semantic analysis of the compiler.
 * It will traverse down the syntax tree and store the corresponding types to each symbol.
 *
 *****************************************************************************/


#include "type_checker.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include "type_checker.h"

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


node *TCprogram(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCprogram");

  // Set info symbol table from program symbol entry
  INFO_SYMBOLTABLE(arg_info) = PROGRAM_SYMBOLENTRY(arg_node);

  DBUG_RETURN(arg_node);
}

node *TCfundef(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCfundef");

  // store the global scope symboltable in place to first traverse into the funbody.
  node *globaltable = INFO_SYMBOLTABLE( arg_info);

  // set the symbol table to symbol table of the corresponding fundef
  //  for one scope deeper to continue with its symbol table.
  INFO_SYMBOLTABLE(arg_info) = FUNDEF_SYMBOLENTRY(arg_node);

  // Traverse into the funbody
  TRAVopt(FUNDEF_FUNBODY(arg_node), arg_info);

  // reset global scope symboltable
  INFO_SYMBOLTABLE(arg_info) = globaltable;

  DBUG_RETURN(arg_node);
}


node *TCassign(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCassign");

  // traverse through the symbol table till variable is found
  // Store name of let
  node *let = ASSIGN_LET(arg_node);
  char *name = VARLET_NAME(let);
  // Find variable in symboltable
  node *temp = INFO_SYMBOLTABLE(arg_info);

  while(SYMBOLENTRY_NEXT(temp) != NULL)
  {
    if(STReq(name, SYMBOLENTRY_NAME(temp)))
    {
      // Check if symbol entry is NOT (one of the allowed types and it matches another one)
      if(!(SYMBOLENTRY_TYPE(temp) == T_bool && NODE_TYPE(ASSIGN_EXPR(let)) == N_bool) &&
        !(SYMBOLENTRY_TYPE(temp) == T_float && NODE_TYPE(ASSIGN_EXPR(let)) == N_float) &&
        !(SYMBOLENTRY_TYPE(temp) == T_int   && NODE_TYPE(ASSIGN_EXPR(let)) == N_num))
      {
        // The symbol entry does not match the type of the expression being assigned
        // TODO: ERROR HANDLING, TYPE WRONG OR NOT MATCHING
      }

      DBUG_RETURN(arg_node);
    }
    temp = SYMBOLENTRY_NEXT(temp);
  }

  // TODO: ERROR COULDNT FIND NODE IN SYMBOLTABLE
  DBUG_RETURN(arg_node);
}

node *TCifelse(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCifelse");

  // Check node type of condition
  // TODO: IS THIS CORRECT
  if(NODE_TYPE(IFELSE_COND(arg_node)) != N_bool) {
    // TODO: ERROR HANDLING, WRONG TYPE
  }

  // Traverse down then
  IFELSE_THEN(arg_node) = TRAVopt(IFELSE_THEN(arg_node), arg_info);

  // Traverse down else if exists
  if (IFELSE_ELSE(arg_node) != NULL) {
    IFELSE_ELSE(arg_node) = TRAVopt(IFELSE_ELSE(arg_node), arg_info);
  }

  DBUG_RETURN(arg_node);
}

// node *TCdowhile(node *arg_node, info *arg_info)
// {
//   DBUG_ENTER("TCdowhile");

//   // Check node type of condition
//   if(NODE_TYPE(DOWHILE_COND(arg_node)) != N_bool) {
//     // TODO: ERROR HANDLING, WRONG TYPE
//   }

//   // Traverse down block
//   DOWHILE_BLOCK(arg_node) = TRAVopt(DOWHILE_BLOCK(arg_node),arg_info);

//   DBUG_RETURN(arg_node);
// }

node *TCwhile(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCwhile");

  // Check node type of condition
  if(NODE_TYPE(WHILE_COND(arg_node)) != N_bool) {
    // TODO: ERROR HANDLING, WRONG TYPE
  }

  // Traverse down block
  WHILE_BLOCK(arg_node) = TRAVopt(WHILE_BLOCK(arg_node),arg_info);

  DBUG_RETURN(arg_node);
}

node *TCfor(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCfor");

  // Check node type of condition
  if(NODE_TYPE(FOR_START(arg_node)) != N_num
    && NODE_TYPE(FOR_STEP(arg_node)) != N_num
    && NODE_TYPE(FOR_STOP(arg_node)) != N_num) {
    // TODO: ERROR HANDLING, WRONG TYPE
  }

  // Traverse down block
  FOR_BLOCK(arg_node) = TRAVopt(FOR_BLOCK(arg_node),arg_info);

  DBUG_RETURN(arg_node);
}

node * TCnum(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCnum");



  DBUG_RETURN(arg_node);
}

node * TCfloat(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCfloat");



  DBUG_RETURN(arg_node);
}

node * TCbool(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCnum");



  DBUG_RETURN(arg_node);
}

// node * TCmonop(node *arg_node, info *arg_info)
// {
//   DBUG_ENTER("TCnum");

//   NODE_TYPE(MONOP_OPERAND(arg_node));

//   DBUG_RETURN(NODE_TYPE(MONOP_OPERAND(arg_node)));
// }

// node * TCnum(node *arg_node, info *arg_info)
// {
//   DBUG_ENTER("TCnum");



//   DBUG_RETURN(arg_node);
// }

node *TCfunbody(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCfunbody");

  //TODO: TRAVERSE DOWN INTO STMTS

  DBUG_RETURN(arg_node);
}

/*
 * Traversal start function
 */

node *TCdoTypeChecking( node *syntaxtree)
{
  info *arg_info;

  DBUG_ENTER("CAdoContextAnalysis");

  arg_info = MakeInfo();

  TRAVpush( TR_tc);   // Push traversal "ca" as defined in ast.xml

  syntaxtree = TRAVdo( syntaxtree, arg_info);   // Initiate ast traversal

  TRAVpop();          // Pop current traversa

  arg_info = FreeInfo( arg_info);

  DBUG_RETURN( syntaxtree);
}
