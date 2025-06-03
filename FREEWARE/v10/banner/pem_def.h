/********************************************************************************************************************************/
/* Created  6-FEB-1992 15:45:03 by VAX SDL V3.2-12     Source:  4-FEB-1992 17:27:19 FIZBUZ$DUA0:[SAUNDERS.BANNER.C]PEM_DEF.SDL;1 */
/********************************************************************************************************************************/
 
/*** MODULE $NISCADEF ***/
#define NISCA$C_DX_LNG_HDR 14
#define NISCA$C_HELLO 0
#define NISCA$C_BYE 1
#define NISCA$C_CCSTART 2
#define NISCA$C_VERF 3
#define NISCA$C_VACK 4
#define NISCA$C_DONE 5
#define NISCA$C_SOLICIT_SRV 6
#define NISCA$m_spx 15
#define NISCA$M_AUTHORIZE 16
#define NISCA$M_OK_XMT 32
#define NISCA$M_NO_XMT 64
#define NISCA$M_CCFLG 128
#define NISCA$M_MAINT_ID 2147483647
#define NISCA$M_D -2147483648
#define NISCA$M_MAINT -2147483640
#define NISCA$M_SRSNTDATWM 128
#define NISCA$M_SRLB 256
#define NISCA$M_RRSTSTRT 512
#define NISCA$M_SSTRT 1024
#define NISCA$M_SRST 2048
#define NISCA$M_RMDATREQ 4096
#define NISCA$M_SMDATREQ 8192
#define NISCA$M_RSNTMDAT 16384
#define NISCA$M_SSNTMDAT 32768
#define NISCA$M_SRID 65536
#define NISCA$M_SRDATREQ 8257536
#define NISCA$M_RSNTDAT 8388608
#define NISCA$M_SSNTDAT 16777216
#define NISCA$M_SRMSG 33554432
#define NISCA$M_UNINIT_MAINT 67108864
#define NISCA$M_UNINIT 134217728
#define NISCA$M_DISAB_MAINT 268435456
#define NISCA$M_DISAB 536870912
#define NISCA$M_ENAB_MAINT 1073741824
#define NISCA$M_ENAB -2147483648
#define NISCA$M_PORT_FCN -2080439936
#define NISCA$M_PEBT_PORT_FCN -2080440064
#define NISCA$M_M 256
#define NISCA$M_PS 1536
#define NISCA$M_SYS_STATE -2048
#define NISCA$M_STA_INFO 1024
#define NISCA$C_CC_LNG_V11 46
#define NISCA$C_CC_LNG_V12 62
#define NISCA$C_NODE_HW_VAX 0
#define NISCA$C_NODE_HW_EVAX 1
#define NISCA$C_CC_LNG_V13 98
#define NISCA$C_CC_LNG 98
#define NISCA$M_SRV_XCHK 1
#define NISCA$M_SRV_RCHK 2
#define NISCA$M_TR_DATA 1
#define NISCA$M_TR_SEQ 2
#define NISCA$M_TR_NAK 4
#define NISCA$M_TR_ACK 8
#define NISCA$M_TR_RSVP 16
#define NISCA$M_TR_REXMT 32
#define NISCA$M_TR_CTL 64
#define NISCA$M_TR_CCFLG 128
#define NISCA$C_TR_LNG_HDR_V10 6
#define NISCA$C_TR_LNG_HDR_V13 9
#define NISCA$C_TR_LNG_HDR 9
#define NISCA$C_PPC_LNG_HDR 1
#define NISCA$C_SMSG_OVHD 24
#define NISCA$C_CMSG_OVHD 112
#define NISCA$C_GROUP_BASE_LO 17039531
#define NISCA$C_GROUP_BASE_HI 256
#define NISCA$C_GROUP_MAX 65279
#define NISCA$C_NI_PROTOCOL 1888
#define NISCA$C_NI_ACCESS_CODE 6007
#define NISCA$C_DELAY_ACK 1
#define NISCA$C_DELAY_SEQ 2
#define NISCA$C_TIMER 10000000
#define NISCA$C_HELLO_TIMER 3
#define NISCA$C_HELLO_MIN_TIM 1
#define NISCA$C_HS_TMO_MAX 5
#define NISCA$C_HS_TMO_1 5
#define NISCA$C_HS_TIMERS 1
#define NISCA$C_LSTN_TMO_MAX 8
#define NISCA$C_LSTN_TMO_1 8
#define NISCA$C_LSTN_TIMERS 1
#define NISCA$C_PIPE_QUOTA 8
#define NISCA$C_MAX_CACHE 31
#define NISCA$C_MAX_REXMT 30
#define NISCA$C_MAJOR 1
#define NISCA$C_MINOR 3
#define NISCA$C_ECO 0
#define NISCA$C_MAX_VCTMO 16
#define NISCA$C_CMDQ_3 0
#define NISCA$C_CMDQ_2 1
#define NISCA$C_CMDQ_HI 2
#define NISCA$C_CMDQ_LO 3
#define NISCA$C_SNT_DG 0
#define NISCA$C_SNT_LB 1
#define NISCA$C_SNT_SEQ 2
#define NISCA$C_SNT_DATM 3
#define NISCA$C_SNT_DAT 4
#define NISCA$C_SNT_DAT_LP 5
#define NISCA$C_REQ_ID 6
#define NISCA$C_REQ_DATM 7
#define NISCA$C_REQ_DAT0 8
#define NISCA$C_REQ_DAT1 9
#define NISCA$C_REQ_DAT2 10
#define NISCA$C_RET_DATM 11
#define NISCA$C_RET_DAT 12
#define NISCA$C_RET_DAT_LP 13
#define NISCA$C_RET_CNFM 14
#define NISCA$C_RET_CNF 15
#define NISCA$C_RET_ID 16
#define NISCA$C_RET_LB 17
#define NISCA$C_RESET 18
#define NISCA$C_START 19
#define NISCA$C_SNT_DATWM 20
#define NISCA$C_SNT_DATWM_LP 21
#define NISCA$C_RET_CNFWM 22
#define NISCA$C_PPDMSG_MAX 22
struct NISCADEF {
    union  {
        struct  {
            unsigned long int NISCA$L_DX_DST_LO;
            unsigned short int NISCA$W_DX_DST_HI;
            unsigned short int NISCA$W_DX_GROUP;
            unsigned long int NISCA$L_DX_SRC_LO;
            unsigned short int NISCA$W_DX_SRC_HI;
            } NISCA$R_DX_HEADER;
        struct  {
            union  {
                unsigned char NISCA$B_MSG;
                struct  {
                    unsigned NISCA$v_spx : 4;
                    unsigned NISCA$V_AUTHORIZE : 1;
                    unsigned NISCA$V_OK_XMT : 1;
                    unsigned NISCA$V_NO_XMT : 1;
                    unsigned NISCA$V_CCFLG : 1;
                    } NISCA$R_MSG_FLAGS;
                } NISCA$R_MSG_OVERLAY;
            unsigned char NISCA$B_REASON;
            union  {
                unsigned long int NISCA$L_MAINT;
                struct  {
                    unsigned NISCA$V_MAINT_ID : 31;
                    unsigned NISCA$V_D : 1;
                    } NISCA$R_MAINT_S;
                } NISCA$R_MAI_OVERLAY;
            unsigned char NISCA$B_MINOR;
            unsigned char NISCA$B_MAJOR;
            unsigned short int NISCA$W_ECO;
            char NISCA$T_NODENAME [8];
            union  {
                unsigned long int NISCA$L_PORT_FCN;
                struct  {
                    unsigned NISCA$V_MBZ : 7;
                    unsigned NISCA$V_SRSNTDATWM : 1;
                    unsigned NISCA$V_SRLB : 1;
                    unsigned NISCA$V_RRSTSTRT : 1;
                    unsigned NISCA$V_SSTRT : 1;
                    unsigned NISCA$V_SRST : 1;
                    unsigned NISCA$V_RMDATREQ : 1;
                    unsigned NISCA$V_SMDATREQ : 1;
                    unsigned NISCA$V_RSNTMDAT : 1;
                    unsigned NISCA$V_SSNTMDAT : 1;
                    unsigned NISCA$V_SRID : 1;
                    unsigned NISCA$V_SRDATREQ : 6;
                    unsigned NISCA$V_RSNTDAT : 1;
                    unsigned NISCA$V_SSNTDAT : 1;
                    unsigned NISCA$V_SRMSG : 1;
                    unsigned NISCA$V_UNINIT_MAINT : 1;
                    unsigned NISCA$V_UNINIT : 1;
                    unsigned NISCA$V_DISAB_MAINT : 1;
                    unsigned NISCA$V_DISAB : 1;
                    unsigned NISCA$V_ENAB_MAINT : 1;
                    unsigned NISCA$V_ENAB : 1;
                    } NISCA$R_FCN_BITMASKS;
                } NISCA$R_FCN_OVERLAY;
            union  {
                unsigned long int NISCA$L_STA_INFO;
                struct  {
                    unsigned NISCA$V_SPARE : 8;
                    unsigned NISCA$V_M : 1;
                    unsigned NISCA$V_PS : 2;
                    unsigned NISCA$V_SYS_STATE : 21;
                    } NISCA$R_STA_FIELDS;
                } NISCA$R_STA_OVERLAY;
            char NISCA$T_RST_PORT [6];
            unsigned char NISCA$B_RESERVED0;
            unsigned char NISCA$B_PIPE_QUOTA;
            unsigned short int NISCA$W_P_SRV;
            unsigned short int NISCA$W_R_SRV;
            unsigned int NISCA$Q_AUTHORIZE [2];
            char NISCA$T_SERVICE [16];
            unsigned short int NISCA$W_LCL_CHAN_SEQ_NUM;
            unsigned short int NISCA$W_BUS_BUFFER_SIZE;
            unsigned int NISCA$Q_TRANSMIT_TIME [2];
            char NISCA$AB_DEVICE_NAME [16];
            char NISCA$AB_LAN_HW_ADDR [6];
            unsigned char NISCA$B_DEVICE_TYPE;
            unsigned char NISCA$B_NODE_HW_TYPE;
            } NISCA$R_CC_HEADER;
        struct  {
            unsigned NISCA$V_SRV_XCHK : 1;
            unsigned NISCA$V_SRV_RCHK : 1;
            unsigned NISCA$V_fill_0 : 6;
            } NISCA$R_SRV_FIELD;
        struct  {
            union  {
                unsigned char NISCA$B_TR_FLAG;
                struct  {
                    unsigned NISCA$V_TR_DATA : 1;
                    unsigned NISCA$V_TR_SEQ : 1;
                    unsigned NISCA$V_TR_NAK : 1;
                    unsigned NISCA$V_TR_ACK : 1;
                    unsigned NISCA$V_TR_RSVP : 1;
                    unsigned NISCA$V_TR_REXMT : 1;
                    unsigned NISCA$V_TR_CTL : 1;
                    unsigned NISCA$V_TR_CCFLG : 1;
                    } NISCA$R_FLAG_BITS;
                } NISCA$R_FLAG_OVERLAY;
            unsigned char NISCA$B_TR_PAD;
            unsigned short int NISCA$W_TR_ACK;
            unsigned short int NISCA$W_TR_SEQ;
            unsigned short int NISCA$W_RMT_CHAN_SEQ_NUM;
            unsigned char NISCA$B_TR_PAD_DATA_LEN;
            } NISCA$R_TR_HEADER;
        struct  {
            unsigned char NISCA$B_PPC_OPC;
            } NISCA$R_PPC_HEADER;
        } NISCA$R_MSG_HEADER;
    } ;
 
