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
 *   HP-UX support
 *
 *   Author: Ti Kan
 *   Contributing author (m68k portion): Avi Cohen Stuart
 *   E-Mail: <avi@baan.nl>
 *
 *   This software fragment contains code that interfaces the
 *   application to the HP-UX Release 9.x and 10.x operating system.
 *   The name "HP" and "hpux" are used here for identification purposes
 *   only.
 */
#ifndef LINT
static char *_os_hpux_c_ident_ = "@(#)os_hpux.c	6.33 98/05/10";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#if defined(__hpux) && defined(DI_SCSIPT) && !defined(DEMO_ONLY)

extern appdata_t	app_data;
extern bool_t		scsipt_notrom_error;
extern FILE		*errfp;
extern di_client_t	*di_clinfo;

STATIC struct utsname	utsn;


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
#ifdef __hp9000s300
	/* m68k systems */
	struct scsi_cmd_parms	scp;
	int			n,
				blen;
	byte_t			ch = 0;
	char			*path,
				title[FILE_PATH_SZ + 20];

	if (fd < 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	(void) memset(&scp, 0, sizeof(scp));

	(void) memcpy(scp.command, cmdpt, cmdlen);
	scp.cmd_type = cmdlen;
	if (rw == OP_DATAIN && datalen > 0)
		scp.cmd_mode = SCTL_READ;
	else
		scp.cmd_mode = 0;
	scp.clock_ticks = (tmout ? tmout : DFLT_CMD_TIMEOUT) * 50;

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, cmdpt, cmdlen);
	}

	/* Set up for sending the SCSI command */
	if (ioctl(fd, SIOC_SET_CMD, &scp) < 0) {
		if (app_data.scsierr_msg && prnerr)
			perror("SIOC_SET_CMD ioctl failed");
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
			if (app_data.scsierr_msg && prnerr) {
				perror("data read failed");
				return FALSE;
			}
		}
		break;
	case OP_DATAOUT:
		n = write(fd, datapt, blen);
		if (n != blen && n != datalen) {
			if (app_data.scsierr_msg && prnerr) {
				perror("data write failed");
				return FALSE;
			}
		}
	default:
		break;
	}

	return TRUE;
#else
	/* PA-RISC Systems */
	struct sctl_io		sctl;
	char			*path,
				title[FILE_PATH_SZ + 20];
	
	if (fd < 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	(void) memset(&sctl, 0, sizeof(sctl));

	/* set up sctl_io */
	(void) memcpy(sctl.cdb, cmdpt, cmdlen);
	sctl.cdb_length = cmdlen;
	sctl.data = datapt;
	sctl.data_length = (unsigned) datalen;
	if (rw == OP_DATAIN && datalen > 0)
		sctl.flags = SCTL_READ;
	else
		sctl.flags = 0;
	sctl.max_msecs = (tmout ? tmout : DFLT_CMD_TIMEOUT) * 1000;

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, cmdpt, cmdlen);
	}

	/* Send the command down via the "pass-through" interface */
	if (ioctl(fd, SIOC_IO, &sctl) < 0) {
		if (app_data.scsierr_msg && prnerr)
			perror("SIOC_IO ioctl failed");
		return FALSE;
	}

	if (sctl.cdb_status != S_GOOD) {
		if (app_data.scsierr_msg && prnerr) {
			(void) fprintf(errfp,
				    "%s: %s %s:\n%s=0x%x %s=0x%x %s=0x%x",
				    APPNAME,
				    "SCSI command fault on",
				    path,
				    "Opcode",
				    cmdpt[0],
				    "Cdb_status",
				    sctl.cdb_status,
				    "Sense_status",
				    sctl.sense_status);

			if (sctl.sense_status == S_GOOD && sctl.sense_xfer > 2)
				(void) fprintf(errfp,
					    " Key=0x%x Code=0x%x Qual=0x%x\n",
					    sctl.sense[2] & 0x0f,
					    sctl.sense[12],
					    sctl.sense[13]);
			else
				(void) fprintf(errfp, "\n");
		}

		return FALSE;
	}

	return TRUE;
#endif	/* __hp9000s300 */
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

#ifdef __hp9000s300
	/* m68k systems */
	if ((fd = open(path, O_RDWR)) < 0) {
		DBGPRN(errfp, "Cannot open %s: errno=%d\n", path, errno);
		return -1;
	}

	/* Enable SCSI pass-through mode */
	{
		int	i = 1;

		if (ioctl(fd, SIOC_CMD_MODE, &i) < 0) {
			DBGPRN(errfp, "Cannot set SIOC_CMD_MODE: errno=%d\n",
				errno);
			(void) close(fd);
			return -1;
		}
	}
#else
	/* PA-RISC systems */
	if ((fd = open(path, O_RDONLY)) < 0) {
		DBGPRN(errfp, "Cannot open %s: errno=%d\n", path, errno);
		return -1;
	}

	/* Find out the machine type */
	if (uname(&utsn) < 0) {
		DBGPRN(errfp, "uname(2) failed (errno=%d): assume 9000/7xx\n",
			errno);
		/* Shrug: assume series 7xx */
		(void) strcpy(utsn.machine, "9000/7xx");
	}

	/* Obtain exclusive open (on Series 700 systems only) */
	if (utsn.machine[5] == '7' && ioctl(fd, SIOC_EXCLUSIVE, 1) < 0) {
		DBGPRN(errfp, "Cannot set SIOC_EXCLUSIVE: errno=%d\n", errno);
		(void) close(fd);
		return -1;
	}
#endif	/* __hp9000s300 */

	(void) di_devreg(fd, path);

	/* HP-UX hack:  The CD-ROM driver allows the open to succeed
	 * even if there is no CD loaded.  We test for the existence of
	 * a disc with scsipt_disc_present().
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
#ifndef __hp9000s300
	/* Relinquish exclusive open (on Series 700 systems only) */
	if (utsn.machine[5] == '7')
		(void) ioctl(fd, SIOC_EXCLUSIVE, 0);
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
	return ("OS Interface module for HP-UX\n");
}

#endif	/* __hpux DI_SCSIPT DEMO_ONLY */

