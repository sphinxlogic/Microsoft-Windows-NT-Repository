/*
**  Copyright Dick Munroe, 1985-2000.
**  Rights to distribution and modification are granted as specified in the GNU
**  Public License.
**
**	The functions within this file are used to maintain
**	a stack structure.  Include here all typedefs, pointers
**	and stack maintenance functions.  These functions include
**	push(), pop(), and peek().  A hidden variable, top, is
**	declared within this file and is used to indicate the
**	top of this stack.  
**
*/

#include <stdio.h>
#include "defs.h"

/*
 *	The following struct definition will be used to maintain
 *	the stack of information that will be used by the break
 *	and next code generating functions.  The information 
 *	contained in each node of the stack is the type of loop
 *	being parsed and the number of the last label that has
 *	been used in the code generation process.
 *
 *	The variable declared after the typedef will always point
 *	to the top of the stack.  By declaring it as a static
 *	variable it becomes hidden by all procedures that reside
 *	outside of this file.  Initialize this pointer to NULL (0).
 *	This value indicates that the stack is empty.
 *
 */

typedef struct nodetype NODETYPE;

struct nodetype
{
	int looptype;  /* While loop = 2, corresponding to WHILE = 2. */
	int label;     /* Last label used before this loop. */
	struct nodetype *next; /* Makes this a self-referential struct. */
};

static NODETYPE *top = NULL;

/* Define a macro that will return the size of a stack node. */

#define NODESIZE sizeof(NODETYPE)

/* Define the push(), pop(), and peek() functions. */

extern int errmsg();

void push(ltype, labl)
    int ltype;
    int labl;
    {
	NODETYPE *ptr;
	extern char *malloc();

	/* First create a new node.  Coerce it to point to a nodetype. */
	ptr = (NODETYPE *) malloc(NODESIZE);
	/* 
	 *	Check to see if there was enough stack memory left
	 *	to allocate.  If there wasn't, ptr will be NULL.
	 *	Time for an error message.
	 *
	 */
	if (ptr == NULL){
		errmsg("Fatal error -- loops nested to deep\n" );
		exit(1);
	}

	ptr->looptype = ltype;
	ptr->label = labl;
	ptr->next = top;
	top = ptr;
    }

int pop(pltype, plabl)
    int *pltype;
    int *plabl;
    {
	NODETYPE *ptr;
	extern char *free();

	if (top){
	    *pltype = top->looptype;
	    *plabl = top->label;
	    ptr = top;
	    top = top->next;
	    free((char *) ptr);
	    return( 1 );
	}
	else {
		errmsg("Internal Error -- Attempt to pop an empty stack--");
		errmsg("continuing\n\n");
		return(0);
	}
    }

peek(pltype, plab)
    int *pltype;
    int *plab;
/*
 *	Return the contents of the top of the stack without actually
 *	popping the stack.  This function is the same as pop(), but 
 *	only up to the point where top is changed.  
 */
    {
	if (top) {
	    *pltype = top->looptype;
	    *plab = top->label;
	    return ( 1 );
	}
	else {
		return(0);
	}
    }
