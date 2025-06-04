#module ms_main "Game-o-minesweep"

/*
**++
**  FACILITY:  AXP/VAX/OpenVMS
**
**  MODULE DESCRIPTION:
**
**	This is the game of mine-sweep taken from the PC/Windows environment.
**	This program was originally created to test DEC/VAX C optimizers.  It
**	was coded with some situations written one way while the same situation
**	was written another way in another part of the code.  This was to see
**	how the optimizer handled it.  The code has changed from all variables
**	passed to and from each routine that needed those variables to all (or
**	mostly) globals.
**
**	This program was also written to help newish programmers with coding
**	techniques.  There is also a BUG (aaahhh) in the current you_win
**	routine.  It is a simple BUG (aaahhh) to find for most programmers.
**	But that's not the point.  I challenge programmers to write their own
**	"you_win" code to see how creative they can get.  But first I have them
**	fix the BUG in the current code.
**
**	New programmers to this program need to determine if the ASCID routine
**	bleeds memory or not.  If it does... FIX IT.  If it does'nt, what
**	happens to it.
**
**  AUTHORS:
**
**      Brian VandeMerwe  (e-mail: brivan@spire.com)
**
**  CREATION DATE:  22-Nov-1993
**
**  DESIGN ISSUES:
**
**	Anything goes in this program.  Just remember to understand what the
**	code is doing in the optimizer.
**
**  FILES USED:
**   
**  	GAMES:MSHS.DAT  (MineSweepHighScores)
**   
**  [@optional module tags@]...
**
**  MODIFICATION HISTORY:
**
**	BVM	22-Nov-93   Original version.
**	JDW	14-Apr-94   Added timing routines to update the clock.
**	BVM	23-Apr-94   (Happy Birthday to me!) Removed JDW's timming
**			    routines and added SYS$SETIMR
**	BVM	31-Mar-95   Made code non-DECC standard.  Now it's another
**			    challenge for new programmers to make it DECC
**			    compatable.
**	BVM	04-Oct-95   Added mouse support.  Because I wanted to...
**
**--
*/

#if 1		    /* Change the 1 to a 0 to see the subrouties at work    */
#   define SPEED
#endif

#include math
#include time
#include stdlib
#include string
#include ctype
#include stdio

#include <smg$routines.h>
#include <smgdef.h>
#include <types.h>
#include <file.h>
#include <descrip.h>
#include <iodef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <libdef.h>

#include <jpidef>
#include <ssdef>

/*									    */
/* define all subroutines declared after the main().			    */
/*									    */
void 	adjust_incs();
void 	check_around();
void 	display_all_bombs();
void 	display_all_spots();
int	flag_spot();
void 	get_new_xy();
void	move_pieces();
void 	show_blanks();
int 	show_spot();
void 	show_status();
void	show_time();
void 	show_undo_message();
void 	you_win();

FILE	*fptr;					    /* File pointer	    */

struct jpiitmlst {
    short	buflen;
    short	code;
    long	bufadr;
    long	retlenadr;
};

/*
** Used to store the high scores.
*/
char	    str1[24];
char	    str2[132];

#define	NO	    0
#define	YES	    1

#define	ON	    0
#define	OFF	    1

#define MAX_COORD   35

typedef struct st_board {
    short   visible;
    short   flagged;
    short   number;
} BOARD;

tbuffer_t some_times;

time_t	start_time;

long	max_row, real_max_row;
long	max_col, real_max_col;

BOARD	board[200][200];		/* What the board looks like	    */

long
	kb_id,				/* Remember the ID's created by the */
	pb_id,				/* SMG$ routines.		    */
	v1_id,
	v2_id,
	v3_id,
	v4_id,
	yw_dsp;

long
	flags = 0,			/* Number of flags that can be used */
	total_bombs,			/* Total number of BOMBS	    */
	total_visible,			/* Total number of visible spots    */
	total_spots = 0;		/* Total number of spots	    */

char
	out_text[80],			/* Holding tank for outgoing text   */
    	time_text[15],			/* Timer text			    */
	win_text[MAX_COORD - 1];	/* Text to display on the U-win scr */

int
	attrib,				/* Color attributes		    */
	delta[2],			/* Delta time for wakeup	    */
	incx,				/* Increment for X		    */
	incy,				/* Increment for Y		    */
	text_len;			/* Length of the WIN text	    */

short
	message_status;			/* Message indication		    */
	timer_started = NO;		/* Indicator for the timer status   */

long
	coord[MAX_COORD],		/* X/Y position of each character   */
	cur_posx,			/* Used for cursor positioning for  */
	cur_posy,			/* the items on the menubar.	    */
	hrs,				/* Elapsed number of hours, HA	    */
	max_undos = 0,			/* Maximum number of times the user */
					/* is allow to use the undo	    */
					/* function			    */
	mins,				/* Elapsed number of minutes	    */
	s,				/* Return status of the SMG$ calls. */
	secs,				/* Elapsed number of seconds	    */
	show_posx,			/* Cursor position		    */
	show_posy,			/* Cursor position		    */
	time_col = 1,			/* Starting column for the timer    */
	time_row = 1;			/* Starting row for the timer	    */

char
	*tops[11][11];			/* Suck up a lot of memory for high */
					/* scores			    */

$DESCRIPTOR(dsp_time,	"0 ::1");	/* Interval for display timer	    */
$DESCRIPTOR(duh_time,	"0 ::0.20");	/* Interval for you_win timer	    */
$DESCRIPTOR(dest_str,	"                                                                                                                                    ");
$DESCRIPTOR(del_str1,	",");
$DESCRIPTOR(del_str2,	" ");
$DESCRIPTOR(datetime,	"                       ");
$DESCRIPTOR(SPACE,	" ");
$DESCRIPTOR(FLAG,	"F");

/* ------------------------------------------------------------------------ */

ascid(s)
char *s;
{
    struct dsc$descriptor *dp;
    int len;

    dp = (struct dsc$descriptor *)malloc(sizeof(struct dsc$descriptor));
    if (dp) {
	len = strlen(s);
	dp->dsc$a_pointer = malloc(len);
	if (dp->dsc$a_pointer) {
	    dp->dsc$w_length = len;
	    dp->dsc$b_dtype = DSC$K_DTYPE_T;
	    dp->dsc$b_class = DSC$K_CLASS_D;
	    strcpy(dp->dsc$a_pointer,s);
	} else {
	    free(dp);
	    return 0;
	}
    }

    return dp;
} /* ascid() */

/* ------------------------------------------------------------------------ */

/*
** Function: asciz
**
** Functional Description:
**      Transfers an ascid string to a asciz string
**
** Inputs:
**      dp - address of a descriptor
**
** Returns:
**      the address of an asciz string
*/
asciz(dp)
struct dsc$descriptor *dp;
{
    char *s;

    s = malloc(dp->dsc$w_length + 1);
    if (s) {
	strncpy(s,dp->dsc$a_pointer,dp->dsc$w_length);
	*(s + dp->dsc$w_length) = 0;
    }

    return s;
} /* asciz() */


