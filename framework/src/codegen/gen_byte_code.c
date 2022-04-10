#include "gen_byte_code.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include "memory.h"
#include "copy.h"
#include "ctinfo.h"
#include "globals.h"
#include "linkedlist.h"
#include "context_analysis.h"
#include "symboltable_linker.h"


struct INFO {
  node *symboltable;
  node *parenttable;

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
#define INFO_PARENTTABLE(n) ((n)->parenttable)
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
  INFO_FILE(tables) = NULL;

  INFO_SYMBOLTABLE(tables) = NULL;
  INFO_PARENTTABLE(tables) = NULL;

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

  // FreeLinkedlist(INFO_GLOBAL(info));
  // FreeLinkedlist(INFO_CONSTANT(info));
  // FreeLinkedlist(INFO_IMPORT(info));
  // FreeLinkedlist(INFO_EXPORT(info));

  DBUG_RETURN(info);
}

static void printGlobals(info *arg_info)
{
  FILE *file = INFO_FILE(arg_info);

  linkedlist *constant = INFO_CONSTANT(arg_info);
  while(constant)
  {
    fprintf(file, ".const %s\n", constant->string);
    constant = constant->next;
  }

  linkedlist *export = INFO_EXPORT(arg_info);
  while(export)
  {
    fprintf(file, ".export%s\n", export->string);
    export = export->next;
  }

  linkedlist *global = INFO_GLOBAL(arg_info);
  while(global)
  {
    fprintf(file, ".global %s\n", global->string);
    global = global->next;
  }

  linkedlist *import = INFO_IMPORT(arg_info);
  while(import)
  {
    fprintf(file, ".import%s\n", import->string);
    import = import->next;
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

    int index = SYMBOLENTRY_OFFSET(GLOBDEF_TABLELINK(arg_node));
    char *string = STRcatn(4, "var \"", GLOBDEF_NAME(arg_node), "\" ", STRitoa(index));

    // Add to export list
    INFO_EXPORT(arg_info) = PushIfExistElseCreate(INFO_EXPORT(arg_info), string, NULL, 0);
  }

  INFO_GLOBAL(arg_info) = PushIfExistElseCreate(INFO_GLOBAL(arg_info), TypePrinter(GLOBDEF_TYPE(arg_node)), NULL, 0);

  GLOBDEF_DIMS(arg_node) = TRAVopt(GLOBDEF_DIMS(arg_node),arg_info);
  GLOBDEF_INIT(arg_node) = TRAVopt(GLOBDEF_INIT(arg_node), arg_info);


  DBUG_RETURN(arg_node);
}


node *GBCglobdecl(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCglobdecl");

  // Add to import list
  // All global declarations are imported, so there is never need for a global one
  INFO_IMPORT(arg_info) = PushIfExistElseCreate(INFO_IMPORT(arg_info), TypePrinter(GLOBDECL_TYPE(arg_node)), NULL, 0);

  GLOBDECL_DIMS(arg_node) = TRAVopt(GLOBDECL_DIMS(arg_node),arg_info);

  DBUG_RETURN(arg_node);
}

