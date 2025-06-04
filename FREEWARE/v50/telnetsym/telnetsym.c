/*====================================================================
**
**  ABSTRACT:   (TELNETSYM.C V1.3)
**
**     This program is an example of how to customize the VMS supplied 
**     print symbiont to support printing over TCP/IP networks to a device
**     such as a telnet terminal server.  It's function is similar to the
**     LATSYM symbiont for LAT.
**      
**     This symbiont can be used to drive a printer connected to a port
**     on a DECserver 300 when the telnet listener options are enabled.
**     At present the symbiont does not negotiate TELNET options and will
**     simply ignore any option requests sent to it.  (This does not appear
**     to be a problem for the DECserver 300)  This symbiont has been
**     used with other terminal servers such as the CISCO STX10 and found
**     to function properly.
**
**     The symbiont was designed for unattended operation in a wide area
**     network.  When a print device becomes unreachable the symbiont will
**     periodically poll waiting for the device to become reachable, and
**     resume printing any queued jobs.  If a device becomes unreachable 
**     during the printing of a job, the remainder of the job's output
**     is discarded.
**
**     This symbiont is multithreaded and can accommodate up to 16 concurrent
**     threads.  Each thread corresponds to a print queue.  The symbiont is
**     fired up when the 1st queue is started using this symbiont as a 
**     processor.
**
**     Two functions of the VMS symbiont are replaced:
**
**	 JOB SETUP - used to open a socket to the TCP/IP port.  Unsuccessful
**       open attempts are retried via a TIMER AST and the PSM_PENDING option.
**       
**       OUTPUT - Sends the data to the socket.
**
** Using the Print Symbiont
**
**   An example of how to utilize this symbiont is given below.
**
**     Build the symbiont and copy to sys$system:
**
**       $ CC TELNETSYM
**       $ LINK TELNETSYM, SYS$SHARE:UCX$IPC/LIB, SYS$SHARE:VAXCRTL/LIB
**       $ COPY TELNETSYM.EXE SYS$SYSTEM:
**
**     To start a queue using the symbiont do the following:
**
**       $ INIT/QUE/PROCESS=TELNETSYM/START/ON="server_ip_nodename:port_number" 
**  
**     The value of "server_ip_nodename:port_number" for a printer connected to
**     a DECserver 300 with an IP nodename of SERVER listening on 
**     port 2007 would be:  "SERVER:2007"
**
**
**  ENVIRONMENT:
**	UCX V1.2 or higher, VMS V5.2 or higher 
**
**  AUTHOR:
**	Kris Anderson - Digital STO DTN 445-6552
**
**  CREATION DATE: Dec 1, 1991 
**
**  MODIFICATION HISTORY:
**
**     03-APR-1992   KAA  Handle "stop/que/...." cases properly
**
**     26-MAY-1992   KAA  Support PSM$K_WRITE_NOFORMAT for print/passall
**                       
**=============================================================================
*/

/*
   INCLUDE FILES
*/
#include  <errno.h>
#include  <types.h>
#include  <stdio.h>
#include  <socket.h>
#include  <in.h>
#include  <netdb.h>		/* change hostent to comply with BSD 4.3*/
#include  <inet.h>
#include  <ucx$inetdef.h>	/* INET symbol definitions */
#include  <prvdef.h>
#include  <ssdef>
#include  <iodef>
#include  <stsdef>
#include  <descrip>

/*
   A header file does not exist for the PSM$ constants, so these
   are declared here.  See SYS$LIBRARY:LIB.MLB ($PSMDEF) for the complete
   MACRO definition of these constants.
*/

#define PSM$K_INPUT 5
#define PSM$K_OUTPUT 7

#define	PSM$K_OPEN   4
#define	PSM$K_JOB_SETUP	8
#define	PSM$K_JOB_COMPLETION	22

#define	PSM$K_WRITE  9
#define PSM$K_WRITE_NOFORMAT 10
#define	PSM$K_START_TASK 16
#define	PSM$K_CLOSE  2
#define FAILURE -1

