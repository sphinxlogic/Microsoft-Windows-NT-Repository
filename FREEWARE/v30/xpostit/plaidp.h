/*
 * $Header$
 *
 * PlaidP.h - private definitions for the plaid widget.
 *
 * Based on the Template widget from the X11R3 distribution.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94086
 * davy@riacs.edu
 *
 * $Log$
 */

/* Modified by:
            Jatin Desai (VIA::DESAI) , Core Applications Group
	    110 Spitbrook Road, Nashua, NH 03060
	    Digital Equipment Corporation

   to work with DECwindows environment. Look for ifndef VMS and
   ifdef VMS to see changes.
*/


#ifndef VMS
#include <X11/copyright.h>
#endif

#ifndef _PlaidP_h
#define _PlaidP_h

#include <X11/CoreP.h>
#include "Plaid.h"

/*
 * The plaid part of the class record is not used.
 */
typedef struct {
	int		empty;
} PlaidClassPart;

/*
 * Declare the class record for the widget.
 */
typedef struct _PlaidClassRec {
	CoreClassPart	core_class;
	PlaidClassPart	plaid_class;
} PlaidClassRec;

/*
 * Declare the plaid class record type.
 */
extern PlaidClassRec	plaidClassRec;

/*
 * Resources specific to the plaid widget.
 */
typedef struct {
	Pixel		foreground;
	XtCallbackList	input_callback;
} PlaidPart;

/*
 * Declare the widget type.
 */
typedef struct _PlaidRec {
	CorePart	core;
	PlaidPart	plaid;
} PlaidRec;

#endif _PlaidP_h
