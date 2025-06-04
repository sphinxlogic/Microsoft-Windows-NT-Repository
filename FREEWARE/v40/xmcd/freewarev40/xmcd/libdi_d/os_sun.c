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
 *   SunOS and Solaris support
 *
 *   This software fragment contains code that interfaces the
 *   application to the SunOS operating systems.  The name "Sun" and
 *   "SunOS" are used here for identification purposes only.
 *
 *   You may want to compile with -DSOL2_RSENSE on Solaris 2.2 or
 *   later systems to enable the auto-request sense feature.
 */
#ifndef LINT
static char *_os_sun_c_ident_ = "@(#)os_sun.c	6.36 98/10/01";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#if (defined(sun) || defined(__sun__)) && \
    defined(DI_SCSIPT) && !defined(DEMO_ONLY)

extern appdata_t	app_data;
extern bool_t		scsipt_notrom_error;
extern FILE		*errfp;
extern di_client_t	*di_clinfo;

STATIC req_sense_data_t	sense_data;	/* Request sense data buffer */

#ifdef SOL2_VOLMGT
STATIC void		sol2_vold_alrm(int);
#endif


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
	struct uscsi_cmd	ucmd;
	char			*path,
				title[FILE_PATH_SZ + 20];
	
	if (fd < 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	(void) memset(&ucmd, 0, sizeof(ucmd));

	/* set up uscsi_cmd */
	ucmd.uscsi_cdb = (caddr_t) cmdpt;
	ucmd.uscsi_cdblen = cmdlen;
	ucmd.uscsi_bufaddr = (caddr_t) datapt;
	ucmd.uscsi_buflen = datalen;
	ucmd.uscsi_flags = USCSI_SILENT | USCSI_ISOLATE;
	if (datalen != 0) {
		switch (rw) {
		case OP_DATAIN:
			ucmd.uscsi_flags |= USCSI_READ;
			break;
		case OP_DATAOUT:
			ucmd.uscsi_flags |= USCSI_WRITE;
			break;
		case OP_NODATA:
		default:
			break;
		}
	}

#if defined(SVR4) && defined(SOL2_RSENSE)
	ucmd.uscsi_flags |= USCSI_RQENABLE;
	ucmd.uscsi_rqbuf = (caddr_t) &sense_data;
	ucmd.uscsi_rqlen = SZ_RSENSE;
#endif

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, cmdpt, cmdlen);
	}

#ifdef SOL2_VOLMGT
	if (app_data.sol2_volmgt) {
		(void) signal(SIGALRM, sol2_vold_alrm);
		(void) alarm(tmout + 5);
	}
#endif

	/* Send the command down via the "pass-through" interface */
	if (ioctl(fd, USCSICMD, &ucmd) < 0) {
		if (app_data.scsierr_msg && prnerr)
			perror("USCSICMD ioctl failed");
#ifdef SOL2_VOLMGT
		if (app_data.sol2_volmgt) {
			(void) alarm(0);
			(void) signal(SIGALRM, SIG_DFL);
		}
#endif
		return FALSE;
	}

#ifdef SOL2_VOLMGT
	if (app_data.sol2_volmgt) {
		(void) alarm(0);
		(void) signal(SIGALRM, SIG_DFL);
	}
