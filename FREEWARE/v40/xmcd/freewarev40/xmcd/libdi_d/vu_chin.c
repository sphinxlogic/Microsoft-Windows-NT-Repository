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
 *   Chinon CDx-431 and CDx-435 support
 *
 *   The name "Chinon" is a trademark of Chinon Industries, and is
 *   used here for identification purposes only.
 */
#ifndef LINT
static char *_vu_chin_c_ident_ = "@(#)vu_chin.c	6.24 98/05/10";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#ifdef VENDOR_CHINON

extern appdata_t	app_data;
extern di_client_t	*di_clinfo;
extern vu_tbl_t		scsipt_vutbl[];
extern byte_t		cdb[];
extern int		devfd;

STATIC bool_t		chin_bcd_hack;


/*
 * chin_playaudio
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
chin_playaudio(
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

	/* Chinon only supports the Play Audio MSF command on the
	 * CDx-43x, which is identical to the SCSI-2 command of
	 * the same name and opcode.
	 */

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

	if (!ret && (addr_fmt & ADDR_MSF))
		ret = scsipt_playmsf(devfd, start_msf, end_msf);

	return (ret);
}


/*
 * chin_start_stop
 *      Start/stop function.
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
chin_start_stop(bool_t start, bool_t loej)
{
	bool_t	ret;

	if (start)
		/* Chinon CDx-43x does not support a start command so
		 * just quietly return success.
		 */
		return TRUE;

	/* Stop the playback */
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VC_STOP;

	ret = pthru_send(devfd, cdb, 10, NULL, 0, OP_NODATA, 10, TRUE);

	/* Eject the caddy if necessary */
	if (ret && loej) {
		SCSICDB_RESET(cdb);
		cdb[0] = OP_VC_EJECT;

		ret = pthru_send(devfd, cdb, 10, NULL, 0, OP_NODATA, 20, TRUE);
	}

	return (ret);
}


/*
 * chin_get_playstatus
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
chin_get_playstatus(curstat_t *s, byte_t *audio_status)
{
	int		trkno,
			idxno;
	byte_t		buf[SZ_RDSUBQ],
			*cp;
	subq_hdr_t	*h;
	subq_01_t	*p;


	/* Chinon CDx-43x supports the Read Subchannel command which
	 * is identical to the SCSI-2 command of the same name and
	 * opcode, but the audio status codes are different.
	 */

	(void) memset(buf, 0, sizeof(buf));

	if (!scsipt_rdsubq(devfd, buf, SUB_ALL, 1, 0, TRUE)) {
		if (!di_check_disc(s))
			/* Someone ejected the disc manually */
			return FALSE;

		/* The read subchannel command failed for some
		 * unknown reason.  Just return success and
		 * hope the next poll succeeds.  We don't want
		 * to return FALSE here because that would stop
		 * the poll.
		 */
		return TRUE;
	}

	h = (subq_hdr_t *)(void *) buf;

	/* Translate Chinon audio status to SCSI-2 audio status */
	switch (h->audio_status) {
	case CAUD_PLAYING:
		*audio_status = AUDIO_PLAYING;
		break;
	case CAUD_PAUSED:
		*audio_status = AUDIO_PAUSED;
		break;
	case CAUD_INVALID:
	default:
		*audio_status = AUDIO_NOTVALID;
		break;
	}

	/* Check the subchannel data */
	cp = (byte_t *) h + sizeof(subq_hdr_t);
	switch (*cp) {
	case SUB_ALL:
	case SUB_CURPOS:
		p = (subq_01_t *)(void *) cp;

		if (chin_bcd_hack) {
			/* Hack: BUGLY CD-ROM firmware */
			trkno = util_bcdtol(p->trkno);
			idxno = util_bcdtol(p->idxno);
		}
		else {
			trkno = p->trkno;
			idxno = p->idxno;
		}

		if (trkno != s->cur_trk) {
			s->cur_trk = trkno;
			DPY_TRACK(s);
		}

		if (idxno != s->cur_idx) {
			s->cur_idx = idxno;
			s->sav_iaddr = s->cur_tot_addr;
			DPY_INDEX(s);
		}

		s->cur_tot_min = p->abs_addr.msf.min;
		s->cur_tot_sec = p->abs_addr.msf.sec;
		s->cur_tot_frame = p->abs_addr.msf.frame;
		util_msftoblk(
			s->cur_tot_min,
			s->cur_tot_sec,
			s->cur_tot_frame,
			&s->cur_tot_addr,
			MSF_OFFSET
		);

		s->cur_trk_min = p->rel_addr.msf.min;
		s->cur_trk_sec = p->rel_addr.msf.sec;
		s->cur_trk_frame = p->rel_addr.msf.frame;
		util_msftoblk(
			s->cur_trk_min,
			s->cur_trk_sec,
			s->cur_trk_frame,
			&s->cur_trk_addr,
			0
		);

		break;
	default:
		/* Something is wrong with the data */
		break;
	}

	return TRUE;
}


