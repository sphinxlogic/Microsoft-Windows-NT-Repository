/*
    test process spawning code

*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <unixlib.h>

#include <ssdef.h>
#include <starlet.h>
#include <libdef.h>
#include <lnmdef.h>
#include <iodef.h>
#include <psldef.h>
#include <prvdef.h>
#include <prcdef.h>
#include <pqldef.h>
#include <dvidef.h>
#include <nfbdef.h>
#include <accdef.h>
#include <lckdef.h>
#include <lib$routines.h>
#include <reentrancy.h>

#include "version.h"
#include "msg.h"
#include "errlog_client.h"
#include "util.h"
#include "proc_mgr.h"
#include "pipe2.h"
#include "mbxq.h"
#include "script.h"
#include "CRINOID.h"
#include "parser.h"

pthread_key_t           TCBkey;
static RQE              GotMsgQueueHead = {0, 0};
#define GotMsgQ         (&GotMsgQueueHead)
static pMBX             NetMBX;
static pMbxQ            NetQ;
static pMbxQ            TermQ;
static  pNDB            NDB_Head = 0;
static int              Threads_initted = 0;
pthread_mutex_t         NDB_mutex;
pthread_cond_t          Main_cond;
pthread_mutex_t         Main_mutex;
static  int             Main_Shutdown = 0;
static pGroup           LocalScriptGroup = 0;
#ifdef ALLOWED_DEFAULT_USERGROUP
#define DEFAULT_USER_CGI_DIRECTORY      "/www-cgi"
static  char *user_cgi_dir  = DEFAULT_USER_CGI_DIRECTORY;
#endif
#define STDERRLOG       "CRINOIDLOG_STDERR_MBX"
#define DEFAULT_SERVICE_NAME "WWWPERL"
#define NET_TIMEOUT     30
static pSTRING          Decnet_Service_Name = 0;
static int              Loglevel = L_CRITICAL;
static int              Allowed_Active;

static RQE MsgToThreadQueueHead = {0,0};
#define ThreadMsgQ  (&MsgToThreadQueueHead)
#define DEFAULT_MAX_THREADS 5
static int MAX_THREADS = DEFAULT_MAX_THREADS;
pthread_t       *Thread;
pthread_cond_t  ThreadMsgReady;
pthread_mutex_t ThreadMutex;
static int  Threads_Ready = 0;

int
main(int argc, char** argv, char** env)
{
    ROUTINE_NAME("main");
    int iss, shutdown, type, command, done_with_packet;
    pProc p;
    pGroup g;
    pMM m;
    pMBX net;
    pMbxE e;
    pPrivs priv;
    struct NETDCL {
        byte      type;
        longword  value;
    } nfb = {NFB$C_DECLNAME, 0};
    pSTRING Nfb;
    IOSB iosb;
    int  logical_Loglevel = -1;

    decc$set_reentrancy (C$C_MULTITHREAD);

/*  error logger startup */

    StartLogger();
    {
        pSTRING p = translate_logical("CRINOID_LOGLEVEL");
        if (p) {
            logical_Loglevel = atoi(asciz_pSTRING(p));
            destroy_STRING(p);
        }
    }

    errlog(L_CRITICAL,"This is CRINOID V!AZ, built on !AZ at !AZ",CRINOID_VERSION,__DATE__,__TIME__);
    errlog_level(logical_Loglevel > 0 ? logical_Loglevel : Loglevel);

/*  i/o initial setup; have to wait until read configfile to finish */

    Decnet_Service_Name = new_STRING(DEFAULT_SERVICE_NAME);
    {
        pSTRING p = translate_logical("CRINOID_DECNET_SERVICE");
        if (p) {
            destroy_STRING(Decnet_Service_Name);
            Decnet_Service_Name = p;
        }
    }
    NetMBX = new_MBX_setquota(0, MAX_MSG, (MAX_MSG+8)*30);

