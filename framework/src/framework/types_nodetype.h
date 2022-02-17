
/**
 * @file types_nodetype.h
 *
 * This file defines the nodetype node enumeration.
 * 
 * THIS FILE HAS BEEN GENERATED USING 
 * $Id: types_nodetype.h.xsl 14593 2006-01-31 17:09:55Z cg $.
 * DO NOT EDIT THIS FILE AS MIGHT BE CHANGED IN A LATER VERSION.
 *
 * ALL CHANGES MADE TO THIS FILE WILL BE OVERWRITTEN!
 *
 */

#ifndef _SAC_TYPES_NODETYPE_H_
#define _SAC_TYPES_NODETYPE_H_

#define MAX_NODES 12
typedef enum
{ N_undefined = 0, N_stmts = 1, N_assign = 2, N_do = 3, N_while = 4, N_binop =
    5, N_varlet = 6, N_var = 7, N_num = 8, N_float = 9, N_bool =
    10, N_symboltableentry = 11, N_error = 12 } nodetype;

#endif /* _SAC_TYPES_NODETYPE_H_ */
