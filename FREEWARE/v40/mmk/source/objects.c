/*
**++
**  FACILITY:	MMK
**
**  ABSTRACT:	Object tree routines.
**
**  MODULE DESCRIPTION:
**
**  	This module contains routines that manipulate the tree
**  of objects.  The VMS RTL LIB$xxx_TREE routines are used to
**  create the tree, we also have our own one-node-at-a-time tree traversal
**  routine, because LIB$TRAVERSE_TREE isn't very handy for some
**  applications.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1992, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  09-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	09-SEP-1992 V1.0    Madison 	Initial coding.
**  	17-JUL-1995 V1.0-1  Madison 	Fix node_compare routine.
**--
*/
#ifdef __DECC
#pragma module OBJECTS "V1.0-1"
#else
#ifndef __GNUC__
#module OBJECTS "V1.0-1"
#endif
#endif
#include "mmk.h"
#include "globals.h"

/*
** Forward declarations
*/
    struct OBJECT   *Find_Object(struct OBJECT *);
    void          Insert_Object(struct OBJECT *);
    static int    node_compare(struct OBJECT *, struct OBJECT *, int);
    static unsigned int node_alloc(char *, struct OBJECT **, struct OBJECT *);

    static unsigned int objtree = 0;

/*
**++
**  ROUTINE:	Find_Object
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Uses LIB$LOOKUP_TREE to locate a group in the tree by name.
**
**  RETURNS:	struct OBJECT *
**
**  PROTOTYPE:
**
**  	Find_Object(char *str)
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
struct OBJECT *Find_Object(struct OBJECT *template) {

    struct OBJECT *obj;

    if (!OK(lib$lookup_tree(&objtree, template, node_compare, &obj))) {
    	obj = NULL;
    }

    return obj;

} /* Find_Object */

/*
**++
**  ROUTINE:	Insert_Object
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Uses LIB$INSERT_TREE to insert an OBJECT structure into the tree.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Insert_Object(struct OBJECT *obj)
**
**  obj:    OBJECT structure, modify, by reference
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
void Insert_Object(struct OBJECT *obj) {

    static int one = 1;
    struct OBJECT *tree_node;

    lib$insert_tree(&objtree, obj, &one, node_compare,
    	    	    	node_alloc, &tree_node, obj);

} /* Insert_Object */

/*
**++
**  ROUTINE:	node_compare
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Comparison routine used by LIB$INSERT_TREE.  Order is based
**  on type and object name.  For library modules, the "object name"
**  incorporates both the library name and the module name.
**
**  RETURNS:	int
**
**  PROTOTYPE:
**
**  	node_compare(struct OBJECT *s, struct OBJECT *obj, int dummy)
**
**  s:	    OBJECT structure, read only, by reference
**  obj:    OBJECT structure, read only, by reference
**  dummy:  not used
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    >0:	s is greater than obj->objnam
**  	     0: s equals          obj->objnam
**  	    <0: s is less than    obj->objnam
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static int node_compare(struct OBJECT *s, struct OBJECT *obj, int d) {

    if (s->type != obj->type) return (s->type - obj->type);

    if (s->type == MMK_K_OBJ_LIBMOD) {
    	int i;
    	i = strcmp(s->libfile->name, obj->libfile->name);
    	if (i == 0) i = strcmp(s->name, obj->name);
    	return i;
    }

    return strcmp(s->name, obj->name);

} /* node_compare */

/*
**++
**  ROUTINE:	node_alloc
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Tree node allocation routine used by LIB$INSERT_TREE.  Since
**  the OBJECT structure we're adding was already allocated by Insert_Object's
**  caller, we just copy the pointer over so LIB$INSERT_TREE can use it.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	node_alloc(char *str, struct OBJECT **onode, struct OBJECT *obj)
**
**  str:    	not used
**  onode:  	pointer to OBJECT structure, write only, by reference
**  obj:    	OBJECT structure, read only, by reference
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
static unsigned int node_alloc(char *str, struct OBJECT **onode, struct OBJECT *obj) {

    *onode = obj;
    return SS$_NORMAL;

} /* node_alloc */
