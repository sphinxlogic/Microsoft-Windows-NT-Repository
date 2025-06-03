/******************************************
 
  HOTJOT - A Quick Note Taking Program

  Written by - Patrick.Meyer@msfc.nasa.gov

  Date - October 31, 1995

******************************************/

/*** INCLUDE FILES       ***/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <Xm/AtomMgr.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/MessageB.h>
#include <Xm/Protocols.h>
#include <Xm/PushB.h>
#include <Xm/SelectioB.H>
#include <Xm/ToggleB.H>
#include <Xm/MwmUtil.h>
#include <Xm/Text.h>
#include <Xm/MrmAppl.h>
#include <DXm/DECSpecific.h>
#include <DXm/DXmPrint.h>
#include "hotjot.xbm"


/*** DATA TYPE DEFS      ***/
#define MAX_JOTS          100        /* The maximum number of jots on screen     */
#define MAX_TITLE_LENGTH  100        /* Maximum characters in a Jot title        */

  /* Main Jot Structure */
typedef struct { 
   Position  x,y;                    /* Jot position on screen                   */
   Dimension height, width;          /* Jot dimension                            */
   int       screen;                 /* Jot's screen for multi-screen systems    */
   time_t    alarm;                  /* Unused currently                         */
   Boolean   open;                   /* Jot's open state                         */
   Boolean   hidden;                 /* Jot's hidden status - unused currently   */
   char      *colorname;             /* Jot's current colorname                  */
} JotStuff;



/*** GLOBAL VARIABLES    ***/

XtAppContext app_context;            /* Program Application Context              */
Widget   toplevel, toplevel2;        /* Toplevel Shells for screens 1 and 2      */
Boolean  edited = FALSE;             /* Once in 15 minutes, true if a jot edited */

Pixmap icon;                         /* HOTJOT ICON Pixmap                       */
Widget HotJots[MAX_JOTS];            /* Array of JOT Windows                     */
Widget printDB = NULL;               /* Printer dialog box                       */
Widget search_current = NULL;        /* Search dialog box                        */
int    HotJot_count = 0;             /* Count of live JOTS                       */
int    print_which = 0;              /* Index of which jot to print              */
GC     gc;                           /* Graphic context for colors               */


/*** INTERNAL PROTOTYPES ***/
void CreateNewNote(Position x, Position y, Dimension height,
                   Dimension width, Boolean, int screen, Boolean hidden, 
                   char *colorname, char *note);
void DeleteHotJot(Widget w, Boolean ask);
void ExtractTitle(char *data, char *title_string); 
void MapDialogCB(Widget dialog, void *nada, XmAnyCallbackStruct *cb);
void MenuCB(Widget w, int item, XmPushButtonCallbackStruct *cb); 
void MenuColorCB(Widget w, int item, XmPushButtonCallbackStruct *cb); 
void MenuPostEH(Widget w, Widget menu, XEvent *event); 
void MenuPrintCB(Widget w, int item, XmPushButtonCallbackStruct *cb);
void ModifyTextCB(Widget w, void *nada, XmTextVerifyCallbackStruct *cb); 
void PrintCreateDB(void);
void PrintDialogCB(Widget w, void *nada, XmPushButtonCallbackStruct *cb);
void PrintJot(Widget w, Boolean all);
void PrintWhichCB(Widget w, int item, XmPushButtonCallbackStruct *cb);
void Sash(Widget w, void *nada, XmPushButtonCallbackStruct *cb);
void SaveHotJot(void);
void SearchJotSearch(Widget w, void *nada, XmAnyCallbackStruct *cb); 
void SearchCB(Widget w, Widget toggle, XmSelectionBoxCallbackStruct *cb);
void SwitchScreens(Widget w);
void UpJotUp(void); 
Boolean VerifyAction(char *title, char *message, Boolean warning); 
void VerifyActionCB(Widget w, char *done, XmAnyCallbackStruct *cb);

/**************************************************************
*                      AutoSaveEH                             *
* Function:                                                   *
*    Auto save event handler waits 15 minutes and check to    *
*    see if any edits have been made.  If so, it saves your   *
*    jots - don't want you to lose them ;)                    *
***************************************************************
* Parameters:
*   w        in:  A widget - unused.
*   id       in:  Timeout Timer Id.
*/
void AutoSaveEH(Widget w, XtIntervalId id) {

    /* Save if edited and reset edited flag */
    if (edited) SaveHotJot();
    edited = FALSE;

    /* Check again in 15 minutes */
    XtAppAddTimeOut(app_context, 15*60*1000, (XtTimerCallbackProc) AutoSaveEH, w);
}



