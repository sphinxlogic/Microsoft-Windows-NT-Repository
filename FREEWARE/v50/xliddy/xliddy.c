#module XLIDDY "X1.4"

/*
*****************************************************************************
*                                                                           *
*  COPYRIGHT (c) 1988, 1990  BY                                             *
*  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.                   *
*  ALL RIGHTS RESERVED.                                                     *
*                                                                           *
*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED    *
*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE    *
*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER    *
*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY    *
*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY    *
*  TRANSFERRED.                                                             * 
*                                                                           *
*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE    *
*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT    *
*  CORPORATION.                                                             *
*                                                                           *
*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS    *
*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.                  *
*                                                                           *
*****************************************************************************
*/

/*
**++
**  FACILITY:
**
**      XLiddy	- X11 Protocol Tap (DECnet)
**
**  ABSTRACT:
**
**      Xliddy is a program that poses as an XServer on a DECnet network in order to
**	record the protocol exchanges between any number of X clients and a single
**	X server, i.e., a bidirectional "tee".  The data from the branch of the tee
**	is logged to a file which is intended to be processed by a companion program,
**	XWoodward, which generates a human-readable description of the protocol.
**
**	File format:
**
**	Record format is undefined.
**
**	Data consists of:
**
**	o  One longword containing the session number
**
**	o  One quadword (two longwords) containing a VMS timestamp
**
**	o  One longword containing code giving type of data (types are defined
**	    in Xl.h)
**
**	o  One longword giving number of bytes of data to follow
**
**	o  data bytes
**
**  AUTHORS:
**
**      Monty C. Brandenberg
**
**
**  CREATION DATE:     August 18, 1987
**
**  MODIFICATION HISTORY:
**
**	X1.4	MCB0003	    Monty Brandenberg	    11-Nov-1988
**		Convert to new object naming convention.
**
**	X1.3	MCB0002	    Monty Brandenberg	    11-Nov-1987
**		Remove the buffer size option (-b) as this seems to confuse
**		people.
**
**	X1.2	MCB0001	    Monty Brandenberg	     4-Sep-1987
**		Changed "f" option to "o" so I could put my name in here again.
**		Also, start using the stream interface noting that this requires
**		VMS V4.6 or later.
**
**	X1.1	JB0001	    John Buford		    28-Aug-1987
**		Added timestamp to the logfile output.
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/

#include    <stdio.h>
#include    <descrip.h>
#include    <file.h>
#include    <errno.h>
#include    <perror.h>
#include    <iodef.h>
#include    <ssdef.h>
#include    <dvidef.h>
#include    <nfbdef.h>
#include    <msgdef.h>
#include    <lnmdef.h>

#ifndef	NO_TCP
#include    <ucx$inetdef.h>
#define	    htonl(l)	\
( (((unsigned long)(l) & 0xff) << 24) | (((unsigned long)(l) & 0xff00) << 8) |\
  (((unsigned long)(l) & 0xff0000) >> 8) | (((unsigned long)(l) & 0xff000000) >> 24) )
#define	    htons(s)	\
( (((unsigned short)(s) & 0xff) << 8) | (((unsigned short)(s) & 0xff00) >> 8) )
#endif	/* NO_TCP */

#include    "xl.h"
#include    "itmdef.h"



/*
**
**  MACRO DEFINITIONS
**
**/

#define	    VMSERR( _expr_ )	if (((status=(_expr_))&1)==0) lib$signal( status )
#define	    VMSFAIL( _expr_ )	(((status=(_expr_))&1)==0)


#define	NET_MBX_BUFFER_SIZE	128
#define	MAXCLIENTS		10
#define	NET_READ_BUFFER_SIZE    8192

#define	PROTO_DECNET    0
#define	PROTO_TCP	1

static int  xl_server = 0,
	    xl_display = 9,
	    xl_old_object = 0,
	    xl_bufsiz = NET_READ_BUFFER_SIZE,
	    xl_protocol = PROTO_DECNET;