/*
    Miscellaneous constants
*/
#define DELAY_EF 1		    /* event flag */
#define MAX_SYMBIONT_STREAMS 16	    /* number of queues supported by 1 symbiont
				       process */
#define PACKET_SIZE 256	    /* largest packet written to socket */

/*
   Miscellaneous MACROs
*/
#define DISABLE_ASTS(status) status = sys$setast(0)
#define ENABLE_ASTS(status) { if (status == SS$_WASSET ) sys$setast(1); }


/*
  These VMS condition codes are pulled in externaly at link time
*/
globalvalue PSM$_FUNNOTSUP;
globalvalue PSM$_PENDING;
globalvalue PSM$_EOF;

/*
  Messages are logged to this file.
*/
static FILE logfile;

/*
   Each program thread has one of these structures.
*/
typedef struct work_area_struct 
   {
   int  chan;               		  /* IO channel to socket */
   char read_check_passed;	          /* flag */
   char stream_open;			  /* flag */
   int  retry_delta[2];		          /* VMS binary time */
   int  read_timeout[2];	          /* VMS binary time */
   int  link_idle_timeout[2];		  /* VMS binary time */
   char socket_address[80];
   } work_area_type;


typedef struct request_packet_struct
    {
    work_area_type *work_area;
    int request_id;
    short iosb[4];			    
    char read_buffer[256];
    } request_packet_type;


main ()
{
  int status;
  int code;
  int privs;
  int streams = MAX_SYMBIONT_STREAMS;  /* number of concurrent threads */
  int bufsiz = PACKET_SIZE; /* size of one network packet */
  int worksiz = sizeof ( struct work_area_struct );
  int output_routine();
  int job_setup_routine();
  int job_completion_routine();

  /*
    When VMS starts the symbiont only SETPRV is turned on.
    We need NETMBX & TMPMBX to do the TCP/IP stuff.
  */

  privs = PRV$M_NETMBX | PRV$M_TMPMBX;
  status = sys$setprv ( 1, &privs, 0, 0);
  if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );


  code = PSM$K_JOB_SETUP;
  status = psm$replace( &code, job_setup_routine );
  if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );

  code = PSM$K_OUTPUT;
  status = psm$replace( &code, output_routine );
  if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );

  /*
     Pass control to the VMS print symbiont.  Our routines will be
     called when needed.
  */

  status = psm$print ( &streams, &bufsiz, &worksiz );
  if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );
}




/*
**++
**  ROUTINE:
**
**    int job_setup_routine ( 
**         int *request_id, 
**         struct work_area_struct *work_area, 
**         int *f_code, 
**         struct dsc$descriptor *funcdesc, 
**         int *funcarg )
**
**
**  FUNCTIONAL DESCRIPTION:
**
**      This routine initializes the work area and prepares to start
**      printing a job by opening a socket to the printer port.
**      
**
**	The VMS print symbiont calls this routine 3 separate times prior 
**      to printing a job.   It is called with the following function codes:
**
**	    1)  PSM$K_OPEN
**	    2)  PSM$K_INPUT
**	    3)  PSM$K_CLOSE
**
**      The routine triggers on the open code to open the socket to the
**      tcp/ip port.
**
**
**  FORMAL PARAMETERS:
**
**      See the VMS print symbiont documenation
**
**  RETURN VALUE:
**
**      VMS condition code
**
**  SIDE EFFECTS:
**
**      None
**
**  DESIGN:
**
**      None
**
**
**
**--
*/
int job_setup_routine ( 
  int *request_id, struct work_area_struct *work_area, int *f_code, 
  struct dsc$descriptor *funcdesc, int *funcarg )
{
  int retval;
  int status;
  int ast_status;
  request_packet_type *request_packet;
  int open_socket_with_retry();

  switch ( *f_code )
   {
       case PSM$K_OPEN:
	  retval = PSM$_PENDING;

          status = sys$cantim ( work_area, 0 );
          if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );

	  request_packet = malloc ( sizeof(*request_packet) );
	    if ( ! request_packet ) 
	       return ( SS$_INSFMEM );

	  /* initialize the request packet and work area */
	  request_packet->work_area = work_area;
	  request_packet->request_id = *request_id;

	  if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );    

          status = sys$dclast ( open_socket_with_retry, request_packet, 0 );
	  if ( ! $VMS_STATUS_SUCCESS(status) ) retval = status;
	  break;

       case PSM$K_CLOSE:
	  retval = SS$_NORMAL;
	  break;

       default:
          retval = PSM$_FUNNOTSUP;
   }

  return ( retval );
}



/*
**++
**  ROUTINE:
**
**    int job_output_routine ( 
**         int *request_id, 
**         struct work_area_struct *work_area, 
**         int *f_code, 
**         struct dsc$descriptor *funcdesc, 
**         int *funcarg )
**
**
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called once when the queue is started with
**	a PSM$K_OPEN code, then multiple times with a PSM$K_OUTPUT code
**	when data is to be sent to a socket.
**
**	When called with the open code, the routine verifies the 
**	address port specification is in the following format:
**
**	    "x.x.x.x:yyyy"	where: x is an octet of an IP address
**				and y is the port to send output to.
**
**	    e.g. "192.67.209.100:2003"  
**   
**	This specification comes from the /ON= qualifier on the
**	START/QUEUE DCL command.
**
**	When called with the PSM$K_OUTPUT code, data is simply written
**      to the output socket.
**
**
**  FORMAL PARAMETERS:
**
**      See the VMS print symbiont documenation
**
**  RETURN VALUE:
**
**      VMS condition code
**
**  SIDE EFFECTS:
**
**      None
**
**  DESIGN:
**
**      None
**
**--
*/
int output_routine ( 
  int *request_id, struct work_area_struct *work_area, int *f_code, 
  struct dsc$descriptor *funcdesc, int *funcarg )
{
   request_packet_type *request_packet;
   int retval;
   int status;
   int ast_status;
   char msg[132];
   char host_str[132];
   char port_str[132];
   int output_completion_ast();
   int shutdown_link_ast();
   $DESCRIPTOR(retry_delta_asc,        "0 00:03:00.00");
   $DESCRIPTOR(link_idle_timeout_asc,  "0 00:02:00.00");
   $DESCRIPTOR(read_timeout_asc,       "0 00:00:05.00");

   retval = SS$_NORMAL;

   switch ( *f_code )
   {
       /* 
	  The PSM$K_OPEN case receives the value of the /on qualifier.  This
	  value is parsed and the contents are verified.
       */
       case PSM$K_OPEN:

	  memcpy ( work_area->socket_address, 
                   funcdesc->dsc$a_pointer, 
                   funcdesc->dsc$w_length );
	  work_area->socket_address[ funcdesc->dsc$w_length ] = '\0';

	  work_area->stream_open = 1;
	  work_area->chan = -1;

	  /* Check the validity of the address */
          if ( sscanf ( work_area->socket_address, "\"%[^:]:%[^\"]", 
                       host_str, port_str) < 2 )
             return ( SS$_IVDEVNAM );
	  
	  if ( gethostbyname (host_str) == NULL) 
             return ( SS$_IVDEVNAM );

	  /*  verify the number is a valid TCP/IP port number */
	  if ( atoi ( port_str ) < 0 || atoi ( port_str ) > 16000 ) 
             return ( SS$_IVDEVNAM );

	  status = sys$bintim (&retry_delta_asc, work_area->retry_delta );
          if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );

	  status = sys$bintim (&read_timeout_asc, work_area->read_timeout );
          if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );

	  status = sys$bintim 
               (&link_idle_timeout_asc, work_area->link_idle_timeout);
          if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );
	  break;

       case PSM$K_WRITE:
       case PSM$K_WRITE_NOFORMAT:
	  /*
	      Disable AST's while the output request is being queued
	  */
	  DISABLE_ASTS(ast_status);

	  retval = PSM$_PENDING;

	  /*  verify we have a channel open */
	  if ( work_area->chan < 0 ) 
	    retval = SS$_NOIOCHAN;
	  else
	    /* 
               if no data to write simply return successful status 
	    */
	    if ( funcdesc->dsc$w_length == 0 )
	       retval = SS$_NORMAL;
	    else 
	       {
		/* 
		    Setup to send data to the socket 
		*/
		request_packet = malloc ( sizeof (*request_packet) );
		if ( ! request_packet ) 
		    {
		    ENABLE_ASTS(ast_status);
		    return ( SS$_INSFMEM );
		    }

		request_packet->request_id = *request_id;
		request_packet->work_area = work_area;

		status = SYS$QIO(0,	                /* Event flag */
				work_area->chan,        /* Channel number */
				IO$_WRITEVBLK,          /* I/O function */
				request_packet->iosb,   /* I/O status block */
				output_completion_ast,  /* AST routine */
				request_packet,         /* AST parameter */
				funcdesc->dsc$a_pointer,/* P1 buffer */          
				funcdesc->dsc$w_length, /* P2 buffer length */
				0,0,0,0) ;

		if ( ! $VMS_STATUS_SUCCESS (status) )
		    {
		    log_error ("send", status);
	            cleanup ( work_area );
		    free ( request_packet );
	            retval = status;
	            }

		status = sys$cantim ( work_area, 0 );
		if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );
	        }

	  /*
	      Restore AST state
	  */
	  ENABLE_ASTS(ast_status);

	  break;
       case PSM$K_CLOSE:
	  if ( work_area->chan > 0 )
	    cleanup ( work_area );

	  work_area->stream_open = 0;

	  retval = SS$_NORMAL;
	  break;
       /*
	   Return the "function not supported" code for all
           unrecognized function codes.
       */
       default:
	  retval = PSM$_FUNNOTSUP;
	  break;
   }

   return ( retval );
}


/*
**++
**
**  ROUTINE:
**
**	int shutdown_link_ast( work_area_type *work_area )
**
**  FUNCTIONAL DESCRIPTION:
**
**	This AST routine is called as the result of the "link" being
**	inactive for a period of time.  This routine terminates the link.
**
**  FORMAL PARAMETERS:
**
**      [INPUT] work_area    pointer to the work area for the link.
**
**  RETURN VALUE:
**
**      N/A (AST routine)
**
**  SIDE EFFECTS:
**
**      None
**
**  DESIGN:
**
**      None
**
**--
*/
int shutdown_link_ast( work_area_type *work_area )
{
    int status;

    status = sys$cantim ( work_area, 0 );
    if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );

    if (work_area->chan > 0) 
	{
	cleanup( work_area );
	}
}




