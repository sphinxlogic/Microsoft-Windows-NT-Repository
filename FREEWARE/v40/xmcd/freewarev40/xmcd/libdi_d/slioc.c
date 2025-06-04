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

/*
 * SunOS/Solaris/Linux/QNX ioctl method module
 *
 * Contributing author: Peter Bauer
 * E-mail: 100136.3530@compuserve.com
 *
 * QNX support added by: D. J. Hawkey Jr. (hints provided by W. A. Flowers)
 * E-mail: hawkeyd@visi.com
 */

#ifndef LINT
static char *_slioc_c_ident_ = "@(#)slioc.c	6.117 98/10/27";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/slioc.h"

#if defined(DI_SLIOC) && !defined(DEMO_ONLY)

extern appdata_t	app_data;
extern FILE		*errfp;
extern di_client_t	*di_clinfo;


STATIC bool_t	slioc_run_ab(curstat_t *),
		slioc_run_sample(curstat_t *),
		slioc_run_prog(curstat_t *),
		slioc_run_repeat(curstat_t *),
		slioc_disc_ready(curstat_t *),
		slioc_disc_present(bool_t),
		slioc_open(curstat_t *);
STATIC void	slioc_stat_poll(curstat_t *),
		slioc_insert_poll(curstat_t *),
		slioc_close(void);


STATIC int	slioc_fd = -1,			/* CD-ROM file descriptor */
		slioc_stat_interval,		/* Status poll interval */
		slioc_ins_interval;		/* Insert poll interval */
STATIC long	slioc_stat_id,			/* Play status poll timer id */
		slioc_insert_id,		/* Disc insert poll timer id */
		slioc_search_id;		/* FF/REW timer id */
STATIC byte_t	slioc_tst_status = MOD_NODISC,	/* Playback status on load */
		slioc_next_sam;			/* Next SAMPLE track */
STATIC bool_t	slioc_not_open = TRUE,		/* Device not opened yet */
		slioc_stat_polling,		/* Polling play status */
		slioc_insert_polling,		/* Polling disc insert */
		slioc_new_progshuf,		/* New program/shuffle seq */
		slioc_start_search,		/* Start FF/REW play segment */
		slioc_idx_pause,		/* Prev/next index pausing */
		slioc_fake_stop,		/* Force a completion status */
		slioc_playing,			/* Currently playing */
		slioc_bcd_hack,			/* Track numbers in BCD hack */
		slioc_mult_autoplay,		/* Auto-play after disc chg */
		slioc_mult_pause,		/* Pause after disc chg */
		slioc_override_ap;		/* Override auto-play */
STATIC word32_t	slioc_ab_start_addr,		/* A->B mode start block */
		slioc_ab_end_addr,		/* A->B mode end block */
		slioc_sav_end_addr;		/* Err recov saved end addr */
STATIC msf_t	slioc_ab_start_msf,		/* A->B mode start MSF */
		slioc_ab_end_msf,		/* A->B mode end MSF */
		slioc_sav_end_msf;		/* Err recov saved end MSF */
STATIC byte_t	slioc_sav_end_fmt;		/* Err recov saved end fmt */


/* SunOS/Solaris/Linux CDROM ioctl names */
STATIC iocname_t iname[] = {
	{ CDROMSUBCHNL,		"CDROMSUBCHNL"		},
	{ CDROMREADTOCHDR,	"CDROMREADTOCHDR"	},
	{ CDROMREADTOCENTRY,	"CDROMREADTOCENTRY"	},
	{ CDROMEJECT,		"CDROMEJECT"		},
	{ CDROMSTART,		"CDROMSTART"		},
	{ CDROMSTOP,		"CDROMSTOP"		},
	{ CDROMPAUSE,		"CDROMPAUSE"		},
	{ CDROMRESUME,		"CDROMRESUME"		},
	{ CDROMVOLCTRL,		"CDROMVOLCTRL"		},
	{ CDROMPLAYTRKIND,	"CDROMPLAYTRKIND"	},
	{ CDROMPLAYMSF,		"CDROMPLAYMSF"		},
#ifdef linux
	{ CDROMCLOSETRAY,	"CDROMCLOSETRAY"	},
	{ CDROM_CHANGER_NSLOTS,	"CDROM_CHANGER_NSLOTS"	},
	{ CDROM_SELECT_DISC,	"CDROM_SELECT_DISC"	},
	{ CDROM_DRIVE_STATUS,	"CDROM_DRIVE_STATUS"	},
	{ CDROM_MEDIA_CHANGED,	"CDROM_MEDIA_CHANGED"	},
#endif
	{ 0,			NULL			},
};



/***********************
 *  internal routines  *
 ***********************/

#ifdef SOL2_VOLMGT
/*
 * slioc_vold_alrm
 *	SIGALRM handler
 *
 * Args:
 *	signo - The signal number
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
slioc_vold_alrm(int signo)
{
	DBGPRN(errfp, "\nSIGALRM!\n");
}
#endif


/*
 * slioc_send
 *	Issue ioctl command.
 *
 * Args:
 *	cmd - ioctl command
 *	arg - ioctl argument
 *	arglen - sizeof(*arg)
 *	prnerr - whether an error message is to be displayed if the ioctl fails
 *	retloc - Pointer to storage where the return value of the ioctl can be
 *		 stored, or NULL.
 *
 * Return:
 *	TRUE - ioctl successful
 *	FALSE - ioctl failed
 */
STATIC bool_t
slioc_send(int cmd, void *arg, int arglen, bool_t prnerr, int *retloc)
{
	int		i,
			ret,
			saverrno;
	curstat_t	*s = di_clinfo->curstat_addr();

	if (slioc_fd < 0)
		return FALSE;

	if (app_data.debug) {
		for (i = 0; iname[i].name != NULL; i++) {
			if (iname[i].cmd == cmd) {
				(void) fprintf(errfp, "\nIOCTL: %s arg=0x%x ",
					       iname[i].name, (int) arg);
				break;
			}
		}
		if (iname[i].name == NULL)
			(void) fprintf(errfp, "\nIOCTL: 0x%x arg=0x%x ",
				       cmd, (int) arg);
	}

#ifdef SOL2_VOLMGT
	if (app_data.sol2_volmgt) {
		(void) signal(SIGALRM, slioc_vold_alrm);
		(void) alarm(15);
	}
#endif

#ifdef __QNX__
	switch (cmd) {
	case CDROMVOLCTRL:
	case CDROMPLAYTRKIND:
#ifdef CDROMCLOSETRAY
	case CDROMCLOSETRAY:
#endif
		ret = ioctl(slioc_fd, cmd, arg);
		break;
	default:
		/* Use proprietary QNX ioctl */
		ret = qnx_ioctl(slioc_fd, cmd, arg, arglen, arg, arglen);
		break;
	}
#else
	ret = ioctl(slioc_fd, cmd, arg);
#endif

	DBGPRN(errfp, "ret=%d\n", ret);

	if (ret < 0) {
		if (prnerr) {
			saverrno = errno;

			(void) fprintf(errfp, "CD audio: ioctl error on %s: ",
				       s->curdev);

			for (i = 0; iname[i].name != NULL; i++) {
				if (iname[i].cmd == cmd) {
					(void) fprintf(errfp,
						       "cmd=%s errno=%d\n",
						       iname[i].name, errno);
					break;
				}
			}
			if (iname[i].name == NULL)
				(void) fprintf(errfp, "cmd=0x%x errno=%d\n",
					       cmd, errno);

			errno = saverrno;
		}

#ifdef SOL2_VOLMGT
		if (app_data.sol2_volmgt) {
			saverrno = errno;
			(void) alarm(0);
			(void) signal(SIGALRM, SIG_DFL);
			errno = saverrno;
		}
#endif

		return FALSE;
	}

#ifdef SOL2_VOLMGT
	if (app_data.sol2_volmgt) {
		(void) alarm(0);
		(void) signal(SIGALRM, SIG_DFL);
	}
#endif

	if (retloc != NULL)
		*retloc = ret;

	return TRUE;
}


/*
 * slioc_open
 *	Open CD-ROM device
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - open successful
 *	FALSE - open failed
 */
STATIC bool_t
slioc_open(curstat_t *s)
{
	struct stat	stbuf;
	char		errstr[ERR_BUF_SZ];
	int		i,
			openflgs,
			nslots,
			curslot,
			ret;
	static bool_t	first = TRUE;

	/* Check for validity of device node */
	if (stat(s->curdev, &stbuf) < 0) {
#ifdef SOL2_VOLMGT
		if (app_data.sol2_volmgt) {
			switch (errno) {
			case ENOENT:
			case EINTR:
			case ESTALE:
				return FALSE;
				/*NOTREACHED*/
			default:
				break;
			}
		}
#endif
		(void) sprintf(errstr, app_data.str_staterr, s->curdev);
		DI_FATAL(errstr);
		return FALSE;
	}

#if defined(linux) || defined(__QNX__)
	/* Linux and QNX CD-ROM devices are block special file! */
	if (!S_ISBLK(stbuf.st_mode))
#else
	if (!S_ISCHR(stbuf.st_mode))
#endif
	{
#ifdef SOL2_VOLMGT
		/* Some CDs have multiple slices (partitions),
		 * so the device node becomes a directory when
		 * vold mounts each slice.
		 */
		if (app_data.sol2_volmgt && S_ISDIR(stbuf.st_mode))
			return FALSE;
#endif
		(void) sprintf(errstr, app_data.str_noderr, s->curdev);
		DI_FATAL(errstr);
		return FALSE;
	}

#ifdef linux
	openflgs = O_RDONLY | O_EXCL | O_NONBLOCK;
#else
	openflgs = O_RDONLY;
#endif

#ifdef SOL2_VOLMGT
	if (app_data.sol2_volmgt) {
		(void) signal(SIGALRM, slioc_vold_alrm);
		(void) alarm(10);
	}
#endif

	if ((slioc_fd = open(s->curdev, openflgs)) < 0) {
		DBGPRN(errfp, "Cannot open %s: errno=%d\n", s->curdev, errno);
#ifdef SOL2_VOLMGT
		if (app_data.sol2_volmgt) {
			(void) alarm(0);
			(void) signal(SIGALRM, SIG_DFL);
		}
#endif
		return FALSE;
	}

#ifdef SOL2_VOLMGT
	if (app_data.sol2_volmgt) {
		(void) alarm(0);
		(void) signal(SIGALRM, SIG_DFL);
	}
#endif

#ifdef linux
	/* CD changer setup */
	if (app_data.numdiscs > 1 && first) {
		first = FALSE;

		/* Find out how many discs the drive says it supports.
		 * Note: Only works on Linux 2.1.x and later.
		 */
		if (slioc_send(CDROM_CHANGER_NSLOTS, NULL, 0,
				app_data.debug, &nslots)) {
			if (nslots != app_data.numdiscs) {
				DBGPRN(errfp,
					"CD changer: Setting numdiscs to %d\n",
					nslots);
				app_data.numdiscs = s->last_disc = nslots;
			}
		}

		if (app_data.numdiscs == 1) {
			/* Not a CD changer:
			 * Set to single-disc mode.
			 */
			app_data.numdiscs = 1;
			app_data.chg_method = CHG_NONE;
			app_data.multi_play = FALSE;
			app_data.reverse = FALSE;
			s->first_disc = s->last_disc = s->cur_disc = 1;

			DBGPRN(errfp, "Not a CD changer: %s\n",
				"Setting to single disc mode");

			DI_INFO(app_data.str_medchg_noinit);
		}
	}

	if (app_data.numdiscs > 1) {
		/* Find out which disc is loaded.
		 * Note: Only works on Linux 2.1.x and later.
		 */
		if (slioc_send(CDROM_SELECT_DISC, (void *) CDSL_CURRENT,
				0, app_data.debug, &curslot)) {
			DBGPRN(errfp, "Current disc=%d\n", curslot + 1);
		}
		else {
			switch (errno) {
			case EINVAL:
				/* Cannot query current disc:
				 * Set to single-disc mode.
				 */
				app_data.numdiscs = 1;
				app_data.chg_method = CHG_NONE;
				app_data.multi_play = FALSE;
				app_data.reverse = FALSE;
				s->first_disc = s->last_disc = s->cur_disc = 1;

				DBGPRN(errfp,
					"Cannot determine current disc: %s\n",
					"Setting to single disc mode");

				DI_INFO(app_data.str_medchg_noinit);
				break;

			case ENOENT:
				/* No disc in current slot */
				DBGPRN(errfp, "No disc in current slot.\n");
				/*FALLTHROUGH*/

			default:
				curslot = -1;
				break;
			}
		}
	}

	/* Change to the desired disc */
	if (app_data.numdiscs > 1 && curslot != (s->cur_disc - 1)) {
#ifdef NOT_NEEDED
	    {
		int	slotstat;

		DBGPRN(errfp, "Checking disc %d\n", s->cur_disc);
		if (slioc_send(CDROM_DRIVE_STATUS, (void *) (s->cur_disc - 1),
				0, app_data.debug, &slotstat)) {
			DBGPRN(errfp, "CDROM_DRIVE_STATUS return=0x%x\n",
				slotstat);

			switch (slotstat) {
			case CDS_NO_DISC:
				DBGPRN(errfp, "%s\n\nClose: %s\n",
					"No disc in slot.", s->curdev);
				slioc_close();
				return FALSE;

			case CDS_TRAY_OPEN:
			case CDS_DRIVE_NOT_READY:
				DBGPRN(errfp, "%s\n\nClose: %s\n",
					"Drive not ready.", s->curdev);
				slioc_close();
				return FALSE;

			case CDS_DISC_OK:
			case CDS_NO_INFO:
			default:
				/* Just proceed */
				break;
			}
		}
	    }
#endif

		DBGPRN(errfp, "Changing to disc %d\n", s->cur_disc);

		if (!slioc_send(CDROM_SELECT_DISC, (void *) (s->cur_disc - 1),
				0, app_data.debug, NULL)) {
			DBGPRN(errfp, "%s\n\nClose: %s\n",
				"Disc change failed.", s->curdev);
			slioc_close();
			return FALSE;
		}
	}

	/* Linux hack:  The CD-ROM driver allows the open to succeed
	 * even if there is no CD loaded.  We test for the existence of
	 * a disc with slioc_disc_present().
	 */
	for (i = 0; i < 3; i++) {
		if ((ret = slioc_disc_present(FALSE)) == TRUE)
			break;
	}
	if (ret == FALSE) {
		DBGPRN(errfp, "%s\n\nClose: %s\n",
			"Disc not ready.", s->curdev);
		slioc_close();
		return FALSE;
	}
#endif	/* linux */

	return TRUE;
}


