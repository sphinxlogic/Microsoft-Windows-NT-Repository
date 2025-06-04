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
static char *_scsipt_c_ident_ = "@(#)scsipt.c	6.129 98/10/13";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#ifdef DI_SCSIPT

extern appdata_t	app_data;
extern FILE		*errfp;
extern di_client_t	*di_clinfo;
extern char		**di_devlist;

STATIC bool_t	scsipt_do_start_stop(int, bool_t, bool_t, bool_t),
		scsipt_do_pause_resume(int, bool_t),
		scsipt_run_ab(curstat_t *),
		scsipt_run_sample(curstat_t *),
		scsipt_run_prog(curstat_t *),
		scsipt_run_repeat(curstat_t *),
		scsipt_disc_ready(curstat_t *),
		scsipt_chg_ready(curstat_t *),
		scsipt_chg_start(curstat_t *);
STATIC void	scsipt_stat_poll(curstat_t *),
		scsipt_insert_poll(curstat_t *);

/* Not a CD-ROM error */
bool_t		scsipt_notrom_error;

/* VU module entry jump table */
vu_tbl_t	scsipt_vutbl[MAX_VENDORS];

/* SCSI command CDB */
byte_t		cdb[MAX_CMDLEN];

/* Device file descriptors */
int		devfd = -1,			/* CD-ROM device */
		chgfd = -1;			/* Medium changer device */


STATIC int	scsipt_stat_interval,		/* Status poll interval */
		scsipt_ins_interval;		/* Insert poll interval */
STATIC long	scsipt_stat_id,			/* Play status poll timer id */
		scsipt_insert_id,		/* Disc insert poll timer id */
		scsipt_search_id;		/* FF/REW timer id */
STATIC byte_t	scsipt_next_sam;		/* Next SAMPLE track */
STATIC bool_t	scsipt_not_open = TRUE,		/* Device not opened yet */
		scsipt_stat_polling,		/* Polling play status */
		scsipt_insert_polling,		/* Polling disc insert */
		scsipt_new_progshuf,		/* New program/shuffle seq */
		scsipt_start_search,		/* Start FF/REW play segment */
		scsipt_idx_pause,		/* Prev/next index pausing */
		scsipt_fake_stop,		/* Force a completion status */
		scsipt_playing,			/* Currently playing */
		scsipt_bcd_hack,		/* Track numbers in BCD hack */
		scsipt_mult_autoplay,		/* Auto-play after disc chg */
		scsipt_mult_pause,		/* Pause after disc chg */
		scsipt_override_ap;		/* Override auto-play */
STATIC word32_t	scsipt_ab_start_addr,		/* A->B mode start block */
		scsipt_ab_end_addr,		/* A->B mode end block */
		scsipt_sav_end_addr;		/* Err recov saved end addr */
STATIC msf_t	scsipt_ab_start_msf,		/* A->B mode start MSF */
		scsipt_ab_end_msf,		/* A->B mode end MSF */
		scsipt_sav_end_msf;		/* Err recov saved end MSF */
STATIC byte_t	scsipt_dev_scsiver,		/* Device SCSI version */
		scsipt_sav_end_fmt,		/* Err recov saved end fmt */
		scsipt_route_left,		/* Left channel routing */
		scsipt_route_right;		/* Right channel routing */
STATIC mcparm_t	scsipt_mcparm;			/* Medium changer parameters */

/* VU module init jump table */
STATIC vuinit_tbl_t	vuinit[] = {
	{ NULL },
	{ chin_init },
	{ hita_init },
	{ nec_init },
	{ pion_init },
	{ sony_init },
	{ tosh_init },
	{ pana_init },
};



/***********************
 *  internal routines  *
 ***********************/


/*
 * scsipt_rdsubq
 *	Send SCSI-2 Read Subchannel command to the device
 *
 * Args:
 *	fd - Device file descriptor
 *	buf - Pointer to the return data buffer
 *	fmt - Subchannel data format code
 *		SUB_ALL		Subchannel-Q data
 *		SUB_CURPOS	CD-ROM Current position data
 *		SUB_CATNO	Media catalog number data
 *		SUB_ISRC	Track Intl Standard Recording Code
 *	subq - Whether the CD-ROM should return subchannel-Q data
 *	trkno - Track number from which the ISRC data is read
 *	msf - Whether to use MSF or logical block address format
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
scsipt_rdsubq(
	int	fd,
	byte_t	*buf,
	byte_t	fmt,
	bool_t	subq,
	int	trkno,
	bool_t	msf
)
{
	int	xfer_len;
	bool_t	ret;

	switch (fmt) {
	case SUB_ALL:
		xfer_len = 48;
		break;
	case SUB_CURPOS:
		xfer_len = 16;
		break;
	case SUB_CATNO:
	case SUB_ISRC:
		xfer_len = 24;
		break;
	default:
		return FALSE;
	}

	if (xfer_len > SZ_RDSUBQ)
		xfer_len = SZ_RDSUBQ;

	SCSICDB_RESET(cdb);
	cdb[0] = OP_M_RDSUBQ;
	cdb[1] = msf ? 0x02 : 0x00;
	cdb[2] = subq ? 0x40 : 0x00;
	cdb[3] = fmt;
	cdb[6] = (byte_t) trkno;
	cdb[7] = (xfer_len & 0xff00) >> 8;
	cdb[8] = xfer_len & 0x00ff;

	if ((ret = pthru_send(fd, cdb, 10, buf, xfer_len,
			      OP_DATAIN, 5, TRUE)) == TRUE) {
		DBGDUMP("Read Subchannel data bytes", buf, xfer_len);
	}

	return (ret);
}


/*
 * scsipt_modesense
 *	Send SCSI Mode Sense command to the device
 *
 * Args:
 *	fd - Device file descriptor
 *	buf - Pointer to the return data buffer
 *	pg_ctrl - Defines the type of parameters to be returned:
 *		0: Current values
 *		1: Changeable values
 *		2: Default values
 *	pg_code - Specifies which page or pages to return:
 *		PG_ERRECOV: Error recovery params page
 *		PG_DISCONN: Disconnect/reconnect params page
 *		PG_CDROMCTL: CD-ROM params page
 *		PG_AUDIOCTL: Audio control params page
 *		PG_ALL: All pages
 *	xfer_len - Data transfer length header + page length (excluding
 *		block descriptor)
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
scsipt_modesense(
	int	fd,
	byte_t	*buf,
	byte_t	pg_ctrl,
	byte_t	pg_code,
	int	xfer_len
)
{
	bool_t	ret;

	if (!app_data.msen_dbd)
		xfer_len += 8;

	SCSICDB_RESET(cdb);
	cdb[0] = OP_S_MSENSE;
	cdb[1] = (byte_t) (app_data.msen_dbd ? 0x08 : 0x00);
	cdb[2] = (byte_t) (((pg_ctrl & 0x03) << 6) | (pg_code & 0x3f));
	cdb[4] = (byte_t) xfer_len;

	if ((ret = pthru_send(fd, cdb, 6, buf, xfer_len,
			      OP_DATAIN, 5, TRUE)) == TRUE) {
		DBGDUMP("Mode Sense data bytes", buf, xfer_len);
	}

	return (ret);
}


/*
 * scsipt_modesel
 *	Send SCSI Mode Select command to the device
 *
 * Args:
 *	fd - Device file descriptor
 *	buf - Pointer to the data buffer
 *	pg_code - Specifies which page or pages to return:
 *		PG_ERRECOV: Error recovery params page
 *		PG_DISCONN: Disconnect/reconnect params page
 *		PG_CDROMCTL: CD-ROM params page
 *		PG_AUDIOCTL: Audio control params page
 *		PG_ALL: All pages
 *	xfer_len - Data transfer length header + page length (excluding
 *		block descriptor)
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
/*ARGSUSED*/
bool_t
scsipt_modesel(int fd, byte_t *buf, byte_t pg_code, int xfer_len)
{
	if (!app_data.msen_dbd)
		xfer_len += 8;

	DBGDUMP("Mode Select data bytes", buf, xfer_len);

	SCSICDB_RESET(cdb);
	cdb[0] = OP_S_MSELECT;
	cdb[1] = 0x10;
	cdb[4] = (byte_t) xfer_len;

	return (pthru_send(fd, cdb, 6, buf, xfer_len, OP_DATAOUT, 5, TRUE));
}


/*
 * scsipt_inquiry
 *	Send SCSI Inquiry command to the device
 *
 * Args:
 *	fd - Device file descriptor
 *	buf - Pointer to the return data buffer
 *	len - Maximum number of inquiry data bytes to transfer
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
scsipt_inquiry(int fd, byte_t *buf, int len)
{
	bool_t	ret;

	SCSICDB_RESET(cdb);
	cdb[0] = OP_S_INQUIR;
	cdb[4] = (byte_t) len;

	if ((ret = pthru_send(fd, cdb, 6, buf, len, OP_DATAIN,
			      5, TRUE)) == TRUE) {
		DBGDUMP("Inquiry data bytes", buf, len);
	}

	return (ret);
}


/*
 * scsipt_rdtoc
 *	Send SCSI-2 Read TOC command to the device
 *
 * Args:
 *	fd - Device file descriptor
 *	buf - Pointer to the return data buffer
 *	msf - Whether to use MSF or logical block address data format
 *	start - Starting track number for which the TOC data is returned
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
scsipt_rdtoc(int fd, byte_t *buf, bool_t msf, int start)
{
	int		xfer_len;
	toc_hdr_t	*thdr;
	bool_t		ret;

	/* Read the TOC header first */
	SCSICDB_RESET(cdb);
	cdb[0] = OP_M_RDTOC;
	cdb[1] = msf ? 0x02 : 0x00;
	cdb[6] = (byte_t) start;
	cdb[7] = 0;
	cdb[8] = SZ_TOCHDR;

	if (!pthru_send(fd, cdb, 10, buf, SZ_TOCHDR, OP_DATAIN, 5, TRUE))
		return FALSE;

	thdr = (toc_hdr_t *)(void *) buf;

	if (start == 0)
		start = (int) thdr->first_trk;

	xfer_len = SZ_TOCHDR + (((int) thdr->last_trk - start + 2) * SZ_TOCENT);

	if (xfer_len > SZ_RDTOC)
		xfer_len = SZ_RDTOC;

	/* Read the appropriate number of bytes of the entire TOC */
	SCSICDB_RESET(cdb);
	cdb[0] = OP_M_RDTOC;
	cdb[1] = msf ? 0x02 : 0x00;
	cdb[6] = (byte_t) start;
	cdb[7] = (xfer_len & 0xff00) >> 8;
	cdb[8] = xfer_len & 0x00ff;

	if ((ret = pthru_send(fd, cdb, 10, buf, xfer_len,
			      OP_DATAIN, 5, TRUE)) == TRUE) {
		DBGDUMP("Read TOC data bytes", buf, xfer_len);
	}

#ifdef __VMS
	/* VMS hack */
	if (ret) {
		int	i;

		ret = FALSE;

		/* Make sure that the return buffer is not all zeros */
		for (i = 0; i < xfer_len; i++) {
			if (buf[i] != 0) {
				ret = TRUE;
				break;
			}
		}
	}
#endif

	return (ret);
}


/*
 * scsipt_tst_unit_rdy
 *	Send SCSI Test Unit Ready command to the device
 *
 * Args:
 *	fd - Device file descriptor
 *	prnerr - Whether to display error messages if the command fails.
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure (drive not ready)
 */
STATIC bool_t
scsipt_tst_unit_rdy(int fd, bool_t prnerr)
{
	SCSICDB_RESET(cdb);
	cdb[0] = OP_S_TEST;

	return (pthru_send(fd, cdb, 6, NULL, 0, OP_NODATA, 20, prnerr));
}


