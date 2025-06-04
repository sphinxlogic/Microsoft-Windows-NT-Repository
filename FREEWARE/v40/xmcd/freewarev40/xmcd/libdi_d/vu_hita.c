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
 *   Hitachi vendor-unique support
 *
 *   The name "Hitachi" is a trademark of Hitachi Corporation, and is
 *   used here for identification purposes only.
 */
#ifndef LINT
static char *_vu_hita_c_ident_ = "@(#)vu_hita.c	6.23 98/05/10";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#ifdef VENDOR_HITACHI

extern appdata_t	app_data;
extern di_client_t	*di_clinfo;
extern vu_tbl_t		scsipt_vutbl[];
extern byte_t		cdb[];
extern int		devfd;

STATIC bool_t		hita_paused,			/* Currently paused */
			hita_playing,			/* Currently playing */
			hita_audio_muted;		/* Audio is muted */
STATIC word32_t		hita_pause_addr;		/* Pause addr */
STATIC msf_t		hita_sav_end;			/* Save addr */


/*
 * Internal functions
 */

/*
 * hita_do_pause
 *	Send a vendor-unique Pause command to the drive
 *
 * Args:
 *	ret_addr - Pointer to a buffer where the paused address will be
 *		   written to.  If NULL, no pause address info will
 *		   returned.
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
hita_do_pause(hmsf_t *ret_addr)
{
	hmsf_t	pause_addr;
	bool_t	ret;

	SCSICDB_RESET(cdb);
	cdb[0] = OP_VH_PAUSE;

	if ((ret = pthru_send(devfd, cdb, 12,
			      (byte_t *) AD_VH_PAUSE(&pause_addr),
			      SZ_VH_PAUSE, OP_DATAIN, 5, TRUE)) == TRUE) {
		DBGDUMP("hita: Pause address", (byte_t *) &pause_addr,
			sizeof(hmsf_t));

		if (ret_addr != NULL)
			*ret_addr = pause_addr;	/* structure copy */
	}
	return (ret);
}


/*
 * Public functions
 */

/*
 * hita_playaudio
 *	Play audio function: send vendor-unique play audio command
 *	to the drive.
 *
 * Args:
 *	addr_fmt - Flags indicating which address formats are passed in
 *	If ADDR_BLK, then:
 *	    start_addr - The logical block starting address
 *	    end_addr - The logical block ending address
 *	If ADD_MSF, then:
 *	    start_msf - Pointer to the starting MSF address structure
 *	    end_msf - Pointer to the ending MSF address structure
 *	If ADDR_TRKIDX, then:
 *	    trk - The starting track number
 *	    idx - The starting index number
 *	If ADDR_OPTEND, then the ending address, if specified, can be
 *	ignored if possible.
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
/*ARGSUSED*/
bool_t
hita_playaudio(
	byte_t		addr_fmt,
	word32_t	start_addr,
	word32_t	end_addr,
	msf_t		*start_msf,
	msf_t		*end_msf,
	byte_t		trk,
	byte_t		idx
)
{
	msf_t		istart_msf,
			iend_msf;
	bool_t		ret = FALSE;

	if (!ret && (addr_fmt & ADDR_BLK) && !(addr_fmt & ADDR_MSF)) {
		/* Convert block address to MSF format */
		util_blktomsf(
			start_addr,
			&istart_msf.min, &istart_msf.sec, &istart_msf.frame,
			MSF_OFFSET
		);

		util_blktomsf(
			end_addr,
			&iend_msf.min, &iend_msf.sec, &iend_msf.frame,
			MSF_OFFSET
		);

		/* Let the ADDR_MSF code handle the request */
		start_msf = &istart_msf;
		end_msf = &iend_msf;
		addr_fmt |= ADDR_MSF;
		ret = FALSE;
	}

	if (!ret && (addr_fmt & ADDR_MSF)) {
		hita_sav_end.min = (byte_t) end_msf->min;
		hita_sav_end.sec = (byte_t) end_msf->sec;
		hita_sav_end.frame = (byte_t) end_msf->frame;

		/* Send a pause command to cease any current audio playback,
		 * then send the actual play audio command.
		 */
		if ((ret = hita_do_pause(NULL)) == TRUE) {
			SCSICDB_RESET(cdb);
			cdb[0] = OP_VH_AUDPLAY;
			cdb[1] = hita_audio_muted ? 0x07 : 0x01;
			cdb[2] = start_msf->min;
			cdb[3] = start_msf->sec;
			cdb[4] = start_msf->frame;
			cdb[7] = end_msf->min;
			cdb[8] = end_msf->sec;
			cdb[9] = end_msf->frame;

			ret = pthru_send(devfd, cdb, 12, NULL, 0, OP_NODATA,
					 10, TRUE);
		}
	}

	if (ret) {
		hita_paused = FALSE;
		hita_playing = TRUE;
	}

	return (ret);
}