/* ------------------------------------------------------------------------ */
/* MAIN...  This routine will do what most MAIN routines do... Neet	    */
/* description huh?							    */
/* ------------------------------------------------------------------------ */
main()
{

    short	ok = YES;		/* Used by the WHILE loop to keep   */
					/* it going			    */

    short	number_of_times;	/* Ring the bell 'number_of_times'  */
					/* when the user has won the game   */

    int		digit;			/* Used by the random number	    */
					/* generator.			    */

    int		seed;			/* Time seed for the random number  */

    int		difficulty;		/* Used to calculate a difficulty   */

    short	key;			/* Key returned by the		    */
    char	key_chr[1];		/* SMG$READ_LOCATOR function	    */

    int		kntr1,			/* Used for misc counting	    */
    		kntr2,			/* Used for misc counting	    */
    		kntr3;			/* Used for misc counting	    */

    long	vd_nr,			/* Used to define the number of	    */
		vd_nc;			/* rows and the number of columns   */
					/* used by the SMG$ features	    */

    long	pd_r,			/* Used to instruct SMG$ calls to   */
		pd_c;			/* where to display the screens	    */

    long	pd_r2,			/* Used to instruct SMG$ calls to   */
		pd_c2;			/* where to display the v2 screens  */

    long	pd_r3,			/* Used to instruct SMG$ calls to   */
		pd_c3;			/* where to display the v4 screens  */

    long	pd_r4,			/* Used to instruct SMG$ calls to   */
		pd_c4;			/* where to display the v4 screens  */

    long	timer_stuff = 1;	/* Pause time for READ_LOCATOR	    */

    long	total_count;		/* Number of bombs surrounding the  */
					/* current position		    */

    long	used_undos = 0;		/* Used to track the number of time */
					/* the user used the undo function  */

    long	end_col,		/* Variables used to draw the	    */
		end_row,		/* rectangle and lines for the main */
		start_col,		/* display.			    */
		start_row;

    long	message_col = 1,	/* Starting column for the message  */
		message_row = 1;	/* Starting row for the message	    */

    int		max_bombs;		/* Maximum # of boms on the field   */

    unsigned short
		mouse_col,		/* Coordinates of the mouse col	    */
    		mouse_row;		/* Coordinates of the mouse row	    */

    struct dsc$descriptor smg_chr_dsc = { 1, DSC$K_DTYPE_T,
	DSC$K_CLASS_S, key_chr };

    times( &some_times );		/* Get the times from the user	    */
					/* processes.			    */

    srand( some_times.proc_user_time );	/* Generate a random seed.	    */

    printf("Difficulty [1 (hard) to 10 (easy)] : ");
    scanf("%d", &difficulty);
    printf("Max number of BOMBS (0 = computer generated) : ");
    scanf("%d", &max_bombs);

    /*
    ** Make the number of times the user can use the undo function directly
    ** porportional to the level of difficulty.
    */
    max_undos = difficulty;

    /*
    ** Calculate the actual difficulty level that the user will be playing.
    ** This value will be used to place the mines on the playing grid.
    */
    difficulty = (difficulty + 1) * 10;

    /*
    ** Create a PASTEBOARD for all SMG$ routines to communicate.
    */
    s = smg$create_pasteboard(&pb_id, 0, &real_max_row, &real_max_col);

    pd_r2 = real_max_row;			/* Paste display 2 row	    */
    pd_c2 = 1;					/* Paste display 2 col	    */

    pd_r3 = pd_r2 - 2;				/* Paste display 3 row	    */
    pd_c3 = 1;					/* Paste display 3 col	    */

    pd_r4 = pd_r2 - 1;				/* Paste display 4 row	    */
    pd_c4 = 1;					/* Paste display 4 col	    */

    max_row = real_max_row / 2 - 2;
    max_col = real_max_col / 2 - 1;

    /*
    ** Clear the board.
    */
    for (kntr1 = 0; kntr1 <= max_row - 1; kntr1++) {
	for (kntr2 = 0; kntr2 <= max_col - 1; kntr2++) {
	    board[kntr1][kntr2].number = 0;
	    board[kntr1][kntr2].flagged = NO;
	    total_spots++;
	}
    }

    /*
    ** Setup the BOMBS.
    **
    ** If the helm driver requested a specific number of bombs then... else go
    ** with flow.
    */
    if (max_bombs > 0) {
	for (kntr3 = 0; kntr3 < max_bombs; kntr3++) {
	    do {
		/*
		** Random number generator does better with bigger numbers, So,
		** multiply MAX by 2 then devide the random number by 2.
		*/
		kntr1 = (rand() % (max_row * 2)) / 2;
		kntr2 = (rand() % (max_col * 2)) / 2;
	    } while (board[kntr1][kntr2].number == -1);	/* Make sure were   */
							/* not on top of a  */
							/* bomb already	    */

	    /*
	    ** Assign a bomb to this spot.
	    */
	    board[kntr1][kntr2].number = -1;
	}
    } else {
	for (kntr1 = 0; kntr1 <= max_row - 1; kntr1++) {
	    for (kntr2 = 0; kntr2 <= max_col - 1; kntr2++) {
		digit = rand() % difficulty;	/* Get a random number	    */

		/*
		** If conditions are right, then slam down a bomb.
		*/
		if (digit >= 7 && digit <= 10)
		    board[kntr1][kntr2].number = -1;
	    }
	}
    }

    /*
    ** Count the bombs.
    */
    for (kntr1 = 0; kntr1 <= max_row - 1; kntr1++) {
	for (kntr2 = 0; kntr2 <= max_col - 1; kntr2++) {
	    /*
	    ** Count the number of bombs in the surrounding area.
	    */
	    if (board[kntr1][kntr2].number == -1) {
		/*
		** Keep track of the number of flags that can be used.  This is
		** also the number of bombs that have been used.
		*/
		flags++;

	    } else {
		/*
		** Start off with the number of bombs in the surrounding area
		** as 0, and count from there.
		*/
		total_count = 0;

		if (kntr1 > 0 && kntr2 > 0)			if (board[kntr1 - 1][kntr2 - 1].number < 0) total_count++;
		if (kntr1 > 0)					if (board[kntr1 - 1][kntr2 + 0].number < 0) total_count++;
		if (kntr1 > 0 && kntr2 < max_col - 1)		if (board[kntr1 - 1][kntr2 + 1].number < 0) total_count++;

		if (kntr2 > 0)					if (board[kntr1 + 0][kntr2 - 1].number < 0) total_count++;
		if (kntr2 < max_col - 1)			if (board[kntr1 + 0][kntr2 + 1].number < 0) total_count++;

		if (kntr1 < max_row - 1 && kntr2 > 0)		if (board[kntr1 + 1][kntr2 - 1].number < 0) total_count++;
		if (kntr1 < max_row - 1)			if (board[kntr1 + 1][kntr2 + 0].number < 0) total_count++;
		if (kntr1 < max_row - 1 && kntr2 < max_col - 1)	if (board[kntr1 + 1][kntr2 + 1].number < 0) total_count++;

		/*
		** Adjust the current position with the number of bombs in the
		** surrounding area.
		*/
		board[kntr1][kntr2].number = total_count;
	    }
	}
    }

    /*
    ** Remember the total number of bombs
    */
    total_bombs = flags;

#if 0	    /* De-bugg-er-ing stuff					    */
    printf("\n\n");
    for (kntr1 = 0; kntr1 <= max_row - 1; kntr1++) {
	for (kntr2 = 0; kntr2 <= max_col - 1; kntr2++) {
	    printf ("%3d", board[kntr1][kntr2].number);
	}
	printf ("\n");
    }
#endif

    /*
    ** Create a virtual keyboard for all I/O to and from the (ODDLY ENOUGH)
    ** keyboard.
    */
    s = smg$create_virtual_keyboard(&kb_id);

    /*
    ** Define the working windows that will be used by this program.
    */
    vd_nr = (max_row + 1) * 2;
    vd_nc = (max_col + 1) * 2;
    s = smg$create_virtual_display(&vd_nr, &vd_nc, &v1_id);
    vd_nr = 1; vd_nc = real_max_col;
    s = smg$create_virtual_display(&vd_nr, &vd_nc, &v2_id);
    vd_nr = 1; vd_nc = real_max_col;
    s = smg$create_virtual_display(&vd_nr, &vd_nc, &v3_id);
    vd_nr = 1; vd_nc = real_max_col;
    s = smg$create_virtual_display(&vd_nr, &vd_nc, &v4_id);

    /*
    ** Paste the virtual display's.
    */
    s = smg$paste_virtual_display(&v2_id, &pb_id, &pd_r2, &pd_c2);
    s = smg$paste_virtual_display(&v4_id, &pb_id, &pd_r4, &pd_c4);

    start_row = 1;
    start_col = 1;
    end_row = ((max_row + 1) * 2) - 1;
    end_col = ((max_col + 1) * 2) - 1;

    attrib = SMG$M_USER2;
    attrib = 0;

    /*
    ** Outline the board.
    */
    s = smg$draw_rectangle (&v1_id, &start_row, &start_col, &end_row, &end_col, &attrib, 0);

    /*
    ** Draw the columns.
    */
    for (kntr1 = start_col; kntr1 <= end_col - 1; kntr1 = kntr1 + 2)
	s = smg$draw_line (&v1_id, &start_row, &kntr1, &end_row, &kntr1, &attrib, 0);

    /*
    ** Draw the rows.
    */
    for (kntr1 = start_col; kntr1 <= end_row - 1; kntr1 = kntr1 + 2)
	s = smg$draw_line (&v1_id, &kntr1, &start_col, &kntr1, &end_col, &attrib, 0);

    /*
    ** Show the uncovered spots
    */
    for (kntr1 = 0; kntr1 <= max_row - 1; kntr1++) {
	for (kntr2 = 0; kntr2 <= max_col - 1; kntr2++) {
	    start_row = (kntr1 + 1) * 2;
	    start_col = (kntr2 + 1) * 2;
	    s = smg$put_chars( &v1_id, &SPACE, &start_row, &start_col, 0, &SMG$M_REVERSE, 0, 0);
	}
    }

    /*
    ** Paste the virtual display's because it's time to.
    */
    pd_r = 1; pd_c = 1;
    s = smg$paste_virtual_display(&v1_id, &pb_id, &pd_r, &pd_c);
    s = smg$paste_virtual_display(&v3_id, &pb_id, &pd_r3, &pd_c3);

    hrs = mins = secs = 0;
    sprintf(time_text, "Time: %02ld:%02ld:%02ld", hrs, mins, secs);
    s = smg$put_chars( &v4_id, ascid(time_text), &time_row, &time_col, 0, 0, 0, 0);









    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
    /*									    */
    /* Stay in the loop until the game is over.				    */
    /*									    */
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
    message_status = OFF;
    cur_posy = cur_posx = 0;

    /*
    ** Setup a timer...
    */
    s = sys$bintim(&dsp_time, delta);
    s = sys$setimr(0, delta, show_time, 0, 0);

    while (ok) {
	/*
	** Update the status line.
	*/
	show_status();

	/*
	** Check for a wee-ner
	*/
	if (flags == 0 && total_visible == total_spots) {
	    /*
	    ** Call the you_win routine.
	    */
	    you_win();

	    /*
	    ** Turn the cursor ON.
	    */
	    s = smg$set_cursor_mode(&pb_id, &SMG$M_CURSOR_ON);
	    s = smg$delete_pasteboard(&pb_id);

	    /*
	    ** Setup to leave.
	    */
	    ok = 0;
	    break;
	}

	/*
	** Where the heck am I.
	*/
	show_posy = (cur_posy + 1) * 2;
	show_posx = (cur_posx + 1) * 2;

	/*
	** Set the cursor at the requested position.
	*/
	s = smg$set_cursor_abs(&v1_id, &show_posy, &show_posx);

	/*
	** Read a keystroke from the keyboard
	*/
	s = smg$read_locator(&kb_id, &mouse_row, &mouse_col, &key, &timer_stuff, 0);
/*	s = smg$read_keystroke(&kb_id, &key, 0, &timer_stuff); */

	/*
	** If there is a message being displayed, then remove it and reset the
	** message indicator.
	*/
	if (message_status == ON) {
	    message_status = OFF;

	    s = smg$erase_line (&v3_id, &message_row, &message_col);
	}

	/*
	** Act on the key used by the user.
	*/
	switch (key) {
	    /*
	    ** CTRL-R: Reset
	    */
	    case 18:
#ifdef SPEED
		s = smg$begin_display_update( &v1_id );
#endif

		/*
		** This is to reset the entire board to start over with the
		** same setup.
		*/
		for (kntr2 = 0; kntr2 <= max_col - 1; kntr2++) {
		    for (kntr1 = 0; kntr1 <= max_row - 1; kntr1++) {
			if (board[kntr1][kntr2].visible == YES) {
			    board[kntr1][kntr2].flagged = NO;
			    board[kntr1][kntr2].visible = NO;

			    start_row = (kntr1 + 1) * 2;
			    start_col = (kntr2 + 1) * 2;
			    s = smg$put_chars( &v1_id, &SPACE, &start_row, &start_col,
				0, &SMG$M_REVERSE, 0, 0);
			}
		    }
		}

#ifdef SPEED
		s = smg$end_display_update( &v1_id );
#endif
		used_undos = 0;
		total_visible = 0;
		flags = total_bombs;

		timer_started = NO;
		start_time = time(0);

		break;

	    /*
	    ** CTRL-W: We-fresh
	    */
	    case 23:
		s = smg$repaint_screen(&pb_id);
		break;

	    /*
	    ** CTRL-H
	    */
	    case 8:
		/*
		** For those of me who want to cheat.  This will "mostly" solve
		** the puzzle for me.  If spots are already flagged, then they
		** will be un-flagged.
		*/
		for (kntr1 = 0; kntr1 <= max_row - 1; kntr1++) {
		    for (kntr2 = 0; kntr2 <= max_col - 1; kntr2++) {
			if (board[kntr1][kntr2].number == -1)
			    s = flag_spot(kntr1, kntr2);

			if (board[kntr1][kntr2].number >= 0)
			    s = show_spot(kntr1, kntr2);
		    }
		}
		break;

	    /*
	    ** CTRL-U: Undo
	    */
	    case 323:	/* right mouse button				    */
	    case 21:
		/*
		** Make sure we have some "undo's" left.
		*/
		if (used_undos < max_undos) {
		    message_status = ON;

		    used_undos++;
		    show_undo_message(max_undos, used_undos);

		    /*
		    ** Show all pieces to this puzzle.
		    */
		    display_all_spots();

		} else {
		    key = 0;
		    while (key != 26) {
			/*
			** Read a keystroke from the keyboard
			*/
			s = smg$read_keystroke(&kb_id, &key);
		    }

		    /*
		    ** Turn the cursor ON.
		    */
		    s = smg$set_cursor_mode(&pb_id, &SMG$M_CURSOR_ON);
		    s = smg$delete_pasteboard(&pb_id);

		    ok = 0;
		}

		break;

	    /*
	    ** CTRL-Z: Exit
	    */
	    case 26:
		/*
		** Turn the cursor ON and remove the pasteboard.
		*/
		s = smg$set_cursor_mode(&pb_id, &SMG$M_CURSOR_ON);
		s = smg$delete_pasteboard(&pb_id);

		/*
		** Hasta la vista baby.
		*/
		ok = 0;
		break;

	    /*
	    ** Up Arrow
	    */
	    case SMG$K_TRM_UP:
		cur_posy--;
		if (cur_posy < 0) cur_posy = max_row - 1;
		break;

	    /*
	    ** Down Arrow
	    */
	    case SMG$K_TRM_DOWN:
		cur_posy++;
		if (cur_posy == max_row) cur_posy = 0;
		break;

	    /*
	    ** Left Arrow
	    */
	    case SMG$K_TRM_LEFT:
		cur_posx--;
		if (cur_posx < 0) cur_posx = max_col - 1;
		break;

	    /*
	    ** Right Arrow
	    */
	    case SMG$K_TRM_RIGHT:
		cur_posx++;
		if (cur_posx == max_col) cur_posx = 0;
		break;

	    /*
	    ** Flag the spot
	    */
	    case 322:	/* middle mouse button				    */
	    case 102:	/* lowercase "f"				    */
	    case 70:	/* uppercase "F"				    */
		/*
		** Try to make sure the mouse was actually used.
		*/
		if (mouse_row > 0 && mouse_col > 0) {
		    cur_posy = (mouse_row <= max_row * 2 ? (mouse_row - 1) / 2 : max_row - 1);
		    cur_posx = (mouse_col <= max_col * 2 ? (mouse_col - 1) / 2 : max_col - 1);
		}

		/*
		** If the timer has not been started yet, duh, then start it.
		*/
		if (timer_started == NO) {
		    timer_started = YES;
		    start_time = time(0);
		}

		s = flag_spot(cur_posy, cur_posx);
		break;

	    /*
	    ** RETURN key and SPACE key.
	    */
	    case 321:	/* left mouse button				    */
	    case 32:	/* space bar					    */
	    case 13:	/* return key					    */
		/*
		** Try to make sure the mouse was actually used.
		*/
		if (mouse_row > 0 && mouse_col > 0) {
		    cur_posy = (mouse_row <= max_row * 2 ? (mouse_row - 1) / 2 : max_row - 1);
		    cur_posx = (mouse_col <= max_col * 2 ? (mouse_col - 1) / 2 : max_col - 1);
		}

		/*
		** If the timer has not been started yet, duh, then start it.
		*/
		if (timer_started == NO) {
		    timer_started = YES;
		    start_time = time(0);
		}

		s = show_spot(cur_posy, cur_posx);
		break;

	    /*
	    ** Parse the last key pressed.
	    */
	    default:
		break;

	}
    }

}


