/* ldfile.c
   This is the callback for the load_file_button.
   Files loaded contain a sequence of cube moves that result
   in some desired pattern.
   Buvos Kocka (Hungarian Magic Cube). 
       

The following references were used:
   "The X Window System Programming And Applications with Xt
   OSF/MOTIF EDITION"
   by Douglas A Young 
   Prentice-Hall, 1990.
   ISBN 0-13-642786-3

   "Mastering Rubik's Cube"
   by Don Taylor
   An Owl Book; Holt, Rinehart and Winston, New York, 1980
   ISBN 0-03-059941-5

-------------------------------------------------------------------
Copyright (C) 1993 by Pierre A. Fleurant
Permission is granted to copy and distribute this program
without charge, provided this copyright notice is included
in the copy.
This Software is distributed on an as-is basis. There will be
ABSOLUTELY NO WARRANTY for any part of this software to work
correct. In no case will the author be liable to you for damages
caused by the usage of this software.
-------------------------------------------------------------------
*/   

#include "ldfile.h"

void (*fileProc)();

void FileNamePopUp();
void okFileNameCallback();
void cancelFileNameCallback();
void FileNameAction();
void ld_sequence_file();
void displayMessage();

extern Widget         saved_w;
extern widget_data    *saved_wdata;
extern caddr_t        saved_call_data;
extern int            number_moves;

extern void F(); /*  front  */
extern void L(); /*  left   */
extern void R(); /*  right  */
extern void B(); /*  back   */
extern void U(); /*  up     */
extern void D(); /*  down   */


void ldfile(w,wdata,call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  /* Save 'em in a global so we don't have to pass 'em 
   * around all the time.
   */
  saved_w = w;
  saved_wdata = wdata;
  saved_call_data = call_data;
  number_moves = 1;

  FileNamePopUp("Init filename", ld_sequence_file, w, wdata);
  
}

void ld_sequence_file(filename)
     char* filename;
{
  char *tptr, *t1ptr, line[MSG_SIZ], buf[MSG_SIZ], function;
  Arg args[1];
  FILE *fp;
  int i, j, parameter;


  
  if (filename[0] == '/')
    strcpy(buf, filename);
  else {
    strcpy(buf, "./");
    strcat(buf, filename);
  }
  
  if ((fp = fopen(buf, "r")) == NULL) {
    strcpy(line, buf);
    sprintf(buf,"Can't open %s", line);
    displayMessage(buf);
    return;
  }else{
    sprintf(line,"\nSequence filename: %s\n",buf);
    displayMessage(line);
  }
  
  /*
   * check if file is empty
   */
  if(!fgets(line, MSG_SIZ, fp)){
    /* NULL pointer is returned if eof */
    sprintf(buf,"file is empty");
    displayMessage(buf);
    fclose(fp);
    return;
  }
  do {
    /* skip comments */
    if(*line == '!')
      continue;
    /* use tptr because it will get incremented */
    tptr = line;

      /* parse function commands */
      while(sscanf(tptr, "%1s(%d);", &function, &parameter) == 2){
	/* execute it */
	switch(function){
	case 'F':
	  F(parameter);
	  break;
	case 'L':
	  L(parameter);
	  break;
	case 'R':
	  R(parameter);
	  break;
	case 'B':
	  B(parameter);
	  break;
	case 'U':
	  U(parameter);
	  break;
	case 'D':
	  D(parameter);
	  break;
	default:
	  sprintf(buf,"file has incorrect syntax");
	  displayMessage(buf);
	  fclose(fp);
	  return;
	}      
	/* point to next command */
	if(!(t1ptr = strchr(tptr,';')))
	  break;
	tptr = t1ptr + 1;
      }
    
  }while (fgets(line, MSG_SIZ, fp)); /* while not eof */
  
  /* close shop */
  fclose(fp);
  
}

void displayMessage(msg)
     char* msg;
{
  printf("%s\n", msg);
}

/************************************************************************/
/** The following is generic stuff for File list popUp windows         **/
/************************************************************************/

/* The following is a generic routine to popUp a file list for loading. */
void FileNamePopUp(label, proc, theParent, wdata)
     char *label;
     void (*proc)();
     Widget theParent;
     widget_data *wdata;
{
  Arg args[4];
  Widget popup, dialog;
  Position x, y;
  Dimension bw_width, pw_width;
  int n;
  
  fileProc = proc;
  
  popup = XtCreatePopupShell("ldfile_popup",
			     transientShellWidgetClass, theParent, NULL, 0);
  
  n=0;
  XtSetArg(args[n], XtNheight, DIALOG_SIZE);n++;
  XtSetArg(args[n], XtNwidth, DIALOG_SIZE);n++;
  dialog = XmCreateFileSelectionDialog(popup,label,
				       args, n);
  
  /* Unmanage the help button */
  XtUnmanageChild(XmFileSelectionBoxGetChild(dialog,
					     XmDIALOG_HELP_BUTTON));
  
  XtManageChild(dialog);
  
  XtAddCallback(dialog, XmNokCallback, okFileNameCallback, (XtPointer) dialog);
  XtAddCallback(dialog, XmNcancelCallback, cancelFileNameCallback, (XtPointer) dialog);
  
  XtRealizeWidget(dialog);
  
  XtSetKeyboardFocus(dialog, popup);
}


void okFileNameCallback(w, client_data, call_data)
     Widget w;
     XtPointer client_data, call_data;
{
  FileNameAction(w, NULL);
}

void cancelFileNameCallback(w, client_data, call_data)
     Widget w;
     XtPointer client_data, call_data;
{
  XtDestroyWidget(w);
}

void FileNameAction(w, event)
     Widget w;
     XEvent *event;
{
  char buf[MSG_SIZ];
  String name;
  int n;
  XmString compoundString;
  Arg args[1];
  XtCallbackProc callbackFunct;
  
  n=0;
  XtSetArg(args[n], XmNtextString, &compoundString);n++;
  XtGetValues(w, args, n);
  name = xs_get_string_from_xmstring(compoundString);
  XtDestroyWidget(w);
  
  if ((name != NULL) && (*name != '\0')) {
    strcpy(buf, name);
    (*fileProc)(buf, NULL, NULL);	 /* I can't see a way not to use a global here [from xboard stuff] */
    return;
  }
  
}
