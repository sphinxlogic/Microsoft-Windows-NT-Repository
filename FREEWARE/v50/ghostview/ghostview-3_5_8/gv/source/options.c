/*
**
** options.c
**
** Copyright (C) 1995, 1996, 1997 Johannes Plass
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
** 
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
*/

/*
#define MESSAGES
*/
#include "message.h"

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "paths.h"
#include INC_X11(Intrinsic.h)
#include INC_X11(StringDefs.h)
#include INC_X11(Shell.h)
#include INC_XAW(Label.h)
#include INC_XAW(SimpleMenu.h)
#include INC_XAW(SmeBSB.h)
#include "Aaa.h"
#include "MButton.h"

#include "types.h"
#include "d_memdebug.h"
#include "main_resources.h"
#include "main_globals.h"
#include "file.h"
#include "note.h"
#include "options.h"
#include "popup.h"
#include "widgets_misc.h"

#ifdef VMS
#   define unlink remove
#else
#   include <unistd.h>
#endif

#define MESSAGE_STRING_LEN 512
#define MAX_RECORD_LENGTH  512

/*######################################################
   options_cb_popup
#######################################################*/

void options_cb_popup(w, client_data, call_data)
  Widget	w;
  XtPointer	client_data, call_data;
{
  OptionPopup op = (OptionPopup) client_data;

  BEGINMESSAGE(options_cb_popup)
  if (!op->popup) (op->create)();
  if (!op->visible) {
    (op->update)();
    popup_positionPopup(op->popup,viewFrame,POPUP_POSITION_POS,4,4);
    XtPopup(op->popup, XtGrabNone);
    op->visible=True;
  }
  ENDMESSAGE(options_cb_popup)
}      

/*######################################################
   options_cb_popdown
#######################################################*/

void options_cb_popdown(w, client_data, call_data)
  Widget	w;
  XtPointer	client_data, call_data;
{
  OptionPopup op = (OptionPopup) client_data;

  BEGINMESSAGE(options_cb_popdown)
  if (op->visible) {
    cb_popdownNotePopup((Widget)NULL,(XtPointer)NULL,NULL);
    XtPopdown(op->popup);
    op->visible=False;
  }
  ENDMESSAGE(options_cb_popdown)
}

/*######################################################
   options_cb_changeMenuLabel
#######################################################*/

void options_cb_changeMenuLabel(w, client_data, call_data)
  Widget	w;
  XtPointer	client_data, call_data;
{
  Arg      args[1];
  Cardinal n;
  char *l;
  Widget p;

  BEGINMESSAGE(options_cb_changeMenuLabel)
  p = XtParent(XtParent(w));
				      	n=0;
   XtSetArg(args[n], XtNlabel, &l);	n++;
   XtGetValues(w, args, n);
				      	n=0;
   XtSetArg(args[n], XtNlabel, l);	n++;
   XtSetValues(p, args, n);
  ENDMESSAGE(options_cb_changeMenuLabel)
}

/*######################################################
   options_textApply
#######################################################*/

void options_textApply(w,bP,sP)
  Widget w;
  Boolean *bP;
  String *sP;
{
  if (!bP || (bP && *bP==True)) GV_XtFree(*sP); 
  if (bP) *bP=True;
  *sP = widgets_getText(w);
  *sP = GV_XtNewString(*sP);
}

/*######################################################
   options_createLabeledMenu
#######################################################*/

void options_createLabeledMenu(name,parent,lP,mbP,mP)
  String name;
  Widget parent;
  Widget *lP;
  Widget *mbP;
  Widget *mP;
{
  char s[50];
  Arg args[1];
  Cardinal n;
  BEGINMESSAGE(options_createLabeledMenu)
                                        n=0;
  XtSetArg(args[n], XtNresize, False);  ++n;
  sprintf(s,"%sLabel",name);
  *lP  = XtCreateManagedWidget(s,labelWidgetClass,parent,args,n);
  sprintf(s,"%sButton",name);
  *mbP = XtCreateManagedWidget(s,mbuttonWidgetClass,parent,args,n);
  *mP  = XtCreatePopupShell("menu", simpleMenuWidgetClass,*mbP,NULL,(Cardinal)0);
  ENDMESSAGE(options_createLabeledMenu)
}

/*######################################################
   options_realize
#######################################################*/

