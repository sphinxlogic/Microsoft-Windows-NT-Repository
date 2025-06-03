/**/
/******************************************************************************/
/**                                                                          **/
/**  Copyright (c) 1994                                                      *1994                                                */
/**  by DIGITAL Equipment Corporation, Maynard, Mass.                        **/
/**                                                                          **/
/**  This software is furnished under a license and may be used and  copied  **/
/**  only  in  accordance  with  the  terms  of  such  license and with the  **/
/**  inclusion of the above copyright notice.  This software or  any  other  **/
/**  copies  thereof may not be provided or otherwise made available to any  **/
/**  other person.  No title to and ownership of  the  software  is  hereby  **/
/**  transferred.                                                            **/
/**                                                                          **/
/**  The information in this software is subject to change  without  notice  **/
/**  and  should  not  be  construed  as  a commitment by DIGITAL Equipment  **/
/**  Corporation.                                                            **/
/**                                                                          **/
/**  DIGITAL assumes no responsibility for the use or  reliability  of  its  **/
/**  software on equipment which is not supplied by DIGITAL.                 **/
/**                                                                          **/
/******************************************************************************/
/********************************************************************************************************************************/
/* Created:  1-APR-1994 18:41:41 by OpenVMS SDL EV1-33     */
/* Source:  19-AUG-1993 17:25:21 $64$DUA213:[LIB_H.SRC]F11DEF.SDL;1 */
/********************************************************************************************************************************/
/*** MODULE $FCHDEF ***/
#ifndef __FCHDEF_LOADED
#define __FCHDEF_LOADED 1
 
#pragma nostandard
#pragma member_alignment __save
#pragma nomember_alignment
 
