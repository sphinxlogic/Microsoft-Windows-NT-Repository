/*
 *  Title:      find_best_window_position
 *
 *  +------------------------------------------------------------------------+
 *  | Copyright © 1991                                                       |
 *  | By Digital Equipment Corporation, Maynard, Mass.                       |
 *  | All Rights Reserved.                                                   |
 *  |                                                                        |
 *  | This software is furnished under a license and may be used and  copied |
 *  | only  in  accordance  with  the  terms  of  such  license and with the |
 *  | inclusion of the above copyright notice.  This software or  any  other |
 *  | copies  thereof may not be provided or otherwise made available to any |
 *  | other person.  No title to and ownership of  the  software  is  hereby |
 *  | transfered.                                                            |
 *  |                                                                        |
 *  | The information in this software is subject to change  without  notice |
 *  | and  should  not  be  construed  as  a commitment by Digital Equipment |
 *  | Corporation.                                                           |
 *  |                                                                        |
 *  | DIGITAL assumes no responsibility for the use or  reliability  of  its |
 *  | software on equipment which is not supplied by DIGITAL.                |
 *  +------------------------------------------------------------------------+
 *
 *  FACILITY:
 *
 *      DECWINDOWS
 *
 *  ABSTRACT:
 *
 *      This is a user-mode subroutine that can be used to find the "best"
 *      location for a new window.  The user can specify a compass point
 *      where the placement should begin, and select X or Y ordering.  The
 *      normal mode searches for unoccluded regions of the screen and then
 *      gives added value for being near the compass location.  The user
 *      can also provide a associated window that the new window should
 *      appear near.  When the associated window is supplied, some amount
 *      of occlusion is tolerated in the region around the associated window.
 *      This routine will work on both Motif and XUI, and it can be used to
 *      position the new window within the root window - or by supplying a
 *      user window instead of the root, it can be used to position subwindows.
 *
 *  ROUTINE DESCRIPTION:
 *
 *  int FindBestWindowPosition( display, rootWindow,
 *                              width, height,
 *                              placement, priority,
 *                              xReturn, yReturn,
 *                              maxDepth, assocWindow)
 *
 *  display		- display id
 *  root		- id of the window to use as the root
 *  width, height	- width and height of proposed window in pixels
 *  placement           - placement 0 to 9 (X11 values, default = CenterGravity)
 *  priority		- 0 = programmed, 1 = Scan X first, 2 = Scan Y first
 *  x, y		- address of integers to return proposed location into
 *  maxDepth		- recursion limit (default = 3)
 *  assocWindow		- a window that the new window should be near (may be 0)
 *
 *  AUTHORS:
 *
 *      Fred Kleinsorge, (based on a BLISS routine from the VWS window system
 *                        by Tim Halvorsen circa 1983)
 *
 *  CREATION DATE:
 *
 *      April 1991
 *
 *  MODIFICATION HISTORY:
 *
 *
 */

#include <stdio.h>
#include <decw$include:Xlib.h>
#include <decw$include:Xutil.h>

extern int FindBestWindowPosition();
static void BuildWindowMap();

/*
 *  Motif has a border that is about 10 pixels.  So in width there is an
 *  additional 20 pixels in the actual window that will be created.  In
 *  Y there is another 20 pixel or so banner.  So, fudge the window size
 *  to allow for decoration when doing placement.  Also, the returned X,Y
 *  placement will be offset to anticipate decoration (since the WM may
 *  place the window at X,Y and let the decoration be -delta).
 *
 */
#define FUDGEWIDTH 32
#define FUDGEXOFF  16
#define FUDGEHEIGHT 48
#define FUDGEBANNER 32

/*
 *  Control the limit of recursion.  More recursion doesn't buy anything
 *  unless the number of layered windows is taken into account in the
 *  placement (it is not currently).  You need a depth of at least 3 for XUI,
 *  but 1 is sufficient for Motif (3 doesn't hurt, it just wastes a few cycles).
 *
 */
#define MAXDEPTH 3

/*
 *  Resolution.  This defines the grid the screen is resolved to.  Too many
 *  and the algorithm is slow, too few and the placement is coarse.  The
 *  higher the number the finer the placement.
 *
 */
#define XRES 32
#define YRES 24

