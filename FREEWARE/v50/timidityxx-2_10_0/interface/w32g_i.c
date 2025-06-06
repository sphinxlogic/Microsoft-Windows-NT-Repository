/*
    TiMidity++ -- MIDI to WAVE converter and player
    Copyright (C) 1999,2000 Masanao Izumo <mo@goice.co.jp>
    Copyright (C) 1995 Tuukka Toivonen <tt@cgs.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    w32g_main.c: Written by Daisuke Aoki <dai@y7.net>
                 Modified by Masanao Izumo <mo@goice.co.jp>
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <stddef.h>
#include <windows.h>
#undef RC_NONE
#include <shlobj.h>
// #include <prsht.h>
#if defined(__CYGWIN32__) || defined(__MINGW32__)
#include <commdlg.h>
#ifndef TPM_TOPALIGN
#define TPM_TOPALIGN	0x0000L	/* for old version of cygwin */
#endif
#define TIME_ONESHOT 0
#define TIME_PERIODIC 1
WINAPI int timeSetEvent(UINT uDelay, UINT uResolution,
								void *fptc, DWORD dwUser, UINT fuEvent);
WINAPI int timeKillEvent(UINT uTimerID);
#else
#include <commctrl.h>
#endif /* __CYGWIN32__ */
#include <commctrl.h>
#ifndef NO_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#include <windowsx.h>	/* There is no <windowsx.h> on CYGWIN.
			 * Edit_* and ListBox_* are defined in
			 * <windowsx.h>
			 */

#include "timidity.h"
#include "common.h"
#include "instrum.h"
#include "playmidi.h"
#include "readmidi.h"
#include "output.h"
#include "controls.h"

#include "w32g.h"
#include "w32g_res.h"
#include "w32g_utl.h"
#include "w32g_pref.h"
#include "w32g_subwin.h"

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#define WIN32GCC
WINAPI void InitCommonControls(void);
#endif


static void InitMainWnd(HWND hStartWnd);

static void ConsoleWndVerbosityApplyIncDec(int num);
void ConsoleWndVerbosityApply(void);

void CanvasPaintAll(void);
void CanvasReset(void);
void CanvasClear(void);
void CanvasUpdate(int flag);
void CanvasReadPanelInfo(int flag);
void CanvasChange(int mode);
void MPanelPaintAll(void);
void MPanelReadPanelInfo(int flag);
void MPanelReset(void);
void MPanelUpdateAll(void);
void ClearConsoleWnd(void);
void InitListWnd(HWND hParentWnd);
void InitTracerWnd(HWND hParentWnd);
void InitWrdWnd(HWND hParentWnd);
void InitDocWnd(HWND hParentWnd);
void PutsDocWnd(char *str);
void ClearDocWnd(void);
static void DlgPlaylistSave(HWND hwnd);
static void DlgPlaylistOpen(HWND hwnd);
static void DlgDirOpen(HWND hwnd);
static void DlgMidiFileOpen(HWND hwnd);
void VprintfEditCtlWnd(HWND hwnd, char *fmt, va_list argList);
void PutsEditCtlWnd(HWND hwnd, char *str);
void ClearEditCtlWnd(HWND hwnd);

#ifndef CLR_INVALID
#define CLR_INVALID 0xffffffff
#endif /* CLR_INVALID */
extern int optind;

HINSTANCE hInst;
static int progress_jump = -1;
static HWND hMainWndScrollbarProgressWnd;
static HWND hMainWndScrollbarVolumeWnd;
#define W32G_VOLUME_MAX 200

// HWND
HWND hStartWnd = 0;
HWND hMainWnd = 0;
HWND hDebugWnd = 0;
HWND hConsoleWnd = 0;
HWND hTracerWnd = 0;
HWND hDocWnd = 0;
HWND hListWnd = 0;
HWND hWrdWnd = 0;
HWND hSoundSpecWnd = 0;
HWND hDebugEditWnd = 0;
HWND hDocEditWnd = 0;

// Process.
HANDLE hProcess = 0;

// Main Thread.
HANDLE hMainThread = 0;
HANDLE hPlayerThread = 0;
HANDLE hMainThreadInfo = 0;
DWORD dwMainThreadID = 0;
static volatile int wait_thread_flag = 1;
typedef struct MAINTHREAD_ARGS_ {
	int *pArgc;
	char ***pArgv;
} MAINTHREAD_ARGS;
void WINAPI MainThread(void *arglist);

// Window Thread
HANDLE hWindowThread = 0;
HANDLE hWindowThreadInfo = 0;

// Thread
volatile int ThreadNumMax = 0;

// Debug Thread
volatile int DebugThreadExit = 1;
volatile HANDLE hDebugThread = 0;
void DebugThreadInit(void);
void PrintfDebugWnd(char *fmt, ...);
void ClearDebugWnd(void);
void InitDebugWnd(HWND hParentWnd);

// Flags
int InitMinimizeFlag = 0;
int DebugWndStartFlag = 1;
int ConsoleWndStartFlag = 0;
int ListWndStartFlag = 0;
int TracerWndStartFlag = 0;
int DocWndStartFlag = 0;
int WrdWndStartFlag = 0;

int DebugWndFlag = 1;
int ConsoleWndFlag = 1;
int ListWndFlag = 1;
int TracerWndFlag = 0;
int DocWndFlag = 1;
int WrdWndFlag = 0;
int SoundSpecWndFlag = 0;

int WrdGraphicFlag;
int TraceGraphicFlag;

char *IniFile;
char *ConfigFile;
char *PlaylistFile;
char *PlaylistHistoryFile;
char *MidiFileOpenDir;
char *ConfigFileOpenDir;
char *PlaylistFileOpenDir;

// Priority
int ProcessPriority;
int PlayerThreadPriority;
int MidiPlayerThreadPriority;
int MainThreadPriority;
int GUIThreadPriority;
int TracerThreadPriority;
int WrdThreadPriority;

// dir
int SeachDirRecursive = 0;	// 再帰的ディレクトリ検索 

// Ini File
int IniFileAutoSave = 1;	// INI ファイルの自動セーブ

// misc
int DocMaxSize;
char *DocFileExt;

static volatile int w32g_wait_for_init;
void w32g_send_rc(int rc, int32 value);
int w32g_lock_open_file = 0;

void TiMidityHeapCheck(void);

volatile DWORD dwWindowThreadID = -1;
void w32g_i_init(void)
{
    ThreadNumMax++;
    hProcess = GetCurrentProcess();
    hWindowThread = GetCurrentThread();
	dwWindowThreadID = GetCurrentThreadId();

    InitCommonControls();

#ifdef W32GUI_DEBUG
    DebugThreadInit();
#endif
}

//int PlayerLanguage = LANGUAGE_ENGLISH;
int PlayerLanguage = LANGUAGE_JAPANESE;
#define PInfoOK 1
long SetValue(int32 value, int32 min, int32 max)
{
  int32 v = value;
  if(v < min) v = min;
  else if( v > max) v = max;
  return v;
}

int w32gSecondTiMidity(int opt, int argc, char **argv);
int w32gSecondTiMidityExit(void);
int SecondMode = 0;
void FirstLoadIniFile(void);

#ifndef WIN32GCC
static void CmdLineToArgv(LPSTR lpCmdLine, int *argc, CHAR ***argv);
extern int win_main(int argc, char **argv); /* timidity.c */
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
LPSTR lpCmdLine, int nCmdShow)
{
	int argc;
	CHAR **argv = NULL;
	CmdLineToArgv(lpCmdLine,&argc,&argv);
#if 0
	FirstLoadIniFile();
	if(w32gSecondTiMidity(SecondMode,argc,argv)==TRUE){
		int res = win_main(argc, argv);
		w32gSecondTiMidityExit();
		return res;
	} else
		return -1;
#else
	return win_main(argc, argv);
#endif
}
#endif /* WIN32GCC */

// ***************************************************************************
// System Function

void CALLBACK KillProcess(UINT IDEvent, UINT uReserved, DWORD dwUser,
	DWORD dwReserved1, DWORD dwReserved2)
{
	exit(0);
	//	ExitProcess(0);
}

void OnExit(void)
{
#ifdef W32GUI_DEBUG
	PrintfDebugWnd("PostQuitMessage\n");
	Sleep(200);
#endif
	PostQuitMessage(0);
}

static int OnExitReadyWait = 200;
void OnExitReady(void)
{
	int i;
#ifdef W32GUI_DEBUG
	PrintfDebugWnd("OnExitReady: Start.\n");
#endif
    w32g_send_rc(RC_STOP, 0);

	/* Exit after 10 sec. */
	
	timeSetEvent(10000, 1000, KillProcess, 0, TIME_ONESHOT);

	/* Wait really stopping to play */
	i = 1000/OnExitReadyWait; /* 1 sec. */
    while(w32g_play_active && i-- > 0)
	{
		Sleep(OnExitReadyWait);
		VOLATILE_TOUCH(w32g_play_active);
	}

#ifdef W32GUI_DEBUG
	PrintfDebugWnd("OnExitReady: End.\n");
#endif
}

void OnQuit(void)
{
	SendMessage(hStartWnd, WM_CLOSE, 0, 0);
}


// ***************************************************************************
// Start Window
// 大元のウインドウ。このウインドウは普段は見えない。デバッグ用に
// 使う予定ではあったがぜんぜん使用されていない。単に親ウインドウ
// であるだけ。

#define STARTWND_XSIZE 100
#define STARTWND_YSIZE 100
static char StartWndClassName[] = "TiMidity_Win32GUI";

static LRESULT CALLBACK StartWinProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam);
void InitStartWnd(int nCmdShow)
{
	WNDCLASS wndclass ;
	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = StartWinProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInst ;
	wndclass.hIcon         = LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON_TIMIDITY)) ;
	wndclass.hCursor       = LoadCursor(0,IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH)(COLOR_SCROLLBAR + 1);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName =  StartWndClassName;

	RegisterClass(&wndclass);
	hStartWnd = CreateWindowEx(WS_EX_DLGMODALFRAME,StartWndClassName,0,
		WS_OVERLAPPED  | WS_MINIMIZEBOX | WS_SYSMENU | WS_CLIPCHILDREN ,
		CW_USEDEFAULT,0,STARTWND_XSIZE,STARTWND_YSIZE,0,0,hInst,0);
	ShowWindow(hStartWnd,SW_HIDE);
//	ShowWindow(hStartWnd,SW_SHOW);
//	ShowWindow(hStartWnd,nCmdShow);
	UpdateWindow(hStartWnd);
	InitMainWnd(hStartWnd);
	InitConsoleWnd(hStartWnd);
	InitListWnd(hStartWnd);
	InitTracerWnd(hStartWnd);
	InitDocWnd(hStartWnd);
	InitWrdWnd(hStartWnd);
	InitSoundSpecWnd(hStartWnd);
        hMainWndScrollbarProgressWnd = GetDlgItem(hMainWnd, IDC_SCROLLBAR_PROGRESS);
	hMainWndScrollbarVolumeWnd = GetDlgItem(hMainWnd, IDC_SCROLLBAR_VOLUME);
	EnableScrollBar(hMainWndScrollbarVolumeWnd, SB_CTL,ESB_ENABLE_BOTH);
	SetScrollRange(hMainWndScrollbarVolumeWnd, SB_CTL,
		       0, W32G_VOLUME_MAX, TRUE);
	SetScrollPos(hMainWndScrollbarVolumeWnd, SB_CTL,
		     W32G_VOLUME_MAX - amplification, TRUE);
}

LRESULT CALLBACK
StartWinProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam)
{
	switch (uMess)
	{
	  case WM_DESTROY:
	    PostQuitMessage(0);
	    break;
	  default:
	    return DefWindowProc(hwnd,uMess,wParam,lParam);
	}
	return 0L;
}

/*****************************************************************************/
// Main Window

#define SWS_EXIST		0x0001
#define SWS_ICON		0x0002
#define SWS_HIDE		0x0004
typedef struct SUBWINDOW_ {
	HWND *hwnd;
	int status;
}	SUBWINDOW;
SUBWINDOW subwindow[] =
{
  {&hConsoleWnd,0},
  {&hListWnd,0},
  {&hTracerWnd,0},
  {&hDocWnd,0},
  {&hWrdWnd,0},
  {&hSoundSpecWnd,0},
  {NULL,0}
};

int SubWindowMax = 3;
SUBWINDOW SubWindowHistory[] =
{
  {&hConsoleWnd,0},
  {&hListWnd,0},
  {&hTracerWnd,0},
  {&hDocWnd,0},
  {&hWrdWnd,0},
  {&hSoundSpecWnd,0},
  {NULL,0}
};

BOOL CALLBACK MainProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam);

void update_subwindow(void);
void OnShow(void);
void OnHide(void);

static void InitMainWnd(HWND hParentWnd)
{
	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_TIMIDITY));
	hMainWnd = CreateDialog(hInst,MAKEINTRESOURCE(IDD_DIALOG_MAIN),hParentWnd,MainProc);
	if (hIcon!=NULL) SendMessage(hMainWnd,WM_SETICON,FALSE,(LPARAM)hIcon);
	{  // Set the title of the main window again.
   	char buffer[256];
   	SendMessage( hMainWnd, WM_GETTEXT, (WPARAM)255, (LPARAM)buffer);
   	SendMessage( hMainWnd, WM_SETTEXT, (WPARAM)0, (LPARAM)buffer);
	}
}

void MainCmdProc(HWND hwnd, int wId, HWND hwndCtl, UINT wNotifyCode);

void MainWndSetPauseButton(int flag);
void MainWndSetPlayButton(int flag);

void MainWndToggleConsoleButton(void);
void MainWndUpdateConsoleButton(void);
void MainWndToggleTracerButton(void);
void MainWndUpdateTracerButton(void);
void MainWndToggleListButton(void);
void MainWndUpdateListButton(void);
void MainWndToggleDocButton(void);
void MainWndUpdateDocButton(void);
void MainWndToggleWrdButton(void);
void MainWndUpdateWrdButton(void);
void MainWndToggleSoundSpecButton(void);
void MainWndUpdateSoundSpecButton(void);

void ShowSubWindow(HWND hwnd,int showflag);
void ToggleSubWindow(HWND hwnd);

static void VersionWnd(HWND hParentWnd);
static void TiMidityWnd(HWND hParentWnd);

static void InitCanvasWnd(HWND hwnd);
static void CanvasInit(HWND hwnd);
static void InitPanelWnd(HWND hwnd);
static void MPanelInit(HWND hwnd);

static void InitMainToolbar(HWND hwnd);
static void InitSubWndToolbar(HWND hwnd);


static UINT PlayerForwardAndBackwardEventID = 0;
static void CALLBACK PlayerForward(UINT IDEvent, UINT uReserved, DWORD dwUser,
	DWORD dwReserved1, DWORD dwReserved2)
{
	w32g_send_rc(RC_FORWARD, play_mode->rate);
}

static void CALLBACK PlayerBackward(UINT IDEvent, UINT uReserved, DWORD dwUser,
	DWORD dwReserved1, DWORD dwReserved2)
{
	w32g_send_rc(RC_BACK, play_mode->rate);
}

