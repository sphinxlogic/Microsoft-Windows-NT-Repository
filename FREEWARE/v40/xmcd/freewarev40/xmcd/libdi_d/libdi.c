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
#ifndef LINT
static char *_libdi_c_ident_ = "@(#)libdi.c	6.108 98/10/27";
#endif

#include "common_d/appenv.h"
#include "common_d/patchlevel.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"
#include "libdi_d/slioc.h"
#include "libdi_d/fbioc.h"
#include "libdi_d/aixioc.h"


#define PARM_BUF_SZ	(STR_BUF_SZ * 16)	/* Temporary buffer size */


extern appdata_t	app_data;
extern FILE		*errfp;


/* libdi module init routines */
diinit_tbl_t		diinit[] = {
	{ scsipt_init },		/* SCSI pass-through method */
	{ slioc_init },			/* SunOS/Solaris/Linux ioctl method */
	{ fbioc_init },			/* FreeBSD ioctl method */
	{ aixioc_init },		/* AIX IDE ioctl method */
	{ NULL }			/* List terminator */
};


/* libdi interface calling table */
di_tbl_t		ditbl[MAX_METHODS];

/* Application callbacks */
di_client_t		*di_clinfo;

/* Device list table */
char			**di_devlist;

/* Web search engines list */
srcheng_ent_t		*srcheng_list_head;

/* Device lock file */
STATIC char		lockfile[FILE_PATH_SZ];

/* Device path registration list head */
STATIC di_devreg_t	*di_reghead;



/*
 * di_boolstr
 *	Return the string "False" or "True" depending upon the
 *	passed in boolean parameter.
 *
 * Args:
 *	parm - boolean parameter
 *
 * Return:
 *	"False" if parm is FALSE.
 *	"True" is parm is TRUE.
 */
STATIC char *
di_boolstr(bool_t parm)
{
	return ((parm == FALSE) ? "False" : "True");
}


/*
 * di_prncfg
 *	Display configuration information
 *
 * Args:
 *	None.
 *
 * Return:
 *	Nothing.
 */
STATIC void
di_prncfg(void)
{
	(void) fprintf(errfp, "\nBasic parameters:\n");

	(void) fprintf(errfp, "\tlibdir:\t\t\t\t%s\n",
		       app_data.libdir);
	(void) fprintf(errfp, "\tcddbMailSite:\t\t\t%s\n",
		       app_data.cddb_mailsite);
	(void) fprintf(errfp, "\tcddbMailCmd:\t\t\t%s\n",
		       app_data.cddb_mailcmd);

	(void) fprintf(errfp, "\nX resources:\n");

	(void) fprintf(errfp, "\tversion:\t\t\t%s\n",
		       app_data.version == NULL ? "(undef)" : app_data.version);
	(void) fprintf(errfp, "\tmainWindowMode:\t\t\t%d\n",
		       app_data.main_mode);
	(void) fprintf(errfp, "\tmodeChangeGravity:\t\t%d\n",
		       app_data.modechg_grav);
	(void) fprintf(errfp, "\tnormalMainWidth:\t\t%d\n",
		       app_data.normal_width);
	(void) fprintf(errfp, "\tnormalMainHeight:\t\t%d\n",
		       app_data.normal_height);
	(void) fprintf(errfp, "\tbasicMainWidth:\t\t\t%d\n",
		       app_data.basic_width);
	(void) fprintf(errfp, "\tbasicMainHeight:\t\t%d\n",
		       app_data.basic_height);
	(void) fprintf(errfp, "\tdisplayBlinkOnInterval:\t\t%d\n",
		       app_data.blinkon_interval);
	(void) fprintf(errfp, "\tdisplayBlinkOffInterval:\t%d\n",
		       app_data.blinkoff_interval);
	(void) fprintf(errfp, "\tmainShowFocus:\t\t\t%s\n",
		       di_boolstr(app_data.main_showfocus));

	(void) fprintf(errfp, "\nCommon parameters:\n");

	(void) fprintf(errfp, "\tdevice:\t\t\t\t%s\n",
		       app_data.device);
	(void) fprintf(errfp, "\tcddbPath:\t\t\t%s\n",
		       app_data.cddb_path);
	(void) fprintf(errfp, "\tcddbFileMode:\t\t\t%s\n",
		       app_data.cddb_filemode);
	(void) fprintf(errfp, "\tproxyServer:\t\t\t%s\n",
		       app_data.proxy_server == NULL ?
		           "(undef)" : app_data.proxy_server);
	(void) fprintf(errfp, "\tbrowserDirect:\t\t\t%s\n",
		       app_data.browser_dir == NULL ?
		           "(undef)" : app_data.browser_dir);
	(void) fprintf(errfp, "\tbrowserRemote:\t\t\t%s\n",
		       app_data.browser_rmt == NULL ?
		           "(undef)" : app_data.browser_rmt);
	(void) fprintf(errfp, "\twebSite1URL:\t\t\t%s\n",
		       app_data.site1_url == NULL ?
		           "(undef)" : app_data.site1_url);
	(void) fprintf(errfp, "\twebSite2URL:\t\t\t%s\n",
		       app_data.site2_url == NULL ?
		           "(undef)" : app_data.site2_url);
	(void) fprintf(errfp, "\twebSite3URL:\t\t\t%s\n",
		       app_data.site3_url == NULL ?
		           "(undef)" : app_data.site3_url);
	(void) fprintf(errfp, "\texcludeWords:\t\t\t%s\n",
		       app_data.exclude_words == NULL ?
		           "(undef)" : app_data.exclude_words);
	(void) fprintf(errfp, "\tcddbDefaultPort:\t\t%d\n",
		       app_data.cddb_port);
	(void) fprintf(errfp, "\tcddbTimeOut:\t\t\t%d\n",
		       app_data.cddb_timeout);
	(void) fprintf(errfp, "\tstatusPollInterval:\t\t%d\n",
		       app_data.stat_interval);
	(void) fprintf(errfp, "\tinsertPollInterval:\t\t%d\n",
		       app_data.ins_interval);
	(void) fprintf(errfp, "\ttimeDisplayMode:\t\t%d\n",
		       app_data.timedpy_mode);
	(void) fprintf(errfp, "\ttooltipDelayInterval:\t\t%d\n",
		       app_data.tooltip_delay);
	(void) fprintf(errfp, "\ttooltipActiveInterval:\t\t%d\n",
		       app_data.tooltip_time);
	(void) fprintf(errfp, "\tinsertPollDisable:\t\t%s\n",
		       di_boolstr(app_data.ins_disable));
	(void) fprintf(errfp, "\tpreviousThreshold:\t\t%d\n",
		       app_data.prev_threshold);
	(void) fprintf(errfp, "\tsearchSkipBlocks:\t\t%d\n",
		       app_data.skip_blks);
	(void) fprintf(errfp, "\tsearchPauseInterval:\t\t%d\n",
		       app_data.skip_pause);
	(void) fprintf(errfp, "\tsearchSpeedUpCount:\t\t%d\n",
		       app_data.skip_spdup);
	(void) fprintf(errfp, "\tsearchVolumePercent:\t\t%d\n",
		       app_data.skip_vol);
	(void) fprintf(errfp, "\tsearchMinVolume:\t\t%d\n",
		       app_data.skip_minvol);
	(void) fprintf(errfp, "\tsampleBlocks:\t\t\t%d\n",
		       app_data.sample_blks);
	(void) fprintf(errfp, "\tstartupVolume:\t\t\t%d\n",
		       app_data.startup_vol);
	(void) fprintf(errfp, "\tcddbRemoteDisable:\t\t%s\n",
		       di_boolstr(app_data.cddb_rmtdsbl));
	(void) fprintf(errfp, "\tcddbRemoteAutoSave:\t\t%s\n",
		       di_boolstr(app_data.cddb_rmtautosave));
	(void) fprintf(errfp, "\tcddbUseHttpProxy:\t\t%s\n",
		       di_boolstr(app_data.http_proxy));
	(void) fprintf(errfp, "\tshowScsiErrMsg:\t\t\t%s\n",
		       di_boolstr(app_data.scsierr_msg));
	(void) fprintf(errfp, "\tsolaris2VolumeManager:\t\t%s\n",
		       di_boolstr(app_data.sol2_volmgt));
	(void) fprintf(errfp, "\ttooltipEnable:\t\t\t%s\n",
		       di_boolstr(app_data.tooltip_enable));
	(void) fprintf(errfp, "\tdebug:\t\t\t\t%s\n",
		       di_boolstr(app_data.debug));

	(void) fprintf(errfp, "\nDevice-specific (privileged) parameters:\n");

	(void) fprintf(errfp, "\tdevnum:\t\t\t\t%d\n",
		       app_data.devnum);
	(void) fprintf(errfp, "\tdeviceList:\t\t\t%s\n",
		       app_data.devlist);
	(void) fprintf(errfp, "\tdeviceInterfaceMethod:\t\t%d\n",
		       app_data.di_method);
	(void) fprintf(errfp, "\tdriveVendorCode:\t\t%d\n",
		       app_data.vendor_code);
	(void) fprintf(errfp, "\tnumDiscs:\t\t\t%d\n",
		       app_data.numdiscs);
	(void) fprintf(errfp, "\tmediumChangeMethod:\t\t%d\n",
		       app_data.chg_method);
	(void) fprintf(errfp, "\tscsiAudioVolumeBase:\t\t%d\n",
		       app_data.base_scsivol);
	(void) fprintf(errfp, "\tminimumPlayBlocks:\t\t%d\n",
		       app_data.min_playblks);
	(void) fprintf(errfp, "\tplayAudio10Support:\t\t%s\n",
		       di_boolstr(app_data.play10_supp));
	(void) fprintf(errfp, "\tplayAudio12Support:\t\t%s\n",
		       di_boolstr(app_data.play12_supp));
	(void) fprintf(errfp, "\tplayAudioMSFSupport:\t\t%s\n",
		       di_boolstr(app_data.playmsf_supp));
	(void) fprintf(errfp, "\tplayAudioTISupport:\t\t%s\n",
		       di_boolstr(app_data.playti_supp));
	(void) fprintf(errfp, "\tloadSupport:\t\t\t%s\n",
		       di_boolstr(app_data.load_supp));
	(void) fprintf(errfp, "\tejectSupport:\t\t\t%s\n",
		       di_boolstr(app_data.eject_supp));
	(void) fprintf(errfp, "\tmodeSenseSetDBD:\t\t%s\n",
		       di_boolstr(app_data.msen_dbd));
	(void) fprintf(errfp, "\tvolumeControlSupport:\t\t%s\n",
		       di_boolstr(app_data.mselvol_supp));
	(void) fprintf(errfp, "\tbalanceControlSupport:\t\t%s\n",
		       di_boolstr(app_data.balance_supp));
	(void) fprintf(errfp, "\tchannelRouteSupport:\t\t%s\n",
		       di_boolstr(app_data.chroute_supp));
	(void) fprintf(errfp, "\tpauseResumeSupport:\t\t%s\n",
		       di_boolstr(app_data.pause_supp));
	(void) fprintf(errfp, "\tcaddyLockSupport:\t\t%s\n",
		       di_boolstr(app_data.caddylock_supp));
	(void) fprintf(errfp, "\tcurposFormat:\t\t\t%s\n",
		       di_boolstr(app_data.curpos_fmt));
	(void) fprintf(errfp, "\tnoTURWhenPlaying:\t\t%s\n",
		       di_boolstr(app_data.play_notur));

	(void) fprintf(errfp,
		       "\nDevice-specific (user-modifiable) parameters:\n");

	(void) fprintf(errfp, "\tvolumeControlTaper:\t\t%d\n",
		       app_data.vol_taper);
	(void) fprintf(errfp, "\tchannelRoute:\t\t\t%d\n",
		       app_data.ch_route);
	(void) fprintf(errfp, "\tspinDownOnLoad:\t\t\t%s\n",
		       di_boolstr(app_data.load_spindown));
	(void) fprintf(errfp, "\tplayOnLoad:\t\t\t%s\n",
		       di_boolstr(app_data.load_play));
	(void) fprintf(errfp, "\tejectOnDone:\t\t\t%s\n",
		       di_boolstr(app_data.done_eject));
	(void) fprintf(errfp, "\tejectOnExit:\t\t\t%s\n",
		       di_boolstr(app_data.exit_eject));
	(void) fprintf(errfp, "\tstopOnExit:\t\t\t%s\n",
		       di_boolstr(app_data.exit_stop));
	(void) fprintf(errfp, "\texitOnEject:\t\t\t%s\n",
		       di_boolstr(app_data.eject_exit));
	(void) fprintf(errfp, "\tcloseOnEject:\t\t\t%s\n",
		       di_boolstr(app_data.eject_close));
	(void) fprintf(errfp, "\tcaddyLock:\t\t\t%s\n",
		       di_boolstr(app_data.caddy_lock));
	(void) fprintf(errfp, "\trepeatMode:\t\t\t%s\n",
		       di_boolstr(app_data.repeat_mode));
	(void) fprintf(errfp, "\tshuffleMode:\t\t\t%s\n",
		       di_boolstr(app_data.shuffle_mode));
	(void) fprintf(errfp, "\tmultiPlay:\t\t\t%s\n",
		       di_boolstr(app_data.multi_play));
	(void) fprintf(errfp, "\treversePlay:\t\t\t%s\n",
		       di_boolstr(app_data.reverse));
	(void) fprintf(errfp, "\tinternalSpeakerEnable:\t\t%s\n",
		       di_boolstr(app_data.internal_spkr));

	(void) fprintf(errfp, "\n");
}


