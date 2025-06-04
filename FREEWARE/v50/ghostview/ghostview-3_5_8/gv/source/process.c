/*
**
** process.c
**
** Copyright (C) 1996, 1997 Johannes Plass
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
** 
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
*/

/*
#define MESSAGES
*/
#include "message.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef VMS
#   include <processes.h>
#   include <jpidef.h>
#   include <signal.h>
#   include <lib$routines.h>
#   include <ssdef.h>
#   include <types.h>
#else
#   include <sys/types.h>
#   include <sys/wait.h>
#   include <signal.h>
#   include <unistd.h>
#endif

#include "paths.h"
#include INC_X11(Intrinsic.h)
#include INC_X11(StringDefs.h)
#include INC_XAW(MenuButton.h)
#include INC_XAW(SimpleMenu.h)
#include INC_XAW(SmeBSB.h)
#include INC_X11(IntrinsicP.h)
#include INC_X11(ShellP.h)

#include "types.h"
#include "config.h"
#include "callbacks.h"
#include "d_memdebug.h"
#include "process.h"
#include "main_resources.h"
#include "main_globals.h"
#include "misc_private.h"

#define CHECK_PERIOD 500
#define ADD_TIMEOUT(widget,interval,data)       \
        XtAppAddTimeOut (                       \
                  app_con,                      \
                  ((unsigned long)(interval)),  \
                  process_notify,               \
                  ((XtPointer)(data))           \
                )

#define DISABLED	((XtIntervalId) 0)
#define DESTROY_TIMER(aaa)			\
	if ((aaa)) {				\
		XtRemoveTimeOut((aaa));		\
		aaa = DISABLED;			\
	}

static ProcessData gpd = NULL;

/*------------------------------------------------------------*/
/* process_get_pd */
/*------------------------------------------------------------*/

static ProcessData process_get_pd()
{
   ProcessData pd;
   Cardinal size = sizeof(ProcessDataStruct);
   BEGINMESSAGE(process_get_pd)
#  ifdef MESSAGES
      if (!gpd) { INFMESSAGE(no processes registered yet) }
#  endif
   pd = (ProcessData) PROC_XtMalloc(size);
   memset((void*) pd ,0,(size_t)size);
   if (!gpd) gpd = pd;
   else {
      ProcessData tmppd;
      tmppd = gpd; while (tmppd->next) tmppd = tmppd->next;
      tmppd->next=pd;
   }
   ENDMESSAGE(process_get_pd)
   return(pd);
}

/*------------------------------------------------------------*/
/* process_remove_pd */
/*------------------------------------------------------------*/

static void process_remove_pd(pd)
   ProcessData pd;
{
   BEGINMESSAGE(process_remove_pd)
   if (gpd == pd) gpd = pd->next;
   else {
      ProcessData tmppd = gpd;
      while (tmppd->next != pd) tmppd=tmppd->next;
      tmppd->next = pd->next;
   }
   process_menu(pd,PROCESS_MENU_DEL_ENTRY);
   DESTROY_TIMER(pd->timer)
   PROC_XtFree(pd->name);
   PROC_XtFree(pd);
#  ifdef MESSAGES
      if (!gpd) { INFMESSAGE(no more processes registered) }
#  endif
   ENDMESSAGE(process_remove_pd)
}

/*------------------------------------------------------------*/
/* process_child_status */
/*------------------------------------------------------------*/

#define CHILD_UNKNOWN_STATUS -2
#define CHILD_ERROR          -1
#define CHILD_OKAY            0
#define CHILD_EXITED          1

static int process_child_status(pd)
   ProcessData pd;
{
   int status;
   pid_t child_pid;

   BEGINMESSAGE(process_child_status)
#  ifdef VMS
   {
      unsigned long ret;
      long item_code = JPI$_LOGINTIM;
      long res_val[2];
      long pid=(long)pd->pid;
      ret = lib$getjpi(&item_code,&pid,NULL,&(res_val[0]),NULL,NULL);
      IMESSAGE((int)ret)
      if (ret==SS$_NORMAL)
         { status=CHILD_OKAY; INFMESSAGE(SS$_NORMAL handled as CHILD_OKAY) }
      else if (ret==SS$_NONEXPR)
         { status=CHILD_EXITED; INFMESSAGE(SS$_NONEXPR handled as CHILD_EXITED) }
      else
         { status=CHILD_OKAY; INFMESSAGE(unknown return value handled as CHILD_OKAY) }
/*
         { status=CHILD_ERROR; INFMESSAGE(WARNING: returning CHILD_ERROR) }
*/
   }
#  else
      child_pid = waitpid(pd->pid,NULL,WNOHANG);
      if      (child_pid==pd->pid) { status=CHILD_EXITED;         INFMESSAGE(CHILD_EXITED) }
      else if (child_pid==0)       { status=CHILD_OKAY;           INFMESSAGE(CHILD_OKAY) }
      else if (child_pid==-1)      { status=CHILD_ERROR;          INFMESSAGE(CHILD_ERROR) }
      else                         { status=CHILD_UNKNOWN_STATUS; INFMESSAGE(CHILD_UNKNOWN_STATUS) }
#  endif
   ENDMESSAGE(process_child_status)
   return(status);
}


