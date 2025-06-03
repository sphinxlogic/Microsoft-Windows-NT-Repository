/********************************************************************************************************************************/
/* Created  6-FEB-1992 15:45:21 by VAX SDL V3.2-12     Source:  4-FEB-1992 17:26:33 FIZBUZ$DUA0:[SAUNDERS.BANNER.C]LANUDEF.SDL;1 */
/********************************************************************************************************************************/
 
/*** MODULE $LANUDEF ***/
/*                                                                          */
/* LANUDEF Version Identification                                           */
/*                                                                          */
#define LAN$C_LANUDEF_VER 12
/* This constant is used (after swapping the protocol type bytes) to validate */
/* a protocol type.                                                         */
#define LAN$C_PTY_MIN 1501
/* Protocol Types                                                           */
#define NI$C_PTY_DUMP_LOAD 352          /* DNA Dump/Load (MOP)              */
#define NI$C_PTY_RMTCN 608              /* DNA Remote Console (MOP)         */
#define NI$C_PTY_ROUTE 864              /* DNA Routing                      */
#define NI$C_PTY_LAT 1120               /* Local Area Transport (LAT)       */
#define NI$C_PTY_DIAG 1376              /* Diagnostics                      */
#define NI$C_PTY_CUST 1632              /* Customer Use                     */
#define NI$C_PTY_SCA 1888               /* System Communication Architecture (SCA) */
#define NI$C_PTY_LOOPB 144              /* Loopback Assist protocol type    */
#define NI$C_PTY_TCPIP 8                /* TCP/IP                           */
#define NI$C_PTY_ARP 1544               /* ARP                              */
#define NI$C_PTY_BRIDGE 14464           /* BRIDGE                           */
#define NI$C_PTY_ELN 15232              /* VAX ELN                          */
#define NI$C_PTY_DNAME 15488            /* DNA Name Service                 */
#define NI$C_PTY_ENCRY 15744            /* Encryption                       */
#define NI$C_PTY_DTIME 16000            /* DNA Time Service                 */
#define NI$C_PTY_LTM 16256              /* LTM                              */
#define NI$C_PTY_BIOS 16512             /* BIOS                             */
#define NI$C_PTY_LAST 16768             /* Local Area Storage (LAST)        */
/* DIGITAL multicast address assignments                                    */
#define NI$C_MCA_L_MOP 16777387         /* DNA Dump/Load Assistance (MOP)   */
#define NI$C_MCA_W_MOP 0                /* DNA Dump/Load Assistance (MOP)   */
#define NI$C_MCA_L_RMTCN 33554603       /* DNA Remote Console (MOP)         */
#define NI$C_MCA_W_RMTCN 0              /* DNA Remote Console (MOP)         */
#define NI$C_MCA_L_ROUTERS 50331819     /* DNA Routing Routers              */
#define NI$C_MCA_W_ROUTERS 0            /* DNA Routing Routers              */
#define NI$C_MCA_L_ENDNODE 67109035     /* DNA Routing End nodes            */
#define NI$C_MCA_W_ENDNODE 0            /* DNA Routing End nodes            */
#define NI$C_MCA_L_LAT 196779           /* Local Area Transport (LAT)       */
#define NI$C_MCA_W_LAT 0                /* Local Area Transport (LAT)       */
#define NI$C_MCA_L_CUST 262315          /* Customer Use                     */
/*  CONSTANT	MCA_W_CUST	EQUALS  %X0000-%XFFFF TAG C PREFIX NI$; /* Customer Use */
#define NI$C_MCA_L_SCA 17039531         /* System Communication Architecture (SCA) */
/*  CONSTANT	MCA_W_SCA	EQUALS  %X0000-%XFFFF TAG C PREFIX NI$; /* System Communication Architecture (SCA) */
/* Cross-company address assignments                                        */
#define NI$C_CCA_L_BROADCAST -1         /* Broadcast                        */
#define NI$C_CCA_W_BROADCAST 65535      /* Broadcast                        */
#define NI$C_CCA_L_LOOPBACKA 207        /* Loopback Assistance              */
#define NI$C_CCA_W_LOOPBACKA 0          /* Loopback Assistance              */
/* DNA SNAP SAP assignments                                                 */
#define NI$C_NULL_SAP 0                 /* The NULL SAP                     */
#define NI$C_LLC_MGT_FNC_SAP 2          /* LLC SUBLAYER MANAGEMENT FUNCTION SAP */
#define NI$C_LLC_MGT_SAP 3              /* LLC SUBLAYER MANAGEMENT SAP      */
#define NI$C_SNAP_SAP 170               /* The SNAP SAP                     */
/* DNA DSAP assignments                                                     */
#define NI$C_GLOBAL_DSAP 255            /* GLOBAL DSAP                      */
/* DNA GROUP SAP assignments                                                */
#define NI$C_MAX_NO_GSAP 4              /* Maximum number of group SAPs per user */
/* LAN frame constants                                                      */
#define NI$C_CRC 4                      /* Size of Ethernet CRC             */
#define NI$C_ADDR_SIZE 6                /* Maximum size of NI address       */
#define NI$C_LEN_SIZE 2                 /* Size of the length field         */
/* CSMACD frame constants                                                   */
#define NI$C_MAX_PKTSIZE 1500           /* Maximum of Ethernet Packet without padding */
#define NI$C_MAX_PKTSIZE_PAD 1498       /* Maximum of Ethernet Packet with padding */
#define NI$C_MAX_PKTSIZE_CLI 1497       /* Maximum of 802 Packet            */
#define NI$C_MAX_PKTSIZE_USR 1496       /* Maximum of 802 Packet            */
#define NI$C_MAX_PKTSIZE_802E 1492      /* Maximum of 802E Packet           */
#define NI$C_MAX_PKTSIZE_HWA 1518       /* Maximum hardware Packet including header and CRC */
#define NI$C_MIN_PKTSIZE 46             /* Minimum of Ethernet Packet       */
/*                                                                          */
/* Frame/packet header sizes for CSMACD                                     */
/*                                                                          */
#define NI$C_HDRSIZE_ETH 14             /* Maximum of Ethernet Packet without padding */
#define NI$C_HDRSIZE_PAD 16             /* Maximum of Ethernet Packet with padding */
#define NI$C_HDRSIZE_CLI 17             /* Maximum of 802 Packet            */
#define NI$C_HDRSIZE_USR 18             /* Maximum of 802 Packet            */
#define NI$C_HDRSIZE_802E 22            /* Maximum of 802E Packet           */
/* LAN Item List definitions                                                */
#define DLL$K_DL_ENTITY 1               /* Create Input                     */
#define DLL$K_CLIENT 2                  /* Create Input                     */
#define DLL$K_DL_TEMPLATE 3             /* Create Input                     */
#define DLL$K_LAN_DEVICE 4              /* Create Input                     */
#define DLL$K_PORT_ENTITY 5             /* Create Output                    */
#define DLL$K_MINIMUM_BUFFER_SIZE 6     /* Enable Input                     */
#define DLL$K_PREFERRED_BUFFER_SIZE 7   /* Enable Input                     */
#define DLL$K_PROTOCOLID 8              /* Enable Input                     */
#define DLL$K_DECUSERDATA 9             /* Enable Input                     */
#define DLL$K_PROFILE 10                /* Enable Output                    */
#define DLL$K_ACTUAL_BUFFER_SIZE 11     /* Enable Output                    */
/*++                                                                        */
/*                                                                          */
/* This structure represents a LAN module entity.  There will be one used to */
/* represent CSMA/CD; even if there are multiple CSMA/CD stations.  There will */
/* also be one used to represent FDDI.                                      */
/*                                                                          */
/* These are the common fields of the LAN module entity.                    */
/*--                                                                        */
#define LAN$K_FIXED_LENGTH 76           /* Length of LAN block              */
struct LANUDEF {
/* This is version 1 of the LAN structure.  Use #1 (no symbol) for the      */
/* version field.                                                           */
    unsigned long int LAN$L_VERSION;    /* Version of LAN database          */
    unsigned long int LAN$L_RESERVED1;  /* Reserved                         */
    int *LAN$A_GET_DEVICE;              /* Address of LAN$GET_DEVICE        */
    unsigned long int LAN$L_RESERVED2;  /* Reserved                         */
    int *LAN$A_CREATE_PORT;             /* Address of LAN$CREATE_PORT       */
    int *LAN$A_DELETE_PORT;             /* Address of LAN$DELETE_PORT       */
    char LAN$T_TQE [52];
/* TQE for the one second timer                                             */
    } ;
 
