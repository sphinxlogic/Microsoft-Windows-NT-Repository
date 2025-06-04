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
 *   Sony vendor-unique support
 *
 *   The name "Sony" is a trademark of Sony Corporation, and is used
 *   here for identification purposes only.
 */
#ifndef LINT
static char *_vu_sony_c_ident_ = "@(#)vu_sony.c	6.27 98/05/10";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#ifdef VENDOR_SONY

extern appdata_t	app_data;
extern di_client_t	*di_clinfo;
extern vu_tbl_t		scsipt_vutbl[];
extern byte_t		cdb[];
extern int		devfd;

STATIC byte_t		sony_route_left,	/* left ch routing control */
			sony_route_right;	/* Right ch routing control */


/*
 * sony_route_val
 *	Return the channel routing control value used in the
 *	Sony Playback Status command.
 *
 * Args:
 *	route_mode - The channel routing mode value.
 *	channel - The channel number desired (0=left 1=right).
 *
 * Return:
 *	The routing control value.
 */
STATIC byte_t
sony_route_val(int route_mode, int channel)
{
	switch (channel) {
	case 0:
		switch (route_mode) {
		case 0:
			return 0x1;
		case 1:
			return 0x2;
		case 2:
			return 0x1;
		case 3:
			return 0x2;
		case 4:
			return 0x3;
		default:
			/* Invalid value */
			return 0x0;
		}
		/*NOTREACHED*/

	case 1:
		switch (route_mode) {
		case 0:
			return 0x2;
		case 1:
			return 0x1;
		case 2:
			return 0x1;
		case 3:
			return 0x2;
		case 4:
			return 0x3;
		default:
			/* Invalid value */
			return 0x0;
		}
		/*NOTREACHED*/

	default:
		/* Invalid value */
		return 0x0;
	}
}


