//========================================================================
//
// PBMOutputDev.h
//
// Copyright 1998 Derek B. Noonburg
//
//========================================================================

#ifndef PBMOUTPUTDEV_H
#define PBMOUTPUTDEV_H

#ifdef __GNUC__
#pragma interface
#endif

#include <stddef.h>
#include "config.h"
#include "XOutputDev.h"

//------------------------------------------------------------------------

class PBMOutputDev: public XOutputDev {
public:

  // NB: Users must use makePBMOutputDev and killPBMOutputDev rather
  // than the constructor and destructor.  (This is due to some
  // constraints in the underlying XOutputDev object.)

  static PBMOutputDev *makePBMOutputDev(char *displayName,
					char *fileRoot1);

  static void killPBMOutputDev(PBMOutputDev *out);

  virtual ~PBMOutputDev();

  //----- initialization and control

  // Start a page.
  virtual void startPage(int pageNum, GfxState *state);

  // End a page.
  virtual void endPage();

private:

  PBMOutputDev(Display *display1, int screen1,
	       Pixmap pixmap1, Window dummyWin1,
	       int invert1, char *fileRoot1);

  char *fileRoot;
  char *fileName;
  int curPage;

  Display *display;
  int screen;
  Pixmap pixmap;
  Window dummyWin;
  int width, height;
  int invert;
};

#endif
