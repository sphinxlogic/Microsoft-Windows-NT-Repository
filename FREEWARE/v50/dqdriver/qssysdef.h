/**/
/******************************************************************************/
/**                                                                          **/
/**  Copyright (c) 2001                                                      **/
/**  by DIGITAL Equipment Corporation, Maynard, Mass.                        **/
/**  All rights reserved.                                                    **/
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
/* Created: 16-MAR-2001 22:24:18 by OpenVMS SDL EV1-60     */
/* Source:  24-JUL-2000 15:59:52 $64$DKC203:[LIB_H.SRC]QSSYSDEF.SDL;1 */
/********************************************************************************************************************************/
/*** MODULE $QSSYSDEF IDENT X-6 ***/
#ifndef __QSSYSDEF_LOADED
#define __QSSYSDEF_LOADED 1
 
#pragma __nostandard			 /* This file uses non-ANSI-Standard features */
#pragma __member_alignment __save
#pragma __nomember_alignment
#ifdef __INITIAL_POINTER_SIZE			 /* Defined whenever ptr size pragmas supported */
#pragma __required_pointer_size __save		 /* Save the previously-defined required ptr size */
#pragma __required_pointer_size __short		 /* And set ptr size default to 32-bit pointers */
#endif
 
#ifdef __cplusplus
    extern "C" {
#define __unknown_params ...
#define __optional_params ...
#else
#define __unknown_params
#define __optional_params ...
#endif
 
#ifndef __struct
#if !defined(__VAXC)
#define __struct struct
#else
#define __struct variant_struct
#endif
#endif
 
#ifndef __union
#if !defined(__VAXC)
#define __union union
#else
#define __union variant_union
#endif
#endif
 
/* Include base definitions                                                 */
	
#include <dtndef.h>
/*                                                                          */
/* The following flags are specified for the first argument to the          */
/* QIOServer client helper routine in the driver.                           */
/*                                                                          */
#define QSRV_HLPR_ACT$C_C_PREP 1        /* Preprocess the client IRP        */
#define QSRV_HLPR_ACT$C_C_POST 2        /* Post process the client IRP      */
#define QSRV_HLPR_ACT$C_C_CLNUP 3       /* Clean client IRP for re-queuing  */
#define QSRV_HLPR_ACT$C_S_PREP 4        /* Preprocess the server IRP        */
#define QSRV_HLPR_ACT$C_S_POST 5        /* Post process the server IRP      */
#define QSRV_HLPR_ACT$C_S_CLNUP 6       /* Clean server IRP for re-queuing  */
/*                                                                          */
/* The following constants are specified in the second argument to the      */
/* QIOServer event notification routine in the driver.                      */
/*                                                                          */
#define QSRV_NTFY_ACT$C_PATH_REST 1     /* Path to device has been restored */
#define QSRV_NTFY_ACT$C_PATH_LOST 2     /* Path to device has been lost     */
/*                                                                          */
/* The following structure defines the set of the UCB fields required to    */
/* be updated on a client in order to bring a disk or tape unit online.     */
/* It does not encompass the entire UCB.  This structure is transferred     */
/* bi-directionally.  This structure is associated with an IO$_PACKACK      */
/* request.                                                                 */
/*                                                                          */
/* These elements were empirically developed using various device           */
/* drivers including DK, DU, MK, and DV.  It was not intended to be         */
/* an all inclusive record.                                                 */
/*                                                                          */
/* A driver writer may allocate any size buffer as appropriate to           */
/* their needs.                                                             */
/*                                                                          */
#define QSRV_PACKACK$M_ORIG_BUFIO 0x1
#define QSRV_PACKACK$M_DATA_VALID 0x2
	
typedef struct _qsrv_packack {
    __union  {
        unsigned int qsrv_packack$l_controls; /* Control flags              */
        __union  {
            unsigned int qsrv_packack$l_ctrl_bits;
            __struct  {
                unsigned qsrv_packack$v_orig_bufio : 1; /*    Original buffered I/O bit setting */
                unsigned qsrv_packack$v_data_valid : 1; /*    Response data in buffer is valid */
                unsigned qsrv_packack$v_fill_2_ : 6;
                } qsrv_packack$r_fill_1_;
            } qsrv_packack$r_fill_0_;
        } qsrv_packack$r_ctrls;
    __union  {
        unsigned int qsrv_packack$l_driver_version; /*   The device drivers version */
        __struct  {
            unsigned short int qsrv_packack$w_major_vers; /*	Driver's major version */
            unsigned short int qsrv_packack$w_minor_vers; /*	Driver's minor version */
            } qsrv_packack$r_vers_words;
        } qsrv_packack$r_vers;
    __struct  {                         /*   UCB elements                   */
        __union  {                      /*   Device characteristic bits     */
            unsigned __int64 qsrv_packack$q_devchar; /*	UCB$Q_DEVCHAR       */
            __struct  {
                unsigned int qsrv_packack$l_devchar; /*	UCB$L_DEVCHAR       */
                unsigned int qsrv_packack$l_devchar2; /*	UCB$L_DEVCHAR2 */
                } qsrv_packack$r_devchar_q_block;
            } qsrv_packack$r_devchar;
        unsigned __int64 qsrv_packack$q_devdepend; /*	UCB$Q_DEVDEPEND     */
        unsigned __int64 qsrv_packack$q_devdepend2; /*	UCB$Q_DEVDEPEND2    */
        unsigned char qsrv_packack$b_devclass; /*	UCB$B_DEVCLASS      */
        unsigned char qsrv_packack$b_devtype; /*	UCB$B_DEVTYPE       */
        unsigned short int qsrv_packack$w_devbufsize; /*	UCB$W_DEVBUFSIZE */
        unsigned int qsrv_packack$l_sts; /*	UCB$L_STS                   */
        unsigned int qsrv_packack$l_devsts; /*	UCB$L_DEVSTS                */
        unsigned int qsrv_packack$l_media_id; /*	UCB$L_MEDIA_ID      */
        } qsrv_packack$r_ucb_elements;
    __struct  {                         /*   Error Log Device elements      */
        unsigned short int qsrv_packack$w_mt3_density; /*	UCB$W_MT3_DENSITY */
        unsigned short int qsrv_packack$w_spare_1;
        } qsrv_packack$r_erl_elements;
    __struct  {                         /*   DISK and TAPE UCB elements     */
        __union  {                      /*                                  */
            __struct  {                 /*	Disk elements               */
                unsigned int qsrv_packack$l_maxblock; /*	    UCB$L_MAXBLOCK */
                unsigned int qsrv_packack$l_maxbcnt; /*	    UCB$L_MAXBCNT   */
                } qsrv_packack$r_disk_elements;
            __struct  {                 /*	Tape elements               */
                unsigned int qsrv_packack$l_record; /*	    UCB$L_RECORD    */
                } qsrv_packack$r_tape_elements;
            } qsrv_packack$r_disktape_overlay;
        unsigned int qsrv_packack$l_alloclass; /*     Device allocation class */
        } qsrv_packack$r_dt_ucb_elements;
    unsigned int qsrv_packack$l_devchar3; /*   SUD$L_DEVCHAR3               */
    unsigned int qsrv_packack$l_wwid_length; /*   SUD$L_WWID_LENGTH         */
    DTN qsrv_packack$r_dtn;             /*   DTN structure for dynamic device names */
/* Calculate fill required to round to                                      */
/* the nearest 64 byte boundary                                             */
    char qsrv_packack$t_wwid [280];     /* Space for a WWID                 */
/* Calculate fill required to round to                                      */
/* the nearest 64 byte boundary                                             */
    } QSRV_PACKACK;
 
#if !defined(__VAXC)
#define qsrv_packack$l_controls qsrv_packack$r_ctrls.qsrv_packack$l_controls
#define qsrv_packack$v_orig_bufio qsrv_packack$r_ctrls.qsrv_packack$r_fill_0_.qsrv_packack$r_fill_1_.qsrv_packack$v_orig_bufio
#define qsrv_packack$v_data_valid qsrv_packack$r_ctrls.qsrv_packack$r_fill_0_.qsrv_packack$r_fill_1_.qsrv_packack$v_data_valid
#define qsrv_packack$l_driver_version qsrv_packack$r_vers.qsrv_packack$l_driver_version
#define qsrv_packack$w_major_vers qsrv_packack$r_vers.qsrv_packack$r_vers_words.qsrv_packack$w_major_vers
#define qsrv_packack$w_minor_vers qsrv_packack$r_vers.qsrv_packack$r_vers_words.qsrv_packack$w_minor_vers
#define qsrv_packack$q_devchar qsrv_packack$r_ucb_elements.qsrv_packack$r_devchar.qsrv_packack$q_devchar
#define qsrv_packack$l_devchar qsrv_packack$r_ucb_elements.qsrv_packack$r_devchar.qsrv_packack$r_devchar_q_block.qsrv_packack$l_dev\
char
#define qsrv_packack$l_devchar2 qsrv_packack$r_ucb_elements.qsrv_packack$r_devchar.qsrv_packack$r_devchar_q_block.qsrv_packack$l_de\
vchar2
#define qsrv_packack$q_devdepend qsrv_packack$r_ucb_elements.qsrv_packack$q_devdepend
#define qsrv_packack$q_devdepend2 qsrv_packack$r_ucb_elements.qsrv_packack$q_devdepend2
#define qsrv_packack$b_devclass qsrv_packack$r_ucb_elements.qsrv_packack$b_devclass
#define qsrv_packack$b_devtype qsrv_packack$r_ucb_elements.qsrv_packack$b_devtype
#define qsrv_packack$w_devbufsize qsrv_packack$r_ucb_elements.qsrv_packack$w_devbufsize
#define qsrv_packack$l_sts qsrv_packack$r_ucb_elements.qsrv_packack$l_sts
#define qsrv_packack$l_devsts qsrv_packack$r_ucb_elements.qsrv_packack$l_devsts
#define qsrv_packack$l_media_id qsrv_packack$r_ucb_elements.qsrv_packack$l_media_id
#define qsrv_packack$w_mt3_density qsrv_packack$r_erl_elements.qsrv_packack$w_mt3_density
#define qsrv_packack$w_spare_1 qsrv_packack$r_erl_elements.qsrv_packack$w_spare_1
#define qsrv_packack$l_maxblock qsrv_packack$r_dt_ucb_elements.qsrv_packack$r_disktape_overlay.qsrv_packack$r_disk_elements.qsrv_pa\
ckack$l_maxblock
#define qsrv_packack$l_maxbcnt qsrv_packack$r_dt_ucb_elements.qsrv_packack$r_disktape_overlay.qsrv_packack$r_disk_elements.qsrv_pac\
kack$l_maxbcnt
#define qsrv_packack$l_record qsrv_packack$r_dt_ucb_elements.qsrv_packack$r_disktape_overlay.qsrv_packack$r_tape_elements.qsrv_pack\
ack$l_record
#define qsrv_packack$l_alloclass qsrv_packack$r_dt_ucb_elements.qsrv_packack$l_alloclass
#endif		/* #if !defined(__VAXC) */
 
#define QSRV_PACKACK$C_LENGTH 128
#define QSRV_PACKACK$K_LENGTH 128
#define QSRV_PACKACK$C_LENGTH_WWID 448
#define QSRV_PACKACK$K_LENGTH_WWID 448
	
#define QSRV_PACKACK$S_QSRV_PACKACK 448
/*                                                                          */
/* Declaration of QIOServer Server notification reason codes.               */
/*                                                                          */
#define QSRV_ACTION$C_MOUNTVER 1        /* Mount verify action              */
/*					   ( QSRV_ACTION$_MOUNTVER )( UCB ) */
#define QSRV_ACTION$C_MPDEV_PATHSWITCH 2 /* Multipath device path change    */
/*					   ( QSRV_ACTION$_MPDEV_PATHSWITCH )( UCB ) */
 
#pragma __member_alignment __restore
#ifdef __INITIAL_POINTER_SIZE			 /* Defined whenever ptr size pragmas supported */
#pragma __required_pointer_size __restore		 /* Restore the previously-defined required ptr size */
#endif
#ifdef __cplusplus
    }
#endif
#pragma __standard
 
#endif /* __QSSYSDEF_LOADED */
 
