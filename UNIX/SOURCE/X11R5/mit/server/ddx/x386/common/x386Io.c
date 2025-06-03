/*
 * $XFree86: mit/server/ddx/x386/common/x386Io.c,v 2.9 1993/09/23 15:45:19 dawes Exp $
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
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
 * $Header: /proj/X11/mit/server/ddx/x386/RCS/x386Io.c,v 1.2 1991/06/27 00:01:38 root Exp $
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

extern KeybdCtrl defaultKeyboardControl;


#include "atKeynames.h"

/*
 * x386KbdBell --
 *	Ring the terminal/keyboard bell for an amount of time proportional to
 *      "loudness".
 */

void
x386KbdBell(loudness, pKeyboard)
     int           loudness;         /* Percentage of full volume */
     DeviceIntPtr  pKeyboard;        /* Keyboard to ring */
{
  xf86SoundKbdBell(loudness, x386Info.bell_pitch, x386Info.bell_duration);
}

#ifdef AMOEBA
#define LED_CAP	IOP_LED_CAP
#define LED_NUM	IOP_LED_NUM
#define LED_SCR	IOP_LED_SCROLL
#endif

#ifdef _MINIX
#define LED_CAP KBD_LEDS_CAPS
#define LED_NUM KBD_LEDS_NUM
#define LED_SCR KBD_LEDS_SCROLL
#endif

void
x386KbdLeds ()
{
#ifdef LED_CAP
  int leds = 0;

  if (x386Info.capsLock && !(x386Info.xleds & XLED1))
    leds |= LED_CAP;

  if (x386Info.numLock && !(x386Info.xleds & XLED2))
    leds |= LED_NUM;

  if ((x386Info.scrollLock || 
       x386Info.modeSwitchLock || 
       x386Info.composeLock) && 
      !(x386Info.xleds & XLED3))
    leds |= LED_SCR;

  if ((x386Info.leds & x386Info.xleds) & XLED1) leds |= LED_CAP;
  if ((x386Info.leds & x386Info.xleds) & XLED2) leds |= LED_NUM;
  if ((x386Info.leds & x386Info.xleds) & XLED3) leds |= LED_SCR;

  xf86SetKbdLeds(leds);
#endif /* LED_CAP */
}

/*
 * x386KbdCtrl --
 *      Alter some of the keyboard control parameters. All special protocol
 *      values are handled by dix (ProgChangeKeyboardControl)
 */

void
x386KbdCtrl (pKeyboard, ctrl)
     DevicePtr     pKeyboard;        /* Keyboard to alter */
     KeybdCtrl     *ctrl;
{
  x386Info.bell_pitch    = ctrl->bell_pitch;
  x386Info.bell_duration = ctrl->bell_duration;
  x386Info.autoRepeat    = ctrl->autoRepeat;
  x386Info.leds          = (ctrl->leds & ~(XCAPS | XNUM | XSCR));

  x386Info.composeLock   = (ctrl->leds & XCOMP) ? TRUE : FALSE;

  x386KbdLeds();
}

/*
 * x386InitKBD --
 *      Reinitialize the keyboard. Only set Lockkeys accrding to ours leds.
 *      Depress all other keys.
 */

void
x386InitKBD(init)
Bool init;
{
  char            leds,rad;
  unsigned int    i;
  xEvent          kevent;
  DevicePtr       pKeyboard = x386Info.pKeyboard;
  KeyClassRec     *keyc = ((DeviceIntPtr)x386Info.pKeyboard)->key;
  KeySym          *map = keyc->curKeySyms.map;

#ifndef MACH386
  kevent.u.keyButtonPointer.time = GetTimeInMillis();
  kevent.u.keyButtonPointer.rootX = 0;
  kevent.u.keyButtonPointer.rootY = 0;

  /*
   * Hmm... here is the biggest hack of every time !
   * It may be possible that a switch-vt procedure has finished BEFORE
   * you released all keys neccessary to do this. That peculiar behavior
   * can fool the X-server pretty much, cause it assumes that some keys
   * were not released. TWM may stuck alsmost completly....
   * OK, what we are doing here is after returning from the vt-switch
   * exeplicitely unrelease all keyboard keys before the input-devices
   * are reenabled.
   */
  for (i = keyc->curKeySyms.minKeyCode; i < keyc->curKeySyms.maxKeyCode; i++)
    if (KeyPressed(i))
      {
	kevent.u.u.detail = i;
	kevent.u.u.type = KeyRelease;
	(* pKeyboard->processInputProc)(&kevent, pKeyboard, 1);
      }
#endif /* MACH386 */
  
  x386Info.scanPrefix      = 0;

  if (init)
    {
      /*
       * we must deal here with the fact, that on some cases the numlock or
       * capslock key are enabled BEFORE the server is started up. So look
       * here at the state on the according LEDS to determine whether a
       * lock-key is allready set.
       */

      x386Info.capsLock        = FALSE;
      x386Info.numLock         = FALSE;
      x386Info.scrollLock      = FALSE;
      x386Info.modeSwitchLock  = FALSE;
      x386Info.composeLock     = FALSE;
    
#ifdef LED_CAP
      leds = x386Info.leds;

      for (i = keyc->curKeySyms.minKeyCode;
           i < keyc->curKeySyms.maxKeyCode;
           i++, map += keyc->curKeySyms.mapWidth)

        switch(*map) {

        case XK_Caps_Lock:
        case XK_Shift_Lock:
          if (leds & LED_CAP) 
	    {
	      kevent.u.u.detail = i;
	      kevent.u.u.type = KeyPress;
	      (* pKeyboard->processInputProc)(&kevent, pKeyboard, 1);
	      x386Info.capsLock = TRUE;
	    }
          break;

        case XK_Num_Lock:
          if (leds & LED_NUM)
	    {
	      kevent.u.u.detail = i;
	      kevent.u.u.type = KeyPress;
	      (* pKeyboard->processInputProc)(&kevent, pKeyboard, 1);
	      x386Info.numLock = TRUE;
	    }
          break;

        case XK_Scroll_Lock:
        case XK_Kana_Lock:
          if (leds & LED_SCR)
	    {
	      kevent.u.u.detail = i;
	      kevent.u.u.type = KeyPress;
	      (* pKeyboard->processInputProc)(&kevent, pKeyboard, 1);
	      x386Info.scrollLock = TRUE;
	    }
          break;
        }
      x386KbdLeds();
#endif /* LED_CAP */

      if      (x386Info.kbdDelay <= 375) rad = 0x00;
      else if (x386Info.kbdDelay <= 625) rad = 0x20;
      else if (x386Info.kbdDelay <= 875) rad = 0x40;
      else                               rad = 0x60;
    
      if      (x386Info.kbdRate <=  2)   rad |= 0x1F;
      else if (x386Info.kbdRate >= 30)   rad |= 0x00;
      else                               rad |= ((58 / x386Info.kbdRate) - 2);
    
      xf86SetKbdRepeat(rad);
    }
}

