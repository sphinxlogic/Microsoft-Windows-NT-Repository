/*
 * $XFree86: mit/server/ddx/x386/common/xf86_Mouse.c,v 2.7 1994/02/10 21:25:34 dawes Exp $
 *
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * Copyright 1993 by David Dawes <dawes@physics.su.oz.au>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Thomas Roell and David Dawes not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Thomas Roell
 * and David Dawes makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THOMAS ROELL AND DAVID DAWES DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THOMAS ROELL OR DAVID DAWES BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "inputstr.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "x386Procs.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"


#ifndef MOUSE_PROTOCOL_IN_KERNEL
/*
 * List of mouse types supported by xf86MouseProtocol()
 *
 * For systems which do the mouse protocol translation in the kernel,
 * this list should be defined in the appropriate *_io.c file under
 * x386/os-support.
 */
Bool xf86SupportedMouseTypes[] =
{
	TRUE,	/* Microsoft */
	TRUE,	/* MouseSystems */
	TRUE,	/* MMSeries */
	TRUE,	/* Logitech */
	TRUE,	/* BusMouse */
	TRUE,	/* MouseMan */
	TRUE,	/* PS/2 */
	TRUE,	/* Hitachi Tablet */
};

int xf86NumMouseTypes = sizeof(xf86SupportedMouseTypes) /
			sizeof(xf86SupportedMouseTypes[0]);

/*
 * termio[s] c_cflag settings for each mouse type.
 *
 * For systems which do the mouse protocol translation in the kernel,
 * this list should be defined in the appropriate *_io.c under
 * x386/os-support if it is required.
 */

unsigned short xf86MouseCflags[] =
{
	(CS7                   | CREAD | CLOCAL | HUPCL ),   /* MicroSoft */
	(CS8 | CSTOPB          | CREAD | CLOCAL | HUPCL ),   /* MouseSystems */
	(CS8 | PARENB | PARODD | CREAD | CLOCAL | HUPCL ),   /* MMSeries */
	(CS8 | CSTOPB          | CREAD | CLOCAL | HUPCL ),   /* Logitech */
	0,						     /* BusMouse */
	(CS7                   | CREAD | CLOCAL | HUPCL ),   /* MouseMan,
                                                              [CHRIS-211092] */
	0,						     /* PS/2 */
	(CS8                   | CREAD | CLOCAL | HUPCL ),   /* mmhitablet */
};
#endif /* ! MOUSE_PROTOCOL_IN_KERNEL */


/*
 * xf86MouseSupported --
 *	Returns true if OS supports mousetype
 */

Bool
xf86MouseSupported(mousetype)
     int mousetype;
{
    if (mousetype < 0 || mousetype >= xf86NumMouseTypes)
    {
	return(FALSE);
    }
    return(xf86SupportedMouseTypes[mousetype]);
}

/*
 * xf86SetupMouse --
 *	Sets up the mouse parameters
 */

