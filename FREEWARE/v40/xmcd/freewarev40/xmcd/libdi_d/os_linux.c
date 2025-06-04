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
 *   Linux support
 *
 *   This software fragment contains code that interfaces the
 *   application to the Linux operating system.
 */
#ifndef LINT
static char *_os_linux_c_ident_ = "@(#)os_linux.c	6.31 98/10/27";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#if defined(linux) && defined(DI_SCSIPT) && !defined(DEMO_ONLY)

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
	byte_t			*ptbuf;
	int			ptbufsz,
				ret;
	req_sense_data_t	*rp;
	char			*path,
				title[FILE_PATH_SZ + 20];

	if (fd < 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	/* Linux hack: use SCSI_IOCTL_TEST_UNIT_READY instead of
	 * SCSI_IOCTL_SEND_COMMAND for test unit ready commands.
	 */
	if (cmdpt[0] == OP_S_TEST) {
		DBGPRN(errfp, "\nSending SCSI_IOCTL_TEST_UNIT_READY (%s)\n",
			path);

		ret = ioctl(fd, SCSI_IOCTL_TEST_UNIT_READY, NULL);
		if (ret == 0)
			return TRUE;

		if (app_data.scsierr_msg && prnerr) {
			(void) fprintf(errfp,
				"SCSI_IOCTL_TEST_UNIT_READY failed: ret=0x%x\n",
				ret
			);
		}
		return FALSE;
	}

	/* Set up SCSI pass-through command/data buffer */
	ptbufsz = (sizeof(int) << 1) + cmdlen + sizeof(req_sense_data_t);
	if (datapt != NULL && datalen > 0)
		ptbufsz += datalen;

	if ((ptbuf = (byte_t *) MEM_ALLOC("ptbuf", ptbufsz)) == NULL) {
		DI_FATAL(app_data.str_nomemory);
		return FALSE;
	}

	(void) memset(ptbuf, 0, ptbufsz);
	(void) memcpy(ptbuf + (sizeof(int) << 1), cmdpt, cmdlen);

	if (datapt != NULL && datalen > 0) {
		if (rw == OP_DATAOUT) {
			*((int *) ptbuf) = datalen;
			(void) memcpy(ptbuf + (sizeof(int) << 1) + cmdlen,
				      datapt, datalen);
		}
		else
			*(((int *) ptbuf) + 1) = datalen;
	}

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, cmdpt, cmdlen);
	}

	/* Send the command down via the "pass-through" interface */
	if ((ret = ioctl(fd, SCSI_IOCTL_SEND_COMMAND, ptbuf)) != 0) {
		if (app_data.scsierr_msg && prnerr) {
			(void) fprintf(errfp, "%s: %s %s:\n",
				APPNAME,
				"SCSI command error on", path);
			(void) fprintf(errfp,
				"%s=0x%x %s=0x%x %s=0x%x %s=0x%x %s=0x%x\n",
				"Opcode", cmdpt[0],
				"Status", status_byte(ret),
				"Msg", msg_byte(ret),
				"Host", host_byte(ret),
				"Driver", driver_byte(ret));

			rp = (req_sense_data_t *)(void *)
				(ptbuf + (sizeof(int) << 1));

			if (rp->key != 0) {
				(void) fprintf(errfp,
					"Key=0x%x Code=0x%x Qual=0x%x\n",
					rp->key, rp->code, rp->qual);
			}
		}

		MEM_FREE(ptbuf);
		return FALSE;
	}

	if (datapt != NULL && rw == OP_DATAIN && datalen > 0)
		(void) memcpy(datapt, ptbuf + (sizeof(int) << 1), datalen);

	MEM_FREE(ptbuf);
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
	int		fd,
			mode;
	struct stat	stbuf;
	char		errstr[ERR_BUF_SZ];
	bool_t		is_sg;

	/* Check for validity of device node */
	if (stat(path, &stbuf) < 0) {
		(void) sprintf(errstr, app_data.str_staterr, path);
		DI_FATAL(errstr);
		return -1;
	}

	/* Linux CD-ROM device is a block special file, but the SCSI generic
	 * device (for medium changer support) is a character special file.
	 */
	if (S_ISCHR(stbuf.st_mode)) {
		is_sg = TRUE;
		mode = O_RDWR | O_EXCL | O_NONBLOCK;
	}
	else if (S_ISBLK(stbuf.st_mode)) {
		is_sg = FALSE;
		mode = O_RDONLY | O_EXCL | O_NONBLOCK;
	}
	else {
		(void) sprintf(errstr, app_data.str_noderr, path);
		DI_FATAL(errstr);
		return -1;
	}

	if ((fd = open(path, mode)) < 0) {
		DBGPRN(errfp, "Cannot open %s: errno=%d\n", path, errno);
		return -1;
	}

	(void) di_devreg(fd, path);

	/* Linux hack:  The CD-ROM driver allows the open to succeed
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
	return ("OS Interface module for Linux\n");
}

#endif	/* linux DI_SCSIPT DEMO_ONLY */