/*
 * chin_get_toc
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
chin_get_toc(curstat_t *s)
{
	int			i,
				ntrks;
	byte_t			buf[SZ_RDTOC],
				*cp,
				*toc_end;
	toc_hdr_t		*h;
	toc_trk_descr_t		*p;


	/* Chinon CDx-43x supports the Read TOC command which is
	 * identical to the SCSI-2 command of the same name and opcode.
	 */

	(void) memset(buf, 0, sizeof(buf));

	if (!scsipt_rdtoc(devfd, buf, TRUE, 0))
		return FALSE;

	/* Fill curstat structure with TOC data */
	h = (toc_hdr_t *)(void *) buf;
	toc_end = (byte_t *) h + util_bswap16(h->data_len) + 2;

	s->first_trk = h->first_trk;
	s->last_trk = h->last_trk;

	ntrks = (int) (h->last_trk - h->first_trk) + 1;

	/* Hack: workaround CD-ROM firmware bug
	 * Some Chinon drives return track numbers in BCD
	 * rather than binary.
	 */
	cp = (byte_t *) h + sizeof(toc_hdr_t);

	for (i = 0; i <= ntrks; i++) {
		int	trk0,
			trk1;

		p = (toc_trk_descr_t *)(void *) cp;

		if (p->trkno == LEAD_OUT_TRACK && i != ntrks) {
			trk0 = util_bcdtol(h->first_trk);
			trk1 = util_bcdtol(h->last_trk);

			if (i == (trk1 - trk0 + 1)) {
				/* BUGLY firmware detected! */
				chin_bcd_hack = TRUE;
				s->first_trk = (byte_t) trk0;
				s->last_trk = (byte_t) trk1;
				break;
			}
		}
	}

	/*
	 * Fill in TOC data
	 */
	cp = (byte_t *) h + sizeof(toc_hdr_t);

	for (i = 0; cp < toc_end && i < MAXTRACK; i++) {
		p = (toc_trk_descr_t *)(void *) cp;

		if (chin_bcd_hack)
			/* Hack: BUGLY CD-ROM firmware */
			s->trkinfo[i].trkno = util_bcdtol(p->trkno);
		else
			s->trkinfo[i].trkno = p->trkno;

		s->trkinfo[i].type = (p->trktype == 0) ?
			TYP_AUDIO : TYP_DATA;
		s->trkinfo[i].min = p->abs_addr.msf.min;
		s->trkinfo[i].sec = p->abs_addr.msf.sec;
		s->trkinfo[i].frame = p->abs_addr.msf.frame;
		util_msftoblk(
			s->trkinfo[i].min,
			s->trkinfo[i].sec,
			s->trkinfo[i].frame,
			&s->trkinfo[i].addr,
			MSF_OFFSET
		);

		if (p->trkno == LEAD_OUT_TRACK ||
		    s->trkinfo[i-1].trkno == s->last_trk ||
		    i == (MAXTRACK - 1)) {
			s->tot_min = s->trkinfo[i].min;
			s->tot_sec = s->trkinfo[i].sec;
			s->tot_frame = s->trkinfo[i].frame;
			s->tot_trks = (byte_t) i;
			s->tot_addr = s->trkinfo[i].addr;

			break;
		}

		cp += sizeof(toc_trk_descr_t);
	}

	return TRUE;
}


/*
 * chin_eject
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
chin_eject(void)
{
	SCSICDB_RESET(cdb);
	cdb[0] = OP_VC_EJECT;

	return(pthru_send(devfd, cdb, 10, NULL, 0, OP_NODATA, 20, TRUE));
}


/*
 * chin_init
 *	Initialize the vendor-unique support module
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
void
chin_init(void)
{
	/* Register vendor_unique module entry points */
	scsipt_vutbl[VENDOR_CHINON].vendor = "Chinon";
	scsipt_vutbl[VENDOR_CHINON].playaudio = chin_playaudio;
	scsipt_vutbl[VENDOR_CHINON].pause_resume = NULL;
	scsipt_vutbl[VENDOR_CHINON].start_stop = chin_start_stop;
	scsipt_vutbl[VENDOR_CHINON].get_playstatus = chin_get_playstatus;
	scsipt_vutbl[VENDOR_CHINON].volume = NULL;
	scsipt_vutbl[VENDOR_CHINON].route = NULL;
	scsipt_vutbl[VENDOR_CHINON].mute = NULL;
	scsipt_vutbl[VENDOR_CHINON].get_toc = chin_get_toc;
	scsipt_vutbl[VENDOR_CHINON].eject = chin_eject;
	scsipt_vutbl[VENDOR_CHINON].start = NULL;
	scsipt_vutbl[VENDOR_CHINON].halt = NULL;
}


#endif	/* VENDOR_CHINON */

