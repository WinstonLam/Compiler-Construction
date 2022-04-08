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
#include "symboltable_linker.h"

struct INFO {
  node *symboltable;
  type currenttype;
  FILE *file;

  linkedlist *constant;
  int constantcount;
  int branchcount;

  linkedlist *global;
  linkedlist *import;
  linkedlist *export;
};

#define INFO_SYMBOLTABLE(n) ((n)->symboltable)
#define INFO_CURRENTTYPE(n) ((n)->currenttype)
#define INFO_FILE(n) ((n)->file)
#define INFO_CONSTANT(n) ((n)->constant)
#define INFO_CONSTANTCOUNT(n) ((n)->constantcount)
#define INFO_BRANCHCOUNT(n) ((n)->branchcount)

#define INFO_GLOBAL(n) ((n)->global)
#define INFO_IMPORT(n) ((n)->import)
#define INFO_EXPORT(n) ((n)->export)

static info *MakeInfo(void)
{
  info *tables;

  DBUG_ENTER("MakeInfo");

  tables = (info *)MEMmalloc(sizeof(info));

  INFO_SYMBOLTABLE(tables) = NULL;
  INFO_CURRENTTYPE(tables) = T_unknown;
  INFO_CONSTANTCOUNT(tables) = 0;
  INFO_BRANCHCOUNT(tables) = 0;

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
// static node *GetNode(char *name, info *arg_info)
// {
//     // traverse through the symbol table untill node is found
//     node *temp = INFO_SYMBOLTABLE(arg_info);
//     while (temp) {
//         if (STReq(SYMBOLENTRY_NAME(temp), name)) {
//             return temp;
//         }
//         temp = SYMBOLENTRY_NEXT(temp);
//     }

//     CTIerror("Could not be found in symbol table: %s", name);
//     return temp;
// }


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


node *GBCprogram(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCprogram");

  INFO_SYMBOLTABLE(arg_info) = PROGRAM_SYMBOLENTRY(arg_node);
  TRAVdo(PROGRAM_DECLS(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}

node *GBCglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCglobdef");

  if(GLOBDEF_ISEXPORT(arg_node)) {
    //TODO: FIX INDEX
    int index = 0;
    char *string = STRcatn(4, "var \"", GLOBDEF_NAME(arg_node), "\" ", index);

    // Add to export list
    INFO_EXPORT(arg_info) = PushIfExistElseCreate(INFO_EXPORT(arg_info), string);
  }

  GLOBDEF_DIMS(arg_node) = TRAVopt(GLOBDEF_DIMS(arg_node),arg_info);
  GLOBDEF_INIT(arg_node) = TRAVopt(GLOBDEF_INIT(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}


node *GBCglobdecl(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCglobdecl");

  // Add to import list
  INFO_GLOBAL(arg_info) = PushIfExistElseCreate(INFO_IMPORT(arg_info), TypePrinter(GLOBDECL_TYPE(arg_node)));

  GLOBDECL_DIMS(arg_node) = TRAVopt(GLOBDECL_DIMS(arg_node),arg_info);

  DBUG_RETURN(arg_node);
}

node *GBCvardecl(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCvardecl");

  if (VARDECL_INIT(arg_node) != NULL) {
    VARDECL_INIT(arg_node) = TRAVopt(VARDECL_INIT(arg_node), arg_info);
    const type currenttype = INFO_CURRENTTYPE(arg_info);
    // TODO: GET INDEX FROM SYMBOL TABLE TO STORE VAL IN CORRECT INDEX
    int index = 0;

    switch (currenttype)
      {
        case T_int:
          fprintf(INFO_FILE(arg_info), "    istore %d\n", index);
          break;

        case T_float:
          fprintf(INFO_FILE(arg_info), "    fstore %d\n", index);
          break;

        case T_bool:
          fprintf(INFO_FILE(arg_info), "    bstore %d\n", index);
          break;

        default:
          break;
      }
  }

  VARDECL_DIMS(arg_node) = TRAVopt(VARDECL_DIMS(arg_node), arg_info);
  VARDECL_NEXT(arg_node) = TRAVopt(VARDECL_NEXT(arg_node), arg_info);

  DBUG_RETURN(arg_node);
}

node *GBCfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCfuncall");


  fprintf(INFO_FILE(arg_info), "  isrg\n");

  FUNCALL_ARGS(arg_node) = TRAVopt(FUNCALL_ARGS(arg_node), arg_info);


  // node *symboltableEntry =
  // TODO: AFTER TRAVERSAL DO jsr OR jsre DEPENDING ON IF IT HAS A FUNDEC WHICH MEANS ITLL BE EXTERN

  DBUG_RETURN(arg_node);
}


node *GBCfundef(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCfundef");

  // node *paramnode = PARAM_TABLELINK(FUNDEF_PARAMS(arg_node));

  char *params = " TEST TEST ";
  // // TODO: FOR EACH PARAM (IF THEY EXIST) ADD IT TO PARAM STRING, ELSE MAKE IT " ". RETRIEVE PARAMS FROM SYMBOL TABLE
  // params = STRcatn(3, params, " ", TypePrinter(SYMBOLENTRY_TYPE(paramnode)));

  // int amountOfVarDecls = CountVarDecls(FUNDEF_TABLELINK(arg_node));
  // fprintf(INFO_FILE(arg_info), "  esr %d\n", amountOfVarDecls);

  // if(FUNDEF_ISEXPORT(arg_node))
  // {

    // char *string = STRcatn(8, "fun \"", FUNDEF_NAME(arg_node), "\" ", TypePrinter(FUNDEF_TYPE(arg_node)), " ", params, " ", FUNDEF_NAME(arg_node));

    // // Add to export list
    // PushIfExistElseCreate(INFO_EXPORT(arg_info), string);
  // }

  // TODO: NIET NODIG?? EXTERN AL IN FUNDECL EN GLOBDECL
  if(FUNDEF_ISEXTERN(arg_node))
  {
    int length = snprintf(NULL, 0, "fun \"%s\" %s %s", FUNDEF_NAME(arg_node), typeToString(FUNDEF_TYPE(arg_node)), params == NULL ? "" : params);
    int lengthPlus = length + 1;
    char *string = (char *)malloc(lengthPlus);

    snprintf(string, lengthPlus, "fun \"%s\" %s %s", FUNDEF_NAME(arg_node), typeToString(FUNDEF_TYPE(arg_node)), params == NULL ? "" : params);

    char *string = STRcatn(8, "fun \"", FUNDEF_NAME(arg_node), "\" ", TypePrinter(FUNDEF_TYPE(arg_node)), " ", params, " ", FUNDEF_NAME(arg_node));
    PushIfExistElseCreate(INFO_IMPORT(arg_info), string);
  } else
  {
    fprintf(INFO_FILE(arg_info), "%s:\n", FUNDEF_NAME(arg_node));


  }

  FUNDEF_PARAMS(arg_node) = TRAVopt(FUNDEF_PARAMS(arg_node), arg_info);

  FUNDEF_FUNBODY(arg_node) = TRAVopt(FUNDEF_FUNBODY(arg_node), arg_info);

  fprintf(INFO_FILE(arg_info), "\n");

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

  fprintf(INFO_FILE(arg_info), "    %s\n", returntype);

  DBUG_RETURN(arg_node);
}


char *generateBranch(const char *type, info *info)
{
    char *branch = STRcatn(3, STRitoa(INFO_BRANCHCOUNT(info)), "_", type);
    INFO_BRANCHCOUNT(info) ++;

    return branch;
}


node *GBCifelse(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCifelse");

  TRAVdo(IFELSE_COND(arg_node), arg_info);

  char *elseBranch;
  bool elseBool = IFELSE_ELSE(arg_node) != NULL;

  if (IFELSE_ELSE(arg_node) != NULL)
  {
    elseBranch = generateBranch("else", arg_info);
  }
  else
  {
    elseBranch = generateBranch("end", arg_info);
  }

  char *endBranch = IFELSE_ELSE(arg_node) ? generateBranch("end", arg_info) : elseBranch;
  fprintf(INFO_FILE(arg_info), "  branch_f %s\n\n", elseBranch);

  TRAVopt(IFELSE_THEN(arg_node), arg_info);

  if (elseBool)
  {
      fprintf(INFO_FILE(arg_info), "  jump %s\n\n", endBranch);
      fprintf(INFO_FILE(arg_info), "%s:\n", elseBranch);
      TRAVopt(IFELSE_ELSE(arg_node), arg_info);
      fputc('\n', INFO_FILE(arg_info));
  }

  fprintf(INFO_FILE(arg_info), "%s:\n", endBranch);
  free(elseBranch);

  if (elseBool)
  {
      free(endBranch);
  }

  DBUG_RETURN(arg_node);

}


node *GBCwhile(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCwhile");

  //TODO:

  DBUG_RETURN(arg_node);
}


node *GBCvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCvar");

    // TODO: GET TYPE FROM SYMBOL TABLE
    // TODO: FIX INDEX MISSCHIEN DOOR
    type symbolentrytype = T_int;
    int index = 0;

    switch (symbolentrytype)
    {
    case T_int:
      fprintf(INFO_FILE(arg_info), "    iload %d\n", index);
      break;

    case T_float:
      fprintf(INFO_FILE(arg_info), "    fload %d\n", index);
      break;

    case T_bool:
      fprintf(INFO_FILE(arg_info), "    bload %d\n", index);
      break;

    default:
      break;
    }

    // node *decl = VAR_DECL(arg_node);
    // node *symboltableEntry = STfindByNode(INFO_SYMBOL_TABLE(arg_info), decl);

    // INFO_TYPE(arg_info) = SYMBOLTABLEENTRY_TYPE(symboltableEntry);

    // if (NODE_TYPE(decl) == N_globdef)
    // {
    //     char scope = GLOBDEF_ISEXTERN(decl) ? 'e' : 'g';

    //     if (GLOBDEF_TYPE(decl) == T_int)
    //     {
    //         fprintf(INFO_FILE(arg_info), "  iload%c %d\n", scope, SYMBOLTABLEENTRY_OFFSET(symboltableEntry));
    //     }
    //     else if (GLOBDEF_TYPE(decl) == T_float)
    //     {
    //         fprintf(INFO_FILE(arg_info), "  fload%c %d\n", scope, SYMBOLTABLEENTRY_OFFSET(symboltableEntry));
    //     }
    // }
    // else
    // {
    //     switch (SYMBOLTABLEENTRY_TYPE(symboltableEntry))
    //     {
    //     case T_int:
    //         fprintf(
    //             INFO_FILE(arg_info),
    //             SYMBOLTABLEENTRY_OFFSET(symboltableEntry) < 4 ? "   iload_%d\n" : "    iload %d\n",
    //             SYMBOLTABLEENTRY_OFFSET(symboltableEntry));
    //         break;
    //     case T_float:
    //         fprintf(
    //             INFO_FILE(arg_info),
    //             SYMBOLTABLEENTRY_OFFSET(symboltableEntry) < 4 ? "   fload_%d\n" : "    fload %d\n",
    //             SYMBOLTABLEENTRY_OFFSET(symboltableEntry));
    //         break;
    //     case T_bool:
    //         fprintf(
    //             INFO_FILE(arg_info),
    //             SYMBOLTABLEENTRY_OFFSET(symboltableEntry) < 4 ? "   bload_%d\n" : "    bload %d\n",
    //             SYMBOLTABLEENTRY_OFFSET(symboltableEntry));
    //         break;
    //     case T_void:
    //         break;
    //     case T_unknown:
    //         break;
    //     }
    // }

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
  int index = 0; // TODO: FIX OFFSET


  switch (nodetype)
  {
    case T_int:
      fprintf(INFO_FILE(arg_info), "  istore %d\n", index);
      break;

    case T_float:
      fprintf(INFO_FILE(arg_info), "  fstore %d\n", index);
      break;

    case T_bool:
      fprintf(INFO_FILE(arg_info), "  bstore %d\n", index);
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

  fprintf(INFO_FILE(arg_info), "    %s%s\n", typestring, opstring);

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

  fprintf(INFO_FILE(arg_info), "    iloadc %d\n", INFO_CONSTANTCOUNT(arg_info));

  char *string = STRcat("int ", STRitoa(NUM_VALUE(arg_node)));
  INFO_CONSTANT(arg_info) = PushIfExistElseCreate(INFO_CONSTANT(arg_info), string);
  INFO_CONSTANTCOUNT(arg_info) ++;

  INFO_CURRENTTYPE(arg_info) = T_int;

  DBUG_RETURN(arg_node);
}


node *GBCfloat(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCfloat");

  fprintf(INFO_FILE(arg_info), "    floadc %d\n", INFO_CONSTANTCOUNT(arg_info));

  // Shamelessly stolen from https://stackoverflow.com/a/29087251/12106583
  int size = snprintf(NULL, 0, "float %f", FLOAT_VALUE(arg_node));
  char *buf = malloc(size + 1);

  snprintf(buf, size + 1, "float %f", FLOAT_VALUE(arg_node));

  INFO_CONSTANT(arg_info) = PushIfExistElseCreate(INFO_CONSTANT(arg_info), buf);
  INFO_CONSTANTCOUNT(arg_info) ++;

  INFO_CURRENTTYPE(arg_info) = T_float;

  DBUG_RETURN(arg_node);
}


node *GBCbool(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCbool");

  fprintf(INFO_FILE(arg_info), "    bloadc %d\n", INFO_CONSTANTCOUNT(arg_info));

  char *string = STRcat("bool ", BOOL_VALUE(arg_node) ? "true" : "false");
  INFO_CONSTANT(arg_info) = PushIfExistElseCreate(INFO_CONSTANT(arg_info), string);
  INFO_CONSTANTCOUNT(arg_info) ++;

  INFO_CURRENTTYPE(arg_info) = T_bool;

  DBUG_RETURN(arg_node);
}


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

  TRAVpush(TR_gbc);
  syntaxtree = TRAVdo(syntaxtree, arg_info);
  TRAVpop();

  printGlobals(arg_info);
  arg_info = FreeInfo(arg_info);

  DBUG_RETURN(syntaxtree);
}