/*
 * di_parse_devlist
 *	Parse the app_data.devlist string and create the di_devlist array.
 *
 * Args:
 *	None.
 *
 * Return:
 *	Nothing.
 */
STATIC void
di_parse_devlist(void)
{
	char		*p,
			*q;
	int		i,
			n,
			listsz;
	curstat_t	*s = di_clinfo->curstat_addr();

	if (app_data.chg_method < 0 || app_data.chg_method >= MAX_CHG_METHODS)
		/* Fix-up in case of mis-configuration */
		app_data.chg_method = CHG_NONE;

	n = app_data.numdiscs;

	switch (app_data.chg_method) {
	case CHG_SCSI_MEDCHG:
		n = 2;
		/*FALLTHROUGH*/

	case CHG_SCSI_LUN:
		/* SCSI LUN addressing method */
		listsz = n * sizeof(char *);

		di_devlist = (char **) MEM_ALLOC("di_devlist", listsz);
		if (di_devlist == NULL) {
			DI_FATAL(app_data.str_nomemory);
			return;
		}
		(void) memset(di_devlist, 0, listsz);

		p = q = app_data.devlist;
		if (p == NULL || *p == '\0') {
			DI_FATAL(app_data.str_devlist_undef);
			return;
		}

		for (i = 0; i < n; i++) {
			q = strchr(p, ';');

			if (q == NULL && i < (n - 1)) {
				DI_FATAL(app_data.str_devlist_count);
				return;
			}

			if (q != NULL)
				*q = '\0';

			di_devlist[i] = (char *) MEM_ALLOC(
				"di_devlist[i]",
				strlen(p) + 1
			);
			if (di_devlist[i] == NULL) {
				DI_FATAL(app_data.str_nomemory);
				return;
			}
			(void) strcpy(di_devlist[i], p);

			if (q != NULL)
				*q = ';';

			p = q + 1;
		}

		/* In this mode, closeOnEject must be True */
		app_data.eject_close = TRUE;
		break;

	case CHG_OS_IOCTL:
	case CHG_NONE:
	default:
		if (app_data.chg_method == CHG_OS_IOCTL) {
			/* In this mode, closeOnEject must be True */
			app_data.eject_close = TRUE;
		}
		else {
			/* Some fix-ups in case of mis-configuration */
			app_data.numdiscs = 1;
		}

		if (app_data.devlist == NULL)
			app_data.devlist = app_data.device;
		else if (strcmp(app_data.devlist, app_data.device) != 0) {
			MEM_FREE(app_data.devlist);
			app_data.devlist = app_data.device;
		}

		di_devlist = (char **) MEM_ALLOC("di_devlist", sizeof(char *));
		if (di_devlist == NULL) {
			DI_FATAL(app_data.str_nomemory);
			return;
		}
		di_devlist[0] = (char *) MEM_ALLOC(
			"di_devlist[0]",
			strlen(app_data.devlist) + 1
		);
		if (di_devlist[0] == NULL) {
			DI_FATAL(app_data.str_nomemory);
			return;
		}
		(void) strcpy(di_devlist[0], app_data.devlist);

		break;
	}

	/* Initialize to the first device */
	s->curdev = di_devlist[0];
}


/*
 * di_init
 *	Top-level function to initialize the libdi modules.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_init(di_client_t *clp)
{
	int		i;
	curstat_t	*s = clp->curstat_addr();

	di_clinfo = clp;

#if defined(DI_SCSIPT) && defined(DEMO_ONLY)
	app_data.di_method = DI_SCSIPT;
#else
	/* Sanity check */
	if (app_data.di_method < 0 || app_data.di_method >= MAX_METHODS) {
		DI_FATAL(app_data.str_nomethod);
		return;
	}
#endif

	/* Parse the device list and set up structure */
	di_parse_devlist();

	/* Check string lengths.  This is to avoid subsequent
	 * string buffer overflows.
	 */
	if (((int) (strlen(app_data.str_notrom) +
		    strlen(app_data.device)) >= ERR_BUF_SZ) ||
	    ((int) (strlen(app_data.str_notscsi2) +
		    strlen(app_data.device)) >= ERR_BUF_SZ) ||
	    ((int) (strlen(app_data.str_staterr) +
		    strlen(app_data.device)) >= ERR_BUF_SZ) ||
	    ((int) (strlen(app_data.str_noderr) +
		    strlen(app_data.device)) >= ERR_BUF_SZ) ||
	    ((int) (strlen(app_data.str_nocfg) + 12 +
		    strlen(app_data.libdir)) >= ERR_BUF_SZ)) {
		DI_FATAL(app_data.str_longpatherr);
		return;
	}

	lockfile[0] = '\0';

	/* Initialize the libdi modules */
	for (i = 0; i < MAX_METHODS; i++) {
		if (diinit[i].init != NULL)
			diinit[i].init(s, &ditbl[i]);
	}

	/* Sanity check again */
	if (ditbl[app_data.di_method].mode == NULL) {
		DI_FATAL(app_data.str_nomethod);
		return;
	}
}