static char *xl_workstation = "0",
	    *xl_logfilename = "sys$output";

static struct con_rec {
    int		    cr_id;
    int		    cr_active;
    int		    cr_protocol;
    unsigned short  cr_schan;
    unsigned short  cr_cchan;
    unsigned short  cr_sriosb[4],
		    cr_swiosb[4],
		    cr_criosb[4],
		    cr_cwiosb[4];
    char	    *cr_sbuf;
    char	    *cr_cbuf;
    int		    cr_session;
    unsigned short  cr_unit;
} cur_conn[MAXCLIENTS];
static int client_index = 0;
static unsigned short	net_chan,
			net_mbx_chan,
			net_iosb[4],
			net_mbx_iosb[4];
static char		net_mbx_buffer[NET_MBX_BUFFER_SIZE];

extern int  optind;
extern char *optarg;
static int  log_fd,
	    session = 0;

static $DESCRIPTOR( net_dev_desc, "NET:" );
static $DESCRIPTOR( net_mbx_dev_desc, "XLIDDY$MAILBOX" );

#ifndef	NO_TCP
static $DESCRIPTOR( inet_dev_desc, "UCX$DEVICE" );
static $DESCRIPTOR( inet_local_desc, "UCX$INET_HOST" );
static unsigned long	inet_server_addr,
			inet_self_addr,
			inet_accept_len;
static unsigned char	inet_accept_info[16];
static unsigned short	inet_chan,
			inet_lchan,
			inet_dev_chan,
			inet_iosb[4],
			inet_liosb[4],
			inet_port,
			inet_lport;
