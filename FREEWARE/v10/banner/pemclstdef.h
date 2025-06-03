/**/
/******************************************************************************/
/**                                                                          **/
/**  Copyright (c) 1991                                                      **/
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
/* Created 11-OCT-1991 20:26:41 by VAX SDL T3.2-8      Source: 15-OCT-1990 13:36:30 _$254$DUA99:[LIB.LIS]PEMCLSTDEF.SDL;1 */
/********************************************************************************************************************************/
 
/*** MODULE $PEMCLSTDEF ***/
struct CLSTDEF {
    int *CLST$A_FAILURE_FLINK;
    int *CLST$A_FAILURE_BLINK;
    unsigned short int CLST$W_SIZE;
    unsigned char CLST$B_TYPE;
    unsigned char CLST$B_SUBTYPE;
    unsigned long int CLST$L_COMP_ENTRIES;
    int *CLST$A_CLST_FLINK;
    int *CLST$A_CLST_BLINK;
    int *CLST$A_HW_HASH_LINK;
    int *CLST$A_DECNET_HASH_LINK;
    int *CLST$A_LOCAL_ADAPTER;
    int *CLST$A_REMOTE_ADAPTER;
    unsigned long int CLST$L_REFERENCE_COUNT;
    char CLST$AA_COMP_LIST [];
    } ;