/*
 * di_check_disc
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
di_check_disc(curstat_t *s)
{
	if (ditbl[app_data.di_method].check_disc != NULL)
		return (ditbl[app_data.di_method].check_disc(s));

	return FALSE;
}


/*
 * di_status_upd
 *	Force update of playback status
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_status_upd(curstat_t *s)
{
	if (ditbl[app_data.di_method].status_upd != NULL)
		ditbl[app_data.di_method].status_upd(s);
}


/*
 * di_lock
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
di_lock(curstat_t *s, bool_t enable)
{
	if (ditbl[app_data.di_method].lock != NULL)
		ditbl[app_data.di_method].lock(s, enable);
}


/*
 * di_repeat
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
di_repeat(curstat_t *s, bool_t enable)
{
	if (ditbl[app_data.di_method].repeat != NULL)
		ditbl[app_data.di_method].repeat(s, enable);
}


/*
 * di_shuffle
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
di_shuffle(curstat_t *s, bool_t enable)
{
	if (ditbl[app_data.di_method].shuffle != NULL)
		ditbl[app_data.di_method].shuffle(s, enable);
}


/*
 * di_load_eject
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
di_load_eject(curstat_t *s)
{
	if (ditbl[app_data.di_method].load_eject != NULL)
		ditbl[app_data.di_method].load_eject(s);
}


/*
 * di_ab
 *	A->B segment play mode function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_ab(curstat_t *s)
{
	if (ditbl[app_data.di_method].ab != NULL)
		ditbl[app_data.di_method].ab(s);
}


/*
 * di_sample
 *	Sample play mode function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_sample(curstat_t *s)
{
	if (ditbl[app_data.di_method].sample != NULL)
		ditbl[app_data.di_method].sample(s);
}


/*
 * di_level
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
void
di_level(curstat_t *s, byte_t level, bool_t drag)
{
	if (ditbl[app_data.di_method].level != NULL)
		ditbl[app_data.di_method].level(s, level, drag);
}


/*
 * di_play_pause
 *	Audio playback and pause function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_play_pause(curstat_t *s)
{
	if (ditbl[app_data.di_method].play_pause != NULL)
		ditbl[app_data.di_method].play_pause(s);
}


/*
 * di_stop
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
di_stop(curstat_t *s, bool_t stop_disc)
{
	if (ditbl[app_data.di_method].stop != NULL)
		ditbl[app_data.di_method].stop(s, stop_disc);
}


/*
 * di_chgdisc
 *	Change disc function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_chgdisc(curstat_t *s)
{
	if (ditbl[app_data.di_method].chgdisc != NULL)
		ditbl[app_data.di_method].chgdisc(s);
}


/*
 * di_prevtrk
 *	Previous track function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_prevtrk(curstat_t *s)
{
	if (ditbl[app_data.di_method].prevtrk != NULL)
		ditbl[app_data.di_method].prevtrk(s);
}


/*
 * di_nexttrk
 *	Next track function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_nexttrk(curstat_t *s)
{
	if (ditbl[app_data.di_method].nexttrk != NULL)
		ditbl[app_data.di_method].nexttrk(s);
}


/*
 * di_previdx
 *	Previous index function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_previdx(curstat_t *s)
{
	if (ditbl[app_data.di_method].previdx != NULL)
		ditbl[app_data.di_method].previdx(s);
}


/*
 * di_nextidx
 *	Next index function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_nextidx(curstat_t *s)
{
	if (ditbl[app_data.di_method].nextidx != NULL)
		ditbl[app_data.di_method].nextidx(s);
}


/*
 * di_rew
 *	Search-rewind function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_rew(curstat_t *s, bool_t start)
{
	if (ditbl[app_data.di_method].rew != NULL)
		ditbl[app_data.di_method].rew(s, start);
}


/*
 * di_ff
 *	Search-fast-forward function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_ff(curstat_t *s, bool_t start)
{
	if (ditbl[app_data.di_method].ff != NULL)
		ditbl[app_data.di_method].ff(s, start);
}


/*
 * di_warp
 *	Track warp function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_warp(curstat_t *s)
{
	if (ditbl[app_data.di_method].warp != NULL)
		ditbl[app_data.di_method].warp(s);
}


/*
 * di_route
 *	Channel routing function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_route(curstat_t *s)
{
	if (ditbl[app_data.di_method].route != NULL)
		ditbl[app_data.di_method].route(s);
}


/*
 * di_mute_on
 *	Mute audio function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_mute_on(curstat_t *s)
{
	if (ditbl[app_data.di_method].mute_on != NULL)
		ditbl[app_data.di_method].mute_on(s);
}


/*
 * di_mute_off
 *	Un-mute audio function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_mute_off(curstat_t *s)
{
	if (ditbl[app_data.di_method].mute_off != NULL)
		ditbl[app_data.di_method].mute_off(s);
}


/*
 * di_start
 *	Start the SCSI pass-through module.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_start(curstat_t *s)
{
	if (app_data.debug)
		di_prncfg();	/* Print debug information */

	if (ditbl[app_data.di_method].start != NULL)
		ditbl[app_data.di_method].start(s);
}


/*
 * di_icon
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
di_icon(curstat_t *s, bool_t iconified)
{
	if (ditbl[app_data.di_method].icon != NULL)
		ditbl[app_data.di_method].icon(s, iconified);
}


/*
 * di_halt
 *	Shut down the SCSI pass-through and vendor-unique modules.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
di_halt(curstat_t *s)
{
	if (ditbl[app_data.di_method].halt != NULL)
		ditbl[app_data.di_method].halt(s);

	di_devunlock(s);
}


/*
 * di_dump_curstat
 *	Display contents of the the curstat_t structure.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
void
di_dump_curstat(curstat_t *s)
{
	int		i;
	di_devreg_t	*rp;

	(void) fprintf(errfp,
		       "\nDumping contents of curstat_t structure at 0x%x:\n",
		       (int) s);
	(void) fprintf(errfp,
		       "curdev=%s\nfirst_disc=%d last_disc=%d\n",
		       s->curdev, s->first_disc, s->last_disc);
	(void) fprintf(errfp,
		       "cur_disc=%d prev_disc=%d\n",
		       s->cur_disc, s->prev_disc);
	(void) fprintf(errfp,
		       "mode=%d time_dpy=%d flags=0x%x\n",
		       s->mode, s->time_dpy, s->flags);
	(void) fprintf(errfp,
		       "first_trk=%d last_trk=%d tot_trks=%d\n",
		       s->first_trk, s->last_trk, s->tot_trks);
	(void) fprintf(errfp, "cur_trk=%d cur_idx=%d\n",
		       s->cur_trk, s->cur_idx);
	(void) fprintf(errfp, "tot_msf=%02u:%02u:%02u addr=0x%x\n",
		       s->tot_min, s->tot_sec, s->tot_frame, s->tot_addr);
	(void) fprintf(errfp, "cur_tot_msf=%02u:%02u:%02u cur_tot_addr=0x%x\n",
		       s->cur_tot_min, s->cur_tot_sec, s->cur_tot_frame,
		       s->cur_tot_addr);
	(void) fprintf(errfp, "cur_trk_msf=%02u:%02u:%02u cur_trk_addr=0x%x\n",
		       s->cur_tot_min, s->cur_tot_sec, s->cur_tot_frame,
		       s->cur_tot_addr);

	for (i = 0; i < MAXTRACK; i++) {
		(void) fprintf(errfp,
			"[%d] trkno=%d addr=0x%x msf=%02d:%02d:%02d type=%d\n",
			       i, s->trkinfo[i].trkno, s->trkinfo[i].addr,
			       s->trkinfo[i].min, s->trkinfo[i].sec,
			       s->trkinfo[i].frame, s->trkinfo[i].type);
		if (s->trkinfo[i].trkno == LEAD_OUT_TRACK)
			break;
	}

	(void) fprintf(errfp, "playorder=");
	for (i = 0; i < (int) s->prog_tot; i++)
		(void) fprintf(errfp, "%d ", s->trkinfo[s->playorder[i]].trkno);
	(void) fprintf(errfp, "\n");

	(void) fprintf(errfp,
		"sav_iaddr=0x%x rptcnt=%d repeat=%d shuffle=%d program=%d\n",
		s->sav_iaddr, s->rptcnt, s->repeat, s->shuffle, s->program);
	(void) fprintf(errfp,
		"onetrk_prog=%d caddy_lk=%d cddb=%d prog_tot=%d prog_cnt=%d\n",
		s->onetrk_prog, s->caddy_lock, s->cddb,
		s->prog_tot, s->prog_cnt);
	(void) fprintf(errfp, "level=%d level_left=%d level_right=%d\n",
		s->level, s->level_left, s->level_right);
	(void) fprintf(errfp,
		"vendor=\"%-8s\" prod=\"%-16s\" revnum=\"%-4s\"\n",
		s->vendor, s->prod, s->revnum);

	(void) fprintf(errfp, "\nDevice registry: %s\n",
		di_reghead == NULL ? "NULL" : "");
	for (rp = di_reghead; rp != NULL; rp = rp->next)
		(void) fprintf(errfp, "[%d]\t%s\n", rp->fd, rp->path);
}


/*
 * di_add_srcheng_ent
 *	Add an entry to the search engines information list.
 *
 * Args:
 *	def - The search engine definition string from common.cfg
 *
 * Return:
 *	Nothing.
 */
void
di_add_srcheng_ent(char *def)
{
	char			*cp1,
				*cp2,
				*cp3,
				*name,
				*action;
	srcheng_ent_t		*p;
	bool_t			line_ent,
				new_ent;
	static srcheng_ent_t	*lastp;

	if ((cp1 = strchr(def, ':')) == NULL) {
		/* Bad definition */
		return;
	}

	name = def;
	action = NULL;
	*cp1 = '\0';
	p = NULL;

	line_ent = (bool_t) (strcmp(name, "-") == 0);
	if (!line_ent) {
		cp2 = cp1 + 1;

		/* Skip white space */
		while (*cp2 == ' ' || *cp2 == '\t')
			cp2++;

		if (*cp2 == '\0' || *cp2 == '\n') {
			/* Bad definition */
			*cp1 = ':';
			return;
		}

		action = cp2;

		/* Find an existing entry that matches the name */
		for (p = srcheng_list_head; p != NULL; p = p->next) {
			if (strcmp(name, p->name) == 0)
				break;
		}
	}

	new_ent = FALSE;
	if (p == NULL) {
		/* Allocate a new list entry */
		p = (srcheng_ent_t *)(void *) MEM_ALLOC(
			"srcheng_ent_t",
			sizeof(srcheng_ent_t)
		);
		if (p == NULL) {
			DI_FATAL(app_data.str_nomemory);
			return;
		}
		p->action = NULL;
		p->aux = NULL;
		new_ent = TRUE;
	}

	if (app_data.debug) {
		if (line_ent)
			(void) fprintf(errfp, "Add    ---\n");
		else
			(void) fprintf(errfp, "%s %s: %s\n",
				new_ent ? "Add   " : "Update",
				name, action);
	}

	/* Name */
	p->name = (char *) MEM_ALLOC("p->name", strlen(name) + 1);
	if (p->name == NULL) {
		DI_FATAL(app_data.str_nomemory);
		return;
	}
	(void) strcpy(p->name, name);

	if (!line_ent) {
		/* Action URL */
		if (p->action != NULL)
			MEM_FREE(p->action);
		p->action = (char *) MEM_ALLOC("p->action", strlen(action) + 1);
		if (p->action == NULL) {
			DI_FATAL(app_data.str_nomemory);
			return;
		}
		(void) strcpy(p->action, action);

		/* Scan URL action string to get a count of the number of
		 * substitutions needed.
		 */
		p->acnt = p->dcnt = p->tcnt = 0;
		for (cp3 = action; *cp3 != '\0'; cp3++) {
			if (*cp3 == '%') {
				switch ((int) *(++cp3)) {
				case 'A':
				case 'a':
					p->acnt++;
					break;
				case 'D':
				case 'd':
					p->dcnt++;
					break;
				case 'T':
				case 't':
					p->tcnt++;
					break;
				case 'B':
				case 'b':
					p->acnt++;
					p->dcnt++;
					break;
				default:
					break;
				}
			}
		}
	}

	/* Restore character */
	*cp1 = ':';

	if (new_ent) {
		/* Add new entry to list */
		p->next = NULL;
		if (srcheng_list_head == NULL)
			srcheng_list_head = lastp = p;
		else {
			lastp->next = p;
			lastp = p;
		}
	}
}