static $DESCRIPTOR( inet_accept_desc, inet_accept_info );
#endif	/* NO_TCP */

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      main	- main program (see print_usage for description)
**
**  FORMAL PARAMETERS:
**
**      int	argc	    Number of arguments on command line
**	char	*argv[]	    Vector of pointers to arguments
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  COMPLETION CODES:
**
**      1 on success, 0 on failure
**
**  SIDE EFFECTS:
**
**      Creates a known object on the host node which appears as an X Server
**	to an X client.
**
**--
**/
main(argc, argv)
int	argc;
char	*argv[];
{
    int	    status, chan, read_mode = 0;
    char    line[80];
    int	    i, j, k, file_seen = 0 ;
    char    option,
	    getopt(),
	    object_block[5] = { NFB$C_DECLNAME, 0, 0, 0, 0 };
    struct dsc$descriptor_s object_block_desc = { 5, DSC$K_DTYPE_T, DSC$K_CLASS_S, object_block },
			    object_name_desc = { 0, DSC$K_DTYPE_T, DSC$K_CLASS_S, NULL };
#ifndef	NO_TCP
    
#endif	/* NO_TCP */
/*
 * parse command arguments
 */

    if ( argc < 2 ) {
	print_usage();
	exit();
    }

    while ( (option = getopt( argc, argv, "s:w:d:o:b:ct" )) != EOF ) {
	switch (option) {
case 't':
#ifndef	NO_TCP
	    xl_protocol = PROTO_TCP;
#else	/* NO_TCP */
	    fprintf( stderr, "[tcp/ip option not compiled in.]\n" );
	    exit();
#endif	/* NO_TCP */
	    break;

case 'c':   /*
	     * compatibility with pre-bl11.2 object naming
	     */
	    xl_old_object = 1;
	    break;

case 's':   if ( sscanf( optarg, "%d", &status ) != 1 ) {
		fprintf( stderr,  "[Bad server number]\n" );
		exit();
	    }
	    xl_server = status;
	    break;

case 'w':   xl_workstation = optarg;
	    break;

case 'd':   if ( sscanf( optarg, "%d", &status ) != 1 ) {
		fprintf( stderr, "[Bad chamelion server number]\n" );
		exit();
	    }
	    xl_display = status;
	    break;

case 'o':   if ( file_seen ) {
		fprintf( stderr, "[Log filename already seen]\n" );
	    } else {
		file_seen = 1;
		xl_logfilename = optarg;
		if ( (log_fd = creat( xl_logfilename, 0, "rfm=udf" ) ) < 0 ) {
		    fprintf( stderr, "[Could not create log file %s]\n", xl_logfilename);
		    perror( "Xliddy" );
		    exit();
		}
	    }
	    break;

case 'b':   fprintf( stderr, "[-b option is obsolete.  Ignored.]\n" );
	    break;

case '?':   print_usage();
	    exit();
	}
    }
/*
 *
 * create a known object to receive connect requests
 *
 */

    switch ( xl_protocol ) {

case PROTO_DECNET:
	if ( xl_old_object ) sprintf( line, "X%d", xl_display );
	else sprintf( line, "X$X%d", xl_display );
	object_name_desc.dsc$w_length = strlen( line );
	object_name_desc.dsc$a_pointer = line;

	VMSERR( (sys$crembx( 0, &net_mbx_chan, 0, 0, 0, 0, &net_mbx_dev_desc ) ) );
	VMSERR( (sys$assign( &net_dev_desc, &net_chan, 0, &net_mbx_dev_desc ) ) );
	VMSERR( (sys$qiow( 0, net_chan, IO$_ACPCONTROL, net_iosb, NULL, NULL,
	    &object_block_desc, &object_name_desc, NULL, NULL, NULL, NULL ) ) );
	VMSERR( (net_iosb[0]) );
	mailbox_read_queue();
	break;

#ifndef	NO_TCP
case PROTO_TCP:
	{
	    static char	    local_node[128];
	    char    	    host_name[16];
	    unsigned long   stuff[4],
			    socktype = (UCX$C_STREAM << 16) | UCX$C_TCP;
	    struct itm$item_list_3	items[3];
	    struct SOCKADDRIN sin = { INET$C_AF_INET, 0, htonl( INET$C_INADDR_ANY ), 0,0,0,0,0,0,0,0 };
	    long    lnlen = 0,
		    i,
		    host_name_len = sizeof( host_name ) - 1,
		    ghbyn_func_code = INETACP_FUNC$C_GETHOSTBYNAME;
	    $DESCRIPTOR( tab_desc, "LNM$FILE_DEV" );
	    struct dsc$descriptor_s ghbyn_desc = { sizeof( ghbyn_func_code ), DSC$K_DTYPE_T, DSC$K_CLASS_S, &ghbyn_func_code };
	    static $DESCRIPTOR( local_node_desc, local_node );
	    $DESCRIPTOR( host_name_desc, host_name );
	    struct dsc$descriptor_s sin_desc = { sizeof( sin ), DSC$K_DTYPE_T, DSC$K_CLASS_S, &sin };
	    struct  dsc$descriptor_s	ws_desc;

	    /*
	     * Connect to inet acp
	     */
	    VMSERR( (sys$assign( &inet_dev_desc, &inet_lchan, 0, 0 ) ) );
	    /*
	     * Get the address of the host we're running xliddy on
	     */
	    items[0].itm$w_itmcod = LNM$_STRING;
	    items[0].itm$w_bufsiz = sizeof( local_node ) - 1;
	    items[0].itm$l_bufadr = local_node;
	    items[0].itm$l_retlen = &lnlen;
	    items[1].itm$w_itmcod = 0;
	    items[1].itm$w_bufsiz = 0;
	    items[1].itm$l_bufadr = 0;
	    items[1].itm$l_retlen = 0;
	    VMSERR( (sys$trnlnm( &LNM$M_CASE_BLIND, &tab_desc, &inet_local_desc, 0, items ) ) );
	    local_node_desc.dsc$w_length = lnlen;
	    host_name_desc.dsc$w_length = sizeof( host_name ) - 1; 
	    VMSERR( (sys$qiow(	0, inet_lchan, IO$_ACPCONTROL, inet_liosb, 0, 0, &ghbyn_desc, &local_node_desc,
				&host_name_len, &host_name_desc, 0, 0 ) ) );
	    VMSERR( (inet_liosb[0]) );
	    host_name[host_name_len] = '\0';
	    if ( (sscanf(host_name, "%d.%d.%d.%d", &stuff[3], &stuff[2], &stuff[1], &stuff[0] ) ) != 4 ) {
		perror( "xliddy" );
		exit();
	    }
	    i = (stuff[3] << 24) | (stuff[2] << 16) | (stuff[1] << 8) | stuff[0];
	    inet_self_addr = htonl( i );
	    /*
	     * Convert listening socket to correct type and port
	     */
	    sin.SIN$W_PORT = htons( 6000 + xl_display );
	    sin.SIN$L_ADDR = inet_self_addr;
	    VMSERR( (sys$qiow(	0, inet_lchan, IO$_SETMODE, inet_liosb, 0, 0,
		&socktype, (INET$M_LINGER|INET$M_KEEPALIVE), &sin_desc, 5, 0, 0 ) ) );
	    VMSERR( (inet_liosb[0]) );
	    /*
	     * Get address of server
	     */
	    if ( !strcmp( "0", xl_workstation ) ) {
		/*
		 * use local
		 */
		inet_server_addr = inet_self_addr;
	    }
	    else {
		/*
		 * Search local host date base for node
		 */
		ws_desc.dsc$a_pointer = xl_workstation;
		ws_desc.dsc$w_length = strlen( xl_workstation );
		ws_desc.dsc$b_dtype = DSC$K_DTYPE_T;
		ws_desc.dsc$b_class = DSC$K_CLASS_S;
		host_name_desc.dsc$w_length = sizeof( host_name ) - 1; 
		VMSERR( (sys$qiow(	0, inet_lchan, IO$_ACPCONTROL, inet_liosb, 0, 0, &ghbyn_desc, &ws_desc,
				&host_name_len, &host_name_desc, 0, 0 ) ) );
		if VMSFAIL( (inet_liosb[0]) ) {
		    if ( inet_liosb[0] != SS$_ENDOFFILE ) {
			/*
			 * bad error, report
			 */
			errno = EVMSERR;
			vaxc$errno = inet_liosb[0];
			perror( "xliddy" );
			exit();
		    }
		    else {
			/*
			 * eof, try to decode workstation string as address
			 */
			if ( (sscanf(xl_workstation, "%d.%d.%d.%d", &stuff[3], &stuff[2], &stuff[1], &stuff[0] ) ) != 4 ) {
			    perror( "xliddy" );
			    exit();
			}
			i = (stuff[3] << 24) | (stuff[2] << 16) | (stuff[1] << 8) | stuff[0];
			inet_server_addr = htonl( i );
		    }
		}
		else {
		    /*
		     * success, decode host as address
		     */
		    host_name[host_name_len] = '\0';
		    if ( (sscanf(host_name, "%d.%d.%d.%d", &stuff[3], &stuff[2], &stuff[1], &stuff[0] ) ) != 4 ) {
			perror( "xliddy" );
			exit();
		    }
		    i = (stuff[3] << 24) | (stuff[2] << 16) | (stuff[1] << 8) | stuff[0];
		    inet_server_addr = htonl( i );
		}
	    }
	    /*
	     * inet_server_addr should now be valid.
	     */

	    /*
	     * start listening for incoming connections.
	     */
	    inet_read_queue();
	}
	break;

#endif	/* NO_TCP */
    }

/*
 * all work is done at AST level
 */
    fprintf( stderr, "[Ready]\n" );
    VMSERR( (sys$hiber()));
/*
 * we wake up when we are finished working
 */
    close( log_fd );
    if ( errno == EVMSERR ) perror( "xliddy" );
    exit();
}