/**************************************************************
*                      CreateNewNote                          *
* Function:                                                   *
*    Creates a jot.                                           *
***************************************************************
* Parameters:
*    x            in:  Jot's X position on screen.
*    y            in:  Jot's Y position on screen.
*    height       in:  Jot's Height.
*    width        in:  Jot's Width.
*    open         in:  Jot's open state (TRUE=OPEN)
*    screen       in:  Which screen for dual screens.
*    hidden       in:  Unused.
*    colorname    in:  Color of Jot.
*    note         in:  Starting note data (usually none).
*/
void CreateNewNote(Position x, Position y, 
                   Dimension height, Dimension width, 
                   Boolean open, int screen, Boolean hidden,
                   char *colorname,
                   char *note) {
   Arg      args[20];
   Cardinal n;
   Widget   button;
   Widget   text;
   Widget   popup_menu;
   Widget   color_pulldown;
   Widget   print_pulldown;
   XmString title;
   JotStuff *new_jot;
   Widget   new_toplevel;
   char     title_string[100];

   if (HotJot_count < MAX_JOTS) {
   
      new_jot = (JotStuff *) XtMalloc(sizeof(JotStuff));

      new_jot->hidden = FALSE;

      n = 0;
      if (x != -255) {
         new_jot->x         = x;
         new_jot->y         = y;
	 new_jot->screen    = screen;
	 new_jot->height    = height;
	 new_jot->width     = width;
	 new_jot->open      = open;
         new_jot->hidden    = hidden;
	 new_jot->colorname = XtNewString(colorname);

         if (hidden) {
            XtSetArg(args[n], XmNmappedWhenManaged, FALSE); n++;
         }

	 XtSetArg(args[n], XmNx, x); n++;
	 XtSetArg(args[n], XmNy, y); n++;

         if (open) {
            XtSetArg(args[n], XmNheight, height); n++;
         }

	 XtSetArg(args[n], XmNwidth, width); n++;
      } else {
         new_jot->x         = 100;
         new_jot->y         = 100;
         new_jot->hidden    = FALSE;
	 new_jot->open      = TRUE;
	 new_jot->colorname = XtNewString("None");
      }
  
      if (screen == XDefaultScreen(XtDisplay(toplevel)))
         new_toplevel = toplevel;
      else
         new_toplevel = toplevel2;
      XtSetArg(args[n], XmNmwmDecorations, 2); n++; 
      XtSetArg(args[n], XmNuserData, new_jot); n++;
      XtSetArg(args[n], XmNautoUnmanage, FALSE); n++;
      XtSetArg(args[n], XmNdefaultPosition, FALSE); n++;
      HotJots[HotJot_count] = XmCreateFormDialog(new_toplevel, "form", args, n);

      ExtractTitle(note, title_string);

      n = 0;
      title = XmStringCreateSimple(title_string);
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNlabelString, title); n++;
      XtSetArg(args[n], XmNresizable, FALSE); n++;
      if (!open)
         XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
      button = XmCreatePushButton(HotJots[HotJot_count], "button", args, n);
      XtAddCallback(button, XmNactivateCallback, (XtCallbackProc) Sash, NULL);
      XtManageChild(button);
      XmStringFree(title);

      n = 0;
      XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
      XtSetArg(args[n], XmNtopWidget, button); n++;
      XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
      XtSetArg(args[n], XmNmaxLength, 10000); n++;
      XtSetArg(args[n], XmNvalue, note); n++;
      XtSetArg(args[n], XmNresizable, FALSE); n++; 
      text = XmCreateText(HotJots[HotJot_count], "Text", args, n); 
      if (open) 
         XtManageChild(text); 
      XtAddCallback(text, XmNmodifyVerifyCallback, (XtCallbackProc) ModifyTextCB, NULL);

      {
	 XmString save = XmStringCreateSimple("Save");
	 XmString new = XmStringCreateSimple("New Jot");
	 XmString delete = XmStringCreateSimple("Delete This Jot");
	 XmString colors = XmStringCreateSimple("Change Color");
         XmString lower = XmStringCreateSimple("Lower All");
         XmString raise = XmStringCreateSimple("Raise All");
         XmString swscr = XmStringCreateSimple("Switch Screens");
         XmString search = XmStringCreateSimple("Search...");
         XmString print = XmStringCreateSimple("Print");
	 XmString exit = XmStringCreateSimple("Exit");
	 popup_menu = XmVaCreateSimplePopupMenu(HotJots[HotJot_count], "popup", (XtCallbackProc) MenuCB, 
	      XmVaCASCADEBUTTON, colors, 'C',
	      XmVaPUSHBUTTON, new, 'N', NULL, NULL,
	      XmVaPUSHBUTTON, delete, 'D', NULL, NULL,
	      XmVaPUSHBUTTON, lower, 'L', NULL, NULL,
	      XmVaPUSHBUTTON, raise, 'R', NULL, NULL,
	      XmVaPUSHBUTTON, swscr, 'w', NULL, NULL,
	      XmVaPUSHBUTTON, search, 'H', NULL, NULL,
	      XmVaCASCADEBUTTON, print, 'P',
	      XmVaPUSHBUTTON, save, 'S', NULL, NULL,
	      XmVaPUSHBUTTON, exit, 'E', NULL, NULL,
	      NULL);
	 XmStringFree(save);
	 XmStringFree(new);
	 XmStringFree(exit);
	 XmStringFree(delete);
	 XmStringFree(colors);
	 XmStringFree(lower);
	 XmStringFree(raise);
         XmStringFree(swscr);
         XmStringFree(search);
         XmStringFree(print);
      }

      {
	 XmString defaul = XmStringCreateSimple("Default");
	 XmString blue = XmStringCreateSimple("Sky Blue");
	 XmString violet = XmStringCreateSimple("Violet");
	 XmString gold = XmStringCreateSimple("Gold");
	 XmString tomato = XmStringCreateSimple("Tomato");
	 XmString palegreen = XmStringCreateSimple("Pale Green");
	 XmString darkkhaki = XmStringCreateSimple("Dark Khaki");
	 XmString slateblue = XmStringCreateSimple("Slate Blue");
	 XmString violetred = XmStringCreateSimple("Violet Red");
	 XmString forestgreen = XmStringCreateSimple("Forest Green");
	 color_pulldown = XmVaCreateSimplePulldownMenu(popup_menu, "colors", 0, 
			     (XtCallbackProc) MenuColorCB,
	    XmVaPUSHBUTTON, defaul, 'e', NULL, NULL,
	    XmVaPUSHBUTTON, blue, 'S', NULL, NULL,
	    XmVaPUSHBUTTON, violet, 'V', NULL, NULL,
	    XmVaPUSHBUTTON, gold, 'G', NULL, NULL,
	    XmVaPUSHBUTTON, tomato, 'T', NULL, NULL,
	    XmVaPUSHBUTTON, palegreen, 'P', NULL, NULL,
	    XmVaPUSHBUTTON, darkkhaki, 'D', NULL, NULL,
	    XmVaPUSHBUTTON, slateblue, 'B', NULL, NULL,
	    XmVaPUSHBUTTON, violetred, 'R', NULL, NULL,
	    XmVaPUSHBUTTON, forestgreen, 'F', NULL, NULL,
	    NULL);
	 XmStringFree(defaul);
	 XmStringFree(blue);
	 XmStringFree(violet);
	 XmStringFree(gold);
	 XmStringFree(violetred);
	 XmStringFree(tomato);
	 XmStringFree(palegreen);
	 XmStringFree(darkkhaki);
	 XmStringFree(slateblue);
	 XmStringFree(forestgreen);     
      }

      {
	 XmString this = XmStringCreateSimple("This Jot");
	 XmString all = XmStringCreateSimple("All Jots");
         XmString dots = XmStringCreateSimple("...");
	 print_pulldown = XmVaCreateSimplePulldownMenu(popup_menu, "prints", 7, 
			     (XtCallbackProc) MenuPrintCB,
	    XmVaPUSHBUTTON, this, 'T', NULL, NULL,
	    XmVaPUSHBUTTON, all, 'A', NULL, NULL,
	    XmVaPUSHBUTTON, dots, 'A', NULL, NULL,
	    NULL);
	 XmStringFree(this);
	 XmStringFree(all);
         XmStringFree(dots);
      }

      XtAddEventHandler(HotJots[HotJot_count], ButtonPressMask, FALSE, (XtEventHandler) MenuPostEH, 
			popup_menu);

      if (strcmp(colorname, "None") != 0) {
	 XtVaSetValues(HotJots[HotJot_count], XtVaTypedArg,  XmNbackground, XmRString, colorname, strlen(colorname), NULL);
	 XtVaSetValues(text,   XtVaTypedArg,  XmNbackground, XmRString, colorname, strlen(colorname), NULL);
	 XtVaSetValues(button, XtVaTypedArg,  XmNbackground, XmRString, colorname, strlen(colorname), NULL);
      } 

      XtManageChild(HotJots[HotJot_count]);

      HotJot_count++;
   } else {
      if (VerifyAction("Too Many", 
	  "You have too many notes 100 is max.", TRUE)) {
	 ;
      }
   }
}