/*
 * slioc_close
 *	Close CD-ROM device
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
void
slioc_close(void)
{
	if (slioc_fd >= 0) {
		(void) close(slioc_fd);
		slioc_fd = -1;
	}
}


/*
 * slioc_rdsubq
 *	Send Read Subchannel command to the device
 *
 * Args:
 *	buf - Pointer to the return data buffer
 *	msf - Whether to use MSF or logical block address format
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
slioc_rdsubq(struct cdrom_subchnl *sub, byte_t msf)
{
	bool_t	ret;

	sub->cdsc_format = msf;
	ret = slioc_send(CDROMSUBCHNL, sub, sizeof(struct cdrom_subchnl),
			 TRUE, NULL);

	if (ret) {
		DBGDUMP("cdrom_subchnl data bytes", (byte_t *) sub,
			sizeof(struct cdrom_subchnl));
	}

	return (ret);
}


/*
 * slioc_rdtoc
 *	Send Read TOC command to the device
 *
 * Args:
 *	buf - Pointer to the return data toc header
 *	h - address of pointer to array of toc entrys will be allocated
 *	    by this routine
 *	start - Starting track number for which the TOC data is returned
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
slioc_rdtoc(struct cdrom_tochdr *h, struct cdrom_tocentry **e, int start)
{
	int			i,
				j,
				k,
				allocsz;
	struct cdrom_tocentry	*t;

	/* Read the TOC header first */
	if (!slioc_send(CDROMREADTOCHDR, h, sizeof(struct cdrom_tochdr),
			TRUE, NULL))
		return FALSE;

	DBGDUMP("cdrom_tochdr data bytes", (byte_t *) h,
		sizeof(struct cdrom_tochdr));

	if (start == 0)
		start = h->cdth_trk0;

	if (start > (int) h->cdth_trk1)
		return FALSE;

	allocsz = (h->cdth_trk1 - start + 2) * sizeof(struct cdrom_tocentry);

	*e = (struct cdrom_tocentry *)(void *) MEM_ALLOC(
		"cdrom_tocentry",
		allocsz
	);
	t = (struct cdrom_tocentry *)(void *) MEM_ALLOC(
		"cdrom_tocentry",
		allocsz
	);

	if (*e == NULL || t == NULL) {
		DI_FATAL(app_data.str_nomemory);
		return FALSE;
	}

	(void) memset((byte_t *) *e, 0, allocsz);
	(void) memset((byte_t *) t, 0, allocsz);

	for (i = start; i <= (int) (h->cdth_trk1 + 1); i++) {
		j = i - start;

		t[j].cdte_track =
			(i <= (int) h->cdth_trk1) ? i : CDROM_LEADOUT;

		t[j].cdte_format = CDROM_MSF;

		if (!slioc_send(CDROMREADTOCENTRY, &t[j],
				sizeof(struct cdrom_tocentry), TRUE, NULL)) {
			MEM_FREE(*e);
			MEM_FREE(t);
			return FALSE;
		}

		DBGDUMP("cdrom_tocentry data bytes", (byte_t *) &t[j],
			sizeof(struct cdrom_tocentry));

		/* Hack: workaround CD-ROM firmware bug
		 * Some CD-ROMs return track numbers in BCD
		 * rather than binary.
		 */
		if ((int) (t[j].cdte_track & 0xf) > 0x9 &&
		    (int) (t[j].cdte_track & 0xf) < 0x10 &&
		    t[j].cdte_addr.lba == 0) {
			/* BUGLY CD-ROM firmware detected! */
			slioc_bcd_hack = TRUE;
		}

		/* Sanity check */
		if (t[j].cdte_track == CDROM_LEADOUT &&
		    t[j].cdte_addr.lba == t[0].cdte_addr.lba) {
			MEM_FREE(*e);
			MEM_FREE(t);
			return FALSE;
		}
	}

	/* Fix up TOC data */
	for (i = start; i <= (int) (h->cdth_trk1 + 1); i++) {
		if (slioc_bcd_hack && (i & 0xf) > 0x9 && (i & 0xf) < 0x10)
			continue;

		j = i - start;
		k = (slioc_bcd_hack ? util_bcdtol(i) : i) - start;

		(*e)[k].cdte_adr = t[j].cdte_adr;
		(*e)[k].cdte_ctrl = t[j].cdte_ctrl;
		(*e)[k].cdte_format = t[j].cdte_format;
		(*e)[k].cdte_addr.lba = t[j].cdte_addr.lba;
		(*e)[k].cdte_datamode = t[j].cdte_datamode;
		if (t[j].cdte_track == CDROM_LEADOUT) {
			(*e)[k].cdte_track = t[j].cdte_track;
			break;
		}
		else {
			if (slioc_bcd_hack)
				(*e)[k].cdte_track = (byte_t)
					util_bcdtol(t[j].cdte_track);
			else
				(*e)[k].cdte_track = t[j].cdte_track;
		}
	}
	if (slioc_bcd_hack)
		h->cdth_trk1 = (byte_t) util_bcdtol(h->cdth_trk1);

	MEM_FREE(t);

	return TRUE;
}


/*
 * slioc_disc_present
 *	Check if a CD is loaded.  Much of the complication in this
 *	routine stems from the fact that the SunOS and the Linux
 *	CD-ROM drivers behave differently when a CD is ejected.
 *	In fact, the scsi, mcd, sbpcd, cdu31a drivers under Linux
 *	are also inconsistent amongst each other (Argh!).
 *
 * Args:
 *	savstat - Whether to save start-up status in slioc_tst_status.
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure (drive not ready)
 */
STATIC bool_t
slioc_disc_present(bool_t savstat)
{
	struct cdrom_subchnl	sub;
	struct cdrom_tochdr	h;
	struct cdrom_tocentry	e;
	word32_t		a1,
				a2;
	int			ret;
	static int		tot_trks = 0;
	static word32_t		sav_a1 = 0,
				sav_a2 = 0;

#ifdef linux
	/* Use new method to get drive status */
	if (slioc_send(CDROM_DRIVE_STATUS, (void *) CDSL_CURRENT, 0,
		       app_data.debug, &ret)) {
		DBGPRN(errfp, "CDROM_DRIVE_STATUS return=0x%x\n", ret);

		switch (ret) {
		case CDS_DISC_OK:
			return TRUE;
		case CDS_NO_DISC:
		case CDS_TRAY_OPEN:
		case CDS_DRIVE_NOT_READY:
			return FALSE;
		case CDS_NO_INFO:
			break;
		default:
			return FALSE;
		}
	}
	/* CDROM_DRIVE_STATUS failed: try the old way */
#endif

	if (savstat)
		slioc_tst_status = MOD_NODISC;

	/* Fake it with CDROMSUBCHNL */
	(void) memset((byte_t *) &sub, 0, sizeof(struct cdrom_subchnl));
	sub.cdsc_format = CDROM_MSF;
	if (!slioc_send(CDROMSUBCHNL, &sub, sizeof(struct cdrom_subchnl),
			app_data.debug, NULL))
		return FALSE;

	switch (sub.cdsc_audiostatus) {
	case CDROM_AUDIO_PLAY:
		if (savstat) {
			DBGPRN(errfp, "\nstatus=CDROM_AUDIO_PLAY\n");
			slioc_tst_status = MOD_PLAY;
			return TRUE;
		}
		break;
	case CDROM_AUDIO_PAUSED:
		if (savstat) {
			DBGPRN(errfp, "\nstatus=CDROM_AUDIO_PAUSED\n");
			slioc_tst_status = MOD_PAUSE;
			return TRUE;
		}
		break;
	case CDROM_AUDIO_ERROR:
		DBGPRN(errfp, "\nstatus=CDROM_AUDIO_ERROR\n");
		break;
	case CDROM_AUDIO_COMPLETED:
		DBGPRN(errfp, "\nstatus=CDROM_AUDIO_COMPLETED\n");
		break;
	case CDROM_AUDIO_NO_STATUS:
		DBGPRN(errfp, "\nstatus=CDROM_AUDIO_NO_STATUS\n");
		break;
	case CDROM_AUDIO_INVALID:
		DBGPRN(errfp, "\nstatus=CDROM_AUDIO_INVALID\n");
		break;
	default:
		DBGPRN(errfp, "\nstatus=unknown (%d)\n", sub.cdsc_audiostatus);
		return FALSE;
	}

	if (savstat)
		slioc_tst_status = MOD_STOP;

	/* CDROMSUBCHNL didn't give useful info.
	 * Try CDROMREADTOCHDR and CDROMREADTOCENTRY.
	 */
	(void) memset((byte_t *) &h, 0, sizeof(struct cdrom_tochdr));
	if (!slioc_send(CDROMREADTOCHDR, &h, sizeof(struct cdrom_tochdr),
			app_data.debug, NULL))
		return FALSE;

	if (h.cdth_trk0 == 0 && h.cdth_trk1 == 0)
		return FALSE;

	if ((h.cdth_trk1 - h.cdth_trk0 + 1) != tot_trks) {
		/* Disk changed */
		tot_trks = h.cdth_trk1 - h.cdth_trk0 + 1;
		return FALSE;
	}

	(void) memset((byte_t *) &e, 0, sizeof(struct cdrom_tocentry));
	e.cdte_format = CDROM_MSF;
	e.cdte_track = h.cdth_trk0;
	if (!slioc_send(CDROMREADTOCENTRY, &e, sizeof(struct cdrom_tocentry),
			app_data.debug, NULL))
		return FALSE;

	a1 = (word32_t) e.cdte_addr.lba;

	(void) memset((byte_t *) &e, 0, sizeof(struct cdrom_tocentry));
	e.cdte_format = CDROM_MSF;
	e.cdte_track = h.cdth_trk1;
	if (!slioc_send(CDROMREADTOCENTRY, &e, sizeof(struct cdrom_tocentry),
			app_data.debug, NULL))
		return FALSE;

	a2 = (word32_t) e.cdte_addr.lba;

	DBGPRN(errfp, "\na1=0x%x a2=0x%x\n", a1, a2);

	if (a1 != sav_a1 || a2 != sav_a2) {
		/* Disk changed */
		sav_a1 = a1;
		sav_a2 = a2;
		return FALSE;
	}

	if (tot_trks > 1 && a1 == a2)
		return FALSE;

	return TRUE;
}


/*
 * slioc_playmsf
 *	Send Play Audio MSF command to the device
 *
 * Args:
 *	start - Pointer to the starting position MSF data
 *	end - Pointer to the ending position MSF data
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
slioc_playmsf(msf_t *start, msf_t *end)
{
	struct cdrom_msf	m;

	m.cdmsf_min0 = start->min;
	m.cdmsf_sec0 = start->sec;
	m.cdmsf_frame0 = start->frame;
	m.cdmsf_min1 = end->min;
	m.cdmsf_sec1 = end->sec;
	m.cdmsf_frame1 = end->frame;

	DBGDUMP("cdrom_msf data bytes", (byte_t *) &m,
		sizeof(struct cdrom_msf));

	return (
		slioc_send(CDROMPLAYMSF, &m, sizeof(struct cdrom_msf),
			   TRUE, NULL)
	);
}


/*
 * slioc_play_trkidx
 *	Send Play Audio Track/Index command to the device
 *
 * Args:
 *	start_trk - Starting track number
 *	start_idx - Starting index number
 *	end_trk - Ending track number
 *	end_idx - Ending index number
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
slioc_play_trkidx(int start_trk, int start_idx, int end_trk, int end_idx)
{
	struct cdrom_ti	t;

	if (slioc_bcd_hack) {
		/* Hack: BUGLY CD-ROM firmware */
		t.cdti_trk0 = util_ltobcd(start_trk);
		t.cdti_ind0 = util_ltobcd(start_idx);
		t.cdti_trk1 = util_ltobcd(end_trk);
		t.cdti_ind1 = util_ltobcd(end_idx);
	}
	else {
		t.cdti_trk0 = start_trk;
		t.cdti_ind0 = start_idx;
		t.cdti_trk1 = end_trk;
		t.cdti_ind1 = end_idx;
	}

	DBGDUMP("cdrom_ti data bytes", (byte_t *) &t, sizeof(struct cdrom_ti));

	return (
		slioc_send(CDROMPLAYTRKIND, &t, sizeof(struct cdrom_ti),
			   TRUE, NULL)
	);
}


/*
 * slioc_start_stop
 *	Send Start/Stop Unit command to the device
 *
 * Args:
 *	start - Whether to start unit or stop unit
 *	loej - Whether caddy load/eject operation should be performed
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
slioc_start_stop(bool_t start, bool_t loej)
{
	bool_t	ret;

	if (start) {
		if (loej)
#ifdef CDROMCLOSETRAY
			ret = slioc_send(CDROMCLOSETRAY, NULL, 0, TRUE, NULL);
#else
			ret = FALSE;
#endif
		else
			ret = slioc_send(CDROMSTART, NULL, 0, TRUE, NULL);
	}
	else {
		slioc_playing = FALSE;

		if (loej)
			ret = slioc_send(CDROMEJECT, NULL, 0, TRUE, NULL);
		else
			ret = slioc_send(CDROMSTOP, NULL, 0, TRUE, NULL);
	}

	/* Delay a bit to let the CD load or eject.  This is a hack to
	 * work around firmware bugs in some CD-ROM drives.  These drives
	 * don't handle new commands well when the CD is loading/ejecting
	 * with the IMMED bit set in the Start/Stop Unit command.
	 */
	if (ret && loej) {
		int	n;

		n = (app_data.ins_interval + 1000 - 1) / 1000;
		if (start)
			n *= 2;

		(void) sleep(n);
	}

	return (ret);

}


