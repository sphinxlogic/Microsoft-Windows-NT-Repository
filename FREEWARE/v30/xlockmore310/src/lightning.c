
#ifndef lint
static char sccsid[] = "@(#)lightning.c	3.10 96/07/20 xlockmore";

#endif

/*-
 * lightning.c - fractal lightning bolds for xlock, the X Window System
 *               lockscreen.
 *
 * See xlock.c for copying information.
 *
 * Revision History:
 * 14-Jul-96: Cleaned up code.
 * 27-Jun-96: Written and submitted by Keith Romberg <kromberg@saxe.com>.
 */

#include "xlock.h"
#include <math.h>

ModeSpecOpt lightning_opts =
{0, NULL, NULL, NULL};

/*----------------------------   defines   -------------------------------*/

#define BOLT_NUMBER 4
#define BOLT_ITERATION 4
#define LONG_FORK_ITERATION 3
#define MEDIUM_FORK_ITERATION 2
#define SMALL_FORK_ITERATION 1

#define WIDTH_VARIATION 30
#define HEIGHT_VARIATION 15

#define DELAY_TIME_AMOUNT 15
#define MULTI_DELAY_TIME_BASE 5

#define MAX_WIGGLES 16
#define WIGGLE_BASE 8
#define WIGGLE_AMOUNT 14

#define RANDOM_FORK_PROBILITY   4

#define FIRST_LEVEL_STRIKE 0
#define LEVEL_ONE_STRIKE 1
#define LEVEL_TWO_STRIKE 2

#define BOLT_VERTICIES ((1<<BOLT_ITERATION)-1)
  /* BOLT_ITERATION = 4. 2^(BOLT_ITERATION) - 1 = 15 */

#define NUMBER_FORK_VERTICIES 9

#define FLASH_PROBILITY 20
#define MAX_FLASH_AMOUNT 2	/*  half the total duration of the bolt  */

typedef struct {
	XPoint      ForkVerticies[NUMBER_FORK_VERTICIES];
	int         num_used;
} Fork;

typedef struct {
	XPoint      end1, end2;
	XPoint      middle[BOLT_VERTICIES];
	int         fork_number;
	int         forks_start[2];
	Fork        branch[2];
	int         wiggle_number;
	int         wiggle_amount;
	int         delay_time;
	int         flash;
	int         flash_begin, flash_stop;
	int         visible;
	int         strike_level;
} Lightning;

typedef struct {
	Lightning   bolts[BOLT_NUMBER];
	int         scr_width, scr_height;
	int         multi_strike;
	int         give_it_hell;
	int         draw_time;
	int         stage;
	unsigned long color;
} Storm;

static Storm *Helga = NULL;

/*-------------------   function prototypes  ----------------------------*/

static int  distance(XPoint a, XPoint b);

static int  setup_multi_strike(void);
static int  flashing_strike(void);
static void flash_duration(int *start, int *end, int total_duration);
static void random_storm(Storm * st);
static void generate(XPoint A, XPoint B, int iter, XPoint * verts, int *index);
static void create_fork(Fork * f, XPoint start, XPoint end, int level);

static void first_strike(Lightning bolt, ModeInfo * mi);
static void draw_bolt(Lightning * bolt, ModeInfo * mi);
static void draw_line(ModeInfo * mi, XPoint * p, int number, GC use, int x_offset);
static void level1_strike(Lightning bolt, ModeInfo * mi);
static void level2_strike(Lightning bolt, ModeInfo * mi);

static int  storm_active(Storm * st);
static void update_bolt(Lightning * bolt, int time);
static void wiggle_bolt(Lightning * bolt);
static void wiggle_line(XPoint * p, int number, int wiggle_amount);

/*-------------------------  functions  ---------------------------------*/

static int
setup_multi_strike(void)
{
	int         result, multi_prob;

	multi_prob = NRAND(100);

	if (multi_prob < 50)
		result = 1;
	else if ((multi_prob >= 51) && (multi_prob < 75))
		result = 2;
	else if ((multi_prob >= 76) && (multi_prob < 92))
		result = 3;
	else
		result = BOLT_NUMBER;	/* 4 */

	return (result);
}

