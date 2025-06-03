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
/* Created 11-OCT-1991 20:26:29 by VAX SDL T3.2-8      Source: 17-FEB-1991 14:03:25 _$254$DUA99:[LIB.LIS]PEMCOMPDEF.SDL;1 */
/********************************************************************************************************************************/
 
/*** MODULE $PEMCOMPDEF ***/
#define COMP$C_MAX_NAME_LEN 110
#define COMP$C_NODE 1
#define COMP$C_ADAPTER 2
#define COMP$C_COMPONENT 3
#define COMP$C_CLOUD 4
#define COMP$C_INVALID 5
#define COMP$M_HW_ADDR_VALID 1
#define COMP$M_DECNET_ADDR_VALID 2
#define COMP$M_PRIMARY_SUSPECT 4
#define COMP$M_SECONDARY_SUSPECT 8
struct COMPDEF {
    int *COMP$A_SUSPECT_FLINK;
    int *COMP$A_SUSPECT_BLINK;
    unsigned short int COMP$W_SIZE;
    unsigned char COMP$B_TYPE;
    unsigned char COMP$B_SUBTYPE;
    unsigned char COMP$B_NAME_LEN;
    unsigned char COMP$B_NODENAME_LEN;
    unsigned char COMP$B_COMP_TYPE;
    union  {
        unsigned char COMP$B_FLAGS;
        struct  {
            unsigned COMP$V_HW_ADDR_VALID : 1;
            unsigned COMP$V_DECNET_ADDR_VALID : 1;
            unsigned COMP$V_PRIMARY_SUSPECT : 1;
            unsigned COMP$V_SECONDARY_SUSPECT : 1;
            unsigned COMP$V_fill_0 : 4;
            } COMP$R_FLAG_BITS;
        } COMP$R_FLAGS_OVERLAY;
    int *COMP$A_NAME;
    char COMP$AB_HW_ADDR [6];
    char COMP$AB_DECNET_ADDR [6];
    int *COMP$A_COMP_FLINK;
    int *COMP$A_COMP_BLINK;
    unsigned long int COMP$L_REFERENCE_COUNT;
    unsigned long int COMP$L_WORKING_COUNT;
    unsigned long int COMP$L_SUSPECT_COUNT;
    unsigned long int COMP$L_PRIME_SUSPECT;
    unsigned long int COMP$L_SECONDARY_SUSPECT;
    } ;
