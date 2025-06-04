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
 *   FreeBSD/NetBSD/OpenBSD support
 *
 *   Contributing author: Gennady B. Sorokopud (SCIOCCOMMAND version)
 *   E-Mail: gena@netvision.net.il
 *
 *   Contributing author: Kenneth D. Merry (CAM version)
 *   E-Mail: ken@kdm.org
 *
 *   This software fragment contains code that interfaces the
 *   application to the FreeBSD (version 2.0.5 or later), NetBSD
 *   (version 1.0A or later) and OpenBSD (version 2.x and later)
 *   operating systems.
 *
 *   SCIOCCOMMAND notes:
 *   The SCIOCCOMMAND code is used for FreeBSD 2.x, NetBSD and OpenBSD.
 *   Generic SCSI-support is required in the kernel configuration file.
 *   Also be sure "SCIOCCOMMAND" in the file "/usr/include/sys/scsiio.h"
 *   is not just a dummy.
 *
 *   CAM notes:
 *   The CAM code is only used for FreeBSD 3.x and later.  It is enabled
 *   when compiled with -DFREEBSD_CAM.
 */
#ifndef LINT
static char *_os_fnbsd_c_ident_ = "@(#)os_fnbsd.c	6.35 98/10/27";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#if (defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)) && \
    defined(DI_SCSIPT) && !defined(DEMO_ONLY)

extern appdata_t	app_data;
extern bool_t		scsipt_notrom_error;
extern FILE		*errfp;
extern di_client_t	*di_clinfo;

#ifdef FREEBSD_CAM

typedef struct cam_devlist {
	struct cam_device	*cam_dev;
	struct cam_devlist	*prev;
	struct cam_devlist	*next;
} cam_devlist_t;

STATIC cam_devlist_t	*cam_devlist_head;


/*
 * get_cam_ent
 *	Given a file descriptor, return an associated cam_devlist_t
 *	pointer.
 *
 * Args:
 *	fd - The file descriptor
 *
 * Return:
 *	Pointer to cam_devlist_t structure, or NULL on failure.
 */
cam_devlist_t *
get_cam_ent(int fd)
{
	cam_devlist_t	*cp;

	for (cp = cam_devlist_head; cp != NULL; cp = cp->next) {
		if (cp->cam_dev->fd == fd)
			return (cp);
	}
	return NULL;
}

#endif	/* FREEBSD_CAM */


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
#ifdef FREEBSD_CAM
	cam_devlist_t		*cp;
	union ccb		ccb;
#else
	static struct scsireq	ucmd;
#endif
	char			*path,
				title[FILE_PATH_SZ + 20];

	if (fd < 0 || scsipt_notrom_error)
		return FALSE;

	path = di_devgetpath(fd);

	if (app_data.debug) {
		(void) sprintf(title, "SCSI CDB bytes (dev=%s rw=%d to=%d)",
			       path, rw, tmout);
		util_dbgdump(title, cmdpt, cmdlen);
	}

#ifdef FREEBSD_CAM
	/* CAM method */
	if ((cp = get_cam_ent(fd)) < 0)
		return FALSE;

	(void) memset(&ccb, 0, sizeof(ccb));

	cam_fill_csio(
		&ccb.csio,
		0,			/* retries */
		NULL,			/* cbfcnp */
		(rw == OP_DATAIN) ? CAM_DIR_IN :
			((rw == OP_DATAOUT) ? CAM_DIR_OUT : CAM_DIR_NONE),
					/* flags */
		MSG_SIMPLE_Q_TAG,	/* tag_action */
		(u_int8_t *) datapt,	/* data_ptr */
		datalen,		/* dxfer_len */
		SSD_FULL_SIZE,		/* sense_len */
		cmdlen,			/* cdb_len */
		(tmout ? tmout : DFLT_CMD_TIMEOUT) * 1000
					/* timeout */
	);

	/* Disable freezing the device queue */
	ccb.ccb_h.flags |= CAM_DEV_QFRZDIS;

	(void) memcpy(ccb.csio.cdb_io.cdb_bytes, cmdpt, cmdlen);

	/* Send the command down via the CAM interface */
	if (cam_send_ccb(cp->cam_dev, &ccb) < 0) {
		if (app_data.scsierr_msg && prnerr)
			perror("cam_send_ccb failed");
		return FALSE;
	}

	if ((ccb.ccb_h.status & CAM_STATUS_MASK) != CAM_REQ_CMP) {
		if (app_data.scsierr_msg && prnerr) {
			if ((ccb.ccb_h.status & CAM_STATUS_MASK) ==
			     CAM_SCSI_STATUS_ERROR) {
				char	tmpstr[4096];

				scsi_sense_string(cp->cam_dev, &ccb.csio,
						  tmpstr, sizeof(tmpstr));

				(void) fprintf(errfp, "%s: %s\n",
					       APPNAME, tmpstr);
			}
			else {
				(void) fprintf(errfp,
					       "%s: %s %s:\n%s=0x%x\n",
					       APPNAME,
					       "SCSI command fault on",
					       path,
					       "Status",
					       ccb.ccb_h.status);
			}
		}
		return FALSE;
	}
