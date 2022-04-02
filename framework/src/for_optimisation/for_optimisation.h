#ifndef _FOR_OPTIMISATION_H_
#define _FOR_OPTIMISATION_H_
#include "types.h"

extern node *FOfunbody (node *arg_node, info *arg_info);
extern node *FOfor (node *arg_node, info *arg_info);
extern node *FOstmts (node *arg_node, info *arg_info);
extern node *FOvarlet (node *arg_node, info *arg_info);
extern node *FOvar (node *arg_node, info *arg_info);

extern node *FOdoForOptimisation( node *syntaxtree);


#endif