/*
 * di_common_parmload
 *	Load the common configuration file and initialize parameters.
 *
 * Args:
 *	path - Path name to the file to load.
 *	priv - Whether the privileged keywords are to be recognized.
 *
 * Return:
 *	Nothing.
 */
void
di_common_parmload(char *path, bool_t priv)
{
	FILE		*fp;
	char		*buf,
			*parm,
			errstr[ERR_BUF_SZ];
	char		trypath[FILE_PATH_SZ];
	struct utsname	*un;
	bool_t		notry2;
	static bool_t	dev_cmdline = FALSE,
			force_debug = FALSE;
#ifndef __VMS
	pid_t		cpid;
	waitret_t	stat_val;
	int		ret,
			pfd[2];
	FILE		*wfp;

	un = util_get_uname();

	if (priv && di_isdemo())
		app_data.device = "(sim1)";

	if (PIPE(pfd) < 0) {
		DBGPRN(errfp, "di_common_parmload: pipe failed (errno=%d)\n",
			errno);
		if (priv && !di_isdemo()) {
			(void) sprintf(errstr, app_data.str_nocfg, path);
			DI_FATAL(errstr);
		}
		return;
	}
	
	switch (cpid = FORK()) {
	case 0:
		/* Child */

		/* Close un-needed pipe descriptor */
		(void) close(pfd[0]);

		DBGPRN(errfp, "Setting uid to %d, gid to %d\n",
			(int) util_get_ouid(), (int) util_get_ogid());

		/* Force uid and gid to original setting */
		if (setuid(util_get_ouid()) < 0 ||
		    setgid(util_get_ogid()) < 0) {
			(void) close(pfd[1]);
			exit(1);
		}

		/* Allocate temporary buffer */
		if ((buf = (char *) MEM_ALLOC("parmbuf",
					      PARM_BUF_SZ)) == NULL) {
			(void) close(pfd[1]);
			exit(2);
		}

		notry2 = FALSE;
		if (((int) strlen(path) + (int) strlen(un->nodename) + 2)
		    > FILE_PATH_SZ) {
			DBGPRN(errfp, "NOTICE: %s: %s\n",
				"Host-specific config files not used",
				"Name too long");
			(void) strcpy(trypath, path);
			notry2 = TRUE;
		}
		else
			/* Try host-specific config file first */
			(void) sprintf(trypath, "%s-%s", path, un->nodename);

		DBGPRN(errfp, "\nLoading common parameters: %s\n", trypath);
		fp = fopen(trypath, "r");

		if (fp == NULL && !notry2) {
			DBGPRN(errfp, "Cannot open %s\n", trypath);

			/* Try generic config file */
			(void) strcpy(trypath, path);

			DBGPRN(errfp, "Loading common parameters: %s\n",
				trypath);
			fp = fopen(trypath, "r");
		}

		if (fp == NULL) {
			DBGPRN(errfp, "Cannot open %s\n", trypath);

			(void) close(pfd[1]);

			if (priv && !di_isdemo())
				exit(3);
			exit(0);
		}
		
		if ((wfp = fdopen(pfd[1], "w")) == NULL) {
			DBGPRN(errfp,
			    "di_common_parmload: write pipe fdopen failed\n");
			(void) close(pfd[1]);
			exit(4);
		}

		while (fgets(buf, PARM_BUF_SZ, fp) != NULL)
			(void) fprintf(wfp, "%s", buf);

		(void) fclose(fp);
		(void) fclose(wfp);

		exit(0);
		/*NOTREACHED*/

	case -1:
		DBGPRN(errfp, "di_common_parmload: fork failed (errno=%d)\n",
			errno);
		(void) close(pfd[0]);
		(void) close(pfd[1]);

		if (priv && !di_isdemo()) {
			(void) sprintf(errstr, app_data.str_nocfg, path);
			DI_FATAL(errstr);
		}
		return;

	default:
		/* Parent */

		/* Close un-needed pipe descriptor */
		(void) close(pfd[1]);

		if ((fp = fdopen(pfd[0], "r")) == NULL) {
			DBGPRN(errfp,
			    "di_common_parmload: read pipe fdopen failed\n");
			if (priv && !di_isdemo()) {
				/* Cannot open pipe */
				(void) sprintf(errstr, app_data.str_nocfg,
					       path);
				DI_FATAL(errstr);
			}
			return;
		}
		break;
	}
#else
	un = util_get_uname();

	notry2 = FALSE;
	if (((int) strlen(path) + (int) strlen(un->nodename) + 2)
	    > FILE_PATH_SZ) {
		DBGPRN(errfp, "NOTICE: %s: %s\n",
			"Host-specific config files not used",
			"Name too long");
		(void) strcpy(trypath, path);
		notry2 = TRUE;
	}
	else
		/* Try host-specific config file first */
		(void) sprintf(trypath, "%s-%s", path, un->nodename);

	DBGPRN(errfp, "\nLoading common parameters: %s\n", trypath);
	fp = fopen(trypath, "r");

	if (fp == NULL && !notry2) {
		DBGPRN(errfp, "Cannot open %s\n", trypath);

		/* Try generic config file */
		(void) strcpy(trypath, path);

		DBGPRN(errfp, "Loading common parameters: %s\n", trypath);
		fp = fopen(trypath, "r");
	}

	if (fp == NULL) {
		DBGPRN(errfp, "Cannot open %s\n", trypath);
		if (priv && !di_isdemo()) {
			/* Cannot open system config file. */
			(void) sprintf(errstr, app_data.str_nocfg, path);
			DI_FATAL(errstr);
		}
		return;
	}
#endif	/* __VMS */

	/* Allocate temporary buffer */
	if ((buf = (char *) MEM_ALLOC("buf", PARM_BUF_SZ)) == NULL ||
	    (parm = (char *) MEM_ALLOC("parmbuf", PARM_BUF_SZ)) == NULL) {
		DI_FATAL(app_data.str_nomemory);
		return;
	}

	if (priv && app_data.debug)
		force_debug = TRUE;

	/* Read in common parameters */
	while (fgets(buf, PARM_BUF_SZ, fp) != NULL) {
		/* Skip comments and blank lines */
		if (buf[0] == '#' || buf[0] == '!' || buf[0] == '\n')
			continue;

		if (sscanf(buf, "device: %[^\n]\n", parm) > 0) {
			if (di_isdemo())
				continue;
			if (priv && app_data.device != NULL)
				dev_cmdline = TRUE;
			if (!dev_cmdline) {
				if (app_data.device != NULL)
					MEM_FREE(app_data.device);

				app_data.device = (char *) MEM_ALLOC(
					"app_data.device",
					strlen(parm) + 1
				);
				if (app_data.device == NULL) {
					DI_FATAL(app_data.str_nomemory);
					return;
				}
				(void) strcpy(app_data.device, parm);
			}
			continue;
		}
		if (sscanf(buf, "cddbPath: %[^\n]\n", parm) > 0) {
			if (!priv || app_data.cddb_path == NULL) {
				if (app_data.cddb_path != NULL)
					MEM_FREE(app_data.cddb_path);

				app_data.cddb_path = (char *) MEM_ALLOC(
					"app_data.cddb_path",
					strlen(parm) + 1
				);
				if (app_data.cddb_path == NULL) {
					DI_FATAL(app_data.str_nomemory);
					return;
				}
				(void) strcpy(app_data.cddb_path, parm);
			}
			continue;
		}
		if (sscanf(buf, "proxyServer: %[^\n]\n", parm) > 0) {
			if (app_data.proxy_server != NULL)
				MEM_FREE(app_data.proxy_server);
			app_data.proxy_server = (char *) MEM_ALLOC(
				"app_data.proxy_server",
				strlen(parm) + 1
			);
			if (app_data.proxy_server == NULL) {
				DI_FATAL(app_data.str_nomemory);
				return;
			}
			(void) strcpy(app_data.proxy_server, parm);
			continue;
		}
		if (sscanf(buf, "browserDirect: %[^\n]\n", parm) > 0) {
			if (app_data.browser_dir != NULL)
				MEM_FREE(app_data.browser_dir);
			app_data.browser_dir = (char *) MEM_ALLOC(
				"app_data.browser_dir",
				strlen(parm) + 1
			);
			if (app_data.browser_dir == NULL) {
				DI_FATAL(app_data.str_nomemory);
				return;
			}
			(void) strcpy(app_data.browser_dir, parm);
			continue;
		}
		if (sscanf(buf, "browserRemote: %[^\n]\n", parm) > 0) {
			if (app_data.browser_rmt != NULL)
				MEM_FREE(app_data.browser_rmt);
			app_data.browser_rmt = (char *) MEM_ALLOC(
				"app_data.browser_rmt",
				strlen(parm) + 1
			);
			if (app_data.browser_rmt == NULL) {
				DI_FATAL(app_data.str_nomemory);
				return;
			}
			(void) strcpy(app_data.browser_rmt, parm);
			continue;
		}
		if (sscanf(buf, "webSite1URL: %[^\n]\n", parm) > 0) {
			if (app_data.site1_url != NULL)
				MEM_FREE(app_data.site1_url);
			app_data.site1_url = (char *) MEM_ALLOC(
				"app_data.site1_url",
				strlen(parm) + 1
			);
			if (app_data.site1_url == NULL) {
				DI_FATAL(app_data.str_nomemory);
				return;
			}
			(void) strcpy(app_data.site1_url, parm);
			continue;
		}
		if (sscanf(buf, "webSite2URL: %[^\n]\n", parm) > 0) {
			if (app_data.site2_url != NULL)
				MEM_FREE(app_data.site2_url);
			app_data.site2_url = (char *) MEM_ALLOC(
				"app_data.site2_url",
				strlen(parm) + 1
			);
			if (app_data.site2_url == NULL) {
				DI_FATAL(app_data.str_nomemory);
				return;
			}
			(void) strcpy(app_data.site2_url, parm);
			continue;
		}
		if (sscanf(buf, "webSite3URL: %[^\n]\n", parm) > 0) {
			if (app_data.site3_url != NULL)
				MEM_FREE(app_data.site3_url);
			app_data.site3_url = (char *) MEM_ALLOC(
				"app_data.site3_url",
				strlen(parm) + 1
			);
			if (app_data.site3_url == NULL) {
				DI_FATAL(app_data.str_nomemory);
				return;
			}
			(void) strcpy(app_data.site3_url, parm);
			continue;
		}
		if (sscanf(buf, "excludeWords: %[^\n]\n", parm) > 0) {
			if (app_data.exclude_words != NULL)
				MEM_FREE(app_data.exclude_words);
			app_data.exclude_words = (char *) MEM_ALLOC(
				"app_data.exclude_words",
				strlen(parm) + 1
			);
			if (app_data.exclude_words == NULL) {
				DI_FATAL(app_data.str_nomemory);
				return;
			}
			(void) strcpy(app_data.exclude_words, parm);
			continue;
		}
		if (sscanf(buf, "cddbDefaultPort: %s\n", parm) > 0) {
			app_data.cddb_port = atoi(parm);
			continue;
		}
		if (sscanf(buf, "cddbTimeOut: %s\n", parm) > 0) {
			app_data.cddb_timeout = atoi(parm);
			continue;
		}
		if (sscanf(buf, "cddbRemoteDisable: %s\n", parm) > 0) {
			app_data.cddb_rmtdsbl = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "cddbRemoteAutoSave: %s\n", parm) > 0) {
			app_data.cddb_rmtautosave = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "cddbUseHttpProxy: %s\n", parm) > 0) {
			app_data.http_proxy = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "cddbFileMode: %s\n", parm) > 0) {
			if (!priv || app_data.cddb_filemode == NULL) {
				if (app_data.cddb_filemode != NULL)
					MEM_FREE(app_data.cddb_filemode);

				app_data.cddb_filemode = (char *) MEM_ALLOC(
					"app_data.cddb_filemode",
					strlen(parm) + 1
				);
				if (app_data.cddb_filemode == NULL) {
					DI_FATAL(app_data.str_nomemory);
					return;
				}
				(void) strcpy(app_data.cddb_filemode, parm);
			}
			continue;
		}
		if (sscanf(buf, "statusPollInterval: %s\n", parm) > 0) {
			app_data.stat_interval = atoi(parm);
			continue;
		}
		if (sscanf(buf, "insertPollInterval: %s\n", parm) > 0) {
			app_data.ins_interval = atoi(parm);
			continue;
		}
		if (sscanf(buf, "insertPollDisable: %s\n", parm) > 0) {
			app_data.ins_disable = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "previousThreshold: %s\n", parm) > 0) {
			app_data.prev_threshold = atoi(parm);
			continue;
		}
		if (sscanf(buf, "sampleBlocks: %s\n", parm) > 0) {
			app_data.sample_blks = atoi(parm);
			continue;
		}
		if (sscanf(buf, "timeDisplayMode: %s\n", parm) > 0) {
			app_data.timedpy_mode = atoi(parm);
			continue;
		}
		if (sscanf(buf, "solaris2VolumeManager: %s\n", parm) > 0) {
			app_data.sol2_volmgt = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "showScsiErrMsg: %s\n", parm) > 0) {
			app_data.scsierr_msg = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "curfileEnable: %s\n", parm) > 0) {
			app_data.write_curfile = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "tooltipEnable: %s\n", parm) > 0) {
			app_data.tooltip_enable = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "tooltipDelayInterval: %s\n", parm) > 0) {
			app_data.tooltip_delay = atoi(parm);
			continue;
		}
		if (sscanf(buf, "tooltipActiveInterval: %s\n", parm) > 0) {
			app_data.tooltip_time = atoi(parm);
			continue;
		}
		if (sscanf(buf, "debugMode: %s\n", parm) > 0) {
			if (!force_debug)
				app_data.debug = util_stob(parm);
			continue;
		}

		/* If we get here, and there are more entries, then we
		 * assume it's the search engines definitions.
		 */
		buf[strlen(buf)-1] = '\0';	/* Eat newline */
		di_add_srcheng_ent(buf);
	}

	(void) fclose(fp);
	MEM_FREE(buf);
	MEM_FREE(parm);

	/* In case of error */
	if (app_data.timedpy_mode < 0 ||
	    app_data.timedpy_mode >= TIMEDPY_MAX_MODES)
		app_data.timedpy_mode = 0;
	if (app_data.tooltip_delay < 0)
		app_data.tooltip_delay = 1000;
	if (app_data.tooltip_time < 0)
		app_data.tooltip_delay = 3000;

