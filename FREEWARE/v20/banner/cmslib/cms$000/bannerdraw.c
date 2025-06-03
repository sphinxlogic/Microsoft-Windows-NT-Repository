 #module BannerDraw "V1.0"
 
 /*
 **++
 **  FACILITY:
 **
 **      The DECwindows Banner program
 **
 **  ABSTRACT:
 **
 **      This module contains all the draw functions used by the banner program
 **  to draw generic objects to the banner windows.
 **
 **  AUTHORS:
 **
 **      Jim Saunders
 **
 **
 **  CREATION DATE:     27th September 1989
 **
 **  MODIFICATION HISTORY:
 **--
 **/
 /*
 **
 **  INCLUDE FILES
 **
 **/
 
*9D
 #include <stdio.h>
 #include <decw$include/DECwDwtWidgetProg.h>
*9E
*9I
 
 #include "stdio.h"
 #include "MrmAppl.h"
 
 
*9E
 #include "Banner"
 
 
 /*
 **
 **  MACRO DEFINITIONS
 **
 **/
 
 /* 
  * Definitions of other static items we use throughout the banner program
  * to reduce our setup times. 
  */
 
*15I
 extern	 Son$_Blk    Son;
*15E
 extern	 Bnr$_Blk    Bnr;
 extern	 Clk$_Blk    Clk;
 extern	 Mon$_Blk    Mon;
 extern	 GC	     BannerGC;
*2I
 extern	 XGCValues   GcValues;
*2E
 extern	 XEvent	     BannerEvent;
 extern	 char	     *BannerDayList[];
 
*2I
*6I
*7D
 extern   char	     BannerVmsNodeName[];
 extern   char	     BannerVmsVersion[];
 
*7E
*6E
 void BannerPaintGraphics ();
*2E
*11I
 void BannerPaintTextLines ();
*11E
 int  BannerWriteDatabase ();
 void BannerWriteText ();
 void BannerDigitalClock ();
 void BannerWriteDate ();
 void BannerWriteDate();
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
*2I
 **      BannerPaintGraphics (Display *display, Window *window,  int DisplaySize,
 **	    int Cblock, int Refresh)
 **
 **  FORMAL PARAMETERS:
 **
 **      This routine paints all the lines in a graphic window. If Refresh is 
 **  set it paints the constant info as well.
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
 void	BannerPaintGraphics(Display *display, Window *window, int DisplaySize, 
 	    Display$_Line *Cblock[], int Refresh)
 {
     int x, x1, y, y1, i;
     Display$_Line *Line, *NextLine;
 
 /*
  * First if Refresh is set, the paint the static information in the window.
  */
*7I
     Line = &Cblock[0];
*7E
 
*7D
     if (Refresh)
*7E
*7I
     if (Refresh || Line->TitleChanged)
*7E
 	{
 	int range, start, fieldlen;
 
 	Line = &Cblock[0];
 
 	y = 2;
*10D
 	fieldlen = XTextWidth(Bnr.font_ptr, "100%", 4);
*10E
*10I
 	fieldlen = XTextWidth(Bnr.font_ptr, Line->tval4, strlen(Line->tval4));
*10E
*6D
 	x = Bnr.font_width + Line->MaxNamePoints + fieldlen/2;
*6E
*6I
 	x = Bnr.font_width/2 + Line->MaxNamePoints + fieldlen/2;
*6E
 	range =  DisplaySize - x - fieldlen/2;
 
 /*
  * Write percentage marks
  */
 
*6I
*7I
 	if (Line->highltitle)
 		XSetForeground (display, BannerGC, Bnr.hwmrk);
 
*7E
 	BannerWriteText (display, window, x, y + Bnr.font_height,
*7D
 	    BannerVmsNodeName, 2, range);
*7E
*7I
 	    Line->Title, 2, range);
 
 	XSetForeground (display, BannerGC, Bnr.foreground);
 
*7E
 
*6E
 	start = x + (range/4)*0 - fieldlen/2;	
 	BannerWriteText (display, window, start, y + Bnr.font_height,
*10D
 	    "0%", 2, fieldlen); 
*10E
*10I
 	    Line->tval1, 2, fieldlen); 
