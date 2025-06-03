/*
*****************************************************************************
** xbmbrowser version 2.0  (c) Copyright Ashley Roll, 1992.
** FILE: callbacks.c
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

extern FileList *get_files();


/*
** This function is called by the widgets and Windowmanager to quit the
** program
*/
void DoQuit(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
  while(file_list != NULL) {
    FileList *t;
    t = file_list->next;
    if(file_list->bmOK == BitmapSuccess)
       XFreePixmap(XtDisplay(toplevel), file_list->bitmap);

    free(file_list);
    file_list = t;
  }

  exit(0);
}

/*
** this function is added to the notify callback on all the 
** menuButtons so that the global 'bname' contains the most 
** reciently selected bitmap name
*/
void set_name(widget,event)
Widget widget;
XButtonEvent *event;
{
char str[255],*t;
int notFound = 1;
WidgetLst *wl;
Dimension iw,ih,w,h;
 
  /* search through the widgetList until we find the widget so we can get the 
     filename */

  wl = widgetList; 
  while(wl != NULL && notFound) {
    if(wl->w == widget) 
      notFound = 0;
    else
    wl = wl->next; 
  }
  
  if(! notFound) {
    bname = wl->fname;
    XtVaGetValues(widget,
                  XtNinternalHeight,&ih,
                  XtNinternalWidth,&iw,
                  XtNheight,&h,
                  XtNwidth,&w,
                  NULL);
    w -= 2 * iw;
    h -= 2 * ih;

    if ((t = strrchr(bname,'/')) == NULL)
      t = bname;
    else
      t++;
    sprintf(str,"%s (%dx%d)",t,(int)w,(int)h);
    XtVaSetValues(lw,XtNlabel,(XtArgVal)str,NULL);
  } else {
    XtVaSetValues(lw,XtNlabel,(XtArgVal)"Error - bad widget",NULL);
  }
}


/*
** this is the callback for the Rescan button
** it just calls setup()
*/
void rescan(w,client_data,call_data )
Widget w;
XtPointer client_data,call_data;
{
  /* set the waitCursor */
  XtVaSetValues(mainpw,XtNcursor,(XtArgVal)waitCursor,NULL);
  XFlush(XtDisplay(toplevel));

  setup();

  /* set the normal cursor */
  XtVaSetValues(mainpw,XtNcursor,(XtArgVal)normalCursor,NULL);
}

/*
** this is the callback for the directory name asciiTextWidget
*/
void change_dir(w,client_data,call_data )
Widget w;
XtPointer client_data,call_data;
{
char *text;

  /* set the waitCursor */
  XtVaSetValues(mainpw,XtNcursor,(XtArgVal)waitCursor,NULL);
  XFlush(XtDisplay(toplevel));

/* get the new directory from the widget and only continue if it is 
   not the same and is a VALID directory. */

  text = XawDialogGetValueString(atw);
  if(strcmp(dname,text) == 0) return; /* nothing to do */ 

/* check if it contains a '~' as the first char and substute the correct dir */ 
   if (*text == '~') expand_tilder(text);


/* change the current directory to the new directory */
  if(chdir(text) != 0) {
    XtVaSetValues(atw,XtNvalue,(XtArgVal)dname,NULL);
    /* set the normal cursor */
    XtVaSetValues(mainpw,XtNcursor,(XtArgVal)normalCursor,NULL);
    return;
  } 
    (void) getcwd(dname,253);
    XtVaSetValues(atw,XtNvalue,(XtArgVal)dname,NULL);

/* reset the bitmaps */
setup();

  /* set the normal cursor */
  XtVaSetValues(mainpw,XtNcursor,(XtArgVal)normalCursor,NULL);

}

/* 
 * This is the function that is used to handle the dirMenu callbacks
 * it places the new directory in the text widget and calles change_dir
 */

void dir_menu(w,client_data,call_data )
Widget w;
XtPointer client_data,call_data;
{
XawListReturnStruct *temp;

  temp = (XawListReturnStruct *) call_data;

XtVaSetValues(atw,XtNvalue,(XtArgVal)temp->string,NULL);
change_dir();
}

/* 
 * This procedure positions the DirPopup under the cursor
 */
void pos_dir(widget,event)
Widget widget;
XButtonEvent *event;
{
Position x,w;

  XtVaGetValues(dirPopup,XtNwidth,(XtArgVal)&w,NULL);

  x = event->x_root - (w/2);

  XtVaSetValues(dirPopup,XtNx,(XtArgVal)x,
                         XtNy,(XtArgVal)event->y_root,NULL);
}
