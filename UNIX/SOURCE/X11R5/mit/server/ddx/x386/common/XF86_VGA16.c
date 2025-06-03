/*
 * $XFree86: mit/server/ddx/x386/common/XF86_VGA16.c,v 2.2 1994/02/01 13:52:34 dawes Exp $
 */

#include "X.h"
#include "os.h"
#include "x386.h"
#include "xf86_Config.h"

extern ScrnInfoRec vga16InfoRec;

#ifdef BUILD_VGA16
#define SCREEN0 &vga16InfoRec
#else
#define SCREEN0 NULL
#endif

ScrnInfoPtr x386Screens[] = 
{
  SCREEN0,
};

int  x386MaxScreens = sizeof(x386Screens) / sizeof(ScrnInfoPtr);

int xf86ScreenNames[] =
{
  VGA16,
  -1
};

#ifdef BUILD_VGA16
int vga16ValidTokens[] =
{
  PSEUDOCOLOR,
  STATICGRAY,
  GRAYSCALE,
  CHIPSET,
  CLOCKS,
  DISPLAYSIZE,
  MODES,
  SCREENNO,
  OPTION,
  VIDEORAM,
  VIEWPORT,
  VIRTUAL,
  CLOCKPROG,
  BIOSBASE,
  -1
};
#endif /* BUILDVGA16 */

/* Dummy function for PEX in LinkKit and non-8-bit server */

PexExtensionInit() {}
