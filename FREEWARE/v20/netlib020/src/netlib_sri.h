#ifndef __NETLIB_SRI_H__
#define __NETLIB_SRI_H__
#ifndef __NETLIB_BUILD__
#define __NETLIB_BUILD__
#endif
#include "netlibdef.h"
#include <iodef.h>

#define IOR_M_COPY_LENGTH  	(1<<16)
#define IOR_M_COPY_FROM    	(1<<17)
#define	IOR_M_NEW_CONTEXT  	(1<<18)
#define IOR_M_COPY_FROM2   	(1<<19)
#define IOR_M_CHECK_LENGTH  	(1<<20)

#define spec_retlen 	    	arg[0].address
#define spec_length 	    	arg[1].longword
#define spec_userfrom	    	arg[2].address
#define spec_newctx    	    	arg[3].address
#define spec_xnewctx	    	arg[4].address

#define __SPECCTX void
#define SPECCTX_SIZE	0

    struct SPECIOR {
    	struct {
    	    unsigned short length;
    	    struct SINDEF address;
    	} from;
    	struct {
    	    unsigned int length;
    	    struct SINDEF address;
    	} from2;
    };
#define __SPECIOR struct SPECIOR

#define IO$S_FCODE	6
#define IO$_SEND	(IO$_WRITEVBLK)
#define IO$_RECEIVE	(IO$_READVBLK)
#define IO$_SOCKET	(IO$_ACCESS | (0 << IO$S_FCODE))
#define IO$_BIND	(IO$_ACCESS | (1 << IO$S_FCODE))
#define IO$_LISTEN	(IO$_ACCESS | (2 << IO$S_FCODE))
#define IO$_ACCEPT	(IO$_ACCESS | (3 << IO$S_FCODE))
#define IO$_CONNECT	(IO$_ACCESS | (4 << IO$S_FCODE))
#define IO$_SETSOCKOPT	(IO$_ACCESS | (5 << IO$S_FCODE))
#define IO$_GETSOCKOPT	(IO$_ACCESS | (6 << IO$S_FCODE))
#define IO$_IOCTL	(IO$_ACCESS | (8 << IO$S_FCODE))
#define IO$_ACCEPT_WAIT (IO$_ACCESS | (10 << IO$S_FCODE))
#define IO$_NETWORK_PTY (IO$_ACCESS | (11 << IO$S_FCODE))
#define IO$_SHUTDOWN	(IO$_ACCESS | (12 << IO$S_FCODE))
#define IO$_GETSOCKNAME (IO$_ACCESS | (13 << IO$S_FCODE))
#define IO$_GETPEERNAME (IO$_ACCESS | (15 << IO$S_FCODE))
#define IO$_SELECT	(IO$_ACCESS | (17 << IO$S_FCODE))

#endif /* __NETLIB_SRI_H__ */
