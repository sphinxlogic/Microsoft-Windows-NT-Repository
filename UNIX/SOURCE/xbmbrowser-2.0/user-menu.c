/*
*****************************************************************************
** xbmbrowser version 2.0  (c) Copyright Ashley Roll, 1992.
** FILE: user-menu.c
**
** xbmbrowser is Public Domain. However it, and all the code still belong to me.
** I do, however grant permission for you to freely copy and distribute it on 
** the condition that this and all other copyright notices remain unchanged in 
** all distributions.
**
** This software comes with NO warranty whatsoever. I therefore take no
** responsibility for any damages, losses or problems that the program may 
** cause.
*****************************************************************************
*/

#define USER_MENU_C_FILE
#include "xbmbrowser.h"
#include "user-menu.h"
#include <pwd.h>

static char trans[] =
  "<Key>Return:  Ok() \n\
   Ctrl<Key>M:   Ok() ";

/* 
 * Macro to test characters
 */
#define NOT_VALID(x) (x != '!') && (x != '\"')
#define IS_QUOTE(x) (x == '\"')
#define IS_MACRO(x) (x == 'b' || x == 'B' || x == 'u' || x == 'n' || x == 'd') 
#define IS_SPACE(x) (x == ' ' || x == '\t' )

/*
 * data structure that is given to each menu entry to return as clientdata.
 */

typedef struct _menuInfo {
  char menuName[100];
  char command[256];
  char queryLabel[100];
  unsigned rescan:1;
  unsigned useQuery:1;
  unsigned needInput:1;
  unsigned useDefText:1;
} MenuInfo;

/*
 * global pointer to hold the current MenuInfo so that
 * all the procedures know about it
 */
Widget query,DWidget;
MenuInfo *G_Info;
int dialogsMade = 0;
char dialogString[255];

void HandleMenu();
MenuInfo *ParseNextLine();
int parseCommand();
void Cancel();
void Ok();
void setup_dialog();
void executeCommand();

/*
** expand a tilder in situ
** This was sent to me by:
_______________________________________________________________________________
Chris McDonald.            _--_|\
                          /      \
                          X_.--._/
                                v
Department of Computer Science,   AARNet: chris@budgie.cs.uwa.edu.au
University of Western Australia,  FTP:    bilby.cs.uwa.edu.au,  130.95.1.11
Crawley, Western Australia, 6009. SCUD:   (31.97 +/-10% S, 115.81 +/-10% E)
PHONE:       +61 9 380 2533       FAX:    +61 9 380 1089
**
** and I have modified it slightly
*/
void expand_tilder(text)                /* expand in-situ in twiddle */
char *text;
{
    static char buf[255];
    char *s, *t, *t1;
    struct passwd *p, *getpwnam();

    s = text;
    s++;                                        /* skip leading twiddle */
    t = buf;
    while (*s && *s != '/')
        *t++ = *s++;
    *t = NULL;
    if(*buf && (p = getpwnam(buf)) == NULL)
        return;
    t1 = *buf ? p->pw_dir : (char *) getenv("HOME");
    t = buf;
    while( *t++ = *t1++ );                      /* buf <- home_dir */
    t--;
    while( *t++ = *s++ );                       /* buf += rest_of_a */
    t  = text;
    t1 = buf;
    while( *t++ = *t1++ );                      /* s   <- buf */
}

/*
** Create a new menu - and put a line under the lable
*/
Widget MakeMenu(parent,name,label)
Widget parent;
char *name,*label;
{
Widget w;


   w = XtVaCreatePopupShell(name,simpleMenuWidgetClass,parent,
                     XtNlabel,(XtArgVal)label,NULL);

   if (label != NULL)
   (void) XtVaCreateManagedWidget("line",smeLineObjectClass,w,NULL);

return(w);
}

/*
** add an smeBSBObjectClass object to a menu (widget)
** callback is the procedure that it calls or NULL
*/
Widget AddMenuItem(menu,label,callback,cbdata,sensitive)
Widget menu;
char *label;
XtCallbackProc callback;
XtPointer cbdata;
int sensitive;
{
Widget w;

  w = XtVaCreateManagedWidget(
             label,smeBSBObjectClass,menu,
             XtNlabel,(XtArgVal)label,XtNjustify,XtJustifyCenter,NULL);

  if(!sensitive)
    XtVaSetValues(w,XtNsensitive,(XtArgVal)False,NULL);

  if(callback != NULL)
  XtAddCallback( w, "callback",callback, cbdata);

return (w);
}

/*
 * create the menu from the given file, give it the name in name and 
 * make it as the child of parent 
 */