/*
 * slioc_pause_resume
 *	Send Pause/Resume command to the device
 *
 * Args:
 *	resume - Whether to resume or pause
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
slioc_pause_resume(bool_t resume)
{
	return (
		slioc_send(resume ? CDROMRESUME : CDROMPAUSE, NULL, 0,
		TRUE, NULL)
	);
}


/*
 * slioc_do_playaudio
 *	General top-level play audio function
 *
 * Args:
 *	addr_fmt - The address formats specified:
 *		ADDR_BLK: logical block address (not supported)
 *		ADDR_MSF: MSF address
 *		ADDR_TRKIDX: Track/index numbers
 *		ADDR_OPTEND: Ending address can be ignored
 *	start_addr - Starting logical block address (not supported)
 *	end_addr - Ending logical block address (not supported)
 *	start_msf - Pointer to start address MSF data
 *	end_msf - Pointer to end address MSF data
 *	trk - Starting track number
 *	idx - Starting index number
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
slioc_do_playaudio(
	byte_t		addr_fmt,
	word32_t	start_addr,
	word32_t	end_addr,
	msf_t		*start_msf,
	msf_t		*end_msf,
	byte_t		trk,
	byte_t		idx
)
{
	msf_t		emsf,
			*emsfp = NULL;
	bool_t		ret = FALSE,
			do_playmsf,
			do_playti;

	/* Fix addresses: Some CD-ROM drives will only allow playing to
	 * the last frame minus a few frames.
	 */
	if ((addr_fmt & ADDR_MSF) && end_msf != NULL) {
		emsf = *end_msf;	/* Structure copy */
		emsfp = &emsf;

		if (emsfp->frame >= CLIP_FRAMES)
			emsfp->frame -= CLIP_FRAMES;
		else {
			if (emsfp->sec > 0)
				emsfp->sec--;
			else {
				emsfp->sec = 59;
				if (emsfp->min > 0)
					emsfp->min--;
			}
			emsfp->frame = FRAME_PER_SEC -
				(CLIP_FRAMES - emsfp->frame);
		}

		emsfp->res = start_msf->res = 0;

		/* Save end address for error recovery */
		slioc_sav_end_msf = *end_msf;
	}
	if (addr_fmt & ADDR_BLK) {
		if (end_addr >= CLIP_FRAMES)
			end_addr -= CLIP_FRAMES;
		else
			end_addr = 0;

		/* Save end address for error recovery */
		slioc_sav_end_addr = end_addr;
	}

	/* Save end address format for error recovery */
	slioc_sav_end_fmt = addr_fmt;

	do_playmsf = (addr_fmt & ADDR_MSF) && app_data.playmsf_supp;
	do_playti = (addr_fmt & ADDR_TRKIDX) && app_data.playti_supp;

	if (do_playmsf || do_playti) {
		if (slioc_playing)
			/* Pause playback first */
			(void) slioc_pause_resume(FALSE);
		else
			/* Spin up CD */
			(void) slioc_start_stop(TRUE, FALSE);
	}

	if (do_playmsf)
		ret = slioc_playmsf(start_msf, emsfp);
	
	if (!ret && do_playti)
		ret = slioc_play_trkidx(trk, idx, trk, idx);

	if (ret)
		slioc_playing = TRUE;

	return (ret);
}


/*
 * slioc_fail_recov
 *	Playback error recovery handler: Restart playback after skipping
 *	some frames.
 *
 * Args:
 *	blk - Fault frame address
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
slioc_fail_recov(word32_t blk)
{
	msf_t		recov_start_msf;
	word32_t	recov_start_addr;
	bool_t		ret;

	ret = TRUE;

	recov_start_addr = blk + ERR_SKIPBLKS;
	util_blktomsf(
		recov_start_addr,
		&recov_start_msf.min,
		&recov_start_msf.sec,
		&recov_start_msf.frame,
		MSF_OFFSET
	);

	/* Check to see if we have skipped past
	 * the end.
	 */
	if (recov_start_msf.min > slioc_sav_end_msf.min)
		ret = FALSE;
	else if (recov_start_msf.min == slioc_sav_end_msf.min) {
		if (recov_start_msf.sec > slioc_sav_end_msf.sec)
			ret = FALSE;
		else if ((recov_start_msf.sec ==
			  slioc_sav_end_msf.sec) &&
			 (recov_start_msf.frame >
			  slioc_sav_end_msf.frame)) {
			ret = FALSE;
		}
	}
	if (recov_start_addr >= slioc_sav_end_addr)
		ret = FALSE;

	if (ret) {
		/* Restart playback */
		(void) fprintf(errfp, "CD audio: %s (%02u:%02u.%02u)\n",
				app_data.str_recoverr,
				recov_start_msf.min,
				recov_start_msf.sec,
				recov_start_msf.frame);

		ret = slioc_do_playaudio(
			slioc_sav_end_fmt,
			recov_start_addr, slioc_sav_end_addr,
			&recov_start_msf, &slioc_sav_end_msf,
			0, 0
		);
	}

	return (ret);
}


/*
 * slioc_get_playstatus
 *	Obtain and update current playback status information
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - Audio playback is in progress
 *	FALSE - Audio playback stopped or command failure
 */
STATIC bool_t
slioc_get_playstatus(curstat_t *s)
{
	struct cdrom_subchnl	sub;
	word32_t		curtrk,
				curidx;
	byte_t			audio_status;
	bool_t			done;
	static int		errcnt = 0;
	static word32_t		errblk = 0;
	static bool_t		in_slioc_get_playstatus = FALSE;


	/* Lock this routine from multiple entry */
	if (in_slioc_get_playstatus)
		return TRUE;

	in_slioc_get_playstatus = TRUE;

	(void) memset((byte_t *) &sub, 0, sizeof(struct cdrom_subchnl));
	if (!slioc_rdsubq(&sub, CDROM_MSF)) {
		/* Check to see if the disc had been manually ejected */
		if (!slioc_disc_ready(s)) {
			slioc_sav_end_addr = 0;
			slioc_sav_end_msf.min = 0;
			slioc_sav_end_msf.sec = 0;
			slioc_sav_end_msf.frame = 0;
			slioc_sav_end_fmt = 0;
			errcnt = 0;
			errblk = 0;

			in_slioc_get_playstatus = FALSE;
			return FALSE;
		}

		/* The read subchannel command failed for some
		 * unknown reason.  Just return success and
		 * hope the next poll succeeds.  We don't want
		 * to return FALSE here because that would stop
		 * the poll.
		 */
		in_slioc_get_playstatus = FALSE;
		return TRUE;
	}

	/* Check the subchannel data */

	s->cur_tot_frame = sub.cdsc_absaddr.msf.frame;
	s->cur_tot_sec = sub.cdsc_absaddr.msf.second;
	s->cur_tot_min = sub.cdsc_absaddr.msf.minute;
	util_msftoblk(
		s->cur_tot_min,
		s->cur_tot_sec,
		s->cur_tot_frame,
		&s->cur_tot_addr,
		MSF_OFFSET
	);

	s->cur_trk_frame = sub.cdsc_reladdr.msf.frame;
	s->cur_trk_sec = sub.cdsc_reladdr.msf.second;
	s->cur_trk_min = sub.cdsc_reladdr.msf.minute;
	util_msftoblk(
		s->cur_trk_min,
		s->cur_trk_sec,
		s->cur_trk_frame,
		&s->cur_trk_addr,
		0
	);

	/* Update time display */
	DPY_TIME(s, FALSE);

	if (slioc_bcd_hack) {
		/* Hack: BUGLY CD-ROM firmware */
		curtrk = util_bcdtol(sub.cdsc_trk);
		curidx = util_bcdtol(sub.cdsc_ind);
	}
	else {
		curtrk = sub.cdsc_trk;
		curidx = sub.cdsc_ind;
	}
	/* Hack: to work around firmware anomalies
	 * in some CD-ROM drives.
	 */
	if (curtrk >= MAXTRACK && curtrk != CDROM_LEADOUT)
		audio_status = CDROM_AUDIO_INVALID;

	if (curtrk != s->cur_trk) {
		s->cur_trk = curtrk;
		DPY_TRACK(s);
	}

	if (curidx != s->cur_idx) {
		s->cur_idx = curidx;
		s->sav_iaddr = s->cur_tot_addr;
		DPY_INDEX(s);
	}

	audio_status = sub.cdsc_audiostatus;

	/* Hack: to work around the fact that some CD-ROM drives
	 * return CDROM_AUDIO_PAUSED status after issuing a Stop Unit command.
	 * Just treat the status as completed if we get a paused status
	 * and we don't expect the drive to be paused.
	 */
	if (audio_status == CDROM_AUDIO_PAUSED && s->mode != MOD_PAUSE &&
	    !slioc_idx_pause)
		audio_status = CDROM_AUDIO_COMPLETED;

	/* Force completion status */
	if (slioc_fake_stop)
		audio_status = CDROM_AUDIO_COMPLETED;

	/* Deal with playback status */
	switch (audio_status) {
	case CDROM_AUDIO_PLAY:
	case CDROM_AUDIO_PAUSED:
		done = FALSE;

		/* If we haven't encountered an error for a while, then
		 * clear the error count.
		 */
		if (errcnt > 0 && (s->cur_tot_addr - errblk) > ERR_CLRTHRESH)
			errcnt = 0;
		break;

	case CDROM_AUDIO_ERROR:
		/* Check to see if the disc had been manually ejected */
		if (!slioc_disc_ready(s)) {
			slioc_sav_end_addr = 0;
			slioc_sav_end_msf.min = 0;
			slioc_sav_end_msf.sec = 0;
			slioc_sav_end_msf.frame = 0;
			slioc_sav_end_fmt = 0;
			errcnt = 0;
			errblk = 0;

			in_slioc_get_playstatus = FALSE;
			return FALSE;
		}

		/* Audio playback stopped due to a disc error.  We will
		 * try to restart the playback by skipping a few frames
		 * and continuing.  This will cause a glitch in the sound
		 * but is better than just stopping.
		 */
		done = FALSE;

		/* Check for max errors limit */
		if (++errcnt > MAX_RECOVERR) {
			done = TRUE;
			(void) fprintf(errfp, "CD audio: %s\n",
				       app_data.str_maxerr);
		}

		errblk = s->cur_tot_addr;

		if (!done && slioc_fail_recov(errblk)) {
			in_slioc_get_playstatus = FALSE;
			return TRUE;
		}

		/*FALLTHROUGH*/
	case CDROM_AUDIO_COMPLETED:
	case CDROM_AUDIO_NO_STATUS:
	case CDROM_AUDIO_INVALID:
		done = TRUE;

		if (!slioc_fake_stop)
			slioc_playing = FALSE;

		slioc_fake_stop = FALSE;

		switch (s->mode) {
		case MOD_SAMPLE:
			done = !slioc_run_sample(s);
			break;

		case MOD_AB:
			done = !slioc_run_ab(s);
			break;

		case MOD_PLAY:
		case MOD_PAUSE:
			s->cur_trk_addr = 0;
			s->cur_trk_min = s->cur_trk_sec = s->cur_trk_frame = 0;

			if (s->shuffle || s->program)
				done = !slioc_run_prog(s);

			if (s->repeat && (s->program || !app_data.multi_play))
				done = !slioc_run_repeat(s);

			break;
		}

		break;

	default:
		/* Something is wrong with the data. */
		done = FALSE;
		break;
	}

	if (done) {
		byte_t	omode;
		bool_t	prog;

		/* Save some old states */
		omode = s->mode;
		prog = (s->program || s->onetrk_prog);

		/* Reset states */
		di_reset_curstat(s, FALSE, FALSE);
		s->mode = MOD_STOP;

		slioc_sav_end_addr = 0;
		slioc_sav_end_msf.min = slioc_sav_end_msf.sec =
			slioc_sav_end_msf.frame = 0;
		slioc_sav_end_fmt = 0;
		errcnt = 0;
		errblk = 0;

		if (app_data.multi_play && omode == MOD_PLAY && !prog) {
			bool_t	cont;

			s->prev_disc = s->cur_disc;

			if (app_data.reverse) {
				if (s->cur_disc > s->first_disc) {
					/* Play the previous disc */
					s->cur_disc--;
					slioc_mult_autoplay = TRUE;
				}
				else {
					/* Go to the last disc */
					s->cur_disc = s->last_disc;

					if (s->repeat) {
						s->rptcnt++;
						slioc_mult_autoplay = TRUE;
					}
				}
			}
			else {
				if (s->cur_disc < s->last_disc) {
					/* Play the next disc */
					s->cur_disc++;
					slioc_mult_autoplay = TRUE;
				}
				else {
					/* Go to the first disc.  */
					s->cur_disc = s->first_disc;

					if (s->repeat) {
						s->rptcnt++;
						slioc_mult_autoplay = TRUE;
					}
				}
			}

			if ((cont = slioc_mult_autoplay) == TRUE) {
				/* Allow recursion from this point */
				in_slioc_get_playstatus = FALSE;
			}

			/* Change disc */
			slioc_chgdisc(s);

			if (cont)
				return TRUE;
		}

		s->rptcnt = 0;
		DPY_ALL(s);

		if (app_data.done_eject) {
			/* Eject the disc */
			slioc_load_eject(s);
		}
		else {
			/* Spin down the disc */
			slioc_start_stop(FALSE, FALSE);
		}

		in_slioc_get_playstatus = FALSE;
		return FALSE;
	}

	in_slioc_get_playstatus = FALSE;
	return TRUE;
}