*10E
 	start = x + (range/4)*1 - fieldlen/2;	
 	BannerWriteText (display, window, start, y + Bnr.font_height,
*10D
 	    "25%", 2, fieldlen); 
*10E
*10I
 	    Line->tval2, 2, fieldlen); 
*10E
*6D
 	start = x + (range/4)*2 - fieldlen/2;	
*6E
*6I
 /*	start = x + (range/4)*2 - fieldlen/2;	
*6E
 	BannerWriteText (display, window, start, y + Bnr.font_height,
*6D
 	    "50%", 2, fieldlen); 
*6E
*6I
 	    "50%", 2, fieldlen); */
*6E
 	start = x + (range/4)*3 - fieldlen/2;	
 	BannerWriteText (display, window, start, y + Bnr.font_height,
*10D
 	    "75%", 2, fieldlen); 
*10E
*10I
 	    Line->tval3, 2, fieldlen); 
*10E
 	start = x + (range/4)*4 - fieldlen/2;	
 	BannerWriteText (display, window, start, y + Bnr.font_height,
*10D
 	    "100%", 2, fieldlen); 
*10E
*10I
 	    Line->tval4, 2, fieldlen); 
*10E
 
 
 	y = Bnr.font_height + 2;
 	x = Bnr.font_width/2;
*16D
 
 /*
  * Now write statics headers
  */	
 	while (1)
*16E
*16I
 	}
     
 /*
 * Now write statics headers
 */	
     while (1)
 	{
 	int Lastline = 0;
 
 	if (Line->LastLine == 1 )
 	    Lastline = 1;
 
 
 /*
 * If this line is active then we have to paint it
 */
 	if (Line->LineActive == 1)
*16E
 	    {
*16D
 	    int Lastline = 0;
 
 	    if (Line->LastLine == 1 )
 		Lastline = 1;
 
*16E
*16I
 /*
 * Write the static info, and move our possition in the window.
 */
 	    if (Refresh || Line->NameChanged)
 		{
 		if (Line->highlname)
 		    XSetForeground (display, BannerGC, Bnr.hwmrk);
*16E
 
*16D
 /*
  * If this line is active then we have to paint it
  */
 	    if (Line->LineActive == 1)
 		{
 /*
  * Write the static info, and move our possition in the window.
  */
*16E
 		BannerWriteText (display, window, x, y + Bnr.font_height,
 		    Line->Name, 1, Line->MaxNamePoints); 
 
*16D
 		y = y + Bnr.font_height;
 /*
  * Move to the next line 
  */
*16E
*16I
 		XSetForeground (display, BannerGC, Bnr.foreground);
*16E
*3D
 		Line = (int)Line + (int)sizeof( Display$_Line);	
*3E
 		}
*3I
*16D
 	    Line = (int)Line + (int)sizeof( Display$_Line);	
*16E
*3E
*16D
 /*
  * Last line?
  */
 	    if (Lastline == 1 )
 		break;
*16E
*16I
 
 	    Line->NameChanged = 0;
 
 	    y = y + Bnr.font_height;
 /*
 * Move to the next line 
 */
 	    }
 	Line = (int)Line + (int)sizeof( Display$_Line);	
 /*
 * Last line?
 */
 	if (Lastline == 1 )
 	    break;
*16E
 /*
*16D
  * If this is a half line, skip the next as its static info will be the 
  * same as it sharing this line of the display, unless its the last then exit.
  */
 	    if (Line->HalfLine == 1 && Line->LastLine == 1)
 		break;
 	    if (Line->HalfLine == 1)
 		Line = (int)Line + (int)sizeof( Display$_Line);
*16E
*16I
 * If this is a half line, skip the next as its static info will be the 
 * same as it sharing this line of the display, unless its the last then exit.
 */
 	if (Line->HalfLine == 1 && Line->LastLine == 1)
 	    break;
 	if (Line->HalfLine == 1)
 	    Line = (int)Line + (int)sizeof( Display$_Line);
*16E
 
*16D
 	    }
*16E
*16I
 	}