/*-------------------------------------------------------------------------*/

static int
flashing_strike(void)
{
	int         tmp = NRAND(FLASH_PROBILITY);

	if (tmp <= FLASH_PROBILITY)
		return (1);
	return (0);
}

/*-------------------------------------------------------------------------*/

static void
flash_duration(start, end, total_duration)
	int        *start, *end, total_duration;


{
	int         mid, d;

	mid = total_duration / MAX_FLASH_AMOUNT;
	d = NRAND(total_duration / MAX_FLASH_AMOUNT) / 2;
	*start = mid - d;
	*end = mid + d;
}

/*-------------------------------------------------------------------------*/

static void
random_storm(Storm * st)
{
	int         i, j, tmp;
	XPoint      p;

	for (i = 0; i < st->multi_strike; i++) {
		st->bolts[i].end1.x = NRAND(st->scr_width);
		st->bolts[i].end1.y = 0;
		st->bolts[i].end2.x = NRAND(st->scr_width);
		st->bolts[i].end2.y = st->scr_height;
		st->bolts[i].wiggle_number = WIGGLE_BASE + NRAND(MAX_WIGGLES);
		if ((st->bolts[i].flash = flashing_strike()))
			flash_duration(&(st->bolts[i].flash_begin), &(st->bolts[i].flash_stop),
				       st->bolts[i].wiggle_number);
		else
			st->bolts[i].flash_begin = st->bolts[i].flash_stop = 0;
		st->bolts[i].wiggle_amount = WIGGLE_AMOUNT;
		if (i == 0)
			st->bolts[i].delay_time = NRAND(DELAY_TIME_AMOUNT);
		else
			st->bolts[i].delay_time = NRAND(DELAY_TIME_AMOUNT) +
				(MULTI_DELAY_TIME_BASE * i);
		st->bolts[i].strike_level = FIRST_LEVEL_STRIKE;
		tmp = 0;
		generate(st->bolts[i].end1, st->bolts[i].end2, BOLT_ITERATION,
			 st->bolts[i].middle, &tmp);
		st->bolts[i].fork_number = 0;
		st->bolts[i].visible = 0;
		for (j = 0; j < BOLT_VERTICIES; j++) {
			if (st->bolts[i].fork_number >= 2)
				break;
			if (NRAND(100) < RANDOM_FORK_PROBILITY) {
				p.x = NRAND(st->scr_width);
				p.y = st->scr_height;
				st->bolts[i].forks_start[st->bolts[i].fork_number] = j;
				create_fork(&(st->bolts[i].branch[st->bolts[i].fork_number]),
					    st->bolts[i].middle[j], p, j);
				st->bolts[i].fork_number++;
			}
		}
	}
}

static void
generate(XPoint A, XPoint B, int iter, XPoint * verts, int *index)
{
	XPoint      mid;

	mid.x = (A.x + B.x) / 2 + NRAND(WIDTH_VARIATION) - WIDTH_VARIATION / 2;
	mid.y = (A.y + B.y) / 2 + NRAND(HEIGHT_VARIATION) - HEIGHT_VARIATION / 2;

	if (!iter) {
		verts[*index].x = mid.x;
		verts[*index].y = mid.y;
		(*index)++;
		return;
	}
	generate(A, mid, iter - 1, verts, index);
	generate(mid, B, iter - 1, verts, index);
}

/*------------------------------------------------------------------------*/

static void
create_fork(Fork * f, XPoint start, XPoint end, int level)
{
	int         tmp = 1;

	f->ForkVerticies[0].x = start.x;
	f->ForkVerticies[0].y = start.y;

	if (level <= 6) {
		generate(start, end, LONG_FORK_ITERATION, f->ForkVerticies, &tmp);
		f->num_used = 9;
	} else if ((level > 6) && (level <= 11)) {
		generate(start, end, MEDIUM_FORK_ITERATION, f->ForkVerticies, &tmp);
		f->num_used = 5;
	} else {
		if (distance(start, end) > 100) {
			generate(start, end, MEDIUM_FORK_ITERATION, f->ForkVerticies, &tmp);
			f->num_used = 5;
		} else {
			generate(start, end, SMALL_FORK_ITERATION, f->ForkVerticies, &tmp);
			f->num_used = 3;
		}
	}

	f->ForkVerticies[f->num_used - 1].x = end.x;
	f->ForkVerticies[f->num_used - 1].y = end.y;
}

