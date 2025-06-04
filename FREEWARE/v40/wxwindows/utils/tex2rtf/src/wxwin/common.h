/*
 * File:     common.h
 * Purpose:  Declarations/definitions common to all wx source files
 *
 *                       wxWindows 1.50
 * Copyright (c) 1993 Artificial Intelligence Applications Institute,
 *                   The University of Edinburgh
 *
 *                     Author: Julian Smart
 *                       Date: 7-9-93
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice, author statement and this permission
 * notice appear in all copies of this software and related documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS,
 * IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL THE ARTIFICIAL INTELLIGENCE APPLICATIONS INSTITUTE OR THE
 * UNIVERSITY OF EDINBURGH BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF
 * DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef wxb_commonh
#define wxb_commonh

#include <stddef.h>
#include <string.h>

#ifdef wx_xview
#define wx_x
#endif

#ifdef wx_motif
#define wx_x
#endif

#ifdef wx_x
typedef int Bool;
#define TRUE 1
#define FALSE 0
#endif

#if (defined(wx_msw) || defined(wx_dos))
#ifndef Bool
typedef int Bool;
#endif
#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif
#endif

typedef short int WXTYPE;

// Styles for wxListBox - Yes, all in Multiple , and nothing in Style
#define wxMULTIPLE_MASK    0x03
#define wxSINGLE           0x00
#define wxMULTIPLE         0x01
#define wxEXTENDED         0x02

#define wxSB_MASK          0x08
#define wxNEEDED_SB        0x00
#define wxALWAYS_SB        0x08

// Frame/dialog/subwindow style flags
#define wxVSCROLL          1
#define wxHSCROLL          2
#define wxCAPTION          4
#define wxABSOLUTE_POSITIONING 8 // Hint to Windowing system not to try anything clever

// Frame/dialog style flags
#define wxSTAY_ON_TOP      8
#define wxICONIZE          16
#define wxMINIMIZE         wxICONIZE
#define wxMAXIMIZE         32
#define wxSDI              64
#define wxMDI_PARENT       128
#define wxMDI_CHILD        256
#define wxTHICK_FRAME      512
#define wxSYSTEM_MENU      1024
#define wxMINIMIZE_BOX     2048
#define wxMAXIMIZE_BOX     4096
#define wxDEFAULT_FRAME    (wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION)

// Subwindow style flags
#define wxBORDER           64
#define wxRETAINED         128

//This style is intended for wxCanvas only -- Then this value could probably
// have another (lower) value ??
// [I choose 16384 because it seems to be never used]
// Please note that this is NOT the same thing as wxRETAINED, and this
// use backing store capability (if any) of the X server.
// Backing store is fully transparent to the application.
#ifndef wx_motif
#define       wxBACKINGSTORE     0
#else
#define       wxBACKINGSTORE     16384
#endif

//This style is intended for wxDialogBox only -- Then this value could probably
// have another (lower) value ??
// [I choose 32768 because it seems to be never used]
// This flag is used by XView, to create FRAME or FRAME_CMD dialog box.
// no effect on other architectures.
#ifndef wx_xview
#define       wxXVIEW_CMD     0
#else
#define       wxXVIEW_CMD     32768
#endif

// This style is intended for wxDialogBox only -- Then this value could probably
// have another (lower) value ??
// [I choose 65536 because it seems to be never used]
// This flag is used by Motif, for shell resize policy
// no effect on other architectures.
// Default policy is controlled by MOTIF_MANAGE, which can be defined at
// compile time.
#ifndef wx_motif
#define       wxMOTIF_RESIZE     0
#else
#define       wxMOTIF_RESIZE     65536
#endif

#ifdef MOTIF_MANAGE
#define DEFAULT_DIALOG_STYLE	wxMOTIF_RESIZE
#else
#define DEFAULT_DIALOG_STYLE	0
#endif


#ifndef wx_motif
#define       wxFLAT          wxBORDER
#else
#define       wxFLAT          256
#endif

// Text font families
#define wxDEFAULT          70
#define wxDECORATIVE       71
#define wxROMAN            72
#define wxSCRIPT           73
#define wxSWISS            74
#define wxMODERN           75

#define wxVARIABLE         80
#define wxFIXED            81

// Text font weight
#define wxNORMAL           90
#define wxLIGHT            91
#define wxBOLD             92

// Text font style
// Also wxNORMAL for normal (non-italic text)
#define wxITALIC           93
#define wxSLANT            94

// Pen styles
#define wxSOLID            100
#define wxDOT              101
#define wxLONG_DASH        102
#define wxSHORT_DASH       103
#define wxDOT_DASH         104
#define	wxUSER_DASH        105

#define wxTRANSPARENT      106

#define wxJOIN_BEVEL       120
#define wxJOIN_MITER       121
#define wxJOIN_ROUND       122

#define wxCAP_ROUND        130
#define wxCAP_PROJECTING   131
#define wxCAP_BUTT         132

// Brush & Pen Stippling. Note that a stippled pen cannot be dashed!!
// Note also that stippling a Pen IS meaningfull, because a Line is
// drawed with a Pen, and without any Brush -- and she can be stippled.

// Note also that there is a big diff between X stipple and Windows:
// On X, stipple is a mask between the wxBitmap and current colour,
// On Windows, current colour is ignored, and bitmap colour is used.
// However, for pre-defined modes like wxCROSS_HATCH,... behavior is the
// same on both platforms.

#define wxSTIPPLE          110
#define	wxBDIAGONAL_HATCH  111
#define wxCROSSDIAG_HATCH  112
#define wxFDIAGONAL_HATCH  113
#define wxCROSS_HATCH      114
#define wxHORIZONTAL_HATCH 115
#define wxVERTICAL_HATCH   116

#define IS_HATCH(s)	((s)>=wxBDIAGONAL_HATCH && (s)<=wxVERTICAL_HATCH)

// Logical ops
#define wxCLEAR            0      // 0
#define wxXOR              1      // src XOR dst
#define wxINVERT           2      // NOT dst
#define wxOR_REVERSE       3      // src OR (NOT dst)
#define wxAND_REVERSE      4      // src AND (NOT dst)
#define wxCOPY             5      // src
#define wxAND              6      // src AND dst
#define wxAND_INVERT       7      // (NOT src) AND dst
#define wxNO_OP            8      // dst
#define wxNOR              9      // (NOT src) AND (NOT dst)
#define wxEQUIV            10     // (NOT src) XOR dst
#define wxSRC_INVERT       11     // (NOT src)
#define wxOR_INVERT        12     // (NOT src) OR dst
#define wxNAND             13     // (NOT src) OR (NOT dst)
#define wxOR               14     // src OR dst
#define wxSET              15     // 1

// Directions
#define wxHORIZONTAL      1
#define wxVERTICAL        2
#define wxBOTH            3

// Dialog specifiers/return values
#define wxOK                     1
#define wxYES_NO                 2
#define wxCANCEL                 4
#define wxYES                    8
#define wxNO                    16

#define wxICON_EXCLAMATION      32
#define wxICON_HAND             64
#define wxICON_QUESTION        128

// Clipboard formats
#ifdef wx_msw
#define wxCF_TEXT               CF_TEXT
#define wxCF_BITMAP             CF_BITMAP
#define wxCF_METAFILE           CF_METAFILEPICT
#define wxCF_DIB                CF_DIB
#define wxCF_OEMTEXT            CF_OEMTEXT
#else
#define wxCF_TEXT               1
#define wxCF_BITMAP             2
#define wxCF_METAFILE           3
#define wxCF_DIB                4
#define wxCF_OEMTEXT            5
#endif

// Virtual keycodes
#define WXK_START           300

#define WXK_LBUTTON	    (WXK_START + 1)
#define WXK_RBUTTON	    (WXK_START + 2)
#define WXK_CANCEL	    (WXK_START + 3)
#define WXK_MBUTTON          (WXK_START + 4)
#define WXK_BACK 	    8
#define WXK_TAB		    9
#define WXK_CLEAR	    (WXK_START + 5)
#define WXK_RETURN	    13
#define WXK_SHIFT	    (WXK_START + 6)
#define WXK_CONTROL	    (WXK_START + 7)
#define WXK_MENU 	    (WXK_START + 8)
#define WXK_PAUSE	    (WXK_START + 9)
#define WXK_CAPITAL	    (WXK_START + 10)
#define WXK_ESCAPE	    27
#define WXK_SPACE	    32
#define WXK_PRIOR	    (WXK_START + 11)
#define WXK_NEXT 	    (WXK_START + 12)
#define WXK_END		    (WXK_START + 13)
#define WXK_HOME 	    (WXK_START + 14)
#define WXK_LEFT 	    (WXK_START + 15)
#define WXK_UP		    (WXK_START + 16)
#define WXK_RIGHT	    (WXK_START + 17)
#define WXK_DOWN 	    (WXK_START + 18)
#define WXK_SELECT	    (WXK_START + 20)
#define WXK_PRINT	    (WXK_START + 21)
#define WXK_EXECUTE	    (WXK_START + 22)
#define WXK_SNAPSHOT	    (WXK_START + 23)
#define WXK_INSERT	    (WXK_START + 24)
#define WXK_DELETE	    127
#define WXK_HELP 	    (WXK_START + 25)
#define WXK_NUMPAD0	    (WXK_START + 26)
#define WXK_NUMPAD1	    (WXK_START + 27)
#define WXK_NUMPAD2	    (WXK_START + 28)
#define WXK_NUMPAD3	    (WXK_START + 29)
#define WXK_NUMPAD4	    (WXK_START + 30)
#define WXK_NUMPAD5	    (WXK_START + 31)
#define WXK_NUMPAD6	    (WXK_START + 32)
#define WXK_NUMPAD7	    (WXK_START + 33)
#define WXK_NUMPAD8	    (WXK_START + 34)
#define WXK_NUMPAD9	    (WXK_START + 35)
#define WXK_MULTIPLY	    (WXK_START + 36)
#define WXK_ADD		    (WXK_START + 37)
#define WXK_SEPARATOR	    (WXK_START + 38)
#define WXK_SUBTRACT	    (WXK_START + 39)
#define WXK_DECIMAL	    (WXK_START + 40)
#define WXK_DIVIDE	    (WXK_START + 41)
#define WXK_F1		    (WXK_START + 42)
#define WXK_F2		    (WXK_START + 43)
#define WXK_F3		    (WXK_START + 44)
#define WXK_F4		    (WXK_START + 45)
#define WXK_F5		    (WXK_START + 46)
#define WXK_F6		    (WXK_START + 47)
#define WXK_F7		    (WXK_START + 48)
#define WXK_F8		    (WXK_START + 49)
#define WXK_F9		    (WXK_START + 50)
#define WXK_F10		    (WXK_START + 51)
#define WXK_F11		    (WXK_START + 52)
#define WXK_F12		    (WXK_START + 53)
#define WXK_F13		    (WXK_START + 54)
#define WXK_F14		    (WXK_START + 55)
#define WXK_F15		    (WXK_START + 56)
#define WXK_F16		    (WXK_START + 57)
#define WXK_F17		    (WXK_START + 58)
#define WXK_F18		    (WXK_START + 59)
#define WXK_F19		    (WXK_START + 60)
#define WXK_F20		    (WXK_START + 61)
#define WXK_F21		    (WXK_START + 62)
#define WXK_F22		    (WXK_START + 63)
#define WXK_F23		    (WXK_START + 64)
#define WXK_F24		    (WXK_START + 65)
#define WXK_NUMLOCK	    (WXK_START + 66)
#define WXK_SCROLL           (WXK_START + 67)

// Colours - see wx_gdi.cc for database

#define wxTYPE_ANY           0
#define wxTYPE_NODE          1
#define wxTYPE_LIST          2
#define wxTYPE_STRING_LIST   3
#define wxTYPE_HASH_TABLE    4

#endif // wxb_commonh
