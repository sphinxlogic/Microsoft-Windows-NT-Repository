/********************************************************************************************************************************/
/* Created:  9-JUL-1992 15:31:38 by OpenVMS SDL EV1-11     */
/* Source:   9-JUL-1992 15:30:04 CRTL$:[ROUTLEY.CRTL.SDLFIX.ALPHAFIX]LBRUSR.SDI; */
/********************************************************************************************************************************/
/*** MODULE $MHDDEF ***/
#ifndef __MHDDEF_LOADED
#define __MHDDEF_LOADED 1
 
#pragma nostandard
#pragma member_alignment __save
#pragma nomember_alignment
 
#ifdef __cplusplus
    extern "C" {
#define __unknown_params ...
#define __struct struct
#define __union union
#else
#define __unknown_params
#define __struct variant_struct
#define __union variant_union
#endif
 
#define MHD$C_MHDID 173                 /* Value that must be in the ident  */
#define MHD$K_REFLNG 8                  /* Length of record to end of ref count  */
#define MHD$C_REFLNG 8                  /* Length of record to end of ref count  */
#define MHD$K_INSTIME 8                 /* Label for start of insert time   */
#define MHD$C_INSTIME 8                 /* Label for start of insert time   */
#define MHD$K_USRDAT 16                 /* Start of user additional header data  */
#define MHD$C_USRDAT 16                 /* Start of user additional header data  */
#define MHD$K_MHDLEN 16                 /* Length of fixed part of MHD      */
#define MHD$C_MHDLEN 16                 /* Length of fixed part of MHD      */
#define MHD$M_SELSRC 0x1
#define MHD$M_OBJTIR 0x2
#define MHD$K_OBJIDENT 18               /*                                  */
#define MHD$C_OBJIDENT 18               /*                                  */
struct mhddef {
    unsigned char mhd$b_lbrflag;        /* Librarian-controlled flag byte   */
    unsigned char mhd$b_id;             /* Ident                            */
    short int mhddef$$_fill_1;          /* Reserved word                    */
    unsigned int mhd$l_refcnt;          /* Reference count                  */
    unsigned int mhd$l_datim;           /* Date/time inserted               */
    __union  {
        int mhddef$$_fill_2;            /* ...                              */
        __struct  {
            char mhddef$$_fill_3 [4];
            char mhd$b_usrdat [1];       /* Start of user additional header data  */
            } mhd$r_fill_2_fields;
        } mhd$r_fill_2_overlay;
    __union  {
        unsigned char mhd$b_objstat;    /* Status of object module          */
        __struct  {
            unsigned mhd$v_selsrc : 1;  /* Selective search                 */
            unsigned mhd$v_objtir : 1;  /* Module contains TIR records      */
            unsigned mhd$v_fill_1 : 6;
            } mhd$r_objstat_bits;
        } mhd$r_objstat_overlay;
    __union  {
        unsigned char mhd$b_objidlng;   /* Length of ident                  */
        __struct  {
            char mhddef$$_fill_4;
            char mhd$t_objident [1];     /* Object module ident              */
            } mhd$r_objidlng_fields;
        } mhd$r_objidlng_overlay;
    } ;
#ifdef __cplusplus
#define mhd$b_usrdat mhd$r_fill_2_overlay.mhd$r_fill_2_fields.mhd$b_usrdat
#define mhd$b_objstat mhd$r_objstat_overlay.mhd$b_objstat
#define mhd$v_selsrc mhd$r_objstat_overlay.mhd$r_objstat_bits.mhd$v_selsrc
#define mhd$v_objtir mhd$r_objstat_overlay.mhd$r_objstat_bits.mhd$v_objtir
#define mhd$b_objidlng mhd$r_objidlng_overlay.mhd$b_objidlng
#define mhd$t_objident mhd$r_objidlng_overlay.mhd$r_objidlng_fields.mhd$t_objident
#endif
 
#ifdef __cplusplus
    }
#endif
#pragma standard
#pragma member_alignment __restore
 
#endif /* __MHDDEF_LOADED */
 