/*** MODULE $LDCDEF ***/
/*++                                                                        */
/*                                                                          */
/* LAN Device Characteristics structure                                     */
/*                                                                          */
/* This data structure is returned on a call to LAN$GET_DEVICE.  It is      */
/* embedded in the LSB.                                                     */
/*                                                                          */
/*--                                                                        */
#define LDC$K_LENGTH 24                 /* Length of LDC block              */
struct LDCDEF {
    int *LDC$A_NAME;                    /* Address of a counted string      */
/* containing the device name                                               */
    unsigned short int LDC$W_TYPE;      /* Type of device - same as         */
/* VCIB$W_DLL_TYPE                                                          */
    unsigned short int LDC$W_RCVSIZE;   /* Number of entries in the receive */
/* ring                                                                     */
    unsigned long int LDC$L_RATE128;    /* Kilobits per second with 128 bytes */
/* of user data (on transmit)                                               */
    unsigned long int LDC$L_RATE256;    /* Kilobits per second with 256 bytes */
/* of user data (on transmit)                                               */
    unsigned char LDC$B_DEVTYPE;        /* VMS Device type (from the UCB)   */
    unsigned char LDC$B_MOPID;          /* MOP ID for this device           */
    short int LDC$W_EXTRA_W;            /* Preserve alignment               */
    unsigned long int LDC$L_MOPNAME;    /* Counted string (always three     */
/* characters) for MOP name                                                 */
/* Remember to make sure that this structure is an integral number of       */
/* quadwords.  That helps keep the LSB fields aligned.                      */
    } ;
 
