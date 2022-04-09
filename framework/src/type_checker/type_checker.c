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
#include "context_analysis.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include "free.h"
#include "memory.h"
#include "ctinfo.h"
#include "copy.h"

/*
 * INFO structure
 */

struct INFO {
  node *symboltable;
  node *parenttable;
  type currenttype;
  type binoptype;
  type fundeftype;
};

/*
 * INFO macros
 */
#define INFO_SYMBOLTABLE(n)  ((n)->symboltable)
#define INFO_PARENTTABLE(n) ((n)->parenttable)
#define INFO_CURRENTTYPE(n)  ((n)->currenttype)
#define INFO_BINOPTYPE(n)  ((n)->binoptype)
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
  INFO_PARENTTABLE(tables) = NULL;
  INFO_CURRENTTYPE(tables) = T_unknown;
  INFO_BINOPTYPE(tables) = T_unknown;
  INFO_FUNDEFTYPE(tables) = T_unknown;

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

  if(!INFO_SYMBOLTABLE(arg_info)) {
      CTInote("EMPTY SYMBOL TABLE");
  }

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
  INFO_PARENTTABLE(arg_info) = globaltable;


  INFO_CURRENTTYPE(arg_info) = FUNDEF_TYPE(arg_node);
  INFO_FUNDEFTYPE(arg_info) = FUNDEF_TYPE(arg_node);
  // Traverse into the funbody
  FUNDEF_FUNBODY(arg_node) = TRAVopt(FUNDEF_FUNBODY(arg_node), arg_info);

  // reset global scope symboltable
  INFO_SYMBOLTABLE(arg_info) = globaltable;
  INFO_PARENTTABLE(arg_info) = NULL;

  DBUG_RETURN(arg_node);
}

node *TCreturn(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCreturn");

  if (!RETURN_EXPR(arg_node) && INFO_FUNDEFTYPE(arg_info) == T_void)
    {
        DBUG_RETURN(arg_node);
    }

  // traverse into the return expression
  RETURN_EXPR(arg_node) = TRAVopt(RETURN_EXPR(arg_node), arg_info);

  CTInote("TCreturn: %s", TypePrinter(INFO_CURRENTTYPE(arg_info)));

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
  node *temp = TRAVopt(FUNCALL_ARGS(arg_node), arg_info);


  node *param = SYMBOLENTRY_PARAMS(GetNode(FUNCALL_NAME(arg_node), INFO_SYMBOLTABLE(arg_info), arg_node, INFO_PARENTTABLE(arg_info)));

  // check for each param if the given type corresponds
  while (temp) {
  
    if(!param) {
      CTIerror("called function %s with too many arguments", FUNCALL_NAME(arg_node));
    }

    // traverse down the expressions of the parameter
    TRAVdo(EXPRS_EXPR(temp), arg_info);
    type temptype = INFO_CURRENTTYPE(arg_info);
    type paramtype = PARAM_TYPE(param);
   
    // check if the type of the param is the same as the type of the expression
    if (temptype != T_unknown) {
      if (temptype != paramtype) {
        CTIerror( "Expected parameter of type %s but got %s", TypePrinter(paramtype), TypePrinter(temptype));
      }
    }
    param = TRAVopt(PARAM_NEXT(param), arg_info);
    temp = TRAVopt(EXPRS_NEXT(temp), arg_info);
  }

  INFO_CURRENTTYPE(arg_info) = SYMBOLENTRY_TYPE(GetNode(STRcpy(FUNCALL_NAME(arg_node)), INFO_SYMBOLTABLE(arg_info), arg_node,INFO_PARENTTABLE(arg_info)));

  DBUG_RETURN(arg_node);
}

