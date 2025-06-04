/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Group tree routines.
**
**  MODULE DESCRIPTION:
**
**  	This module contains routines that manipulate the tree
**  of newsgroups.  The VMS RTL LIB$xxx_TREE routines are used to
**  create the tree, we also have our own one-node-at-a-time tree traversal
**  routine, because LIB$TRAVERSE_TREE isn't very handy for some
**  applications.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  09-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	09-SEP-1992 V1.0    Madison 	Initial coding.
**  	15-FEB-1993 V1.0-1  Madison 	Make compatible with pre-VMS V5.2.
**--
*/
#include "newsrdr.h"
#include "globals.h"

    struct TCTX {
    	struct GRP **stackp;
    	struct GRP *curnode;
    	struct GRP *savenode;
    	struct GRP *stack[4096];
    };

/*
** Forward declarations
*/

    struct GRP   *Find_Group(char *);
    struct GRP	 *Find_Group_Wild(char *, struct TCTX **);
    void          Insert_Group(struct GRP *);
    unsigned int  Set_Initial_Group(struct GRP *, struct dsc$descriptor *);
    static int    node_compare(char *, struct GRP *, int);
    static unsigned int node_alloc(char *, struct GRP **, struct GRP *);
    struct GRP 	  *Traverse_Tree(struct TCTX **);
    unsigned int  Traverse_Finish(struct TCTX **);
    struct GRP    *Current_TreeNode(struct TCTX **);


/*
**++
**  ROUTINE:	Find_Group
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Uses LIB$LOOKUP_TREE to locate a group in the tree by name.
**
**  RETURNS:	struct GRP *
**
**  PROTOTYPE:
**
**  	Find_Group(char *str)
**
**  str:    character string, read only, by reference (ASCIZ)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	non-0:	group was found
**  	    0:	no group was found
**
**  SIDE EFFECTS:   	None.
**
**--
*/
struct GRP *Find_Group(char *str) {

    struct GRP *grp;

    if (!OK(lib$lookup_tree(&news_prof.gtree, str, node_compare, &grp))) {
    	grp = NULL;
    }

    return grp;

} /* Find_Group */

/*
**++
**  ROUTINE:	Find_Group_Wild
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Uses LIB$LOOKUP_TREE to locate a group in the tree by name.
**
**  RETURNS:	struct GRP *
**
**  PROTOTYPE:
**
**  	Find_Group_Wild(char *str)
**
**  str:    character string, read only, by reference (ASCIZ)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	non-0:	group was found
**  	    0:	no group was found
**
**  SIDE EFFECTS:   	None.
**
**--
*/
struct GRP *Find_Group_Wild(char *str, struct TCTX **xctx) {

    struct GRP *grp;
    struct TCTX *ctx;
    struct dsc$descriptor dsc1, dsc2;

    ctx = (xctx == 0) ? 0 : *xctx;
    INIT_SDESC(dsc1, strlen(str), str);
    for (grp = Traverse_Tree(&ctx); grp != 0; grp = Traverse_Tree(&ctx)) {
    	INIT_SDESC(dsc2, strlen(grp->grpnam), grp->grpnam);
    	if (OK(str$match_wild(&dsc2, &dsc1))) {
    	    if (xctx == 0) Traverse_Finish(&ctx);
    	    break;
    	}
    }

    if (xctx != 0) *xctx = ctx;

    return grp;

} /* Find_Group_Wild */

/*
**++
**  ROUTINE:	Insert_Group
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Uses LIB$INSERT_TREE to insert a GRP structure into the tree.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Insert_Group(struct GRP *grp)
**
**  grp:    GRP structure, modify, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void Insert_Group(struct GRP *grp) {

    static int one = 1;
    struct GRP *tree_node;

    lib$insert_tree(&news_prof.gtree, grp->grpnam, &one, node_compare,
    	    	    	node_alloc, &tree_node, grp);

} /* Insert_Group */

/*
**++
**  ROUTINE:	Set_Initial_Group
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Tree-traversal action routine called by Set_Initial_Groups
**  in module NEWSRDR to subscribe a new user to an initial set of
**  newsgroups.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Set_Initial_Group(struct GRP *g, struct dsc$descriptor *pattern)
**
**  g:	    	GRP structure, modify, by reference
**  pattern:	character string, read only, by descriptor
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Always returned.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int Set_Initial_Group(struct GRP *grp, struct dsc$descriptor *pat) {

    struct dsc$descriptor sdsc;

    INIT_SDESC(sdsc, strlen(grp->grpnam), grp->grpnam);
    if (OK(str$match_wild(&sdsc, pat))) {
    	grp->subscribed = 1;
    	lib$signal(NEWS__SUBSCRIBED, 2, sdsc.dsc$w_length, grp->grpnam);
    }

    return SS$_NORMAL;

} /* Set_Initial_Group */

