#ifndef lint
static char *RCSid = "$Id: wprinter.c%v 3.38.2.74 1993/02/19 01:16:40 woo Exp woo $";
#endif

/* GNUPLOT - win/wprinter.c */
/*
 * Copyright (C) 1992   Maurice Castro, Russell Lang
 *
 * Permission to use, copy, and distribute this software and its
 * documentation for any purpose with or without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and 
 * that both that copyright notice and this permission notice appear 
 * in supporting documentation.
 *
 * Permission to modify the software is granted, but not the right to
 * distribute the modified code.  Modifications are to be distributed 
 * as patches to released version.
 *  
 * This software is provided "as is" without express or implied warranty.
 * 
 *
 * AUTHORS
 * 
 *   Maurice Castro
 *   Russell Lang
 * 
 * Send your comments or suggestions to 
 *  info-gnuplot@dartmouth.edu.
 * This is a mailing list; to join it send a note to 
 *  info-gnuplot-request@dartmouth.edu.  
 * Send bug reports to
 *  bug-gnuplot@dartmouth.edu.
 */

/* Dump a file to the printer */

#define STRICT
#include <windows.h>
#include <windowsx.h>
#if WINVER >= 0x030a
#include <commdlg.h>
#endif
#ifdef __MSC__
#include <memory.h>
#else
#include <mem.h>
#endif
#include "wgnuplib.h"
#include "wresourc.h"
#include "wcommon.h"

LPPRINT prlist = NULL;

BOOL CALLBACK _export PrintSizeDlgProc(HWND hdlg, UINT wmsg, WPARAM wparam, LPARAM lparam);

BOOL CALLBACK _export
PrintSizeDlgProc(HWND hdlg, UINT wmsg, WPARAM wparam, LPARAM lparam)
{
	char buf[8];
	LPPRINT lpr;
	lpr = (LPPRINT)GetWindowLong(GetParent(hdlg), 4);

	switch (wmsg) {
		case WM_INITDIALOG:
			wsprintf(buf,"%d",lpr->pdef.x);
			SetDlgItemText(hdlg, PSIZE_DEFX, buf);
			wsprintf(buf,"%d",lpr->pdef.y);
			SetDlgItemText(hdlg, PSIZE_DEFY, buf);
			wsprintf(buf,"%d",lpr->poff.x);
			SetDlgItemText(hdlg, PSIZE_OFFX, buf);
			wsprintf(buf,"%d",lpr->poff.y);
			SetDlgItemText(hdlg, PSIZE_OFFY, buf);
			wsprintf(buf,"%d",lpr->psize.x);
			SetDlgItemText(hdlg, PSIZE_X, buf);
			wsprintf(buf,"%d",lpr->psize.y);
			SetDlgItemText(hdlg, PSIZE_Y, buf);
			CheckDlgButton(hdlg, PSIZE_DEF, TRUE);
			EnableWindow(GetDlgItem(hdlg, PSIZE_X), FALSE);
			EnableWindow(GetDlgItem(hdlg, PSIZE_Y), FALSE);
			return TRUE;
		case WM_COMMAND:
			switch (wparam) {
				case PSIZE_DEF:
					EnableWindow(GetDlgItem(hdlg, PSIZE_X), FALSE);
					EnableWindow(GetDlgItem(hdlg, PSIZE_Y), FALSE);
					return FALSE;
				case PSIZE_OTHER:
					EnableWindow(GetDlgItem(hdlg, PSIZE_X), TRUE);
					EnableWindow(GetDlgItem(hdlg, PSIZE_Y), TRUE);
					return FALSE;
				case IDOK:
					if (SendDlgItemMessage(hdlg, PSIZE_OTHER, BM_GETCHECK, 0, 0L)) {
						SendDlgItemMessage(hdlg, PSIZE_X, WM_GETTEXT, 7, (LPARAM)((LPSTR)buf));
						GetInt(buf, &lpr->psize.x);
						SendDlgItemMessage(hdlg, PSIZE_Y, WM_GETTEXT, 7, (LPARAM)((LPSTR)buf));
						GetInt(buf, &lpr->psize.y);
					}
					else {
						lpr->psize.x = lpr->pdef.x;
						lpr->psize.y = lpr->pdef.y;
					}
					SendDlgItemMessage(hdlg, PSIZE_OFFX, WM_GETTEXT, 7, (LPARAM)((LPSTR)buf));
					GetInt(buf, &lpr->poff.x);
					SendDlgItemMessage(hdlg, PSIZE_OFFY, WM_GETTEXT, 7, (LPARAM)((LPSTR)buf));
					GetInt(buf, &lpr->poff.y);

					if (lpr->psize.x <= 0)
						lpr->psize.x = lpr->pdef.x;
					if (lpr->psize.y <= 0)
						lpr->psize.y = lpr->pdef.y;

					EndDialog(hdlg, IDOK);
					return TRUE;
				case IDCANCEL:
					EndDialog(hdlg, IDCANCEL);
					return TRUE;
			}
			break;
	}
	return FALSE;
}



