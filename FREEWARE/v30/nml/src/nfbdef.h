#pragma member_alignment save
#pragma nomember_alignment
/*                                                                          */
/*	The following generic field identifiers are defined for all databases. */
/*                                                                          */
#define NFB$C_ENDOFLIST 0               /* Used to terminate the field i.d.  */
#define NFB$C_WILDCARD 1                /* Field i.d. used for "match all" database searches  */
#define NFB$C_CTX_SIZE 64               /* Length of context area in P2 buffer  */
/*                                                                          */
/*	The following codes are passed in the second IOSB longword to qualify */
/*	as SS$_ILLCNTRFUNC error.                                           */
/*                                                                          */
/* The high order word of these error codes must be 0                       */
/* so that they won't be confused with field i.d.s                          */
#define NFB$_ERR_FCT 1                  /* Unrecognized NFB$B_FCT value.    */
#define NFB$_ERR_DB 2                   /* Unrecognized NFB$B_DATABASE value.  */
#define NFB$_ERR_P1 3                   /* The P1 buffer is invalid.        */
#define NFB$_ERR_P2 4                   /* The P2 buffer is invalid.        */
#define NFB$_ERR_P3 5                   /* The P3 buffer is invalid.        */
#define NFB$_ERR_P4 6                   /* The P4 buffer is invalid.        */
#define NFB$_ERR_P5 7                   /* The P5 buffer should not have been specified.  */
#define NFB$_ERR_P6 8                   /* The P6 buffer should not have been specified.  */
#define NFB$_ERR_CELL 9                 /* Unrecognized NFB$B_CELL value.   */
#define NFB$_ERR_OPER 10                /* Unrecognized NFB$B_OPER value.   */
#define NFB$_ERR_SRCH 11                /* Unrecognized NFB$L_SRCH_KEY field ID  */
#define NFB$_ERR_SRCH2 12               /* Unrecognized NFB$L_SRCH2_KEY field ID  */
#define NFB$_ERR_OPER2 13               /* Unrecognized NFB$B_OPER2 value.  */
#define NFB$_ERR_FLAGS 14               /* Undefined bits in NFB$B_FLAGS were not zero.  */
#define NFB$_ERR_LOCK 15                /* Lock was not granted             */
/*                                                                          */
/*	Define the P1 buffer format                                         */
/*                                                                          */
#define NFB$C_DECLNAME 21               /* Declare name                     */
#define NFB$C_DECLOBJ 22                /* Declare object                   */
#define NFB$C_DECLSERV 23               /* Declare server process available  */
/* Resume defining function codes                                           */
#define NFB$C_LOGEVENT 28               /* Log a network event              */
#define NFB$C_READEVENT 29              /* Read current raw event queue (used by EVL only)  */
/* Resume defining function codes                                           */
#define NFB$C_FC_DELETE 33              /* Remove an entry from the data base.  */
#define NFB$C_FC_SHOW 34                /* Return specified field values.   */
#define NFB$C_FC_SET 35                 /* Set/modify the field values.     */
#define NFB$C_FC_CLEAR 36               /* Clear specified field values.    */
#define NFB$C_FC_ZERCOU 37              /* Zero (and optionally read) counters  */
#define NFB$C_FC_LOOP 38                /* Loop (used only to PSI to loop an X.25 line)  */
#define NFB$C_REBUILD_PROXY 39          /* Rebuild the proxy data base      */
#define NFB$C_ADD_PROXY 40              /* Add/Modify proxy DB entry        */
#define NFB$C_DELETE_PROXY 41           /* Remove/Delete proxy access       */
/* Maximum FCT value                                                        */
#define NFB$C_FC_MAX 41                 /* Maximum FCT value                */
#define NFB$M_ERRUPD 0x1
#define NFB$M_MULT 0x2
#define NFB$M_NOCTX 0x4
#define NFB$M_LOCAL 0x8
#define NFB$C_DB_LNI 1                  /* Local node                       */
#define NFB$C_DB_NDI 2                  /* Common nodes                     */
#define NFB$C_DB_OBI 3                  /* Network objects                  */
#define NFB$C_DB_CRI 4                  /* Circuits                         */
#define NFB$C_DB_PLI 5                  /* Lines                            */
#define NFB$C_DB_EFI 6                  /* Event logging filters            */
#define NFB$C_DB_ESI 7                  /* Event logging sinks              */
#define NFB$C_DB_LLI 8                  /* Logical-links                    */
#define NFB$C_DB_XNI 9                  /* X.25 networks                    */
#define NFB$C_DB_XGI 10                 /* X.25 groups                      */
#define NFB$C_DB_XDI 11                 /* X.25 DTEs                        */
#define NFB$C_DB_XS5 12                 /* X.25 server                      */
#define NFB$C_DB_XD5 13                 /* X.25 destinations                */
#define NFB$C_DB_XS9 14                 /* X.29 server                      */
#define NFB$C_DB_XD9 15                 /* X.29 destinations                */
#define NFB$C_DB_XTI 16                 /* X.25 trace facility              */
#define NFB$C_DB_XTT 17                 /* X.25 tracepoints                 */
#define NFB$C_DB_SPI 18                 /* Server Process                   */
#define NFB$C_DB_AJI 19                 /* Adjacency information            */
#define NFB$C_DB_ARI 20                 /* Area information                 */
/* (The following codes are reserved for future PSIACP                      */
/*  databases.  These codes should only be used in the                      */
/*  event PSIACP needs a database code before a new                         */
/*  new NETACP can be supplied to support it).                              */
#define NFB$C_DB_XDTE 21                /* PSI reserved database            */
#define NFB$C_DB_PSI2 22                /* PSI reserved database            */
#define NFB$C_DB_PSI3 23                /* PSI reserved database            */
#define NFB$C_DB_PSI4 24                /* PSI reserved database            */
#define NFB$C_DB_PSI5 25                /* PSI reserved database            */
#define NFB$C_DB_SDI 26                 /* Service (DLE) information        */
#define NFB$C_DB_XAI 27                 /* X.25 access database             */
#define NFB$C_DB_PROXY 28               /* Proxy data base                  */
#define NFB$C_DB_XXX 29                 /* Last database definition for NFB$C_DB_MAX calc.  */
/* Maximum DATABASE value                                                   */
#define NFB$C_DB_MAX 28                 /* Maximum DATABASE value           */
#define NFB$C_OP_EQL 0                  /* Match if SEARCH_KEY value EQL database entry field  */
#define NFB$C_OP_GTRU 1                 /* Match if SEARCH_KEY value GTRU database entry field  */
#define NFB$C_OP_LSSU 2                 /* Match if SEARCH_KEY value LSSU database entry field  */
#define NFB$C_OP_NEQ 3                  /* Match if SEARCH_KEY value NEQ database entry field  */
/* The following may only be used internally by NETACP                      */
#define NFB$C_OP_FNDMIN 4               /* Find entry with minimum key value  */
#define NFB$C_OP_FNDMAX 5               /* Find entry with maximum key value  */
#define NFB$C_OP_FNDPOS 6               /* Find entry position in database  */
/* Maximum operator function                                                */
#define NFB$C_OP_MAXFCT 3               /* Maximum operator function        */
#define NFB$C_OP_MAXINT 6               /* Maximum internal function        */
#define NFB$K_LENGTH 16                 /* Minimum structure size.          */
#define NFB$C_LENGTH 16                 /* Minimum structure size.          */
/* counted strings.  If the "cell size" is non-zero, it                     */
struct NFBDEF {
    unsigned char NFB$B_FCT;            /* A function code as follows:      */
/* Function codes for the NFB                                               */
/*  (leaving room for 20 obsolete function codes)                           */
/*  (leave room for 4 obsolete function codes)                              */
/*  (leave room for 3 obsolete function codes)                              */
    union  {
        unsigned char NFB$B_FLAGS;      /* Miscellaneous control flags      */
        struct  {
            unsigned NFB$V_ERRUPD : 1;  /* Update position context, even on error  */
            unsigned NFB$V_MULT : 1;    /* Process as many entries as can be fit into P4  */
            unsigned NFB$V_NOCTX : 1;   /* Don't update position context, even if successful  */
/*  (used to stay on an entry for a while).  This                           */
/*  flag Overrides the ERRUPD flag.                                         */
            unsigned NFB$V_LOCAL : 1;   /* Signal that REBUILD_PROXY should only be */
/* performed locally                                                        */
            unsigned NFB$V_FILL_0 : 4;
            } NFB$R_FLAGS_BITS;
        } NFB$R_FLAGS_OVERLAY;
    unsigned char NFB$B_DATABASE;       /* A code identifying the database as follows:  */
/* ZERO is an illegal value for this field                                  */
    unsigned char NFB$B_OPER;           /* Specifies the sense of the search (e.g. EQL, GEQU)  */
/* when comparing against the SRCH_KEY field.                               */
    unsigned int NFB$L_SRCH_KEY;        /* Search key field identifier specifying the key used  */
/* to locate the entry in the database.  This search is                     */
/* controlled by the sense of the NFB$B_OPER field.                         */
/*                                                                          */
/* If this field has the value "NFB$C_WILDCARD", then                       */
/* the very next entry in the list is assumed to be the                     */
/* target of the search.                                                    */
/*                                                                          */
/* If this field is not specified (zero), then it                           */
/* is assumed to be NFB$C_WILDCARD (no search key).                         */
/*                                                                          */
    unsigned int NFB$L_SRCH2_KEY;       /* Secondary search key field ID specifying the key used  */
/* to locate the entry in the database.  This search is                     */
/* controlled by the sense of the NFB$B_OPER2 field.                        */
/*                                                                          */
/* If both SRCH_KEY and SRCH2_KEY are specified, then                       */
/* only those database entries matching both search keys                    */
/* will be processed.                                                       */
/*                                                                          */
/* If this field is not specified (zero), then it                           */
/* is assumed to be NFB$C_WILDCARD (no search key).                         */
/*                                                                          */
    unsigned char NFB$B_OPER2;          /* Specifies the sense of the search (e.g. EQL, GEQU)  */
/* when comparing against the SRCH2_KEY field.                              */
    unsigned char NFB$B_MBZ1;           /* Reserved. MBZ.                   */
    unsigned short int NFB$W_CELL_SIZE; /* Some of the field values found in the P4 buffer are  */
/* indicates the number of bytes which each string in                       */
/* the P4 buffer occupies.  If it is zero then strings                      */
/* fields are stored as variable lengthed strings.                          */
    unsigned int NFB$L_FLDID;           /* Cell containing the first field ID -- the list  */
/* of field IDs begins here and continues to the                            */
/* end of the structure.                                                    */
/*                                                                          */
/* The list may be terminated before the end of the                         */
/* structure by placing the value NFB$C_ENDOFLIST                           */
/* in the longword following the last field ID.                             */
/*                                                                          */
/*                                                                          */
/*	Define the "field i.d." format.                                     */
/*                                                                          */
    } ;
