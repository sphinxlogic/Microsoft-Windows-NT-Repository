
/*
 * This file is part of the Seyon, Copyright (c) 1992-1993 by Muhammad M.
 * Saggaf. All rights reserved.
 *
 * See the file COPYING (1-COPYING) or the manual page seyon(1) for a full
 * statement of rights and permissions for this program.
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Dialog.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "seyon.h"
#include "SeDecl.h"
#include "SeSig.h"

extern pid_t    readProcPid;
extern int      startupCode;

char            captureFile[REG_BUF];
Boolean         capture = False;
FILE           *cfp;

void
                SendBreak(),
                terminal_refresh(),
                LocalShell(),
                ToggleCapture(),
                kill_handler(),
                DivertFile(),
                DoDivertFile(),
                ExecDivertFile(),
                RunScript(),
                DoRunScript(),
                ExecScript();

void
TopMisc(widget)
     Widget          widget;
{
  Widget          popup,
                  mBox,
                  uBox,
                  lBox,
                  toggle;

  ReturnIfTrue(inhibit_child);

  XtVaSetValues(widget, XtNsensitive, False, NULL);
  popup = AddSimplePopup("misc", widget);
  XtAddCallback(popup, XtNdestroyCallback, SetSensitiveOn, widget);

  mBox = SeAddPaned("mBox", popup);
  uBox = SeAddBox("uBox", mBox);
  lBox = SeAddBox("lBox", mBox);

  SeAddButton("break", uBox, SendBreak);
  SeAddButton("refresh", uBox, terminal_refresh);
  SeAddButton("suspend", uBox, LocalShell);
  toggle = SeAddToggle("capture", uBox, ToggleCapture);
  SeAddButton("divert", uBox, DivertFile);
  SeAddButton("script", uBox, RunScript);

  AddButton("dismiss", lBox, DestroyShell, NULL);

  SeSetUnsetToggle(toggle, capture);

  PositionShell(popup, widget, SHELLPOS_HWFH);
  XtPopup(popup, XtGrabNone);
}

void
SendBreak(widget)
     Widget          widget;
{
  ret_if_up(widget, inhibit_child);
  send_break();
  SeyonMessage("BREAK Sent to Remote Host");
}

void
terminal_refresh(widget)
     Widget          widget;
{
  ret_if_up(widget, inhibit_child);
  restart_terminal();
  SeyonMessage("Terminal Process Refreshed");
}

void
LocalShell(widget)
     Widget          widget;
{
  ret_if_up(widget, inhibit_child);
  exec_cmd("");
  SeyonMessage("Terminal Suspended");
}

void
ToggleCapture(widget)
     Widget          widget;
{
  ret_if_up(widget, inhibit_child);
  DoToggleCapture();

  SeSetUnsetToggle(widget, capture);
  restart_terminal();
}

Boolean
DoToggleCapture()
{
  if (capture) {
    fclose(cfp);
    capture = False;
    SeyonMessage("Capture Turned OFF");
  }
  else {
    if ((cfp = fopen(captureFile, "a")) == NULL) {
      SeyonMessagef("Unable to Open Capture File `%s'", captureFile, "", "");
      return False;
    }
    else {
      capture = True;
      SeyonMessage("Capture Turned ON");
    }
  }
  return True;
}

/*
 * DivertFile: uploads a text file.
 */

void
DivertFile(widget)
     Widget          widget;
{
  ErrorIfBusy();
  SePopupDialogGetStringE("divert_name", widget, DoDivertFile, NULL,
			  NULL, True);
}

void
divert_action_ok(widget)
     Widget          widget;
{
  DoDivertFile(widget);
}

void
DoDivertFile(widget)
     Widget          widget;
{
  Widget          dialog = XtParent(widget);
  String          file_name;

  file_name = XawDialogGetValueString(dialog);
  DestroyShell(dialog);

  ExecDivertFile(XtParent(GetShell(widget)), file_name);
}