/*** MODULE $LILDEF ***/
/*++                                                                        */
/*                                                                          */
/* LAN Item List definitions                                                */
/*                                                                          */
/* This data structure should look like a DECnet-VAX item list.  We don't use */
/* the DECnet-VAX item list symbols because they are not available.         */
/*                                                                          */
/*--                                                                        */
#define LIL$T_DATA 12                   /* Start of the itemlist            */
#define LIL$T_ITEMVAL 4                 /* Value for this item              */
struct LILDEF {
    union  {
/* This structure definition will describe the header of the itemlist.      */
        struct  {
            unsigned long int LIL$L_LISTLEN; /* Number of bytes in the list */
            int *LIL$A_LISTADR;         /* Address of the list              */
            unsigned short int LIL$W_SIZE; /* Size of structure             */
            unsigned char LIL$B_TYPE;   /* Type = DYN$C_NET                 */
            unsigned char LIL$B_SUBTYPE; /* Subtype = DYN$C_NET_ITEM        */
            } LIL$R_HEADER_FIELDS;
/* This structure definition will describe an item in the itemlist.  Note   */
/* that the ITEMLEN field includes the size of the ITEMLEN, ITEMTAG, and    */
/* ITEMVAL fields.                                                          */
        struct  {
            unsigned short int LIL$W_ITEMLEN; /* Number of bytes in the item */
            unsigned short int LIL$W_ITEMTAG; /* Size of structure          */
            } LIL$R_LIST_FIELDS;
        } LIL$R_LIL_UNION;
    } ;
 
