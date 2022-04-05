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
#include "ctinfo.h"

/*
 * INFO structure
 */

struct INFO {
  node *symboltable;
  type currenttype;
  type fundeftype;
  int paramcounter;
};

/*
 * INFO macros
 */
#define INFO_SYMBOLTABLE(n)  ((n)->symboltable)
#define INFO_CURRENTTYPE(n)  ((n)->currenttype)
#define INFO_PARAMCOUNTER(n)  ((n)->paramcounter)
#define INFO_FUNDEFTYPE(n)  ((n)->fundeftype)

/*
 * INFO functions
 */

static info *MakeInfo(void)
{
  info *tables;

  DBUG_ENTER("MakeInfo");

  tables = (info *)MEMmalloc(sizeof(info));

  INFO_SYMBOLTABLE(tables) = NULL;
  INFO_CURRENTTYPE(tables) = T_unknown;
  INFO_FUNDEFTYPE(tables) = T_unknown;
  INFO_PARAMCOUNTER(tables) = 0;

  DBUG_RETURN( tables);
}

static info *FreeInfo( info *info)
{
  DBUG_ENTER ("FreeInfo");

  info = MEMfree(info);

  DBUG_RETURN(info);
}

/*
 * Helper functions
 */

// this function will return a given node in a symboltable
static node *GetNode(char *name, info *arg_info)
{
    // traverse through the symbol table untill node is found
    node *temp = INFO_SYMBOLTABLE(arg_info);
    while (temp) {
        if (STReq(SYMBOLENTRY_NAME(temp), name)) {
            return temp;
        }
        temp = SYMBOLENTRY_NEXT(temp);
    }

    CTIerror("Could not be found in symbol table: %s", name);
    return temp;
}


static char *TypePrinter(type types)
{
  switch (types)
  {
    case T_void:
      return "void";
    case T_bool:
      return "bool";
    case T_int:
      return "int";
    case T_float:
      return "float ";
    default:
      return "UNKNOWN";
  }
}

/*
 * Traversal Functions
 */

node *TCprogram(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCprogram");

  // Set info symbol table from program symbol entry
  INFO_SYMBOLTABLE(arg_info) = PROGRAM_SYMBOLENTRY(arg_node);

  PROGRAM_DECLS(arg_node) = TRAVopt(PROGRAM_DECLS(arg_node), arg_info);


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

  INFO_CURRENTTYPE(arg_info) = FUNDEF_TYPE(arg_node);
  INFO_FUNDEFTYPE(arg_info) = FUNDEF_TYPE(arg_node);
  // Traverse into the funbody
  FUNDEF_FUNBODY(arg_node) = TRAVopt(FUNDEF_FUNBODY(arg_node), arg_info);

  // reset global scope symboltable
  INFO_SYMBOLTABLE(arg_info) = globaltable;

  DBUG_RETURN(arg_node);
}

node *TCreturn(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCreturn");

  // traverse into the return expression
  RETURN_EXPR(arg_node) = TRAVopt(RETURN_EXPR(arg_node), arg_info);

  // check if the return type is the same as the function type
  if (INFO_FUNDEFTYPE(arg_info) != INFO_CURRENTTYPE(arg_info)) {
    CTIerror("expected return type: %s, but got: %s", TypePrinter(INFO_FUNDEFTYPE(arg_info)), TypePrinter(INFO_CURRENTTYPE(arg_info)));
  }

  DBUG_RETURN(arg_node);
}

node *TCfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCfuncall");

  // get the first param
  node *temp = TRAVopt(FUNCALL_PARAMS(arg_node), arg_info);

  // check for each param if the given type corresponds
  while (temp) {

    // get symboltable entry name of the param
    char *name = STRcatn(3,FUNCALL_NAME(arg_node), STRcpy("_p"), STRitoa(INFO_PARAMCOUNTER(arg_info)));
    // get symboltableentry node of the param
    node *paramNode = GetNode(STRcpy(name), arg_info);

    // traverse down the expressions of the parameter
    EXPRS_EXPR(arg_node) = TRAVdo(EXPRS_EXPR(arg_node), arg_info);
    type currenttype = INFO_CURRENTTYPE(arg_info);

    // check if the type of the param is the same as the type of the expression
    if (currenttype != T_unknown) {
      if (currenttype != SYMBOLENTRY_TYPE(paramNode)) {
        CTIerror( "Expected parameter of type %s but got %s", TypePrinter(SYMBOLENTRY_TYPE(paramNode)), TypePrinter(currenttype));
      }
    }

    temp = TRAVopt(EXPRS_NEXT(temp), arg_info);

  }

  INFO_CURRENTTYPE(arg_info) = SYMBOLENTRY_TYPE(GetNode(STRcpy(FUNCALL_NAME(arg_node)), arg_info));
  // reset paramcounter
  INFO_PARAMCOUNTER(arg_info) = 0;
  DBUG_RETURN(arg_node);
}