/*
 * scsipt_playmsf
 *	Send SCSI-2 Play Audio MSF command to the device
 *
 * Args:
 *	fd - Device file descriptor
 *	start - Pointer to the starting position MSF data
 *	end - Pointer to the ending position MSF data
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
scsipt_playmsf(int fd, msf_t *start, msf_t *end)
{
	if (!app_data.playmsf_supp)
		return FALSE;

	SCSICDB_RESET(cdb);
	cdb[0] = OP_M_PLAYMSF;
	cdb[3] = start->min;
	cdb[4] = start->sec;
	cdb[5] = start->frame;
	cdb[6] = end->min;
	cdb[7] = end->sec;
	cdb[8] = end->frame;

	return (pthru_send(fd, cdb, 10, NULL, 0, OP_NODATA, 20, TRUE));
}


/*
 * scsipt_play10
 *	Send SCSI-2 Play Audio (10) command to the device
 *
 * Args:
 *	fd - Device file descriptor
 *	start - The starting logical block address
 *	len - The number of logical blocks to play (max=0xffff)
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
scsipt_play10(int fd, word32_t start, word32_t len)
{
	if (!app_data.play10_supp || len > 0xffff)
		return FALSE;

	SCSICDB_RESET(cdb);
	cdb[0] = OP_M_PLAY;
	cdb[2] = (start & 0xff000000) >> 24;
	cdb[3] = (start & 0x00ff0000) >> 16;
	cdb[4] = (start & 0x0000ff00) >> 8;
	cdb[5] = (start & 0x000000ff);
	cdb[7] = (len & 0xff00) >> 8;
	cdb[8] = (len & 0x00ff);

	return (pthru_send(fd, cdb, 10, NULL, 0, OP_NODATA, 20, TRUE));
}


/*
 * scsipt_play12
 *	Send SCSI-2 Play Audio (12) command to the device
 *
 * Args:
 *	fd - Device file descriptor
 *	start - The starting logical block address
 *	len - The number of logical blocks to play
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
scsipt_play12(int fd, word32_t start, word32_t len)
{
	if (!app_data.play12_supp)
		return FALSE;

	SCSICDB_RESET(cdb);
	cdb[0] = OP_L_PLAY;
	cdb[2] = (start & 0xff000000) >> 24;
	cdb[3] = (start & 0x00ff0000) >> 16;
	cdb[4] = (start & 0x0000ff00) >> 8;
	cdb[5] = (start & 0x000000ff);
	cdb[6] = (len & 0xff000000) >> 24;
	cdb[7] = (len & 0x00ff0000) >> 16;
	cdb[8] = (len & 0x0000ff00) >> 8;
	cdb[9] = (len & 0x000000ff);

	return (pthru_send(fd, cdb, 12, NULL, 0, OP_NODATA, 20, TRUE));
}


/*
 * scsipt_play_trkidx
 *	Send SCSI-2 Play Audio Track/Index command to the device
 *
 * Args:
 *	fd - Device file descriptor
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
scsipt_play_trkidx(
	int	fd,
	int	start_trk,
	int	start_idx,
	int	end_trk,
	int	end_idx
)
{
	if (!app_data.playti_supp)
		return FALSE;

	if (scsipt_bcd_hack) {
		/* Hack: BUGLY CD-ROM firmware */
		start_trk = util_ltobcd(start_trk);
		start_idx = util_ltobcd(start_idx);
		end_trk = util_ltobcd(end_trk);
		end_idx = util_ltobcd(end_idx);
	}

	SCSICDB_RESET(cdb);
	cdb[0] = OP_M_PLAYTI;
	cdb[4] = (byte_t) start_trk;
	cdb[5] = (byte_t) start_idx;
	cdb[7] = (byte_t) end_trk;
	cdb[8] = (byte_t) end_idx;

	return (pthru_send(fd, cdb, 10, NULL, 0, OP_NODATA, 20, TRUE));
}


/*
 * scsipt_prev_allow
 *	Send SCSI Prevent/Allow Medium Removal command to the device
 *
 * Args:
 *	fd - Device file descriptor
 *	prevent - Whether to prevent or allow medium removal
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
scsipt_prev_allow(int fd, bool_t prevent)
{
	if (!app_data.caddylock_supp)
		return FALSE;

	SCSICDB_RESET(cdb);
	cdb[0] = OP_S_PREVENT;
	cdb[4] = prevent ? 0x01 : 0x00;

	return (pthru_send(fd, cdb, 6, NULL, 0, OP_NODATA, 5, TRUE));
}


/*
 * scsipt_start_stop
 *	Send SCSI Start/Stop Unit command to the device
 *
 * Args:
 *	fd - Device file descriptor
 *	start - Whether to start unit or stop unit
 *	loej - Whether caddy load/eject operation should be performed
 *	prnerr - Whether to print error messages if the command fails
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
scsipt_start_stop(int fd, bool_t start, bool_t loej, bool_t prnerr)
{
	byte_t	ctl;
	bool_t	ret;

	if (start)
		ctl = 0x01;
	else
		ctl = 0x00;

	if (loej)
		ctl |= 0x02;

	SCSICDB_RESET(cdb);
	cdb[0] = OP_S_START;
	if (loej)
		cdb[1] = 0x1;	/* Set immediate bit */
	cdb[4] = ctl;

	ret = pthru_send(fd, cdb, 6, NULL, 0, OP_NODATA, 20, prnerr);

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
 * scsipt_pause_resume
 *	Send SCSI-2 Pause/Resume command to the device
 *
 * Args:
 *	fd - Device file descriptor
 *	resume - Whether to resume or pause
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
scsipt_pause_resume(int fd, bool_t resume)
{
	if (!app_data.pause_supp)
		return FALSE;

	SCSICDB_RESET(cdb);
	cdb[0] = OP_M_PAUSE;
	cdb[8] = resume ? 0x01 : 0x00;

	return (pthru_send(fd, cdb, 10, NULL, 0, OP_NODATA, 20, TRUE));
}


/*
 * scsipt_move_medium
 *	Move a unit of media from a source element to a destination element
 *
 * Args:
 *	fd - Device file descriptor
 *	te_addr - Transport element address
 *	src_addr - Source element address
 *	dst_addr - Destination element address
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
scsipt_move_medium(
	int		fd,
	word16_t	te_addr,
	word16_t	src_addr,
	word16_t	dst_addr
)
{
	SCSICDB_RESET(cdb);
	cdb[0] = OP_L_MOVEMED;
	cdb[2] = ((int) (te_addr & 0xff00) >> 8);
	cdb[3] = (te_addr & 0x00ff);
	cdb[4] = ((int) (src_addr & 0xff00) >> 8);
	cdb[5] = (src_addr & 0x00ff);
	cdb[6] = ((int) (dst_addr & 0xff00) >> 8);
	cdb[7] = (dst_addr & 0x00ff);

	return (
		pthru_send(fd, cdb, 12, NULL, 0, OP_NODATA, 30, app_data.debug)
	);
}


/*
 * scsipt_disc_present
 *	Check if a disc is loaded
 *
 * Args:
 *	fd - Device file descriptor
 *
 * Return:
 *	TRUE - ready
 *	FALSE - not ready
 */
bool_t
scsipt_disc_present(int fd)
{
	int	i,
		j;
	bool_t	ready = FALSE;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			/* Send Test Unit Ready command to check
			 * if the drive is ready.
			 */
			if (scsipt_tst_unit_rdy(fd, app_data.debug)) {
				ready = TRUE;
				break;
			}
		}

		if (ready)
			break;

		/* Try spinning up the disc */
		(void) scsipt_do_start_stop(fd, TRUE, FALSE, app_data.debug);
	}

	return (ready);
}


/*
 * scsipt_do_playaudio
 *	General top-level play audio function
 *
 * Args:
 *	fd - Device file descriptor
 *	addr_fmt - The address formats specified:
 *		ADDR_BLK: logical block address
 *		ADDR_MSF: MSF address
 *		ADDR_TRKIDX: Track/index numbers
 *		ADDR_OPTEND: Ending address can be ignored
 *	start_addr - Starting logical block address
 *	end_addr - Ending logical block address
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
scsipt_do_playaudio(
	int		fd,
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
			do_playmsf = FALSE,
			do_play12 = FALSE,
			do_play10 = FALSE,
			do_playti = FALSE;

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
		scsipt_sav_end_msf = *end_msf;
	}
	if (addr_fmt & ADDR_BLK) {
		if (end_addr >= CLIP_FRAMES)
			end_addr -= CLIP_FRAMES;
		else
			end_addr = 0;

		/* Save end address for error recovery */
		scsipt_sav_end_addr = end_addr;
	}

	/* Save end address format for error recovery */
	scsipt_sav_end_fmt = addr_fmt;

	if (scsipt_vutbl[app_data.vendor_code].playaudio != NULL) {
		ret = scsipt_vutbl[app_data.vendor_code].playaudio(
			addr_fmt,
			start_addr, end_addr,
			start_msf, emsfp,
			trk, idx
		);
	}

	if (!ret) {
		/* If the device does not claim SCSI-2 compliance, and the
		 * device-specific configuration is not SCSI-2, then don't
		 * attempt to deliver SCSI-2 commands to the device.
		 */
		if (app_data.vendor_code != VENDOR_SCSI2 &&
		    scsipt_dev_scsiver < 2)
			return FALSE;
	
		do_playmsf = (addr_fmt & ADDR_MSF) && app_data.playmsf_supp;
		do_play12 = (addr_fmt & ADDR_BLK) && app_data.play12_supp;
		do_play10 = (addr_fmt & ADDR_BLK) && app_data.play10_supp;
		do_playti = (addr_fmt & ADDR_TRKIDX) && app_data.playti_supp;

		if (do_playmsf || do_play12 || do_play10 || do_playti) {
			if (scsipt_playing)
				/* Pause playback first */
				(void) scsipt_do_pause_resume(fd, FALSE);
			else
				/* Spin up CD */
				(void) scsipt_do_start_stop(fd, TRUE, FALSE,
							    app_data.debug);
		}
	}

	if (!ret && do_playmsf)
		ret = scsipt_playmsf(fd, start_msf, emsfp);

	if (!ret && do_play12)
		ret = scsipt_play12(fd, start_addr, end_addr - start_addr);

	if (!ret && do_play10)
		ret = scsipt_play10(fd, start_addr, end_addr - start_addr);

	if (!ret && do_playti)
		ret = scsipt_play_trkidx(fd, trk, idx, trk, idx);

	if (ret)
		scsipt_playing = TRUE;

	return (ret);
}


/*
 * scsipt_do_pause_resume
 *	General top-level pause/resume function
 *
 * Args:
 *	fd - Device file descriptor
 *	resume - Whether to resume or pause
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
scsipt_do_pause_resume(int fd, bool_t resume)
{
	bool_t	ret = FALSE;

	if (scsipt_vutbl[app_data.vendor_code].pause_resume != NULL)
		ret = scsipt_vutbl[app_data.vendor_code].pause_resume(resume);

	/* If the device does not claim SCSI-2 compliance, and the
	 * device-specific configuration is not SCSI-2, then don't
	 * attempt to deliver SCSI-2 commands to the device.
	 */
	if (!ret && app_data.vendor_code != VENDOR_SCSI2 &&
	    scsipt_dev_scsiver < 2)
		return FALSE;

	if (!ret && app_data.pause_supp)
		ret = scsipt_pause_resume(fd, resume);

	if (!ret && resume) {
		int		i;
		word32_t	saddr,
				eaddr;
		msf_t		smsf,
				emsf;
		curstat_t	*s = di_clinfo->curstat_addr();

		/* Resume failed: try restarting playback */
		saddr = s->cur_tot_addr;
		smsf.min = s->cur_tot_min;
		smsf.sec = s->cur_tot_sec;
		smsf.frame = s->cur_tot_frame;

		if (s->program || s->shuffle) {
			i = -1;

			if (s->prog_cnt > 0)
				i = (int) s->playorder[s->prog_cnt - 1];

			if (i < 0)
				return (ret);

			eaddr = s->trkinfo[i+1].addr;
			emsf.min = s->trkinfo[i+1].min;
			emsf.sec = s->trkinfo[i+1].sec;
			emsf.frame = s->trkinfo[i+1].frame;
		}
		else {
			eaddr = s->tot_addr;
			emsf.min = s->tot_min;
			emsf.sec = s->tot_sec;
			emsf.frame = s->tot_frame;
		}

		ret = scsipt_do_playaudio(fd,
			ADDR_BLK | ADDR_MSF,
			saddr, eaddr, &smsf, &emsf, 0, 0
		);
	}

	return (ret);
}


