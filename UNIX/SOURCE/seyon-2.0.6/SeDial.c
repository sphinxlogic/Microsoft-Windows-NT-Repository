
/*
 * This file is part of the Seyon, Copyright (c) 1992-1993 by Muhammad M.
 * Saggaf. All rights reserved.
 *
 * See the file COPYING (1-COPYING) or the manual page seyon(1) for a full
 * statement of rights and permissions for this program.
 */

#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Viewport.h>
#include "MultiList.h"

#include "seyon.h"
#include "SeDecl.h"
#include "SeSig.h"

extern char    *get_word();
extern int      IconifyShell(),
                MdmReadLine(),
                MdmTimedWaitFor();
extern int      ConvertStringToIntArray();

int             ReadParsePhoneFile();
void            DoDial(),
                DismissDirectory(),
                DialerEnd(),
                dial_timer_handler(),
                HighlightItems(),
                ClearAllItems(),
                ManualDial(),
                DoManualDial(),
                ReReadPhoneFile();

struct _ddItem {
  char            number[LIT_BUF];
  char            name[LIT_BUF];
  char            baud[10];
  int             bits;
  int             parity;
  int             stopBits;
  char            prefix[LIT_BUF];
  char            suffix[LIT_BUF];
  char            script[SM_BUF];
};

struct _ddItem *ddItems[MAX_ENT];
Boolean         dialDirUp = False,
                manualDial = False;
char           *script_file,
                phone_number[SM_BUF] = "",
                dialMsg[SM_BUF];
int             dialTime,
                dialTry,
                current_item,
                ddCurItemIndex;
jmp_buf         dial_env;
XfwfMultiListWidget mlw;

/*
 * TopDial: the top routine for the dialing directory.
 */

void
TopDial(widget, clientData)
     Widget          widget;
	 XtPointer       clientData;
{
  Widget          form, mBox, uBox, lBox, view, list;
  static Widget   popup;
  static char     phoneFile[REG_BUF];
  static String   disItems[MAX_ENT + 1] = {NULL};

  if (clientData == NULL && dialDirUp)
	SimpleError("Directory is Already Open");

  if (disItems[0] == NULL) {
    strcpy(phoneFile, qres.phoneFile);
    if (ReadParsePhoneFile(phoneFile, disItems) < 0) return;

	form = XtParent(widget);
	popup = SeAddPopupWG("directory", widget, form, form, 0,
						 SeWidgetHeight(form), True, True);
	mBox = AddPaned("mBox", popup);
	uBox = AddBox("uBox", mBox);
	lBox = AddBox("lBox", mBox);
	
	view = XtCreateManagedWidget("view", viewportWidgetClass, uBox, NULL, 0);
	list = XtVaCreateManagedWidget("list", xfwfMultiListWidgetClass,
								   view, XtNlist, disItems, NULL);
	mlw = (XfwfMultiListWidget) list;
	SeSetViewportDimFromMultiList(view, list, 10);
	XtAddCallback(list, XtNcallback, DoDial, NULL);
	
	AddButton("dismiss", lBox, DismissDirectory, (XtPointer)&dialDirUp);
	AddButton("ok", lBox, DoDial, NULL);
	AddButton("manual", lBox, ManualDial, NULL);
	AddButton("clear", lBox, ClearAllItems, NULL);
	AddButton("default", lBox, HighlightItems, qres.defaultPhoneEntries);
	AddButton("edit", lBox, EditFile, (XtPointer)phoneFile);
	AddButton("reread", lBox, ReReadPhoneFile, (XtPointer)disItems);
  }

  if (!XtIsRealized(popup)) XtRealizeWidget(popup);

  if (clientData == NULL) {
	XtPopup(popup, XtGrabNone);
	dialDirUp = True;
	if (qres.dialAutoStart) {
	  qres.dialAutoStart = False;
	  HighlightItems(widget, qres.defaultPhoneEntries);
	  DoDial();
	}
  }
  else {
	  HighlightItems(widget, clientData);
	  DoDial();
	}
}

/*
 * DoDial: gets the first selected item and dials it.
 */

void
DoDial()
{
  static XfwfMultiListReturnStruct *item;

  ErrorIfBusy();

  if ((item = XfwfMultiListGetHighlighted(mlw))->num_selected == 0)
	SimpleError("No Item Selected");
  ddCurItemIndex = item->selected_items[(current_item = 0)];

  inhibit_child = True;
  manualDial = False;
  
  dialTry = 1;
  pre_process();
  exec_dial();
}

