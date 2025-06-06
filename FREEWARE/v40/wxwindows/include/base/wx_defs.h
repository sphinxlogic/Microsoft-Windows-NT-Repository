/*
 * File:	wx_defs.h
 * Purpose:	Declarations/definitions common to all wx source files
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   19 Feb 97    9:29 pm
 */

#ifndef wxb_defsh
#define wxb_defsh

#include <stddef.h>
#include <string.h>
#include "wx_setup.h"
#include "wx_ver.h"

// Helps SGI compilation, apparently
#if defined(__sgi) && defined(__GNUG__)
#define __need_wchar_t
#endif

// Eliminate double/float warnings
#ifdef _MSC_VER
# pragma warning(disable:4244)
#endif

//////////////////////////////////////////////////////////////////////////////////
// Currently Only MS-Windows/NT, XView and Motif are supported
//
#if defined(wx_hp) && !defined(wx_motif) && !defined(wx_xview)
# define wx_motif
#endif
#if defined(wx_xview) || defined(wx_motif)
# define wx_x
#elif defined(__WINDOWS__) || defined(__WINDOWS_386__) || defined(__NT__) || defined(__MSDOS__) 
# ifndef wx_msw
#  define wx_msw
# endif
#endif
// Make sure the environment is set correctly
#if defined(wx_msw) && defined(wx_x)
# error "Target can't be both X and Windows"
#elif defined(wx_xview) && defined(wx_motif)
# error "Target can't be both XView and Motif!"
#elif !defined(wx_xview) && !defined(wx_motif) && !defined(wx_msw)
#error "No Target! Use -D[wx_motif|wx_xview|wx_msw]"
#endif

#ifdef wx_motif
  typedef int Bool;

#ifndef TRUE
# define TRUE  1
# define FALSE 0
# define Bool_DEFINED
#endif

#elif defined(wx_xview)

# define Bool int
# define True  1
# define False 0

#ifndef TRUE
# define TRUE  1
# define FALSE 0
#endif

# define Bool_DEFINED
#elif defined(wx_msw)
# include <windows.h>
# ifndef Bool
   typedef int Bool;
#  define Bool_DEFINED
# endif
#endif

#ifndef TRUE
# define TRUE  1
# define FALSE 0
#endif

typedef short int WXTYPE;

// Macro to cut down on compiler warnings.
#if REMOVE_UNUSED_ARG
#define WXUNUSED(identifier) /* identifier */
#else  // stupid, broken compiler
#define WXUNUSED(identifier) identifier
#endif

// Macro for const compatibility
#if CONST_COMPATIBILITY == 0
#  define Const const
#  define Constdata const
#elif CONST_COMPATIBILITY == 1
#  define Const
#  define Constdata const
#elif CONST_COMPATIBILITY == 2
#  define Constdata
#  define Const
#endif

// Compatibility with old type system
#if USE_OLD_TYPE_SYSTEM
#define WXSET_TYPE(newType, oldType) __type = oldType;
#else
#define WXSET_TYPE(newType, oldType)
#endif

/*
 * Making or using wxWindows as a Windows DLL
 */

#ifdef wx_msw
#  ifdef WXMAKINGDLL
#    define WXDLLEXPORT __declspec( dllexport )
#  elif defined(WXUSINGDLL)
#    define WXDLLEXPORT __declspec( dllimport )
#  else
#    define WXDLLEXPORT
#  endif
#else
#  define WXDLLEXPORT
#endif

/*
 * Window style flags.
 * Values are chosen so they can be |'ed in a bit list.
 * Some styles are used across more than one group,
 * so the values mustn't clash with others in the group.
 * Otherwise, numbers can be reused across groups.
 *
 * From version 1.66:
 * Window (cross-group) styles now take up the first half
 * of the flag, and control-specific styles the
 * second half.
 * 
 */

/*
 * Window (Frame/dialog/subwindow/panel item) style flags
 */
#define wxVSCROLL           0x80000000
#define wxHSCROLL           0x40000000
#define wxCAPTION           0x20000000
#define wxREADONLY          0x10000000
#define wxEDITABLE          0x00000000
#define wxVERTICAL_LABEL    0x08000000
#define wxHORIZONTAL_LABEL  0x04000000

#define wxBORDER            0x02000000

#define wxNATIVE_IMPL       0x01000000
                                 // Use native implementation, 
                                 // e.g. Text EDIT control for wxTextWindow under MSW
#define wxEXTENDED_IMPL     0x00000000
                                 // Extended (or simply alternative) implementation, e.g. large
                                 // but not editable wxTextWindow under Windows
#define wxUSER_COLOURS      0x00800000
                                 // Override CTL3D etc. control colour processing to
                                 // allow own background colour
