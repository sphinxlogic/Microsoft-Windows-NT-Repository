/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Thomas Roell and David Wexelblat 
 * not be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  Thomas Roell and
 * David Wexelblat makes no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * THOMAS ROELL AND DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THOMAS ROELL OR DAVID WEXELBLAT BE LIABLE FOR 
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/os-support/sysv/sysv_video.c,v 2.16 1993/12/25 14:01:24 dawes Exp $ */

#include "X.h"
#include "input.h"
#include "scrnintstr.h"

#define _NEED_SYSI86
#include "x386.h"
#include "x386Priv.h"
#include "xf86_OSlib.h"

/***************************************************************************/
/* Video Memory Mapping section                                            */
/***************************************************************************/

struct kd_memloc MapDSC[MAXSCREENS][NUM_REGIONS];
pointer AllocAddress[MAXSCREENS][NUM_REGIONS];
#ifndef SVR4
static int mmapFd = -2;
#endif

Bool xf86LinearVidMem()
{
#ifdef SVR4
	return TRUE;
#else
#ifdef HAS_SVR3_MMAPDRV
	if(mmapFd >= 0)
	{
		return TRUE;
	}
	if ((mmapFd = open("/dev/mmap", O_RDWR)) != -1)
	{
	    if(ioctl(mmapFd, GETVERSION) < 0x0222) {
		ErrorF("xf86LinearVidMem: MMAP 2.2.2 or above required\n");
		ErrorF(" linear memory access disabled\n");
		return FALSE;
	    }
	    return TRUE;
	}
	ErrorF("xf86LinearVidMem: failed to open /dev/mmap (%s)\n",
	       strerror(errno));
	ErrorF(" linear memory access disabled\n");
#endif
	return FALSE;
#endif
}

pointer xf86MapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
	pointer base;
	int fd;

#if defined(SVR4)
	if ((fd = open("/dev/pmem", O_RDWR)) < 0)
	{
		FatalError("xf86MapVidMem: failed to open /dev/pmem (%s)\n",
			   strerror(errno));
	}
	base = (pointer)mmap((caddr_t)0, Size, PROT_READ|PROT_WRITE,
			     MAP_SHARED, fd, (off_t)Base);
	close(fd);
	if ((long)base == -1)
	{
		FatalError("%s: Could not mmap framebuffer [s=%x,a=%x] (%s)\n",
			   "xf86MapVidMem", Size, Base, strerror(errno));
	}
#else /* SVR4 */
#ifdef HAS_SVR3_MMAPDRV
	if (mmapFd == -2)
	{
		mmapFd = open("/dev/mmap", O_RDWR);
	}
#endif
	if (mmapFd >= 0)
	{
		/* To force the MMAP driver to provide the address */
		base = (pointer)0;
	}
	else
	{
	    AllocAddress[ScreenNum][Region] = (pointer)xalloc(Size + 0x1000);
	    if (AllocAddress[ScreenNum][Region] == (pointer)0)
	    {
		FatalError("xf86MapVidMem: can't alloc framebuffer space\n");
		/* NOTREACHED */
	    }
	    base = (pointer)(((unsigned int)AllocAddress[ScreenNum][Region]
			      & ~0xFFF) + 0x1000);
	}
	MapDSC[ScreenNum][Region].vaddr    = (char *)base;
	MapDSC[ScreenNum][Region].physaddr = (char *)Base;
	MapDSC[ScreenNum][Region].length   = Size;
	MapDSC[ScreenNum][Region].ioflg    = 1;

#ifdef HAS_SVR3_MMAPDRV
	if(mmapFd >= 0)
	{
	    if((base = (pointer)ioctl(mmapFd, MAP,
			   &(MapDSC[ScreenNum][Region]))) == (pointer)-1)
	    {
		FatalError("%s: Could not mmap framebuffer [s=%x,a=%x] (%s)\n",
			   "xf86MapVidMem", Size, Base, strerror(errno));
		/* NOTREACHED */
	    }

	    /* Next time we want the same address! */
	    MapDSC[ScreenNum][Region].vaddr    = (char *)base;
	    return((pointer)base);
	}
#endif
	if (ioctl(x386Info.consoleFd, KDMAPDISP,
		  &(MapDSC[ScreenNum][Region])) < 0)
	{
	    FatalError("xf86MapVidMem: Failed to map video mem (%x,%x) (%s)\n",
		        Base, Size, strerror(errno));
	    /* NOTREACHED */
	}
#endif /* SVR4 */
	return((pointer)base);
}

