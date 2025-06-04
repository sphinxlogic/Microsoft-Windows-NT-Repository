/*
 *   libdi - CD Audio Player Device Interface Library
 *
 *   Copyright (C) 1993-1998  Ti Kan
 *   E-mail: ti@amb.org
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the Free
 *   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef __LIBDI_H__
#define __LIBDI_H__

#ifndef LINT
static char *_libdi_h_ident_ = "@(#)libdi.h	6.38 98/05/03";
#endif

/* Max number of libdi modules */
#define MAX_METHODS	4

/*
 * Supported libdi methods
 *
 * Comment out any of these to remove support for a method.
 * Removing unused methods can reduce the executable
 * binary size and run-time memory usage.  At least one
 * method must be defined.
 *
 * Note: If compiling for DEMO_ONLY or on a non-supported OS
 * platform, DI_SCSIPT must be defined.
 */
#if !defined(__QNX__)
#define DI_SCSIPT	0	/* SCSI pass-through method */
#endif
#if defined(linux) || defined(sun) || defined(__sun__) || defined(__QNX__)
#define DI_SLIOC	1	/* SunOS/Solaris/Linux/QNX ioctl method */
#endif
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#define DI_FBIOC	2	/* FreeBSD/NetBSD/OpenBSD ioctl method */
#endif
#if defined(_AIX) && defined(AIX_IDE)
#define DI_AIXIOC	3	/* AIX IDE ioctl method */
#endif


/*
 * Multi-CD medium change methods
 */
#define MAX_CHG_METHODS	4

#define CHG_NONE	0	/* Changer controls not supported */
#define CHG_SCSI_LUN	1	/* SCSI LUN addressing method */
#define CHG_SCSI_MEDCHG	2	/* SCSI Medium changer device method */
#define CHG_OS_IOCTL	3	/* OS-ioctl method */


/* Application name */
#define APPNAME		"CD audio"


/* Play audio format codes */
#define ADDR_BLK	0x01	/* block address specified */
#define ADDR_MSF	0x02	/* MSF address specified */
#define ADDR_TRKIDX	0x04	/* track/index numbers specified */
#define ADDR_OPTEND	0x80	/* End address can be ignored */


/* Slider control flags */
#define WARP_VOL	0x1	/* Set volume slider thumb */
#define WARP_BAL	0x2	/* Set balance slider thumb */


/* Misc constants */
#define MAX_SRCH_BLKS	225	/* max search play blks per sample */
#define MAX_RECOVERR	20	/* Max number of err recovery tries */
#define ERR_SKIPBLKS	10	/* Number of frame to skip on error */
#define ERR_CLRTHRESH	1500	/* If there hasn't been any errors
				 * for this many blocks of audio
				 * playback, then the previous errors
				 * count is cleared.
				 */


/* CDDB/Program macros */
#define DBCLEAR(s, b)		{					\
	if (di_clinfo != NULL && di_clinfo->dbclear != NULL)		\
		di_clinfo->dbclear((s), (b));				\
}
#define DBGET(s)		{					\
	if (di_clinfo != NULL && di_clinfo->dbget != NULL)		\
		di_clinfo->dbget(s);					\
}
#define PROGCLEAR(s)		{					\
	if (di_clinfo != NULL && di_clinfo->progclear != NULL)		\
		di_clinfo->progclear(s);				\
}


/* Message dialog macros */
#define DI_FATAL(msg)		{					\
	if (di_clinfo != NULL && di_clinfo->fatal_msg != NULL)		\
		di_clinfo->fatal_msg(app_data.str_fatal, (msg));	\
	else {								\
		(void) fprintf(errfp, "Fatal: %s\n", (msg));		\
		exit(1);						\
	}								\
}
#define DI_WARNING(msg)		{					\
	if (di_clinfo != NULL && di_clinfo->warning_msg != NULL)	\
		di_clinfo->warning_msg(app_data.str_warning, (msg));	\
	else								\
		(void) fprintf(errfp, "Warning: %s\n", (msg));		\
}
#define DI_INFO(msg)		{					\
	if (di_clinfo != NULL && di_clinfo->info_msg != NULL)		\
		di_clinfo->info_msg(app_data.str_info, (msg));		\
	else								\
		(void) fprintf(errfp, "Info: %s\n", (msg));		\
}


