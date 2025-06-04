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
static char *_os_demo_c_ident_ = "@(#)os_demo.c	6.22 98/05/01";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"
#include "libdi_d/libdi.h"
#include "libdi_d/scsipt.h"

#ifdef DEMO_ONLY

extern appdata_t	app_data;
extern bool_t		scsipt_notrom_error;
extern FILE		*errfp;
extern di_client_t	*di_clinfo;

int			cdsim_sfd[2] = { -1, -1 },
			cdsim_rfd[2] = { -1, -1 };
STATIC pid_t		cdsim_pid = -1;


/*
 * pthru_send
 *	Build SCSI CDB and send command to the device.
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
	simpkt_t	spkt,
			rpkt;
	static word32_t	pktid = 0;

	if (cdsim_rfd[0] < 0 || cdsim_sfd[1] < 0 || scsipt_notrom_error)
		return FALSE;

	(void) memset(&spkt, 0, CDSIM_PKTSZ);
	(void) memset(&rpkt, 0, CDSIM_PKTSZ);

	(void) memcpy(spkt.cdb, cmdpt, cmdlen);
	spkt.cdbsz = (byte_t) cmdlen;
	spkt.len = (datalen > MAX_DATALEN) ? MAX_DATALEN : datalen;
	spkt.dir = rw;
	spkt.pktid = ++pktid;

	/* Reset packet id if overflowing */
	if (pktid == 0xffff)
		pktid = 0;

	/* Copy data from user buffer into packet */
	if (rw == OP_DATAOUT && datapt != NULL && spkt.len != 0)
		(void) memcpy(spkt.data, datapt, spkt.len);

	/* Send command packet */
	if (!cdsim_sendpkt("pthru", cdsim_sfd[1], &spkt))
		return FALSE;

	/* Get response packet */
	if (!cdsim_getpkt("pthru", cdsim_rfd[0], &rpkt))
		return FALSE;

	/* Sanity check */
	if (rpkt.pktid != spkt.pktid) {
		if (app_data.scsierr_msg && prnerr)
			(void) fprintf(errfp, "pthru: packet sequence error.\n");

		return FALSE;
	}

	/* Check return status */
	if (rpkt.retcode != CDSIM_COMPOK) {
		if (app_data.scsierr_msg && prnerr)
			(void) fprintf(errfp,
				"pthru: cmd error (opcode=0x%x status=%d).\n",
				rpkt.cdb[0], rpkt.retcode);

		return FALSE;
	}

	/* Copy data from packet into user buffer */
	if (rw == OP_DATAIN && datapt != NULL && rpkt.len != 0)
		(void) memcpy(datapt, rpkt.data, rpkt.len);

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
/*ARGSUSED*/
int
pthru_open(char *path)
{
	/* Do nothing here */
	return 0x1234;
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
/*ARGSUSED*/
void
pthru_close(int fd)
{
	/* Do nothing here */
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
	return ("OS Interface module (Demo Dummy)\n");
}


/*
 * pthru_cdsim_start
 *	Start the CD-simulator
 *
 * Args:
 *	None.
 *
 * Return:
 *	TRUE - successful
 *	FALSE - failed
 */
bool_t
pthru_cdsim_start(void)
{
	/* Hard code some capabilities parameters for the
	 * simulated CD-ROM drive.  This overrides the
	 * parameters from the device-specific config files.
	 */
	app_data.vendor_code = VENDOR_SCSI2;
	app_data.play10_supp = TRUE;
	app_data.play12_supp = TRUE;
	app_data.playmsf_supp = TRUE;
	app_data.playti_supp = TRUE;
	app_data.load_supp = TRUE;
	app_data.eject_supp = TRUE;
	app_data.msen_dbd = FALSE;
	app_data.mselvol_supp = TRUE;
	app_data.balance_supp = TRUE;
	app_data.chroute_supp = TRUE;
	app_data.pause_supp = TRUE;
	app_data.caddylock_supp = TRUE;
	app_data.curpos_fmt = TRUE;

	/* Open pipe for IPC */
	if (PIPE(cdsim_sfd) < 0 || PIPE(cdsim_rfd) < 0) {
		DI_FATAL("Cannot open pipe.");
		return FALSE;
	}

	/* Fork the CD simulator child */
	switch (cdsim_pid = FORK()) {
	case -1:
		DI_FATAL("Cannot fork.");
		return FALSE;

	case 0:
		/* Close some file descriptors */
		(void) close(cdsim_sfd[1]);
		(void) close(cdsim_rfd[0]);
		cdsim_sfd[1] = cdsim_rfd[0] = -1;

		/* Child: run CD simulator */
		cdsim_main();
		exit(0);

	default:
		/* Close some file descriptors */
		(void) close(cdsim_sfd[0]);
		(void) close(cdsim_rfd[1]);
		cdsim_sfd[0] = cdsim_rfd[1] = -1;

		/* Parent: continue running the CD player */
		DBGPRN(errfp, "pthru: forked cdsim child pid=%d\n",
			(int) cdsim_pid);
		break;
	}

	return TRUE;
}


/*
 * pthru_cdsim_stop
 *	Stop the CD simulator
 *
 * Args:
 *	None.
 *
 * Return:
 *	Nothing.
 */
void
pthru_cdsim_stop(void)
{
	waitret_t	stat_val;

	/* Close down pipes */
	(void) close(cdsim_sfd[1]);
	(void) close(cdsim_rfd[0]);
	cdsim_sfd[1] = cdsim_rfd[0] = -1;

	/* Shut down child */
	if (cdsim_pid > 0 && kill(cdsim_pid, 0) == 0)
		kill(cdsim_pid, SIGTERM);

	/* Wait for child to exit */
	(void) WAITPID(cdsim_pid, &stat_val, 0);
}

#endif	/* DEMO_ONLY */

