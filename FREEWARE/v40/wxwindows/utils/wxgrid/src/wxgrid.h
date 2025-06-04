/*
 * File:	wxgrid.h
 * Purpose:	wxGrid include file
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

#ifndef wxgridh
#define wxgridh

#if defined(wx_msw) || defined(wx_motif) || defined(wx_xview)
#define USE_GENERIC_GRID 1
#else
#define USE_GENERIC_GRID 0
#endif

#if USE_GENERIC_GRID
#include "wxgridg.h"
#elif defined(wx_motif)
#include "wxgridm.h"
#error "wxGrid not implemented on this platform!"
#endif

#endif
