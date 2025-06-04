/*
 *   libdi - scsipt SCSI Device Interface Library
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
 *   Digital UNIX (OSF/1) and Ultrix support
 *
 *   Contributing author: Matt Thomas
 *   E-Mail: thomas@lkg.dec.com
 *
 *   Contributing author: Gary Field
 *   E-Mail: gfield@zk3.dec.com
 *
 *   This software fragment contains code that interfaces the
 *   application to the Digital UNIX and Ultrix operating systems.
 *   The term Digital, Ultrix and OSF/1 are used here for identification
 *   purposes only.
 */
#ifndef LINT
static char *_os_dec_c_ident_ = "@(#)os_dec.c	6.33 98/05/10";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#if ((defined(__alpha) && defined(__osf__)) || \
     defined(ultrix) || defined(__ultrix)) && \
    defined(DI_SCSIPT) && !defined(DEMO_ONLY)

extern appdata_t	app_data;
extern bool_t		scsipt_notrom_error;
extern FILE		*errfp;
extern di_client_t	*di_clinfo;

STATIC int		bus = -1,
			target = -1,
			lun = -1;
STATIC req_sense_data_t sense_data;	/* Request sense data buffer */


/*
 * pthru_send
 *	Send SCSI command to the device.
 *
 * Args:
 *	fd - Device file descriptor
 *	cmdpt - Pointer to the SCSI command CDB
 *	cmdlen - SCSI command size (6, 10 or 12 bytes)
 *	datapt - Pointer to the data buffer
 *	datalen - Data transfer size (bytes)
 *	rw - Data transfer direction flag (OP_NODATA, OP_DATAIN or OP_DATAOUT)
 *	tmout - Command timeout interval (seconds)
 *	prnerr - Whether an error message should be displayed
 *		 when a command fails
 *
 * Return:
 *	TRUE - command completed successfully
 *	FALSE - command failed
 */
bool_t
pthru_send(
	int		fd,
	byte_t		*cmdpt,
	int		cmdlen,
	byte_t		*datapt,
	int		datalen,
	byte_t		rw,
	int		tmout,
	bool_t		prnerr
)
{
	UAGT_CAM_CCB	uagt;
	CCB_SCSIIO	ccb;
	char		*path,
			title[FILE_PATH_SZ + 20];

	if (fd < 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	(void) memset(&uagt, 0, sizeof(uagt));
	(void) memset(&ccb, 0, sizeof(ccb));
	(void) memset(&sense_data, 0, sizeof(sense_data));

	/* Setup the user agent struct */
	uagt.uagt_ccb = (CCB_HEADER *) &ccb;
	uagt.uagt_ccblen = sizeof(CCB_SCSIIO);
#if defined(__alpha) && defined(__osf__)
	uagt.uagt_snsbuf = ccb.cam_sense_ptr = (u_char *) &sense_data;
	uagt.uagt_snslen = ccb.cam_sense_len = sizeof(sense_data);
#endif

	/* Setup the CAM ccb */
	(void) memcpy((byte_t *) ccb.cam_cdb_io.cam_cdb_bytes, cmdpt, cmdlen);
	ccb.cam_cdb_len = cmdlen;
	ccb.cam_ch.my_addr = (CCB_HEADER *) &ccb;
	ccb.cam_ch.cam_ccb_len = sizeof(CCB_SCSIIO);
	ccb.cam_ch.cam_func_code = XPT_SCSI_IO;

	if (datapt != NULL && datalen > 0) {
		switch (rw) {
		case OP_DATAIN:
			ccb.cam_ch.cam_flags |= CAM_DIR_IN;
			break;
		case OP_DATAOUT:
			ccb.cam_ch.cam_flags |= CAM_DIR_OUT;
			break;
		case OP_NODATA:
		default:
			break;
		}
		uagt.uagt_buffer = (u_char *) datapt;
		uagt.uagt_buflen = datalen;
	}
	else
		ccb.cam_ch.cam_flags |= CAM_DIR_NONE;

#if !defined(__alpha) || !defined(__osf__)
	ccb.cam_ch.cam_flags |= CAM_DIS_AUTOSENSE;
#endif

	ccb.cam_data_ptr = uagt.uagt_buffer;
	ccb.cam_dxfer_len = uagt.uagt_buflen;
	ccb.cam_timeout = tmout ? tmout : DFLT_CMD_TIMEOUT;

	ccb.cam_ch.cam_path_id = bus;
	ccb.cam_ch.cam_target_id = target;
	ccb.cam_ch.cam_target_lun = lun;
    
	/* Set an invalid status so we can be sure it changed */
	ccb.cam_scsi_status = 0xff;

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, cmdpt, ccb.cam_cdb_len);
	}

	if (ioctl(fd, UAGT_CAM_IO, (caddr_t) &uagt) < 0) {
		if (app_data.scsierr_msg && prnerr)
			perror("UAGT_CAM_IO ioctl failed");

		return FALSE;
    	}

	/* Check return status */
	if ((ccb.cam_ch.cam_status & CAM_STATUS_MASK) != CAM_REQ_CMP) {
		if (app_data.scsierr_msg && prnerr) {
			(void) fprintf(errfp,
				       "%s: %s %s:\n%s=0x%x %s=0x%x",
				       APPNAME,
				       "SCSI command fault on",
				       path,
				       "Opcode",
				       cmdpt[0],
				       "Status",
				       ccb.cam_scsi_status);
#if defined(__alpha) && defined(__osf__)
			if (sense_data.key != 0)
				(void) fprintf(errfp,
					" Key=0x%x Code=0x%x Qual=0x%x\n",
					sense_data.key,
					sense_data.code,
					sense_data.qual);
			else
#endif
				(void) fprintf(errfp, "\n");
		}

		if (ccb.cam_ch.cam_status & CAM_SIM_QFRZN) {
			(void) memset(&ccb, 0, sizeof(ccb));
			(void) memset(&uagt, 0, sizeof(uagt));

			/* Setup the user agent struct */
			uagt.uagt_ccb = (CCB_HEADER  *) &ccb;
			uagt.uagt_ccblen = sizeof(CCB_RELSIM);

			/* Setup the CAM ccb */
			ccb.cam_ch.my_addr = (struct ccb_header *) &ccb;
			ccb.cam_ch.cam_ccb_len = sizeof(CCB_RELSIM);
			ccb.cam_ch.cam_func_code = XPT_REL_SIMQ;

			ccb.cam_ch.cam_path_id = bus;
			ccb.cam_ch.cam_target_id = target;
			ccb.cam_ch.cam_target_lun = lun;

			if (ioctl(fd, UAGT_CAM_IO, (caddr_t) &uagt) < 0)
				return FALSE;
		}

		return FALSE;
	}

	return TRUE;
}