/*
 * slioc_cfg_vol
 *	Audio volume control function
 *
 * Args:
 *	vol - Logical volume value to set to
 *	s - Pointer to the curstat_t structure
 *	query - If TRUE, query current volume only
 *
 * Return:
 *	The current logical volume value, or -1 on failure.
 */
STATIC int
slioc_cfg_vol(int vol, curstat_t *s, bool_t query)
{
	struct cdrom_volctrl	volctrl;
	static bool_t		first = TRUE;

	(void) memset((byte_t *) &volctrl, 0, sizeof(struct cdrom_volctrl));

	if (query) {
		if (first) {
			first = FALSE;

			/* SunOS/Solaris/Linux doesn't give a way to read
			 * volume setting via CDROM ioctl.
			 * Force the setting to maximum.
			 */
			vol = 100;
			s->level_left = s->level_right = 100;

			(void) slioc_cfg_vol(vol, s, FALSE);
		}
		return (vol);
	}
	else {
		volctrl.channel0 = di_scale_vol(
			di_taper_vol(vol * (int) s->level_left / 100)
		);
		volctrl.channel1 = di_scale_vol(
			di_taper_vol(vol * (int) s->level_right / 100)
		);

		DBGDUMP("cdrom_volctrl data bytes", (byte_t *) &volctrl,
			sizeof(struct cdrom_volctrl));

		if (slioc_send(CDROMVOLCTRL, &volctrl,
			       sizeof(struct cdrom_volctrl), TRUE, NULL))
			return (vol);
		else if (volctrl.channel0 != volctrl.channel1) {
			/* Set the balance to the center
			 * and retry.
			 */
			volctrl.channel0 = volctrl.channel1 =
				di_scale_vol(di_taper_vol(vol));

			DBGDUMP("cdrom_volctrl data bytes",
				(byte_t *) &volctrl,
				sizeof(struct cdrom_volctrl));

			if (slioc_send(CDROMVOLCTRL, &volctrl,
				       sizeof(struct cdrom_volctrl),
				       TRUE, NULL)) {
				/* Success: Warp balance control */
				s->level_left = s->level_right = 100;
				SET_BAL_SLIDER(0);
				return (vol);
			}

			/* Still failed: just drop through */
		}

	}

	return -1;
}


/*
 * slioc_vendor_model
 *	Query and update CD-ROM vendor/model/revision information
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
slioc_vendor_model(curstat_t *s)
{
	/*
	 * There is currently no way to get this info,
	 * so just fill in some default info.
	 */
	(void) strcpy(s->vendor, "standard");
	(void) strcpy(s->prod, "CD-ROM drive    ");
	(void) strcpy(s->revnum, " -- ");
}


/*
 * slioc_fix_toc
 *	CD Table Of Contents post-processing function.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
slioc_fix_toc(curstat_t *s)
{
	int	i;

	/*
	 * Set the end-of-playback to the first data track
	 * after the first track, if applicable.
	 */
	for (i = 1; i < (int) s->tot_trks; i++) {
		if (s->trkinfo[i].type == TYP_DATA) {
			s->tot_min = s->trkinfo[i].min;
			s->tot_sec = s->trkinfo[i].sec;
			s->tot_frame = s->trkinfo[i].frame;
			s->tot_addr = s->trkinfo[i].addr;
			break;
		}
	}
}


/*
 * slioc_get_toc
 *	Query and update the CD Table Of Contents
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
slioc_get_toc(curstat_t *s)
{
	int			i,
				ntrks;
	struct cdrom_tochdr	h;
	struct cdrom_tocentry	*e,
				*p;

	if (!slioc_rdtoc(&h, &e, 0))
		return FALSE;

	/* Fill curstat structure with TOC data */
	s->first_trk = h.cdth_trk0;
	ntrks = (int) (h.cdth_trk1 - h.cdth_trk0) + 1;

	p = e;

	for (i = 0; i <= (int) ntrks; i++) {
		s->trkinfo[i].trkno = p->cdte_track;
		s->trkinfo[i].type = TYP_AUDIO;	/* shrug */
		s->trkinfo[i].min = p->cdte_addr.msf.minute;
		s->trkinfo[i].sec = p->cdte_addr.msf.second;
		s->trkinfo[i].frame = p->cdte_addr.msf.frame;
		util_msftoblk(
			s->trkinfo[i].min,
			s->trkinfo[i].sec,
			s->trkinfo[i].frame,
			&s->trkinfo[i].addr,
			MSF_OFFSET
		);

		if (p->cdte_track == CDROM_LEADOUT || i == (MAXTRACK - 1)) {
			s->tot_min = s->trkinfo[i].min;
			s->tot_sec = s->trkinfo[i].sec;
			s->tot_frame = s->trkinfo[i].frame;
			s->tot_trks = (byte_t) i;
			s->tot_addr = s->trkinfo[i].addr;
			s->last_trk = s->trkinfo[i-1].trkno;

			break;
		}

		p++;
	}

	MEM_FREE(e);

	slioc_fix_toc(s);
	return TRUE;
}


/*
 * slioc_start_stat_poll
 *	Start polling the drive for current playback status
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
slioc_start_stat_poll(curstat_t *s)
{
	slioc_stat_polling = TRUE;

	/* Start poll timer */
	if (di_clinfo->timeout != NULL) {
		slioc_stat_id = di_clinfo->timeout(
			slioc_stat_interval,
			slioc_stat_poll,
			(byte_t *) s
		);
	}
}


/*
 * slioc_stop_stat_poll
 *	Stop polling the drive for current playback status
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
STATIC void
slioc_stop_stat_poll(void)
{
	if (slioc_stat_polling) {
		/* Stop poll timer */
		if (di_clinfo->untimeout != NULL)
			di_clinfo->untimeout(slioc_stat_id);

		slioc_stat_polling = FALSE;
	}
}


/*
 * slioc_start_insert_poll
 *	Start polling the drive for disc insertion
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
slioc_start_insert_poll(curstat_t *s)
{
	if (slioc_insert_polling || app_data.ins_disable ||
	    (s->mode != MOD_BUSY && s->mode != MOD_NODISC))
		return;

	if (app_data.numdiscs > 1 && app_data.multi_play)
		slioc_ins_interval =
			app_data.ins_interval / app_data.numdiscs;
	else
		slioc_ins_interval = app_data.ins_interval;

	if (slioc_ins_interval < 500)
		slioc_ins_interval = 500;

	/* Start poll timer */
	if (di_clinfo->timeout != NULL) {
		slioc_insert_id = di_clinfo->timeout(
			slioc_ins_interval,
			slioc_insert_poll,
			(byte_t *) s
		);

		if (slioc_insert_id != 0)
			slioc_insert_polling = TRUE;
	}
}


/*
 * slioc_stat_poll
 *	The playback status polling function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
slioc_stat_poll(curstat_t *s)
{
	if (!slioc_stat_polling)
		return;

	/* Get current audio playback status */
	if (slioc_get_playstatus(s)) {
		/* Register next poll interval */
		if (di_clinfo->timeout != NULL) {
			slioc_stat_id = di_clinfo->timeout(
				slioc_stat_interval,
				slioc_stat_poll,
				(byte_t *) s
			);
		}
	}
	else
		slioc_stat_polling = FALSE;
}


/*
 * slioc_insert_poll
 *	The disc insertion polling function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
slioc_insert_poll(curstat_t *s)
{
	/* Check to see if a disc is inserted */
	if (!slioc_disc_ready(s)) {
		/* Register next poll interval */
		if (di_clinfo->timeout != NULL) {
			slioc_insert_id = di_clinfo->timeout(
				slioc_ins_interval,
				slioc_insert_poll,
				(byte_t *) s
			);
		}
	}
	else
		slioc_insert_polling = FALSE;
}


/*
 * slioc_disc_ready
 *	Check if the disc is loaded and ready for use, and update
 *	curstat table.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - Disc is ready
 *	FALSE - Disc is not ready
 */
STATIC bool_t
slioc_disc_ready(curstat_t *s)
{
	int		i,
			vol;
	bool_t		err,
			first_open = FALSE,
			reopen = FALSE;
	static bool_t	opened_once = FALSE,
			in_slioc_disc_ready = FALSE;

	/* Lock this routine from multiple entry */
	if (in_slioc_disc_ready)
		return TRUE;

	in_slioc_disc_ready = TRUE;

	/* If device has not been opened, attempt to open it */
	if (slioc_not_open) {
		/* Check for another copy of the CD player running on
		 * the specified device.
		 */
		if (!s->devlocked && !di_devlock(s, app_data.device)) {
			s->mode = MOD_BUSY;
			DPY_TIME(s, FALSE);
			slioc_start_insert_poll(s);
			in_slioc_disc_ready = FALSE;
			return FALSE;
		}

		s->devlocked = TRUE;
		s->mode = MOD_NODISC;
		DPY_DISC(s);

		/* Open CD-ROM device */
		DBGPRN(errfp, "\nOpen: %s\n", s->curdev);

		if (!slioc_open(s)) {
			DBGPRN(errfp, "Open of %s failed\n", s->curdev);
			DPY_TIME(s, FALSE);

			if (app_data.multi_play) {
				/* This requested disc is not there
				 * or not ready.
				 */
				if (app_data.reverse) {
					if (s->cur_disc > s->first_disc) {
						/* Try the previous disc */
						s->cur_disc--;
					}
					else {
						/* Go to the last disc */
						s->cur_disc = s->last_disc;

						if (slioc_mult_autoplay) {
						    if (s->repeat)
							s->rptcnt++;
						    else
							slioc_mult_autoplay =
							FALSE;
						}
					}
				}
				else {
					if (s->cur_disc < s->last_disc) {
						/* Try the next disc */
						s->cur_disc++;
					}
					else {
						/* Go to the first disc */
						s->cur_disc = s->first_disc;

						if (slioc_mult_autoplay) {
						    if (s->repeat)
							s->rptcnt++;
						    else
							slioc_mult_autoplay =
							FALSE;
						}
					}
				}
			}

			slioc_start_insert_poll(s);
			in_slioc_disc_ready = FALSE;
			return FALSE;
		}

		if (!opened_once)
			first_open = TRUE;
		else
			reopen = TRUE;

		slioc_not_open = FALSE;
		opened_once = TRUE;
	}

	for (i = 0; i < 5; i++) {
		/* Check if a CD is loaded */
		if ((err = !slioc_disc_present(first_open)) == TRUE) {
			s->mode = MOD_NODISC;
			DBCLEAR(s, FALSE);
		}
		else
			break;
	}

	if (!err) {
		if (first_open) {
			/* Let things settle a bit */
			sleep(1);

			/* Fill in inquiry data */
			slioc_vendor_model(s);

			/* Query current volume/balance settings */
			if ((vol = slioc_cfg_vol(0, s, TRUE)) >= 0)
				s->level = (byte_t) vol;
			else
				s->level = 0;

			/* Set volume to preset value, if so configured */
			if (app_data.startup_vol > 0 &&
			    (vol = slioc_cfg_vol(app_data.startup_vol, s,
						 FALSE)) >= 0)
				s->level = (byte_t) vol;

			/* Initialize sliders */
			SET_VOL_SLIDER(s->level);
			SET_BAL_SLIDER(
				(int) (s->level_right - s->level_left) / 2
			);
		}
		else if (reopen) {
			/* Force to current settings */
			(void) slioc_cfg_vol(s->level, s, FALSE);
		}
	}

	/* Read disc table of contents */
	if (err || (s->mode == MOD_NODISC && !slioc_get_toc(s))) {
		if (app_data.eject_close) {
			/* Close device */
			DBGPRN(errfp, "\nClose: %s\n", s->curdev);
			slioc_close();

			slioc_not_open = TRUE;
		}

		di_reset_curstat(s, TRUE, (bool_t) !app_data.multi_play);
		DPY_ALL(s);

		slioc_start_insert_poll(s);
		in_slioc_disc_ready = FALSE;
		return FALSE;
	}

	if (s->mode != MOD_NODISC) {
		in_slioc_disc_ready = FALSE;
		return TRUE;
	}

	s->mode = MOD_STOP;
	DPY_ALL(s);

	if (app_data.load_play || slioc_mult_autoplay) {
		slioc_mult_autoplay = FALSE;

		/* Wait a little while for things to settle */
		(void) sleep(1);

		/* Start auto-play */
		if (!slioc_override_ap)
			slioc_play_pause(s);

		if (slioc_mult_pause) {
			slioc_mult_pause = FALSE;

			if (slioc_pause_resume(FALSE)) {
				slioc_stop_stat_poll();
				s->mode = MOD_PAUSE;
				DPY_PLAYMODE(s, FALSE);
			}
		}
	}
	else if (app_data.load_spindown) {
		/* Spin down disc in case the user isn't going to
		 * play anything for a while.  This reduces wear and
		 * tear on the drive.
		 */
		slioc_start_stop(FALSE, FALSE);
	}
	else {
		switch (slioc_tst_status) {
		case MOD_PLAY:
		case MOD_PAUSE:
			/* Drive is current playing audio or paused:
			 * act appropriately.
			 */
			s->mode = slioc_tst_status;
			(void) slioc_get_playstatus(s);
			DPY_ALL(s);
			if (s->mode == MOD_PLAY)
				slioc_start_stat_poll(s);
			break;
		default:
			/* Drive is stopped: do nothing */
			break;
		}
	}

	in_slioc_disc_ready = FALSE;

	/* Load CD database entry for this disc.
	 * This operation has to be done outside the scope of
	 * in_slioc_disc_ready because it may recurse
	 * back into this function.
	 */
	DBGET(s);

	return TRUE;
}


