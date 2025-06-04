/* Crufty code for running XMU and XAW from MIT X11R5
   in DECWINDOWS MOTIF 1.0 under VMS 5.4-3
   26-APR-92 GJC@MITECH.COM

   Some of the hacks in this code may or may not be reasonable.

 */

/* modified by Johannes Plass (plass@dipmza.physik.uni-mainz.de) in 01/94 */


#include <string.h>

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/Xos.h>
#include <X11/Object.h>
#include <X11/ObjectP.h>
#include <X11/Xlib.h> /* for XlibSpecificationRelease ###jp### 17/10/94*/

#if (XlibSpecificationRelease<5)

/* this is used by XAW. And the following
   definitions *might* work. Or will we have bad malloc/free problems?
*/

XrmQuark 
XrmPermStringToQuark(char *name)
{
   return(XrmStringToQuark(name));
}

/* define the new name for this
   in terms of the old name! */

XrmDatabase 
XrmGetDatabase(Display *dpy)
{
   return(XtDatabase(dpy));
}

XrmDatabase 
XtScreenDatabase(Screen *screen)
{
   Display *dpy;
   dpy = DisplayOfScreen(screen);
   return(XtDatabase(dpy));
}

void 
XrmSetDatabase(Display *dpy,XrmDatabase db)
{
 printf("Xmu Warning: 'XrmSetDatabase' not supported\n");
}

#endif

