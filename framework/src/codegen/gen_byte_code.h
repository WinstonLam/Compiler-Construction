#ifndef _GEN_BYTE_CODE_H_
#define _GEN_BYTE_CODE_H_
#include "types.h"

extern node *GBCdoGenByteCode( node *syntaxtree);
extern node *GBCprogram(node *arg_node, info *arg_info);
extern node *GBCglobdef(node *arg_node, info *arg_info);
extern node *GBCglobdecl(node *arg_node, info *arg_info);
extern node *GBCvardecl(node *arg_node, info *arg_info);
extern node *GBCfuncall(node *arg_node, info *arg_info);
extern node *GBCfundef(node *arg_node, info *arg_info);
extern node *GBCreturn(node *arg_node, info *arg_info);
extern node *GBCifelse(node *arg_node, info *arg_info);
extern node *GBCwhile(node *arg_node, info *arg_info);
extern node *GBCassign(node *arg_node, info *arg_info);
extern node *GBCbinop(node *arg_node, info *arg_info);
extern node *GBCmonop(node *arg_node, info *arg_info);
extern node *GBCcast(node *arg_node, info *arg_info);
extern node *GBCnum(node *arg_node, info *arg_info);
extern node *GBCfloat(node *arg_node, info *arg_info);
extern node *GBCbool(node *arg_node, info *arg_info);
extern node *GBCvar(node *arg_node, info *arg_info);

#endif
