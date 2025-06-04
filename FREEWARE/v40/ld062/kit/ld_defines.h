/* Definitions to use with the LDdriver */

#include <fiddef.h>
#include <sbkdef.h>

#define	IO$_LD_CONTROL		20

#define	LDIO_CONNECT		0
#define	LDIO_DISCONNECT		1
#define	LDIO_ENABLE_TRACE	2
#define	LDIO_DISABLE_TRACE	3
#define	LDIO_GET_TRACE		4
#define	LDIO_RESET_TRACE	5
#define	LDIO_GET_CONNECTION	6
#define	LDIO_SET_SEED		7
#define	LDIO_ENABLE_WATCH	8
#define	LDIO_DISABLE_WATCH	9
#define	LDIO_GET_WATCH		10
#define	LDIO_RESUME_WATCH	11
#define	LDIO_GET_SUSPEND_LIST	12
#define	LDIO_ENABLE_PROTECT	13
#define	LDIO_DISABLE_PROTECT	14
#define	LDIO_SET_ALLOCLASS	15

#define	LDIO_M_REPLACE		256
#define	LDIO_V_REPLACE		8
#define	LDIO_M_ABORT		512                                       
#define	LDIO_V_ABORT		9
#define	LDIO_M_INQUIRE		1024
#define	LDIO_V_INQUIRE		10
#define	LDIO_M_NOWAIT		2048
#define	LDIO_V_NOWAIT	    	11
#define	LDIO_M_RESET		4096
#define	LDIO_V_RESET		12
#define	LDIO_M_SHARE		8192
#define	LDIO_V_SHARE		13

#define	WATCH_ACTION_SUSPEND	0
#define	WATCH_ACTION_CRASH	1
#define	WATCH_ACTION_ERROR	2
#define	WATCH_ACTION_OPCOM	3

/*
  Structure definitions and declarations
*/

struct trace_ent {
            unsigned int pid;
            unsigned int lbn;
            unsigned int bcnt;
            unsigned short func;
            unsigned short reserved;
            int iosb[2];
            int start_time[2];
            int end_time[2];
           };

struct watchpt {
            unsigned int lbn;
            unsigned short flags;
            unsigned short action;
            unsigned short func;
            unsigned short retcode;
	    struct sbkdef *sbk;
            struct fiddef fid;
           };

#define FLAGS_NOLBN		0x01
#define FLAGS_FILE		0x02
#define FLAGS_REMOVE_ALL	0x80

struct suspend_list {
            unsigned int pid;
            unsigned int lbn;
            unsigned short flags;
            unsigned short action;
            unsigned short func;
            unsigned short retcode;
           };