/*
 * hita_pause_resume
 *	Pause/resume function: send vendor-unique commands to implement
 *	the pause and resume capability.
 *
 * Args:
 *	resume - TRUE: resume, FALSE: pause
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
hita_pause_resume(bool_t resume)
{
	hmsf_t		*a;
	bool_t		ret = FALSE;

	a = (hmsf_t *) &hita_pause_addr;

        if (resume) {
		if (!hita_paused)
			return TRUE;

		SCSICDB_RESET(cdb);
		cdb[0] = OP_VH_AUDPLAY;
		cdb[1] = hita_audio_muted ? 0x07 : 0x01;
		cdb[2] = a->min;
		cdb[3] = a->sec;
		cdb[4] = a->frame;
		cdb[7] = hita_sav_end.min;
		cdb[8] = hita_sav_end.sec;
		cdb[9] = hita_sav_end.frame;

		ret = pthru_send(devfd, cdb, 12, NULL, 0, OP_NODATA, 10, TRUE);
        }
        else {
		if (hita_paused)
			return TRUE;

		ret = hita_do_pause(a);
	}

	if (ret) {
		hita_paused = !resume;
		hita_playing = !hita_paused;
	}

	return (ret);
}


/*
 * hita_start_stop
 *	Start/stop function: When playing audio, the Hitachi drive must
 *	first be paused before sending a Start/Stop Unit command to
 *	stop it.
 *
 * Args:
 *	start - TRUE: start unit, FALSE: stop unit
 *	loej - TRUE: eject caddy, FALSE: do not eject
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
hita_start_stop(bool_t start, bool_t loej)
{
	if (!start && hita_playing && !hita_do_pause(NULL))
		return FALSE;

	hita_paused = FALSE;

	return (scsipt_start_stop(devfd, start, loej, TRUE));
}


/*
 * hita_get_playstatus
 *	Send vendor-unique command to obtain current audio playback
 *	status.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *	audio_status - Address where a current status code (SCSI-2
 *		       style) is to be returned.
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
hita_get_playstatus(curstat_t *s, byte_t *audio_status)
{
	int		trkno,
			idxno;
	byte_t		buf[SZ_VH_RDSTAT];
	haudstat_t	*d;


	(void) memset(buf, 0, sizeof(buf));

	SCSICDB_RESET(cdb);
	cdb[0] = OP_VH_RDSTAT;

	if (!pthru_send(devfd, cdb, 12, AD_VH_RDSTAT(buf), SZ_VH_RDSTAT,
			OP_DATAIN, 5, TRUE))
		return FALSE;

	DBGDUMP("hita: Read Status data bytes", buf, SZ_VH_RDSTAT);

	d = (haudstat_t *)(void *) buf;

	trkno = (word32_t) d->trkno;
	if (s->cur_trk != trkno) {
		s->cur_trk = trkno;
		DPY_TRACK(s);
	}
	idxno = 1;	/* Fudge */
	if (s->cur_idx != idxno) {
		s->cur_idx = idxno;
		DPY_INDEX(s);
	}

	s->cur_tot_min = (byte_t) d->abs_addr.min;
	s->cur_tot_sec = (byte_t) d->abs_addr.sec;
	s->cur_tot_frame = (byte_t) d->abs_addr.frame;
	s->cur_trk_min = (byte_t) d->rel_addr.min;
	s->cur_trk_sec = (byte_t) d->rel_addr.sec;
	s->cur_trk_frame = (byte_t) d->rel_addr.frame;
	util_msftoblk(
		s->cur_tot_min, s->cur_tot_sec, s->cur_tot_frame,
		&s->cur_tot_addr, MSF_OFFSET
	);
	util_msftoblk(
		s->cur_trk_min, s->cur_trk_sec, s->cur_trk_frame,
		&s->cur_trk_addr, 0
	);

	/* Make up SCSI-2 style audio status */
	if (hita_paused)
		*audio_status = AUDIO_PAUSED;
	else if (d->playing)
		*audio_status = AUDIO_PLAYING;
	else {
		*audio_status = AUDIO_COMPLETED;
		hita_playing = FALSE;
	}

	return TRUE;
}


