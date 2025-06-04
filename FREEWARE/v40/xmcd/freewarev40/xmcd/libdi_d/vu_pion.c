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
 *   Pioneer vendor-unique support
 *
 *   The name "Pioneer" is a trademark of Pioneer Corporation, and is
 *   used here for identification purposes only.
 */
#ifndef LINT
static char *_vu_pion_c_ident_ = "@(#)vu_pion.c	6.18 98/05/10";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#ifdef VENDOR_PIONEER

extern appdata_t	app_data;
extern di_client_t	*di_clinfo;
extern vu_tbl_t		scsipt_vutbl[];
extern byte_t		cdb[];
extern int		devfd;

/*
 * pion_playaudio
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
pion_playaudio(
	byte_t		addr_fmt,
	word32_t	start_addr,
	word32_t	end_addr,
	msf_t		*start_msf,
	msf_t		*end_msf,
	byte_t		trk,
	byte_t		idx
)
{
	bool_t		ret = FALSE;

	if (!ret && addr_fmt & ADDR_MSF) {
		/* Position laser head at desired location
		 * and start play.
		 */
		SCSICDB_RESET(cdb);
		cdb[0] = OP_VP_AUDSRCH;
		cdb[1] = 0x19;
		cdb[3] = (byte_t) util_ltobcd(start_msf->min);
		cdb[4] = (byte_t) util_ltobcd(start_msf->sec);
		cdb[5] = (byte_t) util_ltobcd(start_msf->frame);
		cdb[9] = 0x40;

		ret = pthru_send(devfd, cdb, 10, NULL, 0, OP_NODATA, 10, TRUE);

		if (ret && !(addr_fmt & ADDR_OPTEND)) {
			/* Specify end location and start play */
			SCSICDB_RESET(cdb);
			cdb[0] = OP_VP_AUDPLAY;
			cdb[1] = 0x19;
			cdb[3] = (byte_t) util_ltobcd(end_msf->min);
			cdb[4] = (byte_t) util_ltobcd(end_msf->sec);
			cdb[5] = (byte_t) util_ltobcd(end_msf->frame);
			cdb[9] = 0x40;

			ret = pthru_send(devfd, cdb, 10, NULL, 0, OP_NODATA,
					 10, TRUE);
		}
	}

	if (!ret && addr_fmt & ADDR_BLK) {
		/* Position laser head at desired location
		 * and start play.
		 */
		SCSICDB_RESET(cdb);
		cdb[0] = OP_VP_AUDSRCH;
		cdb[1] = 0x19;
		cdb[2] = (start_addr & 0xff000000) >> 24;
		cdb[3] = (start_addr & 0x00ff0000) >> 16;
		cdb[4] = (start_addr & 0x0000ff00) >> 8;
		cdb[5] = (start_addr & 0x000000ff);
		cdb[9] = 0x00;

		ret = pthru_send(devfd, cdb, 10, NULL, 0, OP_NODATA, 10, TRUE);

		if (ret && !(addr_fmt & ADDR_OPTEND)) {
			/* Specify end location and start play */
			SCSICDB_RESET(cdb);
			cdb[0] = OP_VP_AUDPLAY;
			cdb[1] = 0x19;
			cdb[2] = (end_addr & 0xff000000) >> 24;
			cdb[3] = (end_addr & 0x00ff0000) >> 16;
			cdb[4] = (end_addr & 0x0000ff00) >> 8;
			cdb[5] = (end_addr & 0x000000ff);
			cdb[9] = 0x00;

			ret = pthru_send(devfd, cdb, 10, NULL, 0, OP_NODATA,
					 10, TRUE);
		}
	}

	return (ret);
}


