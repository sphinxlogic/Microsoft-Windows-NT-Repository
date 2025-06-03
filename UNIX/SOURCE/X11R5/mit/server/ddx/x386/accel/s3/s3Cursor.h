/* $XFree86: mit/server/ddx/x386/accel/s3/s3Cursor.h,v 2.0 1993/08/25 11:01:17 dawes Exp $ */

extern Bool s3BlockCursor;
extern Bool s3ReloadCursor;

#define BLOCK_CURSOR	s3BlockCursor = TRUE;

#define UNBLOCK_CURSOR	{ \
			   if (s3ReloadCursor) \
			      s3RestoreCursor(s3savepScreen); \
			   s3BlockCursor = FALSE; \
			}
