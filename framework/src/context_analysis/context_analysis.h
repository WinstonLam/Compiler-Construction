#ifndef _CONTEXT_ANALYSIS_H_
#define _CONTEXT_ANALYSIS_H_
#include "types.h"


extern node *CAfundef(node *arg_node, info *arg_info);
extern node *CAvardecl(node *arg_node, info *arg_info);
extern node *CAfor(node *arg_node, info *arg_info);
extern node *CAdoContextAnalysis( node *syntaxtree);

#endif
