/*
**++
**  FACILITY:	NSQUERY
**
**  ABSTRACT:	Preset data structures for use with NSQUERY.
**
**  MODULE DESCRIPTION:
**
**  	No routines, just data.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1992, TGV, INC.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  25-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	25-SEP-1992 V1.0    Madison 	Initial coding.
**--
*/

#include "ipns_const.h"

#pragma nostandard
    globalvalue unsigned int
    	NSQ__QUERYOK, NSQ__FMTERR, NSQ__SRVFAIL, NSQ__NAMERR,
    	NSQ__NOTIMPL, NSQ__REFUSED, NSQ__UNKRC;
#pragma standard


    char *cls_name[] 
    	     = {"0", "INTERNET", "2","CHAOS"};
    int cls_name_count = sizeof(cls_name)/sizeof(char *);
    char *pro_name[]
    	     = {"0", "ICMP", "IGMP", "GGP", "4", "ST", "TCP", "UCL",
    	    	"EGP", "IGP", "BBN-RCC-MON", "NVP-II", "PUP",
    	    	"ARGUS", "EMCON", "XNET", "CHAOS", "UDP", "MUX",
    	    	"DCN-MEAS", "HMP", "PRM", "XNS-IDP", "TRUNK-1",
    	    	"TRUNK-2", "LEAF-1", "LEAF-2", "RDP", "IRTP",
    	    	"ISO-TP4", "NETBLT", "MFE-NSP", "MERIT-INP", "SEP",
    	    	"34", "35", "36", "37", "38", "39", "40", "41",
    	    	"42", "43", "44", "45", "46", "47", "48", "49",
    	    	"50", "51", "52", "53", "54", "55", "56", "57",
    	    	"58", "59", "60", "internal", "CFTP", "local-net",
    	    	"SAT-EXPAK", "MIT-SUBNET", "RVD", "IPPC", "distr FS",
    	    	"SAT-MON", "70", "IPCV", "72", "73", "74", "75",
    	    	"BR-SAT-MON", "77", "WB-MON", "WB-EXPAK"};
    int pro_name_count = sizeof(pro_name)/sizeof(char *);
    char *wks_name[]
    	     = {"0", "1", "2", "3", "4", "RJE", "6", "ECHO", "8", "DISCARD",
    	    "10", "USERS", "12", "DAYTIME", "14", "15", "16", "QUOTE",
    	    "18", "CHARGEN", "FTP-DATA", "FTP", "22", "TELNET", "24",
    	    "SMTP", "26", "NSW-FE", "28", "MSG-ICP", "30", "MSG-AUTH",
    	    "32", "DSP", "34", "private prt server", "36", "TIME", "38",
    	    "RLP", "40", "GRAPHICS", "NAMESERVER", "NICNAME", "MPM-FLAGS",
    	    "MPM", "MPM-SND", "NI-FTP", "48", "LOGIN", "50", "LA-MAINT",
    	    "52", "DOMAIN", "54", "ISI-GL", "56", "private TELNET", "58",
    	    "private FTP", "60", "NI-MAIL", "62", "VIA-FTP", "64",
    	    "TACACS-DS", "66", "BOOTPS", "BOOTPC", "TFTP", "70",
    	    "NETRJS-1", "NETRJS-2", "NETRJS-3", "NETRJS-4",
    	    "private dial-out", "76", "private RJE", "78", "FINGER",
    	    "80", "HOSTS2-NS", "82", "MIT-ML-DEV", "84", "MIT-ML-DEV",
    	    "86", "term link", "88", "SU-MIT-TG", "90", "MIT-DOV", "92",
    	    "DCP", "94", "SUPDUP", "96", "SWIFT-RVF", "TACNEWS", "METAGRAM",
    	    "100", "HOSTNAME", "ISO-TSAP", "X400", "X400-SND", "CSNET-NS",
    	    "106", "RTELNET", "108", "POP-2", "110", "SUNRPC", "112",
    	    "AUTH", "114", "SFTP", "116", "UUCP-PATH", "118", "NNTP",
    	    "120", "ERPC", "122", "NTP", "124", "LOCUS-MAP", "126",
    	    "LOCUS-CON", "128", "PWDGEN", "CISCO-FNA", "CISCO-TNA",
    	    "CISCO-SYS", "STATSRV", "INGRES-NET", "LOC-SRV", "PROFILE",
    	    "NETBIOS-NS", "NETBIOS-DGM", "NETBIOS-SSN", "EMFIS-DATA",
    	    "EMFIS-CNTL", "BL-IDM"};
    int wks_name_count = sizeof(wks_name)/sizeof(char *);
    unsigned int rc_msg[]
    	  = {NSQ__QUERYOK, NSQ__FMTERR, NSQ__SRVFAIL, NSQ__NAMERR,
    	     NSQ__NOTIMPL, NSQ__REFUSED, NSQ__UNKRC, NSQ__UNKRC,
    	     NSQ__UNKRC, NSQ__UNKRC, NSQ__UNKRC, NSQ__UNKRC, NSQ__UNKRC,
    	     NSQ__UNKRC, NSQ__UNKRC, NSQ__UNKRC};
    int rc_msg_count = sizeof(rc_msg)/sizeof(unsigned int);
    char *type_name[]
    	 = {"A", "NS", "MD", "MF", "CNAME", "SOA", "MB", "MG",
    	    "MR", "NULL", "WKS", "PTR", "HINFO", "MINFO", "MX",
    	    "TXT", "AXFR", "MAILB", "MAILA", "ALL"};
    int type_name_count = sizeof(type_name)/sizeof(char *);
    char *class_input[]
    	 = {"INTERNET", "CSNET", "CHAOSNET", "HESIOD", "ALL"};
    int class_input_count = sizeof(class_input)/sizeof(char *);
/*
** The following vectors' elements must correspond to their respective
** name vectors above.
*/
    int type_value[] = {IPNS_TYPE_A, IPNS_TYPE_NS, IPNS_TYPE_MD,
    	    	IPNS_TYPE_MF, IPNS_TYPE_CNAME, IPNS_TYPE_SOA,
    	    	IPNS_TYPE_MB, IPNS_TYPE_MG, IPNS_TYPE_MR,
    	    	IPNS_TYPE_NULL, IPNS_TYPE_WKS, IPNS_TYPE_PTR,
    	    	IPNS_TYPE_HINFO, IPNS_TYPE_MINFO, IPNS_TYPE_MX,
    	    	IPNS_TYPE_TXT, IPNS_QTYPE_AXFR, IPNS_QTYPE_MAILB,
    	    	IPNS_QTYPE_MAILA, IPNS_QTYPE_ALL};

    int class_value[] = {IPNS_CLASS_IN, IPNS_CLASS_CS, IPNS_CLASS_CH,
    	    	IPNS_CLASS_HS, IPNS_QCLASS_ALL};