/*------------------------------------------------------------------------*/

static void
update_bolt(bolt, time)
	Lightning  *bolt;
	int         time;

{
	wiggle_bolt(bolt);
	if ((bolt->wiggle_amount == 0) && (bolt->wiggle_number > 2))
		bolt->wiggle_number = 0;
	if (((time % 3) == 0))
		bolt->wiggle_amount++;

	if (((time >= bolt->delay_time) && (time < bolt->flash_begin)) ||
	    (time > bolt->flash_stop))
		bolt->visible = 1;
	else
		bolt->visible = 0;

	if (time == bolt->delay_time)
		bolt->strike_level = FIRST_LEVEL_STRIKE;
	else if (time == (bolt->delay_time + 1))
		bolt->strike_level = LEVEL_ONE_STRIKE;
	else if ((time > (bolt->delay_time + 1)) &&
		 (time <= (bolt->delay_time + bolt->flash_begin - 2)))
		bolt->strike_level = LEVEL_TWO_STRIKE;
	else if (time == (bolt->delay_time + bolt->flash_begin - 1))
		bolt->strike_level = LEVEL_ONE_STRIKE;
	else if (time == (bolt->delay_time + bolt->flash_stop + 1))
		bolt->strike_level = LEVEL_ONE_STRIKE;
	else
		bolt->strike_level = LEVEL_TWO_STRIKE;
}

/*------------------------------------------------------------------------*/

static void
draw_bolt(bolt, mi)
	Lightning  *bolt;
	ModeInfo   *mi;

{
	if (bolt->visible) {
		if (bolt->strike_level == FIRST_LEVEL_STRIKE)
			first_strike(*bolt, mi);
		else if (bolt->strike_level == LEVEL_ONE_STRIKE)
			level1_strike(*bolt, mi);
		else
			level2_strike(*bolt, mi);
	}
}

/*------------------------------------------------------------------------*/

static void
first_strike(bolt, mi)
	Lightning   bolt;
	ModeInfo   *mi;

{
	Display    *display = MI_DISPLAY(mi);
	Window      window = MI_WINDOW(mi);
	GC          gc = MI_GC(mi);
	int         i;

	XSetForeground(display, gc, MI_WIN_WHITE_PIXEL(mi));
	XDrawLine(display, window, gc,
	       bolt.end1.x, bolt.end1.y, bolt.middle[0].x, bolt.middle[0].y);
	draw_line(mi, bolt.middle, BOLT_VERTICIES, gc, 0);
	XDrawLine(display, window, gc,
	bolt.middle[BOLT_VERTICIES - 1].x, bolt.middle[BOLT_VERTICIES - 1].y,
		  bolt.end2.x, bolt.end2.y);

	for (i = 0; i < bolt.fork_number; i++)
		draw_line(mi, bolt.branch[i].ForkVerticies, bolt.branch[i].num_used,
			  gc, 0);
}

/*------------------------------------------------------------------------*/

static void
draw_line(ModeInfo * mi, XPoint * points, int number, GC to_use, int offset)
{
	int         i;

	for (i = 0; i < number - 1; i++) {
		if (points[i].y <= points[i + 1].y)
			XDrawLine(MI_DISPLAY(mi), MI_WINDOW(mi), to_use, points[i].x + offset,
				  points[i].y, points[i + 1].x + offset, points[i + 1].y);
		else {
			if (points[i].x < points[i + 1].x)
				XDrawLine(MI_DISPLAY(mi), MI_WINDOW(mi), to_use, points[i].x +
					  offset, points[i].y + offset, points[i + 1].x + offset,
					  points[i + 1].y + offset);
			else
				XDrawLine(MI_DISPLAY(mi), MI_WINDOW(mi), to_use, points[i].x -
					  offset, points[i].y + offset, points[i + 1].x - offset,
					  points[i + 1].y + offset);
		}
	}
}

