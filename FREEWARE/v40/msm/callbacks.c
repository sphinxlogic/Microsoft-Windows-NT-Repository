/*
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
            %% \___________________________________%% \
            %% |                                   %%  \
            %% |              MiniSm               %%   \
            %% |        callbacks.c  c1995         %%    \
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
#include <X11/cursorfont.h>

extern int AstEvent();

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: AppListSelCB
 * Description: User selected AppList from File menu. If AppForm widget
 *              is address is NULL then create it. Else manage the
 *              AppForm widget and return.
 *
 * Inputs: Normal callback inputs
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void AppListSelCB(Widget w, XtPointer client_data, XtPointer call_data)
{

    if(!AppForm) AppDefDlgCreate();
    else XtManageChild(AppForm);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: AppDefBtnCB
 * Description: User selected one of the buttons from AppDef dialog
 *              popup. Determine which button and perform approp action
 *
 * Inputs: Normal callback inputs
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void AppDefBtnCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    int selpos = 0;
    int status;
    int appscnt;
    char errmsgbuf[80];
    String filespec;

/*  appscnt = AppLstNumApps; */
    selpos = AppLstSelPos;
    switch ((int)client_data) {
        case ADD_BTN:   /* check for valid file & less than MAXAPPS */
            filespec = XmTextGetString(AppCmdTxt);
            status = CmdStrValid(filespec, AppLstNumApps);
            if(status == TRUE) {
                AppLstNumApps++;
                if(AppLstNumApps == MAXAPPS) {
                    sprintf(errmsgbuf, "Exceeded maximum application limit,\n%s",
                        "application not added");
                    DispErrPopup("Add", &errmsgbuf, FALSE, TRUE);
                }
                else {
                    ModifyAppsList(AppLstNumApps+1);
                    AppLstModified = TRUE;
                }
                n = 0;
                XtSetArg(args[n], XmNvalue, "\0"); n++;
                XtSetValues(AppNamTxt, args, n);    /* clear application name */
                n = 0;
                XtSetArg(args[n], XmNvalue, "\0"); n++;
                XtSetValues(AppCmdTxt, args, n);    /* clear application command */

                XtSetSensitive(AddPB, False);
                if(AppLstNumApps != MAXAPPS) {        
                    XtSetSensitive(AppDefApplyPB, True);
                    XtSetSensitive(AppDefOkPB, True);
/*                  AppLstNumApps = appscnt; */
                }
            }
            else {
                sprintf(errmsgbuf, "'%s' is\na non-existant file", filespec);
                DispErrPopup("Add", &errmsgbuf, FALSE, TRUE); 
            }
            XtFree(filespec);
            break;

        case APPLY_BTN:
            CopyAppList(AppLstNumApps, TO_LIST);
            break;

        case CANCEL_BTN:
            n = 0;
            XtSetArg(args[n], XmNvalue, "\0"); n++;
            XtSetValues(AppNamTxt, args, n);    /* clear application name */
            n = 0;
            XtSetArg(args[n], XmNvalue, "\0"); n++;
            XtSetValues(AppCmdTxt, args, n);    /* clear application command */
            XtUnmanageChild(AppForm);
            if(selpos) XmListDeselectPos(AppsLst, selpos);
            AppLstSelPos = 0;
            XtSetSensitive(AppDefOkPB, False);
            XtSetSensitive(AppDefApplyPB, False);
            XtSetSensitive(AddPB, False);
            n = 0;
            XtSetArg(args[n], XmNsensitive, False); n++;
            XtSetValues(RemovePB, args, n);
            XtUnmanageChild(AppForm);
            break;

        case OK_BTN:
            CopyAppList(AppLstNumApps, TO_LIST);
            XtUnmanageChild(AppForm);
            break;

        case REMOVE_BTN:
            XmListDeletePos(AppsLst, AppLstSelPos);
            AppLstSelPos = 0;
            selpos--;
            AppLstNumApps--;
            AdjustAppsLst(selpos, AppLstNumApps);
            AppLstModified = TRUE;
            n = 0;
            XtSetArg(args[n], XmNvalue, "\0"); n++;
            XtSetValues(AppNamTxt, args, n);    /* clear application name */
            n = 0;
            XtSetArg(args[n], XmNvalue, "\0"); n++;
            XtSetValues(AppCmdTxt, args, n);    /* clear application command */
            XtSetSensitive(AppDefApplyPB, True);
            XtSetSensitive(AppDefOkPB, True);
            n = 0;
            XtSetArg(args[n], XmNsensitive, False); n++;
            XtSetValues(RemovePB, args, n);
            break;
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: DispErrPopupCB
 * Description: Popup message box indicating abmormal return status from
 *              a system (non X11) call. Uses OK button. This is a NOP.
 *
 * Inputs: Normal callback inputs
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void DispErrPopupCB(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef ARF
    switch ((int)client_data) {
        case OK_BTN:         /* Ok button */
            break;
        case CANCEL_BTN:     /* Cancel button */
            break;
    }
#else
    int dummy = 0;
#endif
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: ExceedTaskPopupCB
 * Description: Popup message box for exceeding max task count will 
 *              come here after user presses OK button. This is a NOP.
 *
 * Inputs: Irrelevant
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void ExceedTaskPopupCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    int dummy;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: AppTxt_activateCB
 * Description: User pressed Return key while cursor was in either
 *              AppName or AppCmd text box
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void AppTxt_activateCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    int dummy;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: AppsSelCB
 * Description: User selected an existing application from  the   
 *              Applications List widget. Display application name
 *              and application command string. If same item was previously
 *              selected, unselect item and clear the application name
 *              and application command string.
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void AppsSelCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmListCallbackStruct *cb =  (XmListCallbackStruct *) call_data;
    int newpos = 0;
    XmString SelApp;
    APPSLST *AscFiles;

    newpos  = cb->item_position;
    if(AppLstSelPos == newpos) { /* Already selected, unselect item */
        newpos--;
        AscFiles = AscNewAppsList;
        AscFiles += newpos;
        SelApp = XmStringCreateLtoR(AscFiles->ApplName, charset);
        XmListDeselectItem(AppsLst, SelApp);
        XmStringFree(SelApp);
        AppLstSelPos = 0;
        XtSetSensitive(RemovePB, False);
        XtSetSensitive(AppDefOkPB, False);
        XtSetSensitive(AppDefApplyPB, False);
        XtSetSensitive(AddPB, False);
        XmUpdateDisplay(AppForm);
        n = 0;
        XtSetArg(args[n], XmNvalue, ""); n++;
        XtSetValues(AppNamTxt, args, n);    /* Clear application name */
        n = 0;
        XtSetArg(args[n], XmNvalue, ""); n++;
        XtSetValues(AppCmdTxt, args, n);    /* Clear command string */
    }
    else {      /* User selected previously unsel item */
        n = 0;
        XtSetArg(args[n], XmNsensitive, True); n++;
        XtSetValues(RemovePB, args, n);
        AppLstSelPos = newpos;
        newpos--;
        AscFiles = AscNewAppsList;
        AscFiles += newpos;
        n = 0;
        XtSetArg(args[n], XmNvalue, AscFiles->ApplName); n++;
        XtSetValues(AppNamTxt, args, n);    /* Insert application name */
        n = 0;
        XtSetArg(args[n], XmNvalue, AscFiles->ApplCmd); n++;
        XtSetValues(AppCmdTxt, args, n);    /* Insert command string */
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: AppTxtchangedCB
 * Description: User added or deleted char(s) from either AppNamTxt
 *              or AppCmdTxt text box in Application Definition
 *              popup dialog. If text NOT added because of an item
 *              being selected from AppDefLst, the turn off Remove
 *              button & turn on Add button.
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void AppTxtchangedCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    short RemoveStat;
    XmTextPosition curlength;

    switch((int)client_data) {
        case NAMTXT:
            curlength = XmTextGetLastPosition(AppNamTxt);
            if(curlength) AppNamstrlen = curlength;
            else AppNamstrlen = 0;
            break;
        case CMDTXT:
            curlength = XmTextGetLastPosition(AppCmdTxt);
            if(curlength) AppCmdstrlen = curlength;
            else AppNamstrlen = 0;
            break;
    }
    if(!AppLstSelPos) {     /* txt not added due to item selection */
        if(AppNamstrlen && AppCmdstrlen) {
            XtSetSensitive(AddPB, True);
            XtSetSensitive(RemovePB, False);
        }
    }
    else {
        XtSetSensitive(RemovePB, True);
        XtSetSensitive(AddPB, False);
    }
#ifdef ARF
    n = 0;  /* if Remove button sensitive, dont make Add sensitive */
    XtSetArg(args[n], XmNsensitive, &RemoveStat); n++;
    XtGetValues(RemovePB, args, n);
    if(RemoveStat & 0xff) return;
    if(AppNamstrlen && AppCmdstrlen) XtSetSensitive(AddPB, True);
    else XtSetSensitive(AddPB, False);
#endif
}




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: FileSelCB
 * Description: This is a selection callback.  Its function is to
 *              identify the application filename selected by the user
 *              and execute that file via spawn or run, add the last 3 
 *              digits of the mpid to taskname (for the procnam) then
 *              add to to task list. Finally it deselects the application
 *              file which was selected. First checks are made to verify
 *              the value of num_tasks is less than the constant value
 *              of MAXTASKS. Return if this is True.
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void FileSelCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    int indx;
    int flags = 9;
    int status;
    int taskpid;
    char comfile[135];
    char taskstr[30];
    char procname[17];
    char *ptr;
    static Cursor watch = 0;
    XmString TaskName;
    APPSLST *AscFiles;
    TASKLST *TaskPtr;
    $DESCRIPTOR(dsc_com, comfile);
    $DESCRIPTOR(dsc_disp, decw_disp);
    $DESCRIPTOR(dsc_prc, procname);
    XmListCallbackStruct *cb =  (XmListCallbackStruct *) call_data;

    indx = cb->item_position;
    indx--;
    if(num_tasks >= MAXTASKS) {
        ExceedTask(w, client_data, call_data);
        XmListDeselectItem(AppsList, CsAlistItems[indx]);
        return;
    }
    watch = XCreateFontCursor(XtDisplay(MsmForm),XC_watch);
    XDefineCursor(XtDisplay(MsmForm),XtWindow(MsmForm),watch);
    AscFiles = AscAppsList;
    AscFiles += indx;
    strcpy(taskstr, AscFiles->ApplName);
    status = CheckDetached(&taskstr);
    dsc_disp.dsc$w_length = strlen(decw_disp);
    strcpy(procname, AscFiles->ApplName);
    if(strlen(procname) >= 12) procname[11] = '\0';
    strcat(procname, mpidsfx);
    dsc_prc.dsc$w_length = strlen(procname);
    sprintf(comfile, "%s", AscFiles->ApplCmd);
    dsc_com.dsc$w_length = strlen(comfile);
    status = lib$spawn(0,       /* command line */
             &dsc_com,          /* input file */
             &dsc_disp,         /* output file */
             &flags,            /* flags */
             &dsc_prc,          /* process name */
             &taskpid,          /* process id */
             0,                 /* completion status */
             0,                 /* event flag num */
             0,                 /* AST address */
             0,                 /* AST argument */
             0,                 /* prompt string */
             0);                /* cli */
    if(status != 1) DispErrPopup(AscFiles, 0, status, TRUE);
    else {
        XmUpdateDisplay(MsmForm);    /* startup the Watch cursor */
        TaskPtr = TaskStruct;
        TaskPtr += num_tasks;
        TaskPtr->TaskPid = taskpid;
        strcpy(TaskPtr->TaskName, taskstr);
        num_tasks++;
        TaskName = XmStringCreateLtoR(taskstr, charset);
        XmListAddItemUnselected(TaskList, TaskName, 0);
        newtaskpos = XmListItemPos(TaskList, TaskName);
        XmStringFree(TaskName);
        NewTask(taskpid);
        n = 0;
        XtSetArg(args[n], XmNwidth, 120); n++;
        XtSetValues(TaskList, args, n);
    }
    XmListDeselectItem(AppsList, CsAlistItems[indx]);
    if(detached) {
        FindTaskPid(&taskstr,taskpid);
        detached = FALSE;
    }
    n = 0;
    XtSetArg(args[n], XmNwidth, 130); n++;
    XtSetValues(AppsList, args, n);
    XtSetSensitive(StopTaskBtn, FALSE);
    XUndefineCursor(XtDisplay(MsmForm),XtWindow(MsmForm));
    XmUpdateDisplay(MsmForm);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: FsBoxActivateCB
 * Description: User selected either OK or Cancel button in File Selection Box
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void FsBoxActivateCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    int status;
    String NewdirTmp;
    Widget temp;

    switch ((int)client_data) {
        case OK_BTN:         /* Ok Button */
            temp = XmFileSelectionBoxGetChild(w, XmDIALOG_TEXT);
            NewdirTmp = XmTextGetString(temp);
            strcpy(AppsInputFile, NewdirTmp);
            XtFree(NewdirTmp);
            if(num_apps) {
                cfree(AscAppsList);
                XmStringFree(CsAlistItems);
            }
            LoadAppFileList();
            break;

        case CANCEL_BTN:         /* Cancel Button */
            break;
    }
    XtUnmanageChild(FsbDialog);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: FsBoxCreateCB
 * Description: User selected 'Open' option from File menu
 *
 * Inputs: Parent widget
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void FsBoxCreateCB(Widget w)
{
    char *ptr;
    char TargetDir[80];
    Widget nobtn;

    if(Xdefaults) {
        strcpy(TargetDir, AppsInputFile);
        ptr = strchr(TargetDir, ']');
        if(ptr) {
            ptr++;
            *ptr = '\0';
        }
    }
    else {
        strcpy(TargetDir, "SYS$LOGIN");
        GetLogicalName(&TargetDir);
    }

    if(FsbDialog) {
        XtSetArg(args[n], XmNdirectory,
            XmStringCreateLtoR(TargetDir, charset)); n++;
        XtSetValues(FsbDialog, args, n);
        XtManageChild(FsbDialog);
    }
    else {
        n = 0;
        XtSetArg(args[n], XmNdialogTitle, 
            XmStringCreateLtoR("Select Applications File", charset)); n++;
        XtSetArg(args[n], XmNlistLabelString, 
            XmStringCreateLtoR("Appl Files", charset)); n++;
        XtSetArg(args[n], XmNselectionLabelString, 
            XmStringCreateLtoR("New Input File", charset)); n++;
        XtSetArg(args[n], XmNdirectory, 
            XmStringCreateLtoR(TargetDir, charset)); n++;
        XtSetArg(args[n], XmNpattern,
            XmStringCreateLtoR("*.INP", charset)); n++;
        XtSetArg(args[n], XmNtextColumns, 32); n++;
        FsbDialog = XmCreateFileSelectionDialog(w, "FsbDialog", args, n);
        nobtn = XmSelectionBoxGetChild(FsbDialog, XmDIALOG_HELP_BUTTON);
        XtUnmanageChild(nobtn);
        XtManageChild(FsbDialog);

        XtAddCallback(FsbDialog, XmNokCallback, FsBoxActivateCB, OK_BTN);
        XtAddCallback(FsbDialog, XmNcancelCallback, FsBoxActivateCB, CANCEL_BTN);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: SaveCB
 * Description: User entered AppDef filespec and presses Ok or Cancel
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SaveCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    String filename;
    Widget TextWidget;
    APPSLST *AppsPtr;
    XmSelectionBoxCallbackStruct *cb =
        (XmSelectionBoxCallbackStruct *)call_data;

    switch ((int)client_data) {
        case OK_BTN:
            if(AppLstModified) {
                TextWidget = XmSelectionBoxGetChild(w, XmDIALOG_TEXT);
                filename = XmTextGetString(TextWidget);
#ifdef MSMDBG
                printf("SaveCB: Writing file %s\n", filename);
#endif
                AppsFp = fopen(filename, "w+");
                if(!AppsFp) {
                    printf("SaveCB: Cannot open %s for write\n", filename);
                    break;
                }
                AppsPtr = AscNewAppsList;
                fprintf(AppsFp, "# MsmApps.Inp - Input file for MiniSm V2.x\n");
                fprintf(AppsFp, "# %s\n", __DATE__);
                fprintf(AppsFp, "# \n");
                while(strlen(AppsPtr->ApplName)) {
                    fprintf(AppsFp, "\t%-20s%s\n", AppsPtr->ApplName,
                            AppsPtr->ApplCmd);
                    AppsPtr++;
                }
                fprintf(AppsFp, "# \n");
                fprintf(AppsFp, "# End of file\n");
                fclose(AppsFp);
                AppLstModified = FALSE;
                XtFree(filename);
            }
            else break;
        break;

        case CANCEL_BTN:
        break;
    }
    XtUnmanageChild(w);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: InvLogPopupCB
 * Description: User specified invalid logical name
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void InvLogPopupCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    char *newdirptr;

    newdirptr =  XmTextGetString(NewDir);
    switch ((int)client_data) {
        case 1:     /* Ok Button, leave string */
            break;
        case 2:     /* Cancel Button, remove string */
            strcpy(newdirptr,"\0");
            n = 0;
            XtSetArg(args[n], XmNvalue, newdirptr); n++;
            XtSetValues(NewDir, args, n);
            XtFree(newdirptr);
    }
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: QuitCB
 * Description: Callback for quit button. Determine each active
 *              task and delete it. Wait a specified time and look for 
 *              next task. When no more tasks, execute exit call.
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void QuitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    int indx = num_tasks;
    int cntr = num_tasks;
    int status, pid;
    char msgbuf[80];
    TASKLST *TaskPtr;
    $DESCRIPTOR(dsc_old, prevproc);

    if(AppLstModified && !AppLstModAck) {
        sprintf(msgbuf, " Application Definitions have\n%s",
            "been modified but have not been saved");
        DispErrPopup("Warning", &msgbuf, FALSE, TRUE);
        AppLstModAck = TRUE;
        return;
    }
    TaskPtr = TaskStruct;
    dsc_old.dsc$w_length = strlen(prevproc);
    status = sys$cantim(0, 0);
    status = sys$setprn(&dsc_old);
    indx--;
    TaskPtr += indx;
    while(indx >= 0) {
        pid = TaskPtr->TaskPid;
        XmListDeletePos(TaskList, cntr);
        XmUpdateDisplay(TaskList);
        status = sys$delprc(&pid, 0);
        if(status != SS$_NORMAL) printf("QuitCB:$delprc,pid=%8x,err=%x,indx=%d\n",
            pid, status, indx);
        cntr--;
        TaskPtr--;
        indx--;
        sleep(1);
    }
    exit(0);
}




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: QuitCB
 * Description: Callback for quit button. Try to determine each active
 *              task and delete it. Wait a specified time and look for 
 *              next task. When no more tasks, execute exit call.
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void OldQuitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    int indx = 0;
    int tmp = num_tasks;
    int status, pid;
    char msgbuf[80];
    TASKLST *TaskPtr;
    $DESCRIPTOR(dsc_old, prevproc);

    if(AppLstModified && !AppLstModAck) {
        sprintf(msgbuf, " Application Definitions have\n%s",
            "been modified but have not been saved");
        DispErrPopup("Warning", &msgbuf, FALSE, TRUE);
        AppLstModAck = TRUE;
        return;
    }
    TaskPtr = TaskStruct;
    dsc_old.dsc$w_length = strlen(prevproc);
    status = sys$cantim(0, 0);
    status = sys$setprn(&dsc_old);
#ifdef ARF
    XmListDeleteAllItems(TaskList);
    XmUpdateDisplay(TaskList);
#endif
    while(indx < num_tasks) {
        pid = TaskPtr->TaskPid;
#ifndef ARF
        if(tmp > 1) XmListDeletePos(TaskList, indx+1);
        XmUpdateDisplay(TaskList);
#endif
        status = sys$delprc(&pid, 0);
        if(status != SS$_NORMAL) printf("QuitCB:$delprc,pid=%8x,err=%x,indx=%d\n",
            pid, status, indx);
        tmp--;
        TaskPtr++;
        indx++;
        sleep(1);
    }
    exit(0);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: StopTaskCB
 * Description: This is the delete task callback.  Its function is to
 *              verify the application task name selected by the user
 *              is actually active and delete the given task when the
 *              'Stop Task' button is selected. It then updates the
 *              'Tasks' list display
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void StopTaskCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    int status;
    int indx;
    TASKLST *TaskPtr;

    XmPushButtonCallbackStruct *cb =  (XmPushButtonCallbackStruct *) call_data;
    if(task_selpos) {
        TaskPtr = TaskStruct;
        task_selpos--;
        TaskPtr += task_selpos;
        status = sys$cantim(0, 0);
        status = sys$delprc(&TaskPtr->TaskPid, 0);
        XmListDeletePos(TaskList, task_selpos+1);
        AdjustTaskList(TaskPtr->TaskPid);
        num_tasks--;
        XtSetSensitive(StopTaskBtn, FALSE);
        if(num_tasks)  status = sys$setimr(0, &delta_time, AstEvent, 0, 0);
        task_selpos = 0;
    }
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: TaskSelCB
 * Description: This is a selection callback.  Its function is to 
 *              identify the application task name selected by the user
 *              and indicate which task to delete when 'Stop Task' button
 *              is selected (StopTaskCB) by placing the position value into
 *              global var task_selpos; if task_selpos is nonzero, assume
 *              user selected a task to stop, and changed their mind. They
 *              selected same item while hi-lited, so we will deselect it.
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TaskSelCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmListCallbackStruct *cb =  (XmListCallbackStruct *) call_data;
    int newpos = 0;
    XmString SelTask;
    TASKLST *TaskPtr;
    
    newpos  = cb->item_position;
    if(task_selpos == newpos) {
        TaskPtr = TaskStruct;
        TaskPtr += task_selpos -1;
        SelTask = XmStringCreateLtoR(TaskPtr->TaskName, charset);
        XmListDeselectItem(TaskList, SelTask);
        XmStringFree(SelTask);
        task_selpos = 0;
        XtSetSensitive(StopTaskBtn, FALSE);
        XmUpdateDisplay(TaskList);
    }
    else {
        task_selpos = cb->item_position;
        XtSetSensitive(StopTaskBtn, TRUE);
    }
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: VersionCB
 * Description: Callback for Version help button
 *
 * Inputs: Parent widget, client data, call data
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/************************************************************
 * VersionCB      - callback for Version help button
 ************************************************************/
void VersionCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget message_box;

    message_box = CreateVersion(w);
    XtManageChild(message_box);
}