#ifndef __VMS
	while ((ret = WAITPID(cpid, &stat_val, 0)) != cpid) {
		if (ret < 0)
			break;
	}
	if (ret == cpid) {
	    if (WIFEXITED(stat_val)) {
		if (WEXITSTATUS(stat_val) != 0) {
			DBGPRN(errfp,
			    "di_common_parmload: child exited (status=%d)\n",
			    WEXITSTATUS(stat_val));
			(void) sprintf(errstr, app_data.str_nocfg, path);
			if (priv && !di_isdemo()) {
				DI_FATAL(errstr);
			}
			else {
				DI_WARNING(errstr);
			}
		}
	    }
	    else if (WIFSIGNALED(stat_val)) {
		DBGPRN(errfp, "di_common_parmload: child killed.\n");
		(void) sprintf(errstr, app_data.str_nocfg, path);
		if (priv && !di_isdemo()) {
			DI_FATAL(errstr);
		}
		else {
			DI_WARNING(errstr);
		}
	    }
	}
#endif
}


/*
 * di_devspec_parmload
 *	Load the specified device-specific configuration file and
 *	initialize parameters.
 *
 * Args:
 *	path - Path name to the file to load.
 *	priv - Whether the privileged keywords are to be recognized.
 *
 * Return:
 *	Nothing.
 */