Widget Create_user_menu(parent,name,file)
Widget parent;
char *name,*file;
{
Widget Menu;
MenuInfo *CLineInfo;
FILE *fp;

  /* open the menu description file */
  if((fp=fopen(file,"r")) == NULL) {
    fprintf(stderr,"Cannot open Menu Description file: %s\n",file);
    exit(1);
  }

  /* make the menu */
  Menu = MakeMenu(parent,name,NULL);

  /* parse the one line if the file at a time , create a new MenuInfo struct 
  for each line. then fill in the information and add the menu item */
  while(CLineInfo = ParseNextLine(fp)) {
    AddMenuItem(Menu,CLineInfo->menuName,HandleMenu,(XtPointer)CLineInfo,1);
  } 

  /* close the file */
  fclose(fp);

  if(!dialogsMade) setup_dialog();

  return Menu;
}


MenuInfo *ParseNextLine(fp)
FILE *fp;
{
MenuInfo *info;
char line[256];
char *s,*d;
char menuName[100];
char command[256];
char queryLabel[100];
unsigned needInput = 0, useDefText = 0;
static int linenum = 1;


  /* read in a line at a time until find one that is not a comment or a blank 
     If the end of file comes up then return NULL */
getline:  
  fgets(line,256,fp);
  while(NOT_VALID(line[0]) && !feof(fp)) {
    fgets(line,256,fp); 
    linenum++;
  }
  if(feof(fp)) return NULL;
  
  /* get the Menu name string */
    s = line;

    /* find the first quote */
      while(! IS_QUOTE(*s) && *s != '\0') 
         s++;

    /* copy the string into menuName */
      d = menuName;
      s++;
      while(! IS_QUOTE(*s) && *s != '\0')
        *d++ = *s++;
      
      *d = '\0';

      if(*s == '\0') {
        /* line incomplete - print and error and get new line */
        fprintf(stderr,"Error in menu description at line %d\n",linenum);
        fprintf(stderr," - failed to find the menu name.\n");
        goto getline;
      }
  s++; 
  /* get the command string */
    /* find the opening quote */
      while(! IS_QUOTE(*s) && *s != '\0') 
         s++;
  
    /* copy the string into command */ 
      d = command;
      s++;
      while(! IS_QUOTE(*s) && *s != '\0') 
        *d++ = *s++; 
     
      *d = '\0';

      if(*s == '\0') {
        /* line incomplete - print and error and get new line */      
        fprintf(stderr,"Error in menu description at line %d\n",linenum);
        fprintf(stderr," - failed to find the command.\n");
        goto getline;
      }

  s++;
  /* get the query string */
    /* find the opening quote */
      while(! IS_QUOTE(*s) && *s != '\0') 
         s++;
  
    /* copy the string into queryLabel */ 
      d = queryLabel;
      s++;
      while(! IS_QUOTE(*s) && *s != '\0') 
        *d++ = *s++; 
     
      *d = '\0';

      if(*s == '\0') {
        /* line incomplete - print and error and get new line */
        fprintf(stderr,"Error in menu description at line %d\n",linenum);
        fprintf(stderr," - failed to find the query label.\n");
        goto getline;
      }

  s++;
  /* if all are correct allocate a MenuInfo struct and fill it in,
     otherwise go back and get another line */

  if(!parseCommand(command,&needInput,&useDefText)) {
    fprintf(stderr,"Error in menu description at line %d\n",linenum);
    fprintf(stderr," - bad command.\n");
    goto getline;
  } else {
    info = (MenuInfo *) malloc(sizeof(MenuInfo));
    strcpy(info->menuName,menuName);
    strcpy(info->command,command);
    strcpy(info->queryLabel,queryLabel);

    info->rescan = (line[0] != '!');
    info->useQuery = (queryLabel[0] != '\0');
    info->needInput = needInput; 
    info->useDefText = useDefText;

    /* check that if we needInput that we are useQuery */
    if(info->needInput && !info->useQuery) {
      fprintf(stderr,"Error in menu description at line %d\n",linenum);
      fprintf(stderr," - need input but no query.\n");
      free(info);
      goto getline;
    }
  }

  /* return the MenuInfo */
  return info;
}

int parseCommand(command,needInput,useDefText)
char *command;
unsigned *needInput,*useDefText;
{
char temp[256], *s, *d;
char t_expand_temp[256],*t;
int i;

  /* make a copy of the command */
  strcpy(temp,command); 
  s = temp;
  d = command;
  *needInput = *useDefText = 0;

  while(*s != '\0') {
    *d++ = *s;
    if(*s == '%') { /* is a place holder (macro) */
      if(! IS_MACRO(*(s+1))) {
        fprintf(stderr,"Error in command - bad place holder.\n");
        return 0; 
      } 
      if(*(s+1) == 'u') {
        *needInput = 1; 
        *useDefText = 1;
      } 
      if(*(s+1) == 'n') {
        *needInput = 1;
      }
    }

    if(*s == '~') { /* expand the tilder */
      /* empty the temp string */
      for(i=0;i<256;t_expand_temp[i++] = '\0');

      /* copy string into t_expand_temp */
      t = t_expand_temp;
      while((! IS_SPACE(*s)) && *s != '%' && *s != '\0')
        *t++ = *s++;

      *t = '\0';
      s--;
      expand_tilder(t_expand_temp);

      /* copy it back into *d */
      t = t_expand_temp;
      d--;
      while(*d++ = *t++);
      d--;  /* go back over the null */
    }

    s++;
  }
  *d = '\0';
  return(1);
}