void
xf86SetupMouse()
{
#if !defined(MOUSE_PROTOCOL_IN_KERNEL) || defined(MACH386)
      /*
      ** The following lines take care of the Logitech MouseMan protocols.
      **
      ** NOTE: There are different versions of both MouseMan and TrackMan!
      **       Hence I add another protocol P_LOGIMAN, which the user can
      **       specify as MouseMan in his Xconfig file. This entry was
      **       formerly handled as a special case of P_MS. However, people
      **       who don't have the middle button problem, can still specify
      **       Microsoft and use P_MS.
      **
      ** By default, these mice should use a 3 byte Microsoft protocol
      ** plus a 4th byte for the middle button. However, the mouse might
      ** have switched to a different protocol before we use it, so I send
      ** the proper sequence just in case.
      **
      ** NOTE: - all commands to (at least the European) MouseMan have to
      **         be sent at 1200 Baud.
      **       - each command starts with a '*'.
      **       - whenever the MouseMan receives a '*', it will switch back
      **	 to 1200 Baud. Hence I have to select the desired protocol
      **	 first, then select the baud rate.
      **
      ** The protocols supported by the (European) MouseMan are:
      **   -  5 byte packed binary protocol, as with the Mouse Systems
      **      mouse. Selected by sequence "*U".
      **   -  2 button 3 byte MicroSoft compatible protocol. Selected
      **      by sequence "*V".
      **   -  3 button 3+1 byte MicroSoft compatible protocol (default).
      **      Selected by sequence "*X".
      **
      ** The following baud rates are supported:
      **   -  1200 Baud (default). Selected by sequence "*n".
      **   -  9600 Baud. Selected by sequence "*q".
      **
      ** Selecting a sample rate is no longer supported with the MouseMan!
      ** Some additional lines in x386Config.c take care of ill configured
      ** baud rates and sample rates. (The user will get an error.)
      **               [CHRIS-211092]
      */

  
      if (x386Info.mseType == P_LOGIMAN)
        {
          xf86SetMouseSpeed(1200, 1200, xf86MouseCflags[P_LOGIMAN]);
          write(x386Info.mseFd, "*X", 2);
          xf86SetMouseSpeed(1200, x386Info.baudRate,
			    xf86MouseCflags[P_LOGIMAN]);
        }
      else if (x386Info.mseType != P_BM && x386Info.mseType != P_PS2) 
	{
	  xf86SetMouseSpeed(9600, x386Info.baudRate,
			    xf86MouseCflags[x386Info.mseType]);
	  xf86SetMouseSpeed(4800, x386Info.baudRate, 
			    xf86MouseCflags[x386Info.mseType]);
	  xf86SetMouseSpeed(2400, x386Info.baudRate,
			    xf86MouseCflags[x386Info.mseType]);
	  xf86SetMouseSpeed(1200, x386Info.baudRate,
			    xf86MouseCflags[x386Info.mseType]);

	  if (x386Info.mseType == P_LOGI)
	    {
	      write(x386Info.mseFd, "S", 1);
	      xf86SetMouseSpeed(x386Info.baudRate, x386Info.baudRate,
                                xf86MouseCflags[P_MM]);
	    }

	  if (x386Info.mseType == P_MMHIT)
	  {
	    char speedcmd;

	    /*
	     * Initialize Hitachi PUMA Plus - Model 1212E to desired settings.
	     * The tablet must be configured to be in MM mode, NO parity,
	     * Binary Format.  x386Info.sampleRate controls the sensativity
	     * of the tablet.  We only use this tablet for it's 4-button puck
	     * so we don't run in "Absolute Mode"
	     */
	    write(x386Info.mseFd, "z8", 2);	/* Set Parity = "NONE" */
	    usleep(50000);
	    write(x386Info.mseFd, "zb", 2);	/* Set Format = "Binary" */
	    usleep(50000);
	    write(x386Info.mseFd, "@", 1);	/* Set Report Mode = "Stream" */
	    usleep(50000);
	    write(x386Info.mseFd, "R", 1);	/* Set Output Rate = "45 rps" */
	    usleep(50000);
	    write(x386Info.mseFd, "I\x20", 2);	/* Set Incrememtal Mode "20" */
	    usleep(50000);
	    write(x386Info.mseFd, "E", 1);	/* Set Data Type = "Relative */
	    usleep(50000);

	    /* These sample rates translate to 'lines per inch' on the Hitachi
	       tablet */
	    if      (x386Info.sampleRate <=   40) speedcmd = 'g';
	    else if (x386Info.sampleRate <=  100) speedcmd = 'd';
	    else if (x386Info.sampleRate <=  200) speedcmd = 'e';
	    else if (x386Info.sampleRate <=  500) speedcmd = 'h';
	    else if (x386Info.sampleRate <= 1000) speedcmd = 'j';
	    else                                  speedcmd = 'd';
	    write(x386Info.mseFd, &speedcmd, 1);
	    usleep(50000);

	    write(x386Info.mseFd, "\021", 1);	/* Resume DATA output */
	  }
	  else
	  {
	    if      (x386Info.sampleRate <=   0)  write(x386Info.mseFd, "O", 1);
	    else if (x386Info.sampleRate <=  15)  write(x386Info.mseFd, "J", 1);
	    else if (x386Info.sampleRate <=  27)  write(x386Info.mseFd, "K", 1);
	    else if (x386Info.sampleRate <=  42)  write(x386Info.mseFd, "L", 1);
	    else if (x386Info.sampleRate <=  60)  write(x386Info.mseFd, "R", 1);
	    else if (x386Info.sampleRate <=  85)  write(x386Info.mseFd, "M", 1);
	    else if (x386Info.sampleRate <= 125)  write(x386Info.mseFd, "Q", 1);
	    else                                  write(x386Info.mseFd, "N", 1);
	  }
        }

#ifdef CLEARDTR_SUPPORT
      if (x386Info.mseType == P_MSC && (x386Info.mouseFlags & MF_CLEAR_DTR))
        {
          int val = TIOCM_DTR;
          ioctl(x386Info.mseFd, TIOCMBIC, &val);
        }
      if (x386Info.mseType == P_MSC && (x386Info.mouseFlags & MF_CLEAR_RTS))
        {
          int val = TIOCM_RTS;
          ioctl(x386Info.mseFd, TIOCMBIC, &val);
        }
#endif
#endif /* !MOUSE_PROTOCOL_IN_KERNEL || MACH386 */
}
 
