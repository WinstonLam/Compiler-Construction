#include "gen_byte_code.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include "memory.h"
#include "ctinfo.h"
#include "globals.h"
#include "linkedlist.h"

struct INFO {
  node *symboltable;
  type currenttype;
  int count;
  FILE *file;

  // linkedlist *externs;
  linkedlist *import;
  linkedlist *export;
  linkedlist *constant;
};

#define INFO_SYMBOLTABLE(n) ((n)->symboltable)
#define INFO_CURRENTTYPE(n) ((n)->currenttype)
#define INFO_COUNT(n) ((n)->count)
#define INFO_FILE(n) ((n)->file)
#define INFO_GLOBAL(n) ((n)->export)
#define INFO_CONSTANT(n) ((n)->export)
#define INFO_IMPORT(n) ((n)->export)
#define INFO_EXPORT(n) ((n)->export)

static info *MakeInfo(void)
{
  info *tables;

  DBUG_ENTER("MakeInfo");

  tables = (info *)MEMmalloc(sizeof(info));

  INFO_SYMBOLTABLE(tables) = NULL;
  INFO_CURRENTTYPE(tables) = T_unknown;
  INFO_COUNT(tables) = 0;
  INFO_GLOBAL(tables) = NULL;
  INFO_CONSTANT(tables) = NULL;
  INFO_IMPORT(tables) = NULL;
  INFO_EXPORT(tables) = NULL;

  DBUG_RETURN(tables);
}


static info *FreeInfo( info *info)
{
  DBUG_ENTER ("FreeInfo");

  info = MEMfree(info);

  // TODO: Clean others

  DBUG_RETURN(info);
}


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


static void printGlobals(info *arg_info)
{
  FILE *file = INFO_FILE(arg_info);

  linkedlist *global = INFO_GLOBAL(arg_info);
  while(global)
  {
    fprintf(file, ".global %s\n", global->data);
    global = global->next;
  }

  linkedlist *constant = INFO_CONSTANT(arg_info);
  while(constant)
  {
    fprintf(file, ".const %s\n", constant->data);
    constant = constant->next;
  }

  linkedlist *import = INFO_IMPORT(arg_info);
  while(import)
  {
    fprintf(file, ".import%s\n", import->data);
    import = import->next;
  }

  linkedlist *export = INFO_EXPORT(arg_info);
  while(export)
  {
    fprintf(file, ".export%s\n", export->data);
    export = export->next;
  }
}