/*** MODULE $PORTQBDEF ***/
struct PORTQBDEF {
    char PORTQB$R_CMDQ_FILL [32];
    int PORTQB$Q_RSPQ_FILL [2];
    unsigned long int PORTQB$L_DFQHDR;
    unsigned long int PORTQB$L_MFQHDR;
    unsigned short int PORTQB$W_DQELEN;
    unsigned short int PORTQB$w_dqelenhi;
    unsigned short int PORTQB$W_MQELEN;
    unsigned short int PORTQB$w_mqelenhi;
    unsigned long int PORTQB$L_VPORTQB;
    unsigned long int PORTQB$L_VBDT;
    unsigned short int PORTQB$W_BDTLEN;
    unsigned short int PORTQB$w_bdtlenhi;
    unsigned long int PORTQB$L_SPTBASE;
    unsigned long int PORTQB$L_SPTLEN;
    unsigned long int PORTQB$L_GPTBASE;
    unsigned long int PORTQB$L_GPTLEN;
    } ;
 
/*** MODULE $BUSDEF ***/
#define ERR$C_LENGTH 6
struct ERRDEF {
    unsigned short int ERR$W_STATUS;
    unsigned short int ERR$W_UNLOG_COUNT;
    unsigned char ERR$B_LOG_COUNT;
    unsigned char ERR$B_SP1;
    } ;
