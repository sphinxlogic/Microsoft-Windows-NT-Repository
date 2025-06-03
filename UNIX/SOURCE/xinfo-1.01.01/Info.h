#ifndef _Info_h
#define _Info_h

/* $Header: /usr3/xinfo/RCS/Info.h,v 1.4 90/11/12 13:49:47 jkh Exp Locker: jkh $ */

/*
 *
 *                   Copyright 1989, 1990
 *                    Jordan K. Hubbard
 *
 *                PCS Computer Systeme, GmbH.
 *                   Munich, West Germany
 *
 *
 * This file is part of GNU Info widget.
 *
 * The GNU Info widget is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 */

/*
 * $Log:	Info.h,v $
 * Revision 1.4  90/11/12  13:49:47  jkh
 * Fixed bell_volume misspecification.
 * 
 * Revision 1.3  90/11/11  22:25:14  jkh
 * Added retainArg stuff.
 * 
 * Revision 1.2  90/11/11  21:19:53  jkh
 * Release 1.01
 * 
 * Revision 1.1  90/11/06  22:51:15  jkh
 * Initial revision.
 * 
 */

/*
 * Resources:
 *
 * Name			Class		RepType		Default Value
 * ----			-----		-------		-------------
 * bellVolume		BellVolume	Int		XpDefaultBellVolume
 * callback             Callback        XtCallbackList  NULL
 * infoFile		InfoFile	String		XpDefaultInfoFile
 * infoNode		InfoNode	String		XpDefaultInfoNode
 * infoPath		InfoPath	String		XpDefaultInfoPath
 * printCommand		PrintCommand	String		XpDefaultPrintCommand
 * retainArg		RetainArg	Boolean		FALSE
 *
 */

/* resource types */
#define XpDefaultBellVolume	50
#define XpDefaultInfoFile	"dir"
#define XpDefaultInfoNode	"Top"
#define XpDefaultInfoPath	"/usr/gnu/lib/emacs/info:/usr/local/lib/emacs/info:/usr/gnu/lib/info"
#define XpDefaultPrintCommand	"lpr -p"

#define XpNbellVolume		"bellVolume"
#define XpNinfoFile		"infoFile"
#define XpNinfoHelp		"infoHelp"
#define XpNinfoNode		"infoNode"
#define XpNinfoPath		"infoPath"
#define XpNprintCommand		"printCommand"
#define XpNretainArg		"retainArg"

#define XpCBellVolume		"BellVolume"
#define XpCInfoFile		"InfoFile"
#define XpCInfoHelp		"InfoHelp"
#define XpCInfoNode		"InfoNode"
#define XpCInfoPath		"InfoPath"
#define XpCPrintCommand		"PrintCommand"
#define XpCRetainArg		"RetainArg"

#define XpInfoVersion		1.01

/* declare specific InfoWidget class and instance datatypes */
typedef struct _InfoClassRec*		InfoWidgetClass;
typedef struct _InfoRec*		InfoWidget;

/* declare the class constant */
extern WidgetClass infoWidgetClass;

#endif /* _Info_h */