/*
 * scsipt_do_start_stop
 *	General top-level start/stop function
 *
 * Args:
 *	fd - Device file descriptor
 *	start - Whether to start unit or stop unit
 *	loej - Whether caddy load/eject operation should be performed
 *	prnerr - Whether to print error messages if the command fails
 *		 (SCSI-2 mode only)
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
scsipt_do_start_stop(int fd, bool_t start, bool_t loej, bool_t prnerr)
{
	bool_t	ret = FALSE;

	if (!app_data.load_supp && start && loej)
		return FALSE;

	if (!app_data.eject_supp)
		loej = 0;

#if defined(SOL2_VOLMGT) && !defined(DEMO_ONLY)
	/* Sun Hack: Under Solaris 2.x with the Volume Manager
	 * we need to use a special SunOS ioctl to eject the CD.
	 */
	if (app_data.sol2_volmgt && !start && loej)
		ret = sol2_volmgt_eject(fd);
	else
#endif
	{
		if (!start && loej &&
		    scsipt_vutbl[app_data.vendor_code].eject != NULL)
			ret = scsipt_vutbl[app_data.vendor_code].eject();

		if (!ret &&
		    scsipt_vutbl[app_data.vendor_code].start_stop != NULL)
			ret = scsipt_vutbl[app_data.vendor_code].start_stop(
				start, loej
			);

		if (!ret)
			ret = scsipt_start_stop(fd, start, loej, prnerr);
	}

	if (ret && !start)
		scsipt_playing = FALSE;

	return (ret);
}


/*
 * scsipt_fail_recov
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
scsipt_fail_recov(word32_t blk)
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
	if (recov_start_msf.min > scsipt_sav_end_msf.min)
		ret = FALSE;
	else if (recov_start_msf.min == scsipt_sav_end_msf.min) {
		if (recov_start_msf.sec > scsipt_sav_end_msf.sec)
			ret = FALSE;
		else if ((recov_start_msf.sec ==
			  scsipt_sav_end_msf.sec) &&
			 (recov_start_msf.frame >
			  scsipt_sav_end_msf.frame)) {
			ret = FALSE;
		}
	}
	if (recov_start_addr >= scsipt_sav_end_addr)
		ret = FALSE;

	if (ret) {
		/* Restart playback */
		(void) fprintf(errfp, "CD audio: %s (%02u:%02u.%02u)\n",
				app_data.str_recoverr,
				recov_start_msf.min,
				recov_start_msf.sec,
				recov_start_msf.frame);

		ret = scsipt_do_playaudio(devfd,
			scsipt_sav_end_fmt,
			recov_start_addr, scsipt_sav_end_addr,
			&recov_start_msf, &scsipt_sav_end_msf,
			0, 0
		);
	}

	return (ret);
}


/*
 * scsipt_get_playstatus
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
scsipt_get_playstatus(curstat_t *s)
{
	int		trkno,
			idxno;
	byte_t		buf[SZ_RDSUBQ],
			audio_status,
			*cp;
	bool_t		done,
			ret = FALSE;
	subq_hdr_t	*h;
	subq_01_t	*p;
	static int	errcnt = 0;
	static word32_t	errblk = 0;
	static bool_t	in_scsipt_get_playstatus = FALSE;


	/* Lock this routine from multiple entry */
	if (in_scsipt_get_playstatus)
		return TRUE;

	in_scsipt_get_playstatus = TRUE;

	if (scsipt_vutbl[app_data.vendor_code].get_playstatus != NULL) {
		ret = scsipt_vutbl[app_data.vendor_code].get_playstatus(
			s, &audio_status
		);
	}

	/* If the device does not claim SCSI-2 compliance, and the
	 * device-specific configuration is not SCSI-2, then don't
	 * attempt to deliver SCSI-2 commands to the device.
	 */
	if (!ret && app_data.vendor_code != VENDOR_SCSI2 &&
	    scsipt_dev_scsiver < 2) {
		in_scsipt_get_playstatus = FALSE;
		return FALSE;
	}

	if (!ret) {
		(void) memset(buf, 0, sizeof(buf));

		if (!scsipt_rdsubq(devfd, buf, (byte_t)
				 (app_data.curpos_fmt ? SUB_CURPOS : SUB_ALL),
				 1, 0, TRUE)) {
			/* Check to see if the disc had been manually ejected */
			if (!scsipt_disc_ready(s)) {
				scsipt_sav_end_addr = 0;
				scsipt_sav_end_msf.min = 0;
				scsipt_sav_end_msf.sec = 0;
				scsipt_sav_end_msf.frame = 0;
				scsipt_sav_end_fmt = 0;
				errcnt = 0;
				errblk = 0;

				in_scsipt_get_playstatus = FALSE;
				return FALSE;
			}

			/* The read subchannel command failed for some
			 * unknown reason.  Just return success and
			 * hope the next poll succeeds.  We don't want
			 * to return FALSE here because that would stop
			 * the poll.
			 */
			in_scsipt_get_playstatus = FALSE;
			return TRUE;
		}

		h = (subq_hdr_t *)(void *) buf;

		audio_status = h->audio_status;

		/* Check the subchannel data */
		cp = (byte_t *) h + sizeof(subq_hdr_t);
		switch (*cp) {
		case SUB_ALL:
		case SUB_CURPOS:
			p = (subq_01_t *)(void *) cp;

			/* Hack: to work around firmware anomalies
			 * in some CD-ROM drives.
			 */
			if (p->trkno >= MAXTRACK &&
			    p->trkno != LEAD_OUT_TRACK) {
				audio_status = AUDIO_NOTVALID;
				break;
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

			if (scsipt_bcd_hack) {
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
			break;
		default:
			/* Something is wrong with the data */
			break;
		}
	}

	/* Update time display */
	DPY_TIME(s, FALSE);


	/* Hack: to work around the fact that some CD-ROM drives
	 * return AUDIO_PAUSED status after issuing a Stop Unit command.
	 * Just treat the status as completed if we get a paused status
	 * and we don't expect the drive to be paused.
	 */
	if (audio_status == AUDIO_PAUSED && s->mode != MOD_PAUSE &&
	    !scsipt_idx_pause)
		audio_status = AUDIO_COMPLETED;

	/* Force completion status */
	if (scsipt_fake_stop)
		audio_status = AUDIO_COMPLETED;

	/* Deal with playback status */
	switch (audio_status) {
	case AUDIO_PLAYING:
	case AUDIO_PAUSED:
		done = FALSE;

		/* If we haven't encountered an error for a while, then
		 * clear the error count.
		 */
		if (errcnt > 0 && (s->cur_tot_addr - errblk) > ERR_CLRTHRESH)
			errcnt = 0;
		break;

	case AUDIO_FAILED:
		/* Check to see if the disc had been manually ejected */
		if (!scsipt_disc_ready(s)) {
			scsipt_sav_end_addr = 0;
			scsipt_sav_end_msf.min = 0;
			scsipt_sav_end_msf.sec = 0;
			scsipt_sav_end_msf.frame = 0;
			scsipt_sav_end_fmt = 0;
			errcnt = 0;
			errblk = 0;

			in_scsipt_get_playstatus = FALSE;
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

		if (!done && scsipt_fail_recov(errblk)) {
			in_scsipt_get_playstatus = FALSE;
			return TRUE;
		}

		/*FALLTHROUGH*/
	case AUDIO_COMPLETED:
	case AUDIO_NOSTATUS:
	case AUDIO_NOTVALID:
		done = TRUE;

		if (!scsipt_fake_stop)
			scsipt_playing = FALSE;

		scsipt_fake_stop = FALSE;

		switch (s->mode) {
		case MOD_SAMPLE:
			done = !scsipt_run_sample(s);
			break;

		case MOD_AB:
			done = !scsipt_run_ab(s);
			break;

		case MOD_PLAY:
		case MOD_PAUSE:
			s->cur_trk_addr = 0;
			s->cur_trk_min = s->cur_trk_sec = s->cur_trk_frame = 0;

			if (s->shuffle || s->program)
				done = !scsipt_run_prog(s);

			if (s->repeat && (s->program || !app_data.multi_play))
				done = !scsipt_run_repeat(s);

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

		scsipt_sav_end_addr = 0;
		scsipt_sav_end_msf.min = scsipt_sav_end_msf.sec =
			scsipt_sav_end_msf.frame = 0;
		scsipt_sav_end_fmt = 0;
		errcnt = 0;
		errblk = 0;

		if (app_data.multi_play && omode == MOD_PLAY && !prog) {
			bool_t	cont;

			s->prev_disc = s->cur_disc;

			if (app_data.reverse) {
				if (s->cur_disc > s->first_disc) {
					/* Play the previous disc */
					s->cur_disc--;
					scsipt_mult_autoplay = TRUE;
				}
				else {
					/* Go to the last disc */
					s->cur_disc = s->last_disc;

					if (s->repeat) {
						s->rptcnt++;
						scsipt_mult_autoplay = TRUE;
					}
				}
			}
			else {
				if (s->cur_disc < s->last_disc) {
					/* Play the next disc */
					s->cur_disc++;
					scsipt_mult_autoplay = TRUE;
				}
				else {
					/* Go to the first disc.  */
					s->cur_disc = s->first_disc;

					if (s->repeat) {
						s->rptcnt++;
						scsipt_mult_autoplay = TRUE;
					}
				}
			}

			if ((cont = scsipt_mult_autoplay) == TRUE) {
				/* Allow recursion from this point */
				in_scsipt_get_playstatus = FALSE;
			}

			/* Change disc */
			scsipt_chgdisc(s);

			if (cont)
				return TRUE;
		}

		s->rptcnt = 0;
		DPY_ALL(s);

		if (app_data.done_eject) {
			/* Eject the disc */
			scsipt_load_eject(s);
		}
		else {
			/* Spin down the disc */
			(void) scsipt_do_start_stop(devfd, FALSE, FALSE, TRUE);
		}

		in_scsipt_get_playstatus = FALSE;
		return FALSE;
	}

	in_scsipt_get_playstatus = FALSE;
	return TRUE;
}


/*
 * scsipt_cfg_vol
 *	Audio volume control function
 *
 * Args:
 *	vol - Logical volume value to set to
 *	s - Pointer to the curstat_t structure
 *	query - If TRUE, query current volume only
 *	user - Whether a volume update is due to user action
 *
 * Return:
 *	The current logical volume value, or -1 on failure.
 */
STATIC int
scsipt_cfg_vol(int vol, curstat_t *s, bool_t query, bool_t user)
{
	int			vol1,
				vol2;
	mode_sense_data_t	*ms_data;
	blk_desc_t		*bdesc;
	audio_pg_t		*audiopg;
	byte_t			buf[SZ_MSENSE];
	bool_t			ret = FALSE;
	static bool_t		muted = FALSE;


	if (scsipt_vutbl[app_data.vendor_code].volume != NULL) {
		vol = scsipt_vutbl[app_data.vendor_code].volume(vol, s, query);
		return (vol);
	}

	if (scsipt_vutbl[app_data.vendor_code].mute != NULL) {
		if (!query) {
			if (vol < (int) s->level)
				vol = 0;
			else if (vol > (int) s->level ||
				 (vol != 0 && vol != 100))
				vol = 100;

			ret = scsipt_vutbl[app_data.vendor_code].mute(
				(bool_t) (vol == 0)
			);
			if (ret)
				muted = (vol == 0);
		}

		vol = muted ? 0 : MAX_VOL;

		/* Force volume slider to full mute or max positions */
		if (user)
			SET_VOL_SLIDER(vol);

		return (vol);
	}

	if (!app_data.mselvol_supp)
		return 0;

	(void) memset(buf, 0, sizeof(buf));

	if (!scsipt_modesense(devfd, buf, 0, PG_AUDIOCTL, SZ_AUDIOCTL))
		return -1;

	ms_data = (mode_sense_data_t *)(void *) buf;
	bdesc = (blk_desc_t *)(void *) ms_data->data;
	audiopg = (audio_pg_t *)(void *)
		&ms_data->data[ms_data->bdescr_len];

	if (audiopg->pg_code == PG_AUDIOCTL) {
		if (query) {
			vol1 = di_untaper_vol(
				di_unscale_vol((int) audiopg->p0_vol)
			);
			vol2 = di_untaper_vol(
				di_unscale_vol((int) audiopg->p1_vol)
			);
			scsipt_route_left = (byte_t) audiopg->p0_ch_ctrl;
			scsipt_route_right = (byte_t) audiopg->p1_ch_ctrl;

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
			ms_data->medium = 0;

			if (ms_data->bdescr_len > 0)
				bdesc->num_blks = 0;

			audiopg->p0_vol = di_scale_vol(
				di_taper_vol(vol * (int) s->level_left / 100)
			);
			audiopg->p1_vol = di_scale_vol(
				di_taper_vol(vol * (int) s->level_right / 100)
			);

			audiopg->p0_ch_ctrl = scsipt_route_left;
			audiopg->p1_ch_ctrl = scsipt_route_right;

			audiopg->sotc = 0;
			audiopg->immed = 1;

			if (scsipt_modesel(devfd, buf, PG_AUDIOCTL,
					   SZ_AUDIOCTL)) {
				/* Success */
				return (vol);
			}
			else if (audiopg->p0_vol != audiopg->p1_vol) {
				/* Set the balance to the center
				 * and retry.
				 */
				audiopg->p0_vol = audiopg->p1_vol =
					di_scale_vol(di_taper_vol(vol));

				if (scsipt_modesel(devfd, buf, PG_AUDIOCTL,
						   SZ_AUDIOCTL)) {
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
 * scsipt_vendor_model
 *	Query and update CD-ROM vendor/model/revision information
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
scsipt_vendor_model(curstat_t *s)
{
	inquiry_data_t	inq;
	int		i;
	char		errstr[ERR_BUF_SZ];

	i = 0;
	do {
		(void) memset((byte_t *) &inq, 0, sizeof(inq));

		if (scsipt_inquiry(devfd, (byte_t *) &inq, sizeof(inq)))
			break;

		sleep(1);
	} while (++i < 3);

	if (i >= 3)
		/* Can't do inquiry: shrug. */
		return;

#ifdef _AIX
	/* Hack: Some versions of AIX 4.x returns
	 * bogus SCSI inquiry data.  Attempt to work
	 * around this.
	 */
	{
		int	j;
		char	*p;

		p = (char *) &inq;
		for (j = 0; j < 8; j++, p++) {
			if (!isalnum(*p) && !isspace(*p))
				break;
		}
		if (j == 8) {
			/* Fix up inquiry data */
			(void) memset((byte_t *) &inq, 0, sizeof(inq));

			p = (char *) &inq;
			(void) strncpy(s->vendor, p, 8);
			s->vendor[8] = '\0';

			p += 8;
			(void) strncpy(s->prod, p, 16);
			s->prod[16] = '\0';

			(void) strncpy(inq.vendor, s->vendor, 8);
			(void) strncpy(inq.prod, s->prod, 16);
			inq.type = DEV_ROM;
			inq.rmb = 1;
			inq.ver = 2;
		}
	}
#endif	/* AIX */

	DBGPRN(errfp,
	"\nCD-ROM: vendor=\"%.8s\" prod=\"%.16s\" rev=\"%.4s\"\n",
		inq.vendor, inq.prod, inq.revnum);

	(void) strncpy(s->vendor, (char *) inq.vendor, 8);
	s->vendor[8] = '\0';

	(void) strncpy(s->prod, (char *) inq.prod, 16);
	s->prod[16] = '\0';

	(void) strncpy(s->revnum, (char *) inq.revnum, 4);
	s->revnum[4] = '\0';

#ifndef OEM_CDROM
	/* Check for errors.
	 * Note: Some OEM drives identify themselves
	 * as a hard disk instead of a CD-ROM drive
	 * (such as the Toshiba CD-ROM XM revision 1971
	 * OEMed by SGI).  In order to use those units
	 * this file must be compiled with -DOEM_CDROM.
	 */
	if ((inq.type != DEV_ROM && inq.type != DEV_WORM) || !inq.rmb){
		/* Not a CD-ROM or a CD-R device */
		scsipt_notrom_error = TRUE;
		(void) sprintf(errstr, app_data.str_notrom, s->curdev);
		DI_FATAL(errstr);
		return;
	}
#endif

	/* Check for unsupported drives */
	scsipt_dev_scsiver = (byte_t) (inq.ver & 0x07);
	if (scsipt_dev_scsiver < 2 && app_data.vendor_code == VENDOR_SCSI2) {
		/* Not SCSI-2 or later */
		(void) sprintf(errstr, app_data.str_notscsi2, s->curdev);
		DI_WARNING(errstr);
	}
}


/*
 * scsipt_fix_toc
 *	CD Table Of Contents post-processing function.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
scsipt_fix_toc(curstat_t *s)
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
 * scsipt_get_toc
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
scsipt_get_toc(curstat_t *s)
{
	int			i,
				ntrks;
	byte_t			buf[SZ_RDTOC],
				*cp,
				*toc_end;
	bool_t			ret = FALSE;
	toc_hdr_t		*h;
	toc_trk_descr_t		*p;


	if (scsipt_vutbl[app_data.vendor_code].get_toc != NULL)
		ret = scsipt_vutbl[app_data.vendor_code].get_toc(s);

	if (ret) {
		scsipt_fix_toc(s);
		return TRUE;
	}

	/* If the device does not claim SCSI-2 compliance, and the
	 * device-specific configuration is not SCSI-2, then don't
	 * attempt to deliver SCSI-2 commands to the device.
	 */
	if (!ret && app_data.vendor_code != VENDOR_SCSI2 &&
	    scsipt_dev_scsiver < 2)
		return FALSE;

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
	 * Some CD-ROMs return track numbers in BCD
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
				scsipt_bcd_hack = TRUE;
				s->first_trk = (byte_t) trk0;
				s->last_trk = (byte_t) trk1;
				break;
			}
		}

		cp += sizeof(toc_trk_descr_t);
	}

	/*
	 * Fill in TOC data
	 */
	cp = (byte_t *) h + sizeof(toc_hdr_t);

	for (i = 0; cp < toc_end && i < MAXTRACK; i++) {
		p = (toc_trk_descr_t *)(void *) cp;

		/* Hack: Work around firmware problem on some drives */
		if (i > 0 && s->trkinfo[i-1].trkno == s->last_trk &&
		    p->trkno != LEAD_OUT_TRACK) {
			(void) memset(buf, 0, sizeof(buf));

			if (!scsipt_rdtoc(devfd, buf, TRUE, (int) s->last_trk))
				return FALSE;

			cp = (byte_t *) h + sizeof(toc_hdr_t) +
			     sizeof(toc_trk_descr_t);

			toc_end = (byte_t *) h + util_bswap16(h->data_len) + 2;

			p = (toc_trk_descr_t *)(void *) cp;
		}

		if (scsipt_bcd_hack)
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

	scsipt_fix_toc(s);
	return TRUE;
}


/*
 * scsipt_start_stat_poll
 *	Start polling the drive for current playback status
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
scsipt_start_stat_poll(curstat_t *s)
{
	/* Start poll timer */
	if (di_clinfo->timeout != NULL) {
		scsipt_stat_id = di_clinfo->timeout(
			scsipt_stat_interval,
			scsipt_stat_poll,
			(byte_t *) s
		);

		if (scsipt_stat_id != 0)
			scsipt_stat_polling = TRUE;
	}
}


/*
 * scsipt_stop_stat_poll
 *	Stop polling the drive for current playback status
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
STATIC void
scsipt_stop_stat_poll(void)
{
	if (scsipt_stat_polling) {
		/* Stop poll timer */
		if (di_clinfo->untimeout != NULL)
			di_clinfo->untimeout(scsipt_stat_id);

		scsipt_stat_polling = FALSE;
	}
}


/*
 * scsipt_start_insert_poll
 *	Start polling the drive for disc insertion
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
scsipt_start_insert_poll(curstat_t *s)
{
	if (scsipt_insert_polling || app_data.ins_disable ||
	    (s->mode != MOD_BUSY && s->mode != MOD_NODISC))
		return;

	if (app_data.numdiscs > 1 && app_data.multi_play)
		scsipt_ins_interval =
			app_data.ins_interval / app_data.numdiscs;
	else
		scsipt_ins_interval = app_data.ins_interval;

	if (scsipt_ins_interval < 500)
		scsipt_ins_interval = 500;

	/* Start poll timer */
	if (di_clinfo->timeout != NULL) {
		scsipt_insert_id = di_clinfo->timeout(
			scsipt_ins_interval,
			scsipt_insert_poll,
			(byte_t *) s
		);

		if (scsipt_insert_id != 0)
			scsipt_insert_polling = TRUE;
	}
}


/*
 * stat_poll
 *	The playback status polling function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
scsipt_stat_poll(curstat_t *s)
{
	if (!scsipt_stat_polling)
		return;

	/* Get current audio playback status */
	if (scsipt_get_playstatus(s)) {
		/* Register next poll interval */
		if (di_clinfo->timeout != NULL) {
			scsipt_stat_id = di_clinfo->timeout(
				scsipt_stat_interval,
				scsipt_stat_poll,
				(byte_t *) s
			);
		}
	}
	else
		scsipt_stat_polling = FALSE;
}