#endif

	if (ucmd.uscsi_status != USCSI_STATUS_GOOD) {
		if (app_data.scsierr_msg && prnerr) {
			(void) fprintf(errfp,
				       "%s: %s %s:\n%s=0x%x %s=0x%x\n",
				       APPNAME,
				       "SCSI command fault on",
				       path,
				       "Opcode",
				       cmdpt[0],
				       "Status",
				       ucmd.uscsi_status);
		}

#if defined(SVR4) && defined(SOL2_RSENSE)
		if (ucmd.uscsi_rqstatus == USCSI_STATUS_GOOD &&
		    app_data.scsierr_msg && prnerr) {
			(void) fprintf(errfp, " Key=0x%x Code=0x%x Qual=0x%x\n",
				       sense_data.key,
				       sense_data.code,
				       sense_data.qual);
			return FALSE;
		}
#else	/* SVR4 SOL2_RSENSE */
		/* Send Request Sense command */
		SCSICDB_RESET(cmdpt);
		cmdpt[0] = OP_S_RSENSE;
		cmdpt[4] = (byte_t) SZ_RSENSE;

		ucmd.uscsi_cdb = (caddr_t) cmdpt;
		ucmd.uscsi_cdblen = 6;
		ucmd.uscsi_bufaddr = (caddr_t) &sense_data;
		ucmd.uscsi_buflen = (int) SZ_RSENSE;
		ucmd.uscsi_flags = USCSI_READ;

		if (ioctl(fd, USCSICMD, &ucmd) < 0 ||
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
#endif	/* SVR4 SOL2_RSENSE */

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
#ifdef SOL2_VOLMGT
		if (app_data.sol2_volmgt) {
			switch (errno) {
			case ENOENT:
			case EINTR:
			case ESTALE:
				return -1;
				/*NOTREACHED*/
			default:
				break;
			}
		}
#endif
		(void) sprintf(errstr, app_data.str_staterr, path);
		DI_FATAL(errstr);
		return -1;
	}
	if (!S_ISCHR(stbuf.st_mode)) {
#ifdef SOL2_VOLMGT
		/* Some CDs have multiple slices (partitions),
		 * so the device node becomes a directory when
		 * vold mounts each slice.
		 */
		if (app_data.sol2_volmgt && S_ISDIR(stbuf.st_mode))
			return -1;
#endif
		(void) sprintf(errstr, app_data.str_noderr, path);
		DI_FATAL(errstr);
		return -1;
	}

#ifdef SOL2_VOLMGT
	if (app_data.sol2_volmgt) {
		(void) signal(SIGALRM, sol2_vold_alrm);
		(void) alarm(10);
	}
#endif

	if ((fd = open(path, O_RDONLY)) < 0) {
		DBGPRN(errfp, "Cannot open %s: errno=%d\n", path, errno);
#ifdef SOL2_VOLMGT
		if (app_data.sol2_volmgt) {
			(void) alarm(0);
			(void) signal(SIGALRM, SIG_DFL);
		}
#endif
		return -1;
	}

#ifdef SOL2_VOLMGT
	if (app_data.sol2_volmgt) {
		(void) alarm(0);
		(void) signal(SIGALRM, SIG_DFL);
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
#ifdef SVR4
	return ("OS Interface module for SunOS 5.x\n");
#else
	return ("OS Interface module for SunOS 4.x\n");
#endif
}


#if defined(SVR4)

/*
 * sol2_volmgt_eject
 *	Use a special Solaris 2 ioctl to eject the CD.  This is required if
 *	the system is running the Sun Volume Manager.  Note that the use
 *	of this ioctl is likely to be incompatible with SCSI-1 CD-ROM
 *	drives.
 *
 * Args:
 *	fd - Device file descriptor
 *
 * Return:
 *	TRUE - eject successful
 *	FALSE - eject failed
 */
bool_t
sol2_volmgt_eject(int fd)
{
	int	ret;

	DBGPRN(errfp, "Sending DKIOCEJECT ioctl\n");

	if ((ret = ioctl(fd, DKIOCEJECT, 0)) < 0) {
		if (app_data.scsierr_msg)
			perror("DKIOCEJECT failed");
		return FALSE;
	}

	return TRUE;
}


/*
 * sol2_vold_alrm
 *	SIGALRM handler
 *
 * Args:
 *	signo - The signal number
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
sol2_vold_alrm(int signo)
{
	DBGPRN(errfp, "\nSIGALRM!\n");
}

#endif	/* SVR4 */


#endif	/* sun __sun__ DI_SCSIPT DEMO_ONLY */