/* ------------------------------------------------------------------------ */
/* Called by SYS$SETIMR (EF 0) to update the time display		    */
/* ------------------------------------------------------------------------ */
void show_time()
{
    /*
    ** Update the timer on the screen.
    */
    if (timer_started) {
	secs = (time(0) - start_time);
	mins = secs / 60;
	secs -= mins * 60;

	hrs = mins / 60;
	mins -= hrs * 60;

	sprintf(time_text, "Time: %02ld:%02ld:%02ld", hrs, mins, secs);
	s = smg$put_chars(&v4_id, ascid(time_text), &time_row, &time_col,
	    0, 0, 0, 0);
    }

    show_posy = (cur_posy + 1) * 2;
    show_posx = (cur_posx + 1) * 2;

    /*
    ** Set the cursor at the requested position.
    */
    s = smg$set_cursor_abs(&v1_id, &show_posy, &show_posx);

    /*
    ** Setup a timer...
    */
    s = sys$setimr(0, delta, show_time, 0, 0);
}


/* ------------------------------------------------------------------------ */
/* This routine will display the current spot that is sent by the caller.   */
/* If there is a problem, return a status of NO (found a bomb), else return */
/* YES (clear).								    */
/* ------------------------------------------------------------------------ */
int show_spot(int loc_1, int loc_2)
{
    int		kntr1;			/* Used for misc counting	    */
    int		kntr2;			/* Used for misc counting	    */
    int		kntr3;			/* Used for misc counting	    */

    long	start_col;		/* Positional variables		    */
    long	start_row;

    /*
    ** If the current spot was flagged, then return, Else if the current spot
    ** was a bomb, then show all of the bombs, otherwise, just show the number
    ** of bombs around the current spot.
    */
    if (board[loc_1][loc_2].flagged == YES) {
	return;

    } else if (board[loc_1][loc_2].number == -1) {
	display_all_bombs(loc_1, loc_2);

    } else if (board[loc_1][loc_2].visible == NO) {
	start_row = (loc_1 + 1) * 2;
	start_col = (loc_2 + 1) * 2;

	if (board[loc_1][loc_2].number == 0) attrib = SMG$M_BOLD | SMG$M_USER1;
	if (board[loc_1][loc_2].number == 1) attrib = SMG$M_BOLD | SMG$M_USER1;
	if (board[loc_1][loc_2].number == 2) attrib = SMG$M_BOLD | SMG$M_USER2;
	if (board[loc_1][loc_2].number == 3) attrib = SMG$M_BOLD | SMG$M_USER3;
	if (board[loc_1][loc_2].number == 4) attrib = SMG$M_BOLD | SMG$M_USER4;
	if (board[loc_1][loc_2].number == 5) attrib = SMG$M_BOLD | SMG$M_USER5;
	if (board[loc_1][loc_2].number == 6) attrib = SMG$M_BOLD | SMG$M_USER6;
	if (board[loc_1][loc_2].number == 7) attrib = SMG$M_BOLD | SMG$M_USER7;
	if (board[loc_1][loc_2].number == 8) attrib = SMG$M_BOLD | SMG$M_USER8;

	sprintf(out_text, "%d", board[loc_1][loc_2].number);
	board[loc_1][loc_2].visible = YES;
	total_visible++;

	/*
	** If the current spot is a "0", than show as many "blanks" as
	** possible.
	*/
	if (board[loc_1][loc_2].number == 0) {
#ifdef SPEED
	    s = smg$begin_display_update( &v1_id );
	    show_blanks(loc_1, loc_2);
	    s = smg$end_display_update( &v1_id );
#else
	    show_blanks(loc_1, loc_2);
#endif
	} else
	    s = smg$put_chars( &v1_id, ascid(out_text), &start_row, &start_col,
		0, &attrib, 0, 0);
    }
}