/*
 * slioc_run_rew
 *	Run search-rewind operation
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
slioc_run_rew(curstat_t *s)
{
	int		i,
			skip_blks;
	word32_t	addr,
			end_addr;
	msf_t		smsf,
			emsf;
	static word32_t	start_addr,
			seq;

	/* Find out where we are */
	if (!slioc_get_playstatus(s)) {
		DO_BEEP();
		return;
	}

	skip_blks = app_data.skip_blks;
	addr = s->cur_tot_addr;

	if (slioc_start_search) {
		slioc_start_search = FALSE;
		seq = 0;
		i = (int) (addr - skip_blks);
	}
	else {
		if (app_data.skip_spdup > 0 && seq > app_data.skip_spdup)
			/* Speed up search */
			skip_blks *= 3;

		i = (int) (start_addr - skip_blks);
	}

	start_addr = (word32_t) (i > CLIP_FRAMES) ? i : CLIP_FRAMES;

	seq++;

	if (s->shuffle || s->program) {
		if ((i = di_curtrk_pos(s)) < 0)
			i = 0;

		if (start_addr < s->trkinfo[i].addr)
			start_addr = s->trkinfo[i].addr;
	}

	end_addr = start_addr + MAX_SRCH_BLKS;

	util_blktomsf(
		start_addr,
		&smsf.min,
		&smsf.sec,
		&smsf.frame,
		MSF_OFFSET
	);
	util_blktomsf(
		end_addr,
		&emsf.min,
		&emsf.sec,
		&emsf.frame,
		MSF_OFFSET
	);

	/* Play next search interval */
	(void) slioc_do_playaudio(
		ADDR_BLK | ADDR_MSF | ADDR_OPTEND,
		start_addr, end_addr,
		&smsf, &emsf,
		0, 0
	);

	if (di_clinfo->timeout != NULL) {
		slioc_search_id = di_clinfo->timeout(
			app_data.skip_pause,
			slioc_run_rew,
			(byte_t *) s
		);
	}
}


/*
 * slioc_stop_rew
 *	Stop search-rewind operation
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
slioc_stop_rew(curstat_t *s)
{
	if (di_clinfo->untimeout != NULL)
		di_clinfo->untimeout(slioc_search_id);
}


/*
 * slioc_run_ff
 *	Run search-fast-forward operation
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
slioc_run_ff(curstat_t *s)
{
	int		i,
			skip_blks;
	word32_t	addr,
			end_addr;
	msf_t		smsf,
			emsf;
	static word32_t	start_addr,
			seq;

	/* Find out where we are */
	if (!slioc_get_playstatus(s)) {
		DO_BEEP();
		return;
	}

	skip_blks = app_data.skip_blks;
	addr = s->cur_tot_addr;

	if (slioc_start_search) {
		slioc_start_search = FALSE;
		seq = 0;
		start_addr = addr + skip_blks;
	}
	else {
		if (app_data.skip_spdup > 0 && seq > app_data.skip_spdup)
			/* Speed up search */
			skip_blks *= 3;

		start_addr += skip_blks;
	}

	seq++;

	if (s->shuffle || s->program) {
		if ((i = di_curtrk_pos(s)) < 0)
			i = s->tot_trks - 1;
		else if (s->cur_idx == 0)
			/* We're in the lead-in: consider this to be
			 * within the previous track.
			 */
			i--;
	}
	else
		i = s->tot_trks - 1;

	end_addr = start_addr + MAX_SRCH_BLKS;

	if (end_addr >= s->trkinfo[i+1].addr) {
		end_addr = s->trkinfo[i+1].addr;
		start_addr = end_addr - skip_blks;
	}

	util_blktomsf(
		start_addr,
		&smsf.min,
		&smsf.sec,
		&smsf.frame,
		MSF_OFFSET
	);
	util_blktomsf(
		end_addr,
		&emsf.min,
		&emsf.sec,
		&emsf.frame,
		MSF_OFFSET
	);

	/* Play next search interval */
	(void) slioc_do_playaudio(
		ADDR_BLK | ADDR_MSF | ADDR_OPTEND,
		start_addr, end_addr,
		&smsf, &emsf,
		0, 0
	);

	if (di_clinfo->timeout != NULL) {
		slioc_search_id = di_clinfo->timeout(
			app_data.skip_pause,
			slioc_run_ff,
			(byte_t *) s
		);
	}
}


/*
 * slioc_stop_ff
 *	Stop search-fast-forward operation
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
slioc_stop_ff(curstat_t *s)
{
	if (di_clinfo->untimeout != NULL)
		di_clinfo->untimeout(slioc_search_id);
}


/*
 * slioc_run_ab
 *	Run a->b segment play operation
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
/*ARGSUSED*/
STATIC bool_t
slioc_run_ab(curstat_t *s)
{
	return (
		slioc_do_playaudio(
			ADDR_BLK | ADDR_MSF,
			slioc_ab_start_addr, slioc_ab_end_addr,
			&slioc_ab_start_msf, &slioc_ab_end_msf,
			0, 0
		)
	);
}


/*
 * slioc_run_sample
 *	Run sample play operation
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
slioc_run_sample(curstat_t *s)
{
	word32_t	saddr,
			eaddr;
	msf_t		smsf,
			emsf;

	if (slioc_next_sam < s->tot_trks) {
		saddr = s->trkinfo[slioc_next_sam].addr;
		eaddr = saddr + app_data.sample_blks,

		util_blktomsf(
			saddr,
			&smsf.min,
			&smsf.sec,
			&smsf.frame,
			MSF_OFFSET
		);
		util_blktomsf(
			eaddr,
			&emsf.min,
			&emsf.sec,
			&emsf.frame,
			MSF_OFFSET
		);

		if (slioc_do_playaudio(ADDR_BLK | ADDR_MSF,
				 saddr, eaddr, &smsf, &emsf, 0, 0)) {
			slioc_next_sam++;
			return TRUE;
		}
	}

	slioc_next_sam = 0;
	return FALSE;
}


/*
 * slioc_run_prog
 *	Run program/shuffle play operation
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
slioc_run_prog(curstat_t *s)
{
	sword32_t	i;
	word32_t	start_addr,
			end_addr;
	msf_t		start_msf,
			end_msf;
	bool_t		hasaudio,
			ret;

	if (!s->shuffle && !s->program)
		return FALSE;

	if (slioc_new_progshuf) {
		slioc_new_progshuf = FALSE;

		if (s->shuffle)
			/* New shuffle sequence needed */
			di_reset_shuffle(s);
		else
			/* Program play: simply reset the count */
			s->prog_cnt = 0;

		/* Do not allow a program that contains only data tracks */
		hasaudio = FALSE;
		for (i = 0; i < (int) s->prog_tot; i++) {
			if (s->trkinfo[s->playorder[i]].type == TYP_AUDIO) {
				hasaudio = TRUE;
				break;
			}
		}

		if (!hasaudio) {
			DO_BEEP();
			return FALSE;
		}
	}

	if (s->prog_cnt >= s->prog_tot)
		/* Done with program/shuffle play cycle */
		return FALSE;

	if ((i = di_curprog_pos(s)) < 0)
		return FALSE;

	if (s->trkinfo[i].trkno == CDROM_LEADOUT)
		return FALSE;

	s->prog_cnt++;
	s->cur_trk = s->trkinfo[i].trkno;
	s->cur_idx = 1;

	start_addr = s->trkinfo[i].addr + s->cur_trk_addr;
	util_blktomsf(
		start_addr,
		&s->cur_tot_min,
		&s->cur_tot_sec,
		&s->cur_tot_frame,
		MSF_OFFSET
	);
	start_msf.min = s->cur_tot_min;
	start_msf.sec = s->cur_tot_sec;
	start_msf.frame = s->cur_tot_frame;

	end_addr = s->trkinfo[i+1].addr;
	end_msf.min = s->trkinfo[i+1].min;
	end_msf.sec = s->trkinfo[i+1].sec;
	end_msf.frame = s->trkinfo[i+1].frame;

	s->cur_tot_addr = start_addr;

	if (s->mode != MOD_PAUSE)
		s->mode = MOD_PLAY;

	DPY_ALL(s);

	if (s->trkinfo[i].type == TYP_DATA)
		/* Data track: just fake it */
		return TRUE;

	ret = slioc_do_playaudio(
		ADDR_BLK | ADDR_MSF,
		start_addr, end_addr,
		&start_msf, &end_msf,
		0, 0
	);

	if (s->mode == MOD_PAUSE) {
		slioc_pause_resume(FALSE);

		/* Restore volume */
		slioc_mute_off(s);
	}

	return (ret);
}


/*
 * slioc_run_repeat
 *	Run repeat play operation
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
slioc_run_repeat(curstat_t *s)
{
	msf_t	start_msf,
		end_msf;
	bool_t	ret;

	if (!s->repeat)
		return FALSE;

	if (s->shuffle || s->program) {
		ret = TRUE;

		if (s->prog_cnt < s->prog_tot)
			/* Not done with program/shuffle sequence yet */
			return (ret);

		slioc_new_progshuf = TRUE;
		s->rptcnt++;
	}
	else {
		s->cur_trk = s->first_trk;
		s->cur_idx = 1;

		s->cur_tot_addr = 0;
		s->cur_tot_min = 0;
		s->cur_tot_sec = 0;
		s->cur_tot_frame = 0;
		s->rptcnt++;
		DPY_ALL(s);

		start_msf.min = s->trkinfo[0].min;
		start_msf.sec = s->trkinfo[0].sec;
		start_msf.frame = s->trkinfo[0].frame;
		end_msf.min = s->tot_min;
		end_msf.sec = s->tot_sec;
		end_msf.frame = s->tot_frame;

		ret = slioc_do_playaudio(
			ADDR_BLK | ADDR_MSF,
			s->trkinfo[0].addr, s->tot_addr,
			&start_msf, &end_msf, 0, 0
		);

		if (s->mode == MOD_PAUSE) {
			slioc_pause_resume(FALSE);

			/* Restore volume */
			slioc_mute_off(s);
		}

	}

	return (ret);
}


/***********************
 *   public routines   *
 ***********************/


/*
 * slioc_init
 *	Top-level function to initialize the SunOS/Solaris/Linux ioctl
 *	method.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_init(curstat_t *s, di_tbl_t *dt)
{
	if (app_data.di_method != DI_SLIOC)
		/* SunOS/Solaris/Linux/QNX ioctl method not configured */
		return;

	/* Initialize libdi calling table */
	dt->check_disc = slioc_check_disc;
	dt->status_upd = slioc_status_upd;
	dt->lock = slioc_lock;
	dt->repeat = slioc_repeat;
	dt->shuffle = slioc_shuffle;
	dt->load_eject = slioc_load_eject;
	dt->ab = slioc_ab;
	dt->sample = slioc_sample;
	dt->level = slioc_level;
	dt->play_pause = slioc_play_pause;
	dt->stop = slioc_stop;
	dt->chgdisc = slioc_chgdisc;
	dt->prevtrk = slioc_prevtrk;
	dt->nexttrk = slioc_nexttrk;
	dt->previdx = slioc_previdx;
	dt->nextidx = slioc_nextidx;
	dt->rew = slioc_rew;
	dt->ff = slioc_ff;
	dt->warp = slioc_warp;
	dt->route = NULL;
	dt->mute_on = slioc_mute_on;
	dt->mute_off = slioc_mute_off;
	dt->start = slioc_start;
	dt->icon = slioc_icon;
	dt->halt = slioc_halt;
	dt->mode = slioc_mode;
	dt->vers = slioc_vers;

	/* Hardwire some unsupported features */
	app_data.caddylock_supp = FALSE;
	app_data.caddy_lock = FALSE;
	app_data.chroute_supp = FALSE;

	/* Initalize SunOS/Solaris/Linux ioctl method */
	slioc_stat_polling = FALSE;
	slioc_stat_interval = app_data.stat_interval;
	slioc_insert_polling = FALSE;
	slioc_next_sam = FALSE;
	slioc_new_progshuf = FALSE;
	slioc_sav_end_addr = 0;
	slioc_sav_end_msf.min = slioc_sav_end_msf.sec =
		slioc_sav_end_msf.frame = 0;
	slioc_sav_end_fmt = 0;

	/* Initialize curstat structure */
	di_reset_curstat(s, TRUE, TRUE);

	/* Sanity check CD changer configuration */
	if (app_data.numdiscs > 1) {
#ifdef linux
		if (app_data.chg_method != CHG_OS_IOCTL) {
			app_data.numdiscs = 1;
			DBGPRN(errfp,
				"CD changer: invalid method %d:\n%s\n%s\n",
				app_data.chg_method,
				"Only CHG_OS_IOCTL is supported in this mode.",
				"Setting to single disc mode.");
			DI_INFO(app_data.str_medchg_noinit);
		}
#else
		/* Changer support is only for Linux */
		app_data.numdiscs = 1;
		DBGPRN(errfp, "CD changer: only supported on Linux:\n%s\n",
			"Setting to single disc mode.");
#endif
		/* Configuration error: Set to * single-disc mode. */
		if (app_data.numdiscs == 1) {
			app_data.chg_method = CHG_NONE;
			app_data.multi_play = FALSE;
			app_data.reverse = FALSE;
			s->first_disc = s->last_disc = s->cur_disc = 1;
		}
	}

#ifdef linux
	DBGPRN(errfp, "libdi: Linux ioctl method\n");
#endif
#ifdef sun
	DBGPRN(errfp, "libdi: SunOS/Solaris ioctl method\n");
#endif
#ifdef __QNX__
	DBGPRN(errfp, "libdi: QNX ioctl method\n");
#endif
}


