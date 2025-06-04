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
#ifndef LINT
static char *_os_svr4_c_ident_ = "@(#)os_svr4.c	6.36 98/10/27";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#if (defined(SVR4) || defined(SVR5)) && \
    !defined(sun) && !defined(__sun__) && \
    defined(DI_SCSIPT) && !defined(DEMO_ONLY)

extern appdata_t	app_data;
extern bool_t		scsipt_notrom_error;
extern FILE		*errfp;
extern di_client_t	*di_clinfo;


#if defined(i386) || defined(__i386__) || (defined(_FTX) && defined(__hppa))
/*
 *   UNIX SVR4.x/x86, SVR5/x86 support
 *   Stratus UNIX SVR4/PA-RISC FTX 3.x support
 *   Portable Device Interface (PDI) / Storage Device Interface (SDI)
 *
 *   This software fragment contains code that interfaces the
 *   application to the UNIX System V Release 4.x (AT&T, USL,
 *   Univel/Novell/SCO UnixWare) and UNIX System V Release 5
 *   (SCO UnixWare 7) operating systems for the Intel x86 hardware
 *   platforms and Stratus FTX 3.x on the Continuum systems.
 *   The company and product names used here are for identification
 *   purposes only.
 */


STATIC char		ptpath[FILE_PATH_SZ] = { '\0' };
					/* Passthrough device path */
