
/**
 * @file free_node.h
 *
 * Functions to free node structures
 * 
 * THIS FILE HAS BEEN GENERATED USING 
 * $Id: free_node.h.xsl 14593 2006-01-31 17:09:55Z cg $.
 * DO NOT EDIT THIS FILE AS MIGHT BE CHANGED IN A LATER VERSION.
 *
 * ALL CHANGES MADE TO THIS FILE WILL BE OVERWRITTEN!
 *
 */

#ifndef _SAC_FREE_NODE_H_
#define _SAC_FREE_NODE_H_

#include "types.h"

extern node *FREEassign (node * arg_node, info * arg_info);
extern node *FREEbinop (node * arg_node, info * arg_info);
extern node *FREEbool (node * arg_node, info * arg_info);
extern node *FREEdo (node * arg_node, info * arg_info);
extern node *FREEerror (node * arg_node, info * arg_info);
extern node *FREEfloat (node * arg_node, info * arg_info);
extern node *FREEnum (node * arg_node, info * arg_info);
extern node *FREEstmts (node * arg_node, info * arg_info);
extern node *FREEsymboltableentry (node * arg_node, info * arg_info);
extern node *FREEvar (node * arg_node, info * arg_info);
extern node *FREEvarlet (node * arg_node, info * arg_info);
extern node *FREEwhile (node * arg_node, info * arg_info);

#endif /* _SAC_FREE_NODE_H_ */