/*
 * slioc_check_disc
 *	Check if disc is ready for use
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
slioc_check_disc(curstat_t *s)
{
	return (slioc_disc_ready(s));
}


/*
 * slioc_status_upd
 *	Force update of playback status
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_status_upd(curstat_t *s)
{
	(void) slioc_get_playstatus(s);
}


/*
 * slioc_lock
 *	Caddy lock function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *	enable - whether to enable/disable caddy lock
 *
 * Return:
 *	Nothing.
 */
void
slioc_lock(curstat_t *s, bool_t enable)
{
	/* Caddy lock function currently not supported
	 * under SunOS/Solaris/Linux ioctl method
	 */
	if (enable) {
		DO_BEEP();
		SET_LOCK_BTN(FALSE);
	}
}


/*
 * slioc_repeat
 *	Repeat mode function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *	enable - whether to enable/disable repeat mode
 *
 * Return:
 *	Nothing.
 */
void
slioc_repeat(curstat_t *s, bool_t enable)
{
	s->repeat = enable;

	if (!enable && slioc_new_progshuf) {
		slioc_new_progshuf = FALSE;
		if (s->rptcnt > 0)
			s->rptcnt--;
	}
	DPY_RPTCNT(s);
}


/*
 * slioc_shuffle
 *	Shuffle mode function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *	enable - whether to enable/disable shuffle mode
 *
 * Return:
 *	Nothing.
 */
void
slioc_shuffle(curstat_t *s, bool_t enable)
{
	switch (s->mode) {
	case MOD_STOP:
	case MOD_BUSY:
	case MOD_NODISC:
		if (s->program) {
			/* Currently in program mode: can't enable shuffle */
			DO_BEEP();
			SET_SHUFFLE_BTN((bool_t) !enable);
			return;
		}
		break;
	default:
		if (enable) {
			/* Can't enable shuffle unless when stopped */
			DO_BEEP();
			SET_SHUFFLE_BTN((bool_t) !enable);
			return;
		}
		break;
	}

	s->shuffle = enable;
	if (!s->shuffle)
		s->prog_tot = 0;
}


/*
 * slioc_load_eject
 *	CD caddy load and eject function.  If disc caddy is not
 *	loaded, it will attempt to load it.  Otherwise, it will be
 *	ejected.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_load_eject(curstat_t *s)
{
	if (!slioc_disc_ready(s) && app_data.load_supp) {
		/* Disc not ready: try loading the disc */
		if (!slioc_start_stop(TRUE, TRUE))
			DO_BEEP();

		if (slioc_disc_ready(s) || !app_data.eject_supp)
			return;
	}

	/* Eject the disc */

	if (!app_data.eject_supp) {
		DO_BEEP();

		slioc_stop_stat_poll();
		di_reset_curstat(s, TRUE, TRUE);
		s->mode = MOD_NODISC;

		DBCLEAR(s, FALSE);
		DPY_ALL(s);

		if (app_data.eject_close) {
			/* Close device */
			DBGPRN(errfp, "\nClose: %s\n", s->curdev);
			slioc_close();

			slioc_not_open = TRUE;
		}

		slioc_start_insert_poll(s);
		return;
	}

	slioc_stop_stat_poll();
	di_reset_curstat(s, TRUE, TRUE);
	s->mode = MOD_NODISC;

	DBCLEAR(s, FALSE);
	DPY_ALL(s);

	(void) slioc_start_stop(FALSE, TRUE);

	if (app_data.eject_exit)
		di_clinfo->quit(s);
	else {
		if (app_data.eject_close) {
			/* Close device */
			DBGPRN(errfp, "\nClose: %s\n", s->curdev);
			slioc_close();

			slioc_not_open = TRUE;
		}
			
		slioc_start_insert_poll(s);
	}
}


/*
 * slioc_ab
 *	A->B segment play mode function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_ab(curstat_t *s)
{
	switch (s->mode) {
	case MOD_SAMPLE:
	case MOD_PLAY:
		/* Get current location */
		if (!slioc_get_playstatus(s)) {
			DO_BEEP();
			break;
		}

		slioc_ab_start_addr = s->cur_tot_addr;
		slioc_ab_start_msf.min = s->cur_tot_min;
		slioc_ab_start_msf.sec = s->cur_tot_sec;
		slioc_ab_start_msf.frame = s->cur_tot_frame;

		s->mode = MOD_A;
		DPY_PLAYMODE(s, FALSE);
		break;

	case MOD_A:
		/* Get current location */
		if (!slioc_get_playstatus(s)) {
			DO_BEEP();
			break;
		}

		slioc_ab_end_addr = s->cur_tot_addr;
		slioc_ab_end_msf.min = s->cur_tot_min;
		slioc_ab_end_msf.sec = s->cur_tot_sec;
		slioc_ab_end_msf.frame = s->cur_tot_frame;

		/* Make sure that the A->B play interval is no less
		 * than a user-configurable minimum.
		 */
		if ((slioc_ab_end_addr - slioc_ab_start_addr) <
		    app_data.min_playblks) {
			slioc_ab_end_addr = slioc_ab_start_addr +
					    app_data.min_playblks;
			util_blktomsf(
				slioc_ab_end_addr,
				&slioc_ab_end_msf.min,
				&slioc_ab_end_msf.sec,
				&slioc_ab_end_msf.frame,
				MSF_OFFSET
			);
		}

		if (!slioc_run_ab(s)) {
			DO_BEEP();
			return;
		}

		s->mode = MOD_AB;
		DPY_PLAYMODE(s, FALSE);
		break;

	case MOD_AB:
		/* Currently doing A->B playback, just call slioc_play_pause
		 * to resume normal playback.
		 */
		slioc_play_pause(s);
		break;

	default:
		DO_BEEP();
		break;
	}
}


/*
 * slioc_sample
 *	Sample play mode function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_sample(curstat_t *s)
{
	int	i;

	if (!slioc_disc_ready(s)) {
		DO_BEEP();
		return;
	}

	if (s->shuffle || s->program) {
		/* Sample is not supported in program/shuffle mode */
		DO_BEEP();
		return;
	}

	switch (s->mode) {
	case MOD_STOP:
		slioc_start_stat_poll(s);
		/*FALLTHROUGH*/
	case MOD_A:
	case MOD_AB:
	case MOD_PLAY:
		/* If already playing a track, start sampling the track after
		 * the current one.  Otherwise, sample from the beginning.
		 */
		if (s->cur_trk > 0 && s->cur_trk != s->last_trk) {
			i = di_curtrk_pos(s) + 1;
			s->cur_trk = s->trkinfo[i].trkno;
			slioc_next_sam = (byte_t) i;
		}
		else {
			s->cur_trk = s->first_trk;
			slioc_next_sam = 0;
		}
		
		s->cur_idx = 1;

		s->mode = MOD_SAMPLE;
		DPY_ALL(s);

		if (!slioc_run_sample(s))
			return;

		break;

	case MOD_SAMPLE:
		/* Currently doing Sample playback, just call slioc_play_pause
		 * to resume normal playback.
		 */
		slioc_play_pause(s);
		break;

	default:
		DO_BEEP();
		break;
	}
}


/*
 * slioc_level
 *	Audio volume control function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *	level - The volume level to set to
 *	drag - Whether this is an update due to the user dragging the
 *		volume control slider thumb.  If this is FALSE, then
 *		a final volume setting has been found.
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
void
slioc_level(curstat_t *s, byte_t level, bool_t drag)
{
	int	actual;

	/* Set volume level */
	if ((actual = slioc_cfg_vol((int) level, s, FALSE)) >= 0)
		s->level = (byte_t) actual;
}


/*
 * slioc_play_pause
 *	Audio playback and pause function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_play_pause(curstat_t *s)
{
	sword32_t	i;
	word32_t	start_addr;
	msf_t		start_msf,
			end_msf;

	slioc_override_ap = TRUE;

	if (!slioc_disc_ready(s)) {
		slioc_override_ap = FALSE;
		DO_BEEP();
		return;
	}

	slioc_override_ap = FALSE;

	if (s->mode == MOD_NODISC)
		s->mode = MOD_STOP;

	switch (s->mode) {
	case MOD_PLAY:
		/* Currently playing: go to pause mode */

		if (!slioc_pause_resume(FALSE)) {
			DO_BEEP();
			return;
		}
		slioc_stop_stat_poll();
		s->mode = MOD_PAUSE;
		DPY_PLAYMODE(s, FALSE);
		break;

	case MOD_PAUSE:
		/* Currently paused: resume play */

		if (!slioc_pause_resume(TRUE)) {
			DO_BEEP();
			return;
		}
		s->mode = MOD_PLAY;
		DPY_PLAYMODE(s, FALSE);
		slioc_start_stat_poll(s);
		break;

	case MOD_STOP:
		/* Currently stopped: start play */

		if (s->shuffle || s->program) {
			slioc_new_progshuf = TRUE;

			/* Start shuffle/program play */
			if (!slioc_run_prog(s))
				return;
		}
		else {
			/* Start normal play */
			if ((i = di_curtrk_pos(s)) < 0 || s->cur_trk <= 0) {
				/* Start play from the beginning */
				i = 0;
				s->cur_trk = s->first_trk;
				start_addr = s->trkinfo[0].addr +
					     s->cur_trk_addr;
				util_blktomsf(
					start_addr,
					&start_msf.min,
					&start_msf.sec,
					&start_msf.frame,
					MSF_OFFSET
				);
			}
			else {
				/* User has specified a starting track */
				start_addr = s->trkinfo[i].addr +
					     s->cur_trk_addr;
			}

			util_blktomsf(
				start_addr,
				&start_msf.min,
				&start_msf.sec,
				&start_msf.frame,
				MSF_OFFSET
			);

			end_msf.min = s->tot_min;
			end_msf.sec = s->tot_sec;
			end_msf.frame = s->tot_frame;

			if (s->trkinfo[i].type == TYP_DATA) {
				DPY_TRACK(s);
				DPY_TIME(s, FALSE);
				DO_BEEP();
				return;
			}

			s->cur_idx = 1;
			s->mode = MOD_PLAY;

			if (!slioc_do_playaudio(ADDR_BLK | ADDR_MSF,
					  start_addr, s->tot_addr,
					  &start_msf, &end_msf, 0, 0)) {
				DO_BEEP();
				s->mode = MOD_STOP;
				return;
			}
		}

		DPY_ALL(s);
		slioc_start_stat_poll(s);
		break;

	case MOD_A:
		/* Just reset mode to play and continue */
		s->mode = MOD_PLAY;
		DPY_PLAYMODE(s, FALSE);
		break;

	case MOD_AB:
	case MOD_SAMPLE:
		/* Force update of curstat */
		if (!slioc_get_playstatus(s)) {
			DO_BEEP();
			return;
		}

		/* Currently doing a->b or sample playback: just resume play */
		if (s->shuffle || s->program) {
			if ((i = di_curtrk_pos(s)) < 0 ||
			    s->trkinfo[i].trkno == CDROM_LEADOUT)
				return;

			start_msf.min = s->cur_tot_min;
			start_msf.sec = s->cur_tot_sec;
			start_msf.frame = s->cur_tot_frame;
			end_msf.min = s->trkinfo[i+1].min;
			end_msf.sec = s->trkinfo[i+1].sec;
			end_msf.frame = s->trkinfo[i+1].frame;

			if (!slioc_do_playaudio(ADDR_BLK | ADDR_MSF,
					  s->cur_tot_addr,
					  s->trkinfo[i+1].addr,
					  &start_msf, &end_msf, 0, 0)) {
				DO_BEEP();
				return;
			}
		}
		else {
			start_msf.min = s->cur_tot_min;
			start_msf.sec = s->cur_tot_sec;
			start_msf.frame = s->cur_tot_frame;
			end_msf.min = s->tot_min;
			end_msf.sec = s->tot_sec;
			end_msf.frame = s->tot_frame;

			if (!slioc_do_playaudio(ADDR_BLK | ADDR_MSF,
					  s->cur_tot_addr, s->tot_addr,
					  &start_msf, &end_msf, 0, 0)) {
				DO_BEEP();
				return;
			}
		}
		s->mode = MOD_PLAY;
		DPY_PLAYMODE(s, FALSE);
		break;

	default:
		DO_BEEP();
		break;
	}
}


/*
 * slioc_stop
 *	Stop function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *	stop_disc - Whether to actually spin down the disc or just
 *		update status.
 *
 * Return:
 *	Nothing.
 */
void
slioc_stop(curstat_t *s, bool_t stop_disc)
{
	/* The stop_disc parameter will cause the disc to spin down.
	 * This is usually set to TRUE, but can be FALSE if the caller
	 * just wants to set the current state to stop but will
	 * immediately go into play state again.  Not spinning down
	 * the drive makes things a little faster...
	 */
	if (!slioc_disc_ready(s)) {
		DO_BEEP();
		return;
	}

	switch (s->mode) {
	case MOD_PLAY:
	case MOD_PAUSE:
	case MOD_A:
	case MOD_AB:
	case MOD_SAMPLE:
	case MOD_STOP:
		/* Currently playing or paused: stop */

		if (stop_disc && !slioc_start_stop(FALSE, FALSE)) {
			DO_BEEP();
			return;
		}
		slioc_stop_stat_poll();

		di_reset_curstat(s, FALSE, FALSE);
		s->mode = MOD_STOP;
		s->rptcnt = 0;

		DPY_ALL(s);
		break;

	default:
		DO_BEEP();
		break;
	}
}