*16E
 
*16D
 	}
*16E
 /*
  * Now paint the statistics
  */
     
     Line = &Cblock[0];
 
     y1 = Bnr.font_height + 2;
     x = Bnr.font_width + Line->MaxNamePoints;
     DisplaySize =  DisplaySize - x;
 
     while (1)
 	{
 	int Lastline = 0;
*3I
 	int width, hlwidth, height, clearwidth, newhwmrk, clearx;
*3E
 
 	if (Line->LastLine == 1 )
*5I
 	    {
*5E
 	    Lastline = 1;
*5D
 
 	NextLine = (int)Line + (int)sizeof( Display$_Line);	
*5E
*5I
 	    NextLine = Line;	
 	    }
 	else
 	    NextLine = (int)Line + (int)sizeof( Display$_Line);	
*5E
 /*
  * If this line is active then we have to paint it
  */
 	if (Line->LineActive == 1 )
 	    {
*3D
 	    int width, hlwidth, height, clearwidth, newhwmrk, clearx;
*3E
 
 /*
  * Calculate graphic height
  */
 
 	    if (Line->HalfLine == 1 || NextLine->HalfLine == 1)
 		height = (Bnr.font_height - 2)/2;
 	    else
 		height = Bnr.font_height - 2;
 		
 	    y = y1;
 	    if (Line->HalfLine == 1)
 		y = y1 + (Bnr.font_height - 2)/2 + 1;
 /*
  * Calculte draw widths
  */	    
 	    if (Line->MaxVal > 0)
 		width = (DisplaySize*Line->CurVal)/Line->MaxVal;
 	    else
 		width = 0;
 
 	    if (width > DisplaySize - 2)
 		width = DisplaySize - 2;
 
*3D
 /*	    if (width != Line->LastVal || Line->LastVal == 0 || Refresh)*/
*3E
*3I
*5D
 	    if (width != Line->LastVal || Line->LastVal == 0 || Refresh)
*5E
*5I
 	    if (width != Line->LastVal || width != Line->LastVal1 || Line->LastVal == 0 || Refresh)
*5E
*3E
 		{
 /*
  * Calculate the width, and clear width of the item.
  */
 
 
 		if (width > Line->Hwmrk)
 		    newhwmrk = width;
 		else
 		    newhwmrk = Line->Hwmrk;
 
 		if (width >= Line->LastVal)
 		    clearwidth = newhwmrk - width;
 		else
 		    clearwidth = newhwmrk - Line->LastVal;
 
 		if (Line->Hwmrk == 0)
*5I
 		    {
*5E
 		    clearwidth = DisplaySize - width;
*5D
 
 		if (Line->LastVal > width)
 		    clearx = Line->LastVal;
 		else
*5E
 		    clearx = width;
*5I
 		    }
 		else
 		    if (Line->LastVal > width)
 			clearx = Line->LastVal;
 		    else
 			clearx = width;
*5E
 /*
  * Now paint the new area
  */
 		XSetForeground (display, BannerGC, Bnr.foreground);
 		XSetBackground (display, BannerGC, Bnr.background);
 
 		hlwidth = (DisplaySize*Bnr.sys_highlight)/100;
 		if (width > hlwidth)
 		    {
 		    XFillRectangle (display, window,
 			BannerGC, x, 
 			y + 2, 
*3D
 			x + hlwidth,
*3E
*3I
 			hlwidth,
*3E
 			height);
 		    XSetForeground (display, BannerGC, Bnr.hwmrk);
 		    XFillRectangle (display, window,
 			BannerGC, x + hlwidth, 
 			y + 2, 
*3D
 			x + width,
*3E
*3I
*4D
 			width,
*4E
*4I
 			width - hlwidth,
*4E
*3E
 			height);
 		    XSetForeground (display, BannerGC, Bnr.foreground);
*15I
 		    if (Line->Sound != NULL)
 			BannerSound (Line->Sound, Son.Volume, Son.Time);
*15E
 		    }
 		else
 		    XFillRectangle (display, window,
 			BannerGC, x, 
 			y + 2, 
*3D
 			x + width,
*3E
*3I
 			width,
*3E
 			height);
 /*
  * Now paint last time in Hashes
  */
 		if (Line->LastVal > width)
 		    if (Line->LastVal > hlwidth)
 			{
 			int startw;
 			XSetFillStyle (display, BannerGC, FillOpaqueStippled);
 			startw = width;
 			if (width < hlwidth)
 			    {
 			    XFillRectangle (display, window,
 				BannerGC, x + startw, 
 				y + 2, 
*3D
 				x + hlwidth,
*3E
*3I
 				hlwidth - startw,
*3E
 				height);
 			    startw = hlwidth;
 			    }
 			XSetForeground (display, BannerGC, Bnr.hwmrk);
 			XFillRectangle (display, window,
 			    BannerGC, x + startw, 
 			    y + 2, 
*3D
 			    x + Line->LastVal,
*3E
*3I
 			    Line->LastVal - startw,
*3E
 			    height);
 			XSetFillStyle (display, BannerGC, FillSolid);
 			XSetForeground (display, BannerGC, Bnr.foreground);
 			}
 		    else
 			{
 			XSetFillStyle (display, BannerGC, FillOpaqueStippled);
 			XFillRectangle (display, window,
 			    BannerGC, x + width, 
 			    y + 2, 
*3D
 			    x + Line->LastVal,
*3E
*3I
 			    Line->LastVal - width,
*3E
 			    height);
 			XSetFillStyle (display, BannerGC, FillSolid);
 			}
 /*
  * Clear any of the old area left
  */
 		XSetBackground (display, BannerGC, Bnr.foreground);
 		XSetForeground (display, BannerGC, Bnr.background);
 
 		if (clearwidth > 0)
 		    XFillRectangle (display, window,
 			BannerGC, x + clearx, 
 			    y + 2, 
*3D
 			    x + clearwidth,
*3E
*3I
 			    clearwidth,
*3E
 			    height);
 
 		XSetForeground (display, BannerGC, Bnr.foreground);
 		XSetBackground (display, BannerGC, Bnr.background);
 /*
  * Put the hight water mark in if we need to.
  */
 		if (newhwmrk != Line->Hwmrk || Refresh)
 		    {
 		    GcValues.line_width = 2;
 		    XChangeGC (display, BannerGC, GCLineWidth, &GcValues);
 		    
 		    if (newhwmrk > hlwidth)
 			XSetForeground (display, BannerGC, Bnr.hwmrk);
 
 		    XDrawLine (display, window, 
 			BannerGC, 
*3D
 			x + newhwmrk + 2, y + 2, 
 			x + newhwmrk + 2, y + height + 2);		
*3E
*3I
 			x + newhwmrk + 1, y + 2, 
 			x + newhwmrk + 1, y + height + 2);		
*3E
 		    XSetForeground (display, BannerGC, Bnr.foreground);
 
 		    }
 
*3D
 		Line->Hwmrk = newhwmrk;
 		Line->LastVal = width;
*3E
*3I
 		if (!Refresh)
 		    {
 		    Line->Hwmrk = newhwmrk;
*5I
 		    Line->LastVal1 = Line->LastVal;
*5E
 		    Line->LastVal = width;
 		    }
*3E
 		}
 /*
  * Move to the next line 
  */
 	    Line = (int)Line + (int)sizeof( Display$_Line);
 /*
  * If this isnt a half line, then move down a line on the screen.
  */
 	    if (Line->HalfLine != 1)
 		y1 = y1 + Bnr.font_height;	    
 	    }
 	else
 	    Line = (int)Line + (int)sizeof( Display$_Line);
 
 	if (Lastline == 1 )
 	    break;
 	}
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
*2E
*11I
 **      BannerPaintText (Display *display, Window *window,  int DisplayWidth,
 **	    int DisplayHeight, DisplayText$Lines Lines, int Refresh)
 **
 **  FORMAL PARAMETERS:
 **
 **      This routine paints all the lines in a graphic window. If Refresh is 
 **  set it paints the constant info as well.
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
 void	BannerPaintTextLines(Display *display, Window *window, int DispWidth, 
 	    int DispHeight, TextDisplay$_Line *Lines, int Refresh)
 {
     int x, y, i;
 
 /*
  * Now write lines
  */	
 	i = y = 0;
 	x = Bnr.font_height/2;
 	while (1)
 	    {
 	    int Lastline = 0;
 
 	    if (Lines[i].LastLine == 1 )
 		Lastline = 1;
 
 
 /*
  * If this line has changed then we have to paint it
  */
 	    if (Lines[i].text_changed == 1 || Refresh)
 		{
 /*
  * Should it me hightlighted?
  */
 		if (Lines[i].highlight)
 		    XSetForeground (display, BannerGC, Bnr.hwmrk);
 /*
  * Write the static info, and move our possition in the window.
  */
 		BannerWriteText (display, window, x, y + Bnr.font_height,
 		    Lines[i].Text, 1, Lines[0].MaxNamePoints); 
 
 		if (Lines[i].highlight)
 		    XSetForeground (display, BannerGC, Bnr.hwmrk);
 
*13D
 		if (strlen (Lines[i].Text1) > 0)
 		    BannerWriteText (display, window, 
 			x + Lines[0].MaxNamePoints, 
 			y + Bnr.font_height,
 			Lines[i].Text1, 1, Lines[0].MaxTextPoints); 
*13E
*13I
 		BannerWriteText (display, window, 
 		    x + Lines[0].MaxNamePoints, 
 		    y + Bnr.font_height,
 		    Lines[i].Text1, 1, Lines[0].MaxTextPoints); 
*13E
 
 		XSetForeground (display, BannerGC, Bnr.foreground);
 		XSetBackground (display, BannerGC, Bnr.background);
*12I
 		}
*12E
 
*12D
 		y = y + Bnr.font_height;
*12E
*12I
 	    y = y + Bnr.font_height;
*12E
 /*
  * Need to mopve to a new line?
  */
*12D
 		if (y+Bnr.font_height > DispHeight)
 		    {
 		    y = 0;
 		    x = x + Lines[0].MaxNamePoints + Lines[0].MaxTextPoints;
 		    }
*12E
*12I
 	    if (y+Bnr.font_height > DispHeight)
 		{
 		y = 0;
 		x = x + Lines[0].MaxNamePoints + Lines[0].MaxTextPoints;
*12E
 		}
 /*
  * Last line?
  */
 	    if (Lastline == 1 )
 		break;
 	    i++;
 	}
 /*
  * Now clear anything else remaining on the screen
  */
 
*17I
     y = y + 2;	/* Stops us erasing the underscore in the last line */
 
*17E
     if (x < DispWidth && y < DispHeight)
 	{
 	XSetBackground (display, BannerGC, Bnr.foreground);
 	XSetForeground (display, BannerGC, Bnr.background);
 	XFillRectangle (display, window,
 	    BannerGC, x, 
 		y, 
 		DispWidth - x,
 		DispHeight - y);
 	x = x + Lines[0].MaxNamePoints + Lines[0].MaxTextPoints;
 	y = 0;
 	if (x < DispWidth)
 	    XFillRectangle (display, window,
 		BannerGC, x, 
 		    y, 
 		    DispWidth - x,
 		    DispHeight);
 	XSetBackground (display, BannerGC, Bnr.background);
 	XSetForeground (display, BannerGC, Bnr.foreground);
 	}
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
*11E
 **	BannerWriteText (display, window, xpos, ypos, str, position, fieldsize)
 **
 **      This routine writes the text string in the desired coordinates 
 **	in the specified window, using the current font.
 **
 **  FORMAL PARAMETERS:
 **
 **      display		    Current display
 **	window		    Current window
 **	xpos		    X position
 **	ypos		    Y position
 **	str		    The output string
 **	Position	    1 = left Justified
 **			    2 = center Justified
 **			    3 = right Justified
 **
 **  IMPLICIT INPUTS:
 **
 **      The Bnr block
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
 BannerWriteText(display, window, xpos, ypos, str, position, fieldlen)
 Display *display;
 Window *window;
 int xpos;
 int ypos;
 char *str;
 int position;
 int fieldlen;
 {
*5D
     int i, len, clypos, height, width;
*5E
*5I
     int i, len, clypos, height, width, forg;
*5E
     int size;
     char *str_size;
*5I
 
 
 /*
  * Remember the forground colour we were intended to write with
  */
 
     forg = BannerGC->values.foreground;
 
*5E
 /*
  * Set our font, as the current writting font, and strip the input string
  * so there are no leading spaces screwing it up.
  */
 
*5I
 
*5E
     while (*str == ' ') 
 	str++; 
 
     size = name_size (str) - 1;
 /*
  * Kill any trailing spaces as well
  */
 	while (size > 0 && str[size-1] == ' ')
 	    {
 	    str[size-1] = '\0';
 	    size--;
 	    }
 /*
  * now if the string is longer than the field length, cut the high order
  * off before we try to display it.
  */
 
     len = XTextWidth (Bnr.font_ptr, str, size); 
     while (fieldlen < len)
 	{
 	if (size == 0)
 	    break;
*14D
 	str++;
 	size--;
*14E
*14I
 	if (position == 1)
 	    {
 	    str[size-1] = '\0';
 	    size--;
 	    }
 	else
 	    {
 	    str++;
 	    size--;
 	    }
*14E
 	len = XTextWidth (Bnr.font_ptr, str, size); 
 	}
 /*
  * now find the number of Pixels we need to display the string, under the 
  * current font, and calculate the start X, Y coordinates.
  */
 
     clypos = ypos - Bnr.font_ptr->ascent; 
     height = Bnr.font_height; 
 
 /*
  * now select on the justification we need to do for the string.
  */
     switch (position)
     {
 
     case 1:	/* Left Justify */
 
 	{
 /*
  * Draw the string left justified, Draw the string, and then clear the area
  * in the field size, which is left over.
  */
 	    width = fieldlen - len; 
 	    XDrawImageString (display, window, BannerGC, 
 		    xpos, ypos, str, size);
 	    xpos = xpos + len; 
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XFillRectangle (display, window,
 		BannerGC, xpos, clypos, width, height);
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    break;
 	}
     case 2:	/* Center justify */
 
 	{
 /*
  * Draw the string center justified, clear the area infront of the string, 
  * draw the string, then clear the area behind it.
  */
 	    width = (fieldlen - len) / 2; 
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XFillRectangle (display, window,
 		BannerGC, xpos, clypos, width, height);
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XSetForeground (display, 
 		    BannerGC,
*5D
 		    Bnr.foreground);
*5E
*5I
 		    forg);
*5E
 	    xpos = xpos + width;
 	    XDrawImageString (display, window, BannerGC, 
 		    xpos, ypos, str, size);
 	    xpos = xpos + len; 
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XFillRectangle (display, window,
 		BannerGC, xpos, clypos, width, height);
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    break;
 	}
     case 3:	/* Right Justify */
 
 	{
 /*
  * draw the string right justified, clear the area infront, then draw 
  * the string.
  */
 	    width = fieldlen - len; 
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.foreground);
 	    XSetForeground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XFillRectangle (display, window,
 		BannerGC, xpos, clypos, width, height);
 	    XSetBackground (display, 
 		    BannerGC,
 		    Bnr.background);
 	    XSetForeground (display, 
 		    BannerGC,
*5D
 		    Bnr.foreground);
*5E
*5I
 		    forg);
*5E
 	    xpos = xpos + width;
 	    XDrawImageString (display, window, BannerGC, 
 		    xpos, ypos, str, size);
 	    break;
 	}
     }
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerDigitalClock (display, window )
 **
 **      This routine handles the display of the digital clock. It obtains the 
 **	current time stamp, in ASCII, and displays it at the coordinates for 
 **	the digital clock.
 **
 **  FORMAL PARAMETERS:
 **
 **      display		The current display
 **	window		The current window
 **
 **  IMPLICIT INPUTS:
 **
 **      The Bnr block, with the display coordinates
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
 BannerDigitalClock(display, window)
 Display *display;
 Window *window;
 {
 
     long timedesc[2];
     char timebuffer[] = "                               ";
 
 /*
  * Set up the Descriptor and call the VMS SYS$ASCTIM service.
  */
     timedesc[0] = 24;
     timedesc[1] = &timebuffer;
     SYS$ASCTIM (&timedesc, &timedesc, NULL, 1);
 /*
  * now make it pretty for the user
  */
     timebuffer[8] = '\0';
     if (timebuffer[0] == '0')
 	timebuffer[0] = ' ';
 /*
  * now write the time, removing the seconds if no seconds are wanted.
  */
    
     if (! Bnr.Param_Bits.seconds)
 	{
 	    timebuffer[5] = '\0';
 	    BannerWriteText (display, window, 
 		    Bnr.clock_height,
 		    (Bnr.clock_height / 2) - ((Bnr.font_height * 2) / 3),
 		    timebuffer, 2, XTextWidth(Bnr.font_ptr, "88-MMM-8888", 11));
 	}
     else
 	BannerWriteText (display, window, 
 		    Bnr.clock_height,
 		    (Bnr.clock_height / 2) - ((Bnr.font_height * 2) / 3),
 		    timebuffer, 2, XTextWidth(Bnr.font_ptr, "88-MMM-8888", 11));
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerWriteDate (display, window)
 **
 **      The routine obains the current day, and date, and writes this to the 
 **	desired window.
 **
 **  FORMAL PARAMETERS:
 **
 **      display	    Current display
 **	window	    Current window
 **
 **  IMPLICIT INPUTS:
 **
 **      Bnr block, with the positioning information.
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
 BannerWriteDate(display, window)
 Display *display;
 Window *window;
 {
     int daynumber;
     float daydiv;
     long timedesc[2];
     char timebuffer[]    = "                              ";
 
 /*
  * Call the system service to find out the current day, and conver that to the
  * day of this week.
  */
 
     LIB$DAY (&daynumber, NULL, NULL);
     daynumber = daynumber - ((daynumber / 7) * 7); 
 /*
  * Set up the Descriptor and call the VMS SYS$ASCTIM service.
  */
     
     timedesc[0] = 24;
     timedesc[1] = &timebuffer;
     SYS$ASCTIM (&timedesc, &timedesc, NULL, NULL);
 /*
  * now write out the day, and the date.
  */
     
     timebuffer[11] = '\0';
     BannerWriteText (display, window, 
 	Bnr.clock_height,
 	(Bnr.clock_height / 2) - ((Bnr.font_height * 2) / 3) + Bnr.font_height, 
 	BannerDayList[daynumber], 2, XTextWidth(Bnr.font_ptr, "88-MMM-8888", 11));
 
     BannerWriteText (display, window, 
 	Bnr.clock_height,
 	(Bnr.clock_height / 2) - ((Bnr.font_height * 2) / 3) + 
 	    (2 * Bnr.font_height), 
 	timebuffer, 2, XTextWidth(Bnr.font_ptr, "88-MMM-8888", 11));
 
     Bnr.ws_purged = 0;
 }
 
 
 /*
  *++
  *  NAME:
  *	name_size
  *
  *  FUNCTIONAL DESCRIPTION:
  *
  *      [@tbs@]
  *
  *  FORMAL PARAMETERS:
  *
  *      [@description_or_none@]
  *
  *  IMPLICIT INPUTS:
  *
  *      None.
  *
  *  IMPLICIT OUTPUTS:
  *
  *      None.
  *
  *  RETURNS or COMPLETION CODES:
  *
  *      [@description_or_none@]
  *
  *  SIDE EFFECTS:
  *
  *      None.
  *
  *--
  */
 
 
 int name_size(char *in_string)
 {
 int i = 0;
 
 /*
  * count the size
  */
 	while (*in_string++ != '\0')
 		i++;
 
 	i++;
 
 /*
  * remove any trailing spaces
  */
 
 	in_string--;
 	while (*in_string == ' ' && i>0)
 	    {
 	    *in_string = '\0';
 	    in_string--;
 	    i--;
 	    }
 return i;
 }
 
 
 