static int	
print_usage(void)
{
    fprintf( stderr, "\n\n\t\tXLiddy - X Protocol Wire Tap\n\n\
Poses as an XServer in order to listen to protocol exchanges\n\n\
Usage:	xliddy  [-w  server host name]   [-o  log filename]\n\
		[-c  compatible with old object names]\n\
                [-t  use UCX tcp/ip]\n\
                [-s  server display number]\n\
                [-d  listener display number]\n" );
}


mailbox_read_queue()
{
    int	mailbox_read_ast(),
	status;

    VMSERR( (sys$qio( 0, net_mbx_chan, IO$_READVBLK, net_mbx_iosb, mailbox_read_ast, 0,
			net_mbx_buffer, NET_MBX_BUFFER_SIZE, 0, 0, 0, 0 ) ) );
}


mailbox_read_ast()
{
    struct dsc$descriptor_s ncb_desc,
			    dest_desc;
    unsigned short	msgtype = *(unsigned short *) net_mbx_buffer,
			unit = *(unsigned short *) &net_mbx_buffer[2];
    int			name_len = net_mbx_buffer[4],
			info_len = 0,
			prefix[] = { 0, 0, 0, XL_LINK_CONNECT, 0 },
			i,
			newunit,
			status;
    char		*name = net_mbx_buffer+5,
			*info,
			dest[80];
    struct itm$item_list_3  unit_list[2] = {
	{ 4, DVI$_UNIT, &newunit, 0 },
	{ 0, 0, 0, 0 } };

    info_len = *(name + name_len);
    info = name + name_len + 1;
    switch (msgtype) {
case MSG$_CONNECT:
	ncb_desc.dsc$b_dtype = DSC$K_DTYPE_T;
	ncb_desc.dsc$b_class = DSC$K_CLASS_S;
	ncb_desc.dsc$w_length =  info_len;
	ncb_desc.dsc$a_pointer = info;
	if ( client_index < MAXCLIENTS ) {
	    i = client_index;
	    VMSERR( (sys$assign( &net_dev_desc, &cur_conn[i].cr_cchan, 0, &net_mbx_dev_desc ) ) );
	    if ( ( (cur_conn[i].cr_cbuf = malloc( xl_bufsiz )) == 0 ) ||
		 ( (cur_conn[i].cr_sbuf = malloc( xl_bufsiz )) == 0 ) ) VMSERR( (SS$_INSFMEM) );
	    if ( xl_old_object ) sprintf( dest, "%s::\"0=X%d\"", xl_workstation, xl_server );
	    else sprintf( dest, "%s::\"0=X$X%d\"", xl_workstation, xl_server );
	    dest_desc.dsc$b_dtype = DSC$K_DTYPE_T;
	    dest_desc.dsc$b_class = DSC$K_CLASS_S;
	    dest_desc.dsc$w_length =  strlen( dest );
	    dest_desc.dsc$a_pointer = dest;
	    VMSERR( (sys$assign( &dest_desc, &cur_conn[i].cr_schan, 0, 0 ) ) );
	    VMSERR( (sys$qiow( 0, cur_conn[i].cr_cchan, IO$_ACCESS, cur_conn[i].cr_criosb, 0, 0, 0, &ncb_desc, 0, 0, 0, 0 ) ) );
	    VMSERR( (sys$getdviw( 0, cur_conn[i].cr_cchan, 0, unit_list, cur_conn[i].cr_criosb, 0, 0, 0 ) ) );
	    cur_conn[i].cr_protocol = PROTO_DECNET;
	    cur_conn[i].cr_id = i;
	    cur_conn[i].cr_active = 1;
	    cur_conn[i].cr_unit = newunit;
	    cur_conn[i].cr_session = session;
	    prefix[XL_SESSION] = session;
	    get_time( &prefix[XL_TIMESTAMP] );
	    write( log_fd, prefix, sizeof( prefix ) );
	    read_from_server_queue( &cur_conn[i] );
	    read_from_client_queue( &cur_conn[i] );
	    client_index++;
	    fprintf( stderr, "[Session %d Starting]\n", session++ );
	} 
	break;
case MSG$_ABORT:
case MSG$_EXIT:
case MSG$_DISCON:
	for (i=0; i<client_index; i++ ) {
	    if ( unit == cur_conn[i].cr_unit ) {
		close_down( &cur_conn[i] );
		mailbox_read_queue();
		return;
	    }
	}
	fprintf( stderr, "[Message %d (0x%x) received for unit %d (0x%x)]\n", msgtype, msgtype, unit, unit );
	break;
default:
	fprintf( stderr, "[Received message: %d (0x%x)]\n", msgtype, msgtype );
    }
    mailbox_read_queue();
}