node *GBCvardecl(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCvardecl");

  if (VARDECL_INIT(arg_node) != NULL) {
    VARDECL_INIT(arg_node) = TRAVopt(VARDECL_INIT(arg_node), arg_info);
    const type currenttype = INFO_CURRENTTYPE(arg_info);

    int offset = SYMBOLENTRY_OFFSET(GetNode(VARDECL_NAME(arg_node), INFO_SYMBOLTABLE(arg_info), arg_node, INFO_PARENTTABLE(arg_info)));

    switch (currenttype)
      {
        case T_int:
          fprintf(INFO_FILE(arg_info), "    istore %d\n", offset);
          break;

        case T_float:
          fprintf(INFO_FILE(arg_info), "    fstore %d\n", offset);
          break;

        case T_bool:
          fprintf(INFO_FILE(arg_info), "    bstore %d\n", offset);
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

  node *symboltableEntry = GetNode(FUNCALL_NAME(arg_node), INFO_SYMBOLTABLE(arg_info), arg_node, INFO_PARENTTABLE(arg_info));
  int offset = SYMBOLENTRY_OFFSET(symboltableEntry);

  fprintf(INFO_FILE(arg_info), "    isrg\n");
  FUNCALL_ARGS(arg_node) = TRAVopt(FUNCALL_ARGS(arg_node), arg_info);

  if(SYMBOLENTRY_ISEXTERN(symboltableEntry)) {
    fprintf(INFO_FILE(arg_info), "    jsre %d\n", offset);
  } else {
    GetParamcount(symboltableEntry);
    fprintf(INFO_FILE(arg_info), "    jsr %d %s\n", GetParamcount(symboltableEntry), FUNCALL_NAME(arg_node));
  }

  INFO_CURRENTTYPE(arg_info) = SYMBOLENTRY_TYPE(symboltableEntry);

  DBUG_RETURN(arg_node);
}


node *GBCfundef(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCfundef");

  // node *paramnode = PARAM_TABLELINK(FUNDEF_PARAMS(arg_node));

  node *globaltable = INFO_SYMBOLTABLE(arg_info);
  INFO_PARENTTABLE(arg_info) = globaltable;
  INFO_SYMBOLTABLE(arg_info) = FUNDEF_SYMBOLENTRY(arg_node);

  char *params = "";

  node *temp = FUNDEF_PARAMS(arg_node);
  while(temp) {
    params = STRcat(params, TypePrinter(PARAM_TYPE(temp)));
    temp = PARAM_NEXT(temp);
  }

  // If fundef is extern, get the params and add it to
  // .importfun "foo" int bool float
  // Where the first int is the type and the others are the params
  if(FUNDEF_ISEXTERN(arg_node))
  {
    // Shamelessly stolen from https://stackoverflow.com/a/29087251/12106583
    int size = snprintf(NULL, 0, "fun \"%s\" %s %s", FUNDEF_NAME(arg_node), TypePrinter(FUNDEF_TYPE(arg_node)), params);
    char *buf = malloc(size + 1);

    snprintf(buf, size + 1, "fun \"%s\" %s %s", FUNDEF_NAME(arg_node), TypePrinter(FUNDEF_TYPE(arg_node)), params);
    INFO_IMPORT(arg_info) = PushIfExistElseCreate(INFO_IMPORT(arg_info), STRcpy(buf), NULL, 0);
  }
  else
  {
    // print the fundef as
    // "foo:"
    fprintf(INFO_FILE(arg_info), "%s:\n", FUNDEF_NAME(arg_node));

    // if the fundef is exported, print it as
    // .exportfun "foo" void int int[] foo
    if (FUNDEF_ISEXPORT(arg_node))
    {
        int size = snprintf(NULL, 0, "fun \"%s\" %s %s %s", FUNDEF_NAME(arg_node), TypePrinter(FUNDEF_TYPE(arg_node)), params, FUNDEF_NAME(arg_node));
        char *buf = malloc(size + 1);
        snprintf(buf, size + 1, "fun \"%s\" %s %s %s", FUNDEF_NAME(arg_node), TypePrinter(FUNDEF_TYPE(arg_node)), params, FUNDEF_NAME(arg_node));
        INFO_EXPORT(arg_info) = PushIfExistElseCreate(INFO_EXPORT(arg_info), STRcpy(buf), NULL, 0);
    }

    // Print esr as
    // esr L
    // Where L is the amount of vardecls
    // int amountOfVarDecls = GetVardeclcount(FUNDEF_TABLELINK(arg_node));
    int amountOfVarDecls = FUNDEF_VARDECLCOUNT(arg_node);
    if(amountOfVarDecls > 0)
    {
      fprintf(INFO_FILE(arg_info), "    esr %d\n", amountOfVarDecls);
    }

    FUNDEF_PARAMS(arg_node) = TRAVopt(FUNDEF_PARAMS(arg_node), arg_info);
    FUNDEF_FUNBODY(arg_node) = TRAVopt(FUNDEF_FUNBODY(arg_node), arg_info);

    if (FUNDEF_TYPE(arg_node) == T_void)
    {
      fprintf(INFO_FILE(arg_info), "    %s\n", "return");
    }
    fprintf(INFO_FILE(arg_info), "\n");
  }

  INFO_SYMBOLTABLE(arg_info) = globaltable;
  INFO_PARENTTABLE(arg_info) = NULL;

  DBUG_RETURN(arg_node);
}

node *GBCexprstmt(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCexprstmt");

    node *expr = EXPRSTMT_EXPR(arg_node);
    TRAVdo(expr, arg_info);

    if (NODE_TYPE(expr) == N_funcall)
    {
        node *symbolentry = GetNode(FUNCALL_NAME(expr), INFO_SYMBOLTABLE(arg_info), arg_node, INFO_PARENTTABLE(arg_info));

        switch (SYMBOLENTRY_TYPE(symbolentry))
        {
        case T_int:
            fprintf(INFO_FILE(arg_info), "    ipop\n");
            break;
        case T_float:
            fprintf(INFO_FILE(arg_info), "    fpop\n");
            break;
        case T_bool:
            fprintf(INFO_FILE(arg_info), "    bpop\n");
            break;
        default:
            break;
        }
    }

    DBUG_RETURN(arg_node);
}

node *GBCreturn(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCreturn");

  RETURN_EXPR(arg_node) = TRAVopt(RETURN_EXPR(arg_node), arg_info);

  const char *returntype;
  type symboltype = INFO_CURRENTTYPE(arg_info);

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


char *createBranch(const char *type, info *info)
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
  bool elseExists = IFELSE_ELSE(arg_node) != NULL;

  if (elseExists)
  {
    elseBranch = createBranch("else", arg_info);
  }
  else
  {
    elseBranch = createBranch("end", arg_info);
  }

  char *endBranch = IFELSE_ELSE(arg_node) ? createBranch("end", arg_info) : elseBranch;
  fprintf(INFO_FILE(arg_info), "    branch_f %s\n", elseBranch);

  TRAVopt(IFELSE_THEN(arg_node), arg_info);

  if (elseExists)
  {
      fprintf(INFO_FILE(arg_info), "    jump %s\n", endBranch);
      fprintf(INFO_FILE(arg_info), "%s:\n", elseBranch);
      TRAVopt(IFELSE_ELSE(arg_node), arg_info);
      // fputc('\n', INFO_FILE(arg_info));
  }

  fprintf(INFO_FILE(arg_info), "%s:\n", endBranch);
  free(elseBranch);

  if (elseExists)
  {
      free(endBranch);
  }

  DBUG_RETURN(arg_node);

}


node *GBCwhile(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCwhile");

    char *whileBranch = createBranch("while", arg_info);
    char *endBranch = createBranch("end", arg_info);

    fprintf(INFO_FILE(arg_info), "%s:\n", whileBranch);

    TRAVdo(WHILE_COND(arg_node), arg_info);
    fprintf(INFO_FILE(arg_info), "    branch_f %s\n", endBranch);


    TRAVopt(WHILE_BLOCK(arg_node), arg_info);
    fprintf(INFO_FILE(arg_info), "    jump %s\n", whileBranch);
    fprintf(INFO_FILE(arg_info), "%s:\n", endBranch);

    free(whileBranch);
    free(endBranch);

    DBUG_RETURN(arg_node);
}

node *GBCdowhile(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCdowhile");

    char *branchName = createBranch("dowhile", arg_info);
    fprintf(INFO_FILE(arg_info), "\n%s:\n", branchName);

    TRAVopt(DOWHILE_BLOCK(arg_node), arg_info);
    TRAVdo(DOWHILE_COND(arg_node), arg_info);

    fprintf(INFO_FILE(arg_info), "  branch_t %s\n", branchName);

    free(branchName);

    DBUG_RETURN(arg_node);
}

node *GBCvarlet(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCvarlet");
    node *symbolentry = GetNode(VARLET_NAME(arg_node), INFO_SYMBOLTABLE(arg_info), arg_node, INFO_PARENTTABLE(arg_info));

    type symbolentrytype = SYMBOLENTRY_TYPE(symbolentry);
    int index = SYMBOLENTRY_OFFSET(symbolentry);

    if(SYMBOLENTRY_DEPTH(symbolentry) == 0)
    {
      //TODO: check if nodetype is globdecl, if so; the iload should be extern
      // node *decl = VAR_DECL(arg_node);
      // bool isextern = NODE_TYPE(decl) == N_globdecl;

      bool isextern = FALSE;
      char *globalchar = isextern ? "e" : "g";
      switch (symbolentrytype)
      {
        case T_int:
          fprintf(INFO_FILE(arg_info), "    iload%s %d\n", globalchar, index);
          break;

        case T_float:
          fprintf(INFO_FILE(arg_info), "    fload%s %d\n", globalchar, index);
          break;

        case T_bool:
          fprintf(INFO_FILE(arg_info), "    bload%s %d\n", globalchar, index);
          break;

        default:
          break;
      }
    }
    else
    {
      switch (symbolentrytype)
      {
        case T_int:
          fprintf(INFO_FILE(arg_info), index <= 3 ? "    iload_%d\n" : "    iload %d\n", index);
          break;

        case T_float:
          fprintf(INFO_FILE(arg_info), index <= 3 ? "    fload_%d\n" : "    fload %d\n", index);
          break;

        case T_bool:
          fprintf(INFO_FILE(arg_info), index <= 3 ? "    bload_%d\n" : "    bload %d\n", index);
          break;

        default:
          break;
      }
    }

    DBUG_RETURN(arg_node);
}


node *GBCvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCvar");
    node *symbolentry = GetNode(VAR_NAME(arg_node), INFO_SYMBOLTABLE(arg_info), arg_node, INFO_PARENTTABLE(arg_info));

    bool global = FALSE;
    if(VAR_ISGLOBAL(arg_node) == 1)
    {
        global = TRUE;
    }

    type symbolentrytype = SYMBOLENTRY_TYPE(symbolentry);
    int index = SYMBOLENTRY_OFFSET(symbolentry);

    // TODO: Global check needs to check for local first
    if(SYMBOLENTRY_DEPTH(symbolentry) == 0)
    {
      //TODO: check if nodetype is globdecl, if so; the iload should be extern
      // node *decl = VAR_DECL(arg_node);
      // bool isextern = NODE_TYPE(decl) == N_globdecl;

      bool isextern = FALSE;
      char *globalchar = global ? "g" : "e";
      switch (symbolentrytype)
      {
        case T_int:
          fprintf(INFO_FILE(arg_info), "    iload%s %d\n", globalchar, index);
          break;

        case T_float:
          fprintf(INFO_FILE(arg_info), "    fload%s %d\n", globalchar, index);
          break;

        case T_bool:
          fprintf(INFO_FILE(arg_info), "    bload%s %d\n", globalchar, index);
          break;

        default:
          break;
      }
    }
    else
    {
      switch (symbolentrytype)
      {
        case T_int:
          fprintf(INFO_FILE(arg_info), index <= 3 ? "    iload_%d\n" : "    iload %d\n", index);
          break;

        case T_float:
          fprintf(INFO_FILE(arg_info), index <= 3 ? "    fload_%d\n" : "    fload %d\n", index);
          break;

        case T_bool:
          fprintf(INFO_FILE(arg_info), index <= 3 ? "    bload_%d\n" : "    bload %d\n", index);
          break;

        default:
          break;
      }
    }

    INFO_CURRENTTYPE(arg_info) = SYMBOLENTRY_TYPE(symbolentry);

    DBUG_RETURN(arg_node);
}



node *GBCassign(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCassign");

  ASSIGN_LET(arg_node) = TRAVdo(ASSIGN_LET(arg_node), arg_info);
  ASSIGN_EXPR(arg_node) = TRAVdo(ASSIGN_EXPR(arg_node), arg_info);


  node *symboltableentry = GetNode(VARLET_NAME(ASSIGN_LET(arg_node)), INFO_SYMBOLTABLE(arg_info), arg_node, INFO_PARENTTABLE(arg_info));
  type nodetype = SYMBOLENTRY_TYPE(symboltableentry);
  int index = SYMBOLENTRY_OFFSET(symboltableentry);

  int depth = SYMBOLENTRY_DEPTH(symboltableentry);
  char *globalchar = depth == 0 ? "g" : "";
  switch (nodetype)
  {
    case T_int:
      fprintf(INFO_FILE(arg_info), "    istore%s %d\n", globalchar, index);
      break;

    case T_float:
      fprintf(INFO_FILE(arg_info), "    fstore%s %d\n", globalchar, index);
      break;

    case T_bool:
      fprintf(INFO_FILE(arg_info), "    bstore%s %d\n", globalchar, index);
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

  const type currenttype = INFO_CURRENTTYPE(arg_info);
  const char *opstring;
  const char *typestring;
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

    case T_unknown:
      break;

    default:
      break;
  }

  const binop op = BINOP_OP(arg_node);
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

    case T_unknown:
      DBUG_PRINT("GBC", ("Unknown current type found"));
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

    default:
      break;
  }

  INFO_CURRENTTYPE(arg_info) = CAST_TYPE(arg_node);

  fprintf(INFO_FILE(arg_info), "    %s\n", opstring);

  DBUG_RETURN(arg_node);
}