/*##############################################################*/
/* cb_processKillProcess */
/*##############################################################*/

void
cb_processKillProcess(w, client_data, call_data)
   Widget w;
   XtPointer client_data, call_data;
{
   ProcessData pd;
   BEGINMESSAGE(cb_processKillProcess)
   pd = (ProcessData)client_data;
   process_kill_process(pd);
   ENDMESSAGE(cb_processKillProcess)
}

/*##############################################################*/
/* process_kill_process */
/*##############################################################*/

void process_kill_process(pd)
   ProcessData pd;
{
   int status;
   BEGINMESSAGE(process_kill_process)
   kill(pd->pid, SIGTERM);
   status=process_child_status(pd);
   if (status==CHILD_OKAY) {
      INFMESSAGE(waiting for child to exit);
      wait(NULL);
   }
   (*(pd->notify_proc))(pd->data,PROCESS_KILL);
   process_remove_pd(pd);
   ENDMESSAGE(process_kill_process)
}

/*##############################################################*/
/* process_kill_all_processes */
/*##############################################################*/

void process_kill_all_processes()
{
   BEGINMESSAGE(process_kill_all_processes)
   while (gpd) process_kill_process(gpd);
   ENDMESSAGE(process_kill_all_processes)
}

/*------------------------------------------------------------*/
/* process_notify */
/*------------------------------------------------------------*/

static void process_notify (client_data, idp)
   XtPointer client_data;
   XtIntervalId *idp;
{
   pid_t child_pid;
   ProcessData pd = (ProcessData) client_data;

   BEGINMESSAGE(process_notify)
   child_pid = process_child_status(pd);
   if (child_pid==0) {
      INFMESSAGE(child did not exit yet)
      pd->timer = ADD_TIMEOUT(toplevel,CHECK_PERIOD,client_data);
      process_menu(pd,PROCESS_MENU_PROGRESS);
   } else {
      INFMESSAGE(calling notify procedure)
      (*(pd->notify_proc))(pd->data,PROCESS_NOTIFY);
      process_remove_pd(pd);
   }
   ENDMESSAGE(process_notify)
}

/*##############################################################*/
/* process_fork */
/*##############################################################*/

ProcessData process_fork (name,command,notify_proc,data)
   String name;
   String command;
   ProcessNotifyProc notify_proc;
   XtPointer data;
{
   ProcessData pd;
   pid_t       pid;

   BEGINMESSAGE(process_fork)

   pd  = process_get_pd();
#if defined(VMS)
#   define fork vfork
#endif
   pid = fork();

   if (pid == 0) { /* child */
      int  argc=0;
      char *argv[20];
      char *c;

      INFMESSAGE(child process)
      c = command;
      SMESSAGE(c)

#ifdef VMS
      /*  For some reason the combination vfork()/system() works on VMS.
       *  But why isn't it documented somewhere ??? Or is it ???
       */
      system(c);
#else
      while (isspace(*c)) c++;
      while (*c) {
         argv[argc++] = c;
         while (*c && !isspace(*c)) c++;
         if (*c) *c++ = '\0';
         while (isspace(*c)) c++;
         SMESSAGE(argv[argc-1])
      }
      argv[argc] = NULL;

      INFMESSAGE(spawning conversion process)
/*
      if (!freopen("/dev/null", "w", stdout)) perror("/dev/null");
*/
      if (!freopen("/dev/null", "r", stdin))  perror("/dev/null");

      execvp(argv[0], argv);
#endif

      {
         char tmp[512];
         sprintf(tmp, "Exec of %s failed", argv[0]);
         perror(tmp);
         _exit(EXIT_STATUS_ERROR);
      }
   }
   INFMESSAGE(parent process)
   pd->name        = PROC_XtNewString(name);
   pd->notify_proc = notify_proc;
   pd->data        = data;
   pd->pid         = pid;
   pd->timer       = ADD_TIMEOUT(toplevel,CHECK_PERIOD,pd);

   process_menu(pd,PROCESS_MENU_ADD_ENTRY);

   ENDMESSAGE(process_fork)
   return(pd);
}

/*------------------------------------------------------------*/
/* process_set_shell_resize */
/*------------------------------------------------------------*/

static Boolean process_set_shell_resize(allow_resize)
   Boolean allow_resize;
{
   Boolean old_allow_resize;
   ShellWidget sw = (ShellWidget)toplevel;
   old_allow_resize = sw->shell.allow_shell_resize;
   sw->shell.allow_shell_resize=allow_resize;
   return(old_allow_resize);
}

/*##############################################################*/
/* process_menu */
/*##############################################################*/