/*
**++
**  ROUTINE
**
**       open_socket_with_retry ( 
**           struct work_area_struct *work_area )
**
**  FUNCTIONAL DESCRIPTION:
**
**     This routine executes at AST level and opens a socket to the 
**     destination IP address/socket.  If the open attempt is unsuccesful 
**     then a TIMER is set to retry later.  When retry is required 
**     PSM$_PENDING is returned.
**
**  FORMAL PARAMETERS:
**
**	[MODIFY] request_packet->work_area->chan     
**
**	    Should be less than zero to indicate the channel NOT open,
**	    when called the first time.
**
**      [INPUT] request_packet->request_id
**
**	   used as the request_id to PSM$REPORT when work_area->report_status
**	   is non-zero.
**
**      [INPUT] request_packet->open_request_in_progress
**
**	   A flag that is set when an open request has not yet been 
**	   satisfied.
**
**  RETURN VALUE:
**
**      This routine executes as an AST routine, return value N/A
**
**  SIDE EFFECTS:
**
**      None
**
**  DESIGN:
**
**      None
**
**--
*/ 
int open_socket_with_retry ( request_packet_type *request_packet )
{
  int status;
  char try_again = 0;    /* flag */
  work_area_type *work_area = request_packet->work_area;
  int read_completion_ast();
  int read_timeout_ast();

  /*
     Check for spurious timer interrupt
  */
  if ( ! work_area->stream_open )
       {
       free( request_packet );
       return ( SS$_NORMAL );
       }

  /*
     If a connection does not exists make an attempt to establish a connection 
  */
  if ( work_area->chan < 0 )
     {
     work_area->chan = open_socket ( work_area );
     if ( work_area->chan < 0 ) try_again = 1;
     }

  /*
     if a connection does exists make sure the read check has passed.

     The DECserver 300 will accept a connection even if it is
     busy.  The next read or write will disconnect the link.
     Therefore test for this condition by issuing a read.  
	    
     Using this technique is not as clean as I would like, but 
     unfortunately only reading or writing to the socket will 
     reveal whether a true connection has been established.
  */	    

  if ( work_area->chan > 0 )
     if ( work_area->read_check_passed )
	{
	status = psm$report ( &request_packet->request_id );		  
	if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );    

	free ( request_packet );
	}
     else
	{
	status = SYS$QIO (  0,		           /* Event flag */
             work_area->chan,		           /* Channel number */
             IO$_READVBLK,		           /* I/O function */
             request_packet->iosb,                 /* I/O status block */
             read_completion_ast, 
	     request_packet,
             request_packet->read_buffer,            /* P1 buffer */          
             sizeof( *request_packet->read_buffer ), /* P2 buffer length */
             0,0,0,0) ;
        if ( ! $VMS_STATUS_SUCCESS (status) )
	    {
            log_error( "read", status ); 
	    cleanup ( work_area );
	    try_again = 1;
	    }
	else
	    {
	    /*
	      The read will hang for the CISCO STSX10 since no data is
              transmitted by this device.  Therefore a timeout on the read 
              is implemented.
            */
            status = sys$setimr ( 0,  work_area->read_timeout, 
	        read_timeout_ast, work_area, 0);
            if ( ! $VMS_STATUS_SUCCESS (status) ) lib$stop ( status );
	    }
	}

  /*
     If unsuccessful then set a timer and retry later.
  */
  if ( try_again )
     {
     status = sys$setimr (
        0, work_area->retry_delta, open_socket_with_retry, request_packet, 0);
     if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );    
     }
}


/*
**++
**
**  ROUTINE:
**
**	int read_timeout_ast ( work_area_type *work_area )
**
**  FUNCTIONAL DESCRIPTION:
**
**	This AST routine is called should the read check timeout.
**      SYS$CANCEL is used to force the read to terminate.
**
**  FORMAL PARAMETERS:
**
**      [INPUT] work_area    pointer to the work area for the link.
**
**  RETURN VALUE:
**
**      N/A (AST routine)
**
**  SIDE EFFECTS:
**
**      None
**
**  DESIGN:
**
**      None
**
**--
*/
read_timeout_ast ( work_area_type *work_area )
{
  int status;

  if ( work_area->chan > 0 ) 
      {
      status = sys$cancel ( work_area->chan );
      if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );    
      }
}


