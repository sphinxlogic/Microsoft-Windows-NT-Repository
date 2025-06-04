/*
 * File:	main.cc
 * Purpose:	Main stub for calling wxWindows entry point
 *              +++ NOTE +++ this is only an example - WinMain is actually
 *              defined in wx_main.cpp.
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	March 1995
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// May wish not to have a DllMain or WinMain, e.g. if we're programming
// a Netscape plugin.
#ifndef NOMAIN

// NT defines APIENTRY, 3.x not
#if !defined(APIENTRY)
#define APIENTRY FAR PASCAL
#endif

/////////////////////////////////////////////////////////////////////////////////
// WinMain
// Note that WinMain is also defined in dummy.obj, which is linked to
// an application that is using the DLL version of wxWindows.

#if !defined(_DLL)

#ifdef __WATCOMC__
int PASCAL
#else
int APIENTRY
#endif

 WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR m_lpCmdLine, int nCmdShow )
{
  return wxEntry(hInstance, hPrevInstance, m_lpCmdLine, nCmdShow);
}
#endif

/////////////////////////////////////////////////////////////////////////////////
// DllMain

#if defined(_DLL)

// DLL entry point

extern "C"
BOOL WINAPI DllMain (HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
        // Only call wxEntry if the application itself is part of the DLL.
        // If only the wxWindows library is in the DLL, then the initialisation
        // will be called when the application implicitly calls WinMain.

#if !defined(WXMAKINGDLL)
        return wxEntry(hModule);
#endif
	    break;

	case DLL_PROCESS_DETACH:
	default:
	    break;
	}
  return TRUE;
}

#endif

#endif

