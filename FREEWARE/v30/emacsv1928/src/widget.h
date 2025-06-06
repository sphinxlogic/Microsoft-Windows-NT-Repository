/* The emacs frame widget public header file.
   Copyright (C) 1993 Free Software Foundation, Inc.

This file is part of GNU Emacs.

GNU Emacs is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Emacs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Emacs; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Emacs 19 face widget ported by Fred Pierresteguy */

#ifndef _EmacsFrame_h
#define _EmacsFrame_h

#define XtNminibuffer "minibuffer"
#define XtCMinibuffer "Minibuffer"
#define XtNunsplittable "unsplittable"
#define XtCUnsplittable "Unsplittable"
#define XtNinternalBorderWidth "internalBorderWidth"
#define XtCInternalBorderWidth "InternalBorderWidth"
#define XtNinterline "interline"
#define XtCInterline "Interline"

#ifndef XtNfont
#define XtNfont "font"
#endif
#ifndef XtCFont
#define XtCFont "Font"
#endif
#ifndef XtNforeground
#define XtNforeground "foreground"
#endif
#ifndef XtCForeground
#define XtCForeground "Foreground"
#endif

#define XtNcursorColor "cursorColor"
#define XtCCursorColor "CursorColor"
#define XtNbarCursor "barCursor"
#define XtCBarCursor "BarCursor"

#define XtNvisualBell "visualBell"
#define XtCVisualBell "VisualBell"
#define XtCBellVolume "BellVolume"
#define XtNbellVolume "bellVolume"

#define XtNpointerBackground "pointerBackground"
#define XtNpointerColor "pointerColor"

#define XtNtextPointer "textPointer"
#define XtNspacePointer "spacePointer"
#define XtNmodeLinePointer "modePointer"
#define XtNgcPointer "gcPointer"

#define XtNemacsFrame "emacsFrame"
#define XtCEmacsFrame "EmacsFrame"

#ifndef XtNgeometry
#define XtNgeometry "geometry"
#endif
#ifndef XtCGeometry
#define XtCGeometry "Geometry"
#endif

#define XtNinitialGeometry "initialGeometry"
#define XtCInitialGeometry "InitialGeometry"

/* structures
 */
typedef struct _EmacsFrameRec *EmacsFrame;
typedef struct _EmacsFrameClassRec *EmacsFrameClass;

extern WidgetClass emacsFrameClass;

extern struct _DisplayContext* display_context;

/* Special entrypoints */
void EmacsFrameSetCharSize ();

#endif /* _EmacsFrame_h */
