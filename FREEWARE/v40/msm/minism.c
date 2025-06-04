/*
         %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
         %% \___________________________________%% \
         %% |                                   %%  \
         %% |              MiniSm               %%   \
         %% |          MiniSm.c  c1995          %%    \
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
#include "Msm.icon"

#define MAXAPPS 40
#define MAXTASKS 10

#define OK_BTN 1
#define APPLY_BTN 2
#define CANCEL_BTN 3
#define REMOVE_BTN 4
#define ADD_BTN 5
#define NAMTXT 1
#define CMDTXT 2
#define ERRVAL 1        /* not used */
#define ERRSTR 2        /* not used */
#define TO_LST 1        /* copy Apps List to AppDef List */
#define TO_LIST 2       /* copy AppDef List to Apps List */

typedef struct {
    char ApplName[20];
    char ApplCmd[50];
} APPSLST;

typedef struct {
    char TaskName[20];
    int  TaskPid;
} TASKLST;

    /*
     * External Functions and Callbacks
     */

extern int GetFileNames ();
extern int GetLogicalName ();
extern void AppDefBtnCB ();
extern void AppListSelCB ();
extern void AppsSelCB ();
extern void FileNotFoundDlg ();
extern void FsBoxActivateCB ();
extern void FsBoxCreateCB ();
extern void HelpOvCB ();
extern void HelpApCB ();
extern void HelpTskCB ();
extern void SaveAppDefsPopup ();
extern void AppTxtchangedCB ();
extern void VersionCB ();
extern void AppTxt_activateCB ();
extern void QuitCB ();
extern void SaveCB ();
extern void StopTaskCB ();
extern void TaskSelCB ();
extern void FileSelCB ();
extern Widget CreateHelp ();
extern Widget CreateVersion ();


    /*
     * Global Variable Declarations
     */

Arg args[20];

FILE *AppsFp;

APPSLST *AscAppsList;
APPSLST *AscNewAppsList;
TASKLST *TaskStruct;

char DevList[10][20];
char Wtitle[30];
char decw_disp[12];
char delta_str[] = "0 00:00:10.00";
char mpidsfx[6];
char verstr[] = " MiniSm V2.13 \n  20 Oct 1998";
char nodename[10];
char prevproc[18];
char task_detached[132];
char AppsInputFile[132];
char warnstr[80];

int AppCmdstrlen;
int AppLstModified;
int AppLstModAck;
int AppLstNumApps;
int AppLstSelPos;
int AppNamstrlen;
int Xdefaults;
int apps_selpos;
int clrnewdirstr;
int detached;
int n;
int newtaskpos;
int newtask_count;      /* num times to check new task at 10 sec interval */
int num_apps;
int num_tasks;
int task_selpos;

long delta_time[2];

Widget MsmForm, MsmFrame;
Widget Bboard, NewDir, AppsList, TaskList, AppDefApplyPB, AppDefOkPB;
Widget AppsLbl, TaskLbl, Button, RemovePB, AddPB;
Widget StopTaskBtn, FsbDialog, InvDetachedWarn, AppsLst;
Widget ExceedTaskWarn, DispErrWarn, AppForm, AppNamTxt, AppCmdTxt;

XmString WdTitle;
XmString CsAlistItems[MAXAPPS+1];       /* Compound Str application list */
XmString CsAnewlistItems[MAXAPPS+1];    /* Compound Str application list */
XmString CsTlistItems[MAXTASKS+1];      /* Compound Str task list */
XmString CurDirstr;                     /* unused */
XmString SpecifiedDir;                  /* unused */
XmString WarnMsg;
XmStringCharSet charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;



/************************************************************************
 *   main
 ***********************************************************************/