/*------------------------------------------------------------------------*/

static void
level1_strike(bolt, mi)
	Lightning   bolt;
	ModeInfo   *mi;

{
	Display    *display = MI_DISPLAY(mi);
	Window      window = MI_WINDOW(mi);
	Storm      *st = &Helga[MI_SCREEN(mi)];
	GC          gc = MI_GC(mi);
	int         i;

	if (MI_NPIXELS(mi) > 2)	/* color */
		XSetForeground(display, gc, MI_PIXEL(mi, st->color));
	else
		XSetForeground(display, gc, MI_WIN_WHITE_PIXEL(mi));
	XDrawLine(display, window, gc,
	bolt.end1.x - 1, bolt.end1.y, bolt.middle[0].x - 1, bolt.middle[0].y);
	draw_line(mi, bolt.middle, BOLT_VERTICIES, gc, -1);
	XDrawLine(display, window, gc,
		  bolt.middle[BOLT_VERTICIES - 1].x - 1,
	    bolt.middle[BOLT_VERTICIES - 1].y, bolt.end2.x - 1, bolt.end2.y);
	XDrawLine(display, window, gc,
	bolt.end1.x + 1, bolt.end1.y, bolt.middle[0].x + 1, bolt.middle[0].y);
	draw_line(mi, bolt.middle, BOLT_VERTICIES, gc, 1);
	XDrawLine(display, window, gc,
		  bolt.middle[BOLT_VERTICIES - 1].x + 1,
	    bolt.middle[BOLT_VERTICIES - 1].y, bolt.end2.x + 1, bolt.end2.y);

	for (i = 0; i < bolt.fork_number; i++) {
		draw_line(mi, bolt.branch[i].ForkVerticies, bolt.branch[i].num_used,
			  gc, -1);
		draw_line(mi, bolt.branch[i].ForkVerticies, bolt.branch[i].num_used,
			  gc, 1);
	}
	first_strike(bolt, mi);
}

/*------------------------------------------------------------------------*/

static int
distance(XPoint a, XPoint b)
{
	return ((int) sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)));
}

/*------------------------------------------------------------------------*/

static void
level2_strike(bolt, mi)
	Lightning   bolt;
	ModeInfo   *mi;

{
	Display    *display = MI_DISPLAY(mi);
	Window      window = MI_WINDOW(mi);
	Storm      *st = &Helga[MI_SCREEN(mi)];
	GC          gc = MI_GC(mi);
	int         i;

	/* This was originally designed to be a little darker then the
	   level1 strike.  This was changed to get it to work on
		 multiscreens and to add more color variety.   I tried
		 stippling but it did not look good. */
	if (MI_NPIXELS(mi) > 2)
		XSetForeground(display, gc, MI_PIXEL(mi, st->color));
	else
		XSetForeground(display, gc, MI_WIN_WHITE_PIXEL(mi));
	XDrawLine(display, window, gc,
	bolt.end1.x - 2, bolt.end1.y, bolt.middle[0].x - 2, bolt.middle[0].y);
	draw_line(mi, bolt.middle, BOLT_VERTICIES, gc, -2);
	XDrawLine(display, window, gc,
		  bolt.middle[BOLT_VERTICIES - 1].x - 2,
	    bolt.middle[BOLT_VERTICIES - 1].y, bolt.end2.x - 2, bolt.end2.y);

	XDrawLine(display, window, gc,
	bolt.end1.x + 2, bolt.end1.y, bolt.middle[0].x + 2, bolt.middle[0].y);
	draw_line(mi, bolt.middle, BOLT_VERTICIES, gc, 2);
	XDrawLine(display, window, gc,
		  bolt.middle[BOLT_VERTICIES - 1].x + 2,
	    bolt.middle[BOLT_VERTICIES - 1].y, bolt.end2.x + 2, bolt.end2.y);

	for (i = 0; i < bolt.fork_number; i++) {
		draw_line(mi, bolt.branch[i].ForkVerticies, bolt.branch[i].num_used,
			  gc, -2);
		draw_line(mi, bolt.branch[i].ForkVerticies, bolt.branch[i].num_used,
			  gc, 2);
	}
	level1_strike(bolt, mi);
}

