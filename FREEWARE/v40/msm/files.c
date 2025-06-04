/*
         %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
         %% \___________________________________%% \
         %% |                                   %%  \
         %% |              MiniSm               %%   \
         %% |           Files.c  c1995          %%    \
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

extern int apps_sort ();


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: GetAppInfo
 * Description: Using the filepath specified by the global string
 *              'AppsInputFile' read in application names and the
 *              respective filespecs and arguments
 *
 * Inputs: None
 *
 * Returns: None
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void GetAppInfo()
{
    int done, status;
    char buffer[80];
    char *ptr1 = 0;
    char *ptr2 = 0;
    APPSLST *AscFiles;

    status = rewind(AppsFp);
    AscFiles = AscAppsList;
    done = FALSE;
    while (!done) {
        memset(&buffer, 0, sizeof(buffer));
        status = fgets(buffer, sizeof(buffer)-1, AppsFp);
        if (status != &buffer) done = TRUE;
        else {
            ptr1 = status;
            if(isspace(*ptr1)) {
                while(isspace(*ptr1)) ptr1++;
                ptr2 = ptr1;
                while(isalnum(*ptr2)) ptr2++;
                *ptr2 = '\0';
                strcpy(AscFiles->ApplName, ptr1);
                ptr1 = ptr2+1;
                while(isspace(*ptr1)) ptr1++;
                ptr2 = strchr(ptr1, '\n');
                *ptr2 = '\0';
                strcpy(AscFiles->ApplCmd, ptr1);
                AscFiles++;
            }
        }
    }
    fclose(AppsFp);
    AscFiles = AscAppsList;
    qsort((void *)AscFiles, num_apps, sizeof(APPSLST), apps_sort);
}





/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: GetNumApps
 * Description: Using the filepath specified by the global string
 *              'AppsInputFile' count number of application names to be
 *              displayed in main window list of applications. This routine
 *              is not particularly intelligent; ie, it only counts file
 *              entries in which the first non-whitespace character is an
 *              alphanumeric character. Else it assumes entry is a comment.
 *
 * Inputs: None
 *
 * Returns: number of qualified entries
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int GetNumApps()
{
    int done, status;
    char buffer[80];
    char *ptr;

    if ((AppsFp = fopen(AppsInputFile, "r")) == NULL) {
        sprintf(buffer, "%s", AppsInputFile);
        FileNotFoundDlg(&buffer, 0, 0x18292, FALSE);
        return(FALSE);
    }

    num_apps = 0;
    status = fgets(buffer, sizeof(buffer)-1, AppsFp);
    ptr = strstr(buffer, "MiniSm");
    if(!ptr) {
        fclose(AppsFp);
        sprintf(buffer, "  Invalid Format");
        DispErrPopup(&AppsInputFile, &buffer, 0, FALSE);
    }
    else {
        done = FALSE;
        while (!done) {
            status = fgets(buffer, sizeof(buffer)-1, AppsFp);
            if (status != &buffer) done = TRUE;
            else {
                ptr = status;
                if(isspace(*ptr)) {
                    while(isspace(*ptr)) ptr++;
                    if(isalnum(*ptr)) num_apps++;
                }
                else if(isalnum(*ptr)) num_apps++;
            }
        }
    }
    return(num_apps);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: LoadAppFileList
 * Description: Load the Application list of files. Use the global string
 *              'AppsInputFile' which points to selected/default filespec to
 *              read in the application names.
 *
 * Inputs: None
 *
 * Returns: Boolean success status; TRUE = good, FALSE is bad
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int LoadAppFileList()
{
    int status;
    APPSLST *AppsPtr;

    if(num_apps) {
        XmListDeleteAllItems(AppsList);
        num_apps = 0;
    }
    num_apps = GetNumApps();
    if(num_apps) {
        if(num_apps > MAXAPPS) num_apps = MAXAPPS;
        AppLstNumApps = num_apps;
        AscAppsList = (APPSLST *) calloc(num_apps+1, sizeof(APPSLST));
        GetAppInfo();

 /* Create the arrays of compound strings from the character arrays */

        AppsPtr = AscAppsList;
        for (n = 0; n < num_apps; n++) {
            CsAlistItems[n] = (XmString) XmStringCreateLtoR
                (AppsPtr->ApplName, charset);
            AppsPtr++;
        }
        if(AppsList) {
            n = 0;
            XtSetArg(args[n], XmNitems, CsAlistItems); n++;
            XtSetArg(args[n], XmNitemCount, num_apps); n++;
            XtSetValues(AppsList, args, n);
        }
        return(TRUE);
    }
    else return(FALSE);
}

