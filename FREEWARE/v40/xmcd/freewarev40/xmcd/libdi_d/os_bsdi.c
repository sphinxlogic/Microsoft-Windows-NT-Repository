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
 *   BSDI BSD/OS support
 *
 *   Contributing author for BSD/OS 2.x: Danny Braniss
 *   E-Mail: danny@cs.huji.ac.il
 *
 *   Contributing author for BSD/OS 3.x: Chris P. Ross
 *   E-Mail: cross@va.pubnix.com
 *
 *   This software fragment contains code that interfaces the
 *   application to the BSDI BSD/OS (version 2.0 or later)
 *   operating system.
 */
#ifndef LINT
static char *_os_bsdi_c_ident_ = "@(#)os_bsdi.c	6.32 98/09/27";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#if defined(__bsdi__) && defined(DI_SCSIPT) && !defined(DEMO_ONLY)

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
#ifdef OS_BSDI_2
	/* BSD/OS 2.x */
	int		blen,
			n;
	byte_t		ch = 0;
	char		*path,
			title[FILE_PATH_SZ + 20];

	if (fd < 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, cmdpt, cmdlen);
	}

	/* Make sure that this command is supported by the driver */
	n = (int) cmdpt[0];
	if (ioctl(fd, SDIOCADDCOMMAND, &n) < 0) {
		if (app_data.scsierr_msg && prnerr)
			perror("SDIOCADDCOMMAND ioctl failed");
		return FALSE;
	}

	/* Send the command down via the "pass-through" interface */
	if (ioctl(fd, SDIOCSCSICOMMAND, &cmdpt) < 0) {
		if (app_data.scsierr_msg && prnerr)
			perror("SDIOCSCSICOMMAND ioctl failed");
		return FALSE;
	}

	if (datalen == 0) {
		blen = 1;
		datapt = &ch;
	}
	else {
		blen = datalen;
	}

	switch (rw) {
	case OP_NODATA:
	case OP_DATAIN:
		n = read(fd, datapt, blen);
		if (n != blen && n != datalen) {
			if (app_data.scsierr_msg && prnerr)
				perror("data read failed");
			return FALSE;
		}
		break;
	case OP_DATAOUT:
		n = write(fd, datapt, blen);
		if (n != blen && n != datalen) {
			if (app_data.scsierr_msg && prnerr)
				perror("data write failed");
			return FALSE;
		}
		break;
	default:
		break;
	}

	return TRUE;
#endif	/* OS_BSDI_2 */

#ifdef OS_BSDI_3
	/* BSD/OS 3.x */
	scsi_user_cdb_t	suc;
	char		*path,
			title[FILE_PATH_SZ + 20];

	if (fd < 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, cmdpt, cmdlen);
	}

	(void) memset(&suc, 0, sizeof(scsi_user_cdb_t));
	(void) memcpy(suc.suc_cdb, cmdpt, cmdlen);
	suc.suc_cdblen = cmdlen;
	suc.suc_datalen = datalen;
	suc.suc_data = (caddr_t) datapt;

	switch (rw) {
	case OP_NODATA:
	case OP_DATAIN:
		suc.suc_flags = SUC_READ;
		break;
	case OP_DATAOUT:
		suc.suc_flags = SUC_WRITE;
		break;
	default:
		break;
	}

	/* Send the command down via the "pass-through" interface */
	if (ioctl(fd, SCSIRAWCDB, &suc) < 0) {
		if (app_data.scsierr_msg && prnerr)
			perror("SCSIRAWCDB ioctl failed");
		return FALSE;
	}

	if (suc.suc_sus.sus_status != 0) {
		if (app_data.scsierr_msg && prnerr) {
			(void) fprintf(errfp,
					"%s: %s %s:\n%s=0x%02x %s=0x%02x",
					APPNAME,
					"SCSI command fault on",
					path,
					"Opcode",
					cmdpt[0],
					"Status",
					suc.suc_sus.sus_status);
		}
		return FALSE;
	}

	return TRUE;
#endif	/* OS_BSDI_3 */
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
	int		val;

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

	if ((fd = open(path, O_RDWR | O_NONBLOCK)) < 0 &&
	    (fd = open(path, O_RDONLY | O_NONBLOCK)) < 0) {
		DBGPRN(errfp, "Cannot open %s: errno=%d\n", path, errno);
		return -1;
	}

#ifdef OS_BSDI_2
	/* BSD/OS 2.x */

	/* Query if the driver is already in "format mode" */
	val = 0;
	if (ioctl(fd, SDIOCGFORMAT, &val) < 0) {
		perror("SDIOCGFORMAT ioctl failed");
		return -1;
	}

	if (val != 0) {
		DBGPRN(errfp, "Device %s is in use by process %d\n",
			path, val);
		return -1;
	}

	/* Put the driver in "format mode" for SCSI pass-through */
	val = 1;
	if (ioctl(fd, SDIOCSFORMAT, &val) < 0) {
		perror("SDIOCSFORMAT ioctl failed");
		return -1;
	}
#endif

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
#ifdef OS_BSDI_2
	/* BSD/OS 2.x */
	int	val = 0;

	/* Put the driver back to normal mode */
	(void) ioctl(fd, SDIOCSFORMAT, &val);
#endif

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
#ifdef OS_BSDI_2
	return ("OS Interface module for BSDI BSD/OS 2.x\n");
#endif
#ifdef OS_BSDI_3
	return ("OS Interface module for BSDI BSD/OS 3.x\n");
#endif
}

#endif	/* __bsdi__ DI_SCSIPT DEMO_ONLY */

