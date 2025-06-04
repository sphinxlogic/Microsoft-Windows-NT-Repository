/*
**
** config.h
**
** Copyright (C) 1995, 1996, 1997 Johannes Plass
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
** 
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
*/


#ifndef _GV_CONFIG_H_
#define _GV_CONFIG_H_

/*
----------------------------------------------------------
Enabling the following option causes ghostscript to be 
restarted whenever a new page should be displayed but
gv-vms thinks that the interpreter is busy. This may solve
some problems that may occur for some incorrectly
formatted files, but will slow down previewing for many
correct files also. (yes Jim, this option is for you)
----------------------------------------------------------
*/

/* #define GV_RESTART_IF_BUSY */

/*
##########################################################
##########################################################
  Private Declarations
##########################################################
##########################################################
*/

#ifdef VMS
#   define INTERN_RESOURCES_DAT     "gv_intern_res_vms.dat"
#   define INTERN_RESOURCES_H       "gv_intern_res_vms.h"
#else
#   define INTERN_RESOURCES_DAT     "gv_intern_res_unix.dat"
#   define INTERN_RESOURCES_H       "gv_intern_res_unix.h"
#endif

/*
----------------------------------------------------------
Inline copies of optional external files.
----------------------------------------------------------
*/

#ifdef USE_FALLBACK_STYLES
#   define FALLBACK_STYLE_1_DAT         "gv_spartan.dat"
#   define FALLBACK_STYLE_1_H           "gv_spartan.h"
#endif

#define FALLBACK_ICON_PIXMAP         "gv_icon.xbm"
#define FALLBACK_SELECTED_BITMAP     "gv_selected.xbm"
#define FALLBACK_DOCUMENT_BITMAP     "gv_doc.xbm"
#define FALLBACK_MARK_BITMAPS        yes_please
#define FALLBACK_MARK_ODD_BITMAP     "gv_odd.xbm"
#define FALLBACK_MARK_EVEN_BITMAP    "gv_even.xbm"
#define FALLBACK_MARK_CURRENT_BITMAP "gv_current.xbm"
#define FALLBACK_MARK_UNMARK_BITMAP  "gv_unmark.xbm"
#define FALLBACK_MARK_EMPTY_BITMAP   "gv_empty.xbm"
#define FALLBACK_ICON_NAME           GV
#define FALLBACK_SELECTED_NAME       DOT
#define FALLBACK_DOCUMENT_NAME       DOC
#define FALLBACK_MARK_ODD_NAME       odd
#define FALLBACK_MARK_EVEN_NAME      even
#define FALLBACK_MARK_CURRENT_NAME   current
#define FALLBACK_MARK_UNMARK_NAME    unmark
#define FALLBACK_MARK_EMPTY_NAME     empty

/*
----------------------------------------------------------
Miscellaneous defaults
----------------------------------------------------------
*/
#define GV_ERROR_PRINT_FAIL	"Printing via\n '%s'\nfailed."
#define GV_PRINT_MESSAGE	"Print Command:"
#define GV_PRINT_MARKED_MESSAGE	"Print Marked Pages"
#define GV_PRINT_PAGE_MESSAGE	"Print Current Page"
#define GV_PRINT_ALL_MESSAGE	"Print Document"
#define GV_PRINT_BUTTON_LABEL	"Print"
#define GV_ERROR_OPEN_FAIL	"Cannot open file"
#define GV_OPEN_MESSAGE		"Open"
#define GV_ERROR_SAVE_FAIL	"Cannot write to file"
#define GV_SAVE_MESSAGE		"Save"
#define GV_AUTO_RESIZE_YES	"Variable Size"
#define GV_AUTO_RESIZE_NO	"Fixed Size"
#define GV_MINIMUM_SIZE		300
#define MAX_LOCATOR_LENGTH	48
#define TOC3D_INITIAL_HEIGHT	30
#define TOC3D_INITIAL_WIDTH	10

#define	GV_MAX_FILENAME_LENGTH	256
#define GV_APPLICATION_NAME	"gv"
#define GV_CLASS		"GV"

#ifdef VMS
#   define EXIT_STATUS_NORMAL	1
#   define EXIT_STATUS_ERROR	2
#   define EXIT_STATUS_FATAL    4
#else
#   define EXIT_STATUS_NORMAL	0
#   define EXIT_STATUS_ERROR	1
#   define EXIT_STATUS_FATAL    -1
#endif

#endif /* _GV_CONFIG_H_ */





