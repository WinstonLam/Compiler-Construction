
/**
 * @file print.c
 *
 * Functions needed by print traversal.
 *
 */

/**
 * @defgroup print Print Functions.
 *
 * Functions needed by print traversal.
 *
 * @{
 */

#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"

/*
 * INFO structure
 */
struct INFO
{
  bool firsterror;
};

#define INFO_FIRSTERROR(n) ((n)->firsterror)

static info *MakeInfo()
{
  info *result;

  result = MEMmalloc(sizeof(info));

  INFO_FIRSTERROR(result) = FALSE;

  return result;
}

static info *FreeInfo(info *info)
{
  info = MEMfree(info);

  return info;
}
/** <!--******************************************************************-->
 *
 * @fn PRTprogram
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTprogram(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTprogram");

  PROGRAM_DECLS(arg_node) = TRAVdo(PROGRAM_DECLS(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTdecls
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTdecls(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTdecls");

  DECLS_DECL(arg_node) = TRAVdo(DECLS_DECL(arg_node), arg_info);

  DECLS_NEXT(arg_node) = TRAVopt(DECLS_NEXT(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTexprs
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTexprs(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTexprs");

  EXPRS_EXPR(arg_node) = TRAVdo(EXPRS_EXPR(arg_node), arg_info);
  EXPRS_NEXT(arg_node) = TRAVdo(EXPRS_NEXT(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTarrexpr
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTarrexpr(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTarrexpr");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTids
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTids(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTids");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTexprstmt
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTexprstmt(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTexprstmt");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTreturn
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTreturn(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTreturn");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTfuncall
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTfuncall");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTcast
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTcast(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTcast");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTfundefs
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTfundefs(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTfundefs");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTfundef
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTfundef(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTfundef");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTfunbody
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTdecls(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTfunbody");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTifelse
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTifelse(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTifelse");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTdowhile
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process ??
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTdowhile(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTwhile");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTfor
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process ??
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTfor(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTfor");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTglobdecl
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process ??
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTglobdecl(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTglobdecl");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTglobdef
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process ??
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTglobdef");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTparam
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process ??
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTparam(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTparam");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTvardecl
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process ??
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTvardecl(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTvardecl");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTmonop
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node SOME node to process ??
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTmonop(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTmonop");

  DBUG_RETURN(arg_node);
}

/**
 * VANAF HIER IS HET AL BESCHREVEN
 *  * VANAF HIER IS HET AL BESCHREVEN
 *  * VANAF HIER IS HET AL BESCHREVEN
 *  * VANAF HIER IS HET AL BESCHREVEN
 *  * VANAF HIER IS HET AL BESCHREVEN
 *
 * VANAF HIER IS HET AL BESCHREVEN
 *
 */

/** <!--******************************************************************-->
 *
 * @fn PRTassign
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTassign(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTassign");

  if (ASSIGN_LET(arg_node) != NULL)
  {
    ASSIGN_LET(arg_node) = TRAVdo(ASSIGN_LET(arg_node), arg_info);
    printf(" = ");
  }

  ASSIGN_EXPR(arg_node) = TRAVdo(ASSIGN_EXPR(arg_node), arg_info);

  printf(";\n");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTmodule
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node ?? node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTmodule(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTmodule");

  printf("Number of addition artihmetics: %d\n", MODULE_ADD(arg_node));
  printf("Number of subtraction artihmetics: %d\n", MODULE_SUB(arg_node));
  printf("Number of multiplication artihmetics: %d\n", MODULE_MUL(arg_node));
  printf("Number of division artihmetics: %d\n", MODULE_DIV(arg_node));
  printf("Number of modulo artihmetics: %d", MODULE_MOD(arg_node));

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTbinop
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTbinop(node *arg_node, info *arg_info)
{
  char *tmp;

  DBUG_ENTER("PRTbinop");

  printf("( ");

  BINOP_LEFT(arg_node) = TRAVdo(BINOP_LEFT(arg_node), arg_info);

  switch (BINOP_OP(arg_node))
  {
  case BO_add:
    tmp = "+";
    break;
  case BO_sub:
    tmp = "-";
    break;
  case BO_mul:
    tmp = "*";
    break;
  case BO_div:
    tmp = "/";
    break;
  case BO_mod:
    tmp = "%";
    break;
  case BO_lt:
    tmp = "<";
    break;
  case BO_le:
    tmp = "<=";
    break;
  case BO_gt:
    tmp = ">";
    break;
  case BO_ge:
    tmp = ">=";
    break;
  case BO_eq:
    tmp = "==";
    break;
  case BO_ne:
    tmp = "!=";
    break;
  case BO_or:
    tmp = "||";
    break;
  case BO_and:
    tmp = "&&";
    break;
  case BO_unknown:
    DBUG_ASSERT(0, "unknown binop detected!");
  }

  printf(" %s ", tmp);

  BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);

  printf(")");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTfloat
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Float node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTfloat(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTfloat");

  printf("%f", FLOAT_VALUE(arg_node));

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTnum
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Num node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTnum(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTnum");

  printf("%i", NUM_VALUE(arg_node));

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTboolean
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Boolean node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTbool(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTbool");

  if (BOOL_VALUE(arg_node))
  {
    printf("true");
  }
  else
  {
    printf("false");
  }

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTvar
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTvar(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTvar");

  printf("%s", VAR_NAME(arg_node));

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTvarlet
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTvarlet(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTvarlet");

  printf("%s", VARLET_NAME(arg_node));

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTsymboltableentry
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *PRTsymboltableentry(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTsymboltableentry");

  DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTerror
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTerror(node *arg_node, info *arg_info)
{
  bool first_error;

  DBUG_ENTER("PRTerror");

  if (NODE_ERROR(arg_node) != NULL)
  {
    NODE_ERROR(arg_node) = TRAVdo(NODE_ERROR(arg_node), arg_info);
  }

  first_error = INFO_FIRSTERROR(arg_info);

  if ((global.outfile != NULL) && (ERROR_ANYPHASE(arg_node) == global.compiler_anyphase))
  {

    if (first_error)
    {
      printf("\n/******* BEGIN TREE CORRUPTION ********\n");
      INFO_FIRSTERROR(arg_info) = FALSE;
    }

    printf("%s\n", ERROR_MESSAGE(arg_node));

    if (ERROR_NEXT(arg_node) != NULL)
    {
      TRAVopt(ERROR_NEXT(arg_node), arg_info);
    }

    if (first_error)
    {
      printf("********  END TREE CORRUPTION  *******/\n");
      INFO_FIRSTERROR(arg_info) = TRUE;
    }
  }

  DBUG_RETURN(arg_node);
}

/** <!-- ****************************************************************** -->
 * @brief Prints the given syntaxtree
 *
 * @param syntaxtree a node structure
 *
 * @return the unchanged nodestructure
 ******************************************************************************/

node
    *
    PRTdoPrint(node *syntaxtree)
{
  info *info;

  DBUG_ENTER("PRTdoPrint");

  DBUG_ASSERT((syntaxtree != NULL), "PRTdoPrint called with empty syntaxtree");

  printf("\n\n------------------------------\n\n");

  info = MakeInfo();

  TRAVpush(TR_prt);

  syntaxtree = TRAVdo(syntaxtree, info);

  TRAVpop();

  info = FreeInfo(info);

  printf("\n------------------------------\n\n");

  DBUG_RETURN(syntaxtree);
}

/**
 * @}
 */