/* GetWindowLong(hwnd, 4) must be available for use */
BOOL
PrintSize(HDC printer, HWND hwnd, LPRECT lprect)
{
HDC hdc;
DLGPROC lpfnPrintSizeDlgProc ;
BOOL status = FALSE;
PRINT pr;

	SetWindowLong(hwnd, 4, (LONG)((LPPRINT)&pr));
	pr.poff.x = 0;
	pr.poff.y = 0;
	pr.psize.x = GetDeviceCaps(printer, HORZSIZE);
	pr.psize.y = GetDeviceCaps(printer, VERTSIZE);
	hdc = GetDC(hwnd);
	GetClientRect(hwnd,lprect);
	pr.pdef.x = MulDiv(lprect->right-lprect->left, 254, 10*GetDeviceCaps(hdc, LOGPIXELSX));
	pr.pdef.y = MulDiv(lprect->bottom-lprect->top, 254, 10*GetDeviceCaps(hdc, LOGPIXELSX));
	ReleaseDC(hwnd,hdc);
#ifdef __DLL__
	lpfnPrintSizeDlgProc = (DLGPROC)GetProcAddress(hdllInstance, "PrintSizeDlgProc");
#else
	lpfnPrintSizeDlgProc = (DLGPROC)MakeProcInstance((FARPROC)PrintSizeDlgProc, hdllInstance);
#endif
	if (DialogBox (hdllInstance, "PrintSizeDlgBox", hwnd, lpfnPrintSizeDlgProc)
		== IDOK) {
		lprect->left = MulDiv(pr.poff.x*10, GetDeviceCaps(printer, LOGPIXELSX), 254);
		lprect->top = MulDiv(pr.poff.y*10, GetDeviceCaps(printer, LOGPIXELSY), 254);
		lprect->right = lprect->left + MulDiv(pr.psize.x*10, GetDeviceCaps(printer, LOGPIXELSX), 254);
		lprect->bottom = lprect->top + MulDiv(pr.psize.y*10, GetDeviceCaps(printer, LOGPIXELSY), 254);
		status = TRUE;
	}
#ifndef __DLL__
	FreeProcInstance((FARPROC)lpfnPrintSizeDlgProc);
#endif
	SetWindowLong(hwnd, 4, (LONG)(0L));
	return status;
}


#if WINVER >= 0x030a
void 
PrintRegister(LPPRINT lpr)
{
	LPPRINT next;
	next = prlist;
	prlist = lpr;
	lpr->next = next;
}

LPPRINT
PrintFind(HDC hdc)
{
	LPPRINT this;
	this = prlist;
	while (this && (this->hdcPrn!=hdc)) {
		this = this->next;
	}
	return this;
}

void
PrintUnregister(LPPRINT lpr)
{
	LPPRINT this, prev;
	prev = (LPPRINT)NULL;
	this = prlist;
	while (this && (this!=lpr)) {
		prev = this;
		this = this->next;
	}
	if (this && (this == lpr)) {
		/* unhook it */
		if (prev)
			prev->next = this->next;
		else
			prlist = this->next;
	}
}


