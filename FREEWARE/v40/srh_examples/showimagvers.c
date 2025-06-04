/*
//  Demonstrates an undocumented interface that can decode
//  an OpenVMS VAX image header.  (OpenVMS Alpha offsets are 
//  often similar -- VAX uses IHD$ symbols while Alpha uses
//  EIHD$ symbols, etc.)
*/
#include <descrip.h>
#include <rms.h>
#include <ssdef.h>
#include <stsdef.h>

#define SRH$$K_FN_SRH$RTL   "SRH$RTL"
#define SRH$$K_DN_SRH$RTL   "SYS$SHARE:.EXE"
#define SRH$$K_FN_SRH$CP    "SRH$CP"
#define SRH$$K_DN_SRH$CP    "SYS$SYSTEM:.EXE"
#define SRH$$K_FN_SRH$ACP   "SRH$ACP"
#define SRH$$K_DN_SRH$ACP   "SYS$SYSTEM:.EXE"

#define	    IHD$C_NATIVE	-1
#define	    IHD$W_IMGIDOFF	6
#define	    IHI$T_IMGID		40
#define	    IHI$Q_LINKTIME	56
#define	    IHI$S_LINKTIME	56

SRH$show_version()
    {
    SRH$$decode_ihd( SRH$$K_FN_SRH$RTL, SRH$$K_DN_SRH$RTL );
    SRH$$decode_ihd( SRH$$K_FN_SRH$ACP, SRH$$K_DN_SRH$ACP );
    SRH$$decode_ihd( SRH$$K_FN_SRH$CP, SRH$$K_DN_SRH$CP );
    return( SS$_NORMAL );
    }


SRH$$decode_ihd( img_fn, img_dn )
char *img_fn, *img_dn;
    {
    unsigned long int retstat;
    unsigned long int offset = 12;
    unsigned long int vbn = 1;
    struct FAB fab = cc$rms_fab;
    struct NAM nam = cc$rms_nam;
    char blk0[ 512 ];
    char ihd[ 1024 ];
    char *ihi;
    unsigned long int hdrvers = 2;
    unsigned long int last_word = 3;
    struct dsc$descriptor_s asctimbuf;

    asctimbuf.dsc$w_length = 23;
    asctimbuf.dsc$b_dtype = DSC$K_DTYPE_T;
    asctimbuf.dsc$b_class = DSC$K_CLASS_S;
    asctimbuf.dsc$a_pointer = calloc( 24, 1 );

    fab.fab$l_nam = &nam;
    nam.nam$l_rsa = calloc( nam.nam$b_rss = NAM$C_MAXRSS, 1 );
    fab.fab$b_fac = FAB$M_GET;
    fab.fab$l_fop = FAB$M_UFO | FAB$M_NAM;
    fab.fab$b_fns = strlen( img_fn );
    fab.fab$l_fna = img_fn;
    fab.fab$b_dns = strlen( img_dn );
    fab.fab$l_dna = img_dn;
    retstat = SYS$OPEN( &fab );
    
    retstat = IMG$DECODE_IHD(
	fab.fab$l_stv, blk0, ihd, &vbn, &offset, &hdrvers, &last_word );

    retstat = SYS$CLOSE( &fab );

    if ( last_word != (unsigned short int) IHD$C_NATIVE )
	printf( "File: %*s is not a native-mode image.\n",
	    nam.nam$b_rsl, nam.nam$l_rsa );

    printf( "File: %*s ",
	    nam.nam$b_rsl, nam.nam$l_rsa );

    ihi = ihd + *(short *)((char *)ihd + IHD$W_IMGIDOFF);
    printf( " ident: %*s\n",
	    (char) ihi[IHI$T_IMGID], &ihi[IHI$T_IMGID+1] );

    retstat = SYS$ASCTIM(
	0, &asctimbuf, &ihi[IHI$Q_LINKTIME], 0 );
    asctimbuf.dsc$a_pointer[23] = 0;
    printf( " linktime: %23s\n", asctimbuf.dsc$a_pointer );

    cfree( asctimbuf.dsc$a_pointer );
    cfree( nam.nam$l_rsa );

    return( SS$_NORMAL );

    }