/* ------------------------------------------------------------------------ */
/* This routine will display the current spot that is sent by the caller.   */
/* If there is a problem, return a status of NO (found a bomb), else return */
/* YES (clear).								    */
/* ------------------------------------------------------------------------ */
void show_blanks(int loc_1, int loc_2)
{

    int		kntr1;			/* Used for misc counting	    */
    int		kntr2;			/* Used for misc counting	    */
    int		kntr3;			/* Used for misc counting	    */

    short	ok = YES;		/* Used for the while loop	    */

    long	start_col;		/* Positional variables		    */
    long	start_row;

    long	start_loc_1;		/* Original starting position	    */
    long	start_loc_2;

    start_row = (loc_1 + 1) * 2;
    start_col = (loc_2 + 1) * 2;

    /*
    ** Clear the current spot.
    */
    s = smg$put_chars( &v1_id, &SPACE, &start_row, &start_col, 0, 0, 0, 0);

    if (board[loc_1][loc_2].visible == NO) total_visible++;
    board[loc_1][loc_2].visible = YES;

    /*
    ** Display surrounding numbers.
    */
    check_around(loc_1, loc_2);

    /*
    ** Check right.
    */
    if (loc_2 < max_col - 1 && board[loc_1][loc_2 + 1].visible == NO && 
      board[loc_1][loc_2 + 1].number == 0)
	show_blanks(loc_1, loc_2 + 1);

    /*
    ** Check left.
    */
    if (loc_2 > 0 && board[loc_1][loc_2 - 1].visible == NO &&
      board[loc_1][loc_2 - 1].number == 0)
	show_blanks(loc_1, loc_2 - 1);

    /*
    ** Check up.
    */
    if (loc_1 > 0 && board[loc_1 - 1][loc_2].visible == NO &&
      board[loc_1 - 1][loc_2].number == 0)
	show_blanks(loc_1 - 1, loc_2);

    /*
    ** Check down.
    */
    if (loc_1 < max_row - 1 && board[loc_1 + 1][loc_2].visible == NO &&
      board[loc_1 + 1][loc_2].number == 0)
	show_blanks(loc_1 + 1, loc_2);

    /*
    ** Check up/right.
    */
    if (loc_2 < max_col - 1 && loc_1 > 0 && board[loc_1 - 1][loc_2 + 1].visible == NO && 
      board[loc_1 - 1][loc_2 + 1].number == 0)
	show_blanks(loc_1 - 1, loc_2 + 1);

    /*
    ** Check down/right.
    */
    if (loc_2 < max_col - 1 && loc_1 < max_row - 1 && board[loc_1 + 1][loc_2 + 1].visible == NO && 
      board[loc_1 + 1][loc_2 + 1].number == 0)
	show_blanks(loc_1 + 1, loc_2 + 1);

    /*
    ** Check down/left.
    */
    if (loc_2 > 0 && loc_1 < max_row - 1 && board[loc_1 + 1][loc_2 - 1].visible == NO && 
      board[loc_1 + 1][loc_2 - 1].number == 0)
	show_blanks(loc_1 + 1, loc_2 - 1);

    /*
    ** Check up/left.
    */
    if (loc_2 > 0 && loc_1 > 0 && board[loc_1 - 1][loc_2 - 1].visible == NO && 
      board[loc_1 - 1][loc_2 - 1].number == 0)
	show_blanks(loc_1 - 1, loc_2 - 1);

}


