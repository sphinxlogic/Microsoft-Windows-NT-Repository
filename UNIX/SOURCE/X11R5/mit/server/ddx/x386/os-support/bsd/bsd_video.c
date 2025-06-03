/*
 * Copyright 1992 by Rich Murphey <Rich@Rice.edu>
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Rich Murphey and David Wexelblat 
 * not be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  Rich Murphey and
 * David Wexelblat make no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * RICH MURPHEY AND DAVID WEXELBLAT DISCLAIM ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL RICH MURPHEY OR DAVID WEXELBLAT BE LIABLE FOR 
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/os-support/bsd/bsd_video.c,v 2.15 1993/12/17 11:01:36 dawes Exp $ */

#include "X.h"
#include "input.h"
#include "scrnintstr.h"

#include "x386.h"
#include "x386Priv.h"
#include "xf86_OSlib.h"

/***************************************************************************/
/* Video Memory Mapping section                                            */
/***************************************************************************/

#define _386BSD_MMAP_BUG

#ifdef _386BSD_MMAP_BUG
/*
 * Bug prevents multiple mappings, so just map a fixed region between 0xA0000
 * and 0xBFFFF, and return a pointer to the requested Base.
 */
static int MemMapped = FALSE;
static pointer MappedPointer = NULL;
static int MapCount = 0;
#define MAP_BASE 0xA0000
#define MAP_SIZE 0x20000
#endif

static Bool devMemChecked = FALSE;
static Bool useDevMem = FALSE;

/*
 * Check if /dev/mem can be mmap'd.  If it can't print a warning when
 * "warn" is TRUE.
 */
static void checkDevMem(warn)
Bool warn;
{
	int fd;
	pointer base;

	devMemChecked = TRUE;
	if ((fd = open("/dev/mem", O_RDWR)) < 0)
	{
	    if (warn)
	    {
	        ErrorF("checkDevMem: warning: failed to open /dev/mem (%s)\n",
		       strerror(errno));
	        ErrorF("\tlinear fb access unavailable\n");
	    }
	    useDevMem = FALSE;
	    return;
	}
	/* Try to map a page at the VGA address */
	base = (pointer)mmap((caddr_t)0, 4096, PROT_READ|PROT_WRITE,
			     MAP_FILE, fd, (off_t)0xA0000);
	close(fd);
	if (base == (pointer)-1)
	{
	    if (warn)
	    {
	        ErrorF("checkDevMem: warning: failed to mmap /dev/mem (%s)\n",
		       strerror(errno));
	        ErrorF("\tlinear fb access unavailable\n");
	    }
	    useDevMem = FALSE;
	    return;
	}
	munmap((caddr_t)base, 4096);
	useDevMem = TRUE;
	return;
}


pointer xf86MapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
	pointer base;

	if (!devMemChecked)
		checkDevMem(FALSE);

	if (useDevMem)
	{
	    int memFd;

	    if ((memFd = open("/dev/mem", O_RDWR)) < 0)
	    {
		FatalError("xf86MapVidMem: failed to open /dev/mem (%s)\n",
			   strerror(errno));
	    }
	    base = (pointer)mmap((caddr_t)0, Size, PROT_READ|PROT_WRITE,
				 MAP_FILE, memFd, (off_t)Base);
	    close(memFd);
	    if (base == (pointer)-1)
	    {
		FatalError("%s: could not mmap /dev/mem [s=%x,a=%x] (%s)\n",
			   "xf86MapVidMem", Size, Base, strerror(errno));
	    }
	    return(base);
	}
		
	/* else, mmap /dev/vga */
#ifdef _386BSD_MMAP_BUG
	if ((unsigned long)Base < MAP_BASE ||
	    (unsigned long)Base >= MAP_BASE + MAP_SIZE)
	{
		FatalError("%s: Address 0x%x outside allowable range\n",
			   "xf86MapVidMem", Base);
	}
	if ((unsigned long)Base + Size > MAP_BASE + MAP_SIZE)
	{
		FatalError("%s: Size 0x%x too large (Base = 0x%x)\n",
			   "xf86MapVidMem", Size, Base);
	}
	if (!MemMapped)
	{
		base = (pointer)mmap(0, MAP_SIZE, PROT_READ|PROT_WRITE,
				     MAP_FILE, x386Info.screenFd, 0);
		if (base == (pointer)-1)
		{
		    FatalError("xf86MapVidMem: Could not mmap /dev/vga (%s)\n",
			       strerror(errno));
		}
		MappedPointer = base;
		MemMapped = TRUE;
	}
	MapCount++;
	return((pointer)((unsigned long)MappedPointer +
			 ((unsigned long)Base - MAP_BASE)));

#else
	if ((unsigned long)Base < 0xA0000 || (unsigned long)Base >= 0xC0000)
	{
		FatalError("%s: Address 0x%x outside allowable range\n",
			   "xf86MapVidMem", Base);
	}
	base = (pointer)mmap(0, Size, PROT_READ|PROT_WRITE, MAP_FILE,
			     x386Info.screenFd,
			     (unsigned long)Base - 0xA0000);
	if (base == (pointer)-1)
	{
	    FatalError("xf86MapVidMem: Could not mmap /dev/vga (%s)\n",
		       strerror(errno));
	}
	return(base);
#endif
}

void xf86UnMapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
	if (useDevMem)
	{
		munmap((caddr_t)Base, Size);
		return;
	}

#ifdef _386BSD_MMAP_BUG
	if (MapCount == 0 || MappedPointer == NULL)
		return;

	if (--MapCount == 0)
	{
		munmap((caddr_t)MappedPointer, MAP_SIZE);
		MemMapped = FALSE;
	}
#else
	munmap((caddr_t)Base, Size);
#endif
}

Bool xf86LinearVidMem()
{
	/*
	 * Call checkDevMem even if already called by xf86MapVidMem() so that
	 * a warning about no linear fb is printed.
	 */
	if (!useDevMem)
		checkDevMem(TRUE);

	return(useDevMem);
}

/***************************************************************************/
/* Interrupt Handling section                                              */
/***************************************************************************/

Bool xf86DisableInterrupts()
{

#ifdef __GNUC__
	__asm__ __volatile__("cli");
#else 
	asm("cli");
#endif /* __GNUC__ */

	return(TRUE);
}

void xf86EnableInterrupts()
{

#ifdef __GNUC__
	__asm__ __volatile__("sti");
#else 
	asm("sti");
#endif /* __GNUC__ */

	return;
}
