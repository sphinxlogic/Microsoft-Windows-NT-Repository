#module BannerMessage "V1.0"

#include "dvidef.h"
#include "prcdef.h"
#include "jpidef.h"
#include "msgdef.h"
#include "tt2def.h"
#include "ttdef.h"
#include "stdio.h"
#include "iodef.h"
#include "descrip.h"
#include "file.h"
#include "stsdef.h"
#include "ssdef.h"
#include "MrmAppl.h"


#include "Banner"

extern	 Bnr$_Blk    Bnr;

#define fuzz_factor 100
#define maxmsg 1650		/* Always > than minimum setting on MAXBUF */
struct brdcstmsg {
    short type;
    short unit;
    char  controler_len;
    char  controler[3];
    int	  fill_1;
    int	  fill_2;

    int	  fill_3;
    short msg_len;
    char  msg [maxmsg];	        /* Start of variable section */
    };

static int message_size = 65535;	/* Massge size of saved characters */
static volatile char *Messages = 0;
static volatile int MessageSize = 0;
static volatile int LastMessageEnd = 0;
static volatile struct brdcstmsg MessageBuffer;
static short tt_chan = 0;
static short mbx_chan = 0;
static int mbx_status[2];
static volatile uptodate = 1;
static volatile bells = 0;

static int hide_delta[2];
int hide_timer_on = 0;

/*
 * The forward routine declarations
 */

void BannerMessage ();
void BannerMessageAst ();
int BannerMessageSetup ();
void BannerMessageUpdate ();
void BannerMessageCopy ();
void BannerMessageHide ();

extern void BANNERCMKRNLTRAP();


typedef struct _vms_item_list {
	short	 buffer_length;
	short	 item_code;
	void	*buffer;
	int	*returned_buffer_length;
	} VMS_ItemList;

static char dev_name[20];
static int pid = 0;

static VMS_ItemList itemlist[2] = {
    7,  JPI$_TERMINAL, &dev_name, 0,
    0,  0, 0, 0};

static VMS_ItemList owneritemlist[2] = {
    4,  JPI$_MASTER_PID, &pid, 0,
    0,  0, 0, 0};