/*
 * insert_poll
 *	The disc insertion polling function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
scsipt_insert_poll(curstat_t *s)
{
	/* Check to see if a disc is inserted */
	if (!scsipt_disc_ready(s)) {
		/* Register next poll interval */
		if (di_clinfo->timeout != NULL) {
			scsipt_insert_id = di_clinfo->timeout(
				scsipt_ins_interval,
				scsipt_insert_poll,
				(byte_t *) s
			);
		}
	}
	else
		scsipt_insert_polling = FALSE;
}


/*
 * scsipt_disc_ready
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
scsipt_disc_ready(curstat_t *s)
{
	int		vol;
	bool_t		err,
			first_open = FALSE,
			reopen = FALSE;
	static bool_t	opened_once = FALSE,
			in_scsipt_disc_ready = FALSE;

	/* Lock this routine from multiple entry */
	if (in_scsipt_disc_ready)
		return TRUE;

	in_scsipt_disc_ready = TRUE;

	/* If device has not been opened, attempt to open it */
	if (scsipt_not_open) {
		/* Check for another copy of the CD player running on
		 * the specified device.
		 */
		if (!s->devlocked && !di_devlock(s, app_data.device)) {
			s->mode = MOD_BUSY;
			DPY_TIME(s, FALSE);
			scsipt_start_insert_poll(s);
			in_scsipt_disc_ready = FALSE;
			return FALSE;
		}

		s->devlocked = TRUE;
		s->mode = MOD_NODISC;
		DPY_DISC(s);

		if (app_data.numdiscs > 1 &&
		    app_data.chg_method == CHG_SCSI_MEDCHG &&
		    !opened_once) {
			/* Open medium-changer device */

			DBGPRN(errfp, "\nOpen Medium-changer: %s\n",
				di_devlist[1]);

			if ((chgfd = pthru_open(di_devlist[1])) < 0) {
				DBGPRN(errfp, "Open of %s failed\n",
					di_devlist[1]);
			}
			else if (!scsipt_chg_start(s)) {
				DBGPRN(errfp, "\nClose Medium-changer: %s\n",
					di_devlist[1]);
				pthru_close(chgfd);
				chgfd = -1;
			}

			if (chgfd < 0) {
				/* Changer init failed: set to
				 * single-disc mode.
				 */
				app_data.numdiscs = 1;
				app_data.chg_method = CHG_NONE;
				app_data.multi_play = FALSE;
				app_data.reverse = FALSE;
				s->first_disc = s->last_disc = s->cur_disc = 1;

				DI_INFO(app_data.str_medchg_noinit);
			}
		}

		if (chgfd >= 0 && scsipt_chg_ready(s)) {
			/* Try to load disc */
			(void) scsipt_move_medium(
				chgfd,
				scsipt_mcparm.mtbase,
				(word16_t) (scsipt_mcparm.stbase +
					    s->cur_disc - 1),
				scsipt_mcparm.dtbase
			);
		}

		/* Open CD-ROM device */
		DBGPRN(errfp, "\nOpen CD-ROM: %s\n", s->curdev);

		if ((devfd = pthru_open(s->curdev)) < 0) {
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

						if (scsipt_mult_autoplay) {
						    if (s->repeat)
							s->rptcnt++;
						    else
							scsipt_mult_autoplay =
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

						if (scsipt_mult_autoplay) {
						    if (s->repeat)
							s->rptcnt++;
						    else
							scsipt_mult_autoplay =
							FALSE;
						}
					}
				}

				if (app_data.chg_method == CHG_SCSI_LUN)
					s->curdev = di_devlist[s->cur_disc - 1];
			}

			scsipt_start_insert_poll(s);
			in_scsipt_disc_ready = FALSE;
			return FALSE;
		}

		if (!opened_once)
			first_open = TRUE;
		else
			reopen = TRUE;

		scsipt_not_open = FALSE;
		opened_once = TRUE;
	}

	if (app_data.play_notur && s->mode != MOD_STOP &&
	    s->mode != MOD_NODISC) {
		/* For those drives that returns failure status to
		 * the Test Unit Ready command during audio playback,
		 * we just silently return success if the drive is
		 * supposed to be playing audio.  Shrug.
		 */
		err = FALSE;
	}
	else if ((err = !scsipt_disc_present(devfd)) != FALSE) {
		s->mode = MOD_NODISC;
		DBCLEAR(s, FALSE);
	}

	if (!err) {
		if (first_open) {
			/* Let things settle a bit */
			sleep(1);

			/* Start up vendor-unique modules */
			if (scsipt_vutbl[app_data.vendor_code].start != NULL)
				scsipt_vutbl[app_data.vendor_code].start();

			/* Fill in inquiry data */
			scsipt_vendor_model(s);

			/* Query current volume/balance settings */
			if ((vol = scsipt_cfg_vol(0, s, TRUE, FALSE)) >= 0)
				s->level = (byte_t) vol;
			else
				s->level = 0;

			/* Set volume to preset value, if so configured */
			if (app_data.startup_vol > 0 &&
			    (vol = scsipt_cfg_vol(app_data.startup_vol, s,
						  FALSE, FALSE)) >= 0)
				s->level = (byte_t) vol;

			/* Initialize sliders */
			SET_VOL_SLIDER(s->level);
			SET_BAL_SLIDER(
				(int) (s->level_right - s->level_left) / 2
			);

			/* Set up channel routing */
			scsipt_route(s);
		}
		else if (reopen) {
			/* Force to current settings */
			(void) scsipt_cfg_vol(s->level, s, FALSE, FALSE);
			scsipt_route(s);
		}
	}

	/* Read disc table of contents */
	if (err || (s->mode == MOD_NODISC && !scsipt_get_toc(s))) {
		if (app_data.eject_close) {
			/* Close CD-ROM device */
			DBGPRN(errfp, "\nClose CD-ROM: %s\n", s->curdev);
			pthru_close(devfd);
			devfd = -1;

			scsipt_not_open = TRUE;
		}

		di_reset_curstat(s, TRUE, (bool_t) !app_data.multi_play);
		DPY_ALL(s);

		scsipt_start_insert_poll(s);
		in_scsipt_disc_ready = FALSE;
		return FALSE;
	}

	if (s->mode != MOD_NODISC) {
		in_scsipt_disc_ready = FALSE;
		return TRUE;
	}

	s->mode = MOD_STOP;
	DPY_ALL(s);

	/* Set caddy lock configuration */
	if (app_data.caddylock_supp)
		scsipt_lock(s, app_data.caddy_lock);

	if (app_data.load_play || scsipt_mult_autoplay) {
		scsipt_mult_autoplay = FALSE;

		/* Wait a little while for things to settle */
		(void) sleep(1);

		/* Start auto-play */
		if (!scsipt_override_ap)
			scsipt_play_pause(s);

		if (scsipt_mult_pause) {
			scsipt_mult_pause = FALSE;

			if (scsipt_do_pause_resume(devfd, FALSE)) {
				scsipt_stop_stat_poll();
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
		(void) scsipt_do_start_stop(devfd, FALSE, FALSE, TRUE);
	}

	in_scsipt_disc_ready = FALSE;

	/* Load CD database entry for this disc.
	 * This operation has to be done outside the scope of
	 * in_scsipt_disc_ready because it may recurse
	 * back into this function.
	 */
	DBGET(s);

	return TRUE;
}


/*
 * scsipt_chg_ready
 *	Check if the medium changer is ready for use.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - Changer is ready
 *	FALSE - Changer is not ready
 */
/*ARGSUSED*/
STATIC bool_t
scsipt_chg_ready(curstat_t *s)
{
	int	i;
	bool_t	ret;

	if (app_data.chg_method != CHG_SCSI_MEDCHG)
		return FALSE;

	for (i = 0; i < 10; i++) {
		if ((ret = scsipt_tst_unit_rdy(chgfd, app_data.debug)) == TRUE)
			break;

		(void) sleep(1);
	}

	return (ret);
}


/*
 * scsipt_run_rew
 *	Run search-rewind operation
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
scsipt_run_rew(curstat_t *s)
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
	if (!scsipt_get_playstatus(s)) {
		DO_BEEP();
		return;
	}

	skip_blks = app_data.skip_blks;
	addr = s->cur_tot_addr;

	if (scsipt_start_search) {
		scsipt_start_search = FALSE;
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
	(void) scsipt_do_playaudio(devfd,
		ADDR_BLK | ADDR_MSF | ADDR_OPTEND,
		start_addr, end_addr,
		&smsf, &emsf,
		0, 0
	);

	if (di_clinfo->timeout != NULL) {
		scsipt_search_id = di_clinfo->timeout(
			app_data.skip_pause,
			scsipt_run_rew,
			(byte_t *) s
		);
	}
}


/*
 * scsipt_stop_rew
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
scsipt_stop_rew(curstat_t *s)
{
	if (di_clinfo->untimeout != NULL)
		di_clinfo->untimeout(scsipt_search_id);
}


/*
 * scsipt_run_ff
 *	Run search-fast-forward operation
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
STATIC void
scsipt_run_ff(curstat_t *s)
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
	if (!scsipt_get_playstatus(s)) {
		DO_BEEP();
		return;
	}

	skip_blks = app_data.skip_blks;
	addr = s->cur_tot_addr;

	if (scsipt_start_search) {
		scsipt_start_search = FALSE;
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
	(void) scsipt_do_playaudio(devfd,
		ADDR_BLK | ADDR_MSF | ADDR_OPTEND,
		start_addr, end_addr,
		&smsf, &emsf,
		0, 0
	);

	if (di_clinfo->timeout != NULL) {
		scsipt_search_id = di_clinfo->timeout(
			app_data.skip_pause,
			scsipt_run_ff,
			(byte_t *) s
		);
	}
}


/*
 * scsipt_stop_ff
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
scsipt_stop_ff(curstat_t *s)
{
	if (di_clinfo->untimeout != NULL)
		di_clinfo->untimeout(scsipt_search_id);
}


/*
 * scsipt_run_ab
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
scsipt_run_ab(curstat_t *s)
{
	return (
		scsipt_do_playaudio(devfd,
			ADDR_BLK | ADDR_MSF,
			scsipt_ab_start_addr, scsipt_ab_end_addr,
			&scsipt_ab_start_msf, &scsipt_ab_end_msf,
			0, 0
		)
	);
}


/*
 * scsipt_run_sample
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
scsipt_run_sample(curstat_t *s)
{
	word32_t	saddr,
			eaddr;
	msf_t		smsf,
			emsf;

	if (scsipt_next_sam < s->tot_trks) {
		saddr = s->trkinfo[scsipt_next_sam].addr;
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

		if (scsipt_do_playaudio(devfd, ADDR_BLK | ADDR_MSF,
				 saddr, eaddr, &smsf, &emsf, 0, 0)) {
			scsipt_next_sam++;
			return TRUE;
		}
	}

	scsipt_next_sam = 0;
	return FALSE;
}


/*
 * scsipt_run_prog
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
scsipt_run_prog(curstat_t *s)
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

	if (scsipt_new_progshuf) {
		scsipt_new_progshuf = FALSE;

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

	if (s->trkinfo[i].trkno == LEAD_OUT_TRACK)
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

	ret = scsipt_do_playaudio(devfd,
		ADDR_BLK | ADDR_MSF,
		start_addr, end_addr,
		&start_msf, &end_msf,
		0, 0
	);

	if (s->mode == MOD_PAUSE) {
		(void) scsipt_do_pause_resume(devfd, FALSE);

		/* Restore volume */
		scsipt_mute_off(s);
	}

	return (ret);
}


