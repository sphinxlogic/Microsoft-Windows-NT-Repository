 #module BannerMessage "V1.0"
*7D
 #include <jpidef.h>
 #include <msgdef.h>
 #include <tt2def.h>
 #include <ttdef.h>
 #include <stdio.h>
 #include <iodef.h>
 #include <descrip.h>
 #include <file.h>
 #include <stsdef.h>
 #include <ssdef.h>
 #include <decw$include/DECwDwtWidgetProg.h>
*7E
*7I
 
*15I
 #include "dvidef.h"
*15E
*16I
 #include "prcdef.h"
*16E
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
 
 
*7E
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
*14D
 void BannerMessageSetup ();
*14E
*14I
 int BannerMessageSetup ();
*14E
 void BannerMessageUpdate ();
 void BannerMessageCopy ();
 void BannerMessageHide ();
 
*16I
 extern void BANNERCMKRNLTRAP();
 
*16E
 
 typedef struct _vms_item_list {
 	short	 buffer_length;
 	short	 item_code;
 	void	*buffer;
 	int	*returned_buffer_length;
 	} VMS_ItemList;
 
*10D
 static char dev_name[7];
*10E
*10I
 static char dev_name[20];
*10E
 static int pid = 0;
 
 static VMS_ItemList itemlist[2] = {
     7,  JPI$_TERMINAL, &dev_name, 0,
     0,  0, 0, 0};
 
 static VMS_ItemList owneritemlist[2] = {
     4,  JPI$_MASTER_PID, &pid, 0,
     0,  0, 0, 0};
 
*14D
 
*14E
 
 
*14D
 void BannerMessageSetup()
*14E
*14I
 int BannerMessageSetup()