/* GUI macros */
#define DO_BEEP()		{					\
	if (di_clinfo != NULL && di_clinfo->beep != NULL)		\
		di_clinfo->beep();					\
}
#define SET_LOCK_BTN(state)	{					\
	if (di_clinfo != NULL && di_clinfo->set_lock_btn != NULL)	\
		di_clinfo->set_lock_btn(state);				\
}
#define SET_SHUFFLE_BTN(state)	{					\
	if (di_clinfo != NULL && di_clinfo->set_shuffle_btn != NULL)	\
		di_clinfo->set_shuffle_btn(state);			\
}
#define SET_VOL_SLIDER(val)	{					\
	if (di_clinfo != NULL && di_clinfo->set_vol_slider != NULL)	\
		di_clinfo->set_vol_slider(val);				\
}
#define SET_BAL_SLIDER(val)	{					\
	if (di_clinfo != NULL && di_clinfo->set_bal_slider != NULL)	\
		di_clinfo->set_bal_slider(val);				\
}
#define DPY_ALL(s)		{					\
	if (di_clinfo != NULL && di_clinfo->dpy_all != NULL)		\
		di_clinfo->dpy_all(s);					\
}
#define DPY_DISC(s)		{					\
	if (di_clinfo != NULL && di_clinfo->dpy_disc != NULL)		\
		di_clinfo->dpy_disc(s);				\
}
#define DPY_TRACK(s)		{					\
	if (di_clinfo != NULL && di_clinfo->dpy_track != NULL)		\
		di_clinfo->dpy_track(s);				\
}
#define DPY_INDEX(s)		{					\
	if (di_clinfo != NULL && di_clinfo->dpy_index != NULL)		\
		di_clinfo->dpy_index(s);				\
}
#define DPY_TIME(s, b)		{					\
	if (di_clinfo != NULL && di_clinfo->dpy_time != NULL)		\
		di_clinfo->dpy_time((s), (b));				\
}
#define DPY_PLAYMODE(s, b)		{				\
	if (di_clinfo != NULL && di_clinfo->dpy_playmode != NULL)	\
		di_clinfo->dpy_playmode((s), (b));			\
}
#define DPY_RPTCNT(s)		{					\
	if (di_clinfo != NULL && di_clinfo->dpy_rptcnt != NULL)		\
		di_clinfo->dpy_rptcnt(s);				\
}


/* CD position MSF structure */
typedef struct msf {
	byte_t		res;		/* reserved */
	byte_t		min;		/* minutes */
	byte_t		sec;		/* seconds */
	byte_t		frame;		/* frame */
} msf_t;


/* Combined MSF and logical address union */
typedef union lmsf {
	msf_t		msf;		/* MSF address */
	word32_t	logical;	/* logical address */
} lmsf_t;


/*
 * Jump table structure for libdi interface
 */
typedef struct {
	bool_t		(*check_disc)(curstat_t *);
	void		(*status_upd)(curstat_t *);
	void		(*lock)(curstat_t *, bool_t);
	void		(*repeat)(curstat_t *, bool_t);
	void		(*shuffle)(curstat_t *, bool_t);
	void		(*load_eject)(curstat_t *);
	void		(*ab)(curstat_t *);
	void		(*sample)(curstat_t *);
	void		(*level)(curstat_t *, byte_t, bool_t);
	void		(*play_pause)(curstat_t *);
	void		(*stop)(curstat_t *, bool_t);
	void		(*chgdisc)(curstat_t *);
	void		(*prevtrk)(curstat_t *);
	void		(*nexttrk)(curstat_t *);
	void		(*previdx)(curstat_t *);
	void		(*nextidx)(curstat_t *);
	void		(*rew)(curstat_t *, bool_t);
	void		(*ff)(curstat_t *, bool_t);
	void		(*warp)(curstat_t *);
	void		(*route)(curstat_t *);
	void		(*mute_on)(curstat_t *);
	void		(*mute_off)(curstat_t *);
	void		(*start)(curstat_t *);
	void		(*icon)(curstat_t *, bool_t);
	void		(*halt)(curstat_t *);
	char *		(*mode)(void);
	char *		(*vers)(void);
} di_tbl_t;


