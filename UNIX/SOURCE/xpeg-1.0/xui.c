/*************************************************************************\
*   				Xpeg 1.0				  *
*									  *
* Copyright 1990, Kenneth C. Nelson			 	          *
*									  *
* Rights       : I, Ken Nelson own Xpeg.  I am donating the source	  *
*		 to the public domain with the restriction that nobody	  *
*		 sells it, or anything derived from it, for anything more *
*		 than media costs.  Xpeg uses a dynamic object library	  *
*		 that I don't own.  See the subdirectory dynobj for  	  *
*		 restrictions on its use.				  *
*								          *
*                Please feel free to modify Xpeg. See Todo for details.   *
*									  *
\*************************************************************************/

/*
 *  xui.c	- builds the X interface to Xpeg.
 *
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Viewport.h>

#include "pegbd.h"


#include "xpeg.icon"


#define DEFAULT_HELP "XPeg, a tool for the 90's."
#define ABOUT_HELP   "About Xpeg."
#define QUIT_HELP    "Quit Xpeg, no confirmation."
#define SAVE_HELP    "Save any changes in the pegboard."
#define UPDATE_HELP  "Rescan status of people in the pegboard."
#define NEW_HELP     "Add a new person to the pegboard."
#define GROUP_HELP   "Change, or edit the name of the group this pegboard tracks."


#define ABOUT_STR    \
"         Xpeg 1.0\n\
            By\n\
        Ken Nelson\n\n\
  Send me an email if use Xpeg.\n\
  ssdken@jarthur.claremont.edu\n\n\
          Enjoy!!\n\
"

#define NAME_WIDTH 150
#define DATE_WIDTH 140
#define LOC_WIDTH  300
#define XPEG_WIDTH 690

/*
 * Widgets that need (or it is nice) to have global.
 *
 */

Widget	titleBar;
Widget  groupBar;
Widget  toplevel;
Widget  helpArea;
Widget  buttonBox;
Widget  viewport;
Widget  commandPane;
Widget  groupDialogShell;
Widget  newPersonFormShell;
Widget  nameEditor;
Widget  locEditor;
Widget  groupEditor;
Widget  aboutDialogShell;

#include <stdio.h>



/*
 *					Action Routines
 *
 */


/*ARGSUSED*/
void ACTsetHelp(w,event)
  Widget   w;
  XButtonEvent event;
{
   char *widgetLabel;

   XtVaGetValues(w,"label",&widgetLabel,NULL);

   if (strcmp(widgetLabel,"quit") == 0)
   {
     XtVaSetValues(helpArea,XtNlabel,QUIT_HELP,NULL);     
   }
   else
   if (strcmp(widgetLabel,"save") == 0)
   {
     XtVaSetValues(helpArea,XtNlabel,SAVE_HELP,NULL);   
   }
   else
   if (strcmp(widgetLabel,"update") == 0)
   {
     XtVaSetValues(helpArea,XtNlabel,UPDATE_HELP,NULL);   
   }
   else
   if (strcmp(widgetLabel,"new") == 0)
   {
     XtVaSetValues(helpArea,XtNlabel,NEW_HELP,NULL);   
   }
   else
   if (strcmp(widgetLabel,"group") == 0)
   {
     XtVaSetValues(helpArea,XtNlabel,GROUP_HELP,NULL);   
   }
   else
   if (strcmp(widgetLabel,"about") == 0)
   {
     XtVaSetValues(helpArea,XtNlabel,ABOUT_HELP,NULL);   
   }
   else
   {
     XtVaSetValues(helpArea,XtNlabel,DEFAULT_HELP,NULL);   
   }

}


/*ARGSUSED*/
void ACTresetHelp(w,event)
  Widget   w;
  XButtonEvent event;
{
   XtVaSetValues(helpArea,XtNlabel,DEFAULT_HELP,NULL);
}




/*
 *					Call Back Routines
 *
 */

/*
 * Callback for About button
 *
 */
 