/*
**++
**  ROUTINE:	node_compare
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Comparison routine used by LIB$INSERT_TREE.  Just does
**  a string compare on the group name.
**
**  RETURNS:	int
**
**  PROTOTYPE:
**
**  	node_compare(char *s, struct GRP *grp, int dummy)
**
**  s:	    character string, read only, by reference (ASCIZ)
**  grp:    GRP structure, read only, by reference
**  dummy:  not used
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    >0:	s is greater than grp->grpnam
**  	     0: s equals          grp->grpnam
**  	    <0: s is less than    grp->grpnam
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static int node_compare(char *s, struct GRP *grp, int d) {

    return strcmp(s, grp->grpnam);

} /* node_compare */

/*
**++
**  ROUTINE:	node_alloc
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Tree node allocation routine used by LIB$INSERT_TREE.  Since
**  the GRP structure we're adding was already allocated by Insert_Group's
**  caller, we just copy the pointer over so LIB$INSERT_TREE can use it.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	node_alloc(char *str, struct GRP **gnode, struct GRP *grp)
**
**  str:    	not used
**  gnode:  	pointer to GRP structure, write only, by reference
**  grp:    	GRP structure, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Always returned.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static unsigned int node_alloc(char *str, struct GRP **gnode, struct GRP *grp) {

    *gnode = grp;
    return SS$_NORMAL;

} /* node_alloc */

/*
**++
**  ROUTINE:	Traverse_Tree
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Our own node-at-a-time AVL tree traversal.  Does in-order
**  traversal of the tree.  The first call to this routine should
**  pass a context variable reference that was set to zero; it will
**  be updated by this routine as we go through the tree.
**
**  RETURNS:	struct GRP *
**
**  PROTOTYPE:
**
**  	Traverse_Tree(struct TCTX **tctxp)
**
**  tctxp:  pointer to internally-defined structure, modify, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	non-0:	next group in the tree
**  	    0:	no more groups in the tree
**
**  SIDE EFFECTS:   	None.
**
**--
*/
struct GRP *Traverse_Tree(struct TCTX **tctxp) {

    struct TCTX *tctx = *tctxp;
    struct GRP *g;

    if (tctx == NULL) {
    	tctx = malloc(sizeof(struct TCTX));
    	memset(tctx, 0, sizeof(struct TCTX));
    	tctx->stackp = tctx->stack;
    	tctx->curnode = news_prof.gtree;
    	*tctxp = tctx;
    }

    while (tctx->curnode != NULL) {
    	*tctx->stackp++ = tctx->curnode;
    	tctx->curnode = tctx->curnode->left;
    }

    if (tctx->stackp == tctx->stack) {
    	free(tctx);
    	*tctxp = NULL;
    	return NULL;
    }

    g = tctx->savenode = *(--tctx->stackp);
    tctx->curnode = g->right;

    return g;

} /* Traverse_Tree */

/*
**++
**  ROUTINE:	Traverse_Finish
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Prematurely stops a Traverse_Tree sequence.  Just frees up
**  the context block we used.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Traverse_Finish(struct TCTX **tctxp)
**
**  tctxp:  pointer to internally-defined structure, modify, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Always returned.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int Traverse_Finish(struct TCTX **tctxp) {

    if (*tctxp != NULL) free(*tctxp);
    *tctxp = NULL;

    return SS$_NORMAL;

} /* Traverse_Finish */

/*
**++
**  ROUTINE:	Current_TreeNode
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Returns our current position in the group tree traversal
**  we're doing with Traverse_Tree.
**
**  RETURNS:	GRP *
**
**  PROTOTYPE:
**
**  	Current_TreeNode(struct TCTX **tctxp)
**
**  tctxp:  pointer to GRP structure, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	non-0:	current GRP
**  	    0:	No traversal in progress
**
**  SIDE EFFECTS:   	None.
**
**--
*/
struct GRP *Current_TreeNode(struct TCTX **tctxp) {

    if (*tctxp) return (*tctxp)->savenode;

    return NULL;

} /* Current_TreeNode */    