/* ------------------------------------------------------------------------ */
/* This routine will update the positions aroung the current spot.	    */
/* ------------------------------------------------------------------------ */
void check_around(int loc_1, int loc_2)
{
    long	start_col;		/* Positional variables		    */
    long	start_row;

    /*
    ** Check UP/LEFT
    */
    if (loc_1 > 0 && loc_2 > 0) {
	if (board[loc_1 - 1][loc_2 - 1].number > 0 &&
	    board[loc_1 - 1][loc_2 - 1].visible == NO)
	{
	    start_row = ((loc_1 - 1) + 1) * 2;
	    start_col = ((loc_2 - 1) + 1) * 2;

	    sprintf(out_text, "%d", board[loc_1 - 1][loc_2 - 1].number);

	    if (board[loc_1 - 1][loc_2 - 1].number == 0) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 - 1][loc_2 - 1].number == 1) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 - 1][loc_2 - 1].number == 2) attrib = SMG$M_BOLD | SMG$M_USER2;
	    if (board[loc_1 - 1][loc_2 - 1].number == 3) attrib = SMG$M_BOLD | SMG$M_USER3;
	    if (board[loc_1 - 1][loc_2 - 1].number == 4) attrib = SMG$M_BOLD | SMG$M_USER4;
	    if (board[loc_1 - 1][loc_2 - 1].number == 5) attrib = SMG$M_BOLD | SMG$M_USER5;
	    if (board[loc_1 - 1][loc_2 - 1].number == 6) attrib = SMG$M_BOLD | SMG$M_USER6;
	    if (board[loc_1 - 1][loc_2 - 1].number == 7) attrib = SMG$M_BOLD | SMG$M_USER7;
	    if (board[loc_1 - 1][loc_2 - 1].number == 8) attrib = SMG$M_BOLD | SMG$M_USER8;

	    s = smg$put_chars( &v1_id, ascid(out_text), &start_row, &start_col,
		0, &attrib, 0, 0);

	    board[loc_1 - 1][loc_2 - 1].visible = YES;
	    total_visible++;
	}
    }


    /*
    ** Check UP
    */
    if (loc_1 > 0) {
	if (board[loc_1 - 1][loc_2 + 0].number > 0 &&
	    board[loc_1 - 1][loc_2 + 0].visible == NO)
	{
	    start_row = ((loc_1 - 1) + 1) * 2;
	    start_col = ((loc_2 + 0) + 1) * 2;

	    sprintf(out_text, "%d", board[loc_1 - 1][loc_2 + 0].number);

	    if (board[loc_1 - 1][loc_2 + 0].number == 0) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 - 1][loc_2 + 0].number == 1) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 - 1][loc_2 + 0].number == 2) attrib = SMG$M_BOLD | SMG$M_USER2;
	    if (board[loc_1 - 1][loc_2 + 0].number == 3) attrib = SMG$M_BOLD | SMG$M_USER3;
	    if (board[loc_1 - 1][loc_2 + 0].number == 4) attrib = SMG$M_BOLD | SMG$M_USER4;
	    if (board[loc_1 - 1][loc_2 + 0].number == 5) attrib = SMG$M_BOLD | SMG$M_USER5;
	    if (board[loc_1 - 1][loc_2 + 0].number == 6) attrib = SMG$M_BOLD | SMG$M_USER6;
	    if (board[loc_1 - 1][loc_2 + 0].number == 7) attrib = SMG$M_BOLD | SMG$M_USER7;
	    if (board[loc_1 - 1][loc_2 + 0].number == 8) attrib = SMG$M_BOLD | SMG$M_USER8;

	    s = smg$put_chars( &v1_id, ascid(out_text), &start_row, &start_col,
		0, &attrib, 0, 0);

	    board[loc_1 - 1][loc_2 + 0].visible = YES;
	    total_visible++;
	}
    }

    /*
    ** Check UP/RIGHT
    */
    if (loc_1 > 0 && loc_2 < max_col - 1) {
	if (board[loc_1 - 1][loc_2 + 1].number > 0 &&
	    board[loc_1 - 1][loc_2 + 1].visible == NO)
	{
	    start_row = ((loc_1 - 1) + 1) * 2;
	    start_col = ((loc_2 + 1) + 1) * 2;

	    sprintf(out_text, "%d", board[loc_1 - 1][loc_2 + 1].number);

	    if (board[loc_1 - 1][loc_2 + 1].number == 0) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 - 1][loc_2 + 1].number == 1) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 - 1][loc_2 + 1].number == 2) attrib = SMG$M_BOLD | SMG$M_USER2;
	    if (board[loc_1 - 1][loc_2 + 1].number == 3) attrib = SMG$M_BOLD | SMG$M_USER3;
	    if (board[loc_1 - 1][loc_2 + 1].number == 4) attrib = SMG$M_BOLD | SMG$M_USER4;
	    if (board[loc_1 - 1][loc_2 + 1].number == 5) attrib = SMG$M_BOLD | SMG$M_USER5;
	    if (board[loc_1 - 1][loc_2 + 1].number == 6) attrib = SMG$M_BOLD | SMG$M_USER6;
	    if (board[loc_1 - 1][loc_2 + 1].number == 7) attrib = SMG$M_BOLD | SMG$M_USER7;
	    if (board[loc_1 - 1][loc_2 + 1].number == 8) attrib = SMG$M_BOLD | SMG$M_USER8;

	    s = smg$put_chars( &v1_id, ascid(out_text), &start_row, &start_col,
		0, &attrib, 0, 0);

	    board[loc_1 - 1][loc_2 + 1].visible = YES;
	    total_visible++;
	}
    }

    /*
    ** Check LEFT
    */
    if (loc_2 > 0) {
	if (board[loc_1 + 0][loc_2 - 1].number > 0 &&
	    board[loc_1 + 0][loc_2 - 1].visible == NO)
	{
	    start_row = ((loc_1 + 0) + 1) * 2;
	    start_col = ((loc_2 - 1) + 1) * 2;

	    sprintf(out_text, "%d", board[loc_1 + 0][loc_2 - 1].number);

	    if (board[loc_1 + 0][loc_2 - 1].number == 0) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 + 0][loc_2 - 1].number == 1) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 + 0][loc_2 - 1].number == 2) attrib = SMG$M_BOLD | SMG$M_USER2;
	    if (board[loc_1 + 0][loc_2 - 1].number == 3) attrib = SMG$M_BOLD | SMG$M_USER3;
	    if (board[loc_1 + 0][loc_2 - 1].number == 4) attrib = SMG$M_BOLD | SMG$M_USER4;
	    if (board[loc_1 + 0][loc_2 - 1].number == 5) attrib = SMG$M_BOLD | SMG$M_USER5;
	    if (board[loc_1 + 0][loc_2 - 1].number == 6) attrib = SMG$M_BOLD | SMG$M_USER6;
	    if (board[loc_1 + 0][loc_2 - 1].number == 7) attrib = SMG$M_BOLD | SMG$M_USER7;
	    if (board[loc_1 + 0][loc_2 - 1].number == 8) attrib = SMG$M_BOLD | SMG$M_USER8;

	    s = smg$put_chars( &v1_id, ascid(out_text), &start_row, &start_col,
		0, &attrib, 0, 0);

	    board[loc_1 + 0][loc_2 - 1].visible = YES;
	    total_visible++;
	}
    }

    /*
    ** Check RIGHT
    */
    if (loc_2 < max_col - 1) {
	if (board[loc_1 + 0][loc_2 + 1].number > 0 &&
	    board[loc_1 + 0][loc_2 + 1].visible == NO)
	{
	    start_row = ((loc_1 + 0) + 1) * 2;
	    start_col = ((loc_2 + 1) + 1) * 2;

	    sprintf(out_text, "%d", board[loc_1 + 0][loc_2 + 1].number);

	    if (board[loc_1 + 0][loc_2 + 1].number == 0) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 + 0][loc_2 + 1].number == 1) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 + 0][loc_2 + 1].number == 2) attrib = SMG$M_BOLD | SMG$M_USER2;
	    if (board[loc_1 + 0][loc_2 + 1].number == 3) attrib = SMG$M_BOLD | SMG$M_USER3;
	    if (board[loc_1 + 0][loc_2 + 1].number == 4) attrib = SMG$M_BOLD | SMG$M_USER4;
	    if (board[loc_1 + 0][loc_2 + 1].number == 5) attrib = SMG$M_BOLD | SMG$M_USER5;
	    if (board[loc_1 + 0][loc_2 + 1].number == 6) attrib = SMG$M_BOLD | SMG$M_USER6;
	    if (board[loc_1 + 0][loc_2 + 1].number == 7) attrib = SMG$M_BOLD | SMG$M_USER7;
	    if (board[loc_1 + 0][loc_2 + 1].number == 8) attrib = SMG$M_BOLD | SMG$M_USER8;

	    s = smg$put_chars( &v1_id, ascid(out_text), &start_row, &start_col,
		0, &attrib, 0, 0);

	    board[loc_1 + 0][loc_2 + 1].visible = YES;
	    total_visible++;
	}
    }

    /*
    ** Check DOWN/LEFT
    */
    if (loc_1 < max_row - 1 && loc_2 > 0) {
	if (board[loc_1 + 1][loc_2 - 1].number > 0 &&
	    board[loc_1 + 1][loc_2 - 1].visible == NO)
	{
	    start_row = ((loc_1 + 1) + 1) * 2;
	    start_col = ((loc_2 - 1) + 1) * 2;

	    sprintf(out_text, "%d", board[loc_1 + 1][loc_2 - 1].number);

	    if (board[loc_1 + 1][loc_2 - 1].number == 0) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 + 1][loc_2 - 1].number == 1) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 + 1][loc_2 - 1].number == 2) attrib = SMG$M_BOLD | SMG$M_USER2;
	    if (board[loc_1 + 1][loc_2 - 1].number == 3) attrib = SMG$M_BOLD | SMG$M_USER3;
	    if (board[loc_1 + 1][loc_2 - 1].number == 4) attrib = SMG$M_BOLD | SMG$M_USER4;
	    if (board[loc_1 + 1][loc_2 - 1].number == 5) attrib = SMG$M_BOLD | SMG$M_USER5;
	    if (board[loc_1 + 1][loc_2 - 1].number == 6) attrib = SMG$M_BOLD | SMG$M_USER6;
	    if (board[loc_1 + 1][loc_2 - 1].number == 7) attrib = SMG$M_BOLD | SMG$M_USER7;
	    if (board[loc_1 + 1][loc_2 - 1].number == 8) attrib = SMG$M_BOLD | SMG$M_USER8;

	    s = smg$put_chars( &v1_id, ascid(out_text), &start_row, &start_col,
		0, &attrib, 0, 0);

	    board[loc_1 + 1][loc_2 - 1].visible = YES;
	    total_visible++;
	}
    }

    /*
    ** Check DOWN
    */
    if (loc_1 < max_row - 1) {
	if (board[loc_1 + 1][loc_2 + 0].number > 0 &&
	    board[loc_1 + 1][loc_2 + 0].visible == NO)
	{
	    start_row = ((loc_1 + 1) + 1) * 2;
	    start_col = ((loc_2 + 0) + 1) * 2;

	    sprintf(out_text, "%d", board[loc_1 + 1][loc_2 + 0].number);

	    if (board[loc_1 + 1][loc_2 + 0].number == 0) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 + 1][loc_2 + 0].number == 1) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 + 1][loc_2 + 0].number == 2) attrib = SMG$M_BOLD | SMG$M_USER2;
	    if (board[loc_1 + 1][loc_2 + 0].number == 3) attrib = SMG$M_BOLD | SMG$M_USER3;
	    if (board[loc_1 + 1][loc_2 + 0].number == 4) attrib = SMG$M_BOLD | SMG$M_USER4;
	    if (board[loc_1 + 1][loc_2 + 0].number == 5) attrib = SMG$M_BOLD | SMG$M_USER5;
	    if (board[loc_1 + 1][loc_2 + 0].number == 6) attrib = SMG$M_BOLD | SMG$M_USER6;
	    if (board[loc_1 + 1][loc_2 + 0].number == 7) attrib = SMG$M_BOLD | SMG$M_USER7;
	    if (board[loc_1 + 1][loc_2 + 0].number == 8) attrib = SMG$M_BOLD | SMG$M_USER8;

	    s = smg$put_chars( &v1_id, ascid(out_text), &start_row, &start_col,
		0, &attrib, 0, 0);

	    board[loc_1 + 1][loc_2 + 0].visible = YES;
	    total_visible++;
	}
    }

    /*
    ** Check DOWN/RIGHT
    */
    if (loc_1 < max_row - 1 && loc_2 < max_col - 1) {
	if (board[loc_1 + 1][loc_2 + 1].number > 0 &&
	    board[loc_1 + 1][loc_2 + 1].visible == NO)
	{
	    start_row = ((loc_1 + 1) + 1) * 2;
	    start_col = ((loc_2 + 1) + 1) * 2;

	    sprintf(out_text, "%d", board[loc_1 + 1][loc_2 + 1].number);

	    if (board[loc_1 + 1][loc_2 + 1].number == 0) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 + 1][loc_2 + 1].number == 1) attrib = SMG$M_BOLD | SMG$M_USER1;
	    if (board[loc_1 + 1][loc_2 + 1].number == 2) attrib = SMG$M_BOLD | SMG$M_USER2;
	    if (board[loc_1 + 1][loc_2 + 1].number == 3) attrib = SMG$M_BOLD | SMG$M_USER3;
	    if (board[loc_1 + 1][loc_2 + 1].number == 4) attrib = SMG$M_BOLD | SMG$M_USER4;
	    if (board[loc_1 + 1][loc_2 + 1].number == 5) attrib = SMG$M_BOLD | SMG$M_USER5;
	    if (board[loc_1 + 1][loc_2 + 1].number == 6) attrib = SMG$M_BOLD | SMG$M_USER6;
	    if (board[loc_1 + 1][loc_2 + 1].number == 7) attrib = SMG$M_BOLD | SMG$M_USER7;
	    if (board[loc_1 + 1][loc_2 + 1].number == 8) attrib = SMG$M_BOLD | SMG$M_USER8;

	    s = smg$put_chars( &v1_id, ascid(out_text), &start_row, &start_col,
		0, &attrib, 0, 0);

	    board[loc_1 + 1][loc_2 + 1].visible = YES;
	    total_visible++;
	}
    }

}