/*
 *  FindBestWindowPosition
 *
 *  Find the "best" position for the proposed window using a low-resolution
 *  map of subwindows of the "root" window and the placement information
 *  supplied.  The placement allows compass direction of where the window
 *  should be located - and the priority indicates a preference in which
 *  direction the search is done in first (X or Y or let the routine pick).
 *  The "maxDepth" can limit recursion on the search for subwindows (the
 *  default is 3 to allow the routine to work on both XUI and Motif). The
 *  assocWindow provides a "gravity well" window that the window should
 *  appear near if possible.  The normal strategy is to find unoccluded
 *  positions, and there is a bonus value if the position is near the
 *  compass location (and unoccluded) or if supplied, the associated window.
 *  When supplied, the assocWindow will allow "some" occlusion within the
 *  bonus region if there is no unoccluded area within the region.
 *
 *  display		- display id
 *  root		- id of window to use as the root
 *  width, height	- width and height of proposed window
 *  placement           - gravity of placement (0-9) default = CenterGravity
 *  gravity		- 0 = default, 1 = X, 2 = Y
 *  x, y		- address of integers to return proposed location
 *  maxDepth		- limit recursion (default 3)
 *  assocWindow		- a window that the proposed window shold be near
 *
 */
int FindBestWindowPosition( display, rootWindow, width, height, placement, gravity,
                             xReturn, yReturn, maxDepth, assocWindow)
  Display *display;
  Window rootWindow, assocWindow;
  int *xReturn, *yReturn, placement, gravity, width, height, maxDepth;
{
  Window foo, root;
  XWindowAttributes wat, bonus;
  int level = 0, i, j, x, y, chunkX, chunkY, windowMap[XRES][YRES],
      xOrder[XRES], yOrder[YRES], widthChunks, heightChunks, maxD,
      proposedX1, proposedX2, proposedY1, proposedY2, suggestedX, suggestedY,
      perfect = 0, best = 0, score = 0, adjustedWidth, adjustedHeight,
      minor, major, priority = 0, bonusX1, bonusY1, bonusX2, bonusY2,
      bonusValid = 0, nonOccluded;

 /*
  *  Set X, Y or let the placement pick it priority on search
  *
  */
  if ((gravity > 2) || (gravity < 0)) priority = 0;
    else priority = gravity;

 /*
  *  Set max depth, this is how deep in the structure to look.  Normally
  *  a very shallow look (1 below the root) is all that is needed.
  *
  */
  if (maxDepth == 0) maxD = MAXDEPTH;
    else maxD = maxDepth;

 /*
  *  Get the root window ID.  Note, the "root" could simply be a regular
  *  window - and this call could be to find locations for subwindows.
  *
  */
  if (rootWindow == 0) root = XDefaultRootWindow(display);
    else root = rootWindow;

 /*
  *  Fudge the width and height to account for possible window decorations
  *
  */
  adjustedWidth  = width  + FUDGEWIDTH;
  adjustedHeight = height + FUDGEHEIGHT;

 /*
  *  Get the root width and height to set up the clipping area
  *
  */
  XGetWindowAttributes(display, root, &wat);

 /*
  *  Compute the number of bits in each screen chunk for the low-res map
  *
  */
  chunkX = (wat.width + (XRES-1))  / XRES;
  chunkY = (wat.height + (YRES-1)) / YRES;

 /*
  *  Compute the number of chunks in the proposed window (rounded up)
  *
  */
  widthChunks  = (adjustedWidth  + (chunkX-1)) / chunkX;
  heightChunks = (adjustedHeight + (chunkY-1)) / chunkY;

 /*
  *  See if there is a window to place the new window near, and if there
  *  is, get it's geometry.  To do this, first get the attributes to find
  *  it's width and then translate (0,0) into the root windows coordinate
  *  space, and then translate the width and height (which is +1 off) into
  *  the roots space.
  *
  */
  if (assocWindow != 0)
    {
      XGetWindowAttributes(display, assocWindow, &bonus);

     /*
      *  Translate (0,0) to the root coordinate space
      *
      */
      if (XTranslateCoordinates(display, assocWindow, root,
                            0, 0,
                            &bonusX1, &bonusY1, &foo)) 
        {
         /*
          *  If it translated above, the window is on the same screen,
          *  Now, translate the width and height to get X2 and Y2.
          *
          */
          XTranslateCoordinates(display, assocWindow, root,
                                bonus.width-1, bonus.height-1,
                                &bonusX2, &bonusY2, &foo);

         /*
          *  Turn the coordinates into chunk coordinates.  The region for bonus
          *  points is 1.5 times the width or height of the requested window that
          *  surrounds the associated window.
          *
          *    +---------------------------+ -
          *    |        Bonus Region       |  1.5 x height of requested window
          *    |    +-----------------+    | -
          *    |    |   assocWindow   |    |
          *    |    |                 |    |
          *    |    |                 |    |
          *    |    +-----------------+    |
          *    |                           |
          *    +---------------------------+
          *                           !    !
          *                              + 1.5 x width of requested window
          *
          */
          bonusX2  = (bonusX2 + (chunkX-1) + (adjustedWidth + (adjustedWidth/2)))  / chunkX;
          bonusX1  = (bonusX1 - (adjustedWidth + (adjustedWidth/2)))  / chunkX;
          bonusY2  = (bonusY2 + (chunkY-1) + (adjustedHeight + (adjustedHeight/2)))  / chunkY;
          bonusY1  = (bonusY1 - (adjustedHeight + (adjustedHeight/2)))  / chunkY;

         /*
          *  Clip it to be within the root window
          *
          */
          if (bonusX1 < 0) bonusX1 = 0;
          if (bonusX1 >= XRES) bonusX1 = XRES-1;
          if (bonusY1 < 0) bonusY1 = 0;
          if (bonusY1 >= YRES) bonusY1 = YRES-1;
          if (bonusX2 < 0) bonusX2 = 0;
          if (bonusX2 >= XRES) bonusX2 = XRES-1;
          if (bonusY2 < 0) bonusY2 = 0;
          if (bonusY2 >= YRES) bonusY2 = YRES-1;

         /*
          *  The bonus is valid
          *
          */
          bonusValid = 1;

        }
    }

 /*
  *  Clear the window map array.  This array is a map of what parts of the
  *  root are occluded.
  *
  */
  for (i = 0; i < YRES; i += 1)
    {
      for (j = 0; j < XRES; j += 1)
        {
          windowMap[j][i] = 0;
        }
    }

 /*
  *  Set up the ordered list of the screen sections to search for a free
  *  area.  This routine uses the ...Gravity symbols to select the
  *  pattern.  The default is a center-out search, the north, south, east
  *  and west patterns vary the search to use top-bottom, bottom-top,
  *  left-right, right-left search patterns as well.
  *
  *  0 = CenterGravity (default, equiv to 5)
  *  1 = NorthWestGravity
  *  2 = NorthGravity               1     2     3
  *  3 = NorthEastGravity
  *  4 = WestGravity                4     5     6
  *  5 = CenterGravity
  *  6 = EastGravity                7     8     9
  *  7 = SouthWestGravity
  *  8 = SouthGravity
  *  9 = SouthEastGravity
  *
  */
  switch (placement)
    {

      case NorthWestGravity:

        for (i = 0; i < XRES; i += 1)
          {
            xOrder[i] = i;
          }

        for (i = 0; i < YRES; i += 1)
          {
            yOrder[i] = i;
          }

        if (priority == 0) priority = 1;

       /*
        *  Add a bonus if located in the top-left 1/3 of the
        *  screen unless there is already a gravity well
        *  established.
        *
        *  +-----+-----+-----+
        *  |     |           |
        *  |     |           |
        *  +-----+           +
        *  |                 |
        *  |                 |
        *  +                 +
        *  |                 |
        *  |                 |
        *  +-----+-----+-----+
        */
        if (!bonusValid)
          {
            bonusX1 = 0;
            bonusY1 = 0;
            bonusX2 = XRES/3;
            bonusY2 = YRES/3;
            bonusValid = 1;
          }

        break;

      case NorthGravity:

        for (i = 0; i < XRES; i += 1)
          {
            if (i & 1) xOrder[i] = (XRES/2)+(i/2);
              else xOrder[i] = ((XRES/2)-1)-(i/2);
          }

        for (i = 0; i < YRES; i += 1)
          {
            yOrder[i] = i;
          }

        if (priority == 0) priority = 1;

       /*
        *  Add a bonus if located in the top 1/3 of the
        *  screen unless there is already a gravity well
        *  established.
        *
        *  +-----+-----+-----+
        *  |                 |
        *  |                 |
        *  +-----------------+
        *  |                 |
        *  |                 |
        *  +                 +
        *  |                 |
        *  |                 |
        *  +-----+-----+-----+
        *
        */
        if (!bonusValid)
          {
            bonusX1 = 0;
            bonusY1 = 0;
            bonusX2 = XRES-1;
            bonusY2 = YRES/3;
            bonusValid = 1;
          }

        break;

      case NorthEastGravity:

        for (i = 0; i < XRES; i += 1)
          {
            xOrder[i] = (XRES-1) - i;
          }

        for (i = 0; i < YRES; i += 1)
          {
            yOrder[i] = i;
          }

        if (priority == 0) priority = 1;

       /*
        *  Add a bonus if located in the top-right 1/3 of the
        *  screen unless there is already a gravity well
        *  established.
        *
        *  +-----+-----+-----+
        *  |           |     |
        *  |           |     |
        *  +           +-----+
        *  |                 |
        *  |                 |
        *  +                 +
        *  |                 |
        *  |                 |
        *  +-----+-----+-----+
        *
        */
        if (!bonusValid)
          {
            bonusX1 = XRES/3;
            bonusY1 = 0;
            bonusX2 = XRES-1;
            bonusY2 = YRES/3;
            bonusValid = 1;
          }

        break;

      case WestGravity:

        for (i = 0; i < XRES; i += 1)
          {
            xOrder[i] = i;
          }

        for (i = 0; i < YRES; i += 1)
          {
            if (i & 1) yOrder[i] = (YRES/2)+(i/2);
              else yOrder[i] = ((YRES/2)-1)-(i/2);
          }

        if (priority == 0) priority = 2;

       /*
        *  Add a bonus if located in the left 1/3 of the
        *  screen unless there is already a gravity well
        *  established.
        *
        *  +-----+-----+-----+
        *  |     |           |
        *  |     |           |
        *  +     |           +
        *  |     |           |
        *  |     |           |
        *  +     |           +
        *  |     |           |
        *  |     |           |
        *  +-----+-----+-----+
        *
        */
        if (!bonusValid)
          {
            bonusX1 = 0;
            bonusY1 = 0;
            bonusX2 = XRES/3;
            bonusY2 = YRES-1;
            bonusValid = 1;
          }

        break;

      case EastGravity:

        for (i = 0; i < XRES; i += 1)
          {
            xOrder[i] = (XRES-1) - i;
          }

        for (i = 0; i < YRES; i += 1)
          {
            if (i & 1) yOrder[i] = (YRES/2)+(i/2);
              else yOrder[i] = ((YRES/2)-1)-(i/2);
          }

        if (priority == 0) priority = 2;

       /*
        *  Add a bonus if located in the right 1/3 of the
        *  screen unless there is already a gravity well
        *  established.
        *
        *  +-----+-----+-----+
        *  |           |     |
        *  |           |     |
        *  +           |     +
        *  |           |     |
        *  |           |     |
        *  +           |     +
        *  |           |     |
        *  |           |     |
        *  +-----+-----+-----+
        *
        */
        if (!bonusValid)
          {
            bonusX1 = XRES/3;
            bonusY1 = 0;
            bonusX2 = XRES-1;
            bonusY2 = YRES-1;
            bonusValid = 1;
          }

        break;

      case SouthWestGravity:

        for (i = 0; i < XRES; i += 1)
          {
            xOrder[i] = i;
          }

        for (i = 0; i < YRES; i += 1)
          {
            yOrder[i] = (YRES-1) - i;
          }

        if (priority == 0) priority = 1;

       /*
        *  Add a bonus if located in the bottom-left 1/3 of the
        *  screen unless there is already a gravity well
        *  established.
        *
        *  +-----+-----+-----+
        *  |                 |
        *  |                 |
        *  +                 +
        *  |                 |
        *  |                 |
        *  +-----+           +
        *  |     |           |
        *  |     |           |
        *  +-----+-----+-----+
        *
        */
        if (!bonusValid)
          {
            bonusX1 = 0;
            bonusY1 = YRES/3;
            bonusX2 = XRES/3;
            bonusY2 = YRES-1;
            bonusValid = 1;
          }

        break;

      case SouthGravity:

        for (i = 0; i < XRES; i += 1)
          {
            if (i & 1) xOrder[i] = (XRES/2)+(i/2);
              else xOrder[i] = ((XRES/2)-1)-(i/2);
          }

        for (i = 0; i < YRES; i += 1)
          {
            yOrder[i] = (YRES-1) - i;
          }

        if (priority == 0) priority = 1;

       /*
        *  Add a bonus if located in the bottom 1/3 of the
        *  screen unless there is already a gravity well
        *  established.
        *
        *  +-----+-----+-----+
        *  |                 |
        *  |                 |
        *  +                 +
        *  |                 |
        *  |                 |
        *  +-----------------+
        *  |                 |
        *  |                 |
        *  +-----+-----+-----+
        *
        */
        if (!bonusValid)
          {
            bonusX1 = 0;
            bonusY1 = YRES/3;
            bonusX2 = XRES-1;
            bonusY2 = YRES-1;
            bonusValid = 1;
          }

        break;

      case SouthEastGravity:

        for (i = 0; i < XRES; i += 1)
          {
            xOrder[i] = (XRES-1) - i;
          }

        for (i = 0; i < YRES; i += 1)
          {
            yOrder[i] = (YRES-1) - i;
          }

        if (priority == 0) priority = 1;

       /*
        *  Add a bonus if located in the bottom-right 1/3 of the
        *  screen unless there is already a gravity well
        *  established.
        *
        *  +-----+-----+-----+
        *  |                 |
        *  |                 |
        *  +                 +
        *  |                 |
        *  |                 |
        *  +           +-----+
        *  |           |     |
        *  |           |     |
        *  +-----+-----+-----+
        *
        */
        if (!bonusValid)
          {
            bonusX1 = XRES/3;
            bonusY1 = YRES/3;
            bonusX2 = XRES-1;
            bonusY2 = YRES-1;
            bonusValid = 1;
          }

        break;

      case CenterGravity:
      default:

        for (i = 0; i < XRES; i += 1)
          {
            if (i & 1) xOrder[i] = (XRES/2)+(i/2);
              else xOrder[i] = ((XRES/2)-1)-(i/2);
          }

        for (i = 0; i < YRES; i += 1)
          {
            if (i & 1) yOrder[i] = (YRES/2)+(i/2);
              else yOrder[i] = ((YRES/2)-1)-(i/2);
          }

        if (priority == 0) priority = 1;

       /*
        *  Add a bonus if located in the middle of the
        *  screen unless there is already a gravity well
        *  established.
        *
        *  +-----+-----+-----+
        *  |                 |
        *  |                 |
        *  +     +-----+     +
        *  |     |     |     |
        *  |     |     |     |
        *  +     +-----+     +
        *  |                 |
        *  |                 |
        *  +-----+-----+-----+
        *
        */
        if (!bonusValid)
          {
            bonusX1 = (XRES-widthChunks)  >> 1;
            bonusY1 = (YRES-heightChunks) >> 1;
            bonusX2 = bonusX1 + widthChunks;
            bonusY2 = bonusY1 + heightChunks;
            bonusValid = 1;
          }

        break;

    }

 /*
  *  Walk the window tree and build the occlusion map
  *
  */
  BuildWindowMap( display, root, 0, 0, 0,
                  chunkX, chunkY, maxD, &windowMap,
                  wat.x, wat.y, wat.width-1, wat.height-1);

 /*
  *  Now find the best fit.  A "perfect score" is the first completely
  *  empty area found in our scan using the pattern we built earlier.
  *  If there is a associated window - then a perfect score is double
  *  the normal perfect score.  This means that a unoccluded spot was
  *  found in the bonus area around the associated window.
  *
  */
  best    = 0;

 /*
  *  Get the score for an unoccluded window
  *
  */
  nonOccluded = (heightChunks * widthChunks);

 /*
  *  If the bonus area is valid, add it
  *
  */
  if (bonusValid)
    {

     /*
      *  A perfect score would be twice the unoccluded score
      *
      */
      perfect = nonOccluded * 2;
    }
  else
    perfect = nonOccluded;

 /*
  *  Select X or Y precedence
  *
  */
  if (priority > 1)
    {
      major = XRES;
      minor = YRES;
    }
  else
    {
      major = YRES;
      minor = XRES;
    }

/* 
/* printf("Width %d, Height %d\n",widthChunks,heightChunks);
/* printf("Bonus: x1 %d, y1 %d, x2 %d, y2 %d\n", bonusX1, bonusY1, 
/* 						bonusX2, bonusY2);
/* printf("Perfect Score = %d, unoccluded = %d\n",perfect,nonOccluded);
/* printf("Search Order: ");
/* for (i = 0; i < major; i += 1)
/*  {
/*    for (j = 0; j < minor; j += 1)
/*     {
/*       if (priority > 1)
/*         {
/*           printf("[x %d, y %d] ", xOrder[i], yOrder[j]);
/*         }
/*       else
/*         {
/*           printf("[x %d, y %d] ", xOrder[j], yOrder[i]);
/*        }
/*     }
/*  }
/* printf("Window Map:\n");
/* for (i = 0; i < YRES; i += 1) {
/*   for (j = 0; j < XRES; j += 1) {
/*     printf("%2.2d ",windowMap[j][i]);
/*   }
/*   printf("\n");
/* }
/* 
/*
 */

   /*
    *  Major
    *
    */
    for (i = 0; i < major; i += 1)
      {

       /*
        *  Minor
        *
        */
        for (j = 0; j < minor; j += 1)
          {
           /*
            *  Propose a location.  Use the ordered search list and locate
            *  a rectangle.
            *
            */
            if (priority > 1)
              {
                proposedX1 = xOrder[i] - (widthChunks/2);
                proposedY1 = yOrder[j] - (heightChunks/2);
              }
            else
              {
                proposedX1 = xOrder[j] - (widthChunks/2);
                proposedY1 = yOrder[i] - (heightChunks/2);
              }

            if (proposedX1 < 0) proposedX1 = 0;
            if (proposedY1 < 0) proposedY1 = 0;
           
            proposedX2 = proposedX1 + (widthChunks-1);
            if (proposedX2 >= XRES) proposedX2 = XRES-1;

            proposedY2 = proposedY1 + (heightChunks-1);
            if (proposedY2 >= YRES) proposedY2 = YRES-1;

            score = 0;

           /*
            *  For each unoccluded chunk, score a point
            *
            */
            for (y = proposedY1; y <= proposedY2; y += 1)
              {
                for (x = proposedX1; x <= proposedX2; x += 1)
                  {
                    if (windowMap[x][y] == 0) score += 1;
                  }
              }           

           /*
            *  Now, try to localize the window
            *
            */
            if (bonusValid)
              {
                int xx = proposedX1;
                int yy = proposedY1;

               /*
                *  Trivial reject
                *
                */
                if ((proposedX1 > bonusX2)  ||
                    (proposedY1 > bonusY2)  ||
                    (proposedX2 < bonusX1)  ||
                    (proposedY2 < bonusY1))
                  {
                  }
                else
                  {
                   /*
                    *  Intersect the window with the bonus region
                    *
                    */
                    if (bonusX1 > proposedX1) proposedX1 = bonusX1;
                    if (bonusY1 > proposedY1) proposedY1 = bonusY1;
                    if (bonusX2 < proposedX2) proposedX2 = bonusX2;
                    if (bonusY2 < proposedY2) proposedY2 = bonusY2;

                   /*
                    *  For each unoccluded chunk of the bonus region,
                    *  score a point
                    *
                    */
                    for (y = proposedY1; y <= proposedY2; y += 1)
                      {
                        for (x = proposedX1; x <= proposedX2; x += 1)
                          {
                            if (windowMap[x][y] == 0) score += 1;
                          }
                      }           
                  }
                proposedX1 = xx;
                proposedY1 = yy;
              }

           /*
            *  Remember the high score as it's suggested location
            *
            */
            if (score > best)
              {
                best = score;
                suggestedX = proposedX1;
                suggestedY = proposedY1;
              }

            if (score == perfect) break;

          }

       /*
        *  Exit from the loop if we detect a perfect score
        *
        */
        if (score == perfect) break;

      }

/* 
/* printf("Score: %d\n", score);
/* printf("X %d, Y %d\n", suggestedX, suggestedY);
/* printf("X' %d, Y' %d\n",  (suggestedX * chunkX) + FUDGEXOFF,
/* 
/*			(suggestedY * chunkY) + FUDGEBANNER);
 */

 /*
  *  Now return the suggestion
  *
  */
  *xReturn = (suggestedX * chunkX) + FUDGEXOFF;
  *yReturn = (suggestedY * chunkY) + FUDGEBANNER;

 /*
  *  Return the score, in case someone is interested
  *
  */
  return (score);
}

