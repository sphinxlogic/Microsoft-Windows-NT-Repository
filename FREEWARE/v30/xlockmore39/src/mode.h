
#ifndef __XLOCK_MODE_H__
#define __XLOCK_MODE_H__

/*-
 * @(#)mode.h	3.9 96/05/25 xlockmore
 *
 * mode.h - mode management for xlock, the X Window System lockscreen.
 *
 * Copyright (c) 1991 by Patrick J. Naughton.
 *
 * See xlock.c for copying information.
 *
 * Revision History:
 *
 * Changes of David Bagley (bagleyd@hertz.njit.edu)
 * 18-Mar-96: Ron Hitchens <ron@utw.com>
 *		Extensive revision to define new data types for
 *		the new mode calling scheme.
 * 02-Jun-95: Extracted out of resource.c.
 *
 */

/* 
 * Declare external interface routines for supported screen savers.
 */

/* -------------------------------------------------------------------- */

/* 
 * The xlock code attempts to be K&R compatible to support older
 * systems.  But if the compiler advertises ANSI capability by
 * defining __STDC__, we'll declare prototypes here.  This gives us
 * type checking for free of the hook procs in each mode.  Most
 * compilers will type-check properly if the prototype exists,
 * even though the function itself may use the old K&R argument
 * style.
 */

#ifdef __STDC__
struct LockStruct_s;
struct ModeInfo_s;

typedef void (ModeHook) (struct ModeInfo_s *);
typedef void (HookProc) (struct LockStruct_s *, struct ModeInfo_s *);

#else
typedef void (ModeHook) ();
typedef void (HookProc) ();

#endif

typedef struct LockStruct_s {
	char       *cmdline_arg;	/* mode name */
	ModeHook   *init_hook;	/* func to init a mode */
	ModeHook   *callback_hook;	/* func to run (tick) a mode */
	ModeHook   *release_hook;	/* func to shutdown a mode */
	ModeHook   *refresh_hook;	/* tells mode to repaint */
	ModeHook   *change_hook;	/* user wants mode to change */
	ModeHook   *unused_hook;	/* for future expansion */
	ModeSpecOpt *msopt;	/* this mode's def resources */
	int         def_delay;	/* default delay for mode */
	int         def_batchcount;
	int         def_cycles;
	float       def_saturation;
	char       *desc;	/* text description of mode */
	unsigned int flags;	/* state flags for this mode */
	void       *userdata;	/* for use by the mode */
} LockStruct;

#define LS_FLAG_INITED		1

typedef struct {
	Display    *display;	/* handle to X display */
	Screen     *screenptr;	/* ptr to screen info */
	int         screen;	/* number of current screen */
	int         real_screen;	/* for debugging */
	int         num_screens;	/* number screens locked */
	int         max_screens;	/* max # active screens */
	Window      window;	/* handle to current window */
	int         win_width;	/* width of current window */
	int         win_height;	/* height of current window */
	int         win_depth;	/* depth of current window */
	Visual     *visual;	/* visual of current window */
	unsigned long black_pixel;	/* pixel value for black */
	unsigned long white_pixel;	/* pixel value for white */
	unsigned int flags;	/* xlock window flags */
} WindowInfo;

#define WI_FLAG_INFO_INITTED	0x001	/* private state flag */
#define WI_FLAG_ICONIC		0x002
#define WI_FLAG_MONO		0x004
#define WI_FLAG_INWINDOW	0x008
#define WI_FLAG_INROOT		0x010
#define WI_FLAG_NOLOCK	0x020
#define WI_FLAG_INSTALL		0x040
#define WI_FLAG_USE3D		0x080
#define WI_FLAG_VERBOSE		0x100
#define WI_FLAG_JUST_INITTED	0x200	/* private state flag */

typedef struct {
	float       delta3d;
	long        pause;	/* output, set by mode */
	long        delay;	/* inputs, current settings */
	long        batchcount;
	long        cycles;
	float       saturation;
} RunInfo;

typedef struct ModeInfo_s {
	WindowInfo  windowinfo;
	perscreen  *screeninfo;
	RunInfo     runinfo;
	struct LockStruct_s *lockstruct;
} ModeInfo;

