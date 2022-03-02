#ifndef _STR_RED_H_
#define _STR_RED_H_
#include "types.h"

extern node *SRbinop (node *arg_node, info *arg_info);
extern node *SRbinopZ (node *arg_node, info *arg_info);
extern node *SRbinopR (node *arg_node, info *arg_info);
extern node *SRbinopL (node *arg_node, info *arg_info);
extern node *SRdoStrengthReduction( node *syntaxtree);

#endif