#define BUS$M_RUN 1
#define BUS$M_ONLINE 2
#define BUS$M_LDL 4
#define BUS$M_HELLO_VCRP_BUSY 8
#define BUS$M_BUILD_HELLO_MESSAGE 16
#define BUS$M_INIT 32
#define BUS$M_WAIT_MGMT 64
#define BUS$M_WAIT_EVNT 128
#define BUS$M_BROKEN 256
#define BUS$M_XMT_CHAINING_DISABLED 512
#define BUS$M_DELETE_PENDING 1024
#define BUS$M_RESTART 2048
#define BUS$M_RESTART_DELAY 4096
#define BUS$V_NET_ADDR_HASH 32
#define BUS$S_NET_ADDR_HASH 6
#define BUS$C_NUM_COU 6
#define BUS$C_LENGTH 440
struct BUSDEF {
    unsigned int BUS$Q_BUS_LIST [2];
    unsigned short int BUS$W_SIZE;
    unsigned char BUS$B_TYPE;
    unsigned char BUS$B_SUB_TYPE;
    int *BUS$A_DL_DEV_NAME;
    union  {
        unsigned long int BUS$L_STS;
        struct  {
            unsigned BUS$V_RUN : 1;
            unsigned BUS$V_ONLINE : 1;
            unsigned BUS$V_LDL : 1;
            unsigned BUS$V_HELLO_VCRP_BUSY : 1;
            unsigned BUS$V_BUILD_HELLO_MESSAGE : 1;
            unsigned BUS$V_INIT : 1;
            unsigned BUS$V_WAIT_MGMT : 1;
            unsigned BUS$V_WAIT_EVNT : 1;
            unsigned BUS$V_BROKEN : 1;
            unsigned BUS$V_XMT_CHAINING_DISABLED : 1;
            unsigned BUS$V_DELETE_PENDING : 1;
            unsigned BUS$V_RESTART : 1;
            unsigned BUS$V_RESTART_DELAY : 1;
            unsigned BUS$V_fill_1 : 3;
            } BUS$R_STS_BITS;
        } BUS$R_STS_OVERLAY;
    int *BUS$A_DELETE_BUS_ROUTINE;
    int *BUS$A_PORT;
    unsigned long int BUS$L_RCV_MESSAGES;
    unsigned long int BUS$L_RCV_BYTES;
    unsigned long int BUS$L_RCV_MULTICAST_MSGS;
    unsigned long int BUS$L_RCV_MULTICAST_BYTES;
    unsigned short int BUS$W_OUTSTANDING_IO_COUNT;
    unsigned char BUS$B_DEVICE_TYPE;
    char BUS$B_FILL_BYTE;
    unsigned short int BUS$W_BUFFER_SIZE;
    unsigned char BUS$B_RECEIVE_RING_SIZE;
    unsigned char BUS$B_HELLO_TIMER;
    int *BUS$A_HANDSHAKE_TIMEOUT;
    int *BUS$A_LISTEN_TIMEOUT;
    int *BUS$A_HELLO_MESSAGE;
    int *BUS$A_BYE_MESSAGE;
    unsigned long int BUS$L_XMT_BYTES;
    unsigned long int BUS$L_XMT_MESSAGES;
    unsigned long int BUS$L_XMT_MULTICAST_MSGS;
    unsigned long int BUS$L_XMT_MULTICAST_BYTES;
    unsigned long int BUS$L_LAST_EVENT_TYPE;
    unsigned int BUS$Q_LAST_EVENT_TIME [2];
    unsigned short int BUS$W_PORT_USABLE_EVENT;
    unsigned short int BUS$W_PORT_UNUSABLE_EVENT;
    unsigned short int BUS$W_ADDRESS_CHANGE_EVENT;
    unsigned short int BUS$W_RESTART_FAIL_EVENT;
    char BUS$AA_CH_HASH_TBL [256];
    unsigned short int BUS$W_CC_RCV_BAD_SCSSYSTEMID;
    unsigned short int BUS$W_RCV_TR_MC;
    unsigned short int BUS$W_SHORT_DX_MSG;
    unsigned short int BUS$W_CH_ALLOC_FAIL;
    unsigned short int BUS$W_VC_ALLOC_FAIL;
    unsigned short int BUS$W_WRONG_PORT;
    unsigned long int BUS$L_ERROR_COUNT;
    unsigned int BUS$Q_LAST_ERROR_TIME [2];
    unsigned long int BUS$L_LAST_ERROR_REASON;
    unsigned long int BUS$L_HELLO_XMT_ERRORS;
    char BUS$AL_VEC0_ERR [36];
    char BUS$AB_LAN_HARDWARE_ADDR [6];
    char BUS$AB_CURRENT_LAN_ADDR [6];
    char BUS$T_VCIB [];
    } ;
 
