/* $Id: names.h,v 1.3 1984/08/25 11:19:06 tom Exp $
 *
 * Title:	names.h
 * Author:	Thomas E. Dickey
 * Created:	25 Aug 1984
 * Last update:	25 Aug 1984
 *
 *	Define maximum length of filenames on VAX/VMS (RMS-dependent).
 */

#ifndef	NAM$C_MAXRSS
#define	NAM$C_MAXRSS	252
#endif

#define	MAX_PATH	256	/* next power-of-2, if close	*/