node * TCexprs(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCexprs");

  // Traverse through expressions
  EXPRS_EXPR(arg_node) = TRAVdo(EXPRS_EXPR(arg_node), arg_info);
  EXPRS_NEXT(arg_node) = TRAVopt(EXPRS_NEXT(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}


node *TCfunbody(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCfunbody");
  FUNBODY_VARDECLS(arg_node) = TRAVopt(FUNBODY_VARDECLS(arg_node), arg_info);
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

node *TCcast(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCcast");

  // Traverse down the expression
  INFO_CURRENTTYPE(arg_info) = CAST_TYPE(arg_node);

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
  DBUG_ENTER("TCmonop");

  monop op = MONOP_OP(arg_node);
  // Traverse to yield operand type to get ty[e]
  MONOP_OPERAND(arg_node) = TRAVdo(MONOP_OPERAND(arg_node), arg_info);
  type currenttype = INFO_CURRENTTYPE(arg_info);

  if ((op == MO_not && currenttype != T_bool) || (op == MO_neg && currenttype == T_bool))
  {
    CTIerror("Type of op %u does not match type %s", op, TypePrinter(currenttype));
  }

  DBUG_RETURN(arg_node);
}


node *TCbinop(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCbinop");

  BINOP_LEFT(arg_node) = TRAVopt(BINOP_LEFT(arg_node), arg_info);
  type lefttype = INFO_CURRENTTYPE(arg_info);

  BINOP_RIGHT(arg_node) = TRAVopt(BINOP_RIGHT(arg_node), arg_info);

  type righttype = INFO_CURRENTTYPE(arg_info);

  if(righttype != lefttype)
  {
    CTIerror( "Type of left expression %s does not match right type of right expression %s", TypePrinter(lefttype), TypePrinter(righttype));
  }
  binop bop = (BINOP_OP(arg_node));
    if (bop == BO_lt || bop == BO_le || bop == BO_gt || bop == BO_ge || bop == BO_eq || bop == BO_ne || bop == BO_or || bop == BO_and) {
      INFO_CURRENTTYPE(arg_info) = T_bool;
    }

  DBUG_RETURN(arg_node);
}


node *TCassign(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCassign");
  type let = SYMBOLENTRY_TYPE(GetNode(VARLET_NAME(ASSIGN_LET(arg_node)),INFO_SYMBOLTABLE(arg_info), arg_node, INFO_PARENTTABLE(arg_info)));

  ASSIGN_EXPR(arg_node) = TRAVopt(ASSIGN_EXPR(arg_node), arg_info);
  type currenttype = INFO_CURRENTTYPE(arg_info);
  if (ASSIGN_LET(arg_node)) {

    if(let != currenttype)
    {
      CTIerror( "Type of variable %s does not match assigned type %s", TypePrinter(SYMBOLENTRY_TYPE(VARLET_TABLELINK(ASSIGN_LET(arg_node)))), TypePrinter(currenttype));
    }
  }
  DBUG_RETURN(arg_node);
}

node *TCvar(node *arg_node, info *arg_info)
{
  DBUG_ENTER("TCvar");
  DBUG_PRINT("TC", ("var %s", VAR_NAME(arg_node)));
  CTInote("checking var %s", VAR_NAME(arg_node));
  node *node = GetNode(VAR_NAME(arg_node), INFO_SYMBOLTABLE(arg_info),arg_node,INFO_PARENTTABLE(arg_info));

  CTInote("var type %s", TypePrinter(SYMBOLENTRY_TYPE(node)));
  
  INFO_CURRENTTYPE(arg_info) = SYMBOLENTRY_TYPE(node);
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
  DBUG_ENTER("TCbool");

  INFO_CURRENTTYPE(arg_info) = T_bool;

  DBUG_RETURN(arg_node);
}


/*
 * Traversal start function
 */

node *TCdoTypeChecking( node *syntaxtree)
{
  DBUG_ENTER("TCdoTypeChecking");

  info *arg_info;

  arg_info = MakeInfo();

  TRAVpush( TR_tc);   // Push traversal "tc" as defined in ast.xml

  syntaxtree = TRAVdo( syntaxtree, arg_info);   // Initiate ast traversal

  TRAVpop();          // Pop current traversal

  arg_info = FreeInfo( arg_info);

  DBUG_RETURN( syntaxtree);
}