/*ARGSUSED*/
void CBabout(w,client_data,call_data)
 Widget w;
 XtPointer client_data;
 XtPointer call_data;
{
  Position x,y;
  Dimension width,height;

  /*
   * Locate where the invoking widget is.
   * So we can place the popup in the middle.
   *
   */
   
  XtVaGetValues(w,"width",&width,"height",&height,NULL);
  XtTranslateCoords(toplevel,
		    (Position) width/2,		/* X */
		    (Position) height/2,	/* Y */
		    &x,&y
		   );
		   
  XtVaSetValues(aboutDialogShell,
                XtNx,x,
		XtNy,y,
		NULL
	       );
   
   XtPopup(aboutDialogShell,XtGrabNonexclusive);
}


/*
 * CBaboutDismiss() - callback for the dismiss button of the about popup.
 *
 */

/*ARGUSED*/
void CBaboutDismiss(w,client_data,call_data)
  Widget	w;
  XtPointer	client_data;
  XtPointer	call_data;
{
   XtPopdown(aboutDialogShell);
}



/*
 * Callback for the Quit Button
 *
 */

/*ARGSUSED*/
void CBquit(w,client_data,call_data)
 Widget w;
 XtPointer client_data;
 XtPointer call_data;
{
  exit(0);
}


/*
 * CBupdate() - Callback for update button.
 *
 */
 
/*ARGSUSED*/
void CBupdate(w,client_data,call_data)
 Widget w;
 XtPointer client_data;
 XtPointer call_data;
{
  int personNum;
  void addXpegPerson();
  void setGroupBar();

  /*
   * Use updateXpegBoard to read in the file
   *
   */
   
  updateXpegBoard(pegfileName());

  /*
   * Now take the info read in and update what the
   * user sees.
   *
   */

  setGroupBar(groupName());
  if (numPeople() > 0)
  {
    for (personNum=firstPerson(); personNum<=lastPerson(); personNum=nextPerson(personNum))
    {
     /*
      * If there is a widget for this person then update it, else
      * this is a new person and the X pegboard needs to be updated
      *
      */
      
     if (personNameWidget(personNum) != NULL)
     {
        XtVaSetValues(personNameWidget(personNum),
		      XtNlabel,personName(personNum),
		      XtNwidth,NAME_WIDTH,
		      NULL
		     );
        XtVaSetValues(personDateWidget(personNum),
		      XtNlabel,personDate(personNum),
		      XtNwidth,DATE_WIDTH,
		      NULL
		     );
        XtVaSetValues(personLocationWidget(personNum),
		      XtNstring,personLocation(personNum),
		      XtNwidth,LOC_WIDTH,
		      NULL
		     );
		     
       }
       else
       {
          addXpegPerson(personNum,
		        personName(personNum),
		        personDate(personNum),
		        personLocation(personNum)
		       );
       }
    }
  }
  
}



/*
 * CBsave() - callback for the save button.
 *
 */

 
/*ARGSUSED*/
void CBsave(w,client_data,call_data)
 Widget w;
 XtPointer client_data;
 XtPointer call_data;
{
  int personNum;
  char *location;
  char *date;
  
  /*
   * update the strings in the person record, from the
   * text editors.
   *
   */

  if (numPeople() > 0)
  {
    for (personNum=firstPerson(); personNum<=lastPerson(); personNum=nextPerson(personNum))
    {
      if (personLocationWidget(personNum) != NULL)
      {
        XtVaGetValues(personLocationWidget(personNum),
	  	      "string",&location,
		      NULL
		     );

	/*
	 * if the location has changed update the time stamp in the pegboard and
	 * on the screen. Also update the new location in the pegfile.
	 */
	 
        if (strcmp(location,personLocation(personNum)) != 0)
        {
	  date = dateString(time(0));
 	
	  XtVaSetValues(personDateWidget(personNum),
			XtNlabel,date,
			XtNwidth,DATE_WIDTH,
			NULL
		       );

	  setPersonDate(personNum,date);
	  setPersonLocation(personNum,location);
        }
      }
    }
  }  

  saveXpegBoard(pegfileName());

}


/*
 * CBnew() - callback for the new button.
 *	     Pops up the new person form.
 *
 */

 