/* -------------------------------------------------------------------- */

/* 
 * These are the public interfaces that a mode should use to obtain
 * information about the display and other environmental parameters.
 * Everything hangs off a ModeInfo pointer.  A mode should NOT cache
 * a ModeInfo pointer, the struct it points to is volatile.  The mode
 * can safely make a copy of the data it points to, however.  But it
 * is recommended the mode make use of the passed-in pointer and pass
 * it along to functions it calls.
 * Use these macros, don't look at the fields directly.  The insides
 * of the ModeInfo struct are certain to change in the future.
 */

#define MODE_IS_INITED(ls)	((ls)->flags & LS_FLAG_INITED)
#define MODE_NOT_INITED(ls)	( ! MODE_IS_INITED(ls))

#define MI_DISPLAY(mi)		((mi)->windowinfo.display)
#define MI_SCREEN(mi)		((mi)->windowinfo.screen)
#define MI_SCREENPTR(mi)      ((mi)->windowinfo.screenptr)
#define MI_REAL_SCREEN(mi)	((mi)->windowinfo.real_screen)
#define MI_NUM_SCREENS(mi)	((mi)->windowinfo.num_screens)
#define MI_MAX_SCREENS(mi)	((mi)->windowinfo.max_screens)
#define MI_WINDOW(mi)		((mi)->windowinfo.window)
#define MI_WIN_WIDTH(mi)	((mi)->windowinfo.win_width)
#define MI_WIN_HEIGHT(mi)	((mi)->windowinfo.win_height)
#define MI_WIN_DEPTH(mi)	((mi)->windowinfo.win_depth)
#define MI_VISUAL(mi)	((mi)->windowinfo.visual)
#define MI_WIN_BLACK_PIXEL(mi)	((mi)->windowinfo.black_pixel)
#define MI_WIN_WHITE_PIXEL(mi)	((mi)->windowinfo.white_pixel)
#define MI_WIN_FLAGS(mi)	((mi)->windowinfo.flags)
#define MI_WIN_SET_FLAG_STATE(mi,f,bool) ((mi)->windowinfo.flags = \
					(bool) ? (mi)->windowinfo.flags | f \
					: (mi)->windowinfo.flags & ~(f))
#define MI_WIN_FLAG_IS_SET(mi,f) ((mi)->windowinfo.flags & f)
#define MI_WIN_FLAG_NOT_SET(mi,f) ( ! MI_WIN_FLAG_IS_SET(mi,f))
#define MI_WIN_IS_ICONIC(mi)	(MI_WIN_FLAG_IS_SET (mi, WI_FLAG_ICONIC))
#define MI_WIN_IS_MONO(mi)	(MI_WIN_FLAG_IS_SET (mi, WI_FLAG_MONO))
#define MI_WIN_IS_INWINDOW(mi)	(MI_WIN_FLAG_IS_SET (mi, WI_FLAG_INWINDOW))
#define MI_WIN_IS_INROOT(mi)	(MI_WIN_FLAG_IS_SET (mi, WI_FLAG_INROOT))
#define MI_WIN_IS_NOLOCK(mi)	(MI_WIN_FLAG_IS_SET (mi, WI_FLAG_NOLOCK))
#define MI_WIN_IS_INSTALL(mi)	(MI_WIN_FLAG_IS_SET (mi, WI_FLAG_INSTALL))
#define MI_WIN_IS_USE3D(mi)	(MI_WIN_FLAG_IS_SET (mi, WI_FLAG_USE3D))
#define MI_WIN_IS_VERBOSE(mi)	(MI_WIN_FLAG_IS_SET (mi, WI_FLAG_VERBOSE))

#define MI_PERSCREEN(mi)	((mi)->screeninfo)
#define MI_GC(mi)		((mi)->screeninfo->gc)
#define MI_NPIXELS(mi)		((mi)->screeninfo->npixels)
#define MI_CMAP(mi)		((mi)->screeninfo->cmap)
#define MI_PIXELS(mi)		((mi)->screeninfo->pixels)
#define MI_PIXEL(mi,n)		((mi)->screeninfo->pixels[n])

