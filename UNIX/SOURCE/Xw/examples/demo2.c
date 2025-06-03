/********************************************************/
/* This program displays a text string and two sets     */
/* of button boxes.  One set controls the foreground    */
/* color of the image.  The other controls the back-    */
/* ground color.  The user presses a button to select   */
/* a color.                                             */
/********************************************************/
/* Include files */
#include <stdio.h>
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
void quit (w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     caddr_t call_data;
{
  exit (0);
  }


/* Callback routine for Foreground Toggles */
void Fselected (w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     caddr_t call_data;
{
  Cardinal i;
  Arg arglist[5];
  printf ("Foreground color selected.\n");
  i = 0;
  XtSetArg (arglist[i], XtNforeground, client_data);
  i++;
  XtSetValues (TargetW, arglist, i);
  }

/* Callback routine for Background Toggles */
void Bselected (w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     caddr_t call_data;
{
  Cardinal i;
  Arg arglist[5];
  printf ("Background color selected.\n");
  i = 0;
  XtSetArg (arglist[i], XtNbackground, client_data);
  i++;
  XtSetValues (TargetW, arglist, i);
  }

/* Function to return a pixel value when passed a named color */
Pixel String_to_Pixel (color_name)
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

void main (argc, argv)
     unsigned int argc;
     char **argv;
{
  Widget toplevel, PanelW, TBarW, QuitW, FormW, ColorsW, ForegroundW, BackgroundW, FTitleW,
  FRedW, FGreenW, FBlueW, BTitleW, BRedW, BGreenW, BBlueW;
  /* Callback list for QuitW push button */
  static XtCallbackRec QuitWcallbacks[] =
    {
    {quit, NULL},
    {NULL, NULL},
    };
  /* Callback lists for each foreground toggle */
  static XtCallbackRec FRedcallbacks[] =
    {
    {Fselected, NULL},
    {NULL, NULL},
    };
  static XtCallbackRec FGreencallbacks[] =
    {
    {Fselected, NULL},
    {NULL, NULL},
    };
  static XtCallbackRec FBluecallbacks[] =
    {
    {Fselected, NULL},
    {NULL, NULL},
    };
  /* Callback lists for each background toggle */
  static XtCallbackRec BRedcallbacks[] =
    {
    {Bselected, NULL},
    {NULL, NULL},
    };
  static XtCallbackRec BGreencallbacks[] =
    {
    {Bselected, NULL},
    {NULL, NULL},
    };
  
  
  static XtCallbackRec BBluecallbacks[] =
    {
    {Bselected, NULL},
    {NULL, NULL},
    };
  Arg arglist[20];
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
  FRedcallbacks[0].closure = (caddr_t) Red;
  FGreencallbacks[0].closure = (caddr_t) Green;
  FBluecallbacks[0].closure = (caddr_t) Blue;
  BRedcallbacks[0].closure = (caddr_t) Red;
  BGreencallbacks[0].closure = (caddr_t) Green;
  BBluecallbacks[0].closure = (caddr_t) Blue;
  /* Set up Panel widget */
  PanelW = XtCreateManagedWidget ("Panel", XwpanelWidgetClass,
				  toplevel, NULL, NULL);
  /* Set up Title Bar widget */
  i = 0;
  /* Argument for parent panel widget */
  XtSetArg (arglist[i], XtNwidgetType, XwTITLE);
  i++;
  /* Arguments for implicit static text widget */
  XtSetArg (arglist[i], XtNstring, "The Color Game");
  i++;
  XtSetArg (arglist[i], XtNwrap, False);
  i++;
  TBarW = XtCreateManagedWidget ("TBar", XwtitlebarWidgetClass,
				 PanelW, arglist, i);
  /* Set up Push button widget inside title bar */
  i = 0;
  /* Argument for push button widget */
  XtSetArg (arglist[i], XtNlabel, "Quit");
  i++;
  XtSetArg (arglist[i], XtNselect, QuitWcallbacks);
  i++;
  /* Argument for title bar parent */
  XtSetArg (arglist[i], XtNregion, XwALIGN_RIGHT);
  i++;
  QuitW = XtCreateManagedWidget ("Quit", XwpushButtonWidgetClass,
				 TBarW, arglist, i);
  /* Set up Forms widget */
  i = 0;
  /* Argument for panel parent */
  XtSetArg (arglist[i], XtNwidgetType, XwWORK_SPACE);
  i++;
  FormW = XtCreateManagedWidget ("Form", XwformWidgetClass,
				 PanelW, arglist, i);
  /* Set up Static Text widget */
  i = 0;
  /* Arguments for form parent */
  XtSetArg (arglist[i], XtNxRefWidget, FormW);
  i++;
  XtSetArg (arglist[i], XtNxResizable, True);
  i++;
  XtSetArg (arglist[i], XtNxAttachRight, True);
  i++;
  XtSetArg (arglist[i], XtNyRefWidget, FormW);
  i++;
  XtSetArg (arglist[i], XtNyResizable, True);
  i++;
  TargetW = XtCreateManagedWidget ("Text", XwstatictextWidgetClass,
				   FormW, arglist, i);
  /* Set up Colors Row Column widget */
  i = 0;
  /* Arguments for row column widget */
  XtSetArg (arglist[i], XtNcolumns, 2);
  i++;
  XtSetArg (arglist[i], XtNforceSize, True);
  i++;
  XtSetArg (arglist[i], XtNlayout, XwMAXIMIZE);
  i++;
  /* Arguments for form parent */
  XtSetArg (arglist[i], XtNxRefWidget, FormW);
  i++;
  XtSetArg (arglist[i], XtNxResizable, True);
  i++;
  XtSetArg (arglist[i], XtNxAttachRight, True);
  i++;
  XtSetArg (arglist[i], XtNyRefWidget, TargetW);
  i++;
  XtSetArg (arglist[i], XtNyAddHeight, True);
  i++;
  XtSetArg (arglist[i], XtNyResizable, True);
  i++;
  ColorsW = XtCreateManagedWidget ("Colors", XwrowColWidgetClass,
				   FormW, arglist, i);
  /* Set up Foreground Row Column widget */
  i = 0;
  /* Arguments for row column widget */
  XtSetArg (arglist[i], XtNmode, XwONE_OF_MANY);
  i++;
  XtSetArg (arglist[i], XtNforceSize, True);
  i++;
  ForegroundW = XtCreateManagedWidget ("Foreground", XwrowColWidgetClass,
				       ColorsW, arglist, i);
  
  
  /* Set up Background Row Column widget */
  i = 0;
  /* Arguments for row column widget */
  XtSetArg (arglist[i], XtNmode, XwONE_OF_MANY);
  i++;
  XtSetArg (arglist[i], XtNforceSize, True);
  i++;
  BackgroundW = XtCreateManagedWidget ("Background", XwrowColWidgetClass,
				       ColorsW, arglist, i);
  /* Set up Foreground Toggles */
  /* Title */
  i = 0;
  XtSetArg (arglist[i], XtNstring, "Foreground Colors");
  i++;
  XtSetArg (arglist[i], XtNalignment, XwALIGN_CENTER);
  i++;
  FTitleW = XtCreateManagedWidget ("Title", XwstatictextWidgetClass,
				   ForegroundW, arglist, i);
  /* Red */
  i = 0;
  XtSetArg (arglist[i], XtNlabel, "Red");
  i++;
  XtSetArg (arglist[i], XtNforeground, Red);
  i++;
  XtSetArg (arglist[i], XtNsquare, False);
  i++;
  XtSetArg (arglist[i], XtNselect, FRedcallbacks);
  i++;
  FRedW = XtCreateManagedWidget ("Red", XwtoggleWidgetClass,
				 ForegroundW, arglist, i);
  /* Green */
  i = 0;
  XtSetArg (arglist[i], XtNlabel, "Green");
  i++;
  XtSetArg (arglist[i], XtNforeground, Green);
  i++;
  XtSetArg (arglist[i], XtNsquare, False);
  i++;
  XtSetArg (arglist[i], XtNselect, FGreencallbacks);
  i++;
  FGreenW = XtCreateManagedWidget ("Green", XwtoggleWidgetClass,
				   ForegroundW, arglist, i);
  /* Blue */
  i = 0;
  XtSetArg (arglist[i], XtNlabel, "Blue");
  i++;
  XtSetArg (arglist[i], XtNforeground, Blue);
  i++;
  XtSetArg (arglist[i], XtNsquare, False);
  i++;
  XtSetArg (arglist[i], XtNselect, FBluecallbacks);
  i++;
  FBlueW = XtCreateManagedWidget ("Blue", XwtoggleWidgetClass,
				  ForegroundW, arglist, i);
  /* Set up Background Toggles */
  /* Title */
  i = 0;
  XtSetArg (arglist[i], XtNstring, "Background Colors");
  i++;
  XtSetArg (arglist[i], XtNalignment, XwALIGN_CENTER);
  i++;
  BTitleW = XtCreateManagedWidget ("Title", XwstatictextWidgetClass,
				   BackgroundW, arglist, i);
  /* Red */
  i = 0;
  XtSetArg (arglist[i], XtNlabel, "Red");
  i++;
  XtSetArg (arglist[i], XtNforeground, Red);
  i++;
  XtSetArg (arglist[i], XtNsquare, False);
  i++;
  XtSetArg (arglist[i], XtNselect, BRedcallbacks);
  i++;
  BRedW = XtCreateManagedWidget ("Red", XwtoggleWidgetClass,
				 BackgroundW, arglist, i);
  /* Green */
  i = 0;
  XtSetArg (arglist[i], XtNlabel, "Green");
  i++;
  XtSetArg (arglist[i], XtNforeground, Green);
  i++;
  XtSetArg (arglist[i], XtNsquare, False);
  i++;
  XtSetArg (arglist[i], XtNselect, BGreencallbacks);
  i++;
  BGreenW = XtCreateManagedWidget ("Green", XwtoggleWidgetClass,
				   BackgroundW, arglist, i);
  /* Blue */
  i = 0;
  XtSetArg (arglist[i], XtNlabel, "Blue");
  i++;
  XtSetArg (arglist[i], XtNforeground, Blue);
  i++;
  XtSetArg (arglist[i], XtNsquare, False);
  i++;
  XtSetArg (arglist[i], XtNselect, BBluecallbacks);
  i++;
  BBlueW = XtCreateManagedWidget ("Blue", XwtoggleWidgetClass,
				  BackgroundW, arglist, i);
  /* Realize widget and loop */
  XtRealizeWidget (toplevel);
  XtMainLoop ();
  }