/*ARGSUSED*/
void CBnew(w,client_data,call_data)
 Widget w;
 XtPointer client_data;
 XtPointer call_data;
{

  Position x,y;
  Dimension width,height;

  /*
   * Locate where the invoking widget is.
   * So we can place the popup in the middle.
   *
   */
   
  XtVaGetValues(w,"width",&width,"height",&height,NULL);
  XtTranslateCoords(toplevel,
		    (Position) width/2,		/* X */
		    (Position) height/2,	/* Y */
		    &x,&y
		   );
		   
  XtVaSetValues(newPersonFormShell,
                XtNx,x,
		XtNy,y,
		NULL
	       );
   
   XtPopup(newPersonFormShell,XtGrabNonexclusive);

}



/*
 * CBgroup() - callback for the group button.
 *
 */

 
/*ARGSUSED*/
void CBgroup(w,client_data,call_data)
 Widget w;
 XtPointer client_data;
 XtPointer call_data;
{
  Position x,y;
  Dimension width,height;

  XtVaGetValues(w,"width",&width,"height",&height,NULL);
  XtTranslateCoords(toplevel,
		    (Position) width/2,		/* X */
		    (Position) height/2,	/* Y */
		    &x,&y
		   );
		   
  XtVaSetValues(groupDialogShell,
                XtNx,x,
		XtNy,y,
		NULL
	       );

   
   XtPopup(groupDialogShell,XtGrabNonexclusive);
}


/*
 * CBgroupOk() - callback for the change group dialog OK button.
 *
 */

 
/*ARGSUSED*/
void CBgroupOk(w,dialog,call_data)
 Widget w;
 Widget dialog;		/* The dialog this button was in. */
 XtPointer call_data;
{
  char *group;
  void setGroupBar();
  
  XtVaGetValues(groupEditor,"string",&group,NULL);
  setGroupBar(group);
  
  /*
   *  Set the group name to the one in the title bar
   */  

  setGroupName(group);
  
  XtPopdown(groupDialogShell);
  
}


/*
 * CBnewPersonAdd() - callback for the change group dialog OK button.
 *
 */

 
/*ARGSUSED*/
void CBnewPersonAdd(w,client_data,call_data)
 Widget w;
 XtPointer client_data;
 XtPointer call_data;
{
   char *name,*location;
   void addXpegPerson();
   char *date;
   
   XtPopdown(newPersonFormShell);

   XtVaGetValues(nameEditor,"string",&name,NULL);
   XtVaGetValues(locEditor,"string",&location,NULL);
   date = dateString(time(0));
   addPerson(name,date,location,TRUE);
   addXpegPerson(lastPerson(),name,date,location);
}




/*
 * CBnewPersonCancel() - callback for the add new person cancel button.
 *
 */

 
/*ARGSUSED*/
void CBnewPersonCancel(w,dialog,call_data)
 Widget w;
 Widget dialog;		/* The dialog this button was in. */
 XtPointer call_data;
{  
   XtPopdown(newPersonFormShell);
}


void setTitleBar(title)
  char *title;
{
  XtVaSetValues(titleBar,XtNlabel,title,NULL);
}


Widget addTitleBar(parent,title)
 Widget parent;
 char	*title;
{
   return XtVaCreateManagedWidget(
				   "titlebar",
				   labelWidgetClass,
				   parent,
				   XtNlabel,title,
				   NULL
				 );
}


Widget addGroupBar(parent,group)
  Widget parent;
  char   *group;
{
  return XtVaCreateManagedWidget(
				 "groupbar",
				 labelWidgetClass,
				 parent,
				 XtNlabel,group,
				 NULL
				);
}

void setGroupBar(group)
  char  *group;
{
  XtVaSetValues(groupBar,XtNlabel,group,NULL);
}



char *groupBarStr()
{
  char *group;

  XtVaGetValues(groupBar,"string",&group,NULL);
  return group;
  
}


/*
 * buildGroupDialog() - Builds the group dialog and returns the shell that holds it.
 *
 */