BOOL CALLBACK
MainProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam)
{
	// PrintfDebugWnd("MainProc: Mess%lx WPARAM%lx LPARAM%lx\n",uMess,wParam,lParam);
	switch (uMess)
	{
      case WM_INITDIALOG:
		update_subwindow();
		MainWndUpdateConsoleButton();
		MainWndUpdateTracerButton();
		MainWndUpdateListButton();
		MainWndUpdateDocButton();
		MainWndUpdateWrdButton();
		MainWndUpdateSoundSpecButton();
		InitPanelWnd(hwnd);
		InitCanvasWnd(hwnd);
		InitMainToolbar(hwnd);
		InitSubWndToolbar(hwnd);
	    {
			HMENU hMenu = GetSystemMenu(hwnd, FALSE);
#if 1
			RemoveMenu(hMenu,SC_MAXIMIZE,MF_BYCOMMAND);
			RemoveMenu(hMenu,SC_SIZE,MF_BYCOMMAND);
#else
			EnableMenuItem(hMenu, SC_MAXIMIZE, MF_BYCOMMAND	| MF_GRAYED);
			EnableMenuItem(hMenu, SC_SIZE, MF_BYCOMMAND | MF_GRAYED);
#endif
			EnableMenuItem(hMenu, SC_MOVE, MF_BYCOMMAND | MF_GRAYED);
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
			InsertMenu(hMenu, 0, MF_BYPOSITION, SC_SCREENSAVE, "Screen Saver");
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, IDM_STOP, "Stop");
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, IDM_PAUSE, "Pause");
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, IDM_PREV, "Prev");
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, IDM_NEXT, "Next");
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, IDM_PLAY, "Play");
			DrawMenuBar(hwnd);
    	}
		return FALSE;
	  HANDLE_MSG(hwnd,WM_COMMAND,MainCmdProc);

	  case WM_CLOSE:
		DestroyWindow(hStartWnd);
		break;

      case WM_SIZE:
		if(wParam == SIZE_MINIMIZED){
			update_subwindow();
			OnHide();
		}
		return FALSE;
      case WM_QUERYOPEN:
		OnShow();
		return FALSE;
      case WM_DROPFILES:
		w32g_send_rc(RC_EXT_DROP, (int32)wParam);
		return FALSE;
      case WM_HSCROLL: {
		  int nScrollCode = (int)LOWORD(wParam);
		  int nPos = (int) HIWORD(wParam);
		  HWND bar = (HWND)lParam;

		  if(bar != hMainWndScrollbarProgressWnd)
			  break;

		  switch(nScrollCode)
		  {
			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:
			  progress_jump = nPos;
			  break;
			case SB_LINELEFT:
			  progress_jump = GetScrollPos(bar, SB_CTL) - 1;
			  if(progress_jump < 0)
				  progress_jump = 0;
			  break;
			case SB_PAGELEFT:
			  progress_jump = GetScrollPos(bar, SB_CTL) - 10;
			  if(progress_jump < 0)
				  progress_jump = 0;
			  break;
			case SB_LINERIGHT:
			  progress_jump = GetScrollPos(bar, SB_CTL) + 1;
			  break;
			case SB_PAGERIGHT:
			  progress_jump = GetScrollPos(bar, SB_CTL) + 10;
			  break;
			case SB_ENDSCROLL:
			  if(progress_jump != -1)
			  {
				  w32g_send_rc(RC_JUMP, progress_jump * play_mode->rate);
				  SetScrollPos(hMainWndScrollbarProgressWnd, SB_CTL,
							   progress_jump, TRUE);
				  progress_jump = -1;
			  }
			  break;
		  }
		  break;
		}
	  break;

	  case WM_VSCROLL: {
		int nScrollCode = (int) LOWORD(wParam);
		int nPos = (int) HIWORD(wParam);
		HWND bar = (HWND) lParam;
		static int pos = -1;

		if(bar != hMainWndScrollbarVolumeWnd)
			break;

		switch(nScrollCode)
		{
		  case SB_THUMBTRACK:
		  case SB_THUMBPOSITION:
			pos = nPos;
			break;
		  case SB_LINEUP:
		  case SB_PAGEUP:
			pos = GetScrollPos(bar, SB_CTL) - 5;
			if(pos < 0)
				pos = 0;
			break;
		  case SB_LINEDOWN:
		  case SB_PAGEDOWN:
			pos = GetScrollPos(bar, SB_CTL) + 5;
			if(pos > W32G_VOLUME_MAX)
				pos = W32G_VOLUME_MAX;
			break;
		  case SB_ENDSCROLL:
			if(pos != -1)
			{
				w32g_send_rc(RC_CHANGE_VOLUME,
							 (W32G_VOLUME_MAX - pos) - amplification);
				SetScrollPos(bar, SB_CTL, pos, TRUE);
				pos = -1;
			}
			break;
		}
	  }
	  break;

	  case WM_SYSCOMMAND:
		switch(wParam){
		  case IDM_STOP:
		  case IDM_PAUSE:
		  case IDM_PREV:
		  case IDM_PLAY:
		  case IDM_NEXT:
		  case IDM_FOREWARD:
		  case IDM_BACKWARD:
			SendMessage(hwnd,WM_COMMAND,wParam,(LPARAM)NULL);
			break;
		  default:
			break;
		}
		return FALSE;
	/*
	   case WM_SETFOCUS:
	   HideCaret((HWND)wParam);
	   break;
	   case WM_KILLFOCUS:
	   ShowCaret((HWND)wParam);
	   break;
	   */
      case WM_NOTIFY:
		switch(wParam) {
		  case IDC_TOOLBARWINDOW_MAIN:{
			  LPTBNOTIFY TbNotify = (LPTBNOTIFY)lParam;
			  switch(TbNotify->iItem){
				case IDM_BACKWARD:
				  if(TbNotify->hdr.code==TBN_BEGINDRAG){
#ifdef W32GUI_DEBUG
					  //PrintfDebugWnd("IDM_BACKWARD: BUTTON ON\n");
#endif
					  PlayerBackward(0,0,0,0,0);
					  PlayerForwardAndBackwardEventID =
						  timeSetEvent(100, 100, PlayerBackward, 0, TIME_PERIODIC);
				  }
				  if(PlayerForwardAndBackwardEventID != 0)
					  if(TbNotify->hdr.code==TBN_ENDDRAG){
#ifdef W32GUI_DEBUG
						  //PrintfDebugWnd("IDM_BACKWARD: BUTTON OFF\n");
#endif
						  timeKillEvent(PlayerForwardAndBackwardEventID);
						  PlayerForwardAndBackwardEventID = 0;
					  }
				  break;
				case IDM_FOREWARD:
				  if(TbNotify->hdr.code ==TBN_BEGINDRAG &&
					 PlayerForwardAndBackwardEventID == 0){
#ifdef W32GUI_DEBUG
					  //PrintfDebugWnd("IDM_FOREWARD: BUTTON ON\n");
#endif
					  PlayerForward(0,0,0,0,0);
					  PlayerForwardAndBackwardEventID =
						  timeSetEvent(100,300,PlayerForward,0,TIME_PERIODIC);
				  }
				  else if((TbNotify->hdr.code == TBN_ENDDRAG ||
						   TbNotify->hdr.code == NM_CLICK ||
						   TbNotify->hdr.code == NM_RCLICK) &&
						  PlayerForwardAndBackwardEventID != 0)
				  {
#ifdef W32GUI_DEBUG
					  //PrintfDebugWnd("IDM_FOREWARD: BUTTON OFF\n");
#endif
					  timeKillEvent(PlayerForwardAndBackwardEventID);
					  PlayerForwardAndBackwardEventID = 0;
				  }
				  break;
				default:
				  break;
			  }
		    }
			break; /* end of case IDC_TOOLBARWINDOW_MAIN */
		  default:
			break;
		}
		return FALSE;
      default:
		return FALSE;
    }
	return FALSE;
}

void MainCmdProc(HWND hwnd, int wId, HWND hwndCtl, UINT wNotifyCode)
{
	 // PrintfDebugWnd("WM_COMMAND: ID%lx HWND%lx CODE%lx\n",wId,hwndCtl,wNotifyCode);
    switch(wId)
    {
      case IDM_STOP:
		  w32g_send_rc(RC_STOP, 0);
		  break;
      case IDM_PAUSE:
		  SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_MAIN,
									TB_CHECKBUTTON, IDM_PAUSE,
									(LPARAM)MAKELONG(!play_pause_flag, 0));
		  w32g_send_rc(RC_TOGGLE_PAUSE, 0);
		  break;
      case IDM_PREV:
		  w32g_send_rc(RC_REALLY_PREVIOUS, 0);
		  break;
      case IDM_BACKWARD:
		  /* Do nothing here. See WM_NOTIFY in MainProc() */
		  break;
      case IDM_PLAY:
		  if(play_pause_flag)
		  {
				SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_MAIN,
										 TB_CHECKBUTTON, IDM_PAUSE,
										 (LPARAM)MAKELONG(FALSE, 0));
				w32g_send_rc(RC_TOGGLE_PAUSE, 0);
		  }
		  if(!w32g_play_active)
				w32g_send_rc(RC_LOAD_FILE, 0);
		  break;
      case IDM_FOREWARD:
		  /* Do nothing here. See WM_NOTIFY in MainProc() */
		  break;
      case IDM_NEXT:
		  w32g_send_rc(RC_NEXT, 0);
		  break;
      case IDM_CONSOLE:
      case IDM_MWCONSOLE:
		  ToggleSubWindow(hConsoleWnd);
		  break;
      case IDM_TRACER:
      case IDM_MWTRACER:
		  MainWndUpdateTracerButton();
		  MessageBox(hwnd, "Not Supported.","Warning!",MB_OK);
		  break;
      case IDM_LIST:
      case IDM_MWPLAYLIST:
		  ToggleSubWindow(hListWnd);
		  if(IsWindowVisible(hListWnd))
			w32g_send_rc(RC_EXT_UPDATE_PLAYLIST, 0);
		  break;
      case IDM_DOC:
      case IDM_MWDOCUMENT:
		  ToggleSubWindow(hDocWnd);
//		  if(IsWindowVisible(hDocWnd))
//			  w32g_send_rc(RC_EXT_DOC, 0);
		  break;
      case IDM_WRD:
      case IDM_MWWRDTRACER:
		  MainWndUpdateWrdButton();
		  MessageBox(hwnd, "Not Supported.","Warning!",MB_OK);
		  break;
      case IDM_SOUNDSPEC:
      case IDM_MWSOUNDSPEC:
		  MainWndUpdateSoundSpecButton();
		  MessageBox(hwnd, "Not Supported.","Warning!",MB_OK);
		  break;
      case IDOK:
		  break;
      case IDCANCEL:
		  OnQuit();
		  break;
      case IDM_MFOPENFILE:
		  DlgMidiFileOpen(hwnd);
		  break;
      case IDM_MFOPENDIR:
		  DlgDirOpen(hwnd);
		  break;
      case IDM_MFLOADPLAYLIST:
		  DlgPlaylistOpen(hwnd);
		  break;
      case IDM_MFSAVEPLAYLISTAS:
		  DlgPlaylistSave(hwnd);
		  break;
      case IDM_MFEXIT:
		  OnQuit();
		  break;

      case IDM_SETTING:
		  PrefWndCreate(hStartWnd);
		  break;

      case IDM_MCSAVEINIFILE:
		  VOLATILE_TOUCH(PrefWndDoing);
		  if(PrefWndDoing){
				MessageBox(hMainWnd, "Can't Save Ini file while preference dialog.",
							  "Warning", MB_OK);
				break;
		  }
		  SaveIniFile(sp_current, st_current);
		  break;
      case IDM_MCLOADINIFILE:
		 if(!w32g_has_ini_file) {
		     MessageBox(hMainWnd, "Can't load Ini file.",
				"Warning", MB_OK);
		     break;
		 }

		  VOLATILE_TOUCH(PrefWndDoing);
		  if(PrefWndDoing){
				MessageBox(hMainWnd, "Can't load Ini file while preference dialog.",
							  "Warning", MB_OK);
				break;
		  }
		  LoadIniFile(sp_temp,st_temp);
		  PrefWndCreate(hStartWnd);
		  break;
      case IDM_MWDEBUG:
#ifdef W32GUI_DEBUG
		  if(IsWindowVisible(hDebugWnd))
				ShowWindow(hDebugWnd,SW_HIDE);
		  else
				ShowWindow(hDebugWnd,SW_SHOW);
#endif
		  break;
      case IDM_MHTOPIC:
		  MessageBox(hwnd, "Not Supported.","Warning!",MB_OK);
		  break;
      case IDM_MHHELP:
		  MessageBox(hwnd, "Not Supported.","Warning!",MB_OK);
		  break;
      case IDM_MHVERSION:
		  VersionWnd(hwnd);
		  break;
      case IDM_MHTIMIDITY:
		  TiMidityWnd(hwnd);
		  break;
    }
}



void update_subwindow(void)
{
  SUBWINDOW *s = subwindow;
  int i;
  for(i=0;s[i].hwnd!=NULL;i++){
		if(IsWindow(*(s[i].hwnd)))
  		s[i].status |= SWS_EXIST;
		else {
  		s[i].status = 0;
    	continue;
    }
		if(IsIconic(*(s[i].hwnd)))
  		s[i].status |= SWS_ICON;
		else
  		s[i].status &= ~ SWS_ICON;
		if(IsWindowVisible(*(s[i].hwnd)))
  		s[i].status &= ~ SWS_HIDE;
		else
  		s[i].status |= SWS_HIDE;
	}
}

void MainWndSetPauseButton(int flag)
{
	if(flag)
  	SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_MAIN,
    	TB_CHECKBUTTON, IDM_PAUSE, (LPARAM)MAKELONG(TRUE, 0));
	else
  	SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_MAIN,
    	TB_CHECKBUTTON, IDM_PAUSE, (LPARAM)MAKELONG(FALSE, 0));
}

void MainWndSetPlayButton(int flag)
{
	return;
}

void MainWndUpdateConsoleButton(void)
{
	if(IsWindowVisible(hConsoleWnd))
  	SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_SUBWND,
    	TB_CHECKBUTTON, IDM_CONSOLE, (LPARAM)MAKELONG(TRUE, 0));
	else
  	SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_SUBWND,
    	TB_CHECKBUTTON, IDM_CONSOLE, (LPARAM)MAKELONG(FALSE, 0));
}

void MainWndUpdateListButton(void)
{
	if(IsWindowVisible(hListWnd))
  	SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_SUBWND,
    	TB_CHECKBUTTON, IDM_LIST, (LPARAM)MAKELONG(TRUE, 0));
	else
  	SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_SUBWND,
    	TB_CHECKBUTTON, IDM_LIST, (LPARAM)MAKELONG(FALSE, 0));
}

void MainWndUpdateDocButton(void)
{
	if(IsWindowVisible(hDocWnd))
  	SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_SUBWND,
    	TB_CHECKBUTTON, IDM_DOC, (LPARAM)MAKELONG(TRUE, 0));
	else
  	SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_SUBWND,
    	TB_CHECKBUTTON, IDM_DOC, (LPARAM)MAKELONG(FALSE, 0));
}

void MainWndUpdateTracerButton(void)
{
	if(IsWindowVisible(hTracerWnd))
		SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_SUBWND,
			TB_CHECKBUTTON, IDM_TRACER,
			(LPARAM)MAKELONG(TRUE, 0));
   else
  		SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_SUBWND,
			TB_CHECKBUTTON, IDM_TRACER,
			(LPARAM)MAKELONG(FALSE, 0));
}

void MainWndUpdateWrdButton(void)
{
	if(IsWindowVisible(hWrdWnd))
  	SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_SUBWND,
    	TB_CHECKBUTTON, IDM_WRD, (LPARAM)MAKELONG(TRUE, 0));
	else
  	SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_SUBWND,
    	TB_CHECKBUTTON, IDM_WRD, (LPARAM)MAKELONG(FALSE, 0));
}

void MainWndUpdateSoundSpecButton(void)
{
}

void ShowSubWindow(HWND hwnd,int showflag)
{
	int i, num;
  RECT rc,rc2;
	int max = 0;
	if(showflag){
		if(IsWindowVisible(hwnd))
  		return;
		for(i=0;SubWindowHistory[i].hwnd!=NULL;i++)
  		if(*(SubWindowHistory[i].hwnd)==hwnd)
      	num = i;
		for(i=0;SubWindowHistory[i].hwnd!=NULL;i++)
  		if(*(SubWindowHistory[i].hwnd)!=hwnd){
	  		if(SubWindowHistory[i].status > 0)
					SubWindowHistory[i].status += 1;
	  		if(SubWindowHistory[i].status>SubWindowMax){
					if(SubWindowHistory[i].status>max){
    				GetWindowRect(*(SubWindowHistory[i].hwnd), &rc);
						max = SubWindowHistory[i].status;
					}
      		ShowWindow(*(SubWindowHistory[i].hwnd),SW_HIDE);
					SubWindowHistory[i].status = 0;
				}
			}
		if(max>0){
			GetWindowRect(hwnd, &rc2);
			MoveWindow(hwnd,rc.left,rc.top,rc2.right-rc2.left,rc2.bottom-rc2.top,TRUE);
		}
		ShowWindow(hwnd,SW_SHOW);
		SubWindowHistory[num].status = 1;
	} else {
		if(!IsWindowVisible(hwnd))
  		return;
		for(i=0;SubWindowHistory[i].hwnd!=NULL;i++)
  		if(*(SubWindowHistory[i].hwnd)==hwnd)
      	num = i;
		for(i=0;SubWindowHistory[i].hwnd!=NULL;i++)
    	if(i!=num)
	  		if(SubWindowHistory[i].status>=SubWindowHistory[num].status)
					SubWindowHistory[i].status -= 1;
    ShowWindow(hwnd,SW_HIDE);
		SubWindowHistory[num].status = 0;
	}
	MainWndUpdateConsoleButton();
	MainWndUpdateListButton();
	MainWndUpdateTracerButton();
	MainWndUpdateDocButton();
	MainWndUpdateWrdButton();
	MainWndUpdateSoundSpecButton();
}

void ToggleSubWindow(HWND hwnd)
{
	if(IsWindowVisible(hwnd))
		ShowSubWindow(hwnd,0);
  else
		ShowSubWindow(hwnd,1);
}

void OnShow(void)
{
  SUBWINDOW *s = subwindow;
	int i;
  for(i=0;s[i].hwnd!=NULL;i++)
		if(s[i].status & SWS_EXIST) {
			if(s[i].status & SWS_HIDE)
    		ShowWindow(*(s[i].hwnd),SW_HIDE);
			else
    		ShowWindow(*(s[i].hwnd),SW_SHOW);
		}
}

void OnHide(void)
{
  SUBWINDOW *s = subwindow;
	int i;
  for(i=0;s[i].hwnd!=NULL;i++){
		if(s[i].status & SWS_EXIST)
    	ShowWindow(*(s[i].hwnd),SW_HIDE);
  }
}

#ifdef W32GUI_DEBUG
void DebugThread(void *args)
{
	MSG msg;
	DebugThreadExit = 0;
	InitDebugWnd(NULL);
	AttachThreadInput(GetWindowThreadProcessId(hDebugThread,NULL),
   	GetWindowThreadProcessId(hWindowThread,NULL),TRUE);
	AttachThreadInput(GetWindowThreadProcessId(hWindowThread,NULL),
   	GetWindowThreadProcessId(hDebugThread,NULL),TRUE);
	while( GetMessage(&msg,NULL,0,0) ){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		Sleep(0);
	}
	DebugThreadExit = 1;
	crt_endthread();
}

void DebugThreadInit(void)
{
	DWORD dwThreadID;
	if(!DebugThreadExit)
   	return;
	hDebugThread = crt_beginthreadex(NULL,0,DebugThread,0,0,&dwThreadID);
}
#endif





//-----------------------------------------------------------------------------
// Toolbar Main