/*
 * sony_playaudio
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
sony_playaudio(
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

	if (!ret && addr_fmt & ADDR_MSF) {
		SCSICDB_RESET(cdb);
		cdb[0] = OP_VS_PLAYMSF;
		cdb[3] = start_msf->min;
		cdb[4] = start_msf->sec;
		cdb[5] = start_msf->frame;
		cdb[6] = end_msf->min;
		cdb[7] = end_msf->sec;
		cdb[8] = end_msf->frame;

		ret = pthru_send(devfd, cdb, 10, NULL, 0, OP_NODATA, 10, TRUE);
	}

	return (ret);
}


/*
 * sony_pause_resume
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
sony_pause_resume(bool_t resume)
{
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VS_PAUSE;
	cdb[1] = resume ? 0x00 : 0x10;

	return(pthru_send(devfd, cdb, 10, NULL, 0, OP_NODATA, 5, TRUE));
}


/*
 * sony_get_playstatus
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
sony_get_playstatus(curstat_t *s, byte_t *audio_status)
{
	int		i,
			trkno,
			idxno;
	byte_t		dbuf[sizeof(sstat_data_t)],
			qbuf[sizeof(ssubq_data_t)];
	sstat_data_t	*d;
	ssubq_data_t	*q;


	/*
	 * Send Sony Playback Status command
	 */

	(void) memset(dbuf, 0, sizeof(dbuf));

	SCSICDB_RESET(cdb);
	cdb[0] = OP_VS_PLAYSTAT;
	cdb[8] = SZ_VS_PLAYSTAT;

	if (!pthru_send(devfd, cdb, 10, dbuf, SZ_VS_PLAYSTAT,
			OP_DATAIN, 5, TRUE))
		return FALSE;

	DBGDUMP("sony: Playback Status data bytes", dbuf, SZ_VS_PLAYSTAT);

	d = (sstat_data_t *)(void *) dbuf;

	/* Translate Sony audio status to SCSI-2 audio status */
	switch (d->audio_status) {
	case SAUD_PLAYING:
	case SAUD_MUTED:
		*audio_status = AUDIO_PLAYING;
		break;

	case SAUD_PAUSED:
		*audio_status = AUDIO_PAUSED;
		break;

	case SAUD_COMPLETED:
		*audio_status = AUDIO_COMPLETED;
		break;

	case SAUD_ERROR:
		*audio_status = AUDIO_FAILED;
		break;

	case SAUD_NOTREQ:
		*audio_status = AUDIO_NOSTATUS;
		break;
	}

	if ((i = di_curtrk_pos(s)) >= 0)
		s->trkinfo[i].type = (d->trktype == 0) ? TYP_AUDIO : TYP_DATA;


	/*
	 * Send Sony Read Subchannel command
	 */

	(void) memset(qbuf, 0, sizeof(qbuf));

	SCSICDB_RESET(cdb);
	cdb[0] = OP_VS_RDSUBQ;
	cdb[2] = 0x40;
	cdb[8] = SZ_VS_RDSUBQ;

	if (!pthru_send(devfd, cdb, 10, qbuf, SZ_VS_RDSUBQ, OP_DATAIN, 5, TRUE))
		return FALSE;

	DBGDUMP("sony: Read Subchannel data bytes", qbuf, SZ_VS_RDSUBQ);

	q = (ssubq_data_t *)(void *) qbuf;

	trkno = (word32_t) q->trkno;
	if (s->cur_trk != trkno) {
		s->cur_trk = trkno;
		DPY_TRACK(s);
	}

	idxno = (word32_t) q->idxno;
	if (s->cur_idx != idxno) {
		s->cur_idx = idxno;
		s->sav_iaddr = s->cur_tot_addr;
		DPY_INDEX(s);
	}

	s->cur_tot_min = (byte_t) q->abs_min;
	s->cur_tot_sec = (byte_t) q->abs_sec;
	s->cur_tot_frame = (byte_t) q->abs_frame;
	s->cur_trk_min = (byte_t) q->rel_min;
	s->cur_trk_sec = (byte_t) q->rel_sec;
	s->cur_trk_frame = (byte_t) q->rel_frame;
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
 * sony_get_toc
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
sony_get_toc(curstat_t *s)
{
	int		i,
			j,
			xfer_len;
	byte_t		buf[sizeof(stoc_data_t)];
	stoc_data_t	*d;
	stoc_ent_t	*e;

	(void) memset(buf, 0, sizeof(buf));

	/* Read TOC header to find the number of tracks */
	for (i = 1; i < MAXTRACK - 1; i++) {
		SCSICDB_RESET(cdb);
		cdb[0] = OP_VS_RDTOC;
		cdb[5] = (byte_t) i;
		cdb[8] = SZ_VS_TOCHDR;

		if (pthru_send(devfd, cdb, 10, buf, SZ_VS_TOCHDR, OP_DATAIN,
			       5, TRUE))
			break;
	}
	if (i == MAXTRACK - 1)
		return FALSE;

	d = (stoc_data_t *)(void *) buf;

	s->first_trk = (byte_t) d->first_trk;
	s->last_trk = (byte_t) d->last_trk;

	xfer_len = SZ_VS_TOCHDR +
		   ((int) (d->last_trk - d->first_trk + 2) * SZ_VS_TOCENT);

	if (xfer_len > SZ_VS_RDTOC)
		xfer_len = SZ_VS_RDTOC;

	SCSICDB_RESET(cdb);
	cdb[0] = OP_VS_RDTOC;
	cdb[5] = (byte_t) s->first_trk;
	cdb[7] = (xfer_len & 0xff00) >> 8;
	cdb[8] = (xfer_len & 0x00ff);

	if (!pthru_send(devfd, cdb, 10, buf, xfer_len, OP_DATAIN, 5, TRUE))
		return FALSE;

	DBGDUMP("sony: Read TOC data bytes", buf, SZ_VS_RDTOC);

	/* Get the starting position of each track */
	for (i = 0, j = (int) s->first_trk; j <= (int) s->last_trk; i++, j++) {
		e = (stoc_ent_t *)(void *) &d->trkdata[i * SZ_VS_TOCENT];
		s->trkinfo[i].trkno = j;
		s->trkinfo[i].min = (byte_t) e->min;
		s->trkinfo[i].sec = (byte_t) e->sec;
		s->trkinfo[i].frame = (byte_t) e->frame;
		util_msftoblk(
			s->trkinfo[i].min,
			s->trkinfo[i].sec,
			s->trkinfo[i].frame,
			&s->trkinfo[i].addr,
			MSF_OFFSET
		);
		s->trkinfo[i].type = (e->trktype == 0) ? TYP_AUDIO : TYP_DATA;
	}
	s->tot_trks = (byte_t) i;

	/* Get the lead-out track position */
	e = (stoc_ent_t *)(void *) &d->trkdata[i * SZ_VS_TOCENT];
	s->trkinfo[i].trkno = LEAD_OUT_TRACK;
	s->tot_min = s->trkinfo[i].min = (byte_t) e->min;
	s->tot_sec = s->trkinfo[i].sec = (byte_t) e->sec;
	s->tot_frame = s->trkinfo[i].frame = (byte_t) e->frame;
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
 * sony_volume
 *	Send vendor-unique command to query/control the playback volume.
 *
 * Args:
 *	vol - Volume level to set to
 *	s - Pointer to the curstat_t structure
 *	query - This call is to query the current volume setting rather
 *		than to set it.
 *
 * Return:
 *	The current volume value.
 */
int
sony_volume(int vol, curstat_t *s, bool_t query)
{
	int		vol1,
			vol2;
	byte_t		buf[sizeof(sstat_data_t)];
	sstat_data_t	*d;

	(void) memset(buf, 0, sizeof(buf));

	SCSICDB_RESET(cdb);
	cdb[0] = OP_VS_PLAYSTAT;
	cdb[8] = SZ_VS_PLAYSTAT;

	if (!pthru_send(devfd, cdb, 10, buf, SZ_VS_PLAYSTAT,
			OP_DATAIN, 5, TRUE))
		return -1;

	DBGDUMP("sony: Playback Status data bytes", buf, SZ_VS_PLAYSTAT);

	d = (sstat_data_t *)(void *) buf;

	if (query) {
		vol1 = di_untaper_vol(di_unscale_vol((int) d->vol0));
		vol2 = di_untaper_vol(di_unscale_vol((int) d->vol1));
		sony_route_left = (byte_t) d->sel0;
		sony_route_right = (byte_t) d->sel1;

		if (vol1 == vol2) {
			s->level_left = s->level_right = 100;
			vol = vol1;
		}
		else if (vol1 > vol2) {
			s->level_left = 100;
			s->level_right = (byte_t) ((vol2 * 100) / vol1);
			vol = vol1;
		}
		else {
			s->level_left = (byte_t) ((vol1 * 100) / vol2);
			s->level_right = 100;
			vol = vol2;
		}

		return (vol);
	}
	else {
		(void) memset(buf, 0, 10);

		d->vol0 = (byte_t) di_scale_vol(
			di_taper_vol(vol * (int) s->level_left / 100)
		);
		d->vol1 = (byte_t) di_scale_vol(
			di_taper_vol(vol * (int) s->level_right / 100)
		);
		d->sel0 = sony_route_left;
		d->sel1 = sony_route_right;

		DBGDUMP("sony: Playback Control data bytes",
			buf, SZ_VS_PLAYSTAT);

		SCSICDB_RESET(cdb);
		cdb[0] = OP_VS_PLAYCTL;
		cdb[8] = SZ_VS_PLAYSTAT;

		if (pthru_send(devfd, cdb, 10, buf, SZ_VS_PLAYSTAT,
			       OP_DATAOUT, 5, TRUE)) {
			/* Success */
			return (vol);
		}
		else if (d->vol0 != d->vol1) {
			/* Set the balance to the center
			 * and retry.
			 */
			d->vol0 = d->vol1 = di_scale_vol(di_taper_vol(vol));

			DBGDUMP("sony: Playback Control data bytes",
				buf, SZ_VS_PLAYSTAT);

			SCSICDB_RESET(cdb);
			cdb[0] = OP_VS_PLAYCTL;
			cdb[8] = SZ_VS_PLAYSTAT;

			if (pthru_send(devfd, cdb, 10, buf, SZ_VS_PLAYSTAT,
					OP_DATAOUT, 5, TRUE)) {
				/* Success: Warp balance control */
				s->level_left = s->level_right = 100;
				return (vol);
			}

			/* Still failed: just drop through */
		}
	}

	return -1;
}


/*
 * sony_route
 *	Configure channel routing via Sony Vendor-unique commands.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
sony_route(curstat_t *s)
{
	byte_t	val0,
		val1;

	val0 = sony_route_val(app_data.ch_route, 0);
	val1 = sony_route_val(app_data.ch_route, 1);

	if (val0 == sony_route_left && val1 == sony_route_right)
		/* No change: just return */
		return TRUE;

	sony_route_left = val0;
	sony_route_right = val1;

	/* Sony channel routing is done with the volume control */
	(void) sony_volume(s->level, s, FALSE);

	return TRUE;
}


