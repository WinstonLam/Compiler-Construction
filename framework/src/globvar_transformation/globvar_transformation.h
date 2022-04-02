#ifndef _GLOBVAR_TRANSFORMATION_H_
#define _GLOBVAR_TRANSFORMATION_H_
#include "types.h"

extern node *GTglobdef (node *arg_node, info *arg_info);
extern node *GTprogram (node *arg_node, info *arg_info);
extern node *GTdoGlobvarTransformation( node *syntaxtree);

#endif
