#ifndef _CONTEXT_ANALYSIS_H_
#define _CONTEXT_ANALYSIS_H_
#include "types.h"


extern node *CAprogram(node *arg_node, info *arg_info);
extern node *CAfundef(node *arg_node, info *arg_info);
extern node *CAvardecl(node *arg_node, info *arg_info);
extern node *CAparam(node *arg_node, info *arg_info);
extern node *CAglobdef(node *arg_node, info *arg_info);
extern node *GetNode(char *entry, node *symboltable, node *arg_node, node *parenttable);
extern node *CAfor(node *arg_node, info *arg_info);
extern node *CAvar(node *arg_node, info *arg_info);
extern node *CAvarlet(node *arg_node, info *arg_info);
extern node *CAdoContextAnalysis( node *syntaxtree);

#endif