/*
 * sony_init
 *	Initialize the vendor-unique support module
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
void
sony_init(void)
{
	/* Register vendor_unique module entry points */
	scsipt_vutbl[VENDOR_SONY].vendor = "Sony";
	scsipt_vutbl[VENDOR_SONY].playaudio = sony_playaudio;
	scsipt_vutbl[VENDOR_SONY].pause_resume = sony_pause_resume;
	scsipt_vutbl[VENDOR_SONY].start_stop = NULL;
	scsipt_vutbl[VENDOR_SONY].get_playstatus = sony_get_playstatus;
	scsipt_vutbl[VENDOR_SONY].volume = sony_volume;
	scsipt_vutbl[VENDOR_SONY].route = sony_route;
	scsipt_vutbl[VENDOR_SONY].mute = NULL;
	scsipt_vutbl[VENDOR_SONY].get_toc = sony_get_toc;
	scsipt_vutbl[VENDOR_SONY].eject = NULL;
	scsipt_vutbl[VENDOR_SONY].start = sony_start;
	scsipt_vutbl[VENDOR_SONY].halt = NULL;
}


/*
 * sony_start
 *	Start the vendor-unique support module.  In the case of the Sony,
 *	we want to set the drive's CD addressing to MSF mode.  Some Sony
 *	drives use the Mode Select command (page 0x8) to do this while
 *	others use a special vendor-unique "Set Address Format" command.
 *	We determine which to use by overloading the meaning of the
 *	app_data.msen_dbd configuration parameter.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
void
sony_start(void)
{
	byte_t			buf[SZ_VS_CDPARM];
	mode_sense_data_t	*ms_data;
	blk_desc_t		*bdesc;
	cdparm_pg_t		*parm_pg;

	if (app_data.msen_dbd) {
		/* Send "Set Address Format" command */
		SCSICDB_RESET(cdb);
		cdb[0] = OP_VS_SETADDRFMT;
		cdb[8] = 0x01;

		(void) pthru_send(devfd, cdb, 10, NULL, 0, OP_NODATA, 5, TRUE);
	}
	else {
		/* Do Mode Sense command, CD-ROM parameters page */
		if (!scsipt_modesense(devfd, buf, 0, PG_VS_CDPARM,
				      SZ_VS_CDPARM))
			return;		/* Error */

		ms_data = (mode_sense_data_t *)(void *) buf;
		bdesc = (blk_desc_t *)(void *) ms_data->data;
		parm_pg = (cdparm_pg_t *)(void *)
			  &ms_data->data[ms_data->bdescr_len];

		if (parm_pg->pg_code != PG_VS_CDPARM)
			return;		/* Error */

		ms_data->data_len = 0;
		if (ms_data->bdescr_len > 0)
			bdesc->num_blks = 0;

		/* Set the drive up for MSF address mode */
		parm_pg->lbamsf = 1;

		(void) scsipt_modesel(devfd, buf, PG_VS_CDPARM, SZ_VS_CDPARM);
	}
}


#endif	/* VENDOR_SONY */

