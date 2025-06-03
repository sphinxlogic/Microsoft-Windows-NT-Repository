
/*
 * This file is part of the Seyon, Copyright (c) 1992-1993 by Muhammad M.
 * Saggaf. All rights reserved.
 *
 * See the file COPYING (1-COPYING) or the manual page seyon(1) for a full
 * statement of rights and permissions for this program.
 */

#include <signal.h>

#include <fcntl.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Dialog.h>

#include "seyon.h"
#include "SeDecl.h"

#ifndef HELPFILE
#define HELPFILE "/usr/lib/X11/seyon.help"
#endif

extern void     top_shell(),
                TopAbout(),
                TopHelp(),
                TopSet(),
                TopTransfer(),
                TopMisc(),
                SetNewlineTrMode(),
                FunMessage();

extern void     upload_acc_ok(),
                shell_acc_ok(),
                setVal_action_ok(),
                manual_dial_action_ok(),
                divert_action_ok(),
                script_action_ok();

extern Boolean  CvtStringToStringArray();

void            HangupConfirm(),
                ExecHangup(),
                ExitConfirm(),
                kill_w_child(),
                ExitAction(),
                test();

Boolean         inhibit_child = False;
Widget          w_exit,
                w_kill,
                statusMessage;

Widget          dialWidget;

pid_t           w_child_pid = 0;
Pixmap          progIcon;

#include "progIcon.h"

void
InitVariables(topLevel)
     Widget          topLevel;
{
  char            buffer[REG_BUF];

  sprintf(captureFile, "%s/%s", expand_fname(qres.defaultDirectory, buffer),
	  qres.captureFile);

  SetNewlineTrMode(qres.newlineTranslation);
}

void
SetIcon(topLevel)
     Widget          topLevel;
{
  progIcon = XCreateBitmapFromData(XtDisplay(topLevel),
				   DefaultRootWindow(XtDisplay(topLevel)),
				   progIcon_bits, progIcon_width,
				   progIcon_height);
  XtVaSetValues(topLevel, XtNiconPixmap, progIcon, NULL);
}

