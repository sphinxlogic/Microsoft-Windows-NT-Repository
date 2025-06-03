/*
 * $XFree86: mit/server/ddx/x386/common/x386Init.c,v 2.7 1993/08/28 07:52:41 dawes Exp $
 * $XConsortium: x386Init.c,v 1.2 91/08/20 15:39:58 gildea Exp $
 *
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
 */

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "servermd.h"
#include "scrnintstr.h"
#include "site.h"

#include "compiler.h"

#include "x386Procs.h"
#include "xf86_OSlib.h"
#include "x386Version.h"

#ifdef XTESTEXT1
#include "atKeynames.h"
extern int xtest_command_key;
#endif /* XTESTEXT1 */

/* x386Exiting is set while the screen is shutting down (even on a reset) */
Bool x386Exiting = FALSE;
Bool x386Resetting = FALSE;
Bool x386ProbeFailed = TRUE;
Bool x386Verbose = TRUE;
Bool x386fpFlag = FALSE;
Bool x386coFlag = FALSE;
char x386ConfigFile[PATH_MAX] = "";

static void x386PrintConfig();

extern ScrnInfoPtr x386Screens[];
extern int x386MaxScreens;

x386InfoRec x386Info;
int         x386ScreenIndex;
static Bool ProbeOnly = FALSE;

/*
 * InitOutput --
 *	Initialize screenInfo for all actually accessible framebuffers.
 *      That includes vt-manager setup, querying all possible devices and
 *      collecting the pixmap formats.
 */