static TBBUTTON MainTbb[] = {
    {4, IDM_STOP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {3, IDM_PAUSE, TBSTATE_ENABLED, TBSTYLE_CHECK, 0, 0},
    {0, IDM_PREV, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {1, IDM_BACKWARD, TBSTATE_ENABLED,TBSTYLE_BUTTON, 0, 0},
    {2, IDM_PLAY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {5, IDM_FOREWARD, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
    {6, IDM_NEXT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0}
};

static void InitMainToolbar(HWND hwnd)
{
	TBADDBITMAP MainTbab;
  SendDlgItemMessage(hwnd, IDC_TOOLBARWINDOW_MAIN,
  		TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
  SendDlgItemMessage(hwnd, IDC_TOOLBARWINDOW_MAIN,
  		TB_SETBUTTONSIZE, (WPARAM)0, (LPARAM)MAKELONG(16,16));
	MainTbab.hInst = hInst;
	MainTbab.nID =(int)IDB_BITMAP_MAIN_BUTTON;
  SendDlgItemMessage(hwnd, IDC_TOOLBARWINDOW_MAIN,
  	TB_ADDBITMAP, 7, (LPARAM)&MainTbab);
  SendDlgItemMessage(hwnd, IDC_TOOLBARWINDOW_MAIN,
  	TB_ADDBUTTONS, (WPARAM)7,(LPARAM)&MainTbb);
  SendDlgItemMessage(hwnd, IDC_TOOLBARWINDOW_MAIN,
		TB_AUTOSIZE, 0, 0);
}

//-----------------------------------------------------------------------------
// Toolbar SubWnd

static TBBUTTON SubWndTbb[] = {
    {3, IDM_CONSOLE, TBSTATE_ENABLED, TBSTYLE_CHECK, 0, 0},
    {1, IDM_LIST, TBSTATE_ENABLED, TBSTYLE_CHECK, 0, 0},
    {2, IDM_TRACER, TBSTATE_ENABLED, TBSTYLE_CHECK, 0, 0},
    {0, IDM_DOC, TBSTATE_ENABLED, TBSTYLE_CHECK, 0, 0},
    {4, IDM_WRD, TBSTATE_ENABLED, TBSTYLE_CHECK, 0, 0},
};

static void InitSubWndToolbar(HWND hwnd)
{
	TBADDBITMAP SubWndTbab;
  SendDlgItemMessage(hwnd, IDC_TOOLBARWINDOW_SUBWND,
  		TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
  SendDlgItemMessage(hwnd, IDC_TOOLBARWINDOW_SUBWND,
  		TB_SETBUTTONSIZE, (WPARAM)0, (LPARAM)MAKELONG(16,16));
	SubWndTbab.hInst = hInst;
	SubWndTbab.nID =(int)IDB_BITMAP_SUBWND_BUTTON;
  SendDlgItemMessage(hwnd, IDC_TOOLBARWINDOW_SUBWND,
  	TB_ADDBITMAP, 5, (LPARAM)&SubWndTbab);
  SendDlgItemMessage(hwnd, IDC_TOOLBARWINDOW_SUBWND,
  	TB_ADDBUTTONS, (WPARAM)5,(LPARAM)&SubWndTbb);
  SendDlgItemMessage(hwnd, IDC_TOOLBARWINDOW_SUBWND,
		TB_AUTOSIZE, 0, 0);
}



//-----------------------------------------------------------------------------
// Canvas Window

#define MAPBAR_LIKE_TIMIDI	// note bar view like timidi
#define TM_CANVAS_XMAX 160
#define TM_CANVAS_YMAX 160
#define TM_CANVASMAP_XMAX 16
#define TM_CANVASMAP_YMAX 16
#define CANVAS_XMAX 160
#define CANVAS_YMAX 160
#define CMAP_XMAX 16
#define CMAP_YMAX 16
#define CK_MAX_CHANNELS 16
#define CMAP_MODE_16		1
#define CMAP_MODE_32		2
#define CMAP_MAX_CHANNELS	32
struct Canvas_ {
	HWND hwnd;
	HWND hParentWnd;
	RECT rcMe;
	int Mode;
	HDC hdc;
	HDC hmdc;
	HGDIOBJ hgdiobj_hmdcprev;
	int UpdateAll;
	int PaintDone;
	HANDLE hPopupMenu;
	HANDLE hPopupMenuKeyboard;
// Sleep mode
	RECT rcSleep;
	HBITMAP hbitmap;
	HBITMAP hBitmapSleep;
	int SleepUpdateFlag;
// Map mode
	RECT rcMap;
	char MapMap[CMAP_MAX_CHANNELS][CMAP_YMAX];
	char MapMapOld[CMAP_MAX_CHANNELS][CMAP_YMAX];
	char MapBar[CMAP_MAX_CHANNELS];
	char MapBarOld[CMAP_MAX_CHANNELS];
	int MapDelay;
	int MapResidual;
	int MapUpdateFlag;
	int MapMode;
	int MapBarWidth;
	int MapBarMax;
   int MapCh;
   int MapPan[CMAP_MAX_CHANNELS];
   int MapPanOld[CMAP_MAX_CHANNELS];
   int MapSustain[CMAP_MAX_CHANNELS];
   int MapSustainOld[CMAP_MAX_CHANNELS];
   int MapExpression[CMAP_MAX_CHANNELS];
   int MapExpressionOld[CMAP_MAX_CHANNELS];
   int MapVolume[CMAP_MAX_CHANNELS];
   int MapVolumeOld[CMAP_MAX_CHANNELS];
   int MapPitchbend[CMAP_MAX_CHANNELS];
   int MapPitchbendOld[CMAP_MAX_CHANNELS];
   int MapChChanged;
	ChannelBitMask DrumChannel;
   RECT rcMapMap;
   RECT rcMapSub;
// Keyboard mode
	RECT rcKeyboard;
	uint32 CKxnote[MAX_W32G_MIDI_CHANNELS][4];
	uint32 CKxnote_old[MAX_W32G_MIDI_CHANNELS][4];
	int CKNoteFrom;
   int CKNoteTo;
	int CKCh;
	int CKPart;
	int KeyboardUpdateFlag;
  	int xnote_reset;
// misc
   Channel channel[MAX_W32G_MIDI_CHANNELS];
} volatile Canvas;

#define IDC_CANVAS 4242

static HWND hCanvasWnd;
static char CanvasWndClassName[] = "TiMidity Canvas";
static LRESULT CALLBACK CanvasWndProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam);
static void CanvasPaintDo(void);

#define IDM_CANVAS_SLEEP		2321
#define IDM_CANVAS_MAP        2322
#define IDM_CANVAS_KEYBOARD   2323
#define IDM_CANVAS_REDRAW     2324
#define IDM_CANVAS_MAP16      2325
#define IDM_CANVAS_MAP32      2326
#define IDM_CANVAS_KEYBOARD_A   2327
#define IDM_CANVAS_KEYBOARD_B   2328
#define IDM_CANVAS_KEYBOARD_C   2329
static void InitCanvasWnd(HWND hwnd)
{
	WNDCLASS wndclass ;
	hCanvasWnd = 0;
	wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc   = CanvasWndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInst ;
	wndclass.hIcon         = NULL;
	wndclass.hCursor       = LoadCursor(0,IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH)(COLOR_SCROLLBAR + 1);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = CanvasWndClassName;
	RegisterClass(&wndclass);
   hCanvasWnd = CreateWindowEx(0,CanvasWndClassName,0,WS_CHILD,CW_USEDEFAULT,
   	0,CANVAS_XMAX,CANVAS_YMAX,GetDlgItem(hwnd,IDC_RECT_CANVAS),0,hInst,0);
	CanvasInit(hCanvasWnd);
	CanvasReset();
	CanvasClear();
	CanvasReadPanelInfo(1);
	CanvasUpdate(1);
	CanvasPaintAll();
	Canvas.hPopupMenuKeyboard = CreatePopupMenu();
	AppendMenu(Canvas.hPopupMenuKeyboard,MF_STRING,IDM_CANVAS_KEYBOARD_A,"A Part");
	AppendMenu(Canvas.hPopupMenuKeyboard,MF_STRING,IDM_CANVAS_KEYBOARD_B,"B Part");
	Canvas.hPopupMenu = CreatePopupMenu();
	AppendMenu(Canvas.hPopupMenu,MF_STRING,IDM_CANVAS_SLEEP,"Sleep Mode");
	AppendMenu(Canvas.hPopupMenu,MF_STRING,IDM_CANVAS_MAP16,"Map16 Mode");
	AppendMenu(Canvas.hPopupMenu,MF_STRING,IDM_CANVAS_MAP32,"Map32 Mode");
//	AppendMenu(Canvas.hPopupMenu,MF_STRING,IDM_CANVAS_KEYBOARD,"Keyboard Mode");
	AppendMenu(Canvas.hPopupMenu,MF_POPUP,(UINT)Canvas.hPopupMenuKeyboard,"Keyboard Mode");
	AppendMenu(Canvas.hPopupMenu,MF_SEPARATOR,0,0);
	AppendMenu(Canvas.hPopupMenu,MF_STRING,IDM_CANVAS_REDRAW,"Redraw");
	ShowWindow(hCanvasWnd,SW_SHOW);
	UpdateWindow(hCanvasWnd);
}

static LRESULT CALLBACK
CanvasWndProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam)
{
	switch (uMess)
	{
		case WM_CREATE:
			break;
		case WM_PAINT:
			CanvasPaintDo();
    		return 0;
		case WM_LBUTTONDBLCLK:
			w32g_send_rc(RC_EXT_MODE_CHANGE, 0);
			break;
      case WM_RBUTTONDOWN:
      {
      	RECT rc;
      	GetWindowRect(Canvas.hwnd,(RECT *)&rc);
			TrackPopupMenu(Canvas.hPopupMenu,TPM_TOPALIGN|TPM_LEFTALIGN,
         	rc.left+(int)LOWORD(lParam),rc.top+(int)HIWORD(lParam),
            0,Canvas.hwnd,NULL);
      }
			break;
		case WM_COMMAND:
 	    	switch (LOWORD(wParam)) {
         case IDM_CANVAS_SLEEP:
         	CanvasChange(CANVAS_MODE_SLEEP);
            break;
         case IDM_CANVAS_MAP16:
				Canvas.MapMode = CMAP_MODE_16;
         	CanvasChange(CANVAS_MODE_MAP);
            break;
         case IDM_CANVAS_MAP32:
				Canvas.MapMode = CMAP_MODE_32;
         	CanvasChange(CANVAS_MODE_MAP);
            break;
         case IDM_CANVAS_KEYBOARD:
         	CanvasChange(CANVAS_MODE_KEYBOARD);
            break;
         case IDM_CANVAS_KEYBOARD_A:
				Canvas.CKPart = 1;
         	CanvasChange(CANVAS_MODE_KEYBOARD);
            break;
         case IDM_CANVAS_KEYBOARD_B:
				Canvas.CKPart = 2;
         	CanvasChange(CANVAS_MODE_KEYBOARD);
            break;
         case IDM_CANVAS_REDRAW:
//				PanelResetPart(PANELRESET_CHANNEL);
				CanvasReset();
			  	CanvasClear();
				CanvasReadPanelInfo(1);
				CanvasUpdate(1);
				CanvasPaintAll();
            break;
			}
			break;
		default:
			return DefWindowProc(hwnd,uMess,wParam,lParam) ;
	}
	return 0L;
}

// Color
#define CCR_FORE	 	RGB(0x00,0x00,0x00)
#define CCR_BACK 		RGB(0x00,0xf0,0x00)
#define CCR_DFORE	 	RGB(0x70,0x00,0x00)
#define CCR_DBACK		RGB(0x00,0xf0,0x00)
#define CCR_LOW		RGB(0x80,0xd0,0x00)
#define CCR_MIDDLE	RGB(0xb0,0xb0,0x00)
#define CCR_HIGH		RGB(0xe0,0x00,0x00)
// 色を m : n で混ぜる
static COLORREF HalfColorMN(COLORREF fc, COLORREF bc, int m, int n)
{
	return fc*m/(m+n) + bc*n/(m+n);
}
static COLORREF HalfColor23(COLORREF fc, COLORREF bc)
{
	return HalfColorMN(fc,bc,2,3);
}
static COLORREF HalfColor15(COLORREF fc, COLORREF bc)
{
	return HalfColorMN(fc,bc,1,5);
}
#define CC_BACK				0
#define CC_FORE				1
#define CC_LOW					2
#define CC_MIDDLE				3
#define CC_HIGH				4
#define CC_FORE_HALF			11
#define CC_LOW_HALF			12
#define CC_MIDDLE_HALF		13
#define CC_HIGH_HALF			14
#define CC_FORE_WEAKHALF	15
#define CC_DBACK				21
#define CC_DFORE				22
#define CC_DFORE_HALF		23
#define CC_DFORE_WEAKHALF	24
static COLORREF CanvasColor(int c)
{
  switch(c){
  	case CC_BACK:
			return CCR_BACK;
  	case CC_FORE:
			return CCR_FORE;
  	case CC_FORE_HALF:
			return HalfColor15(CCR_FORE,CCR_BACK);
  	case CC_FORE_WEAKHALF:
			return HalfColor23(CCR_FORE,CCR_BACK);
  	case CC_DBACK:
			return CCR_DBACK;
  	case CC_DFORE:
			return CCR_DFORE;
  	case CC_DFORE_HALF:
			return HalfColor15(CCR_DFORE,CCR_DBACK);
  	case CC_DFORE_WEAKHALF:
			return HalfColor23(CCR_DFORE,CCR_DBACK);
  	case CC_LOW:
			return CCR_LOW;
  	case CC_MIDDLE:
			return CCR_MIDDLE;
  	case CC_HIGH:
			return CCR_HIGH;
  	case CC_LOW_HALF:
			return HalfColor23(CCR_LOW,CCR_BACK);
  	case CC_MIDDLE_HALF:
			return HalfColor23(CCR_MIDDLE,CCR_BACK);
  	case CC_HIGH_HALF:
			return HalfColor23(CCR_HIGH,CCR_BACK);
		default:
			return CCR_BACK;
	}
}

static int CanvasOK = 0;
static void CanvasInit(HWND hwnd)
{
	RECT rc;
	Canvas.hwnd = hwnd;
	Canvas.hParentWnd = GetParent(Canvas.hwnd);
	GetClientRect(Canvas.hParentWnd,&rc);
	MoveWindow(Canvas.hwnd,0,0,rc.right-rc.left,rc.bottom-rc.top,FALSE);
	GetClientRect(Canvas.hwnd,(RECT *)&(Canvas.rcMe));
	Canvas.hdc = GetDC(Canvas.hwnd);
	Canvas.hbitmap = CreateCompatibleBitmap(Canvas.hdc,CANVAS_XMAX,CANVAS_YMAX);
	Canvas.hmdc = CreateCompatibleDC(Canvas.hdc);
	ReleaseDC(Canvas.hwnd,Canvas.hdc);
	Canvas.hBitmapSleep = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BITMAP_SLEEP));
	Canvas.hgdiobj_hmdcprev = SelectObject(Canvas.hmdc,Canvas.hbitmap);

	SetRect((RECT *)&(Canvas.rcSleep),0,0,96,64);
	SetRect((RECT *)&(Canvas.rcMap),3,2+2,0,0);
	SetRect((RECT *)&(Canvas.rcKeyboard),1,1,0,0);
   Canvas.rcMapMap.left = Canvas.rcMap.left;
   Canvas.rcMapMap.top = Canvas.rcMap.top;
   Canvas.rcMapMap.right = Canvas.rcMapMap.left + 6*16 - 1;
   Canvas.rcMapMap.bottom = Canvas.rcMapMap.top + 3*16 - 1;
   Canvas.rcMapSub.left = Canvas.rcMapMap.left;
   Canvas.rcMapSub.top = Canvas.rcMapMap.bottom + 2;
   Canvas.rcMapSub.right = Canvas.rcMapSub.left + 6*16 - 1;
   Canvas.rcMapSub.bottom = Canvas.rcMapSub.top +4+3+3+3+4 - 1;
	Canvas.MapDelay = 1;
	Canvas.MapResidual = 0;
	Canvas.MapMode = CMAP_MODE_16;
//	Canvas.MapMode = CMAP_MODE_32;
	Canvas.MapBarMax = 16;
//	Canvas.CKNoteFrom = 24;
//   Canvas.CKNoteTo = 24 + 96;
	Canvas.CKNoteFrom = 12;
   Canvas.CKNoteTo = Canvas.CKNoteFrom + 96 + 3;
	Canvas.CKCh = 16;
	Canvas.CKPart = 1;
	Canvas.UpdateAll = 0;
	Canvas.Mode = CANVAS_MODE_SLEEP;
	Canvas.PaintDone = 0;
	CanvasReset();
	CanvasOK = 1;
}

// Canvas Map

static void CanvasMapClear(void)
{
	HPEN hPen;
	HBRUSH hBrush;
	HGDIOBJ hgdiobj_hpen, hgdiobj_hbrush;
	if(!CanvasOK)
   	return;
	hPen = CreatePen(PS_SOLID,1,CanvasColor(CC_BACK));
	hBrush = CreateSolidBrush(CanvasColor(CC_BACK));
	hgdiobj_hpen = SelectObject(Canvas.hmdc, hPen);
	hgdiobj_hbrush = SelectObject(Canvas.hmdc, hBrush);
	Rectangle(Canvas.hmdc,
   	Canvas.rcMe.left,Canvas.rcMe.top,Canvas.rcMe.right,Canvas.rcMe.bottom);
	SelectObject(Canvas.hmdc, hgdiobj_hpen);
	DeleteObject(hPen);
	SelectObject(Canvas.hmdc, hgdiobj_hbrush);
	DeleteObject(hBrush);
	InvalidateRect(hCanvasWnd, NULL, FALSE);
}

static void CanvasMapReset(void)
{
	int i,j,ch;
	if(!CanvasOK)
   	return;
	switch(Canvas.MapMode){
   case CMAP_MODE_32:
		Canvas.MapCh = 32;
#ifdef MAPBAR_LIKE_TIMIDI
		Canvas.MapBarWidth = 5;
		Canvas.MapBarMax = 10;
		Canvas.rcMapMap.bottom = Canvas.rcMapMap.top + 3*Canvas.MapBarMax*2 + 6 - 1;
#else
  		Canvas.MapBarWidth = 2;
		Canvas.MapBarMax = 16;
		Canvas.rcMapMap.bottom = Canvas.rcMapMap.top + 3*Canvas.MapBarMax - 1;
#endif
		break;
   default:
   case CMAP_MODE_16:
		Canvas.MapCh = 16;
		Canvas.MapBarWidth = 5;
		Canvas.MapBarMax = 16;
		Canvas.rcMapMap.bottom = Canvas.rcMapMap.top + 3*Canvas.MapBarMax - 1;
		break;
	}
	for(i=0;i<Canvas.MapCh;i++){
		for(j=0;j<Canvas.MapBarMax;j++){
    		Canvas.MapMap[i][j] = CC_FORE_HALF;
    		Canvas.MapMapOld[i][j] = -1;
		}
   	Canvas.MapBar[i] = -1;
    	Canvas.MapBarOld[i] = -1;
	}
	for(ch=0;ch<Canvas.MapCh;ch++){
		Canvas.MapPan[ch] = Canvas.MapPanOld[ch] = 2;
		Canvas.MapSustain[ch] = Canvas.MapSustainOld[ch] = 0;
		Canvas.MapExpression[ch] = Canvas.MapExpressionOld[ch] = 0;
		Canvas.MapVolume[ch] = Canvas.MapVolumeOld[ch] = 0;
		Canvas.MapPitchbend[ch] = Canvas.MapPitchbendOld[ch] = 0x2000;
	}
	Canvas.MapResidual = -1;
	Canvas.MapUpdateFlag = 1;
	Canvas.PaintDone = 0;
}

static void CanvasMapReadPanelInfo(int flag)
{
	int ch,v;

	if(!CanvasOK)
		return;
	if(!PInfoOK)
    	return;
   if(Canvas.UpdateAll)
   	flag = 1;
	if(!Panel->changed && !flag)
   	return;
	// Bar
	Canvas.DrumChannel = drumchannels;
	for(ch=0;ch<Canvas.MapCh;ch++){
		Canvas.MapBarOld[ch] = Canvas.MapBar[ch];
		if(Panel->v_flags[ch] == FLAG_NOTE_ON)
#if 0
  			v = Panel->ctotal[ch]/8;
#else
			v = (int) Panel->ctotal[ch] * Canvas.MapBarMax / 128;
#endif
		else
    		v = 0;
		if(v<0) v = 0; else if(v>Canvas.MapBarMax-1) v = Canvas.MapBarMax-1;
#if 0		// 振動
		if(v == Canvas.MapBar[ch]){
			v = v * (rand()%7 + 7) / 10;
      if(v<0)
      	v = 0;
		}
    	{
#else
		if(v != Canvas.MapBar[ch]){
#endif
			// 遅延
	    	if(Canvas.MapDelay){
   	  		int old = Canvas.MapBar[ch];
				if(Canvas.MapBar[ch]<0)
					Canvas.MapBar[ch] = v;
				else
					Canvas.MapBar[ch] = (old*10*1/3 + v*10*2/3)/10;
				if(old == Canvas.MapBar[ch]){
     				if(v>old)
						Canvas.MapBar[ch] = old + 1;
     				else if(v<old)
						Canvas.MapBar[ch] = old - 1;
      		}
			} else
				Canvas.MapBar[ch] = v;
		}
		if(Canvas.MapBarOld[ch] != Canvas.MapBar[ch])
			Canvas.MapResidual = -1;
	}
	// Sub
	if(Canvas.MapMode==CMAP_MODE_16){
		Canvas.MapChChanged = 0;
		for(ch=0;ch<Canvas.MapCh;ch++){
			int changed = 0;
			Canvas.MapPanOld[ch] = Canvas.MapPan[ch];
			Canvas.MapSustainOld[ch] = Canvas.MapSustain[ch];
			Canvas.MapExpressionOld[ch] = Canvas.MapExpression[ch];
			Canvas.MapVolumeOld[ch] = Canvas.MapVolume[ch];
			Canvas.MapPitchbendOld[ch] = Canvas.MapPitchbend[ch];
			if(Panel->channel[ch].panning==NO_PANNING)
         	Canvas.MapPan[ch] = -1;
			else {
				Canvas.MapPan[ch] = (Panel->channel[ch].panning - 64) * 3 /128;
				Canvas.MapPan[ch] = SetValue(Canvas.MapPan[ch],-2,2) + 2;
			}
			if(Panel->channel[ch].sustain)
				Canvas.MapSustain[ch] = 5;
         else
				Canvas.MapSustain[ch] = 0;
			//Canvas.MapSustain[ch] = SetValue(Canvas.MapSustain[ch],0,10);
#if 0
			Canvas.MapExpression[ch] = (Panel->channel[ch].expression * 11) >>8;
#else
		    Canvas.MapExpression[ch] = (Panel->channel[ch].expression * 11) >>7;
#endif
			Canvas.MapExpression[ch] = SetValue(Canvas.MapExpression[ch],0,10);
#if 0
			Canvas.MapVolume[ch] = (Panel->channel[ch].volume * 11) >>8;
#else
			Canvas.MapVolume[ch] = (Panel->channel[ch].volume * 11) >>7;
#endif
			Canvas.MapVolume[ch] = SetValue(Canvas.MapVolume[ch],0,10);
			Canvas.MapPitchbend[ch] = Panel->channel[ch].pitchbend;
			if(Canvas.MapPanOld[ch]!=Canvas.MapPan[ch]) changed = 1;
			if(Canvas.MapSustainOld[ch]!=Canvas.MapSustain[ch]) changed = 1;
			if(Canvas.MapExpressionOld[ch]!=Canvas.MapExpression[ch]) changed = 1;
			if(Canvas.MapVolumeOld[ch]!=Canvas.MapVolume[ch]) changed = 1;
			if(Canvas.MapPitchbendOld[ch]!=Canvas.MapPitchbend[ch]) changed = 1;
			if(changed)
  		    	Canvas.MapChChanged |= 1 << ch;
		}
	}
}

static void CanvasMapDrawMapBar(int flag)
{
	int i,ch;

	if(!CanvasOK)
		return;
   if(Canvas.UpdateAll)
   	flag = 1;
	if(!Canvas.MapResidual && !flag)
   	return;
	Canvas.MapResidual = 0;
	for(ch=0;ch<Canvas.MapCh;ch++){
		int drumflag = IS_SET_CHANNELMASK(Canvas.DrumChannel,ch);
 		char color1,color2,color3;
		if(drumflag){
			color1 = CC_DFORE;
			color2 = CC_DFORE_WEAKHALF;
         color3 = CC_DFORE_HALF;
      } else {
			color1 = CC_FORE;
			color2 = CC_FORE_WEAKHALF;
         color3 = CC_FORE_HALF;
      }
		for(i=0;i<Canvas.MapBarMax;i++){
			int y = Canvas.MapBarMax-1-i;
			Canvas.MapMapOld[ch][y] = Canvas.MapMap[ch][y];
			if(i<=Canvas.MapBar[ch]){
				Canvas.MapMap[ch][y] = color1;
				Canvas.MapResidual = 1;
#if 1		// 残像
			} else if(i<=Canvas.MapBarOld[ch]){
				Canvas.MapMap[ch][y] = color2;
				Canvas.MapResidual = 1;
#endif
    		} else {
				Canvas.MapMap[ch][y] = color3;
			}
		}
	}
	if(Canvas.MapResidual==0)
		Canvas.MapResidual = -1;
}

// #define CMAP_ALL_UPDATE
static void CanvasMapUpdate(int flag)
{
	RECT rc;
	int i,j;
	int change_flag = 0;
	if(!CanvasOK)
   	return;
	CanvasMapDrawMapBar(flag);
   if(Canvas.UpdateAll)
   	flag = 1;
	if(PInfoOK && Canvas.MapMode==CMAP_MODE_16)
   if(flag || Panel->changed){
		int ch;
		for(ch=0;ch<Canvas.MapCh;ch++){
			int x,y;
         COLORREF color;
			COLORREF colorFG, colorBG;
			int drumflag = IS_SET_CHANNELMASK(Canvas.DrumChannel,ch);
			if(!flag && !(Canvas.MapChChanged & (1<<ch)))
         	continue;
			if(drumflag){
				colorFG = CanvasColor(CC_DFORE);
				colorBG = CanvasColor(CC_DFORE_HALF);
      	} else {
				colorFG = CanvasColor(CC_FORE);
				colorBG = CanvasColor(CC_FORE_HALF);
      	}
         rc.left = Canvas.rcMapSub.left + (5 + 1) * ch;
         rc.right = rc.left + 5 - 1;
			// PAN
         rc.top = Canvas.rcMapSub.top;
         rc.bottom = rc.top + 3 - 1;
			for(x=rc.left;x<=rc.right;x++)
				for(y=rc.top;y<=rc.bottom;y++)
        			SetPixelV(Canvas.hmdc,x,y,colorBG);
			if(Canvas.MapPan[ch]>=0){
				x = rc.left + Canvas.MapPan[ch];
				for(y=rc.top;y<=rc.bottom;y++)
     				SetPixelV(Canvas.hmdc,x,y,colorFG);
			}
         // SUSTAIN
         rc.top = rc.bottom + 2;
         rc.bottom = rc.top + 2 - 1;
         if(Canvas.MapSustain[ch])
				for(x=rc.left;x<=rc.right;x++)
					for(y=rc.top;y<=rc.bottom;y++)
        				SetPixelV(Canvas.hmdc,x,y,colorFG);
			else
				for(x=rc.left;x<=rc.right;x++)
					for(y=rc.top;y<=rc.bottom;y++)
        				SetPixelV(Canvas.hmdc,x,y,colorBG);
         // EXPRESSION
         rc.top = rc.bottom + 2;
         rc.bottom = rc.top + 2 - 1;
			for(i=1;i<=10;i++){
         		if(i <= Canvas.MapExpression[ch])
            	color = colorFG;
            else
            	color = colorBG;
			x = rc.left + (i-1)/2;
			y = rc.top + (i+1)%2;
  				SetPixelV(Canvas.hmdc,x,y,color);
			}
         // VOLUME
         rc.top = rc.bottom + 2;
         rc.bottom = rc.top + 2 - 1;
			for(i=1;i<=10;i++){
        		if(i <= Canvas.MapVolume[ch])
            	color = colorFG;
            else
            	color = colorBG;
            x = rc.left + (i-1)/2;
				y = rc.top + (i+1)%2;
  				SetPixelV(Canvas.hmdc,x,y,color);
			}
         // PITCH_BEND
         rc.top = rc.bottom + 2;
         rc.bottom = rc.top + 3 - 1;
			for(x=rc.left;x<=rc.right;x++)
				for(y=rc.top;y<=rc.bottom;y++)
        			SetPixelV(Canvas.hmdc,x,y,colorBG);
         if(Canvas.MapPitchbend[ch]==-2){
         	y = rc.top + 1;
				for(x=rc.left;x<=rc.right;x++)
  					SetPixelV(Canvas.hmdc,x,y,colorFG);
				y++;
				for(x=rc.left;x<=rc.right;x++)
  					SetPixelV(Canvas.hmdc,x,y,colorFG);
			} else if(Canvas.MapPitchbend[ch]>0x2000){
         	y = rc.top;
				for(x=rc.left;x<=rc.left;x++)
  					SetPixelV(Canvas.hmdc,x,y,colorFG);
				y++;
				for(x=rc.left;x<=rc.left+2;x++)
  					SetPixelV(Canvas.hmdc,x,y,colorFG);
				y++;
				for(x=rc.left;x<=rc.left+4;x++)
		 			SetPixelV(Canvas.hmdc,x,y,colorFG);
			} else if(Canvas.MapPitchbend[ch]<0x2000){
         	y = rc.top;
				for(x=rc.right;x<=rc.right;x++)
  					SetPixelV(Canvas.hmdc,x,y,colorFG);
				y++;
				for(x=rc.right-2;x<=rc.right;x++)
  					SetPixelV(Canvas.hmdc,x,y,colorFG);
				y++;
				for(x=rc.right-4;x<=rc.right;x++)
		 			SetPixelV(Canvas.hmdc,x,y,colorFG);
         }
      }
  		InvalidateRect(hCanvasWnd,(RECT *)&(Canvas.rcMapSub),FALSE);
   }
	if(!Canvas.MapResidual && !flag)
   	return;
	change_flag = 0;
#ifndef MAPBAR_LIKE_TIMIDI
	for(i=0;i<Canvas.MapCh;i++){
		for(j=0;j<Canvas.MapBarMax;j++){
			if(Canvas.MapMap[i][j]!=Canvas.MapMapOld[i][j] || flag){
				int x,y;
				COLORREF color = CanvasColor(Canvas.MapMap[i][j]);
            rc.left = Canvas.rcMap.left + (Canvas.MapBarWidth + 1) * i;
            rc.right = rc.left -1 + Canvas.MapBarWidth;
            rc.top = Canvas.rcMap.top + (2 + 1) * j;
            rc.bottom = rc.top -1 + 2;
				for(x=rc.left;x<=rc.right;x++)
					for(y=rc.top;y<=rc.bottom;y++)
          			SetPixelV(Canvas.hmdc,x,y,color);
				change_flag = 1;
    		}
   	}
	}
#else
	if(Canvas.MapMode==CMAP_MODE_16)
	for(i=0;i<Canvas.MapCh;i++){
		for(j=0;j<Canvas.MapBarMax;j++){
			if(Canvas.MapMap[i][j]!=Canvas.MapMapOld[i][j] || flag){
				int x,y;
				COLORREF color = CanvasColor(Canvas.MapMap[i][j]);
				rc.left = Canvas.rcMap.left + (Canvas.MapBarWidth + 1) * i;
				rc.right = rc.left -1 + Canvas.MapBarWidth;
				rc.top = Canvas.rcMap.top + (2 + 1) * j;
				rc.bottom = rc.top -1 + 2;
				for(x=rc.left;x<=rc.right;x++)
					for(y=rc.top;y<=rc.bottom;y++)
          			SetPixelV(Canvas.hmdc,x,y,color);
				change_flag = 1;
    		}
   		}
	}
	else
	for(i=0;i<Canvas.MapCh;i++){
		for(j=0;j<Canvas.MapBarMax;j++){
			if(Canvas.MapMap[i][j]!=Canvas.MapMapOld[i][j] || flag){
				int x,y;
				COLORREF color = CanvasColor(Canvas.MapMap[i][j]);
				if(i<=15){
					rc.left = Canvas.rcMap.left + (Canvas.MapBarWidth + 1) * i;
					rc.right = rc.left -1 + Canvas.MapBarWidth;
					rc.top = 3 + Canvas.rcMap.top + (2 + 1) * j;
					rc.bottom = rc.top -1 + 2;
				} else {
					rc.left = Canvas.rcMap.left + (Canvas.MapBarWidth + 1) * (i-16);
					rc.right = rc.left -1 + Canvas.MapBarWidth;
					rc.top = 3 + Canvas.rcMap.top + (2 + 1) * j + Canvas.MapBarMax*(2+1) + 3 ;
					rc.bottom = rc.top -1 + 2;
				}
				for(x=rc.left;x<=rc.right;x++)
					for(y=rc.top;y<=rc.bottom;y++)
	          			SetPixelV(Canvas.hmdc,x,y,color);
				change_flag = 1;
    		}
   		}
	}
#endif
#ifdef CMAP_ALL_UPDATE
	if(change_flag)
		InvalidateRect(hCanvasWnd,NULL,FALSE);
#else
	if(change_flag)
		InvalidateRect(hCanvasWnd,(RECT *)&(Canvas.rcMapMap),FALSE);
#endif
	if(Canvas.UpdateAll)
		InvalidateRect(hCanvasWnd,NULL,FALSE);
	if(Canvas.MapResidual<0)
		Canvas.MapResidual = 0;
}

static void CanvasSleepClear(void)
{
	RECT rc;
	HPEN hPen;
	HBRUSH hBrush;
	HGDIOBJ hgdiobj_hpen, hgdiobj_hbrush;
	hPen = CreatePen(PS_SOLID,1,GetSysColor(COLOR_SCROLLBAR));
	hBrush = CreateSolidBrush(GetSysColor(COLOR_SCROLLBAR));
	hgdiobj_hpen = SelectObject(Canvas.hmdc, hPen);
	hgdiobj_hbrush = SelectObject(Canvas.hmdc, hBrush);
	GetClientRect(Canvas.hwnd, &rc);
	Rectangle(Canvas.hmdc,rc.left,rc.top,rc.right,rc.bottom);
	SelectObject(Canvas.hmdc, hgdiobj_hpen);
	DeleteObject(hPen);
	SelectObject(Canvas.hmdc, hgdiobj_hbrush);
	DeleteObject(hBrush);
	InvalidateRect(hCanvasWnd, NULL, FALSE);
}

static void CanvasSleepReset(void)
{
	Canvas.PaintDone = 0;
}

static void CanvasSleepReadPanelInfo(int flag)
{
}

static int CanvasSleepDone = 0;
static void CanvasSleepUpdate(int flag)
{
	HDC hdc;
	RECT rc;
	int x,y;
	CanvasSleepReset();
   hdc = CreateCompatibleDC(Canvas.hmdc);
	SelectObject(hdc,Canvas.hBitmapSleep);
	GetClientRect(Canvas.hwnd, &rc);
	x = (rc.right - rc.left - Canvas.rcSleep.right)/2;
	y = (rc.bottom - rc.top - Canvas.rcSleep.bottom)/2;
	if(x<0) x = 0;
	if(y<0) y = 0;
	BitBlt(Canvas.hmdc,x,y,Canvas.rcSleep.right,Canvas.rcSleep.bottom,hdc,0,0,SRCCOPY);
	DeleteDC(hdc);
}


// Canvas Keyboard

static void CanvasKeyboardReset(void)
{
	int i,j;
	int ChFrom = (Canvas.CKPart - 1) * Canvas.CKCh;
	int ChTo = Canvas.CKPart * Canvas.CKCh - 1;
	for(i=ChFrom;i<=ChTo;i++){
		for(j=0;j<4;j++){
			Canvas.CKxnote[i][j] = 0;
			Canvas.CKxnote_old[i][j] = 0;
		}
	}
	Canvas.PaintDone = 0;
}

static void CanvasKeyboardReadPanelInfo(int flag)
{
	int i,j;
	int ChFrom, ChTo;
	if(!CanvasOK)
		return;
	if(!PInfoOK)
    	return;
	ChFrom = (Canvas.CKPart - 1) * Canvas.CKCh;
	ChTo = Canvas.CKPart * Canvas.CKCh - 1;
   if(Canvas.UpdateAll)
   	flag = 1;
	if(!Panel->changed && !flag)
   	return;
	for(i=ChFrom;i<=ChTo;i++)
		for(j=0;j<4;j++){
      	Canvas.CKxnote_old[i][j] = Canvas.CKxnote[i][j];
			Canvas.CKxnote[i][j] = Panel->xnote[i][j];
      }
}

#define CK_KEY_BLACK	1
#define CK_KEY_WHITE	2
#define CK_ON RGB(0xff,0x00,0x00)
#define CK_OFF_WHITE RGB(0xff,0xff,0xff)
#define CK_OFF_BLACK RGB(0x00,0x00,0x00)
#define CK_DOFF_WHITE RGB(0xcc,0xcc,0xcc)
#define CK_DOFF_BLACK RGB(0x00,0x00,0x00)
static void CanvasKeyboardUpdate(int flag)
{
	int j,k,l;
   int channel;
	int ChFrom, ChTo;

	if(!PInfoOK)
		return;
   if(Canvas.UpdateAll)
   	flag = 1;
	if(Canvas.DrumChannel!=drumchannels)
   	flag = 1;
	if(!Panel->changed && !flag)
   	return;
	ChFrom = (Canvas.CKPart - 1) * Canvas.CKCh;
	ChTo = Canvas.CKPart * Canvas.CKCh - 1;
	for(channel=ChFrom;channel<=ChTo;channel++){
		int change_flag = 0;
		int drumflag = IS_SET_CHANNELMASK(drumchannels,channel);
		COLORREF colorON, colorOFF_WHITE, colorOFF_BLACK;
		if(drumflag){
			colorON = CK_ON;
         colorOFF_WHITE = CK_DOFF_WHITE;
         colorOFF_BLACK = CK_OFF_BLACK;
      } else {
			colorON = CK_ON;
         colorOFF_WHITE = CK_OFF_WHITE;
         colorOFF_BLACK = CK_OFF_BLACK;
      }
		for(j=0;j<4;j++){
			int32 xnote, xnote_diff;
			xnote = Canvas.CKxnote[channel][j];
        	xnote_diff = Canvas.CKxnote[channel][j] ^ Canvas.CKxnote_old[channel][j];
			if(!flag && xnote_diff == 0)
         	continue;
			for(k=0;k<32;k++){
				int key = 0;
				int KeyOn = 0;
  	   		int note = j*32+k;
            int reff = (int32)1 << k;
				int x,y;
				if(note < Canvas.CKNoteFrom || note > Canvas.CKNoteTo)
      			continue;
            if(!flag && !(xnote_diff & reff))
              	continue;
            if(xnote & reff)
					KeyOn = 1;
				note = note % 12;
            if(note == 1 || note == 3 || note == 6 || note == 8 || note == 10)
			 		key = CK_KEY_BLACK;
            else
			 		key = CK_KEY_WHITE;
     			x = Canvas.rcKeyboard.left + j * 32 + k - Canvas.CKNoteFrom;
   			y = Canvas.rcKeyboard.top + (channel - ChFrom) * 4;
				switch(key){
            case CK_KEY_BLACK:
	            if(KeyOn){
						for(l=0;l<2;l++)
      	  				SetPixelV(Canvas.hmdc,x,y+l,colorON);
  						SetPixelV(Canvas.hmdc,x,y+2,colorOFF_WHITE);
	            } else {
						for(l=0;l<2;l++)
      	  				SetPixelV(Canvas.hmdc,x,y+l,colorOFF_BLACK);
  						SetPixelV(Canvas.hmdc,x,y+2,colorOFF_WHITE);
         	   }
					break;
            case CK_KEY_WHITE:
	            if(KeyOn){
  						SetPixelV(Canvas.hmdc,x,y,colorOFF_WHITE);
						for(l=1;l<3;l++)
      	  				SetPixelV(Canvas.hmdc,x,y+l,colorON);
	            } else {
  						SetPixelV(Canvas.hmdc,x,y,colorOFF_WHITE);
						for(l=1;l<3;l++)
      	  				SetPixelV(Canvas.hmdc,x,y+l,colorOFF_WHITE);
         	   }
					break;
				default:
            	break;
	  			}
            change_flag = 1;
      	}
		}
		if(change_flag){
         RECT rc;
			GetClientRect(Canvas.hwnd,&rc);
         rc.top = Canvas.rcKeyboard.top + (channel - ChFrom) * 4;
			rc.bottom = rc.top + 4;
       	InvalidateRect(Canvas.hwnd, &rc, FALSE);
		}
	}
	if(flag)
		InvalidateRect(hCanvasWnd, NULL, FALSE);
	Canvas.DrumChannel = drumchannels;
}

static void CanvasKeyboardClear(void)
{
	int i;
	HPEN hPen;
	HBRUSH hBrush;
	HGDIOBJ hgdiobj_hpen, hgdiobj_hbrush;
	COLORREF FGcolor, BGcolor;
	HFONT hfont;
   HGDIOBJ hgdiobj;
	RECT rc;
	char buffer[16];
	if(!CanvasOK)
   	return;
#if 0
	hPen = CreatePen(PS_SOLID,1,CanvasColor(CC_BACK));
	hBrush = CreateSolidBrush(CanvasColor(CC_BACK));
#else
	FGcolor = RGB(0xff,0xff,0xff);
   BGcolor = RGB(0x00,0x00,0x00);
	hPen = CreatePen(PS_SOLID,1,BGcolor);
	hBrush = CreateSolidBrush(BGcolor);
#endif
	hgdiobj_hpen = SelectObject(Canvas.hmdc, hPen);
	hgdiobj_hbrush = SelectObject(Canvas.hmdc, hBrush);
	Rectangle(Canvas.hmdc,
   	Canvas.rcMe.left,Canvas.rcMe.top,Canvas.rcMe.right,Canvas.rcMe.bottom);
	SelectObject(Canvas.hmdc, hgdiobj_hpen);
	DeleteObject(hPen);
	SelectObject(Canvas.hmdc, hgdiobj_hbrush);
	DeleteObject(hBrush);

	hfont = CreateFont(7,7,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,
		DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
     	DEFAULT_PITCH | FF_DONTCARE,"Arial Bold");
	hgdiobj = SelectObject(Canvas.hmdc,hfont);
   SetTextAlign(Canvas.hmdc, TA_LEFT | TA_TOP | TA_NOUPDATECP);
	rc.left =	Canvas.rcMe.left+1 ;
	rc.top =		Canvas.rcMe.bottom-7;
	rc.right =	Canvas.rcMe.left+1 + 40;
	rc.bottom =	Canvas.rcMe.bottom;
	SetTextColor(Canvas.hmdc,RGB(0xff,0xff,0xff));
	SetBkColor(Canvas.hmdc,RGB(0x00,0x00,0x00));
	strcpy(buffer,"Part");
 	ExtTextOut(Canvas.hmdc,rc.left,rc.top,ETO_CLIPPED|ETO_OPAQUE,&rc,
 		buffer,strlen(buffer),NULL);
	for(i=1;i<=3;i++){
		if(i==Canvas.CKPart){
   		SetTextColor(Canvas.hmdc,RGB(0xff,0xff,0xff));
   		SetBkColor(Canvas.hmdc,RGB(0x00,0x00,0x00));
      } else {
   		SetTextColor(Canvas.hmdc,RGB(0x80,0x80,0x80));
   		SetBkColor(Canvas.hmdc,RGB(0x00,0x00,0x00));
		}
		rc.left =	Canvas.rcMe.left+1 + 40 + (i-1)*13;
		rc.top =		Canvas.rcMe.bottom-7;
		rc.right =	Canvas.rcMe.left+1 + 40 + (i)*13 - 1;
		rc.bottom =	Canvas.rcMe.bottom;
		sprintf(buffer,"%c",i+'A'-1);
 	 	ExtTextOut(Canvas.hmdc,rc.left,rc.top,ETO_CLIPPED|ETO_OPAQUE,&rc,
   		buffer,strlen(buffer),NULL);
	}
   if((HGDIOBJ)hgdiobj!=(HGDIOBJ)NULL && (HGDIOBJ)hgdiobj!=(HGDIOBJ)GDI_ERROR)
   	SelectObject(Canvas.hmdc,hgdiobj);
	DeleteObject(hfont);

	CanvasKeyboardReset();
	CanvasKeyboardReadPanelInfo(1);
	CanvasKeyboardUpdate(1);
	InvalidateRect(hCanvasWnd, NULL, FALSE);
}

// Canvas All

static void CanvasPaintDo(void)
{
	PAINTSTRUCT ps;
	RECT rc;
	Canvas.hdc = BeginPaint(Canvas.hwnd, &ps);
	GetClientRect(Canvas.hwnd, &rc);
	BitBlt(Canvas.hdc,rc.left,rc.top,rc.right,rc.bottom,Canvas.hmdc,0,0,SRCCOPY);
	EndPaint(Canvas.hwnd, &ps);
}
void CanvasPaint(void)
{
	Canvas.PaintDone = 0;
	UpdateWindow(hCanvasWnd);
}
void CanvasPaintAll(void)
{
	InvalidateRect(hCanvasWnd, NULL, FALSE);
	CanvasPaint();
}

void CanvasReset(void)
{
	if(!CanvasOK)
   	return;
	switch(Canvas.Mode){
		case CANVAS_MODE_MAP:
			CanvasMapReset();
			break;
		case CANVAS_MODE_KEYBOARD:
			CanvasKeyboardReset();
			break;
		default:
		case CANVAS_MODE_SLEEP:
			CanvasSleepReset();
      break;
  }
}

void CanvasClear(void)
{
	if(!CanvasOK)
   	return;
	switch(Canvas.Mode){
		case CANVAS_MODE_MAP:
			CanvasMapClear();
			break;
		case CANVAS_MODE_KEYBOARD:
			CanvasKeyboardClear();
			break;
		default:
		case CANVAS_MODE_SLEEP:
			CanvasSleepClear();
      break;
  }
}

void CanvasUpdate(int flag)
{
	if(!CanvasOK)
   	return;
	switch(Canvas.Mode){
		case CANVAS_MODE_MAP:
			CanvasMapUpdate(flag);
			break;
		case CANVAS_MODE_KEYBOARD:
			CanvasKeyboardUpdate(flag);
		break;
		default:
		case CANVAS_MODE_SLEEP:
			CanvasSleepUpdate(flag);
      break;
  }
}

void CanvasReadPanelInfo(int flag)
{
	if(!CanvasOK)
   	return;
	switch(Canvas.Mode){
		case CANVAS_MODE_MAP:
			CanvasMapReadPanelInfo(flag);
			break;
		case CANVAS_MODE_KEYBOARD:
			CanvasKeyboardReadPanelInfo(flag);
		break;
		default:
		case CANVAS_MODE_SLEEP:
//			CanvasSleepReadPanelInfo(flag);
      break;
  }
}

void CanvasChange(int mode)
{
	if(mode!=0)
   	Canvas.Mode = mode;
	else {
		if(Canvas.Mode==CANVAS_MODE_SLEEP)
   		Canvas.Mode = CANVAS_MODE_MAP;
		else if(Canvas.Mode==CANVAS_MODE_MAP)
   		Canvas.Mode = CANVAS_MODE_KEYBOARD;
		else if(Canvas.Mode==CANVAS_MODE_KEYBOARD)
   		Canvas.Mode = CANVAS_MODE_SLEEP;
		else
   		Canvas.Mode = CANVAS_MODE_SLEEP;
	}
	CanvasReset();
	CanvasClear();
	CanvasReadPanelInfo(1);
	CanvasUpdate(1);
	CanvasPaintAll();
}

int CanvasGetMode(void)
{
	return Canvas.Mode;
}































//-----------------------------------------------------------------------------
// Main Panel
//  メインパネルウインドウ関連
//
//
//
//
//
//
//

#define MPANEL_XMAX 440
#define MPANEL_YMAX 88

// update flag.
#define MP_UPDATE_ALL		0xffffL
#define MP_UPDATE_NONE		0x0000L
#define MP_UPDATE_TITLE		0x0001L
#define MP_UPDATE_FILE		0x0002L
#define MP_UPDATE_TIME			0x0004L
#define MP_UPDATE_VOICES		0x0010L
#define MP_UPDATE_MVOLUME		0x0020L
#define MP_UPDATE_RATE			0x0040L
#define MP_UPDATE_PLAYLIST		0x0080L
#define MP_UPDATE_MISC		0x0200L
#define MP_UPDATE_MESSAGE	0x0400L
#define MP_UPDATE_BACKGROUND	0x0800L

#define MP_TITLE_MAX	256
#define MP_FILE_MAX		256
struct MPanel_ {
	HWND hwnd;
	HWND hParentWnd;
	HDC hdc;
   HDC hmdc;
	HGDIOBJ hgdiobj_hmdcprev;
	HBITMAP hbitmap;
	HBITMAP hbitmapBG;			/* the background bitmap */
	HBITMAP hbitmapBGFilter;	/* the background bitmap filter */
	HFONT hfont;
	char Font[256];
	char FontLang[256];
	RECT rcMe;
	RECT rcTitle;
	RECT rcFile;
	RECT rcTime;
	RECT rcVoices;
	RECT rcMVolume;
	RECT rcRate;
	RECT rcList;
	RECT rcMisc;
	RECT rcMessage;
	char Title[MP_TITLE_MAX+1];
	char File[MP_FILE_MAX+1];
	int CurTime_h; int CurTime_m; int CurTime_s; int CurTime_ss;
	int TotalTime_h; int TotalTime_m; int TotalTime_s; int TotalTime_ss;
	int CurVoices;
	int MaxVoices;
   int MVolume;
	int Rate;
	int PlaylistNum;
	int PlaylistMax;
	HFONT hFontTitle;
	HFONT hFontFile;
	HFONT hFontTime;
	HFONT hFontVoices;
	HFONT hFontMVolume;
	HFONT hFontRate;
	HFONT hFontList;
	HFONT hFontMisc;
	HFONT hFontMessage;
	long UpdateFlag;
	COLORREF FGColor;
	COLORREF BGColor;
	enum play_system_modes play_system_mode;
	int current_file_info_file_type;
	int current_file_info_max_channel;
} MPanel;
extern volatile int MPanelOK;



static HWND hPanelWnd;
static char PanelWndClassName[] = "TiMidity Main Panel";
static LRESULT CALLBACK PanelWndProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam);
int MPanelMode = 0;

static void InitPanelWnd(HWND hwnd)
{
	WNDCLASS wndclass ;
	hPanelWnd = 0;
	wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc   = PanelWndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInst ;
	wndclass.hIcon         = NULL;
	wndclass.hCursor       = LoadCursor(0,IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH)(COLOR_SCROLLBAR + 1);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = PanelWndClassName;
	RegisterClass(&wndclass);
  	hPanelWnd =
  	CreateWindowEx(0,PanelWndClassName,0,WS_CHILD,
    	CW_USEDEFAULT,0,MPANEL_XMAX,MPANEL_YMAX,
      GetDlgItem(hwnd,IDC_RECT_PANEL),0,hInst,0);
	MPanelInit(hPanelWnd);
	MPanelReset();
	MPanelReadPanelInfo(1);
	MPanelUpdateAll();
  	MPanelPaintAll();
	UpdateWindow(hPanelWnd);
	ShowWindow(hPanelWnd,SW_SHOW);
}

static void MPanelPaintDo(void);
static LRESULT CALLBACK
PanelWndProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam)
{
	switch (uMess)
	{
		case WM_CREATE:
			break;
		case WM_PAINT:
      	MPanelPaintDo();
	    	return 0;
		case WM_LBUTTONDBLCLK:
//			MPanelReset();
		  	MPanelReadPanelInfo(1);
			MPanelUpdateAll();
		  	MPanelPaintAll();
			break;
		default:
			return DefWindowProc(hwnd,uMess,wParam,lParam) ;
	}
	return 0L;
}

// Initialization of MPanel strucuter at once.
volatile int MPanelOK = 0;
static void MPanelInit(HWND hwnd)
{
	RECT rc;
	MPanel.hwnd = hwnd;
	MPanel.hParentWnd = GetParent(MPanel.hwnd);
	GetClientRect(MPanel.hParentWnd,&rc);
	MoveWindow(MPanel.hwnd,0,0,rc.right-rc.left,rc.bottom-rc.top,FALSE);
	MPanel.hdc = GetDC(MPanel.hwnd);
	MPanel.hbitmap =
  		CreateCompatibleBitmap(MPanel.hdc,MPANEL_XMAX,MPANEL_YMAX);
	MPanel.hmdc =
    	CreateCompatibleDC(MPanel.hdc);
	MPanel.hgdiobj_hmdcprev = SelectObject(MPanel.hmdc,MPanel.hbitmap);
	ReleaseDC(MPanel.hwnd,MPanel.hdc);

	GetClientRect(MPanel.hwnd,&rc);
	// RECT reft,top,right,bottom
	SetRect(&(MPanel.rcMe),rc.left,rc.top,rc.right,rc.bottom);
	rc = MPanel.rcMe;
	SetRect(&(MPanel.rcTitle),	rc.left+2,	rc.top+2,				rc.right-2,				rc.top+2+14);
	SetRect(&(MPanel.rcFile),	rc.left+2,	rc.top+2+14+1,		rc.right-2,				rc.top+2+14+1+12);
	SetRect(&(MPanel.rcTime),	rc.left+2,	rc.top+2+14+1+12+1,	rc.left+2+180,		rc.top+2+14+1+12+1+25);
	SetRect(&(MPanel.rcVoices), rc.right-2-36-1-24-48,	rc.top+2+14+1+12+1,	rc.right-2-36-1-24,	rc.top+2+14+1+12+1+12);
	SetRect(&(MPanel.rcMVolume),rc.right-2-36,			rc.top+2+14+1+12+1,	rc.right-2,				rc.top+2+14+1+12+1+12);
	SetRect(&(MPanel.rcRate), 	rc.right-2-60-1-18-48,	rc.top+2+14+1+12+1+12+1,	rc.right-2-60-1-18,	rc.top+2+14+1+12+1+12+1+12);
	SetRect(&(MPanel.rcList),	rc.right-2-60,				rc.top+2+14+1+12+1+12+1,	rc.right-2,				rc.top+2+14+1+12+1+12+1+12);
	SetRect(&(MPanel.rcMisc),	rc.left+2,	rc.top+2+14+1+12+1+25+1,rc.right-2,rc.top+2+14+1+12+1+25+1+12);
	SetRect(&(MPanel.rcMessage),rc.left,rc.top,rc.right,rc.bottom);
	MPanel.hFontTitle = NULL;
	MPanel.hFontFile = NULL;
	MPanel.hFontTime = NULL;
	MPanel.hFontVoices = NULL;
	MPanel.hFontMVolume = NULL;
	MPanel.hFontRate = NULL;
	MPanel.hFontList = NULL;
//	strcpy(MPanel.Font,"Times New Roman");
	strcpy(MPanel.Font,"Arial Bold");
	switch(PlayerLanguage){
   case LANGUAGE_ENGLISH:
		strcpy(MPanel.FontLang,"Times New Roman");
     	break;
	default:
	case LANGUAGE_JAPANESE:
		strcpy(MPanel.FontLang,"ＭＳ Ｐ明朝");
		break;
	}
	rc = MPanel.rcTitle;
	MPanel.hFontTitle =
		CreateFont(rc.bottom-rc.top,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,
			DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
      	DEFAULT_PITCH | FF_DONTCARE,MPanel.FontLang);
	rc = MPanel.rcFile;
	MPanel.hFontFile =
		CreateFont(rc.bottom-rc.top,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,
			DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
      	DEFAULT_PITCH | FF_DONTCARE,MPanel.FontLang);
	rc = MPanel.rcTime;
	MPanel.hFontTime =
		CreateFont(24,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,
			DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
      	DEFAULT_PITCH | FF_DONTCARE,MPanel.Font);
	rc = MPanel.rcVoices;
	MPanel.hFontVoices =
		CreateFont(rc.bottom-rc.top,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,
			DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
      	DEFAULT_PITCH | FF_DONTCARE,MPanel.Font);
	rc = MPanel.rcMVolume;
	MPanel.hFontMVolume =
		CreateFont(rc.bottom-rc.top,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,
			DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
      	DEFAULT_PITCH | FF_DONTCARE,MPanel.Font);
	rc = MPanel.rcRate;
	MPanel.hFontRate =
		CreateFont(rc.bottom-rc.top,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,
			DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
      	DEFAULT_PITCH | FF_DONTCARE,MPanel.Font);
	rc = MPanel.rcList;
	MPanel.hFontList =
		CreateFont(rc.bottom-rc.top,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,
			DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
      	DEFAULT_PITCH | FF_DONTCARE,MPanel.Font);
	rc = MPanel.rcMisc;
	MPanel.hFontMisc =
#if 0
		CreateFont(rc.bottom-rc.top,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,
			DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
      	DEFAULT_PITCH | FF_DONTCARE,MPanel.Font);
#else
		CreateFont(rc.bottom-rc.top,(rc.bottom-rc.top)/2,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,
			DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
      	FIXED_PITCH | FF_DONTCARE,MPanel.Font);
#endif
	MPanelOK = 1;
}

// パネル構造体をリセットする。
void MPanelReset(void)
{
	if(!MPanelOK)
		return;
	MPanel.Title[0] = '\0';
	MPanel.File[0] = '\0';
	strcpy(MPanel.Title,"No title.");
	strcpy(MPanel.File,"No file.");
	MPanel.CurTime_h = 0;
	MPanel.CurTime_m = 0;
	MPanel.CurTime_s = 0;
	MPanel.CurTime_ss = 0;
	MPanel.TotalTime_h = 0;
	MPanel.TotalTime_m = 0;
	MPanel.TotalTime_s = 0;
	MPanel.TotalTime_ss = 0;
	MPanel.CurVoices = 0;
	MPanel.MaxVoices = 0;
	MPanel.MVolume = 0;
	MPanel.Rate = 0;
	MPanel.PlaylistNum = 0;
	MPanel.PlaylistMax = 0;
	MPanel.UpdateFlag = MP_UPDATE_ALL;
//	MPanel.FGColor = RGB(0x00,0x00,0x00);
//	MPanel.BGColor = RGB(0xff,0xff,0xff);
	MPanel.FGColor = RGB(0x00,0x00,0x00);
	MPanel.BGColor = RGB(0xc0,0xc0,0xc0);
#if 0
	if(MPanel.hFontTitle!=NULL)
   	DeleteObject(MPanel.hFontTitle);
	if(MPanel.hFontFile!=NULL)
   	DeleteObject(MPanel.hFontFile);
	if(MPanel.hFontTime!=NULL)
   	DeleteObject(MPanel.hFontTime);
	if(MPanel.hFontVoices!=NULL)
   	DeleteObject(MPanel.hFontVoices);
	if(MPanel.hFontMVolume!=NULL)
   	DeleteObject(MPanel.hFontMVolume);
	if(MPanel.hFontRate!=NULL)
   	DeleteObject(MPanel.hFontRate);
	if(MPanel.hFontList!=NULL)
   	DeleteObject(MPanel.hFontList);
#endif
	MPanel.play_system_mode = DEFAULT_SYSTEM_MODE;
	MPanel.current_file_info_file_type = IS_OTHER_FILE;
	MPanel.current_file_info_max_channel = -1;
}

// パネル構造体を元に更新する。
void MPanelUpdate(void)
{
	if(!MPanelOK)
		return;
	if(MPanel.UpdateFlag==MP_UPDATE_NONE)
   	return;
	if(MPanel.UpdateFlag & MP_UPDATE_BACKGROUND){
		// ビットマップを貼り付けるが今は塗りつぶし。
		HPEN hPen;
      HBRUSH hBrush;
		COLORREF color = MPanel.FGColor;
		RECT rc = MPanel.rcMe;
		HGDIOBJ hgdiobj_hpen, hgdiobj_hbrush;
		hPen = CreatePen(PS_SOLID,1,color);
		hBrush = CreateSolidBrush(color);
		hgdiobj_hpen = SelectObject(MPanel.hmdc, hPen);
		hgdiobj_hbrush = SelectObject(MPanel.hmdc, hBrush);
		Rectangle(MPanel.hmdc,rc.left,rc.top,rc.right,rc.bottom);
		SelectObject(MPanel.hmdc, hgdiobj_hpen);
		DeleteObject(hPen);
		SelectObject(MPanel.hmdc, hgdiobj_hbrush);
		DeleteObject(hBrush);
		InvalidateRect(MPanel.hwnd,&rc, FALSE);
   }
	if(MPanel.UpdateFlag & MP_UPDATE_TITLE){
		HGDIOBJ hgdiobj = SelectObject(MPanel.hmdc,MPanel.hFontTitle);
		SetTextColor(MPanel.hmdc,MPanel.FGColor);
		SetBkColor(MPanel.hmdc,MPanel.BGColor);
	//#include "w32g2_c.h"
	SetTextAlign(MPanel.hmdc, TA_LEFT | TA_TOP | TA_NOUPDATECP);
		ExtTextOut(MPanel.hmdc,MPanel.rcTitle.left,MPanel.rcTitle.top,
    		ETO_CLIPPED	| ETO_OPAQUE,&(MPanel.rcTitle),
    		MPanel.Title,strlen(MPanel.Title),NULL);
		if((HGDIOBJ)hgdiobj!=(HGDIOBJ)NULL && (HGDIOBJ)hgdiobj!=(HGDIOBJ)GDI_ERROR)
			SelectObject(MPanel.hmdc,hgdiobj);
		InvalidateRect(hPanelWnd, &(MPanel.rcTitle), FALSE);
   }
	if(MPanel.UpdateFlag & MP_UPDATE_FILE){
		HGDIOBJ hgdiobj = SelectObject(MPanel.hmdc,MPanel.hFontFile);
		SetTextColor(MPanel.hmdc,MPanel.FGColor);
		SetBkColor(MPanel.hmdc,MPanel.BGColor);
		SetTextAlign(MPanel.hmdc, TA_LEFT | TA_TOP | TA_NOUPDATECP);
		ExtTextOut(MPanel.hmdc,MPanel.rcFile.left,MPanel.rcFile.top,
    		ETO_CLIPPED	| ETO_OPAQUE,&(MPanel.rcFile),
    		MPanel.File,strlen(MPanel.File),NULL);
		if((HGDIOBJ)hgdiobj!=(HGDIOBJ)NULL && (HGDIOBJ)hgdiobj!=(HGDIOBJ)GDI_ERROR)
			SelectObject(MPanel.hmdc,hgdiobj);
		InvalidateRect(hPanelWnd, &(MPanel.rcFile), FALSE);
   }
	if(MPanel.UpdateFlag & MP_UPDATE_TIME){
		char buffer[256];
		HGDIOBJ hgdiobj = SelectObject(MPanel.hmdc,MPanel.hFontTime);
   	sprintf(buffer," %02d:%02d:%02d/%02d:%02d:%02d",
			MPanel.CurTime_h,MPanel.CurTime_m,MPanel.CurTime_s,
    		MPanel.TotalTime_h,MPanel.TotalTime_m,MPanel.TotalTime_s);
		SetTextColor(MPanel.hmdc,MPanel.FGColor);
		SetBkColor(MPanel.hmdc,MPanel.BGColor);
		SetTextAlign(MPanel.hmdc, TA_LEFT | TA_TOP | TA_NOUPDATECP);
		ExtTextOut(MPanel.hmdc,MPanel.rcTime.left,MPanel.rcTime.top,
    		ETO_CLIPPED	| ETO_OPAQUE,&(MPanel.rcTime),
    		buffer,strlen(buffer),NULL);
		if((HGDIOBJ)hgdiobj!=(HGDIOBJ)NULL && (HGDIOBJ)hgdiobj!=(HGDIOBJ)GDI_ERROR)
			SelectObject(MPanel.hmdc,hgdiobj);
		InvalidateRect(hPanelWnd, &(MPanel.rcTime), FALSE);
   }
	if(MPanel.UpdateFlag & MP_UPDATE_VOICES){
		char buffer[256];
		HGDIOBJ hgdiobj = SelectObject(MPanel.hmdc,MPanel.hFontVoices);
   	sprintf(buffer," %03d/%03d",MPanel.CurVoices,MPanel.MaxVoices);
		SetTextColor(MPanel.hmdc,MPanel.FGColor);
		SetBkColor(MPanel.hmdc,MPanel.BGColor);
		SetTextAlign(MPanel.hmdc, TA_LEFT | TA_TOP | TA_NOUPDATECP);
		ExtTextOut(MPanel.hmdc,MPanel.rcVoices.left,MPanel.rcVoices.top,
    		ETO_CLIPPED	| ETO_OPAQUE,&(MPanel.rcVoices),
    		buffer,strlen(buffer),NULL);
		if((HGDIOBJ)hgdiobj!=(HGDIOBJ)NULL && (HGDIOBJ)hgdiobj!=(HGDIOBJ)GDI_ERROR)
			SelectObject(MPanel.hmdc,hgdiobj);
		InvalidateRect(hPanelWnd, &(MPanel.rcVoices), FALSE);
   }
	if(MPanel.UpdateFlag & MP_UPDATE_MVOLUME){
		char buffer[256];
		HGDIOBJ hgdiobj = SelectObject(MPanel.hmdc,MPanel.hFontVoices);
   	sprintf(buffer," %03d%%",MPanel.MVolume);
		SetTextColor(MPanel.hmdc,MPanel.FGColor);
		SetBkColor(MPanel.hmdc,MPanel.BGColor);
		SetTextAlign(MPanel.hmdc, TA_LEFT | TA_TOP | TA_NOUPDATECP);
		ExtTextOut(MPanel.hmdc,MPanel.rcMVolume.left,MPanel.rcMVolume.top,
    		ETO_CLIPPED	| ETO_OPAQUE,&(MPanel.rcMVolume),
    		buffer,strlen(buffer),NULL);
		if((HGDIOBJ)hgdiobj!=(HGDIOBJ)NULL && (HGDIOBJ)hgdiobj!=(HGDIOBJ)GDI_ERROR)
			SelectObject(MPanel.hmdc,hgdiobj);
		InvalidateRect(hPanelWnd, &(MPanel.rcMVolume), FALSE);
   }
	if(MPanel.UpdateFlag & MP_UPDATE_RATE){
		char buffer[256];
		HGDIOBJ hgdiobj = SelectObject(MPanel.hmdc,MPanel.hFontRate);
   	sprintf(buffer," %05dHz",MPanel.Rate);
		SetTextColor(MPanel.hmdc,MPanel.FGColor);
		SetBkColor(MPanel.hmdc,MPanel.BGColor);
		SetTextAlign(MPanel.hmdc, TA_LEFT | TA_TOP | TA_NOUPDATECP);
		ExtTextOut(MPanel.hmdc,MPanel.rcRate.left,MPanel.rcRate.top,
    		ETO_CLIPPED	| ETO_OPAQUE,&(MPanel.rcRate),
    		buffer,strlen(buffer),NULL);
		if((HGDIOBJ)hgdiobj!=(HGDIOBJ)NULL && (HGDIOBJ)hgdiobj!=(HGDIOBJ)GDI_ERROR)
			SelectObject(MPanel.hmdc,hgdiobj);
		InvalidateRect(hPanelWnd, &(MPanel.rcRate), FALSE);
   }
	if(MPanel.UpdateFlag & MP_UPDATE_PLAYLIST){
		char buffer[256];
		HGDIOBJ hgdiobj = SelectObject(MPanel.hmdc,MPanel.hFontList);
   	sprintf(buffer," %04d/%04d",MPanel.PlaylistNum,MPanel.PlaylistMax);
		SetTextColor(MPanel.hmdc,MPanel.FGColor);
		SetBkColor(MPanel.hmdc,MPanel.BGColor);
		SetTextAlign(MPanel.hmdc, TA_LEFT | TA_TOP | TA_NOUPDATECP);
		ExtTextOut(MPanel.hmdc,MPanel.rcList.left,MPanel.rcList.top,
    		ETO_CLIPPED	| ETO_OPAQUE,&(MPanel.rcList),
    		buffer,strlen(buffer),NULL);
		if((HGDIOBJ)hgdiobj!=(HGDIOBJ)NULL && (HGDIOBJ)hgdiobj!=(HGDIOBJ)GDI_ERROR)
			SelectObject(MPanel.hmdc,hgdiobj);
		InvalidateRect(hPanelWnd, &(MPanel.rcList), FALSE);
   }
	if(MPanel.UpdateFlag & MP_UPDATE_MISC){
		char buffer[256];
		HGDIOBJ hgdiobj = SelectObject(MPanel.hmdc,MPanel.hFontMisc);
		buffer[0] = '\0';
		switch(MPanel.play_system_mode){
    	case GM_SYSTEM_MODE:
			strcat(buffer,"[GM]");
			break;
    	case GS_SYSTEM_MODE:
			strcat(buffer,"[GS]");
			break;
    	case XG_SYSTEM_MODE:
			strcat(buffer,"[XG]");
			break;
		default:
      case DEFAULT_SYSTEM_MODE:
			strcat(buffer,"[--]");
			break;
		}
		switch(MPanel.current_file_info_file_type){
		case  IS_SMF_FILE:
			strcat(buffer,"[SMF]");
			break;
		case  IS_MCP_FILE:
			strcat(buffer,"[MCP]");
			break;
		case  IS_RCP_FILE:
			strcat(buffer,"[RCP]");
			break;
		case  IS_R36_FILE:
			strcat(buffer,"[R36]");
			break;
		case  IS_G18_FILE:
			strcat(buffer,"[G18]");
			break;
		case  IS_G36_FILE:
			strcat(buffer,"[G36]");
			break;
		case  IS_SNG_FILE:
			strcat(buffer,"[SNG]");
			break;
		case  IS_MM2_FILE:
			strcat(buffer,"[MM2]");
			break;
		case  IS_MML_FILE:
			strcat(buffer,"[MML]");
			break;
		case  IS_FM_FILE:
			strcat(buffer,"[FM ]");
			break;
		case  IS_FPD_FILE:
			strcat(buffer,"[FPD]");
			break;
		case  IS_MOD_FILE:
			strcat(buffer,"[MOD]");
			break;
		case  IS_669_FILE:
			strcat(buffer,"[669]");
			break;
		case  IS_MTM_FILE:
			strcat(buffer,"[MTM]");
			break;
		case  IS_STM_FILE:
			strcat(buffer,"[STM]");
			break;
		case  IS_S3M_FILE:
			strcat(buffer,"[S3M]");
			break;
		case  IS_ULT_FILE:
			strcat(buffer,"[ULT]");
			break;
		case  IS_XM_FILE:
			strcat(buffer,"[XM ]");
			break;
		case  IS_FAR_FILE:
			strcat(buffer,"[FAR]");
			break;
		case  IS_WOW_FILE:
			strcat(buffer,"[WOW]");
			break;
		case  IS_OKT_FILE:
			strcat(buffer,"[OKT]");
			break;
		case  IS_DMF_FILE:
			strcat(buffer,"[DMF]");
			break;
		case  IS_MED_FILE:
			strcat(buffer,"[MED]");
			break;
		case  IS_IT_FILE:
			strcat(buffer,"[IT ]");
			break;
		case  IS_PTM_FILE:
			strcat(buffer,"[PTM]");
			break;
		default:
		case  IS_OTHER_FILE:
			strcat(buffer,"[---]");
			break;
		}
		if(MPanel.current_file_info_max_channel>=0){
      	char local[16];
         sprintf(local,"[%02dch]",MPanel.current_file_info_max_channel+1);
			strcat(buffer,local);
      }else
			strcat(buffer,"[--ch]");
		SetTextColor(MPanel.hmdc,MPanel.FGColor);
		SetBkColor(MPanel.hmdc,MPanel.BGColor);
		SetTextAlign(MPanel.hmdc, TA_LEFT | TA_TOP | TA_NOUPDATECP);
		ExtTextOut(MPanel.hmdc,MPanel.rcMisc.left,MPanel.rcMisc.top,
    		ETO_CLIPPED	| ETO_OPAQUE,&(MPanel.rcMisc),
    		buffer,strlen(buffer),NULL);
		if((HGDIOBJ)hgdiobj!=(HGDIOBJ)NULL && (HGDIOBJ)hgdiobj!=(HGDIOBJ)GDI_ERROR)
			SelectObject(MPanel.hmdc,hgdiobj);
		InvalidateRect(hPanelWnd, &(MPanel.rcMisc), FALSE);
   }
	if(MPanel.UpdateFlag & MP_UPDATE_MESSAGE){
   }
	if(MPanel.UpdateFlag==MP_UPDATE_ALL)
		InvalidateRect(hPanelWnd, NULL, FALSE);
	MPanel.UpdateFlag = MP_UPDATE_NONE;
}

static void MPanelPaintDo(void)
{
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = BeginPaint(MPanel.hwnd, &ps);
	BitBlt(hdc,
		MPanel.rcMe.left,MPanel.rcMe.top,MPanel.rcMe.right,MPanel.rcMe.bottom,
  		MPanel.hmdc,0,0,SRCCOPY);
	EndPaint(MPanel.hwnd, &ps);
}

// 描画
void MPanelPaint(void)
{
	UpdateWindow(hPanelWnd);
}

// 完全描画
void MPanelPaintAll(void)
{
	InvalidateRect(hPanelWnd, NULL, FALSE);
	MPanelPaint();
}

// パネル構造体を元に完全更新をする。
void MPanelUpdateAll(void)
{
	if(!MPanelOK)
		return;
	MPanel.UpdateFlag = MP_UPDATE_ALL;
	MPanelUpdate();
}

// PanelInfo 構造体を読み込んでパネル構造体へ適用する。
// flag は強制更新する。
void MPanelReadPanelInfo(int flag)
{
	int cur_pl_num, playlist_num;

	if(!MPanelOK)
		return;
	if(!PInfoOK)
    	return;

	if(!Panel->changed && !flag)
   	return;

	if(flag
   	||	MPanel.CurTime_s != Panel->cur_time_s
//    || MPanel.CurTime_ss != Panel->cur_time_ss
		|| MPanel.CurTime_m != Panel->cur_time_m
		|| MPanel.CurTime_h != Panel->cur_time_h
		|| MPanel.TotalTime_s != Panel->total_time_s
//		|| MPanel.TotalTime_ss != Panel->total_time_ss
		|| MPanel.TotalTime_m != Panel->total_time_m
		|| MPanel.TotalTime_h != Panel->total_time_h
	){
		MPanel.CurTime_h = Panel->cur_time_h;
		MPanel.CurTime_m = Panel->cur_time_m;
		MPanel.CurTime_s = Panel->cur_time_s;
		MPanel.CurTime_ss = Panel->cur_time_ss;
		MPanel.TotalTime_h = Panel->total_time_h;
		MPanel.TotalTime_m = Panel->total_time_m;
		MPanel.TotalTime_s = Panel->total_time_s;
//		MPanel.TotalTime_ss = Panel->total_time_ss;
		RANGE(MPanel.CurTime_h,0,99);
      RANGE(MPanel.TotalTime_h,0,99);
     	MPanel.UpdateFlag |=	MP_UPDATE_TIME;
	}
	if(flag || MPanel.MaxVoices != Panel->voices){
		MPanel.MaxVoices = Panel->voices;
     	MPanel.UpdateFlag |=	MP_UPDATE_VOICES;
   }
	if(flag || MPanel.CurVoices != Panel->cur_voices){
		MPanel.CurVoices = Panel->cur_voices;
     	MPanel.UpdateFlag |=	MP_UPDATE_VOICES;
   }
	if(flag || MPanel.MVolume != amplification){
		MPanel.MVolume = amplification;
     	MPanel.UpdateFlag |=	MP_UPDATE_MVOLUME;
   }
	if(flag || MPanel.Rate != play_mode->rate){
		MPanel.Rate = play_mode->rate;
     	MPanel.UpdateFlag |=	MP_UPDATE_RATE;
   }

	w32g_get_playlist_index(&cur_pl_num, &playlist_num, NULL);
	if(playlist_num > 0)
		cur_pl_num++;
	if(flag || MPanel.PlaylistNum != cur_pl_num){
		MPanel.PlaylistNum = cur_pl_num;
     	MPanel.UpdateFlag |=	MP_UPDATE_PLAYLIST;
   }
	if(flag || MPanel.PlaylistMax != playlist_num){
		MPanel.PlaylistMax = playlist_num;
     	MPanel.UpdateFlag |=	MP_UPDATE_PLAYLIST;
   }

	if(flag || MPanel.play_system_mode != play_system_mode){
		MPanel.play_system_mode = play_system_mode;
     	MPanel.UpdateFlag |=	MP_UPDATE_MISC;
   }
	if(current_file_info!=NULL){
	if(flag || MPanel.current_file_info_file_type != current_file_info->file_type){
      MPanel.current_file_info_file_type = current_file_info->file_type;
     	MPanel.UpdateFlag |=	MP_UPDATE_MISC;
   }
	if(flag || MPanel.current_file_info_max_channel != current_file_info->max_channel){
      MPanel.current_file_info_max_channel = current_file_info->max_channel;
     	MPanel.UpdateFlag |=	MP_UPDATE_MISC;
   }
   }
}

void MPanelStartLoad(char *filename)
{
    strncpy((char *)MPanel.File, filename, MP_FILE_MAX);
    MPanel.UpdateFlag |= MP_UPDATE_FILE;
    MPanelUpdate();
}




// ----------------------------------------------------------------------------
// Misc. Controls











// ----------------------------------------------------------------------------









// ****************************************************************************
// Version Window

static void VersionWnd(HWND hParentWnd)
{
	char VersionText[2024];
  sprintf(VersionText,
"TiMidity++ version %s" NLS NLS
"TiMidity-0.2i by Tuukka Toivonen <tt@cgs.fi>." NLS
"TiMidity Win32 version by Davide Moretti <dave@rimini.com>." NLS
"TiMidity Windows 95 port by Nicolas Witczak." NLS
"TiMidity Win32 GUI by Daisuke Aoki <dai@y7.net>." NLS
"TiMidity++ by Masanao Izumo <mo@goice.co.jp>." NLS
,timidity_version);
	MessageBox(hParentWnd, VersionText, "Version", MB_OK);
}

static void TiMidityWnd(HWND hParentWnd)
{
	char TiMidityText[2024];
  sprintf(TiMidityText,
" TiMidity++ version %s -- Experimental MIDI to WAVE converter" NLS
" Copyright (C) 1999 Masanao Izumo <mo@goice.co.jp>" NLS
" Copyright (C) 1995 Tuukka Toivonen <tt@cgs.fi>" NLS
NLS
" Win32 version by Davide Moretti <dmoretti@iper.net>" NLS
" GUI by Daisuke Aoki <dai@y7.net>." NLS
" Modified by Masanao Izumo <mo@goice.co.jp>." NLS
NLS
" This program is free software; you can redistribute it and/or modify" NLS
" it under the terms of the GNU General Public License as published by" NLS
" the Free Software Foundation; either version 2 of the License, or" NLS
" (at your option) any later version." NLS
NLS
" This program is distributed in the hope that it will be useful," NLS
" but WITHOUT ANY WARRANTY; without even the implied warranty of"NLS
" MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" NLS
" GNU General Public License for more details." NLS
NLS
" You should have received a copy of the GNU General Public License" NLS
" along with this program; if not, write to the Free Software" NLS
" Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA." NLS
,
timidity_version
	);
	MessageBox(hParentWnd, TiMidityText, "TiMidity", MB_OK);
}


// ****************************************************************************

#ifdef W32GUI_DEBUG
void TiMidityHeapCheck(void)
{
	HANDLE *ProcessHeaps = NULL;
   DWORD dwNumberOfHeaps;
	DWORD dw = 10;
   int i;
   PrintfDebugWnd("\n[Heaps Check Start]\n");
	if(GetProcessHeap()!=NULL)
   	if(HeapValidate(GetProcessHeap(),0,NULL)==TRUE)
   		PrintfDebugWnd("Process Heap is Valid\n");
     	else
   		PrintfDebugWnd("Process Heap is Invalid\n");
   ProcessHeaps = (HANDLE *)realloc(ProcessHeaps,sizeof(HANDLE)*dw);
   dwNumberOfHeaps = GetProcessHeaps(dw, ProcessHeaps);
   if(dw<dwNumberOfHeaps){
		dw = dwNumberOfHeaps;
	   ProcessHeaps = (HANDLE *)realloc(ProcessHeaps,sizeof(HANDLE)*dw);
   	dwNumberOfHeaps = GetProcessHeaps(dw, ProcessHeaps);
   }
   PrintfDebugWnd("NumberOfHeaps=%ld\n",(int)dwNumberOfHeaps);
	for(i=0;i<dwNumberOfHeaps;i++){
     	if(HeapValidate(ProcessHeaps[i],0,NULL)==TRUE)
    		PrintfDebugWnd("Heap %d is Valid\n",i+1);
      else
     		PrintfDebugWnd("Heap %d is Invalid\n",i+1);
   }
   PrintfDebugWnd("[Heaps Check End]\n\n");
	free(ProcessHeaps);
}
#endif

void TiMidityVariablesCheck(void)
{
#if 0
// player_status
	PrintfDebugWnd("[player_status]\n");
	PrintfDebugWnd("player_status=%ld\n",player_status);
	switch(player_status){
	case PLAYERSTATUS_NONE:
	PrintfDebugWnd("player_status=PLAYERSTATUS_NONE\n");
	break;
	case PLAYERSTATUS_STOP:
	PrintfDebugWnd("player_status=PLAYERSTATUS_STOP\n");
	break;
	case PLAYERSTATUS_PAUSE:
	PrintfDebugWnd("player_status=PLAYERSTATUS_PAUSE\n");
	break;
	case PLAYERSTATUS_PLAY:
	PrintfDebugWnd("player_status=PLAYERSTATUS_PLAY\n");
	break;
	case PLAYERSTATUS_PLAYSTART:
	PrintfDebugWnd("player_status=PLAYERSTATUS_PLAYSTART\n");
	break;
	case PLAYERSTATUS_DEMANDPLAY:
	PrintfDebugWnd("player_status=PLAYERSTATUS_DEMANDPLAY\n");
	break;
	case PLAYERSTATUS_PLAYEND:
	PrintfDebugWnd("player_status=PLAYERSTATUS_PLAYEND\n");
	break;
	case PLAYERSTATUS_PLAYERROREND:
	PrintfDebugWnd("player_status=PLAYERSTATUS_PLAYERROREND\n");
	break;
	case PLAYERSTATUS_QUIT:
	PrintfDebugWnd("player_status=PLAYERSTATUS_QUIT\n");
	break;
	case PLAYERSTATUS_ERROR:
	PrintfDebugWnd("player_status=PLAYERSTATUS_ERROR\n");
	break;
	case PLAYERSTATUS_FORCED_EXIT:
	PrintfDebugWnd("player_status=PLAYERSTATUS_FORCED_EXIT\n");
	break;
	default:
   break;
   }
#endif
}




// ****************************************************************************
// Debug Window
#ifdef W32GUI_DEBUG

BOOL CALLBACK DebugWndProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam);
void InitDebugEditWnd(HWND hParentWnd);