/*** MODULE $VCIBDLLDEF ***/
/*++                                                                        */
/*                                                                          */
/* Data Link definitions that follow the standard VCIB fields               */
/*                                                                          */
/*--                                                                        */
#define VCIB$M_DLL_RCV_DCB 1
#define VCIB$M_DLL_RCV_MAC 2
#define VCIB$M_DLL_XMT_CHAIN 1
#define VCIB$M_DLL_XMT_RCVRSB 2
#define VCIB$M_DLL_PORT_STATUS 4
#define VCIB$K_DLL_CSMACD 1             /* CSMACD data link                 */
#define VCIB$K_DLL_DDCMP 2              /* DDCMP  data link                 */
#define VCIB$K_DLL_FDDI 3               /* FDDI   data link                 */
#define VCIB$K_DLL_HDLC 4               /* HDLC   data link                 */
#define VCIB$K_DLL_LAPB 5               /* LAPB   data link                 */
#define VCIB$M_LAN_RCV_LIM 1
#define VCIB$M_LAN_RCV_ICS 2
#define VCIB$M_LAN_FTC 4
#define VCIB$M_LAN_2ND_DEV 8
#define VCIB$M_LAN_SFR 16
#define VCIB$M_LAN_SCC 32
#define VCIB$M_LAN_RCV_DCB 1
#define VCIB$M_LAN_DP_DELAY 2
#define VCIB$K_LAN_FIXED_LENGTH 140     /* Length of LAN VCIB               */
#define VCIB$M_SYN_UNSEQ 1
#define VCIB$K_SYN_FIXED_LENGTH 86      /* Length of SYN VCIB               */
struct VCIBDLLDEF {
/* First get to the end of the common VCIB section.  Then we can start      */
/* the overlay.                                                             */
    char VCIB$B_DLL_FILLER [60];
/* DLL extensions to the VCIB.  Note that the DLL extension includes        */
/* some generic extensions (DLL) followed by an overlay of the LAN          */
/* and SYN extensions.                                                      */
    int *VCIB$A_DLL_INPUT_LIST;         /* Address of input  item list      */
    int *VCIB$A_DLL_OUTPUT_LIST;        /* Address of output item list      */
    union  {
        unsigned short int VCIB$W_DLL_CLIENT_FLAGS; /* Flags specified by client */
        struct  {
            unsigned VCIB$V_DLL_RCV_DCB : 1; /* 1 = Can pass DCB on RCV     */
            unsigned VCIB$V_DLL_RCV_MAC : 1; /* determines where            */
/* VCRP$A_LAN_R_HEADER points:                                              */
/* 0 = the LHI structure                                                    */
/* 1 = MAC specific frame header                                            */
/*	FDDI:	[FC]                                                        */
/*	CSMACD:	[DA]                                                        */
            unsigned VCIB$V_fill_0 : 6;
            } VCIB$R_DLL_FLAGS_1;
        } VCIB$R_DLL_OVERLAY_1;
    union  {
        unsigned short int VCIB$W_DLL_FLAGS; /* Flags specified by data link */
        struct  {
            unsigned VCIB$V_DLL_XMT_CHAIN : 1; /* 1 = device can chain      */
            unsigned VCIB$V_DLL_XMT_RCVRSB : 1; /* 1 = DL can XMT on RCV RSB */
            unsigned VCIB$V_DLL_PORT_STATUS : 1; /* 1 = PORT USABLE was last event */
/* 0 = PORT UNUSABLE was last                                               */
/*     event, OR no USABLE/UNUSABLE                                         */
/*     event has occured yet                                                */
            unsigned VCIB$V_fill_1 : 5;
            } VCIB$R_DLL_FLAGS_2;
        } VCIB$R_DLL_OVERLAY_2;
    unsigned short int VCIB$W_DLL_TYPE; /* Type of data link                */
    unsigned short int VCIB$W_DLL_HDR_SIZE; /* Max data link header in bytes */
    unsigned short int VCIB$W_DLL_XMT_SIZE; /* Max transmit in bytes        */
    unsigned short int VCIB$W_DLL_CHAIN_SIZE; /* Min size of 1st chain in bytes */
/* Define the values for the field VCIB$W_DLL_TYPE.                         */
    union  {
/* LAN extensions to the VCIB; after the DLL extensions.                    */
        struct  {
            int *VCIB$A_LAN_LPB;        /* Address of LAN Port Block        */
            int *VCIB$A_LAN_UCB;        /* Address of UCB                   */
            int *VCIB$A_LAN_BUILD_HDR;  /* Address of LAN's Build Header    */
/* service                                                                  */
            int *VCIB$A_LAN_TRANSMIT_FRAME; /* Address of LAN's Transmit    */
/* Frame service                                                            */
            int *VCIB$A_LAN_TRANSMIT_AVAIL; /* Address of available Transmits */
            unsigned short int VCIB$W_LAN_MAX_RCV; /* Max outstanding receives */
            unsigned short int VCIB$W_LAN_OUT_RCV; /* Number of outstanding receives */
            union  {
                unsigned short int VCIB$W_LAN_CLIENT_FLAGS; /* Flags specified by client */
                struct  {
                    unsigned VCIB$V_LAN_RCV_LIM : 1; /* 1 = DL will limit the # */
/*     of RCV VCRPs                                                         */
                    unsigned VCIB$V_LAN_RCV_ICS : 1; /* 1 = DL must init context */
/*     stack in RCV VCRPs                                                   */
                    unsigned VCIB$V_LAN_FTC : 1; /* 1 = Fast Transmit Complete */
                    unsigned VCIB$V_LAN_2ND_DEV : 1; /* 1 = Use 2nd device  */
                    unsigned VCIB$V_LAN_SFR : 1; /* 1 = Skip field restore  */
                    unsigned VCIB$V_LAN_SCC : 1; /* 1 = Skip completion check */
                    unsigned VCIB$V_fill_2 : 2;
                    } VCIB$R_LAN_FLAGS_1;
                } VCIB$R_LAN_OVERLAY_1;
            union  {
                unsigned short int VCIB$W_LAN_FLAGS; /* Flags specified by LAN */
                struct  {
                    unsigned VCIB$V_LAN_RCV_DCB : 1; /* 1 = DL supports delivery of */
/*     VCRP/DCB sets                                                        */
                    unsigned VCIB$V_LAN_DP_DELAY : 1; /* 1 = Disable Port is being */
/*     delayed                                                              */
                    unsigned VCIB$V_fill_3 : 6;
                    } VCIB$R_LAN_FLAGS_2;
                } VCIB$R_LAN_OVERLAY_2;
            int *VCIB$A_LAN_LDC;        /* Address of the LDC structure     */
            long int VCIB$L_LAN_OOCNT;  /* Number of operations outstanding */
            int *VCIB$A_LAN_DP_VCRP;    /* Address of the Disable Port VCRP */
/* being disabled.  Only valid if                                           */
/* the DP_DELAY flag is set.                                                */
            int *VCIB$A_LAN_HWA;        /* pointer to HWA/MLA               */
            int *VCIB$A_LAN_PHA;        /* pointer to PHA                   */
            long int VCIB$L_LAN_RESERVED1; /* For future expansion          */
            int VCIB$Q_LAN_RESERVED2 [2]; /* For future expansion           */
            } VCIB$R_LAN_VCIB;
/* SYN extensions to the VCIB; after the DLL extensions.                    */
        struct  {
            int *VCIB$A_SYN_UP_PORT;    /* Used only by the data link       */
            union  {
                unsigned short int VCIB$W_SYN_CLIENT_FLAGS; /* Flags specified by client */
                struct  {
                    unsigned VCIB$V_SYN_UNSEQ : 1; /* 1 = Client wants unseq port */
                    unsigned VCIB$V_fill_4 : 7;
                    } VCIB$R_SYN_FLAGS_1;
                } VCIB$R_SYN_OVERLAY_1;
            } VCIB$R_SYN_VCIB;
        } VCIB$R_DLL_VCIB_ADDITIONS;
    } ;
 