void
divert_handler(
#if NeedFunctionPrototypes
		int signo,
		XtPointer client_data
#endif
)
{
#if defined(SUNOS_3) || defined(Mips)
  union wait status;
#else
  int             status;
#endif

  XoAppIgnoreSignal(app_con, SIGCHLD);
  wait(&status);

#if defined(SUNOS_3) || defined(Mips)
  switch (status.w_retcode) {
#else
  switch (WEXITSTATUS(status)) {
#endif
  case 0:
    SeyonMessage("Text Upload Successful");
    break;
  case 10:
    SeyonMessage("Text Upload Canceled");
    break;
  }

  inhibit_child = False;
  post_process();
}

void
killdivert_handler(
#if NeedFunctionPrototypes
		    int signo
#endif
)
{
  signal(SIGTERM, SIG_IGN);

  if (kill(readProcPid, SIGTERM) == 0) wait((int*)0);
  exit(10);
}

void
ExecDivertFile(widget, file_name)
     Widget          widget;
     String          file_name;
{
  char            fullname[REG_BUF];
  FILE           *fp;
  int             c;

  expand_fname(file_name, fullname);
  if ((fp = fopen(fullname, "r")) == NULL) {
    SeyonMessagef("Unable to Open File `%s'", file_name, "", "");
    PopupError("errFileAccess", widget);
    return;
  }

  inhibit_child = True;
  SeyonMessagef("Uploading Text File '%s'...", file_name, "", "");

  pre_process();
  XoAppAddSignal(app_con, SIGCHLD, divert_handler, NULL);

  if ((w_child_pid = SeFork()) == 0) {
    signal(SIGTERM, killdivert_handler);

    if ((readProcPid = SeFork()) == 0)
      PortToTty();

    while ((c = getc(fp)) != EOF) {
      send_tbyte(c);
      if (c == '\r' || c == '\n')
	usleep(MDELAY);
    }
    fclose(fp);
	if (kill(readProcPid, SIGTERM) == 0) wait((int*)0);
    exit(0);
  }
}

/*
 * run a script
 */

void
RunScript(widget)
     Widget          widget;
{
  Widget          popup;
  
  ErrorIfBusy();
  popup = GetShell(PopupDialogGetValue("script_name", widget, DoRunScript, 
									   NULL, NULL));
  PopupCentered(popup, widget);
}

void
script_action_ok(widget)
     Widget          widget;
{
  DoRunScript(widget);
}

void
DoRunScript(widget)
     Widget          widget;
{
  Widget          dialog = XtParent(widget);
  String          script_name;

  script_name = XawDialogGetValueString(dialog);
  DestroyShell(dialog);

  linkflag = 0;
  ExecScript(script_name);
}

void
script_handler(
#if NeedFunctionPrototypes
		int signo,
		XtPointer client_data
#endif
)
{
#if defined(SUNOS_3) || defined(Mips)
  union wait status;
#else
  int             status;
#endif

  XoAppIgnoreSignal(app_con, SIGCHLD);
  wait(&status);

#if defined(SUNOS_3) || defined(Mips)
  switch (status.w_retcode) {
#else
  switch (WEXITSTATUS(status)) {
#endif
  case 0:
    SeyonMessage("Script Completed");
    break;
  case 1:
    SeyonMessage("Script Execution Failed");
    break;
  case 10:
    SeyonMessage("Script Canceled by User");
    get_modem_attr();
    break;
  }

  inhibit_child = False;

  if (startupCode < 4) startupCode++;
  if (startupCode < 2)
	SeyonMessagef("Welcome to Seyon version %s%s", VERSION, REVISION);
  if (startupCode < 2 && qres.script) 
	{ProcRequest(EXEC_SCRIPT, "", qres.script); return;}
  else {
	post_process();
	if (startupCode < 3 && (qres.dialDirAutoOpen || qres.dialAutoStart)) {
	  startupCode = 2; 
	  ProcRequest(TOP_DIAL, "", ""); 
	  return;
	}
  } /* else... */
}

void
killscript_handler(
#if NeedFunctionPrototypes
		    int signo
#endif
)
{
  signal(SIGTERM, SIG_IGN);
  put_parameters();
  exit(10);
}

void
ExecScript(scriptName)
     String          scriptName;
{
  int scriptRet;

  ErrorIfBusy();

  inhibit_child = True;
  SeyonMessagef("Running Script `%s'...", scriptName, "", "");

  pre_process();
  XoAppAddSignal(app_con, SIGCHLD, script_handler, NULL);

  if ((w_child_pid = SeFork()) == 0) {
    signal(SIGTERM, killscript_handler);

    scriptRet = (int)do_script(scriptName);
	put_parameters();
	exit(scriptRet ? 0 : 1);
  }
}

void ExecExit()
{
  s_exit();
}