/*
 * Jump table for libdi initialization
 */
typedef struct {
	void	(*init)(curstat_t *, di_tbl_t *);
} diinit_tbl_t;


/*
 * Callbacks into the application
 */
typedef struct {
	curstat_t *	(*curstat_addr)(void);			/* Required */
	void		(*quit)(curstat_t *);			/* Required */
	long		(*timeout)(word32_t, void (*)(), byte_t *);
	void		(*untimeout)(long);
	void		(*dbclear)(curstat_t *, bool_t);
	void		(*dbget)(curstat_t *);
	void		(*progclear)(curstat_t *);
	void		(*fatal_msg)(char *, char *);
	void		(*warning_msg)(char *, char *);
	void		(*info_msg)(char *, char *);
	void		(*beep)(void);
	void		(*set_lock_btn)(bool_t);
	void		(*set_shuffle_btn)(bool_t);
	void		(*set_vol_slider)(int);
	void		(*set_bal_slider)(int);
	void		(*dpy_all)(curstat_t *);
	void		(*dpy_disc)(curstat_t *);
	void		(*dpy_track)(curstat_t *);
	void		(*dpy_index)(curstat_t *);
	void		(*dpy_time)(curstat_t *, bool_t);
	void		(*dpy_playmode)(curstat_t *, bool_t);
	void		(*dpy_rptcnt)(curstat_t *);
} di_client_t;


/*
 * Device path registry structure
 */
typedef struct di_devreg {
	int		fd;		/* file descriptor */
	char		*path;		/* path name string */
	struct di_devreg *next;		/* link list pointer */
} di_devreg_t;


/*
 * Public function prototypes: libdi external calling interface.
 */
extern void	di_init(di_client_t *);
extern bool_t	di_check_disc(curstat_t *);
extern void	di_status_upd(curstat_t *);
extern void	di_lock(curstat_t *, bool_t);
extern void	di_repeat(curstat_t *, bool_t);
extern void	di_shuffle(curstat_t *, bool_t);
extern void	di_load_eject(curstat_t *);
extern void	di_ab(curstat_t *);
extern void	di_sample(curstat_t *);
extern void	di_level(curstat_t *, byte_t, bool_t);
extern void	di_play_pause(curstat_t *);
extern void	di_stop(curstat_t *, bool_t);
extern void	di_chgdisc(curstat_t *);
extern void	di_prevtrk(curstat_t *);
extern void	di_nexttrk(curstat_t *);
extern void	di_previdx(curstat_t *);
extern void	di_nextidx(curstat_t *);
extern void	di_rew(curstat_t *, bool_t);
extern void	di_ff(curstat_t *, bool_t);
extern void	di_warp(curstat_t *);
extern void	di_route(curstat_t *);
extern void	di_mute_on(curstat_t *);
extern void	di_mute_off(curstat_t *);
extern void	di_start(curstat_t *);
extern void	di_icon(curstat_t *, bool_t);
extern void	di_halt(curstat_t *);
extern void	di_dump_curstat(curstat_t *);
extern void	di_common_parmload(char *, bool_t);
extern void	di_devspec_parmload(char *, bool_t);
extern void	di_devspec_parmsave(char *);
extern bool_t	di_devlock(curstat_t *, char *);
extern void	di_devunlock(curstat_t *);
extern bool_t	di_devreg(int, char *);
extern void	di_devunreg(int);
extern char	*di_devgetpath(int);
extern char	*di_mode(void);
extern char	*di_vers(void);
extern bool_t	di_isdemo(void);
extern int	di_curtrk_pos(curstat_t *);
extern int	di_curprog_pos(curstat_t *);
extern void	di_reset_curstat(curstat_t *, bool_t, bool_t);
extern void	di_reset_shuffle(curstat_t *);
extern int	di_taper_vol(int);
extern int	di_untaper_vol(int);
extern int	di_scale_vol(int);
extern int	di_unscale_vol(int);


#endif	/* __LIBDI_H__ */

