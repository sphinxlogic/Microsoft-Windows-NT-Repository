/*****************************************************************************
 *
 * FACILITY:
 *   XPool	DECwindows Pool game
 *	
 * ABSTRACT:
 *   This module contains routines to deal with cluster locks.
 *
 * AUTHOR:
 *   Doug Stefanelli
 *
 * CREATION DATE: 6-Mar-1992
 *
 * Edit History 
 *
 *  DS	6-Mar-1992	Original version
 *
 */

/*
 * Routines in this module
 */
int grab_lock();		/* get 1st or 2nd lock */
int lock_grabbed();		/* see if 2nd lock grabbed */
int lock_read();		/* read data from the lock value block */
int lock_write();		/* write data from the lock value block */

#ifdef VMS
/*
 * Include files
 */
#include <lckdef.h>
#include <ssdef.h>
#include <descrip.h>

/*
 * Local variables
 */
static struct lksb {
    short status;
    short filler;
    long id;
};
static struct lksb_val {
    short status;
    short filler;
    long id;
    long data[4];
};

$DESCRIPTOR(xpool1_res, "XPOOL1");
$DESCRIPTOR(xpool2_res, "XPOOL2");
$DESCRIPTOR(xpool3_res, "XPOOL3");
$DESCRIPTOR(player1_res, "Player_1");
$DESCRIPTOR(player2_res, "Player_2");

struct lksb player1_lock;
struct lksb player2_lock;
struct lksb_val xpool1_lock;
struct lksb_val xpool2_lock;
struct lksb_val xpool3_lock;

int sequence_expected = 0;
int saved_size;
long saved_data[20];

int grab_lock()
{
    int status;
    int num_locks = 0;

    status = sys$enqw(0, LCK$K_EXMODE, &player1_lock,
		LCK$M_SYNCSTS | LCK$M_NOQUEUE,
		&player1_res, 0, 0, 0, 0, 0, 0);
    if (status != SS$_SYNCH) {
	status = sys$enqw(0, LCK$K_EXMODE, &player2_lock,
		LCK$M_SYNCSTS | LCK$M_NOQUEUE,
		&player2_res, 0, 0, 0, 0, 0, 0);
	if (status != SS$_SYNCH) return 2;
	num_locks = 1;
    } else {
	status = sys$enqw(0, LCK$K_NLMODE, &player2_lock, 0,
		&player2_res, 0, 0, 0, 0, 0, 0);
	if (!(status&1)) return -1;
    }
    status = sys$enqw(0, LCK$K_EXMODE, &xpool1_lock, LCK$M_VALBLK,
		&xpool1_res, 0, 0, 0, 0, 0, 0);
    if (!(status&1) || !(xpool1_lock.status&1)) return -1;
    xpool1_lock.data[0] = 0;
    status = sys$enqw(0, LCK$K_NLMODE, &xpool1_lock,
		LCK$M_VALBLK | LCK$M_CONVERT,
		0, 0, 0, 0, 0, 0, 0);
    if (!(status&1) || !(xpool1_lock.status&1)) return -1;
    status = sys$enqw(0, LCK$K_NLMODE, &xpool2_lock, LCK$M_VALBLK,
		&xpool2_res, 0, 0, 0, 0, 0, 0);
    if (!(status&1) || !(xpool2_lock.status&1)) return -1;
    status = sys$enqw(0, LCK$K_NLMODE, &xpool3_lock, LCK$M_VALBLK,
		&xpool3_res, 0, 0, 0, 0, 0, 0);
    if (!(status&1) || !(xpool3_lock.status&1)) return -1;
    return num_locks;
}

void lock_close(player)
{
    int status;
    struct lksb *lock;

    status = sys$enqw(0, LCK$K_EXMODE, &xpool1_lock,
		LCK$M_VALBLK | LCK$M_CONVERT,
		0, 0, 0, 0, 0, 0, 0);
    xpool1_lock.data[0] = -1;
    status = sys$enqw(0, LCK$K_NLMODE, &xpool1_lock,
		LCK$M_VALBLK | LCK$M_CONVERT,
		0, 0, 0, 0, 0, 0, 0);
    lock = (player ? &player2_lock : &player1_lock);
    status = sys$enqw(0, LCK$K_NLMODE, lock, LCK$M_CONVERT,
		0, 0, 0, 0, 0, 0, 0);
    sequence_expected = 0;
}

int lock_grabbed()
{
    int status;

    status = sys$enqw(0, LCK$K_EXMODE, &player2_lock,
		LCK$M_SYNCSTS | LCK$M_NOQUEUE | LCK$M_CONVERT,
		0, 0, 0, 0, 0, 0, 0);
    if (status == SS$_SYNCH) {
	status = sys$enqw(0, LCK$K_NLMODE, &player2_lock,
		LCK$M_SYNCSTS | LCK$M_NOQUEUE | LCK$M_CONVERT,
		0, 0, 0, 0, 0, 0, 0);
	return 0;	/* the other player isn't there yet */
    } else
	return 1;	/* the other player has it */
}