/* ARGSUSED */
void xf86UnMapVidMem(ScreenNum, Region, Base, Size)
int ScreenNum;
int Region;
pointer Base;
unsigned long Size;
{
#if defined (SVR4)
	munmap(Base, Size);
#else /* SVR4 */
#ifdef HAS_SVR3_MMAPDRV
	if(mmapFd >= 0)
	{
		ioctl(mmapFd, UNMAPRM, MapDSC[ScreenNum][Region].vaddr);
		return;
	}
#endif
	/* XXXX This is a problem because it unmaps all regions */
	ioctl(x386Info.consoleFd, KDUNMAPDISP, 0);
	xfree(AllocAddress[ScreenNum][Region]);
#endif /* SVR4 */
}

/* ARGSUSED */
void xf86MapDisplay(ScreenNum, Region)
int ScreenNum;
int Region;
{
#if !defined(SVR4)
#ifdef HAS_SVR3_MMAPDRV
	if(mmapFd >= 0)
	{
		ioctl(mmapFd, MAP, &(MapDSC[ScreenNum][Region]));
		return;
	}
#endif
	ioctl(x386Info.consoleFd, KDMAPDISP, &(MapDSC[ScreenNum][Region]));
#endif /* SVR4 */
	return;
}

/* ARGSUSED */
void xf86UnMapDisplay(ScreenNum, Region)
int ScreenNum;
int Region;
{
#if !defined(SVR4)
#ifdef HAS_SVR3_MMAPDRV
	if(mmapFd > 0)
	{
		ioctl(mmapFd, UNMAP, MapDSC[ScreenNum][Region].vaddr);
		return;
	}
#endif
	ioctl(x386Info.consoleFd, KDUNMAPDISP, 0);
#endif /* SVR4 */
	return;
}

/***************************************************************************/
/* I/O Permissions section                                                 */
/***************************************************************************/

#define DISABLED	0
#define NON_EXTENDED	1
#define EXTENDED	2

static unsigned *EnabledPorts[MAXSCREENS];
static int NumEnabledPorts[MAXSCREENS];
static Bool ScreenEnabled[MAXSCREENS];
static Bool ExtendedPorts[MAXSCREENS];
static Bool ExtendedEnabled = FALSE;
static Bool InitDone = FALSE;
static struct kd_disparam OrigParams;

void xf86ClearIOPortList(ScreenNum)
int ScreenNum;
{
	if (!InitDone)
	{
		xf86InitPortLists(EnabledPorts, NumEnabledPorts, ScreenEnabled,
				  ExtendedPorts, MAXSCREENS);
		if (ioctl(x386Info.consoleFd, KDDISPTYPE, &OrigParams) < 0)
		{
			FatalError("%s: Could not get display parameters\n",
				   "xf86ClearIOPortList");
		}
		InitDone = TRUE;
		return;
	}
	ExtendedPorts[ScreenNum] = FALSE;
	if (EnabledPorts[ScreenNum] != (unsigned *)NULL)
		xfree(EnabledPorts[ScreenNum]);
	EnabledPorts[ScreenNum] = (unsigned *)NULL;
	NumEnabledPorts[ScreenNum] = 0;
}

void xf86AddIOPorts(ScreenNum, NumPorts, Ports)
int ScreenNum;
int NumPorts;
unsigned *Ports;
{
	int i;

	if (!InitDone)
	{
	    FatalError("xf86AddIOPorts: I/O control lists not initialised\n");
	}
	EnabledPorts[ScreenNum] = (unsigned *)xrealloc(EnabledPorts[ScreenNum], 
			(NumEnabledPorts[ScreenNum]+NumPorts)*sizeof(unsigned));
	for (i = 0; i < NumPorts; i++)
	{
		EnabledPorts[ScreenNum][NumEnabledPorts[ScreenNum] + i] =
								Ports[i];
		if (Ports[i] > 0x3FF)
			ExtendedPorts[ScreenNum] = TRUE;
	}
	NumEnabledPorts[ScreenNum] += NumPorts;
}

