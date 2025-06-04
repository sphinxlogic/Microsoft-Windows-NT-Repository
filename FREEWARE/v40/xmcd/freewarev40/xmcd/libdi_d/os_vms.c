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
 *   Digital OpenVMS support
 *
 *   Contributing author: Rick Jones
 *   E-Mail: rjones@zko.dec.com
 *
 *   This software fragment contains code that interfaces the
 *   application to the Digital OpenVMS operating system.
 *   The terms Digital and OpenVMS are used here for identification
 *   purposes only.
 */
#ifndef LINT
static char *_os_vms_c_ident_ = "@(#)os_vms.c	6.29 98/05/31";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#if defined(__VMS) && defined(DI_SCSIPT) && !defined(DEMO_ONLY)

extern appdata_t	app_data;
extern bool_t		scsipt_notrom_error;
extern FILE		*errfp;

STATIC int		context;	
STATIC char		*str;


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
	int		ret,
			pthru_iosb[2],		/* Pass-through iosb */
			pthru_desc[15];		/* Pass-through command block */
	char 		scsi_status,
			*path,
			title[FILE_PATH_SZ + 20];

	if (fd <= 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	/* Build command desc. for driver call */
	pthru_desc[0] = 1;		/* Opcode */
	pthru_desc[1] = (rw == OP_DATAOUT ? 0 : 1) | FLAGS_DISCONNECT;
					/* Flags */
	pthru_desc[2] = (int) cmdpt;	/* Command address */
	pthru_desc[3] = cmdlen;

	pthru_desc[4] = (int) datapt;	/* Data address */
	pthru_desc[5] = datalen;	/* Data length */
	pthru_desc[6] = 0;		/* Pad length */
	pthru_desc[7] = 0;		/* Phase timeout */
	pthru_desc[8] = tmout ? tmout : DFLT_CMD_TIMEOUT;
					/* Disconnect timeout */
	pthru_desc[9] = 0;
	pthru_desc[10] = 0;
	pthru_desc[11] = 0;
	pthru_desc[12] = 0;
	pthru_desc[13] = 0;
	pthru_desc[14] = 0;

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, cmdpt, cmdlen);
	}

	/* Send the command to the driver */
	ret = sys$qiow(1, fd, IO$_DIAGNOSE, pthru_iosb, 0, 0,
		       &pthru_desc[0], 15*4, 0, 0, 0, 0);

	if (!(ret & 1))
		sys$exit(ret);

	if (!(pthru_iosb[0] & 1))
		sys$exit(pthru_iosb[0] & 0xffff);

	scsi_status = (pthru_iosb[1] >> 24) & SCSI_STATUS_MASK;

	if (scsi_status != GOOD_SCSI_STATUS) {
		if (app_data.scsierr_msg && prnerr) {
			(void) fprintf(errfp,
				       "%s: %s %s:\n%s=0x%x %s=0x%x\n",
				       APPNAME,
				       "SCSI command fault on",
				       path,
				       "Opcode",
				       cmdpt[0],
				       "Status",
				       scsi_status);
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
	int	fd,
		status,
		pthru_desc[2];	/* Pass-through string desc */

	if (path == NULL) {
		(void) fprintf(errfp, "CD Audio: CD-ROM Device not defined.\n");
		sys$exit(SS$_NODEVAVL);
	}

	pthru_desc[0] = strlen(path);
	pthru_desc[1] = (int) path;

	status = sys$assign(&pthru_desc[0], &fd, 0, 0);

	if (!(status & 1)) {
		(void) fprintf(errfp,
			       "CD Audio: Error assigning to device %s\n",
			       path);
		sys$exit(status);
	}

	(void) di_devreg(fd, path);

	/* Hack:  The driver allows the assign to succeed even if
	 * there is no CD loaded.  We test for the existence of a disc
	 * with scsipt_disc_present().
	 */
	if (!scsipt_disc_present(fd)) {
		/* No CD loaded */
		pthru_close(fd);
		return -1;
	}

	return (fd);
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
	int	ret;

	di_devunreg(fd);
	ret = sys$dassgn(fd);
	if (!(ret & 1)) {
		DBGPRN(errfp,
			"CD Audio: Cannot deassign channel from device\n");
	}
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
	return ("OS Interface module for Digital OpenVMS\n");
}

#endif	/* __VMS DI_SCSIPT DEMO_ONLY */

