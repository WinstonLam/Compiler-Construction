#ifndef _SYMBOLTABLE_LINKER_H_
#define _SYMBOLTABLE_LINKER_H_
#include "types.h"

extern node *GetNode(char *entry, node *symboltable, node *arg_node, node *parenttable);
extern node *SLfundef(node *arg_node, info *arg_info);
extern node *SLvardecl(node *arg_node, info *arg_info);
extern node *SLvar(node *arg_node, info *arg_info);
extern node *SLvarlet(node *arg_node, info *arg_info);
extern node *SLprogram(node *arg_node, info *arg_info);
extern int GetParamcount(node *arg_node);
extern node *SLdoSymboltableLinker( node *syntaxtree);

#endif