/*
 * slioc_chgdisc
 *	Change disc function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
void
slioc_chgdisc(curstat_t *s)
{
	int	sav_rptcnt;

	if (s->first_disc == s->last_disc) {
		/* Single-CD drive: cannot change discs */
		DO_BEEP();
		return;
	}

	if (s->cur_disc < s->first_disc || s->cur_disc > s->last_disc) {
		/* Bogus disc number */
		s->cur_disc = s->first_disc;
		return;
	}

	/* If we're currently in normal playback mode, after we change
	 * disc we want to automatically start playback.
	 */
	if ((s->mode == MOD_PLAY || s->mode == MOD_PAUSE) && !s->program)
		slioc_mult_autoplay = TRUE;

	/* If we're currently paused, go to pause mode after disc change */
	slioc_mult_pause = (s->mode == MOD_PAUSE);

	sav_rptcnt = s->rptcnt;

	/* Stop the CD first */
	slioc_stop(s, TRUE);

	di_reset_curstat(s, TRUE, FALSE);
	s->mode = MOD_NODISC;
	DBCLEAR(s, FALSE);

	s->rptcnt = sav_rptcnt;

	/* Close CD-ROM device */
	DBGPRN(errfp, "\nClose CD-ROM: %s\n", s->curdev);
	slioc_close();

	slioc_not_open = TRUE;

	/* Load desired disc */
	slioc_disc_ready(s);
}


/*
 * slioc_prevtrk
 *	Previous track function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_prevtrk(curstat_t *s)
{
	sword32_t	i;
	word32_t	start_addr;
	msf_t		start_msf,
			end_msf;
	bool_t		go_prev;

	if (!slioc_disc_ready(s)) {
		DO_BEEP();
		return;
	}

	switch (s->mode) {
	case MOD_A:
	case MOD_AB:
	case MOD_SAMPLE:
		s->mode = MOD_PLAY;
		DPY_PLAYMODE(s, FALSE);
		/*FALLTHROUGH*/
	case MOD_PLAY:
	case MOD_PAUSE:
		/* Find appropriate track to start */
		if (s->shuffle || s->program) {
			if (s->prog_cnt > 0) {
				s->prog_cnt--;
				slioc_new_progshuf = FALSE;
			}
			i = di_curprog_pos(s);
		}
		else
			i = di_curtrk_pos(s);

		go_prev = FALSE;

		if (i == 0 && s->cur_idx == 0) {
			i = 0;
			start_addr = CLIP_FRAMES;
			start_msf.min = 0;
			start_msf.sec = 2;
			start_msf.frame = CLIP_FRAMES;
			s->cur_trk = s->trkinfo[i].trkno;
			s->cur_idx = 0;
		}
		else {
			start_addr = s->trkinfo[i].addr;
			start_msf.min = s->trkinfo[i].min;
			start_msf.sec = s->trkinfo[i].sec;
			start_msf.frame = s->trkinfo[i].frame;
			s->cur_trk = s->trkinfo[i].trkno;
			s->cur_idx = 1;

			/* If the current track has been playing for less
			 * than app_data.prev_threshold blocks, then go
			 * to the beginning of the previous track (if we
			 * are not already on the first track).
			 */
			if ((s->cur_tot_addr - start_addr) <=
			    app_data.prev_threshold)
				go_prev = TRUE;
		}

		if (go_prev) {
			if (s->shuffle || s->program) {
				if (s->prog_cnt > 0) {
					s->prog_cnt--;
					slioc_new_progshuf = FALSE;
				}
				if ((i = di_curprog_pos(s)) < 0)
					return;

				start_addr = s->trkinfo[i].addr;
				start_msf.min = s->trkinfo[i].min;
				start_msf.sec = s->trkinfo[i].sec;
				start_msf.frame = s->trkinfo[i].frame;
				s->cur_trk = s->trkinfo[i].trkno;
			}
			else if (i == 0) {
				/* Go to the very beginning: this may be
				 * a lead-in area before the start of track 1.
				 */
				start_addr = CLIP_FRAMES;
				start_msf.min = 0;
				start_msf.sec = 2;
				start_msf.frame = CLIP_FRAMES;
				s->cur_trk = s->trkinfo[i].trkno;
			}
			else if (i > 0) {
				i--;

				/* Skip over data tracks */
				while (s->trkinfo[i].type == TYP_DATA) {
					if (i <= 0)
						break;
					i--;
				}

				if (s->trkinfo[i].type != TYP_DATA) {
					start_addr = s->trkinfo[i].addr;
					start_msf.min = s->trkinfo[i].min;
					start_msf.sec = s->trkinfo[i].sec;
					start_msf.frame = s->trkinfo[i].frame;
					s->cur_trk = s->trkinfo[i].trkno;
				}
			}
		}

		if (s->mode == MOD_PAUSE)
			/* Mute: so we don't get a transient */
			slioc_mute_on(s);

		if (s->shuffle || s->program) {
			/* Program/Shuffle mode: just stop the playback
			 * and let slioc_run_prog go to the previous track
			 */
			slioc_fake_stop = TRUE;

			/* Force status update */
			(void) slioc_get_playstatus(s);
		}
		else {
			end_msf.min = s->tot_min;
			end_msf.sec = s->tot_sec;
			end_msf.frame = s->tot_frame;

			s->cur_tot_addr = start_addr;
			s->cur_tot_min = start_msf.min;
			s->cur_tot_sec = start_msf.sec;
			s->cur_tot_frame = start_msf.frame;
			s->cur_trk_addr = 0;
			s->cur_trk_min = s->cur_trk_sec = s->cur_trk_frame = 0;

			DPY_TRACK(s);
			DPY_INDEX(s);
			DPY_TIME(s, FALSE);

			if (!slioc_do_playaudio(ADDR_BLK | ADDR_MSF,
					  start_addr, s->tot_addr,
					  &start_msf, &end_msf, 0, 0)) {
				DO_BEEP();

				/* Restore volume */
				slioc_mute_off(s);
				return;
			}

			if (s->mode == MOD_PAUSE) {
				slioc_pause_resume(FALSE);

				/* Restore volume */
				slioc_mute_off(s);
			}
		}

		break;

	case MOD_STOP:
		if (s->shuffle || s->program) {
			/* Pre-selecting tracks not supported in shuffle
			 * or program mode.
			 */
			DO_BEEP();
			return;
		}

		/* Find previous track */
		if (s->cur_trk <= 0) {
			s->cur_trk = s->trkinfo[0].trkno;
			DPY_TRACK(s);
		}
		else {
			i = di_curtrk_pos(s);

			if (i > 0) {
				s->cur_trk = s->trkinfo[i-1].trkno;
				DPY_TRACK(s);
			}
		}
		break;

	default:
		DO_BEEP();
		break;
	}
}


/*
 * slioc_nexttrk
 *	Next track function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_nexttrk(curstat_t *s)
{
	sword32_t	i;
	word32_t	start_addr;
	msf_t		start_msf,
			end_msf;

	if (!slioc_disc_ready(s)) {
		DO_BEEP();
		return;
	}

	switch (s->mode) {
	case MOD_A:
	case MOD_AB:
	case MOD_SAMPLE:
		s->mode = MOD_PLAY;
		DPY_PLAYMODE(s, FALSE);
		/*FALLTHROUGH*/
	case MOD_PLAY:
	case MOD_PAUSE:
		if (s->shuffle || s->program) {
			if (s->prog_cnt >= s->prog_tot) {
				/* Disallow advancing beyond current
				 * shuffle/program sequence if
				 * repeat mode is not on.
				 */
				if (s->repeat && !app_data.multi_play)
					slioc_new_progshuf = TRUE;
				else
					return;
			}

			if (s->mode == MOD_PAUSE)
				/* Mute: so we don't get a transient */
				slioc_mute_on(s);

			/* Program/Shuffle mode: just stop the playback
			 * and let slioc_run_prog go to the next track.
			 */
			slioc_fake_stop = TRUE;

			/* Force status update */
			(void) slioc_get_playstatus(s);

			return;
		}

		/* Find next track */
		if ((i = di_curtrk_pos(s)) < 0)
			return;

		if (i > 0 || s->cur_idx > 0)
			i++;

		/* Skip over data tracks */
		while (i < MAXTRACK && s->trkinfo[i].type == TYP_DATA)
			i++;

		if (i < MAXTRACK &&
		    s->trkinfo[i].trkno >= 0 &&
		    s->trkinfo[i].trkno != CDROM_LEADOUT) {

			start_addr = s->trkinfo[i].addr;
			start_msf.min = s->trkinfo[i].min;
			start_msf.sec = s->trkinfo[i].sec;
			start_msf.frame = s->trkinfo[i].frame;
			s->cur_trk = s->trkinfo[i].trkno;
			s->cur_idx = 1;

			if (s->mode == MOD_PAUSE)
				/* Mute: so we don't get a transient */
				slioc_mute_on(s);

			end_msf.min = s->tot_min;
			end_msf.sec = s->tot_sec;
			end_msf.frame = s->tot_frame;

			s->cur_tot_addr = start_addr;
			s->cur_tot_min = start_msf.min;
			s->cur_tot_sec = start_msf.sec;
			s->cur_tot_frame = start_msf.frame;
			s->cur_trk_addr = 0;
			s->cur_trk_min = s->cur_trk_sec = s->cur_trk_frame = 0;

			DPY_TRACK(s);
			DPY_INDEX(s);
			DPY_TIME(s, FALSE);

			if (!slioc_do_playaudio(ADDR_BLK | ADDR_MSF,
					  start_addr, s->tot_addr,
					  &start_msf, &end_msf, 0, 0)) {
				DO_BEEP();
				return;
			}

			if (s->mode == MOD_PAUSE) {
				slioc_pause_resume(FALSE);

				/* Restore volume */
				slioc_mute_off(s);
			}
		}

		break;

	case MOD_STOP:
		if (s->shuffle || s->program) {
			/* Pre-selecting tracks not supported in shuffle
			 * or program mode.
			 */
			DO_BEEP();
			return;
		}

		/* Find next track */
		if (s->cur_trk <= 0) {
			s->cur_trk = s->trkinfo[0].trkno;
			DPY_TRACK(s);
		}
		else {
			i = di_curtrk_pos(s) + 1;

			if (i > 0 && s->trkinfo[i].trkno != CDROM_LEADOUT) {
				s->cur_trk = s->trkinfo[i].trkno;
				DPY_TRACK(s);
			}
		}
		break;

	default:
		DO_BEEP();
		break;
	}
}


/*
 * slioc_previdx
 *	Previous index function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_previdx(curstat_t *s)
{
	msf_t		start_msf,
			end_msf;
	byte_t		idx;

	if (s->shuffle || s->program) {
		/* Index search is not supported in program/shuffle mode */
		DO_BEEP();
		return;
	}

	switch (s->mode) {
	case MOD_A:
	case MOD_AB:
	case MOD_SAMPLE:
		s->mode = MOD_PLAY;
		DPY_PLAYMODE(s, FALSE);
		/*FALLTHROUGH*/
	case MOD_PLAY:
	case MOD_PAUSE:
		/* Find appropriate index to start */
		if (s->cur_idx > 1 &&
		    (s->cur_tot_addr - s->sav_iaddr) <= app_data.prev_threshold)
			idx = s->cur_idx - 1;
		else
			idx = s->cur_idx;
		
		/* This is a Hack...
		 * Since there is no standard command to start
		 * playback on an index boundary and then go on playing
		 * until the end of the disc, we will use the PLAY AUDIO
		 * TRACK/INDEX command to go to where we want to start,
		 * immediately followed by a PAUSE.  We then find the
		 * current block position and issue a PLAY AUDIO MSF
		 * or PLAY AUDIO(12) command to start play there.
		 * We mute the audio in between these operations to
		 * prevent unpleasant transients.
		 */

		/* Mute */
		slioc_mute_on(s);

		if (!slioc_do_playaudio(ADDR_TRKIDX, 0, 0, NULL, NULL,
				  (byte_t) s->cur_trk, idx)) {
			/* Restore volume */
			slioc_mute_off(s);
			DO_BEEP();
			return;
		}

		slioc_idx_pause = TRUE;

		if (!slioc_pause_resume(FALSE)) {
			/* Restore volume */
			slioc_mute_off(s);
			slioc_idx_pause = FALSE;
			return;
		}

		/* Use slioc_get_playstatus to update the current status */
		if (!slioc_get_playstatus(s)) {
			/* Restore volume */
			slioc_mute_off(s);
			slioc_idx_pause = FALSE;
			return;
		}

		/* Save starting block addr of this index */
		s->sav_iaddr = s->cur_tot_addr;

		if (s->mode != MOD_PAUSE)
			/* Restore volume */
			slioc_mute_off(s);

		start_msf.min = s->cur_tot_min;
		start_msf.sec = s->cur_tot_sec;
		start_msf.frame = s->cur_tot_frame;
		end_msf.min = s->tot_min;
		end_msf.sec = s->tot_sec;
		end_msf.frame = s->tot_frame;

		if (!slioc_do_playaudio(ADDR_BLK | ADDR_MSF,
				  s->cur_tot_addr, s->tot_addr,
				  &start_msf, &end_msf, 0, 0)) {
			DO_BEEP();
			slioc_idx_pause = FALSE;
			return;
		}

		slioc_idx_pause = FALSE;

		if (s->mode == MOD_PAUSE) {
			slioc_pause_resume(FALSE);

			/* Restore volume */
			slioc_mute_off(s);

			/* Force update of curstat */
			(void) slioc_get_playstatus(s);
		}

		break;

	default:
		DO_BEEP();
		break;
	}
}


