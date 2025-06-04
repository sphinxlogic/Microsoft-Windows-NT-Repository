
/*
 * File:	wb_mf.cc
 * Purpose:	Metafiles and metafile DCs
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wb_mf.cc,v 1.3 1994/08/14 21:34:01 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "@(#)wb_mf.cc	1.2 5/9/94"; */

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_defs.h"
#endif

#if USE_METAFILE
#include "wx_mf.h"
#include "wx_types.h"

/*
 * Metafiles - Windows 3.1 only
 * Currently, the only purpose for making a metafile is to put
 * it on the clipboard.
 */

wxbMetaFile::wxbMetaFile(void)
{
  WXSET_TYPE(wxMetaFile, wxTYPE_METAFILE)
}

wxbMetaFile::~wxbMetaFile(void)
{
}

Bool wxbMetaFile::SetClipboard(int WXUNUSED(width), int WXUNUSED(height))
{
  return FALSE;
}

/*
 * Metafile device context
 *
 */

wxbMetaFileDC::wxbMetaFileDC(char *WXUNUSED(file))
{
  WXSET_TYPE(wxMetaFileDC, wxTYPE_DC_METAFILE)
}

wxbMetaFileDC::~wxbMetaFileDC(void)
{
}

wxMetaFile *wxbMetaFileDC::Close(void)
{
  return NULL;
}

void wxbMetaFileDC::SetMapMode(int WXUNUSED(mode))
{
}

#endif // USE_METAFILE
