/*
 * $Header: PaneP.h,v 1.2 89/01/05 17:58:38 gringort Exp $
 */
 
/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts.
 * 
 *                         All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.  
 * 
 * 
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* 
 * PanePrivate.h - Private definitions for Pane widget
 * 
 * Author:	Terry Weissman
 * 		Digital Equipment Corporation
 * 		Western Software Laboratory
 * Date:	Tue 6 Oct 1987
 *
 * V1.0d 17-Mar-1988	RDB
 *	fix major bugs and enhancements
 * V1.1  15-Apr-1988	MRR
 *	Upward compatibility support.
 * V1.4	 02-Nov-1988	RDB
 *	Apply standards to definitions
 * V1.5	 06-Jan-1988	JG
 *	Modify for PMAX
 */
 
#ifndef _DwtPanePrivate_h
#define _DwtPanePrivate_h
 
/***********************************************************************
 *
 * Pane Widget Private Data
 *
 ***********************************************************************/
 
typedef struct _MullionPart {
    DwtCallbackStruct helpcallback;
} MullionPart;
 
typedef struct _MullionRec {
    CorePart	core;
    MullionPart	mullion;
} MullionRec, *MullionWidget;
/* $Log:	PaneP.h,v $
 * Revision 1.2  89/01/05  17:58:38  gringort
 * fixed position
 * 
 * Revision 1.1  89/01/03  16:59:56  gringort
 * Initial revision
 * 
 * Revision 1.3  88/02/26  13:29:52  weissman
 * Add focus handling to subwindows.
 * 
 * Revision 1.2  87/12/16  15:31:12  weissman
 * Conversion to BL6.
 * 
 * Revision 1.1  87/12/16  15:31:12  asente
 * Initial revision
 * 
 * Revision 1.1  87/11/12  13:36:57  susan
 * Initial revision
 * 
 * Revision 1.1  87/11/04  15:03:12  chow
 * Initial revision
 * 
 * Revision 1.1  87/11/01  12:25:01  haynes
 * Initial revision
 * 
 *  */
 
 
#endif _DwtPanePrivate_h
/* DON'T ADD STUFF AFTER THIS #endif */
 
/*  DEC/CMS REPLACEMENT HISTORY, Element PANEP.H */
/*  *4    30-MAR-1989 14:17:07 BRINKLEY "definitions of global variables fixed" */
/*  *3    28-MAR-1989 11:07:58 BRINKLEY "Style Guide Conformity" */
/*  *2     6-JAN-1989 12:36:17 BRINKLEY "new version" */
/*  *1    20-DEC-1988 14:51:26 RYAN "Initial elements for V2" */
/*  DEC/CMS REPLACEMENT HISTORY, Element PANEP.H */