/**************************************************************
*                      DownJotDown                            *
* Function:                                                   *
*    Lowers all jot windows                                   *
***************************************************************
* Parameters:
*    None. 
*/
void DownJotDown(void) {

   XLowerWindow(XtDisplay(toplevel), XtWindow(toplevel));

   /* If 2 screens, do both */
   if (toplevel != toplevel2)
      XLowerWindow(XtDisplay(toplevel2), XtWindow(toplevel2));
}

/**************************************************************
*                      ExtractTitle                           *
* Function:                                                   *
*    Extracts a title from the first line in a jot to display *
*    on button when closed.                                   *
***************************************************************
* Parameters:
*   data           in:  The contents of a jot.                      
*   title_string  out:  The returned title.                       
*/
void ExtractTitle(char *data, char *title_string) {
   int i,j;               /* Loop control variables */
   Boolean done, start;   

   done = start = FALSE;

   /* While not end of text search for title */
   for (i = j = 0; i < strlen(data) && !done && j < MAX_TITLE_LENGTH-1; i++) {

      /* Don't start collecting letters 'til first non-space */
      if (!isspace(data[i]))
	 start = TRUE;
      if (data[i] == '\n' || data[i] == '\0')
	 done = TRUE;
      if (start)
	 title_string[j++] = data[i];

   }
   /* Null terminate */
   title_string[j++] = '\0';
   
   /* Remove any trailing spaces */
   j = strlen(title_string);
   while (isspace(title_string[j-1])) {
      title_string[j-1] = '\0';
      j--;
   }

   /* If no title found, say no title */
   if (strcmp(title_string, "") == 0) {
      strcpy(title_string, "No Title");
   }
}

/**************************************************************
*                      DeleteHotJot                           *
* Function:                                                   *
*    Deletes and unsed jot... confirm it first.               *
***************************************************************
* Parameters:
*    w      in:   Delete button widget.
*    ask    in:   Whether to ask if use is sure.  FALSE
*                 if called from switch screens.
*/
void DeleteHotJot(Widget w, Boolean ask) {
   Widget parent;
   Widget form;
   int i;
   JotStuff *jot_data;

   parent = XtParent(XtParent(XtParent(w)));

   if (!ask || VerifyAction("Delete", 
                     "Are you sure you wish to delete this Jot", TRUE)) {

      if (HotJot_count > 1) {
	 i = 0;
	 while (i < HotJot_count && parent != HotJots[i])
	    i++;

	 if (i < HotJot_count) {
	    XtVaGetValues(parent, XmNuserData, &jot_data, NULL);
	    XtFree((char *) jot_data->colorname);
	    XtFree((char *) jot_data);
	    XtDestroyWidget(parent);
	    memmove(&HotJots[i], &HotJots[i+1], ((HotJot_count-i-1) * sizeof(Widget)));
	    HotJot_count--;
	    edited = TRUE;
	 }
      } else {
	 Widget text = XtNameToWidget(parent, "*Text");
	 XmTextSetString(text, "");
      }
   }
}

/**************************************************************
*                      HideJot                                *
* Function:                                                   *
*    Currently unused.  I would like to have hidden jots.     *
***************************************************************
* Parameters:
*/
void HideJot(Widget w) {
   Widget  parent, form;
   JotStuff *jot_data;
   int     i;

   parent = XtParent(XtParent(XtParent(XtParent(w))));
   form   = XtNameToWidget(parent, "*form");
   XtVaGetValues(form, XmNuserData, &jot_data, NULL);

   jot_data->hidden;
   XtUnmapWidget(parent);   
}