/*
 * executes the dialing as a forked process and communicates progress
 * to the main process
 */

void
dial_handler(
#if NeedFunctionPrototypes
	      int signo,
	      XtPointer client_data
#endif
)
{
#if defined(SUNOS_3) || defined(Mips)
  union wait      status;
#else
  int             status;
#endif
  Widget          dirWidget;

  XoAppIgnoreSignal(app_con, SIGCHLD);
  wait(&status);

#if defined(SUNOS_3) || defined(Mips)
  switch (status.w_retcode) {
#else
  switch (WEXITSTATUS(status)) {
#endif
  case 0:
	if (qres.beepOnConnect) Beep();
    SeyonMessage("Connected to Remote Host");
	UpdateStatusBox(NULL);

	if ((dirWidget = XtNameToWidget(dialWidget, "directory"))) {
	  RemoveCurrentItem();
	  if (qres.dialDirAutoClose) DismissDirectory((Widget)mlw);
	  else if (qres.dialDirAutoIconify) IconifyShell((Widget)mlw);
	  if (ddItems[ddCurItemIndex]->script[0] && !manualDial) {
		linkflag = 1;
		inhibit_child = False;
		ExecScript(ddItems[ddCurItemIndex]->script);
		return;
	  }
	}
    break;
  case 1:
    DialCirculate();
    return;
  case 2:
	Beep();
    SeyonMessage("Dial Aborted: Online");
    break;
  case 10:
    SeyonMessage("Dialing Canceled by User");
    break;
  }

  inhibit_child = False;
  post_process();
}

/*
 * signal handler for canceling the dialing process
 */

void
killdial_handler(
#if NeedFunctionPrototypes
		  int signo
#endif
)
{
  signal(SIGTERM, SIG_IGN);
  sleep(1);
  cancel_dial(0);
  exit(10);
}

void
exec_dial()
{
  static int retStatus;

  XoAppAddSignal(app_con, SIGCHLD, dial_handler, NULL);

  if ((w_child_pid = SeFork())) return;

  /* Child process */
  signal(SIGTERM, killdial_handler);
  retStatus = dial_number();
  exit(retStatus);
}

/*
 * ReReadPhoneFile: updates the dialing directory from the phonebook file.
 */

void
ReReadPhoneFile(widget, disItems)
     Widget          widget;
     XtPointer       disItems[];
{
  Widget          dirWidget = XtParent(GetShell(widget));

  ErrorIfBusy();

  FreeList(disItems);
  DestroyShell(widget);
  dialDirUp = False;
  TopDial(dirWidget, NULL);
}

/*
 * ManualDial: sets up the manual dialing popup.
 */

void
ManualDial(widget)
     Widget          widget;
{
  Widget          popup;
  
  ErrorIfBusy();
  popup = GetShell(PopupDialogGetValue("manual_dial", widget, DoManualDial, 
									   NULL, phone_number));
  PopupCentered(popup, widget);
}

/*
 * action proc for manual dialing
 */

void
manual_dial_action_ok(widget)
     Widget          widget;
{
  DoManualDial(widget);
}

/*
 * does manual dialing
 */

void
DoManualDial(widget)
     Widget          widget;
{
  void            ExecManualDial();
  Widget          dialog = XtParent(widget);
  char            phoneNumber[SM_BUF];

  strcpy(phoneNumber, XawDialogGetValueString(dialog));
  DestroyShell(dialog);
  ExecManualDial(XtParent(GetShell(widget)), phoneNumber);
}

void
ExecManualDial(widget, phoneNumber)
     Widget          widget;
     String          phoneNumber;
{
  inhibit_child = True;
  manualDial = True;

  strcpy(phone_number, phoneNumber);

  dialTry = 1;
  pre_process();
  exec_dial();
}

/*
 * DismissDirectory: dismisses the dialing directory.
 */

void
DismissDirectory(widget, clientData)
     Widget          widget;
	 XtPointer       clientData;
{
  *((Boolean*)clientData) = False;
  XtPopdown(GetShell(widget));
}

/*
 * ClearAllItems: clears all selected items.
 */

void
ClearAllItems()
{
  XfwfMultiListUnhighlightAll(mlw);
}

void
HighlightItems(widget, clientData)
     Widget          widget;
	 XtPointer       clientData;
{
  int dialEntries[MAX_ENT], i;

  ConvertStringToIntArray((String*)clientData, dialEntries);

  ClearAllItems();
  for (i = 0; dialEntries[i]; i++)
	XfwfMultiListHighlightItem(mlw, dialEntries[i] - 1);
}

/*
 * unhighlights (unselects) the current item
 */

void
RemoveCurrentItem()
{
  static XfwfMultiListReturnStruct *item;

  item = XfwfMultiListGetHighlighted(mlw);
  XfwfMultiListUnhighlightItem(mlw, item->selected_items[current_item]);
}

/*
 * circulates to the next selected item and dials it
 */

void
DialCirculate()
{
  static XfwfMultiListReturnStruct *item;

  if (manualDial)
    dialTry++;
  else {
    item = XfwfMultiListGetHighlighted(mlw);
    if (++current_item == item->num_selected) {
      current_item = 0;
      dialTry++;
    }
	ddCurItemIndex = item->selected_items[current_item];
  }

  if (dialTry > qres.dialRepeat) {
    SeyonMessage("Max Tries Exhausted");
    inhibit_child = False;
    post_process();
    return;
  }

  exec_dial();
}

void
GetStrField(raw, keyword, var, def)
     String          raw,
                     keyword,
                     var,
                     def;
{
  char           *ptr,
                  buf[REG_BUF],
                  wrd[REG_BUF];

  if ((ptr = (char *) strstr(raw, keyword)) != NULL) {
    ptr += strlen(keyword);
    if (strncmp(ptr, "CURRENT", 3)) {
      strcpy(buf, ptr);
      GetWord(buf, wrd);
      strcpy(var, wrd);
    }
    else
      strcpy(var, "CURRENT");
  }
  else
    strcpy(var, def);
}

void
GetIField(raw, keyword, var, def)
     String          raw,
                     keyword;
     int            *var,
                     def;
{
  char            svar[TIN_BUF],
                  sdef[TIN_BUF];

  sprintf(svar, "%d", *var);
  sprintf(sdef, "%d", def);

  GetStrField(raw, keyword, svar, sdef);

  if (strcmp(svar, "CURRENT"))
    *var = atoi(svar);
  else
    *var = 100;
}

/*
 * this routine actually does the dialing
 */

#define DIALALARM 5

int
dial_number()
{
  char            modemResponse[SM_BUF],
                  smBuf[SM_BUF],
                 *itemName,
                  dialString[REG_BUF];
  int             i,
                  k;

  if (setjmp(dial_env) != 0) {
    signal(SIGALRM, SIG_DFL);
    alarm(0);

	sprintf(smBuf, "%s: Sleeping (%ds)...", dialMsg, qres.dialDelay);
	ProcRequest(SET_MESSAGE, smBuf, "");
    cancel_dial(0);
    sleep(qres.dialDelay);

    return 1;
  }

  k = ddCurItemIndex;
  dialTime = qres.dialTimeOut;

  if (!manualDial) {

    itemName = ddItems[k]->name;
    sprintf(dialString, "\r%s %s%s", ddItems[k]->prefix, ddItems[k]->number,
	    ddItems[k]->suffix);

    if (mbaud(ddItems[k]->baud) < 0)
      se_warningf("invalid BPS value in dialing directory: %s",
		  ddItems[k]->baud, "", "");
    if (MdmSetGetCSize(ddItems[k]->bits) < 0)
      se_warningf("invalid BITS value in dialing directory: %d",
		  ddItems[k]->bits, "", "");
    if (MdmSetGetParity(ddItems[k]->parity) < 0)
      se_warningf("invalid PARITY value in dialing directory: %d",
		  ddItems[k]->parity, "", "");
    if (MdmSetGetStopBits(ddItems[k]->stopBits) < 0)
      se_warningf("invalid STOPB value in dialing directory: %d",
		  ddItems[k]->stopBits, "", "");
  }
  else {
    itemName = phone_number;
    sprintf(dialString, "\r%s %s%s", qres.dialPrefix, phone_number,
	    qres.dialSuffix);
  }

  if (dialTry == 1)
    sprintf(dialMsg, "Dialing %s", itemName);
  else
    sprintf(dialMsg, "Redialing(%d) %s", dialTry, itemName);

  ProcRequest(SET_MESSAGE, "Setting Up...", "");

  if (qres.hangupBeforeDial) hangup();

  if (!qres.ignoreModemDCD && Online())	return 2;


  MdmPutString("AT^M");
  MdmTimedWaitFor("OK", 5);

  MdmIFlush();
  sleep(1);

  sprintf(smBuf, "%s... %d", dialMsg, dialTime);
  ProcRequest(SET_MESSAGE, smBuf, "");

  MdmPutString(dialString);
  MdmPurge();

  signal(SIGALRM, dial_timer_handler);
  alarm(DIALALARM);

  while (1) {
    MdmReadLine(modemResponse);

    str_stripspc_copy(smBuf, qres.connectString);
    if (!strncmp(modemResponse, smBuf, strlen(smBuf))) {
      signal(SIGALRM, SIG_DFL);
      alarm(0);
	  showf("\r\n%s", modemResponse, "", "");
      return 0;
    }

    for (i = 0; i < 3; i++) {
      str_stripspc_copy(smBuf, qres.noConnectString[i]);
      if ((*smBuf) && (!strncmp(modemResponse, smBuf, strlen(smBuf)))) {
		strcpy(dialMsg, modemResponse);
		longjmp(dial_env, 1);
	  }
    } /* for... */
  }	/* while(1)... */
}

/*
 * alarm handler for the dialing timeout
 */

void
dial_timer_handler(dummy)
     int             dummy;
{
  char            smBuf[SM_BUF];

  sprintf(smBuf, "%s... %d", dialMsg, dialTime -= DIALALARM);
  ProcRequest(SET_MESSAGE, smBuf, "");

  if (dialTime > 0) {
    signal(SIGALRM, dial_timer_handler);
    alarm(DIALALARM);
  }
  else {
    signal(SIGALRM, SIG_DFL);
    alarm(0);
	strcpy(dialMsg, "TIMEOUT");
    longjmp(dial_env, 1);
  }
}

int
ReadParsePhoneFile(fname, disItems)
     String          fname;
     String          disItems[];
{
  FILE           *fp;
  String          rawItems[MAX_ENT + 1];
  char           *buf,
                 *sHold,
                  disItemsBuf[REG_BUF];
  int             i,
                  n,
                  iHold;

  if ((fp = open_file(fname, qres.defaultDirectory)) == NULL)
    return -1;

  ReadCommentedFile(fp, rawItems);
  fclose(fp);

  FreeList(ddItems);
  for (i = 0; (buf = rawItems[i]); i++) {

    /* Allocate the record */
    ddItems[i] = XtNew(struct _ddItem);

    /* Find the number */
    GetWord(buf, ddItems[i]->number);

    /* Find the name */
    GetWord((buf = lptr), ddItems[i]->name);

    /* Find other stuff */
    GetStrField(buf, "BPS=", ddItems[i]->baud, qres.defaultBPS);
    GetIField(buf, "BITS=", &(ddItems[i]->bits), qres.defaultBits);
    GetIField(buf, "PARITY=", &(ddItems[i]->parity), qres.defaultParity);
    GetIField(buf, "STOPB=", &(ddItems[i]->stopBits), qres.defaultStopBits);
    GetStrField(buf, "PREFIX=", ddItems[i]->prefix, qres.dialPrefix);
    GetStrField(buf, "SUFFIX=", ddItems[i]->suffix, qres.dialSuffix);
    GetStrField(buf, "SCRIPT=", ddItems[i]->script, "\000");
  }
  ddItems[i] = (struct _ddItem *)NULL;

  FreeList(rawItems);
  FreeList(disItems);

  for (n = 0; n < i; n++) {
    sprintf(disItemsBuf, qres.dialDirFormat,
	    ddItems[n]->name,
	    ddItems[n]->number,
	    strncmp((sHold = ddItems[n]->baud), "CUR", 3) ? sHold : "????",
	    (iHold = ddItems[n]->bits) == 100 ? '?' : itoa(iHold),
	    (iHold = ddItems[n]->parity) ? (iHold == 1 ? 'O' :
					    (iHold == 2 ? 'E' : '?')) : 'N',
	    (iHold = ddItems[n]->stopBits) == 100 ? '?' : itoa(iHold),
	    strncmp((sHold = ddItems[n]->prefix), "CUR", 3) ?
	    strcmp(sHold, qres.dialPrefix) ? 'P' : 'D' : '?',
	    strncmp((sHold = ddItems[n]->suffix), "CUR", 3) ?
	    strcmp(sHold, qres.dialSuffix) ? 'S' : 'D' : '?',
	    ddItems[n]->script);

    disItemsBuf[SM_BUF - 1] = '\0';
    disItems[n] = XtNewString(disItemsBuf);
  }
  disItems[n] = NULL;

  return 0;
}