/*
 * scsipt_run_repeat
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
scsipt_run_repeat(curstat_t *s)
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

		scsipt_new_progshuf = TRUE;
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

		ret = scsipt_do_playaudio(devfd,
			ADDR_BLK | ADDR_MSF,
			s->trkinfo[0].addr, s->tot_addr,
			&start_msf, &end_msf, 0, 0
		);

		if (s->mode == MOD_PAUSE) {
			(void) scsipt_do_pause_resume(devfd, FALSE);

			/* Restore volume */
			scsipt_mute_off(s);
		}

	}

	return (ret);
}


/*
 * scsipt_route_val
 *	Return the channel routing control value used in the
 *	SCSI-2 mode parameter page 0xE (audio parameters).
 *
 * Args:
 *	route_mode - The channel routing mode value.
 *	channel - The channel number desired (0=left 1=right).
 *
 * Return:
 *	The routing control value.
 */
STATIC byte_t
scsipt_route_val(int route_mode, int channel)
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
 * scsipt_chg_start
 *	Multi-disc changer startup initialization
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
scsipt_chg_start(curstat_t *s)
{
	inquiry_data_t		*inq;
	mode_sense_data_t	*ms_data;
	dev_capab_t		*dcapab;
	elem_addr_t		*eaddr;
	byte_t			buf[SZ_MSENSE];

	inq = (inquiry_data_t *)(void *) buf;
	ms_data = (mode_sense_data_t *)(void *) buf;

	switch (app_data.chg_method) {
	case CHG_SCSI_MEDCHG:
		break;

	case CHG_OS_IOCTL:
		DBGPRN(errfp,
		    "%s: OS-ioctl changer method not supported in this mode.\n",
		    "SCSI pass-through");
		return FALSE;

	default:
		/* Nothing to do here */
		return TRUE;
	}

	(void) memset(buf, 0, sizeof(buf));

	/* Send SCSI Inquiry */
	if (!scsipt_inquiry(chgfd, buf, sizeof(inquiry_data_t))) {
		DBGPRN(errfp, "%s: SCSI Inquiry failed\n", di_devlist[1]);
		return FALSE;
	}

	DBGPRN(errfp,
		"\nChanger: vendor=\"%.8s\" prod=\"%.16s\" rev=\"%.4s\"\n",
		inq->vendor, inq->prod, inq->revnum);

	if (inq->type != DEV_CHANGER) {
		DBGPRN(errfp, "%s (type=%d) is not a medium changer device\n",
			di_devlist[1], inq->type);
		return FALSE;
	}

	/* Clear any unit attention condition */
	(void) scsipt_chg_ready(s);

	(void) memset(buf, 0, sizeof(buf));

	/* Check device capabilities */
	if (!scsipt_modesense(chgfd, buf, 0, PG_DEVCAPAB, SZ_DEVCAPAB)) {
		DBGPRN(errfp, "%s: Mode sense command error\n", di_devlist[1]);
		return FALSE;
	}

	dcapab = (dev_capab_t *)(void *) &ms_data->data[ms_data->bdescr_len];

	if (dcapab->pg_code != PG_DEVCAPAB) {
		DBGPRN(errfp, "%s: Mode sense data error\n", di_devlist[1]);
		return FALSE;
	}

	if (dcapab->stor_dt == 0 || dcapab->stor_st == 0 ||
	    dcapab->move_st_dt == 0 || dcapab->move_dt_st == 0) {
		DBGPRN(errfp, "%s: Missing required changer capabilities\n",
			di_devlist[1]);
		return FALSE;
	}

	(void) memset(buf, 0, sizeof(buf));

	/* Get element addresses */
	if (!scsipt_modesense(chgfd, buf, 0, PG_ELEMADDR, SZ_ELEMADDR)) {
		DBGPRN(errfp, "%s: Mode sense command error\n", di_devlist[1]);
		return FALSE;
	}

	eaddr = (elem_addr_t *)(void *) &ms_data->data[ms_data->bdescr_len];

	if (eaddr->pg_code != PG_ELEMADDR) {
		DBGPRN(errfp, "%s: Mode sense data error\n", di_devlist[1]);
		return FALSE;
	}

	scsipt_mcparm.mtbase = util_bswap16((word16_t) eaddr->mt_addr);
	scsipt_mcparm.stbase = util_bswap16((word16_t) eaddr->st_addr);
	scsipt_mcparm.iebase = util_bswap16((word16_t) eaddr->ie_addr);
	scsipt_mcparm.dtbase = util_bswap16((word16_t) eaddr->dt_addr);

	DBGPRN(errfp, "\nMedium changer: MT=0x%x ST=0x%x IE=0x%x DT=0x%x\n",
		(int) scsipt_mcparm.mtbase,
		(int) scsipt_mcparm.stbase,
		(int) scsipt_mcparm.iebase,
		(int) scsipt_mcparm.dtbase);

	if ((int) util_bswap16((word16_t) eaddr->st_num) != app_data.numdiscs) {
		DBGPRN(errfp, "Number of discs configuration mismatch\n");
		return FALSE;
	}
	if ((int) util_bswap16((word16_t) eaddr->mt_num) > 1) {
		DBGPRN(errfp, "Multi-transport changer not supported\n");
		return FALSE;
	}
	if ((int) util_bswap16((word16_t) eaddr->dt_num) > 1) {
		DBGPRN(errfp, "Multi-spindle changer not supported\n");
		return FALSE;
	}

	return TRUE;
}