/*** MODULE $VCRPLANDEF ***/
/*++                                                                        */
/*                                                                          */
/* LAN definitions that are embedded in the standard VCRP fields            */
/*                                                                          */
/*--                                                                        */
#define VCRP$K_FC_DLL_BASE 512
#define VCRP$K_FC_SYN_ENTER_MAINT 512
#define VCRP$K_FC_SYN_EXIT_MAINT 513
#define VCRP$K_FC_X25_RESET 514
#define VCRP$K_FC_X25_ACK_NOCOM 515
#define VCRP$K_FC_LAN_CHANGE_PORT 516
#define VCRP$K_FC_DLL_LAST 517
/* Data Link event codes                                                    */
#define VCRP$K_EC_DLL_BASE 4608
#define VCRP$K_EC_DLL_PORT_USABLE 4608
#define VCRP$K_EC_DLL_PORT_UNUSABLE 4609
#define VCRP$K_EC_LAN_NEW_ADDRESS 4610
#define VCRP$K_EC_LAN_RCV_CONGESTION 4611
#define VCRP$K_EC_LAN_RCV_PDU_LOST 4612
#define VCRP$K_EC_SYN_STATION_DELETED 4613
#define VCRP$K_EC_X25_INCOMING_RESET 4614
#define VCRP$K_EC_X25_INCOMING_NOCOM 4615
#define VCRP$K_EC_LAN_RESTART_FAIL 4616
#define VCRP$K_EC_LAN_STATION_RENAMED 4617
#define VCRP$K_EC_DLL_LAST 4618
/* Since we need to overlay many fields, we will do each field in a         */
#define VCRP$L_LAN_802XMT 204           /* 802.2 XMT parameters             */
#define VCRP$M_LAN_FLTR_MCA 1
#define VCRP$M_LAN_FLTR_CTL 2
#define VCRP$M_LAN_FLTR_SRC 4
#define VCRP$M_LAN_FLTR_STARTUP 1
#define VCRP$M_LAN_FLTR_INTXMIT 2
#define VCRP$M_LAN_FLTR_DAT 4
#define VCRP$M_LAN_CHAINED 1
#define VCRP$M_LAN_SVAPTE 2
#define VCRP$M_LAN_1ST_SEG 4
#define VCRP$T_LAN_DATA 264
/* Now define maximum length needed, allowing for maximum alignment         */
#define VCRP$C_LAN_OVERHEAD 326
/* Now define all the fields in the various headers                         */
#define VCRP$W_LAN_LEN_802 276          /* 802 length field                 */
#define VCRP$T_LAN_USER_ETH 278         /* Start of user ETH data           */
#define VCRP$W_LAN_SIZE 278             /* Size of message if padded        */
#define VCRP$X_LAN_CTL 280              /* 802 CTL field                    */
#define VCRP$G_LAN_PID 281              /* 5-byte Protocol Identifier       */
#define VCRP$T_LAN_USER_802E 286        /* Start of user 802E data          */
struct VCRPLANDEF {
/* First we will define all the function codes and all the event codes      */
/* used specifically by the data links.                                     */
/* Data Link function codes                                                 */
/* separate overlay.  That way, we can do them in any order.                */
    union  {
/*========================================================================  */
/* Data Request - For receive, LAN needs a field that will point to the     */
/* data link header.                                                        */
        struct  {
            char VCRP$B_LAN_FILLER1 [60];
            int *VCRP$A_LAN_R_HEADER;   /* Address of receive packet header	 */
            unsigned char VCRP$B_LAN_FC; /* FC value for FDDI xmit/recv     */
            } VCRP$R_VCRPLAN_DATA_REQUEST;
/*========================================================================  */
/* Mgmt Request - LAN needs the P2 buffer fields.                           */
        struct  {
            char VCRP$B_LAN_FILLER2 [48];
            unsigned long int VCRP$L_LAN_P2BUFF_SIZE;
/* LAN P2 buffer size                                                       */
            int *VCRP$A_LAN_P2BUFF;     /* LAN P2 buffer                    */
            } VCRP$R_VCRPLAN_MGMT_REQUEST;
/*========================================================================  */
/* Scratch area - LAN needs many fields here.                               */
        struct  {
            char VCRP$B_LAN_FILLER3 [196];
            unsigned int VCRP$Q_LAN_T_DEST [2]; /* Destination address      */
            unsigned short int VCRP$W_LAN_T_CTL; /* 802.2 ctl field         */
            unsigned char VCRP$B_LAN_T_CTL_SIZE; /* 802.2 ctl field value size */
            unsigned char VCRP$B_LAN_T_DSAP; /* 802.2 XMT dest. sap         */
            unsigned char VCRP$B_LAN_T_RESP; /* 802.2 command/response flag */
            union  {
                union  {
                    unsigned char VCRP$B_LAN_R_FILTER; /* Receive filtering mask  */
                    struct  {
                        unsigned VCRP$V_LAN_FLTR_MCA : 1; /* MCA filtering has been performed */
                        unsigned VCRP$V_LAN_FLTR_CTL : 1; /* CTL filtering has been performed */
                        unsigned VCRP$V_LAN_FLTR_SRC : 1; /* SRC filtering has been performed */
                        unsigned VCRP$V_fill_7 : 5;
                        } VCRP$r_fill_6;
                    } VCRP$r_fill_5;
                union  {
                    unsigned char VCRP$B_LAN_T_FILTER; /* Transmit filtering mask  */
                    struct  {
                        unsigned VCRP$V_LAN_FLTR_STARTUP : 1; /* Delete VCRP, complete IRP */
                        unsigned VCRP$V_LAN_FLTR_INTXMIT : 1; /* Add VCRP to Receive list */
                        unsigned VCRP$V_LAN_FLTR_DAT : 1; /* Duplicate Address Test */
/* VCRP for LOOPBACK                                                        */
                        unsigned VCRP$V_fill_10 : 5;
                        } VCRP$r_fill_9;
                    } VCRP$r_fill_8;
                } VCRP$R_LAN_FILTER;
            unsigned char VCRP$B_LAN_PORT1; /* Port specific byte           */
            unsigned char VCRP$B_LAN_PORT2; /* Port specific byte           */
            unsigned short int VCRP$W_LAN_HDR_SIZE; /* Size of receive hdr  */
            unsigned char VCRP$B_LAN_R_FMT; /* Receive packet format        */
            int *VCRP$A_LAN_UCB;        /* Address of UCB                   */
            unsigned char VCRP$B_LAN_DIAG_FLAGS; /* Diagnostic flags        */
            unsigned char VCRP$B_LAN_FUNCTION; /* Function for this request */
            unsigned short int VCRP$W_LAN_CODE; /* Parameter code           */
            unsigned short int VCRP$W_LAN_LENERR; /* Diagnostic info.       */
            unsigned short int VCRP$W_LAN_STATUS; /* LAN status info.       */
            union  {
                unsigned char VCRP$B_LAN_FLAGS; /* Flags                    */
                struct  {
                    unsigned VCRP$V_LAN_CHAINED : 1; /* Transmit is chained */
                    unsigned VCRP$V_LAN_SVAPTE : 1; /* 2nd entry uses SVAPTE/BOFF */
                    unsigned VCRP$V_LAN_1ST_SEG : 1; /* 1st entry is being worked */
                    unsigned VCRP$V_fill_13 : 5;
                    } VCRP$r_fill_12;
                } VCRP$r_fill_11;
/* When the CHAINED bit is set, the following fields are used to get to     */
/* the second buffer in the chain.  DCBBCNT has the number of bytes in      */
/* the second entry.  Either DCBSVA or DCBSVAPTE/DCBBOFF are used to        */
/* locate the buffer.  Use DCBSVA if the SVAPTE flag is clear.  Use         */
/* DCBSVAPTE/DCBBOFF if the SVAPTE flag is set.                             */
            unsigned long int VCRP$L_LAN_DCBBCNT; /* Size of 2nd entry      */
            union  {
                unsigned long int VCRP$L_LAN_DCBSVA; /* SVA of 2nd entry    */
                unsigned long int VCRP$L_LAN_DCBSVAPTE; /* SVAPTE of 2nd entry */
                } VCRP$R_LAN_UNION1;
            unsigned long int VCRP$L_LAN_DCBBOFF; /* BOFF of 2nd entry      */
/* If requested, we will save the buffer descriptor fields here on a        */
/* transmit request so that we can restore them when we are finished.       */
            unsigned long int VCRP$L_LAN_SBOFF; /* Saved BOFF value         */
            unsigned long int VCRP$L_LAN_SBCNT; /* Saved BCNT value         */
            unsigned long int VCRP$L_LAN_STPS; /* Saved TOTAL_PDU_SIZE value */
            } VCRP$R_VCRPLAN_SCRATCH;
/*========================================================================  */
/* Receive PDU area - We will define the fields that describe the           */
/* fields in the received frames.  Note that the DA must be quadword        */
/* aligned for some devices.                                                */
        struct  {
            char VCRP$B_LAN_FILLER4 [260];
/* First we need to align the data field to be quadword aligned.            */
/* In the furture we should do this so that it adjusts itself.              */
/* Right now we are hacking in a longword because we know that's            */
/* what we need.                                                            */
            unsigned long int VCRP$L_LAN_ALIGN; /* Need quadword alignment  */
/* Now define the generic "beginning of receive data" field                 */
/* in any driver (currently that maximum is 31 bytes for FXDRIVER)          */
            short int VCRP$G_LAN_DEST [3]; /* Destination address           */
            short int VCRP$G_LAN_SRC [3]; /* Source address                 */
            unsigned short int VCRP$W_LAN_PTYPE; /* Protocol type           */
            unsigned char VCRP$B_LAN_DSAP; /* 802 DSAP field                */
            unsigned char VCRP$B_LAN_SSAP; /* 802 SSAP field                */
            } VCRP$R_VCRPLAN_RCVPDU;
        } VCRP$R_VCRPLAN_OVERLAY;
    } ;
 