/* ------------------------------------------------------------------------ */
/* This routine will flag the current spot that is sent by the caller.	    */
/* ------------------------------------------------------------------------ */
int flag_spot(int loc_1, int loc_2)
{

    int		kntr1;			/* Used for misc counting	    */
    int		kntr2;			/* Used for misc counting	    */
    int		kntr3;			/* Used for misc counting	    */

    long	start_col;		/* Positional variables		    */
    long	start_row;

    /*
    ** Flag current spot.
    */
    start_row = (loc_1 + 1) * 2;
    start_col = (loc_2 + 1) * 2;

    /*
    ** If the current spot is not flagged, duh, then flag it.
    */
    if (board[loc_1][loc_2].flagged == NO) {
	/*
	** Adjust the visible counter if this is the first time, this spot is
	** exposed.
	*/
	if (board[loc_1][loc_2].visible == NO) total_visible++;

	board[loc_1][loc_2].flagged = YES;
	board[loc_1][loc_2].visible = YES;
	attrib = SMG$M_REVERSE | SMG$M_BOLD | SMG$M_USER6;
	s = smg$put_chars( &v1_id, &FLAG, &start_row, &start_col, 0, &attrib, 0, 0);

	flags--;

    /*
    ** If the spot was flagged, then un-flag it.
    */
    } else {
	board[loc_1][loc_2].flagged = NO;
	board[loc_1][loc_2].visible = NO;
	s = smg$put_chars( &v1_id, &SPACE, &start_row, &start_col, 0, &SMG$M_REVERSE, 0, 0);

	flags++;
	total_visible--;
    }

    return(0);

}