node *GBCprogram(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCprogram");

  INFO_SYMBOLTABLE(arg_info) = PROGRAM_SYMBOLENTRY(arg_node);
  TRAVdo(PROGRAM_DECLS(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}


// node *GBCdecls(node *arg_node, info *arg_info)
// {
//   DBUG_ENTER("GBCdecls");

//   DECLS_DECL(arg_node) = TRAVdo(DECLS_DECL(arg_node), arg_info);
//   DECLS_NEXT(arg_node) = TRAVdo(DECLS_NEXT(arg_node), arg_info);

//   DBUG_RETURN("GBCprogram");
// }

node *GBCglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCglobdef");

  if(GLOBDEF_ISEXPORT(arg_node)) {
    //TODO: FIX INDEX
    int index = 0;
    char *string = STRcatn(4, "var \"", GLOBDECL_NAME(arg_node), "\" ", index);

    // Add to export list
    PushIfExistElseCreate(INFO_EXPORT(arg_info), string);
  }

  GLOBDEF_DIMS(arg_node) = TRAVopt(GLOBDEF_DIMS(arg_node),arg_info);
  GLOBDEF_INIT(arg_node) = TRAVopt(GLOBDEF_INIT(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}


node *GBCglobdecl(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCglobdecl");

  //TODO: FIX INDEX
  int index = 0;
  char *string = STRcatn(4, "var \"", GLOBDECL_NAME(arg_node), "\" ", index);

  // Add to export list
  PushIfExistElseCreate(INFO_EXPORT(arg_info), string);

  GLOBDECL_DIMS(arg_node) = TRAVopt(GLOBDECL_DIMS(arg_node),arg_info);

  DBUG_RETURN(arg_node);
}


// node *GBCexprs(node *arg_node, info *arg_info)
// {
//   DBUG_ENTER("GBCexprs");

//   INFO_SYMBOLTABLE(arg_info) = TRAVdo(EXPRS_EXPR(arg_node), arg_info);
//   INFO_SYMBOLTABLE(arg_info) = TRAVopt(EXPRS_NEXT(arg_node), arg_info);

//   DBUG_RETURN("GBCexprs");
// }


// node *PRTarrexpr(node *arg_node, info *arg_info)
// {
//   DBUG_ENTER("PRTarrexpr");

//   ARREXPR_EXPRS(arg_node) = TRAVdo(ARREXPR_EXPRS(arg_node), arg_info);

//   DBUG_RETURN(arg_node);
// }


// node *GBCids(node *arg_node, info *arg_info)
// {
//   DBUG_ENTER("GBCids");

//   IDS_NEXT(arg_node) = TRAVopt(IDS_NEXT(arg_node), arg_info);

//   DBUG_RETURN(arg_node);
// }


// node *GBCexprstmt(node *arg_node, info *arg_info)
// {
//   DBUG_ENTER("GBCexprstmt");

//   EXPRSTMT_EXPR(arg_node) = TRAVopt(EXPRSTMT_EXPR(arg_node), arg_info);

//   DBUG_RETURN(arg_node);
// }

// node *GBCfundef(node *arg_node, info *arg_info)
// {
//   DBUG_ENTER("GBCfundef");
//   // FUNDEF_ISEXPORT()
//   DBUG_RETURN(arg_node);
// }

node *GBCfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCfuncall");

  // node *symboltableEntry =

  // TODO: shouldnt be global if we have inner function extensions? I think
  fprintf(INFO_FILE(arg_info), "  isrg\n");

  FUNCALL_ARGS(arg_node) = TRAVopt(FUNCALL_ARGS(arg_node), arg_info);

  // TODO: AFTER TRAVERSAL DO jsr OR jsre DEPENDING ON IF ITS EXTERN

  DBUG_RETURN(arg_node);
}


node *GBCreturn(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCreturn");

  RETURN_EXPR(arg_node) = TRAVopt(RETURN_EXPR(arg_node), arg_info);

  const char *returntype;

  type symboltype = T_int; // TODO: GET ACTUAL RETURN TYPE

  switch (symboltype)
  {
    case T_int:
      returntype = "ireturn";
      break;

    case T_float:
      returntype = "freturn";
      break;

    case T_bool:
      returntype = "breturn";
      break;

    case T_void:
      returntype = "return";
      break;

    case T_unknown:
      CTIabortLine(NODE_LINE(arg_node), "Unknown type found, aborting byte code generation");
      break;
  }

  fprintf(INFO_FILE(arg_info), " %s\n", returntype);

  DBUG_RETURN(arg_node);
}


node *GBCifelse(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCifelse");

  TRAVdo(IFELSE_COND(arg_node), arg_info);

  // TODO: Generate else and then


  DBUG_RETURN(arg_node);
}


node *GBCfor(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCfor");

  // TODO: ZOU DIT EEN WHILE MOETEN ZIJN?

  DBUG_RETURN(arg_node);
}


node *GBCwhile(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCwhile");

  //TODO:

  DBUG_RETURN(arg_node);
}


node *GBCassign(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCassign");

  ASSIGN_LET(arg_node) = TRAVdo(ASSIGN_LET(arg_node), arg_info);
  ASSIGN_EXPR(arg_node) = TRAVdo(ASSIGN_EXPR(arg_node), arg_info);

  // TODO: GET NODE OF LET
  node *symboltableentry = NULL;
  type nodetype = SYMBOLENTRY_TYPE(symboltableentry);
  int offset = 0; // TODO: FIX OFFSET

  // TODO: CHECK FI THEY ARE GLOBAL??
  switch (nodetype)
  {
  case T_int:
    fprintf(INFO_FILE(arg_info), "  istore %d\n", offset);
    break;

  case T_float:
    fprintf(INFO_FILE(arg_info), "  fstore %d\n", offset);
    break;

  case T_bool:
    fprintf(INFO_FILE(arg_info), "  bstore %d\n", offset);
    break;

  default:
    break;
  }

  DBUG_RETURN(arg_node);
}


node *GBCbinop(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCbinop");

  BINOP_LEFT(arg_node) = TRAVdo(BINOP_LEFT(arg_node), arg_info);
  BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);

  const binop op = BINOP_OP(arg_node);
  const type currenttype = INFO_CURRENTTYPE(arg_info);
  const char *opstring;
  const char *typestring;

  switch (op)
  {
    case BO_add:
      opstring = "add";
      break;

    case BO_sub:
      opstring = "sub";
      break;

    case BO_mul:
      opstring = "mul";
      break;

    case BO_div:
      opstring = "div";
      break;

    case BO_lt:
      opstring = "lt";
      break;

    case BO_le:
      opstring = "le";
      break;

    case BO_gt:
      opstring = "gt";
      break;

    case BO_ge:
      opstring = "ge";
      break;

    case BO_eq:
      opstring = "eq";
      break;

    case BO_ne:
      opstring = "ne";
      break;

    // TODO: klopt dit?
    case BO_and:
      opstring = "mul";
      break;

    case BO_or:
      opstring = "add";
      break;

    case BO_unknown:
      CTIabortLine(NODE_LINE(arg_node), "Unknown Binop operator found, aborting Byte code generation");
      break;

    default:
      break;
  }

  switch (currenttype)
  {
    case T_int:
      typestring = "i";
      break;

    case T_float:
      typestring = "f";
      break;

    case T_bool:
      typestring = "b";
      break;

    default:
      break;
  }

  fprintf(INFO_FILE(arg_info), "  %s%s\n", typestring, opstring);

  DBUG_RETURN(arg_node);
}


node *GBCmonop(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCmonop");

  MONOP_OPERAND(arg_node) = TRAVdo(MONOP_OPERAND(arg_node), arg_info);

  const monop op = MONOP_OP(arg_node);
  const type currenttype = INFO_CURRENTTYPE(arg_info);
  const char *opstring;
  const char *typestring;

  switch (op)
  {
    case MO_not:
      opstring = "not";
      break;

    case MO_neg:
      opstring = "neg";
      break;

    case MO_unknown:
      CTIabortLine(NODE_LINE(arg_node), "Unknown Monop operator found, aborting Byte code generation");
      break;

    default:
      break;
  }

  switch (currenttype)
  {
    case T_int:
      typestring = "i";
      break;

    case T_float:
      typestring = "f";
      break;

    case T_bool:
      typestring = "b";
      break;

    default:
      break;
  }

  fprintf(INFO_FILE(arg_info), "  %s%s\n", typestring, opstring);

  DBUG_RETURN(arg_node);
}

node *GBCcast(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCcast");

  CAST_EXPR(arg_node) = TRAVdo(CAST_EXPR(arg_node), arg_info);
  const type currenttype = CAST_TYPE(arg_node);
  char *opstring;

  switch (currenttype)
  {
    case T_int:
      opstring = "f2i";
      break;

    case T_float:
      opstring = "i2f";
      break;

    case T_bool:
      // TODO: I DONT THINK THERES AN OPERATION TO CONVERT BOOLEANS?
      // opstring = "f2i"
      break;

    default:
      break;
  }

  fprintf(INFO_FILE(arg_info), "  %s\n", opstring);

  DBUG_RETURN(arg_node);
}

node *GBCnum(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCnum");

  // char *string = strcat("int ", STRitoa(NUM_VALUE(arg_node)));
  fprintf(INFO_FILE(arg_info), " iloadc %d\n", INFO_COUNT(arg_info));
  // TODO: UP COUNT

  INFO_CURRENTTYPE(arg_info) = T_int;

  DBUG_RETURN(arg_node);
}


node *GBCfloat(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCfloat");

  // char *string = strcat("int ", STRitoa(FLOAT_VALUE(arg_node)));
  fprintf(INFO_FILE(arg_info), " floadc %d\n", INFO_COUNT(arg_info));
  // TODO: UP COUNT

  INFO_CURRENTTYPE(arg_info) = T_float;

  DBUG_RETURN(arg_node);
}


node *GBCbool(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCbool");

  fprintf(INFO_FILE(arg_info), " bloadc %d\n", INFO_COUNT(arg_info));
  // TODO: UP COUNT

  INFO_CURRENTTYPE(arg_info) = T_bool;

  DBUG_RETURN(arg_node);
}


node *GBCvar(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCvar");

  //TODO: FIND SYMBOLTABLE
  node *vardecl = VAR_DECL(arg_node);
  node *symboltableEntry = GetNode(VARDECL_NAME(vardecl), arg_info);

  INFO_CURRENTTYPE(arg_info) = SYMBOLENTRY_TYPE(symboltableEntry);

  if(NODE_TYPE(vardecl) == N_globdef)
  {

  }

  DBUG_RETURN(arg_node);
}

/*
 * INFO structure
 */

// struct INFO {
//   node *symboltable;
//   // TODO: Constant pool
// };

/*
 * Traversal start function
 */
node *GBCdoGenByteCode( node *syntaxtree)
{
  DBUG_ENTER("GBCdoGenByteCode");

  DBUG_ASSERT((syntaxtree != NULL), "GBCdoGenByteCode called with empty syntaxtree");

  info *arg_info = MakeInfo();

  // Check if global output file is set, otherwise use stdout
  if(global.outfile != NULL)
  {
    INFO_FILE(arg_info) = fopen(global.outfile, "w");
  } else
  {
    INFO_FILE(arg_info) = stdout;
  }

  TRAVpush(TR_tc);
  syntaxtree = TRAVdo(syntaxtree, arg_info);
  TRAVpop();

  // printGlobals(arg_info);
  arg_info = FreeInfo(arg_info);

  DBUG_RETURN(syntaxtree);
}