/* GetWindowLong(GetParent(hDlg), 4) must be available for use */
BOOL CALLBACK _export
PrintDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPPRINT lpr;
	lpr = (LPPRINT)GetWindowLong(GetParent(hDlg), 4);

	switch(message) {
		case WM_INITDIALOG:
			lpr->hDlgPrint = hDlg;
			SetWindowText(hDlg,(LPSTR)lParam);
			EnableMenuItem(GetSystemMenu(hDlg,FALSE),SC_CLOSE,MF_GRAYED);
			return TRUE;
		case WM_COMMAND:
			lpr->bUserAbort = TRUE;
			lpr->hDlgPrint = 0;
			EnableWindow(GetParent(hDlg),TRUE);
			EndDialog(hDlg, FALSE);
			return TRUE;
	}
	return FALSE;
}

	
BOOL CALLBACK _export
PrintAbortProc(HDC hdcPrn, int code)
{
    MSG msg;
    LPPRINT lpr;
    lpr = PrintFind(hdcPrn);

    while (!lpr->bUserAbort && PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
		if (!lpr->hDlgPrint || !IsDialogMessage(lpr->hDlgPrint,&msg)) {
        	TranslateMessage(&msg);
        	DispatchMessage(&msg);
		}
    }
    return(!lpr->bUserAbort);
}


/* GetWindowLong(hwnd, 4) must be available for use */
void WDPROC
DumpPrinter(HWND hwnd, LPSTR szAppName, LPSTR szFileName)
{
HDC printer;
char *buf;
int *bufcount, count;
HFILE hfile;
DLGPROC lpfnAbortProc;
DLGPROC lpfnPrintDlgProc;
PRINTDLG pd;
PRINT pr;

	_fmemset(&pd, 0, sizeof(PRINTDLG));
	pd.lStructSize = sizeof(PRINTDLG);
	pd.hwndOwner = hwnd;
	pd.Flags = PD_PRINTSETUP | PD_RETURNDC;

	if (PrintDlg(&pd)) {
	printer = pd.hDC;
	if (printer != (HDC)NULL) {
	  pr.hdcPrn = printer;
	  SetWindowLong(hwnd, 4, (LONG)((LPPRINT)&pr));
	  PrintRegister((LPPRINT)&pr);
 	  if ( (hfile = _lopen(szFileName, READ)) != HFILE_ERROR) {
	      if ( (buf = LocalAllocPtr(LHND, 4096+2)) != (char *)NULL ) {
	    	bufcount = (int *)buf;
			EnableWindow(hwnd,FALSE);
			pr.bUserAbort = FALSE;
#ifdef __DLL__
			lpfnPrintDlgProc = (DLGPROC)GetProcAddress(hdllInstance, "PrintDlgProc");
			lpfnAbortProc = (DLGPROC)GetProcAddress(hdllInstance, "PrintAbortProc");
#else
			lpfnPrintDlgProc = (DLGPROC)MakeProcInstance((FARPROC)PrintDlgProc, hdllInstance);
			lpfnAbortProc = (DLGPROC)MakeProcInstance((FARPROC)PrintAbortProc, hdllInstance);
#endif
			pr.hDlgPrint = CreateDialogParam(hdllInstance,"PrintDlgBox",hwnd,lpfnPrintDlgProc,(LPARAM)szAppName);
			Escape(printer,SETABORTPROC,0,(LPSTR)lpfnAbortProc,NULL);  
			if (Escape(printer, STARTDOC, lstrlen(szAppName),szAppName, NULL) > 0)
			{
				while ( (count = _lread(hfile, buf+2, 4096)) != 0 ) {
					*bufcount = count;
					Escape(printer, PASSTHROUGH, count+2, (LPSTR)buf, NULL);
				}
				if (pr.bUserAbort) 
					Escape(printer,ABORTDOC,0,NULL,NULL);
				else
					Escape(printer,ENDDOC,0,NULL,NULL);
			}
			if (!pr.bUserAbort) {
				EnableWindow(hwnd,TRUE);
				DestroyWindow(pr.hDlgPrint);
			}
			LocalFreePtr(buf);
#ifndef __DLL__
			FreeProcInstance((FARPROC)lpfnPrintDlgProc);
			FreeProcInstance((FARPROC)lpfnAbortProc);
#endif
		}
		_lclose(hfile);
	  }
	  DeleteDC(printer);
	  SetWindowLong(hwnd, 4, (LONG)(0L));
	  PrintUnregister((LPPRINT)&pr);
	}
	}
}

#else
void WDPROC
DumpPrinter(HWND hwnd, LPSTR szAppName, LPSTR szFileName);
{
	MessageBox(GetDesktopWindow(), "wgnuplib.dll needs to be compiled with WINVER >= 0x030a",
		"wgnuplib.dll", MB_OK | MB_ICONEXCLAMATION
}
#endif
