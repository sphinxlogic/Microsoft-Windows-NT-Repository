#module BannerHanoi "V1.0"

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


#include "stdio.h"
#include "MrmAppl.h"


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

static int last_number_of_disks = -1;
static int hanoi_init = 0;

static int next_cycle = 1;
static int disk_cycle = 0;
static int wait = 1;
static int wait_cycle = 2;

static int nil = -1;

static int next_move = 0;

static Hanoi$_MoveVec *move_vector = NULL;
static int number_of_disks;
static int zero = 0;
static int tower_attribute_block, disk_attribute_block;
static int tower_line_width, disk_line_width, disk_thickness,
	   tower_bottom, tower_top, tower_shift, tower_base_size;

static int tower_left[3], tower_middle[3], tower_right[3], 
	   tower_height[3];

static int *disk_left = NULL, *disk_right = NULL, *disk_height = NULL;

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	MoveDisk (k, x_increment, y_increment, tower);
**
**      This routine moves the k'th disk.
**
**  FORMAL PARAMETERS:
**
**      K - selects which disk to erase
**	X_INCREMENT - added to disk's x coordinate
**	Y_INCREMENT - added to disk's y coordinate
**	TOWER - selects which spindle to redraw (vertical movement only)
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
static void	
MoveDisk(k, x_increment, y_increment, tower)
int x_increment, y_increment, k, tower;
{
    int i, height, width, x, y, step;
    int current_left_x, current_right_x, current_bottom_y, current_top_y,
        current_y;
    int delta_x, delta_y, one_delta_x, one_delta_y;
    int new_left_x, new_right_x, new_y, new_top_y, new_bottom_y, n_pixels;
    int left, right, up, down;

/*
 * Compute number of pixelone-pixel steps in the move, and the increments
 * to the x and y coordinates in each step.
 */

    n_pixels = abs(y_increment);
    left = right = up = down = 0;
    step = 1;
    if (Bnr.hanoi_height/12 > 1)
	step = Bnr.hanoi_height/12;
    
    if (n_pixels == 0)
	{
	n_pixels = abs (x_increment);
	one_delta_x = x_increment/n_pixels;
	one_delta_y = 0;
	delta_x = one_delta_x * step;
	delta_y = 0;
	right = (one_delta_x == 1);
	left = (one_delta_x == -1);
	one_delta_y = 0;
	} 
    else
	{
	one_delta_x = 0;
	one_delta_y = y_increment/abs (y_increment);
	delta_x = 0;
	delta_y = one_delta_y * step;
	up = (one_delta_y == 1);
	down = (one_delta_y == -1);
	}
/*
 * Compute the current top, bottom, left, and right coordinates for a
 * rectangle that exactly fits the disk.
 */

    current_left_x = disk_left[k] - x_increment;
    current_right_x = disk_right[k] - x_increment;
    current_y = disk_height[k] + y_increment;
    current_top_y = current_y - disk_thickness;
    current_bottom_y = current_y + disk_thickness;

    i = 1;
    while (i <= n_pixels)
	{
	if ((i + abs (delta_y + delta_x)) > n_pixels)
	    {
	    delta_y = one_delta_y;
	    delta_x = one_delta_x;
	    }
	i = i + abs(delta_y + delta_x);
	/*
	 * First, compute the disk's new location
	 */

	new_left_x = current_left_x + delta_x;
	new_right_x = current_right_x + delta_x;
	new_y = current_y - delta_y;
	new_top_y = current_top_y - delta_y;
	new_bottom_y = current_bottom_y - delta_y;
	/*
	 * Draw the disk in its new location
	 */
	GcValues.line_width = disk_line_width;
	XChangeGC (XtDisplay(Bnr.main_widget), BannerGC, GCLineWidth, &GcValues);
	XDrawLine (XtDisplay(Bnr.hanoi_widget), XtWindow (Bnr.hanoi_widget),
	    BannerGC, new_left_x, new_y, new_right_x, new_y);
	XSync (XtDisplay(Bnr.hanoi_widget), FALSE);
	/*
	 * Erase the row of pixels from the old location.  For vertical
	 * motion, replace the missing tower pixel if necessary.
	 */

	if (up)
	    {
	    width = abs (current_right_x - current_left_x);
	    height = abs (current_bottom_y - new_bottom_y);
	    x = current_left_x;
	    y = new_bottom_y;
	    XSetBackground (XtDisplay (Bnr.hanoi_widget), 
		    BannerGC,
		    Bnr.foreground);
	    XSetForeground (XtDisplay (Bnr.hanoi_widget), 
		    BannerGC,
		    Bnr.background);
	    XFillRectangle (XtDisplay(Bnr.hanoi_widget), XtWindow (Bnr.hanoi_widget),
		BannerGC, x, y, width, height);
	    XSetBackground (XtDisplay (Bnr.hanoi_widget), 
		    BannerGC,
		    Bnr.background);
	    XSetForeground (XtDisplay (Bnr.hanoi_widget), 
		    BannerGC,
		    Bnr.foreground);
	    GcValues.line_width = tower_line_width;
	    XChangeGC (XtDisplay(Bnr.main_widget), BannerGC, GCLineWidth, &GcValues);
	    if (new_bottom_y >= tower_top)
		XDrawLine (XtDisplay(Bnr.hanoi_widget), XtWindow (Bnr.hanoi_widget),
		    BannerGC, tower_middle[tower], tower_top, 
		    tower_middle[tower], current_bottom_y);
	    XSync (XtDisplay(Bnr.hanoi_widget), FALSE);
	    }
	else
	    if (down)
		{
		width = abs (current_right_x - current_left_x);
		height = abs (new_bottom_y - current_bottom_y);
		x = current_left_x;
		y = current_top_y;
		XSetBackground (XtDisplay (Bnr.hanoi_widget), 
		    BannerGC,
		    Bnr.foreground);
		XSetForeground (XtDisplay (Bnr.hanoi_widget), 
		    BannerGC,
		    Bnr.background);
		XFillRectangle (XtDisplay(Bnr.hanoi_widget), XtWindow (Bnr.hanoi_widget),
		    BannerGC, x, y, width, height);
		XSetBackground (XtDisplay (Bnr.hanoi_widget), 
		    BannerGC,
		    Bnr.background);
		XSetForeground (XtDisplay (Bnr.hanoi_widget), 
		    BannerGC,
		    Bnr.foreground);
		GcValues.line_width = tower_line_width;
		XChangeGC (XtDisplay(Bnr.main_widget), BannerGC, GCLineWidth, &GcValues);
		if (current_top_y >= tower_top)
		    XDrawLine (XtDisplay(Bnr.hanoi_widget), XtWindow (Bnr.hanoi_widget),
			BannerGC, tower_middle[tower], tower_top, 
			tower_middle[tower], new_bottom_y);
		XSync (XtDisplay(Bnr.hanoi_widget), FALSE);
		}
	    else
		if (left)
		    {
		    width = abs (current_right_x - new_right_x);
		    height = abs (current_bottom_y - new_top_y);
		    x = new_right_x;
		    y = new_top_y;
		    XSetBackground (XtDisplay (Bnr.hanoi_widget), 
			BannerGC,
			Bnr.foreground);
		    XSetForeground (XtDisplay (Bnr.hanoi_widget), 
			BannerGC,
			Bnr.background);
		    XFillRectangle (XtDisplay(Bnr.hanoi_widget), XtWindow (Bnr.hanoi_widget),
			BannerGC, x, y, width, height);
		    XSetBackground (XtDisplay (Bnr.hanoi_widget), 
			BannerGC,
			Bnr.background);
		    XSetForeground (XtDisplay (Bnr.hanoi_widget), 
			BannerGC,
			Bnr.foreground);
		    XSync (XtDisplay(Bnr.hanoi_widget), FALSE);
		    }
		else
		    {
		    width = abs (new_right_x - current_right_x);
		    height = abs (current_bottom_y - new_top_y);
		    x = current_left_x;
		    y = new_top_y;
		    XSetBackground (XtDisplay (Bnr.hanoi_widget), 
			BannerGC,
			Bnr.foreground);
		    XSetForeground (XtDisplay (Bnr.hanoi_widget), 
			BannerGC,
			Bnr.background);
		    XFillRectangle (XtDisplay(Bnr.hanoi_widget), XtWindow (Bnr.hanoi_widget),
			BannerGC, x, y, width, height);
		    XSetBackground (XtDisplay (Bnr.hanoi_widget), 
			BannerGC,
			Bnr.background);
		    XSetForeground (XtDisplay (Bnr.hanoi_widget), 
			BannerGC,
			Bnr.foreground);
		    XSync (XtDisplay(Bnr.hanoi_widget), FALSE);
		    }
	/*
	 * Finally, update the current coordinates.
	 */
	 current_left_x = new_left_x;
	 current_right_x = new_right_x;
	 current_y = new_y;
	 current_top_y = new_top_y;
	 current_bottom_y = new_bottom_y;
	}
GcValues.line_width = 0;
XChangeGC (XtDisplay(Bnr.main_widget), BannerGC, GCLineWidth, &GcValues);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Move (n, from_tower, to_tower, using_tower)
**
**      This routine moves N disk from from_tower, to to_tower, using
**  using_tower as tomporary storage.
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
static void	
Move(n, from_tower, to_tower, using_tower)
int n, from_tower, to_tower, using_tower;
{
    int vertical_shift, lateral_shift;

    /*
     * First move (.N - 1) disks to the USING_TOWER temporarily.
     */
    if (n > 1)
	Move (n-1, from_tower, using_tower, to_tower);
    
    move_vector[next_move].move = 1;
    move_vector[next_move].from = from_tower;
    move_vector[next_move].to = to_tower;
    move_vector[next_move].using = using_tower;
    move_vector[next_move].num_disk = n;
    next_move++;
    /*
     * Finally move the (.N - 1) smaller disks from the USING_TOWER
     * to the TO_TOWER.
     */
    if (n > 1)
	Move (n-1, using_tower, to_tower, from_tower);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	MakeMove (n, from_tower, to_tower, using_tower);
**
**      This routine activates the actual move of a tower to another tower.
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
static void	
MakeMove(n, from_tower, to_tower, using_tower)
int n, from_tower, to_tower, using_tower;
{
    int vertical_shift, lateral_shift;

    /*
     * Now move the bottom disk.
     */

    vertical_shift = disk_height[n];
    disk_height[n] = tower_top - disk_thickness;
    vertical_shift = vertical_shift - disk_height[n];
    tower_height[from_tower] = tower_height[from_tower] +
	(disk_thickness * 3);
    MoveDisk (n, 0, vertical_shift, from_tower);
    lateral_shift = tower_shift * (to_tower - from_tower);
    disk_left[n] = lateral_shift + disk_left[n];
    disk_right[n] = lateral_shift + disk_right[n];
    MoveDisk (n, lateral_shift, 0, nil);
    vertical_shift = disk_height[n];
    tower_height[to_tower] = tower_height[to_tower] -
	(disk_thickness * 3);
    disk_height[n] = tower_height[to_tower] +
	(disk_thickness * 2);
    vertical_shift = vertical_shift - disk_height[n];
    MoveDisk (n, 0, vertical_shift, to_tower);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**  BannerHanopiSetup ()
**
**      this sets up all the preset information for hanoi.
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
BannerHanoiSetup()
{
    int disk_indentation, i;
    float width;

    number_of_disks = Bnr.hanoi_height/12;
    if (number_of_disks < 3 )
	number_of_disks = 3;

    tower_line_width = 0;

    /*
     * Width of tower base
     */
    width = Bnr.hanoi_width;
    tower_base_size = ((width/3.0) * 0.8) - 2;

    disk_line_width = Bnr.hanoi_height/(number_of_disks*2);
    disk_line_width = (disk_line_width/2) * 2;
    disk_thickness = disk_line_width/2;

    /*
     * Compute Y-coordinates of the tops and bottoms of the towers
     */

    tower_bottom = Bnr.hanoi_height - 
	(Bnr.hanoi_height - (3 * disk_thickness * number_of_disks + 
         disk_thickness))/3;

    tower_top = tower_bottom - (3 * disk_thickness * number_of_disks +
	    disk_thickness);

    /*
     * Distance between towers.
     */

    width = tower_base_size;
    tower_shift = tower_base_size + (0.2 * width);

    /*
     * Compute X-coordinates of the endpoints of the tower lines.
     */

    tower_left[0] = (Bnr.hanoi_width - (2 * tower_shift + tower_base_size)) / 2;
    tower_left[1] = tower_left[0] + tower_shift;
    tower_left[2] = tower_left[1] + tower_shift;
    tower_right[0] = tower_left[0] + tower_base_size;
    tower_right[1] = tower_right[0] + tower_shift;
    tower_right[2] = tower_right[1] + tower_shift;
    tower_middle[0] = (tower_left[0] + tower_right[0])/2;
    tower_middle[1] = tower_middle[0] + tower_shift;
    tower_middle[2] = tower_middle[1] + tower_shift;

    /*
     * Y-coordinates of the top disk of each tower
     */

    tower_height[0] = tower_top;
    tower_height[1] = tower_height[2] = tower_bottom - disk_thickness;

    /*
     * Allocate storage for the vectors of disk coordinates.  The 0'th
     * element of each vector is not used.
     */

    if (last_number_of_disks != number_of_disks)
	{
	int size, i;

	last_number_of_disks = number_of_disks;

	size = 2;	
	
	/*
	 * size = sizeof (Hanoi$_MoveVec) * ((2^n - 1) + 1)
	 */
	for (i=1;  i<=number_of_disks;  i++)
	    size = size * 2;
	
	size = size * sizeof(Hanoi$_MoveVec);

	if (move_vector != NULL)
	    free(move_vector);
	move_vector = malloc(size);

	if (move_vector == NULL)
	    {
	    printf ("Failed to allocate memory for the Hanoi\n %d disks needs %d bytes of memory\n",
	    	number_of_disks, size);
	    exit (0);
	    }

	if (disk_left != NULL)
	    free(disk_left);
	disk_left = malloc (4 * (number_of_disks + 1));

	if (disk_height != NULL)
	    free(disk_height);
	disk_height = malloc (4 * (number_of_disks + 1));

	if (disk_right != NULL)
	    free(disk_right);
	disk_right = malloc (4 * (number_of_disks + 1));

	tower_attribute_block = 21;
	disk_attribute_block = 22;
	}
    /*
     * Place the disks in their initial configuration:
     *
     * Compute indentation of one disk with respect to the next larger
     * disk (which will be immediately underneath it).
     */

    disk_indentation = (tower_base_size / number_of_disks) / 2;

    /*
     * Compute the initial coordinates of the bottom disk.
     */

    disk_left[number_of_disks] = tower_left[0];
    disk_right[number_of_disks] = tower_right[0];
    disk_height[number_of_disks] = tower_bottom - (disk_thickness * 2);
    
    for (i=number_of_disks-1;  i>=1;  i--)
	{
	disk_left[i] = disk_left[i+1] + disk_indentation;
	disk_right[i] = disk_right[i+1] - disk_indentation;
	disk_height[i] = disk_height[i + 1] - 3*disk_thickness;
	}
Bnr.ws_purged = 0;
hanoi_init=1;
next_move=0;
move_vector[next_move].move=0;
disk_cycle = 0;
next_cycle = wait = 1;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerHanoiRefresh ()
**
**      This routine repaints the current hanoi image.
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
BannerHanoiRefresh()
{
    int i;

    if (! hanoi_init)
	BannerHanoiSetup ();

    GcValues.line_width = tower_line_width;
    XChangeGC (XtDisplay(Bnr.main_widget), BannerGC, GCLineWidth, &GcValues);

    /*
     * Draw the three tower bases, then the three spindles.
     */

    XDrawLine (XtDisplay (Bnr.hanoi_widget), XtWindow (Bnr.hanoi_widget),
	BannerGC, tower_left[0], tower_bottom, tower_right[0], 
	tower_bottom);
    XDrawLine (XtDisplay (Bnr.hanoi_widget), XtWindow (Bnr.hanoi_widget),
	BannerGC, tower_left[1], tower_bottom, tower_right[1], 
	tower_bottom);
    XDrawLine (XtDisplay (Bnr.hanoi_widget), XtWindow (Bnr.hanoi_widget),
	BannerGC, tower_left[2], tower_bottom, tower_right[2], 
	tower_bottom);

    XDrawLine (XtDisplay (Bnr.hanoi_widget), XtWindow (Bnr.hanoi_widget),
	BannerGC, tower_middle[0], tower_bottom, tower_middle[0], 
	tower_top);
    XDrawLine (XtDisplay (Bnr.hanoi_widget), XtWindow (Bnr.hanoi_widget),
	BannerGC, tower_middle[1], tower_bottom, tower_middle[1], 
	tower_top);
    XDrawLine (XtDisplay (Bnr.hanoi_widget), XtWindow (Bnr.hanoi_widget),
	BannerGC, tower_middle[2], tower_bottom, tower_middle[2], 
	tower_top);

    /*
     * Now draw the disks.
     */

    GcValues.line_width = disk_line_width;
    XChangeGC (XtDisplay(Bnr.main_widget), BannerGC, GCLineWidth, &GcValues);

    for (i=1;  i<=number_of_disks;  i++)
	XDrawLine (XtDisplay(Bnr.hanoi_widget), XtWindow(Bnr.hanoi_widget),
	    BannerGC, disk_left[i], disk_height[i], 
	    disk_right[i], disk_height[i]);

    GcValues.line_width = 0;
    XChangeGC (XtDisplay(Bnr.main_widget), BannerGC, GCLineWidth, &GcValues);

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**  BannerHanoi ()
**
**      This is the main action routine for hanoi.
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
BannerHanoi()
{

    if (! hanoi_init)
	{
	BannerHanoiSetup ();
	BannerHanoiRefresh();
	}

    if (wait && wait_cycle != 0)
	{
	wait_cycle --;
	return;
	}
    else
	{
	wait_cycle = 3;
	wait = 0;
	}

    if (next_cycle)
	{
	disk_cycle++;
	next_cycle = 0;
	next_move = 0;
	if (disk_cycle == 1 || disk_cycle == 4)
	    {
	    disk_cycle = 1;
	    Move (number_of_disks, 0, 1, 2);
	    }
	else
	    if (disk_cycle == 2)
		Move (number_of_disks, 1, 2, 0);
	    else
		if (disk_cycle == 3)
		    Move (number_of_disks, 2, 0, 1);
	move_vector[next_move].move=0;
	next_move=0;
	}

if (move_vector[next_move].move != 0)
    {
    MakeMove (move_vector[next_move].num_disk,
	move_vector[next_move].from,
	move_vector[next_move].to,
	move_vector[next_move].using);
    next_move++;
    }
else
    next_cycle=wait=1;

}
