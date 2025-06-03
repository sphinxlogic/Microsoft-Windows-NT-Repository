/*
*****************************************************************************
** xbmbrowser version 2.0  (c) Copyright Ashley Roll, 1992.
** FILE: misc.c
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

#include "xbmbrowser.h"
#include "user-menu.h"
#include <pwd.h>


/*
** these two procedures handle the help window 
**
** the first is the callback for the 'Done' button the other created the 
** window and puts the stuff in it.
*/
Widget h_shell;

void Quit_Help(w,client_data,call_data )
Widget w;
XtPointer client_data,call_data;
{
  XtPopdown(h_shell);
}

void Show_Help(w,client_data,call_data )
Widget w;
XtPointer client_data,call_data;
{
Widget button,about,form;
static int created=0;

   if(created) {
     XtPopup(h_shell,XtGrabNone);
   } else {
     /* create the popup shell to put the about into */
     h_shell = XtVaCreatePopupShell("Help_Window",shellWidgetClass,bfw,NULL);

     form = XtVaCreateManagedWidget("Help_fw",formWidgetClass,h_shell,NULL);

    /* create the widget to put the about into */
    about = XtVaCreateManagedWidget("text",asciiTextWidgetClass,form,
            XtNdisplayCaret,(XtArgVal)False,
            XtNscrollVertical,(XtArgVal)XawtextScrollWhenNeeded,
            XtNscrollHorizontal,(XtArgVal)XawtextScrollWhenNeeded,
            XtNwidth,(XtArgVal)575,
            XtNheight,(XtArgVal)200,
            XtNtype,(XtArgVal)XawAsciiFile,
            XtNstring,(XtArgVal)HELPFILE,NULL);


     button =  XtVaCreateManagedWidget(
               "Done",commandWidgetClass,form,XtNlabel,(XtArgVal)"Done",
               XtNfromVert,(XtArgVal)about,NULL);
     XtAddCallback(button,"callback",Quit_Help,NULL);

     created = 1;

     XtPopup(h_shell,XtGrabNone);
     XStoreName(XtDisplay(h_shell),XtWindow(h_shell),"xbmbrowser Help");
     XSetIconName(XtDisplay(h_shell),XtWindow(h_shell),"xbmbrowser Help");
   }
}

/* 
** sort a linked list 
*/
FileList *r; /* global - pointer to the first element in the list 2 b sorted */

FileList *merge(a,b)
FileList *a,*b;
{
FileList *temp;
FileList aux;

  temp = &aux;
  while(b != NULL)
    if(a == NULL) { 
      a = b; 
      break;
    }
    else if(strcmp(b->fname,a->fname) > 0) {
      temp = temp->next = a; 
      a = a->next;
    } else {
      temp = temp->next = b;
      b = b->next;
    }

  temp->next = a;
return(aux.next);
}

FileList *sort(n)
int n;
{
FileList *fi,*la,*temp;

  if(r == NULL) return(NULL);
  else if(n > 1)
    return(merge(sort(n/2),sort((n+1)/2)));
  else {
    fi = r;
    la = r;
    /* build list as long as possible */
    for(r = r->next; r != NULL;)
      if(strcmp(r->fname,la->fname) >= 0) {
        la->next = r;
        la = r;
        r = r->next;
      }
      else if(strcmp(r->fname,fi->fname) <= 0) {
        temp = r;
        r = r->next;
        temp->next = fi;
        fi = temp;
      }
      else break;
    
  la->next = NULL;
  return(fi);
  }
}


/*
** gets all the file names from the directory 'dir'
** and puts then into a FileList linked list.
*/
FileList *get_files(dir)
char *dir;
{
DIR *dirp;
struct dirent *dp;
FileList *start=NULL,*t;
int count,i;
struct stat buf;
Pixmap b;

  dirp = opendir(dir);
  for(dp = readdir(dirp),count = 1; dp != NULL; dp = readdir(dirp),count++) {
    if(start == NULL) {
      start = (FileList *) malloc(sizeof(FileList));
      t = start; t->next = NULL;
    } else {
      t->next = (FileList *) malloc(sizeof(FileList));
      t = t->next;
      t->next = NULL;
    }
    strcpy(t->fname,dp->d_name);
    stat(t->fname,&buf);
    t->is_dir = S_ISDIR(buf.st_mode);
    if(! t->is_dir)
      t->bmOK = XReadBitmapFile(XtDisplay(toplevel),
                              DefaultRootWindow(XtDisplay(toplevel)),
                              t->fname, &i,&i,&b,&i,&i);
    else
      t->bmOK = BitmapFileInvalid;

    t->bitmap = b;
  }
  closedir(dirp);
  r = start;
return(sort(count));
}