void
InitOutput(pScreenInfo, argc, argv)
     ScreenInfo	*pScreenInfo;
     int     	argc;
     char    	**argv;
{
  int                    i, j, scr_index;
  static int             numFormats = 0;
  static PixmapFormatRec formats[MAXFORMATS];
  static unsigned long   generation = 0;
  int                    any_screens = 0;
   

  if (serverGeneration == 1) {

    x386PrintConfig();

    xf86OpenConsole();

#if !defined(AMOEBA) && !defined(_MINIX)
    /*
     * If VTInit was set, run that program with consoleFd as stdin and stdout
     */

    if (x386Info.vtinit) {
      switch(fork()) {
        case -1:
          FatalError("Fork failed for VTInit (%s)\n", strerror(errno));
          break;
        case 0:  /* child */
          setuid(getuid());
          /* set stdin, stdout to the consoleFd */
          for (i = 0; i < 2; i++) {
            if (x386Info.consoleFd != i) {
              close(i);
              dup(x386Info.consoleFd);
            }
          }
          execl("/bin/sh", "sh", "-c", x386Info.vtinit, NULL);
          ErrorF("Warning: exec of /bin/sh failed for VTInit (%s)\n",
                 strerror(errno));
          exit(255);
          break;
        default:  /* parent */
          wait(NULL);
      }
    }
#endif /* !AMOEBA && !_MINIX */

    /* Do this after Xconfig is read (it's normally in OsInit()) */
    OsInitColors();

    x386Config(TRUE); /* Probe displays, and resolve modes */

    /*
     * collect all possible formats
     */
    formats[0].depth = 1;
    formats[0].bitsPerPixel = 1;
    formats[0].scanlinePad = BITMAP_SCANLINE_PAD;
    numFormats++;
  
    for ( i=0;
          i < x386MaxScreens && x386Screens[i] && x386Screens[i]->configured;
          i++ )
      { 
	/*
	 * At least one probe function succeeded.
	 */
	any_screens = 1;

	/*
	 * add new pixmap format
	 */
	for ( j=0; j < numFormats; j++ ) {
	  
	  if (formats[j].depth == x386Screens[i]->depth &&
	      formats[j].bitsPerPixel == x386Screens[i]->bitsPerPixel)
	    break; /* found */
        }
	  
        if (j == numFormats) {   /* not already there */
	  formats[j].depth = x386Screens[i]->depth;
	  formats[j].bitsPerPixel = x386Screens[i]->bitsPerPixel;
	  formats[j].scanlinePad = BITMAP_SCANLINE_PAD;
	  numFormats++;
	  if ( numFormats > MAXFORMATS )
	    FatalError( "Too many pixmap formats! Exiting\n" );
        }
      }
    if (!any_screens)
      if (x386ProbeFailed)
        ErrorF("\n *** None of the configured devices was detected.***\n\n");
      else
        ErrorF(
         "\n *** A configured device found, but display modes could not be resolved.***\n\n");
    if (ProbeOnly)
    {
      extern void AbortDDX();
      x386VTSema = FALSE;
      AbortDDX();
      fflush(stderr);
      exit(0);
    }
  }
  else {
    /*
     * serverGeneration != 1; some OSs have to do things here, too.
     */
    xf86OpenConsole();
  }

  /*
   * Install signal handler for unexpected signals
   */
  if (!x386Info.notrapSignals)
  {
     x386Info.caughtSignal=FALSE;
     signal(SIGSEGV,x386SigHandler);
     signal(SIGILL,x386SigHandler);
#ifdef SIGEMT
     signal(SIGEMT,x386SigHandler);
#endif
     signal(SIGFPE,x386SigHandler);
#ifdef SIGBUS
     signal(SIGBUS,x386SigHandler);
#endif
#ifdef SIGSYS
     signal(SIGSYS,x386SigHandler);
#endif
#ifdef SIGXCPU
     signal(SIGXCPU,x386SigHandler);
#endif
#ifdef SIGBUS
     signal(SIGBUS,x386SigHandler);
#endif
#ifdef SIGXFSZ
     signal(SIGXFSZ,x386SigHandler);
#endif
  }


  /*
   * Use the previous collected parts to setup pScreenInfo
   */
  pScreenInfo->imageByteOrder = IMAGE_BYTE_ORDER;
  pScreenInfo->bitmapScanlineUnit = BITMAP_SCANLINE_UNIT;
  pScreenInfo->bitmapScanlinePad = BITMAP_SCANLINE_PAD;
  pScreenInfo->bitmapBitOrder = BITMAP_BIT_ORDER;
  pScreenInfo->numPixmapFormats = numFormats;
  for ( i=0; i < numFormats; i++ ) pScreenInfo->formats[i] = formats[i];

  if (generation != serverGeneration)
    {
      x386ScreenIndex = AllocateScreenPrivateIndex();
      generation = serverGeneration;
    }


  for ( i=0;
        i < x386MaxScreens && x386Screens[i] && x386Screens[i]->configured;
        i++ )
    {    
      /*
       * On a server-reset, we have explicitely to remap all stuff ...
       * (At startuptime this is implicitely done by probing the device
       */
      if (serverGeneration != 1)
        {
          x386Resetting = TRUE;
          x386Exiting = FALSE;
#ifdef HAS_USL_VTS
          if (!x386VTSema)
            ioctl(x386Info.consoleFd,VT_RELDISP,VT_ACKACQ);
#endif
          x386VTSema = TRUE;
          (x386Screens[i]->EnterLeaveVT)(ENTER, i);
          x386Resetting = FALSE;
        }
#ifdef SCO
        else {
          /*
           * Under SCO we must ack that we got the console at startup,
           * I think this is the safest way to assure it
           */
          static int once = 1;
          if (once) {
            once = 0;
            if (ioctl(x386Info.consoleFd, VT_RELDISP, VT_ACKACQ) < 0)
              ErrorF("VT_ACKACQ failed");
          }
        }
#endif /* SCO */
      scr_index = AddScreen(x386Screens[i]->Init, argc, argv);
      if (scr_index > -1)
      {
	screenInfo.screens[scr_index]->devPrivates[x386ScreenIndex].ptr
	  = (pointer)x386Screens[i];
      }

      /*
       * Here we have to let the driver getting access of the VT. Note that
       * this doesn't mean that the graphics board may access automatically
       * the monitor. If the monitor is shared this is done in x386CrossScreen!
       */
      if (!x386Info.sharedMonitor) (x386Screens[i]->EnterLeaveMonitor)(ENTER);
    }

#ifndef AMOEBA
  RegisterBlockAndWakeupHandlers(x386Block, x386Wakeup, (void *)0);
#endif
}


/*
 * InitInput --
 *      Initialize all supported input devices...what else is there
 *      besides pointer and keyboard? Two DeviceRec's are allocated and
 *      registered as the system pointer and keyboard devices.
 */

void
InitInput(argc, argv)
     int     	  argc;
     char    	  **argv;
{
  x386Info.vtRequestsPending = FALSE;
  x386Info.inputPending = FALSE;
#ifdef XTESTEXT1
  xtest_command_key = KEY_Begin + MIN_KEYCODE;
#endif /* XTESTEXT1 */

  x386Info.pKeyboard = AddInputDevice(x386Info.kbdProc, TRUE); 
  x386Info.pPointer =  AddInputDevice(x386Info.mseProc, TRUE);
  RegisterKeyboardDevice(x386Info.pKeyboard); 
  RegisterPointerDevice(x386Info.pPointer); 

  miRegisterPointerDevice(screenInfo.screens[0], x386Info.pPointer);
  mieqInit (x386Info.pKeyboard, x386Info.pPointer);
}



