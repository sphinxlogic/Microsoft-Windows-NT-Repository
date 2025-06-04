/*
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
        %% \___________________________________%% \
        %% |                                   %%  \
        %% |              MiniSm               %%   \
        %% |          externs.h  c1995         %%    \
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
     of the author     */

    /*
     * Constant Declarations
     */

typedef struct {
    char ApplName[20];
    char ApplCmd[50];
} APPSLST;

typedef struct {
    char TaskName[20];
    int  TaskPid;
} TASKLST;

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

    /*
     *      Global Variable Declarations
     */

extern Arg args[20];

extern FILE *AppsFp;

extern APPSLST *AscAppsList;
extern APPSLST *AscNewAppsList;
extern TASKLST *TaskStruct;

extern char DevList[10][20];
extern char Wtitle[30];
extern char AppsInputFile[132];
extern char decw_disp[12];
extern char delta_str[14];
extern char mpidsfx[6];
extern char nodename[10];
extern char prevproc[18];
extern char task_detached[132];
extern char verstr[];
extern char warnstr[80];

extern int AppLstModified;
extern int AppLstModAck;
extern int AppLstSelPos;
extern int AppLstNumApps;
extern int AppNamstrlen;
extern int AppCmdstrlen;
extern int Xdefaults;
extern int clrnewdirstr;
extern int detached;
extern int n;
extern int newtaskpos;
extern int newtask_count;
extern int num_apps;
extern int num_tasks;
extern int task_selpos;

extern long delta_time[2];

extern Widget MsmForm, MsmFrame;
extern Widget Bboard, NewDir, AppsList, TaskList, AppDefApplyPB, AppDefOkPB;
extern Widget AppsLbl, TaskLbl, Button, RemovePB, AddPB;
extern Widget StopTaskBtn, FsbDialog, InvDetachedWarn, AppsLst;
extern Widget ExceedTaskWarn, DispErrWarn, AppForm, AppNamTxt, AppCmdTxt;

extern XmString WdTitle;
extern XmString CsAlistItems[MAXAPPS+1];
extern XmString CsAnewlistItems[MAXAPPS+1];
extern XmString CsTlistItems[MAXTASKS+1];
extern XmString CurDirstr;
extern XmString SpecifiedDir;
extern XmString WarnMsg;
extern XmStringCharSet charset;

/* end of externs.h */