STATIC req_sense_data_t	sense_data;	/* Request sense data buffer */


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
	struct sb	sb;
	struct scb	*scbp;
	char		*path,
			title[FILE_PATH_SZ + 20];
	
	if (fd < 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	(void) memset(&sense_data, 0, sizeof(sense_data));
	(void) memset(&sb, 0, sizeof(sb));

	sb.sb_type = ISCB_TYPE;
	scbp = &sb.SCB;

	/* set up scsicmd */
	scbp->sc_cmdpt = (caddr_t) cmdpt;
	scbp->sc_cmdsz = cmdlen;
	scbp->sc_datapt = (caddr_t) datapt;
	scbp->sc_datasz = datalen;
	switch (rw) {
	case OP_NODATA:
	case OP_DATAIN:
		scbp->sc_mode = SCB_READ;
		break;
	case OP_DATAOUT:
		scbp->sc_mode = SCB_WRITE;
		break;
	default:
		break;
	}
	scbp->sc_time = (tmout ? tmout : DFLT_CMD_TIMEOUT) * 1000;

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, (byte_t *) scbp->sc_cmdpt, scbp->sc_cmdsz);
	}

	/* Send the command down via the "pass-through" interface */
	while (ioctl(fd, SDI_SEND, &sb) < 0) {
		if (errno == EAGAIN)
			/* Wait a little while and retry */
			(void) sleep(1);
		else {
			if (app_data.scsierr_msg && prnerr)
				perror("SDI_SEND ioctl failed");
			return FALSE;
		}
	}

	if (scbp->sc_comp_code != SDI_ASW) {
		if (app_data.scsierr_msg && prnerr) {
			(void) fprintf(errfp,
				    "%s: %s %s:\n%s=0x%x %s=0x%x %s=0x%x",
				    APPNAME,
				    "SCSI command fault on",
				    path,
				    "Opcode",
				    cmdpt[0],
				    "Completion_code",
				    (int) scbp->sc_comp_code,
				    "Target_status",
				    (int) scbp->sc_status);
		}

		/* Send Request Sense command */
		SCSICDB_RESET(cmdpt);
		cmdpt[0] = OP_S_RSENSE;
		cmdpt[4] = SZ_RSENSE;
		scbp->sc_cmdpt = (caddr_t) cmdpt;
		scbp->sc_cmdsz = 6;
		scbp->sc_datapt = (caddr_t) &sense_data;
		scbp->sc_datasz = SZ_RSENSE;
		scbp->sc_mode = SCB_READ;

		if (ioctl(fd, SDI_SEND, &sb) < 0 ||
		    sense_data.key == 0) {
			if (app_data.scsierr_msg && prnerr)
				(void) fprintf(errfp, "\n");
		}
		else if (app_data.scsierr_msg && prnerr) {
			(void) fprintf(errfp, " Key=0x%x Code=0x%x Qual=0x%x\n",
				    sense_data.key,
				    sense_data.code,
				    sense_data.qual);
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
 *	fd - Device file descriptor, or -1 on failure.
 */
int
pthru_open(char *path)
{
	int		fd;
	dev_t		ptdev;
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

	/* Open CD-ROM device */
	if ((fd = open(path, O_RDONLY)) < 0) {
		DBGPRN(errfp, "Cannot open %s: errno=%d\n", path, errno);
		return -1;
	}

	/* Get pass-through interface device number */
	if (ioctl(fd, B_GETDEV, &ptdev) < 0) {
		DBGPRN(errfp, "B_GETDEV ioctl failed: errno=%d\n", errno);
		(void) close(fd);
		return -1;
	}

	(void) close(fd);

	/* Make pass-through interface device node */
	(void) sprintf(ptpath, "%s/pass.%x", TEMP_DIR, (int) ptdev);

	if (mknod(ptpath, S_IFCHR | 0700, ptdev) < 0) {
		if (errno == EEXIST) {
			(void) UNLINK(ptpath);

			if (mknod(ptpath, S_IFCHR | 0700, ptdev) < 0) {
				DBGPRN(errfp, "Cannot mknod %s: errno=%d\n",
					ptpath, errno);
				return -1;
			}
		}
		else {
			DBGPRN(errfp, "Cannot mknod %s: errno=%d\n",
				ptpath, errno);
			return -1;
		}
	}

	/* Open pass-through device node */
	if ((fd = open(ptpath, O_RDONLY)) < 0) {
		DBGPRN(errfp, "Cannot open %s: errno=%d\n", ptpath, errno);
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

	if (ptpath[0] != '\0')
		(void) UNLINK(ptpath);
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
#ifdef _FTX
	return ("OS Interface module for Stratus SVR4 FTX 3.x\n");
#else
	return ("OS Interface module for UNIX SVR4/SVR5 PDI/SDI\n");
#endif
}

#endif	/* i386 _FTX __hppa */

#ifdef MOTOROLA
/*
 *   Motorola 88k UNIX SVR4 support
 *
 *   Contributing author: Mark Scott
 *   E-mail: mscott@urbana.mcd.mot.com
 *
 *   Note: Audio CDs sometimes produce "Blank check" warnings on the console, 
 *         just ignore these.
 *
 *   This software fragment contains code that interfaces the
 *   application to the System V Release 4 operating system
 *   from Motorola.  The name "Motorola" is used here for
 *   identification purposes only.
 */


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
	char			scsistat = '\0',
				*tmpbuf,
				*path,
				title[FILE_PATH_SZ + 20];
	int			cdb_l = 0,
				i;
	long			residual = 0L;
	unsigned long		errinfo  = 0L,
				ccode = 0L;
	struct scsi_pass	spass,
				*sp = &spass;
	struct ext_sense	sense,
				*esp = &sense;

	if (fd < 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	(void) memset(sp, 0, sizeof(struct scsi_pass));
	(void) memset(esp, 0, sizeof(struct ext_sense));

	/* Setup passthru structure */
	sp->resid = &residual;
	sp->sense_data = esp;
	sp->status = &scsistat;
	sp->error_info = &errinfo;
	sp->ctlr_code = &ccode;
	sp->xfer_len = (unsigned long) datalen;

	/* Align on a page boundary */
	tmpbuf = NULL;
	if (sp->xfer_len > 0) {
		tmpbuf = (char *) MEM_ALLOC("xfer_buf", 2 * NBPP);
		sp->data = tmpbuf;
		sp->data += NBPP - ((unsigned int) sp->data & (NBPP - 1));
	}
	else
		sp->data = tmpbuf;


	if (rw == OP_DATAOUT && sp->xfer_len > 0)	/* Write operation */
		(void) memcpy(sp->data, datapt, sp->xfer_len);

	(void) memcpy(sp->cdb, cmdpt, cmdlen);
	cdb_l = (char) (cmdlen << 4);

	/* Check CDB length & flags */

	if (!SPT_CHK_CDB_LEN(cdb_l))
		(void) fprintf(errfp, "%d: invalid CDB length\n", cmdlen);

	sp->flags = cdb_l | SPT_ERROR_QUIET;
	if (rw == OP_DATAIN || rw == OP_NODATA)
		sp->flags |= SPT_READ;

	if (SPT_CHK_FLAGS(cdb_l))
		(void) fprintf(errfp, "0x%2x: bad CDB flags\n", sp->flags);

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, cmdpt, cmdlen);
	}

	/* Send the command down via the "pass-through" interface */
	if (ioctl(fd, DKPASSTHRU, sp) < 0) {
		if (app_data.scsierr_msg && prnerr)
			perror("DKPASSTHRU ioctl failed");

		if (tmpbuf != NULL)
			MEM_FREE(tmpbuf);

		return FALSE;
	}

	if (*sp->error_info != SPTERR_NONE) {
		if (*sp->error_info != SPTERR_SCSI && *sp->status != 2 &&
		    app_data.scsierr_msg && prnerr) {

			/* Request Sense is done automatically by the driver */
			(void) fprintf(errfp,
				"%s: %s %s:\n%s=0x%x %s=0x%x %s=0x%x\n",
				APPNAME,
				"SCSI command fault on",
				path,
				"opcode",
				cmdpt[0],
				"xfer_len",
				sp->xfer_len,
				"err_info",
				*sp->error_info);

			(void) fprintf(errfp, "%s=0x%x %s=0x%x %s=0x%x\n",
				"ctlr_code",
				*sp->ctlr_code,
				"status",
				*sp->status,
				"resid",
				*sp->resid);
		}

		if (tmpbuf != NULL)
			MEM_FREE(tmpbuf);

		return FALSE;
	}

	/* pass the data back to caller */
	if (sp->xfer_len > 0 && rw == OP_DATAIN)	/* read operation */
		(void) memcpy(datapt, sp->data, sp->xfer_len);

	if (tmpbuf != NULL)
		MEM_FREE(tmpbuf);

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
bool_t
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

	/* Open CD-ROM device */
	if ((fd = open(path, O_RDONLY | O_NDELAY | O_EXCL)) < 0) {
		DBGPRN(errfp, "Cannot open %s: errno=%d\n", path, errno);
		return -1;
	}

	(void) di_devreg(fd, path);

	/* Motorola hack:  The CD-ROM driver allows the open to succeed
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
	return ("OS Interface module for Motorola SVR4-m88k\n");
}

#endif	/* MOTOROLA */

#endif	/* SVR4 SVR5 sun __sun__ DI_SCSIPT DEMO_ONLY */

