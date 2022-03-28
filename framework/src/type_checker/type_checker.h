#ifndef _TYPE_CHECKER_H_
#define _TYPE_CHECKER_H_
#include "types.h"

extern node *TCdoTypeChecking( node *syntaxtree);
extern node *CAfundef(node *arg_node, info *arg_info);
extern node *CAfunbody(node *arg_node, info *arg_info);

#endif