void
di_devspec_parmload(char *path, bool_t priv)
{
	FILE		*fp;
	char		*buf,
			*parm,
			errstr[ERR_BUF_SZ];
	char		trypath[FILE_PATH_SZ];
	struct utsname	*un;
	bool_t		notry2;
#ifndef __VMS
	pid_t		cpid;
	waitret_t	stat_val;
	int		ret,
			pfd[2];
	FILE		*wfp;


	un = util_get_uname();

	if (priv && di_isdemo()) {
		char	*cp;

		cp = "(sim1);(sim2);(sim3);(sim4);(sim5);(sim6);(sim7);(sim8)";
		app_data.devlist = MEM_ALLOC(
			"app_data.devlist",
			strlen(cp) + 1
		);
		if (app_data.devlist == NULL) {
			DI_FATAL(app_data.str_nomemory);
			return;
		}
		strcpy(app_data.devlist, cp);
		app_data.numdiscs = 8;
		app_data.chg_method = CHG_SCSI_LUN;
		app_data.multi_play = TRUE;
	}

	if (PIPE(pfd) < 0) {
		DBGPRN(errfp, "di_devspec_parmload: pipe failed (errno=%d)\n",
			errno);
		if (priv && !di_isdemo()) {
			(void) sprintf(errstr, app_data.str_nocfg, path);
			DI_FATAL(errstr);
		}
		return;
	}
	
	switch (cpid = FORK()) {
	case 0:
		/* Child */

		/* Close un-needed pipe descriptor */
		(void) close(pfd[0]);

		DBGPRN(errfp, "Setting uid to %d, gid to %d\n",
			(int) util_get_ouid(), (int) util_get_ogid());

		/* Force uid and gid to original setting */
		if (setuid(util_get_ouid()) < 0 ||
		    setgid(util_get_ogid()) < 0) {
			(void) close(pfd[1]);
			exit(1);
		}

		/* Allocate temporary buffer */
		if ((buf = (char *) MEM_ALLOC("parmbuf",
					      PARM_BUF_SZ)) == NULL) {
			(void) close(pfd[1]);
			exit(2);
		}
		notry2 = FALSE;
		if (((int) strlen(path) + (int) strlen(un->nodename) + 2)
		    > FILE_PATH_SZ) {
			DBGPRN(errfp, "NOTICE: %s: %s\n",
				"Host-specific config files not used",
				"Name too long");
			(void) strcpy(trypath, path);
			notry2 = TRUE;
		}
		else
			/* Try host-specific config file first */
			(void) sprintf(trypath, "%s-%s", path, un->nodename);

		DBGPRN(errfp, "\nLoading device-specific parameters: %s\n",
			trypath);

		fp = fopen(trypath, "r");

		if (fp == NULL && !notry2) {
			DBGPRN(errfp, "Cannot open %s\n", trypath);

			/* Try generic config file */
			(void) strcpy(trypath, path);

			DBGPRN(errfp,
				"Loading device-specific parameters: %s\n",
				trypath);
			fp = fopen(trypath, "r");
		}

		if (fp == NULL) {
			DBGPRN(errfp, "Cannot open %s\n", trypath);
			(void) close(pfd[1]);

			if (priv && !di_isdemo())
				exit(3);
			exit(0);
		}
		
		if ((wfp = fdopen(pfd[1], "w")) == NULL) {
			DBGPRN(errfp,
			    "di_devspec_parmload: write pipe fdopen failed\n");
			(void) close(pfd[1]);
			exit(4);
		}

		while (fgets(buf, PARM_BUF_SZ, fp) != NULL)
			(void) fprintf(wfp, "%s", buf);

		(void) fclose(fp);
		(void) fclose(wfp);

		exit(0);
		/*NOTREACHED*/

	case -1:
		DBGPRN(errfp, "di_devspec_parmload: fork failed (errno=%d)\n",
			errno);
		(void) close(pfd[0]);
		(void) close(pfd[1]);

		if (priv && !di_isdemo()) {
			(void) sprintf(errstr, app_data.str_nocfg, path);
			DI_FATAL(errstr);
		}
		return;

	default:
		/* Parent */

		/* Close un-needed pipe descriptor */
		(void) close(pfd[1]);

		if ((fp = fdopen(pfd[0], "r")) == NULL) {
			DBGPRN(errfp,
			    "di_devspec_parmload: read pipe fdopen failed\n");
			if (priv && !di_isdemo()) {
				/* Cannot open pipe */
				(void) sprintf(errstr, app_data.str_nocfg,
					       path);
				DI_FATAL(errstr);
			}
			return;
		}
		break;
	}
#else
	un = util_get_uname();

	notry2 = FALSE;
	if (((int) strlen(path) + (int) strlen(un->nodename) + 2)
	    > FILE_PATH_SZ) {
		DBGPRN(errfp, "NOTICE: %s: %s\n",
			"Host-specific config files not used",
			"Name too long");
		(void) strcpy(trypath, path);
		notry2 = TRUE;
	}
	else
		/* Try host-specific config file first */
		(void) sprintf(trypath, "%s-%s", path, un->nodename);

	DBGPRN(errfp, "\nLoading device-specific parameters: %s\n", trypath);
	fp = fopen(trypath, "r");

	if (fp == NULL && !notry2) {
		DBGPRN(errfp, "Cannot open %s\n", trypath);

		/* Try generic config file */
		(void) strcpy(trypath, path);

		DBGPRN(errfp, "Loading device-specific parameters: %s\n",
			trypath);
		fp = fopen(trypath, "r");
	}

	if (fp == NULL) {
		DBGPRN(errfp, "Cannot open %s\n", trypath);
		if (priv && !di_isdemo()) {
			/* Cannot open master device-specific
			 * config file.
			 */
			(void) sprintf(errstr, app_data.str_nocfg, path);
			DI_FATAL(errstr);
		}
		return;
	}
#endif	/* __VMS */

	/* Allocate temporary buffer */
	if ((buf = (char *) MEM_ALLOC("buf", PARM_BUF_SZ)) == NULL ||
	    (parm = (char *) MEM_ALLOC("parmbuf", PARM_BUF_SZ)) == NULL) {
		DI_FATAL(app_data.str_nomemory);
		return;
	}

	/* Read in device-specific parameters */
	while (fgets(buf, PARM_BUF_SZ, fp) != NULL) {
		/* Skip comments */
		if (buf[0] == '#' || buf[0] == '!' || buf[0] == '\n')
			continue;

		/* These are privileged parameters and users
		 * cannot overide them in their .xmcdcfg file.
		 */
		if (priv) {
			if (sscanf(buf, "logicalDriveNumber: %s\n",
				   parm) > 0) {
				app_data.devnum = atoi(parm);
				continue;
			}
			if (sscanf(buf, "deviceList: %[^\n]\n", parm) > 0) {
				if (app_data.devlist != NULL)
					MEM_FREE(app_data.devlist);

				app_data.devlist = (char *) MEM_ALLOC(
					"app_data.devlist",
					strlen(parm) + 1
				);
				if (app_data.devlist == NULL) {
					DI_FATAL(app_data.str_nomemory);
					return;
				}
				(void) strcpy(app_data.devlist, parm);
				continue;
			}
			if (sscanf(buf, "deviceInterfaceMethod: %s\n",
				   parm) > 0) {
				app_data.di_method = atoi(parm);
				continue;
			}
			if (sscanf(buf, "driveVendorCode: %s\n",
				   parm) > 0) {
				app_data.vendor_code = atoi(parm);
				continue;
			}
			if (sscanf(buf, "numDiscs: %s\n", parm) > 0) {
				app_data.numdiscs = atoi(parm);
				continue;
			}
			if (sscanf(buf, "mediumChangeMethod: %s\n",
				   parm) > 0) {
				app_data.chg_method = atoi(parm);
				continue;
			}
			if (sscanf(buf, "scsiAudioVolumeBase: %s\n",
				   parm) > 0) {
				app_data.base_scsivol = atoi(parm);
				continue;
			}
			if (sscanf(buf, "minimumPlayBlocks: %s\n",
				   parm) > 0) {
				app_data.min_playblks = atoi(parm);
				continue;
			}
			if (sscanf(buf, "playAudio10Support: %s\n",
				   parm) > 0) {
				app_data.play10_supp = util_stob(parm);
				continue;
			}
			if (sscanf(buf, "playAudio12Support: %s\n",
				   parm) > 0) {
				app_data.play12_supp = util_stob(parm);
				continue;
			}
			if (sscanf(buf, "playAudioMSFSupport: %s\n",
				   parm) > 0) {
				app_data.playmsf_supp = util_stob(parm);
				continue;
			}
			if (sscanf(buf, "playAudioTISupport: %s\n",
				   parm) > 0) {
				app_data.playti_supp = util_stob(parm);
				continue;
			}
			if (sscanf(buf, "loadSupport: %s\n",
				   parm) > 0) {
				app_data.load_supp = util_stob(parm);
				continue;
			}
			if (sscanf(buf, "ejectSupport: %s\n",
				   parm) > 0) {
				app_data.eject_supp = util_stob(parm);
				continue;
			}
			if (sscanf(buf, "modeSenseSetDBD: %s\n",
				   parm) > 0) {
				app_data.msen_dbd = util_stob(parm);
				continue;
			}
			if (sscanf(buf, "volumeControlSupport: %s\n",
				   parm) > 0) {
				app_data.mselvol_supp = util_stob(parm);
				continue;
			}
			if (sscanf(buf, "balanceControlSupport: %s\n",
				   parm) > 0) {
				app_data.balance_supp = util_stob(parm);
				continue;
			}
			if (sscanf(buf, "channelRouteSupport: %s\n",
				   parm) > 0) {
				app_data.chroute_supp = util_stob(parm);
				continue;
			}
			if (sscanf(buf, "pauseResumeSupport: %s\n",
				   parm) > 0) {
				app_data.pause_supp = util_stob(parm);
				continue;
			}
			if (sscanf(buf, "caddyLockSupport: %s\n",
				   parm) > 0) {
				app_data.caddylock_supp = util_stob(parm);
				continue;
			}
			if (sscanf(buf, "curposFormat: %s\n",
				   parm) > 0) {
				app_data.curpos_fmt = util_stob(parm);
				continue;
			}
			if (sscanf(buf, "noTURWhenPlaying: %s\n",
				   parm) > 0) {
				app_data.play_notur = util_stob(parm);
				continue;
			}
		}

		/* These are general parameters that can be
		 * changed by the user.
		 */
		if (sscanf(buf, "volumeControlTaper: %s\n", parm) > 0) {
			app_data.vol_taper = atoi(parm);
			continue;
		}
		if (sscanf(buf, "startupVolume: %s\n", parm) > 0) {
			app_data.startup_vol = atoi(parm);
			continue;
		}
		if (sscanf(buf, "channelRoute: %s\n", parm) > 0) {
			app_data.ch_route = atoi(parm);
			continue;
		}
		if (sscanf(buf, "searchSkipBlocks: %s\n", parm) > 0) {
			app_data.skip_blks = atoi(parm);
			continue;
		}
		if (sscanf(buf, "searchPauseInterval: %s\n", parm) > 0) {
			app_data.skip_pause = atoi(parm);
			continue;
		}
		if (sscanf(buf, "searchSpeedUpCount: %s\n", parm) > 0) {
			app_data.skip_spdup = atoi(parm);
			continue;
		}
		if (sscanf(buf, "searchVolumePercent: %s\n", parm) > 0) {
			app_data.skip_vol = atoi(parm);
			continue;
		}
		if (sscanf(buf, "searchMinVolume: %s\n", parm) > 0) {
			app_data.skip_minvol = atoi(parm);
			continue;
		}
		if (sscanf(buf, "spinDownOnLoad: %s\n", parm) > 0) {
			app_data.load_spindown = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "playOnLoad: %s\n", parm) > 0) {
			app_data.load_play = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "ejectOnDone: %s\n", parm) > 0) {
			app_data.done_eject = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "ejectOnExit: %s\n", parm) > 0) {
			app_data.exit_eject = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "stopOnExit: %s\n", parm) > 0) {
			app_data.exit_stop = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "exitOnEject: %s\n", parm) > 0) {
			app_data.eject_exit = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "closeOnEject: %s\n", parm) > 0) {
			app_data.eject_close = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "caddyLock: %s\n", parm) > 0) {
			app_data.caddy_lock = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "repeatMode: %s\n", parm) > 0) {
			app_data.repeat_mode = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "shuffleMode: %s\n", parm) > 0) {
			app_data.shuffle_mode = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "multiPlay: %s\n", parm) > 0) {
			app_data.multi_play = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "reversePlay: %s\n", parm) > 0) {
			app_data.reverse = util_stob(parm);
			continue;
		}
		if (sscanf(buf, "internalSpeakerEnable: %s\n", parm) > 0) {
			app_data.internal_spkr = util_stob(parm);
			continue;
		}
	}

	(void) fclose(fp);
	MEM_FREE(buf);
	MEM_FREE(parm);

