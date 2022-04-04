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
};

/*
 * INFO macros
 */
#define INFO_SYMBOLTABLE(n)  ((n)->symboltable)
#define INFO_CURRENTTYPE(n)  ((n)->currenttype)

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

  // Traverse into the funbody
  FUNDEF_FUNBODY(arg_node) = TRAVopt(FUNDEF_FUNBODY(arg_node), arg_info);

  // reset global scope symboltable
  INFO_SYMBOLTABLE(arg_info) = globaltable;

  DBUG_RETURN(arg_node);
}


node *TCfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCfuncall");

  FUNCALL_ARGS(arg_node) = TRAVopt(FUNCALL_ARGS(arg_node), arg_info);

  node *funcallNode = GetNode(FUNCALL_NAME(arg_node), arg_info);

  // for each funcall arg
  // traverse through arg (/expr)
  // retrieve type

  // SYMBOLTABLE_
  INFO_CURRENTTYPE(arg_info) = SYMBOLENTRY_TYPE(GetNode(FUNCALL_NAME(arg_node), arg_info));

  DBUG_RETURN(arg_node);
}


node * TCexprs(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCexprs");

  // Traverse through expression
  EXPRS_EXPR(arg_node) = TRAVdo(EXPRS_EXPR(arg_node), arg_info);
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