main(int argc, char **argv)
{
    Widget toplevel, main_window, menu_bar;
    Widget cascade, menu_pane;
    XtAppContext app_context;
    Display *display;
    String Button_string;
    char *cur_dir;
    char iconstr[20];
    char titlestr[80];
    char tmpstr[20];
    char pathstr[80];
    char *NewDirStr;
    int status;
    APPSLST *AppsPtr;

    /* get some environmental information  */
    /* first see if DECW$USER_DEFAULTS is defined */

    strcpy(AppsInputFile, "DECW$USER_DEFAULTS");
    status = GetLogicalName(&AppsInputFile);
    if(status != SS$_NORMAL) {
        strcpy(AppsInputFile, "SYS$LOGIN");
        status = GetLogicalName(&AppsInputFile);
    }
    else Xdefaults = TRUE; /* we found it, so look for MSMAPPS.INP */
    strcat(AppsInputFile, "MSMAPPS.INP");

    status = GetLogicalName(FALSE); /* get nodename, display */
    if(status != SS$_NORMAL) {
        printf("Undefined Logical Name(s). Exiting...\n\n");
        exit();
    }
    status = GetMpid();

 /*
  *  Initialize the toolkit.
  */
    toplevel = XtAppInitialize(&app_context, "MiniSm", NULL, 0, &argc, argv, NULL, args, 0);

 /*
  *  Create main window.
  */
    n = 0;
    main_window = XmCreateMainWindow(toplevel, "MiniSm", args, n);
    XtManageChild(main_window);
#ifdef MSMDBG
    printf("created main window\n");
#endif

 /*
  *  Set main window title, icon pix, and icon name
  */

    n = 0;
    sprintf(titlestr," Mini Session Mgr - %s ", nodename);
    XtSetArg(args[n], XmNtitle, titlestr); n++;
    sprintf(iconstr, "%s",nodename);
    XtSetArg(args[n], XmNiconName, iconstr); n++;
    XtSetArg(args[n], XmNiconPixmap,
            XCreateBitmapFromData(XtDisplay(toplevel),
                                  RootWindowOfScreen(XtScreen(toplevel)),
                                  Msm_bits,
                                  Msm_width,
                                  Msm_height)); n++;
    XtSetValues (toplevel, args, n);

 /*
  *  Create menu bar in main window.
  */

    n = 0;
    menu_bar = XmCreateMenuBar(main_window, "menu_bar", args, n);
    XtManageChild(menu_bar);

 /*
  *  Create "File" pulldown menu.
  */

    n = 0;
    menu_pane = XmCreatePulldownMenu(menu_bar, "menu_pane", args, n); n++;

    n = 0;
    Button = XmCreatePushButton(menu_pane, " Open ", args, n); n++;
    XtManageChild(Button);
    XtAddCallback(Button, XmNactivateCallback, FsBoxCreateCB, NULL);

    n = 0;
    Button = XmCreatePushButton(menu_pane, " Modify ", args, n); n++;
    XtManageChild(Button);
    XtAddCallback(Button, XmNactivateCallback, AppListSelCB, NULL);

    n = 0;
    Button = XmCreatePushButton(menu_pane, " Save ", args, n); n++;
    XtManageChild(Button);
    XtAddCallback(Button, XmNactivateCallback, SaveAppDefsPopup, NULL);

    n = 0;
    Button = XmCreatePushButton(menu_pane, " Exit ", args, n); n++;
    XtManageChild(Button);
    XtAddCallback(Button, XmNactivateCallback, QuitCB, NULL);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, menu_pane); n++;
    cascade = XmCreateCascadeButton(menu_bar, " File ", args, n);
    XtManageChild(cascade);


 /*
  *  Create "Help" pulldown menu.
  */

    n = 0;
    menu_pane = XmCreatePulldownMenu(menu_bar, "menu_pane", args, n);

    n = 0;
    Button = XmCreatePushButton(menu_pane, " Overview ", args, n);
    XtManageChild(Button);
    XtAddCallback(Button, XmNactivateCallback, HelpOvCB, NULL);

    n = 0;
    Button = XmCreatePushButton(menu_pane, " Applications ", args, n);
    XtManageChild(Button);
    XtAddCallback(Button, XmNactivateCallback, HelpApCB, NULL);

    n = 0;
    Button = XmCreatePushButton(menu_pane, " Tasks ", args, n);
    XtManageChild(Button);
    XtAddCallback(Button, XmNactivateCallback, HelpTskCB, NULL);

    n = 0;
    Button = XmCreatePushButton(menu_pane, " Version ", args, n);
    XtManageChild(Button);
    XtAddCallback(Button, XmNactivateCallback, VersionCB, NULL);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, menu_pane); n++;
    cascade = XmCreateCascadeButton(menu_bar, "Help", args, n);
    XtManageChild(cascade);

    n = 0;
    XtSetArg(args[n], XmNmenuHelpWidget, cascade); n++;
    XtSetValues(menu_bar, args, n);


    /*
     * Create a Form widget as the main window 
     */


    n = 0;
    XtSetArg(args[n], XmNnoResize, True); n++;
    XtSetArg(args[n], XmNheight, 230); n++;
    XtSetArg(args[n], XmNwidth, 330); n++;
    MsmForm = XmCreateForm(main_window, "MsmForm", args, n);
    XtManageChild(MsmForm);


    n = 0;
    XtSetArg(args[n], XmNy, 20); n++;
    XtSetArg(args[n], XmNx, 25); n++;
    AppsLbl = XmCreateLabel(MsmForm, "Applications", args, n);
    XtManageChild(AppsLbl);

    LoadAppFileList();  /* Check for default application filelist */

     /* Create a list widget for Applications */

    n = 0;
    XtSetArg(args[n], XmNy, 45); n++;
    XtSetArg(args[n], XmNleftOffset, 25); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNwidth, 130); n++;
    XtSetArg(args[n], XmNshadowThickness, 3); n++;
    XtSetArg(args[n], XmNselectionPolicy, XmSINGLE_SELECT);
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED); n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); n++;
    XtSetArg(args[n], XmNvisibleItemCount, 8); n++;
    if(num_apps) {
        XtSetArg(args[n], XmNitems, CsAlistItems); n++;
        XtSetArg(args[n], XmNitemCount, num_apps); n++;
    }
    else {
        XtSetArg(args[n], XmNitemCount, 0); n++;
    }
    AppsList = XmCreateScrolledList(MsmForm, "AppsList", args, n);
    XtManageChild(AppsList);

    /*
     * Add the callback for applications selection.
     */

    XtAddCallback(AppsList, XmNbrowseSelectionCallback, FileSelCB, NULL);

    /* Create a label widget for TaskList */

    n = 0;
    XtSetArg(args[n], XmNy,  20); n++;
    XtSetArg(args[n], XmNx, 185); n++;
    TaskLbl = XmCreateLabel(MsmForm, "Tasks", args, n);
    XtManageChild(TaskLbl);


     /* Create a list widget for active tasks */

    n = 0;
    XtSetArg(args[n], XmNy, 45); n++;
    XtSetArg(args[n], XmNleftOffset, 185); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 25); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNshadowThickness, 3); n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED); n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); n++;
    XtSetArg(args[n], XmNvisibleItemCount, 5); n++;
    XtSetArg(args[n], XmNitemCount, 0); n++;
    TaskList = XmCreateScrolledList(MsmForm, "TaskList", args, n);
    XtManageChild(TaskList);

    /*
     * Add the callback for Task selection.
     */

    XtAddCallback(TaskList, XmNbrowseSelectionCallback, TaskSelCB, NULL);

    /*
     * Create the 'Stop Task' push button widget
     */

    n = 0;
    XtSetArg(args[n], XmNy, 165); n++;
    XtSetArg(args[n], XmNx, 200); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNwidth, 90); n++;
    XtSetArg(args[n], XmNshadowThickness, 3); n++;
    StopTaskBtn = XmCreatePushButton(MsmForm, " Stop Task ", args, n);
    XtManageChild(StopTaskBtn);
    XtAddCallback(StopTaskBtn, XmNactivateCallback, StopTaskCB, NULL);

    /* Until a task selected, Button non_functional */

    XtSetSensitive(StopTaskBtn, FALSE);

    /* Manage the  widget */
    XtManageChild(MsmForm);

    n = 0;
    XtSetArg(args[n], XmNshadowType, XmSHADOW_IN); n++;
    XtSetArg(args[n], XmNshadowThickness, 3); n++;
    XtSetArg(args[n], XmNtopOffset, 10); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset, 10); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 10); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 10); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNmarginHeight, 10); n++;
    XtSetArg(args[n], XmNmarginWidth, 10); n++;
    MsmFrame = XmCreateFrame(MsmForm, "MsmFrame", args, n);
    XtManageChild(MsmFrame);