/**************************************************************
*                      JotSearch                              *
* Function:                                                   *
*    Searches a single jot, or all jots for the given string  *
***************************************************************
* Parameters:
*   current  in:  The JOT initiating the search.              
*   string   in:  The string to search for.
*   all      in:  Indicates whether to search only current or all jots.
*/
void JotSearch(Widget current, char *string, Boolean all) {
   char           *data;          /* Pointer to jot's string */
   char           *p;             /* Ptr. used during search */
   int            i;              /* Loop control            */
   int            pass = 0;      
   Boolean        done = FALSE;
   JotStuff       *jot_data;
   Widget         text;
   Widget         temp;
   XmTextPosition position;

   i = 0;
   while (current != HotJots[i] && i < HotJot_count) i++;

   if (i < HotJot_count) {
      text = XtNameToWidget(HotJots[i], "*Text");
      XtVaGetValues(text, XmNvalue, &data, NULL);
      position = XmTextGetCursorPosition(text);

      do {
	 if (current == HotJots[i]) {
	   pass++;
	 }

	 text = XtNameToWidget(HotJots[i], "*Text");
         XtVaGetValues(text, XmNvalue, &data, NULL);
         p = strstr(&data[position], string);

         /* If not found from current position, search from beginning */         
         if (p == NULL && !all) 
   	    p = strstr(data, string);

         if (!all) {
            if (p != NULL) {
               position = (XmTextPosition) (p - data);
               XmTextSetInsertionPosition(text, position);
               XmTextSetSelection(text, position, position+strlen(string), CurrentTime);

               XtVaGetValues(HotJots[i], XmNuserData, &jot_data, NULL);
               if (!jot_data->open) {
                  temp = XtNameToWidget(HotJots[i], "*button");
                  Sash(temp, NULL, NULL);
               }
               done = TRUE;
            } else { 
               XBell(XtDisplay(toplevel), 0);
            }
         } else {
            if (p != NULL) {
               position = (XmTextPosition) (p - data);
               XmTextSetInsertionPosition(text, position);
               XmTextSetSelection(text, position, position+strlen(string), CurrentTime);
               
               XtVaGetValues(HotJots[i], XmNuserData, &jot_data, NULL);
               if (!jot_data->open) {
                  temp = XtNameToWidget(HotJots[i], "*button");
                  Sash(temp, NULL, NULL);
               }
               done = TRUE;
            } else {
               i = (i+1)%HotJot_count;
               position = 0;
            }
         }

         if (pass == 2) done = TRUE;         

      } while (!done);

      if (p == NULL)
	 XBell(XtDisplay(toplevel), 0); 
   }

}

/**************************************************************
*                      LoadHotJot                             *
* Function:                                                   *
*    Loads Hot Jot from ".HOTJOT" file.                       *
***************************************************************
* Parameters:
*    None.
*/
void LoadHotJot(void) {
   FILE *fp;              /* File Ptr. for .HOTJOT file */
   char buffer[10000];    /* Buffer to hold single JOT  */
   char colorname[30];    /* Colorname string           */
   int ch;                
   int i;
   int screen;
   int x,y;
   int height, width;
   int open, hidden;

   /* Open JOT file */
   fp = fopen("SYS$LOGIN:.HotJot", "r");

   /* Load in jot info. for each jot */
   if (fp != NULL) {
      while (!feof(fp)) {
         i = 0;
	 fscanf(fp, "%d %d %d %d %d %d %d %s\n", &x, &y, &height, &width, &open, &screen,
                                                 &hidden,
                                                 colorname);
         do {
            ch = fgetc(fp);
            if (ch != 255 && i < 9999 && ch != EOF)
               buffer[i++] = ch;
         } while (ch != EOF && ch != 255 && i < 9999);
         buffer[i] = '\0';
         CreateNewNote(x,y,height,width,open,screen,hidden,colorname,buffer);
      }
      fclose(fp);
   }

   /* If no jots, create at least one */
   if (!HotJot_count) {
      CreateNewNote(-255,0,0,0,TRUE,0,0,"None","");
   }
}


/**************************************************************
*                      MapDialogCB                            *
* Function:                                                   *
*    Centers verify action dialog.                            *
***************************************************************
* Parameters:
*    dialog   in:  The verify action dialog.
*    nada     in:  Nothing (unused).
*    cb       in:  Map callback data.
*/
void MapDialogCB(Widget dialog, void *nada, XmAnyCallbackStruct *cb) {
   Position x,y;
   Dimension w, h;
   Dimension s_w, s_h;
  
   XtVaGetValues(dialog, XmNwidth, &w, XmNheight, &h, NULL);
   s_w = WidthOfScreen(XtScreen(dialog));
   s_h = HeightOfScreen(XtScreen(dialog));
   x = (s_w/2) - (w/2);
   y = (s_h/2) - (h/2);
   XtVaSetValues(dialog, XmNx, x, XmNy, y, NULL);
}


/**************************************************************
*                      MenuPostEH                             *
* Function:                                                   *
*    Event handler to post the MB3 popup menu.                *
***************************************************************
* Parameters:
*   w        in:  The event widget.                           
*   string   in:  The menu to post.
*   event    in:  The MB3 event.
*/
void MenuPostEH(Widget w, Widget menu, XEvent *event) {
 
   /* Position and post */
   XmMenuPosition(menu, (XButtonPressedEvent *)event);
   XtManageChild(menu);
}

/**************************************************************
*                      MenuCB                                 *
* Function:                                                   *
*    Acts on users choice from popup menu.                    *
***************************************************************
* Parameters:
*   w        in:  Push button on menu.                        
*   item     in:  Selected item number.    
*   cb       in:  Push button callback data.                           
*/
void MenuCB(Widget w, int item, XmPushButtonCallbackStruct *cb) {
   Widget parent;
   int    decor;

   switch (item) {
      case 1:  /* New Jot */
         { 
            Widget parent, form;
            Dimension height, width;
            Position x,y;
            JotStuff *jot_data;
            
            parent = XtParent(XtParent(XtParent(XtParent(w))));
            form   = XtNameToWidget(parent, "*form");
            XtVaGetValues(form, XmNx, &x, XmNy, &y, XmNuserData, &jot_data, NULL);
            if (jot_data->open) {
               XtVaGetValues(form, XmNheight, &height, XmNwidth, &width, NULL);
            } else {
               height = jot_data->height;
               width = jot_data->width;
            }

            CreateNewNote(x,y+=50,height,width,TRUE,XDefaultScreen(XtDisplay(w)),
                          FALSE,"None","");
         }
         break;
      case 2:  /* Delete Jot */
         DeleteHotJot(w, TRUE);
         break;
      case 3:  /* Lower Jots */
         DownJotDown();
         break;
      case 4:  /* Raise Jots */
         UpJotUp();
         break;
      case 5:  /* Switch screens */
         SwitchScreens(w);
         break;
      case 6:  /* Do search */
         SearchJotSearch(w, NULL, NULL);
         break;
      case 8:  /* Save jot  */
         SaveHotJot();
         break;
      case 9:  /* Exit      */
         if (edited) {
            if (VerifyAction("Exit", 
                "There are unsaved edits.  Save before exiting?", FALSE)) {
               SaveHotJot();
            }
         } 
         exit(0);
         break;
   }
}