#ifndef __VMS
	while ((ret = WAITPID(cpid, &stat_val, 0)) != cpid) {
		if (ret < 0)
			break;
	}
	if (ret == cpid) {
	    if (WIFEXITED(stat_val)) {
		if (WEXITSTATUS(stat_val) != 0) {
			DBGPRN(errfp,
			    "di_devspec_parmload: child exited (status=%d)\n",
			    WEXITSTATUS(stat_val));
			(void) sprintf(errstr, app_data.str_nocfg, path);
			if (priv && !di_isdemo()) {
				DI_FATAL(errstr);
			}
			else {
				DI_WARNING(errstr);
			}
		}
	    }
	    else if (WIFSIGNALED(stat_val)) {
		DBGPRN(errfp, "di_devspec_parmload: child killed.\n");
		(void) sprintf(errstr, app_data.str_nocfg, path);
		if (priv && !di_isdemo()) {
			DI_FATAL(errstr);
		}
		else {
			DI_WARNING(errstr);
		}
	    }
	}
#endif

	if (!priv) {
		/* If the drive does not support software eject, then we
		 * can't lock the caddy.
		 */
		if (!app_data.eject_supp) {
			app_data.caddylock_supp = FALSE;
			app_data.done_eject = FALSE;
			app_data.exit_eject = FALSE;
		}

		/* playOnLoad overrides spinDownOnLoad */
		if (app_data.load_play)
			app_data.load_spindown = FALSE;

		/* If the drive does not support locking the caddy, don't
		 * attempt to lock it.
		 */
		if (!app_data.caddylock_supp)
			app_data.caddy_lock = FALSE;

		/* If the drive does not support software volume
		 * control, then it can't support the balance
		 * control either.  Also, force the volume control
		 * taper selector to the linear position.
		 */
		if (!app_data.mselvol_supp) {
			app_data.balance_supp = FALSE;
			app_data.vol_taper = 0;
		}

		/* If the drive does not support channel routing,
		 * force the channel routing setting to normal.
		 */
		if (!app_data.chroute_supp)
			app_data.ch_route = 0;

		/* Other fix-ups as needed */
		if (app_data.numdiscs <= 0)
			app_data.numdiscs = 1;

		if (app_data.numdiscs == 1) {
			app_data.multi_play = app_data.reverse = FALSE;
		}

		if (app_data.startup_vol > 100)
			app_data.startup_vol = 100;
		else if (app_data.startup_vol < -1)
			app_data.startup_vol = -1;

	}
}


/*
 * di_devspec_parmsave
 *	Save the device-specific configuration parameters to file.
 *
 * Args:
 *	path - Path name to the file to save to.
 *
 * Return:
 *	Nothing.
 */
void
di_devspec_parmsave(char *path)
{
	int		ret;
	pid_t		cpid;
	waitret_t	stat_val;
	FILE		*fp;
	char		*truestr = "True",
			*falsestr = "False",
			*dirpath,
			errstr[ERR_BUF_SZ];

	/* Fork child to perform actual I/O */
	switch (cpid = FORK()) {
	case 0:
		/* Child process */
		break;

	case -1:
		DBGPRN(errfp, "di_devspec_parmsave: fork failed (errno=%d)\n",
			errno);
		return;

	default:
		/* Parent process: wait for child to exit */
		while ((ret = WAITPID(cpid, &stat_val, 0)) != cpid) {
			if (ret < 0)
				return;
		}

		if (WIFEXITED(stat_val)) {
		    if (WEXITSTATUS(stat_val) != 0) {
			DBGPRN(errfp,
			    "di_devspec_parmsave: child exited (status=%d)\n",
			    WEXITSTATUS(stat_val));
			(void) sprintf(errstr, app_data.str_nocfg,
				       path);
			DI_WARNING(errstr);
		    }
		    return;
		}
		else if (WIFSIGNALED(stat_val)) {
		    DBGPRN(errfp, "di_devspec_parmsave: child killed.\n");
		    (void) sprintf(errstr, app_data.str_nocfg, path);
		    DI_WARNING(errstr);
		    return;
		}
		else
		    return;
	}

	DBGPRN(errfp, "Setting uid to %d, gid to %d\n",
		(int) util_get_ouid(), (int) util_get_ogid());

	/* Force uid and gid to original setting */
	if (setuid(util_get_ouid()) < 0 || setgid(util_get_ogid()) < 0)
		exit(1);

#ifndef __VMS
	dirpath = util_dirname(path);
	if (!util_mkdir(dirpath, 0755)) {
		DBGPRN(errfp, "di_devspec_parmsave: cannot mkdir %s.\n",
			dirpath);
		exit(2);
	}
#endif

	DBGPRN(errfp, "Writing device-specific file %s\n", path);

	/* Open file for writing */
	if ((fp = fopen(path, "w")) == NULL) {
		DBGPRN(errfp, "di_devspec_parmsave: cannot open %s.\n", path);
		exit(2);
	}

	/* Write banner */
	(void) fprintf(fp, "# xmcd %s Device-Specific Configuration File\n",
		       VERSION);
	(void) fprintf(fp, "# %s\n#\n# %s %s%s\n# %s\n#\n",
		       COPYRIGHT,
		       "See the", app_data.libdir, "/config/device.cfg",
		       "file for details about the parameters.");

	/* Write only user-changeable parameters */
	(void) fprintf(fp, "volumeControlTaper:\t%d\n",
		       app_data.vol_taper);
	(void) fprintf(fp, "channelRoute:\t\t%d\n",
		       app_data.ch_route);
	(void) fprintf(fp, "spinDownOnLoad:\t\t%s\n",
		       app_data.load_spindown ? truestr : falsestr);
	(void) fprintf(fp, "playOnLoad:\t\t%s\n",
		       app_data.load_play ? truestr : falsestr);
	(void) fprintf(fp, "ejectOnDone:\t\t%s\n",
		       app_data.done_eject ? truestr : falsestr);
	(void) fprintf(fp, "ejectOnExit:\t\t%s\n",
		       app_data.exit_eject ? truestr : falsestr);
	(void) fprintf(fp, "stopOnExit:\t\t%s\n",
		       app_data.exit_stop ? truestr : falsestr);
	(void) fprintf(fp, "exitOnEject:\t\t%s\n",
		       app_data.eject_exit ? truestr : falsestr);
	(void) fprintf(fp, "caddyLock:\t\t%s\n",
		       app_data.caddy_lock ? truestr : falsestr);
	(void) fprintf(fp, "repeatMode:\t\t%s\n",
		       app_data.repeat_mode ? truestr : falsestr);
	(void) fprintf(fp, "shuffleMode:\t\t%s\n",
		       app_data.shuffle_mode ? truestr : falsestr);
	(void) fprintf(fp, "multiPlay:\t\t%s\n",
		       app_data.multi_play ? truestr : falsestr);
	(void) fprintf(fp, "reversePlay:\t\t%s\n",
		       app_data.reverse ? truestr : falsestr);

	if (fclose(fp) != 0)
		exit(1);

	(void) chmod(path, 0644);

	/* Child exits here */
	exit(0);

	/*NOTREACHED*/
}


/*
 * di_devlock
 *	Create a lock to prevent another cooperating CD audio process
 *	from accessing the same CD-ROM device.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	path - The CD-ROM device node path name.
 *
 * Return:
 *	TRUE if the lock was successful.  If FALSE, then it indicates
 *	that another xmcd process currently has the lock.
 */
/*ARGSUSED*/
bool_t
di_devlock(curstat_t *s, char *path)
{
#ifndef __VMS
/* UNIX */
	int		fd;
	pid_t		pid,
			mypid;
	char		buf[12];
	struct stat	stbuf;

	if (di_isdemo())
		return TRUE;	/* No locking needed in demo mode */

	if (stat(path, &stbuf) < 0) {
		if (errno == ENOENT)
			/* If the device node is missing it is probably
			 * due to dynamic node creation/removal
			 * on some platforms based on presence of media.
			 * In this case, let the lock call succeed without
			 * actually setting a lock.  Otherwise the status
			 * would erroneously become "cd busy".
			 */
			return TRUE;
		else
			return FALSE;
	}

	(void) sprintf(lockfile, "%s/lock.%x", TEMP_DIR, (int) stbuf.st_rdev);

	DBGPRN(errfp, "\nLock file: %s\n", lockfile);

	mypid = getpid();

	for (;;) {
		fd = open(lockfile, O_CREAT | O_EXCL | O_WRONLY);
		if (fd < 0) {
			if (errno == EEXIST) {
				if ((fd = open(lockfile, O_RDONLY)) < 0)
					return FALSE;

				if (read(fd, buf, 12) > 0)
					pid = (pid_t) atoi(buf);
				else {
					(void) close(fd);
					return FALSE;
				}

				(void) close(fd);

				if (pid == mypid)
					/* Our own lock */
					return TRUE;

				if (pid <= 0 ||
				    (kill(pid, 0) < 0 && errno == ESRCH)) {
					/* Pid died, steal its lockfile */
					(void) UNLINK(lockfile);
				}
				else {
					/* Pid still running: clash */
					return FALSE;
				}
			}
			else
				return FALSE;
		}
		else {
			(void) sprintf(buf, "%d\n", (int) mypid);
			(void) write(fd, buf, strlen(buf));

			(void) close(fd);
			(void) chmod(lockfile, 0644);

			return TRUE;
		}
	}
#else
/* OpenVMS */
	static char		ref_cnt;
	int			status;

	struct {
		short		buflen;
		short		code;
		char		*bufadr;
		int		*length;
		int		terminator;
	} itemlist = {
		4,
		DVI$_REFCNT,
		&ref_cnt,
		NULL,
		0
	};

        struct dsc$descriptor	dev_nam_desc;

	if (path == NULL)
		return TRUE;

	ref_cnt = 1;

	dev_nam_desc.dsc$b_class = DSC$K_CLASS_S;
	dev_nam_desc.dsc$b_dtype = DSC$K_DTYPE_T;
	dev_nam_desc.dsc$a_pointer = path;
	dev_nam_desc.dsc$w_length = strlen(path);

	status = SYS$GETDVIW(0, 0, &dev_nam_desc, &itemlist, 0, 0, 0, 0);

	if (status != 1) { 
		(void) fprintf(errfp,
		       "CD audio: Cannot get information on device: %s\n",
			    path);
		return FALSE;
	}
	
	return ((bool_t) (ref_cnt == 0));
#endif	/* __VMS */
}