/*
 * pthru_open
 *	Open SCSI pass-through device
 *
 * Args:
 *	path - device path name string
 *
 * Return:
 *	Device file descriptor, or -1 on failure.
 */
int
pthru_open(char *path)
{
	int		fd;
	struct stat	stbuf;
	char		errstr[ERR_BUF_SZ];
	int		saverr;

	/* Check for validity of device node */
	if (stat(path, &stbuf) < 0) {
		(void) sprintf(errstr, app_data.str_staterr, path);
		DI_FATAL(errstr);
		return -1;
	}
	if (!S_ISCHR(stbuf.st_mode)) {
		(void) sprintf(errstr, app_data.str_noderr, path);
		DI_FATAL(errstr);
		return -1;
	}

	if ((fd = open(path, O_RDONLY | O_NDELAY, 0)) >= 0) {
		struct devget	devget;

		if (ioctl(fd, DEVIOCGET, &devget) >= 0) {
#ifdef __osf__
			lun = devget.slave_num % 8;
			devget.slave_num /= 8;
#else
			lun = 0;
#endif
			target = devget.slave_num % 8;
			devget.slave_num /= 8;
			bus = devget.slave_num % 8;
			(void) close(fd);

			if ((fd = open(DEV_CAM, O_RDWR, 0)) >= 0 ||
			    (fd = open(DEV_CAM, O_RDONLY, 0)) >= 0) {

				(void) di_devreg(fd, path);

				/* Hack:  The CAM driver allows
				 * the open to succeed even if there
				 * is no CD loaded.  We test for the
				 * existence of a disc with
				 * scsipt_disc_present().
				 */
				if (!scsipt_disc_present(fd)) {
					/* No CD loaded */
					pthru_close(fd);
					return -1;
				}

				return (fd);
			}

			DBGPRN(errfp, "Cannot open %s: errno=%d\n",
			       DEV_CAM, errno);
		}
		else {
			(void) close(fd);

			DBGPRN(errfp,
			       "DEVIOCGET ioctl failed on %s: errno=%d\n",
			       path, errno);
		}
	}
	else {
		saverr = errno;
		(void) sprintf(errstr, "Cannot open %s: errno=%d",
			       path, saverr);

		if (saverr != EIO)
			DI_FATAL(errstr);

		DBGPRN(errfp, "%s\n", errstr);
	}

	fd = bus = target = lun = -1;
	return -1;
}


/*
 * pthru_close
 *	Close SCSI pass-through device
 *
 * Args:
 *	fd - Device file descriptor
 *
 * Return:
 *	Nothing.
 */
void
pthru_close(int fd)
{
	di_devunreg(fd);
	(void) close(fd);
	bus = target = lun = -1;
}


/*
 * pthru_vers
 *	Return OS Interface Module version string
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Module version text string.
 */
char *
pthru_vers(void)
{
	return ("OS Interface module for Digital UNIX (OSF/1) & Ultrix\n");
}

#endif	/* __alpha __osf__ ultrix __ultrix DI_SCSIPT DEMO_ONLY */