#ifndef MOUSE_PROTOCOL_IN_KERNEL
void
xf86MouseProtocol(rBuf, nBytes)
     unsigned char *rBuf;
     int nBytes;
{
  int                  i, buttons, dx, dy;
  static int           pBufP = 0;
  static unsigned char pBuf[8];

  static unsigned char proto[8][5] = {
    /*  hd_mask hd_id   dp_mask dp_id   nobytes */
    { 	0x40,	0x40,	0x40,	0x00,	3 	},  /* MicroSoft */
    {	0xf8,	0x80,	0x00,	0x00,	5	},  /* MouseSystems */
    {	0xe0,	0x80,	0x80,	0x00,	3	},  /* MMSeries */
    {	0xe0,	0x80,	0x80,	0x00,	3	},  /* Logitech */
    {	0xf8,	0x80,	0x00,	0x00,	5	},  /* BusMouse */
    { 	0x40,	0x40,	0x40,	0x00,	3 	},  /* MouseMan
                                                       [CHRIS-211092] */
    {	0xc0,	0x00,	0x00,	0x00,	3	},  /* PS/2 mouse */
    {	0xe0,	0x80,	0x80,	0x00,	3	},  /* MM_HitTablet */
  };
  
  for ( i=0; i < nBytes; i++) {
    /*
     * Hack for resyncing: We check here for a package that is:
     *  a) illegal (detected by wrong data-package header)
     *  b) invalid (0x80 == -128 and that might be wrong for MouseSystems)
     *  c) bad header-package
     *
     * NOTE: b) is a voilation of the MouseSystems-Protocol, since values of
     *       -128 are allowed, but since they are very seldom we can easily
     *       use them as package-header with no button pressed.
     * NOTE/2: On a PS/2 mouse any byte is valid as a data byte. Furthermore,
     *         0x80 is not valid as a header byte. For a PS/2 mouse we skip
     *         checking data bytes.
     *         For resyncing a PS/2 mouse we require the two most significant
     *         bits in the header byte to be 0. These are the overflow bits,
     *         and in case of an overflow we actually lose sync. Overflows
     *         are very rare, however, and we quickly gain sync again after
     *         an overflow condition. This is the best we can do. (Actually,
     *         we could use bit 0x08 in the header byte for resyncing, since
     *         that bit is supposed to be always on, but nobody told
     *         Microsoft...)
     */
    if (pBufP != 0 && x386Info.mseType != P_PS2 &&
	((rBuf[i] & proto[x386Info.mseType][2]) != proto[x386Info.mseType][3]
	 || rBuf[i] == 0x80))
      {
	pBufP = 0;          /* skip package */
      }

    if (pBufP == 0 &&
	(rBuf[i] & proto[x386Info.mseType][0]) != proto[x386Info.mseType][1])
      {
	/*
	 * Hack for Logitech MouseMan Mouse - Middle button
	 *
	 * Unfortunately this mouse has variable length packets: the standard
	 * Microsoft 3 byte packet plus an optional 4th byte whenever the
	 * middle button status changes.
	 *
	 * We have already processed the standard packet with the movement
	 * and button info.  Now post an event message with the old status
	 * of the left and right buttons and the updated middle button.
	 */

        /*
	 * Even worse, different MouseMen and TrackMen differ in the 4th
         * byte: some will send 0x00/0x20, others 0x01/0x21, or even
         * 0x02/0x22, so I have to strip off the lower bits. [CHRIS-211092]
	 */
	if ((x386Info.mseType == P_MS || x386Info.mseType == P_LOGIMAN)
          && (char)(rBuf[i] & ~0x23) == 0)
	  {
	    buttons = ((int)(rBuf[i] & 0x20) >> 4)
	      | (x386Info.lastButtons & 0x05);
	    x386PostMseEvent(buttons, 0, 0);
	  }

	continue;            /* skip package */
      }


    pBuf[pBufP++] = rBuf[i];
    if (pBufP != proto[x386Info.mseType][4]) continue;

    /*
     * assembly full package
     */
    switch(x386Info.mseType) {
      
    case P_LOGIMAN:	    /* MouseMan / TrackMan   [CHRIS-211092] */
    case P_MS:              /* Microsoft */
      if (x386Info.chordMiddle)
	buttons = (((int) pBuf[0] & 0x30) == 0x30) ? 2 :
		  ((int)(pBuf[0] & 0x20) >> 3)
		  | ((int)(pBuf[0] & 0x10) >> 4);
      else
	buttons = (x386Info.lastButtons & 2)
		  | ((int)(pBuf[0] & 0x20) >> 3)
		  | ((int)(pBuf[0] & 0x10) >> 4);
      dx = (char)(((pBuf[0] & 0x03) << 6) | (pBuf[1] & 0x3F));
      dy = (char)(((pBuf[0] & 0x0C) << 4) | (pBuf[2] & 0x3F));
      break;
      
    case P_MSC:             /* Mouse Systems Corp */
      buttons = (~pBuf[0]) & 0x07;
      dx =    (char)(pBuf[1]) + (char)(pBuf[3]);
      dy = - ((char)(pBuf[2]) + (char)(pBuf[4]));
      break;
      
    case P_MMHIT:           /* MM_HitTablet */
      buttons = pBuf[0] & 0x07;
      if (buttons != 0)
        buttons = 1 << (buttons - 1);
      dx = (pBuf[0] & 0x10) ?   pBuf[1] : - pBuf[1];
      dy = (pBuf[0] & 0x08) ? - pBuf[2] :   pBuf[2];
      break;

    case P_MM:              /* MM Series */
    case P_LOGI:            /* Logitech Mice */
      buttons = pBuf[0] & 0x07;
      dx = (pBuf[0] & 0x10) ?   pBuf[1] : - pBuf[1];
      dy = (pBuf[0] & 0x08) ? - pBuf[2] :   pBuf[2];
      break;
      
    case P_BM:              /* BusMouse */
      buttons = (~pBuf[0]) & 0x07;
      dx =   (char)pBuf[1];
      dy = - (char)pBuf[2];
      break;

    case P_PS2:		    /* PS/2 mouse */
      buttons = (pBuf[0] & 0x04) >> 1 |       /* Middle */
	        (pBuf[0] & 0x02) >> 1 |       /* Right */
		(pBuf[0] & 0x01) << 2;        /* Left */
      dx = (pBuf[0] & 0x10) ?    pBuf[1]-256  :  pBuf[1];
      dy = (pBuf[0] & 0x20) ?  -(pBuf[2]-256) : -pBuf[2];
      break;
    }

    x386PostMseEvent(buttons, dx, dy);
    pBufP = 0;
  }
}
#endif /* MOUSE_PROTOCOL_IN_KERNEL */
