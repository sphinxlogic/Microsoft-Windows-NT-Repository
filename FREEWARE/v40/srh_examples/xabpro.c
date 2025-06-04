#include descrip
#include rms
#include ssdef
#include stdio

main()
    {
    struct dsc$descriptor_s fname_d =
	{
	NAM$C_MAXRSS, DSC$K_DTYPE_T, DSC$K_CLASS_S, malloc( NAM$C_MAXRSS )
	};
    $DESCRIPTOR( prompt_d, "FILENAME: ");
    struct FAB fab;
    struct XABPRO xabpro;
    int retstat;

    retstat = lib$get_input( &fname_d, &prompt_d, &fname_d.dsc$w_length );

    /*
    **	Initialize the FAB and XABPRO structures.
    */
    fab = cc$rms_fab;
    xabpro = cc$rms_xabpro;

    fab.fab$l_fna = fname_d.dsc$a_pointer;
    fab.fab$b_fns = fname_d.dsc$w_length;
    fab.fab$b_fac = FAB$M_PUT;

    /*
    **	Open the file, then hook in the XABPRO.
    */
    retstat = sys$open( &fab );
    printf("$OPEN = 0%%x%08x\n", retstat );
    printf("  stv = 0%%x%08x\n", fab.fab$l_stv );

    fab.fab$l_xab = &xabpro;
    xabpro.xab$w_pro = (XAB$M_NOWRITE|XAB$M_NODEL)<<XAB$V_WLD;
    xabpro.xab$l_nxt = 0;

    /*
    **	Close the file.
    */
    retstat = sys$close( &fab );
    printf("$CLOSE = 0%%x%08x\n", retstat );
    printf("   stv = 0%%x%08x\n", fab.fab$l_stv );

    /*
    **	Clean up and leave...
    */
    free( fname_d.dsc$a_pointer );

    return SS$_NORMAL;
    }