/*** MODULE $VCDEF ***/
#define VC$M_OPEN 1
#define VC$M_DQI 2
#define VC$M_PATH 4
#define VC$M_QUEUED 8
#define VC$M_TIM_RXMT 16
#define VC$M_PIPE_QUOTA 32
#define VC$M_RWAIT 64
#define VC$M_RESTART 128
#define VC$K_FIRST_WRK 0
#define VC$M_WRK_SSEQ 1
#define VC$M_WRK_SACK 2
#define VC$M_WRK_ABUF 4
#define VC$K_LAST_WRK 2
#define VC$C_COUNTER_SIZE 94
#define VC$C_QUE_NUMBER 4
#define VC$M_MAINT_ID 2147483647
#define VC$M_D -2147483648
#define VC$M_M 256
#define VC$M_PS 1536
#define VC$M_SYS_STATE -2048
#define VC$C_LENGTH 316
struct VCDEF {
    unsigned int VC$Q_QLNK [2];
    unsigned short int VC$W_SIZE;
    unsigned char VC$B_TYPE;
    unsigned char VC$B_SUB_TYPE;
    union  {
        unsigned short int VC$W_STS;
        struct  {
            unsigned VC$V_OPEN : 1;
            unsigned VC$V_DQI : 1;
            unsigned VC$V_PATH : 1;
            unsigned VC$V_QUEUED : 1;
            unsigned VC$V_TIM_RXMT : 1;
            unsigned VC$V_PIPE_QUOTA : 1;
            unsigned VC$V_RWAIT : 1;
            unsigned VC$V_RESTART : 1;
            } VC$R_STS_BITS;
        } VC$R_STS_OVERLAY;
    unsigned char VC$B_NODE_HW_TYPE;
    union  {
        unsigned char VC$B_WRK;
        struct  {
            unsigned VC$V_WRK_SSEQ : 1;
            unsigned VC$V_WRK_SACK : 1;
            unsigned VC$V_WRK_ABUF : 1;
            unsigned VC$V_fill_2 : 5;
            } VC$R_WRK_BITS;
        } VC$R_WRK_OVERLAY;
    int *VC$A_PREFERRED_CHANNEL;
    unsigned long int VC$L_DELAY_TIME;
    unsigned long int VC$L_BUFFER_SIZE;
    unsigned long int VC$L_FILL_LONGWORD;
    unsigned long int VC$L_CHANNEL_COUNT;
    unsigned long int VC$L_CHANNEL_SELECTION_COUNT;
    unsigned int VC$Q_OPEN_TIME [2];
    unsigned int VC$Q_CLOSE_TIME [2];
    unsigned short int VC$W_TIM_XACK;
    unsigned short int VC$W_TIM_RACK;
    unsigned long int VC$L_RET_ID_ATTEMPTS;
    unsigned long int VC$L_RET_IDS_SENT;
    unsigned long int VC$L_TOPOLOGY_CHANGE;
    unsigned long int VC$L_NPAGEDYN_LOW;
    unsigned long int VC$L_XMT_MSG;
    unsigned long int VC$L_XMT_UNSEQ;
    unsigned long int VC$L_XMT_SEQ;
    unsigned long int VC$L_XMT_ACK;
    unsigned long int VC$L_XMT_REXMT;
    unsigned long int VC$L_XMT_BYTES;
    unsigned long int VC$L_XMT_NOXCH;
    unsigned long int VC$L_RCV_MSG;
    unsigned long int VC$L_RCV_UNSEQ;
    unsigned long int VC$L_RCV_SEQ;
    unsigned long int VC$L_RCV_ACK;
    unsigned long int VC$L_RCV_RERCV;
    unsigned long int VC$L_RCV_BYTES;
    unsigned long int VC$L_RCV_CACHE;
    unsigned long int VC$L_TR_PIPE_QUOTA;
    unsigned short int VC$W_RCV_TR_SHORT;
    unsigned short int VC$W_RCV_ILL_ACK;
    unsigned short int VC$W_RCV_ILL_SEQ;
    unsigned short int VC$W_RCV_BAD_CKSUM;
    unsigned short int VC$W_XMT_SEQ_TMO;
    unsigned short int VC$W_TR_DFQ_EMPTY;
    unsigned short int VC$W_TR_MFQ_EMPTY;
    unsigned short int VC$W_CC_DFQ_EMPTY;
    unsigned short int VC$W_CC_MFQ_EMPTY;
    short int VC$W_FILL_WORD;
    int *VC$A_HASH_LINK;
    char VC$T_NODENAME [8];
    unsigned long int VC$L_REMSYS_LO;
    unsigned short int VC$W_REMSYS_HI;
    unsigned char VC$B_PORT_INX;
    unsigned char VC$B_RETRIES;
    int *VC$A_BUF_ACTION;
    int *VC$A_BUF_RCV;
    int *VC$A_BUF_FIRST;
    int *VC$A_BUF_LAST;
    int *VC$A_BUF_REXMT;
    unsigned short int VC$W_CMDQ_LEN;
    unsigned short int VC$W_HAA;
    unsigned short int VC$W_LAR;
    unsigned short int VC$W_HSR;
    unsigned short int VC$W_NSU;
    unsigned short int VC$W_RSVP_THRESH;
    unsigned char VC$B_PIPE_QUOTA;
    unsigned char VC$B_MASK_QUE;
    unsigned short int VC$W_MAX_CMD_LEN;
    unsigned long int VC$L_CACHE_MASK;
    unsigned int VC$Q_RWAITQ [2];
    unsigned int VC$Q_CMDQ3 [2];
    unsigned int VC$Q_CMDQ2 [2];
    unsigned int VC$Q_CMDQHI [2];
    unsigned int VC$Q_CMDQLO [2];
    unsigned int VC$Q_ACTIVE_CHANNELS [2];
    unsigned int VC$Q_DEAD_CHANNELS [2];
    union  {
        unsigned long int VC$L_MAINT;
        struct  {
            unsigned VC$V_MAINT_ID : 31;
            unsigned VC$V_D : 1;
            } VC$R_MAINT_S;
        } VC$R_MAI_OVERLAY;
    unsigned char VC$B_MINOR;
    unsigned char VC$B_MAJOR;
    unsigned short int VC$W_ECO;
    unsigned long int VC$L_PORT_FCN;
    union  {
        unsigned long int VC$L_STA_INFO;
        struct  {
            unsigned VC$V_SPARE : 8;
            unsigned VC$V_M : 1;
            unsigned VC$V_PS : 2;
            unsigned VC$V_SYS_STATE : 21;
            } VC$R_STA_FIELDS;
        } VC$R_STA_OVERLAY;
    unsigned long int VC$L_SRV;
    unsigned long int VC$L_CACHE_MISS;
    unsigned int VC$Q_CHAN_SEL_TIME [2];
    unsigned long int VC$L_CH_XMT_MSGS;
    unsigned long int VC$L_CH_XMT_BYTES;
    long int VC$L_PREV_NET_DELAY;
    } ;
 