/*** MODULE $PROTODEF ***/
/*                                                                          */
/*	LAN FRAME/PACKET DEFINITONS                                         */
/*	FRAME TYPES:                                                        */
/*		FDDI                                                        */
/*		CSMA/CD                                                     */
/*		    Ethernet                                                */
/*		    IEEE 802.3                                              */
/*	PACKET TYPES                                                        */
/*		Mapped Ethernet	(FDDI only)                                 */
/*		802.2                                                       */
/*		802E                                                        */
/*                                                                          */
/*                                                                          */
/*	FDDI Frame Header                                                   */
/*                                                                          */
#define FDDI$C_HDR_LEN 13               /* Size of FDDI Header              */
struct FDDI_HDR {                       /* FDDI Frame header structure      */
    unsigned char FDDI$B_FC;            /* Frame Control field              */
    unsigned char FDDI$G_DA [6];        /* Destination Address field        */
    unsigned char FDDI$G_SA [6];        /* Source Address field             */
    } ;
/*                                                                          */
/* Maximum Frame length for FDDI including all fields from FC through CRC   */
/*                                                                          */
#define FDDI$C_MAX_FRM_LEN 4495
/*                                                                          */
/* Maximum Packet lengths for FDDI users                                    */
/*                                                                          */
#define FDDI$C_MAX_PKTSIZE 4478         /* Maximum Packet for FDDI (SMT user can send this size) */
#define FDDI$C_MAX_PKTSIZE_ETH 4470     /* Maximum of Mapped Ethernet Packet on FDDI */
#define FDDI$C_MAX_PKTSIZE_PAD 4468     /* Maximum of Mapped Ethernet Packet with padding on FDDI */
#define FDDI$C_MAX_PKTSIZE_CLI 4475     /* Maximum of 802 Packet on FDDI    */
#define FDDI$C_MAX_PKTSIZE_USR 4474     /* Maximum of 802 Packet on FDDI    */
#define FDDI$C_MAX_PKTSIZE_802E 4470    /* Maximum of 802E Packet on FDDI   */
#define FDDI$C_MAX_PKTSIZE_HWA 4495     /* Maximum hardware Packet including header and CRC */
#define FDDI$C_MIN_PKTSIZE 0            /* Minimum Packet (any user)        */
/*                                                                          */
/* Frame/packet header sizes for FDDI                                       */
/*                                                                          */
#define FDDI$C_HDRSIZE_ETH 21           /* Maximum of Ethernet Packet without padding */
#define FDDI$C_HDRSIZE_PAD 23           /* Maximum of Ethernet Packet with padding */
#define FDDI$C_HDRSIZE_CLI 16           /* Maximum of 802 Packet            */
#define FDDI$C_HDRSIZE_USR 17           /* Maximum of 802 Packet            */
#define FDDI$C_HDRSIZE_802E 21          /* Maximum of 802E Packet           */
#define FDDI$C_MAX802_PKTSIZ 4475
#define FDDI$C_MAX802E_PKTSIZ 4470
#define FDDI$C_MAXMPETH_PKTSIZ 4470
#define FDDI$C_MAXMPETHPAD_PKTSIZ 4468
/*                                                                          */
/* Constants for FC definitions                                             */
/*                                                                          */
#define FDDI$V_FC_PPP 0                 /* FC PPP field starts at bit 0     */
#define FDDI$M_FC_PPP 7                 /* FC PPP field is low order three bits */
#define FDDI$S_FC_PPP 3                 /* FC PPP field is 3 bits wide      */
#define FDDI$C_FC_LLC_MIN 80            /* FC for LLC Users (minimum)       */
#define FDDI$C_FC_LLC_DEF 80            /* FC for LLC Users (default)       */
#define FDDI$C_FC_LLC_MAX 87            /* FC for LLC Users (maximum)       */
#define FDDI$C_FC_SMT_MIN 65            /* FC for LLC Users (minimum)       */
#define FDDI$C_FC_SMT_DEF 65            /* FC for LLC Users (default)       */
#define FDDI$C_FC_SMT_MAX 79            /* FC for LLC Users (maximum)       */
/*                                                                          */
/* CSMACD (Ethernet/IEEE 802.3) Frame Header                                */
/*                                                                          */
#define CSMACD$C_HDR_LEN 14             /* Size of CSMACD Header            */
#define CSMACD$C_PAD_LEN 16             /* Size of CSMACD Header            */
struct CSMACD_HDR {
    unsigned char CSMACD$G_DA [6];      /* Destination Address field        */
    unsigned char CSMACD$G_SA [6];      /* Source Address field             */
/* The next field (field3) has different meanings depending on the          */
/* Specific CSMACD frame type (Ethernet vs. IEEE 802.3):                    */
    union  {
        struct  {
            unsigned short int CSMACD$W_PTY; /* Ethernet Protocol Type field */
            } CSMACD$r_field3_eth;
        struct  {
            unsigned short int CSMACD$W_LEN_8023; /* IEEE 802.3 Length field */
            } CSMACD$r_field3_802;
        } CSMACD$r_field3;
    unsigned short int CSMACD$W_PAD;    /* optional Ethenet Length (PAD) field */
    } ;