Widget buildGroupDialog(parent)
  Widget  parent;
{
  Widget  popupShell;
  Widget  dialog,button,label;


  popupShell = XtCreatePopupShell(
				   "groupShell",
				   transientShellWidgetClass,
				   parent,
				   NULL,
				   0
				 );

  dialog     = XtVaCreateManagedWidget(
				       "groupDialog",
				       formWidgetClass,
				       popupShell,
				       NULL
				      );


  label      = XtVaCreateManagedWidget(
				       "groupDialogLabel",
				       labelWidgetClass,
				       dialog,
				       XtNborderWidth,0,
				       XtNlabel,"Group Description: ",
				       NULL
				      );
				      
  groupEditor = XtVaCreateManagedWidget(
					"groupEditor",
					asciiTextWidgetClass,
					dialog,
					XtNfromHoriz,label,
					XtNstring,groupName(),
				        XtNwidth,400,
					XtNeditType,XawtextEdit,
					NULL
				       );
  
				      
  button     = XtVaCreateManagedWidget(
				       "groupDialogButton",
				       commandWidgetClass,
				       dialog,
				       XtNlabel,"OK",
				       XtNfromVert,label,
				       NULL
				      );

  XtAddCallback(button,XtNcallback,CBgroupOk,dialog);
  return popupShell;
}




/*
 * buildAboutDialog() - Builds the about dialog and returns the shell that holds it.
 *
 */

Widget buildAboutDialog(parent)
  Widget  parent;
{
  Widget  popupShell;
  Widget  dialog,button,label;


  popupShell = XtCreatePopupShell(
				   "About",
				   transientShellWidgetClass,
				   parent,
				   NULL,
				   0
				 );

  dialog     = XtVaCreateManagedWidget(
				       "aboutForm",
				       formWidgetClass,
				       popupShell,
				       NULL
				      );


  label      = XtVaCreateManagedWidget(
				       "groupDialogLabel",
				       labelWidgetClass,
				       dialog,
				       XtNborderWidth,0,
				       XtNjustify,XtJustifyCenter,
				       XtNlabel,ABOUT_STR,
				       NULL
				      );
				      
				      
  button     = XtVaCreateManagedWidget(
				       "aboutDismissButton",
				       commandWidgetClass,
				       dialog,
				       XtNlabel,"Dismiss",
				       XtNfromVert,label,
				       NULL
				      );

  XtAddCallback(button,XtNcallback,CBaboutDismiss,dialog);

  return popupShell;
}



Widget buildNewPersonForm(parent)
 Widget parent;
{

  Widget popupShell;
  Widget form;
  Widget addButton,cancelButton;
  Widget nameLabel,locLabel;


    popupShell = XtCreatePopupShell(
				   "NewPerson",
				   transientShellWidgetClass,
				   parent,
				   NULL,
				   0
				 );

    form = XtVaCreateManagedWidget(
				   "newPersonForm",
				   formWidgetClass,
				   popupShell,
				   NULL
				  );

    nameLabel	= XtVaCreateManagedWidget(
					   "newPersonLabel",
					   labelWidgetClass,
					   form,
					   XtNlabel,"Name :",					   
					   XtNborderWidth,0,
					   NULL
					 );
					 
    nameEditor	= XtVaCreateManagedWidget(
					  "nameEditor",
					  asciiTextWidgetClass,
					  form,
					  XtNwidth,300,
					  XtNeditType,XawtextEdit,
					  XtNfromHoriz,nameLabel,
					  NULL
					 );

    locLabel	= XtVaCreateManagedWidget(
					   "newPersonLabel",
					   labelWidgetClass,
					   form,
					   XtNlabel,"Enter Location: ",
					   XtNfromVert,nameLabel,
					   XtNborderWidth,0,
					   NULL
					 );
					 

    locEditor	= XtVaCreateManagedWidget(
					  "locationEditor",
					  asciiTextWidgetClass,
					  form,
					  XtNwidth,300,
					  XtNeditType,XawtextEdit,
					  XtNfromVert,nameEditor,
					   XtNfromHoriz,locLabel,
					  NULL
					 );



    cancelButton = XtVaCreateManagedWidget(
				     "newPersonCancel",
				     commandWidgetClass,
				     form,
				     XtNlabel,"cancel",
				     XtNfromVert,locLabel,
				     NULL
				    );


    XtAddCallback(
		  cancelButton,
		  XtNcallback,
		  CBnewPersonCancel,
		  NULL
		 );


    addButton = XtVaCreateManagedWidget(
				     "newPersonAdd",
				     commandWidgetClass,
				     form,
				     XtNlabel,"Add",
				     XtNfromHoriz,cancelButton,
				     XtNfromVert,locLabel,
				     NULL
				    );
    XtAddCallback(
		  addButton,
		  XtNcallback,
		  CBnewPersonAdd,
		  NULL
		 );


  return popupShell;

}



