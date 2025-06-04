/*	SOCKET_UCX.C
 *	V2.1			20-Jan-1995	IfN/Mey
 *+
 * This module contains the routines missing in UCX but supported in
 * SOCKETSHR. These are all the stream I/O routine. Since we do not
 * have access to internal UCX data structures, we maintain our own
 * socket table here. This is easy because UCX uses real file descriptors
 * here starting with 3 instead of channel numbers (other socket packages
 * use channel numbers). Fortunately UCX tells us the channel number and
 * - more important - if the file descriptor is *not* a socket: vaxc$get_sdc
 * For DECC, there is decc$get_sdc, but the compiler also accepts vaxc$get_sdc.
 *-
 */

#include <stdio.h>
#include <unixio.h>
#include <socket.h>

#include "[-]socketprv.h"
#include "[-]si_trace.h"

/*
 * Our file descriptor table here. We support file descriptors 0...256.
 * Note that most of the other socketshr packages only support 0...32.
 */

struct file_descriptor {
	int fd;
	FILE *fptr;
};

#define FDSIZE 256
static struct file_descriptor sd[FDSIZE];
static isclear = 0;

/*
 *================================
 * isUCXsocket
 *================================
 *
 * Checks if the socket is an UCX socket. If so return 1 if not return 0.
 */
int
isUCXsocket(int s)
{
#ifdef __DECC
	return( (decc$get_sdc(s) != 0) ? 1 : 0);
#else
	return( (vaxc$get_sdc(s) != 0) ? 1 : 0);
#endif
}

/*
 *================================
 * ucx_socket_fptr
 *================================
 *
 * get a file pointer and store it.
 */
FILE *
ucx_socket_fptr(int s)
{
	int i;
/*
 * clear out table if we are called for the first time.
 */
	if (!isclear) {
		for (i=0;i<FDSIZE;i++) sd[i].fptr = (FILE *)NULL;
		isclear = 1;
	}
	if (s < 0  ||  s > FDSIZE) return(0);
	sd[s].fd = dup(0);
	sd[s].fptr = fdopen(sd[s].fd,"r");
	(*sd[s].fptr)->_file = s; /* backlink to enable standard fileno() */
	return(sd[s].fptr);
}

/*
 *================================
 * ucx_close_fptr
 *================================
 *
 * clear out file pointer (internal used only)
 */
void
ucx_close_fptr(int s)
{
/*
 * Maybe we have to free the dup'ed file descriptor and the fdopen'ed
 * file pointer. If not neccessary, don't care...
 */
	if (sd[s].fd != 0) close(sd[s].fd);
	sd[s].fd = 0;
	if (sd[s].fptr != NULL) fclose(sd[s].fptr);
	sd[s].fptr = (FILE *)NULL;
}

/*
 *================================
 * ucx_get_fd
 *================================
 *
 * return the socket from a file pointer.
 */
int
ucx_get_fd(FILE *fptr)
{
	int i;
	for (i=0;i<FDSIZE;i++) {
		if (sd[i].fptr == fptr) return(i);
	}
	return(-1);
}

/*
 *================================
 * ucx_fdopen
 *================================
 *
 * return the file pointer. if this is a valid ucx socket but no file
 * pointer has been created yet, do it now.
 * Note that creating more than one file pointer for one socket is
 * not supported! fdopen always returns the same pointer. This pointer
 * is never used for stream I/O, it only identifies the socket.
 */
FILE *
ucx_fdopen(int s)
{
	if (sd[s].fptr == NULL) {
		ucx_socket_fptr(s);
	}
	return(sd[s].fptr);
}

/*
 *================================
 * ucx_fcntl
 *================================
 *
 * since there is no way to tell UCX what we want, we only support
 * FNDELAY, which is done through ioctl's FIONBIO.
 */
int
ucx_fcntl(int s, int cmd, int arg)
{
	int flags;
	return(0);	/* How can we do? FIONBIO is also unsupported!!! */
}
