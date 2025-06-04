#module SRHRMS "SRH V1.0-0"

/*
/*  SRHRMS.C
/*
/*  Program displays some RMS calls made from the c language.  Types out
/*  the calling process's SYS$LOGIN:LOGIN.COM file to SYS$OUTPUT.
/*
/*  Included is a main and three subroutines.  The subroutines open,
/*  read a record, and close the file.  Several hooks, such as the
/*  use of the NAM block to obtain the specification of the file that
/*  was actually opened, are included but are not currently used.
*/

#include <rms.h>
#include <stsdef.h>
#include <ssdef.h>

/*
/* SRH_MRS is the maximum record size that can be read (and thus
/* displayed) by this program.
*/
#define SRH_MRS	255

main()
    {
    int retstat;
    int context;
    char *bufadr;
    int buflen;

    /*
    /* Open the file.  Minimal checking is performed.  Read access only.
    */
    retstat = SRH_open_file( &context, "LOGIN", "SYS$LOGIN:.COM", 0, 0 );

    /*
    /* Read the file.  Minimal checking is performed.
    */
    for (;;)
	{
	retstat = SRH_read_file( &context, &bufadr, &buflen );
	if ( $VMS_STATUS_SUCCESS( retstat ) )
	    printf("%*.*s\n", buflen, buflen, bufadr );
	else
	    break;
	}

    /*
    /* Close up shop.
    */
    retstat = SRH_close_file( &context );
    }

/*
/* The following is the core data structure for the program.
/* The various RMS subroutines all communicate via a pointer
/* referencing this struct.
*/
struct SRH_file_context
    {
    struct FAB fab;
    struct RAB rab;
    struct NAM nam;
    char rss[NAM$C_MAXRSS];
    short max_rec_siz;
    char *data_buffer;
    };

SRH_open_file( ctxt_arg, file_name, default_file_name, flags, rss )
struct SRH_file_context **ctxt_arg;
char *file_name;
char *default_file_name;
int flags;
int *rss;
    {
    int retstat;
    struct SRH_file_context *ctxt;
    int howbig = sizeof( struct SRH_file_context );

    /*
    /* acquire some space for a context block.
    */
    retstat = LIB$GET_VM( &howbig, ctxt_arg, 0 );

    if ( !$VMS_STATUS_SUCCESS( retstat ) )
	return( retstat );

    ctxt = *ctxt_arg;

    /*
    /* Fill in the various fields of the context block.
    /* -- Builds the File Access Block (FAB), the Record Access
    /* Block (RAB) and the Name (NAM) Block.  Along with some
    /* other miscellaneous housekeeping stuff.
    */
    ctxt->fab = cc$rms_fab;
    ctxt->rab = cc$rms_rab;
    ctxt->nam = cc$rms_nam;

    ctxt->fab.fab$l_nam = &ctxt->nam;
    ctxt->fab.fab$l_fop = FAB$M_NAM;
    ctxt->fab.fab$b_fac = FAB$M_GET;

    ctxt->fab.fab$l_fna = file_name;
    ctxt->fab.fab$b_fns = strlen( file_name );
    ctxt->fab.fab$l_dna = default_file_name;
    ctxt->fab.fab$b_dns = strlen( default_file_name );

    ctxt->rab.rab$l_fab = &ctxt->fab;

    ctxt->nam.nam$b_rss = NAM$C_MAXRSS;
    ctxt->nam.nam$l_rsa = ctxt->rss;

    ctxt->rab.rab$b_rac = RAB$C_SEQ;

    /*
    /* Attempt to open the file...
    */
    retstat = SYS$OPEN( &ctxt->fab, 0, 0 );

    if ( !$VMS_STATUS_SUCCESS( retstat ) )
	return( retstat );

    /*
    /* Allocate a buffer large enough for the biggest record.
    */
    retstat = LIB$GET_VM( &SRH_MRS, &ctxt->data_buffer, 0 );

    /*
    /* Attempt to connect the record stream to the file...
    */
    retstat = SYS$CONNECT( &ctxt->rab, 0, 0 );

    return( retstat );

    }

SRH_read_file( ctxt_arg, bufadr, buflen )
struct SRH_file_context **ctxt_arg;
char **bufadr;
int *buflen;

    {
    int retstat;
    struct SRH_file_context *ctxt = *ctxt_arg;

    ctxt->rab.rab$l_ubf = ctxt->data_buffer;
    ctxt->rab.rab$w_usz = SRH_MRS;

    retstat = SYS$GET( &ctxt->rab, 0, 0 );

    if ( !$VMS_STATUS_SUCCESS( retstat ) )
	{
        *buflen = *bufadr = 0;
	return( retstat );
	}

    *bufadr = ctxt->rab.rab$l_rbf;
    *buflen = ctxt->rab.rab$w_rsz;

    return( retstat );
    }

SRH_close_file( ctxt_arg )
struct SRH_file_context **ctxt_arg;
    {
    int retstat;
    struct SRH_file_context *ctxt = *ctxt_arg;

    /*
    /* Free up the record buffer...
    */
    retstat = LIB$FREE_VM( &SRH_MRS, ctxt->data_buffer, 0 );

    if ( !$VMS_STATUS_SUCCESS( retstat ) )
	{
	return( retstat );
	}

    /*
    /* Be nice and clean up the record stream...
    */
    retstat = SYS$DISCONNECT( &ctxt->rab, 0, 0 );

    if ( !$VMS_STATUS_SUCCESS( retstat ) )
	return( retstat );

    /*
    /* And close the file...
    */
    retstat = SYS$CLOSE( &ctxt->fab, 0, 0 );

    if ( !$VMS_STATUS_SUCCESS( retstat ) )
	return( retstat );

    /*
    /* And free up the allocated memory...
    */
    retstat = LIB$FREE_VM( &sizeof( struct SRH_file_context ), ctxt_arg );

    return( retstat );

    }
