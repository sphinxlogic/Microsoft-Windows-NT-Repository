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
 *   IBM AIX version 3.2.x and 4.x support
 *
 *   Contributing author: Kurt Brunton
 *   E-Mail: kbrunton@ccd.harris.com
 *
 *   Contributing author: Tom Crawley
 *   E-Mail: tomc@osi.curtin.edu.au
 *
 *   This software fragment contains code that interfaces the
 *   application to the IBM AIX operating system.  The name "IBM" is
 *   used here for identification purposes only.
 */
#ifndef LINT
static char *_os_aix_c_ident_ = "@(#)os_aix.c	6.28 98/05/10";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#if defined(_AIX) && defined(DI_SCSIPT) && !defined(DEMO_ONLY)

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
	struct sc_iocmd	sc_cmd;
	int		kstat;
	char		*path,
			title[FILE_PATH_SZ + 20];

	if (fd < 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	(void) memset(&sc_cmd, 0, sizeof(sc_cmd));

	/* set up sc_iocmd */
	(void) memcpy(sc_cmd.scsi_cdb, cmdpt, cmdlen);
	sc_cmd.command_length = cmdlen;
	sc_cmd.buffer = (caddr_t) datapt;
	sc_cmd.data_length = (int) datalen;
	sc_cmd.timeout_value = tmout ? tmout : DFLT_CMD_TIMEOUT;
#ifdef AIX_ENABLE_ASYNC
	/* This enables synchronous negotiation mode.  Some CD-ROM drives
	 * don't handle this well.
	 */
	sc_cmd.flags = SC_ASYNC; 
#else
	sc_cmd.flags = 0; 
#endif
	if (datalen != 0) {
		switch (rw) {
		case OP_DATAIN:
			sc_cmd.flags |= B_READ;
			break;
		case OP_DATAOUT:
			sc_cmd.flags |= B_WRITE;
			break;
		case OP_NODATA:
		default:
			break;
		}
	}

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, cmdpt, cmdlen);
	}

	/* Send the command down via the "pass-through" interface */
	if ((kstat = ioctl(fd, CDIOCMD, &sc_cmd)) < 0) {
		if (app_data.scsierr_msg && prnerr)
			perror("CDIOCMD ioctl failed");

		return FALSE;
	}

	if ((sc_cmd.status_validity & 0x01) != 0) {
		if (sc_cmd.scsi_bus_status != SC_GOOD_STATUS) {
			if (app_data.scsierr_msg && prnerr) {
				(void) fprintf(errfp,
					"%s: %s %s:\n%s=0x%x %s=0x%x\n",
					APPNAME,
					"SCSI bus status error on",
					path,
					"Opcode",
					cmdpt[0],
					"Status",
					sc_cmd.scsi_bus_status);
			}
		}

		return FALSE;
	}
	
	if ((sc_cmd.status_validity & 0x02) != 0) {
		if (sc_cmd.adapter_status != SC_GOOD_STATUS) {
			if (app_data.scsierr_msg && prnerr) {
				(void) fprintf(errfp,
					"%s: %s %s:\n%s=0x%x %s=0x%x\n",
					APPNAME,
					"adapter status error on",
					di_devgetpath(fd),
					"Opcode",
					cmdpt[0],
					"Status",
					sc_cmd.adapter_status);
			}
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

	if ((fd = openx(path, O_RDONLY, NULL, SC_DIAGNOSTIC)) < 0) {
		DBGPRN(errfp, "Cannot open %s: errno=%d\n", path, errno);
		return -1;
	}

	(void) di_devreg(fd, path);

	/* AIX hack:  The CD-ROM driver allows the open to succeed
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
	return ("OS Interface module for IBM AIX 3.2.x and 4.x\n");
}

#endif	/* _AIX DI_SCSIPT DEMO_ONLY */