Widget addXpegButtonArea(parent)
  Widget parent;
  {
     Widget buttonbox;
     Widget button,label;
     XtTranslations  buttonTranslations;


     /*
      * Append to the translation table for thse buttons so
      * that help is updated on entry and exit.
      *
      */

     static char buttonTranslationsStr[] = "#override\n\
				     <EnterWindow> : highlight() sethelp() \n\
				     <LeaveWindow> : reset() resethelp() ";

     /*
      * These are the actions that update the help based on widget label,
      * and reset the help to the default help.
      *
      */

     static XtActionsRec buttonActions[] = {
						{"sethelp",ACTsetHelp},
						{"resethelp",ACTresetHelp}
					   };

					 
     buttonTranslations = XtParseTranslationTable(buttonTranslationsStr);


     /*
      * Setup the actions that provide the help when entering a window.
      *
      */
      

     XtAddActions(buttonActions,XtNumber(buttonActions));


     /*
      * Now create an Athena Box widget which will manage these
      * buttons.
     */
     					 
     buttonbox = XtVaCreateManagedWidget(
					  "buttonbox",
					  boxWidgetClass,
					  parent,
					  XtNwidth,200,
					  XtNheight,200,
					  NULL
					);

     /*
      * And then the buttons, which are Athena Command Widgets.
      *
      */

    button = XtVaCreateManagedWidget(
				     "about",
				     commandWidgetClass,
				     buttonbox,
				     XtNlabel,"about",
				     NULL
				    );


    XtAddCallback(button,XtNcallback,CBabout,NULL);
    XtOverrideTranslations(button,buttonTranslations);    

    button = XtVaCreateManagedWidget(
				     "quit",
				     commandWidgetClass,
				     buttonbox,
				     XtNlabel,"quit",
				     NULL
				    );

    XtAddCallback(button,XtNcallback,CBquit,NULL);
    XtOverrideTranslations(button,buttonTranslations);    

    

    button = XtVaCreateManagedWidget(
				     "update",
				     commandWidgetClass,
				     buttonbox,
				     XtNlabel,"update",
				     NULL
				    );
    XtAddCallback(button,XtNcallback,CBupdate,NULL);
    XtOverrideTranslations(button,buttonTranslations);    

   
    button = XtVaCreateManagedWidget(
  				     "save",
				     commandWidgetClass,
				     buttonbox,
				     XtNlabel,"save",
				     NULL
				    );
    XtAddCallback(button,XtNcallback,CBsave,NULL);
    XtOverrideTranslations(button,buttonTranslations);    

    button = XtVaCreateManagedWidget(
  				     "new",
				     commandWidgetClass,
				     buttonbox,
				     XtNlabel,"new",
				     NULL
				    );
    XtAddCallback(button,XtNcallback,CBnew,NULL);
    XtOverrideTranslations(button,buttonTranslations);    


    button = XtVaCreateManagedWidget(
  				     "group",
				     commandWidgetClass,
				     buttonbox,
				     XtNlabel,"group",
				     NULL
				    );
    XtAddCallback(button,XtNcallback,CBgroup,NULL);
    XtOverrideTranslations(button,buttonTranslations);


   label = XtVaCreateManagedWidget(
				    "yourusername",
				    labelWidgetClass,
				    buttonbox,
				    XtNlabel,"Your Username: ",
				    XtNborderWidth,0,
				    NULL
				  );

   label = XtVaCreateManagedWidget(
				    "username",
				    labelWidgetClass,
				    buttonbox,
				    XtNlabel,userName(),
				    XtNborderWidth,0,
				    NULL
				  );

    return buttonbox;
}