read_from_server_queue( conn )
struct con_rec *conn;
{
int		read_from_server_ast(),
		read_from_client_ast(),
		status;

    if VMSFAIL( (sys$qio( 0, conn->cr_schan, (conn->cr_protocol==PROTO_DECNET?IO$_READVBLK|IO$M_MULTIPLE:IO$_READVBLK), 
	    conn->cr_sriosb, read_from_server_ast, conn, conn->cr_sbuf, xl_bufsiz, 0, 0, 0, 0 ) ) ) {
	if ( status == SS$_LINKABORT || status == SS$_LINKEXIT || status == SS$_LINKDISCON ) {
	    close_down( conn );
	    return;
	}
	VMSERR( (status) );
    }
}

read_from_client_queue( conn )
struct con_rec *conn;
{
int		read_from_server_ast(),
		read_from_client_ast(),
		status;

    if VMSFAIL( (sys$qio( 0, conn->cr_cchan, (conn->cr_protocol==PROTO_DECNET?IO$_READVBLK|IO$M_MULTIPLE:IO$_READVBLK), 
	    conn->cr_criosb, read_from_client_ast, conn, conn->cr_cbuf, xl_bufsiz, 0, 0, 0, 0 ) ) ) {
	if ( status == SS$_LINKABORT || status == SS$_LINKEXIT || status == SS$_LINKDISCON ) {
	    close_down( conn );
	    return;
	}
	VMSERR( (status) );
    }
}