/*
 * scsipt_chg_halt
 *	Multi-disc changer shutdown
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
scsipt_chg_halt(curstat_t *s)
{
	/* Close medium changer device */
	if (chgfd >= 0) {
		DBGPRN(errfp, "\nClose Medium-changer: %s\n", di_devlist[1]);
		pthru_close(chgfd);
		chgfd = -1;
	}
}


/***********************
 *   public routines   *
 ***********************/


/*
 * scsipt_init
 *	Top-level function to initialize the SCSI pass-through and
 *	vendor-unique modules.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_init(curstat_t *s, di_tbl_t *dt)
{
	int	i;

	if (app_data.di_method != DI_SCSIPT)
		/* SCSI pass-through not configured */
		return;

	/* Initialize libdi calling table */
	dt->check_disc = scsipt_check_disc;
	dt->status_upd = scsipt_status_upd;
	dt->lock = scsipt_lock;
	dt->repeat = scsipt_repeat;
	dt->shuffle = scsipt_shuffle;
	dt->load_eject = scsipt_load_eject;
	dt->ab = scsipt_ab;
	dt->sample = scsipt_sample;
	dt->level = scsipt_level;
	dt->play_pause = scsipt_play_pause;
	dt->stop = scsipt_stop;
	dt->chgdisc = scsipt_chgdisc;
	dt->prevtrk = scsipt_prevtrk;
	dt->nexttrk = scsipt_nexttrk;
	dt->previdx = scsipt_previdx;
	dt->nextidx = scsipt_nextidx;
	dt->rew = scsipt_rew;
	dt->ff = scsipt_ff;
	dt->warp = scsipt_warp;
	dt->route = scsipt_route;
	dt->mute_on = scsipt_mute_on;
	dt->mute_off = scsipt_mute_off;
	dt->start = scsipt_start;
	dt->icon = scsipt_icon;
	dt->halt = scsipt_halt;
	dt->mode = scsipt_mode;
	dt->vers = scsipt_vers;

	/* Initalize SCSI pass-through module */
	scsipt_stat_polling = FALSE;
	scsipt_stat_interval = app_data.stat_interval;
	scsipt_insert_polling = FALSE;
	scsipt_next_sam = FALSE;
	scsipt_new_progshuf = FALSE;
	scsipt_sav_end_addr = 0;
	scsipt_sav_end_msf.min = scsipt_sav_end_msf.sec =
		scsipt_sav_end_msf.frame = 0;
	scsipt_sav_end_fmt = 0;

#ifdef SETUID_ROOT
#ifdef SOL2_VOLMGT
	if (!app_data.sol2_volmgt)
#endif	/* SOL2_VOLMGT */
	{
		DBGPRN(errfp, "\nSetting uid to 0\n");

		if (setuid(0) < 0 || getuid() != 0) {
			DI_FATAL(app_data.str_moderr);
			return;
		}
	}
#endif	/* SETUID_ROOT */

	/* Initialize curstat structure */
	di_reset_curstat(s, TRUE, TRUE);

	/* Initialize the SCSI-2 entry of the scsipt_vutbl jump table */
	scsipt_vutbl[VENDOR_SCSI2].vendor = "SCSI-2";
	scsipt_vutbl[VENDOR_SCSI2].playaudio = NULL;
	scsipt_vutbl[VENDOR_SCSI2].pause_resume = NULL;
	scsipt_vutbl[VENDOR_SCSI2].start_stop = NULL;
	scsipt_vutbl[VENDOR_SCSI2].get_playstatus = NULL;
	scsipt_vutbl[VENDOR_SCSI2].volume = NULL;
	scsipt_vutbl[VENDOR_SCSI2].route = NULL;
	scsipt_vutbl[VENDOR_SCSI2].mute = NULL;
	scsipt_vutbl[VENDOR_SCSI2].get_toc = NULL;
	scsipt_vutbl[VENDOR_SCSI2].eject = NULL;
	scsipt_vutbl[VENDOR_SCSI2].start = NULL;
	scsipt_vutbl[VENDOR_SCSI2].halt = NULL;

	DBGPRN(errfp, "%s\n\t%s\n",
		"libdi: SCSI pass-through method", "SCSI-2");

	/* Initialize all configured vendor-unique modules */
	for (i = 0; i < MAX_VENDORS; i++) {
		if (vuinit[i].init != NULL) {
			vuinit[i].init();
			DBGPRN(errfp, "\t%s\n", scsipt_vutbl[i].vendor);
		}
	}

	if (app_data.vendor_code != VENDOR_SCSI2 &&
	    vuinit[app_data.vendor_code].init == NULL) {
		DI_FATAL(app_data.str_novu);
	}
}


/*
 * scsipt_check_disc
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
scsipt_check_disc(curstat_t *s)
{
	return (scsipt_disc_ready(s));
}


/*
 * scsipt_status_upd
 *	Force update of playback status
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_status_upd(curstat_t *s)
{
	(void) scsipt_get_playstatus(s);
}


/*
 * scsipt_lock
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
scsipt_lock(curstat_t *s, bool_t enable)
{
	if (s->mode == MOD_BUSY || s->mode == MOD_NODISC) {
		/* Cannot lock/unlock caddy */
		SET_LOCK_BTN((bool_t) !enable);
		return;
	}

	if (!scsipt_prev_allow(devfd, enable)) {
		DO_BEEP();
		SET_LOCK_BTN((bool_t) !enable);
		return;
	}

	s->caddy_lock = enable;
	SET_LOCK_BTN((bool_t) enable);
}


/*
 * scsipt_repeat
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
scsipt_repeat(curstat_t *s, bool_t enable)
{
	s->repeat = enable;

	if (!enable && scsipt_new_progshuf) {
		scsipt_new_progshuf = FALSE;
		if (s->rptcnt > 0)
			s->rptcnt--;
	}
	DPY_RPTCNT(s);
}


/*
 * scsipt_shuffle
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
scsipt_shuffle(curstat_t *s, bool_t enable)
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
 * scsipt_load_eject
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
scsipt_load_eject(curstat_t *s)
{
	if (!scsipt_disc_ready(s) && app_data.load_supp) {
		/* Disc not ready: try loading the disc */
		if (!scsipt_do_start_stop(devfd, TRUE, TRUE, TRUE))
			DO_BEEP();

		if (scsipt_disc_ready(s) || !app_data.eject_supp)
			return;
	}

	/* Eject the disc */

	if (!app_data.eject_supp) {
		DO_BEEP();

		scsipt_stop_stat_poll();
		di_reset_curstat(s, TRUE, TRUE);
		s->mode = MOD_NODISC;

		DBCLEAR(s, FALSE);
		DPY_ALL(s);

		if (app_data.eject_close) {
			/* Close CD-ROM device */
			DBGPRN(errfp, "\nClose CD-ROM: %s\n", s->curdev);
			pthru_close(devfd);
			devfd = -1;

			scsipt_not_open = TRUE;
		}

		scsipt_start_insert_poll(s);
		return;
	}

	/* Unlock caddy if supported */
	if (app_data.caddylock_supp)
		scsipt_lock(s, FALSE);

	scsipt_stop_stat_poll();
	di_reset_curstat(s, TRUE, TRUE);
	s->mode = MOD_NODISC;

	DBCLEAR(s, FALSE);
	DPY_ALL(s);

	(void) scsipt_do_start_stop(devfd, FALSE, TRUE, TRUE);

	if (app_data.eject_exit)
		di_clinfo->quit(s);
	else {
		if (app_data.eject_close) {
			/* Close CD-ROM device */
			DBGPRN(errfp, "\nClose CD-ROM: %s\n", s->curdev);
			pthru_close(devfd);
			devfd = -1;

			scsipt_not_open = TRUE;
		}

		scsipt_start_insert_poll(s);
	}
}


/*
 * scsipt_ab
 *	A->B segment play mode function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_ab(curstat_t *s)
{
	switch (s->mode) {
	case MOD_SAMPLE:
	case MOD_PLAY:
		/* Get current location */
		if (!scsipt_get_playstatus(s)) {
			DO_BEEP();
			break;
		}

		scsipt_ab_start_addr = s->cur_tot_addr;
		scsipt_ab_start_msf.min = s->cur_tot_min;
		scsipt_ab_start_msf.sec = s->cur_tot_sec;
		scsipt_ab_start_msf.frame = s->cur_tot_frame;

		s->mode = MOD_A;
		DPY_PLAYMODE(s, FALSE);
		break;

	case MOD_A:
		/* Get current location */
		if (!scsipt_get_playstatus(s)) {
			DO_BEEP();
			break;
		}

		scsipt_ab_end_addr = s->cur_tot_addr;
		scsipt_ab_end_msf.min = s->cur_tot_min;
		scsipt_ab_end_msf.sec = s->cur_tot_sec;
		scsipt_ab_end_msf.frame = s->cur_tot_frame;

		/* Make sure that the A->B play interval is no less
		 * than a user-configurable minimum.
		 */
		if ((scsipt_ab_end_addr - scsipt_ab_start_addr) <
		    app_data.min_playblks) {
			scsipt_ab_end_addr = scsipt_ab_start_addr +
					     app_data.min_playblks;
			util_blktomsf(
				scsipt_ab_end_addr,
				&scsipt_ab_end_msf.min,
				&scsipt_ab_end_msf.sec,
				&scsipt_ab_end_msf.frame,
				MSF_OFFSET
			);
		}

		if (!scsipt_run_ab(s)) {
			DO_BEEP();
			return;
		}

		s->mode = MOD_AB;
		DPY_PLAYMODE(s, FALSE);
		break;

	case MOD_AB:
		/* Currently doing A->B playback, just call scsipt_play_pause
		 * to resume normal playback.
		 */
		scsipt_play_pause(s);
		break;

	default:
		DO_BEEP();
		break;
	}
}


/*
 * scsipt_sample
 *	Sample play mode function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_sample(curstat_t *s)
{
	int	i;

	if (!scsipt_disc_ready(s)) {
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
		scsipt_start_stat_poll(s);
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
			scsipt_next_sam = (byte_t) i;
		}
		else {
			s->cur_trk = s->first_trk;
			scsipt_next_sam = 0;
		}
		
		s->cur_idx = 1;

		s->mode = MOD_SAMPLE;
		DPY_ALL(s);

		if (!scsipt_run_sample(s))
			return;

		break;

	case MOD_SAMPLE:
		/* Currently doing Sample playback, just call scsipt_play_pause
		 * to resume normal playback.
		 */
		scsipt_play_pause(s);
		break;

	default:
		DO_BEEP();
		break;
	}
}


