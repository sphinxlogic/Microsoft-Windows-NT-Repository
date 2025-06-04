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
 *   Siemens Pyramid SINIX (Reliant UNIX) support
 *
 *   Contributing author: Eckhard Einert
 *   E-Mail: einert.pad@sni.de
 *
 *   This software fragment contains code that interfaces the
 *   application to the SINIX (Reliant UNIX) operating system.
 *   The names "Siemens Pyramid", "SINIX" and "Reliant UNIX" are
 *   used here for identification purposes only.
 */
#ifndef LINT
static char *_os_sinix_c_ident_ = "@(#)os_sinix.c	6.22 98/05/10";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#if defined(SNI) && defined(DI_SCSIPT) && !defined(DEMO_ONLY)

extern appdata_t	app_data;
extern bool_t		scsipt_notrom_error;
extern FILE		*errfp;
extern di_client_t	*di_clinfo;


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
/*ARGSUSED*/
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
	struct scsictl	req;
	char		*path,
			title[FILE_PATH_SZ + 20];

	if (fd < 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	(void) memset(&req, 0, sizeof(req));

	/* Set up scsictl */
	(void) memcpy(&req.scsi0, cmdpt, cmdlen);

#ifdef SCSFCDBLEN	/* Reliant UNIX 5.43 */
	req.scs_cdblen = cmdlen;
	req.scs_timeout = tmout;
	req.scs_flags |= SCSFCDBLEN;
	if (tmout != 0)
		req.scs_flags |= SCSFTIMEOUT;
#else
	req.scs_spare = 0;
#endif

	/* Note that SINIX's concept of data direction is opposite of rw */
	switch (rw) {
	case OP_DATAIN:
		req.scs_flags = SCSFDATAOUT;
		break;
	case OP_DATAOUT:
		req.scs_flags = SCSFDATAIN;
		break;
	case OP_NODATA:
	default:
		req.scs_flags = SCSFNODATA;
		break;
	}
	req.scs_addr = (caddr_t) datapt;
	req.scs_bytes = (ulong) datalen;

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, cmdpt, cmdlen);
	}

	/* Send the command down via the "pass-through" interface */
	if (ioctl(fd, IOSSCSICMD, &req) < 0) {
		if (app_data.scsierr_msg && prnerr)
			perror("IOSSCSICMD ioctl failed");
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

	if ((fd = open(path, O_RDONLY)) < 0) {
		DBGPRN(errfp, "Cannot open %s: errno=%d\n", path, errno);
		return -1;
	}

	(void) di_devreg(fd, path);
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
	di_devunreg(fd);
	(void) close(fd);
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
	return ("OS Interface module for Siemens Pyramid Reliant UNIX\n");
}

#endif	/* SNI DI_SCRIPT DEMO_ONLY */

