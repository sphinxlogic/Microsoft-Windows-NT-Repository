#ifndef __MAIL_LOADED
#define __MAIL_LOADED 1
/*
 * MAIL.H, prototypes for callable mail routines.
 *
 * Hand coded by Roland B. Roberts from the VMS Utilities manual
 * descriptions of these routines.
 */
typedef struct
{
  unsigned short int buflen;
  unsigned short int item;
  void *bufadr;
  size_t *retlen;
} ITMLST;

unsigned MAIL$MAILFILE_BEGIN	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MAILFILE_CLOSE	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MAILFILE_COMPRESS	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MAILFILE_END	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MAILFILE_INFO_FILE    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MAILFILE_MODIFY	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MAILFILE_OPEN	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MAILFILE_PURGE_WASTE  (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MAILFILE_BEGIN	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MESSAGE_COPY	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MESSAGE_DELETE	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MESSAGE_END	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MESSAGE_GET	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MESSAGE_INFO	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MESSAGE_MODIFY	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$MESSAGE_SELECT	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$SEND_ABORT	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$SEND_ADD_ADDRESS	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$SEND_ADD_ATTRIBUTE    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$SEND_ADD_BODYPART	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$SEND_BEGIN	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$SEND_END		    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$SEND_MESSAGE	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$USER_BEGIN	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$USER_DELETE_INFO	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$USER_END		    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$USER_GET_INFO	    (unsigned *, ITMLST *, ITMLST *);
unsigned MAIL$USER_SET_INFO	    (unsigned *, ITMLST *, ITMLST *);

#endif __MAIL_LOADED
