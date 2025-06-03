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
/* Created 11-OCT-1991 20:26:53 by VAX SDL T3.2-8      Source:  5-NOV-1990 09:48:05 _$254$DUA99:[LIB.LIS]PEMMTRDEF.SDL;1 */
/********************************************************************************************************************************/
 
/*** MODULE $PEMMTRDEF ***/
#define MTR$C_START 1
#define MTR$C_STOP 2
#define MTR$C_CHANNEL_OPENED 3
#define MTR$C_CHANNEL_CLOSED 4
#define MTR$C_BETTER_CHANNEL 5
#define MTR$C_TRANSMIT_ERROR 6
#define MTR$C_RETRANSMISSION 7
#define MTR$C_PREF_CH_CLOSED 8
#define MTR$C_SHUTDOWN_MTR 9
#define MTR$C_HELLO_SENT 10
#define MTR$C_HELLO_RECEIVED 11
#define MTR$C_START_LEN 32
#define MTR$C_STOP_LEN 32
#define MTR$C_HELLO_SENT_LEN 93
#define MTR$C_CHANNEL_CLOSED_LEN 139
#define MTR$C_CHANNEL_OPENED_LEN 139
#define MTR$C_HELLO_RECEIVED_LEN 156
#define MTR$C_TRANSMIT_ERROR_LEN 164
#define MTR$C_RETRANSMISSION_LEN 164
#define MTR$C_PREF_CH_CLOSED_LEN 164
#define MTR$C_SHUTDOWN_MTR_LEN 164
#define MTR$C_BETTER_CHANNEL_LEN 168
struct MTRDEF {
    unsigned int MTR$Q_TIME_STAMP [2];
    char MTR$AB_SCSNODE [8];
    char MTR$AB_SCSSYSTEMID [6];
    unsigned short int MTR$W_EVENT_CODE;
    unsigned long int MTR$L_MISSED_EVENTS;
    unsigned long int MTR$L_LAST_ERROR;
    unsigned long int MTR$L_XMT_MSGS;
    unsigned long int MTR$L_XMT_BYTES;
    unsigned long int MTR$L_XMT_MC_MSGS;
    unsigned long int MTR$L_XMT_MC_BYTES;
    unsigned long int MTR$L_RCV_MSGS;
    unsigned long int MTR$L_RCV_BYTES;
    unsigned long int MTR$L_RCV_MC_MSGS;
    unsigned long int MTR$L_RCV_MC_BYTES;
    char MTR$AB_LOCAL_BUS_NAME [16];
    char MTR$AB_LOCAL_BUS_HW_ADDR [6];
    char MTR$AB_LOCAL_BUS_ADDR [6];
    unsigned char MTR$B_LOCAL_BUS_DEV_TYPE;
    unsigned char MTR$B_REMOTE_BUS_DEV_TYPE;
    char MTR$AB_REMOTE_SCSSYSTEMID [6];
    char MTR$AB_REMOTE_SCSNODE [8];
    char MTR$AB_REMOTE_BUS_NAME [16];
    char MTR$AB_REMOTE_BUS_ADDR [6];
    char MTR$AB_REMOTE_BUS_HW_ADDR [6];
    unsigned short int MTR$W_VC_STS;
    unsigned char MTR$B_CH_STS;
    char MTR$B_FILL_BYTE;
    long int MTR$L_PRV_NETWORK_DELAY;
    long int MTR$L_CUR_NETWORK_DELAY;
    union  {
        unsigned int MTR$Q_REMOTE_TIME_STAMP [2];
        struct  {
            unsigned int MTR$Q_CHAN_SEL_TIME [2];
            unsigned long int MTR$L_TOTAL_XMIT_MESSAGES;
            unsigned long int MTR$L_TOTAL_XMIT_BYTES;
            long int MTR$L_BEST_CHAN_NET_DLY;
            } MTR$R_XMIT_DATA;
        } MTR$R_MSG_DATA_OVERLAY;
    } ;