void options_realize(popup,aaa)
  Widget popup;
  Widget aaa;
{
  Dimension	minw,minh;
  Arg args[3];
  Cardinal n;

  BEGINMESSAGE(options_realize)
  XtRealizeWidget(popup);
  AaaWidgetGetNaturalSize((AaaWidget)aaa,&minw,&minh);
  IIMESSAGE(minw,minh)
                                         n=0;
  XtSetArg(args[n], XtNminWidth, minw);  ++n;
  XtSetArg(args[n], XtNminHeight, minh); ++n;
  XtSetArg(args[n], XtNmaxWidth, XtUnspecifiedShellInt);  ++n;
  XtSetValues(popup, args, n);
  XSetWMProtocols(XtDisplay(popup),XtWindow(popup),&wm_delete_window,1);
  ENDMESSAGE(options_realize)
}

/*######################################################
   options_setArg
#######################################################*/

void options_setArg(argiP,argvP,format,name,value)
  String *argiP;
  String *argvP;
  String format;
  String name;
  String value;
{
  *argiP = XtMalloc((strlen(format)+strlen(name)+3)*sizeof(char));
  strcpy(*argiP,name);
  if (*format != '*') strcat(*argiP,".");
  strcat(*argiP,format);
  strcat(*argiP,":");
  *argvP = XtMalloc((strlen(value)+1)*sizeof(char));
  strcpy(*argvP,value);
}

/*######################################################
   options_squeezeMultiline
#######################################################*/

String  options_squeezeMultiline(s)
  String s;
{
  char *b,*e,*d,*md;

  BEGINMESSAGE(options_squeezeMultiline)
  if (!s) s = "";
  md = s = d = GV_XtNewString(s);
  while (*d) {
    while (isspace(*d)) d++;
    b=e=d;
    while (*d && *d != '\n') if (!isspace(*d)) e=d++; else d++;
    while (b!=e) *s++ = *b++; if (e!=d) *s++ = *e;
    if (*d) *s++=*d++;
  }
  *s='\0';
  d = md +strlen(md);
  if (d != md) while (isspace(*(--d))) *d = '\0';
  ENDMESSAGE(options_squeezeMultiline)
  return(md);
}

/*######################################################
   options_squeeze
#######################################################*/

String  options_squeeze(s)
  String s;
{
  String d,md;

  BEGINMESSAGE(options_squeeze)
  if (!s) s = "";
  md = s = d = GV_XtNewString(s);
  while (isspace(*d)) d++;
  while (*d) {
    if (!isspace(*d) || !isspace(*(d+1))) {
       if (isspace(*d)) *s++=' ';
       else *s++=*d;
    }
    d++;
  }
  *s='\0';
  d = md +strlen(md);
  if (d != md) while (isspace(*(--d))) *d = '\0';
  return(md);
  ENDMESSAGE(options_squeeze)
}

/*------------------------------------------------------
   options_readline
------------------------------------------------------*/

#   define END_OF_RECORD "\n"

static char* options_readline(infile,lineP)
  FILE *infile;
  String *lineP;
{
  char tmp[MAX_RECORD_LENGTH];
  char *esc;
  int  multi;

  BEGINMESSAGE(options_readline)
  while (fgets(tmp,MAX_RECORD_LENGTH,infile)) {
    multi = *lineP ? strlen(*lineP) : 0;
    *lineP = GV_XtRealloc(*lineP,(multi+strlen(tmp)+1)*sizeof(char));
    if (!multi) (*lineP)[0] = '\0';
    strcat(*lineP,tmp);
    multi=0; esc = strrchr(tmp,'\\');
    if (esc && *(esc+1) == '\n') while (esc != tmp && *esc-- == '\\') multi = !multi;
    if (!multi) break;
  }
  ENDMESSAGE(options_readline)
  return(*lineP);
}

/*------------------------------------------------------
   options_writeline
------------------------------------------------------*/

