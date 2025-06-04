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
 *   Apple A/UX version 3.x support
 *
 *   Contributing author: Eric Rosen
 *
 *   This software fragment contains code that interfaces the
 *   application to the Apple A/UX operating system.  The name "Apple"
 *   is used here for identification purposes only.
 */
#ifndef LINT
static char *_os_aux_c_ident_ = "@(#)os_aux.c	6.22 98/05/10";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#if defined(macII) && defined(DI_SCSIPT) && !defined(DEMO_ONLY)

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
	struct userscsireq	sc_cmd;
	req_sense_data_t	sense_data;
	char			*path,
				title[FILE_PATH_SZ + 20];

	if (fd < 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	(void) memset(&sc_cmd, 0, sizeof(sc_cmd));
	(void) memset(&sense_data, 0, sizeof(sense_data));

	(void) memcpy(sc_cmd.cmdbuf, cmdpt, cmdlen);
	sc_cmd.cmdlen = cmdlen;
	sc_cmd.databuf = datapt;	/* data to transfer and */
	sc_cmd.datalen = datalen;	/* number of bytes to transfer */
	sc_cmd.datasent	= 0;
	sc_cmd.sensebuf	= (uchar *) &sense_data;	
	sc_cmd.senselen	= (uchar) SZ_RSENSE;
	if (datalen != 0) {
		switch (rw) {
		case OP_DATAIN:
			sc_cmd.flags |= SRQ_READ;
			break;
		case OP_DATAOUT:
		case OP_NODATA:
		default:
			break;
		}
	}
	sc_cmd.timeout = tmout ? tmout : DFLT_CMD_TIMEOUT;

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, cmdpt, cmdlen);
	}

	/* Send the command down via the "pass-through" interface */
	if (ioctl(fd, SCSISTART, (uchar *) &sc_cmd) < 0) {
		if (app_data.scsierr_msg && prnerr) 
			perror("SCSISTART ioctl failed");
		return FALSE;
	}

	if (sc_cmd.ret != SMG_COMP && sc_cmd.ret != SMG_LNKFLG) {
		if (app_data.scsierr_msg && prnerr) {
			(void) fprintf(errfp,
				       "%s: %s %s:\n%s=0x%x %s=0x%x",
				       APPNAME,
				       "SCSI bus status error on",
				       di_reggetpath(fd),
				       "Opcode",
				       cmdpt[0],
				       "Status",
				       sc_cmd.msg);

			if (sense_data.key == 0)
				(void) fprintf(errfp, "\n");
			else {
				(void) fprintf(errfp,
					" Key=0x%x Code=0x%x Qual=0x%x\n",
					sense_data.key,
					sense_data.code,
					sense_data.qual);
			}
		}
		return FALSE;
	}

	if (sc_cmd.stat != 0) {
		if (app_data.scsierr_msg && prnerr) {
			(void) fprintf(errfp,
				       "%s: %s %s:\n%s=0x%x %s=0x%x\n",
				       APPNAME,
				       "adapter status error on",
				       path,
				       "Opcode",
				       cmdpt[0],
				       "Status",
				       sc_cmd.stat);
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

	if ((fd = open(path, O_RDONLY, NULL)) < 0) {
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
	return ("OS Interface module for Apple A/UX 3.x\n");
}

#endif	/* macII DI_SCSIPT DEMO_ONLY */

