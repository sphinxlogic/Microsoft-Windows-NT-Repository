/*
 * File:     wx_prec.h
 * Purpose:  Include the appropriate files for precompilation.
 */
 
// check if to use precompiled headers
#if (defined(__BORLANDC__) || defined(_MSC_VER) || defined(__WATCOMC__) || defined(GNUWIN32)) && !defined(WXMAKINGDLL)
#define WX_PRECOMP
#endif

// For some reason, this must be defined for common dialogs to work.
#ifdef __WATCOMC__
#define INCLUDE_COMMDLG_H	1
#endif

// include the wx definitions
#ifdef WX_PRECOMP
#include "wx.h"
#endif

