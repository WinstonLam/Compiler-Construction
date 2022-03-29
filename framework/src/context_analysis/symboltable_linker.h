#ifndef _SYMBOLTABLE_LINKER_H_
#define _SYMBOLTABLE_LINKER_H_
#include "types.h"

extern node *SLfundef(node *arg_node, info *arg_info);
extern node *SLvardecl(node *arg_node, info *arg_info);
extern node *SLfor(node *arg_node, info *arg_info);
extern node *SLprogram(node *arg_node, info *arg_info);
extern node *SLdoSymboltableLinker( node *syntaxtree);

#endif
