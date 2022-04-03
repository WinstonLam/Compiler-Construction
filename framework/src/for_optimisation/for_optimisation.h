#ifndef _FOR_OPTIMISATION_H_
#define _FOR_OPTIMISATION_H_
#include "types.h"

extern node *FOfundef (node *arg_node, info *arg_info);
extern node *FOfor (node *arg_node, info *arg_info);
extern node *FOprogram (node *arg_node, info *arg_info);

extern node *FOdoForOptimisation( node *syntaxtree);


#endif