/**************************************************************
*                      MenuPrintCB                            *
* Function:                                                   *
*    Acts on users choice from popup menu.                    *
***************************************************************
* Parameters:
*   w        in:  Push button on menu.                        
*   item     in:  Selected item number.    
*   cb       in:  Push button callback data.                           
*/
void MenuPrintCB(Widget w, int item, XmPushButtonCallbackStruct *cb) {
   Widget parent;

   switch (item) {
      case 0:   /* This jot */
         PrintJot(w, FALSE);
         break;
      case 1:   /* All jots */
         PrintJot(w, TRUE);
         break;
      case 2:   /* Give dialog box */
         if (printDB == NULL)
            PrintCreateDB();
         XtVaSetValues(printDB, XmNuserData, w, NULL);
         XtManageChild(printDB);
         break;
   }
}

/**************************************************************
*                      MenuColorCB                            *
* Function:                                                   *
*    Acts on users choice from popup menu.                    *
***************************************************************
* Parameters:
*   w        in:  Push button on menu.                        
*   item     in:  Selected item number.    
*   cb       in:  Push button callback data.                           
*/
void MenuColorCB(Widget w, int item, XmPushButtonCallbackStruct *cb) {
   Widget parent;
   Widget form;
   Widget text;
   Widget button;
   char   *colorname;
   JotStuff *jot_data;

   parent = XtParent(XtParent(XtParent(XtParent(XtParent(w)))));
   form = XtNameToWidget(parent, "*form");
   text = XtNameToWidget(parent, "*Text");
   button = XtNameToWidget(parent, "*button");
   XtVaGetValues(form, XmNuserData, &jot_data, NULL);

   edited = TRUE;

   switch (item) {
      case 0:
         colorname = "None";
         break;
      case 1:
         colorname = "SkyBlue";
         break;
      case 2:
         colorname = "Violet";
         break;
      case 3:
         colorname = "Gold";
         break;
      case 4:
         colorname = "Tomato";
         break;
      case 5:
         colorname = "PaleGreen";
         break;
      case 6:
         colorname = "DarkKhaki";
         break;
      case 7:
         colorname = "SlateBlue";
         break;
      case 8:
         colorname = "VioletRed";
         break;
      case 9:
         colorname = "ForestGreen";
         break;
   }

   if (strcmp(colorname, "None") != 0) {
      XtVaSetValues(form,   XtVaTypedArg,  XmNbackground, XmRString, colorname, strlen(colorname), NULL);
      XtVaSetValues(text,   XtVaTypedArg,  XmNbackground, XmRString, colorname, strlen(colorname), NULL);
      XtVaSetValues(button, XtVaTypedArg,  XmNbackground, XmRString, colorname, strlen(colorname), NULL);
   } else {
      Pixel background;
      XtVaGetValues(toplevel, XmNbackground, &background, NULL);
      XtVaSetValues(form, XmNbackground, background, NULL);
      XtVaSetValues(text, XmNbackground, background, NULL);
      XtVaSetValues(button, XmNbackground, background, NULL);
   }

   XtFree((char *) jot_data->colorname);
   jot_data->colorname = XtNewString(colorname);   
}

/**************************************************************
*                      ModifyTextCB                           *
* Function:                                                   *
*    Toggle edited flag when user edits any jot.              *
***************************************************************
* Parameters:
*    w     in:   The text field widget.
*    nada  in:   Unused.
*    cb    in:   Unused callback data.
*/
void ModifyTextCB(Widget w, void *nada, XmTextVerifyCallbackStruct *cb) {
   edited = TRUE;
}


/**************************************************************
*                      PrintCreateDB                          *
* Function:                                                   *
*    Creates the print dialog box.                            *
***************************************************************
* Parameters:
*    None. 
*/
void PrintCreateDB(void) {
   Arg      args[15];     /* Widget resources to change                   */
   Cardinal n;            /* Widget resource count                        */
   Widget   which_menu;   /* Which jot to print menu                      */
   int      suppress;     /* BIT flag for which print options to suppress */

   /* Create the print dialog box */
   n = 0;
   suppress = DXmSUPPRESS_PAGE_RANGE;
   XtSetArg(args[n], DXmNsuppressOptionsMask, suppress); n++;
   XtSetArg(args[n], DXmNunmanageOnCancel, TRUE); n++;
   XtSetArg(args[n], DXmNunmanageOnOk, TRUE); n++;
   XtSetArg(args[n], XmNdefaultPosition, FALSE); n++;
   printDB = DXmCreatePrintDialog(toplevel, "Print Dialog", args, n);

   /* Create the popup menu which asks which jot to print */
   {
      XmString this = XmStringCreateSimple("This Jot");
      XmString all = XmStringCreateSimple("All Jots");
      which_menu = XmVaCreateSimpleRadioBox(printDB, "which", 0, (XtCallbackProc) PrintWhichCB, 
	   XmVaRADIOBUTTON, this, 'D', NULL, NULL,
	   XmVaRADIOBUTTON, all, 'L', NULL, NULL,
	   NULL);
      XmStringFree(this);
      XmStringFree(all);
   }
   XtManageChild(which_menu);

   /* Add callbacks */
   XtAddCallback(printDB, XmNmapCallback, (XtCallbackProc) MapDialogCB, NULL);
   XtAddCallback(printDB, XmNokCallback, (XtCallbackProc) PrintDialogCB, NULL);
}

/**************************************************************
*                      PrintDialogCB                          *
* Function:                                                   *
*    Searches a single jot, or all jots for the given string  *
***************************************************************
* Parameters:
*   w        in:  Print Dialog Box widget.                    
*   nada     in:  Nothing (unused).
*   cb       in:  Unused callback data.
*/
void PrintDialogCB(Widget w, void *nada, XmPushButtonCallbackStruct *cb) {
   /* If print_which is non-zero, we print all.  Else print one */
   if (print_which) {
      PrintJot(w, TRUE);
   } else {
      /* Find the jot to print and send the button which will 
       * be used to indicate which jot to print.
       */
      Widget button;
      XtVaGetValues(printDB, XmNuserData, &button, NULL);
      PrintJot(button, FALSE);
   }
}