void process_menu(pd,action)
   ProcessData pd;
   int action;
{

   Arg args[5];
   Cardinal n;
   static int visible=1;
   static int progress=0;

   if (action==PROCESS_MENU_HIDE) {
      INFMESSAGE(PROCESS_MENU_HIDE)
      if (visible) {
         Boolean allow_resize;

         allow_resize=process_set_shell_resize(False);
   					        n=0;
         XtSetArg(args[n], XtNwidth,   0);	n++;
         XtSetArg(args[n], XtNheight,  0);	n++;
         XtSetValues(processButton,args,n);

         process_set_shell_resize(allow_resize);

         visible=0;
         progress=0;
      }
   }
   else if (action==PROCESS_MENU_SHOW) {
      INFMESSAGE(PROCESS_MENU_SHOW)
      if (!visible) {
         String label;
         Boolean allow_resize;

         allow_resize=process_set_shell_resize(False);

                                                n=0;
         XtSetArg(args[n], XtNlabel,  &label);	n++;
         XtGetValues(processButton,args,n);
         label=PROC_XtNewString(label);
         SMESSAGE(label)

                                                n=0;
         XtSetArg(args[n], XtNresize,  True);	n++;
         XtSetArg(args[n], XtNlabel,   "");	n++;
         XtSetValues(processButton,args,n);

     					        n=0;
         XtSetArg(args[n], XtNlabel,   label);	n++;
         XtSetValues(processButton,args,n);
   					        n=0;
         XtSetArg(args[n], XtNresize,  False);	n++;
         XtSetValues(processButton,args,n);
         PROC_XtFree(label);

         process_set_shell_resize(allow_resize);
         visible=1; 
         progress=0;
      }
   }
   else if (action==PROCESS_MENU_ADD_ENTRY) {
      Widget entry;
      char label[512];
      INFMESSAGE(PROCESS_MENU_ADD_ENTRY)
      if (!processMenu) {
                                                n=0;
         processMenu = XtCreatePopupShell("menu",
                       simpleMenuWidgetClass,processButton,args,n);
      }
      sprintf(label,"Stop %s",pd->name);

          					n=0;
         XtSetArg(args[n], XtNlabel, label);    n++;      
      entry = XtCreateManagedWidget("aaa", smeBSBObjectClass,processMenu,args,n);
      XtAddCallback(entry, XtNcallback, cb_processKillProcess, (XtPointer)pd);
      pd->menuentry  = entry;
      process_menu(NULL,PROCESS_MENU_SHOW);
      process_menu(pd,PROCESS_MENU_PROGRESS);
   }
   else if (action==PROCESS_MENU_DEL_ENTRY) {
      INFMESSAGE(PROCESS_MENU_DEL_ENTRY)
      if (!gv_exiting) {
         if (!gpd) {
            INFMESSAGE(destroying processMenu)
            XtDestroyWidget(processMenu);
            processMenu=NULL;
            process_menu(NULL,PROCESS_MENU_HIDE);
         }
         else {
            INFMESSAGE(destroying menu entry)
            XtDestroyWidget(pd->menuentry);
         }
      }
   }
   else if (action==PROCESS_MENU_PROGRESS) {
      INFMESSAGE(PROCESS_MENU_PROGRESS)
      if (visible) {
         char *label;
         char *tmp;
         size_t len;
                                                n=0;
         XtSetArg(args[n], XtNlabel,  &label);	n++;
         XtGetValues(processButton,args,n);
         len = strlen(label);
         tmp = (char*) PROC_XtMalloc(len*sizeof(char)+1);
         strcpy(tmp,&(label[progress]));
         if (progress) {
            if (progress>(int)len) progress=(int)len;
            strncpy(&(tmp[(int)len-progress]),label,(size_t)progress);
         }
         tmp[len]='\0';
         progress++;
         if (progress==(int)len+1) progress=0;
         update_label(processButton,tmp);
         PROC_XtFree(tmp);
      }
   }
}

/*##############################################################*/
/* process_disallow_quit */
/*##############################################################*/

char *process_disallow_quit()
{
# define MAX_DISALLOW_QUIT_MESSAGE 512
  static char message[MAX_DISALLOW_QUIT_MESSAGE];
  ProcessData pd;
  int l;

  BEGINMESSAGE(process_disallow_quit)
  if (!gpd) {
    ENDMESSAGE(process_disallow_quit)
    return NULL;
  }
  strcpy(message,"Still in progress:");
  l = strlen(message);
  for (pd = gpd; pd ; pd = pd->next) {
      l = l + strlen(pd->name) + 1;
      if (l + 10 < MAX_DISALLOW_QUIT_MESSAGE) { 
        strcat(message,"\n");
	strcat(message,pd->name);
      } else break;
  };
  if (pd) strcat(message,"\n...");
  ENDMESSAGE(process_disallow_quit)
  return message;
}

