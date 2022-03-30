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
#include "memory.h"

/*
 * INFO structure
 */

struct INFO {
  node *symboltable;
  type *currenttype;
  node *errornodes;
};

/*
 * INFO macros
 */
#define INFO_SYMBOLTABLE(n)  ((n)->symboltable)
#define INFO_CURRENTTYPE(n)  ((n)->currenttype)
#define INFO_ERRORNODES(n)  ((n)->errornodes)

/*
 * INFO functions
 */
static info *MakeInfo(void)
{
  info *tables;

  DBUG_ENTER( "MakeInfo");

  tables = (info *)MEMmalloc(sizeof(info));


  INFO_SYMBOLTABLE( tables) = NULL;
  INFO_CURRENTTYPE( tables) = NULL;
  INFO_ERRORNODES( tables) = NULL;

  DBUG_RETURN( tables);
}

static info *FreeInfo( info *info)
{
  DBUG_ENTER ("FreeInfo");

  info = MEMfree( info);

  DBUG_RETURN( info);
}

static node *FindSymbolEntry( node *symboltable, info *arg_info, char *name)
{
  // Find variable in symboltable
  node *temp = INFO_SYMBOLTABLE(arg_info);

  while(SYMBOLENTRY_NEXT(temp) != NULL)
  {
    if(STReq(name, SYMBOLENTRY_NAME(temp)))
    {
      return temp;
    }

    temp = SYMBOLENTRY_NEXT(temp);
  }

  return NULL;
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

  TRAVopt(ASSIGN_EXPR(arg_node), arg_info);
  type currenttype = INFO_CURRENTTYPE(arg_info);

  // if found
  if(SYMBOLENTRY_TYPE(temp) != currenttype)
  {
    // TODO: ERROR HANDLING, TYPE WRONG OR NOT MATCHING
    INFO_ERRORNODES(arg_info) += NODE_ERROR(arg_node);
  }

  // TODO: ERROR COULDNT FIND NODE IN SYMBOLTABLE
  DBUG_RETURN(arg_node);
}

node *TCifelse(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCifelse");

  // Check node type of condition
  // TODO: IS THIS CORRECT
  TRAVdo(IFELSE_COND(arg_node), arg_info);
  type currenttype = INFO_CURRENTTYPE(arg_info);

  if(currenttype != T_bool) {
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

node *TCdowhile(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCdowhile");

  TRAVdo(DOWHILE_COND(arg_node), arg_info);

  // Check node type of condition
  if(NODE_TYPE(DOWHILE_COND(arg_node)) != N_bool) {
    // TODO: ERROR HANDLING, WRONG TYPE
  }

  // Traverse down block
  DOWHILE_BLOCK(arg_node) = TRAVopt(DOWHILE_BLOCK(arg_node),arg_info);

  DBUG_RETURN(arg_node);
}

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

  TRAVdo(FOR_START(arg_node), arg_info);
  type starttype = INFO_CURRENTTYPE(arg_info);

  TRAVopt(FOR_STEP(arg_node), arg_info);
  type steptype = INFO_CURRENTTYPE(arg_info);

  TRAVdo(FOR_STOP(arg_node), arg_info);
  type stoptype = INFO_CURRENTTYPE(arg_info);

  if(FOR_START(arg_node) != T_int
    && FOR_STEP(arg_node) != T_int
    && FOR_STOP(arg_node) != T_int) {
    // TODO: ERROR HANDLING, WRONG TYPE
  }

  // Traverse down block
  FOR_BLOCK(arg_node) = TRAVopt(FOR_BLOCK(arg_node),arg_info);

  DBUG_RETURN(arg_node);
}

node *TCnum(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCnum");

  INFO_CURRENTTYPE(arg_info) = T_int;

  DBUG_RETURN(arg_node);
}

node *TCfloat(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCfloat");

  INFO_CURRENTTYPE(arg_info) = T_float;

  DBUG_RETURN(arg_node);
}

node *TCbool(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCnum");

  INFO_CURRENTTYPE(arg_info) = T_bool;

  DBUG_RETURN(arg_node);
}

node * TCmonop(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCnum");

  // Traverse monop for current type
  TRAVdo(MONOP_OPERAND(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}

node *TCbinop(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCbinop");

  TRAVdo(BINOP_LEFT(arg_node), arg_info);
  type lefttype = INFO_CURRENTTYPE(arg_info);

  TRAVdo(BINOP_RIGHT(arg_node), arg_info);
  type righttype = INFO_CURRENTTYPE(arg_info);

  if(righttype != lefttype)
  {
    // TODO: ERROR HANDLING, TYPE WRONG OR NOT MATCHING
  } else
  {
    // TODO: MAYBE ALSO BREAK FROM TRAVERSAL?
    INFO_CURRENTTYPE(arg_info) = righttype;
  }

  DBUG_RETURN(arg_node);
}

node *TCfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCfuncall");

  TRAVdo(FUNCALL_ARGS(arg_node), arg_info);
  // TODO: HOE WERKT DEZE?

  // TODO: CHANGE CURRENT TYPE TO FUNCALL TYPE - GET FROM SYMBOL TABLE
  INFO_CURRENTTYPE(arg_info) = 1;

  DBUG_RETURN(arg_node);
}

node * TCexprs(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCexprs");

  // Traverse through expression
  EXPRS_EXPR(arg_node) = TRAVdo(EXPRS_EXPR(arg_node), arg_info);

  // TODO: CHECK IF CURRENT TYPE IS SAME AS PARAMETER TYPE

  // TRAVERSE THROUGH OTHERS
  if (EXPRS_NEXT(arg_node) != NULL) {
    EXPRS_NEXT(arg_node) = TRAVopt(EXPRS_NEXT(arg_node), arg_info);
  }

  DBUG_RETURN(arg_node);
}

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
  DBUG_ENTER("CAdoContextAnalysis");

  info *arg_info;

  arg_info = MakeInfo();

  TRAVpush( TR_tc);   // Push traversal "tc" as defined in ast.xml

  syntaxtree = TRAVdo( syntaxtree, arg_info);   // Initiate ast traversal

  TRAVpop();          // Pop current traversal

  arg_info = FreeInfo( arg_info);

  DBUG_RETURN( syntaxtree);
}