static void options_writeline(outfile,header,value)
  FILE *outfile;
  String header;
  String value;
{
  int num_nl=0;
  char *line;
  char *nl=value;
  int i,l;
  char *s;
  
  BEGINMESSAGE(options_writeline)
  while ((nl=strchr(nl,'\n'))) { nl++; num_nl++; }
  header = GV_XtNewString(header);
  s = strrchr(header,':');
  i=0;  
  if (s) {
    s++;
    i = (int)(s-header);
    while (isspace(*s)) {
      if (*s=='\t') i = 8*(i/8)+8;
      else i += 1;
      *s++='\0';
    }
  }
  if (i==strlen(header)) i=24;
  line = GV_XtMalloc((strlen(header) + strlen(value) + num_nl*3 + i + 2)*sizeof(char));
  line[0]='\0';
  strcpy(line,header);
  while (num_nl>=0) {
    if (num_nl>0) {
      nl=strchr(value,'\n');
      *nl++='\0';
    }
    l = strlen(line);
    while (l<i) {
      if (l+8<=i) {strcat(line,"\t"); l= 8*(l/8)+8;}
      else        {strcat(line," "); l+=1;}
    }
    strcat(line,value);
    if (num_nl) {
      value=nl;
      while(isspace(*value)) value++;
      if (*value=='\0') {
	strcat(line,"\n");
	num_nl=0;
      }
      else strcat(line,"\\n\\\n");
    }
    else        strcat(line,"\n");
    INFSMESSAGE(writing,line)
    fputs(line,outfile);
    num_nl--;
    value=nl;
    line[0]='\0';
  }
  GV_XtFree(line);
  GV_XtFree(header);
  ENDMESSAGE(options_writeline)
}

/*######################################################
   options_save
#######################################################*/

void options_save(argn,argi,argv)
  int  argn;
  String *argi;
  String *argv;
{
  FILE *tempfile;
  FILE *infile;
  char *tempfilename;
  char errorMessage[MESSAGE_STRING_LEN];

  BEGINMESSAGE(options_save)
  if (argn == 0) {
     INFMESSAGE(nothing to do)
     ENDMESSAGE(SaveOptionsToFile)
     return;
  }
  else --argn;

  if (!gv_user_defaults_file) {
    sprintf(errorMessage,"Save aborted: \nUndefined destination file.");
    NotePopupShowMessage(errorMessage);
    INFMESSAGE(undefined destination file)
    ENDMESSAGE(options_save)
    return;
  }
  INFSMESSAGE(trying to write to,gv_user_defaults_file)

  infile=fopen(gv_user_defaults_file,"r"); 
  tempfilename=file_getTmpFilename(gv_user_defaults_file,gv_user_defaults_file);
  INFSMESSAGE(using temporary file,tempfilename)
   
  if (!tempfilename || !(tempfile = fopen(tempfilename,"w"))) {
    sprintf(errorMessage,"Save aborted: \nCannot create temporary file");
    NotePopupShowMessage(errorMessage);
    INFMESSAGE(cannot create temporary file)
    fclose(infile);
    GV_XtFree(tempfilename);
    ENDMESSAGE(options_save)
    return;
  }

  if (infile) {
    char *tmp;
    char *line=NULL;
    int i,j;
    while (options_readline(infile,&line)) {
      SMESSAGE(line)
      i=j=0;
      while (i <= argn) {
	if (argi[i][0] != '\0' && !strncmp(line,argi[i],strlen(argi[i]))) {
	  INFSMESSAGE(found:,line)
	  tmp=strchr(line,':')+1;
	  while (isspace(*tmp) && *tmp != '\n') tmp++; *tmp = '\0';
	  options_writeline(tempfile,line,argv[i]);
          argi[i][0]='\0';
          j=i;
	  while (j<argn) {
	    tmp=argi[j]; argi[j]=argi[j+1]; argi[j+1]=tmp;
	    tmp=argv[j]; argv[j]=argv[j+1]; argv[j+1]=tmp;
	    j++;
	  }
	  j=1;
	  --argn;
	  break;
	}
	else ++i;
      }
      if (!j) fputs(line,tempfile);
      GV_XtFree(line);
      line=NULL;
    }
    fclose(infile);
  }
  if (argn>=0) fputs(END_OF_RECORD,tempfile);
  while (argn >= 0) {
    options_writeline(tempfile,argi[argn],argv[argn]);
    argn--;
  }
  fclose(tempfile);

  if (rename(tempfilename,gv_user_defaults_file)) {
    sprintf(errorMessage,"Save aborted: \nCannot rename temporary '%s'\n to '%s'",tempfilename,gv_user_defaults_file);
    NotePopupShowMessage(errorMessage);
    unlink(tempfilename);
    GV_XtFree(tempfilename);
    INFMESSAGE(cannot rename temporary file)
    ENDMESSAGE(options_save)
    return;
  }
  GV_XtFree(tempfilename);

  ENDMESSAGE(options_save)
}










