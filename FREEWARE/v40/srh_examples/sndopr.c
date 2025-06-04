#include <descrip.h>
#include <iodef.h>
#include <opcdef.h>
#include <ssdef.h>
#include <stsdef.h>

#define OPC_BUFSIZ 1024
#define OPC_RQSTID 127
main()
    {
    unsigned short int chan;
    unsigned short int iosb[4];
    unsigned long int reqid;
    struct dsc$descriptor_s opc_d;
    unsigned long int retstat, dasstat;
    struct OPC *opc = calloc( 8, 1), *rply = calloc( OPC_BUFSIZ, 1 );

    opc_d.dsc$w_length = 12;
    opc_d.dsc$b_dtype = DSC$K_DTYPE_T;
    opc_d.dsc$b_class = DSC$K_CLASS_S;
    opc_d.dsc$a_pointer = opc;
 
    retstat = sys$crembx( 0, &chan,
	    OPC_BUFSIZ, (2 * OPC_BUFSIZ ),
	    0x0ff00, 0, 0);

    if ( !$VMS_STATUS_SUCCESS(retstat) )
	return retstat;
 
    *(unsigned long int *) opc = ~0;
    opc->opc$b_ms_type = OPC$_RQ_RQST;
    opc->opc$l_ms_rqstid = OPC_RQSTID;
    strcpy( &opc->opc$l_ms_text, "help" );
 
    printf("Please REPLY/PEND=nn to this request\n" );

    retstat = sys$sndopr( &opc_d, chan );

    if ( !$VMS_STATUS_SUCCESS(retstat) ) goto bail_out;
 
    retstat = sys$qiow( 0, chan, IO$_READVBLK, iosb, 0, 0, 
	    rply, OPC_BUFSIZ, 0, 0, 0, 0 );
    if (! $VMS_STATUS_SUCCESS(retstat)) goto bail_out;
    retstat = sys$synch( 0, iosb ); 
    if (! $VMS_STATUS_SUCCESS(iosb[0])) goto bail_out;
    
    *(unsigned long int *) opc = ~0;
    opc->opc$b_ms_type = OPC$_RQ_CANCEL;
    opc->opc$l_ms_rqstid = OPC_RQSTID;
 
    retstat = sys$sndopr( &opc_d, chan );

    if ( !$VMS_STATUS_SUCCESS(retstat) ) goto bail_out;
 
    retstat = sys$qiow( 0, chan, IO$_READVBLK, iosb, 0, 0, 
	    rply, OPC_BUFSIZ, 0, 0, 0, 0 );
    if (! $VMS_STATUS_SUCCESS(retstat)) goto bail_out;
    retstat = sys$synch( 0, iosb ); 
    if (! $VMS_STATUS_SUCCESS(iosb[0])) goto bail_out;

bail_out:
 
    dasstat = sys$dassgn( chan );
    return retstat;
    };