int BannerMessageSetup()
	{
	static int ttbuf[2];
	static int mbxcreated = 0;
	static $DESCRIPTOR (mbx, "BANNER$MAILBOX");       
	static int dev_desc[2];
	static char res[65];
	static $DESCRIPTOR (res_desc, res);
	static int buffer[2], status, i = 120;
	static int firstmessage = 1, gotdevice = 0;
	/*
	 * Turn AST's off
	 */

	SYS$SETAST(0);

	/*
	 * Create the permament mailbox, and then delete it so that
	 * it goes away when the last channel is deassigned.
	 */

	if (!mbxcreated)
	    if (!((status = SYS$CREMBX (1, &mbx_chan, sizeof(MessageBuffer),
		sizeof(MessageBuffer) * 3, 0, 0, &mbx)) & STS$K_SUCCESS))
		    {
		    BannerSignalError ("Failed to create mailbox BANNER$MAILBOX\n",
			status);	
		    }
	    else
		{
		SYS$DELMBX(mbx_chan);
		mbxcreated = 1;
		}

	/*
	 * Allocate 2 PAGES of memory for the Terminal Driver
	 */
	SYS$EXPREG (2, &ttbuf, NULL, NULL);	

	/*
	 * Now create our terminal
	 */

	status = PTD$CREATE(&tt_chan, NULL, NULL, NULL, 
	    NULL, NULL, NULL, &ttbuf);
	if (status != SS$_NORMAL)
	    {
	    printf ("Failed to CREATE PTD Terminal\n");	    
	    }
	else
	    {
	    int pid;
	    VMS_ItemList itemlist[2] = {
		4,  JPI$_PID, &pid, 0,
		0,  0, 0, 0};
	    char tt[120];
	    int  ttdesc[2];
	    int  len;
	    int  loginout[2] = {23, "SYS$SYSTEM:LOGINOUT.EXE"};
	    int	 trmmbx;
	    VMS_ItemList itmlst[2] = {
		120, DVI$_DEVNAM, tt, &len, 0, 0 , 0, 0}; 

	    if ( (status = SYS$GETJPIW (NULL, NULL, NULL, &itemlist, NULL, NULL, NULL))
		 != 1)
		BannerSignalError ("Failed to get our pid for SYS$GETJPI service",
		    status);

	    
	    /*
	     * Get the Device Name for this channel
	     */
	    len = 0;
	    memset (tt, 0, sizeof(tt));
	    if (!(status = SYS$GETDVIW (NULL, tt_chan, NULL, &itmlst, NULL,
		NULL, NULL, NULL)))
		printf ("GETDEVI failed for channel  %d\n", tt_chan);

	    ttdesc[0] = strlen (tt);
	    ttdesc[1] = tt;

	    CHANGEPROCESSCHAR (pid, tt);

	    
	    /*
	     * Allocate the device
	     */

	    if (SYS$ASSIGN (ttdesc, &tt_chan, 0, &mbx) != STS$K_SUCCESS)
		printf ("Assign failed for %s", tt);

	    /*
	     * Astablish the broadcaste mailbox
	     */

/*	Hangs processes with DECWINDOWS V3
	    SYS$QIOW (0, tt_chan, 
		IO$_WRITEVBLK | IO$M_ENABLMBX, &mbx_status, 
		0, 0, &buffer, &sizeof(buffer), 0, 0, 0, 0);
*/
	    SYS$QIOW (0, 
		tt_chan, IO$_SENSEMODE , &mbx_status, 
		0, 0, &buffer, &sizeof(buffer), 0, 0, 0, 0);
	    buffer[2] = buffer[2] | TT2$M_BRDCSTMBX;
	    buffer[1] = buffer[1] | TT$M_NOBRDCST;
	    SYS$QIOW (0, tt_chan, IO$_SETMODE, &mbx_status, 
		0, 0, &buffer, &sizeof(buffer), 0, 0, 0, 0);
	    gotdevice = 1;
	    }
	/*
	 * Establish a read on our MBX
	 */
	memset( &MessageBuffer, 0, sizeof(MessageBuffer) );
	if (!((status = SYS$QIO (0, mbx_chan, IO$_READVBLK, &mbx_status, 
	    &BannerMessageAst, 0, &MessageBuffer, sizeof (MessageBuffer),
	    0, 0, 0, 0)) & STS$K_SUCCESS))
		printf ("SYS$QIO sevice failed status = %d\n", status);
	/*
	 * Turn AST's back on
	 */

	SYS$SETAST(1);
	return 1;
	}


void BannerMessageAst()
	{
	register unsigned char *src, *dst;
	int status;

	if (mbx_status[0] & STS$K_SUCCESS)
		{

		/*
		 * Check the buffer, and create it as necessary
		 */

		if (Messages == 0)
			if ((Messages = malloc (message_size + fuzz_factor)) == 0)
				BannerSignalError ("Failed to allocate Message buffer", SS$_INSFMEM);

		/*
		 * Is this a broadcast message?
		 */

		if (MessageBuffer.type == MSG$_TRMBRDCST)
			BannerMessageCopy (&MessageBuffer.msg, MessageBuffer.msg_len);
		else
			if (MessageBuffer.type == MSG$_TRMUNSOLIC &&
			    MessageBuffer.type == MSG$_TRMHANGUP)
				BannerMessageCopy (&MessageBuffer, strlen (&MessageBuffer));
		else
				BannerMessageCopy (&MessageBuffer, strlen (&MessageBuffer));

		}

	/*
	 * Issue another read on our AST
	 */

	memset( &MessageBuffer, 0, sizeof(MessageBuffer) );
	if ((!(status = SYS$QIO (0, mbx_chan, IO$_READVBLK, &mbx_status, 
	    &BannerMessageAst, 0, &MessageBuffer, sizeof (MessageBuffer),
	    0, 0, 0, 0)) & STS$K_SUCCESS))
		printf ("SYS$QIO sevice failed status = %d\n", status);

	}


