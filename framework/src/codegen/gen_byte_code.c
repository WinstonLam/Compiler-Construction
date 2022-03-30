#include "gen_byte_code.h"
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
  // TODO: Constant pool
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

node *PRTstmts(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTstmts");

  // Traverse to last statement first
  TRAVopt(STMTS_NEXT(arg_node), arg_info);
  TRAVopt(STMTS_STMT(arg_node), arg_info);

  DBUG_RETURN(arg_info);
}

node *PRTassign(node *arg_node, info *arg_info)
{
  DBUG_ENTER("PRTassign");

  if (ASSIGN_LET(arg_node) != NULL)
  {
    // printf("istore %s", cf_index);
    ASSIGN_LET(arg_node) = TRAVdo(ASSIGN_LET(arg_node), arg_info);
    printf(" = ");
  }

  ASSIGN_EXPR(arg_node) = TRAVdo(ASSIGN_EXPR(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}

/*
 * Traversal start function
 */
node *GBCdoGenByteCode( node *syntaxtree)
{
  DBUG_ENTER("GBCdoGenByteCode");

  DBUG_ASSERT((syntaxtree != NULL), "GBCdoGenByteCode called with empty syntaxtree");

  printf("\n\n------------------------------\n\n");

  // START WITH TREE
  // NAIVELY!!!!!!!!!!
  // INTERMEDIATE REPRESENTATION TO TARGET LANGUAGE
  info *arg_info;
  arg_info = MakeInfo();

  TRAVpush(TR_prt);

  syntaxtree = TRAVdo(syntaxtree, arg_info);

  arg_info = FreeInfo(arg_info);

  printf("\n------------------------------\n\n");

  DBUG_RETURN( syntaxtree);
}
