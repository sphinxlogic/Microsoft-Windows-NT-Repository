/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/os-support/sysv/xqueue.c,v 2.0 1993/07/23 08:58:15 dawes Exp $ */


#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "inputstr.h"
#include "scrnintstr.h"
#include "compiler.h"

#include "x386Procs.h"
#include "xf86_OSlib.h"

#ifdef XQUEUE

extern int GetMotionEvents(
#if NeedFunctionPrototypes
	CARD32, CARD32,
	xTimecoord *,
	ScreenPtr
#endif
);

static xqEventQueue      *XqueQaddr;

/*
 * x386XqueRequest --
 *      Notice an i/o request from the xqueue.
 */

static void
x386XqueRequest()
{
  xqEvent  *XqueEvents = XqueQaddr->xq_events;
  int      XqueHead = XqueQaddr->xq_head;

  while (XqueHead != XqueQaddr->xq_tail)
    {

      switch(XqueEvents[XqueHead].xq_type) {
	
      case XQ_BUTTON:
	x386PostMseEvent(~(XqueEvents[XqueHead].xq_code) & 0x07, 0, 0);
	break;

      case XQ_MOTION:
	x386PostMseEvent(~(XqueEvents[XqueHead].xq_code) & 0x07,
		       XqueEvents[XqueHead].xq_x,
		       XqueEvents[XqueHead].xq_y);
	break;

      case XQ_KEY:
	x386PostKbdEvent(XqueEvents[XqueHead].xq_code);
	break;
	
      default:
	ErrorF("Unknown Xque Event: 0x%02x\n", XqueEvents[XqueHead].xq_type);
      }
      
      if ((++XqueHead) == XqueQaddr->xq_size) XqueHead = 0;
    }

  /* reenable the signal-processing */
  x386Info.inputPending = TRUE;
  signal(SIGUSR2, (void (*)()) x386XqueRequest);
  XqueQaddr->xq_head = XqueQaddr->xq_tail;
  XqueQaddr->xq_sigenable = 1; /* UNLOCK */
}



/*
 * x386XqueEnable --
 *      Enable the handling of the Xque
 */

static int
x386XqueEnable()
{
  static struct kd_quemode xqueMode;
  static Bool              was_here = FALSE;

  if (!was_here) {
    if ((x386Info.xqueFd = open("/dev/mouse", O_RDONLY|O_NDELAY)) < 0)
      {
	Error ("Cannot open /dev/mouse");
	return (!Success);
      }
    was_here = TRUE;
  }

  if (x386Info.xqueSema++ == 0) 
    {
      (void) signal(SIGUSR2, (void (*)()) x386XqueRequest);
      xqueMode.qsize = 64;    /* max events */
      xqueMode.signo = SIGUSR2;
      ioctl(x386Info.consoleFd, KDQUEMODE, NULL);
      
      if (ioctl(x386Info.consoleFd, KDQUEMODE, &xqueMode) < 0) {
	Error ("Cannot set KDQUEMODE");
	/* CONSTCOND */
	return (!Success);
      }
      
      XqueQaddr = (xqEventQueue *)xqueMode.qaddr;
      XqueQaddr->xq_sigenable = 1; /* UNLOCK */
    }

  return(Success);
}



/*
 * x386XqueDisable --
 *      disable the handling of the Xque
 */

static int
x386XqueDisable()
{
  if (x386Info.xqueSema-- == 1)
    {
      
      XqueQaddr->xq_sigenable = 0; /* LOCK */
      
      if (ioctl(x386Info.consoleFd, KDQUEMODE, NULL) < 0) {
	Error ("Cannot unset KDQUEMODE");
	/* CONSTCOND */
	return (!Success);
      }
    }

  return(Success);
}



/*
 * x386XqueMseProc --
 *      Handle the initialization, etc. of a mouse
 */

int
x386XqueMseProc(pPointer, what)
     DevicePtr	pPointer;
     int        what;
{
  unchar        map[4];

  switch (what)
    {
    case DEVICE_INIT: 
      
      pPointer->on = FALSE;
      
      map[1] = 1;
      map[2] = 2;
      map[3] = 3;
      InitPointerDeviceStruct(pPointer, 
			      map, 
			      3, 
			      GetMotionEvents, 
			      x386MseCtrl, 
			      0);
      break;
      
    case DEVICE_ON:
      x386Info.lastButtons = 0;
      x386Info.emulateState = 0;
      pPointer->on = TRUE;
      return(x386XqueEnable());
      
    case DEVICE_CLOSE:
    case DEVICE_OFF:
      pPointer->on = FALSE;
      return(x386XqueDisable());
    }
  
  return Success;
}



/*
 * x386XqueKbdProc --
 *	Handle the initialization, etc. of a keyboard.
 */

int
x386XqueKbdProc (pKeyboard, what)
     DevicePtr pKeyboard;	/* Keyboard to manipulate */
     int       what;	    	/* What to do to it */
{
  KeySymsRec  keySyms;
  CARD8       modMap[MAP_LENGTH];

  switch (what) {
      
  case DEVICE_INIT:
    
    x386KbdGetMapping(&keySyms, modMap);
    
    /*
     * Get also the initial led settings
     */
    ioctl(x386Info.consoleFd, KDGETLED, &x386Info.leds);
    
    /*
     * Perform final initialization of the system private keyboard
     * structure and fill in various slots in the device record
     * itself which couldn't be filled in before.
     */
    pKeyboard->on = FALSE;
    
    InitKeyboardDeviceStruct(x386Info.pKeyboard,
			     &keySyms,
			     modMap,
			     x386KbdBell,
			     x386KbdCtrl);
    
    x386InitKBD(TRUE);
    break;
    
  case DEVICE_ON:
    pKeyboard->on = TRUE;
    x386InitKBD(FALSE);
    return(x386XqueEnable());
    
  case DEVICE_CLOSE:
  case DEVICE_OFF:
    pKeyboard->on = FALSE;
    return(x386XqueDisable());
  }
  
  return (Success);
}


/*
 * x386XqueEvents --
 *      Get some events from our queue. Nothing to do here ...
 */

void
x386XqueEvents()
{
}

#endif /* XQUEUE */