#ifdef ARF
    /* create the Application Support popup */
    AppDefDlgCreate();
    XtUnmanageChild(AppForm);
#endif

    /*
     * Realize the toplevel widget, which displays all children
     */

    XtRealizeWidget(toplevel);

    /*
     *  Initialize task structure
     */

    TaskStruct = (TASKLST *) calloc(MAXTASKS+1, sizeof(TASKLST));

    /*
     * Go into a loop and wait for input
     */

    XtAppMainLoop(app_context);
}



int AppDefDlgCreate()
{
    char textstr[50];
    XmString CsStr;
    XmString label_text;
    Widget AppDefDlg, AppsLbl1, AppsLbl2, AppsLbl3;
    Widget ListFrame, TextFrame, CancelPB;

#ifdef ARF
    n = 0;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    AppDefDlg = XmCreateDialogShell(MsmForm, "AppDefDlg", args, n);
#endif

    label_text = XmStringCreateLtoR("Application Definitions", charset);
    n = 0;
    XtSetArg(args[n], XmNheight, 495); n++;
    XtSetArg(args[n], XmNwidth, 440); n++;
    XtSetArg(args[n], XmNdialogTitle, label_text);  n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
#ifdef ARF
    AppForm = XmCreateFormDialog(AppDefDlg, "AppForm", args, n);
#else
    AppForm = XmCreateFormDialog(MsmForm, "AppForm", args, n);
#endif
    XmStringFree(label_text);
#ifdef ARF
    XtManageChild(AppForm);
#else
    XtRealizeWidget(AppForm);
#endif

    n = 0;
    XtSetArg(args[n], XmNheight, 215); n++;
    XtSetArg(args[n], XmNshadowType, XmSHADOW_IN); n++;
    XtSetArg(args[n], XmNshadowThickness, 3); n++;
    XtSetArg(args[n], XmNtopOffset, 10); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 10); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 10); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNmarginHeight, 10); n++;
    XtSetArg(args[n], XmNmarginWidth, 10); n++;
    ListFrame = XmCreateFrame(AppForm, "ListFrame", args, n);
    XtManageChild(ListFrame);

    n = 0;
    XtSetArg(args[n], XmNy, 240); n++;
    XtSetArg(args[n], XmNheight, 195); n++;
    XtSetArg(args[n], XmNshadowType, XmSHADOW_IN); n++;
    XtSetArg(args[n], XmNshadowThickness, 3); n++;
    XtSetArg(args[n], XmNleftOffset, 10); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 10); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNmarginHeight, 10); n++;
    XtSetArg(args[n], XmNmarginWidth, 10); n++;
    TextFrame = XmCreateFrame(AppForm, "TextFrame", args, n);
    XtManageChild(TextFrame);

        /* Create labels for list widget and textfields */

    sprintf(textstr,"Application List");
    CsStr = XmStringCreateLtoR(textstr, charset);
    n = 0;
    XtSetArg(args[n], XmNy, 15); n++;
    XtSetArg(args[n], XmNx, 20); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    XtSetArg(args[n], XmNtraversalOn, False); n++;
    AppsLbl1 = XmCreateLabel(AppForm , "AppsLbl1", args, n);
    XtManageChild(AppsLbl1);
    XmStringFree(CsStr);

    sprintf(textstr,"Application Name");
    CsStr = XmStringCreateLtoR(textstr, charset);
    n = 0;
    XtSetArg(args[n], XmNy, 250); n++;
    XtSetArg(args[n], XmNx, 20); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    XtSetArg(args[n], XmNtraversalOn, False); n++;
    AppsLbl2 = XmCreateLabel(AppForm, "AppsLbl2", args, n);
    XtManageChild(AppsLbl2);
    XmStringFree(CsStr);

    sprintf(textstr,"Application Command");
    CsStr = XmStringCreateLtoR(textstr, charset);
    n = 0;
    XtSetArg(args[n], XmNy, 315); n++;
    XtSetArg(args[n], XmNx, 20); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNlabelString, CsStr); n++;
    XtSetArg(args[n], XmNtraversalOn, False); n++;
    AppsLbl3 = XmCreateLabel(AppForm, "AppsLbl3", args, n);
    XtManageChild(AppsLbl3);
    XmStringFree(CsStr);

        /* Copy the list of Applications into the Application
           Definitions lists */

    CopyAppList(num_apps, TO_LST);

        /* Create the list box for Application files */

    n = 0;
    XtSetArg(args[n], XmNx, 15); n++;
    XtSetArg(args[n], XmNy, 40); n++;
    XtSetArg(args[n], XmNshadowThickness, 3); n++;
    XtSetArg(args[n], XmNselectionPolicy, XmSINGLE_SELECT);
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED); n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); n++;
    XtSetArg(args[n], XmNvisibleItemCount, 6); n++;
    XtSetArg(args[n], XmNitems, CsAnewlistItems); n++;
    XtSetArg(args[n], XmNitemCount, num_apps); n++;
    XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 20); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 20); n++;
    XtSetArg(args[n], XmNtraversalOn, True); n++;
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
    AppsLst = XmCreateScrolledList(AppForm , "AppsLst", args, n);
    XtManageChild(AppsLst);
    XtAddCallback(AppsLst, XmNbrowseSelectionCallback, AppsSelCB, NULL);

        /* Create the text boxes for Application Info */

    n = 0;
    XtSetArg(args[n], XmNx, 20); n++;
    XtSetArg(args[n], XmNy, 270); n++;
    XtSetArg(args[n], XmNcolumns, 23); n++;
    XtSetArg(args[n], XmNshadowThickness, 3); n++;
    XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
    XtSetArg(args[n], XmNeditable, True); n++;
    XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 20); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 20); n++;
    XtSetArg(args[n], XmNtraversalOn, True); n++;
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
    AppNamTxt = XmCreateText(AppForm, "AppNamTxt", args, n);
    XtManageChild(AppNamTxt);
    XtAddCallback(AppNamTxt, XmNactivateCallback, AppTxt_activateCB, NAMTXT);
    XtAddCallback(AppNamTxt, XmNvalueChangedCallback, AppTxtchangedCB, NAMTXT);

    n = 0;
    XtSetArg(args[n], XmNx, 20); n++;
    XtSetArg(args[n], XmNy, 340); n++;
    XtSetArg(args[n], XmNcolumns, 23); n++;
    XtSetArg(args[n], XmNshadowThickness, 3); n++;
    XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
    XtSetArg(args[n], XmNeditable, True); n++;
    XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 20); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 20); n++;
    XtSetArg(args[n], XmNtraversalOn, True); n++;
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
    AppCmdTxt = XmCreateText(AppForm, "AppCmdTxt", args, n);
    XtManageChild(AppCmdTxt);
    XtAddCallback(AppCmdTxt, XmNactivateCallback, AppTxt_activateCB, CMDTXT);
    XtAddCallback(AppCmdTxt, XmNvalueChangedCallback, AppTxtchangedCB, CMDTXT);




    /*
     * Create the Remove button
     */

    label_text = XmStringCreateLtoR("Remove", charset);

    n = 0;
    XtSetArg(args[n], XmNx, 135); n++;
    XtSetArg(args[n], XmNy, 180); n++;
    XtSetArg(args[n], XmNwidth, 80); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, label_text); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 105); n++;
    XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 105); n++;
    XtSetArg(args[n], XmNtraversalOn, False); n++;
    XtSetArg(args[n], XmNsensitive, False); n++;
    RemovePB = XmCreatePushButton(AppForm, "Remove", args, n);
    XtManageChild(RemovePB);
    XtAddCallback(RemovePB, XmNactivateCallback, AppDefBtnCB, REMOVE_BTN);