int lock_read(data, size)
char *data;
int size;
{
    int status;
    int bytes_read = 0;

    status = sys$enqw(0, LCK$K_EXMODE, &xpool1_lock,
		LCK$M_VALBLK | LCK$M_CONVERT,
		0, 0, 0, 0, 0, 0, 0);
    if (!(status&1) || !(xpool1_lock.status&1) ||
	xpool1_lock.data[0] < 0) return -1;
    if (sequence_expected+1 == xpool1_lock.data[0]) {
	if (size <= 12)
	    memcpy(data, &xpool1_lock.data[1], size);
	else {
	    memcpy(data, &xpool1_lock.data[1], 12);
	    status = sys$enqw(0, LCK$K_EXMODE, &xpool2_lock,
			LCK$M_VALBLK | LCK$M_CONVERT,
			0, 0, 0, 0, 0, 0, 0);
	    if (!(status&1) || !(xpool2_lock.status&1)) return -1;
	    if (size <= 28) {
		memcpy(&data[12], xpool2_lock.data, size-12);
		status = sys$enqw(0, LCK$K_NLMODE, &xpool2_lock,
			LCK$M_VALBLK | LCK$M_CONVERT,
			0, 0, 0, 0, 0, 0, 0);
		if (!(status&1) || !(xpool2_lock.status&1)) return -1;
	    } else {
		memcpy(&data[12], xpool2_lock.data, 16);
		status = sys$enqw(0, LCK$K_NLMODE, &xpool2_lock,
			LCK$M_VALBLK | LCK$M_CONVERT,
			0, 0, 0, 0, 0, 0, 0);
		if (!(status&1) || !(xpool2_lock.status&1)) return -1;
		status = sys$enqw(0, LCK$K_EXMODE, &xpool3_lock,
			LCK$M_VALBLK | LCK$M_CONVERT,
			0, 0, 0, 0, 0, 0, 0);
		if (!(status&1) || !(xpool3_lock.status&1)) return -1;
		memcpy(&data[28], xpool3_lock.data, size-28);
		status = sys$enqw(0, LCK$K_NLMODE, &xpool3_lock,
			LCK$M_VALBLK | LCK$M_CONVERT,
			0, 0, 0, 0, 0, 0, 0);
		if (!(status&1) || !(xpool3_lock.status&1)) return -1;
	    }
	}
	bytes_read = size;
	sequence_expected++;
	xpool1_lock.data[0] = 0;
    }
    status = sys$enqw(0, LCK$K_NLMODE, &xpool1_lock,
		LCK$M_VALBLK | LCK$M_CONVERT,
		0, 0, 0, 0, 0, 0, 0);
    if (!(status&1) || !(xpool1_lock.status&1)) return -1;
    return bytes_read;
}

int lock_write(data, size)
char *data;
int size;
{
    int status;
    int bytes_written = 0;

    status = sys$enqw(0, LCK$K_EXMODE, &xpool1_lock,
		LCK$M_VALBLK | LCK$M_CONVERT,
		0, 0, 0, 0, 0, 0, 0);
    if (!(status&1) || !(xpool1_lock.status&1) ||
	xpool1_lock.data[0] < 0) return -1;
    if (xpool1_lock.data[0] == 0) {
	if (size <= 12)
	    memcpy(&xpool1_lock.data[1], data, size);
	else {
	    memcpy(&xpool1_lock.data[1], data, 12);
	    status = sys$enqw(0, LCK$K_EXMODE, &xpool2_lock,
			LCK$M_VALBLK | LCK$M_CONVERT,
			0, 0, 0, 0, 0, 0, 0);
	    if (!(status&1) || !(xpool2_lock.status&1)) return -1;
	    if (size <= 28) {
		memcpy(xpool2_lock.data, &data[12], size-12);
		status = sys$enqw(0, LCK$K_NLMODE, &xpool2_lock,
			LCK$M_VALBLK | LCK$M_CONVERT,
			0, 0, 0, 0, 0, 0, 0);
		if (!(status&1) || !(xpool2_lock.status&1)) return -1;
	    } else {
		memcpy(xpool2_lock.data, &data[12], 16);
		status = sys$enqw(0, LCK$K_NLMODE, &xpool2_lock,
			LCK$M_VALBLK | LCK$M_CONVERT,
			0, 0, 0, 0, 0, 0, 0);
		if (!(status&1) || !(xpool2_lock.status&1)) return -1;
		status = sys$enqw(0, LCK$K_EXMODE, &xpool3_lock,
			LCK$M_VALBLK | LCK$M_CONVERT,
			0, 0, 0, 0, 0, 0, 0);
		if (!(status&1) || !(xpool3_lock.status&1)) return -1;
		memcpy(xpool3_lock.data, &data[28], size-28);
		status = sys$enqw(0, LCK$K_NLMODE, &xpool3_lock,
			LCK$M_VALBLK | LCK$M_CONVERT,
			0, 0, 0, 0, 0, 0, 0);
		if (!(status&1) || !(xpool3_lock.status&1)) return -1;
	    }
	}
	bytes_written = size;
	xpool1_lock.data[0] = ++sequence_expected;
    }
    status = sys$enqw(0, LCK$K_NLMODE, &xpool1_lock,
		LCK$M_VALBLK | LCK$M_CONVERT,
		0, 0, 0, 0, 0, 0, 0);
    if (!(status&1) || !(xpool1_lock.status&1)) return -1;
    return bytes_written;
}

#else	/* not VMS */

int grab_lock() {}
int lock_grabbed() {}
int lock_read() {}
int lock_write() {}

#endif	/* VMS */