/*** MODULE $PEMCHDEF ***/
#define CH$M_PATH 1
#define CH$M_OPEN 2
#define CH$M_XMT_CHAINING_DISABLED 4
#define CH$M_RMT_HWA_VALID 8
#define CH$C_REXMT_PENALTY 500
#define CH$C_XMTFAIL_PENALTY 10000
#define CH$C_LENGTH 190
struct CHDEF {
    unsigned int CH$Q_TIMER_ENTRY [2];
    unsigned short int CH$W_SIZE;
    unsigned char CH$B_TYPE;
    unsigned char CH$B_SUB_TYPE;
    unsigned short int CH$W_STATE;
    union  {
        unsigned char CH$B_STS;
        struct  {
            unsigned CH$V_PATH : 1;
            unsigned CH$V_OPEN : 1;
            unsigned CH$V_XMT_CHAINING_DISABLED : 1;
            unsigned CH$V_RMT_HWA_VALID : 1;
            unsigned CH$V_fill_3 : 4;
            } CH$R_STS_BITS;
        } CH$R_STS_OVERLAY;
    unsigned char CH$B_RING_INDEX;
    int *CH$A_BUS;
    int *CH$A_VC;
    unsigned int CH$Q_CHANNEL_LIST [2];
    int *CH$A_CH_HASH_LINK;
    unsigned long int CH$L_AVERAGE_XMT_TIME;
    unsigned short int CH$W_RSVP_THRESH;
    unsigned char CH$B_REMOTE_RING_SIZE;
    unsigned char CH$B_REMOTE_DEVICE_TYPE;
    unsigned short int CH$W_LCL_CHAN_SEQ_NUM;
    unsigned short int CH$W_RMT_CHAN_SEQ_NUM;
    unsigned short int CH$W_REMOTE_BUFFER_SIZE;
    unsigned short int CH$W_MAX_BUFFER_SIZE;
    unsigned long int CH$L_SUPPORTED_SERVICES;
    unsigned int CH$Q_REMOTE_NET_ADDRESS [2];
    unsigned int CH$Q_OPEN_TIME [2];
    unsigned int CH$Q_CLOSE_TIME [2];
    unsigned long int CH$L_BEST_CHANNEL_COUNT;
    unsigned long int CH$L_PREFERRED_CHANNEL_CNT;
    unsigned long int CH$L_REXMT_PENALTY;
    unsigned long int CH$L_XMTFAIL_PENALTY;
    unsigned long int CH$L_XMT_MSGS;
    unsigned long int CH$L_XMT_BYTES;
    unsigned long int CH$L_XMT_CTRL_MSGS;
    unsigned long int CH$L_XMT_CTRL_BYTES;
    unsigned long int CH$L_RCV_MC_MSGS;
    unsigned long int CH$L_RCV_MC_BYTES;
    unsigned long int CH$L_RCV_CTRL_MSGS;
    unsigned long int CH$L_RCV_CTRL_BYTES;
    unsigned long int CH$L_RCV_MSGS;
    unsigned long int CH$L_RCV_BYTES;
    int *CH$A_CLST;
    unsigned char CH$B_MINOR;
    unsigned char CH$B_MAJOR;
    unsigned short int CH$W_ECO;
    char CH$AB_RMT_DEV_NAME [16];
    char CH$AB_RMT_LAN_HWA [6];
    unsigned char CH$B_RMT_NODE_HW_TYPE;
    char CH$B_FILL_BYTE;
    unsigned short int CH$W_CC_HS_TMO;
    unsigned short int CH$W_CC_LISTEN_TMO;
    unsigned short int CH$W_CC_RCV_BAD_AUTHORIZE;
    unsigned short int CH$W_CC_RCV_BAD_ECO;
    unsigned short int CH$W_CC_RCV_BAD_MC;
    unsigned short int CH$W_CC_RCV_SHORT_MSG;
    unsigned short int CH$W_CC_RCV_INCOMPAT_CHANNEL;
    unsigned short int CH$W_TR_RCV_OLD_MSG;
    unsigned short int CH$W_CC_NO_MSCP_SERVER;
    unsigned short int CH$W_CC_DISK_NOT_SERVED;
    unsigned short int CH$W_TOPOLOGY_CHANGE;
    } ;
 