/*
 * scsipt_level
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
scsipt_level(curstat_t *s, byte_t level, bool_t drag)
{
	int	actual;

	if (drag && app_data.vendor_code != VENDOR_SCSI2 &&
	    scsipt_vutbl[app_data.vendor_code].volume == NULL)
		return;

	/* Set volume level */
	if ((actual = scsipt_cfg_vol((int) level, s, FALSE, TRUE)) >= 0)
		s->level = (byte_t) actual;
}


/*
 * scsipt_play_pause
 *	Audio playback and pause function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_play_pause(curstat_t *s)
{
	sword32_t	i;
	word32_t	start_addr;
	msf_t		start_msf,
			end_msf;

	scsipt_override_ap = TRUE;

	if (!scsipt_disc_ready(s)) {
		scsipt_override_ap = FALSE;
		DO_BEEP();
		return;
	}

	scsipt_override_ap = FALSE;

	if (s->mode == MOD_NODISC)
		s->mode = MOD_STOP;

	switch (s->mode) {
	case MOD_PLAY:
		/* Currently playing: go to pause mode */

		if (!scsipt_do_pause_resume(devfd, FALSE)) {
			DO_BEEP();
			return;
		}
		scsipt_stop_stat_poll();
		s->mode = MOD_PAUSE;
		DPY_PLAYMODE(s, FALSE);
		break;

	case MOD_PAUSE:
		/* Currently paused: resume play */

		if (!scsipt_do_pause_resume(devfd, TRUE)) {
			DO_BEEP();
			return;
		}
		s->mode = MOD_PLAY;
		DPY_PLAYMODE(s, FALSE);
		scsipt_start_stat_poll(s);
		break;

	case MOD_STOP:
		/* Currently stopped: start play */

		if (s->shuffle || s->program) {
			scsipt_new_progshuf = TRUE;

			/* Start shuffle/program play */
			if (!scsipt_run_prog(s))
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

			if (!scsipt_do_playaudio(devfd, ADDR_BLK | ADDR_MSF,
					  start_addr, s->tot_addr,
					  &start_msf, &end_msf, 0, 0)) {
				DO_BEEP();
				s->mode = MOD_STOP;
				return;
			}
		}

		DPY_ALL(s);
		scsipt_start_stat_poll(s);
		break;

	case MOD_A:
		/* Just reset mode to play and continue */
		s->mode = MOD_PLAY;
		DPY_PLAYMODE(s, FALSE);
		break;

	case MOD_AB:
	case MOD_SAMPLE:
		/* Force update of curstat */
		if (!scsipt_get_playstatus(s)) {
			DO_BEEP();
			return;
		}

		/* Currently doing a->b or sample playback: just resume play */
		if (s->shuffle || s->program) {
			if ((i = di_curtrk_pos(s)) < 0 ||
			    s->trkinfo[i].trkno == LEAD_OUT_TRACK)
				return;

			start_msf.min = s->cur_tot_min;
			start_msf.sec = s->cur_tot_sec;
			start_msf.frame = s->cur_tot_frame;
			end_msf.min = s->trkinfo[i+1].min;
			end_msf.sec = s->trkinfo[i+1].sec;
			end_msf.frame = s->trkinfo[i+1].frame;

			if (!scsipt_do_playaudio(devfd, ADDR_BLK | ADDR_MSF,
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

			if (!scsipt_do_playaudio(devfd, ADDR_BLK | ADDR_MSF,
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
 * scsipt_stop
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
scsipt_stop(curstat_t *s, bool_t stop_disc)
{
	/* The stop_disc parameter will cause the disc to spin down.
	 * This is usually set to TRUE, but can be FALSE if the caller
	 * just wants to set the current state to stop but will
	 * immediately go into play state again.  Not spinning down
	 * the drive makes things a little faster...
	 */
	if (!scsipt_disc_ready(s)) {
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

		if (stop_disc &&
		    !scsipt_do_start_stop(devfd, FALSE, FALSE, TRUE)) {
			DO_BEEP();
			return;
		}
		scsipt_stop_stat_poll();

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
 * scsipt_chgdisc
 *	Change disc function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_chgdisc(curstat_t *s)
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
		scsipt_mult_autoplay = TRUE;

	/* If we're currently paused, go to pause mode after disc change */
	scsipt_mult_pause = (s->mode == MOD_PAUSE);

	sav_rptcnt = s->rptcnt;

	/* Stop the CD first */
	scsipt_stop(s, TRUE);

	/* Unlock caddy if supported */
	if (app_data.caddylock_supp)
		scsipt_lock(s, FALSE);

	di_reset_curstat(s, TRUE, FALSE);
	s->mode = MOD_NODISC;
	DBCLEAR(s, FALSE);

	s->rptcnt = sav_rptcnt;

	/* Close CD-ROM device */
	DBGPRN(errfp, "\nClose CD-ROM: %s\n", s->curdev);
	pthru_close(devfd);
	devfd = -1;
	scsipt_not_open = TRUE;

	switch (app_data.chg_method) {
	case CHG_SCSI_LUN:
		/* SCSI LUN addressing method */

		/* Set new device */
		s->curdev = di_devlist[s->cur_disc - 1];

		/* Load desired disc */
		(void) scsipt_disc_ready(s);

		break;

	case CHG_SCSI_MEDCHG:
		/* SCSI medium changer method */

		/* Unload old disc */
		if (s->prev_disc > 0 && scsipt_chg_ready(s)) {
			(void) scsipt_move_medium(
				chgfd,
				scsipt_mcparm.mtbase,
				scsipt_mcparm.stbase,
				(word16_t) (scsipt_mcparm.dtbase +
					    s->prev_disc - 1)
			);
		}

		/* Load desired disc */
		(void) scsipt_disc_ready(s);

		break;

	default:
		/* Do nothing */
		break;
	}
}


/*
 * scsipt_prevtrk
 *	Previous track function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_prevtrk(curstat_t *s)
{
	sword32_t	i;
	word32_t	start_addr;
	msf_t		start_msf,
			end_msf;
	bool_t		go_prev;

	if (!scsipt_disc_ready(s)) {
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
				scsipt_new_progshuf = FALSE;
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
					scsipt_new_progshuf = FALSE;
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
			scsipt_mute_on(s);

		if (s->shuffle || s->program) {
			/* Program/Shuffle mode: just stop the playback
			 * and let scsipt_run_prog go to the previous track
			 */
			scsipt_fake_stop = TRUE;

			/* Force status update */
			(void) scsipt_get_playstatus(s);
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

			if (!scsipt_do_playaudio(devfd, ADDR_BLK | ADDR_MSF,
					  start_addr, s->tot_addr,
					  &start_msf, &end_msf, 0, 0)) {
				DO_BEEP();

				/* Restore volume */
				scsipt_mute_off(s);
				return;
			}

			if (s->mode == MOD_PAUSE) {
				(void) scsipt_do_pause_resume(devfd, FALSE);

				/* Restore volume */
				scsipt_mute_off(s);
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
 * scsipt_nexttrk
 *	Next track function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_nexttrk(curstat_t *s)
{
	sword32_t	i;
	word32_t	start_addr;
	msf_t		start_msf,
			end_msf;

	if (!scsipt_disc_ready(s)) {
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
					scsipt_new_progshuf = TRUE;
				else
					return;
			}

			if (s->mode == MOD_PAUSE)
				/* Mute: so we don't get a transient */
				scsipt_mute_on(s);

			/* Program/Shuffle mode: just stop the playback
			 * and let scsipt_run_prog go to the next track.
			 */
			scsipt_fake_stop = TRUE;

			/* Force status update */
			(void) scsipt_get_playstatus(s);

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
		    s->trkinfo[i].trkno != LEAD_OUT_TRACK) {

			start_addr = s->trkinfo[i].addr;
			start_msf.min = s->trkinfo[i].min;
			start_msf.sec = s->trkinfo[i].sec;
			start_msf.frame = s->trkinfo[i].frame;
			s->cur_trk = s->trkinfo[i].trkno;
			s->cur_idx = 1;

			if (s->mode == MOD_PAUSE)
				/* Mute: so we don't get a transient */
				scsipt_mute_on(s);

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

			if (!scsipt_do_playaudio(devfd, ADDR_BLK | ADDR_MSF,
					  start_addr, s->tot_addr,
					  &start_msf, &end_msf, 0, 0)) {
				DO_BEEP();
				return;
			}

			if (s->mode == MOD_PAUSE) {
				(void) scsipt_do_pause_resume(devfd, FALSE);

				/* Restore volume */
				scsipt_mute_off(s);
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

			if (i > 0 && s->trkinfo[i].trkno != LEAD_OUT_TRACK) {
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
 * scsipt_previdx
 *	Previous index function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_previdx(curstat_t *s)
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
		 * Since there is no standard SCSI-2 command to start
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
		scsipt_mute_on(s);

		if (!scsipt_do_playaudio(devfd, ADDR_TRKIDX, 0, 0, NULL, NULL,
				  (byte_t) s->cur_trk, idx)) {
			/* Restore volume */
			scsipt_mute_off(s);
			DO_BEEP();
			return;
		}

		scsipt_idx_pause = TRUE;

		if (!scsipt_do_pause_resume(devfd, FALSE)) {
			/* Restore volume */
			scsipt_mute_off(s);
			scsipt_idx_pause = FALSE;
			return;
		}

		/* Use scsipt_get_playstatus to update the current status */
		if (!scsipt_get_playstatus(s)) {
			/* Restore volume */
			scsipt_mute_off(s);
			scsipt_idx_pause = FALSE;
			return;
		}

		/* Save starting block addr of this index */
		s->sav_iaddr = s->cur_tot_addr;

		if (s->mode != MOD_PAUSE)
			/* Restore volume */
			scsipt_mute_off(s);

		start_msf.min = s->cur_tot_min;
		start_msf.sec = s->cur_tot_sec;
		start_msf.frame = s->cur_tot_frame;
		end_msf.min = s->tot_min;
		end_msf.sec = s->tot_sec;
		end_msf.frame = s->tot_frame;

		if (!scsipt_do_playaudio(devfd, ADDR_BLK | ADDR_MSF,
				  s->cur_tot_addr, s->tot_addr,
				  &start_msf, &end_msf, 0, 0)) {
			DO_BEEP();
			scsipt_idx_pause = FALSE;
			return;
		}

		scsipt_idx_pause = FALSE;

		if (s->mode == MOD_PAUSE) {
			(void) scsipt_do_pause_resume(devfd, FALSE);

			/* Restore volume */
			scsipt_mute_off(s);

			/* Force update of curstat */
			(void) scsipt_get_playstatus(s);
		}

		break;

	default:
		DO_BEEP();
		break;
	}
}


/*
 * scsipt_nextidx
 *	Next index function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_nextidx(curstat_t *s)
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
		 * Since there is no standard SCSI-2 command to start
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
		scsipt_mute_on(s);

		if (!scsipt_do_playaudio(devfd, ADDR_TRKIDX, 0, 0, NULL, NULL,
				  (byte_t) s->cur_trk,
				  (byte_t) (s->cur_idx + 1))) {
			/* Restore volume */
			scsipt_mute_off(s);
			DO_BEEP();
			return;
		}

		scsipt_idx_pause = TRUE;

		if (!scsipt_do_pause_resume(devfd, FALSE)) {
			/* Restore volume */
			scsipt_mute_off(s);
			scsipt_idx_pause = FALSE;
			return;
		}

		/* Use scsipt_get_playstatus to update the current status */
		if (!scsipt_get_playstatus(s)) {
			/* Restore volume */
			scsipt_mute_off(s);
			scsipt_idx_pause = FALSE;
			return;
		}

		/* Save starting block addr of this index */
		s->sav_iaddr = s->cur_tot_addr;

		if (s->mode != MOD_PAUSE)
			/* Restore volume */
			scsipt_mute_off(s);

		start_msf.min = s->cur_tot_min;
		start_msf.sec = s->cur_tot_sec;
		start_msf.frame = s->cur_tot_frame;
		end_msf.min = s->tot_min;
		end_msf.sec = s->tot_sec;
		end_msf.frame = s->tot_frame;

		if (!scsipt_do_playaudio(devfd, ADDR_BLK | ADDR_MSF,
				  s->cur_tot_addr, s->tot_addr,
				  &start_msf, &end_msf, 0, 0)) {
			DO_BEEP();
			scsipt_idx_pause = FALSE;
			return;
		}

		scsipt_idx_pause = FALSE;

		if (s->mode == MOD_PAUSE) {
			(void) scsipt_do_pause_resume(devfd, FALSE);

			/* Restore volume */
			scsipt_mute_off(s);

			/* Force update of curstat */
			(void) scsipt_get_playstatus(s);
		}

		break;

	default:
		DO_BEEP();
		break;
	}
}


/*
 * scsipt_rew
 *	Search-rewind function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_rew(curstat_t *s, bool_t start)
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
		scsipt_play_pause(s);

		/*FALLTHROUGH*/
	case MOD_PLAY:
	case MOD_PAUSE:
		if (start) {
			/* Button press */

			if (s->mode == MOD_PLAY)
				scsipt_stop_stat_poll();

			/* Reduce volume */
			vol = (byte_t) ((int) s->level *
				app_data.skip_vol / 100);

			(void) scsipt_cfg_vol((int)
				((vol < (byte_t)app_data.skip_minvol) ?
				 (byte_t) app_data.skip_minvol : vol),
				s,
				FALSE,
				FALSE
			);

			/* Start search rewind */
			scsipt_start_search = TRUE;
			scsipt_run_rew(s);
		}
		else {
			/* Button release */

			scsipt_stop_rew(s);

			/* Update display */
			(void) scsipt_get_playstatus(s);

			if (s->mode == MOD_PAUSE)
				/* Mute: so we don't get a transient */
				scsipt_mute_on(s);
			else
				/* Restore volume */
				scsipt_mute_off(s);

			if (s->shuffle || s->program) {
				if ((i = di_curtrk_pos(s)) < 0 ||
				    s->trkinfo[i].trkno == LEAD_OUT_TRACK) {
					/* Restore volume */
					scsipt_mute_off(s);
					return;
				}

				start_msf.min = s->cur_tot_min;
				start_msf.sec = s->cur_tot_sec;
				start_msf.frame = s->cur_tot_frame;
				end_msf.min = s->trkinfo[i+1].min;
				end_msf.sec = s->trkinfo[i+1].sec;
				end_msf.frame = s->trkinfo[i+1].frame;

				if (!scsipt_do_playaudio(devfd,
						ADDR_BLK | ADDR_MSF,
						s->cur_tot_addr,
						s->trkinfo[i+1].addr,
						&start_msf, &end_msf,
						0, 0)) {
					DO_BEEP();

					/* Restore volume */
					scsipt_mute_off(s);
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

				if (!scsipt_do_playaudio(devfd,
						ADDR_BLK | ADDR_MSF,
						s->cur_tot_addr,
						s->tot_addr,
						&start_msf, &end_msf,
						0, 0)) {
					DO_BEEP();

					/* Restore volume */
					scsipt_mute_off(s);
					return;
				}
			}

			if (s->mode == MOD_PAUSE) {
				(void) scsipt_do_pause_resume(devfd, FALSE);

				/* Restore volume */
				scsipt_mute_off(s);
			}
			else
				scsipt_start_stat_poll(s);
		}
		break;

	default:
		if (start)
			DO_BEEP();
		break;
	}
}


/*
 * scsipt_ff
 *	Search-fast-forward function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_ff(curstat_t *s, bool_t start)
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
		scsipt_play_pause(s);

		/*FALLTHROUGH*/
	case MOD_PLAY:
	case MOD_PAUSE:
		if (start) {
			/* Button press */

			if (s->mode == MOD_PLAY)
				scsipt_stop_stat_poll();

			/* Reduce volume */
			vol = (byte_t) ((int) s->level *
				app_data.skip_vol / 100);

			(void) scsipt_cfg_vol((int)
				((vol < (byte_t)app_data.skip_minvol) ?
				 (byte_t) app_data.skip_minvol : vol),
				s,
				FALSE,
				FALSE
			);

			/* Start search forward */
			scsipt_start_search = TRUE;
			scsipt_run_ff(s);
		}
		else {
			/* Button release */

			scsipt_stop_ff(s);

			/* Update display */
			(void) scsipt_get_playstatus(s);

			if (s->mode == MOD_PAUSE)
				/* Mute: so we don't get a transient */
				scsipt_mute_on(s);
			else
				/* Restore volume */
				scsipt_mute_off(s);

			if (s->shuffle || s->program) {
				if ((i = di_curtrk_pos(s)) < 0 ||
				    s->trkinfo[i].trkno == LEAD_OUT_TRACK) {
					/* Restore volume */
					scsipt_mute_off(s);
					return;
				}

				start_msf.min = s->cur_tot_min;
				start_msf.sec = s->cur_tot_sec;
				start_msf.frame = s->cur_tot_frame;
				end_msf.min = s->trkinfo[i+1].min;
				end_msf.sec = s->trkinfo[i+1].sec;
				end_msf.frame = s->trkinfo[i+1].frame;

				if (!scsipt_do_playaudio(devfd,
						ADDR_BLK | ADDR_MSF,
						s->cur_tot_addr,
						s->trkinfo[i+1].addr,
						&start_msf, &end_msf,
						0, 0)) {
					DO_BEEP();

					/* Restore volume */
					scsipt_mute_off(s);
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

				if (!scsipt_do_playaudio(devfd,
						ADDR_BLK | ADDR_MSF,
						s->cur_tot_addr,
						s->tot_addr,
						&start_msf, &end_msf,
						0, 0)) {
					DO_BEEP();

					/* Restore volume */
					scsipt_mute_off(s);
					return;
				}
			}
			if (s->mode == MOD_PAUSE) {
				(void) scsipt_do_pause_resume(devfd, FALSE);

				/* Restore volume */
				scsipt_mute_off(s);
			}
			else
				scsipt_start_stat_poll(s);
		}
		break;

	default:
		if (start)
			DO_BEEP();
		break;
	}
}


/*
 * scsipt_warp
 *	Track warp function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_warp(curstat_t *s)
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
		scsipt_play_pause(s);

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
			if (!scsipt_do_start_stop(devfd, FALSE, FALSE, TRUE))
				DO_BEEP();
		}
		else {
			if (s->mode == MOD_PAUSE)
				/* Mute: so we don't get a transient */
				scsipt_mute_on(s);

			if (!scsipt_do_playaudio(devfd,
						 ADDR_BLK | ADDR_MSF,
						 start_addr, end_addr,
						 &start_msf, &end_msf,
						 0, 0)) {
				DO_BEEP();

				/* Restore volume */
				scsipt_mute_off(s);
				return;
			}

			if (s->mode == MOD_PAUSE) {
				(void) scsipt_do_pause_resume(devfd, FALSE);

				/* Restore volume */
				scsipt_mute_off(s);
			}
		}
		break;

	default:
		break;
	}
}


