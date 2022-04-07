#ifndef _TYPE_CHECKER_H_
#define _TYPE_CHECKER_H_
#include "types.h"

extern node *TCdoTypeChecking( node *syntaxtree);
extern node *TCprogram(node *arg_node, info *arg_info);
extern node *TCfundef(node *arg_node, info *arg_info);
extern node *TCfunbody(node *arg_node, info *arg_info);
extern node *TCfuncall(node *arg_node, info *arg_info);
extern node *TCexprs(node *arg_node, info *arg_info);
extern node *TCifelse(node *arg_node, info *arg_info);
extern node *TCdowhile(node *arg_node, info *arg_info);
extern node *TCwhile(node *arg_node, info *arg_info);
extern node *TCfor(node *arg_node, info *arg_info);
extern node *TCbinop(node *arg_node, info *arg_info);
extern node *TCmonop(node *arg_node, info *arg_info);
extern node *TCassign(node *arg_node, info *arg_info);
extern node *TCreturn(node *arg_node, info *arg_info);
extern node *TCnum(node *arg_node, info *arg_info);
extern node *TCfloat(node *arg_node, info *arg_info);
extern node *TCbool(node *arg_node, info *arg_info);
#endif