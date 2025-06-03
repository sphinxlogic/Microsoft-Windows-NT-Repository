
/*
 * This file is part of the Seyon, Copyright (c) 1992-1993 by Muhammad M.
 * Saggaf. All rights reserved.
 *
 * See the file COPYING (1-COPYING) or the manual page seyon(1) for a full
 * statement of rights and permissions for this program.
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <string.h>

/* SeDecl.h includes stdio.h */
#include "SeDecl.h"

#define CheckNumParam(num) {if (*numParam != num) \
	 SimpleError("Wrong Number of Parameters");}

extern Widget   dialWidget;

void
BeepAction(widget, event, param, numParam)
	 Widget          widget;
	 XEvent*         event;
	 String*         param;
	 Cardinal*       numParam;
{
  CheckNumParam(0);
  Beep();
}

void
DialEntriesAction(widget, event, param, numParam)
	 Widget          widget;
	 XEvent*         event;
	 String*         param;
	 Cardinal*       numParam;
{
  ErrorIfBusy();
  CheckNumParam(1);

  if (strcmp(*param, "Default")) TopDial(dialWidget, (XtPointer)*param);
  else TopDial(dialWidget, (XtPointer)qres.defaultPhoneEntries);
}

void
DivertFileAction(widget, event, param, numParam)
	 Widget          widget;
	 XEvent*         event;
	 String*         param;
	 Cardinal*       numParam;
{
  void            DivertFile(),
                  ExecDivertFile();
         
  ErrorIfBusy();
  if (*numParam > 1)
	SimpleError("Wrong Number of Parameters");

  if (*numParam == 0) DivertFile(widget);
  else ExecDivertFile(widget, *param);
}

void
EchoAction(widget, event, param, numParam)
	 Widget          widget;
	 XEvent*         event;
	 String*         param;
	 Cardinal*       numParam;
{
  ErrorIfBusy();
  CheckNumParam(1);
  show(*param);
}

void
FileTransferAction(widget, event, param, numParam)
	 Widget          widget;
	 XEvent*         event;
	 String*         param;
	 Cardinal*       numParam;
{
  void            TopTransfer();
  static String   pParam[2];
  int             i;

  ErrorIfBusy();
  if (*numParam < 1 || *numParam > 2)
	SimpleError("Wrong Number of Parameters");

  for (i = 0; i < *numParam;) 
	{pParam[i] = param[i]; pParam[++i] = NULL;}

  TopTransfer(widget, (XtPointer)pParam);
}

void
HangupAction(widget, event, param, numParam)
	 Widget          widget;
	 XEvent*         event;
	 String*         param;
	 Cardinal*       numParam;
{
  void            ExecHangup();

  ErrorIfBusy();
  CheckNumParam(0);
  ExecHangup();
}

void
ManualDialAction(widget, event, param, numParam)
	 Widget          widget;
	 XEvent*         event;
	 String*         param;
	 Cardinal*       numParam;
{
  void            ManualDial(),
                  ExecManualDial();

  ErrorIfBusy();
  if (*numParam > 1)
	SimpleError("Wrong Number of Parameters");

  if (*numParam == 0) ManualDial(widget);
  else ExecManualDial(widget, *param);
}

void
OpenDialDirAction(widget, event, param, numParam)
	 Widget          widget;
	 XEvent*         event;
	 String*         param;
	 Cardinal*       numParam;
{
  Widget          dirWidget;

  CheckNumParam(0);

  if ((dirWidget = XtNameToWidget(dialWidget, "directory")))
	{XtPopup(dirWidget, XtGrabNone); XtMapWidget(dirWidget);}
  else
	TopDial(dialWidget, NULL);
}

void
QuitAction(widget, event, param, numParam)
	 Widget          widget;
	 XEvent*         event;
	 String*         param;
	 Cardinal*       numParam;
{
  ErrorIfBusy();
  CheckNumParam(0);
  ExecExit();
}

void
RunScriptAction(widget, event, param, numParam)
	 Widget          widget;
	 XEvent*         event;
	 String*         param;
	 Cardinal*       numParam;
{
  void            RunScript();
           
  ErrorIfBusy();
  if (*numParam > 1)
	SimpleError("Wrong Number of Parameters");

  if (*numParam == 0) RunScript(widget);
  else ExecScript(*param);
}

void
SetAction(widget, event, param, numParam)
	 Widget          widget;
	 XEvent*         event;
	 String*         param;
	 Cardinal*       numParam;
{
  void            s_set();

  ErrorIfBusy();
  CheckNumParam(2);
  sprintf((lptr = line), "%s %s", param[0], param[1]);
  eof_flag = 0;
  s_set();
}

void
ShellCommandAction(widget, event, param, numParam)
	 Widget          widget;
	 XEvent*         event;
	 String*         param;
	 Cardinal*       numParam;
{
  ErrorIfBusy();
  CheckNumParam(1);
  ShellCommand(*param);
}

void
TransmitAction(widget, event, param, numParam)
	 Widget          widget;
	 XEvent*         event;
	 String*         param;
	 Cardinal*       numParam;
{
  ErrorIfBusy();
  CheckNumParam(1);
  MdmPutString(*param);
}