/**************************************************************
*                      PrintWhichCB                           *
* Function:                                                   *
*    Sets which jot to print when selected from print dialog  *
***************************************************************
* Parameters:
*    w     in:  The toggle Button on print dialog.
*    item  in:  The selected item no.
*    cb    in:  Push button/ Toggle button callback data. (unused).
*/
void PrintWhichCB(Widget w, int item, XmPushButtonCallbackStruct *cb) {
   print_which = item;
}

/**************************************************************
*                      PrintJot                               *
* Function:                                                   *
*    Creates and prints print file.                           *
***************************************************************
* Parameters:
*     w    in:  Jot to print.
*     all  in:  True if to print all.
*/
void PrintJot(Widget w, Boolean all) {
   FILE      *fp;
   int       i;
   char      *data;
   Widget    text;
   Widget    top_parent;
   XmString  file_pointer[1];

   if (printDB == NULL)
      PrintCreateDB();

   fp = fopen("SYS$LOGIN:HotJot.txt", "w");

   if (all) {
      /* Loop through all jots and print them to file */
      for (i = 0; i < HotJot_count; i++) {
	 text = XtNameToWidget(HotJots[i], "*Text");
	 XtVaGetValues(text, XmNvalue, &data, NULL);

	 fprintf(fp, data);
	 if (i+1 < HotJot_count) {
	    fputc('\f', fp);
	 }
      }
   } else {
      top_parent = w;
      while(strcmp(XtName(top_parent), "form") != 0)
	 top_parent = XtParent(top_parent);

      text = XtNameToWidget(top_parent, "*Text");
      XtVaGetValues(text, XmNvalue, &data, NULL);
      fprintf(fp, data);
   }

   fclose(fp);

   file_pointer[0] = XmStringCreateSimple("SYS$LOGIN:HOTJOT.TXT");
   DXmPrintWgtPrintJob(printDB, file_pointer, 1);
}

/**************************************************************
*                      Sash                                   *
* Function:                                                   *
*    Opens and closes a jot window.                           *
***************************************************************
* Parameters:
*    w       in:  Sash push button.
*    nada    in:  Nothing (unused).
*    cb      in:  Push button callback data.
*/
void Sash(Widget w, void *nada, XmPushButtonCallbackStruct *cb) {
   Widget parent;
   Widget form;
   Widget text;
   char   *data;
   char   title_string[100];
   Dimension high,wide;
   int     i,j;
   Boolean done,start;
   XmString title;
   JotStuff *jot_data;

   parent = XtParent(XtParent(w));
   form   = XtNameToWidget(parent, "*form");
   XtVaGetValues(form, XmNuserData, &jot_data, NULL);
   text = XtNameToWidget(form, "*Text");

   edited = TRUE;

   /* Open because text managed */
   if (XtIsManaged(text)) {
      XtVaGetValues(form, XmNheight, &high, XmNwidth, &wide, NULL);

      XtUnmanageChild(text);

      /* Get current size ... here if minimal 
      XtVaGetValues(parent, XmNheight, &high, XmNwidth, &wide, NULL);
      */

      /* Get text data to get title */
      XtVaGetValues(text, XmNvalue, &data, NULL);

      ExtractTitle(data, title_string);

      title = XmStringCreateSimple(title_string);      

      jot_data->height = high;
      jot_data->width  = wide;
      jot_data->open   = FALSE;
      XtVaSetValues(w, XmNbottomAttachment, XmATTACH_FORM,
                       XmNwidth, wide, 
                       XmNlabelString, title,
                       NULL);
      XtVaSetValues(parent, XmNwidth, wide, NULL);
      XmStringFree(title);
   } else {

      XtVaGetValues(form, XmNwidth, &wide, NULL);
      XtVaSetValues(w, XmNbottomAttachment, XmATTACH_NONE, NULL);
      XtVaSetValues(text, XmNwidth, wide, NULL);
      XtManageChild(text);
      XtVaSetValues(parent, XmNheight, jot_data->height, NULL);
      XtVaSetValues(form, XmNheight, jot_data->height, 
                          XmNwidth, wide, 
                          NULL);

      XRaiseWindow(XtDisplay(parent), XtWindow(parent));

      title = XmStringCreateSimple(" ");      
      XtVaSetValues(w, XmNlabelString, title,
                       NULL);
      jot_data->open = TRUE;
      XmStringFree(title);

      XmProcessTraversal(text, XmTRAVERSE_CURRENT);
   }
}


/**************************************************************
*                      SaveHotJot                             *
* Function:                                                   *
*    Saves the jot info.                                      *
***************************************************************
* Parameters:
*    None.
*/
void SaveHotJot() {
   FILE      *fp;
   int       i;
   char      *data;
   Widget    text;
   Position  x,y;
   Dimension height, width;
   Boolean    open;
   JotStuff   *jot_data;
   
   fp = fopen("SYS$LOGIN:.HotJot", "w");

   for (i = 0; i < HotJot_count; i++) {
      XtVaGetValues(HotJots[i], XmNx,        &x, 
                                XmNy,        &y,
                                XmNheight,   &height, 
                                XmNwidth,    &width,
                                NULL);
      text = XtNameToWidget(HotJots[i], "*Text");
      XtVaGetValues(HotJots[i], XmNuserData, &jot_data, NULL);
      XtVaGetValues(text, XmNvalue, &data, NULL);

      if (!jot_data->open) {
         height = jot_data->height;
         width  = jot_data->width;
      }

      fprintf(fp, "%d %d %d %d %d %d %d %s\n", x, y, 
                                      height, width, 
                                      jot_data->open,
                                      jot_data->screen,
                                      jot_data->hidden,
                                      jot_data->colorname);
      fprintf(fp, data);
      if (i+1 < HotJot_count) {
         fputc(255, fp);
         fputc('\n', fp);
      }
   }

   fclose(fp);
   edited = FALSE;
}