/*
 * pion_pause_resume
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
pion_pause_resume(bool_t resume)
{
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VP_PAUSE;
	cdb[1] = resume ? 0x00 : 0x10;

	return (pthru_send(devfd, cdb, 10, NULL, 0, OP_NODATA, 5, TRUE));
}


/*
 * pion_get_playstatus
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
pion_get_playstatus(curstat_t *s, byte_t *audio_status)
{
	int			i,
				trkno,
				idxno;
	byte_t			buf[sizeof(psubq_data_t)];
	bool_t			stopped;
	paudstat_data_t		*a;
	psubq_data_t		*d;


	(void) memset(buf, 0, sizeof(buf));

	/* Send Pioneer Read Audio Status command */
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VP_AUDSTAT;
	cdb[8] = SZ_VP_AUDSTAT;

	if (!pthru_send(devfd, cdb, 10, buf, SZ_VP_AUDSTAT, OP_DATAIN, 5, TRUE))
		return FALSE;

	DBGDUMP("pion: Read Audio Status data bytes", buf, SZ_VP_AUDSTAT);

	a = (paudstat_data_t *)(void *) buf;

	stopped = FALSE;

	/* Translate Pioneer audio status to SCSI-2 audio status */
	switch (a->status) {
	case PAUD_PLAYING:
	case PAUD_MUTEPLAY:
		*audio_status = AUDIO_PLAYING;
		break;

	case PAUD_PAUSED:
		*audio_status = AUDIO_PAUSED;
		break;

	case PAUD_COMPLETED:
		*audio_status = AUDIO_COMPLETED;
		stopped = TRUE;
		break;

	case PAUD_ERROR:
		*audio_status = AUDIO_FAILED;
		stopped = TRUE;
		break;

	case PAUD_NOSTATUS:
		*audio_status = AUDIO_NOSTATUS;
		stopped = TRUE;
		break;
	}

	if (stopped) {
		s->cur_tot_min = (byte_t) util_bcdtol(a->abs_min);
		s->cur_tot_sec = (byte_t) util_bcdtol(a->abs_sec);
		s->cur_tot_frame = (byte_t) util_bcdtol(a->abs_frame);

		if ((i = di_curtrk_pos(s)) >= 0)
			s->trkinfo[i].type =
				(a->trktype == 0) ? TYP_AUDIO : TYP_DATA;

		return TRUE;
	}

	(void) memset(buf, 0, sizeof(buf));

	/* Send Pioneer Read Subcode Q command */
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VP_RDSUBQ;
	cdb[8] = SZ_VP_RDSUBQ;

	if (!pthru_send(devfd, cdb, 10, buf, SZ_VP_RDSUBQ, OP_DATAIN, 5, TRUE))
		return FALSE;

	DBGDUMP("pion: Read Subcode Q data bytes", buf, SZ_VP_RDSUBQ);

	d = (psubq_data_t *)(void *) buf;

	trkno = util_bcdtol((word32_t) d->trkno);
	if (s->cur_trk != trkno) {
		s->cur_trk = trkno;
		DPY_TRACK(s);
	}

	idxno = util_bcdtol((word32_t) d->idxno);
	if (s->cur_idx != idxno) {
		s->cur_idx = idxno;
		s->sav_iaddr = s->cur_tot_addr;
		DPY_INDEX(s);
	}

	if ((i = di_curtrk_pos(s)) >= 0)
		s->trkinfo[i].type =
			(d->trktype == 0) ? TYP_AUDIO : TYP_DATA;

	s->cur_tot_min = (byte_t) util_bcdtol(d->abs_min);
	s->cur_tot_sec = (byte_t) util_bcdtol(d->abs_sec);
	s->cur_tot_frame = (byte_t) util_bcdtol(d->abs_frame);
	s->cur_trk_min = (byte_t) util_bcdtol(d->rel_min);
	s->cur_trk_sec = (byte_t) util_bcdtol(d->rel_sec);
	s->cur_trk_frame = (byte_t) util_bcdtol(d->rel_frame);
	util_msftoblk(
		s->cur_tot_min, s->cur_tot_sec, s->cur_tot_frame,
		&s->cur_tot_addr, MSF_OFFSET
	);
	util_msftoblk(
		s->cur_trk_min, s->cur_trk_sec, s->cur_trk_frame,
		&s->cur_trk_addr, 0
	);

	return TRUE;
}