/*
**++
**
**  ROUTINE:
**
**	int read_completion_ast ( request_packet_type *request_packet )
**
**  FUNCTIONAL DESCRIPTION:
**
**      This function is called when the "read check" IO completes.  If
**	the IO status block indicates that the read was cancelled this is
**      not considered an error, since this condition occurs when a device
**      does not send any data. 
**
**  FORMAL PARAMETERS:
**
**      [OUTPUT] request_packet->work_area->read_check_passed  
**				       set to true if the check passed
**			    
**
**  RETURN VALUE:
**
**      N/A (AST routine)
**
**  SIDE EFFECTS:
**
**      None
**
**  DESIGN:
**
**      None
**
**--
*/
read_completion_ast ( request_packet_type *request_packet )
{
  int status;
  work_area_type *work_area = request_packet->work_area;

  status = sys$cantim( work_area, 0 );
  if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );    

  status = request_packet->iosb[0];
  /*
    If an error occurred set a timer interrupt to try again later.

    Abort and Cancel codes are not treated as errors.  They are the result
    of canceling IO on the channel when the read timed out.
  */
  if ( ! $VMS_STATUS_SUCCESS (status) 
	&& status != SS$_ABORT && status != SS$_CANCEL )
     {
     log_error( "read", status );
     if ( work_area->chan > 0 ) cleanup ( work_area );

     status = sys$setimr (
        0, work_area->retry_delta, open_socket_with_retry, request_packet, 0);
     if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );    
     }
  else
     {
     work_area->read_check_passed = 1;

     status = psm$report ( &request_packet->request_id );		  
     free ( request_packet );
     }
}



/*
**++
**
**  ROUTINE:
**
**	int output_completion_ast ( 
**		   request_packet_type *request_packet )
**
**  FUNCTIONAL DESCRIPTION:
**
**	This AST routine is called after a block of data has been written to
**	a socket.  
**
**  FORMAL PARAMETERS:
**
**      [INPUT] request_packet->iosb        VMS condition code indicating 
**					    success or failure
**
**	[INPUT]	request_packet->chan        IO channel of the socket
**
**	[INPUT] request_packet->request_id  The symbiont request id for 
**					    the output function.
**
**  RETURN VALUE:
**
**      N/A (AST routine)
**
**  SIDE EFFECTS:
**
**      None
**
**  DESIGN:
**
**      None
**
**--
*/
int output_completion_ast ( request_packet_type *request_packet )
{
   int status;
   int shutdown_link_ast();
   work_area_type *work_area = request_packet->work_area;
   
   status = sys$cantim ( work_area, 0 );
   if ( ! $VMS_STATUS_SUCCESS(status) ) lib$stop ( status );

   if ( ! $VMS_STATUS_SUCCESS(request_packet->iosb[0]) )
	{
	log_error ( "output", (int) request_packet->iosb[0] );
	/* 
	    If the channel is still assigned then cleanup the link.
	*/
	if ( work_area->chan > 0 ) cleanup ( work_area );
	}	    
   else
	{
	/*
	    Set a timer to disconnect the link if idle for a long period of
	    time.
	*/  
	sys$setimr (
           0, work_area->link_idle_timeout, shutdown_link_ast, work_area, 0);
	}
   
   status = psm$report ( &request_packet->request_id );
   if ( ! $VMS_STATUS_SUCCESS (status) ) lib$stop ( status );

   /*
     NOTE: The request_packet is allocated by the send_data_to_socket routine.
   */       
   free ( request_packet );

   return ( status ); /* not truly necessary for an AST routine */
}




/*
**++
**
**  ROUTINE:
**
**	int open_socket (char *socket_address )
**
**  FUNCTIONAL DESCRIPTION:
**
**	This routine attempts to open a socket to the print device.  The
**	code is a modified version of the UCX example code,
**	"UCX$TCP_SERVER_IPC.C".  
**
**  FORMAL PARAMETERS:
**
**	[INPUT] char *socket_address	a character string which specifies
**					the destination IP address and
**					port.  Format is as follows:
**
**					    "ip_node:tcp_port"
**		    
**
**  RETURN VALUE:
**
**	-1 if an error occured
**	if no error then the channel to the socket
**
**  SIDE EFFECTS:
**
**      None
**
**  DESIGN:
**
**      None
**
**--
*/

