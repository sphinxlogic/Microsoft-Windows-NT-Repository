/*
 * File:	wx_mf.h
 * Purpose:	Metafiles
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_mf.h	1.2 5/9/94" */


#ifndef wx_mfh
#define wx_mfh

#include "wx_setup.h"

#if USE_METAFILE
#include "wb_mf.h"

/*
 * Metafile and metafile device context classes - work in Windows 3.1 only
 *
 */

#ifdef IN_CPROTO
typedef       void    *wxMetaFile ;
typedef       void    *wxMetaFileDC ;
#else

class WXDLLEXPORT wxMetaFile: public wxbMetaFile
{
  DECLARE_DYNAMIC_CLASS(wxMetaFile)
 public:
  HMETAFILE metafile;
  int windowsMappingMode;

  wxMetaFile(char *file = NULL);
  ~wxMetaFile(void);

  // After this is called, the metafile cannot be used for anything
  // since it is now owned by the clipboard.
  Bool SetClipboard(int width = 0, int height = 0);

  Bool Play(wxDC *dc);
  inline Bool Ok(void) { return metafile != 0; };

};

class WXDLLEXPORT wxMetaFileDC: public wxbMetaFileDC
{
  DECLARE_DYNAMIC_CLASS(wxMetaFileDC)

 public:
  int windowsMappingMode;
  wxMetaFile *metafile;

  // Don't supply origin and extent
  // Supply them to wxMakeMetaFilePlaceable instead.
  wxMetaFileDC(char *file = NULL);

  // Supply origin and extent (recommended).
  // Then don't need to supply them to wxMakeMetaFilePlaceable.
  wxMetaFileDC(char *file, int xext, int yext, int xorg, int yorg);

  ~wxMetaFileDC(void);

  // Should be called at end of drawing
  wxMetaFile *Close(void);
  void SetMapMode(int mode);
  void GetTextExtent(const char *string, float *x, float *y,
                     float *descent = NULL, float *externalLeading = NULL,
                     wxFont *theFont = NULL, Bool use16bit = FALSE);
};

/*
 * Pass filename of existing non-placeable metafile, and bounding box.
 * Adds a placeable metafile header, sets the mapping mode to anisotropic,
 * and sets the window origin and extent to mimic the MM_TEXT mapping mode.
 *
 */

// No origin or extent
WXDLLEXPORT Bool wxMakeMetaFilePlaceable(char *filename, float scale = 1.0);

// Optional origin and extent
WXDLLEXPORT Bool wxMakeMetaFilePlaceable(char *filename, int x1, int y1, int x2, int y2, float scale = 1.0, Bool useOriginAndExtent = TRUE);

#endif // IN_CPROTO
#endif // USE_METAFILE
#endif // wx_mfh