void InitDebugWnd(HWND hParentWnd)
{
	hDebugWnd = CreateDialog
  			(hInst,MAKEINTRESOURCE(IDD_DIALOG_DEBUG),hParentWnd,DebugWndProc);
	ShowWindow(hDebugWnd,SW_HIDE);
	UpdateWindow(hDebugWnd);
}

BOOL CALLBACK
DebugWndProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam)
{
	switch (uMess){
		case WM_INITDIALOG:
//			InitDebugEditWnd(hwnd);
		hDebugEditWnd = GetDlgItem(hwnd,IDC_EDIT);
		if(DebugWndFlag)
			CheckDlgButton(hwnd, IDC_CHECKBOX_DEBUG_WND_VALID, 1);
      else
			CheckDlgButton(hwnd, IDC_CHECKBOX_DEBUG_WND_VALID, 0);
		TiMidityHeapCheck();
			return FALSE;
		case WM_DESTROY:
      	PostQuitMessage(0);
			return 0;
    case WM_COMMAND:
    	switch (LOWORD(wParam)) {
      case IDCLOSE:
      	ShowWindow(hwnd, SW_HIDE);
         break;
      case IDCLEAR:
			ClearDebugWnd();
         break;
			case IDC_CHECKBOX_DEBUG_WND_VALID:
				if(IsDlgButtonChecked(hwnd,IDC_CHECKBOX_DEBUG_WND_VALID))
					DebugWndFlag = 1;
				else
					DebugWndFlag = 0;
         break;
      case IDC_BUTTON_EXITPROCESS:
        	ExitProcess(0);
			return 0;
      case IDC_BUTTON_EXIT:
        	return DestroyWindow(hwnd);
      case IDC_BUTTON_HEAP_CHECK:
			TiMidityHeapCheck();
        	break;
      case IDC_BUTTON_VARIABLES_CHECK:
			TiMidityVariablesCheck();
        	break;
      default:
        		break;
      }
   	switch (HIWORD(wParam)) {
		case EN_ERRSPACE:
      	ClearConsoleWnd();
//      	PutsConsoleWnd("### EN_ERRSPACE -> Clear! ###\n");
			break;
		default:
      	break;
      }
    	break;
    case WM_SIZE:
//      GetClientRect(hDebugWnd, &rc);
//      MoveWindow(hDebugEditWnd, rc.left, rc.top,rc.right, rc.bottom - 30,TRUE);
	    return FALSE;
		case WM_CLOSE:
					ShowWindow(hDebugWnd, SW_HIDE);
          break;
    default:
      	return FALSE;
	}
	return FALSE;
}