int open_socket (work_area_type *work_area )
{
    /* Structures used to pass parameters to QIO calls */
    struct socket_descriptor_struct 
       {
       int length ;
       struct sockaddr *sock ;
       };

    short channel ;         /* INET channel */
    short sck_parm[2] ;     /* Socket creation parameter */
    char data[512];
    char *socket_address = work_area->socket_address;

    struct sockaddr_in remote_host;
    
    struct socket_descriptor_struct socket_desc = 
           { sizeof ( remote_host ), &remote_host };

    char remote_addr[32];
    int remote_addr_len;

    struct  hostent         hostentstruct;  /* Storage for hostent data.  */
    struct  hostent         *hostentptr;    /* Pointer to hostent data.   */
    short   iosb [4];        /* I/O status block           */
    int	    status;
    char    host_str[128];
    char    port_str[128];
    char    msg[128];

    struct dsc$descriptor dev =
            { 3, DSC$K_CLASS_S, DSC$K_DTYPE_T, "BG:"} ;     /* INET device */

    /*
	Parse the host and port str from the input string.
	NOTE:   the quotation marks are expected to be in the string
    */
    sscanf (socket_address, "\"%[^:]:%[^\"]", host_str, port_str);

    sprintf ( msg, "Open %s", socket_address );

    /*
     * Initialize the parameters. 
     */
    sck_parm[0] = INET$C_TCP ;     		/* TCP/IP protocol */
    sck_parm[1] = INET_PROTYP$C_STREAM ; 	/* stream type of socket */


    /*
     * Get pointer to network data structure for socket 2 (remote host). 
     */
    if ((hostentptr = gethostbyname (host_str)) == NULL) 
	{
        log_error( "gethostbyname");
	return ( FAILURE );
	}

    /*
     * Copy hostent data to safe storage. 
     */
    hostentstruct = *hostentptr;

    /*
     * Fill in the name & address structure for socket 2. 
     */
    remote_host.sin_family = hostentstruct.h_addrtype;
    remote_host.sin_port = htons( atoi ( port_str ) ); 
    remote_host.sin_addr = * ((struct in_addr *) hostentstruct.h_addr);

    /* 
     * Assign a channel to INET device. 
     */
    status = SYS$ASSIGN( &dev, &channel, 0, 0) ;
    if ( ! $VMS_STATUS_SUCCESS(status) ) 
	{
	log_error( msg, status );
	return ( FAILURE );
	}

    work_area->chan = channel;

    /*
     * Create and bind the device socket to local host.
     */
    status = SYS$QIOW(  0,           /* Event flag */
	                channel,     	/* Channel number */
                        IO$_SETMODE,    /* I/O function */
                        iosb,        	/* I/O status block */
                        0, 0,
                        &sck_parm,   	/* P1 Socket creation parameter */
			0x01000000 | SOCKOPT$M_REUSEADDR,  
					/*P2 - by value with command bit=set */
                        0,     /* P3 Socket bind parameter */
                        0,0,0) ;

    if ( $VMS_STATUS_SUCCESS (status) ) status = iosb[0];
    if ( ! $VMS_STATUS_SUCCESS(status) )
	{
	log_error( msg, status );
	cleanup ( work_area );
	return ( FAILURE );
	}

    /*
     * Define the peer socket name and address-Connect.
     */
    status = SYS$QIOW(  0,        /* Event flag */
                        channel,     /* Channel number */
                        IO$_ACCESS,  /* I/O function */
                        iosb,        /* I/O status block */
                        0,0,
                        0,0,
                        &socket_desc, /* P3 Remote IP address*/
			0,0,0);

    if ( $VMS_STATUS_SUCCESS (status) ) status = iosb[0];
    if ( ! $VMS_STATUS_SUCCESS (status) )
	{
	log_error( msg, status ); /* msg is setup at begining of routine */
	cleanup ( work_area );
	return ( FAILURE );
	}

    return ( channel );
}