/* ------------------------------------------------------------------------ */
/* This routine will display the status line on the bottom of the screen    */
/* ------------------------------------------------------------------------ */
void show_status()
{
    long	start_col;		/* Positional variables		    */
    long	start_row;

    /*
    ** Tell the user how many flags there are and how much time has been used.
    */
    start_row = 1;
    start_col = 1;
    sprintf(out_text, "BOMBS = %3d   Flags = %3d   Total Visible = %3d   Total Spots = %3d   ",
	total_bombs, flags, total_visible, total_spots);
    s = smg$put_chars( &v2_id, ascid(out_text), &start_row, &start_col, 0, 0, 0, 0);
}


/* ------------------------------------------------------------------------ */
/* This routine will display the message concerning the number of undo's    */
/* ------------------------------------------------------------------------ */
void show_undo_message(long max, long current)
{
    long	start_col;		/* Positional variables		    */
    long	start_row;

    message_status = ON;

    start_row = 1;
    start_col = 1;

    /*
    ** The the helm driver how many time's he/she/it can undo.
    */
    if (current < max)
	sprintf(out_text, "You have %d %s left!", max - current,
	    (max - current == 1) ? "UNDO" : "UNDO's");
    else
	sprintf(out_text, "No more UNDO's left Butthead!");

    s = smg$put_chars( &v3_id, ascid(out_text), &start_row, &start_col,
	0, &SMG$M_REVERSE, 0, 0);

}


/* ------------------------------------------------------------------------ */
/* This routine will display all bombs and highlight the one passed in the  */
/* arg list.								    */
/* ------------------------------------------------------------------------ */
void display_all_spots ()
{

    int		kntr1;			/* Used for misc counting	    */
    int		kntr2;			/* Used for misc counting	    */
    int		kntr3;			/* Used for misc counting	    */

    long	attribute;		/* Display attributes		    */

    long	start_col;		/* Positional variables		    */
    long	start_row;

#ifdef SPEED
    s = smg$begin_display_update( &v1_id );
#endif

    /*
    ** This section intentionally not commented for newish programmers to try
    ** and figure out what's going on.
    */

    /*
    ** Display all SPOTS.
    */
    for (kntr1 = 0; kntr1 <= max_row - 1; kntr1++) {
	for (kntr2 = 0; kntr2 <= max_col - 1; kntr2++) {
	    if (board[kntr1][kntr2].visible == NO)
	    {
		start_row = (kntr1 + 1) * 2;
		start_col = (kntr2 + 1) * 2;

		attribute = SMG$M_REVERSE;

		if (board[kntr1][kntr2].number == -1)
		{
		    if (board[kntr1][kntr2].flagged == YES){
			sprintf(out_text, "F");

		    } else {
			sprintf(out_text, " ");
		    }

		} else {
		    if (board[kntr1][kntr2].flagged == YES)
			sprintf(out_text, "F");
		    else
			if (board[kntr1][kntr2].visible == NO)
			    sprintf(out_text, " ");
		}

		s = smg$put_chars( &v1_id, ascid(out_text), &start_row,
		    &start_col, 0, &attribute, 0, 0);

	    } else {
		if (board[kntr1][kntr2].flagged == YES &&
		    board[kntr1][kntr2].number != -1)
		{
		    sprintf(out_text, "F");

		    start_row = (kntr1 + 1) * 2;
		    start_col = (kntr2 + 1) * 2;

		    s = smg$put_chars( &v1_id, ascid(out_text), &start_row,
			&start_col, 0, &attribute, 0, 0);

		}
	    }
	}
    }

#ifdef SPEED
    s = smg$end_display_update( &v1_id );
#endif
}


/* ------------------------------------------------------------------------ */
/* This routine will display all bombs and highlight the one passed in the  */
/* arg list.								    */
/* ------------------------------------------------------------------------ */
void display_all_bombs (int loc_1, int loc_2)
{
    int		kntr1;			/* Used for misc counting	    */
    int		kntr2;			/* Used for misc counting	    */
    int		kntr3;			/* Used for misc counting	    */

    long	attribute;		/* Display attributes		    */

    long	start_col;		/* Positional variables		    */
    long	start_row;

#ifdef SPEED
    s = smg$begin_display_update( &v1_id );
#endif

    /*
    ** Display all BOMBS.
    */
    for (kntr1 = 0; kntr1 <= max_row - 1; kntr1++) {
	for (kntr2 = 0; kntr2 <= max_col - 1; kntr2++) {
	    if (board[kntr1][kntr2].visible == NO)
	    {
		start_row = (kntr1 + 1) * 2;
		start_col = (kntr2 + 1) * 2;

		attribute = SMG$M_NORMAL;

		if (board[kntr1][kntr2].number == -1)
		{
		    if (board[kntr1][kntr2].flagged == YES){
			attribute = SMG$M_REVERSE;
			sprintf(out_text, "F");

		    } else {
			attribute = SMG$M_REVERSE | SMG$M_USER4;
			sprintf(out_text, "*");
		    }

		} else {
		    if (board[kntr1][kntr2].flagged == YES)
		    {
			attribute = SMG$M_BLINK;
			sprintf(out_text, "F");

		    } else {
			if (board[kntr1][kntr2].visible == NO)
			    sprintf(out_text, "~");
			else {
			    if (board[kntr1][kntr2].number == 0)
				sprintf(out_text, " ");
			    else
				sprintf(out_text, "%d", board[kntr1][kntr2].number);
			}
		    }
		}

		s = smg$put_chars( &v1_id, ascid(out_text), &start_row,
		    &start_col, 0, &attribute, 0, 0);

	    } else {
		if (board[kntr1][kntr2].flagged == YES &&
		    board[kntr1][kntr2].number != -1)
		{
		    attribute = SMG$M_BLINK;
		    sprintf(out_text, "F");

		    start_row = (kntr1 + 1) * 2;
		    start_col = (kntr2 + 1) * 2;

		    s = smg$put_chars( &v1_id, ascid(out_text), &start_row,
			&start_col, 0, &attribute, 0, 0);
		}
	    }
	}
    }

    start_row = (loc_1 + 1) * 2;
    start_col = (loc_2 + 1) * 2;
    attribute = SMG$M_BOLD | SMG$M_USER3;
    s = smg$put_chars( &v1_id, ascid("*"), &start_row, &start_col,
	0, &attribute, 0, 0);

#ifdef SPEED
    s = smg$end_display_update( &v1_id );
#endif

}