Widget
CreateCommandCenter(app_con, topLevelWidget)
     XtAppContext    app_con;
     Widget          topLevelWidget;
{
  void            BeepAction(),
                  DialEntriesAction(),
                  DivertFileAction(),
                  EchoAction(),
                  FileTransferAction(),
                  HangupAction(),
                  ManualDialAction(),
                  OpenDialDirAction(),
                  QuitAction(),
                  RunScriptAction(),
                  SetAction(),
                  ShellCommandAction(),
                  TransmitAction();

  Widget          mainBox,
                  menuBox,
                  statusBox,
                  messageBox,
                  quickKeyBox;
  Widget          quickKeyW;
  static Widget   status[NUM_MDM_STAT];
  static Atom     wm_delete_window;
  Dimension       menuBoxWidth;
  char            qKBuf[REG_BUF];
  int             i, n = 0;

  static XtActionsRec actionTable[] =
  {
    {"UploadOk", upload_acc_ok},
    {"ShellOk", shell_acc_ok},
    {"SetValOk", setVal_action_ok},
    {"ManualDialOk", manual_dial_action_ok},
    {"DivertOk", divert_action_ok},
    {"ScriptOk", script_action_ok},
    {"Exit", ExitAction},
    {"Beep", BeepAction},
    {"DialEntries", DialEntriesAction},
    {"DivertFile", DivertFileAction},
    {"Echo", EchoAction},
	{"FileTransfer", FileTransferAction},
    {"Hangup", HangupAction},
    {"ManualDial", ManualDialAction},
    {"OpenDialDir", OpenDialDirAction},
    {"Quit", QuitAction},
    {"RunScript", RunScriptAction},
    {"Set", SetAction},
    {"ShellCommand", ShellCommandAction},
    {"Transmit", TransmitAction},
  };

  XtAppAddActions(app_con, actionTable, XtNumber(actionTable));

  mainBox = SeAddPaned("mainBox", topLevelWidget);
  statusBox = AddBox("statusBox", mainBox);
  messageBox = SeAddForm("messageBox", mainBox);
  quickKeyBox = AddBox("quickKeyBox", mainBox);
  menuBox = AddBox("menuBox", mainBox);

  status[0] = SeAddToggle("dcd", statusBox, NULL);
  status[1] = SeAddToggle("dtr", statusBox, NULL);
  status[2] = SeAddToggle("dsr", statusBox, NULL);
  status[3] = SeAddToggle("rts", statusBox, NULL);
  status[4] = SeAddToggle("cts", statusBox, NULL);
  status[5] = SeAddToggle("rng", statusBox, NULL);

  statusMessage = SeAddLabel("message", messageBox);

  AddButton("about", menuBox, TopAbout, NULL);
  AddButton("help", menuBox, TopHelp, NULL);
  AddButton("set", menuBox, TopSet, NULL);
  dialWidget = AddButton("dial", menuBox, TopDial, NULL);
  AddButton("transfer", menuBox, TopTransfer, NULL);
  AddButton("shellCommand", menuBox, top_shell, NULL);
  AddButton("misc", menuBox, TopMisc, NULL);

  AddButton("hangup", menuBox, HangupConfirm, NULL);
  w_exit = AddButton("exit", menuBox, ExitConfirm, NULL);
  w_kill = AddButton("kill", menuBox, kill_w_child, NULL);

  for (i = 0; i < 10; i++)
	if (qres.quickKey[i][0]) {
      n++;
	  sprintf(qKBuf, "quickKey%d", i+1);
	  quickKeyW = AddButton(qKBuf, quickKeyBox, NULL, NULL);
	  sprintf(qKBuf, "<Btn1Up>: %s unset()", qres.quickKey[i]);
	  XtOverrideTranslations(quickKeyW, XtParseTranslationTable(qKBuf));
	}
  if (n == 0) XtDestroyWidget(quickKeyBox);

  XtOverrideTranslations(topLevelWidget,
		  XtParseTranslationTable("<Message>WM_PROTOCOLS: Exit()"));

  /* Call UpdateStatusBox() before realizing the top-level widget so that
	 the status widget array in that function would be properly initialized
	 before the user is able to call hangup(), since the latter passes NULL
	 to UpdateStatusBox() */
  UpdateStatusBox((XtPointer)status);

  XtSetMappedWhenManaged(topLevelWidget, False);
  XtRealizeWidget(topLevelWidget);
  XtVaGetValues(menuBox, XtNwidth, &menuBoxWidth, NULL);
  XtVaSetValues(statusBox, XtNwidth, menuBoxWidth, NULL);
  if (n) XtVaSetValues(quickKeyBox, XtNwidth, menuBoxWidth, NULL);

  /*
   * (ideally one should not assume any fixed numbers, but this will do 
   * for now).
   *
   * width of message box = label width
   *                      + 2*border width (2*1)
   *                      + defaultDistance (right) (1)
   *                      + horizDistance (left) (4)
   *
   * desired width of label = width of menu box
   *                        - hSpace of menu box (2*4) (to align with buttons)
   *                        - border with of label (2*1)
   *                        = width of menu box - 10
   *
   * => width of message box = width of menu box - 10 + 7
   * => width of message box < width of menu box
   * => menu box width is the controlling factor in deciding the widnow
   *    width, as desired.
   */
  XtVaSetValues(statusMessage, XtNresizable, True, NULL);
  XtVaSetValues(statusMessage, XtNwidth, menuBoxWidth - 10, NULL);
  XtVaSetValues(statusMessage, XtNresizable, False, NULL);

  XtSetMappedWhenManaged(topLevelWidget, True);
  XtMapWidget(topLevelWidget);	/* I don't know why I need this, but I do */
  wm_delete_window = XInternAtom(XtDisplay(topLevelWidget), 
								 "WM_DELETE_WINDOW", False);
  XSetWMProtocols(XtDisplay(topLevelWidget), XtWindow(topLevelWidget),
				  &wm_delete_window, 1);

  IdleGuard();
  if (qres.showFunMessages)
    XtAppAddTimeOut(app_con, qres.funMessagesInterval * 1000, FunMessage,
					(XtPointer) app_con);

  return dialWidget;
}