/*** MODULE $PEMVCIBDEF ***/
#define VCIB$C_LENGTH 144
struct VCIBDEF {
    char VCIB$T_VCIB_FILLER [140];
    } ;
 
/*** MODULE $PEMVCRPDEF ***/
#define VCRP$C_ACT_NOP 0
#define VCRP$C_ACT_RET_ID 1
#define VCRP$C_ACT_RET_LB 2
#define VCRP$C_ACT_IDLE_BFR 3
#define VCRP$M_CD_SYNC 1
#define VCRP$M_CD_XMTPND 2
#define VCRP$M_CD_ACKPND 4
#define VCRP$M_CD_ACTION 8
#define VCRP$C_DLL_HDR_SIZE 26
#define VCRP$C_DLL_CHAIN_SIZE 101
#define VCRP$C_DLL_XMT_SIZE 1498
#define VCRP$C_HEADER 288
#define VCRP$C_TRAILER 0
#define VCRP$C_OVERHEAD 288
struct VCRPDEF {
    char VCRP$B_CH_FILLER [92];
    int *VCRP$A_CH;
    char VCRP$B_STACK_FILLER [16];
    union  {
        char VCRP$B_STACK_OVERLAY [84];
        struct  {
            int *VCRP$A_DL_VCIB;
            char VCRP$T_LAVC_DCBE [56];
            int *VCRP$A_LINK;
            unsigned short int VCRP$W_LENGTH;
            unsigned short int VCRP$W_OFFSET;
            int *VCRP$A_ACK_ACTION;
            int *VCRP$A_END_ACTION;
            int *VCRP$A_XCTID_PTR;
            unsigned char VCRP$B_X_ACTION;
            union  {
                unsigned char VCRP$B_CODE;
                struct  {
                    unsigned VCRP$V_CD_SYNC : 1;
                    unsigned VCRP$V_CD_XMTPND : 1;
                    unsigned VCRP$V_CD_ACKPND : 1;
                    unsigned VCRP$V_CD_ACTION : 1;
                    unsigned VCRP$V_fill_6 : 4;
                    } VCRP$R_CODE_BITS;
                } VCRP$R_CODE_OVERLAY;
            } VCRP$r_fill_5;
        } VCRP$r_fill_4;
    char VCRP$T_VCRP_FILLER [64];
    char VCRP$T_VCRP_DLL_FILLER [28];
    char VCRP$T_PPD_START_FILLER [23];
    char VCRP$T_START_PPD [];
    } ;
 
