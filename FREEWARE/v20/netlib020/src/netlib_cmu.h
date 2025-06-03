#ifndef __NETLIB_CMU_H__
#define __NETLIB_CMU_H__
#ifndef __NETLIB_BUILD__
#define __NETLIB_BUILD__
#endif
#include "netlibdef.h"
#include <iodef.h>

#define IO__OPEN    IO$_CREATE
#define IO__CLOSE   IO$_DELETE
#define IO__STATUS  IO$_ACPCONTROL
#define IO__INFO    IO$_MODIFY
#define IO__GTHST   IO$_SKIPFILE

#define CMU_K_GTHST_LOCAL_HOST	0
#define CMU_K_GTHST_NAMEADDR	1
#define CMU_K_GTHST_ADDRNAME	2
#define CMU_K_GTHST_DNSLOOKUP	3

#define CMU_M_OPEN_ACTIVE   	1
#define CMU_M_OPEN_UDPDATA  	1
#define CMU_M_OPEN_NOWAIT   	2
#define CMU_M_OPEN_BYADDRESS	4

#define CMU_K_PROTO_TCP	    	0
#define CMU_K_PROTO_UDP	    	1

    struct CMU_ConnInfo {
    	unsigned char ci_b_remote_host;
    	unsigned char ci_b_local_host;
    	char ci_t_foreign_host[128];
    	unsigned short ci_w_fill1;
    	unsigned int ci_l_remote_port;
    	char ci_t_local_host[128];
    	unsigned int ci_l_local_port;
    	struct INADDRDEF ci_x_local_address;
    	struct INADDRDEF ci_x_remote_address;
    };

    struct CMU_IPAddr {
    	struct INADDRDEF ipa_x_srcaddr;
    	struct INADDRDEF ipa_x_dstaddr;
    	unsigned short ipa_w_srcport;
    	unsigned short ipa_w_dstport;
    	unsigned int ipa_l_fill1;
    };

    struct CMU_NtoA {
    	unsigned int ntoa_l_count;
    	struct INADDRDEF ntoa_x_addr[20];
    	unsigned int ntoa_l_namelen;
    	char ntoa_t_name[128];
    };

    struct CMU_AtoN {
    	unsigned int aton_l_namelen;
    	char aton_t_name[128];
    };

    struct CMU_RR {
    	unsigned short rr_w_length;
    	unsigned char rr_x_data[512];
    };

    struct CMUIosb {
    	unsigned short status;
    	unsigned short count;
    	unsigned long cmu_status;
    };

#define __SPECCTX struct SPECCTX

    struct SPECCTX {
    	unsigned int socket_type;
    	unsigned int flags;
#define SPECCTX_M_OPEN	    	(1<<0)
    	unsigned short local_port, remote_port;
    	struct INADDRDEF local_addr, remote_addr;
    };
#define SPECCTX_SIZE (sizeof(struct SPECCTX))

#define spec_retlen 	    	arg[0].address
#define spec_length 	    	arg[1].word
#define spec_userfrom	    	arg[2].address
#define spec_newctx    	    	arg[3].address
#define spec_xnewctx	    	arg[4].address
#define spec_userbuf	    	arg[5].address
#define spec_addr_info	    	arg[6].address
#define spec_name_info	    	arg[6].address
#define spec_rr_info	    	arg[6].address
#define spec_conn_info	    	arg[6].address
#define spec_usersize	    	arg[7].longword

#define IOR_M_COPY_LENGTH  	(1<<16)
#define IOR_M_COPY_FROM    	(1<<17)
#define	IOR_M_NEW_CONTEXT  	(1<<18)
#define IOR_M_COPY_ADDRS   	(1<<19)
#define IOR_M_COPY_HOSTNAME	(1<<20)
#define IOR_M_COPY_CI_REMOTE	(1<<21)
#define IOR_M_COPY_CI_LOCAL	(1<<22)
#define IOR_M_COPY_MXRRS   	(1<<23)
#define IOR_M_SET_OPENFLAG  	(1<<24)

    struct SPECIOR {
    	unsigned short fromlen;
    	struct CMU_IPAddr from;
    	struct CMU_NtoA addr_info;
    };
#define __SPECIOR struct SPECIOR

#define NET$_IR 	0X0863800A
#define NET$_IFC	0X08638012
#define NET$_IPC	0X0863801A
#define NET$_UCT	0X08638022
#define NET$_IFS	0X0863802A
#define NET$_ILP	0X08638032
#define NET$_NUC	0X0863803A
#define NET$_CSE	0X08638042
#define NET$_NOPRV	0X0863804A
#define NET$_CIP	0X08638052
#define NET$_CDE	0X0863805A
#define NET$_CR 	0X08638062
#define NET$_FSU	0X0863806A
#define NET$_UNN	0X08638072
#define NET$_VTF	0X0863807A
#define NET$_CREF	0X08638082
#define NET$_CCAN	0X0863808A
#define NET$_FIP	0X08638092
#define NET$_BTS	0X0863809A
#define NET$_IHI	0X086380A2
#define NET$_BDI	0X086380AA
#define NET$_EPD	0X086380B2
#define NET$_URC	0X086380BA
#define NET$_IGF	0X086380C2
#define NET$_UNA	0X086380CA
#define NET$_UNU	0X086380D2
#define NET$_CC 	0X086380DA
#define NET$_CTO	0X086380E2
#define NET$_TWT	0X086380EA
#define NET$_TE 	0X086380F2
#define NET$_FTO	0X086380FA
#define NET$_NYI	0X08638102
#define NET$_NOPN	0X0863810A
#define NET$_NOINA	0X08638112
#define NET$_NOANA	0X0863811A
#define NET$_NOADR	0X08638122
#define NET$_GTHFUL	0X0863812A
#define NET$_DAE	0X08638132
#define NET$_NMLTO	0X0863813A
#define NET$_NSEXIT	0X08638142
#define NET$_NONS	0X0863814A
#define NET$_NSQFULL	0X08638152
#define NET$_DSDOWN	0X0863815A
#define NET$_DSNODS	0X08638162
#define NET$_DSINCOMP	0X0863816A
#define NET$_DSNOADDR	0X08638172
#define NET$_DSNONAME	0X0863817A
#define NET$_DSFMTERR	0X08638182
#define NET$_DSSRVERR	0X0863818A
#define NET$_DSNAMERR	0X08638192
#define NET$_DSNOTIMP	0X0863819A
#define NET$_DSREFUSD	0X086381A2
#define NET$_DSNONSRV	0X086381AA
#define NET$_DSUNKERR	0X086381B2
#define NET$_DSREFEXC	0X086381BA
#define NET$_GREENERR	0X086381C2
#define NET$_GP_INVREQ	0X086381CA
#define NET$_GP_INVINF	0X086381D2
#define NET$_GP_INVNAM	0X086381DA
#define NET$_GP_INVADR	0X086381E2
#define NET$_GP_INVMBX	0X086381EA
#define NET$_GP_INVCLS	0X086381F2
#define NET$_GP_RSBUSY	0X086381FA
#define NET$_GP_NONMSR	0X08638202
#define NET$_GP_NOHINF	0X0863820A
#define NET$_GP_NOTFND	0X08638212
#define NET$_GP_UNKMBX	0X0863821A
#define NET$_GP_NOTIMP	0X08638222
#define NET$_GP_TOOBIG	0X0863822A
#define NET$_GP_NSDOWN	0X08638232
#define NET$_NRT    	0X0863823A
#define NET$_KILL	0X08638242

#endif /* __NETLIB_CMU_H__ */