node *GBCnum(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCnum");

  int count = INFO_CONSTANTCOUNT(arg_info);
  int value = NUM_VALUE(arg_node);
  // FPRINTF(INFO_FILE(arg_info), "%d");
  // If value is 0 or 1, load instantly; otherwise load as constant from table and add to table
  if(value == 0 || value == 1)
  {
    fprintf(INFO_FILE(arg_info), "    iloadc_%d\n", value);
  }
  else if(value == -1)
  {
    fprintf(INFO_FILE(arg_info), "    iloadc_m1\n");
  }
  else
  {
    fprintf(INFO_FILE(arg_info), "    iloadc %d\n", count);

    // TODO: Make table unique
    char *string = STRcat("int ", STRitoa(value));
    INFO_CONSTANT(arg_info) = PushIfExistElseCreate(INFO_CONSTANT(arg_info), string, NULL, 0);
    INFO_CONSTANTCOUNT(arg_info) ++;
  }

  INFO_CURRENTTYPE(arg_info) = T_int;

  DBUG_RETURN(arg_node);
}


node *GBCfloat(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCfloat");

  int count = INFO_CONSTANTCOUNT(arg_info);
  float value = FLOAT_VALUE(arg_node);
  // If value is 0.0 or 1.0, load instantly; otherwise load as constant from table and add to table
  if(value == 0.0)
  {
    fprintf(INFO_FILE(arg_info), "    floadc_0\n");
  }
  else if (value == 1.0)
  {
    fprintf(INFO_FILE(arg_info), "    floadc_1\n");
  }
  else
  {
    fprintf(INFO_FILE(arg_info), "    floadc %d\n", count);

    // Shamelessly stolen from https://stackoverflow.com/a/29087251/12106583
    int size = snprintf(NULL, 0, "float %f", value);
    char *buf = malloc(size + 1);
    snprintf(buf, size + 1, "float %f\n", value);

    INFO_CONSTANT(arg_info) = PushIfExistElseCreate(INFO_CONSTANT(arg_info), buf, NULL, 0);
    INFO_CONSTANTCOUNT(arg_info) ++;
  }

  INFO_CURRENTTYPE(arg_info) = T_float;

  DBUG_RETURN(arg_node);
}


