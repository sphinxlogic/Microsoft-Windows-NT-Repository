/****************************************************************************/
/*									    */
/*  FACILITY:	Generic Support Library					    */
/*									    */
/*  MODULE:	Object Memory Management 				    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains routines to support memory allocation */
/*  and deallocation of application objects from the heap. It supports	    */
/*  string objects directly, and provides routines to be used by other	    */
/*  object types, both to do the actual memory management, and for tracing  */
/*  the activity to stdout.						    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#include "objalloc.h"

/*************************************************************************++*/
char *obj_alloc(
/* Allocates and clears memory for an object, warning if the allocation	    */
/* exceeds the expected limit for the object, or the available memory.	    */

    long    vNBytes,
	    /* (READ, BY VAL):  					    */
	    /* Number of bytes to allocate.				    */

    long    vMaxBytes,
	    /* (READ, BY VAL):  					    */
	    /* Maximum number of bytes for this type of object. If vNBytes  */
	    /* is larger than vMaxBytes, a warning will be issued, since    */
	    /* this indicates that some assumption about the size of an	    */
	    /* object has been violated, but the object will still be	    */
	    /* allocated.						    */

    char    *aTypeName
	    /* (READ, BY ADDR):  					    */
	    /* Object type name string.					    */

)	/* Returns ptr to allocated object, or NULL if the memory could not */
	/* be allocated.						    */
	/*****************************************************************--*/

{
    char    *ptr;			    /* Ptr to allocated memory.	    */

    if (vNBytes > vMaxBytes) {
	printf("WARNING: Alloc of %ld bytes for %s exceeds max of %ld bytes\n",
	    vNBytes, aTypeName, vMaxBytes);
    }
    if ((ptr = calloc(1, vNBytes)) == NULL) {
	printf("WARNING: Alloc of %ld bytes for %s failed\n", vNBytes,
	    aTypeName);
    }
    else {
	inc_nobjects();
	inc_nobjbytes(vNBytes);
	if (trace_mem_enabled()) {
	    printf(
	"TRACE: Alloc %ld bytes for %s @ %lxh, %ld objects (%ld bytes)\n",
		vNBytes, aTypeName, ptr, num_objects(), num_objbytes());
	}
    }
    return ptr;
}

/*************************************************************************++*/
char *obj_free(
/* Frees memory allocated for an object.				    */

    void    *aObject,
	    /* (DELETE, BY ADDR):  					    */
	    /* Object to be deleted.					    */

    long    vNBytes,
	    /* (READ, BY VAL):  					    */
	    /* Size of object to free.					    */

    char    *aTypeName
	    /* (READ, BY ADDR):  					    */
	    /* Object type name string.					    */

)	/* Returns NULL ptr.						    */
	/*****************************************************************--*/

{
    dec_nobjects();
    dec_nobjbytes(vNBytes);
    if (trace_mem_enabled()) {
	printf("TRACE: Free %ld bytes for %s @ %lxh, %ld objects (%ld bytes)\n",
	    vNBytes, aTypeName, aObject, num_objects(), num_objbytes());
    }
    free(aObject);
    return NULL;
}

/*************************************************************************++*/
char *new_str(
/* Allocates and initializes a string object.				    */

    char    *aStr,
	    /* (READ, BY ADDR):  					    */
	    /* String value. The memory will be allocated just large enough */
	    /* to hold this string.					    */

    long    vMaxBytes,
	    /* (READ, BY VAL):						    */
	    /* Maximum number of bytes for this type of object. If length   */
	    /* of aStr is larger than vMaxBytes (which is assumed to	    */
	    /* exclude the string null terminator), a warning will be	    */
	    /* issued, since this indicates that some assumption about the  */
	    /* size of an object has been violated, but the object will	    */
	    /* still be allocated.					    */

    char    *aTypeName
	    /* (READ, BY ADDR):  					    */
	    /* Object type name string.					    */

)	/* Returns ptr to allocated object, or NULL if the memory could not */
	/* be allocated.						    */
	/*****************************************************************--*/

{
    int	    length;			    /* Length of string.	    */
    char    *ptr;			    /* Ptr to allocated memory.	    */

    length = strlen(aStr) + 1;
    if ((ptr = obj_alloc(length, vMaxBytes + 1, aTypeName)) != NULL) {
	inc_nstrings();
	strcpy(ptr, aStr);
	if (trace_str_enabled()) {
	    printf("TRACE: Alloc %s string \"%s\"\n", aTypeName, ptr);
	    printf("       @ %lxh, %ld objects (%ld bytes), %ld strings\n",
		ptr, num_objects(), num_objbytes(), num_strings());
    	}
    }
    return ptr;
}

/*************************************************************************++*/
char *free_str(
/* Frees a string object.						    */

    char    *aStr,
	    /* (DELETE, BY ADDR):  					    */
	    /* String to free.						    */

    char    *aTypeName
	    /* (READ, BY ADDR):  					    */
	    /* Object type name string.					    */

)	/* Returns NULL ptr.						    */
	/*****************************************************************--*/

{
    dec_nstrings();
    if (trace_str_enabled()) {
	printf("TRACE: Free %s string \"%s\"\n", aTypeName, aStr);
	printf("       @ %lxh freed, %ld objects (%ld bytes), %ld strings\n",
	    aStr, num_objects(), num_objbytes(), num_strings());
    }
    obj_free(aStr, strlen(aStr) + 1, aTypeName);
    return NULL;
}

/*************************************************************************++*/
void trace_new_obj(
/* Prints a trace message for an object allocation.			    */

    void    *aObject,
	    /* (READ, BY ADDR):  					    */
	    /* Object to be traced.					    */

    char    *aTypeName,
	    /* (READ, BY ADDR):  					    */
	    /* Object type name string.					    */

    char    *aName,
	    /* (READ, BY ADDR):  					    */
	    /* Object instance name string.				    */

    long    vNObjects
	    /* (READ, BY VAL):  					    */
	    /* Total number of objects of this type allocated after this    */
	    /* one was allocated.					    */

)	/* No return value.      					    */
	/*****************************************************************--*/

{
    printf("TRACE: Alloc %s %s\n", aTypeName, aName);
    printf("       @ %lxh, %ld objects (%ld bytes), %ld %ss\n",
	aObject, num_objects(), num_objbytes(), vNObjects, aTypeName);
}

/*************************************************************************++*/
void trace_free_obj(
/* Prints a trace message for an object deallocation.			    */

    void    *aObject,
	    /* (READ, BY ADDR):  					    */
	    /* Object to be traced.					    */

    char    *aTypeName,
	    /* (READ, BY ADDR):  					    */
	    /* Object type name string.					    */

    char    *aName,
	    /* (READ, BY ADDR):  					    */
	    /* Object instance name string.				    */

    long    vNObjects
	    /* (READ, BY VAL):  					    */
	    /* Total number of objects of this type allocated before this   */
	    /* one is freed.						    */

)	/* No return value.      					    */
	/*****************************************************************--*/

{
    printf("TRACE: Free %s %s\n", aTypeName, aName);
    printf("       @ %lxh, %ld objects (%ld bytes), %ld %ss\n",
	aObject, num_objects(), num_objbytes(), vNObjects - 1, aTypeName);
}

