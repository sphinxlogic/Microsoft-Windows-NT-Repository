/* Revised version of demo2.c using Varargs. This has the
 * same functionality as demo2.c, but is intended to be
 * readable.
 *
 * Greg Lehey, LEMIS, 30 July 1993 */
/********************************************************/
/* This program displays a text string and two sets     */
/* of button boxes.  One set controls the foreground    */
/* color of the image.  The other controls the back-    */
/* ground color.  The user presses a button to select   */
/* a color.                                             */
/********************************************************/
/* Include files */
#include <stdio.h>
#include <varargs.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <Xw/Xw.h>
#include <Xw/Panel.h>
#include <Xw/TitleBar.h>
#include <Xw/PButton.h>
#include <Xw/Form.h>
#include <Xw/SText.h>
#include <Xw/RCManager.h>
#include <Xw/Toggle.h>

Widget TargetW;						    /* Global widget to be changed by
							     * callbacks */
Display *Current_display;				    /* Global display ID */
/* Callback routine for QuitW "Quit" Push Button */
void 
quit (w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     caddr_t call_data;
{
  exit (0);
}


/* Callback routine for Foreground Toggles */
void 
Fselected (w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     caddr_t call_data;
{
  Cardinal i;
  Arg arglist [5];
  printf ("Foreground color selected.\n");
  i = 0;
  XtSetArg (arglist [i], XtNforeground, client_data);
  i++;
  XtSetValues (TargetW, arglist, i);
}

/* Callback routine for Background Toggles */
void 
Bselected (w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     caddr_t call_data;
{
  Cardinal i;
  Arg arglist [5];
  printf ("Background color selected.\n");
  i = 0;
  XtSetArg (arglist [i], XtNbackground, client_data);
  i++;
  XtSetValues (TargetW, arglist, i);
}

/* Function to return a pixel value when passed a named color */
Pixel 
String_to_Pixel (color_name)
     char *color_name;
{
  int i;
  Colormap cmap;
  XColor screen_def_return, exact_def_return;
  cmap = XDefaultColormap (Current_display,
			   DefaultScreen (Current_display));
  i = XAllocNamedColor (Current_display, cmap, color_name,
			&screen_def_return, &exact_def_return);
  return (screen_def_return.pixel);			    /* Return the closest pixel for the
							    * server */
}

void 
main (argc, argv)
     unsigned int argc;
     char **argv;
{
  Widget toplevel,
         PanelW,
         TBarW,
         QuitW,
         FormW,
         ColorsW,
         ForegroundW,
         BackgroundW,
         FTitleW,
         FRedW,
         FGreenW,
         FBlueW,
         BTitleW,
         BRedW,
         BGreenW,
         BBlueW;
  /* Callback list for QuitW push button */
  static XtCallbackRec QuitWcallbacks [] =
  {
    {quit, NULL},
    {NULL, NULL},
  };
  /* Callback lists for each foreground toggle */
  static XtCallbackRec FRedcallbacks [] =
  {
    {Fselected, NULL},
    {NULL, NULL},
  };
  static XtCallbackRec FGreencallbacks [] =
  {
    {Fselected, NULL},
    {NULL, NULL},
  };
  static XtCallbackRec FBluecallbacks [] =
  {
    {Fselected, NULL},
    {NULL, NULL},
  };
  /* Callback lists for each background toggle */
  static XtCallbackRec BRedcallbacks [] =
  {
    {Bselected, NULL},
    {NULL, NULL},
  };
  static XtCallbackRec BGreencallbacks [] =
  {
    {Bselected, NULL},
    {NULL, NULL},
  };


  static XtCallbackRec BBluecallbacks [] =
  {
    {Bselected, NULL},
    {NULL, NULL},
  };
  Cardinal i;						    /* Counter used for arglists */
  Pixel Red, Green, Blue;				    /* Pixel values for colors */
  /* Initialize toolkit */
  toplevel = XtInitialize ("main", "DEmo", NULL, NULL, &argc, argv);
  /* Get pixel values for colors */
  Current_display = XtDisplay (toplevel);		    /* Get display-ID */
  Red = String_to_Pixel ("Red");
  Green = String_to_Pixel ("Green");
  Blue = String_to_Pixel ("Blue");
  /* Set color values in callback records */
  FRedcallbacks [0].closure = (caddr_t) Red;
  FGreencallbacks [0].closure = (caddr_t) Green;
  FBluecallbacks [0].closure = (caddr_t) Blue;
  BRedcallbacks [0].closure = (caddr_t) Red;
  BGreencallbacks [0].closure = (caddr_t) Green;
  BBluecallbacks [0].closure = (caddr_t) Blue;
  /* Set up Panel widget */
  PanelW = XtCreateManagedWidget ("Panel",
				  XwpanelWidgetClass,
				  toplevel,
				  NULL,
				  NULL);
  /* Set up Title Bar widget */
  i++;
  TBarW = XtVaCreateManagedWidget ("TBar",
				   XwtitlebarWidgetClass,
				   PanelW,
				   XtNwidgetType, XwTITLE,  /* Argument for parent panel widget */
				   XtNstring, "The Color Game", /* Arguments for implicit static text widget */
				   XtNwrap, False,
				   NULL );
  
  
  /* Set up Push button widget inside title bar */
  QuitW = XtVaCreateManagedWidget ("Quit",
				   XwpushButtonWidgetClass,
				   TBarW,
				   XtNlabel, "Quit",	    /* Argument for push button widget */
				   XtNselect, QuitWcallbacks,
				   XtNregion, XwALIGN_RIGHT,
				   NULL );		    /* Argument for title bar parent */
  FormW = XtVaCreateManagedWidget ("Form",
				   XwformWidgetClass,
				   PanelW,
				   XtNwidgetType, XwWORK_SPACE, /* Argument for panel parent */
				   NULL );
  
  TargetW = XtVaCreateManagedWidget ("Text",
				     XwstatictextWidgetClass,
				     FormW,
				     XtNxRefWidget, FormW,  /* Arguments for form parent */
				     XtNxResizable, True,
				     XtNxAttachRight, True,
				     XtNyRefWidget, FormW,
				     XtNyResizable, True,
				     NULL );
  
  ColorsW = XtVaCreateManagedWidget ("Colors",
				     XwrowColWidgetClass,
				     FormW,
				     XtNcolumns, 2,	    /* Arguments for row column widget */
				     XtNforceSize, True,
				     XtNlayout, XwMAXIMIZE,
				     XtNxRefWidget, FormW,  /* Arguments for form parent */
				     XtNxResizable, True,
				     XtNxAttachRight, True,
				     XtNyRefWidget, TargetW,
				     XtNyAddHeight, True,
				     XtNyResizable, True,
				     NULL );
  
  ForegroundW = XtVaCreateManagedWidget ("Foreground",
					 XwrowColWidgetClass,
					 ColorsW,
					 XtNmode, XwONE_OF_MANY, /* Arguments for row column widget */
					 XtNforceSize, True,
					 NULL );
  
  BackgroundW = XtVaCreateManagedWidget ("Background",
					 XwrowColWidgetClass,
					 ColorsW,
					 XtNmode, XwONE_OF_MANY, /* Arguments for row column widget */
					 XtNforceSize, True,
					 NULL );
  /* Title  Set up Foreground Toggles */
  FTitleW = XtVaCreateManagedWidget ("Title",
				     XwstatictextWidgetClass,
				     ForegroundW,
				     XtNstring, "Foreground Colors",
				     XtNalignment, XwALIGN_CENTER,
				     NULL );
  FRedW = XtVaCreateManagedWidget ("Red",
				   XwtoggleWidgetClass,
				   ForegroundW,
				   XtNlabel, "Red",
				   XtNforeground, Red,
				   XtNsquare, False,
				   XtNselect, FRedcallbacks,
				   NULL );
  FGreenW = XtVaCreateManagedWidget ("Green",
				     XwtoggleWidgetClass,
				     ForegroundW,
				     XtNlabel, "Green",
				     XtNforeground, Green,
				     XtNsquare, False,
				     XtNselect, FGreencallbacks,
				     NULL );
  FBlueW = XtVaCreateManagedWidget ("Blue",
				    XwtoggleWidgetClass,
				    ForegroundW,
				    XtNlabel, "Blue",
				    XtNforeground, Blue,
				    XtNsquare, False,
				    XtNselect, FBluecallbacks,
				    NULL );
  
  /* Title  Set up Background Toggles */
  BTitleW = XtVaCreateManagedWidget ("Title",
				     XwstatictextWidgetClass,
				     BackgroundW,
				     XtNstring, "Background Colors",
				     XtNalignment, XwALIGN_CENTER,
				     NULL );
  
  BRedW = XtVaCreateManagedWidget ("Red",
				   XwtoggleWidgetClass,
				   BackgroundW,
				   XtNlabel, "Red",
				   XtNforeground, Red,
				   XtNsquare, False,
				   XtNselect, BRedcallbacks,
				   NULL );
  BGreenW = XtVaCreateManagedWidget ("Green",
				     XwtoggleWidgetClass,
				     BackgroundW,
				     XtNlabel, "Green",
				     XtNforeground, Green,
				     XtNsquare, False,
				     XtNselect, BGreencallbacks,
				     NULL );
  BBlueW = XtVaCreateManagedWidget ("Blue",
				    XwtoggleWidgetClass,
				    BackgroundW,
				    XtNlabel, "Blue",
				    XtNforeground, Blue,
				    XtNsquare, False,
				    XtNselect, BBluecallbacks,
				    NULL );
  XtRealizeWidget (toplevel);				    /* Realize widget and loop */
  XtMainLoop ();
  }