/* are these of interest to modes? */
#define MI_BG_COLOR(mi)		((mi)->screeninfo->bgcol)
#define MI_FG_COLOR(mi)		((mi)->screeninfo->fgcol)
#define MI_RIGHT_COLOR(mi)	((mi)->screeninfo->rightcol)
#define MI_LEFT_COLOR(mi)	((mi)->screeninfo->leftcol)

#define MI_DELTA3D(mi)	((mi)->runinfo.delta3d)
#define MI_PAUSE(mi)		((mi)->runinfo.pause)
#define MI_DELAY(mi)		((mi)->runinfo.delay)
#define MI_BATCHCOUNT(mi)	((mi)->runinfo.batchcount)
#define MI_CYCLES(mi)		((mi)->runinfo.cycles)
#define MI_SATURATION(mi)	((mi)->runinfo.saturation)

#define MI_LOCKSTRUCT(mi)	((mi)->lockstruct)
#define MI_DEFDELAY(mi)		((mi)->lockstruct->def_delay)
#define MI_DEFBATCHCOUNT(mi)	((mi)->lockstruct->def_batchcount)
#define MI_DEFCYCLES(mi)	((mi)->lockstruct->def_cycles)
#define MI_DEFSATURATION(mi)	((mi)->lockstruct->def_saturation)

#define MI_NAME(mi)		((mi)->lockstruct->cmdline_arg)
#define MI_DESC(mi)		((mi)->lockstruct->desc)
#define MI_USERDATA(mi)		((mi)->lockstruct->userdata)

/* -------------------------------------------------------------------- */

extern HookProc call_init_hook;
extern HookProc call_callback_hook;
extern HookProc call_refresh_hook;
extern HookProc call_change_hook;

#ifdef __STDC__
extern void set_default_mode(LockStruct *);
extern void release_last_mode(ModeInfo *);

#else
extern void set_default_mode();
extern void release_last_mode();

#endif

/* -------------------------------------------------------------------- */

extern ModeHook init_ant;
extern ModeHook draw_ant;
extern ModeHook release_ant;
extern ModeSpecOpt ant_opts;

extern ModeHook init_bat;
extern ModeHook draw_bat;
extern ModeSpecOpt bat_opts;

extern ModeHook init_bouboule;
extern ModeHook draw_bouboule;
extern ModeSpecOpt bouboule_opts;

extern ModeHook init_bounce;
extern ModeHook draw_bounce;
extern ModeSpecOpt bounce_opts;

extern ModeHook init_braid;
extern ModeHook draw_braid;
extern ModeSpecOpt braid_opts;

extern ModeHook init_blot;
extern ModeHook draw_blot;
extern ModeSpecOpt blot_opts;

extern ModeHook init_bug;
extern ModeHook draw_bug;
extern ModeSpecOpt bug_opts;

extern ModeHook init_clock;
extern ModeHook draw_clock;
extern ModeSpecOpt clock_opts;

extern ModeHook init_demon;
extern ModeHook draw_demon;
extern ModeHook release_demon;
extern ModeSpecOpt demon_opts;

extern ModeHook init_eyes;
extern ModeHook draw_eyes;
extern ModeHook release_eyes;
extern ModeHook refresh_eyes;
extern ModeSpecOpt eyes_opts;

extern ModeHook init_flag;
extern ModeHook draw_flag;
extern ModeHook release_flag;
extern ModeSpecOpt flag_opts;

extern ModeHook init_flame;
extern ModeHook draw_flame;
extern ModeSpecOpt flame_opts;

extern ModeHook init_forest;
extern ModeHook draw_forest;
extern ModeSpecOpt forest_opts;

extern ModeHook init_galaxy;
extern ModeHook draw_galaxy;
extern ModeSpecOpt galaxy_opts;

extern ModeHook init_geometry;
extern ModeHook draw_geometry;
extern ModeSpecOpt geometry_opts;

extern ModeHook init_grav;
extern ModeHook draw_grav;
extern ModeSpecOpt grav_opts;

