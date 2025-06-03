/***********************************************************
		Copyright IBM Corporation 1987,1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $XFree86: mit/server/ddx/x386/vga16/ibm/vgaOSD.c,v 2.2 1993/08/28 07:54:22 dawes Exp $ */

#include <sys/types.h> /* GJA -- moved this here */

#include "X.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"
#include "pixmapstr.h"
#include "OScompiler.h"

#include "vgaVideo.h"
#include "vgaSave.h"
#include "ppc.h"

/* Global Variables */
int BYTES_PER_ROW, MAX_ROW, MAX_COLUMN, MAX_OFFSCREEN_ROW;
int vgaDisplayTubeType = 0 ;

int
vga16Probe(virtx,virty)
int virtx, virty;
{
  extern ppcScrnPriv vgaScrnPriv;
  extern ScreenRec vgaScreenRec;
  int i;

  /*
   * patch the original structures
   */
  BYTES_PER_ROW = virtx / 8;
  MAX_ROW       = virty - 1;
  MAX_COLUMN    = virtx - 1;

  vgaScrnPriv.pixmap.drawable.width = MAX_COLUMN + 1;
  vgaScrnPriv.pixmap.drawable.height = MAX_ROW + 1;
  vgaScrnPriv.pixmap.devKind = (MAX_ROW + 1) / 8;
  vgaScreenRec.width = MAX_COLUMN + 1;
  vgaScreenRec.height = MAX_ROW + 1;

  vgaDisplayTubeType = COLOR_TUBE;

  return 0;
}