#else
	/* SCIOCCOMMAND ioctl method */
	(void) memset(&ucmd, 0, sizeof(ucmd));

	/* set up uscsi_cmd */
	(void) memcpy(ucmd.cmd, cmdpt, cmdlen);
	ucmd.cmdlen = cmdlen;
	ucmd.databuf = (caddr_t) datapt;	/* data buffer */
	ucmd.datalen = datalen;			/* buffer length */
	ucmd.timeout = (tmout ? tmout : DFLT_CMD_TIMEOUT) * 1000;
	if (datalen != 0) {
		switch (rw) {
		case OP_DATAIN:
			ucmd.flags |= SCCMD_READ;
			break;
		case OP_DATAOUT:
			ucmd.flags |= SCCMD_WRITE;
			break;
		case OP_NODATA:
		default:
			break;
		}
	}

	/* Send the command down via the "pass-through" interface */
	if (ioctl(fd, SCIOCCOMMAND, &ucmd) < 0) {
		if (app_data.scsierr_msg && prnerr)
			perror("SCIOCCOMMAND ioctl failed");
		return FALSE;
	}

	if (ucmd.retsts != SCCMD_OK) {
		if (app_data.scsierr_msg && prnerr) {
			(void) fprintf(errfp,
				       "%s: %s %s:\n%s=0x%x %s=0x%x\n",
				       APPNAME,
				       "SCSI command fault on",
				       path,
				       "Opcode",
				       cmdpt[0],
				       "Status",
				       ucmd.retsts);
		}

		return FALSE;
	}
#endif	/* FREEBSD_CAM */

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
#ifdef FREEBSD_CAM
	/* CAM method */
	cam_devlist_t	*cp;

	cp = (cam_devlist_t *) MEM_ALLOC(
		"cam_devlist_t",
		sizeof(cam_devlist_t)
	);
	if (cp == NULL) {
		DI_FATAL(app_data.str_nomemory);
		return -1;
	}

	if ((cp->cam_dev = cam_open_device(path, O_RDWR)) == NULL) {
		DBGPRN(errfp, "%s\n", cam_errbuf);
		MEM_FREE(cp);
		return -1;
	}

	if (cam_devlist_head != NULL)
		cam_devlist_head->prev = cp;
	cp->next = cam_devlist_head;
	cp->prev = NULL;
	cam_devlist_head = cp;

	(void) di_devreg(cp->cam_dev->fd, path);
	return (cp->cam_dev->fd);
#else
	/* SCIOCCOMMAND ioctl method */
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

	if ((fd = open(path, O_RDWR)) < 0) {
		DBGPRN(errfp, "Cannot open %s: errno=%d\n", path, errno);
		return -1;
	}

	(void) di_devreg(fd, path);
	return (fd);
#endif	/* FREEBSD_CAM */
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
#ifdef FREEBSD_CAM
	/* CAM method */
	cam_devlist_t	*cp;

	if ((cp = get_cam_ent(fd)) == NULL)
		return;

	di_devunreg(fd);
	cam_close_device(cp->cam_dev);

	if (cp->prev != NULL)
		cp->prev->next = cp->next;
	if (cp->next != NULL)
		cp->next->prev = cp->prev;
	if (cp == cam_devlist_head)
		cam_devlist_head = cp->next;
	MEM_FREE(cp);
#else
	/* SCIOCCOMMAND ioctl method */
	di_devunreg(fd);
	(void) close(fd);
#endif	/* FREEBSD_CAM */
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
#ifdef __FreeBSD__
#ifdef FREEBSD_CAM
	return ("OS Interface module for FreeBSD (CAM)\n");
#else
	return ("OS Interface module for FreeBSD (SCIOCCOMMAND)\n");
#endif
#else
#ifdef __NetBSD__
	return ("OS Interface module for NetBSD\n");
#else
#ifdef __OpenBSD__
	return ("OS Interface module for OpenBSD\n");
#else
	return ("OS Interface module (unsupported BSD variant!)\n");
#endif	/* __OpenBSD__ */
#endif	/* __NetBSD__ */
#endif	/* __FreeBSD__ */
}

#endif	/* __FreeBSD__ __NetBSD__ __OpenBSD__ DI_SCSIPT DEMO_ONLY */