#define wxOVERRIDE_KEY_TRANSLATIONS 0x00400000
#define wxFLAT              0x00200000

// Orientations
#define wxHORIZONTAL     0x01
#define wxVERTICAL       0x02
#define wxBOTH           (wxVERTICAL|wxHORIZONTAL)
#define wxCENTER_FRAME   0x04  /* centering into frame rather than screen */

/*
 * Frame/dialog style flags
 */
#define wxSTAY_ON_TOP       0x8000
#define wxICONIZE           0x4000
#define wxMINIMIZE          wxICONIZE
#define wxMAXIMIZE          0x2000
#define wxTHICK_FRAME       0x1000
#define wxSYSTEM_MENU       0x0800
#define wxMINIMIZE_BOX      0x0400
#define wxMAXIMIZE_BOX      0x0200
#define wxTINY_CAPTION_HORIZ 0x0100
#define wxTINY_CAPTION_VERT 0x0080
#define wxRESIZE_BOX        wxMAXIMIZE_BOX
#define wxRESIZE_BORDER	    0x0040
#define wxPUSH_PIN          0x0020

#define wxMDI_PARENT        0x0010
#define wxMDI_CHILD         0x0008
#define wxSDI               0x0000

#define wxDEFAULT_FRAME    (wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION)
#define wxDEFAULT_FRAME_STYLE wxDEFAULT_FRAME
#define wxDEFAULT_DIALOG_STYLE	(wxSYSTEM_MENU|wxCAPTION|wxTHICK_FRAME)

/*
 * Subwindow style flags
 */
#define wxRETAINED          0x0001
#define wxBACKINGSTORE      wxRETAINED
// wxCanvas or wxPanel can optionally have a thick frame under MS Windows.
// #define wxTHICK_FRAME       0x1000

/*
 * wxToolBar style flags
 */
 
#define wxTB_3DBUTTONS      0x8000

/*
 * Apply to all panel items
 */
 
#define wxCOLOURED          0x0800
// Alignment for panel item labels: replaces characters with zeros
// when creating label, so spaces can be included in string for alignment.
#define wxFIXED_LENGTH      0x0400
#define wxALIGN_LEFT        0x0000
#define wxALIGN_CENTER      0x0100
#define wxALIGN_CENTRE      0x0100
#define wxALIGN_RIGHT       0x0200

/*
 * Styles for wxListBox
 */
// In Multiple argument
#define wxMULTIPLE_MASK     0x0003
#define wxSINGLE            0x0000
#define wxMULTIPLE          0x0001
#define wxEXTENDED          0x0002

// In wxListBox style flag
#define wxSB_MASK           0x0008
#define wxNEEDED_SB         0x0000
#define wxALWAYS_SB         0x0008

// New naming convention
#define wxLB_NEEDED_SB      wxNEEDED_SB
#define wxLB_ALWAYS_SB      wxALWAYS_SB
#define wxLB_SORT           0x0010
// These duplicate the styles in the Multiple argument
#define wxLB_SINGLE         0x0000
#define wxLB_MULTIPLE       0x0040
#define wxLB_EXTENDED       0x0080

/*
 * wxText style flags
 */
#define wxPROCESS_ENTER     0x0004
#define wxPASSWORD          0x0008
#define wxTE_PROCESS_ENTER  wxPROCESS_ENTER
#define wxTE_PASSWORD       wxPASSWORD
#define wxTE_READONLY       wxREADONLY

/*
 * wxComboBox style flags
 */
#define wxCB_SIMPLE         0x0004
#define wxCB_DROPDOWN       0x0000
#define wxCB_SORT           0x0008
#define wxCB_READONLY       wxREADONLY

/*
 * wxRadioBox/wxRadioButton style flags
 */
#define wxRB_GROUP          0x0004

/*
 * wxGauge flags
 */
#define wxGA_PROGRESSBAR     0x0004
#define wxGA_HORIZONTAL      wxHORIZONTAL
#define wxGA_VERTICAL        wxVERTICAL

/*
 * wxSlider flags
 */
 
// Motif only so far
#define wxSL_NOTIFY_DRAG     0x0004

/*
 * GDI descriptions
 */

enum {
// Text font families
  wxDEFAULT    = 70,
  wxDECORATIVE,
  wxROMAN,
  wxSCRIPT,
  wxSWISS,
  wxMODERN,
  wxTELETYPE,  /* @@@@ */

// Proportional or Fixed width fonts (not yet used)
  wxVARIABLE   = 80,
  wxFIXED,