node *GBCbool(node *arg_node, info *arg_info)
{
  DBUG_ENTER("GBCbool");

  int count = INFO_CONSTANTCOUNT(arg_info);
  if(BOOL_VALUE(arg_node))
  {
    bool value = BOOL_VALUE(arg_node);
    if(value)
    {
      fprintf(INFO_FILE(arg_info), "    bloadc_t\n");
    }
    else if (!value)
    {
      fprintf(INFO_FILE(arg_info), "    bloadc_f\n");
    }
  }
  else
  {
    fprintf(INFO_FILE(arg_info), "    bloadc %d\n", count);

    char *string = STRcat("bool ", BOOL_VALUE(arg_node) ? "true" : "false");
    INFO_CONSTANT(arg_info) = PushIfExistElseCreate(INFO_CONSTANT(arg_info), string, NULL, 0);
    INFO_CONSTANTCOUNT(arg_info) ++;
  }

  INFO_CURRENTTYPE(arg_info) = T_bool;

  DBUG_RETURN(arg_node);
}


// Congrats you found the easter egg:
// //
//                                 ████████
//                               ██        ██
//                             ██▒▒▒▒        ██
//                           ██▒▒▒▒▒▒      ▒▒▒▒██
//                           ██▒▒▒▒▒▒      ▒▒▒▒██
//                         ██  ▒▒▒▒        ▒▒▒▒▒▒██
//                         ██                ▒▒▒▒██
//                       ██▒▒      ▒▒▒▒▒▒          ██
//                       ██      ▒▒▒▒▒▒▒▒▒▒        ██
//                       ██      ▒▒▒▒▒▒▒▒▒▒    ▒▒▒▒██
//                       ██▒▒▒▒  ▒▒▒▒▒▒▒▒▒▒  ▒▒▒▒▒▒██
//                         ██▒▒▒▒  ▒▒▒▒▒▒    ▒▒▒▒██
//                         ██▒▒▒▒            ▒▒▒▒██
//                           ██▒▒              ██
//                             ████        ████
//                                 ████████
//



/*
 * Traversal start function
 */
node *GBCdoGenByteCode( node *syntaxtree)
{
  DBUG_ENTER("GBCdoGenByteCode");

  DBUG_ASSERT((syntaxtree != NULL), "GBCdoGenByteCode called with empty syntaxtree");

  info *arg_info = MakeInfo();
  INFO_FILE(arg_info) = stdout;

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