/*
 * Handle the executing of commands. Called by the menu.
 */
void HandleMenu(w,client_data,call_data )
Widget w;
XtPointer client_data,call_data;
{
  Position    x, y;

  G_Info = (MenuInfo *)client_data;

  /* Check if we need the dialog box */
  if(G_Info->useQuery) {
    /* setup the dialog box */
      /* set the label */
      XtVaSetValues(DWidget,XtNlabel,(XtArgVal)G_Info->queryLabel,NULL);        
      /* check if we need the text widget */
      if(G_Info->needInput) {
        /* activate the text widget */

        /* set the bitmap name as the default string if useDefText */
        if(G_Info->useDefText) 
          XtVaSetValues(DWidget,XtNvalue,(XtArgVal)bname,NULL);
        else
          XtVaSetValues(DWidget,XtNvalue,(XtArgVal)"",NULL);

        XtVaSetValues(XtNameToWidget(DWidget,"value"),
                      XtNresizable,(XtArgVal)True,
                      NULL);

        XtOverrideTranslations(XtNameToWidget (DWidget, "value"),
              XtParseTranslationTable(trans));

      } else {
        /* deactivate the text widget */
        XtVaSetValues(DWidget,XtNvalue,(XtArgVal)NULL,NULL);
      }

      /* check the position and move the window to the same relative place */
      XtVaGetValues(toplevel,XtNx,&x,XtNy,&y,NULL);
      XtVaSetValues(query,XtNx,(XtArgVal)x+50,XtNy,(XtArgVal)y+100,NULL);

      /* popup the dialog widget */
      XtPopup(query,XtGrabExclusive);

  } else {
    /* execute the command */
    executeCommand();
  }

}


/*
** setup the dialog window
*/
void setup_dialog()
{
  Position x,y;

  XtVaGetValues(toplevel,XtNx,&x,XtNy,&y,NULL);

  query = XtVaCreatePopupShell("Query",transientShellWidgetClass,mainpw,
                       XtNx,(XtArgVal)x+50,XtNy,(XtArgVal)y+100,NULL);

  DWidget = XtVaCreateManagedWidget("GetName",dialogWidgetClass,query,
                       XtNvalue,(XtArgVal)"",
                       NULL);
  XtOverrideTranslations(XtNameToWidget (DWidget, "value"),
              XtParseTranslationTable(trans));

  XawDialogAddButton(DWidget,"Ok",Ok,NULL);
  XawDialogAddButton(DWidget,"Cancel",Cancel,NULL);

}


/*
** callback for the OK button in the rename and copy requesters
*/
void Ok(w,client_data,call_data )
Widget w;
XtPointer client_data,call_data;
{
int i;

  if(G_Info->needInput) {
    strcpy(dialogString, XawDialogGetValueString(DWidget));

    /* search for the first non-space char */
    i = 0;
    while(isspace(dialogString[i])) i++;

    if(dialogString[i] == '~') expand_tilder(&dialogString[i]);
  }

  XtPopdown(query);

  executeCommand();
}

/*
** callback for the cancel button
*/
void Cancel(w,client_data,call_data )
Widget w;
XtPointer client_data,call_data;
{
  XtPopdown(query);
}

/* 
 * Use the information in the global variables to execute the command that the
 * user enter into the menu definition
 */
void executeCommand()
{
char cmd[256],*s,*d,*t;

  /* copy the command from G_Info->command to cmd, expanding all % things. */

  s = G_Info->command;
  d = cmd;

  while(*s != '\0') {
    if(*s == '%') { /* is a place holder (macro) */
      /* expand macro */  
      s++;
      switch(*s) {
        case'b': /* Current bitmap name (no path) */
                 for(t = bname;*t != '\0';*d++ = *t++);
                 break;

        case'B': /* Current bitmap name (with full path) */
                 /* copy the directory */
                   for(t = directory;*t != '\0';*d++ = *t++);
                   *d++ = '/';
                 /* copy the bitmap name */
                   for(t = bname;*t != '\0';*d++ = *t++);
                 break;

        case'u': /* Users input string */
        case'n': /* Users input string - no default val */
                 for(t = dialogString;*t != '\0';*d++ = *t++);
                 break;

        case'd': /* Current Directory */
                 for(t = directory;*t != '\0';*d++ = *t++);
                 break;
      }

    } else /* just copy char */
    *d++ = *s;

    s++;
  }
  *d = '\0';

  /* do the command */
  system(cmd);

  /* re setup the bitmaps if the flag is set */
  if(G_Info->rescan) setup();

}