*14E
 	{
*14I
*15I
 	static int ttbuf[2];
*15E
 	static int mbxcreated = 0;
*14E
 	static $DESCRIPTOR (mbx, "BANNER$MAILBOX");       
 	static int dev_desc[2];
*10D
 	char res[65];
 	$DESCRIPTOR (res_desc, res);
 	int buffer[3], status, i;
*10E
*10I
 	static char res[65];
 	static $DESCRIPTOR (res_desc, res);
*11D
 	static int buffer[10], status, i;
*11E
*11I
*14D
 	static int buffer[2], status, i;
*14E
*11E
*10E
*3D
 
*3E
*3I
*14D
 	int firstmessage = 1, gotdevice = 0;
*14E
*14I
 	static int buffer[2], status, i = 120;
 	static int firstmessage = 1, gotdevice = 0;
*14E
*3E
 	/*
 	 * Turn AST's off
 	 */
 
 	SYS$SETAST(0);
 
 	/*
 	 * Create the permament mailbox, and then delete it so that
 	 * it goes away when the last channel is deassigned.
 	 */
 
*14D
 	if (!((status = SYS$CREMBX (1, &mbx_chan, sizeof(MessageBuffer),
 	    sizeof(MessageBuffer) * 3, 0, 0, &mbx)) & STS$K_SUCCESS))
 		BannerSignalError ("Failed to create mailbox BANNER$MAILBOX\n",
 		    status);	
 	SYS$DELMBX(mbx_chan);
 
*14E
*14I
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
*14E
*15D
 	/*
 	 * Allocate the lowest TWA. We assume that this is the TWA that
 	 * the Session Manager owns.
 	 * Now assign the device we want to pinch AST's from
 	 *
 	 * We will try for a period of 10 seconds, and then give up. 
 	 * This is incase the session manager does not create it, by the
 	 * tme we get here.
 	 */
 
 	if ( (status = SYS$GETJPIW (NULL, NULL, NULL, &owneritemlist, NULL, NULL, NULL))
 	     != 1)
 	    BannerSignalError ("Failed to get the time for SYS$GETJPI service",
 		status);
*15E
*2D
 	for (i=0;  i<10;  i++)
*2E
*2I
*14D
 	for (i=0;  i<120;  i++)
*14E
*14I
*15D
 	while (i > 0)
*15E
*14E
*2E
*15D
 	    {
*15E
*14I
*15D
 		    i--;
*15E
*14E
*10D
 		    
 		    if ( (status = SYS$GETJPIW (NULL, &pid, NULL, &itemlist, NULL, NULL, NULL))
*10E
*10I
*15D
 		    memset (dev_name, 0, sizeof(dev_name));
 		    if ( (status = SYS$GETJPIW (NULL, &pid, NULL, &itemlist, 
 			    NULL, NULL, NULL))
*15E
*10E
*15D
 			 != 1)
 			BannerSignalError ("Failed to get the time for SYS$GETJPI service",
 			    status);
 		    dev_desc[0] = name_size(dev_name)-1;
 		    dev_desc[1] = &dev_name;
*15E
*5D
 		    if (!((status = SYS$ASSIGN (&dev_desc, &tt_chan, 0, &mbx)) & STS$K_SUCCESS))
*5E
*5I
*15D
 /*
  * Now try to get the device, make sure its a 'T' device before we even
  * start, otherwise SYS$SETAST seems to ACCVIO with reserved OP fault.
  */
 
*15E
*8D
 		    if (dev_name[0] == 'T')
*8E
*8I
*9D
 		    if ((dev_name[0] == 'T' || 
 			dev_name[0] == 'L' ||
 			dev_name[0] == 'R') && dev_name[1] == 'T')
*9E
*9I
*13D
 		    if (strncmp(dev_name, "TWA", 3) ||
 			strncmp(dev_name, "LTA", 3) ||
 			strncmp(dev_name, "RTA", 3))
*13E
*13I
*15D
 		    if (strncmp(dev_name, "TWA", 3) == 0 ||
 			strncmp(dev_name, "LTA", 3) == 0 ||
 			strncmp(dev_name, "FTA", 3) == 0 ||
 			strncmp(dev_name, "RTA", 3) == 0 )
*15E
*13E
*9E
*8E
*15D
 			{
*15E
*10D
 			if (!((status = SYS$ASSIGN (&dev_desc, &tt_chan, 0, &mbx)) & STS$K_SUCCESS))
*10E
*10I
*15D
 			if (!((status = SYS$ASSIGN (&dev_desc, &tt_chan, 0, 
 			    &mbx)) & STS$K_SUCCESS))
*15E
*10E
*15D
 			    {
 			    if (firstmessage)
 				{
 				printf ("Waiting for Session Manager to create the Broadcaste Messages Device.\n", status);
 				firstmessage = 0;
 				}
*15E
*14I
*15D
 			    SYS$SETAST(1);
 			    return 0;
*15E
*14E
*15D
 			    }
 			else
 			    {
*15E
*10D
 			    SYS$QIO (0, tt_chan, IO$_WRITEVBLK | IO$M_ENABLMBX, &mbx_status, 0, 0, &buffer, 0, 0, 0, 0, 0);
 			    SYS$QIO (0, tt_chan, IO$_SENSEMODE , &mbx_status, 0, 0, &buffer, &sizeof(buffer), 0, 0, 0, 0);
*10E
*10I
*15D
 			    SYS$QIO (0, tt_chan, 
 				IO$_WRITEVBLK | IO$M_ENABLMBX, &mbx_status, 
 				0, 0, &buffer, &sizeof(buffer), 0, 0, 0, 0);
 			    SYS$QIO (0, 
 				tt_chan, IO$_SENSEMODE , &mbx_status, 
 				0, 0, &buffer, &sizeof(buffer), 0, 0, 0, 0);
*15E
*10E
*15D
 			    buffer[2] = buffer[2] | TT2$M_BRDCSTMBX;
 			    buffer[1] = buffer[1] | TT$M_NOBRDCST;
*15E
*10D
 			    SYS$QIO (0, tt_chan, IO$_SETMODE, &mbx_status, 0, 0, &buffer, &sizeof(buffer), 0, 0, 0, 0);
*10E
*10I
*15D
 			    SYS$QIO (0, tt_chan, IO$_SETMODE, &mbx_status, 
 				0, 0, &buffer, &sizeof(buffer), 0, 0, 0, 0);
*15E
*10E
*15D
 			    gotdevice = 1;
 			    break;
 			    }
 			}
 		    else			
*15E
*5E
*3D
 			printf ("Falied to assign the Session Manager's terminal\n", status);
*3E
*3I
*15D
 			{
 			if (firstmessage)
 			    {
*15E
*5D
 			    printf ("Waiting for Session Manager to create the Broadcaste Messages Device.\n", status);
*5E
*5I
*15D
 			    printf ("Processes device is not a Terminal.\n", status);
*15E
*5E
*6I
*15D
 			    printf ("Process device is type - ");
 			    printf (dev_name);
 			    printf ("\n");
*15E
*6E
*15D
 			    firstmessage = 0;
*15E
*5I
*6D
 			    break;
*6E
*5E
*15D
 			    }
*15E
*14I
*15D
 			SYS$SETAST(1);
 			return 0;
*15E
*14E
*15D
 			}
*15E
*3E
*5D
 		    else
 			{
 			SYS$QIO (0, tt_chan, IO$_WRITEVBLK | IO$M_ENABLMBX, &mbx_status, 0, 0, &buffer, 0, 0, 0, 0, 0);
 			SYS$QIO (0, tt_chan, IO$_SENSEMODE , &mbx_status, 0, 0, &buffer, &sizeof(buffer), 0, 0, 0, 0);
 			buffer[2] = buffer[2] | TT2$M_BRDCSTMBX;
 			buffer[1] = buffer[1] | TT$M_NOBRDCST;
 			SYS$QIO (0, tt_chan, IO$_SETMODE, &mbx_status, 0, 0, &buffer, &sizeof(buffer), 0, 0, 0, 0);
*5E
*3I
*5D
 			gotdevice = 1;
*5E
*3E
*5D
 			break;
 			}
*5E
*14D
 	    lib$wait (&1.0);
*14E
*15D
 	    }
*15E
*3I
*15D
 
*15E
*14I
*15D
 	/*
 	 * Now issue a read on our mail box
 	 */
*15E
*14E
*15D
 	if (gotdevice != 1)
*15E
*14I
*15D
 	    {
*15E
*14E
*15D
 	    printf ("Failed to Allocate a Terminal Device, BroadCaste messages are not being trapped!\n");
*15E
*3E
*14D
 	/*
 	 * Now issue a read on our mail box
 	 */
 
 	memset( &MessageBuffer, 0, sizeof(MessageBuffer) );
 	if (!((status = SYS$QIO (0, mbx_chan, IO$_READVBLK, &mbx_status, 
 	    &BannerMessageAst, 0, &MessageBuffer, sizeof (MessageBuffer),
 	    0, 0, 0, 0)) & STS$K_SUCCESS))
 		printf ("SYS$QIO sevice failed status = %d\n", status);
 
*14E
*14I
*15D
 	    }
 	else
 	    {
 	    memset( &MessageBuffer, 0, sizeof(MessageBuffer) );
 	    if (!((status = SYS$QIO (0, mbx_chan, IO$_READVBLK, &mbx_status, 
 		&BannerMessageAst, 0, &MessageBuffer, sizeof (MessageBuffer),
 		0, 0, 0, 0)) & STS$K_SUCCESS))
 		    printf ("SYS$QIO sevice failed status = %d\n", status);
 	    }	    
*15E
*15I
 
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
*16I
 	    int pid;
 	    VMS_ItemList itemlist[2] = {
 		4,  JPI$_PID, &pid, 0,
 		0,  0, 0, 0};
*16E
 	    char tt[120];
 	    int  ttdesc[2];
 	    int  len;
*16I
 	    int  loginout[2] = {23, "SYS$SYSTEM:LOGINOUT.EXE"};
*16E
 	    int	 trmmbx;
*16D
 
*16E
 	    VMS_ItemList itmlst[2] = {
 		120, DVI$_DEVNAM, tt, &len, 0, 0 , 0, 0}; 
 
*16I
 	    if ( (status = SYS$GETJPIW (NULL, NULL, NULL, &itemlist, NULL, NULL, NULL))
 		 != 1)
 		BannerSignalError ("Failed to get our pid for SYS$GETJPI service",
 		    status);
 
 	    
*16E
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
*16I
 
 	    CHANGEPROCESSCHAR (pid, tt);
 
 	    
*16E
 	    /*
 	     * Allocate the device
 	     */
*16I
 
*16E
 	    if (SYS$ASSIGN (ttdesc, &tt_chan, 0, &mbx) != STS$K_SUCCESS)
 		printf ("Assign failed for %s", tt);
*16I
 
*16E
 	    /*
 	     * Astablish the broadcaste mailbox
 	     */
*17I
 
 /*	Hangs processes with DECWINDOWS V3
*17E
 	    SYS$QIOW (0, tt_chan, 
 		IO$_WRITEVBLK | IO$M_ENABLMBX, &mbx_status, 
 		0, 0, &buffer, &sizeof(buffer), 0, 0, 0, 0);
*17I
 */
*17E
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
*15E
*14E
 	/*
 	 * Turn AST's back on
 	 */
 
 	SYS$SETAST(1);
*14I
 	return 1;
*14E
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
*14I
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
 
*14E
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
*7D
 	DwtSTextSetString(Bnr.message_text_widget, "");
 	DwtSTextSetString (Bnr.message_text_widget, Messages);
*7E
*7I
 	XmTextSetString(Bnr.message_text_widget, "");
 	XmTextSetString (Bnr.message_text_widget, Messages);
*7E
 	XtGetValues (Bnr.message_text_widget, &arglist, 1);
 	if (LastMessageEnd != 0)
 		for (j = 0; j < rows/2 && LastMessageEnd < MessageSize; j++, LastMessageEnd++)
 			while (LastMessageEnd < MessageSize && Messages[LastMessageEnd]
 			    != '\n')
 				LastMessageEnd++;
*7D
 	DwtTextShowPosition (Bnr.message_text_widget, LastMessageEnd);
*7E
*7I
 	XmTextShowPosition (Bnr.message_text_widget, LastMessageEnd);
*7E
 	LastMessageEnd = MessageSize;	
 	if (Bnr.Param_Bits.bell && bells)
 		{
*12D
 		Xbell (XtDisplay (Bnr.message_text_widget), Bnr.bell_volume);
*12E
*12I
 		XBell (XtDisplay (Bnr.message_text_widget), Bnr.bell_volume);
*12E
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
*14D
 		BannerMessageSetup ();
 		setup = 1;
*14E
*14I
 		setup = BannerMessageSetup ();
*14E
 		}
 
 	if (!uptodate)
 		{
 		BannerMessageUpdate ();
 		XtMapWidget (Bnr.message_shell_widget);
 		if (hide_timer_on)
*15I
 			{
*15E
 			SYS$CANTIM (&BannerMessageHide, 0);
 			SYS$SETIMR (0, &hide_delta, &BannerMessageHide, &BannerMessageHide);
*15I
 			}
*15E
 		}
 	}
 
 
 void BannerMessageErase ()
 	{
 	MessageSize = LastMessageEnd = 0;
*7D
 	DwtSTextSetString(Bnr.message_text_widget, "");
*7E
*7I
 	XmTextSetString(Bnr.message_text_widget, "");
*7E
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