node * TCexprs(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCexprs");

  // Traverse through expression



  EXPRS_NEXT(arg_node) = TRAVopt(EXPRS_NEXT(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}


node *TCfunbody(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCfunbody");

  FUNBODY_STMTS(arg_node) = TRAVopt(FUNBODY_STMTS(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}

node *TCifelse(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCifelse");

  // Check node type of condition
  IFELSE_COND(arg_node) = TRAVdo(IFELSE_COND(arg_node), arg_info);
  type currenttype = INFO_CURRENTTYPE(arg_info);

  if(currenttype != T_bool) {
    CTIerror( "Type %s does not match required type %s", TypePrinter(currenttype), TypePrinter(T_bool));
  }

  // Traverse down then
  IFELSE_THEN(arg_node) = TRAVopt(IFELSE_THEN(arg_node),arg_info);

  // Traverse down else
  IFELSE_ELSE(arg_node) = TRAVopt(IFELSE_ELSE(arg_node),arg_info);

  DBUG_RETURN(arg_node);
}


node *TCdowhile(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCdowhile");

  DOWHILE_COND(arg_node) = TRAVdo(DOWHILE_COND(arg_node), arg_info);
  type currenttype = INFO_CURRENTTYPE(arg_info);

  // Check node type of boolean
  if(currenttype != T_bool) {
      CTIerror( "Type %s does not match required type %s", TypePrinter(currenttype), TypePrinter(T_bool));
  }

  // Traverse down block
  DOWHILE_BLOCK(arg_node) = TRAVopt(DOWHILE_BLOCK(arg_node),arg_info);

  DBUG_RETURN(arg_node);
}


node *TCwhile(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCwhile");

  WHILE_COND(arg_node) = TRAVdo(WHILE_COND(arg_node), arg_info);
  type currenttype = INFO_CURRENTTYPE(arg_info);

  // Check node type of condition
  if(currenttype != T_bool) {
    CTIerror( "Type %s does not match required type %s", TypePrinter(currenttype), TypePrinter(T_bool));
  }

  // Traverse down block
  WHILE_BLOCK(arg_node) = TRAVopt(WHILE_BLOCK(arg_node),arg_info);

  DBUG_RETURN(arg_node);
}


node *TCfor(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCfor");

  FOR_START(arg_node) = TRAVdo(FOR_START(arg_node), arg_info);
  type starttype = INFO_CURRENTTYPE(arg_info);

  FOR_STEP(arg_node) = TRAVopt(FOR_STEP(arg_node), arg_info);
  type steptype = INFO_CURRENTTYPE(arg_info);

  FOR_STOP(arg_node) = TRAVdo(FOR_STOP(arg_node), arg_info);
  type stoptype = INFO_CURRENTTYPE(arg_info);

  //TODO: GOEDE ERROR AFHANDELING
  if(starttype != T_int) {
    CTIerror( "Type of start type %s does not match type Integer", TypePrinter(starttype));
  }

  if(steptype != T_int) {
    CTIerror( "Type of step type %s does not match type Integer", TypePrinter(steptype));
  }

  if(stoptype != T_int) {
    CTIerror( "Type of stop type %s does not match type Integer", TypePrinter(stoptype));
  }

  // Traverse down block
  FOR_BLOCK(arg_node) = TRAVopt(FOR_BLOCK(arg_node),arg_info);

  DBUG_RETURN(arg_node);
}


node * TCmonop(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCnum");

  monop op = MONOP_OP(arg_node);
  // Traverse to yield operand type to get ty[e]
  MONOP_OPERAND(arg_node) = TRAVdo(MONOP_OPERAND(arg_node), arg_info);
  type currenttype = INFO_CURRENTTYPE(arg_info);

  if ((op == MO_not && currenttype == T_bool) || (op == MO_neg && currenttype != T_bool))
  {
    CTIerror("Type of op %u does not match type %s", op, TypePrinter(currenttype));
  }

  DBUG_RETURN(arg_node);
}


node *TCbinop(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCbinop");

  BINOP_LEFT(arg_node) = TRAVdo(BINOP_LEFT(arg_node), arg_info);
  type lefttype = INFO_CURRENTTYPE(arg_info);

  BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);
  type righttype = INFO_CURRENTTYPE(arg_info);

  if(righttype != lefttype)
  {
    CTIerror( "Type of left expression %s does not match right type of right expression %s", TypePrinter(lefttype), TypePrinter(righttype));
  } else
  {
    INFO_CURRENTTYPE(arg_info) = lefttype;
  }

  DBUG_RETURN(arg_node);
}


node *TCassign(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCassign");

  ASSIGN_EXPR(arg_node) = TRAVopt(ASSIGN_EXPR(arg_node), arg_info);
  type currenttype = INFO_CURRENTTYPE(arg_info);
  CTInote("Linked variable %s to global scope",TypePrinter(currenttype));

  node *symbolnode = GetNode(VARLET_NAME(ASSIGN_LET(arg_node)), arg_info);
  type nodetype = SYMBOLENTRY_TYPE(symbolnode);
  CTInote("Linked variable %s to global scope",TypePrinter(nodetype));

  if(nodetype != currenttype)
  {
    CTIerror( "Type of variable %s does not match assigned type %s", TypePrinter(nodetype), TypePrinter(currenttype));
  }

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