/**************************************************************
*                      SearchJotSearch                        *
* Function:                                                   *
*    Creates and manages search dialog.                       *
***************************************************************
* Parameters:
*   w        in:  Unused widget.                              
*   nada     in:  Nothing (unused).         
*   cb       in:  Unused callback data.                                
*/
void SearchJotSearch(Widget w, void *nada, XmAnyCallbackStruct *cb) {
   static Widget search_dialog = NULL;

   if (search_dialog == NULL) {
      Cardinal n;
      Arg      args[10];
      XmString title;
      Widget   toggle;  

      n = 0;
      title = XmStringCreateSimple("Hot Jot Search");
      XtSetArg(args[n], XmNdialogTitle, title); n++;
      XtSetArg(args[n], XmNdefaultPosition, FALSE); n++;
      search_dialog = XmCreatePromptDialog(toplevel, "SearchEm", args, n);
      XtAddCallback(search_dialog, XmNmapCallback, (XtCallbackProc) MapDialogCB, NULL);
      XmStringFree(title);

      n = 0;
      toggle = XmCreateToggleButton(search_dialog, "Search All", args, n);
      XtManageChild(toggle);

      XtAddCallback(search_dialog, XmNokCallback, (XtCallbackProc) SearchCB, toggle);
   }

   search_current = XtParent(XtParent(XtParent(w)));

   XtManageChild(search_dialog);
}



/**************************************************************
*                      SearchCB                               *
* Function:                                                   *
*    Extracts string from search dialog and initiate search   *
***************************************************************
* Parameters:
*    w      in:  The search dialog.
*    toggle in:  Search toggle widget (all or this jot).
*    cb     in:  The dialog callback structure.
*/
void SearchCB(Widget w, Widget toggle, XmSelectionBoxCallbackStruct *cb) {
   char *string;
 
   if (XmStringGetLtoR(cb->value, XmSTRING_DEFAULT_CHARSET, &string)) {
      JotSearch(search_current, string, XmToggleButtonGetState(toggle));
   }
}

/**************************************************************
*                      SwitchScreens                          *
* Function:                                                   *
*    Deletes a jot on one screen after duplicating it on the  *
*    other.  Only useful for two screen systems.              *
***************************************************************
* Parameters:
*    w     in:  The menu button for the jot to switch screens.
*/
void SwitchScreens(Widget w) {
   int      i;           /* Loop control variable       */
   JotStuff *jot_data;   /* Jot data structure          */
   Widget   parent;      /* Parent shell of menu button */
   Widget   form;        /* Form widget in found JOT    */
   Widget   text;        /* Text widget in found JOT    */

   /* switching screens is an edit */
   edited = TRUE;

   /* Figure which jot is being moved */
   parent = XtParent(XtParent(XtParent(XtParent(w))));
   form   = XtNameToWidget(parent, "*form");
   text   = XtNameToWidget(parent, "*Text");

   /* If we have two screens, begin switch */
   if (toplevel != toplevel2) {
      Position x,y;               /* Jot's position on screen */
      Dimension height, width;    /* Jot's dimensions         */
      Boolean hidden = FALSE;     /* Not used                 */
      Boolean open;               /* Open state               */
      int     screen;             /* New Screen               */
      char    *data;              /* Text data                */

      /* Get jot data */
      XtVaGetValues(form, XmNuserData, &jot_data, NULL);

      /* Get position and dimension */
      XtVaGetValues(parent,  XmNx,        &x, 
                             XmNy,        &y,
                             XmNheight,   &height, 
                             XmNwidth,    &width,
                             NULL);
      /* Get from jot data if it was open because the
       *  height and width would be incorrect otherwise 
       */
      if (!jot_data->open) {
         height = jot_data->height;
         width  = jot_data->width;
      }
      XtVaGetValues(text, XmNvalue, &data, NULL);
      open = jot_data->open;
      screen = !XDefaultScreen(XtDisplay(parent));

      /* Create new jot on other screen and delete old jot */
      CreateNewNote(x,y,height,width,open,screen,hidden,jot_data->colorname,
                    data);
      DeleteHotJot(w, FALSE);
   }
}


/**************************************************************
*                      UpJotUp                                *
* Function:                                                   *
*    Raises all Jot windows to foreground.                    *
***************************************************************
* Parameters:
*    None.
*/
void UpJotUp(void) {
   XRaiseWindow(XtDisplay(toplevel), XtWindow(toplevel));

   /* If 2 screens, do both */
   if (toplevel != toplevel2)
      XRaiseWindow(XtDisplay(toplevel2), XtWindow(toplevel2));
}


/**************************************************************
*                      VerifyAction                           *
* Function:                                                   *
*    Builds verify dialog and loops until user response is    *
*    given.  Note re-entrant, but thats okay.                 *
***************************************************************
* Parameters:
*    title     in:   Dialog title.
*    message   in:   Message to display.
*    warning   in:   Type of dialog box (TRUE if warning).
*/
Boolean VerifyAction(char *title, char *message, Boolean warning) {
   static   Widget question_dialog = NULL;     /* Question dialog               */
   static   int    done;                       /* Loop controlling variable     */
   Arg      args[6];                           /* Widget resource args          */
   Atom     wm_delete_window;                  /* Make close do a NO action     */
   Cardinal n;                                 /* Count of resource changes     */
   XmString new_title;                         /* Replacement title XmString    */
   XmString new_message;                       /* Replacement message XmString  */
   XmString yes_string;                        /* Replacement OK XmString       */
   XmString no_string;                         /* Replacement CANCEL XmString   */
   XEvent   event;                             /* Used for Blocking events      */

   /* If dialog does not exist create it */
   if (question_dialog == NULL) {
      n = 0;
      XtSetArg(args[n], XmNdefaultPosition, FALSE); n++;
      XtSetArg(args[n], XmNdefaultButtonType, XmDIALOG_CANCEL_BUTTON); n++;
      question_dialog = XmCreateWarningDialog(toplevel, "verify_action", args, n);
      XtAddCallback(question_dialog, XmNokCallback, (XtCallbackProc) VerifyActionCB, &done);
      XtAddCallback(question_dialog, XmNcancelCallback, (XtCallbackProc) VerifyActionCB, &done);
      XtAddCallback(question_dialog, XmNmapCallback, (XtCallbackProc) MapDialogCB, NULL);
      wm_delete_window = XmInternAtom(XtDisplay(question_dialog), "WM_DELETE_WINDOW", FALSE);
      XmAddWMProtocolCallback(XtParent(question_dialog), wm_delete_window, 
                              (XtCallbackProc) VerifyActionCB, &done);
   }

   /* Modify to either YES,NO or OK,CANCEL */
   n = 0;
   if (warning) {
      yes_string = XmStringCreateSimple("OK");
      no_string = XmStringCreateSimple("Cancel");
      XtSetArg(args[n], XmNokLabelString, yes_string); n++;
      XtSetArg(args[n], XmNcancelLabelString, no_string); n++;
      XtSetArg(args[n], XmNdialogType, XmDIALOG_WARNING); n++;
   } else {
      yes_string = XmStringCreateSimple("Yes");
      no_string  = XmStringCreateSimple("No");
      XtSetArg(args[n], XmNokLabelString, yes_string); n++;
      XtSetArg(args[n], XmNcancelLabelString, no_string); n++;
      XtSetArg(args[n], XmNdialogType, XmDIALOG_QUESTION); n++;
   }

   /* Update message and title */
   new_title = XmStringCreateSimple(title);
   new_message = XmStringCreateLtoR(message, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[n], XmNdialogTitle, new_title); n++;
   XtSetArg(args[n], XmNmessageString, new_message); n++;
   XtSetValues(question_dialog, args, n);
   XmStringFree(new_title);
   XmStringFree(new_message);
   XmStringFree(yes_string);
   XmStringFree(no_string);

   /* Bring up dialog */
   XtManageChild(question_dialog);   
   done = 0;

   while (!done) {
      XtAppNextEvent(app_context, &event);
      XtDispatchEvent(&event);
   }

   if (done == 1)
      return TRUE;
   else
      return FALSE;
}


