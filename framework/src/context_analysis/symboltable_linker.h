#ifndef _SYMBOLTABLE_LINKER_H_
#define _SYMBOLTABLE_LINKER_H_
#include "types.h"

extern node *SLfundef(node *arg_node, info *arg_info);
extern node *SLvardecl(node *arg_node, info *arg_info);
extern node *SLvar(node *arg_node, info *arg_info);
extern node *SLglobdef(node *arg_node, info *arg_info);
extern node *SLvarlet(node *arg_node, info *arg_info);
extern node *SLprogram(node *arg_node, info *arg_info);
extern int GetParamcount(node *arg_node);
extern int GetVardeclcount(node *arg_node);
extern bool IsLocal(char *entry, node *symboltable);
extern node *SLdoSymboltableLinker( node *syntaxtree);
extern size_t CountVarDecls(node *table);

#endif
