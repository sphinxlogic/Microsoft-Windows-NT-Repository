/*-
 * mach32bstore.c --
 *	Functions required by the backing-store implementation in MI.
 *
 * Copyright (c) 1987 by the Regents of the University of California
 * Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 *
 * KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 */

/* $XFree86: mit/server/ddx/x386/accel/mach32/mach32bstor.c,v 2.3 1994/02/12 11:05:31 dawes Exp $ */

#include    "cfb.h"
#include    "X.h"
#include    "mibstore.h"
#include    "regionstr.h"
#include    "scrnintstr.h"
#include    "pixmapstr.h"
#include    "windowstr.h"
#include    "mach32.h"

void
mach32SaveAreas(pPixmap, prgnSave, xorg, yorg)
    PixmapPtr	  	pPixmap;  	/* Backing pixmap */
    RegionPtr	  	prgnSave; 	/* Region to save (pixmap-relative) */
    int	    	  	xorg;	    	/* X origin of region */
    int	    	  	yorg;	    	/* Y origin of region */
{
    register BoxPtr	pBox;
    register int	i;
    int			pixWidth;

    if (!x386VTSema)
    {
	cfbSaveAreas(pPixmap, prgnSave, xorg, yorg);
	return;
    }

    i = REGION_NUM_RECTS(prgnSave);
    pBox = REGION_RECTS(prgnSave);

    pixWidth = PixmapBytePad(pPixmap->drawable.width, pPixmap->drawable.depth);

    while (i--) {
	(mach32ImageReadFunc)(pBox->x1 + xorg, pBox->y1 + yorg,
			 pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
			 pPixmap->devPrivate.ptr, pixWidth,
			 pBox->x1, pBox->y1, ~0);
	pBox++;
    }

    WaitIdleEmpty(); /* Make sure that all commands have finished */
}

void
mach32RestoreAreas(pPixmap, prgnRestore, xorg, yorg)
    PixmapPtr	  	pPixmap;  	/* Backing pixmap */
    RegionPtr	  	prgnRestore; 	/* Region to restore (screen-relative)*/
    int	    	  	xorg;	    	/* X origin of window */
    int	    	  	yorg;	    	/* Y origin of window */
{
    register BoxPtr	pBox;
    register int	i;
    int			pixWidth;

    if (!x386VTSema)
    {
	cfbRestoreAreas(pPixmap, prgnRestore, xorg, yorg);
	return;
    }

    i = REGION_NUM_RECTS(prgnRestore);
    pBox = REGION_RECTS(prgnRestore);

    pixWidth = PixmapBytePad(pPixmap->drawable.width, pPixmap->drawable.depth);

    while (i--) {
	(mach32ImageWriteFunc)(pBox->x1, pBox->y1,
			  pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
			  pPixmap->devPrivate.ptr, pixWidth,
			  pBox->x1 - xorg, pBox->y1 - yorg,
			  mach32alu[GXcopy], 0xffffffff);
	pBox++;
    }

    WaitIdleEmpty(); /* Make sure that all commands have finished */
}
