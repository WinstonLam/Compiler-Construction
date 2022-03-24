/*****************************************************************************
 *
 * Module: CO
 *
 * Prefix: co
 *
 * Description:
 *
 * This module implements a traversal of the abstract syntax tree that 
 * that counts the number of occurrences of each identifier (left or right hand side).
 *
 *****************************************************************************/


#include "count_occ.h"

#include "types.h"
#include "tree_basic.h"
#include "lookup_table.h"
#include "traverse.h"
#include "dbug.h"

#include "memory.h"
#include "ctinfo.h"


/*
 * INFO structure
 */

struct INFO {
  lut_t *lut;
};


struct value {
  long val;
  char *name;
};

/*
 * INFO macros
 */

#define INFO_LUT(n)  ((n)->lut)


/*
 * INFO functions
 */

static info *MakeInfo(void)
{
  
  info *result;

  DBUG_ENTER( "MakeInfo");

  result = (info *)MEMmalloc(sizeof(info));


  INFO_LUT(result) = LUTgenerateLut();


  DBUG_RETURN( result);
}


static info *FreeInfo( info *info)
{
  DBUG_ENTER ("FreeInfo");

  INFO_LUT( info) = LUTremoveLut( INFO_LUT( info));
  info = MEMfree (info);

  DBUG_RETURN (info);
}

// Printer function to print value of LUT given a pointer.
void *Printer (void *ptr) {
  DBUG_ENTER("Printer");

  // Dereference void pointer to struct value pointer to access values.
  struct value *node = (struct value *)ptr; 
  printf("\nvariable: %s\nappears: %ld time(s)\n ", node->name, node->val);

  DBUG_RETURN( node);
}

// This function creates a new value node with a given key and value.
static struct value *Makenode (char *s, long n) {

  struct value *new = (struct value *)MEMmalloc(sizeof(struct value));
  new->name = s;
  new->val = n;

  return new;
}

/*
 * Traversal functions
 */

node *COvar (node *arg_node, info *arg_info)
{
  DBUG_ENTER("COvar");
  
  lut_t *lut = INFO_LUT(arg_info);
  
  void **lookup = LUTsearchInLutS(lut, VAR_NAME(arg_node));

  // Search if arg_node is already in the lut.
  if (lookup == NULL) {
    
    // If not in LUT create a new value node to insert.
    struct value *new = Makenode(VAR_NAME(arg_node), 1);
    
    // Insert the new value node into the LUT.
    LUTinsertIntoLutS(lut, VAR_NAME( arg_node), new);

  } else {

    // Dereference void pointer to struct value pointer to access values.
    struct value *old = (struct value *)*lookup; 

    // Update the new node by creating a new one.
    struct value *new = Makenode(VAR_NAME(arg_node), (old->val + 1));

    // Update the node in the LUT.
    LUTupdateLutS(lut, VAR_NAME( arg_node), new, lookup);

  }
  DBUG_RETURN( arg_node);
}

/*
 * Traversal start function
 */

node *COdoCountOcc( node *syntaxtree)
{
  info *arg_info;

  DBUG_ENTER("COdoCountOcc");

  arg_info = MakeInfo();

  TRAVpush( TR_co);
  syntaxtree = TRAVdo( syntaxtree, arg_info);
  TRAVpop();

  LUTmapLutS(INFO_LUT(arg_info) ,Printer);

  arg_info = FreeInfo( arg_info);

  DBUG_RETURN( syntaxtree);
}
