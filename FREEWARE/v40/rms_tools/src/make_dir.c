/*	MAKE_DIR.C

	Just for the heck of it, here is a mostly RMS program 
    to create and optionally pre-allocate a directory.  It is 
    'mostly' RMS because XAB$_UCHAR_DIRECTORY is XAB$K_SENSEMODE only.
    
    One really should call LIB$CREATE_DIR for this but for now
    (to be fixed in future VMS version) that does not take a
    size for input nor can be made Async.
    

    It could no should be 'mostly' XQP QIOWs but I like RMS better...
    (Actually, in some future VMS version there may well be a single
    XQP QIO to nicely create a dir file with no timing windows. )
    
    Note: fchdef can be found in SYS$LIBRARY:SYS$LIB_C.TLB
    
    	Have fun, Hein van den Heuvel. 24-Aug-1994.
*/
    
#include atrdef
#include fibdef
#include <fchdef>
#include iodef
#include rms
#include stdio
#include stdlib
#include string

struct fibdef fib;  /* cheap init to 0 */
int uchar;
int sys$create(), sys$open(), sys$close(), lib$stop();
int sys$connect(), sys$qiow(), sys$write();
main(argc,argv)
int argc;
char *argv[];
    {
    int status, channel;
    struct FAB fab;
    struct RAB rab;
    short iosb[4], buf[256] = { -1 } ;
    struct { int count; void *address; } fibdes = {sizeof fib, &fib};
    struct { short size, type; void *address;} atr[] = 
		{ ATR$S_UCHAR, ATR$C_UCHAR, &uchar, 0, 0, 0 };
    char usage[] = "Usage: $ MAKE_DIR directory_filename optional_size\n";

    if (argc < 2) printf("%s",&usage), exit(1);

    fab = cc$rms_fab;
    fab.fab$l_fna = argv[1];
    fab.fab$b_fns = strlen(fab.fab$l_fna);
    fab.fab$l_dna = ".DIR;1";
    fab.fab$b_dns = strlen(fab.fab$l_dna);
    fab.fab$l_fop = FAB$M_CTG;
    fab.fab$b_rat = FAB$M_BLK;
    fab.fab$b_fac = FAB$M_BIO | FAB$M_PUT | FAB$M_GET ;
    fab.fab$w_mrs = 512;
    rab = cc$rms_rab;
    rab.rab$l_fab = &fab;
    rab.rab$l_rbf = (char *) buf;
    rab.rab$w_rsz = sizeof buf;

    fab.fab$l_alq = 42; /* Thanks for all the fish */
    if (argc > 2) sscanf (argv[2], "%d", &fab.fab$l_alq );

    /* too bad XAB$_UCHAR_DIRECTORY is XAB$K_SENSEMODE only */
    
    status=sys$create(&fab);
    if (status & 1) status = sys$connect(&rab);
    if (status & 1) status = sys$write(&rab);
    if (status & 1) status = sys$close(&fab);
    
    fab.fab$l_fop = FAB$M_UFO;
    
    if (status & 1)  status = sys$open(&fab);
    if (!(status & 1)) lib$stop(status);
    
    channel = fab.fab$l_stv;
    uchar = FCH$M_DIRECTORY;

    status = sys$qiow(0,channel,IO$_DEACCESS,iosb,0,0,&fibdes,0,0,0,atr,0);
    if (status & 1) status = iosb[0];

    return status;
    }
                                                   