  wxNORMAL     = 90,
  wxLIGHT,
  wxBOLD,
// Also wxNORMAL for normal (non-italic text)
  wxITALIC,
  wxSLANT,

// Pen styles
  wxSOLID      =   100,
  wxDOT,
  wxLONG_DASH,
  wxSHORT_DASH,
  wxDOT_DASH,
  wxUSER_DASH,

  wxTRANSPARENT,

// Brush & Pen Stippling. Note that a stippled pen cannot be dashed!!
// Note also that stippling a Pen IS meaningfull, because a Line is
// drawn with a Pen, and without any Brush -- and it can be stippled.
  wxSTIPPLE =          110,
  wxBDIAGONAL_HATCH,
  wxCROSSDIAG_HATCH,
  wxFDIAGONAL_HATCH,
  wxCROSS_HATCH,
  wxHORIZONTAL_HATCH,
  wxVERTICAL_HATCH,
#define IS_HATCH(s)	((s)>=wxBDIAGONAL_HATCH && (s)<=wxVERTICAL_HATCH)

  wxJOIN_BEVEL =     120,
  wxJOIN_MITER,
  wxJOIN_ROUND,

  wxCAP_ROUND =      130,
  wxCAP_PROJECTING,
  wxCAP_BUTT
};


// Logical ops
typedef enum {
  wxCLEAR,      // 0
  wxXOR,        // src XOR dst
  wxINVERT,     // NOT dst
  wxOR_REVERSE, // src OR (NOT dst)
  wxAND_REVERSE,// src AND (NOT dst)
  wxCOPY,       // src
  wxAND,        // src AND dst
  wxAND_INVERT, // (NOT src) AND dst
  wxNO_OP,      // dst
  wxNOR,        // (NOT src) AND (NOT dst)
  wxEQUIV,      // (NOT src) XOR dst
  wxSRC_INVERT, // (NOT src)
  wxOR_INVERT,  // (NOT src) OR dst
  wxNAND,       // (NOT src) OR (NOT dst)
  wxOR,         // src OR dst
  wxSET,        // 1
  wxSRC_OR,     // source _bitmap_ OR destination
  wxSRC_AND     // source _bitmap_ AND destination
} form_ops_t;

// Flood styles
#define  wxFLOOD_SURFACE   1
#define  wxFLOOD_BORDER    2

// Polygon filling mode
#define  wxODDEVEN_RULE    1
#define  wxWINDING_RULE    2

// ToolPanel in wxFrame
#define	wxTOOL_TOP	   1
#define	wxTOOL_BOTTOM	   2
#define	wxTOOL_LEFT	   3
#define	wxTOOL_RIGHT	   4

// Dialog specifiers/return values
// Unfortunately const's cause too many 'defined but not used'
// in GCC.
// messages. So we're returning to defines for now.
/*
const wxOK =                0x0001;
const wxYES_NO =            0x0002;
const wxCANCEL =            0x0004;
const wxYES =               0x0008;
const wxNO =                0x0010;

const wxICON_EXCLAMATION =  0x0020;
const wxICON_HAND =         0x0040;
const wxICON_QUESTION =     0x0080;
const wxICON_INFORMATION =  0x0100;
*/

#define wxOK                0x0001
#define wxYES_NO            0x0002
#define wxCANCEL            0x0004
#define wxYES               0x0008
#define wxNO                0x0010

#define wxICON_EXCLAMATION  0x0020
#define wxICON_HAND         0x0040
#define wxICON_QUESTION     0x0080
#define wxICON_INFORMATION  0x0100

#define wxICON_STOP         wxICON_HAND
#define wxICON_ASTERISK     wxICON_INFORMATION
#define wxICON_MASK         (0x0020|0x0040|0x0080|0x0100)

#define wxCENTRE            0x0200
#define wxCENTER wxCENTRE

// Possible SetSize flags

// Use internally-calculated width if -1
#define wxSIZE_AUTO_WIDTH       1
// Use internally-calculated height if -1
#define wxSIZE_AUTO_HEIGHT      2
// Use internally-calculated width and height if each is -1
#define wxSIZE_AUTO             3
// Ignore missing (-1) dimensions (use existing).
// For readability only: test for wxSIZE_AUTO_WIDTH/HEIGHT in code.
#define wxSIZE_USE_EXISTING     0
// Allow -1 as a valid position
#define wxSIZE_ALLOW_MINUS_ONE  4

// Clipboard formats
#ifdef wx_msw
# define wxCF_TEXT               CF_TEXT
# define wxCF_BITMAP             CF_BITMAP
# define wxCF_METAFILE           CF_METAFILEPICT
# define wxCF_DIB                CF_DIB
# define wxCF_OEMTEXT            CF_OEMTEXT
#else
# define wxCF_TEXT               1
# define wxCF_BITMAP             2
# define wxCF_METAFILE           3
# define wxCF_DIB                4
# define wxCF_OEMTEXT            5
#endif

