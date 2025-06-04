/*
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
            %% \___________________________________%% \
            %% |                                   %%  \
            %% |              MiniSm               %%   \
            %% |          Support.c  c1995         %%    \
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

#include <jpidef.h>
#include <stat.h>
#include "includes.h"
#include "externs.h"

/* some quick defines for SYS$PROCESS_SCAN */

#define    JPI$_GETJPI_CONTROL_FLAGS       -2
#define    JPI$M_IGNORE_TARGET_STATUS      4
#define    PSCAN$_NODENAME 13
#define    PSCAN$_MODE     11
#define    PSCAN$_USERNAME 23
#define    PSCAN$_GETJPI_BUFFER_SIZE       24


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Forward/External Declarations
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void DispErrPopupCB ();
void ExceedTaskPopupCB ();
void InvalidDetachedDef ();
extern int AstEvent ();
extern void InvLogPopupCB ();
extern int apps_sort ();

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: GetCurDev
 * Description: Locate all devices of type 'devnam' which are owned by
 *              current process. If mode is TRUE, save devicename in 
 *              array TermList. If mode is FALSE, when a device is found
 *              which is not in the list, return the process id
 *              of that device.
 *
 * Inputs: String pointer to the device name, and a arg of mode which I
 *         have no idea what it does (and I wrote the code!)
 *
 * Returns: Process id located device
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int GetCurDev(char *devnam, int mode)
{
    short   username_len, device_len, srchname_len;

    long    context, emode, pid, status;

    char    srchname[16];
    char    thisnode[12];
    char    username[16];
    char    devicename[20];
    char    *bptr;
    char    *eptr;

    int    mlen, indx;
    int    found, jpiflags;
    int    no_more_proc;

    struct { short buflen, code; char *bufadr, *itmflags; } pscanlist[5];
    struct { short len, code; char *buf, *retlen; } itmlis[6];
    struct { long sts, reserved; } iosblk;

    for(indx = 0; indx <= strlen(nodename); indx++)
        thisnode[indx] = toupper(nodename[indx]);

    itmlis[0].len   = sizeof(srchname);
    itmlis[0].code   = JPI$_USERNAME;
    itmlis[0].buf   = &srchname;
    itmlis[0].retlen = &srchname_len;
    itmlis[1].len   = 0;
    itmlis[1].code   = 0;


    status = sys$getjpiw(0, 0, 0, &itmlis, &iosblk, 0, 0);
    srchname[srchname_len] = '\0';
    found = FALSE;
    no_more_proc = FALSE;
    context = 0;
    mlen = 0;

    pscanlist[0].buflen     = strlen(thisnode);
    pscanlist[0].code       = PSCAN$_NODENAME;
    pscanlist[0].bufadr     = &thisnode;
    pscanlist[0].itmflags   = 0;
    pscanlist[1].buflen     = 0;
    pscanlist[1].code       = PSCAN$_MODE;
    pscanlist[1].bufadr     = JPI$K_INTERACTIVE;
    pscanlist[1].itmflags   = 0;
    pscanlist[2].buflen     = strlen(srchname);
    pscanlist[2].code       = PSCAN$_USERNAME;
    pscanlist[2].bufadr     = &srchname;
    pscanlist[2].itmflags   = 0;
    pscanlist[3].buflen     = 0;
    pscanlist[3].code       = PSCAN$_GETJPI_BUFFER_SIZE;
    pscanlist[3].bufadr     = 1000;
    pscanlist[3].itmflags   = 0;
    pscanlist[4].buflen     = 0;
    pscanlist[4].code       = 0;
Retry:      /* I hate goto's, but I just gotta do it */
    status = sys$process_scan(&context, &pscanlist);
    if (status != SS$_NORMAL)  lib$signal(status);

    jpiflags = 0;
    jpiflags = (JPI$M_IGNORE_TARGET_STATUS);

    itmlis[0].len = 4;
    itmlis[0].code = 0xffff & JPI$_GETJPI_CONTROL_FLAGS;
    itmlis[0].buf = &jpiflags;
    itmlis[0].retlen = 0;
    itmlis[1].len   = sizeof(username);
    itmlis[1].code   = JPI$_USERNAME;
    itmlis[1].buf   = &username;
    itmlis[1].retlen = &username_len;
    itmlis[2].len   = sizeof(devicename);
    itmlis[2].code   = JPI$_TERMINAL;
    itmlis[2].buf   = &devicename;
    itmlis[2].retlen = &device_len;
    itmlis[3].len   = 4;
    itmlis[3].code   = JPI$_PID;
    itmlis[3].buf   = &pid;
    itmlis[3].retlen = 0;
    itmlis[4].len   = 4;
    itmlis[4].code   = JPI$_MODE;
    itmlis[4].buf   = &emode;
    itmlis[4].retlen = 0;
    itmlis[5].len   = 0;
    itmlis[5].code   = 0;

    indx = 0;
#ifdef MSMDBG
    printf("\n - - - - - - - - - - - - - - - - - -\n");
#endif
    while (!no_more_proc) {
        status = sys$getjpiw(0, &context, 0, &itmlis, &iosblk, 0, 0);
        switch(status) {
            case SS$_NOMOREPROC: 
                no_more_proc = TRUE;
                if(mode) {  /* zero out rest of DevList array */
                    for(; indx <= 9; indx++) 
                        memset(&DevList[indx],0,sizeof(DevList[indx]));
                }
                else {
                    if(mlen <= 10) { /* wait max of 60 sec for new term */
                        sleep(10);
                        no_more_proc = FALSE;
                        context = 0;
                        mlen++;
                        goto Retry;
                    }
                }
#ifdef MSMDBG
                printf("\n - - - - - timeout - - - - - - - - -\n");
#endif
                break;

            case SS$_NORMAL:
                devicename[device_len] = '\0';
                username[username_len] = '\0';
#ifdef MSMDBG
                printf("%s %X %d %s\n", username,pid,emode,devicename);
#endif
                if(strcmp(username, srchname) != 0) break;
                else {
                    if(strncmp(devicename, devnam, strlen(devnam)) == 0) {
                        if(!mode) {
                            found = FALSE;
                            indx = 0;
                            while(!found && strlen(DevList[indx]) > 0) {
                                if(strcmp(DevList[indx],devicename) == 0) 
                                    found = TRUE;
                                else indx++;
                            }
                            if(!found) {    /* free proc_scan memory */
                                status = sys$process_scan(&context, 0);
#ifdef MSMDBG
                                printf("\n - - - - - %s - - - - - - - - -\n\n",
                                    devicename);
#endif
                                return(pid);
                            }
                        }
                        else {
                            strcpy(DevList[indx],devicename);
                            indx++;
                        }
                    }
                }
                break;

            default:  /* should never get here using process scan */
                break;
        }
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: CmdStrValid
 * Description: User has added an application name and application
 *              command to the 'Application definitions' window.
 *              Verify the application command points to a valid
 *              filespec and adding this application does not exceed
 *              the MAXAPPS value.
 *
 * Inputs: String pointer to application command file
 *
 * Returns: If filespec is valid & MAXAPPS not exceeded, return TRUE,
 *          else return FALSE
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int CmdStrValid(char *filespec, int appscount)
{
    int status;
    struct stat filebuf;

    status = stat(filespec, &filebuf);
    if(status < 0) return(FALSE);   /* filespec not found */
    if(appscount+1 > MAXAPPS) return(FALSE); 
    else return(TRUE);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: CopyAppList()
 * Description: Routine used to copy application list stuctures from the
 *              main window to the App Def dialog, or vice-versa.
 *
 * Inputs: Number of apps to copy, destination list
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CopyAppList(int appcount, int destlist)
{
    APPSLST *SrcPtr;
    APPSLST *DestPtr;

    if(destlist == TO_LST) {        
        if(AscNewAppsList) {
            cfree(AscNewAppsList);
            XmStringFree(CsAnewlistItems);
        }
        AscNewAppsList = (APPSLST *) calloc(appcount+1, sizeof(APPSLST));
        DestPtr = AscNewAppsList;
        SrcPtr = AscAppsList;
    }
    else {
        if(AscAppsList) {
            cfree(AscAppsList);
            XmStringFree(CsAnewlistItems);
        }
        AscAppsList = (APPSLST *) calloc(appcount+1, sizeof(APPSLST));
        DestPtr = AscAppsList;
        SrcPtr = AscNewAppsList;
    }

    while(strlen(SrcPtr->ApplName)) {
        strcpy(DestPtr->ApplName, SrcPtr->ApplName);
        strcpy(DestPtr->ApplCmd, SrcPtr->ApplCmd);
        DestPtr++;
        SrcPtr++;
    }
    if(destlist == TO_LST) DestPtr = AscNewAppsList;
    else DestPtr = AscAppsList;
    for (n = 0; n < appcount; n++) {
        if(destlist == TO_LST) {
            CsAnewlistItems[n] = (XmString) XmStringCreateLtoR
                (DestPtr->ApplName, charset);
        }
        else {
            CsAlistItems[n] = (XmString) XmStringCreateLtoR
                (DestPtr->ApplName, charset);
        }
        DestPtr++;
    }
    n = 0;
    if(destlist == TO_LST) {
        if(AppsLst) {
            XtSetArg(args[n], XmNitems, CsAnewlistItems); n++;
            XtSetArg(args[n], XmNitemCount, appcount); n++;
            XtSetValues(AppsLst, args, n);
        }
    }
    else {
        XtSetArg(args[n], XmNitems, CsAlistItems); n++;
        XtSetArg(args[n], XmNitemCount, appcount); n++;
        XtSetValues(AppsList, args, n);
    }
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: ModifyAppsList
 * Description: User has selected either the 'Remove' or 'Add' button
 *              in the 'Application definitions' window. This routine
 *              creates a new TASKLST structure reflecting the change,
 *              sorts the applications alphabeticly, and places the
 *              app names into the 'Application definitions' app list.
 *
 * Inputs: New application count + 1
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ModifyAppsList(int appcount)
{
    String  cmdpath;
    String  appname;
    APPSLST *AppsPtr;
    APPSLST *OldPtr;

    cmdpath = XmTextGetString(AppCmdTxt);
    appname = XmTextGetString(AppNamTxt);
    AscNewAppsList = (APPSLST *) calloc(appcount, sizeof(APPSLST));
    AppsPtr = AscNewAppsList;
    OldPtr = AscAppsList;

    /* if AppLstSelPos is nonzero, user has selected the Remove button */

    if(AppLstSelPos) {
        while(strlen(OldPtr->ApplName)) {
            if(!strcmp(OldPtr->ApplName, appname)) {
                OldPtr++;
                if(!strlen(OldPtr->ApplName)) break;
            }
            strcpy(AppsPtr->ApplName, OldPtr->ApplName);
            strcpy(AppsPtr->ApplCmd, OldPtr->ApplCmd);
            AppsPtr++;
            OldPtr++;
        }
        for (n = 0; n < appcount-1; n++) {
            CsAnewlistItems[n] = (XmString) XmStringCreateLtoR
                (AppsPtr->ApplName, charset);
            AppsPtr++;
        }
    }
    else {      /* user added another application */
        strcpy(AppsPtr->ApplName, appname);
        strcpy(AppsPtr->ApplCmd, cmdpath);
        AppsPtr++;
        while(strlen(OldPtr->ApplName)) {
            strcpy(AppsPtr->ApplName, OldPtr->ApplName);
            strcpy(AppsPtr->ApplCmd, OldPtr->ApplCmd);
            AppsPtr++;
            OldPtr++;
        }
        AppsPtr = AscNewAppsList;
        qsort((void *)AppsPtr, appcount-1, sizeof(APPSLST), apps_sort);
#ifdef ARF
        for (n = 0; n < appcount-1; n++) {
            CsAnewlistItems[n] = (XmString) XmStringCreateLtoR
                (AppsPtr->ApplName, charset);
            AppsPtr++;
        }
#else
        n = 0;
        AppsPtr = AscNewAppsList;
        while(strlen(AppsPtr->ApplName)) {
            CsAnewlistItems[n] = (XmString) XmStringCreateLtoR
                (AppsPtr->ApplName, charset);
            AppsPtr++;
            n++;
        }
#endif
    }

    n = 0;
    XtSetArg(args[n], XmNitems, CsAnewlistItems); n++;
    XtSetArg(args[n], XmNitemCount, appcount-1); n++;
    XtSetValues(AppsLst, args, n);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: GetLogicalName
 * Description: Translate a logical name string to an equivalance string.
 *
 * Inputs: String pointer to target logical name
 *
 * Returns: If translated succesfully, length of equivalance string,
 *          else return FALSE
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int GetLogicalName(char *lognam)
{

    int status;
    int crecnt = 0;
    short eqvfile_len;
    short table_len;
    char eqvfile[132];
    char lnm[80];
    char table[132];
    char *ptr;

    $DESCRIPTOR(lnm_table, table);
    $DESCRIPTOR(lnm_string, lnm);

    struct {
        short len, code;
        int *bufadr, *retlen;
    } itm[3];
    struct {
        long sts, reserved;
    } iosblk;

    itm[0].len = sizeof(table);
    itm[0].code = LNM$_TABLE;
    itm[0].bufadr = &table;
    itm[0].retlen = &table_len;
    itm[1].len = sizeof(eqvfile);
    itm[1].code = LNM$_STRING;
    itm[1].bufadr = &eqvfile;
    itm[1].retlen = &eqvfile_len;
    itm[2].len = 0;
    itm[2].code = 0;

#ifdef MSMDBG
    printf("Inside GetLogicalName\n");
#endif

 /* if passed a logical name string, translate & return ... */

    if(lognam) {
        strcpy(lnm, lognam);
        strcpy(table, "LNM$DCL_LOGICAL");
retry1:		/* I hate this, but... */
        lnm_string.dsc$w_length = strlen(lnm);
        lnm_table.dsc$w_length = strlen(table);

    /* convert input str to upcase */

        for(n = 0; n <= strlen(lnm); n++)
            lnm[n] = toupper(lnm[n]);
        status = sys$trnlnm(0, &lnm_table, &lnm_string, 0, &itm);
        if (status != SS$_NORMAL) return(FALSE); /* not found */
        else {
            eqvfile[eqvfile_len] = '\0';
            ptr = strchr(eqvfile,'[');
            if(!ptr) {      /* lnm is another lnm) */
                strcpy(lnm,eqvfile);
                goto retry1;
            }
            table[table_len] = '\0';
            strcpy(lognam, eqvfile);
            return(TRUE);
        }
    }
    else {

    /* We are starting up so get vms node name, and
       necessary decterm definitions. These are all logicals */

#ifdef ARF
    /* first get home directory (SYS$LOGIN) */

        strcpy(lnm, "SYS$LOGIN");
        strcpy(table, "LNM$DCL_LOGICAL");
retry2:		/* I STILL hate this, but... */
        lnm_string.dsc$w_length = strlen(lnm);
        lnm_table.dsc$w_length = strlen(table);
        status = sys$trnlnm(0, &lnm_table, &lnm_string, 0, &itm);
        if (status != SS$_NORMAL) {
#ifdef MSMDBG
            printf("didn't find SYS$NODE\n");
#endif
            return(FALSE);
        }
        else {
            eqvfile[eqvfile_len] = '\0';
            strcpy(AppsInputFile, eqvfile);
#ifdef MSMDBG
            printf("found sys$login: %s\n", AppsInputFile);
#endif
        }
#endif  /* ARF */
 
    /* then get node on which we are executing */

        strcpy(lnm, "SYS$NODE");
        strcpy(table, "LNM$DCL_LOGICAL");
        lnm_string.dsc$w_length = strlen(lnm);
        lnm_table.dsc$w_length = strlen(table);

        status = sys$trnlnm(0, &lnm_table, &lnm_string, 0, &itm);
        if (status != SS$_NORMAL) {
#ifdef MSMDBG
            printf("didn't find SYS$NODE\n");
#endif
            return(FALSE);
        }
        else {
            eqvfile[eqvfile_len-2] = '\0';	/* strip off '::' */
            for(n = 1; n <= strlen(eqvfile); n++)
                eqvfile[n] = tolower(eqvfile[n]);
            table[table_len] = '\0';
            strcpy(nodename, eqvfile);
#ifdef MSMDBG
            printf("found node: %s\n", nodename);
#endif
        }

    /* then get list of detached processes */

        strcpy(lnm, "MSM_DETACHED");
        strcpy(table, "LNM$DCL_LOGICAL");
        lnm_string.dsc$w_length = strlen(lnm);
        lnm_table.dsc$w_length = strlen(table);

        status = sys$trnlnm(0, &lnm_table, &lnm_string, 0, &itm);
        if(status == SS$_NORMAL) {
            eqvfile[eqvfile_len] = '\0';
            for(n = 0; n <= strlen(eqvfile); n++)
                eqvfile[n] = toupper(eqvfile[n]);
            table[table_len] = '\0';
            strcpy(task_detached, eqvfile);
#ifdef MSMDBG
            printf("found %s in %s\n", lnm, table);
#endif
        }
        else {
            printf("didn't find %s\n", lnm);
            return(FALSE);
        }

    /* get users current display device */

        strcpy(lnm, "DECW$DISPLAY");
        strcpy(table, "LNM$DCL_LOGICAL");
        lnm_string.dsc$w_length = strlen(lnm);
        lnm_table.dsc$w_length = strlen(table);

get_disp:
        status = sys$trnlnm(0, &lnm_table, &lnm_string, 0, &itm);
        if (status != 1) DispErrPopup(lnm, 0, status, FALSE); 
        else {
            eqvfile[eqvfile_len] = '\0';
            table[table_len] = '\0';
            strcpy(decw_disp, eqvfile);
#ifdef MSMDBG
            printf("Using display: %s\n", decw_disp);
#endif
        }
    }
    return TRUE;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: InvalidDetachedDef
 * Description: Most applications are started only once per session and
 *              therefore are assigned a unique process name. One exception
 *              is DecTerm, for which it is desirable to have multiple
 *              tasks existing. We look for the logical MSM_DETACHED to
 *              define a comma seperated list of applications and the
 *              respective device type expected. Currently, only DecTerm
 *              is implemented.
 *
 * Inputs: None
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void InvalidDetachedDef()
{
    Widget w;

    sprintf(Wtitle, "Invalid Logical Definition");
    sprintf(warnstr, "The definition MSM_DETACHED is invalid. The \n\
equivalence string should be enclosed in double\n\
quotes, and indicate the application name, then a '%' \n\
followed by the first 3 chars of the device:\n\
 $ define MSM_DETACHED \"decterm%fta\"\n");
    n = 0;
    WdTitle = XmStringCreateLtoR(Wtitle, charset);
    WarnMsg = XmStringCreateLtoR(warnstr, charset);
    XtSetArg(args[n], XmNdialogTitle, WdTitle); n++;
    XtSetArg(args[n], XmNmessageString, WarnMsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    InvDetachedWarn = XmCreateWarningDialog(w, "InvDetachedWarn", args, n);
    XtAddCallback (InvDetachedWarn, XmNokCallback, InvLogPopupCB, 1);
    XmStringFree(WdTitle);
    XmStringFree(WarnMsg);

    /* remove unneeded buttons */
    Button = XmMessageBoxGetChild(InvDetachedWarn, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(Button);
    Button = XmMessageBoxGetChild(InvDetachedWarn, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(Button);
    XtManageChild(InvDetachedWarn);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: apps_sort
 * Description: Called by qsort, will test the value of the first three
 *              characters of two tasknames and will return 1, -1, or 0
 *              depending on result.
 *
 * Inputs: 2 pointers to stuctures of type APPSLST
 *
 * Returns: Result of sort
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int apps_sort(APPSLST *file1, APPSLST *file2)
{
    if(file1->ApplName[0] > file2->ApplName[0]) return(1);
    else {
        if(file2->ApplName[0] > file1->ApplName[0]) return(-1);
        else {
            if(file1->ApplName[1] > file2->ApplName[1]) return(1);
            else {
                if(file2->ApplName[1] > file1->ApplName[1]) return(-1);
                else {
                    if(file1->ApplName[2] > file2->ApplName[2]) return(1);
                    else {
                        if(file2->ApplName[2] > file1->ApplName[2]) return(-1);
                        else return(0);
                    }
                }
            }
        }
    }
}