/*** MODULE $PORTDEF ***/
#define PORT$C_INIT_LBUF_MAX 384
#define PORT$C_INIT_SBUF_MAX 768
#define PORT$C_INIT_LBUF_QUO 1
#define PORT$C_INIT_SBUF_QUO 8
#define PORT$C_SBUF_QUO_INCR 1
#define PORT$M_AUTHORIZE 1
#define PORT$M_FORK 2
#define PORT$M_NEED_LBUF 4
#define PORT$M_NEED_SBUF 8
#define PORT$M_NEED_MFQ 16
#define PORT$M_NEED_DFQ 32
#define PORT$M_DISABLED 64
#define PORT$M_SYNCH 128
#define PORT$C_QUE_NUMBER 10
#define PORT$K_FIRST_WRK 0
#define PORT$M_WRK_RWAITQ 1
#define PORT$K_FIRST_CYCL_WRK 1
#define PORT$M_WRK_VCQ 2
#define PORT$M_WRK_INTR 4
#define PORT$M_WRK_LDL 8
#define PORT$M_WRK_TIMER 16
#define PORT$K_LAST_WRK 4
#define PORT$C_PORT_COUNTER_SIZE 94
#define PORT$C_ERRLOG_THRES 10
#define PORT$C_ERR_RATE_INT 3
#define PORT$M_MAINT_ID 2147483647
#define PORT$M_D -2147483648
#define PORT$M_M 256
#define PORT$M_PS 1536
#define PORT$M_SYS_STATE -2048
#define PORT$C_VC_MAX 256
#define PORT$V_HASH 32
#define PORT$S_HASH 6
#define PORT$C_LENGTH 612
struct PORTDEF {
    int *PORT$A_PORTQB;
    int *PORT$A_INTR_SRV;
    unsigned short int PORT$W_SIZE;
    unsigned char PORT$B_TYPE;
    unsigned char PORT$B_SUB_TYPE;
    unsigned char PORT$B_VC_NUM;
    unsigned char PORT$B_VC_CNT;
    unsigned char PORT$B_VC_LAST;
    unsigned char PORT$B_MAX_CACHE;
    int PORT$Q_FORK [2];
    union  {
        unsigned short int PORT$W_STS;
        struct  {
            unsigned PORT$V_AUTHORIZE : 1;
            unsigned PORT$V_FORK : 1;
            unsigned PORT$V_NEED_LBUF : 1;
            unsigned PORT$V_NEED_SBUF : 1;
            unsigned PORT$V_NEED_MFQ : 1;
            unsigned PORT$V_NEED_DFQ : 1;
            unsigned PORT$V_DISABLED : 1;
            unsigned PORT$V_SYNCH : 1;
            } PORT$R_STS_BITS;
        } PORT$R_STS_OVERLAY;
    unsigned char PORT$B_MAX_REXMT;
    unsigned char PORT$B_FLCK;
    int *PORT$A_FPC;
    unsigned long int PORT$L_FR3;
    unsigned long int PORT$L_FR4;
    union  {
        int PORT$Q_QUE_FIRST [2];
        struct  {
            int PORT$Q_VC_WORK [2];
            int PORT$Q_SBUF_FREE [2];
            int PORT$Q_LBUF_FREE [2];
            int PORT$Q_DFQ [2];
            int PORT$Q_MFQ [2];
            int PORT$Q_XMT_LDL [2];
            int PORT$Q_RWAITQ [2];
            int PORT$Q_DELAYQ [2];
            int PORT$Q_RSPQ [2];
            int PORT$Q_BUS_LIST [2];
            } PORT$R_QUE_ACTUAL;
        } PORT$R_QUE_OVERLAY;
    unsigned long int PORT$L_SECS_ZEROED;
    unsigned short int PORT$W_CLOCK;
    union  {
        unsigned short int PORT$W_WRK;
        struct  {
            unsigned PORT$V_WRK_RWAITQ : 1;
            unsigned PORT$V_WRK_VCQ : 1;
            unsigned PORT$V_WRK_INTR : 1;
            unsigned PORT$V_WRK_LDL : 1;
            unsigned PORT$V_WRK_TIMER : 1;
            unsigned PORT$V_fill_7 : 3;
            } PORT$R_WRK_BITS;
        } PORT$R_WRK_OVERLAY;
    int PORT$Q_AUTHORIZE [2];
    unsigned long int PORT$L_SERVICES;
    unsigned short int PORT$W_MAX_LNGMSG;
    unsigned short int PORT$W_MAX_LNGDG;
    int *PORT$A_VCVEC0;
    unsigned short int PORT$W_DELAY_ACK;
    unsigned short int PORT$W_DELAY_SEQ;
    char PORT$T_NODENAME [8];
    unsigned short int PORT$W_SBUF_SIZE;
    unsigned short int PORT$W_LBUF_SIZE;
    unsigned short int PORT$W_SBUF_CNT;
    unsigned short int PORT$W_SBUF_QUO;
    unsigned short int PORT$W_SBUF_MAX;
    unsigned short int PORT$W_LBUF_CNT;
    unsigned short int PORT$W_LBUF_QUO;
    unsigned short int PORT$W_LBUF_MAX;
    unsigned long int PORT$L_TOT_LBUF_ALLOCS;
    unsigned long int PORT$L_TOT_SBUF_ALLOCS;
    unsigned short int PORT$W_LBUF_LOOKASIDE_MISS;
    unsigned short int PORT$W_SBUF_LOOKASIDE_MISS;
    unsigned short int PORT$W_SBUF_INUSE_CNT;
    unsigned short int PORT$W_SBUF_INUSE_PEAK;
    unsigned short int PORT$W_LBUF_INUSE_CNT;
    unsigned short int PORT$W_LBUF_INUSE_PEAK;
    unsigned short int PORT$W_PCI_SBUF_EMPTY;
    unsigned short int PORT$W_PCI_LBUF_EMPTY;
    unsigned short int PORT$W_TR_SBUF_EMPTY;
    unsigned short int PORT$W_BUS_COUNT;
    int *PORT$A_LINK;
    unsigned char PORT$B_SCAN_XACK;
    unsigned char PORT$B_SCAN_XSEQ;
    unsigned char PORT$B_SCAN_SBUF;
    unsigned char PORT$B_NEED_SACK;
    unsigned long int PORT$L_SYSID_LO;
    unsigned short int PORT$W_SYSID_HI;
    unsigned short int PORT$W_GROUP;
    unsigned long int PORT$L_MCAST_LO;
    unsigned short int PORT$W_MCAST_HI;
    char PORT$B_FILL_BYTE_1;
    unsigned char PORT$B_TIM_ERR_LOG;
    union  {
        unsigned long int PORT$L_MAINT;
        struct  {
            unsigned PORT$V_MAINT_ID : 31;
            unsigned PORT$V_D : 1;
            } PORT$R_MAINT_S;
        } PORT$R_MAI_OVERLAY;
    unsigned char PORT$B_MINOR;
    unsigned char PORT$B_MAJOR;
    unsigned short int PORT$W_ECO;
    unsigned long int PORT$L_PORT_FCN;
    union  {
        unsigned long int PORT$L_STA_INFO;
        struct  {
            unsigned PORT$V_SPARE : 8;
            unsigned PORT$V_M : 1;
            unsigned PORT$V_PS : 2;
            unsigned PORT$V_SYS_STATE : 21;
            } PORT$R_STA_FIELDS;
        } PORT$R_STA_OVERLAY;
    char PORT$T_RST_PORT [6];
    short int PORT$W_FILL_WORD;
    char PORT$T_MASK_SBUF [32];
    char PORT$T_MASK_XACK [32];
    char PORT$T_MASK_XSEQ [32];
    char PORT$AL_VEC0_HASH [256];
    int *PORT$A_PDT;
    int *PORT$A_UCB;
    } ;
 