/*                                                                          */
/* Maximum Frame length for FDDI including all fields from FC through CRC   */
/*                                                                          */
#define CSMACD$C_MAX_FRM_LEN 1518
/*                                                                          */
/* Maximum Packet lengths for CSMACD users                                  */
/*                                                                          */
#define CSMACD$C_MAX_PKTSIZE 1500       /* Maximum of Ethernet Packet without padding */
#define CSMACD$C_MAX_PKTSIZE_ETH 1500   /* Maximum of Ethernet Packet without padding */
#define CSMACD$C_MAX_PKTSIZE_PAD 1498   /* Maximum of Ethernet Packet with padding */
#define CSMACD$C_MAX_PKTSIZE_CLI 1497   /* Maximum of 802 Packet            */
#define CSMACD$C_MAX_PKTSIZE_USR 1496   /* Maximum of 802 Packet            */
#define CSMACD$C_MAX_PKTSIZE_802E 1492  /* Maximum of 802E Packet           */
#define CSMACD$C_MAX_PKTSIZE_HWA 1518   /* Maximum hardware Packet including header and CRC */
#define CSMACD$C_MIN_PKTSIZE 46         /* Minimum of Ethernet Packet       */
#define CSMACD$C_HDRSIZE_ETH 14         /* Maximum of Ethernet Packet without padding */
#define CSMACD$C_HDRSIZE_PAD 16         /* Maximum of Ethernet Packet with padding */
#define CSMACD$C_HDRSIZE_CLI 17         /* Maximum of 802 Packet            */
#define CSMACD$C_HDRSIZE_USR 18         /* Maximum of 802 Packet            */
#define CSMACD$C_HDRSIZE_802E 22        /* Maximum of 802E Packet           */
#define CSMACD$C_MAX802_PKTSIZ 1497
#define CSMACD$C_MAX802E_PKTSIZ 1492
#define CSMACD$C_MAXETH_PKTSIZ 1500
#define CSMACD$C_MAXETHPAD_PKTSIZ 1498
/*                                                                          */
/*	Packet Header Definitions                                           */
/*                                                                          */
/*	Packet headers begin at the byte following the FDDI or CSCMACD      */
/*	frame header. (at offsets FDDI$C_HDR_LEN/CSMACD$C_HDR_LEN)          */
/*                                                                          */
/*	Packet types supported:                                             */
/*		- FDDI mapped ethernet (for FDDI only)                      */
/*		- IEEE 802.1 SNAP SAP (802E/802PID)                         */
/*		- IEEE 802.2, Class I, including (Type 1 packets only):     */
/*			-- UI                                               */
/*			-- XID                                              */
/*			-- TEST                                             */
/*                                                                          */
/*	FDDI Mapped Ethernet packet header                                  */
/*                                                                          */
#define MPDETH$C_LEN 8                  /* Size of Mapped Ethernet Header without Padding */
#define MPDETH$C_PAD_LEN 10             /* Size of Mapped Eth Header with Padding */
struct MPDETH_HDR {
    unsigned char MPDETH$G_SNAP_SAP [3]; /* SNAP-SAP <AA-00-03> hex         */
    unsigned char MPDETH$G_MAP_PID [3]; /* SNAP PID <00-00-00>              */
    unsigned short int MPDETH$W_PTY;    /* Ethernet Protocol Type field     */
    unsigned short int MPDETH$W_PAD;    /* PAD (length) field (If User enables it) */
    } ;