/*
 * x386KbdProc --
 *	Handle the initialization, etc. of a keyboard.
 */

int
x386KbdProc (pKeyboard, what)
     DevicePtr pKeyboard;	/* Keyboard to manipulate */
     int       what;	    	/* What to do to it */
{
  KeySymsRec           keySyms;
  CARD8                modMap[MAP_LENGTH];
  int                  kbdFd;

  switch (what) {

  case DEVICE_INIT:
    /*
     * First open and find the current state of the keyboard.
     */

    xf86KbdInit();

    x386KbdGetMapping(&keySyms, modMap);
    
    defaultKeyboardControl.leds = xf86GetKbdLeds();

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
    /*
     * Set the keyboard into "direct" mode and turn on
     * event translation.
     */

    kbdFd = xf86KbdOn();

    if (kbdFd != -1)
      AddEnabledDevice(kbdFd);

    pKeyboard->on = TRUE;
    x386InitKBD(FALSE);
    break;
    
  case DEVICE_CLOSE:
  case DEVICE_OFF:
    /*
     * Restore original keyboard directness and translation.
     */

    kbdFd = xf86KbdOff();

    if (kbdFd != -1)
      RemoveEnabledDevice(kbdFd);

    pKeyboard->on = FALSE;
    break;

  }
  return (Success);
}

/*
 * x386KbdEvents --
 *      Read the new events from the device, and pass them to the eventhandler.
 */

void
x386KbdEvents()
{
	xf86KbdEvents();
}

/*
 * x386MseCtrl --
 *      Alter the control parameters for the mouse. Note that all special
 *      protocol values are handled by dix.
 */

void
x386MseCtrl(pPointer, ctrl)
     DevicePtr pPointer;
     PtrCtrl   *ctrl;
{
  x386Info.num       = ctrl->num;
  x386Info.den       = ctrl->den;
  x386Info.threshold = ctrl->threshold;
}

/*
 * GetMotionEvents --
 *      Return the (number of) motion events in the "motion history
 *      buffer" (snicker) between the given times.
 */

int
GetMotionEvents (buff, start, stop, pScreen)
     CARD32 start, stop;
     xTimecoord *buff;
     ScreenPtr pScreen;
{
  return 0;
}

/*
 * x386MseProc --
 *      Handle the initialization, etc. of a mouse
 */

int
x386MseProc(pPointer, what)
     DevicePtr	pPointer;
     int        what;
{
  unsigned char                map[5];
  int                          nbuttons;
  int                          mousefd;

  switch (what)
    {
    case DEVICE_INIT: 
      pPointer->on = FALSE;
 
      map[1] = 1;
      map[2] = 2;
      map[3] = 3;
      map[4] = 4;

      if (x386Info.mseType == P_MMHIT)
        nbuttons = 4;
      else
        nbuttons = 3;

      InitPointerDeviceStruct(pPointer, 
			      map, 
			      nbuttons, 
			      GetMotionEvents, 
			      x386MseCtrl, 
			      0);

      xf86MouseInit();

      break;
      
    case DEVICE_ON:

      mousefd = xf86MouseOn();

      if (mousefd != -1)
      {
        if (x386Info.mseType == P_PS2)
          write(mousefd, "\364", 1);

        AddEnabledDevice(mousefd);
      }

      x386Info.lastButtons = 0;
      x386Info.emulateState = 0;
      pPointer->on = TRUE;

      break;
      
    case DEVICE_OFF:
    case DEVICE_CLOSE:

      mousefd = xf86MouseOff(what == DEVICE_CLOSE);

      if (mousefd != -1)
        RemoveEnabledDevice(x386Info.mseFd);

      pPointer->on = FALSE;
      usleep(300000);
      break;
    }
  return Success;
}

/*
 * x386MseEvents --
 *      Read the new events from the device, and pass them to the eventhandler.
 */

void
x386MseEvents()
{
  xf86MouseEvents();
}