#if 0
void InitDebugEditWnd(HWND hParentWnd)
{
  RECT rc;
	GetClientRect(hParentWnd, &rc);
	hDebugEditWnd = CreateWindowEx(
  	WS_EX_CLIENTEDGE|WS_EX_TOOLWINDOW|WS_EX_DLGMODALFRAME,
  	"EDIT","",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOHSCROLL | WS_HSCROLL
    |ES_READONLY | ES_WANTRETURN | ES_MULTILINE | ES_AUTOVSCROLL ,
//      	0,0,rc.right, rc.bottom - 30,hParentWnd,NULL,hInst,NULL);
      	0,0,100,100,hParentWnd,NULL,hInst,NULL);
	SendMessage(hDebugEditWnd, EM_SETLIMITTEXT, (WPARAM)1024*640, 0);
//  SendMessage(hDebugEditWnd, WM_PAINT, 0, 0);
	GetClientRect(hParentWnd, &rc);
	MoveWindow(hDebugEditWnd,rc.left,rc.top,rc.right,rc.bottom-30,TRUE);
 	ClearDebugWnd();
	ShowWindow(hDebugEditWnd,SW_SHOW);
	UpdateWindow(hDebugEditWnd);
}
#endif

void PutsDebugWnd(char *str)
{
	if(!IsWindow(hDebugEditWnd) || !DebugWndFlag)
		return;
	PutsEditCtlWnd(hDebugEditWnd,str);
}