/**************************************************************
*                      VerifyActionCB                         *
* Function:                                                   *
*    Callback to accept users response to action verification *
***************************************************************
* Parameters:
*   w        in:  Verify action dialog.                       
*   done     in:  Loop control variable for verification.
*   cb       in:  Dialog callback structure. 
*/
void VerifyActionCB(Widget w, char *done, XmAnyCallbackStruct *cb) {
   if (cb->reason == XmCR_OK) {
      *done = 1;
   } else {
      *done = 2;

      if (cb->reason != XmCR_CANCEL) {
         XtUnmanageChild(w);
      }
   }
}


/**************************************************************
*                      main                                   *
***************************************************************
*/
int main(int argc, char *argv[]) {
   Display     *display;                     /* Ptr. to X Display when open    */
   int         fake_argc = 1;                /* We are not really a C program  */
   char        *fake_argv[5];                /* Fake version of argv           */
   int         num_screens;                  /* Number of screens for X Device */
   char        second_display[50];           /* Name of secondary display      */
   XGCValues   gcv;                          /* For Color GC setup             */
 
   /* Fake args for AppInitialize, so we can configure
    * for a two screen system.
    */
   fake_argv[0] = "HOTJOT";
   fake_argv[1] = "-display";
   fake_argv[2] = second_display;

   /* Ask Display how many screens it has */
   display = XOpenDisplay(NULL);
   num_screens = XScreenCount(display);
   XCloseDisplay(display);

   /* Initialize Dxm and Mrm for PrintBox */
   DXmInitialize();
   MrmInitialize();

   /* Create toplevel shell */
   toplevel = XtVaAppInitialize(&app_context, "HotJot", NULL, 0, 
                                &argc, argv, NULL, NULL);

   /* Create icon */
   icon = XCreatePixmapFromBitmapData(XtDisplay(toplevel),
            RootWindowOfScreen(XtScreen(toplevel)),
            (char *) HOTJOT_bits, HOTJOT_width, HOTJOT_height, 1, 0, 1);
   XtVaSetValues(toplevel, XmNiconPixmap, icon, NULL);
   
   /* If more than one screen, create second toplevel shell */
   if (num_screens > 1) {
      char *screen_str;
      char temp_str[5];

      if ((screen_str = getenv("SYS$REM_NODE")) == NULL)
	 if ((screen_str = getenv("SYS$NODE")) == NULL)
	    exit(0);
 
      if (screen_str != NULL) {
	 fake_argc += 2;
	 strcpy(second_display, screen_str);
	 XtFree(screen_str);

	 if (strcmp(screen_str, "FALSE") != 0) {
	    sprintf(temp_str, "0.%d", !XDefaultScreen(display));
	    strcat(second_display, temp_str);               
	 }

	 XtFree(screen_str);

      } else {
	 sprintf(temp_str, "0.%d", !XDefaultScreen(display));
	 strcat(second_display, temp_str);
      }
   }

   if (fake_argc > 1) {
      Display *second_display;

      second_display =  XtOpenDisplay(app_context, NULL, 
				      "HOTJOT2", "HOTJOT", 
				      NULL, 0, 
				      &fake_argc, fake_argv);
      toplevel2 = XtAppCreateShell("HOTJOT2", NULL, 
				       applicationShellWidgetClass, second_display, 
				       NULL, 0);
      XtVaSetValues(toplevel2, XmNiconPixmap, icon, NULL);
      XtRealizeWidget(toplevel2);
   } else {
      toplevel2 = toplevel;
   }

   XtRealizeWidget(toplevel);

   /* Place little window either off screen or in upper left hand corner */
   XtVaSetValues(toplevel, XmNmwmDecorations, 2, XmNx, -100, XmNy, -100, 
                 XmNdeleteResponse, XmDO_NOTHING,
                 NULL);
   XtVaSetValues(toplevel2, XmNmwmDecorations, 2, XmNx, -100, XmNy, -100, 
                 XmNdeleteResponse, XmDO_NOTHING,
                 NULL);

   /* Load in HOT JOT file */
   LoadHotJot();

   /* Create a graphics context */
   gcv.function   = GXxor;  
   gc = XCreateGC(XtDisplay(toplevel), 
                  DefaultRootWindow(XtDisplay(toplevel)), GCFunction, &gcv); 

   /* Start autosave loop */
   AutoSaveEH(toplevel,0);

   XtAppMainLoop(app_context);
}
