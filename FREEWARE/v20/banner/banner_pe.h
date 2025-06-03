/********************************************************************************************************************************/
/* Created  6-FEB-1992 15:42:05 by VAX SDL V3.2-12     Source:  4-FEB-1992 13:15:48 FIZBUZ$DUA0:[SAUNDERS.BANNER.C]BANNER_PE.SDL; */
/********************************************************************************************************************************/
 
/*** MODULE $pe_bus ***/
#define PE_BUS$C_BUS_COUNT 16
#define PE_BUS$C_DISPLAY_BUS 4
struct PE_BUS {
    char PE_BUS$T_NAME [8];
    unsigned long int PE_BUS$L_FLAGS;
    unsigned long int PE_BUS$L_XMT_BYTES;
    unsigned long int PE_BUS$L_XMT_ERRORS;
    unsigned long int PE_BUS$L_RCV_BYTES;
    unsigned long int PE_BUS$L_LAN_TYPE;
    unsigned long int PE_BUS$L_RCV_LAST;
    unsigned long int PE_BUS$L_XMT_ERRORS_LAST;
    unsigned long int PE_BUS$L_XMT_LAST;
    unsigned long int PE_BUS$L_HIGHLIGHT_BUS;
    unsigned long int PE_BUS$L_LAN_UTILIZATION;
    unsigned long int PE_BUS$L_XMT_PERCENT;
    unsigned long int PE_BUS$L_RCV_PERCENT;
    unsigned long int PE_BUS$L_HIGHLIGHT_ERRORS;
    } ;
 
/*** MODULE $pe_vc ***/
#define PE_VC$C_VC_COUNT 224
#define PE_VC$C_DISPLAY_VC 10
struct PE_VC {
    char PE_VC$T_NODENAME [8];
    unsigned long int PE_VC$L_FLAGS;
    unsigned long int PE_VC$L_XMT_BYTES;
    unsigned long int PE_VC$L_REXMT_MSGS;
    unsigned long int PE_VC$L_RCV_BYTES;
    unsigned long int PE_VC$L_RERCV_MSGS;
    unsigned long int PE_VC$L_PIPE_QUOTA;
    unsigned long int PE_VC$L_RCV_LAST;
    unsigned long int PE_VC$L_XMT_LAST;
    unsigned long int PE_VC$L_REXMT_LAST;
    unsigned long int PE_VC$L_RERCV_LAST;
    unsigned long int PE_VC$L_REXMT_RATE;
    unsigned long int PE_VC$L_RERCV_RATE;
    unsigned long int PE_VC$L_HIGHLIGHT_VC;
    unsigned long int PE_VC$L_LAN_UTILIZATION;
    unsigned long int PE_VC$L_XMT_PERCENT;
    unsigned long int PE_VC$L_RCV_PERCENT;
    unsigned long int PE_VC$L_HIGHLIGHT_REXMT;
    unsigned long int PE_VC$L_HIGHLIGHT_RERCV;
    unsigned long int PE_VC$L_CLOSED_COUNT;
    int *PE_VC$A_VC_DISPLAY_LINK;
    } ;