#define NFB$M_INX 0xFFFF
#define NFB$M_TYP 0x30000
#define NFB$M_SPARE 0xFC0000
#define NFB$M_DB 0xFF000000
#define NFB$C_TYP_BIT 0                 /* Field type for bits              */
#define NFB$C_TYP_V 0                   /* Field type for bits              */
#define NFB$C_TYP_LNG 1                 /* Field type for longwords         */
#define NFB$C_TYP_L 1                   /* Field type for longwords         */
#define NFB$C_TYP_STR 2                 /* Field type for strings           */
#define NFB$C_TYP_S 2                   /* Field type for strings           */
/*                                                                          */
union NFBDEF1 {
    unsigned int NFB$L_PARAM_ID;        /* Define parameter ID longword     */
    struct  {
        unsigned NFB$V_INX : 16;        /* Index into semantic table        */
        unsigned NFB$V_TYP : 2;         /* Field type (string, bit, etc.)   */
        unsigned NFB$V_SPARE : 6;       /* Reserved, MBZ                    */
        unsigned NFB$V_DB : 8;          /* Data-base i.d.                   */
        } NFB$R_PARAM_ID_BITS;
/*   Define useful symbols for storing and retreiving binary and string     */
/*   values from the P2 and P4 buffers                                      */
/*                                                                          */
    } ;
union NFBDEF2 {
    unsigned int NFB$L_LNG_VALUE;       /* Longword value                   */
    } ;
union NFBDEF3 {
    unsigned int NFB$L_BIT_VALUE;       /* Boolean value                    */
    } ;