void BannerMessageCopy (s, len)
    char *s;
    int len;
	{
	register char *src, *dst;
	register int dst_len;

	/*
	 * Make a quick check for useful characters, and count them
	 */
	while ((s[0] < ' ' || s[0] > '~') && len > 0)
	    {
	    if (s[0] == 007)
		bells++;
	    s++;
	    len--;
	    }

	while ((s[len-1] < ' ' || s[len-1] > '~') && len > 0)
	    {
	    len--;
	    }

	while ((s[0] <= ' ' || s[0] > '~') && len > 0)
	    {
	    if (s[0] == 007)
		bells++;
	    s++;
	    len--;
	    }

	for (src = s, dst_len = 0; src < &s [len]; src++)
		if ((*src >= ' ' && *src <= '~') ||
	  	    *src == '\n' || *src == '\t')
	    		dst_len++;

	if (dst_len <= 0)
		return;
	uptodate = 0;

	/*
	 * Check on the space and remove old text as required
	 */

	if (MessageSize + dst_len > message_size)
		{
		register int i = dst_len;
		while (i < MessageSize)
			if (Messages[i++] == '\n')
				break;
		if (i < MessageSize)
			{
			for (src = &Messages[i], dst = &Messages[0];
			     *dst++ = *src++;);
			MessageSize -= i;
			if ((LastMessageEnd -= i) < 0) LastMessageEnd = 0;
			}
		else
			Messages[LastMessageEnd = MessageSize = 0] = '\0';
		}

	/*
	 * Now copy in the new message
	 */

	for (src = s, dst = &Messages[MessageSize];
	    src < &s [len];
	    src++)
		if ((*src >= ' ' && *src <= '~') ||
	  	    *src == '\n' || *src == '\t')
	    		*dst++ = *src;
		else if (*src == 007)
			bells++;
	if (*dst != '\n')
		*dst++ = '\n';
	MessageSize += dst - &Messages[MessageSize];
	*dst++ = '\0';
	}


void BannerMessageUpdate ()
	{
	register int j;
	short rows;
	int arglist[2] = {"rows", &rows};

	SYS$SETAST(0);
	XmTextSetString(Bnr.message_text_widget, "");
	XmTextSetString (Bnr.message_text_widget, Messages);
	XtGetValues (Bnr.message_text_widget, &arglist, 1);
	if (LastMessageEnd != 0)
		for (j = 0; j < rows/2 && LastMessageEnd < MessageSize; j++, LastMessageEnd++)
			while (LastMessageEnd < MessageSize && Messages[LastMessageEnd]
			    != '\n')
				LastMessageEnd++;
	XmTextShowPosition (Bnr.message_text_widget, LastMessageEnd);
	LastMessageEnd = MessageSize;	
	if (Bnr.Param_Bits.bell && bells)
		{
		XBell (XtDisplay (Bnr.message_text_widget), Bnr.bell_volume);
		bells = 0;
		}
	uptodate = 1;
	SYS$SETAST(1);
	}


void BannerMessage()
	{
	static setup = 0;

	/*
	 * first make sure we are setup
	 */

	if (!setup)
		{
		setup = BannerMessageSetup ();
		}

	if (!uptodate)
		{
		BannerMessageUpdate ();
		XtMapWidget (Bnr.message_shell_widget);
		if (hide_timer_on)
			{
			SYS$CANTIM (&BannerMessageHide, 0);
			SYS$SETIMR (0, &hide_delta, &BannerMessageHide, &BannerMessageHide);
			}
		}
	}


void BannerMessageErase ()
	{
	MessageSize = LastMessageEnd = 0;
	XmTextSetString(Bnr.message_text_widget, "");
	XtMapWidget (Bnr.message_shell_widget);
	}


void BannerMessageHide ()
	{
	XtUnmapWidget (Bnr.message_shell_widget);
	SYS$CANTIM (&BannerMessageHide, 0);
	}


void BannerMessageSetHide (str)
    char *str;
	{
	int s[2] = {strlen (str), str};
	int status;
	if (!((status = SYS$BINTIM (&s, &hide_delta)) & STS$K_SUCCESS))
		printf ("Failed to set specified message_hide_timer, \"%s\", no timer set", str);
	else
		hide_timer_on = 1;
	}
char *BannerMessageGetHide ()
	{
	static char time[40];
	int buf[2] = {sizeof(time) - 1, &time};
	SYS$ASCTIM (&buf, &buf, &hide_delta, 0);
	time[buf[0]] = '\0';
	return time;
	}


void BannerMessageSetSize (siz)
    int siz;
	{
	if (siz > 5000)
		message_size = siz;
	else
		printf ("Minimum message_size of 5000 used");
	}
BannerMessageGetSize ()
	{
	return message_size;
	}