/*                                                                          */
/*	IEEE Standard 802.2 (LLC), packet header                            */
/*                                                                          */
#define LLC$C_ONEBYTCTL_LEN 3           /* Size of 802.2 Header with        */
/* one byte CTL                                                             */
#define LLC$G_ONEBYT_DATA 3             /* Start of User Data               */
/* with 1 byte control field                                                */
#define LLC$C_TWOBYTCTL_LEN 4           /* Size of 802.2 Header with        */
/* two byte CTL                                                             */
#define LLC$G_TWOBYT_DATA 4             /* Start of User Data               */
/* with 1 byte control field                                                */
struct HDR_802 {
    unsigned char LLC$B_DSAP;           /* Destination Service Access Point */
    unsigned char LLC$B_SSAP;           /* Source Service Access Point      */
    unsigned char LLC$B_CTL1;           /* Control field                    */
    unsigned char LLC$B_CTL2;           /* optional second byte of CTL      */
    } ;
/*                                                                          */
/*	IEEE 802.1 SNAP SAP packet header (802E/802PID)                     */
/*                                                                          */
#define SNAP$C_LEN 8                    /* Size of 802 SNAP Header          */
struct SNAP_HDR {
    unsigned char SNAP$G_SAP [3];       /* SNAP-SAP <AA-00-03> hex          */
    unsigned char SNAP$G_PID [5];       /* Protocol Identifier (PID)        */
    } ;
