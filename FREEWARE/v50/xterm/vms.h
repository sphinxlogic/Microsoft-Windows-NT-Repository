/* vms.h
 */
#include <ssdef.h>
#include <iodef.h>
#include <msgdef.h>
#include <descrip.h>
#include <dvidef.h>
#include <jpidef.h>
#include <prcdef.h>
#include <dcdef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <accdef.h>
#include <prvdef.h>

struct IOSB
{
	short int status;
	short int len;
	int unused;
} mbx_read_iosb,iosb;

#define MAXITEMLIST   5
#define TT_BUF_SIZE 512

short int	tt_chan;    /* channell to the Pseudo terminal */
short int	mbx_chan;   /* channell to the mailbox */
struct accdef	mbx_buf;    /* mailbox buffer */
short int	mbxunit;    /* mailbox unit number */
struct IOSB 	tt_read_iosb;
char 		tt_read_buf[TT_BUF_SIZE];
struct IOSB 	tt_write_iosb;
int		pid;        	/* PID of created process */
static $DESCRIPTOR  (image, "SYS$SYSTEM:LOGINOUT.EXE");

static struct	    items {
	short int	buflen;
	short int	code;
	int		buffer;
	int		return_addr;
} itemlist[MAXITEMLIST];