/*
 * slioc_nextidx
 *	Next index function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_nextidx(curstat_t *s)
{
	msf_t		start_msf,
			end_msf;

	if (s->shuffle || s->program) {
		/* Index search is not supported in program/shuffle mode */
		DO_BEEP();
		return;
	}

	switch (s->mode) {
	case MOD_A:
	case MOD_AB:
	case MOD_SAMPLE:
		s->mode = MOD_PLAY;
		DPY_PLAYMODE(s, FALSE);
		/*FALLTHROUGH*/
	case MOD_PLAY:
	case MOD_PAUSE:
		/* Find appropriate index to start */
		
		/* This is a Hack...
		 * Since there is no standard command to start
		 * playback on an index boundary and then go on playing
		 * until the end of the disc, we will use the PLAY AUDIO
		 * TRACK/INDEX command to go to where we want to start,
		 * immediately followed by a PAUSE.  We then find the
		 * current block position and issue a PLAY AUDIO MSF
		 * or PLAY AUDIO(12) command to start play there.
		 * We mute the audio in between these operations to
		 * prevent unpleasant transients.
		 */

		/* Mute */
		slioc_mute_on(s);

		if (!slioc_do_playaudio(ADDR_TRKIDX, 0, 0, NULL, NULL,
				  (byte_t) s->cur_trk,
				  (byte_t) (s->cur_idx + 1))) {
			/* Restore volume */
			slioc_mute_off(s);
			DO_BEEP();
			return;
		}

		slioc_idx_pause = TRUE;

		if (!slioc_pause_resume(FALSE)) {
			/* Restore volume */
			slioc_mute_off(s);
			slioc_idx_pause = FALSE;
			return;
		}

		/* Use slioc_get_playstatus to update the current status */
		if (!slioc_get_playstatus(s)) {
			/* Restore volume */
			slioc_mute_off(s);
			slioc_idx_pause = FALSE;
			return;
		}

		/* Save starting block addr of this index */
		s->sav_iaddr = s->cur_tot_addr;

		if (s->mode != MOD_PAUSE)
			/* Restore volume */
			slioc_mute_off(s);

		start_msf.min = s->cur_tot_min;
		start_msf.sec = s->cur_tot_sec;
		start_msf.frame = s->cur_tot_frame;
		end_msf.min = s->tot_min;
		end_msf.sec = s->tot_sec;
		end_msf.frame = s->tot_frame;

		if (!slioc_do_playaudio(ADDR_BLK | ADDR_MSF,
				  s->cur_tot_addr, s->tot_addr,
				  &start_msf, &end_msf, 0, 0)) {
			DO_BEEP();
			slioc_idx_pause = FALSE;
			return;
		}

		slioc_idx_pause = FALSE;

		if (s->mode == MOD_PAUSE) {
			slioc_pause_resume(FALSE);

			/* Restore volume */
			slioc_mute_off(s);

			/* Force update of curstat */
			(void) slioc_get_playstatus(s);
		}

		break;

	default:
		DO_BEEP();
		break;
	}
}


/*
 * slioc_rew
 *	Search-rewind function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_rew(curstat_t *s, bool_t start)
{
	sword32_t	i;
	msf_t		start_msf,
			end_msf;
	byte_t		vol;

	switch (s->mode) {
	case MOD_A:
	case MOD_AB:
	case MOD_SAMPLE:
		/* Go to normal play mode first */
		slioc_play_pause(s);

		/*FALLTHROUGH*/
	case MOD_PLAY:
	case MOD_PAUSE:
		if (start) {
			/* Button press */

			if (s->mode == MOD_PLAY)
				slioc_stop_stat_poll();

			/* Reduce volume */
			vol = (byte_t) ((int) s->level *
				app_data.skip_vol / 100);

			(void) slioc_cfg_vol((int)
				((vol < (byte_t)app_data.skip_minvol) ?
				 (byte_t) app_data.skip_minvol : vol),
				s,
				FALSE
			);

			/* Start search rewind */
			slioc_start_search = TRUE;
			slioc_run_rew(s);
		}
		else {
			/* Button release */

			slioc_stop_rew(s);

			/* Update display */
			(void) slioc_get_playstatus(s);

			if (s->mode == MOD_PAUSE)
				/* Mute: so we don't get a transient */
				slioc_mute_on(s);
			else
				/* Restore volume */
				slioc_mute_off(s);

			if (s->shuffle || s->program) {
				if ((i = di_curtrk_pos(s)) < 0 ||
				    s->trkinfo[i].trkno == CDROM_LEADOUT) {
					/* Restore volume */
					slioc_mute_off(s);
					return;
				}

				start_msf.min = s->cur_tot_min;
				start_msf.sec = s->cur_tot_sec;
				start_msf.frame = s->cur_tot_frame;
				end_msf.min = s->trkinfo[i+1].min;
				end_msf.sec = s->trkinfo[i+1].sec;
				end_msf.frame = s->trkinfo[i+1].frame;

				if (!slioc_do_playaudio(ADDR_BLK | ADDR_MSF,
						  s->cur_tot_addr,
						  s->trkinfo[i+1].addr,
						  &start_msf, &end_msf,
						  0, 0)) {
					DO_BEEP();

					/* Restore volume */
					slioc_mute_off(s);
					return;
				}
			}
			else {
				start_msf.min = s->cur_tot_min;
				start_msf.sec = s->cur_tot_sec;
				start_msf.frame = s->cur_tot_frame;
				end_msf.min = s->tot_min;
				end_msf.sec = s->tot_sec;
				end_msf.frame = s->tot_frame;

				if (!slioc_do_playaudio(ADDR_BLK | ADDR_MSF,
						  s->cur_tot_addr,
						  s->tot_addr,
						  &start_msf, &end_msf,
						  0, 0)) {
					DO_BEEP();

					/* Restore volume */
					slioc_mute_off(s);
					return;
				}
			}

			if (s->mode == MOD_PAUSE) {
				slioc_pause_resume(FALSE);

				/* Restore volume */
				slioc_mute_off(s);
			}
			else
				slioc_start_stat_poll(s);
		}
		break;

	default:
		if (start)
			DO_BEEP();
		break;
	}
}


/*
 * slioc_ff
 *	Search-fast-forward function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_ff(curstat_t *s, bool_t start)
{
	sword32_t	i;
	msf_t		start_msf,
			end_msf;
	byte_t		vol;

	switch (s->mode) {
	case MOD_A:
	case MOD_AB:
	case MOD_SAMPLE:
		/* Go to normal play mode first */
		slioc_play_pause(s);

		/*FALLTHROUGH*/
	case MOD_PLAY:
	case MOD_PAUSE:
		if (start) {
			/* Button press */

			if (s->mode == MOD_PLAY)
				slioc_stop_stat_poll();

			/* Reduce volume */
			vol = (byte_t) ((int) s->level *
				app_data.skip_vol / 100);

			(void) slioc_cfg_vol((int)
				((vol < (byte_t)app_data.skip_minvol) ?
				 (byte_t) app_data.skip_minvol : vol),
				s,
				FALSE
			);

			/* Start search forward */
			slioc_start_search = TRUE;
			slioc_run_ff(s);
		}
		else {
			/* Button release */

			slioc_stop_ff(s);

			/* Update display */
			(void) slioc_get_playstatus(s);

			if (s->mode == MOD_PAUSE)
				/* Mute: so we don't get a transient */
				slioc_mute_on(s);
			else
				/* Restore volume */
				slioc_mute_off(s);

			if (s->shuffle || s->program) {
				if ((i = di_curtrk_pos(s)) < 0 ||
				    s->trkinfo[i].trkno == CDROM_LEADOUT) {
					/* Restore volume */
					slioc_mute_off(s);
					return;
				}

				start_msf.min = s->cur_tot_min;
				start_msf.sec = s->cur_tot_sec;
				start_msf.frame = s->cur_tot_frame;
				end_msf.min = s->trkinfo[i+1].min;
				end_msf.sec = s->trkinfo[i+1].sec;
				end_msf.frame = s->trkinfo[i+1].frame;

				if (!slioc_do_playaudio(ADDR_BLK | ADDR_MSF,
						  s->cur_tot_addr,
						  s->trkinfo[i+1].addr,
						  &start_msf, &end_msf,
						  0, 0)) {
					DO_BEEP();

					/* Restore volume */
					slioc_mute_off(s);
					return;
				}
			}
			else {
				start_msf.min = s->cur_tot_min;
				start_msf.sec = s->cur_tot_sec;
				start_msf.frame = s->cur_tot_frame;
				end_msf.min = s->tot_min;
				end_msf.sec = s->tot_sec;
				end_msf.frame = s->tot_frame;

				if (!slioc_do_playaudio(ADDR_BLK | ADDR_MSF,
						  s->cur_tot_addr,
						  s->tot_addr,
						  &start_msf, &end_msf,
						  0, 0)) {
					DO_BEEP();

					/* Restore volume */
					slioc_mute_off(s);
					return;
				}
			}
			if (s->mode == MOD_PAUSE) {
				slioc_pause_resume(FALSE);

				/* Restore volume */
				slioc_mute_off(s);
			}
			else
				slioc_start_stat_poll(s);
		}
		break;

	default:
		if (start)
			DO_BEEP();
		break;
	}
}


/*
 * slioc_warp
 *	Track warp function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_warp(curstat_t *s)
{
	word32_t	start_addr,
			end_addr;
	msf_t		start_msf,
			end_msf;
	int		i;

	start_addr = s->cur_tot_addr;
	start_msf.min = s->cur_tot_min;
	start_msf.sec = s->cur_tot_sec;
	start_msf.frame = s->cur_tot_frame;

	switch (s->mode) {
	case MOD_A:
	case MOD_AB:
	case MOD_SAMPLE:
		/* Go to normal play mode first */
		slioc_play_pause(s);

		/*FALLTHROUGH*/
	case MOD_PLAY:
	case MOD_PAUSE:
		if (s->shuffle || s->program) {
			if ((i = di_curtrk_pos(s)) < 0) {
				DO_BEEP();
				return;
			}

			end_addr = s->trkinfo[i+1].addr;
			end_msf.min = s->trkinfo[i+1].min;
			end_msf.sec = s->trkinfo[i+1].sec;
			end_msf.frame = s->trkinfo[i+1].frame;
		}
		else {
			end_addr = s->tot_addr;
			end_msf.min = s->tot_min;
			end_msf.sec = s->tot_sec;
			end_msf.frame = s->tot_frame;
		}

		if ((end_addr - start_addr) < app_data.min_playblks) {
			/* No more left to play: just stop */
			if (!slioc_start_stop(FALSE, FALSE))
				DO_BEEP();
		}
		else {
			if (s->mode == MOD_PAUSE)
				/* Mute: so we don't get a transient */
				slioc_mute_on(s);

			if (!slioc_do_playaudio(ADDR_BLK | ADDR_MSF,
						start_addr, end_addr,
						&start_msf, &end_msf,
						0, 0)) {
				DO_BEEP();

				/* Restore volume */
				slioc_mute_off(s);
				return;
			}

			if (s->mode == MOD_PAUSE) {
				slioc_pause_resume(FALSE);

				/* Restore volume */
				slioc_mute_off(s);
			}
		}
		break;

	default:
		break;
	}
}


/*
 * slioc_mute_on
 *	Mute audio function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_mute_on(curstat_t *s)
{
	(void) slioc_cfg_vol(0, s, FALSE);
}


/*
 * slioc_mute_off
 *	Un-mute audio function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_mute_off(curstat_t *s)
{
	(void) slioc_cfg_vol((int) s->level, s, FALSE);
}


/*
 * slioc_start
 *	Start the SunOS/Solaris/Linux ioctl method.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_start(curstat_t *s)
{
	/* Check to see if disc is ready */
	(void) slioc_disc_ready(s);

	/* Update display */
	DPY_ALL(s);
}


/*
 * slioc_icon
 *	Handler for main window iconification/de-iconification
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *	iconified - Whether the main window is iconified
 *
 * Return:
 *	Nothing.
 */
void
slioc_icon(curstat_t *s, bool_t iconified)
{
	/* This function attempts to reduce the status polling frequency
	 * when possible to cut down on CPU and bus usage.  This is
	 * done when the CD player is iconified.
	 */

	/* Increase status polling interval by 4 seconds when iconified */
	if (iconified)
		slioc_stat_interval = app_data.stat_interval + 4000;
	else
		slioc_stat_interval = app_data.stat_interval;

	switch (s->mode) {
	case MOD_BUSY:
	case MOD_NODISC:
	case MOD_STOP:
	case MOD_PAUSE:
		break;

	case MOD_A:
	case MOD_AB:
	case MOD_SAMPLE:
		/* No optimization in these modes */
		slioc_stat_interval = app_data.stat_interval;
		break;

	case MOD_PLAY:
		if (!iconified) {
			/* Force an immediate update */
			slioc_stop_stat_poll();
			slioc_start_stat_poll(s);
		}
		break;

	default:
		break;
	}
}


/*
 * slioc_halt
 *	Shut down the SunOS/Solaris/Linux ioctl method.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
slioc_halt(curstat_t *s)
{
	if (s->mode != MOD_BUSY && s->mode != MOD_NODISC) {
		if (app_data.exit_eject && app_data.eject_supp) {
			/* User closing application: Eject disc */
			slioc_start_stop(FALSE, TRUE);
		}
		else {
			if (app_data.exit_stop)
				/* User closing application: Stop disc */
				slioc_start_stop(FALSE, FALSE);

			switch (s->mode) {
			case MOD_PLAY:
			case MOD_PAUSE:
			case MOD_A:
			case MOD_AB:
			case MOD_SAMPLE:
				slioc_stop_stat_poll();
				break;
			}
		}
	}

	/* Close device */
	DBGPRN(errfp, "\nClose: %s\n", s->curdev);
	slioc_close();
}


/*
 * slioc_mode
 *	Return a text string indicating the current method.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Method text string.
 */
char *
slioc_mode(void)
{
#ifdef linux
	return ("Linux ioctl method");
#endif
#ifdef sun
	return ("SunOS/Solaris ioctl method");
#endif
#ifdef __QNX__
	return ("QNX ioctl method");
#endif
}


/*
 * slioc_vers
 *	Return a text string indicating the method's version number.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Version text string.
 */
char *
slioc_vers(void)
{
	return ("");
}

#endif	/* DI_SLIOC DEMO_ONLY */

