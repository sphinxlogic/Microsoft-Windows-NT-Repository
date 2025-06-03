 #module BannerCube "V1.0"
 
 /*
 **++
 **  FACILITY:
 **
 **      The DECwindows Banner program
 **
 **  ABSTRACT:
 **
 **      This module contains all the code, to run the CPU histogram.
 **
 **  AUTHORS:
 **
 **      JIM SAUNDERS
 **
 **
 **  CREATION DATE:     20th June 1988
 **
 **  MODIFICATION HISTORY:
 **--
 **/
 
 
 /*
 **
 **  INCLUDE FILES
 **
 **/
 
*4D
 #include <stdio.h>
 #include <decw$include/DECwDwtWidgetProg.h>
*4E
*4I
 
 #include "stdio.h"
 #include "MrmAppl.h"
 
 
*4E
 #include "Banner"
 
 
 /*
 **
 **  MACRO DEFINITIONS
 **
 **/
 
 /*
  * now our runtime data structures
  */
 
 extern	 Bnr$_Blk    Bnr;
 extern	 Clk$_Blk    Clk;
 extern	 GC	     BannerGC;
 extern	 XGCValues   GcValues;
 
 extern void BannerSignalError();
 
 void BannerCubeRotationSwitch ();
 void BannerCubeSetup ();
 void BannerCubeRefresh ();
 void BannerCube ();
 
 static int cube_init = 0;
 
 static int cube_paint = 2;
 static int cube_cycle = 0;
 
 static float one = 1;
 static float minus1 = -1;
 
 static int l[24] = { 0, 1, 1, 2,
 		     2, 3, 3, 0,
 		     4, 5, 5, 6,
 		     6, 7, 7, 4,
 		     0, 4, 1, 5,
 		     2, 6, 3, 7 };
 
 static float init_x0[8] = {-0.5, 0.5, 0.5, -0.5, 
 			   -0.5, 0.5, 0.5, -0.5};
 
 static float init_y0[8] = {0.5, 0.5, -0.5, -0.5, 
 			   0.5, 0.5, -0.5, -0.5};
 
 static float init_z0[8] = {-0.5, -0.5, -0.5, -0.5,
 			    0.5, 0.5, 0.5, 0.5};
 
 static int rotation_switch_constant = 10;
 
 /* Factors, alpha=beta=theta=1.2 */
 
 static float rtc = 0.99978068;	/* cos(2*pi*theta/360) */
 static float rts = 0.02094242;	/* sin(2*pi*theta/360) */
 static float rac = 0.99978068;	/* cos(2*pi*alpha/360) */
 static float ras = 0.02094242;	/* sin(2*pi*alpha/360) */
 static float rbc = 0.99978068;	/* cos(2*pi*beta/360) */
 static float rbs = 0.02094242;	/* sin(2*pi*beta/360) */
 
 static int rotation_switch_count, rotation_switch_last;
 
 static float cube_zero, cube_minx, 
 	   cube_maxx, cube_miny, cube_maxy;
 
 static float x0[8], y0[8], z0[8];
 static float xr[8], yr[8], zr[8];
 
 static float x[8], y[8], xx[8], yy[8];
 
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerRotationSwitch()
 **
 **      This routine counts donw the rotation switch constant, which controls 
 **	the switching of the cube rotation direction.
 **
 **  FORMAL PARAMETERS:
 **
 **      none
 **
 **  IMPLICIT INPUTS:
 **
 **      none
 **
 **  IMPLICIT OUTPUTS:
 **
 **      none
 **
 **  COMPLETION CODES:
 **
 **      none
 **
 **  SIDE EFFECTS:
 **
 **      none
 **
 **--
 **/
 void	
 BannerRotationSwitch()
 {
 
     rotation_switch_count++;
     if (rotation_switch_count == rotation_switch_constant)
 	{
 	rotation_switch_count = 0;
 	if (rotation_switch_last == 0)
 	    {
 	    rts = minus1 * rts;
 	    rotation_switch_last = 1;
 	    }
         else
 	    if (rotation_switch_last == 1)
 		{
 		ras = minus1 * ras;
 		rotation_switch_last = 2;
 		}
 	    else
 		{
 		rbs = minus1 * rbs;
 		rotation_switch_last = 0;
 		}
 	}
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerCubeSetup ()
 **
 **
 **      this routine initialises all the banner cube data structures.
 **
 **  FORMAL PARAMETERS:
 **
 **      none
 **
 **  IMPLICIT INPUTS:
 **
 **      none
 **
 **  IMPLICIT OUTPUTS:
 **
 **      none
 **
 **  COMPLETION CODES:
 **
 **      none
 **
 **  SIDE EFFECTS:
 **
 **      none
 **
 **--
 **/
 void	
 BannerCubeSetup()
 {
     int i;
 
     cube_zero = Bnr.cube_width/2;
     if (Bnr.cube_width > Bnr.cube_height)
 	cube_zero = Bnr.cube_height/2;
 
     cube_minx = 0;
     cube_miny = 0;
     cube_maxx = cube_minx + Bnr.cube_width;
     cube_maxy = cube_miny + Bnr.cube_height;
 
     cube_init = 1;
     memcpy(&x0[0],&init_x0[0],sizeof(init_x0) );
     memcpy(&y0[0],&init_y0[0],sizeof(init_y0) );
     memcpy(&z0[0],&init_z0[0],sizeof(init_z0) );
     for (i=0;  i<=7;  i++)
 	{
 	xr[i] = yr[i] = zr[i] = x[i] = y[i] = xx[i] = yy[i] = 0;
 	}
 
     Bnr.ws_purged = 0;
     cube_cycle = 0;
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerCube ();
 **
 **      This rouitne actually draws and rotates the Banner cube.
 **
 **  FORMAL PARAMETERS:
 **
 **      none
 **
 **  IMPLICIT INPUTS:
 **
 **      none
 **
 **  IMPLICIT OUTPUTS:
 **
 **      none
 **
 **  COMPLETION CODES:
 **
 **      none
 **
 **  SIDE EFFECTS:
 **
 **      none
 **
 **--
 **/
 void	
 BannerCube()
 {
     int x_max, x_min, y_max, y_min;
     int i, j;
     float zmin;
 /*
  * first do the initialisation.
  */
     if (! cube_init)
 	BannerCubeSetup ();
 
 /*
  * Now calculate the new possitions of the cube
  */
 
     for (i=0;  i<=7;  i++)
 	{
 	xr[i] = x0[i];
 	yr[i] = (y0[i] * rbc) + (z0[i] * rbs);
 	zr[i] = (z0[i] * rbc) - (y0[i] * rbs);
 	}
 
     for (i=0;  i<=7;  i++)
 	{
 	x0[i] = (xr[i] * rtc) + (yr[i] * rts);
 	y0[i] = (yr[i] * rtc) - (xr[i] * rts);
 	z0[i] = zr[i];
 	}
 
     for (i=0;  i<=7;  i++)
 	{
 	xr[i] = (x0[i] * rac) + (z0[i] * ras);
 	yr[i] = y0[i];
 	zr[i] = (z0[i] * rac) - (x0[i] * ras);
 	}
 
     if (cube_cycle++ != cube_paint)
 	return;
 
     cube_cycle=0;
 
     zmin = 9999.0;
     for (i=0;  i<=7;  i++)
 	if (zr[i] < zmin)
 	    zmin = zr[i];
    for (i=0; i<=7; i++)
     {
     if (zmin == zr[i])	/* Interger check */
 	x[i] = y[i] = minus1;
     else
 	{
 	x[i] = xr[i];
 	y[i] = yr[i];
 	}
     }
 
     BannerCubeRefresh ();
 
     memcpy(&xx[0], &x[0], sizeof(xx));
     memcpy(&yy[0], &y[0], sizeof(yy));
 
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerCubeRefresh ();
 **
 **      This rouitne actually draws and rotates the Banner cube.
 **
 **  FORMAL PARAMETERS:
 **
 **      none
 **
 **  IMPLICIT INPUTS:
 **
 **      none
 **
 **  IMPLICIT OUTPUTS:
 **
 **      none
 **
 **  COMPLETION CODES:
 **
 **      none
 **
 **  SIDE EFFECTS:
 **
 **      none
 **
 **--
 **/
 void	
 BannerCubeRefresh()
 {
     int x_max, x_min, y_max, y_min;
     int i, j;
     float zmin;
 /*
  * first do the initialisation.
  */
     if (! cube_init)
 	BannerCubeSetup ();
 
*3I
     GcValues.line_width = 0;
     XChangeGC (XtDisplay(Bnr.dp_id), BannerGC, GCLineWidth, &GcValues);
 
*3E
     for (j=0;  j<=23;  j=j+2)
 	{
 	if ( xx[l[j]] > minus1 &&
 	     xx[l[j+1]] > minus1)
 	    {
 	    x_min = cube_minx + (xx[l[j]] * cube_zero) + cube_zero;
 	    y_min = cube_miny + (yy[l[j]] * cube_zero) + cube_zero;
 	    x_max = cube_minx + (xx[l[j+1]] * cube_zero) + cube_zero;
 	    y_max = cube_miny + (yy[l[j+1]] * cube_zero) + cube_zero;
*3I
 
*3E
 	    XSetForeground (XtDisplay (Bnr.cube_widget), 
 		    BannerGC,
 		    Bnr.background);
 	    XSetBackground (XtDisplay (Bnr.cube_widget), 
 		    BannerGC,
 		    Bnr.foreground);
 	    XDrawLine (XtDisplay(Bnr.cube_widget), XtWindow(Bnr.cube_widget),
 		BannerGC, x_min, y_min, x_max, y_max);
 	    }
 
 	if ( x[l[j]] > minus1 &&
 	     x[l[j+1]] > minus1)
 	    {
 	    x_min = cube_minx + (x[l[j]] * cube_zero) + cube_zero;
 	    y_min = cube_miny + (y[l[j]] * cube_zero) + cube_zero;
 	    x_max = cube_minx + (x[l[j+1]] * cube_zero) + cube_zero;
 	    y_max = cube_miny + (y[l[j+1]] * cube_zero) + cube_zero;
 	    XSetBackground (XtDisplay (Bnr.cube_widget), 
 		    BannerGC,
 		    Bnr.background);
 	    XSetForeground (XtDisplay (Bnr.cube_widget), 
 		    BannerGC,
 		    Bnr.foreground);
 	    XDrawLine (XtDisplay(Bnr.cube_widget), XtWindow(Bnr.cube_widget),
 		BannerGC, x_min, y_min, x_max, y_max);
 	    }
 	}
 
 }