/*
 * ddxGiveUp --
 *      Device dependent cleanup. Called by by dix before normal server death.
 *      For SYSV386 we must switch the terminal back to normal mode. No error-
 *      checking here, since there should be restored as much as possible.
 */

void
ddxGiveUp()
{
  xf86CloseConsole();

  /*
   * Remove lock on this server
   */
  Unlock_Server();

  /* If an unexpected signal was caught, dump a core for debugging */
  if (x386Info.caughtSignal)
    abort();
}



/*
 * AbortDDX --
 *      DDX - specific abort routine.  Called by AbortServer(). The attempt is
 *      made to restore all original setting of the displays. Also all devices
 *      are closed.
 */

void
AbortDDX()
{
  int i;

  if (x386Exiting)
    return;

  x386Exiting = TRUE;

  /*
   * try to deinitialize all input devices
   */
  if (x386Info.pPointer) (x386Info.mseProc)(x386Info.pPointer, DEVICE_CLOSE);
  if (x386Info.pKeyboard) (x386Info.kbdProc)(x386Info.pKeyboard, DEVICE_CLOSE);

  /*
   * try to restore the original video state
   */
#ifdef HAS_USL_VTS
  /* Need the sleep when starting X from within another X session */
  sleep(1);
#endif
  if (x386VTSema)
    for ( i=0;
          i < x386MaxScreens && x386Screens[i] && x386Screens[i]->configured;
          i++ )
      (x386Screens[i]->EnterLeaveVT)(LEAVE, i);

  /*
   * This is needed for a abnormal server exit, since the normal exit stuff
   * MUST also be performed (i.e. the vt must be left in a defined state)
   */
  ddxGiveUp();
}



/*
 * ddxProcessArgument --
 *	Process device-dependent command line args. Returns 0 if argument is
 *      not device dependent, otherwise Count of number of elements of argv
 *      that are part of a device dependent commandline option.
 */

/* ARGSUSED */
int
ddxProcessArgument (argc, argv, i)
     int argc;
     char *argv[];
     int i;
{
  if (!strcmp(argv[i], "-xconfig"))
  {
    if (!argv[i+1])
      return 0;
    if (strlen(argv[i+1]) >= PATH_MAX)
      FatalError("Xconfig path name too long\n");
    strcpy(x386ConfigFile, argv[i+1]);
    return 2;
  }
  if (!strcmp(argv[i],"-probeonly"))
  {
    ProbeOnly = TRUE;
    x386Verbose = TRUE;
    return 1;
  }
  if (!strcmp(argv[i],"-verbose"))
  {
    x386Verbose = TRUE;
    return 1;
  }
  if (!strcmp(argv[i],"-quiet"))
  {
    x386Verbose = FALSE;
    return 1;
  }
  if (!strcmp(argv[i],"-showconfig"))
  {
    x386PrintConfig();
    exit(0);
  }
  /* Notice the -fp flag, but allow it to pass to the dix layer */
  if (!strcmp(argv[i], "-fp"))
  {
    x386fpFlag = TRUE;
    return 0;
  }
  /* Notice the -co flag, but allow it to pass to the dix layer */
  if (!strcmp(argv[i], "-co"))
  {
    x386coFlag = TRUE;
    return 0;
  }
  return xf86ProcessArgument(argc, argv, i);
}


/*
 * ddxUseMsg --
 *	Print out correct use of device dependent commandline options.
 *      Maybe the user now knows what really to do ...
 */

void
ddxUseMsg()
{
  ErrorF("\n");
  ErrorF("\n");
  ErrorF("Device Dependent Usage\n");
  ErrorF("-xconfig file          specify a configuration file\n");
  ErrorF("-probeonly             probe for devices, then exit\n");
  ErrorF("-verbose               verbose startup messages\n");
  ErrorF("-quiet                 minimal startup messages\n");
  ErrorF(
   "-showconfig            show which drivers are included in the server\n");
  xf86UseMsg();
  ErrorF("\n");
}


#ifndef OSNAME
#define OSNAME "unknown"
#endif
#ifndef OSVENDOR
#define OSVENDOR ""
#endif

static void
x386PrintConfig()
{
  int i;

  ErrorF("XFree86 Version%s/ X Window System\n",X386_VERSION);
  ErrorF("(protocol Version %d, revision %d, vendor release %d)\n",
         X_PROTOCOL, X_PROTOCOL_REVISION, VENDOR_RELEASE );
  ErrorF("Operating System: %s %s\n", OSNAME, OSVENDOR);
  ErrorF("Configured drivers:\n");
  for (i = 0; i < x386MaxScreens; i++)
    if (x386Screens[i])
      (x386Screens[i]->PrintIdent)();
}
