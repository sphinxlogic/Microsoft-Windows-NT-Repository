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
 *   Panasonic/Matsushita vendor-qunie support
 *
 *   The name "Panasonic" is a trademark of Matsushita Electric
 *   Corporation, and is used here for identification purposes only.
 */
#ifndef LINT
static char *_vu_pana_c_ident_ = "@(#)vu_pana.c	6.23 98/05/10";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#ifdef VENDOR_PANASONIC


#define MAX_NOTVAL	5			/* Max invalid status */

extern appdata_t	app_data;
extern di_client_t	*di_clinfo;
extern vu_tbl_t		scsipt_vutbl[];
extern byte_t		cdb[];
extern int		devfd;

STATIC byte_t		pana_route_left,	/* left ch routing control */
			pana_route_right;	/* Right ch routing control */


/*
 * pana_route_val
 *	Return the channel routing control value used in the
 *	Panasonic mode parameter page 0x2E (audio parameters).
 *
 * Args:
 *	route_mode - The channel routing mode value.
 *	channel - The channel number desired (0=left 1=right).
 *
 * Return:
 *	The routing control value.
 */
STATIC byte_t
pana_route_val(int route_mode, int channel)
{
	/* Panasonic: note that the port control bits are reversed
	 * compared to the SCSI-2 audio page 0xe.
	 */

	switch (channel) {
	case 0:
		switch (route_mode) {
		case 0:
			return 0x2;
		case 1:
			return 0x1;
		case 2:
			return 0x2;
		case 3:
			return 0x1;
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
			return 0x1;
		case 1:
			return 0x2;
		case 2:
			return 0x2;
		case 3:
			return 0x1;
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
 * pana_playaudio
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
pana_playaudio(
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
		cdb[0] = OP_VM_PLAYMSF;
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
 * pana_pause_resume
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
pana_pause_resume(bool_t resume)
{
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VM_PAUSE;
	cdb[8] = resume ? 0x01 : 0x00;

	return (pthru_send(devfd, cdb, 10, NULL, 0, OP_NODATA, 5, TRUE));
}


/*
 * pana_get_playstatus
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
pana_get_playstatus(curstat_t *s, byte_t *audio_status)
{
	int		trkno,
			idxno;
	subq_hdr_t	*h;
	subq_01_t	*p;
	byte_t		dbuf[SZ_VM_RDSUBQ];
	static int	notvalid_cnt = 0;
	static byte_t	prev_stat = AUDIO_NOTVALID;

	/*
	 * Send Panasonic Read Subchannel command
	 */

	(void) memset(dbuf, 0, sizeof(dbuf));

	SCSICDB_RESET(cdb);
	cdb[0] = OP_VM_RDSUBQ;
	cdb[1] = 0x02;
	cdb[2] = 0x40;
	cdb[3] = 0x01;
	cdb[6] = 0x00;
	cdb[7] = (SZ_VM_RDSUBQ & 0xff00) >> 8;
	cdb[8] = SZ_VM_RDSUBQ & 0x00ff;

	if (!pthru_send(devfd, cdb, 10, dbuf, SZ_VM_RDSUBQ, OP_DATAIN, 5, TRUE))
		return FALSE;

	DBGDUMP("pana: Read Subchannel data bytes", dbuf, SZ_VM_RDSUBQ);

	h = (subq_hdr_t *)(void *) dbuf;
	p = (subq_01_t *)(void *) (dbuf + sizeof(subq_hdr_t));

	/* Hack: The Panasonic drive seems to spuriously return
	 * AUDIO_NOTVALID status during playback for no apparent
	 * reason.  Work around this.
	 */
	if (h->audio_status == AUDIO_NOTVALID && notvalid_cnt < MAX_NOTVAL) {
		/* Allow up to MAX_NOTVAL consecutive occurances
		 * of this anomaly.
		 */
		*audio_status = prev_stat;
		if (prev_stat != AUDIO_NOTVALID)
			notvalid_cnt++;
	}
	else {
		prev_stat = h->audio_status;
		*audio_status = h->audio_status;
		notvalid_cnt = 0;
	}

	trkno = (word32_t) p->trkno;
	if (s->cur_trk != trkno) {
		s->cur_trk = trkno;
		DPY_TRACK(s);
	}

	idxno = (word32_t) p->idxno;
	if (s->cur_idx != idxno) {
		s->cur_idx = idxno;
		s->sav_iaddr = s->cur_tot_addr;
		DPY_INDEX(s);
	}

	s->cur_tot_min = p->abs_addr.msf.min;
	s->cur_tot_sec = p->abs_addr.msf.sec;
	s->cur_tot_frame = p->abs_addr.msf.frame;
	util_msftoblk(
		s->cur_tot_min, s->cur_tot_sec, s->cur_tot_frame,
		&s->cur_tot_addr, MSF_OFFSET
	);

	s->cur_trk_min = p->rel_addr.msf.min;
	s->cur_trk_sec = p->rel_addr.msf.sec;
	s->cur_trk_frame = p->rel_addr.msf.frame;
	util_msftoblk(
		s->cur_trk_min, s->cur_trk_sec, s->cur_trk_frame,
		&s->cur_trk_addr, 0
	);

	return TRUE;
}


/*
 * pana_get_toc
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
pana_get_toc(curstat_t *s)
{
	int		i,
			j,
			xfer_len;
	toc_hdr_t	*h;
	toc_trk_descr_t	*p;
	byte_t		*cp,
			buf[SZ_VM_RDTOC];

	(void) memset(buf, 0, sizeof(buf));

	/* Read TOC header to find the number of tracks */
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VM_RDTOC;
	cdb[1] = 0x02;
	cdb[8] = SZ_VM_TOCHDR;

	if (!pthru_send(devfd, cdb, 10, buf, SZ_VM_TOCHDR, OP_DATAIN, 5, TRUE))
		return FALSE;

	h = (toc_hdr_t *)(void *) buf;

	s->first_trk = (byte_t) h->first_trk;
	s->last_trk = (byte_t) h->last_trk;

	xfer_len = SZ_VM_TOCHDR +
		   ((int) (h->last_trk - h->first_trk + 2) * SZ_VM_TOCENT);

	if (xfer_len > SZ_VM_RDTOC)
		xfer_len = SZ_VM_RDTOC;

	SCSICDB_RESET(cdb);
	cdb[0] = OP_VM_RDTOC;
	cdb[1] = 0x02;
	cdb[7] = (xfer_len & 0xff00) >> 8;
	cdb[8] = xfer_len & 0x00ff;

	if (!pthru_send(devfd, cdb, 10, buf, xfer_len, OP_DATAIN, 5, TRUE))
		return FALSE;

	DBGDUMP("pana: Read TOC data bytes", buf, xfer_len);

	/* Get the starting position of each track */
	cp = buf + sizeof(toc_hdr_t);
	for (i = 0, j = (int) s->first_trk; j <= (int) s->last_trk; i++, j++) {
		p = (toc_trk_descr_t *)(void *) (cp + (i * SZ_VM_TOCENT));
		s->trkinfo[i].trkno = (byte_t) p->trkno;
		s->trkinfo[i].min = (byte_t) p->abs_addr.msf.min;
		s->trkinfo[i].sec = (byte_t) p->abs_addr.msf.sec;
		s->trkinfo[i].frame = (byte_t) p->abs_addr.msf.frame;
		util_msftoblk(
			s->trkinfo[i].min,
			s->trkinfo[i].sec,
			s->trkinfo[i].frame,
			&s->trkinfo[i].addr,
			MSF_OFFSET
		);
		s->trkinfo[i].type = (p->trktype == 0) ? TYP_AUDIO : TYP_DATA;
	}
	s->tot_trks = (byte_t) i;

	/* Get the lead-out track position */
	p = (toc_trk_descr_t *)(void *) (cp + (i * SZ_VM_TOCENT));
	s->trkinfo[i].trkno = LEAD_OUT_TRACK;
	s->tot_min = s->trkinfo[i].min = (byte_t) p->abs_addr.msf.min;
	s->tot_sec = s->trkinfo[i].sec = (byte_t) p->abs_addr.msf.sec;
	s->tot_frame = s->trkinfo[i].frame = (byte_t) p->abs_addr.msf.frame;
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
 * pana_volume
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
pana_volume(int vol, curstat_t *s, bool_t query)
{
	int			vol1,
				vol2;
	mode_sense_data_t	*ms_data;
	blk_desc_t		*bdesc;
	audio_pg_t		*audiopg;
	byte_t			buf[SZ_MSENSE];

	if (!app_data.mselvol_supp)
		return -1;

	(void) memset(buf, 0, SZ_MSENSE);

	if (!scsipt_modesense(devfd, buf, 0, PG_VM_AUDCTL, SZ_VM_AUDCTL))
		return -1;

	ms_data = (mode_sense_data_t *)(void *) buf;
	bdesc = (blk_desc_t *)(void *) ms_data->data;
	audiopg = (audio_pg_t *)(void *) &ms_data->data[ms_data->bdescr_len];

	if (audiopg->pg_code == PG_VM_AUDCTL) {
		if (query) {
			vol1 = di_untaper_vol(
				di_unscale_vol((int) audiopg->p0_vol)
			);
			vol2 = di_untaper_vol(
				di_unscale_vol((int) audiopg->p1_vol)
			);
			pana_route_left = (byte_t) audiopg->p0_ch_ctrl;
			pana_route_right = (byte_t) audiopg->p1_ch_ctrl;
	
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
			ms_data->data_len = 0;
			if (ms_data->bdescr_len > 0)
				bdesc->num_blks = 0;

			audiopg->p0_vol = di_scale_vol(
				di_taper_vol(vol * (int) s->level_left / 100)
			);
			audiopg->p1_vol = di_scale_vol(
				di_taper_vol(vol * (int) s->level_right / 100)
			);

			audiopg->p0_ch_ctrl = pana_route_left;
			audiopg->p1_ch_ctrl = pana_route_right;

			audiopg->sotc = 0;
			audiopg->immed = 0;	/* Panasonic: reserved */

			if (scsipt_modesel(devfd, buf, PG_VM_AUDCTL,
					   SZ_VM_AUDCTL)) {
				/* Success */
				return (vol);
			}
			else if (audiopg->p0_vol != audiopg->p1_vol) {
				/* Set the balance to the center
				 * and retry.
				 */
				audiopg->p0_vol = audiopg->p1_vol =
					di_scale_vol(di_taper_vol(vol));

				if (scsipt_modesel(devfd, buf, PG_VM_AUDCTL,
						   SZ_VM_AUDCTL)) {
					/* Success: Warp balance control */
					s->level_left = s->level_right = 100;
					SET_BAL_SLIDER(0);

					return (vol);
				}

				/* Still failed: just drop through */
			}
		}
	}

	return -1;
}


/*
 * pana_route
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
pana_route(curstat_t *s)
{
	byte_t	val0,
		val1;

	if (!app_data.chroute_supp)
		return FALSE;

	val0 = pana_route_val(app_data.ch_route, 0);
	val1 = pana_route_val(app_data.ch_route, 1);

	if (val0 == pana_route_left && val1 == pana_route_right)
		/* No change: just return */
		return TRUE;

	pana_route_left = val0;
	pana_route_right = val1;

	/* Panasonic channel routing is done with the volume control */
	(void) pana_volume(s->level, s, FALSE);

	return TRUE;
}


/*
 * pana_init
 *	Initialize the vendor-unique support module
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
void
pana_init(void)
{
	/* Register vendor_unique module entry points */
	scsipt_vutbl[VENDOR_PANASONIC].vendor = "Panasonic";
	scsipt_vutbl[VENDOR_PANASONIC].playaudio = pana_playaudio;
	scsipt_vutbl[VENDOR_PANASONIC].pause_resume = pana_pause_resume;
	scsipt_vutbl[VENDOR_PANASONIC].start_stop = NULL;
	scsipt_vutbl[VENDOR_PANASONIC].get_playstatus = pana_get_playstatus;
	scsipt_vutbl[VENDOR_PANASONIC].volume = pana_volume;
	scsipt_vutbl[VENDOR_PANASONIC].route = pana_route;
	scsipt_vutbl[VENDOR_PANASONIC].mute = NULL;
	scsipt_vutbl[VENDOR_PANASONIC].get_toc = pana_get_toc;
	scsipt_vutbl[VENDOR_PANASONIC].eject = NULL;
	scsipt_vutbl[VENDOR_PANASONIC].start = NULL;
	scsipt_vutbl[VENDOR_PANASONIC].halt = NULL;
}


#endif	/* VENDOR_PANASONIC */