/*
 * di_devunlock
 *	Unlock the lock that was created with di_devlock().
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
void
di_devunlock(curstat_t *s)
{
#ifndef __VMS
	if (lockfile[0] != '\0' && s->devlocked)
		(void) UNLINK(lockfile);
#endif
}


/*
 * di_devreg
 *	Register a path name in the devicepath registry.
 *
 * Args:
 *	fd - The device file descriptor
 *	path - The device path name
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
di_devreg(int fd, char *path)
{
	di_devreg_t	*rp;

	if (path == NULL || *path == '\0')
		return FALSE;

	rp = (di_devreg_t *)(void *) MEM_ALLOC(
		"di_devreg_t",
		sizeof(di_devreg_t)
	);
	if (rp == NULL) {
		DI_FATAL(app_data.str_nomemory);
		return FALSE;
	}
	rp->path = (char *) MEM_ALLOC("rp->path", strlen(path) + 1);
	if (rp == NULL) {
		DI_FATAL(app_data.str_nomemory);
		return FALSE;
	}

	(void) strcpy(rp->path, path);
	rp->fd = fd;
	rp->next = di_reghead;
	di_reghead = rp;
	DBGPRN(errfp, "\nRegistered %s\n", path);

	return TRUE;
}


/*
 * di_devunreg
 *	Unregister a device from the device path registry.
 *
 * Args:
 *	fd - The device file descriptor.
 *
 * Return:
 *	Nothing.
 */
void
di_devunreg(int fd)
{
	di_devreg_t	*rp,
			*prp;

	for (rp = di_reghead, prp = NULL; rp != NULL; rp = rp->next) {
		if (rp->fd == fd) {
			if (prp == NULL)
				di_reghead = rp->next;
			else
				prp->next = rp->next;

			DBGPRN(errfp, "\nUnregistered %s\n", rp->path);

			MEM_FREE(rp->path);
			MEM_FREE(rp);
			break;
		}
		prp = rp;
	}
}


/*
 * di_devgetpath
 *	Search the device path registry and return the path name
 *	string corresponding to the specified file descriptor.
 *
 * Args:
 *	fd - The device file descriptor.
 *
 * Return:
 *	Pointer to the device path string, or "(unknown)" if not found
 *	in the registered path list.
 */
char *
di_devgetpath(int fd)
{
	static di_devreg_t	*rp = NULL;

	if (rp != NULL && rp->fd == fd)
		return (rp->path);

	for (rp = di_reghead; rp != NULL; rp = rp->next) {
		if (rp->fd == fd)
			break;
	}

	return (rp == NULL ? "(unknown)" : rp->path);
}


/*
 * di_mode
 *	Return a text string indicating the current operating mode.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Mode text string.
 */
char *
di_mode(void)
{
	if (ditbl[app_data.di_method].mode != NULL)
		return (ditbl[app_data.di_method].mode());

	return ("");
}


/*
 * di_vers
 *	Return a text string indicating active module's version number
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Version text string.
 */
char *
di_vers(void)
{
	if (ditbl[app_data.di_method].vers != NULL)
		return (ditbl[app_data.di_method].vers());

	return ("");
}


/*
 * di_isdemo
 *	Query if this is a demo-only version of the CD player.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	TRUE - demo-only version.
 *	FALSE - real version.
 */
bool_t
di_isdemo(void)
{
#ifdef DEMO_ONLY
	return TRUE;
#else
	return FALSE;
#endif
}


/*
 * di_curtrk_pos
 *	Return the trkinfo table offset location of the current playing
 *	CD track.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Integer offset into the trkinfo table, or -1 if not currently
 *	playing audio.
 */
int
di_curtrk_pos(curstat_t *s)
{
	int	i;

	if ((int) s->cur_trk <= 0)
		return -1;

	i = (int) s->cur_trk - 1;

	if (s->trkinfo[i].trkno == s->cur_trk)
		return (i);

	for (i = 0; i < MAXTRACK; i++) {
		if (s->trkinfo[i].trkno == s->cur_trk)
			return (i);
	}
	return -1;
}


/*
 * di_curprog_pos
 *	Return an integer representing the position of the current
 *	program or shuffle mode playing order (0 = first, 1 = second, ...).
 *	This routine should be used only when in program or shuffle play
 *	mode.
 *
 * Arg:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	An integer representing the position of the current program
 *	or shuffle mode playing order, or -1 if not in the appropriate mode.
 */
int
di_curprog_pos(curstat_t *s)
{
	return ((int) s->playorder[s->prog_cnt]);
}


/*
 * di_reset_curstat
 *	Reset the curstat_t structure to initial defaults.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *	clear_toc - Whether the trkinfo CD table-of-contents 
 *		should be cleared.
 *	eject - Whether the medium is being ejected (clear cur_disc)
 *
 * Return:
 *	Nothing.
 */
void
di_reset_curstat(curstat_t *s, bool_t clear_toc, bool_t eject)
{
	sword32_t	i;
	static bool_t	first_time = TRUE;

	s->cur_trk = s->cur_idx = -1;
	s->cur_tot_min = s->cur_tot_sec = s->cur_tot_frame = 0;
	s->cur_trk_min = s->cur_trk_sec = s->cur_trk_frame = 0;
	s->cur_tot_addr = s->cur_trk_addr = 0;
	s->sav_iaddr = 0;
	s->prog_cnt = 0;

	if (s->onetrk_prog) {
		s->onetrk_prog = FALSE;
		PROGCLEAR(s);
	}

	if (clear_toc) {
		s->flags = 0;
		s->first_trk = s->last_trk = -1;
		s->tot_min = s->tot_sec = 0;
		s->tot_trks = 0;
		s->tot_addr = 0;
		s->prog_tot = 0;
		s->program = FALSE;

		for (i = 0; i < MAXTRACK; i++) {
			s->trkinfo[i].trkno = -1;
			s->trkinfo[i].min = 0;
			s->trkinfo[i].sec = 0;
			s->trkinfo[i].frame = 0;
			s->trkinfo[i].addr = 0;
			s->trkinfo[i].type = TYP_AUDIO;
			s->playorder[i] = -1;
		}
	}

	if (eject) {
		s->mode = MOD_NODISC;
		s->rptcnt = 0;
	}

	if (first_time) {
		/* These are to be initialized only once */
		first_time = FALSE;

		s->first_disc = 1;
		s->last_disc = app_data.numdiscs;
		s->cur_disc = app_data.reverse ? s->last_disc : s->first_disc;
		s->prev_disc = -1;
		s->time_dpy = T_ELAPSED;
		s->repeat = s->shuffle = FALSE;
		s->rptcnt = 0;
		s->cddb = 0;
		s->level = 0;
		s->caddy_lock = FALSE;
		s->vendor[0] = '\0';
		s->prod[0] = '\0';
		s->revnum[0] = '\0';
	}
}


/*
 * di_reset_shuffle
 *	Recompute a new shuffle play sequence.  Updates the playorder
 *	table in the curstat_t structure.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	Nothing.
 */
void
di_reset_shuffle(curstat_t *s)
{
	sword32_t	i,
			j,
			n;

	srand((unsigned) time(NULL));
	s->prog_cnt = 0;
	s->prog_tot = s->tot_trks;

	for (i = 0; i < MAXTRACK; i++) {
		if (i >= (int) s->prog_tot) {
			s->playorder[i] = -1;
			continue;
		}

		do {
			n = rand() % (int) s->prog_tot;
			for (j = 0; j < i; j++) {
				if (n == s->playorder[j])
					break;
			}
		} while (j < i);

		s->playorder[i] = n;
	}


	if (app_data.debug) {
		(void) fprintf(errfp, "\nShuffle tracks: ");

		for (i = 0; i < (int) s->prog_tot; i++)
			(void) fprintf(errfp, "%d ",
				       s->trkinfo[s->playorder[i]].trkno);

		(void) fprintf(errfp, "\n");
	}
}


/*
 * di_taper_vol
 *	Translate the volume level based on the configured taper
 *	characteristics.
 *
 * Args:
 *	v - The linear volume value.
 *
 * Return:
 *	The curved volume value.
 */
int
di_taper_vol(int v)
{
	switch (app_data.vol_taper) {
	case 1:
		/* squared taper */
		return (SQR(v) / MAX_VOL);
	case 2:
		/* inverse-squared taper */
		return (MAX_VOL - (SQR(MAX_VOL - v) / MAX_VOL));
	case 0:
	default:
		/* linear taper */
		return (v);
	}
	/*NOTREACHED*/
}


/*
 * di_untaper_vol
 *	Translate the volume level based on the configured taper
 *	characteristics.
 *
 * Args:
 *	v - The curved volume value.
 *
 * Return:
 *	The linear volume value.
 */
int
di_untaper_vol(int v)
{
	switch (app_data.vol_taper) {
	case 1:
		/* squared taper */
		return (util_isqrt(v) * 10);
	case 2:
		/* inverse-squared taper */
		return (MAX_VOL - util_isqrt(SQR(MAX_VOL) - (MAX_VOL * v)));
	case 0:
	default:
		/* linear taper */
		return (v);
	}
	/*NOTREACHED*/
}


/*
 * di_scale_vol
 *	Scale logical audio volume value (0-100) to an 8-bit value
 *	(0-0xff) range.
 *
 * Args:
 *	v - The logical volume value
 *
 * Return:
 *	The scaled volume value
 */
int
di_scale_vol(int v)
{
	/* Convert logical audio volume value to 8-bit volume */
	return ((v * (0xff - app_data.base_scsivol) / MAX_VOL) +
	        app_data.base_scsivol);
}


/*
 * di_unscale_vol
 *	Scale an 8-bit audio volume parameter value (0-0xff) to the
 *	logical volume value (0-100).
 *
 * Args:
 *	v - The 8-bit volume value
 *
 * Return:
 *	The logical volume value
 */
int
di_unscale_vol(int v)
{
	register int	val;

	/* Convert 8-bit audio volume value to logical volume */
	val = (v - app_data.base_scsivol) * MAX_VOL /
	      (0xff - app_data.base_scsivol);

	return ((val < 0) ? 0 : val);
}