read_from_server_ast( conn )
struct con_rec *conn;
{
int	status,
	len = conn->cr_sriosb[1],
	prefix[] = { conn->cr_session, 0, 0, XL_SERVER_DATA, conn->cr_sriosb[1] };

    if VMSFAIL( (conn->cr_sriosb[0]) ) {
	    errno = EVMSERR;
	    vaxc$errno = status;
	    close_down( conn );
	    return;
    }
    get_time( &prefix[XL_TIMESTAMP] );
    write( log_fd, prefix, sizeof( prefix ) );
    write( log_fd, conn->cr_sbuf, len );
    VMSERR( (sys$qiow( 0, conn->cr_cchan, IO$_WRITEVBLK, conn->cr_cwiosb, 0, 0, conn->cr_sbuf, len, 0, 0, 0, 0 ) ) );
    read_from_server_queue( conn );
}

read_from_client_ast( conn )
struct con_rec *conn;
{
int	status,
	len = conn->cr_criosb[1],
	prefix[] = { conn->cr_session, 0, 0, XL_CLIENT_DATA, conn->cr_criosb[1] };
                                             
    if VMSFAIL( (conn->cr_criosb[0]) ) {
	/*
	 * Cancel server i/o to stimulate run-down
	 */
	sys$cancel( conn->cr_schan );
	errno = EVMSERR;
	vaxc$errno = status ;
	close_down( conn );
	return;
    }
    get_time( &prefix[XL_TIMESTAMP] );
    write( log_fd, prefix, sizeof( prefix ) );
    write( log_fd, conn->cr_cbuf, len );
    VMSERR( (sys$qiow( 0, conn->cr_schan, IO$_WRITEVBLK, conn->cr_swiosb, 0, 0, conn->cr_cbuf, len, 0, 0, 0, 0 ) ) );
    read_from_client_queue( conn );
}


close_down( conn )
struct con_rec *conn;
{
    int	    prefix[] = { conn->cr_session, 0, 0, XL_LINK_ABORT, 0 };