/*  XtSetSensitive(RemovePB, False); */
    XmStringFree (label_text);


    /*
     * Create the Add button
     */

    label_text = XmStringCreateLtoR("Add", charset);

    n = 0;
    XtSetArg(args[n], XmNx, 135); n++;
    XtSetArg(args[n], XmNy, 385); n++;
    XtSetArg(args[n], XmNwidth, 80); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, label_text); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 105); n++;
    XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 105); n++;
    XtSetArg(args[n], XmNtraversalOn, False); n++;
    AddPB = XmCreatePushButton(AppForm, "Add", args, n);
    XtManageChild(AddPB);
    XtAddCallback(AddPB, XmNactivateCallback, AppDefBtnCB, ADD_BTN);
    XtSetSensitive(AddPB, False);
    XmStringFree (label_text);

    /*
     * Create the Ok button
     */

    label_text = XmStringCreateLtoR("OK", charset);

    n = 0;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 450); n++;
    XtSetArg(args[n], XmNwidth, 80); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, label_text); n++;
    XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 10); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 450); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset, 10); n++;
    XtSetArg(args[n], XmNtraversalOn, False); n++;
    AppDefOkPB = XmCreatePushButton(AppForm, "Ok", args, n);
    XtManageChild(AppDefOkPB);
    XtAddCallback(AppDefOkPB, XmNactivateCallback, AppDefBtnCB, OK_BTN);
    XmStringFree (label_text);
    XtSetSensitive(AppDefOkPB, False);


    /*
     * Create the Apply button
     */
    label_text = XmStringCreateLtoR("Apply", charset);

    n = 0;
    XtSetArg(args[n], XmNx, 105); n++;
    XtSetArg(args[n], XmNy, 450); n++;
    XtSetArg(args[n], XmNwidth, 80); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, label_text); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 450); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset, 10); n++;
    XtSetArg(args[n], XmNtraversalOn, False); n++;
    AppDefApplyPB = XmCreatePushButton(AppForm, "Apply", args, n);
    XtManageChild(AppDefApplyPB);
    XtAddCallback(AppDefApplyPB, XmNactivateCallback, AppDefBtnCB, APPLY_BTN);
    XmStringFree (label_text);
    XtSetSensitive(AppDefApplyPB, False);


   /*
    * Create the Cancel button
    */

    label_text = XmStringCreateLtoR("Cancel", charset);

    n = 0;
    XtSetArg(args[n], XmNx, 235); n++;
    XtSetArg(args[n], XmNy, 450); n++;
    XtSetArg(args[n], XmNwidth, 80); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNlabelString, label_text); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 10); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 450); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset, 10); n++;
    XtSetArg(args[n], XmNtraversalOn, False); n++;
    CancelPB = XmCreatePushButton(AppForm, "Cancel", args, n);
    XtManageChild(CancelPB);
    XtAddCallback(CancelPB, XmNactivateCallback, AppDefBtnCB, CANCEL_BTN);
    XmStringFree (label_text);

    XtManageChild(AppForm);
    return(TRUE);
}