/*------------------------------------------------------------------------*/

static int
storm_active(st)
	Storm      *st;

{
	int         i, atleast_1 = 0;

	for (i = 0; i < st->multi_strike; i++)
		if (st->bolts[i].wiggle_number > 0)
			atleast_1++;

	return (atleast_1);
}

/*------------------------------------------------------------------------*/

static void
wiggle_bolt(bolt)
	Lightning  *bolt;

{
	int         i;

	wiggle_line(bolt->middle, BOLT_VERTICIES, bolt->wiggle_amount);
	bolt->end2.x += NRAND(bolt->wiggle_amount) - bolt->wiggle_amount / 2;
	bolt->end2.y += NRAND(bolt->wiggle_amount) - bolt->wiggle_amount / 2;

	for (i = 0; i < bolt->fork_number; i++) {
		wiggle_line(bolt->branch[i].ForkVerticies, bolt->branch[i].num_used,
			    bolt->wiggle_amount);
		bolt->branch[i].ForkVerticies[0].x = bolt->middle[bolt->forks_start[i]].x;
		bolt->branch[i].ForkVerticies[0].y = bolt->middle[bolt->forks_start[i]].y;
	}

	if (bolt->wiggle_amount > 1)
		bolt->wiggle_amount -= 1;
	else
		bolt->wiggle_amount = 0;
}

/*------------------------------------------------------------------------*/

static void
wiggle_line(XPoint * p, int number, int amount)
{
	int         i;

	for (i = 0; i < number; i++) {
		p[i].x += NRAND(amount) - amount / 2;
		p[i].y += NRAND(amount) - amount / 2;
	}
}

/*------------------------------------------------------------------------*/

void
init_lightning(mi)
	ModeInfo   *mi;

{
	Storm      *st;

	if (Helga == NULL) {
		if ((Helga = (Storm *) calloc(MI_NUM_SCREENS(mi),
					      sizeof (Storm))) == NULL)
			return;
	}
	st = &Helga[MI_SCREEN(mi)];

	st->scr_width = MI_WIN_WIDTH(mi);
	st->scr_height = MI_WIN_HEIGHT(mi);

	st->multi_strike = setup_multi_strike();
	random_storm(st);
	st->stage = 0;
}

/*------------------------------------------------------------------------*/

void
draw_lightning(mi)
	ModeInfo   *mi;

{
	Storm      *st = &Helga[MI_SCREEN(mi)];
	int         i;

	switch (st->stage) {
		case 0:
			XClearWindow(MI_DISPLAY(mi), MI_WINDOW(mi));
			st->color = NRAND(MI_NPIXELS(mi));
			st->draw_time = 0;
			if (storm_active(st))
				st->stage++;
			else
				st->stage = 3;
			break;
		case 1:
			for (i = 0; i < st->multi_strike; i++) {
				if (st->bolts[i].visible)
					draw_bolt(&(st->bolts[i]), mi);
				update_bolt(&(st->bolts[i]), st->draw_time);
			}
			st->draw_time++;
			XFlush(MI_DISPLAY(mi));
			MI_PAUSE(mi) = 60000;
			st->stage++;
			break;
		case 2:
			XClearWindow(MI_DISPLAY(mi), MI_WINDOW(mi));
			if (storm_active(st))
				st->stage = 1;
			else
				st->stage++;
			break;
		case 3:
			MI_PAUSE(mi) = 1000000;
			init_lightning(mi);
			break;
	}
}

void
release_lightning(ModeInfo * mi)
{
	if (Helga != NULL) {
		(void) free((void *) Helga);
		Helga = NULL;
	}
}