/*
 *  void BuildWindowMap(display, window, level, xOff, yOff,
 *                      chunkX, chunkY, maxDepth, windowMap,
 *                      clipX1, clipY1, clipX2, clipY2)
 *
 *  Build a map representing the occupied portions of the root window.  This
 *  procedure is recursive to a programmable depth.
 *
 *  display			- display ID
 *  window			- window ID of current level
 *  level			- how deep we currently are
 *  XOff, YOff			- Accumulated offset from root (0,0)
 *  chunkX, chunkY		- number of bits for window map resolution
 *  maxDepth			- How deep can the recursion go
 *  windowMap			- Address of window map array
 *  clip..			- Bounds to clip window against
 *
 */
void BuildWindowMap(display, window, level, xOff, yOff,
                    chunkX, chunkY, maxDepth, windowMap,
                    clipX1, clipY1, clipX2, clipY2)
  Display *display;
  Window window;
  int level, xOff, yOff, chunkX, chunkY, maxDepth, windowMap[XRES][YRES];

{
  Window root, parent, *children;
  int nchildren, x, x1, x2, y, y1, y2, w, h, weight;
  XWindowAttributes wat;

    {

     /*
      *  Get information about the window
      *
      */
      XGetWindowAttributes(display, window, &wat);

      if (wat.map_state != IsViewable)  return;
      if (wat.class     != InputOutput) return;

     /*
      *  Get actual x and y positions by accumulating the offset
      *  from the root.
      *
      */
      x = wat.x + xOff;
      y = wat.y + yOff;
      w = x + wat.width;
      h = y + wat.height;


     /*
      *  Do a trivial reject for windows that are not on the screen.
      *
      */
      if ( (w < clipX1) || (h < clipY1) || (x > clipX2) || (y > clipY2))
        {
          return; /* Window is not in the clipping bounds */
        }

     /*
      *  Compute intersection of the clip and the window
      *
      */
      if (w > clipX2) w = clipX2;
      if (h > clipY2) h = clipY2;
      if (x < clipX1) x = clipX1;
      if (y < clipY1) y = clipY1;

     /*
      *  Find the windows children (if any)
      *
      */
      XQueryTree(display, window, &root, &parent, &children, &nchildren);

     /*
      *  If this isn't the root, then build the screen map of window coverage
      *
      */
      if (window != root)
        {

          x1 = x / chunkX;
          y1 = y / chunkY;
          x2 = w / chunkX;
          y2 = h / chunkY;

         /*
          *  If the window covers the entire area - ignore it.  It means
          *  that there is complete occlusion.  So why bother.  This
          *  handles the XUI pseudo-root as well.
          *
          */
	  if ((x1 != 0) || (x2 != XRES-1) || (y1 != 0) || (y2 != YRES-1))
            {

              for (h = y1; h <= y2; h += 1)
                {
                  for (w = x1; w <= x2; w += 1)
                    {
                      windowMap[w][h] += 1;
                    }
                }
            }
        }

     /*
      *  Recurse for each child until the max depth is reached or there
      *  are no more children.
      *
      */
      if (nchildren)
        {
          register int i;
          register Window *child;

         /*
          *  Recurse for every child
          *
          */
          if (level+1 <= maxDepth)
            for (i = 0, child = children; i < nchildren; ++i, ++child)
              {
                  BuildWindowMap(display, *child, level+1, x, y, 
                               chunkX, chunkY, maxDepth, windowMap,
                               clipX1, clipY1, clipX2, clipY2);
              }

         /*
          *  The query allocates a chunk of memory that needs to be freed
          *
          */
          XFree(children);
       }
    }
  }