// Virtual keycodes
enum _Virtual_keycodes {
 WXK_BACK    =   8,
 WXK_TAB     =   9,
 WXK_RETURN  =	13,
 WXK_ESCAPE  =	27,
 WXK_SPACE   =	32,
 WXK_DELETE  = 127,

 WXK_START   = 300,
 WXK_LBUTTON,
 WXK_RBUTTON,
 WXK_CANCEL,
 WXK_MBUTTON,
 WXK_CLEAR,
 WXK_SHIFT,
 WXK_CONTROL,
 WXK_MENU,
 WXK_PAUSE,
 WXK_CAPITAL,
 WXK_PRIOR,  // Page up
 WXK_NEXT,   // Page down
 WXK_END,
 WXK_HOME,
 WXK_LEFT,
 WXK_UP,
 WXK_RIGHT,
 WXK_DOWN,
 WXK_SELECT,
 WXK_PRINT,
 WXK_EXECUTE,
 WXK_SNAPSHOT,
 WXK_INSERT,
 WXK_HELP,
 WXK_NUMPAD0,
 WXK_NUMPAD1,
 WXK_NUMPAD2,
 WXK_NUMPAD3,
 WXK_NUMPAD4,
 WXK_NUMPAD5,
 WXK_NUMPAD6,
 WXK_NUMPAD7,
 WXK_NUMPAD8,
 WXK_NUMPAD9,
 WXK_MULTIPLY,
 WXK_ADD,
 WXK_SEPARATOR,
 WXK_SUBTRACT,
 WXK_DECIMAL,
 WXK_DIVIDE,
 WXK_F1,
 WXK_F2,
 WXK_F3,
 WXK_F4,
 WXK_F5,
 WXK_F6,
 WXK_F7,
 WXK_F8,
 WXK_F9,
 WXK_F10,
 WXK_F11,
 WXK_F12,
 WXK_F13,
 WXK_F14,
 WXK_F15,
 WXK_F16,
 WXK_F17,
 WXK_F18,
 WXK_F19,
 WXK_F20,
 WXK_F21,
 WXK_F22,
 WXK_F23,
 WXK_F24,
 WXK_NUMLOCK,
 WXK_SCROLL,
 WXK_PAGEUP,
 WXK_PAGEDOWN
};

// Colours - see wx_gdi.cc for database

// OS mnemonics -- Identify the running OS (useful for Windows)
// [Not all platforms are currently available or supported]
enum {
  wxCURSES,
  wxXVIEW_X,	// Sun's XView OpenLOOK toolkit
  wxMOTIF_X,	// OSF Motif 1.x.x
  wxCOSE_X,	// OSF Common Desktop Environment
  wxNEXTSTEP,	// NeXTStep
  wxMACINTOSH,	// Apple System 7
  wxGEOS,	// GEOS
  wxOS2_PM,	// OS/2 Workplace
  wxWINDOWS,	// Windows or WfW
  wxPENWINDOWS,	// Windows for Pen Computing
  wxWINDOWS_NT,	// Windows NT
  wxWIN32S,	// Windows 32S API
  wxWIN95,	// Windows 95
  wxWIN386	// Watcom 32-bit supervisor modus
};

// Standard menu identifiers
#define wxID_OPEN               5000
#define wxID_CLOSE              5001
#define wxID_NEW                5002
#define wxID_SAVE               5003
#define wxID_SAVEAS             5004
#define wxID_REVERT             5005
#define wxID_EXIT               5006
#define wxID_UNDO               5007
#define wxID_REDO               5008
#define wxID_HELP               5009
#define wxID_PRINT              5010
#define wxID_PRINT_SETUP        5011
#define wxID_PREVIEW            5012
#define wxID_ABOUT              5013
#define wxID_HELP_CONTENTS      5014
#define wxID_HELP_COMMANDS      5015
#define wxID_HELP_PROCEDURES    5016
#define wxID_HELP_CONTEXT       5017

#define wxID_CUT                5030
#define wxID_COPY               5031
#define wxID_PASTE              5032
#define wxID_CLEAR              5033
#define wxID_FIND               5034

#define wxID_FILE1              5050
#define wxID_FILE2              5051
#define wxID_FILE3              5052
#define wxID_FILE4              5053
#define wxID_FILE5              5054
#define wxID_FILE6              5055
#define wxID_FILE7              5056
#define wxID_FILE8              5057
#define wxID_FILE9              5058

#endif // wxb_defsh