    if ( conn->cr_active ) {
	conn->cr_active = 0;
	get_time( &prefix[XL_TIMESTAMP] );
	write( log_fd, prefix, sizeof( prefix ) );
	perror( "xliddy" );
	fprintf( stderr, "[Session %d Ending]\n", conn->cr_session );
	sys$dassgn( conn->cr_cchan );
	sys$dassgn( conn->cr_schan );
    }

}


get_time( timadr )
int	*timadr;
{
    int     status;

    VMSERR( (sys$gettim( timadr ) ) );
}



#ifndef	NO_TCP
inet_read_queue()
{
    int	inet_read_ast(),
	status;
    unsigned long   items[] = { sizeof( inet_accept_info ), inet_accept_info, &inet_accept_len };

    VMSERR( (sys$assign( &inet_dev_desc, &inet_chan, 0, 0 ) ) );
    VMSERR( (sys$qio( 0, inet_lchan, IO$_ACCESS|IO$M_ACCEPT, inet_iosb, inet_read_ast, 0, 0, 0, items, &inet_chan, 0, 0 ) ) );
}


inet_read_ast()
{
    char    	    host_name[16];
    unsigned long   stuff[4],
		    socktype = (UCX$C_STREAM << 16) | UCX$C_TCP;
    struct itm$item_list_3	items[3];
    struct SOCKADDRIN sin = { INET$C_AF_INET, 0, htonl( INET$C_INADDR_ANY ), 0,0,0,0,0,0,0,0 };
    long    lnlen = 0,
	    host_name_len = sizeof( host_name ) - 1;
    $DESCRIPTOR( host_name_desc, host_name );
    struct dsc$descriptor_s sin_desc = { sizeof( sin ), DSC$K_DTYPE_T, DSC$K_CLASS_S, &sin };
    int			info_len = 0,
			prefix[] = { 0, 0, 0, XL_LINK_CONNECT, 0 },
			i,
			status;

	VMSERR( (inet_iosb[0]) );
	if ( client_index < MAXCLIENTS ) {
	    i = client_index;
	    /*
	     * Get a channel for the server
	     */
	    VMSERR( (sys$assign( &inet_dev_desc, &cur_conn[i].cr_schan, 0, 0 ) ) );
	    cur_conn[i].cr_cchan = inet_chan;
	    if ( ( (cur_conn[i].cr_cbuf = malloc( xl_bufsiz )) == 0 ) ||
		 ( (cur_conn[i].cr_sbuf = malloc( xl_bufsiz )) == 0 ) ) VMSERR( (SS$_INSFMEM) );
	    /*
	     * Set socket options
	     */
	    VMSERR( (sys$qiow( 0, cur_conn[i].cr_schan, IO$_SETMODE, cur_conn[i].cr_swiosb, 0, 0,
		&socktype, INET$M_LINGER, &sin_desc, 0, 0, 0 ) ) );
	    VMSERR( (cur_conn[i].cr_swiosb[0] ) );
	    /*
	     * Now access
	     */
	    sin.SIN$W_PORT = htons( 6000 + xl_server );
	    sin.SIN$L_ADDR = inet_server_addr;
	    VMSERR( (sys$qiow( 0, cur_conn[i].cr_schan, IO$_ACCESS, cur_conn[i].cr_swiosb, 0, 0, 0, 0, &sin_desc,
		0, 0, 0 ) ) );
	    VMSERR( (cur_conn[i].cr_swiosb[0] ) );
	    /*
	     * We're in
	     */
	    cur_conn[i].cr_protocol = PROTO_TCP;
	    cur_conn[i].cr_id = i;
	    cur_conn[i].cr_active = 1;
	    cur_conn[i].cr_session = session;
	    prefix[XL_SESSION] = session;
	    get_time( &prefix[XL_TIMESTAMP] );
	    write( log_fd, prefix, sizeof( prefix ) );
	    read_from_server_queue( &cur_conn[i] );
	    read_from_client_queue( &cur_conn[i] );
	    client_index++;
	    fprintf( stderr, "[TCP/IP Session %d Starting]\n", session++ );
	} 
    inet_read_queue();
}


#endif	/* NO_TCP */