void
TopAbout(parent)
     Widget          parent;
{
  Widget          popup,
                  mBox,
                  uBox,
                  lBox,
                  pic,
                  msg,
                  caption;
  Pixmap          pix;
  Dimension       width1,
                  width2;
  char            msgStr[LRG_BUF];
#include "authPic.h"

  popup = SeAddPopup("about", parent);
  mBox = SeAddPaned("mBox", popup);
  uBox = SeAddBox("uBox", mBox);
  lBox = SeAddBox("lBox", mBox);

  msg = SeAddLabel("msg", uBox);
  pic = SeAddLabel("pic", uBox);
  caption = SeAddLabel("caption", uBox);

  sprintf(msgStr, "%s %s rev. %s\n%s\n%s\n%s", "Seyon version", VERSION,
	  REVISION, "Copyright 1992-1993", "(c) Muhammad M. Saggaf",
	  "All rights reserved");
  XtVaSetValues(msg, XtNlabel, msgStr, NULL);

  pix = XCreateBitmapFromData(XtDisplay(pic),
			      DefaultRootWindow(XtDisplay(pic)),
			      authPic_bits, authPic_width,
			      authPic_height);
  XtVaSetValues(pic, XtNbitmap, pix, NULL);

  width1 = SeWidgetWidth(msg);
  width2 = SeWidgetWidth(pic);
  width1 = width1 > width2 ? width1 : width2;
  width2 = SeWidgetWidth(caption);
  width1 = width1 > width2 ? width1 : width2;

  XtVaSetValues(msg, XtNwidth, width1, NULL);
  XtVaSetValues(pic, XtNwidth, width1, NULL);
  XtVaSetValues(caption, XtNwidth, width1, NULL);

  SeAddButton("dismiss", lBox, DestroyShell);

  XtPopupSpringLoaded(popup);
}

/*
 * TopHelp: displays the help file.
 */

void
TopHelp(widget)
     Widget          widget;
{
  Widget          displayPopup;

  XtVaSetValues(widget, XtNsensitive, False, NULL);
  displayPopup = DisplayFile(widget, qres.helpFile);
  XtAddCallback(displayPopup, XtNdestroyCallback, SetSensitiveOn, widget);

  PositionShell(displayPopup, widget, SHELLPOS_HWFH);
  XtPopup(displayPopup, XtGrabNone);
}

void
ExecHangup()
{
  hangup();
  SeyonMessage("Line Disconnected");
}

void
DoHangup(widget)
     Widget          widget;
{
  DestroyShell(widget);
  ExecHangup();
}

void
HangupConfirm(widget)
     Widget          widget;
{
  Widget          popup,
                  dialog;

  ret_if_up(widget, inhibit_child);

  if (qres.hangupConfirm) {
	popup = AddSimplePopup("hangup", widget);
	dialog = SeAddDialog("dialog", popup);
	
	XawDialogAddButton(dialog, "yes", DoHangup, (XtPointer) dialog);
	XawDialogAddButton(dialog, "cancel", DestroyShell, NULL);
	
	PopupCentered(popup, widget);
  }
  else
	ExecHangup();
}

void
ExitNoHangup(widget)
     Widget          widget;
{
  DestroyShell(widget);
  s_exit(widget);
}

void
ExitHangup(widget)
     Widget          widget;
{
  ExecHangup();
  ExitNoHangup(widget);
}

void
ExitConfirm(widget)
     Widget          widget;
{
  Widget          popup,
                  dialog;

  if (qres.exitConfirm && !qres.ignoreModemDCD && Online()) {
	popup = AddSimplePopup("exit", widget);
	dialog = SeAddDialog("dialog", popup);
	
	XawDialogAddButton(dialog, "yes", ExitHangup, NULL);
	XawDialogAddButton(dialog, "no", ExitNoHangup, NULL);
	XawDialogAddButton(dialog, "cancel", DestroyShell, NULL);
	
	PopupCentered(popup, widget);
  }
  else
	s_exit();
}

