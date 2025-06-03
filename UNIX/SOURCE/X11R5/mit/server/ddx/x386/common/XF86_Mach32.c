/*
 * $XFree86: mit/server/ddx/x386/common/XF86_Mach32.c,v 2.3 1994/02/26 05:02:04 dawes Exp $
 */

#include "X.h"
#include "os.h"
#include "x386.h"
#include "xf86_Config.h"

extern ScrnInfoRec mach32InfoRec;

/*
 * This limit is currently set to 80MHz because this is the limit of many
 * ramdacs when running in 1:1 mode.  It will be increased when support
 * is added for using the ramdacs in 2:1 mode.  Increasing this limit
 * could result in damage to your hardware.
 */
#define MAX_MACH32_CLOCK		80000
#define MAX_MACH32_TLC34075_CLOCK	135000

int mach32MaxClock = MAX_MACH32_CLOCK;
int mach32MaxTlc34075Clock = MAX_MACH32_TLC34075_CLOCK;

ScrnInfoPtr x386Screens[] = 
{
  &mach32InfoRec,
};

int  x386MaxScreens = sizeof(x386Screens) / sizeof(ScrnInfoPtr);

int xf86ScreenNames[] =
{
  ACCEL,
  -1
};

int mach32ValidTokens[] =
{
  STATICGRAY,
  GRAYSCALE,
  STATICCOLOR,
  PSEUDOCOLOR,
  TRUECOLOR,
  DIRECTCOLOR,
  CHIPSET,
  CLOCKS,
  DISPLAYSIZE,
  MODES,
  OPTION,
  VIDEORAM,
  VIEWPORT,
  VIRTUAL,
  CLOCKPROG,
  BIOSBASE,
  MEMBASE,
  -1
};

/* Dummy function for PEX in LinkKit and mono server */

#if defined(LINKKIT) && !defined(PEXEXT)
PexExtensionInit() {}
#endif