void xf86EnableIOPorts(ScreenNum)
int ScreenNum;
{
	struct kd_disparam param;
	int i, j;

	if (ScreenEnabled[ScreenNum])
		return;

	for (i = 0; i < MAXSCREENS; i++)
	{
		if (ExtendedPorts[i] && (ScreenEnabled[i] || i == ScreenNum))
		{
		    if (sysi86(SI86V86, V86SC_IOPL, PS_IOPL) < 0)
		    {
			FatalError("%s: Failed to set IOPL for extended I/O\n",
				   "xf86EnableIOPorts");
		    }
		    ExtendedEnabled = TRUE;
		    break;
		}
	}
	/* If extended I/O was used, but isn't any more */
	if (ExtendedEnabled && i == MAXSCREENS)
	{
		sysi86(SI86V86, V86SC_IOPL, 0);
		ExtendedEnabled = FALSE;
	}
	/*
	 * Turn on non-extended ports even when using extended I/O
	 * so they are there if extended I/O gets turned off when it's no
	 * longer needed.
	 */
	if (ioctl(x386Info.consoleFd, KDDISPTYPE, &param) < 0)
	{
		FatalError("%s: Could not get display parameters\n",
			   "xf86EnableIOPorts");
	}
	for (i = 0; i < NumEnabledPorts[ScreenNum]; i++)
	{
		unsigned port = EnabledPorts[ScreenNum][i];

		if (port > 0x3FF)
			continue;

		if (!xf86CheckPorts(port, EnabledPorts, NumEnabledPorts,
				    ScreenEnabled, MAXSCREENS))
		{
			continue;
		}
		for (j=0; j < MKDIOADDR; j++)
		{
			if (param.ioaddr[j] == port)
			{
				break;
			}
		}
		if (j == MKDIOADDR)
		{
			if (ioctl(x386Info.consoleFd, KDADDIO, port) < 0)
			{
				FatalError("%s: Failed to enable port 0x%x\n",
					   "xf86EnableIOPorts", port);
			}
		}
	}
	if (ioctl(x386Info.consoleFd, KDENABIO, 0) < 0)
	{
		FatalError("xf86EnableIOPorts: I/O port enable failed (%s)\n",
			   strerror(errno));
	}
	ScreenEnabled[ScreenNum] = TRUE;
	return;
}

void xf86DisableIOPorts(ScreenNum)
int ScreenNum;
{
	struct kd_disparam param;
	int i, j;

	if (!ScreenEnabled[ScreenNum])
		return;

	ScreenEnabled[ScreenNum] = FALSE;
	for (i = 0; i < MAXSCREENS; i++)
	{
		if (ScreenEnabled[i] && ExtendedPorts[i])
			break;
	}
	if (ExtendedEnabled && i == MAXSCREENS)
	{
		sysi86(SI86V86, V86SC_IOPL, 0);
		ExtendedEnabled = FALSE;
	}
	/* Turn off I/O before changing the access list */
	ioctl(x386Info.consoleFd, KDDISABIO, 0);
	if (ioctl(x386Info.consoleFd, KDDISPTYPE, &param) < 0)
	{
		ErrorF("%s: Could not get display parameters\n",
		       "xf86DisableIOPorts");
		return;
	}

	for (i=0; i < MKDIOADDR; i++)
	{
		/* 0 indicates end of list */
		if (param.ioaddr[i] == 0)
		{
			break;
		}
		if (!xf86CheckPorts(param.ioaddr[i], EnabledPorts,
				    NumEnabledPorts, ScreenEnabled, MAXSCREENS))
		{
			continue;
		}
		for (j=0; j < MKDIOADDR; j++)
		{
			if (param.ioaddr[i] == OrigParams.ioaddr[j])
			{
				/*
				 * Port was one of the original ones; don't
				 * touch it.
				 */
				break;
			}
		}
		if (j == MKDIOADDR)
		{
			/*
			 * We added this port, so remove it.
			 */
			ioctl(x386Info.consoleFd, KDDELIO, param.ioaddr[i]);
		}
	}
	/* If any other screens are enabled, turn I/O back on */
	for (i = 0; i < MAXSCREENS; i++)
	{
		if (ScreenEnabled[i])
		{
			ioctl(x386Info.consoleFd, KDENABIO, 0);
			break;
		}
	}
	return;
}

void xf86DisableIOPrivs()
{
	if (ExtendedEnabled)
		sysi86(SI86V86, V86SC_IOPL, 0);
	return;
}

/***************************************************************************/
/* Interrupt Handling section                                              */
/***************************************************************************/

Bool xf86DisableInterrupts()
{
	if (!ExtendedEnabled)
	{
		if (sysi86(SI86V86, V86SC_IOPL, PS_IOPL) < 0)
		{
			return(FALSE);
		}
	}

#ifdef __GNUC__
	__asm__ __volatile__("cli");
#else 
	asm("cli");
#endif /* __GNUC__ */

	if (!ExtendedEnabled)
	{
		sysi86(SI86V86, V86SC_IOPL, 0);
	}
	return(TRUE);
}

void xf86EnableInterrupts()
{
	if (!ExtendedEnabled)
	{
		if (sysi86(SI86V86, V86SC_IOPL, PS_IOPL) < 0)
		{
			return;
		}
	}

#ifdef __GNUC__
	__asm__ __volatile__("sti");
#else 
	asm("sti");
#endif /* __GNUC__ */

	if (!ExtendedEnabled)
	{
		sysi86(SI86V86, V86SC_IOPL, 0);
	}
	return;
}