void PrintfDebugWnd(char *fmt, ...)
{
	va_list ap;
	if(!IsWindow(hDebugEditWnd) || !DebugWndFlag)
		return;
	va_start(ap, fmt);
	VprintfEditCtlWnd(hDebugEditWnd,fmt,ap);
	va_end(ap);
}

void ClearDebugWnd(void)
{
	if(!IsWindow(hDebugEditWnd))
		return;
	ClearEditCtlWnd(hDebugEditWnd);
}
#endif






















// ****************************************************************************
// Main Thread


void WINAPI MainThread(void *arglist)
{
    MSG msg;

	ThreadNumMax++;

#ifdef W32GUI_DEBUG
	PrintfDebugWnd("(*/%d)MainThread : Start.\n",ThreadNumMax);
#endif
#ifdef USE_THREADTIMES
	ThreadTimesAddThread(hMainThread,"MainThread");
#endif
// Thread priority
//	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_BELOW_NORMAL);
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
//	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);


    InitStartWnd(SW_HIDE);

    w32g_wait_for_init = 0;

// message loop for the application
	while( GetMessage(&msg,NULL,0,0) ){
//		HandleFastSearch(msg);
//PrintfDebugWnd("H%lu M%lu WP%lu LP%lu T%lu x%d y%d\n",
//	msg.hwnd, msg.message, msg.wParam, msg.lParam, msg.time, msg.pt.x, msg.pt.y);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

#ifdef W32GUI_DEBUG
	Sleep(200);
	PrintfDebugWnd("w32gui_main: DebugWndExit\n");
//   MessageBox(NULL, "Exit!","Exit!",MB_OK);
	if(hDebugWnd!=NULL)
		for(;;){
			if(!DebugThreadExit){
   			SendMessage(hDebugWnd,WM_COMMAND,(WPARAM)IDC_BUTTON_EXIT,0);
				Sleep(100);
         } else
         	break;
      }
#endif
	if(!w32g_restart_gui_flag)
	{
		OnExitReady();
		w32g_send_rc(RC_QUIT, 0);
	}
	crt_endthread();
}




// **************************************************************************
// Misc Dialog
#define DialogMaxFileName 16536
static char DialogFileNameBuff[DialogMaxFileName];
static char *DlgFileOpen(HWND hwnd, char *title, char *filter, char *dir)
{
	OPENFILENAME ofn;
    memset(DialogFileNameBuff, 0, sizeof(DialogFileNameBuff));
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = hInst ;
	ofn.lpstrFilter = filter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 1 ;
	ofn.lpstrFile = DialogFileNameBuff;
	ofn.nMaxFile = sizeof(DialogFileNameBuff);
	ofn.lpstrFileTitle = 0;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir	= dir;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST
		| OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_READONLY;
	ofn.lpstrDefExt = 0;
	ofn.lCustData = 0;
	ofn.lpfnHook = 0;
	ofn.lpTemplateName= 0;

	if(GetOpenFileName(&ofn)==TRUE)
		return DialogFileNameBuff;
	else
		return NULL;
}

static void DlgMidiFileOpen(HWND hwnd)
{
    char *dir, *file;
    char *filter = "timidity file\0*.mid;*.smf;*.rcp;*.r36;*.g18;*.g36;*.lzh;*.zip;*.gz\0"
		"midi file\0*.mid;*.smf;*.rcp;*.r36;*.g18;*.g36\0"
		"archive file\0*.lzh;*.zip;*.gz\0"
		"playlist file\0*.pls;*.m3u;*.asx\0"
		"all files\0*.*\0"
		"\0\0";

    if(w32g_lock_open_file)
		return;

    if(MidiFileOpenDir[0])
		dir = MidiFileOpenDir;
    else
		dir = NULL;

	if((file = DlgFileOpen(hwnd, NULL, filter, dir)) == NULL)
		return;

    w32g_lock_open_file = 1;
    w32g_send_rc(RC_EXT_LOAD_FILE, (int32)file);
}

static void DlgDirOpen(HWND hwnd)
{
	static int initflag = 1;
	static char biBuffer[MAXPATH + 256];
	static char Buffer[MAXPATH + 256];
	BROWSEINFO bi;
	LPITEMIDLIST itemidlist;

	if(w32g_lock_open_file)
	    return;

	if(initflag==1){
		biBuffer[0] = '\0';
		initflag = 0;
	}
	memset(&bi, 0, sizeof(bi));
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
    bi.pszDisplayName = biBuffer;
	bi.lpszTitle = "Select a directory with MIDI files.";
	bi.ulFlags = 0;
	bi.lpfn = NULL;
    bi.lParam = 0;
    bi.iImage = 0;
	itemidlist = SHBrowseForFolder(&bi);
	if(!itemidlist)
		return; /* Cancel */
	SHGetPathFromIDList(itemidlist, Buffer);
	strncpy(biBuffer, Buffer, sizeof(Buffer) - 1);
    w32g_lock_open_file = 1;
	directory_form(Buffer);
    w32g_send_rc(RC_EXT_LOAD_FILE, (int32)Buffer);
}

static void DlgPlaylistOpen(HWND hwnd)
{
    char *dir, *file;
    char *filter =
		"playlist file\0*.pls;*.m3u;*.asx\0"
		"all files\0*.*\0"
		"\0\0";

    if(w32g_lock_open_file)
		return;

    if(MidiFileOpenDir[0])
		dir = MidiFileOpenDir;
    else
		dir = NULL;

	if((file = DlgFileOpen(hwnd, NULL, filter, dir)) == NULL)
		return;

    w32g_lock_open_file = 1;
    w32g_send_rc(RC_EXT_LOAD_PLAYLIST, (int32)file);
}

#include <sys/stat.h> /* for stat() */
static int CheckOverWrite(HWND hwnd, char *filename)
{
	char buff[BUFSIZ];
	int exists;

#if 0
	FILE *fp;
    if((fp = fopen(filename, "r")) == NULL)
		exists = 0;
	else
	{
		fclose(fp);
		exists = 1;
	}
#else
	struct stat st;
	exists = (stat(filename, &st) != -1);
#endif

	if(!exists)
		return 1;
	snprintf(buff, sizeof(buff), "%s exists. Overwrite it?", filename);
	return MessageBox(hwnd, buff, "Warning", MB_YESNO) == IDYES;
}

static void DlgPlaylistSave(HWND hwnd)
{
	OPENFILENAME ofn;
	static char *dir;
    char *filter =
		"playlist file\0*.pls;*.m3u;*.asx\0"
		"all files\0*.*\0"
		"\0\0";

    if(w32g_lock_open_file)
		return;

    if(MidiFileOpenDir[0])
		dir = MidiFileOpenDir;
    else
		dir = NULL;

	memset(DialogFileNameBuff, 0, sizeof(DialogFileNameBuff));
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = hInst;
	ofn.lpstrFilter = filter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 1 ;
	ofn.lpstrFile = DialogFileNameBuff;
	ofn.nMaxFile = sizeof(DialogFileNameBuff);
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir	= dir;
	ofn.lpstrTitle	= "Save Playlist File";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = 0;
	ofn.lCustData = 0;
	ofn.lpfnHook = 0;
	ofn.lpTemplateName= 0;

	if(GetSaveFileName(&ofn) != TRUE)
		return;
	if(!CheckOverWrite(hwnd, DialogFileNameBuff))
		return;
    w32g_lock_open_file = 1;
    w32g_send_rc(RC_EXT_SAVE_PLAYLIST, (int32)DialogFileNameBuff);
}

// ****************************************************************************
// Edit Ctl.

void VprintfEditCtlWnd(HWND hwnd, char *fmt, va_list argList)
{
	 char buffer[BUFSIZ], out[BUFSIZ];
	 char *in;
	 int i;

	 if(!IsWindow(hwnd))
		  return;

	 vsnprintf(buffer, sizeof(buffer), fmt, argList);
	 in = buffer;
	 i = 0;
	 for(;;){
		  if(*in == '\0' || i>sizeof(out)-3){
				out[i] = '\0';
				break;
		  }
		  if(*in=='\n'){
				out[i] = 13;
				out[i+1] = 10;
				in++;
				i += 2;
				continue;
		  }
		  out[i] = *in;
		  in++;
		  i++;
	 }
	 Edit_SetSel(hwnd,-1,-1);
	 Edit_ReplaceSel(hwnd,out);
}

void PrintfEditCtlWnd(HWND hwnd, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    VprintfEditCtlWnd(hwnd,fmt,ap);
    va_end(ap);
}

#if 1
void PutsEditCtlWnd(HWND hwnd, char *str)
{
	char *in = str;
	int i;
	char out[BUFSIZ];
	i = 0;
	for(;;){
		if(*in == '\0' || i>sizeof(out)-3){
			out[i] = '\0';
			break;
    }
  	if(*in=='\n'){
    	out[i] = 13;
    	out[i+1] = 10;
			in++;
      i += 2;
      continue;
    }
    out[i] = *in;
		in++;
    i++;
  }
	if(IsWindow(hwnd)){
		Edit_SetSel(hwnd,-1,-1);
		Edit_ReplaceSel(hwnd,out);
	}
}
#else
void PutsEditCtlWnd(HWND hwnd, char *str)
{
	if(!IsWindow(hwnd))
		return;
	PrintfEditCtlWnd(hwnd,"%s",str);
}
#endif

void ClearEditCtlWnd(HWND hwnd)
{
	char pszVoid[]="";
	if(!IsWindow(hwnd))
		return;
	if(IsWindow(hwnd)){
//		Edit_SetSel(hwnd,0,-1);
		Edit_SetSel(hwnd,-1,-1);
	}
	Edit_SetText(hwnd,pszVoid);
}


// ****************************************************************************
// Misc funciton.

#ifndef WIN32GCC
static char *get_filename(char *src, char *dest)
{
	char *p = src;
	char *start = NULL;
	int quot_flag = 0;
	if(p == NULL)
		return NULL;
	for(;;){
		if(*p != ' ' && *p != '\0' && start == NULL)
			start = p;
		if(*p == '\'' || *p == '\"'){
			if(quot_flag){
				if(p - start != 0)
					strncpy(dest, start, p - start);
				dest[p-start] = '\0';
				p++;
				return p;
			} else {
				quot_flag = !quot_flag;
				p++;
				start = p;
				continue;
			}
		}
		if(*p == '\0' || (*p == ' ' && !quot_flag)){
			if(start == NULL)
				return NULL;
			if(p - start != 0)
				strncpy(dest, start, p - start);
			dest[p-start] = '\0';
			if(*p != '\0')
				p++;
			return p;
		}
		p++;
	}
}

static void CmdLineToArgv(LPSTR lpCmdLine, int *pArgc, CHAR ***pArgv)
{
	LPSTR p = lpCmdLine , buffer, lpsRes;
	int i, max = -1, inc = 16;
	int buffer_size;

	*pArgv = NULL;
	buffer_size = strlen(lpCmdLine) + 1024;
//	buffer = safe_malloc(sizeof(CHAR) * buffer_size + 1);
	buffer = (LPSTR)malloc(sizeof(CHAR) * buffer_size + 1);
	strcpy(buffer, lpCmdLine);

	for(i=0;;i++)
	{
	if(i > max){
		max += inc;
//		*pArgv = (CHAR **)safe_realloc(*pArgv, sizeof(CHAR *) * (max + 1));
		*pArgv = (CHAR **)realloc(*pArgv, sizeof(CHAR *) * (max + 2));
	}
	if(i==0){
		GetModuleFileName(NULL,buffer,buffer_size);
		lpsRes = p;
	} else
		lpsRes = get_filename(p,buffer);
	if(lpsRes != NULL){
//		(*pArgv)[i] = (CHAR *)safe_malloc(sizeof(CHAR) * strlen(buffer) + 1);
		(*pArgv)[i] = (CHAR *)malloc(sizeof(CHAR) * strlen(buffer) + 1);
		strcpy((*pArgv)[i],buffer);
		p = lpsRes;
	} else {
		*pArgc = i;
		free(buffer);
		return;
	}
	}
}
#endif /* !WIN32GCC */

int w32g_msg_box(char *message, char *title, int type)
{
    return MessageBox(hMainWnd, message, title, type);
}


#define RC_QUEUE_SIZE 8
static struct
{
    int rc;
    int32 value;
} rc_queue[RC_QUEUE_SIZE];
static volatile int rc_queue_len, rc_queue_beg, rc_queue_end;

static HANDLE w32g_lock_sem = NULL;
static HANDLE w32g_empty_sem = NULL;

void w32g_lock(void)
{
    WaitForSingleObject(w32g_lock_sem, INFINITE);
}

void w32g_unlock(void)
{
    ReleaseSemaphore(w32g_lock_sem, 1, NULL);
}

void w32g_send_rc(int rc, int32 value)
{
    w32g_lock();

    if(rc_queue_len == RC_QUEUE_SIZE)
    {
	/* Over flow.  Remove the oldest message */
	rc_queue_len--;
	rc_queue_beg = (rc_queue_beg + 1) % RC_QUEUE_SIZE;
    }

    rc_queue_len++;
    rc_queue[rc_queue_end].rc = rc;
    rc_queue[rc_queue_end].value = value;
    rc_queue_end = (rc_queue_end + 1) % RC_QUEUE_SIZE;
    ReleaseSemaphore(w32g_empty_sem, 1, NULL);
    w32g_unlock();
}

int w32g_get_rc(int32 *value, int wait_if_empty)
{
    int rc;

    while(rc_queue_len == 0)
    {
	if(!wait_if_empty)
	    return RC_NONE;
	WaitForSingleObject(w32g_empty_sem, INFINITE);
	VOLATILE_TOUCH(rc_queue_len);
    } 

    w32g_lock();
    rc = rc_queue[rc_queue_beg].rc;
    *value = rc_queue[rc_queue_beg].value;
    rc_queue_len--;
    rc_queue_beg = (rc_queue_beg + 1) % RC_QUEUE_SIZE;
    w32g_unlock();

    return rc;
}

int w32g_open(void)
{
    SaveSettingTiMidity(st_current);
    memcpy(st_temp, st_current, sizeof(SETTING_TIMIDITY));

    w32g_lock_sem = CreateSemaphore(NULL, 1, 1, "TiMidity Mutex Lock");
    w32g_empty_sem = CreateSemaphore(NULL, 0, 8, "TiMidity Empty Lock");

    hPlayerThread = GetCurrentThread();
    w32g_wait_for_init = 1;
    hMainThread = crt_beginthreadex(NULL, 0,
				    (LPTHREAD_START_ROUTINE)MainThread,
				    NULL, 0, &dwMainThreadID);
    while(w32g_wait_for_init)
    {
		Sleep(0);
		VOLATILE_TOUCH(w32g_wait_for_init);
    }
    return 0;
}

static void terminate_main_thread(void)
{
	DWORD status;

	switch(WaitForSingleObject(hMainThread, 0))
	{
	  case WAIT_OBJECT_0:
		break;
	  case WAIT_TIMEOUT:
		OnQuit();
		status = WaitForSingleObject(hMainThread, 5000);
		if(status == WAIT_TIMEOUT)
			TerminateThread(hMainThread, 0);
		break;
	  default:
		TerminateThread(hMainThread, 0);
		break;
	}
}

void w32g_close(void)
{
	terminate_main_thread();
    CloseHandle(w32g_lock_sem);
    CloseHandle(w32g_empty_sem);
}

void w32g_restart(void)
{
	w32g_restart_gui_flag = 1;
	terminate_main_thread();
    CloseHandle(w32g_lock_sem);
    CloseHandle(w32g_empty_sem);

	if(MPanel.hFontTitle!=NULL)
		DeleteObject(MPanel.hFontTitle);
	if(MPanel.hFontFile!=NULL)
		DeleteObject(MPanel.hFontFile);
	if(MPanel.hFontTime!=NULL)
		DeleteObject(MPanel.hFontTime);
	if(MPanel.hFontVoices!=NULL)
		DeleteObject(MPanel.hFontVoices);
	if(MPanel.hFontMVolume!=NULL)
		DeleteObject(MPanel.hFontMVolume);
	if(MPanel.hFontRate!=NULL)
		DeleteObject(MPanel.hFontRate);
	if(MPanel.hFontList!=NULL)
		DeleteObject(MPanel.hFontList);

	/* Reset variable */
    hStartWnd = 0;
    hMainWnd = 0;
    hDebugWnd = 0;
    hConsoleWnd = 0;
    hTracerWnd = 0;
    hDocWnd = 0;
    hListWnd = 0;
    hWrdWnd = 0;
    hSoundSpecWnd = 0;
    hDebugEditWnd = 0;
    hDocEditWnd = 0;

	/* Now ready to start */
	w32g_open();
	w32g_restart_gui_flag = 0;
}

void w32g_ctle_play_start(int sec)
{
    char *title;

    if(sec >= 0)
    {
	SetScrollRange(hMainWndScrollbarProgressWnd, SB_CTL, 0, sec, TRUE);
	MainWndScrollbarProgressUpdate(0);
    }
    else
	MainWndScrollbarProgressUpdate(-1);

    Panel->cur_time_h = MPanel.CurTime_h = 0;
    Panel->cur_time_m = MPanel.CurTime_m = 0;
    Panel->cur_time_s = MPanel.CurTime_s = 0;
    Panel->cur_time_ss = MPanel.CurTime_ss = 0;

    MPanel.TotalTime_h = sec / 60 / 60;
    RANGE(MPanel.TotalTime_h, 0, 99);
    Panel->total_time_h = MPanel.TotalTime_h;

    sec %= 60 * 60;
    Panel->total_time_m = MPanel.TotalTime_m = sec / 60;
    Panel->total_time_s = MPanel.TotalTime_s = sec % 60;
    Panel->total_time_ss = MPanel.TotalTime_ss = 0;

    MPanel.UpdateFlag |= MP_UPDATE_TIME;

    /* Now, ready to get the title of MIDI */
    if((title = get_midi_title(MPanel.File)) != NULL)
    {
	strncpy(MPanel.Title, title, MP_TITLE_MAX);
	MPanel.UpdateFlag |= MP_UPDATE_TITLE;
    }
    MPanelUpdate();
}

void MainWndScrollbarProgressUpdate(int sec)
{
    static int lastsec = -1, enabled = 0;

    if(sec == lastsec)
	return;

    if(sec == -1)
    {
  	EnableWindow(hMainWndScrollbarProgressWnd, FALSE);
	enabled = 0;
	progress_jump = -1;
    }
    else
    {
	if(!enabled)
	{
	    EnableWindow(hMainWndScrollbarProgressWnd, TRUE);
	    enabled = 1;
	}
	if(progress_jump == -1)
	    SetScrollPos(hMainWndScrollbarProgressWnd, SB_CTL, sec, TRUE);
    }
    lastsec = sec;
}

void w32g_show_console(void)
{
	ShowWindow(hConsoleWnd, SW_SHOW);
	SendDlgItemMessage(hMainWnd, IDC_TOOLBARWINDOW_SUBWND,
					   TB_CHECKBUTTON, IDM_CONSOLE, (LPARAM)MAKELONG(TRUE, 0));
}