/*  thread initialization */

    iss = pthread_mutex_init(&Main_mutex,pthread_mutexattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("main(), mutex_init");
    iss = pthread_cond_init(&Main_cond,pthread_condattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("main(), cond_init");
    iss = pthread_mutex_init(&NDB_mutex,pthread_mutexattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("main(), ndb mutex_init");
    iss  = pthread_keycreate(&TCBkey, &destroy_TCB2);
    if (UNIX_ERR(iss)) UNIX_ABORT("main(), keycreate");

    Threads_initted = 1;

/*  get program names, device names used for process startup */

    TermQ = MbxQ_new(0     , MBXQ$_READ, &messageAST);

    {
        pSTRING p = translate_logical("CRINOID_HOME");
        char program[500];

        tu_strcpy(program,asciz_pSTRING(p));
        tu_strcat(program,"tentacle.com");
        init_Group(program,"",NetMBX->name,"");

        tu_strcpy(program, asciz_pSTRING(p));
        tu_strcat(program, "stub.com");

        init_Proc(TermQ->mbx, program);
        destroy_STRING(p);
    }

/*  configure groups, scripts, etc */

    LocalScriptGroup = new_Group("LOCAL");
    ReadMainConfig("CRINOID_HOME:CRINOID.CONFIG");

/* loglevel logical overrides config file, so reset */

    if (logical_Loglevel > 0) Loglevel = logical_Loglevel;

/*  start DECNET task stuff */

    net = assign_new_MBX("_NET:", NetMBX);
    Nfb = new_STRING2(sizeof(nfb));
    asciz_pSTRING(Nfb) = (char *) &nfb;

    priv = new_Privs();
    priv->prv$v_sysnam = 1;
    iss = Set_Privs(priv);
    if (VMS_ERR(iss)) VMS_ABORT(iss,"main, setprv");

    iss = sys$qiow(0,net->chan,IO$_ACPCONTROL, &iosb, 0,0, Nfb, Decnet_Service_Name, 0, 0, 0, 0);
    if ((iss&0xFFFF) == SS$_BADPARAM) {
        printf("Another CRINOID already running, aborting\n");
        VMS_ABORT(SS$_ABORT,"main, another CRINOID already running");
    }
    if (VMS_ERR(iss)) VMS_ABORT(iss,"main, acpcontrol");
    if (VMS_ERR(iosb.status)) VMS_ABORT(iosb.status,"main, acpcontrol iosb");

    iss = Reset_Privs(priv);
    if (VMS_ERR(iss)) VMS_ABORT(iss,"setprv, resetting privs");

    NetQ  = MbxQ_new(NetMBX, MBXQ$_READ, &messageAST);

    init_manage_Proc();             /* process manager thread */
    init_threads();                 /* connection handling threads */
    init_ATE_Proc();                /* abort timer entry processor */
/*  atexit(&Exit_handler);          */

/*  estimate Allowed_Active from current quota, default sizes */

    {
        longword pid = get_pid();
        int per_connection;
        per_connection = 2*DEFAULT_MAILBOX_QUOTA;    /* 2 mbx's */
        per_connection += 256 + 640;                 /* + NET: + ? extra */

        Allowed_Active = get_bytcnt(pid)/per_connection;
        Allowed_Active -= 2;  /* safety margin */

        errlog(L_CRITICAL,"BYTLM quota allows !SL simultaneous active Tentacles",Allowed_Active);
        if (Allowed_Active < 1) {
            Allowed_Active = 1;
            errlog(L_CRITICAL,"--> adjusted to 1 active Tentacle, but may have BYTCNT quota problems!");
        }
    }

/*  main message-handling loop */

    while (!Main_Shutdown) {
        iss = pthread_mutex_lock(&Main_mutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("main, mutex_lock");

        iss = lib$remqhi(GotMsgQ, &e);
        while (iss == LIB$_QUEWASEMP) {
            iss = pthread_cond_wait(&Main_cond, &Main_mutex);
            if (UNIX_ERR(iss)) UNIX_ABORT("main, cond_wait");
            iss = lib$remqhi(GotMsgQ, &e);
        }
        if (VMS_ERR(iss)) VMS_ABORT(iss,"main, dequeuing msg packet");

        iss = pthread_mutex_unlock(&Main_mutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("main, mutex_unlock");

        m = (pMM) e->buf;
        command = m->code;
        errlog(L_MAIN|L_BABBLE,"got NETMSG packet, code:!UW (!AZ)",command, msg_text(command));
        done_with_packet = 1;

        switch (command) {
            case MSG$_DEVONLIN:
                do_online(e);
                break;
            case MSG$_IDLE:
                do_idle(e);
                break;
            case MSG$_NETSHUT:
                Main_Shutdown = 1;
                errlog(L_CRITICAL,"got NETSHUT packet, shutting down");
                break;
            case MSG$_CONNECT:
                thread_handle(e);
                done_with_packet = 0;
                break;
            case MSG$_ABORT:
            case MSG$_EXIT:
            case MSG$_PATHLOST:
            case MSG$_PROTOCOL:
            case MSG$_THIRDPARTY:
            case MSG$_TIMEOUT:
                break;
            case MSG$_PENDABORT:
                do_pendabort(e);
                break;
            case MSG$_DISCON:
                do_disconnect(e);
                break;
            case MSG$_DELPROC:
                do_offline(e);      /* process exited */
                break;
            case MSG$_HEARTBEAT:
                do_heartbeat(e);
                break;
            case MSG$_STUBREADY:
                do_stubstart(e);
                break;
            case MSG$_STARTPID:
                do_startpid(e);
                break;
            case MSG$_LOGQLOCK:
                StartLogger_queue_lock();
                break;
            case MSG$_LOGSTART:
                StartLoggerProc();
                break;
            default:
                break;
        }
        if (done_with_packet) {
            MbxQ_dispose(e);
        }

    }

    stop_manage_Proc();
    kill_all_Procs();
    stop_threads();
    stop_ATE_Proc();

    return(SS$_NORMAL);
}




void
messageAST(pMbxE e)
{
    int process = 0;
    int iss = e->iosb.status;
    word size = e->iosb.count;
    word code;
    pMM  m = (pMM) e->buf;

    if (VMS_ERR(iss)) {
        errlog(L_MAIN|L_ERROR,"messageAST iosb err 0x!XL ",iss);
    } else if (size < 4) {
        errlog(L_MAIN|L_ERROR,"messageAST runt message, !UW bytes from PID !XL",size,e->iosb.dvispec);
        logdump(L_ERROR,"messageAST runt message:", e->buf, size);
    } else {
        int max = size > 64 ? 64 : size;

        errlog(L_MAIN|L_BABBLE,"messageAST message, code: !UW (!AZ), size !UW from PID !XL", m->code, msg_text(m->code),size,e->iosb.dvispec);
        if (m->code > MSG$_MAXMSG) {
            errlog(L_ERROR,"Error text?: '!AF'",size,e->buf);
        } else {
            logdump(L_MAIN|L_DEBUG,"messageAST message:", e->buf, max);
            process = 1;
        }
    }

    if (process && !Main_Shutdown) {
        if (m->code == MSG$_CONNECT && Threads_Ready) {
            errlog(L_MAIN|L_BABBLE,"messageAST, do_connect queued");
            iss = lib$insqti(e, ThreadMsgQ);
            if (VMS_ERR(iss)) VMS_ABORT(iss,"messageAST, insert ThreadMsgQ");
            iss = pthread_cond_signal_int_np(&ThreadMsgReady);
            if (UNIX_ERR(iss)) UNIX_ABORT("messageAST, cond_signal ThreadMsg");
        } else {
            iss = lib$insqti(e, GotMsgQ);
            if (VMS_ERR(iss)) VMS_ABORT(iss,"messageAST, insqti");
            iss = pthread_cond_signal_int_np(&Main_cond);
            if (UNIX_ERR(iss)) UNIX_ABORT("messageAST, cond_signal");
        }
    } else {
        MbxQ_dispose(e);
    }
}



int
do_online(pMbxE e)
{
    ROUTINE_NAME("do_online");
    int iss, len;
    longword pid;
    char outbuf[256];
    pMM m = (pMM) e->buf;
    pSTRING errlogmbx;
    pProc p;
    pGroup g;
    pMBX cmbx;

    pid = e->iosb.dvispec;

    errlog(L_MAIN|L_TRACE,"do_online (pid=!XL) ",pid);

    p = get_Proc(pid);

    errlog(L_MAIN|L_BABBLE,"do_online, lookup found p=!XL",p);
    if (!p) return 0;

    lock_Proc(p);
    p->maintain_lock = 1;
    g = p->group;

    sprintf(outbuf,"_MBA%d:",(longword)m->unit);
    cmbx = assign_new_MBX(outbuf,0);
    p->commandQ = MbxQ_new(cmbx, MBXQ$_WRITE|MBXQ$M_RETRY, 0);

    setidle_Proc(p);
    p->maintain_lock = 0;
    unlock_Proc(p);

    return 1;
}

int
send_message(pMbxQ q, word code, word unit, void *s, int n)
{
    ROUTINE_NAME("send_message");
    int length, iss;
    MbxE  e;
    pMM m = (pMM) &e.buf;

    errlog(L_MAIN|L_TRACE,"send_message(queue=!XL,code=!UW(!AZ),unit=!UW,bytes=!SL)",q,code,msg_text(code),unit,n);
    logdump(L_MAIN|L_DEBUG,"send_message data:",s,(n>64?64:0));

    m->code = code;
    m->unit = unit;

    length = sizeof(m->code) + sizeof(m->unit);
    if (s && n > 0) {
        if (n+length > sizeof(e.buf)) VMS_ABORT(SS$_INSFMEM,"send_message, buffer too big");
        memcpy(m->info, s, n);
        length += n;
    }

    iss = MbxQ_write(q, e.buf, length);
    return iss;
}



void
do_offline(pMbxE e)
{
    ROUTINE_NAME("do_offline");
    pMM m = (pMM) e->buf;
    pProc p;
    pNDB ndb;
    longword pid;
    int iss, len;
    struct accdef accmsg;

    pid = e->iosb.dvispec;
    errlog(L_MAIN|L_INFO,"do_offline: got DELPROC msg, pid=!XL",pid);

    memcpy((char *)&accmsg, (char *)m, ACC$K_TERMLEN);
    errlog(L_MAIN|L_INFO,"do_offline: final status !XL",accmsg.acc$l_finalsts);
    errlog(L_MAIN|L_BABBLE,"do_offline: CPU time !UL * 10ms",accmsg.acc$l_cputim);
    errlog(L_MAIN|L_BABBLE,"do_offline: pagefaults: !UL",accmsg.acc$l_pageflts);
    errlog(L_MAIN|L_BABBLE,"do_offline: peak pagefile usage !UL",accmsg.acc$l_pgflpeak);
    errlog(L_MAIN|L_BABBLE,"do_offline: working set peak usage: !UL",accmsg.acc$l_wspeak);
    errlog(L_MAIN|L_BABBLE,"do_offline: buffered i/o operations: !UL",accmsg.acc$l_biocnt);
    errlog(L_MAIN|L_BABBLE,"do_offline: direct i/o operations: !UL",accmsg.acc$l_diocnt);

    p = get_Proc(pid);
    errlog(L_MAIN|L_INFO,"do_offline: pid !XL ndb !XL",pid, (p?p->ndb:0));
    if (!p) return;

    iss = pthread_mutex_lock(&NDB_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_offline, NDBmutex_lock");

    lock_Proc(p);
    p->maintain_lock = 1;
    if (p->commandQ) {
        pMBX mc = p->commandQ->mbx;
        p->commandQ = MbxQ_destroy(p->commandQ);
        destroy_MBX(mc);
    }

    if (ndb = p->ndb) {
        errlog(L_MAIN|L_TRACE,"tentacle offline, connection #!SL",ndb->connum);

        iss = pthread_mutex_lock(&ndb->mutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("do_offline, mutex_lock");
        ndb->pending_idle = 1;
        ndb->process = 0;
        if (ndb->pending_disconnect) {
            dequeue_ATE(ndb->unit, ATE_ABORT);
            destroy_NDB(ndb);
        } else {
            errlog(L_MAIN|L_BABBLE,"do_offline, waiting for disconnect to destroy NDB");
            queue_ATE(ndb->unit, ATE_DISCON, 10);
            iss = pthread_mutex_unlock(&ndb->mutex);
            if (UNIX_ERR(iss)) UNIX_ABORT("do_offline, mutex_unlock");
        }
    }
    destroy_Proc(p);

    iss = pthread_mutex_unlock(&NDB_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_offline, NDBmutex_unlock");

}


void
init_threads()
{
    ROUTINE_NAME("init_threads");
    int iss, j;
    int *id;

    Thread = (pthread_t *) malloc(MAX_THREADS * sizeof(pthread_t));
    id  = (int *) malloc(MAX_THREADS * sizeof(int));

    errlog(L_MAIN|L_BABBLE, "init_threads()");
    iss = pthread_cond_init(&ThreadMsgReady,pthread_condattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("init_threads, cond_init");
    iss = pthread_mutex_init(&ThreadMutex,pthread_mutexattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("init_threads, mutex_init");

    for (j=0; j<MAX_THREADS; j++) {
        id[j] = j;
        iss = pthread_create(&Thread[j],pthread_attr_default,&thread_code,&id[j]);
        if (UNIX_ERR(iss)) UNIX_ABORT("init_threads, pthread_create");
    }
    Threads_Ready = 1;
}

void
stop_threads()
{
    ROUTINE_NAME("stop_threads");
    int iss, j;

    errlog(L_MAIN|L_BABBLE, "stop_threads()");
    for (j = 0; j < MAX_THREADS; j++) {
        iss = pthread_cancel(Thread[j]);
        if (UNIX_ERR(iss)) UNIX_ABORT("stop_threads, pthread_cancel");
    }
}

void *
thread_code(void *pNum)
{
    ROUTINE_NAME("thread_code");
    int iss;
    pMM   m;
    pMbxE e;
    int command;
    int ThNum;
    pTCB tc;

    ThNum = * ((int *)pNum);
    errlog(L_MAIN|L_BABBLE,"thread_code(!SL)",ThNum);

    tc = new_TCB(ThNum);
    iss = pthread_setspecific(TCBkey, tc);
    if (UNIX_ERR(iss)) UNIX_ABORT("thread_code, setspecific");
    errlog(L_MAIN|L_BABBLE,"thread_code(!SL) tc at 0x!XL",ThNum,tc);

    while (1) {
        iss = pthread_mutex_lock(&ThreadMutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("thread_code, mutex_lock");

        iss = Allowed_Active ? lib$remqhi(ThreadMsgQ, &e) : LIB$_QUEWASEMP;
        while (iss == LIB$_QUEWASEMP) {
            iss = pthread_cond_wait(&ThreadMsgReady,&ThreadMutex);
            if (UNIX_ERR(iss)) UNIX_ABORT("thread_code, cond_wait");
            iss = Allowed_Active ? lib$remqhi(ThreadMsgQ, &e) : LIB$_QUEWASEMP;
        }

        if (VMS_ERR(iss)) VMS_ABORT(iss,"thread_code, get msg from q");
        Allowed_Active--;
        errlog(L_INFO,"do_connect: Allowed_Active now !SL",Allowed_Active);

        iss = pthread_mutex_unlock(&ThreadMutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("thread_code, mutex_unlock");

        errlog(L_MAIN|L_BABBLE,"thread_code(thread=!SL) got Msg @ 0x!XL",ThNum,e);

        do_connect(tc, e);
        MbxQ_dispose(e);
    }
    destroy_TCB(tc);
    return 0;
}


void
thread_handle(pMbxE e)
{
    ROUTINE_NAME("thread_handle");
    int iss;

    iss = lib$insqti(e, ThreadMsgQ);
    if (VMS_ERR(iss)) VMS_ABORT(iss,"thread_handle, insert thread msg q");

    iss = pthread_mutex_lock(&ThreadMutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("thread_handle, mutex_lock");

    iss = pthread_cond_signal(&ThreadMsgReady);
    if (UNIX_ERR(iss)) UNIX_ABORT("thread_handle, cond_signal");

    iss = pthread_mutex_unlock(&ThreadMutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("thread_handle, mutex_unlock");
}


void
do_connect(pTCB tc, pMbxE e)
{
    ROUTINE_NAME("do_connect");
    int iss;
    pProc  p = 0;
    pGroup g;
    pNDB   ndb;

    errlog(L_MAIN|L_TRACE,"do_connect(tc=0x!XL, e=0x!XL)",tc, e);

/*  assume we start with a clean tc */

    ndb  = net_connect(e);
    if (!ndb) return;
    tc->ndb  = ndb;
    ndb->tc  = tc;
    errlog(L_MAIN|L_TRACE,"new connection #!SL",ndb->connum);
    errlog(L_MAIN|L_BABBLE,"do_connect, got new ndb !XL, hooked to tc !XL",ndb,tc);


    if (ndb->pending_disconnect) goto done;
    net_readENV();
    if (ndb->pending_disconnect) goto done;
    netwrite("<DNETCGI>");

/*  start talking to the process that is handling this request */

    if (ndb->pending_disconnect) goto done;

    g = net_select_group();

    if (!g) {
        if (ndb->pending_disconnect) goto done;
        netwrite("Content-type: text/plain\n\n");
        netwrite("unable to find appropriate process group\n");
        netwrite("</DNETCGI>");
        goto done;
    }
    if (g == LocalScriptGroup) {
        if (ndb->pending_disconnect) goto done;
        local_script();
        goto done;
    }
    if (ndb->pending_disconnect) goto done;
    p = getidle_Proc(g);

    if (ndb->pending_disconnect) goto done;
    iss = pthread_mutex_lock(&NDB_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_connect, NDBmutex_lock");

    ndb->process = p;
    p->ndb       = ndb;
    errlog(L_MAIN|L_BABBLE,"do_connect: connecting ndb !XL chan !UW to proc !XL PID !XL",ndb, ndb->unit,p,p->pid);

    iss = pthread_mutex_unlock(&NDB_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_connect, NDBmutex_unlock");

/*  set up i/o mailboxes to Perl */

    add_ENV("CRINOID:>PERL",ndb->Q2perl->mbx->name,1);
    add_ENV("CRINOID:<PERL",ndb->Pout->in_mbx->name,1);
    {
        pSTRING s = translate_logical(STDERRLOG);
        if (s) {
            add_ENV("CRINOID:2<PERL",asciz_pSTRING(s),1);
            destroy_STRING(s);
        }
    }
    if (ndb->pending_disconnect) goto done;
    p->state = PS_BUSY;
    net_sendENV(p);

/*  connect i/o pipe from process to network, from content to proc */

    if (ndb->pending_disconnect) goto done;
    if (tc->content) {
        MbxQ_write(ndb->Q2perl, tc->content, tc->content_length);
        free(tc->content);
        tc->content = 0;
        tc->content_length = 0;
    }
    MbxQ_write(ndb->Q2perl,  0, -1);    /* EOF at end of content */

/*  tell proc to start the script */

    errlog(L_MAIN|L_BABBLE,"do_connect: triggering tentacle pid=!XL",p->pid);
    if (ndb->pending_disconnect) goto done;
    net_runscript(p);

done:
    if (ndb->pending_disconnect) {
        if (p) send_message(p->commandQ, MSG$_ABORT, 0, 0, 0);
        errlog(L_CRITICAL,"do_connect: pending_disconnect detected, abort sent");
    }
    iss = pthread_mutex_unlock(&ndb->mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_connect, ndb->mutex lock");
    clean_TCB(tc);
}


void
do_idle(pMbxE e)
{
    ROUTINE_NAME("do_idle");
    int iss;
    longword pid;
    pProc p;
    pNDB  ndb;

    pid = e->iosb.dvispec;
    p = get_Proc(pid);
    errlog(L_MAIN|L_TRACE,"do_idle pid=!XL ndb=!XL",pid,(p?p->ndb:0));
    if (!p) return;

    iss = pthread_mutex_lock(&NDB_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_idle, NDBmutex_lock");

    errlog(L_MAIN|L_BABBLE,"do_idle: ndb !XL",p->ndb);
    if (ndb = p->ndb) {
        errlog(L_MAIN|L_BABBLE,"idling connection #!SL",ndb->connum);

        iss = pthread_mutex_lock(&ndb->mutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("do_idle, mutex_lock");

        ndb->pending_idle = 1;
        if (!ndb->pending_disconnect) {
            /* ndb->process = 0;  */

            queue_ATE(ndb->unit, ATE_DISCON, 10);

            iss = pthread_mutex_unlock(&ndb->mutex);
            if (UNIX_ERR(iss)) UNIX_ABORT("do_idle, mutex_unlock");
            iss = pthread_mutex_unlock(&NDB_mutex);
            if (UNIX_ERR(iss)) UNIX_ABORT("do_idle, NDBmutex_unlock");
            setidle_Proc(p);
            return;
        } else {
            dequeue_ATE(ndb->unit, ATE_ABORT);
        }
        destroy_NDB(ndb);
    }
    p->ndb = 0;

    iss = pthread_mutex_unlock(&NDB_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_idle, NDBmutex_unlock");

    setidle_Proc(p);
}



void
net_sendENV(pProc p)
{
    ROUTINE_NAME("net_sendENV");
    int n, l, remain, jbuf=0;
    char *b, *q, *bend, buffer[MAX_NCB];
    pENV e = *get_ENVBase();

    errlog(L_MAIN|L_BABBLE,"net_sendENV(p = 0x!XL)",p);
    b = buffer;
    bend = buffer + sizeof(buffer);
    while (e) {
        q = e->name;
        remain = tu_strlen(q)+1;
        while (remain > 0) {
            l = b+remain > bend ? bend-b : remain;
            if (l == 0) {
                send_message(p->commandQ,MSG$_ENVDATA,jbuf++,buffer,b-buffer);
                b = buffer;
                l = b+remain > bend ? bend-b : remain;
            }
            memcpy(b,q,l);
            b += l;
            q += l;
            remain -= l;
        }
        q = e->value;
        remain = tu_strlen(q)+1;
        while (remain > 0) {
            l = b+remain > bend ? bend-b : remain;
            if (l == 0) {
                send_message(p->commandQ,MSG$_ENVDATA,jbuf++,buffer,b-buffer);
                b = buffer;
                l = b+remain > bend ? bend-b : remain;
            }
            memcpy(b,q,l);
            b += l;
            q += l;
            remain -= l;
        }

        e = e->next;
    }
    if (b-buffer)
        send_message(p->commandQ,MSG$_ENVDATA,jbuf,buffer,b-buffer);
}


pNDB
net_connect(pMbxE e)
{
    ROUTINE_NAME("net_connect");
    pMM m = (pMM) e->buf;
    pMBX mbx, mbx2;
    char *p;
    int iss, length;
    pNDB ndb;
    $DESCRIPTOR(d_ncb,"");
    IOSB iosb;
    static int connect_number = 0;
#ifdef PRINT_QUOTA
    int quota;
    longword pid;
#endif

    errlog(L_MAIN|L_BABBLE,"net_connect(e = 0x!XL)",e);
#ifdef PRINT_QUOTA
    pid = get_pid();
    quota = get_bytcnt(pid);
    errlog(L_INFO,"quota @ net_connect: !SL bytes",quota);
#endif

    p = m->info;
    p = p + *p + 1;
    length = *p++;
    d_ncb.dsc$a_pointer = p;
    d_ncb.dsc$w_length  = length;
    logdump(L_MAIN|L_DEBUG,"net_connect, ncb ",p,length);

    ndb = malloc(sizeof(NDB));
    if (!ndb) VMS_ABORT(SS$_INSFMEM,"net_connect, malloc NDB");

    ndb->tc      = 0;
    ndb->process = 0;
    ndb->pending_disconnect = 0;
    ndb->pending_idle = 0;

    mbx = assign_new_MBX("_NET:",NetMBX);

    iss = sys$qiow(0,mbx->chan,IO$_ACCESS,&iosb,0,0,0,&d_ncb,0,0,0,0);
    if (VMS_ERR(iss)) VMS_ABORT(iss,"net_connect, io$_access");
    iss = iosb.status;
    if (VMS_ERR(iss)) {
        pSTRING s = errormsg(iss);
        errlog(L_ERROR,"net_connect: io$access err 0x!XL (!AS)",iss,s);
        destroy_STRING(s);
        destroy_MBX(mbx);
        free(ndb);
        return 0;
    }

    ndb->unit = mbx->unit;
    ndb->Qin  = MbxQ_new(mbx,MBXQ$_READ,0);
    ndb->Q2perl = MbxQ_new(0, MBXQ$_WRITE|MBXQ$M_RETRY|MBXQ$M_SEGMENT,0);
/*  ndb->Merr = assign_new_MBX(STDERRLOG,0);       */
    ndb->Pout = Pipe_new(0, mbx);
    ndb->Pout->outQ->option |= MBXQ$M_NETMBX;
    MbxQ_setname(ndb->Pout->inQ,"pipeIN");
    MbxQ_setname(ndb->Pout->outQ,"pipeOUT");

    errlog(L_MAIN|L_BABBLE,"net_connect connected NDB @ 0x!XL on chan !UW, unit:!UW",ndb,mbx->chan, mbx->unit);

    iss = pthread_mutex_init(&ndb->mutex,pthread_mutexattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("net_connect, mutex_init");
    iss = pthread_cond_init(&ndb->cond,pthread_condattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("net_connect, cond_init");

    iss = pthread_mutex_lock(&ndb->mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("net_connect, ndb->mutex lock");

    iss = pthread_mutex_lock(&NDB_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("net_connect, NDB_mutex lock");
    ndb->next = NDB_Head;
    NDB_Head = ndb;
    ndb->connum = connect_number++;
    iss = pthread_mutex_unlock(&NDB_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("net_connect, mutex_unlock");

#ifdef PRINT_QUOTA
    quota -= get_bytcnt(pid);
    errlog(L_INFO,"net_connect consumed !SL bytes of quota",quota);
#endif

    return ndb;
}




void
net_readENV()
{
    ROUTINE_NAME("net_readENV");
    errlog(L_MAIN|L_BABBLE,"net_readENV()");

    new_ENV();
    get_main_params();
    get_id2_params();
    get_path_params();
    get_header_params();
    get_input();
}

void
clean_TCB(pTCB tc)
{
    ROUTINE_NAME("clean_TCB");
    if (!tc) return;
    errlog(L_MAIN|L_BABBLE,"clean_TCB: tc !XL unhooked from ndb !XL",tc,tc->ndb);
    if (tc->ndb)
       tc->ndb->tc = 0;
    tc->ndb = 0;
    if (tc->ENVBase) destroy_ENV(tc->ENVBase);
    tc->ENVBase = 0;
    if (tc->content) free(tc->content);
    tc->content = 0;
    tc->content_length = 0;
}

pTCB
destroy_TCB(pTCB tc)
{
    ROUTINE_NAME("destroy_TCB");
    if (!tc) return 0;
    clean_TCB(tc);
    free(tc);
    return 0;
}

pTCB
new_TCB(int ThreadID)
{
    ROUTINE_NAME("new_TCB");
    int iss;
    pTCB tc;

    tc = (pTCB) malloc(sizeof(TCB));
    if (!tc) VMS_ABORT(SS$_INSFMEM,"new_TCB, malloc");

    tc->id                  = ThreadID;
    tc->thread              = 0;
    tc->ENVBase             = 0;
    tc->content             = 0;
    tc->content_length      = 0;
    tc->ndb                 = 0;
    return tc;
}


void
destroy_ENV(pENV e)
{
    ROUTINE_NAME("destroy_ENV");
    pENV p, qp = e;

    while (p=qp) {
        qp = p->next;
        if (p->name) free(p->name);
        if (p->value) free(p->value);
        free(p);
    }
}

pENV *
get_ENVBase()
{
    ROUTINE_NAME("get_ENVBase");
    int iss;
    pTCB tc = get_current_TCB();


    return &tc->ENVBase;
}

void
new_ENV(void)
{
    ROUTINE_NAME("new_ENV");
    int j;
    static char *default_blank[] = {
        "SUBFUNCTION",
        "SERVER_SOFTWARE", "SERVER_NAME", "SERVER_PROTOCOL",
        "SERVER_PORT", "REQUEST_METHOD", "PATH_INFO",
        "PATH_TRANSLATED", "SCRIPT_NAME", "SCRIPT_PATH",
        "QUERY_STRING", "REMOTE_USER", "REMOTE_ADDR", "REMOTE_PORT",
        "REMOTE_HOST", "AUTH_TYPE", "REMOTE_IDENT", "CONTENT_TYPE",
        "CONTENT_LENGTH", NULL};

    pENV *ENVBase = get_ENVBase();

    for (j = 0; default_blank[j] != NULL; j++) {
        add_ENV(default_blank[j],"",0);
    }

    add_ENV("GATEWAY_INTERFACE", "CGI/1.0", 1);
}

pENV
add_ENV(char *name, char *value, int state)
{
    ROUTINE_NAME("add_ENV");
    char *qp;
    pENV p, *ENVBase;


    if (p = find_ENV(name)) {
        qp = p->value;
        p->value = malloc(tu_strlen(value)+1);
        if (!p->value) {
            if (qp) free(qp);
            return 0;
        }
        tu_strcpy(p->value,value);
        if (qp) free(qp);
    } else {
        p = (pENV) malloc(sizeof(ENV));
        if (!p) return 0;
        p->name = malloc(tu_strlen(name)+1);
        if (!p->name) {
            free(p);
            return 0;
        }
        tu_strcpy(p->name, name);

        p->value = malloc(tu_strlen(value)+1);
        if (!p->value) {
            if (p->name) free(p->name);
            free(p);
            return 0;
        }
        tu_strcpy(p->value,value);
        ENVBase = get_ENVBase();
        p->next = *ENVBase;
        *ENVBase = p;
    }
    p->state = state;
    return p;
}

pENV
find_ENV(char *name)
{
    ROUTINE_NAME("find_ENV");
    pENV p = *get_ENVBase();

    while (p) {
        if (tu_strcmp(p->name,name) == 0) return p;
        p = p->next;
    }
    return 0;
}

void
get_main_params(void)
{
    ROUTINE_NAME("get_main_params");
    pSTRING s1, s2, s3, s4;

    errlog(L_MAIN|L_BABBLE,"get_main_params()");
    s1 = fetch_value(0);
    s2 = fetch_value(0);
    s3 = fetch_value(0);
    s4 = fetch_value(0);
    add_ENV("SUBFUNCTION",s1->dsc$a_pointer,1);
    add_ENV("REQUEST_METHOD",s2->dsc$a_pointer,1);
    add_ENV("SERVER_PROTOCOL",s3->dsc$a_pointer,1);
    add_ENV("SCRIPT_NAME",s4->dsc$a_pointer,1);
    errlog(L_MAIN|L_BABBLE,"SUBFUNCTION='!AS'",s1);
    errlog(L_MAIN|L_BABBLE,"REQUEST_MET='!AS'",s2);
    errlog(L_MAIN|L_BABBLE,"SERVER_PROT='!AS'",s3);
    errlog(L_MAIN|L_BABBLE,"SCRIPT_NAME='!AS'",s4);
    destroy_STRING(s1);
    destroy_STRING(s2);
    destroy_STRING(s3);
    destroy_STRING(s4);
}


void
get_id2_params(void)
{
    ROUTINE_NAME("get_id2_params");
    char *value, *p, temp[16];
    pSTRING s;
    int addr;
    pENV e;

    errlog(L_MAIN|L_BABBLE,"get_id2_params()");
    s = fetch_value("<DNETID2>");
    if (!s) return;
    errlog(L_MAIN|L_BABBLE,"id2 params = '!AS'",s);
    value = s->dsc$a_pointer;
    if (value)
        add_ENV("SERVER_SOFTWARE",pop_token(&value),1);
    if (value)
        add_ENV("SERVER_NAME",pop_token(&value),1);
    if (value)
        add_ENV("SERVER_PORT",pop_token(&value),1);
    if (value)
        add_ENV("REMOTE_PORT",pop_token(&value),1);
    if (value) {
        addr = atoi (pop_token(&value));
        sprintf (temp, "%d.%d.%d.%d", (addr&255),
            ((addr>>8)&255), ((addr>>16)&255), ((addr>>24)&255) );
        add_ENV("REMOTE_ADDR",temp,1);
    }
    if (value) {
        /*
         * Only set value if string is non-null.
         */
        p = pop_token(&value);
        if (*p != ' ') add_ENV("REMOTE_USER",p,1);
    }
    if (value)
        add_ENV("REMOTE_HOST",pop_token(&value),1);

    /* Fallback remote host to REMOTE_ADDR */

    e = find_ENV("REMOTE_ADDR");
    if (e->state)
        add_ENV("REMOTE_HOST",e->value,1);

    destroy_STRING(s);
}

void
get_path_params(void)
{
    ROUTINE_NAME("get_path_params");
    int iss, length;
    char *value, *qp;
    pENV e;
    pSTRING s, s2 = 0;

    errlog(L_MAIN|L_BABBLE,"get_path_params()");
    /*
     * Ask for name of script being executed and derive path_info and
     * script name from that.
     */
    s = fetch_value ("<DNETPATH>");
    if (s) {
        errlog(L_MAIN|L_BABBLE,"<DNETPATH> -> '!AS'",s);
        value = s->dsc$a_pointer;
        length= s->dsc$w_length;
        add_ENV("SCRIPT_PATH",value,1);
        e = find_ENV("SCRIPT_NAME");
        if (e) {
            if (tu_strncmp(e->value,value,length)==0) {
                if (length > 0) {           /* stuff after script name */
                    for (qp = &e->value[length]; *qp && (*qp != '/'); qp++);
                    add_ENV("PATH_INFO",qp,1);
                } else {
                    /*
                     * Get original URL to bypass server translation done on arg2
                     * Fixup path_info: strip search arg and unescape.
                     */
                    int qlen;

                    s2 = fetch_value("<DNETRQURL>");
                    qp = s2->dsc$a_pointer;
                    for (qlen = 0; qp[qlen]; qlen++) {
                        if (qp[qlen] == '?') {
                            qp[qlen] = '\0';             /* hack off at ? */
                            break;
                        }
                    }
                    net_unescape_string(qp,&qlen);
                    qp[qlen] = '\0';
                }
                load_translation ( "PATH_TRANSLATED", "<DNETXLATE>", qp );
            }
            if (s2) destroy_STRING(s2);

            if ( tu_strlen(e->value) > length ) {
                for ( qp = &e->value[length]; *qp && (*qp != '/'); qp++ );
                *qp = '\0';              /* Truncate script name */
                add_ENV("SCRIPT_NAME",e->value,1);
            }

            if (tu_strncmp(e->value,value,length) == 0) {
                add_ENV("SCRIPT_BARE_NAME",e->value+length,1);
            } else {
                add_ENV("SCRIPT_BARE_NAME",e->value,1);
            }
        }
        destroy_STRING(s);
    }

    s = fetch_value("<DNETBINDIR>");
    if (s) {
        add_ENV("SCRIPT_BINDIR",s->dsc$a_pointer,1);
        destroy_STRING(s);
    }
    /*
     * Get query string and convert escaped characters.
     */

    s = fetch_value ("<DNETARG>");
    if (s) {
        length = s->dsc$w_length;
        value  = s->dsc$a_pointer;
        if (length > 0) {
            length--; value++;              /* remove leading '?' */
#ifdef DECODE_QUERY
            net_unescape_string ( value, &length );
#endif
            value[length] = '\0';
            add_ENV("QUERY_STRING",value,1);
        }
        destroy_STRING(s);
    }
}


void
get_header_params(void)
{
    ROUTINE_NAME("get_header_params");
    int iss, length;
    char buffer[4096], *label, *value, *qp;
    pENV e;
    pTCB tc = get_current_TCB();

    errlog(L_MAIN|L_BABBLE,"get_header_params()");
    /*
     * Get header lines from HTTP server and save in environment array.
     */
    netwrite("<DNETHDR>");

    length = netread(buffer,sizeof(buffer));
    while (length > 0) {
        buffer[length] = '\0';
        if (buffer[length-1] == '\n') {
            buffer[--length] = '\0';        /* trailing \n */
        }

        /* Parse out header label */

        value = tu_strchr(buffer,':');
        if (value) {
            *value++ = '\0';
            /*
             * Construct label.  Upcase characters and convert '-' to '_'.
             */
            label = malloc(tu_strlen(buffer)+6);
            if (!label) VMS_ABORT(SS$_INSFMEM,"");
            tu_strcpy(label,"HTTP_");
            for (qp = buffer; *qp; qp++) {
                *qp = _toupper(*qp);
                if (*qp == '-') *qp = '_';
            }
            tu_strcat(label,buffer);
            for (qp = value; *qp && isspace(*qp); qp++);
            value = qp;

            /*
             * Check for special header lines that go into pre-defined
             * variables.
             */
            if ( 0 == tu_strcmp(label,"HTTP_CONTENT_LENGTH") ) {
                add_ENV("CONTENT_LENGTH",value,1);
            } else if ( 0 == tu_strcmp (label,"HTTP_CONTENT_TYPE") ) {
                add_ENV("CONTENT_TYPE",value,1);
            } else if (e = find_ENV(label)) {
                /*
                 * Append string to existing.  Allocate extra.
                 */
                char *cat_value;
                cat_value = malloc ( tu_strlen(e->value) + tu_strlen(value) + 3 );
                if (!cat_value) VMS_ABORT(SS$_INSFMEM,"");
                tu_strcpy ( cat_value, e->value );
                tu_strcat ( cat_value, ", ");
                tu_strcat ( cat_value, value);
                add_ENV(label,cat_value,1);
                free (cat_value);
            } else {
                add_ENV(label,value,1);
            }
            free(label);
        } else {
           /* Continuation header line, handle later ??? */
        }
        length = netread(buffer,sizeof(buffer));
    }
}

void
get_input(void)
{
    ROUTINE_NAME("get_input");
    int remain, length, content_length;
    char *p, *s;
    pENV e;
    pTCB tc = get_current_TCB();

    errlog(L_MAIN|L_BABBLE,"get_input()");
    if (tc->content) free(tc->content);
    tc->content = 0;
    tc->content_length = 0;
    e = find_ENV("CONTENT_LENGTH");
    if (!e) return;

    remain = content_length = atoi (e->value);
    if (content_length <= 0) return;
    tc->content_length = content_length;

    p = s = malloc(content_length+2);

    while (remain > 0) {
        netwrite("<DNETINPUT>");
        length = netread(p,remain+2);
        if (length <= 0) break;
        p += length;
        remain -= length;
    }
    tc->content = s;
}

pTCB
get_current_TCB()
{
    ROUTINE_NAME("get_current_TCB");
    int iss;
    pTCB tc;

    iss = pthread_getspecific(TCBkey, (void *)&tc);
    if (UNIX_ERR(iss)) UNIX_ABORT("get_current_TCB, getspecific");
    return tc;
}

int
netread(char *s, int lmax)
{
    ROUTINE_NAME("netread");
    int iss, count;
    pTCB tc    = get_current_TCB();
    pMbxQ q    = tc->ndb->Qin;
    pMbxE e;

    e = MbxQ_read2(q, NET_TIMEOUT);
    if (!e) VMS_ABORT(SS$_TIMEOUT,"netread timeout");

    iss = e->iosb.status;
    if (VMS_ERR(iss)) VMS_ABORT(iss,"netread, readvblk iosb");

    count = e->iosb.count;
    if (count > lmax) VMS_ABORT(SS$_ABORT,"netread, buffer too small");
    memcpy(s, e->buf, count);

    logdump(L_NETIO|L_DEBUG,"netread ",e->buf, count > 64 ? 64 : count);
    MbxQ_dispose(e);

    return count;

}


void
netwrite2(pMbxQ q, char *s)
{
    ROUTINE_NAME("netwrite2");
    int iss, l = tu_strlen(s);

    logdump(L_NETIO|L_DEBUG,"netwrite",s, l>64?64:l);
    iss = MbxQ_write(q, s, l);
    if (VMS_ERR(iss)) VMS_ABORT(iss,"netwrite2");

}

void
netwrite(char *s)
{
    ROUTINE_NAME("netwrite");
    pTCB  tc = get_current_TCB();

    netwrite2(tc->ndb->Pout->outQ, s);
}

pSTRING
fetch_value(char *tag)
{
    ROUTINE_NAME("fetch_value");
    int len;
    char buffer[4096];
    pSTRING s;

    if (tag && tu_strlen(tag) > 0) netwrite(tag);
    len = netread(buffer,4096);

    if ( len > 0 ) {
        buffer[len] = '\0';
        if (buffer[len-1] == '\n')
            buffer[--len] = '\0';
        s = new_STRING(buffer);
    } else {
        s = new_STRING("");
    }
    return s;
}


char *
pop_token(char **s)
{
    char *p, *qp;

    qp = *s;
    p = tu_strchr(qp,' ');
    if (p) {
        *p++ = '\0';
    }
    *s = p;
    return qp;
}



/**************************************************************************/
/* Convert escaped characters in string to actual values.
 *
 * Arguments:
 *      string          Character string.  Modified.
 *      length          Int.  On input, original length of string.
 *                      On output, final length of unescaped string.
 */
char*
net_unescape_string ( char *string, int *length )
{
    int i, j, reslen, modified;
    /*
     * Scan string.
     */
    for ( modified = reslen = i = 0; i < *length; i++ ) {
        if ( string[i] == '%' ) {
            /*
             * Escape seen, decode next 2 characters as hex and replace
             * all three with single byte.
             */
            char value[4];
            int val;
            value[0] = string[i+1]; value[1] = string[i+2]; value[2] = '\0';
            i += 2;
            sscanf ( value, "%2x", &val );
            if  ( val > 127 ) val |= (-1 ^ 255);        /* Sign extend */
            string[reslen] = val;
            modified = 1;
        }
        else {
            /* Only copy bytes if escape edit took place. */
            if ( modified ) string[reslen] = string[i];
        }
        reslen++;
    }
    /* Return value is point to string editted. */
    *length = reslen;
    return string;
}


void
net_runscript(pProc p)
{
    ROUTINE_NAME("net_runscript");
    send_message(p->commandQ, MSG$_CONNECT, 0, 0, 0);
}


void
load_translation ( char *name, char *tag, char *arg )
{
    ROUTINE_NAME("load_translation");
    char *value;
    pSTRING s = 0;
    pTCB tc = get_current_TCB();

    if (*arg) {
        netwrite(tag);
        s = fetch_value(arg);
        value = s->dsc$a_pointer;
    } else {
        value = arg;
    }
    add_ENV(name,value,1);
    if (s) destroy_STRING(s);
}


void
destroy_TCB2(void *tc)
{
    ROUTINE_NAME("destroy_TCB2");
    destroy_TCB((pTCB) tc);
}

/*  special handling to abort a hanging connection */
void
abort_connection(word unit)
{
    ROUTINE_NAME("abort_connection");
    queue_special(MSG$_DISCON, unit);
}


/*  queue a special abort packet for processing */
void
abort_tentacle(word unit)
{
    ROUTINE_NAME("abort_tentacle");
    queue_special(MSG$_PENDABORT,unit);
}


void
do_pendabort(pMbxE e)
{
    ROUTINE_NAME("do_pendabort");
    pMM m = (pMM) e->buf;
    int iss;
    word unit;
    pNDB ndb;

    unit = m->unit;
    errlog(L_MAIN|L_BABBLE,"do_pendabort, unit=!UW",unit);

    iss = pthread_mutex_lock(&NDB_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_pendabort, mutex_lock");

    ndb = NDB_Head;
    while (ndb) {
        if (ndb->unit == unit) break;
        ndb = ndb->next;
    }

    if (!ndb) {
        errlog(L_MAIN|L_BABBLE,"do_pendabort, didn't find unit !UW",unit);
        iss = pthread_mutex_unlock(&NDB_mutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("do_pendabort, mutex_unlock");
        return;
    }

    errlog(L_MAIN|L_TRACE,"pendaborting connection #!SL",ndb->connum);
    errlog(L_MAIN|L_BABBLE,"do_pendabort, got ndb !XL",ndb);

    iss = pthread_mutex_lock(&ndb->mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_pendabort, mutex_lock");

    if (!ndb->pending_idle && ndb->process && ndb->process->state == PS_BUSY) {
        errlog(L_ERROR,"sending abort message to tentacle process");
        send_message(ndb->process->commandQ, MSG$_ABORT, 0, 0, 0);
    } else {
        errlog(L_WARNING,"do_pendabort prevented from aborting: pending_idle=!SL process=!XL state=!SL",ndb->pending_idle,ndb->process, ndb->process ? ndb->process->state : 0);
    }

    iss = pthread_mutex_unlock(&ndb->mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_pendabort, mutex_unlock");
    iss = pthread_mutex_unlock(&NDB_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_pendabort, mutex_unlock");

}


void
do_disconnect(pMbxE e)
{
    ROUTINE_NAME("do_disconnect");
    pMM m = (pMM) e->buf;
    int iss;
    word unit;
    pNDB ndb;

    unit = m->unit;
    errlog(L_MAIN|L_BABBLE,"do_disconnect, unit=!UW",unit);

    iss = pthread_mutex_lock(&NDB_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_disconnect, mutex_lock");

    ndb = NDB_Head;
    while (ndb) {
        if (ndb->unit == unit) break;
        ndb = ndb->next;
    }

    if (!ndb) {
        errlog(L_ERROR,"do_disconnect, can't find unit !UW",unit);
        iss = pthread_mutex_unlock(&NDB_mutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("do_disconnect, mutex_unlock");
        return;
    }
    errlog(L_MAIN|L_TRACE,"disconnecting ndb !XL con#!SL",ndb, ndb->connum);

    iss = pthread_mutex_lock(&ndb->mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_disconnect, mutex_lock");

    ndb->pending_disconnect = 1;
    if (!ndb->pending_idle) {
        longword delta[2];

        errlog(L_MAIN|L_BABBLE,"do_disconnect, wait for idle");
        queue_ATE(unit,ATE_ABORT,10);

        iss = pthread_mutex_unlock(&ndb->mutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("do_disconnect, mutex_lock");
        iss = pthread_mutex_unlock(&NDB_mutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("do_disconnect, mutex_unlock");
        return;
    } else {
        dequeue_ATE(unit,ATE_DISCON);
    }

    destroy_NDB(ndb);

    iss = pthread_mutex_unlock(&NDB_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("do_disconnect, mutex_unlock");

}


/*
 *  a script we don't need a perl subprocess to deal with...
 */
int
local_script(void)
{
    ROUTINE_NAME("local_script");
    pENV e;

    e = find_ENV("SCRIPT_BARE_NAME");
    if (!e) return 0;

    if (tu_strcmp_uc(e->value, "SHUTDOWN_SERVER") == 0) {
        shutdown_server();
        return 1;
    }

    if (tu_strcmp_uc(e->value, "GET_LOGLEV") == 0) {
        int level;
        char msg[100];

        netwrite("Content-type: text/plain\n\n");
        level = errlog_level(-1);
        sprintf(msg,"Current logging level is %d, options bits = 0x%06x\n",level&L_LEVMASK,level>>L_MINTYPE);
        netwrite(msg);
        netwrite("</DNETCGI>");
        return 1;

    }

    if (tu_strcmp_uc(e->value, "SET_LOGLEV") == 0) {
        int dirty = 0;
        longword level=0;
        long int bit;
        pSTRING tmp;
        pTCB tc = get_current_TCB();
        pENV e0;
        char msg[100];

        netwrite("Content-type: text/plain\n\n");
        tmp = new_STRING(tc->content);
        e = e0 = parse_input(tmp);
        destroy_STRING(tmp);

        while (e) {
            if (tu_strcmp("OPTION",e->name) == 0 && e->value) {
                bit = atoi(e->value);
                if (bit >= L_MINTYPE && bit <= 31) level |= 1<<bit;
                dirty = 1;
            }
            if (tu_strcmp("LEVEL",e->name) == 0 && e->value) {
                bit = atoi(e->value);
                if (bit >= 0 && bit <= L_LEVMASK) level |= bit;
                dirty = 1;
            }
            e = e->next;
        }
        if (e0) destroy_ENV(e0);
        if (dirty) {
            errlog_level(level);
            sprintf(msg,"Changing logging level to %d, options bits = 0x%06x\n",level&L_LEVMASK,level>>L_MINTYPE);
        } else {
            sprintf(msg,"No valid input; logging unchanged\n");
        }
        netwrite(msg);
        if (dirty) broadcast_loglevel(level);
        netwrite("</DNETCGI>");
        return 1;
    }

    if (tu_strcmp_uc(e->value, "NEWLOG") == 0) {
        NewLog();
        netwrite("Content-type: text/plain\n\n");
        netwrite("CRINOIDLOG requested to start new logfile");
        netwrite("</DNETCGI>");
        return 1;
    }
    return 0;
}

pENV
parse_input(pSTRING s)
{
    ROUTINE_NAME("parse_input");
    pENV e0, e;
    char *p, *pend, *q;
    int state, length;

    if (!s) return 0;
    p = s->dsc$a_pointer;
    pend = p + s->dsc$w_length;

    e0 = 0;
    state = 0;

    while (p < pend) {
        for (q = p; q < pend && *q != '&' & *q != '='; q++);
        if (state == 0) {
            e = malloc(sizeof(ENV));
            if (!e) VMS_SIGNAL(SS$_INSFMEM);
            e->next = e0;
            e0 = e;
            length = q-p;
            net_unescape_string(p, &length);
            e->name = malloc(length+1);
            if (!e->name) VMS_SIGNAL(SS$_INSFMEM);
            tu_strnzcpy(e->name, p, length);
            e->value = 0;
            e->state = 0;
            state = (*q == '=')? 1 : 0;
        } else if (state == 1) {
            length = q-p;
            net_unescape_string(p, &length);
            e->value = malloc(length+1);
            if (!e->value) VMS_SIGNAL(SS$_INSFMEM);
            tu_strnzcpy(e->value, p, length);
            state = 0;
        }
        p = q + 1;
    }
    return e0;
}



static void
Exit_handler(void)
{
    exit_handle_Proc();
}




int
do_heartbeat(pMbxE e)
{
    ROUTINE_NAME("do_heartbeat");
    longword pid;
    pProc p;

    pid = e->iosb.dvispec;
    errlog(L_MAIN|L_TRACE,"do_heartbeat (pid=!XL) ",pid);
    p = get_Proc(pid);
    errlog(L_MAIN|L_BABBLE,"do_heartbeat, lookup found p=!XL",p);
    if (!p) return 0;
    send_message(p->commandQ, MSG$_LUBDUB, 0, 0, 0);
    return 1;
}


int
do_stubstart(pMbxE e)
{
    ROUTINE_NAME("do_stubstart");
    pMM m = (pMM) e->buf;
    longword pid;
    pProc p;
    pGroup g;
    pMBX cmbx;
    char outbuf[100];

    pid = e->iosb.dvispec;
    errlog(L_MAIN|L_TRACE,"do_stubstart (pid=!XL) ",pid);
    p = get_Proc(pid);
    errlog(L_MAIN|L_BABBLE,"do_stubstart, lookup found p=!XL",p);
    if (!p) return 0;

    sprintf(outbuf,"_MBA%d:",(longword)m->unit);
    cmbx = assign_new_MBX(outbuf,0);
    p->commandQ = MbxQ_new(cmbx, MBXQ$_WRITE|MBXQ$M_RETRY, 0);

    g = p->group;
/* proc name still a question ... */

    send_message(p->commandQ, MSG$_USERNAME , 0, asciz_pSTRING(g->username), strlen_pSTRING(g->username));
    send_message(p->commandQ, MSG$_PROCPRIVS, 0, (char *)&g->priv, sizeof(g->priv));
    send_message(p->commandQ, MSG$_PROCQUOTA, 0, (char *)&g->quota, sizeof(g->quota));
    send_message(p->commandQ, MSG$_PROCFLAGS, 0, (char *)&g->stsflg, sizeof(g->stsflg));
    send_message(p->commandQ, MSG$_PROCPROG , 0, asciz_pSTRING(g->program), strlen_pSTRING(g->program));
    send_message(p->commandQ, MSG$_PROCPRIOR, 0, (char *)&g->baspri, sizeof(g->baspri));
    send_message(p->commandQ, MSG$_PROCTMBX , 0, (char *)&TermQ->mbx->unit, sizeof(word));
    send_message(p->commandQ, MSG$_PROCSTART, 0, 0, 0);

    return SS$_NORMAL;
}

int
do_startpid(pMbxE e)
{
    ROUTINE_NAME("do_startpid");
    pMM m = (pMM) e->buf;
    longword pid;
    pProc p;

    pid = e->iosb.dvispec;
    errlog(L_MAIN|L_INFO,"do_startpid (pid=!XL) ",pid);
    p = get_Proc(pid);
    errlog(L_MAIN|L_BABBLE,"do_startpid, lookup found p=!XL",p);
    if (!p) return 0;

    memcpy(&p->pid, m->info, e->iosb.count - 4);

    errlog(L_MAIN|L_BABBLE,"do_startpid new process pid = !XL",p->pid);
    {
        pMBX mc = p->commandQ->mbx;
        p->commandQ = MbxQ_destroy(p->commandQ);
        destroy_MBX(mc);
    }
    return SS$_NORMAL;
}


void
shutdown_server(void)
{
    ROUTINE_NAME("shutdown_server");
    int j, n, nalive, tries;
    longword * PIDList = 0;
    pProc p;
    pGroup g;
    char text[100];

    netwrite("Content-type: text/plain\n\n");
    netwrite("server shutting down...\n");

    PIDList = list_Proc();
    if (PIDList) {
        for (j = nalive = n = 0; PIDList[j] != 0; j++, n++) {
            p = get_Proc(PIDList[j]);
            if (p) {
                g = p->group;

                lock_Group(g);
                g->n_max = g->n_min = g->n_pad_max = g->n_pad_min = 0;
                unlock_Group(g);

                sprintf(text, "sending shutdown message to process PID: %08X (group: %s)\n",
                    p->pid, asciz_pSTRING(g->name));
                netwrite(text);
                send_message(p->commandQ, MSG$_NETSHUT, 0, 0, 0);
                nalive++;
            } else
                PIDList[j] = 0;
        }


        tries = 0;
        while (nalive && tries < 10) {
            tries++;
            for (j = nalive = 0; j < n; j++) {
                if (PIDList[j]) {
                    p = get_Proc(PIDList[j]);
                    if (p)
                        nalive++;
                    else {
                        sprintf(text,"Process PID: %08X exited\n",PIDList[j]);
                        netwrite(text);
                        PIDList[j] = 0;
                    }
                }
            }
            if (nalive > 0) delay(1);
        }

        if (nalive > 0) {
            netwrite("some tentacle processes still active, shutting down anyway\n");
        }
    }
    netwrite("Tentacle processes shutdown complete...now shutting down main server\n");
    Main_Shutdown = 1;
    netwrite("</DNETCGI>");
}


pGroup
net_select_group()
{
    ROUTINE_NAME("net_select_group");
    char *script;
    int madescript = 0;
    pENV e,e2,eh;
    pGroup g = 0;
    pScript s;
    pSTRING user = 0, user_group = 0;
    pSTRING logdir = 0, logx = 0, bindir = 0, remain = 0;

    e = find_ENV("SCRIPT_NAME");
    errlog(L_MAIN|L_TRACE,"net_select_group, SCRIPT_NAME = '!AZ'",(e->value ? e->value : "") );
    if (!e) goto done;
    if (!e->value) goto done;
    script = e->value;
    e2 = find_ENV("SCRIPT_BARE_NAME");
    if (e2) {
        script = malloc(strlen(e->value)+strlen(e2->value) + 2);
        tu_strcpy(script,e->value);
        tu_strcat(script,"/");
        tu_strcat(script,e2->value);
        madescript = 1;
    }

    eh = find_ENV("SERVER_NAME");

    s = findwild_Script(script, eh ? eh->value : 0);
    errlog(L_MAIN|L_BABBLE,"net_select_group,findwild returned s=0x!XL",s);
    if (!s) goto done;
    switch (s->type) {
        case SCRIPT$C_DEFAULTUSER:
            user = extract_user(script, asciz_pSTRING(s->wild));
            remain = url_remainder(script, asciz_pSTRING(s->wild), 1);
            if (user) {
                user_group = new_STRING2(strlen_pSTRING(user) + 1);
                tu_strcpy(asciz_pSTRING(user_group),"~");
                tu_strcat(asciz_pSTRING(user_group),asciz_pSTRING(user));

                g = find_named_Group(asciz_pSTRING(user_group));
                if (g) {
                    bindir = g->bindir;
                } else {
                    char *log2, userbin[256];
                    pGroup g2 = find_named_Group("~");

                    logdir = get_userlogin(asciz_pSTRING(user));            /* user login directory */
                    if (!logdir) goto done;
                    pthread_lock_global_np();
                    log2 = decc$translate_vms(asciz_pSTRING(logdir));       /* translate to unixoid */
                    if (log2 && log2 != (char *)-1)
                        logx = new_STRING(log2);
                    pthread_unlock_global_np();
                    if (!logx) goto done;
                    tu_strcpy(userbin, asciz_pSTRING(logx));
                    if (g2 && g2->usergroup && g2->bindir) {                /* append bindir */
                        tu_strcat(userbin, asciz_pSTRING(g2->bindir));
                    } else {
#ifdef ALLOWED_DEFAULT_USERGROUP
                        tu_strcat(userbin, user_cgi_dir);
#else
                        goto done;
#endif
                    }

                    g = new_Group(asciz_pSTRING(user_group));
                    setuser_Group(g,asciz_pSTRING(user));
                    g->bindir = new_STRING(userbin);
                    bindir = g->bindir;
                    if (g2) {
                        g->priv       = g2->priv;
                        memcpy(g->quota, g2->quota, sizeof(g2->quota));
                        g->baspri     = g2->baspri;
                        g->n_max      = g2->n_max;
                        g->n_min      = g2->n_min;
                        g->n_pad_max  = g2->n_pad_max;
                        g->n_pad_min  = g2->n_pad_min;
                    }
                    link_Group(g);
                }
            }
        case SCRIPT$C_SPECIFICUSER:
            if (!g && s->user)  {
                g = find_named_Group(asciz_pSTRING(s->user));
                bindir = s->bindir;
                remain = url_remainder(script, asciz_pSTRING(s->wild), 0);
            }
            if (!g) break;

            if (bindir) {
                e2 = find_ENV("SCRIPT_BINDIR");
                if (!e2) {
                    add_ENV("SCRIPT_BINDIR",asciz_pSTRING(bindir),1);
                } else {
                    free(e2->value);
                    e2->value = malloc(strlen_pSTRING(bindir)+1);
                    tu_strcpy(e2->value, asciz_pSTRING(bindir));
                }
                movedown_level();
            }

            if (remain) {
                e2 = find_ENV("SCRIPT_PREFIX");
                if (!e2) {
                    add_ENV("SCRIPT_PREFIX",asciz_pSTRING(remain),1);
                } else {
                    free(e2->value);
                    e2->value = malloc(strlen_pSTRING(remain)+1);
                    tu_strcpy(e2->value, asciz_pSTRING(remain));
                }
            }

            break;

        case SCRIPT$C_DEBUGPREFIX:  /* not implemented yet */
            break;

        case SCRIPT$C_SERVERMAINT:
            g = LocalScriptGroup;
            break;
    }

    if (s->flags) {
        char buf[32];
        sprintf(buf,"%d",s->flags);
        add_ENV("CRINOID:DEBUGFLAGS",buf,1);
    }
    if (s->permit) add_ENV("CRINOID:ALLOW", s->permit, 1);
    if (s->deny)   add_ENV("CRINOID:DENY",   s->deny, 1);
    if (s->warn)   add_ENV("CRINOID:WARN",   "1", 1);

done:
    if (user)         destroy_STRING(user);
    if (user_group)   destroy_STRING(user_group);
    if (logdir)       destroy_STRING(logdir);
    if (logx)         destroy_STRING(logx);
    if (remain)       destroy_STRING(remain);
    if (madescript)   free(script);
    return g;

}



pSTRING
extract_user(char *input, char *prefix)
{
    ROUTINE_NAME("extract_user");
    pSTRING out = 0;
    char c1, c2, *p;

    while (*input && *prefix) {
        c1 = *input;
        c2 = *prefix;
        c1 = toupper(c1);
        c2 = toupper(c2);
        if (c1 != c2) break;
        input++;
        prefix++;
    }

    p = tu_strchr(input,'/');
    if (!p)
        p = input + tu_strlen(input);

    out = new_STRING2(p-input);
    memcpy(asciz_pSTRING(out),input,p-input);
    uc(asciz_pSTRING(out));
    return out;
}

/*       adjust SCRIPT_NAME, SCRIPT_PATH, SCRIPT_BARENAME, PATH_INFO
         for "moving down one level"

        needed for   .../~user/... type URLs

        SCRIPT_NAME = (SCRIPT_PATH) + (SCRIPT_BARE_NAME)
        URL =         (SCRIPT_NAME) + (PATH_INFO)

        Note that Oyster will adjust when it finds the actual script
*/

void
movedown_level(void)
{
    ROUTINE_NAME("movedown_level");
    pENV eSN, eSP, eSBN, ePI;
    char *p, *q;

    eSN = find_ENV("SCRIPT_NAME");          /* this should always exist */
    eSP = find_ENV("SCRIPT_PATH");          /* this should always exist */
    ePI = find_ENV("PATH_INFO");            /* this should always exist */
    eSBN= find_ENV("SCRIPT_BARE_NAME");     /* this might not exist */

    if (!eSBN) {
        add_ENV("SCRIPT_BARE_NAME","",1);
        eSBN= find_ENV("SCRIPT_BARE_NAME");
    }

    q = malloc(tu_strlen(eSN->value) + tu_strlen(ePI->value) + 1);
    tu_strcpy(q, eSN->value);
    p = q + tu_strlen(q);
    tu_strcat(q, ePI->value);
    if (*p == '/') p++;
    p = tu_strchr(p, '/');
    if (!p) p = q + tu_strlen(q);
    eSN->value = realloc(eSN->value, p-q+1);
    tu_strncpy(eSN->value, q, p-q);
    eSN->value[p-q] = 0;
    ePI->value = realloc(ePI->value, tu_strlen(p)+1);
    tu_strcpy(ePI->value, p);
    *p = 0;
    p = tu_strrchr(q,'/');
    if (!p) p = q;
    if (*p == '/') p++;
    eSBN->value = realloc(eSBN->value, tu_strlen(p)+1);
    tu_strcpy(eSBN->value, p);
    *p = 0;
    eSP->value = realloc(eSP->value, tu_strlen(q)+1);
    tu_strcpy(eSP->value, q);

    free(q);
}


void
broadcast_loglevel(int level)
{
    ROUTINE_NAME("broadcast_loglevel");
    int j;
    longword * PIDList = 0;
    pProc p;
    pGroup g;
    char text[100];

    PIDList = list_Proc();
    if (PIDList) {
        for (j = 0; PIDList[j] != 0; j++) {
            p = get_Proc(PIDList[j]);
            g = p->group;
            if (p) {
                sprintf(text, "sending LOGLEV message to process PID: %08X (group: %s)\n",
                    p->pid, asciz_pSTRING(g->name));
                netwrite(text);
                send_message(p->commandQ, MSG$_LOGLEV, 0, &level, sizeof(level));
            }
        }
    }
}

static int sublock_initted = 0;
static LSB logger_main_lsb;
static LSB logger_sub_lsb;
static     $DESCRIPTOR(d_lock,LOG_MAIN_LOCK);
static     $DESCRIPTOR(d_sublock,LOG_SUB_LOCK);

void
StartLogger(void)
{
    ROUTINE_NAME("StartLogger");
    int iss;

    StartLogger_queue_lock();
}

void
StartLogger_queue_lock(void)
{
    ROUTINE_NAME("StartLogger_queue_lock");
    int iss;
    pPrivs p0, p1;

    p0 = Current_Privs();
    if (!p0->prv$v_syslck) {
        p1 = new_Privs();
        p1->prv$v_syslck = 1;
        iss = Set_Privs(p1);
        if (VMS_ERR(iss)) lib$signal(iss);
    }

    iss = sys$enq(0,LCK$K_PWMODE,&logger_main_lsb,LCK$M_SYSTEM|LCK$M_NODLCKWT,&d_lock,0,&StartLogger_AST,0,0,0,0);
    if (VMS_ERR(iss)) lib$signal(iss);

    if (!p0->prv$v_syslck) {
        iss = Reset_Privs(p1);
        if (VMS_ERR(iss)) lib$signal(iss);
        destroy_Privs(p1);
    }
    destroy_Privs(p0);
}

void
StartLogger_AST(int flag)
{
    ROUTINE_NAME("StartLogger_AST");
    int iss;

    if (flag == 0) {    /* logger exited, try restarting */
        if (! sublock_initted) {
            pPrivs p0, p1;

            p0 = Current_Privs();
            if (!p0->prv$v_syslck) {
                p1 = new_Privs();
                p1->prv$v_syslck = 1;
                iss = Set_Privs(p1);
                if (VMS_ERR(iss)) lib$signal(iss);
            }

            iss = sys$enqw(0,LCK$K_EXMODE,&logger_sub_lsb,LCK$M_SYSTEM|LCK$M_NODLCKBLK,
                 &d_sublock,logger_main_lsb.id,0,1,&StartLogger_AST,0,0);
            if (VMS_ERR(iss)) lib$signal(iss);

            if (!p0->prv$v_syslck) {
                iss = Reset_Privs(p1);
                if (VMS_ERR(iss)) lib$signal(iss);
                destroy_Privs(p1);
            }
            destroy_Privs(p0);
            sublock_initted = 1;
        } else {
            iss = sys$enqw(0,LCK$K_EXMODE,&logger_sub_lsb,LCK$M_CONVERT|LCK$M_NODLCKBLK,
                 0,0,0,1,&StartLogger_AST,0,0);
            if (VMS_ERR(iss)) lib$signal(iss);
        }

        iss = sys$enqw(0,LCK$K_PRMODE,&logger_main_lsb,LCK$M_CONVERT,0,0,0,0,0,0,0);
        if (VMS_ERR(iss)) lib$signal(iss);

        queue_special(MSG$_LOGSTART,0);

    } else {            /* logger restarted, release locks */

        iss = sys$enq(0,LCK$K_NLMODE,&logger_main_lsb,LCK$M_CONVERT,0,0,0,0,0,0,0);
        if (VMS_ERR(iss)) lib$signal(iss);

        iss = sys$enq(0,LCK$K_NLMODE,&logger_sub_lsb,LCK$M_CONVERT,0,0,0,0,0,0,0);
        if (VMS_ERR(iss)) lib$signal(iss);

        queue_special(MSG$_LOGQLOCK,0);
    }
}


void
StartLoggerProc(void)
{
    ROUTINE_NAME("StartLoggerProc");
    longword iss, pid;
    pSTRING prog, loc;
    char program[500];
    $DESCRIPTOR(loginout,"SYS$SYSTEM:LOGINOUT.EXE");
    $DESCRIPTOR(null,"NL:");
    Quota  quota[PQL$_LENGTH];
    pPrivs  privs;

    loc = translate_logical("CRINOID_HOME");
    if (!loc) return;

    tu_strcpy(program,asciz_pSTRING(loc));
    destroy_STRING(loc);
    tu_strcat(program,"LOGGER.COM");
    prog = new_STRING(program);
    privs = new_Privs();
    privs->prv$v_syslck = 1;
    privs->prv$v_sysnam = 1;
    privs->prv$v_tmpmbx = 1;
    quota[0].id = PQL$_LISTEND;

    iss = sys$creprc(&pid,
        &loginout,      /* image  */
        prog,           /* input  */
        &null,          /* output */
        &null,          /* error  */
        &privs,         /* privs  */
        quota,          /* quotas */
        0,              /* name */
        2,              /* base priority */
        0,              /* uic */
        0,              /* term mbx */
        PRC$M_DETACH   /* stsflg */
    );

    if (VMS_ERR(iss)) printf("Error starting LOGGER, status = 0x%08.8x\n",iss);

    destroy_Privs(privs);
    destroy_STRING(prog);
}




void
StopLogger(void)
{
    ROUTINE_NAME("StopLogger");
    pPrivs p0 = Current_Privs();
    pPrivs p;

    if (!p0->prv$v_sysnam) {
        p  = new_Privs();
        p->prv$v_sysnam = 1;
        Set_Privs(p);
    }

    define_logical("CRINOIDLOG_COMMAND","STOP","LNM$SYSTEM");

    if (!p0->prv$v_sysnam) {
        Reset_Privs(p);
        destroy_Privs(p);
    }
    errlog(L_CRITICAL,"Request to stop CRINOIDLOG");
    destroy_Privs(p0);
}


void
NewLog(void)
{
    ROUTINE_NAME("NewLog");
    pPrivs p0 = Current_Privs();
    pPrivs p  = new_Privs();

    if (!p0->prv$v_sysnam) {
        p  = new_Privs();
        p->prv$v_sysnam = 1;
        Set_Privs(p);
    }

    define_logical("CRINOIDLOG_COMMAND","NEWLOG","LNM$SYSTEM");

    if (!p0->prv$v_sysnam) {
        Reset_Privs(p);
        destroy_Privs(p);
    }
    errlog(L_CRITICAL,"CRINOIDLOG_COMMAND: Request to new logfile");

    destroy_Privs(p0);
}


static int hGlobalVerb(pVerb v, char **args, char *errmsg);
static int hGroupVerb(pVerb v, char **args, char *errmsg);

#define V_SERVICE           11
#define V_LOGLEVEL          12
#define V_USERGROUP         14
#define V_THREADS           13

#define V_GROUP             21
#define V_SCRIPT            23
#define V_CONTROL           24
#define V_DEBUG             25
#define V_PROCESSES         26
#define V_PAD               27
#define V_SCRIPTFLAGS       31
#define V_SCRIPTPERMIT      32
#define V_SCRIPTDENY        33
#define V_SCRIPTWARN        34
#define V_SCRIPTLOCALHOST   35
#define V_SCRIPTBINDIR      36

#define MAXBUF 512

int
ReadMainConfig (char *file)
{
    ROUTINE_NAME("ReadMainConfig");
    FILE *fp;
    pVerb v;
    char buffer[MAXBUF+1];
    char errmsg[MAXBUF];
    int n, line;

    v = new_Verb("SERVICE" , V_SERVICE , &hGlobalVerb, 0);
    v = new_Verb("LOGLEVEL", V_LOGLEVEL, &hGlobalVerb, v);
    v = new_Verb("CONTROL",  V_CONTROL,  &hGlobalVerb, v);
    v = new_Verb("DEBUG",  V_DEBUG,  &hGlobalVerb, v);
    v = new_Verb("THREADS",  V_THREADS,  &hGlobalVerb, v);

    v = new_Verb("USERGROUP",  V_USERGROUP , &hGroupVerb, v);
    v = new_Verb("GROUP", V_GROUP, &hGroupVerb, v);
    v = new_Verb(".SCRIPT",  V_SCRIPT,  &hGroupVerb, v);
    v = new_Verb("..FLAGS",  V_SCRIPTFLAGS,  &hGroupVerb, v);
    v = new_Verb("..PERMIT",  V_SCRIPTPERMIT,  &hGroupVerb, v);
    v = new_Verb("..DENY",  V_SCRIPTDENY,  &hGroupVerb, v);
    v = new_Verb("..WARN",  V_SCRIPTWARN,  &hGroupVerb, v);
    v = new_Verb("..LOCALHOST",  V_SCRIPTLOCALHOST,  &hGroupVerb, v);
    v = new_Verb("..BINDIR",  V_SCRIPTBINDIR,  &hGroupVerb, v);
    v = new_Verb(".PROCESSES",  V_PROCESSES,  &hGroupVerb, v);
    v = new_Verb(".PAD",  V_PAD,  &hGroupVerb, v);
    v = new_Verb(".IDLE",  V_PAD,  &hGroupVerb, v);

    uniquify_Verb(v);

    fp = fopen(file,"r","shr=get");
    if (!fp) return SS$_ABORT;
    buffer[MAXBUF] = 0;
    line = 0;
    while (fgets(buffer, MAXBUF, fp)) {
        line++;
        errmsg[0] = 0;
        if (!parse_line(v, buffer, errmsg))
            errlog(L_ERROR,"Error (!AZ) parsing config file, line !SL",errmsg,line);
    }
    fclose(fp);
    destroy_Verb(v);
}


static int
hGlobalVerb(pVerb v, char **args, char *errmsg)
{
    ROUTINE_NAME("hGlobalVerb");
    char *p;
    int status = 0;
    int j;

    switch (v->id) {
        case V_SERVICE:       /* SERVICE service_name */
            if (!args[1]) {
                tu_strcpy(errmsg,"SERVICE missing parameter");
                break;
            }
            if (Decnet_Service_Name) destroy_STRING(Decnet_Service_Name);
            Decnet_Service_Name = new_STRING(args[1]);
            status = 1;
            break;

        case V_LOGLEVEL:        /* LOGLEVEL level */
            if (!args[1]) {
                tu_strcpy(errmsg,"LOGLEVEL missing parameter");
                break;
            }
            Loglevel = atoi(args[1]);
            status = 1;
            break;

        case V_CONTROL:        /* CONTROL url */
            if (!args[1]) {
                tu_strcpy(errmsg,"CONTROL missing parameter");
                break;
            }
            new_Script(args[1], SCRIPT$C_SERVERMAINT,"HTTP");
            status = 1;
            break;

        case V_THREADS: /* THREADS nthreads */
            if (!args[1]) {
                tu_strcpy(errmsg,"THREADS missing parameter");
                break;
            }
            j = atoi(args[1]);
            if (j < 1) {
                tu_strcpy(errmsg,"THREADS must be >0");
                break;
            }
            MAX_THREADS = j;
            status = 1;
            break;

        case V_DEBUG:   /* DEBUG url */
            if (!args[1]) {
                tu_strcpy(errmsg,"DEBUG missing parameter");
                break;
            }
            new_Script(args[1], SCRIPT$C_DEBUGPREFIX,"default");
            status = 1;
            break;
        default:
            tu_strcpy(errmsg, "Unknown Verb id");
    }
    return status;
}

#define MAX(a,b)        ((a)>(b)?(a):(b))
#define MIN(a,b)        ((a)>(b)?(b):(a))


static int
hGroupVerb(pVerb v, char **args, char *errmsg)
{
    ROUTINE_NAME("hGroupVerb");
    static pGroup g = 0;
    static pScript s = 0;
    int status = 0;
    int j, l;
    char *p;

    switch (v->id) {



        case V_USERGROUP:                /* USERGROUP bindir */
            if (!args[1]) {
                tu_strcpy(errmsg,"USERGROUP missing bindir paramter");
                break;
            }
            if (*args[1] != '/') {
                tu_strcpy(errmsg,"USERDIR bindir should be unixoid: /...");
                break;
            }
            g = new_Group("~");
            if (!g) {
                sprintf(errmsg,"error creating USERGROUP %s",args[1]);
                break;
            }
            setuser_Group(g, "~");
            g->usergroup = 1;

            l = strlen(args[1]);         /* truncate trailing slash */
            if (l && args[1][l-1] == '/') args[1][l-1] = '\0';
            g->bindir = new_STRING(args[1]);

            link_Group(g);
            status = 1;
            break;

        case V_GROUP:                    /* GROUP groupname username [bindir]*/
            if (!args[1] || !args[2]) {
                tu_strcpy(errmsg,"GROUP missing parameter(s)");
                break;
            }
            if (find_named_Group(args[1])) {
                sprintf(errmsg,"GROUP %s already exists",args[1]);
                break;
            }
            if (args[3] && *args[3] != '/') {
                tu_strcpy(errmsg,"GROUP bindir must be unixoid form: /...");
                break;
            }
            g = new_Group(args[1]);
            if (!g) {
                sprintf(errmsg,"error creating GROUP %s",args[1]);
                break;
            }
            setuser_Group(g, args[2]);
            if (args[3]) {                  /* truncate trailing slash */
                l = strlen(args[3]);
                if (l && args[3][l-1] == '/') args[3][l-1] = '\0';
                g->bindir = new_STRING(args[3]);
            }
            link_Group(g);
            status = 1;
            break;

        case V_SCRIPT:         /* SCRIPT url [group] */
            if (args[2]) g = find_named_Group(args[2]);
            if (!args[1] || !g) {
                tu_strcpy(errmsg,"SCRIPT missing parameter(s)");
                break;
            }
            s = new_Script(args[1],g->usergroup? SCRIPT$C_DEFAULTUSER : SCRIPT$C_SPECIFICUSER,asciz_pSTRING(g->name));
            s->bindir = g->bindir;          /*default bindir*/
            status = 1;
            break;

        case V_SCRIPTBINDIR:    /* BINDIR directory */
            if (!s) {
                tu_strcpy(errmsg,"BINDIR must be preceeded by SCRIPT command");
                break;
            }
            if (!args[1]) {
                tu_strcpy(errmsg,"BINDIR needs directory parameter");
                break;
            }
            if (*args[1] != '/') {
                tu_strcpy(errmsg,"BINDIR directory must be unixoid form: /...");
                break;
            }
            l = strlen(args[1]);
            if (l && args[1][l-1] == '/') args[1][l-1] = '\0';
            s->bindir = new_STRING(args[1]);
            status = 1;
            break;
        case V_SCRIPTFLAGS:
            if (!s) {
                tu_strcpy(errmsg,"SCRIPTFLAGS needs to be preceeded by SCRIPT");
                break;
            }
            if (!args[1]) {
                tu_strcpy(errmsg,"SCRIPTFLAGS missing parameter(s)");
                break;
            }
            s->flags = atoi(args[1]);
            status = 1;
            break;

        case V_SCRIPTPERMIT:
            if (!s) {
                tu_strcpy(errmsg,"SCRIPTPERMIT needs to be preceeded by SCRIPT");
                break;
            }
            if (!args[1]) {
                tu_strcpy(errmsg,"SCRIPTPERMIT missing parameter(s)");
                break;
            }
            p = malloc(strlen(args[1]) + 1);
            tu_strcpy(p, args[1]);
            j = 1;
            while (args[++j]) {
                p = realloc(p,strlen(p)+strlen(args[j])+2);
                tu_strcat(p, " ");
                tu_strcat(p,args[j]);
            }
            s->permit = p;

            status = 1;
            break;

        case V_SCRIPTDENY:
            if (!s) {
                tu_strcpy(errmsg,"SCRIPTDENY needs to be preceeded by SCRIPT");
                break;
            }
            if (!args[1]) {
                s->deny = malloc(1);
                *s->deny = '\0';
                status = 1;
                break;
            }
            p = malloc(strlen(args[1]) + 1);
            tu_strcpy(p, args[1]);
            j = 1;
            while (args[++j]) {
                p = realloc(p,strlen(p)+strlen(args[j])+2);
                tu_strcat(p, " ");
                tu_strcat(p,args[j]);
            }
            s->deny = p;

            status = 1;
            break;

        case V_SCRIPTWARN:
            if (!s) {
                tu_strcpy(errmsg,"SCRIPTWARN needs to be preceeded by SCRIPT");
                break;
            }
            s->warn = 1;
            status = 1;
            break;

        case V_SCRIPTLOCALHOST:
            if (!s) {
                tu_strcpy(errmsg,"LOCALHOST needs to be preceeded by SCRIPT");
                break;
            }
            if (!args[1]) {
                tu_strcpy(errmsg,"LOCALHOST needs to have a 'hostname' parameter");
                break;
            }
            s->server = new_STRING(args[1]);
            uc(asciz_pSTRING(s->server));
            status = 1;
            break;

        case V_PROCESSES:      /* PROCESSES min max [group] */
            if (args[3]) g = find_named_Group(args[3]);
            if (!args[1] || !args[2] || !g) {
                tu_strcpy(errmsg,"MAXMIN missing parameter(s)");
                break;
            }
            {
                int k1, k2;
                k1 = atoi(args[1]);
                k2 = atoi(args[2]);
                g->n_min = MIN(MAX(MIN(k1,k2),0),10);
                g->n_max = MIN(MAX(MAX(k1,k2),1),10);
            }
            status = 1;
            break;

        case V_PAD:       /* PAD min max [group] */
            if (args[3]) g = find_named_Group(args[3]);
            if (!args[1] || !args[2] || !g) {
                tu_strcpy(errmsg,"PAD missing parameter(s)");
                break;
            }
            {
                int k1, k2;
                k1 = atoi(args[1]);
                k2 = atoi(args[2]);
                g->n_pad_min = MIN(MAX(MIN(k1,k2),0),10);
                g->n_pad_max = MIN(MAX(MAX(k1,k2),0),10);
            }
            status = 1;
            break;
        default:
            tu_strcpy(errmsg, "Unknown Verb id");
    }
    return status;
}




void
delay(int secs)
{
    struct timespec t = {0, 0};
    int iss;

    t.tv_sec = secs;
    iss = pthread_delay_np(&t);
    if (UNIX_ERR(iss)) UNIX_ABORT("delay");
}


pNDB
destroy_NDB(pNDB ndb)
{
    ROUTINE_NAME("destroy_NDB");
    int iss;
    pNDB nlast = 0, n = NDB_Head;

    errlog(L_MAIN|L_BABBLE,"destroy_NDB(ndb=!XL)",ndb);
    if (!ndb) return 0;

    while (n) {
        if (n == ndb) break;
        nlast = n;
        n = n->next;
    }


    if (nlast)
        nlast->next = ndb->next;
    else
        NDB_Head = ndb->next;

    if (ndb->process) ndb->process->ndb = 0;
    ndb->process = 0;
/*  if (ndb->Merr)    ndb->Merr   = destroy_MBX(ndb->Merr);     */
    if (ndb->Q2perl)  ndb->Q2perl = MbxQ_destroy(ndb->Q2perl);
    if (ndb->Qin)     ndb->Qin    = MbxQ_destroy(ndb->Qin);
    if (ndb->Pout)    ndb->Pout   = Pipe_destroy(ndb->Pout);

    iss = pthread_mutex_unlock(&ndb->mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("destroy_NDB, mutex_unlock");
    iss = pthread_mutex_destroy(&ndb->mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("destroy_NDB, mutex_destroy");
    iss = pthread_cond_destroy(&ndb->cond);
    if (UNIX_ERR(iss)) UNIX_ABORT("destroy_NDB, cond_destroy");

    iss = pthread_mutex_lock(&ThreadMutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("thread_code, mutex_lock");
    Allowed_Active++;
    iss = pthread_cond_signal(&ThreadMsgReady);
    if (UNIX_ERR(iss)) UNIX_ABORT("thread_handle, cond_signal");
    iss = pthread_mutex_unlock(&ThreadMutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("thread_code, mutex_lock");

#ifdef PRINT_QUOTA
    {
        longword pid = get_pid();
        longword quota = get_bytcnt(pid);
        errlog(L_INFO,"destroy_NDB, quota now !UL bytes",quota);
    }
#endif
    free(ndb);
    return 0;
}



pATE                    Head_ATE = 0;
pthread_mutex_t         ATE_mutex;
pthread_t               ATE_thread;

void
queue_ATE(word unit, int type, int ticks)
{
    ROUTINE_NAME("queue_ATE");
    int iss;
    pATE a = (pATE) malloc(sizeof(ATE));

    if (!a) {
        errlog(L_CRITICAL,"queue_ATE: malloc failed!!");
        return;
    }

    a->type = type;
    a->unit = unit;
    a->ticks = ticks;

    iss = pthread_mutex_lock(&ATE_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("queue_ATE: mutex_lock");

    a->next = Head_ATE;
    Head_ATE = a;

    iss = pthread_mutex_unlock(&ATE_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("queue_ATE: mutex_unlock");

}

void
dequeue_ATE(word unit, int type)
{
    ROUTINE_NAME("dequeue_ATE");
    int iss;
    pATE a, a0;

    iss = pthread_mutex_lock(&ATE_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("queue_ATE: mutex_lock");

    a0 = 0;
    a = Head_ATE;
    while (a) {
        if (a->unit == unit && a->type == type) {
            if (a0 == 0)
                Head_ATE = a->next;
            else
                a0->next = a->next;
            break;
        }
        a0 = a;
        a = a->next;
    }
    if (!a) errlog(L_WARNING,"dequeue_ATE: entry not found unit !UW type !SL (ok, if just aborted)",unit,type);
    if (a) free(a);

    iss = pthread_mutex_unlock(&ATE_mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("queue_ATE: mutex_unlock");

}

void *
ATE_processor(void *pNum)
{
    ROUTINE_NAME("ATE_processor");
    int iss;
    pATE a, a0, a1;

    while (1) {
        iss = pthread_mutex_lock(&ATE_mutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("ATE_processor: mutex_unlock");

        a0 = 0;
        a = Head_ATE;
        while (a) {
            if (--(a->ticks) <= 0) {
                if (a0)
                    a0->next = a->next;
                else
                    Head_ATE = a->next;
                if (a->type == ATE_DISCON) abort_connection(a->unit);
                else if (a->type == ATE_ABORT) abort_tentacle(a->unit);

                a1 = a->next;
                free(a);
                a = a1;
            } else {
                a0 = a;
                a = a->next;
            }
        }

        iss = pthread_mutex_unlock(&ATE_mutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("ATE_processor: mutex_unlock");

        delay(1);
    }
    return 0;
}


void
init_ATE_Proc(void)
{
    ROUTINE_NAME("init_ATE_Proc");
    int foo = 0, iss;

    iss = pthread_mutex_init(&ATE_mutex,pthread_mutexattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("init_ATE_proc, mutex_init");

    iss = pthread_create(&ATE_thread,pthread_attr_default,&ATE_processor,&foo);
    if (UNIX_ERR(iss)) UNIX_ABORT("init_ATE_Proc, pthread_create");
}

void
stop_ATE_Proc(void)
{
    ROUTINE_NAME("stop_ATE_Proc");
    int iss;

    iss = pthread_cancel(ATE_thread);
    if (UNIX_ERR(iss)) UNIX_ABORT("stop_ATE_Proc, pthread_cancel");
}


pSTRING
url_remainder(char *input, char *prefix, int usermode)
{
    ROUTINE_NAME("url_remainder");
    char *url;
    pSTRING out = 0;
    char c1, c2, *p;
    int j = 0;

    url = malloc(tu_strlen(input)+1);
    tu_strcpy(url, input);

    while (*input && *prefix) {
        c1 = *input;
        c2 = *prefix;
        c1 = toupper(c1);
        c2 = toupper(c2);
        if (c1 != c2) break;
        input++;
        prefix++;
        j++;
    }

    if (usermode) {
        while (*input) {
            input++;
            j++;
            if (*input == '/') break;
        }
    }
    url[j] = '\0';
    out =  new_STRING(url);
    free(url);
    return out;
}


void
queue_special(word code, word unit)
{
    ROUTINE_NAME("queue_special");
    int iss;
    pMbxE e;
    pMM   m;

    e = MbxQ_new_entry();
    if (!e) VMS_ABORT(SS$_INSFMEM,"queue_special, MbxQ_new_entry");

    e->iosb.status = SS$_NORMAL;
    e->iosb.count  = 4;

    m = (pMM) e->buf;
    m->code = code;
    m->unit = unit;

    iss = lib$insqti(e, GotMsgQ);
    if (VMS_ERR(iss)) VMS_ABORT(iss,"queue_special, insqti");
    if (Threads_initted) {
        iss = pthread_cond_signal_int_np(&Main_cond);
        if (UNIX_ERR(iss)) UNIX_ABORT("queue_special, cond_signal");
    }
}

