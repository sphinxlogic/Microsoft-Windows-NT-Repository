/*
 *   patchlevel.h - Common patchlevel header file for xmcd, cda and libdi.
 *
 *	xmcd  - Motif(tm) CD Audio Player
 *	cda   - Command-line CD Audio Player
 *	libdi - CD Audio Player Device Interface Library
 *
 *
 *   Copyright (C) 1993-1998  Ti Kan
 *   E-mail: ti@amb.org
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#ifndef __PATCHLEVEL_H__
#define __PATCHLEVEL_H__

#ifndef LINT
static char *_patchlevel_h_ident_ = "@(#)patchlevel.h	6.27 98/10/27";
#endif

#define VERSION		"2.4"		/* The version number */
#define VERSION_EXT	""		/* Version number extension */
#define PATCHLEVEL	0		/* The patch level */
#define COPYRIGHT	"Copyright (C) 1993-1998  Ti Kan"
#define EMAIL		"E-mail: xmcd@amb.org"
#define WWWURL		"URL: http://sunsite.unc.edu/tkan/xmcd/"
#define GNU_BANNER	\
"This is free software and comes with no warranty.\n\
See the GNU General Public License for details."

#endif	/* __PATCHLEVEL_H__ */