/*
 * scsipt_route
 *	Channel routing function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_route(curstat_t *s)
{
	byte_t	val0,
		val1;

	if (!app_data.chroute_supp)
		return;

	if (scsipt_vutbl[app_data.vendor_code].route != NULL) {
		(void) scsipt_vutbl[app_data.vendor_code].route(s);
		return;
	}

	val0 = scsipt_route_val(app_data.ch_route, 0);
	val1 = scsipt_route_val(app_data.ch_route, 1);

	if (val0 == scsipt_route_left && val1 == scsipt_route_right)
		/* No change: just return */
		return;

	scsipt_route_left = val0;
	scsipt_route_right = val1;

	/* With SCSI-2, channel routing is done with the volume control */
	(void) scsipt_cfg_vol(s->level, s, FALSE, TRUE);
}


/*
 * scsipt_mute_on
 *	Mute audio function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_mute_on(curstat_t *s)
{
	(void) scsipt_cfg_vol(0, s, FALSE, FALSE);
}


/*
 * scsipt_mute_off
 *	Un-mute audio function
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_mute_off(curstat_t *s)
{
	(void) scsipt_cfg_vol((int) s->level, s, FALSE, FALSE);
}


/*
 * scsipt_start
 *	Start the SCSI pass-through module.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_start(curstat_t *s)
{
#ifdef DEMO_ONLY
	/* Start CD-simulator in demo mode */
	if (!pthru_cdsim_start()) {
		DI_FATAL("Cannot start CD simulator.\n");
		return;
	}
#endif

	/* Check to see if disc is ready */
	(void) scsipt_disc_ready(s);

	/* Update display */
	DPY_ALL(s);
}


/*
 * scsipt_icon
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
scsipt_icon(curstat_t *s, bool_t iconified)
{
	/* This function attempts to reduce the status polling frequency
	 * when possible to cut down on CPU and SCSI bus usage.  This is
	 * done when the CD player is iconified.
	 */

	/* Increase status polling interval by 4 seconds when iconified */
	if (iconified)
		scsipt_stat_interval = app_data.stat_interval + 4000;
	else
		scsipt_stat_interval = app_data.stat_interval;

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
		scsipt_stat_interval = app_data.stat_interval;
		break;

	case MOD_PLAY:
		if (!iconified) {
			/* Force an immediate update */
			scsipt_stop_stat_poll();
			scsipt_start_stat_poll(s);
		}
		break;

	default:
		break;
	}
}


/*
 * scsipt_halt
 *	Shut down the SCSI pass-through and vendor-unique modules.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing.
 */
void
scsipt_halt(curstat_t *s)
{
	int	i;

	/* Re-enable front-panel eject button */
	if (app_data.caddylock_supp)
		scsipt_lock(s, FALSE);

	if (s->mode != MOD_BUSY && s->mode != MOD_NODISC) {
		if (app_data.exit_eject && app_data.eject_supp) {
			/* User closing application: Eject disc */
			(void) scsipt_do_start_stop(devfd, FALSE, TRUE, TRUE);
		}
		else {
			if (app_data.exit_stop)
				/* User closing application: Stop disc */
				(void) scsipt_do_start_stop(devfd, FALSE,
							    FALSE, TRUE);

			switch (s->mode) {
			case MOD_PLAY:
			case MOD_PAUSE:
			case MOD_A:
			case MOD_AB:
			case MOD_SAMPLE:
				scsipt_stop_stat_poll();
				break;
			}
		}
	}

	/* Shut down the vendor unique modules */
	for (i = 0; i < MAX_VENDORS; i++) {
		if (scsipt_vutbl[i].halt != NULL)
			scsipt_vutbl[i].halt();
	}

	/* Close CD-ROM device */
	DBGPRN(errfp, "\nClose CD-ROM: %s\n", s->curdev);
	pthru_close(devfd);
	devfd = -1;

	/* Shut down multi-disc changer */
	if (app_data.numdiscs > 1)
		scsipt_chg_halt(s);

#ifdef DEMO_ONLY
	/* Stop CD-simulator in demo mode */
	pthru_cdsim_stop();
#endif
}


/*
 * scsipt_mode
 *	Return a text string indicating the current SCSI mode
 *	("SCSI-2" or a particular vendor-unique string).
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	SCSI mode text string.
 */
char *
scsipt_mode(void)
{
	static char	str[STR_BUF_SZ];

	(void) sprintf(str, "%s%s", scsipt_vutbl[app_data.vendor_code].vendor,
		       app_data.vendor_code == VENDOR_SCSI2 ?
			   "" : " vendor unique");

	return (str);
}


/*
 * scsipt_vers
 *	Return a text string indicating the SCSI pass-through module's
 *	version number and which SCSI-1 vendor-unique modes are
 *	supported in this binary.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Version text string.
 */
char *
scsipt_vers(void)
{
	static char	vers[STR_BUF_SZ * 4];

	(void) sprintf(vers, "SCSI Pass-through method\n%s", pthru_vers());
	return (vers);
}

#endif	/* DI_SCSIPT */