void addXpegPerson(personNumber,name,date,loc)
  int  personNumber;
  char *name;
  char *date;
  char *loc;
{
  Widget box;
  Widget wname,wdate,wloc;
  char   text[256];
  Person *person;

  
  box = XtVaCreateManagedWidget(
				tmpnam(text),
				boxWidgetClass,
				commandPane,
				NULL				
			       );

  XawPanedAllowResize(box,FALSE);

  wname = XtVaCreateManagedWidget(
				   tmpnam(text),
				   labelWidgetClass,
				   box,
				   XtNlabel,name,
				   XtNwidth,NAME_WIDTH,
				   NULL
				 );

  wdate = XtVaCreateManagedWidget(
				   tmpnam(text),
				   labelWidgetClass,
				   box,
				   XtNlabel,date,
				   XtNwidth,DATE_WIDTH,
				   NULL
				 );

  if (personEditable(personNumber))
  {
     wloc = XtVaCreateManagedWidget(
				   tmpnam(text),
				   asciiTextWidgetClass,
				   box,
				   XtNstring,loc,
				   XtNwidth,LOC_WIDTH,
				   XtNeditType,XawtextEdit,
				   NULL
				 );
  }
  else
  {
     wloc = XtVaCreateManagedWidget(
				   tmpnam(text),
				   asciiTextWidgetClass,
				   box,
				   XtNstring,loc,
				   XtNwidth,LOC_WIDTH,
				   XtNeditType,XawtextRead,
				   NULL
				 );
  }

  /*
   * Update this persons Widgets.
   *
  */

  setPersonNameWidget(personNumber,wname);
  setPersonDateWidget(personNumber,wdate);
  setPersonLocationWidget(personNumber,wloc);
  
}




/*
 * initXpegInterface() - parses the command line for xtoolkit options,
 *			 and then makes a toplevel shell.
*/

void initXpegInterface(argc,argv)
  int     *argc;
  char    **argv;  
{

  
  toplevel = XtInitialize(
			    argv[0],
			    (char *)xpeg_version(),
			    NULL,
			    0,
			    argc,
			    argv
			  );
			  
}



void addXpegIcon()
{
  Pixmap icon_pixmap;

  icon_pixmap = XCreateBitmapFromData(XtDisplay(toplevel),
				      RootWindowOfScreen(XtScreen(toplevel)),
				      xpeg_bits,
				      xpeg_width,
				      xpeg_height
				     );
  XtVaSetValues(toplevel,XtNiconPixmap,icon_pixmap,NULL);				      
}


/*
 * buildXpegInterface() - does just that, builds the X interface.
 *
 */

void buildXpegInterface()
{

  int	 personNum;


  addXpegIcon();
  
  commandPane = XtVaCreateManagedWidget("commandpane",
				  panedWidgetClass,
				  toplevel,
				  XtNwidth,XPEG_WIDTH,
				  NULL
				 );
				 
  titleBar	   = addTitleBar(commandPane,(char *)xpeg_version());
  
  groupBar	   = addGroupBar(commandPane,groupName());
  groupDialogShell = buildGroupDialog(toplevel);
  aboutDialogShell = buildAboutDialog(toplevel);
  newPersonFormShell = buildNewPersonForm(toplevel);
  
  
  helpArea	   = XtVaCreateManagedWidget(
					     "help",
					     labelWidgetClass,
					     commandPane,
					     XtNlabel,DEFAULT_HELP,
					     NULL
					    );


  XawPanedAllowResize(titleBar,FALSE);

  /*
   * Add the button command area.
   *
   */
   
  buttonBox = addXpegButtonArea(commandPane);
  XawPanedAllowResize(buttonBox,FALSE);


  /*
   * Now for each person read in from the pegboard file, add them to
   * the interface.
   *
  */


  if (numPeople() > 0)
  {
    for (personNum = firstPerson(); personNum <= lastPerson(); personNum = nextPerson(personNum))
    {
      addXpegPerson(personNum,
	  	    personName(personNum),
		    personDate(personNum),
		    personLocation(personNum)
		   );
    }
  }

}



/*
 * runXpegInterface()  - realizes the toplevel x widget, and then does the X event loop.
 *
 */

void runXpegInterface()
{
  XtRealizeWidget(toplevel);		/* Realize the widgets */
  XtMainLoop();
}