/* ------------------------------------------------------------------------ */
/* This routine will display the YOU_WIN stuff				    */
/* ------------------------------------------------------------------------ */
void you_win()
{
    short	direction;		/* Direction of the YOU WIN text    */

    short	key;			/* Key returned by the		    */

    short	ok = YES;		/* Used by the WHILE loop to keep   */
					/* it going			    */

    int		attribute;		/* Attribute for characters	    */

    int		kntr1;			/* Used for misc counting	    */
    int		kntr2;			/* Used for misc counting	    */
    int		kntr3;			/* Used for misc counting	    */
    int		shift;			/* Used for misc counting	    */

    long	start_col;		/* Positional variables		    */
    long	start_row;

    long	secs;			/* Elapsed number of seconds	    */
    long	mins;			/* Elapsed number of minutes	    */
    long	hrs;			/* Elapsed number of hours, HA	    */

    long	new_timer_stuff = 0;	/* Used to break out of the message */
					/* loop				    */

    /*
    ** Cancel update timer.
    */
    s = sys$cantim (0, 0);

    /*
    ** All coordinates start as 0
    */
    for (shift = 0; shift <= MAX_COORD; shift++)
	coord[shift] = 0;

    /*
    ** Get the total number of secs/mins/hrs the user used to finish the game.
    */
    secs = (time(0) - start_time);
    mins = secs / 60;
    secs -= mins * 60;

    hrs = mins / 60;
    mins -= hrs * 60;

    sprintf(win_text, "Elapsed Time: %02ld:%02ld:%02ld  ", hrs, mins, secs);
    sprintf(win_text, "You Win!! ");

    kntr1 = rand() % (max_col - 1) + 1;		/* Get a random number	    */
    kntr2 = rand() % (max_row - 1) + 1;		/* Get a random number	    */

    get_new_xy(&incx, &incy);

    /*
    ** Find a starting point for the message.
    */
    coord[0] = kntr1 * max_col + kntr2;

    /*
    ** Make sure we don't start on a bomb.
    */
    while (board[kntr1][kntr2].number == -1) {
	if (kntr1 < max_row)
	    kntr1++;
	else
	    if (kntr2 < max_row)
		kntr2++;
    }

    /*
    ** Length of the YOU_WIN text message.
    */
    text_len = strlen(win_text);

#ifdef SPEED
    s = smg$begin_display_update( &v1_id );
#endif

    /*
    ** Show them where the bombs are.
    */
    for (kntr1 = 0; kntr1 <= max_row - 1; kntr1++) {
	for (kntr2 = 0; kntr2 <= max_col - 1; kntr2++) {
	    if (board[kntr1][kntr2].number != 0) {
		attribute = SMG$M_REVERSE | SMG$M_USER4;
		sprintf(out_text, "*");

		/*
		** Remove all numbers too.
		*/
		if (board[kntr1][kntr2].number > 0) {
		    attribute = SMG$M_NORMAL;
		    sprintf(out_text, " ");
		}

		start_row = (kntr1 + 1) * 2;
		start_col = (kntr2 + 1) * 2;

		s = smg$put_chars( &v1_id, ascid(out_text), &start_row,
		    &start_col, 0, &attribute, 0, 0);
	    }
	}
    }

#ifdef SPEED
    s = smg$end_display_update( &v1_id );
#endif

    /*
    ** Turn the cursor OFF
    */
    s = smg$set_cursor_mode(&pb_id, &SMG$M_CURSOR_OFF);

    /*
    ** Setup a timer...
    */
    s = sys$bintim(&duh_time, delta);
    s = sys$setimr(1, delta, move_pieces, 1, 0);

    /*
    ** Get things moving.
    */
    ok = TRUE;
    while (ok) {
	s = smg$read_keystroke(&kb_id, &key);
	if (key == 26) break;
    }

}


/* ------------------------------------------------------------------------ */
/* This routine will move the "peices" around the board for the you_win	    */
/* routine.								    */
/* ------------------------------------------------------------------------ */
void move_pieces()
{

    int		attribute;		/* Attribute for characters	    */

    int		kntr1;			/* Used for misc counting	    */
    int		kntr2;			/* Used for misc counting	    */
    int		kntr3;			/* Used for misc counting	    */
    int		shift;			/* Used for misc counting	    */

    long	start_col;		/* Positional variables		    */
    long	start_row;

#ifdef SPEED
    s = smg$begin_display_update( &v1_id );
#endif

    for (kntr3 = 0; kntr3 <= text_len && coord[kntr3] != 0; kntr3++) {
	/*
	** Get logical coordinates.
	*/
	kntr1 = coord[kntr3] / max_col;
	kntr2 = coord[kntr3] - (kntr1 * max_col);

	/*
	** Get physical coordinates.
	*/
	start_col = (kntr1 + 1) * 2;
	start_row = (kntr2 + 1) * 2;

	sprintf(out_text, "%c", win_text[kntr3]);

	/*
	** Get the color of the text.
	*/
	if (kntr3 - 8 * (kntr3 / 8) == 0) attribute = SMG$M_USER1;
	if (kntr3 - 8 * (kntr3 / 8) == 1) attribute = SMG$M_USER2;
	if (kntr3 - 8 * (kntr3 / 8) == 2) attribute = SMG$M_USER3;
	if (kntr3 - 8 * (kntr3 / 8) == 3) attribute = SMG$M_USER4;
	if (kntr3 - 8 * (kntr3 / 8) == 4) attribute = SMG$M_USER5;
	if (kntr3 - 8 * (kntr3 / 8) == 5) attribute = SMG$M_USER6;
	if (kntr3 - 8 * (kntr3 / 8) == 6) attribute = SMG$M_USER7;
	if (kntr3 - 8 * (kntr3 / 8) == 7) attribute = SMG$M_USER8;

	s = smg$put_chars( &v1_id, ascid(out_text), &start_row,
	    &start_col, 0, &attribute, 0, 0);
    }

#ifdef SPEED
    s = smg$end_display_update( &v1_id );
#endif

    /*
    ** Shift all following pieces.
    */
    for (shift = MAX_COORD; shift > 0; shift--)
	coord[shift] = coord[shift - 1];

    /*
    ** Get logical coordinates.
    */
    kntr1 = coord[0] / max_col;
    kntr2 = coord[0] - (kntr1 * max_col);

    /*
    ** Move the head. (Thats what she says)
    */
    kntr1 += incx;
    kntr2 += incy;

    /*
    ** If we hit a bomb, then change directions.
    */
    while (board[kntr2][kntr1].number == -1) {
	/*
	** Put the head back.
	*/
	kntr1 -= incx;
	kntr2 -= incy;

	get_new_xy(&incx, &incy);

	/*
	** Move the head of the win text.
	*/
	kntr1 += incx;
	kntr2 += incy;
    }

    /*
    ** If we hit an edge, change directions.
    */
    if (kntr1 >= max_col - 1 || kntr1 <= 0 && incx != 0) incx = 0 - incx;
    if (kntr2 >= max_row - 1 || kntr2 <= 0 && incy != 0) incy = 0 - incy;
#if 0
    if (kntr1 >= max_col || kntr1 < 0 && incx == 0) get_new_xy(&incx, &incy);
    if (kntr2 >= max_row || kntr2 < 0 && incy == 0) get_new_xy(&incx, &incy);
#endif

    /*
    ** Catch-all. Yea Right.
    */
    do {
	if (kntr1 >= max_col || kntr1 < 0) kntr1 = rand() % (max_col - 1) + 1;
	if (kntr2 >= max_row || kntr2 < 0) kntr2 = rand() % (max_row - 1) + 1;
    } while ((kntr1 >= max_col || kntr1 < 0) || (kntr2 >= max_row || kntr2 < 0));

    /*
    ** Start the head of the YOU WIN message at the newly calculated starting
    ** position
    */
    coord[0] = kntr1 * max_col + kntr2;

    /*
    ** Setup a timer...
    */
    s = sys$setimr(1, delta, move_pieces, 1, 0);
}


/* ------------------------------------------------------------------------ */
/* This routine will make sure the increments are in the necessary limits.  */
/* ------------------------------------------------------------------------ */
void get_new_xy(int *p_x, int *p_y)
{
    int x = *p_x;
    int y = *p_y;

    do {
	/*
	** Get a random direction.
	*/
	x = rand() % 10 + 1;
	y = rand() % 10 + 1;

	/*
	** Make sure we have a correct direction.
	*/
	adjust_incs(&x, &y);
    } while (x == 0 && y == 0);

    *p_x = x;
    *p_y = y;
}


/* ------------------------------------------------------------------------ */
/* This routine will make sure the increments are in the necessary limits.  */
/* ------------------------------------------------------------------------ */
void adjust_incs(int *p_x, int *p_y)
{
    int x = *p_x;
    int y = *p_y;

#if 0

    if (x < 3)		    *p_x = -1;
    if (x >=3 && x <= 6)    *p_x = 0;
    if (x > 6)		    *p_x = 1;

    if (y < 3)		    *p_y = -1;
    if (y >=3 && y <= 6)    *p_y = 0;
    if (y > 6)		    *p_y = 1;

#else

    /*
    ** Make only diags.
    */
    *p_x = (x < 5) ? -1 : 1;
    *p_y = (y < 5) ? -1 : 1;

#endif
}