/*
**++
**
**  ROUTINE:
**
**	cleanup ( work_area_type *work_area )
**
**  FUNCTIONAL DESCRIPTION:
**
**	This routine closes a socket by cancelling all IO on the socket 
**	channel and deassigning the channel.
**
**  FORMAL PARAMETERS:
**
**      [MODIFY]	work_area->channel	    IO channel of socket
**
**  RETURN VALUE:
**
**      None
**
**  SIDE EFFECTS:
**
**      None
**
**  DESIGN:
**
**      None
**
**--
*/
cleanup ( work_area_type *work_area )
{
    int status;
    int channel = work_area->chan;
    short iosb [4] ;        /* I/O status block  */

    /*
	Cancel all IO currently on the channel first, such as the
	asynchronous read.
    */
    status = sys$cancel ( channel );
    if ( ! $VMS_STATUS_SUCCESS (status) )
	{
	log_error ( "cancel", status );
	}
    /*
     * Close the socket.
     */
    status = SYS$QIOW( 0,
                       channel,
                       IO$_DEACCESS,
                       iosb,
		       0,0,0,0,0,0,0,0) ;
    if ( $VMS_STATUS_SUCCESS (status) ) status = iosb[0];
    if ( ! $VMS_STATUS_SUCCESS (status) )
	{
	log_error ( "deaccess", status );
	}

    /*
      Finished with the channel, so free it.
    */
    status = SYS$DASSGN (channel);
    if ( ! $VMS_STATUS_SUCCESS (status) )
	{
	log_error ( "deassign", status );
	}

    work_area->chan = -1;
    work_area->read_check_passed = 0;

    return ( SS$_NORMAL );
}


/*
**++
**
**  ROUTINE:
**
**	int log_message ( char *msg )
**
**  FUNCTIONAL DESCRIPTION:
**
**    This routine logs a message to the file defined by the logical
**    TELNETSYM_LOGFILE.  If the logical is not defined then
**    the message is logged to SYS$SYSTEM:TELNETSYM.LOG.  The current
**    date and time is prefixed to all messages.
**
**  FORMAL PARAMETERS:
**
**      [INPUT] char *msg	message to write to logfile
**
**  RETURN VALUE:
**
**      None
**
**  SIDE EFFECTS:
**
**      None
**
**  DESIGN:
**
**      None
**
**--
*/
log_message ( char *msg )
{
  FILE *logfile;
  int  current_time;
  char *current_time_str;
  char *log_file_name;
  int time_len;

  if ( ! (log_file_name = getenv ("TELNETSYM_LOGFILE")) )  
      log_file_name = "TELNETSYM.LOG";

  logfile = fopen ( log_file_name, "a" );
  if ( ! logfile )
     {
     lib$stop ( vaxc$errno );
     }

  time ( &current_time );
  current_time_str = ctime ( &current_time );
  time_len = strlen ( current_time_str );

  fprintf (logfile, "%.*s, %s\n", time_len - 1, current_time_str, msg);

  fclose (logfile);
}






/*
**++
**
**  ROUTINE:
**
**	int log_error ( char *prefix, 
**			[int vms_status] )
**
**  FUNCTIONAL DESCRIPTION:
**
**      This routine is similar to perror(), except its output is
**	sent to the logging file.
**
**  FORMAL PARAMETERS:
**
**
**	[INPUT]	char *prefix	    pointer to a character string which will
**				    be printed prior to printing an error
**				    message.
**
**	[INPUT] int vms_status	    [optional] VMS condition code to use
**				    for error text retrieval
**				    
**  RETURN VALUE:
**
**      None
**
**  SIDE EFFECTS:
**
**      None
**
**  DESIGN:
**
**      None
**
**--
*/
int log_error ( char *prefix, int vms_status )
{
   char   msg[256];
   int	  argc;


   /*
     Note: This is a VAX specific, non-transportable way of determing the 
     number of arguments passed to a routine.  It is used here because
     the va_count macro is not available in all versions of VAXC.
   */
   argc = *((int *) &prefix - 1);

   if ( argc == 2 ) 
      sprintf (msg, "%s: %s", prefix, strerror ( EVMSERR, vms_status ) );
   else if ( errno == EVMSERR ) 
      sprintf (msg, "%s: %s", prefix, strerror ( EVMSERR, vaxc$errno ) );
   else
      sprintf (msg, "%s: %s", prefix, strerror ( errno ) );
   log_message ( msg );
}