#define NFB$C_NDI_LCK 33554433          /* Set if conditionally writable fields are not writable  */
#define NFB$C_NDI_LOO 33554434          /* Set if CNF is for a "loopback" node  */
#define NFB$C_NDI_REA 33554435          /* Set if node is reachable         */
/*                                                                          */
#define NFB$C_NDI_TAD 33619984          /* "transformed address" - uses local node address  */
/* for the local NDI (instead of zero as does ADD)                          */
#define NFB$C_NDI_CTA 33619985          /* Absolute due time for logging counters  */
#define NFB$C_NDI_ADD 33619986          /* Address                          */
#define NFB$C_NDI_CTI 33619987          /* Counter timer                    */
#define NFB$C_NDI_ACL 33619988          /* Active links                     */
#define NFB$C_NDI_DEL 33619989          /* Delay                            */
#define NFB$C_NDI_DTY 33619990          /* Destination Type                 */
#define NFB$C_NDI_DCO 33619991          /* Destination Cost                 */
#define NFB$C_NDI_DHO 33619992          /* Destination Hops                 */
#define NFB$C_NDI_SDV 33619993          /* Service Device                   */
#define NFB$C_NDI_CPU 33619994          /* CPU type                         */
#define NFB$C_NDI_STY 33619995          /* Software type                    */
#define NFB$C_NDI_DAD 33619996          /* Dump address                     */
#define NFB$C_NDI_DCT 33619997          /* Dump count                       */
#define NFB$C_NDI_OHO 33619998          /* Host                             */
#define NFB$C_NDI_IHO 33619999          /* Host                             */
#define NFB$C_NDI_ACC 33620000          /* Access switch (inbound, outbound, etc)  */
#define NFB$C_NDI_PRX 33620001          /* ** obsolete ** (Node proxy parameter)  */
#define NFB$C_NDI_NND 33620002          /* Next node address                */
#define NFB$C_NDI_SNV 33620003          /* Service Node Version             */
#define NFB$C_NDI_INB 33620004          /* Async Line - Inbound node type   */
/*                                                                          */
#define NFB$C_NDI_COL 33685568          /* Collating field                  */
#define NFB$C_NDI_HAC 33685569          /* Node address/loop linename combination  */
#define NFB$C_NDI_CNT 33685570          /* Counters                         */
#define NFB$C_NDI_NNA 33685571          /* Name                             */
#define NFB$C_NDI_SLI 33685572          /* Service line                     */
#define NFB$C_NDI_SPA 33685573          /* Service password                 */
#define NFB$C_NDI_LOA 33685574          /* Load file                        */
#define NFB$C_NDI_SLO 33685575          /* Secondary loader                 */
#define NFB$C_NDI_TLO 33685576          /* Tertiary loader                  */
#define NFB$C_NDI_SID 33685577          /* Software ID                      */
#define NFB$C_NDI_DUM 33685578          /* Dump file                        */
#define NFB$C_NDI_SDU 33685579          /* Secondary dumper                 */
#define NFB$C_NDI_NLI 33685580          /* Loopback Line                    */
#define NFB$C_NDI_DLI 33685581          /* Destination Line                 */
#define NFB$C_NDI_PUS 33685582          /* Privileged user id               */
#define NFB$C_NDI_PAC 33685583          /* Privileged account               */
#define NFB$C_NDI_PPW 33685584          /* Privileged password              */
#define NFB$C_NDI_NUS 33685585          /* Non-privileged user id           */
#define NFB$C_NDI_NAC 33685586          /* Non-privileged account           */
#define NFB$C_NDI_NPW 33685587          /* Non-privileged password          */
#define NFB$C_NDI_RPA 33685588          /* Receive password                 */
#define NFB$C_NDI_TPA 33685589          /* Transmit password                */
#define NFB$C_NDI_DFL 33685590          /* Diagnostic load file             */
#define NFB$C_NDI_HWA 33685591          /* Hardware NI address (ROM address)  */
#define NFB$C_NDI_LPA 33685592          /* Loop assistant NI address        */
#define NFB$C_NDI_NNN 33685593          /* Next node name to destination (goes with NND)  */
#define NFB$C_NDI_LAA 33685594          /* Load Assist Agent                */
#define NFB$C_NDI_LAP 33685595          /* Load Assist Parameter            */
#define NFB$C_NDI_MFL 33685596          /* Management File                  */
/*                                                                          */
#define NFB$C_LNI_LCK 16777217          /* Set if conditionally writable fields are not writable  */
#define NFB$C_LNI_ALI 16777218          /* Set if ALIAS INBOUND has been enabled */
#define NFB$C_LNI_IPR 16777219          /* Incoming proxy enabled/disabled  */
#define NFB$C_LNI_OPR 16777220          /* Outgoing proxy enabled/disabled  */
#define NFB$C_LNI_DNS 16777221          /* DNS interface enabled/disabled   */
/*                                                                          */
#define NFB$C_LNI_ADD 16842768          /* Address                          */
#define NFB$C_LNI_ACL 16842769          /* Total number of active links     */
#define NFB$C_LNI_ITI 16842770          /* Incoming timer                   */
#define NFB$C_LNI_OTI 16842771          /* Outgoing timer                   */
#define NFB$C_LNI_STA 16842772          /* State                            */
#define NFB$C_LNI_MLK 16842773          /* Maximum links                    */
#define NFB$C_LNI_DFA 16842774          /* Delay factor                     */
#define NFB$C_LNI_DWE 16842775          /* Delay weight                     */
#define NFB$C_LNI_IAT 16842776          /* Inactivity timer                 */
#define NFB$C_LNI_RFA 16842777          /* Retransmit factor                */
#define NFB$C_LNI_ETY 16842778          /* Executor Type                    */
#define NFB$C_LNI_RTI 16842779          /* Routing timer                    */
#define NFB$C_LNI_RSI 16842780          /* Routing suppression timer        */
#define NFB$C_LNI_SAD 16842781          /* Subaddress                       */
/* (lower word = lower limit, upper word = upper limit)                     */
#define NFB$C_LNI_MAD 16842782          /* Maximum address                  */
#define NFB$C_LNI_MLN 16842783          /* Maximum lines                    */
#define NFB$C_LNI_MCO 16842784          /* Maximum cost                     */
#define NFB$C_LNI_MHO 16842785          /* Maximum hops                     */
#define NFB$C_LNI_MVI 16842786          /* Maximum visits                   */
#define NFB$C_LNI_MBU 16842787          /* Maximum buffers                  */
#define NFB$C_LNI_BUS 16842788          /* Forwarding buffer size           */
#define NFB$C_LNI_LPC 16842789          /* Loop count                       */
#define NFB$C_LNI_LPL 16842790          /* Loop length                      */
#define NFB$C_LNI_LPD 16842791          /* Loop Data type                   */
#define NFB$C_LNI_DAC 16842792          /* Default access switch (inbound, outbound, etc)  */
#define NFB$C_LNI_fill1 16842793        /* Place holder, used to be Default proxy access (inbound, outbound, etc)  */
#define NFB$C_LNI_PIQ 16842794          /* Pipeline quota                   */
#define NFB$C_LNI_LPH 16842795          /* Loop help type of assistance given to loop requestors  */
#define NFB$C_LNI_BRT 16842796          /* Broadcast routing timer          */
#define NFB$C_LNI_MAR 16842797          /* Maximum areas                    */
#define NFB$C_LNI_MBE 16842798          /* Maximum nonrouters on NI         */
#define NFB$C_LNI_MBR 16842799          /* Maximum routers on NI            */
#define NFB$C_LNI_AMC 16842800          /* Area maximum cost                */
#define NFB$C_LNI_AMH 16842801          /* Area maximum hops                */
#define NFB$C_LNI_SBS 16842802          /* Segment buffer size              */
#define NFB$C_LNI_ALA 16842803          /* Alias local node address (cluster address)  */
#define NFB$C_LNI_ALM 16842804          /* Alias maximum links              */
#define NFB$C_LNI_PSP 16842805          /* Path split policy normal/interim */
#define NFB$C_LNI_MPS 16842806          /* Maximum path split               */
#define NFB$C_LNI_MDO 16842807          /* Maximum Declared Object          */
/*                                                                          */
#define NFB$C_LNI_COL 16908352          /* Collating field                  */
#define NFB$C_LNI_NAM 16908353          /* Local node name                  */
#define NFB$C_LNI_CNT 16908354          /* Counters                         */
#define NFB$C_LNI_IDE 16908355          /* Identification                   */
#define NFB$C_LNI_MVE 16908356          /* Management version               */
#define NFB$C_LNI_NVE 16908357          /* Nsp version                      */
#define NFB$C_LNI_RVE 16908358          /* Routing version                  */
#define NFB$C_LNI_PHA 16908359          /* Physical NI address (current address) */
#define NFB$C_LNI_IDP 16908360          /* IDP of ISO address               */
#define NFB$C_LNI_DNM 16908361          /* DNS namespace                    */
/*                                                                          */
#define NFB$C_OBI_LCK 50331649          /* Set if conditionally writable fields are not writable  */
#define NFB$C_OBI_SET 50331650          /* Set if a "set" QIO has ever modified the CNF. If  */
/* not then the CNF was due to a "declare name/obect"                       */
/* only and may be deleted when the declaring process                       */
/* breaks the channel over which the object was declared                    */
#define NFB$C_OBI_ALO 50331651          /* Alias Outgoing enabled/disabled  */
#define NFB$C_OBI_ALI 50331652          /* Alias Incoming enabled/disabled  */
/*                                                                          */
#define NFB$C_OBI_LPR 50397200          /* Low order privileges             */
#define NFB$C_OBI_HPR 50397201          /* High order privileges            */
#define NFB$C_OBI_DOV 50397202          /* Point to Owners UCB              */
#define NFB$C_OBI_CHN 50397203          /* Owner's channel                  */
#define NFB$C_OBI_NUM 50397204          /* Number                           */
#define NFB$C_OBI_PID 50397205          /* Process id                       */
#define NFB$C_OBI_PRX 50397206          /* Proxy login switch (inbound, outbound, etc)  */
/*                                                                          */
#define NFB$C_OBI_COL 50462784          /* Collating field                  */
#define NFB$C_OBI_ZNA 50462785          /* Zero obj+name identifier         */
#define NFB$C_OBI_SFI 50462786          /* Parsed file i.d.                 */
#define NFB$C_OBI_IAC 50462787          /* Default inbound combined access control string  */
#define NFB$C_OBI_NAM 50462788          /* Name                             */
#define NFB$C_OBI_FID 50462789          /* File id                          */
#define NFB$C_OBI_USR 50462790          /* User id                          */
#define NFB$C_OBI_ACC 50462791          /* Account                          */
#define NFB$C_OBI_PSW 50462792          /* Password                         */
#define NFB$C_OBI_OCPRV 50462793        /* Outgoing Connect Privileges      */
/*                                                                          */
#define NFB$C_CRI_LCK 67108865          /* D Set if conditionally writable fields are  */
/*	not writable                                                        */
#define NFB$C_CRI_SER 67108866          /* D Set if Service functions not allowed  */
#define NFB$C_CRI_BLK_FILL 67108867     /*   Filler (BLK retired)           */
#define NFB$C_CRI_VER_FILL 67108868     /*   Filler (VER retired)           */
#define NFB$C_CRI_DLM 67108869          /* E Circuit to be used as X.25 datalink, if set  */
/*	If clear, circuit is for X.25 native use                            */
#define NFB$C_CRI_OWPID 67174416        /* D PID of temp owner of line in service state  */
#define NFB$C_CRI_CTA 67174417          /* D Absolute due time for counter logging  */
#define NFB$C_CRI_SRV 67174418          /* D Service substate qualifier     */
#define NFB$C_CRI_STA 67174419          /* C State                          */
#define NFB$C_CRI_SUB 67174420          /* C Substate                       */
#define NFB$C_CRI_LCT 67174421          /* C Counter timer                  */
#define NFB$C_CRI_PNA 67174422          /* E Adjacent node address          */
#define NFB$C_CRI_BLO 67174423          /* E Partner's receive block size   */
#define NFB$C_CRI_COS 67174424          /* E Cost                           */
#define NFB$C_CRI_HET 67174425          /* E Hello timer                    */
#define NFB$C_CRI_LIT 67174426          /* E Listen timer                   */
#define NFB$C_CRI_MRC 67174427          /* E Maximum recalls                */
#define NFB$C_CRI_RCT 67174428          /* E Recall timer                   */
#define NFB$C_CRI_POL 67174429          /* D Polling state                  */
#define NFB$C_CRI_PLS 67174430          /* D Polling substate               */
#define NFB$C_CRI_USE 67174431          /* X Usage                          */
#define NFB$C_CRI_TYP 67174432          /* C Type                           */
#define NFB$C_CRI_CHN 67174433          /* X X.25 Channel                   */
#define NFB$C_CRI_MBL 67174434          /* X Maximum block                  */
#define NFB$C_CRI_MWI 67174435          /* X Maximum window                 */
#define NFB$C_CRI_TRI 67174436          /* D Tributary                      */
#define NFB$C_CRI_BBT 67174437          /* D Babble timer                   */
#define NFB$C_CRI_TRT 67174438          /* D Transmit timer                 */
#define NFB$C_CRI_MRB 67174439          /* D Maximum receive buffers        */
#define NFB$C_CRI_MTR 67174440          /* D Maximum transmits              */
#define NFB$C_CRI_ACB 67174441          /* D Active base                    */
#define NFB$C_CRI_ACI 67174442          /* D Active increment               */
#define NFB$C_CRI_IAB 67174443          /* D Inactive base                  */
#define NFB$C_CRI_IAI 67174444          /* D Inactive increment             */
#define NFB$C_CRI_IAT 67174445          /* D Inactive threshold             */
#define NFB$C_CRI_DYB 67174446          /* D Dying base                     */
#define NFB$C_CRI_DYI 67174447          /* D Dying increment                */
#define NFB$C_CRI_DYT 67174448          /* D Dying threshold                */
#define NFB$C_CRI_DTH 67174449          /* D Dead threshold                 */
#define NFB$C_CRI_MST 67174450          /* D Maintenance mode state (0 => On, 1 => Off>  */
#define NFB$C_CRI_XPT 67174451          /* E Transport protocol to use      */
#define NFB$C_CRI_MRT 67174452          /* E Maximum routers on this NI     */
#define NFB$C_CRI_RPR 67174453          /* E Router priority                */
#define NFB$C_CRI_DRT 67174454          /* E Designated router on NI (node address)  */
#define NFB$C_CRI_VER 67174455          /* D Verification Enabled/Disabled/Inbound on circuit */
/*                                                                          */
#define NFB$C_CRI_COL 67240000          /* D Collating field                */
#define NFB$C_CRI_NAM 67240001          /* C Circuit name                   */
#define NFB$C_CRI_VMSNAM 67240002       /* D Device name in VMS format      */
#define NFB$C_CRI_CHR 67240003          /* D Characteristics buffer for startup control QIO  */
#define NFB$C_CRI_CNT 67240004          /* C Counters                       */
#define NFB$C_CRI_P2P 67240005          /* D Line's PhaseII partner name (for loopback)  */
#define NFB$C_CRI_LOO 67240006          /* E Loopback name                  */
#define NFB$C_CRI_PNN 67240007          /* E Adjacent node name             */
#define NFB$C_CRI_NUM 67240008          /* X Call Number                    */
#define NFB$C_CRI_DTE 67240009          /* X DTE                            */
#define NFB$C_CRI_DEVNAM 67240010       /* D Device name in VMS format, with unit included  */
#define NFB$C_CRI_net 67240011          /* XD Network name                  */
/*                                                                          */
#define NFB$C_PLI_LCK 83886081          /* D Set if conditionally writable fields are  */
/*	not writable                                                        */
#define NFB$C_PLI_SER 83886082          /* D Service                        */
#define NFB$C_PLI_DUP 83886083          /* C Duplex (set if half)           */
#define NFB$C_PLI_CON 83886084          /* C Controller (set if loopback)   */
#define NFB$C_PLI_CLO 83886085          /* C Clock mode (set if internal)   */
#define NFB$C_PLI_SWI 83886086          /* D Async Line - Switch            */
#define NFB$C_PLI_HNG 83886087          /* D Async Line - Hangup            */
#define NFB$C_PLI_ADR_TYP 83886088      /* T Address type                   */
#define NFB$C_PLI_RNG_SPD 83886089      /* T Ring speed                     */
#define NFB$C_PLI_ETR 83886090          /* T Early token release            */
#define NFB$C_PLI_SRC_ROU 83886091      /* T Source routing                 */
/*                                                                          */
#define NFB$C_PLI_CTA 83951632          /* D Absolute time for counter read and clear  */
#define NFB$C_PLI_STA 83951633          /* C State                          */
#define NFB$C_PLI_SUB 83951634          /* C Substate                       */
#define NFB$C_PLI_LCT 83951635          /* D Counter timer                  */
#define NFB$C_PLI_PRO 83951636          /* C Protocol                       */
#define NFB$C_PLI_STI 83951637          /* D Service timer                  */
#define NFB$C_PLI_HTI 83951638          /* L Holdback timer                 */
#define NFB$C_PLI_MBL 83951639          /* L Maximum block                  */
#define NFB$C_PLI_MRT 83951640          /* L Maximum retransmits            */
#define NFB$C_PLI_MWI 83951641          /* L Maximum window                 */
#define NFB$C_PLI_SLT 83951642          /* D Scheduling timer               */
#define NFB$C_PLI_DDT 83951643          /* D Dead timer                     */
#define NFB$C_PLI_DLT 83951644          /* D Delay timer                    */
#define NFB$C_PLI_SRT 83951645          /* D Stream timer                   */
#define NFB$C_PLI_BFN 83951646          /* D Receive buffers                */
#define NFB$C_PLI_BUS 83951647          /* D Action routine returns bufsiz used for line  */
#define NFB$C_PLI_PLVEC 83951648        /* D PLVEC i.d.                     */
#define NFB$C_PLI_RTT 83951649          /* D Retransmit timer               */
#define NFB$C_PLI_MOD 83951650          /* L X.25 mode (DCE, DTE, etc).     */
#define NFB$C_PLI_LPC 83951651          /* L Loop count                     */
#define NFB$C_PLI_LPL 83951652          /* L Loop length                    */
#define NFB$C_PLI_LPD 83951653          /* L Loop Data type                 */
#define NFB$C_PLI_EPT 83951654          /* E Ethernet protocol type for datalink  */
#define NFB$C_PLI_LNS 83951655          /* D Async Line - Line speed        */
#define NFB$C_PLI_BFS 83951656          /* C Line buffer size (overrides executor bufsiz)  */
#define NFB$C_PLI_TPI 83951657          /* D Transmit Pipeline              */
#define NFB$C_PLI_TREQ 83951658         /* F Requested TRT                  */
#define NFB$C_PLI_TVX 83951659          /* F Valid transmission time        */
#define NFB$C_PLI_REST_TTO 83951660     /* F Restricted token timeout       */
#define NFB$C_PLI_RPE 83951661          /* F Ring purger enable             */
#define NFB$C_PLI_ECHO_DAT 83951662     /* F Echo data                      */
#define NFB$C_PLI_ECHO_LEN 83951663     /* F Echo length                    */
#define NFB$C_PLI_T_NEG 83951664        /* F Negotiated TRT                 */
#define NFB$C_PLI_DAT 83951665          /* F Duplicate address flag         */
#define NFB$C_PLI_UN_DAT 83951666       /* F Upstream neighbor DA flag      */
#define NFB$C_PLI_RPS 83951667          /* F Ring purger state              */
#define NFB$C_PLI_RER 83951668          /* F Ring error reason              */
#define NFB$C_PLI_NBR_PHY 83951669      /* F Neighbor PHY type              */
#define NFB$C_PLI_LEE 83951670          /* F Link error estimate            */
#define NFB$C_PLI_RJR 83951671          /* F Reject reason                  */
#define NFB$C_PLI_RNG_NUM 83951672      /* T Ring number                    */
#define NFB$C_PLI_AUTH_PR 83951673      /* T Authorized access priority     */
#define NFB$C_PLI_A_TIM 83951674        /* T Aging timer                    */
/*                                                                          */
#define NFB$C_PLI_COL 84017216          /* D Collating field                */
#define NFB$C_PLI_NAM 84017217          /* C Line name                      */
#define NFB$C_PLI_VMSNAM 84017218       /* D Device name in VMS format      */
#define NFB$C_PLI_CHR 84017219          /* D Set-mode $QIO line Characteristics buffer  */
#define NFB$C_PLI_CNT 84017220          /* C Counters                       */
#define NFB$C_PLI_MCD 84017221          /* L Filespec for microcode dump (initiates dump)  */
#define NFB$C_PLI_HWA 84017222          /* D NI hardware address (ROM address)  */
#define NFB$C_PLI_DEVNAM 84017223       /* D Device name in VMS format, with unit included  */
#define NFB$C_PLI_NET 84017224          /* L Network name                   */
#define NFB$C_PLI_NIF_TARG 84017225     /* F NIF target                     */
#define NFB$C_PLI_SIF_CONF_TARG 84017226 /* F SIF configuration target      */
#define NFB$C_PLI_SIF_OP_TARG 84017227  /* F SIF operation target           */
#define NFB$C_PLI_ECHO_TARG 84017228    /* F Echo target                    */
#define NFB$C_PLI_MAC_CHR 84017229      /* F Set-mode $QIO MAC line characteristics buffer */
#define NFB$C_PLI_UNA 84017230          /* F Upstream neighbor              */
#define NFB$C_PLI_OLD_UNA 84017231      /* F Old upstream neighbor          */
#define NFB$C_PLI_DNA 84017232          /* F Downstream neighbor            */
#define NFB$C_PLI_OLD_DNA 84017233      /* F Old downstream neighbor        */
#define NFB$C_PLI_STN_ADR 84017234      /* T Station address                */
/*                                                                          */
#define NFB$C_EFI_LCK 100663297         /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_EFI_SIN 100728848
#define NFB$C_EFI_SP1 100728849
#define NFB$C_EFI_B1 100728850
#define NFB$C_EFI_B2 100728851
/*                                                                          */
#define NFB$C_EFI_COL 100794432         /* Collating field                  */
#define NFB$C_EFI_EVE 100794433
#define NFB$C_EFI_SB1 100794434
#define NFB$C_EFI_SB2 100794435
#define NFB$C_EFI_SB3 100794436
/*                                                                          */
#define NFB$C_ESI_LCK 117440513         /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_ESI_SNK 117506064
#define NFB$C_ESI_STA 117506065
#define NFB$C_ESI_SP1 117506066
#define NFB$C_ESI_B1 117506067
#define NFB$C_ESI_B2 117506068
/*                                                                          */
#define NFB$C_ESI_COL 117571648         /* Collating field                  */
#define NFB$C_ESI_LNA 117571649
#define NFB$C_ESI_SB1 117571650
#define NFB$C_ESI_SB2 117571651
#define NFB$C_ESI_SB3 117571652
/*                                                                          */
#define NFB$C_LLI_LCK 134217729         /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_LLI_DLY 134283280         /* Round trip delay time            */
#define NFB$C_LLI_STA 134283281         /* State                            */
#define NFB$C_LLI_LLN 134283282         /* Local link number                */
#define NFB$C_LLI_RLN 134283283         /* Remote link number               */
#define NFB$C_LLI_PNA 134283284         /* Partner's node address           */
#define NFB$C_LLI_PID 134283285         /* External Process I.D.            */
#define NFB$C_LLI_IPID 134283286        /* Internal Process I.D.            */
#define NFB$C_LLI_XWB 134283287         /* Pointer to XWB                   */
#define NFB$C_LLI_CNT 134283288         /* Counters                         */
/*                                                                          */
#define NFB$C_LLI_COL 134348864         /* Collating field                  */
#define NFB$C_LLI_USR 134348865         /* User name                        */
#define NFB$C_LLI_PRC 134348866         /* Process name                     */
#define NFB$C_LLI_PNN 134348867         /* Partner's node name              */
#define NFB$C_LLI_RID 134348868         /* Partner's process i.d.           */
/*                                                                          */
#define NFB$C_XNI_LCK 150994945         /* Set if conditionally writable fields are not writable  */
#define NFB$C_XNI_MNS_FILL 150994946    /* X.25 multi-network support (set if enabled) [No longer used] */
/*                                                                          */
#define NFB$C_XNI_CAT 151060496         /* Call timer                       */
#define NFB$C_XNI_CLT 151060497         /* Clear timer                      */
#define NFB$C_XNI_DBL 151060498         /* Default data                     */
#define NFB$C_XNI_DWI 151060499         /* Default window                   */
#define NFB$C_XNI_MBL 151060500         /* Maximum data                     */
#define NFB$C_XNI_MCL 151060501         /* Maximum clears                   */
#define NFB$C_XNI_MRS 151060502         /* Maximum resets                   */
#define NFB$C_XNI_MST 151060503         /* Maximum restarts                 */
#define NFB$C_XNI_MWI 151060504         /* Maximum window                   */
#define NFB$C_XNI_RST 151060505         /* Reset timer                      */
#define NFB$C_XNI_STT 151060506         /* Restart timer                    */
/*                                                                          */
#define NFB$C_XNI_COL 151126080         /* Collating field                  */
#define NFB$C_XNI_netent 151126081      /* Network                          */
#define NFB$C_XNI_PROF 151126082        /* Profile name                     */
/*                                                                          */
#define NFB$C_XDI_LCK 184549377         /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_XDI_ACH 184614928         /* Active channels                  */
#define NFB$C_XDI_ASW 184614929         /* Active switched                  */
#define NFB$C_XDI_CTM 184614930         /* Counter timer                    */
#define NFB$C_XDI_MCH 184614931         /* Maximum channels                 */
#define NFB$C_XDI_STA 184614932         /* State                            */
#define NFB$C_XDI_SUB 184614933         /* Substate                         */
#define NFB$C_XDI_MCI 184614934         /* Maximum circuits [VMS only]      */
#define NFB$C_XDI_CAT 184614935         /* Call timer                       */
#define NFB$C_XDI_CLT 184614936         /* Clear timer                      */
#define NFB$C_XDI_DBL 184614937         /* Default data                     */
#define NFB$C_XDI_DWI 184614938         /* Default window                   */
#define NFB$C_XDI_MBL 184614939         /* Maximum data                     */
#define NFB$C_XDI_MCL 184614940         /* Maximum clears                   */
#define NFB$C_XDI_MRS 184614941         /* Maximum resets                   */
#define NFB$C_XDI_MST 184614942         /* Maximum restarts                 */
#define NFB$C_XDI_MWI 184614943         /* Maximum window                   */
#define NFB$C_XDI_RST 184614944         /* Reset timer                      */
#define NFB$C_XDI_STT 184614945         /* Restart timer                    */
#define NFB$C_XDI_mode 184614946        /* DTE Mode                         */
#define NFB$C_XDI_itt 184614947         /* Interrupt timer                  */
/*                                                                          */
#define NFB$C_XDI_COL 184680512         /* Collating field                  */
#define NFB$C_XDI_DTE 184680513         /* DTE address                      */
#define NFB$C_XDI_CHN 184680514         /* Channels                         */
#define NFB$C_XDI_LIN 184680515         /* Line                             */
#define NFB$C_XDI_dnt 184680516         /* Network                          */
#define NFB$C_XDI_CNT 184680517         /* Counters                         */
/*                                                                          */
#define NFB$C_XGI_LCK 167772161         /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_XGI_GNM 167837712         /* Group number                     */
#define NFB$C_XGI_GTY 167837713         /* Group type                       */
/*                                                                          */
#define NFB$C_XGI_COL 167903296         /* Collating field. This field must be unique across  */
/* all entries in this database.  It consists of the                        */
/* group-name string followed by the DTE address.                           */
#define NFB$C_XGI_GRP 167903297         /* Group name                       */
#define NFB$C_XGI_GDT 167903298         /* Group DTE address                */
#define NFB$C_XGI_gnt 167903299         /* Group Network                    */
/*                                                                          */
#define NFB$C_XS5_LCK 201326593         /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_XS5_MCI 201392144         /* Maximum circuits allowed         */
#define NFB$C_XS5_STA 201392145         /* State                            */
#define NFB$C_XS5_ACI 201392146         /* Active circuits                  */
#define NFB$C_XS5_CTM 201392147         /* Counter timer                    */
/*                                                                          */
#define NFB$C_XS5_COL 201457728         /* Collating field. This field must be unique across  */
/* all entries in this database.                                            */
#define NFB$C_XS5_CNT 201457729         /* Counters                         */
/*                                                                          */
#define NFB$C_XD5_LCK 218103809         /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_XD5_PRI 218169360         /* Priority                         */
#define NFB$C_XD5_SAD 218169361         /* Subaddress range                 */
/* (lower word = lower limit, upper word = upper limit)                     */
#define NFB$C_XD5_NOD 218169362         /* Remote node address containing server (gateways only)  */
#define NFB$C_XD5_red 218169363         /* Redirect reason                  */
/*                                                                          */
#define NFB$C_XD5_COL 218234944         /* Collating field. This field must be unique across  */
/* all entries in this database.                                            */
#define NFB$C_XD5_DST 218234945         /* Destination DTE address          */
#define NFB$C_XD5_CMK 218234946         /* Call mask                        */
#define NFB$C_XD5_CVL 218234947         /* Call value                       */
#define NFB$C_XD5_GRP 218234948         /* Group name                       */
#define NFB$C_XD5_SDTE 218234949        /* Sending DTE address (formally number) */
#define NFB$C_XD5_OBJ 218234950         /* && Object name                   */
#define NFB$C_XD5_FIL 218234951         /* Command procedure to execute when starting object  */
#define NFB$C_XD5_USR 218234952         /* User name                        */
#define NFB$C_XD5_PSW 218234953         /* Password                         */
#define NFB$C_XD5_ACC 218234954         /* Account                          */
#define NFB$C_XD5_cdte 218234955        /* Called DTE                       */
#define NFB$C_XD5_rdte 218234956        /* Receiving DTE                    */
#define NFB$C_XD5_net 218234957         /* Network                          */
#define NFB$C_XD5_emk 218234958         /* Extension mask                   */
#define NFB$C_XD5_evl 218234959         /* Extension value                  */
#define NFB$C_XD5_acl 218234960         /* ACL, a list of ACE'structure, parto of ORB */
#define NFB$C_XD5_idte 218234961        /* Incoming address                 */
/*                                                                          */
#define NFB$C_XS9_LCK 234881025         /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_XS9_MCI 234946576         /* Maximum circuits allowed         */
#define NFB$C_XS9_STA 234946577         /* State                            */
#define NFB$C_XS9_ACI 234946578         /* Active circuits                  */
#define NFB$C_XS9_CTM 234946579         /* Counter timer                    */
/*                                                                          */
#define NFB$C_XS9_COL 235012160         /* Collating field. This field must be unique across  */
/* all entries in this database.                                            */
#define NFB$C_XS9_CNT 235012161         /* Counters                         */
/*                                                                          */
#define NFB$C_XD9_LCK 251658241         /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_XD9_PRI 251723792         /* Priority                         */
#define NFB$C_XD9_SAD 251723793         /* Subaddress range                 */
/* (lower word = lower limit, upper word = upper limit)                     */
#define NFB$C_XD9_NOD 251723794         /* Remote node address containing server (gateways only)  */
#define NFB$C_XD9_red 251723795         /* Redirect reason                  */
/*                                                                          */
#define NFB$C_XD9_COL 251789376         /* Collating field. This field must be unique across  */
/* all entries in this database.                                            */
#define NFB$C_XD9_DST 251789377         /* Destination DTE address          */
#define NFB$C_XD9_CMK 251789378         /* Call mask                        */
#define NFB$C_XD9_CVL 251789379         /* Call value                       */
#define NFB$C_XD9_GRP 251789380         /* Group name                       */
#define NFB$C_XD9_sdte 251789381        /* Sending DTE                      */
#define NFB$C_XD9_OBJ 251789382         /* && Object name                   */
#define NFB$C_XD9_FIL 251789383         /* Command procedure to execute when starting object  */
#define NFB$C_XD9_USR 251789384         /* User name                        */
#define NFB$C_XD9_PSW 251789385         /* Password                         */
#define NFB$C_XD9_ACC 251789386         /* Account                          */
#define NFB$C_XD9_cdte 251789387        /* Caller DTE                       */
#define NFB$C_XD9_rdte 251789388        /* Receiving DTE                    */
#define NFB$C_XD9_net 251789389         /* Network                          */
#define NFB$C_XD9_emk 251789390         /* Extension mask                   */
#define NFB$C_XD9_evl 251789391         /* Extension value                  */
#define NFB$C_XD9_acl 251789392         /* ACL, a list of ACE'structure, parto of ORB */
#define NFB$C_XD9_idte 251789393        /* Incoming address                 */
/*                                                                          */
#define NFB$C_XTI_LCK 268435457         /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_XTI_STA 268501008         /* State                            */
#define NFB$C_XTI_BFZ 268501009         /* Buffer size                      */
#define NFB$C_XTI_CPL 268501010         /* Capture limit                    */
#define NFB$C_XTI_MBK 268501011         /* Maximum blocks/file              */
#define NFB$C_XTI_MBF 268501012         /* Maximum number of buffers        */
#define NFB$C_XTI_MVR 268501013         /* Maximum trace file version number  */
/*                                                                          */
#define NFB$C_XTI_COL 268566592         /* Collating field. This field must be unique across  */
/* all entries in this database.                                            */
#define NFB$C_XTI_FNM 268566593         /* Trace file name                  */
/*                                                                          */
#define NFB$C_XTT_LCK 285212673         /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_XTT_TST 285278224         /* State                            */
#define NFB$C_XTT_CPS 285278225         /* Capture size                     */
/*                                                                          */
#define NFB$C_XTT_COL 285343808         /* Collating field. This field must be unique across  */
/* all entries in this database.                                            */
#define NFB$C_XTT_TPT 285343809         /* Tracepoint name                  */
/*                                                                          */
#define NFB$C_XAI_LCK 452984833         /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_XAI_NDA 453050384         /* Node address                     */
/*                                                                          */
#define NFB$C_XAI_COL 453115968         /* Collating field                  */
#define NFB$C_XAI_NET 453115969         /* Network                          */
#define NFB$C_XAI_USR 453115970         /* User id                          */
#define NFB$C_XAI_PSW 453115971         /* Password                         */
#define NFB$C_XAI_ACC 453115972         /* Account                          */
#define NFB$C_XAI_NOD 453115973         /* Node id                          */
/*                                                                          */
#define NFB$C_XDTE_LCK 352321537        /* Set if conditionally writable fields are not writable */
/*    C(,$C_XDTE_,(((NFB$C_DB_XDTE@24)+(NFB$C_TYP_LNG@16)+16)),1            */
#define NFB$C_XDTE_COL 352452672        /* Collating field                  */
#define NFB$C_XDTE_NET 352452673        /* Network                          */
#define NFB$C_XDTE_DTE 352452674        /* DTE address                      */
#define NFB$C_XDTE_ID 352452675         /* ID list, ARB rights list         */
#define NFB$C_XDTE_ACL 352452676        /* ACL, a list of ACE's, part of ORB */
/*                                                                          */
#define NFB$C_SPI_LCK 301989889         /* Set if conditionally writable fields are not writable  */
#define NFB$C_SPI_PRL 301989890         /* Proxy flag which initially started server process  */
/*                                                                          */
#define NFB$C_SPI_PID 302055440         /* Server PID                       */
#define NFB$C_SPI_IRP 302055441         /* IRP of waiting DECLSERV QIO (0 if process active)  */
#define NFB$C_SPI_CHN 302055442         /* Channel associated with DECLSERV IRP  */
#define NFB$C_SPI_RNA 302055443         /* Remote node address which initially started server  */
/*                                                                          */
#define NFB$C_SPI_COL 302121024         /* Collating field                  */
#define NFB$C_SPI_ACS 302121025         /* ACS used to initally start server process  */
#define NFB$C_SPI_RID 302121026         /* Remote user ID which initially started server  */
#define NFB$C_SPI_SFI 302121027         /* Last (current) SFI given to server process  */
#define NFB$C_SPI_NCB 302121028         /* Last (current) NCB given to server process  */
#define NFB$C_SPI_PNM 302121029         /* Last (current) process name given to server  */
/*                                                                          */
#define NFB$C_AJI_LCK 318767105         /* Set if conditionally writable fields are not writable  */
#define NFB$C_AJI_REA 318767106         /* Reachable (set if two-way communication established)  */
#define NFB$C_AJI_RRA 318767107         /* Reachable Routing Adjacency      */
/*                                                                          */
#define NFB$C_AJI_ADD 318832656         /* Node address                     */
#define NFB$C_AJI_TYP 318832657         /* Node type                        */
#define NFB$C_AJI_LIT 318832658         /* Listen timer for this adjacency  */
#define NFB$C_AJI_BLO 318832659         /* Partner's block size             */
#define NFB$C_AJI_RPR 318832660         /* Partner's router priority (on NI)  */
/*                                                                          */
#define NFB$C_AJI_COL 318898240         /* Collating field                  */
#define NFB$C_AJI_NNA 318898241         /* Node name                        */
#define NFB$C_AJI_CIR 318898242         /* Circuit name                     */
/*                                                                          */
#define NFB$C_SDI_LCK 436207617         /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_SDI_SUB 436273168         /* Service substate                 */
#define NFB$C_SDI_PID 436273169         /* PID of process owning this DLE link  */
/*                                                                          */
#define NFB$C_SDI_COL 436338752         /* Collating field                  */
#define NFB$C_SDI_CIR 436338753         /* Circuit name                     */
#define NFB$C_SDI_PHA 436338754         /* Service physical address (BC only)  */
#define NFB$C_SDI_PRC 436338755         /* Name of process owning this DLE link  */
/*                                                                          */
#define NFB$C_ARI_LCK 335544321         /* Set if conditionally writable fields are not writable  */
#define NFB$C_ARI_REA 335544322         /* Set if node is reachable         */
/*                                                                          */
#define NFB$C_ARI_ADD 335609872         /* Address                          */
#define NFB$C_ARI_DCO 335609873         /* Destination Cost                 */
#define NFB$C_ARI_DHO 335609874         /* Destination Hops                 */
#define NFB$C_ARI_NND 335609875         /* Next node address                */
/*                                                                          */
#define NFB$C_ARI_COL 335675456         /* Collating field                  */
#define NFB$C_ARI_DLI 335675457         /* Circuit used for normal traffic to area  */
/*                                                                          */
#define NFB$C_PROXY_LCK 469762049       /* Set if conditionally writable fields are not writable  */
/*                                                                          */
#define NFB$C_PROXY_RUIC 469827600      /* Remote UIC                       */
/*                                                                          */
#define NFB$C_PROXY_RNODE 469893184     /* Remote node                      */
#define NFB$C_PROXY_RNAME 469893185     /* Remote user name                 */
#define NFB$C_PROXY_DEFACCOUNT 469893186 /* Default local proxy account     */
#define NFB$C_PROXY_ACCOUNT1 469893187  /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT2 469893188  /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT3 469893189  /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT4 469893190  /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT5 469893191  /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT6 469893192  /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT7 469893193  /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT8 469893194  /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT9 469893195  /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT10 469893196 /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT11 469893197 /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT12 469893198 /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT13 469893199 /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT14 469893200 /* Local proxy account              */
#define NFB$C_PROXY_ACCOUNT15 469893201 /* Local proxy account              */
#define NFB$C_PROXY_HASHKEY 469893202   /* Hash key lookup string           */
#define NFB$C_PROXY_COL 469893184       /* Collating field                  */
#define NFB$C_PROXY_MAXACC 15           /* Maximum numver of local proxy accounts */
/*  including the default                                                   */

union NFBDEF4 {
    unsigned short int NFB$W_STR_COUNT; /* String count field               */
    struct  {
        char NFBDEF$$_FILL_1 [2];
#if defined(__VAXC)
        char NFB$B_STR_TEXT[];
#else
        char NFB$B_STR_TEXT[1];
#endif		/* #if defined(__VAXC)  */
        } NFB$R_STR_COUNT_FIELDS;
    } ;