/*** MODULE $ROOTDEF ***/
#define ROOT$C_LENGTH 24
struct ROOTDEF {
    long int ROOT$L_FILL_LONG;
    int *ROOT$A_PORT_LIST;
    unsigned short int ROOT$W_SIZE;
    unsigned char ROOT$B_TYPE;
    unsigned char ROOT$B_SUB_TYPE;
    unsigned long int ROOT$L_INX_SPT0;
    int *ROOT$A_SVA;
    int *ROOT$A_PTE0;
    char ROOT$T_TQE [];
    } ;
 
/*** MODULE $CCDEF ***/
#define CC$C_EVT_HELLO 0
#define CC$C_EVT_BYE 1
#define CC$C_EVT_CCSTART 2
#define CC$C_EVT_VERF 3
#define CC$C_EVT_VACK 4
#define CC$C_EVT_DONE 5
#define CC$C_EVT_SOLICIT_SRV 6
#define CC$C_EVT_RESERVED 7
#define CC$C_EVT_TRANS 8
#define CC$C_EVT_CC_TIMER 9
#define CC$C_EVT_SHORT_MSG 10
#define CC$C_EVT_BAD_ECO 11
#define CC$C_EVT_AUTHORIZE 12
#define CC$C_EVT_BAD_MC 13
#define CC$C_EVT_INCOMP_CHNL 14
#define CC$C_STATE_CLOSED 0
#define CC$C_STATE_CCSTART_SENT 1
#define CC$C_STATE_CCSTART_REC 2
#define CC$C_STATE_SOLICIT_REC 3
#define CC$C_STATE_OPEN 4
#define CC$C_STATE_HELLO_DLY 5
#define CC$C_STATE_SOLICIT_DLY 6
#define CC$C_STATE_reserved 7
#define CC$C_STATE_TOTAL 8
struct CCDEF {
    unsigned long int CC$L_DUMMY;
    } ;
 
/*** MODULE $PEERLDEF ***/
#define PEERL$C_LENGTH 64
#define PEERL$C_PEM_LENGTH 62
#define PEERL$C_TOTAL_LONGWORDS 14
struct PEERLDEF {
    unsigned short int PEERL$W_SCS_PAD;
    unsigned short int PEERL$W_REGSAV_SIZE;
    unsigned short int PEERL$W_MBZ;
    unsigned char PEERL$B_ERROR_SUBTYPE;
    unsigned char PEERL$B_ERROR_TYPE;
    unsigned long int PEERL$L_IOSB1;
    unsigned long int PEERL$L_IOSB2;
    char PEERL$T_DL_DEVNAM [16];
    char PEERL$T_DST_NODENAME [16];
    unsigned long int PEERL$L_DST_ADDR_LO;
    unsigned short int PEERL$W_DST_ADDR_HI;
    unsigned long int PEERL$L_LOCAL_ADDR_LO;
    unsigned short int PEERL$W_LOCAL_ADDR_HI;
    unsigned short int PEERL$W_ERROR_COUNT;
    unsigned short int PEERL$W_DL_UNIT;
    } ;