extern ModeHook init_helix;
extern ModeHook draw_helix;
extern ModeSpecOpt helix_opts;

extern ModeHook init_hop;
extern ModeHook draw_hop;
extern ModeSpecOpt hop_opts;

extern ModeHook init_hyper;
extern ModeHook draw_hyper;
extern ModeSpecOpt hyper_opts;

extern ModeHook init_image;
extern ModeHook draw_image;
extern ModeSpecOpt image_opts;

extern ModeHook init_kaleid;
extern ModeHook draw_kaleid;
extern ModeSpecOpt kaleid_opts;

extern ModeHook init_laser;
extern ModeHook draw_laser;
extern ModeSpecOpt laser_opts;

extern ModeHook init_life;
extern ModeHook draw_life;
extern ModeSpecOpt life_opts;

extern ModeHook init_life1d;
extern ModeHook draw_life1d;
extern ModeSpecOpt life1d_opts;

extern ModeHook init_life3d;
extern ModeHook draw_life3d;
extern ModeSpecOpt life3d_opts;

extern ModeHook init_lissie;
extern ModeHook draw_lissie;
extern ModeSpecOpt lissie_opts;

extern ModeHook init_marquee;
extern ModeHook draw_marquee;
extern ModeSpecOpt marquee_opts;

extern ModeHook init_maze;
extern ModeHook draw_maze;
extern ModeSpecOpt maze_opts;

extern ModeHook init_mountain;
extern ModeHook draw_mountain;
extern ModeSpecOpt mountain_opts;

extern ModeHook init_nose;
extern ModeHook draw_nose;
extern ModeSpecOpt nose_opts;

extern ModeHook init_petal;
extern ModeHook draw_petal;
extern ModeSpecOpt petal_opts;

extern ModeHook init_puzzle;
extern ModeHook draw_puzzle;
extern ModeSpecOpt puzzle_opts;

extern ModeHook init_pyro;
extern ModeHook draw_pyro;
extern ModeSpecOpt pyro_opts;

extern ModeHook init_qix;
extern ModeHook draw_qix;
extern ModeSpecOpt qix_opts;

extern ModeHook init_rock;
extern ModeHook draw_rock;
extern ModeSpecOpt rock_opts;

extern ModeHook init_rotor;
extern ModeHook draw_rotor;
extern ModeSpecOpt rotor_opts;

extern ModeHook init_shape;
extern ModeHook draw_shape;
extern ModeSpecOpt shape_opts;

extern ModeHook init_slip;
extern ModeHook draw_slip;
extern ModeSpecOpt slip_opts;

extern ModeHook init_sphere;
extern ModeHook draw_sphere;
extern ModeSpecOpt sphere_opts;

extern ModeHook init_spiral;
extern ModeHook draw_spiral;
extern ModeSpecOpt spiral_opts;

extern ModeHook init_spline;
extern ModeHook draw_spline;
extern ModeSpecOpt spline_opts;

extern ModeHook init_swarm;
extern ModeHook draw_swarm;
extern ModeSpecOpt swarm_opts;

extern ModeHook init_swirl;
extern ModeHook draw_swirl;
extern ModeSpecOpt swirl_opts;

extern ModeHook init_triangle;
extern ModeHook draw_triangle;
extern ModeSpecOpt triangle_opts;

extern ModeHook init_wator;
extern ModeHook draw_wator;
extern ModeSpecOpt wator_opts;

extern ModeHook init_world;
extern ModeHook draw_world;
extern ModeSpecOpt world_opts;

extern ModeHook init_worm;
extern ModeHook draw_worm;
extern ModeSpecOpt worm_opts;

extern ModeHook init_blank;
extern ModeHook draw_blank;
extern ModeHook release_blank;
extern ModeSpecOpt blank_opts;

extern ModeHook init_random;
extern ModeHook draw_random;
extern ModeHook refresh_random;
extern ModeHook change_random;
extern ModeSpecOpt random_opts;

extern LockStruct LockProcs[];
extern int  numprocs;

/* -------------------------------------------------------------------- */

#endif /* __XLOCK_MODE_H__ */
