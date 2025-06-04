/*
         %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
         %% \___________________________________%% \
         %% |                                   %%  \
         %% |              MiniSm               %%   \
         %% |           Tasks.c  c1995          %%    \
         %% |            Lyle W. West           %%    |
         %% |                                   %%    |
         %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    |
         \                                        \   |
          \                                        \  |
           \                                        \ |
            \________________________________________\|



     Copyright (C) 1995 Lyle W. West, All Rights Reserved.
     Permission is granted to copy and use this program so long as [1] this
     copyright notice is preserved, and [2] no financial gain is involved
     in copying the program.  This program may not be sold as "shareware"
     or "public domain" software without the express, written permission
     of the author.

*/

#include "includes.h"
#include "externs.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Forward/External Declarations
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void DispErrPopupCB ();
void ExceedTaskPopupCB ();
void SaveCB ();


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: AdjustAppsLst
 * Description: Rebuild the AscNewAppsLst structure after an Application 
 *              Definition has been removed
 *
 * Inputs: Offset into the AscNewAppslst
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void AdjustAppsLst(int item, int appscnt)
{
    int indx = item;
    APPSLST *AppsPtr1;
    APPSLST *AppsPtr2;

    AppsPtr1 = AscNewAppsList;
    AppsPtr2 = AscNewAppsList;
    AppsPtr1 += item + 1;
    AppsPtr2 += item;
    while(indx < appscnt) {
        strcpy(AppsPtr2->ApplName, AppsPtr1->ApplName);
        strcpy(AppsPtr2->ApplCmd, AppsPtr1->ApplCmd);
        indx++;
        AppsPtr2++;
        AppsPtr1++;
    }
    AppsPtr1 = AscNewAppsList;
    AppsPtr1 +=  appscnt - 2;
    memset(AppsPtr2->ApplName, 0, sizeof(APPSLST));
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: AdjustTaskList
 * Description: Rebuild the tasklist structure after a task has been
 *              deleted or has exited. 
 *
 * Inputs: Offset into the task list
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void AdjustTaskList(int procid)
{
    int indx = 0;
    int targetpid;
    TASKLST *TaskPtr1;
    TASKLST *TaskPtr2;

    TaskPtr1 = TaskStruct;  /* previous structure pointer */
    TaskPtr2 = TaskStruct;  /* new structure pointer */
#ifdef ARF
    TaskPtr1 += offset + 1;
    TaskPtr2 += offset;
    while(indx < num_tasks) {
        strcpy(TaskPtr2->TaskName, TaskPtr1->TaskName);
        TaskPtr2->TaskPid = TaskPtr1->TaskPid;
        indx++;
        TaskPtr2++;
        TaskPtr1++;
    }
    TaskPtr1 = TaskStruct;
    TaskPtr1 += num_tasks - 1;
    memset(TaskPtr1->TaskName, 0, sizeof(TASKLST));
    TaskPtr1->TaskPid = 0;
#else
    targetpid = procid;
    while(indx < num_tasks && indx < MAXTASKS) {
        if(TaskPtr1->TaskPid == targetpid) {
            TaskPtr2++;
            targetpid = -1;
        }
        strcpy(TaskPtr1->TaskName, TaskPtr2->TaskName);
        TaskPtr1->TaskPid = TaskPtr2->TaskPid;
        indx++;
        TaskPtr2++;
        TaskPtr1++;
    }
    memset(TaskPtr2->TaskName, 0, sizeof(TaskPtr2->TaskName));
    TaskPtr2->TaskPid = 0;
#endif

#ifdef ARF
    n = 0;
    XtSetArg(args[n], XmNwidth, 160); n++;
    XtSetValues(TaskList, args, n);
#endif
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: AstEvent
 * Description: Check if tasks listed in TaskPtr are still active
 *
 * Inputs: Target pid
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void AstEvent(int pid)
{
    int indx = 0;
    int status;
    int task_cnt;
    TASKLST *TaskPtr;

    TaskPtr = TaskStruct;
    task_cnt = num_tasks - 1;
    if(detached) {
        status = sys$setimr(0, &delta_time, AstEvent, 0, 0);
        return;
    }
    while(indx <= task_cnt) {
        status = CheckProcess(TaskPtr->TaskPid);
        if(!status) {
            XmListDeletePos(TaskList, indx+1);
            num_tasks--;
            AdjustTaskList(indx);
            XmUpdateDisplay(TaskList);
            if(!num_tasks) {
                XtSetSensitive(StopTaskBtn, FALSE);
                status = sys$cantim(0, 0);
            }
        }
        indx++;
        TaskPtr++;
    }
    if(num_tasks) status = sys$setimr(0, &delta_time, AstEvent, 0, 0);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: CheckProcess
 * Description: Check if task is  still active
 *
 * Inputs: Target pid
 *
 * Returns: TRUE if active, else FALSE
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int CheckProcess(int pid)
{
    int status, mpid;
    struct { short len, code; char *buf, *retlen; } itmlis[2];
    $DESCRIPTOR(dsc_delta, delta_str);

    itmlis[0].len    = 4;
    itmlis[0].code   = JPI$_MASTER_PID;
    itmlis[0].buf    = &mpid;
    itmlis[0].retlen = 0;
    itmlis[1].len    = 0;
    itmlis[1].code   = 0;

    if(newtask_count) {
        newtask_count--;
        if(!newtask_count) {
            sprintf(delta_str, "0 00:00:20.00");
            dsc_delta.dsc$w_length = strlen(delta_str);
            status = sys$bintim(&dsc_delta, &delta_time);
        }
    }
    status = sys$getjpiw(0, &pid, 0, &itmlis, 0, 0, 0);
    if(status == SS$_NORMAL) return(TRUE);
    else return(FALSE);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: CheckDetached
 * Description: MiniSm creates a task and assigns a process name which is
 *              the filename plus the last three hex digits of the master
 *              process id (MiniSm). This prohibits, by default, multiple
 *              occurances of a given task due to duplicate process names.
 *              However, limiting task instances to 1 is not acceptable
 *              with certain tasks (such as DecTerm). This routine references
 *              a process logical name equivalence string and looks for the
 *              taskname in that string. If found, create a unique taskname
 *              and return to caller.
 *
 * Inputs: Pointer to application list name selected
 *
 * Returns: Detached process status, or FALSE if not detached
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int CheckDetached(char *name)
{
    int count = 1;
    int done = FALSE;
    int indx;
    int match;
    char *bptr;
    char *eptr;
    char taskname[30];
    char detachedstr[30];
    TASKLST *TaskPtr;

#ifdef MSMDBG
    printf("task_detached: %s\n", task_detached);
#endif
    if(!strlen(task_detached)) return(FALSE);
    TaskPtr = TaskStruct;
    for(indx = 0; indx <= strlen(name); indx++)
        taskname[indx] = toupper(name[indx]);
    taskname[strlen(name)] = '\0';
#ifdef MSMDBG
    printf("CheckDetached: taskname = %s\n", taskname);
#endif
    bptr = strstr(task_detached, taskname);
    if(!bptr) return(FALSE);
    eptr = strchr(task_detached, '%');
    if(eptr) {
        bptr = eptr + 1;
        eptr = strchr(task_detached, ',');
        if(eptr) {
            match = eptr - bptr;
            strncpy(detachedstr, bptr, match);
            detachedstr[match] = '\0';
        }
        else strcpy(detachedstr, bptr);
    }
    else InvalidDetachedDef();
#ifdef MSMDBG
    printf("CheckDetached: detachedstr = %s\n", detachedstr);
#endif
    while(done == FALSE && count <= 9) {
        sprintf(taskname,"%s%d", name, count);
#ifdef MSMDBG
        printf("CheckDetached: taskname = %s\n", taskname);
#endif
        match = FALSE;
        indx = 0;
        while(strlen(TaskPtr->TaskName) && !match) {
            if(strcmp(taskname, TaskPtr->TaskName) == 0) {
                match = TRUE;
                count++;
            }
            TaskPtr++;
            indx++;
        }
        if(!match) done = TRUE;
    }
    strcpy(name, taskname);
    GetCurDev(&detachedstr, TRUE);
    detached = TRUE;
    return(TRUE);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: SaveAppDefsPopup
 * Description: User selected 'File Save' item from menubar
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SaveAppDefsPopup(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmString filespec;
    Widget DispSaveFile;

    sprintf(Wtitle, "Save AppDef List");

    n = 0;
    filespec = XmStringCreateLtoR(AppsInputFile, charset);
    WdTitle = XmStringCreateLtoR(Wtitle, charset);
    WarnMsg = XmStringCreateLtoR("Save as:", charset);
    XtSetArg(args[n], XmNdialogTitle, WdTitle); n++;
    XtSetArg(args[n], XmNselectionLabelString, WarnMsg); n++;
    XtSetArg(args[n], XmNtextColumns, 36); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNtextString, filespec); n++;
    DispSaveFile = XmCreatePromptDialog(MsmForm, "DispSaveFile", args, n);
    XtManageChild(DispSaveFile);    
    Button = XmSelectionBoxGetChild(DispSaveFile, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(Button);
    XtAddCallback (DispSaveFile, XmNokCallback, SaveCB, OK_BTN);
    XtAddCallback (DispSaveFile, XmNcancelCallback, SaveCB, CANCEL_BTN);
    XmStringFree(WdTitle);
    XmStringFree(WarnMsg);
    XmStringFree(filespec);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: DispErrPopup
 * Description: An error was detected. Display calling function string,
 *              determine if an error code was passed or and error string,
 *              get the error value or address of the error string, and 
 *              how to handle the exit whether to quit or ret to caller.
 *
 * Inputs: funct: string ponter to calling function
 *         ErrStr:  if NULL then err_msg is vms error code, else is address
 *                of error message string to display
 *         err_msg: if NULL then ErrStr is address of error message, else
 *                is vms error code
 *         ExitStat: Boolean, TRUE = ret to caller, FALSE = exit (this is
 *                performed by the callback routine)
 *
 * Returns: If exit status is TRUE return to caller, else exit
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void DispErrPopup(char *funct, char *ErrStr, int err_msg, int ExitStat)
{
    int status;
    short msglen;
    char msgbuf[120];
    char tmpmsg[30];
    $DESCRIPTOR(msgdsc, msgbuf);
    XmString OkLabel;

#ifdef ARF
    n = 0;
    XtSetArg(args[n], XmNy, 10); n++;
    XtSetArg(args[n], XmNx, 10); n++;
    w = XmCreateDialogShell(MsmForm, "Dlogshell", args, n);
#endif
    sprintf(Wtitle, " MiniSm Error ");
    if(ErrStr) {
        sprintf(warnstr, "%s: %s\n\n", funct, ErrStr);
    }
    else {
        status = sys$getmsg(err_msg, &msglen, &msgdsc, 0, 0);
        msgbuf[msglen] = '\0';
        sprintf(warnstr, "%s: %s\n\n", funct, msgbuf);
    }
    OkLabel = XmStringCreateLtoR("Acknowledge", charset);
    n = 0;
    WdTitle = XmStringCreateLtoR(Wtitle, charset);
    WarnMsg = XmStringCreateLtoR(warnstr, charset);
    XtSetArg(args[n], XmNdialogTitle, WdTitle); n++;
    XtSetArg(args[n], XmNmessageString, WarnMsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNokLabelString, OkLabel); n++;
    DispErrWarn = XmCreateWarningDialog(MsmForm, "DispErrWarn", args, n);
    XtAddCallback (DispErrWarn, XmNokCallback, DispErrPopupCB, ExitStat);
    XmStringFree(WdTitle);
    XmStringFree(WarnMsg);
    XmStringFree(OkLabel);

    /* remove unneeded buttons */
    Button = XmMessageBoxGetChild(DispErrWarn, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(Button);
    Button = XmMessageBoxGetChild(DispErrWarn, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(Button);
    XtManageChild(DispErrWarn);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: ExceedTask
 * Description: User selected an application to execute after the maximum
 *              task limit has been reached. This is a compile time value
 *              arbitrarily selected by the programmer and is an
 *              application modal implementation, requiring an
 *              acknowledgement by the user to continue MiniSm.
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ExceedTask(Widget w, XtPointer client_data, XtPointer call_data)
{
    char tmpmsg[30];
    XmString OkLabel;

    sprintf(Wtitle, "Exceeded Maximum");
    sprintf(tmpmsg, "  is %d active tasks ", num_tasks);
    sprintf(warnstr, " Maximum task limit \n%s", tmpmsg);

    n = 0;
    OkLabel = XmStringCreateLtoR("Bummer!", charset);
    WdTitle = XmStringCreateLtoR(Wtitle, charset);
    WarnMsg = XmStringCreateLtoR(warnstr, charset);
    XtSetArg(args[n], XmNdialogTitle, WdTitle); n++;
    XtSetArg(args[n], XmNmessageString, WarnMsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNokLabelString, OkLabel); n++;
    ExceedTaskWarn = XmCreateWarningDialog(w, "ExceedTaskWarn", args, n);
    XtAddCallback (ExceedTaskWarn, XmNokCallback, ExceedTaskPopupCB, 0);
    XmStringFree(WdTitle);
    XmStringFree(WarnMsg);
    XmStringFree(OkLabel);

    /* remove unneeded buttons */
    Button = XmMessageBoxGetChild(ExceedTaskWarn, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(Button);
    Button = XmMessageBoxGetChild(ExceedTaskWarn, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(Button);
    XtManageChild(ExceedTaskWarn);
}




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: FileNotFoundDlg
 * Description: Input file containing application information for the  
 *              AppsList box could not be found. User must acknowledge    
 *              popup to continue. 
 *
 * Inputs: funct: string ponter to calling function
 *         ErrStr:  if NULL then err_msg is vms error code, else is address
 *                of error message string to display
 *         err_msg: if NULL then ErrStr is address of error message, else
 *                is vms error code
 *         ExitStat: Boolean, TRUE = ret to caller, FALSE = exit (this is
 *                performed by the callback routine)
 *
 * Returns: If exit status is TRUE return to caller, else exit
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void FileNotFoundDlg(char *funct, char *ErrStr, int err_msg, int ExitStat)
{
    int status;
    short msglen;
    char msgbuf[120];
    char tmpmsg[30];
    $DESCRIPTOR(msgdsc, msgbuf);
    Widget w;
    XmString OkLabel;

    n = 0;
    XtSetArg(args[n], XmNy, 10); n++;
    XtSetArg(args[n], XmNx, 10); n++;
    w = XmCreateDialogShell(MsmForm, "Dlogshell", args, n);
    sprintf(Wtitle, " Input File Error ");
    if(ErrStr) {
        sprintf(warnstr, "%s: %s\n\n", funct, ErrStr);
    }
    else {
        status = sys$getmsg(err_msg, &msglen, &msgdsc, 0, 0);
        msgbuf[msglen] = '\0';
        sprintf(warnstr, "%s\n %s\n", funct, msgbuf);
    }
    OkLabel = XmStringCreateLtoR("Acknowledge", charset);
    n = 0;
    WdTitle = XmStringCreateLtoR(Wtitle, charset);
    WarnMsg = XmStringCreateLtoR(warnstr, charset);
    XtSetArg(args[n], XmNdialogTitle, WdTitle); n++;
    XtSetArg(args[n], XmNmessageString, WarnMsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNokLabelString, OkLabel); n++;
    DispErrWarn = XmCreateWarningDialog(w, "DispErrWarn", args, n);
    XtAddCallback (DispErrWarn, XmNokCallback, DispErrPopupCB, ExitStat);
    XmStringFree(WdTitle);
    XmStringFree(WarnMsg);
    XmStringFree(OkLabel);

    /* remove unneeded buttons */
    Button = XmMessageBoxGetChild(DispErrWarn, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(Button);
    Button = XmMessageBoxGetChild(DispErrWarn, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(Button);
    XtManageChild(DispErrWarn);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: FindTaskPid
 * Description: Some tasks can conceivably create detached processes.
 *              In this case, MiniSm's process id value from lib$spawn
 *              is not the pid of the task. This routine will locate
 *              the task, get the actual process id, and replace the
 *              task pid reported by lib$spawn.
 *
 * Inputs: String pointer to taskname, pid from lib$spawn
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void FindTaskPid(char *taskstr, int taskpid)
{
    int     mlen, indx;
    long    pid, status;
    char    devicestr[30];
    char    tasktmp[30];
    char    *bptr;
    char    *eptr;
    TASKLST *TaskPtr;

    TaskPtr = TaskStruct;
    strncpy(tasktmp, taskstr, strlen(taskstr));
    tasktmp[strlen(taskstr)-1] = '\0';
    for(indx = 0; indx < strlen(tasktmp); indx++)
        tasktmp[indx] = toupper(tasktmp[indx]);
    bptr = strstr(task_detached, tasktmp);
    eptr = strchr(bptr, '%');
    bptr = eptr + 1;
    eptr = strchr(bptr, ',');
    if(eptr) {
        mlen = eptr-bptr;
        strncpy(devicestr, bptr, mlen);
        devicestr[mlen] = '\0';
    }
    else strcpy(devicestr,bptr);

    sleep(5);  /* wait for window to come up */
    pid = GetCurDev(&devicestr,FALSE);
    indx = 0;
    while(TaskPtr->TaskPid != taskpid) TaskPtr++;
    TaskPtr->TaskPid = pid;
}




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: GetMpid
 * Description: Get the process id of our master process and get the
 *              entry process name to restore at exit.
 *              (additionally, set our process name with a suffix)
 *
 * Inputs: None
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int GetMpid()
{
    int status, mpid;
    char mpidstr[9];
    char msmpnam[16];
    char *ptr;
    short prevproc_len;

    struct { short len, code; char *buf, *retlen; } itmlis[3];
    $DESCRIPTOR(dsc_prn, msmpnam);
    $DESCRIPTOR(dsc_old, prevproc);

    itmlis[0].len    = 4;
    itmlis[0].code   = JPI$_MASTER_PID;
    itmlis[0].buf    = &mpid;       
    itmlis[0].retlen = 0;
    itmlis[1].len    = sizeof(prevproc);
    itmlis[1].code   = JPI$_PRCNAM;
    itmlis[1].buf    = &prevproc;
    itmlis[1].retlen = &prevproc_len;
    itmlis[2].len    = 0;
    itmlis[2].code   = 0;

    status = sys$getjpiw(0, 0, 0, &itmlis, 0, 0, 0);
    if(status == SS$_NORMAL) {
        sprintf(mpidstr, "%08x", mpid);
        ptr = &mpidstr + 5;
        sprintf(mpidsfx,"-%s", ptr);
        sprintf(msmpnam, "MiniSm%s", mpidsfx);
        dsc_prn.dsc$w_length = strlen(msmpnam);
        status = sys$setprn(&dsc_prn);
        return(TRUE);
    }
    else return(FALSE);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: NewTask
 * Description: We have started a new task. Check 3 times to see if the
 *              pid is still active. If after 3 checks is stive active,
 *              then it must have started so cancel timer.
 *
 * Inputs: Pid of new task
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void NewTask(int pid)
{
    int status;
    int indx;
    $DESCRIPTOR(dsc_delta, delta_str);

    /* set the number of times we will wait and check for this newly
       created process still being active (name is probably wrong) */

    newtask_count = 3;  /* numtimes to check if proc is running */
    sprintf(delta_str, "0 00:00:10.00");
    dsc_delta.dsc$w_length = strlen(delta_str);
    status = sys$bintim(&dsc_delta, &delta_time);
    status = CheckProcess(pid);
    if(status == SS$_NORMAL) {
        status = sys$cantim(0, 0);
        status = sys$setimr(0, &delta_time, AstEvent, 0, 0);
    }
    else {
        XmListDeletePos(TaskList, newtaskpos);
        num_tasks--;
        indx = num_tasks;
        AdjustTaskList(indx);  /* remove last one created */
        if(!num_tasks) XtSetSensitive(StopTaskBtn, FALSE);
        XmUpdateDisplay(TaskList);
        newtask_count = 0;
        sprintf(delta_str, "0 00:00:20.00");
        status = sys$bintim(&dsc_delta, &delta_time);
        status = sys$cantim(0, 0);
        if(num_tasks) status = sys$setimr(0, &delta_time, AstEvent, 0, 0);
        else status = sys$cantim(0, 0);
    }
}




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: HandleXevents
 * Description: Service X events for a particular widget when
 *              it is not feasable to get back to the XtAppMainLoop.
 *
 * Inputs: Pointer to the widget to service
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HandleXevents(Widget widget)
{
    XtAppContext      context;

    context = XtWidgetToApplicationContext (widget);
    while(XtAppPending (context) ) {
        XtAppProcessEvent (context, XtIMAll);
    }
}