#ifdef __cplusplus
    extern "C" {
#define __unknown_params ...
#else
#define __unknown_params
#endif
 
#if !defined(__VAXC) && !defined(VAXC)
#define __struct struct
#define __union union
#else
#define __struct variant_struct
#define __union variant_union
#endif
 
/*+                                                                         */
/*                                                                          */
/* File characteristics bit definitions. These are identical to, and must   */
/* track, the bits in FILECHAR above, but are defined relative to the file  */
/* characteristics longword instead of relative to the file header.         */
/*                                                                          */
/*-                                                                         */
#define FCH$M_VCC_STATE 0x700
#define FCH$M_ASSOCIATED 0x100000
#define FCH$M_EXISTENCE 0x200000
#define FCH$M_WASCONTIG 0x1
#define FCH$M_NOBACKUP 0x2
#define FCH$M_WRITEBACK 0x4
#define FCH$M_READCHECK 0x8
#define FCH$M_WRITCHECK 0x10
#define FCH$M_CONTIGB 0x20
#define FCH$M_LOCKED 0x40
#define FCH$M_CONTIG 0x80
#define FCH$M_BADACL 0x800
#define FCH$M_SPOOL 0x1000
#define FCH$M_DIRECTORY 0x2000
#define FCH$M_BADBLOCK 0x4000
#define FCH$M_MARKDEL 0x8000
#define FCH$M_NOCHARGE 0x10000
#define FCH$M_ERASE 0x20000
#define FCH$M_SHELVED 0x80000
#define FCH$M_SCRATCH 0x100000
#define FCH$M_NOMOVE 0x200000
#define FCH$M_NOSHELVABLE 0x400000
#define FCH$S_FCHDEF 4                  /* Old size name - synonym          */
typedef struct _fch {
    __union  {
        int fch$$_fill_1;
        __struct  {
            unsigned fch$$_fill_31 : 8; /* reserved                         */
            unsigned fch$v_vcc_state : 3; /* VCC state bits                 */
            unsigned fch$$_fill_32 : 7; /* reserved                         */
/***********The following line is different from FH2                        */
            unsigned fch$$_alm_state : 2; /* ALM state bits removed         */
            unsigned fch$v_associated : 1; /* ISO 9660 Associated file      */
            unsigned fch$v_existence : 1; /* ISO 9660 Existence file        */
            unsigned fch$v_fill_6 : 2;
            } fch$r_fill_1_chunks;
        __struct  {
            unsigned fch$v_wascontig : 1; /* file was (and should be) contiguous */
            unsigned fch$v_nobackup : 1; /* file is not to be backed up     */
            unsigned fch$v_writeback : 1; /* file may be write-back cached  */
            unsigned fch$v_readcheck : 1; /* verify all read operations     */
            unsigned fch$v_writcheck : 1; /* verify all write operations    */
            unsigned fch$v_contigb : 1; /* keep file as contiguous as possible  */
            unsigned fch$v_locked : 1;  /* file is deaccess locked          */
            unsigned fch$v_contig : 1;  /* file is contiguous               */
            unsigned fch$$_fill_3 : 3;  /* reserved                         */
            unsigned fch$v_badacl : 1;  /* ACL is invalid                   */
            unsigned fch$v_spool : 1;   /* intermediate spool file          */
            unsigned fch$v_directory : 1; /* file is a directory            */
            unsigned fch$v_badblock : 1; /* file contains bad blocks        */
            unsigned fch$v_markdel : 1; /* file is marked for delete        */
            unsigned fch$v_nocharge : 1; /* file space is not to be charged  */
            unsigned fch$v_erase : 1;   /* erase file contents before deletion  */
/***********The following line is different from FH2                        */
            unsigned fch$$_fill_4 : 1;  /* Place holder for ALM bit in FH2  */
            unsigned fch$v_shelved : 1; /* File shelved                     */
            unsigned fch$v_scratch : 1; /* Scratch Header used by movefile  */
            unsigned fch$v_nomove : 1;  /* Disable movefile on this file    */
            unsigned fch$v_noshelvable : 1; /* File is not allowed to be shelved */
/***********The following line is different from FH2                        */
            unsigned fch$$_fill_5 : 1;  /* Place holder for shelving bit in FH2 */
/* Note: The high 8 bits of this longword                                   */
/* are reserved for user and CSS use.                                       */
            } fch$r_fill_1_bits;
        } fch$r_fch_union;
    } FCH;
 
#if !defined(__VAXC) && !defined(VAXC)
#define fch$v_vcc_state fch$r_fch_union.fch$r_fill_1_chunks.fch$v_vcc_state
#define fch$v_associated fch$r_fch_union.fch$r_fill_1_chunks.fch$v_associated
#define fch$v_existence fch$r_fch_union.fch$r_fill_1_chunks.fch$v_existence
#define fch$v_wascontig fch$r_fch_union.fch$r_fill_1_bits.fch$v_wascontig
#define fch$v_nobackup fch$r_fch_union.fch$r_fill_1_bits.fch$v_nobackup
#define fch$v_writeback fch$r_fch_union.fch$r_fill_1_bits.fch$v_writeback
#define fch$v_readcheck fch$r_fch_union.fch$r_fill_1_bits.fch$v_readcheck
#define fch$v_writcheck fch$r_fch_union.fch$r_fill_1_bits.fch$v_writcheck
#define fch$v_contigb fch$r_fch_union.fch$r_fill_1_bits.fch$v_contigb
#define fch$v_locked fch$r_fch_union.fch$r_fill_1_bits.fch$v_locked
#define fch$v_contig fch$r_fch_union.fch$r_fill_1_bits.fch$v_contig
#define fch$v_badacl fch$r_fch_union.fch$r_fill_1_bits.fch$v_badacl
#define fch$v_spool fch$r_fch_union.fch$r_fill_1_bits.fch$v_spool
#define fch$v_directory fch$r_fch_union.fch$r_fill_1_bits.fch$v_directory
#define fch$v_badblock fch$r_fch_union.fch$r_fill_1_bits.fch$v_badblock
#define fch$v_markdel fch$r_fch_union.fch$r_fill_1_bits.fch$v_markdel
#define fch$v_nocharge fch$r_fch_union.fch$r_fill_1_bits.fch$v_nocharge
#define fch$v_erase fch$r_fch_union.fch$r_fill_1_bits.fch$v_erase
#define fch$v_shelved fch$r_fch_union.fch$r_fill_1_bits.fch$v_shelved
#define fch$v_scratch fch$r_fch_union.fch$r_fill_1_bits.fch$v_scratch
#define fch$v_nomove fch$r_fch_union.fch$r_fill_1_bits.fch$v_nomove
#define fch$v_noshelvable fch$r_fch_union.fch$r_fill_1_bits.fch$v_noshelvable
#endif		/* #if !defined(__VAXC) && !defined(VAXC) */
 
 
#ifdef __cplusplus
    }
#endif
#pragma standard
#pragma member_alignment __restore
 
#endif /* __FCHDEF_LOADED */
 
