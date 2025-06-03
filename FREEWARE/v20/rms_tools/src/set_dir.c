/*	SET_DIR.C

	Here is a litlle program (re)mark a file as Directory.
	To be used on directories that accidently (yeah right)
	got mismanaged with $SET FILE/NODIR.

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
int sys$open(), lib$stop(), sys$qiow();

main(argc,argv)
int argc;
char *argv[];
    {
    int status, channel, uchar;
    struct FAB fab;
    struct NAM nam;
    short iosb[4];
    struct { long count; void *address; } fibdes;
    struct fibdef fib;
    struct atrdef atr[2];
    char usage[] = "Usage: $ SET_DIR filename\n";

    if (argc != 2) printf("%s",&usage), exit(1);

    fab = cc$rms_fab;
    fab.fab$l_fna = argv[1];
    fab.fab$b_fns = strlen(fab.fab$l_fna);
    fab.fab$l_dna = ".DIR;1";
    fab.fab$b_dns = strlen(fab.fab$l_dna);
    fab.fab$b_fac = FAB$M_GET | FAB$M_PUT;
    fab.fab$l_fop = FAB$M_UFO;
    fab.fab$b_org = FAB$C_SEQ;	/* ebk and ffb are only valid for seq files */
    fab.fab$l_nam = &nam;
    nam = cc$rms_nam;

    status=sys$open(&fab);
    if (!(status & 1)) lib$stop(status);
    channel = fab.fab$l_stv;

    fibdes.count = sizeof(fib);
    fibdes.address = &fib;

    fib.fib$l_acctl  = ( FIB$M_WRITE );
    fib.fib$w_fid_num = nam.nam$w_fid[0];
    fib.fib$w_fid_seq =	nam.nam$w_fid[1];
    fib.fib$w_fid_rvn =	nam.nam$w_fid[2];

    atr[0].atr$w_type  = ATR$C_UCHAR;
    atr[0].atr$w_size  = ATR$S_UCHAR;
    atr[0].atr$l_addr  = &uchar;
    atr[1].atr$w_type  = 0;
    atr[1].atr$w_size  = 0;

/*  Get the file's current uchar */

    status = sys$qiow(0,channel,IO$_ACCESS,iosb,0,0,&fibdes,0,0,0,&atr,0);
    if (status & 1) status = iosb[0];
    if (!(status & 1)) lib$stop(status);

/*  Set the new uchar */

    uchar |= FCH$M_DIRECTORY;

    status = sys$qiow(0,channel,IO$_MODIFY,iosb,0,0,&fibdes,0,0,0,&atr,0);
    if (status & 1) status = iosb[0];
    if (!(status & 1)) lib$stop(status);

/*  Release the file */

    status = sys$qiow(0,channel,IO$_DEACCESS,iosb,0,0,&fibdes,0,0,0,0,0);

    }

