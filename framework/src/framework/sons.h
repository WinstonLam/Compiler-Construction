
/**
 * @file sons.h
 *
 * Defines the NodesUnion and node structures.
 * 
 * THIS FILE HAS BEEN GENERATED USING 
 * $Id: sons.h.xsl 14593 2006-01-31 17:09:55Z cg $.
 * DO NOT EDIT THIS FILE AS MIGHT BE CHANGED IN A LATER VERSION.
 *
 * ALL CHANGES MADE TO THIS FILE WILL BE OVERWRITTEN!
 *
 */

#ifndef _SAC_SONS_H_
#define _SAC_SONS_H_

#include "types.h"


/******************************************************************************
 * For each node a structure of its sons is defined, named 
 * SONS_<nodename>
 *****************************************************************************/
struct SONS_N_ASSIGN
{
  node *Let;
  node *Expr;
};
struct SONS_N_BINOP
{
  node *Left;
  node *Right;
};
struct SONS_N_BOOL
{
};
struct SONS_N_ERROR
{
  node *Next;
};
struct SONS_N_FLOAT
{
};
struct SONS_N_NUM
{
};
struct SONS_N_STMTS
{
  node *Stmt;
  node *Next;
};
struct SONS_N_SYMBOLTABLEENTRY
{
};
struct SONS_N_VAR
{
};
struct SONS_N_VARLET
{
};
/*****************************************************************************
 * This union handles all different types of sons. Its members are
 * called N_nodename.
 ****************************************************************************/
struct SONUNION
{
  struct SONS_N_ASSIGN *N_assign;
  struct SONS_N_BINOP *N_binop;
  struct SONS_N_BOOL *N_bool;
  struct SONS_N_ERROR *N_error;
  struct SONS_N_FLOAT *N_float;
  struct SONS_N_NUM *N_num;
  struct SONS_N_STMTS *N_stmts;
  struct SONS_N_SYMBOLTABLEENTRY *N_symboltableentry;
  struct SONS_N_VAR *N_var;
  struct SONS_N_VARLET *N_varlet;
};
#endif /* _SAC_SONS_H_ */