/*
 * hita_get_toc
 *	Send vendor-unique command to obtain the disc table-of-contents
 *
 * Args:
 *	s - Pointer to the curstat_t structure, which contains the TOC
 *	    table to be updated.
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
hita_get_toc(curstat_t *s)
{
	int		i,
			j,
			xfer_len;
	byte_t		buf[SZ_VH_RDXINFO];
	hxdiscinfo_t	*p;
	hxmsf_t		*a;


	if (hita_playing)
		return FALSE;	/* Drive is busy */

	(void) memset(buf, 0, sizeof(buf));

	/* Read the TOC header first */
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VH_RDXINFO;
	cdb[10] = SZ_VH_XTOCHDR;

	if (!pthru_send(devfd, cdb, 12, AD_VH_RDXINFO(buf), SZ_VH_XTOCHDR,
			OP_DATAIN, 5, TRUE))
		return FALSE;

	p = (hxdiscinfo_t *)(void *) buf;

	s->first_trk = (byte_t) p->first_trk;
	s->last_trk = (byte_t) p->last_trk;

	xfer_len = SZ_VH_XTOCHDR +
		   ((int) (p->last_trk - p->first_trk + 2) * SZ_VH_XTOCENT);

	if (xfer_len > SZ_VH_RDXINFO)
		xfer_len = SZ_VH_RDXINFO;

	/* Read the appropriate number of bytes of the entire TOC */
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VH_RDXINFO;
	cdb[9] = (xfer_len & 0xff00) >> 8;
	cdb[10] = xfer_len & 0x00ff;

	if (!pthru_send(devfd, cdb, 12, AD_VH_RDXINFO(buf), xfer_len, OP_DATAIN,
			5, TRUE))
		return FALSE;

	DBGDUMP("hita: Read Extended Disc Info data bytes", buf, xfer_len);

	/* Get the starting position of each track */
	for (i = 0, j = (int) s->first_trk; j <= (int) s->last_trk; i++, j++) {
		a = (hxmsf_t *)(void *) &p->xmsfdata[(i+1) * SZ_VH_XTOCENT];
		s->trkinfo[i].trkno = j;
		s->trkinfo[i].min = (byte_t) a->min;
		s->trkinfo[i].sec = (byte_t) a->sec;
		s->trkinfo[i].frame = (byte_t) a->frame;
		util_msftoblk(
			s->trkinfo[i].min,
			s->trkinfo[i].sec,
			s->trkinfo[i].frame,
			&s->trkinfo[i].addr,
			MSF_OFFSET
		);
		s->trkinfo[i].type = (a->trktype == 0) ? TYP_AUDIO : TYP_DATA;
	}
	s->tot_trks = (byte_t) i;

	/* Get the lead-out track position */
	a = (hxmsf_t *)(void *) &p->xmsfdata[0];
	s->trkinfo[i].trkno = LEAD_OUT_TRACK;
	s->tot_min = s->trkinfo[i].min = (byte_t) a->min;
	s->tot_sec = s->trkinfo[i].sec = (byte_t) a->sec;
	s->tot_frame = s->trkinfo[i].frame = (byte_t) a->frame;
	util_msftoblk(
		s->trkinfo[i].min,
		s->trkinfo[i].sec,
		s->trkinfo[i].frame,
		&s->trkinfo[i].addr,
		MSF_OFFSET
	);
	s->tot_addr = s->trkinfo[i].addr;

	return TRUE;
}


/*
 * hita_mute
 *	Send vendor-unique command to mute/unmute the audio
 *
 * Args:
 *	mute - TRUE: mute audio, FALSE: un-mute audio
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
hita_mute(bool_t mute)
{
	curstat_t	*s = di_clinfo->curstat_addr();

	if (mute == hita_audio_muted)
		return TRUE;

	if (hita_playing) {
		/* Pause the playback first */
        	if (!hita_do_pause(NULL))
			return FALSE;

		SCSICDB_RESET(cdb);
		cdb[0] = OP_VH_AUDPLAY;
		cdb[1] = mute ? 0x07 : 0x01;
		cdb[2] = s->cur_tot_min;
		cdb[3] = s->cur_tot_sec;
		cdb[4] = s->cur_tot_frame;
		cdb[7] = hita_sav_end.min;
		cdb[8] = hita_sav_end.sec;
		cdb[9] = hita_sav_end.frame;

		if (!pthru_send(devfd, cdb, 12, NULL, 0, OP_NODATA, 10, TRUE))
			return FALSE;
	}

	hita_audio_muted = mute;

	return TRUE;
}


/*
 * hita_eject
 *	Send vendor-unique command to eject the caddy
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
hita_eject(void)
{
	/* If audio playback is in progress, pause the playback first */
	if (hita_playing && !hita_do_pause(NULL))
		return FALSE;

	hita_playing = hita_paused = FALSE;

	/* Eject the caddy */
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VH_EJECT;
	cdb[10] = 0x01;
	return (pthru_send(devfd, cdb, 12, NULL, 0, OP_NODATA, 20, TRUE));
}


/*
 * hita_init
 *	Initialize the vendor-unique support module
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
void
hita_init(void)
{
	/* Register vendor_unique module entry points */
	scsipt_vutbl[VENDOR_HITACHI].vendor = "Hitachi";
	scsipt_vutbl[VENDOR_HITACHI].playaudio = hita_playaudio;
	scsipt_vutbl[VENDOR_HITACHI].pause_resume = hita_pause_resume;
	scsipt_vutbl[VENDOR_HITACHI].start_stop = hita_start_stop;
	scsipt_vutbl[VENDOR_HITACHI].get_playstatus = hita_get_playstatus;
	scsipt_vutbl[VENDOR_HITACHI].volume = NULL;
	scsipt_vutbl[VENDOR_HITACHI].route = NULL;
	scsipt_vutbl[VENDOR_HITACHI].mute = hita_mute;
	scsipt_vutbl[VENDOR_HITACHI].get_toc = hita_get_toc;
	scsipt_vutbl[VENDOR_HITACHI].eject = hita_eject;
	scsipt_vutbl[VENDOR_HITACHI].start = NULL;
	scsipt_vutbl[VENDOR_HITACHI].halt = NULL;
}


#endif	/* VENDOR_HITACHI */