void
ExitAction(widget)
     Widget          widget;
{
  Boolean         wExitButtonStatus;

  /* Prevent the user from exiting the program by f.delete if exiting
	 is not permitted */
  XtVaGetValues(w_exit, XtNsensitive, &wExitButtonStatus, NULL);
  ret_if_up(widget, !wExitButtonStatus);
  s_exit();
}

void
w_exit_up(w_exit_status)
     Boolean         w_exit_status;
{
  XtVaSetValues(w_exit, XtNsensitive, w_exit_status, NULL);
}

void
w_kill_up(w_kill_status)
     Boolean         w_kill_status;
{
  XtVaSetValues(w_kill, XtNsensitive, w_kill_status, NULL);
  w_exit_up(!w_kill_status);
}

void
kill_w_child()
{
  if (w_child_pid) {
    kill(w_child_pid, SIGTERM);
    w_child_pid = 0;
  }
}

void
GetResources(topLevelWidget)
     Widget          topLevelWidget;
{
#define offset(field) XtOffsetOf(struct QueryResources, field)

  static XtResource resources[] = {
    {"modem", "Modem", XtRString, sizeof(String),
	   offset(modem), XtRString, (XtPointer) NULL},
    {"script", "Script", XtRString, sizeof(String),
	   offset(script), XtRString, (XtPointer) NULL},
	
    {"defaultBPS", "DefaultBPS", XtRString, sizeof(String),
	   offset(defaultBPS), XtRString, (XtPointer) "9600"},
    {"defaultBits", "DefaultBits", XtRInt, sizeof(int),
	   offset(defaultBits), XtRImmediate, (XtPointer) 8},
    {"defaultParity", "DefaultParity", XtRInt, sizeof(int),
	   offset(defaultParity), XtRImmediate, (XtPointer) 0},
    {"defaultStopBits", "DefaultStopBits", XtRInt, sizeof(int),
	   offset(defaultStopBits), XtRImmediate, (XtPointer) 1},
    {"stripHighBit", "StripHighBit", XtRBoolean, sizeof(Boolean),
	   offset(stripHighBit), XtRImmediate, (XtPointer) False},
    {"backspaceTranslation", "BackspaceTranslation", XtRBoolean,
	   sizeof(Boolean), offset(backspaceTranslation), XtRImmediate,
	   (XtPointer) False},
    {"metaKeyTranslation", "MetaKeyTranslation", XtRBoolean,
	   sizeof(Boolean), offset(metaKeyTranslation), XtRImmediate,
	   (XtPointer) True},
    {"xonxoffFlowControl", "XonxoffFlowControl", XtRBoolean,
	   sizeof(Boolean), offset(xonxoffFlowControl), XtRImmediate,
	   (XtPointer) False},
    {"rtsctsFlowControl", "RtsctsFlowControl", XtRBoolean,
	   sizeof(Boolean), offset(rtsctsFlowControl), XtRImmediate,
	   (XtPointer) False},
    {"newlineTranslation", "NewlineTranslation", XtRString,
	   sizeof(String), offset(newlineTranslation), XtRImmediate,
	   (XtPointer) "cr"},

    {"dialPrefix", "DialPrefix", XtRString, sizeof(String),
	   offset(dialPrefix), XtRString, (XtPointer) "ATDT"},
    {"dialSuffix", "DialSuffix", XtRString, sizeof(String),
	   offset(dialSuffix), XtRString, (XtPointer) "^M"},
    {"dialCancelString", "DialCancelString", XtRString, sizeof(String),
	   offset(dialCancelString), XtRString, (XtPointer) "^M"},

    {"dialTimeOut", "DialTimeOut", XtRInt, sizeof(int),
	   offset(dialTimeOut), XtRImmediate, (XtPointer) 45},
    {"dialDelay", "DialDelay", XtRInt, sizeof(int),
	   offset(dialDelay), XtRImmediate, (XtPointer) 10},
    {"dialRepeat", "DialRepeat", XtRInt, sizeof(int),
	   offset(dialRepeat), XtRImmediate, (XtPointer) 5},

    {"connectString", "ConnectString", XtRString, sizeof(String),
	   offset(connectString), XtRString, (XtPointer) "CONNECT"},
    {"noConnectString1", "NoConnectString1", XtRString, sizeof(String),
	   offset(noConnectString[0]), XtRString, (XtPointer) "NO CARRIER"},
    {"noConnectString2", "NoConnectString2", XtRString, sizeof(String),
	   offset(noConnectString[1]), XtRString, (XtPointer) "NO DIALTONE"},
    {"noConnectString3", "NoConnectString3", XtRString, sizeof(String),
	   offset(noConnectString[2]), XtRString, (XtPointer) "BUSY"},
    {"noConnectString4", "NoConnectString4", XtRString, sizeof(String),
	   offset(noConnectString[3]), XtRString, (XtPointer) "VOICE"},

    {"hangupBeforeDial", "HangupBeforeDial", XtRBoolean, sizeof(Boolean),
	   offset(hangupBeforeDial), XtRImmediate, (XtPointer) True},
	{"dialDirAutoOpen", "DialDirAutoOpen", XtRBoolean, sizeof(Boolean),
	   offset(dialDirAutoOpen), XtRImmediate, (XtPointer) False},
    {"dialAutoStart", "DialAutoStart", XtRBoolean, sizeof(Boolean),
	   offset(dialAutoStart), XtRImmediate, (XtPointer) False},
    {"dialDirAutoClose", "DialDirAutoClose", XtRBoolean, sizeof(Boolean),
	   offset(dialDirAutoClose), XtRImmediate, (XtPointer) False},
    {"dialDirAutoIconify", "DialDirAutoIconify", XtRBoolean, sizeof(Boolean),
	   offset(dialDirAutoIconify), XtRImmediate, (XtPointer) False},
    {"beepOnConnect", "BeepOnConnect", XtRBoolean, sizeof(Boolean),
	   offset(beepOnConnect), XtRImmediate, (XtPointer)True},
    {"dialDirFormat", "DialDirFormat", XtRString, sizeof(String),
	   offset(dialDirFormat), XtRString,
	   (XtPointer)"%-15s %-15s %6s %1c%1c%1c %1c%1c %s"},
    {"defaultPhoneEntries", "DefaultPhoneEntries", XtRString, sizeof(String),
	   offset(defaultPhoneEntries), XtRString, (XtPointer) NULL},

    {"modemVMin", "ModemVMin", XtRInt, sizeof(int),
	   offset(modemVMin), XtRImmediate, (XtPointer) 1},
    {"ignoreModemDCD", "IgnoreModemDCD", XtRBoolean, sizeof(Boolean),
	   offset(ignoreModemDCD), XtRImmediate, (XtPointer) False},
    {"hayesHangup", "hayesHangup", XtRBoolean, sizeof(Boolean),
	   offset(hayesHangup), XtRImmediate, (XtPointer) False},
    {"hangupConfirm", "HangupConfirm", XtRBoolean, sizeof(Boolean),
	   offset(hangupConfirm), XtRImmediate, (XtPointer) True},
    {"exitConfirm", "ExitConfirm", XtRBoolean, sizeof(Boolean),
	   offset(exitConfirm), XtRImmediate, (XtPointer) True},

    {"zmodemAutoDownload", "ZmodemAutoDownload", XtRBoolean,
	   sizeof(Boolean), offset(zmodemAutoDownload), XtRImmediate,
	   (XtPointer) True},
    {"zmodemAutoDownloadCommand", "ZmodemAutoDownloadCommand",
	   XtRString, sizeof(String), offset(zmodemAutoDownloadCommand),
	   XtRString, (XtPointer) "$rz"},

    {"defaultDirectory", "DefaultDirectory", XtRString, sizeof(String),
	   offset(defaultDirectory), XtRString, (XtPointer) "~/.seyon"},
    {"scriptDirectory", "scriptDirectory", XtRString, sizeof(String),
	   offset(scriptDirectory), XtRString, (XtPointer) NULL},
    {"startupFile", "StartupFile", XtRString, sizeof(String),
	   offset(startupFile), XtRString, (XtPointer) "startup"},
    {"phoneFile", "PhoneFile", XtRString, sizeof(String),
	   offset(phoneFile), XtRString, (XtPointer) "phonelist"},
    {"protocolsFile", "ProtocolsFile", XtRString, sizeof(String),
	   offset(protocolsFile), XtRString, (XtPointer) "protocols"},
    {"captureFile", "CaptureFile", XtRString, sizeof(String),
	   offset(captureFile), XtRString, (XtPointer) "capture"},
    {"helpFile", "HelpFile", XtRString, sizeof(String),
	   offset(helpFile), XtRString, (XtPointer) HELPFILE},

    {"modemStatusInterval", "ModemStatusInterval", XtRInt, sizeof(int),
	   offset(modemStatusInterval), XtRImmediate, (XtPointer) 5},

    {"idleGuard", "IdleGuard", XtRBoolean, sizeof(Boolean),
	   offset(idleGuard), XtRImmediate, (XtPointer) False},
    {"idleGuardInterval", "IdleGuardInterval", XtRInt, sizeof(int),
	   offset(idleGuardInterval), XtRImmediate, (XtPointer) 300},
    {"idleGuardString", "IdleGuardString", XtRString, sizeof(String),
	   offset(idleGuardString), XtRImmediate, (XtPointer) " ^H"},

    {"showFunMessages", "ShowFunMessages", XtRBoolean, sizeof(Boolean),
	   offset(showFunMessages), XtRImmediate, (XtPointer) True},
    {"funMessagesInterval", "FunMessagesInterval", XtRInt, sizeof(int),
	   offset(funMessagesInterval), XtRImmediate, (XtPointer) 15},
    {"funMessages", "FunMessages", XtRStringArray, sizeof(String*),
	   offset(funMessages), XtRStringArray, (XtPointer) NULL},

    {"quickKey1", "quickKey1", XtRString, sizeof(String),
	   offset(quickKey[0]), XtRImmediate, (XtPointer)""},
    {"quickKey2", "quickKey2", XtRString, sizeof(String),
	   offset(quickKey[1]), XtRImmediate, (XtPointer)""},
    {"quickKey3", "quickKey3", XtRString, sizeof(String),
	   offset(quickKey[2]), XtRImmediate, (XtPointer)""},
    {"quickKey4", "quickKey4", XtRString, sizeof(String),
	   offset(quickKey[3]), XtRImmediate, (XtPointer)""},
    {"quickKey5", "quickKey5", XtRString, sizeof(String),
	   offset(quickKey[4]), XtRImmediate, (XtPointer)""},

    {"quickKey6", "quickKey6", XtRString, sizeof(String),
	   offset(quickKey[5]), XtRImmediate, (XtPointer)""},
    {"quickKey7", "quickKey7", XtRString, sizeof(String),
	   offset(quickKey[6]), XtRImmediate, (XtPointer)""},
    {"quickKey8", "quickKey8", XtRString, sizeof(String),
	   offset(quickKey[7]), XtRImmediate, (XtPointer)""},
    {"quickKey9", "quickKey9", XtRString, sizeof(String),
	   offset(quickKey[8]), XtRImmediate, (XtPointer)""},
    {"quickKey10", "quickKey10", XtRString, sizeof(String),
	   offset(quickKey[9]), XtRImmediate, (XtPointer)""},
  };

#undef offset

  XtSetTypeConverter(XtRString, XtRStringArray, CvtStringToStringArray,
					 NULL, 0, XtCacheNone, NULL);

  XtGetApplicationResources(topLevelWidget, (XtPointer)&qres, resources,
							XtNumber(resources), NULL, 0);
}
