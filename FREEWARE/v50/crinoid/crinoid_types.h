/**************************************************************************\
 *                                                                        *
 *          Common data structures                                        *
 *                                                                        *
 *                                                                        *
\**************************************************************************/
#ifndef __CRINOID_TYPES_H
#define __CRINOID_TYPES_H

#include "vms_data.h"

#define _PTHREAD_USE_D4

#ifdef EXC_WORKAROUND       /* some versions have problems with this */
#if EXC_WORKAROUND
#include "exc_handling.h"   /* "fixed" version, but may be old */
#endif
#endif

#include <pthread.h>
#include <pqldef.h>
#include <prvdef.h>

typedef struct ENV_entry             ENV;
typedef struct ENV_entry*           pENV;
typedef struct MBXMSG                MM;
typedef struct MBXMSG*              pMM;
typedef struct mbx_stuff             MBX;
typedef struct mbx_stuff*           pMBX;
typedef struct NetDataBlock          NDB;
typedef struct NetDataBlock*        pNDB;
typedef struct ThreadContextBlock    TCB;
typedef struct ThreadContextBlock*  pTCB;
typedef struct ProcessInfo           Proc;
typedef struct ProcessInfo*         pProc;
typedef struct quota_listitem        Quota;
typedef struct quota_listitem*      pQuota;
typedef struct ProcessGroup          Group;
typedef struct ProcessGroup*        pGroup;
typedef struct MBXQ                  MbxQ;
typedef struct MBXQ  *              pMbxQ;
typedef struct MBXQE                 MbxE;
typedef struct MBXQE *              pMbxE;
typedef struct _PIPE2   Pipe2;
typedef struct _PIPE2*  pPipe2;
typedef struct AbortTimerEntry      ATE;
typedef struct AbortTimerEntry*    pATE;

struct ENV_entry {
    pENV next;
    char *name;
    char *value;
    int   state;
};

#define MAX_NCB 1024
#define MAX_MSG sizeof(struct MBXMSG)


struct MBXMSG {
    word code;
    word unit;
    char info[MAX_NCB];
};



struct mbx_stuff {
    word    chan;
    STRING  d_name;
    char    name[65];
    word    unit;
    word    size;
};


#define MBXQEBUF             MAX_MAILBOX_SIZE

struct MBXQ {
    RQE      msgq;                 /* put here to keep alignment right ! */
    pMBX     mbx;
    longword option;              /* read/write & other flags */
    longword bytes;
    int      next_id;
    longword ef;
    longword shut_ef;
    longword pending;             /* write AST queued*/
    longword backlog;             /* write backlog */
    longword maxbacklog;
    int      inprogress;
    void     (*AST)(pMbxE);
    void    *extra;
    char    *name;
};

struct MBXQE {
    RQE link;
    pMbxQ queue;
    IOSB iosb;
    char buf[MBXQEBUF];
    int  len;
    int  id;
};



struct NetDataBlock {
    pNDB            next;
    pMbxQ           Qin;
    pMbxQ           Q2perl;
    pPipe2          Pout;
    pMBX            Merr;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    word            unit;
    int             pending_disconnect;
    int             pending_idle;
    int             connum;

    pTCB            tc;
    pProc           process;

};

struct ThreadContextBlock {
    int             id;
    pthread_t       *thread;
    pENV            ENVBase;
    char            *content;
    int             content_length;
    pNDB            ndb;
};

struct ProcessInfo {
    pProc            next;
    pProc            last;
    pProc            next_idle;
    pGroup           group;
    int              state;

    pMbxQ            commandQ;
    pNDB             ndb;

    longword         pid;
    pSTRING          name;
    pthread_mutex_t  mutex;
    int              maintain_lock;
    int              activations;
    int              killpend;
    longword         killtime[2];
};


#define PS_NEW          1
#define PS_STARTING     2
#define PS_STOPPING     3
#define PS_BUSY         4
#define PS_IDLE         5
#define PS_DEAD         6
#define PS_ANY         (-1)

struct quota_listitem {
    byte     id;
    longword value;
};

struct ProcessGroup {
    pGroup              next;
    pProc               idle_list;
    pthread_cond_t      cond;
    pthread_mutex_t     mutex;

    pSTRING             name;
    longword            uic;
    pSTRING             username;
    pSTRING             bindir;
    union prvdef        priv;
    Quota               quota[PQL$_LENGTH];
    longword            baspri;
    longword            stsflg;
    int                 usergroup;

    int                 n_idle;
    int                 n_busy;
    int                 n_total;
    int                 n_max;
    int                 n_min;
    int                 n_pad_max;
    int                 n_pad_min;
    int                 n_req;
    int                 n_starting;
    int                 n_serial;

    pSTRING             program;
    pSTRING             input;
    pSTRING             output;
    pSTRING             error;
};


typedef struct _SCRIPT_MATCH      Script;
typedef struct _SCRIPT_MATCH*    pScript;

struct _SCRIPT_MATCH {
    pScript next;
    pSTRING wild;
    pSTRING user;
    pSTRING server;
    pSTRING bindir;
    int type;
    unsigned int flags;
    char *permit;
    char *deny;
    int warn;
};

#define SCRIPT$C_SPECIFICUSER       1
#define SCRIPT$C_SERVERMAINT        2
#define SCRIPT$C_DEBUGPREFIX        3
#define SCRIPT$C_DEFAULTUSER        4

struct _PIPE2 {
    pMBX        in_mbx;
    pMBX        out_mbx;
    pMbxQ       inQ;
    pMbxQ       outQ;
    char *      buffer;
    int         bcount;
};

struct AbortTimerEntry {
    pATE    next;
    word    unit;
    int     type;
    int     ticks;
};

#define ATE_DISCON  1
#define ATE_ABORT   2

#define  LOG_MAIN_LOCK   "CRINOID_LOGGER"
#define  LOG_SUB_LOCK    "CRINIOD_LOGSTART"

#ifdef DEBUG_MEMORY
#include <stdlib.h>

#define free(p)      my_free(p,curr_routine_name)
#define malloc(s)    my_malloc(s,curr_routine_name)
#define realloc(p,s) my_realloc(p,s,curr_routine_name)
#endif
#define ROUTINE_NAME(n)   static char curr_routine_name[]=n


#endif