/*
 * pion_get_toc
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
pion_get_toc(curstat_t *s)
{
	int		i,
			j;
	byte_t		buf[SZ_VP_RDTOC];
	pinfo_00_t	*t0;
	pinfo_01_t	*t1;
	pinfo_02_t	*t2;


	(void) memset(buf, 0, sizeof(buf));

	/* Find number of tracks */
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VP_RDTOC;
	cdb[5] = 0x00;
	cdb[7] = (SZ_VP_RDTOC & 0xff00) >> 8;
	cdb[8] = (SZ_VP_RDTOC & 0x00ff);
	cdb[9] = 0x00;

	if (!pthru_send(devfd, cdb, 10, buf, SZ_VP_RDTOC, OP_DATAIN, 5, TRUE))
		return FALSE;

	DBGDUMP("pion: Read TOC data bytes", buf, SZ_VP_RDTOC);

	t0 = (pinfo_00_t *) buf;
	s->first_trk = (byte_t) util_bcdtol(t0->first_trk);
	s->last_trk = (byte_t) util_bcdtol(t0->last_trk);

	/* Get the starting position of each track */
	for (i = 0, j = (int) s->first_trk; j <= (int) s->last_trk; i++, j++) {
		(void) memset(buf, 0, sizeof(buf));

		SCSICDB_RESET(cdb);
		cdb[0] = OP_VP_RDTOC;
		cdb[5] = (byte_t) util_ltobcd(j);
		cdb[7] = (SZ_VP_RDTOC & 0xff00) >> 8;
		cdb[8] = (SZ_VP_RDTOC & 0x00ff);
		cdb[9] = 0x02;

		if (!pthru_send(devfd, cdb, 10, buf, SZ_VP_RDTOC, OP_DATAIN,
				5, TRUE))
			return FALSE;

		DBGDUMP("pion: Read TOC data bytes", buf, SZ_VP_RDTOC);

		t2 = (pinfo_02_t *)(void *) buf;

		s->trkinfo[i].trkno = j;
		s->trkinfo[i].min = (byte_t) util_bcdtol(t2->min);
		s->trkinfo[i].sec = (byte_t) util_bcdtol(t2->sec);
		s->trkinfo[i].frame = (byte_t) util_bcdtol(t2->frame);
		util_msftoblk(
			s->trkinfo[i].min,
			s->trkinfo[i].sec,
			s->trkinfo[i].frame,
			&s->trkinfo[i].addr,
			MSF_OFFSET
		);
	}
	s->tot_trks = (byte_t) i;

	(void) memset(buf, 0, sizeof(buf));

	/* Get the lead out track position */
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VP_RDTOC;
	cdb[5] = 0x00;
	cdb[7] = (SZ_VP_RDTOC & 0xff00) >> 8;
	cdb[8] = (SZ_VP_RDTOC & 0x00ff);
	cdb[9] = 0x01;

	if (!pthru_send(devfd, cdb, 10, buf, SZ_VP_RDTOC, OP_DATAIN, 5, TRUE))
		return FALSE;

	DBGDUMP("pion: Read TOC data bytes", buf, SZ_VP_RDTOC);

	t1 = (pinfo_01_t *) buf;

	s->trkinfo[i].trkno = LEAD_OUT_TRACK;
	s->tot_min = s->trkinfo[i].min = (byte_t) util_bcdtol(t1->min);
	s->tot_sec = s->trkinfo[i].sec = (byte_t) util_bcdtol(t1->sec);
	s->tot_frame = s->trkinfo[i].frame = (byte_t) util_bcdtol(t1->frame);
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
 * pion_eject
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
pion_eject(void)
{
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VP_EJECT;
	cdb[1] = 0x01;	/* Set immediate bit */

	return (pthru_send(devfd, cdb, 10, NULL, 0, OP_NODATA, 20, TRUE));
}


/*
 * pion_init
 *	Initialize the vendor-unique support module
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
void
pion_init(void)
{
	/* Register vendor_unique module entry points */
	scsipt_vutbl[VENDOR_PIONEER].vendor = "Pioneer";
	scsipt_vutbl[VENDOR_PIONEER].playaudio = pion_playaudio;
	scsipt_vutbl[VENDOR_PIONEER].pause_resume = pion_pause_resume;
	scsipt_vutbl[VENDOR_PIONEER].start_stop = NULL;
	scsipt_vutbl[VENDOR_PIONEER].get_playstatus = pion_get_playstatus;
	scsipt_vutbl[VENDOR_PIONEER].volume = NULL;
	scsipt_vutbl[VENDOR_PIONEER].route = NULL;
	scsipt_vutbl[VENDOR_PIONEER].mute = NULL;
	scsipt_vutbl[VENDOR_PIONEER].get_toc = pion_get_toc;
	scsipt_vutbl[VENDOR_PIONEER].eject = pion_eject;
	scsipt_vutbl[VENDOR_PIONEER].start = NULL;
	scsipt_vutbl[VENDOR_PIONEER].halt = NULL;
}


#endif	/* VENDOR_PIONEER */

