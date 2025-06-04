/****************************************************************************/
/*									    */
/*  FACILITY:	Generic Support Library					    */
/*									    */
/*  MODULE:	Object Memory Management Header				    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This header file contains forward declarations for the	    */
/*  Object Memory Management module.					    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#ifndef __OBJALLOC_H				/* If not already included. */
#define __OBJALLOC_H
#include "globdb.h"				/* Program-specific global  */
						/* database.		    */

/*									    */
/* Forward declarations.						    */
/*									    */

char *obj_alloc();
char *obj_free();
char *new_str();
char *free_str();
void trace_new_obj();
void trace_free_obj();

#endif						/* If not already included. */
