/*
 *
 *  manage the list of worker processes
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <ssdef.h>
#include <starlet.h>
#include <libdef.h>
#include <descrip.h>
#include <psldef.h>
#include <prvdef.h>
#include <pqldef.h>
#include <dvidef.h>
#include <prcdef.h>
#include <LIB$ROUTINES.H>       /* DECC */

#define __PROC_MGR_MAIN
#include "util.h"
#include "proc_mgr.h"
#include "errlog_client.h"

#define VMS_OK(s)    (((s)&1)!=0)
#define VMS_ERR(s)   (((s)&1)==0)
#define UNIX_OK(s)   (s==0)
#define UNIX_ERR(s)  (s!=0)


static pProc                PListHead = 0;
static pthread_mutex_t      ProcListMutex;
static pthread_mutex_t      ProcMgrMutex;
static pthread_cond_t       ProcMgrCond;
static pGroup               GroupHead = 0;
static pthread_mutex_t      GroupMutex;
static pthread_t            ProcMgrThread;
static pMBX                 TerminationMBX;
static pSTRING              GroupProgram;
static pSTRING              GroupInput;
static pSTRING              GroupOutput;
static pSTRING              GroupError;
static pSTRING              StubProg;

#define DEF_MAXPROC_UIC0    5
#define DEF_MINPROC_UIC0    1
#define DEF_MAXPAD_UIC0     2
#define DEF_MINPAD_UIC0     0

#define DEF_MAXPROC_OTHER   3
#define DEF_MINPROC_OTHER   0
#define DEF_MAXPAD_OTHER    1
#define DEF_MINPAD_OTHER    0



void
init_Proc(pMBX term_mbx, char* stub)
{
    ROUTINE_NAME("init_Proc");
    int iss;

    errlog(L_PROC|L_TRACE,"init_Proc(mbx = !AZ)", &term_mbx->d_name);
    iss = pthread_mutex_init(&ProcListMutex, pthread_mutexattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("init_Proc, mutex_init");

    TerminationMBX = term_mbx;
    StubProg = new_STRING(stub);
}

pProc
new_Proc(pGroup g)
{
    ROUTINE_NAME("new_Proc");
    pProc p;
    int iss, len;

    errlog(L_PROC|L_TRACE,"new_Proc(g = 0x!XL)",g);
    len = sizeof(Proc);
    iss = lib$get_vm(&len, &p);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    p->state   = PS_NEW;
    p->group   = g;
    p->pid     = 0;
    p->name    = 0;
    p->commandQ= 0;
    p->maintain_lock  = 0;
    p->ndb = 0;
    p->next_idle = 0;
    p->activations = 0;
    p->killpend = 0;

    iss = pthread_mutex_init(&p->mutex, pthread_mutexattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("new_Proc, mutex_init");

    link_Proc(p);

    errlog(L_PROC|L_TRACE,"new_Proc returns p=0x!XL",p);
    return p;
}


void
start_Proc(pProc p)
{
    ROUTINE_NAME("start_Proc");
    int iss, n;
    pGroup g;
    char name[16];
    $DESCRIPTOR(loginout,"SYS$SYSTEM:LOGINOUT.EXE");
    Privs stubpriv;

    stubpriv.prv$l_l1_bits = 0;
    stubpriv.prv$l_l2_bits = 0;
    stubpriv.prv$v_detach = 1;
    stubpriv.prv$v_sysprv = 1;

    errlog(L_PROC|L_TRACE,"start_Proc(p=0x!XL)",p);
    lock_Proc(p);
    g = p->group;
    lock_Group(g);
    for (n = 0; n < 5; n++) {
        errlog(L_PROC|L_TRACE,"proc(p=0x!XL) ",p);


        iss = sys$creprc(&p->pid,
           &loginout,  /* g->program,   */          /* image  */
           StubProg,   /* g->input,     */          /* input  */
                         g->output,                /* output */
                         g->error,                 /* error  */
           &stubpriv, /* &g->priv,     */          /* privs  */
                         g->quota,                 /* quotas */
           0,         /* p->name,      */          /* name */
                         g->baspri,
           0,         /* g->uic,       */
                         TerminationMBX->unit,
           PRC$M_DETACH   /* g->stsflg */ );

        if (iss != SS$_DUPLNAM) break;
    }
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    g->n_starting++;
    unlock_Group(g);
    p->state = PS_STARTING;
    unlock_Proc(p);
    errlog(L_PROC|L_TRACE,"start_Proc started process, pid=!XL",p->pid);
}

void
link_Proc(pProc p)
{
    ROUTINE_NAME("link_Proc");
    int iss;
    pGroup g;

    errlog(L_PROC|L_TRACE,"link_Proc(p=0x!XL)",p);

    lock_Proc(0);
    if (PListHead) PListHead->last = p;
    p->last = 0;
    p->next = PListHead;
    PListHead = p;
    unlock_Proc(0);

    g = p->group;
    lock_Group(g);
    g->n_total++;
    unlock_Group(g);
}

void
unlink_Proc(pProc p)
{
    ROUTINE_NAME("unlink_Proc");
    int iss;
    pGroup g;
    pProc  pi, plast;

    errlog(L_PROC|L_TRACE,"unlink_Proc(p=0x!XL)",p);

    lock_Proc(0);
    if (p->last)
        p->last->next = p->next;
    else
        PListHead = p->next;

    if (p->next)
        p->next->last = p->last;
    unlock_Proc(0);

    g = p->group;
    lock_Group(g);
    g->n_total--;
    if (p->state == PS_STARTING) g->n_starting--;

    plast = 0;
    pi = g->idle_list;
    while (pi) {
        errlog(L_PROC|L_DEBUG,"unlink_Proc (g=!XL p=!XL) plast=!XL pi=!XL next=!XL",g,p,plast,pi,pi->next_idle);
        if (pi == p) {
            if (plast)
                plast->next_idle = p->next_idle;
            else
                g->idle_list = p->next_idle;
            g->n_idle--;
            check_Proc();
            break;
        }
        plast = pi;
        if (pi == pi->next_idle) {
            errlog(L_CRITICAL,"unlink_Proc: infinite loop detected!!!");
            break;
        }
        pi = pi->next_idle;
    }
    unlock_Group(g);
}


pProc
get_Proc(longword pid)
{
    ROUTINE_NAME("get_Proc");
    int iss;
    pProc p;

    errlog(L_PROC|L_BABBLE,"get_Proc(pid=!XL)",pid);

    lock_Proc(0);
    p = PListHead;

    while (p) {
        errlog(L_PROC|L_BABBLE,"get_proc checking p=0x!XL, pid=!XL",p,p->pid);
        if (p->pid == pid) break;
        p = p->next;
    }
    unlock_Proc(0);

    errlog(L_PROC|L_BABBLE,"returning from get_Proc, p=0x!XL",p);
    return p;
}

void
destroy_Proc(pProc p)
{
    ROUTINE_NAME("destroy_Proc");
    int len, iss;

    errlog(L_PROC|L_TRACE,"destroy_Proc(p=0x!XL)",p);

    unlink_Proc(p);
    if (p->maintain_lock) {
        p->maintain_lock = 0;
        unlock_Proc(p);
    }

    if (p->name) destroy_STRING(p->name);
    iss = pthread_mutex_destroy(&p->mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("destroy_Proc, mutex_destroy");
    len = sizeof(Proc);
    iss = lib$free_vm(&len,&p);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
}

void
lock_Proc(pProc p)
{
    ROUTINE_NAME("lock_Proc");
    int iss;

    errlog(L_LOCKING|L_BABBLE,"lock_Proc(p=0x!XL)",p);
    if (p) {
        if (!p->maintain_lock) {
            iss = pthread_mutex_lock(&p->mutex);
            if (UNIX_ERR(iss)) UNIX_ABORT("lock_Proc");
        }
    } else {
        iss = pthread_mutex_lock(&ProcListMutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("lock_Proc");
    }
}

void
unlock_Proc(pProc p)
{
    ROUTINE_NAME("unlock_Proc");
    int iss;
    errlog(L_LOCKING|L_BABBLE,"unlock_Proc(p=0x!XL)",p);

    if (p) {
        if (!p->maintain_lock) {
            iss = pthread_mutex_unlock(&p->mutex);
            if (UNIX_ERR(iss)) UNIX_ABORT("unlock_Proc");
        }
    } else {
        iss = pthread_mutex_unlock(&ProcListMutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("unlock_Proc");
    }
}


longword *
list_Proc(void)
{
    ROUTINE_NAME("list_Proc");
    pProc p;
    longword *PIDList = 0;
    int n = 100, j = 0;

    errlog(L_PROC|L_TRACE,"list_Proc()");
    PIDList = malloc(n * sizeof(longword));
    if (!PIDList) return 0;
    lock_Proc(0);
    p = PListHead;
    while (p) {
        if (j == n - 1) {
            n += 100;
            PIDList = realloc(PIDList,n*sizeof(longword));
            if (!PIDList) {
                unlock_Proc(0);
                return 0;
            }
        }
        PIDList[j++] = p->pid;
        p = p->next;
    }
    unlock_Proc(0);
    PIDList[j] = 0;
    return PIDList;
}

void
stop_Proc(pProc p)
{
    ROUTINE_NAME("stop_Proc");
    int iss;

    errlog(L_PROC|L_TRACE,"stop_Proc(p=0x!XL)",p);
    lock_Proc(p);
    iss = sys$forcex(&p->pid,0,0);
    if (iss != SS$_NONEXPR && VMS_ERR(iss)) VMS_SIGNAL(iss);
    p->state = PS_STOPPING;
    unlock_Proc(p);
}

pProc
getidle_Proc(pGroup g)
{
    ROUTINE_NAME("getidle_Proc");
    int iss;
    pProc p;

    errlog(L_PROC|L_TRACE,"getidle_Proc(g=0x!XL)",g);

    lock_Group(g);

    while (!g->idle_list) {
        g->n_req++;
        check_Proc();
        errlog(L_PROC|L_INFO,"getidle_Proc, waiting...");
        iss = pthread_cond_wait(&g->cond,&g->mutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("getidle_proc, cond wait");
        g->n_req--;
    }

    p = g->idle_list;
    p->killpend =  0;
    p->activations++;
    g->idle_list = p->next_idle;
    errlog(L_PROC|L_DEBUG,"getidle_Proc, got (g=!XL)->idle_list -> p=!XL returned, next is !XL",g,p,p->next_idle);
    g->n_idle--;

    unlock_Group(g);
    check_Proc();
    errlog(L_PROC|L_TRACE,"getidle_Proc, p=!XL pid=!XL removed from idle",p,p->pid);
    return p;
}

void
setidle_Proc(pProc p)
{
    ROUTINE_NAME("setidle_Proc");
    int iss, maintain;
    pGroup g;
    pProc pi;

    errlog(L_PROC|L_TRACE,"setidle_Proc(p=0x!XL)",p);
    errlog(L_PROC|L_TRACE,"setidle_Proc, p=!XL pid=!XL set idle",p,p->pid);

    g = p->group;

    lock_Group(g);

    pi = g->idle_list;
    while (pi) {
        if (pi == p) {
            errlog(L_PROC|L_INFO,"setidle_Proc (g=!XL p=!XL) already idle",g,p);
            break;
        }
        pi = pi->next_idle;
    }

    if (!pi) {
        errlog(L_PROC|L_DEBUG,"setidle_Proc (g=!XL)->idle_list = p , (p=!XL)->next_idle = !XL",g,p,g->idle_list);
        p->next_idle = g->idle_list;
        g->idle_list = p;
        g->n_idle++;
    }
    lock_Proc(p);
    maintain = p->maintain_lock;
    p->maintain_lock = 1;

    if (p->state == PS_STARTING) g->n_starting--;
    p->state = PS_IDLE;
    if (get_pflquota(p->pid) < 10000) {
        p->state = PS_STOPPING;
        unlock_Group(g);
        stop_Proc(p);
        p->maintain_lock = maintain;
        unlock_Proc(p);
    } else {
        p->maintain_lock = maintain;
        unlock_Proc(p);
        iss = pthread_cond_signal(&g->cond);
        if (UNIX_ERR(iss)) UNIX_ABORT("setidle_proc, cond_signal");
        unlock_Group(g);
    }
    check_Proc();

}


void
lock_Group(pGroup g)
{
    ROUTINE_NAME("lock_Group");
    int iss;
    errlog(L_LOCKING|L_BABBLE,"lock_Group(g=0x!XL)",g);
    if (g) {
        iss = pthread_mutex_lock(&g->mutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("lock_Group, mutex_lock");
    } else {
        iss = pthread_mutex_lock(&GroupMutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("lock_Group, mutex_lock");
    }
}

void
unlock_Group(pGroup g)
{
    ROUTINE_NAME("unlock_Group");
    int iss;
    errlog(L_LOCKING|L_BABBLE,"unlock_Group(g=0x!XL)",g);
    if (g) {
        iss = pthread_mutex_unlock(&g->mutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("unlock_Group, mutex_unlock");
    } else {
        iss = pthread_mutex_unlock(&GroupMutex);
        if (UNIX_ERR(iss)) UNIX_ABORT("unlock_Group, mutex_unlock");
    }
}

pGroup
new_Group(char *name)
{
    ROUTINE_NAME("new_Group");
    int iss, length;
    pGroup g;

    errlog(L_PROC|L_TRACE,"new_Group(name='!AZ')",name);

    g = malloc(sizeof(Group));
    if (!g) VMS_SIGNAL(SS$_INSFMEM);

    g->uic = 0;
    g->priv.prv$l_l1_bits = 0;
    g->priv.prv$l_l2_bits = 0;
    g->priv.prv$v_netmbx = 1;
    g->priv.prv$v_tmpmbx = 1;
    g->quota[0].id = PQL$_LISTEND;
    g->baspri = 2;
    g->stsflg = 0;
    g->usergroup = 0;

    g->n_idle   = 0;
    g->n_busy   = 0;
    g->n_total  = 0;
    g->n_starting = 0;
    g->n_max    =  DEF_MAXPROC_UIC0;
    g->n_min    =  DEF_MINPROC_UIC0;
    g->n_pad_max = DEF_MAXPAD_UIC0 ;
    g->n_pad_min = DEF_MINPAD_UIC0 ;
    iss = pthread_cond_init(&g->cond, pthread_condattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("new_Group, cond_init");
    iss = pthread_mutex_init(&g->mutex, pthread_mutexattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("new_Group, mutex_init");

    g->idle_list   = 0;
    g->n_req       = 0;
    g->name        = new_STRING(name);
    g->program     = new_STRING(asciz_pSTRING(GroupProgram));
    g->input       = new_STRING(asciz_pSTRING(GroupInput));
    g->output      = new_STRING(asciz_pSTRING(GroupOutput));
    g->error       = new_STRING(asciz_pSTRING(GroupError));
    g->username    = 0;
    g->bindir      = 0;
    errlog(L_PROC|L_TRACE,"new_Group returning g=0x!XL",g);
    return g;
}

void
setuser_Group(pGroup g, char *user)
{
    ROUTINE_NAME("setuser_Group");

    errlog(L_PROC|L_TRACE,"setuser_Group(g = 0x!XL, user = '!AZ')",g,(user?user:""));
    lock_Group(g);

    if (g->username) {
        destroy_STRING(g->username);
        g->username = 0;
    }
    if (!user) {
        unlock_Group(g);
        return;
    }


    g->username = new_STRING(user);
    uc(asciz_pSTRING(g->username));

    unlock_Group(g);
}

void
link_Group(pGroup g)
{
    ROUTINE_NAME("link_Group");
    errlog(L_PROC|L_TRACE,"link_Group(g = 0x!XL)",g);
    lock_Group(g);
    lock_Group(0);
    g->next = GroupHead;
    GroupHead = g;
    unlock_Group(0);
    unlock_Group(g);
}



void
init_Group(char *program, char *input, char *output, char *error)
{
    ROUTINE_NAME("init_Group");
    int iss;

    errlog(L_PROC|L_TRACE,"init_Group(in='!AZ', out='!AZ')",input,output);
    GroupHead = 0;
    iss = pthread_mutex_init(&GroupMutex,pthread_mutexattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("init_Group, mutex_init");

    GroupProgram = new_STRING(program);
    GroupInput  = new_STRING(input);
    GroupOutput = new_STRING(output);
    GroupError  = new_STRING(error);
}

void
destroy_Group(pGroup g)
{
    ROUTINE_NAME("destroy_Group");
    int iss;

    if (!g) return;
    iss = pthread_cond_destroy(&g->cond);
    if (UNIX_ERR(iss)) UNIX_ABORT("destroy_Group, cond_destroy");
    iss = pthread_mutex_destroy(&g->mutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("destroy_Group, mutex_destroy");

    destroy_STRING(g->name);
    destroy_STRING(g->program);
    destroy_STRING(g->input);
    destroy_STRING(g->output);
    destroy_STRING(g->error);
    free(g);
}

void
check_Proc()
{
    ROUTINE_NAME("check_Proc");
    int iss;

    errlog(L_PROC|L_TRACE,"check_Proc()");
    iss = pthread_cond_signal(&ProcMgrCond);
    if (UNIX_ERR(iss)) UNIX_ABORT("check_Proc, cond_signal");
}


void *
manage_Proc(void *arg)
{
    ROUTINE_NAME("manage_Proc");
    int iss, changed, maxact, havepending;
    pGroup g;
    pProc  p, p0;
    struct timespec twake, twait = {60, 0};        /* 60 seconds between updates */
    longword now[2], delta_kill[2];


    errlog(L_PROC|L_TRACE,"manage_Proc() started...");
    sec2vms(30, delta_kill);

    while (1) {
        lock_Group(0);
        g = GroupHead;
        changed = 0;

        while (g) {
            if (g->usergroup) {
                g = g->next;
                continue;
            }

            lock_Group(g);
            unlock_Group(0);

            errlog(L_PROC|L_BABBLE,"manage_Proc, group=0x!XL, n_idle=!SL, n_starting=!SL n_req=!SL",g,g->n_idle, g->n_starting, g->n_req);
            errlog(L_PROC|L_BABBLE,"manage_Proc, group=0x!XL, n_pad_min=!SL n_pad_max=!SL n_total=!SL n_max=!SL n_min=!SL",g,g->n_pad_min,g->n_pad_max,g->n_total, g->n_max, g->n_min);
            if (((g->n_idle + g->n_starting < g->n_pad_min || g->n_req > g->n_starting) && g->n_total < g->n_max) ||
                (g->n_total < g->n_min)) {
                errlog(L_PROC|L_BABBLE,"manage_Proc, starting new proc");
                unlock_Group(g);
                p = new_Proc(g);
                start_Proc(p);
#ifdef UNTHROTTLED
                changed = 1;
#endif
            } else if (g->n_idle > g->n_pad_max && g->n_total > g->n_min && !g->n_req) {
                iss = sys$gettim(now);
                if (VMS_ERR(iss)) VMS_SIGNAL(iss);
                havepending = 0;

                errlog(L_PROC|L_BABBLE,"manage_Proc, killing idle proc");
                p0 = 0;
                if (p = g->idle_list) {
                    while (p) {
                        if (p->killpend) {
                            longword dum[2];

                            havepending = 1;
                            iss = lib$sub_times(now, p->killtime, dum);
                            if (iss == LIB$_NORMAL) {
                                errlog(L_PROC|L_INFO,"manage_Proc killing killpending p=!XL pid=!XL",p,p->pid);
                                if (p0)
                                    p0->next_idle = p->next_idle;
                                else
                                    g->idle_list  = p->next_idle;

                                g->n_idle--;
                                stop_Proc(p);
                                changed = 1;
                                break;
                            } else if (iss != LIB$_NEGTIM)  {
                                VMS_SIGNAL(iss);
                            }
                        }
                        p0 = p;
                        p = p->next_idle;
                    }


                    if (!havepending) {           /* no kill_pending ones...pick one for pending status */
                        p = g->idle_list;
                        maxact = -1;
                        p0 = 0;
                        while (p) {
                            if (p->activations > maxact) {
                                p0 = p;
                                maxact = p->activations;
                            }
                            p = p->next_idle;
                        }
                        if (p0) {
                            p0->killpend = 1;
                            iss = lib$add_times(now, delta_kill, p0->killtime);
                            if (VMS_ERR(iss)) VMS_SIGNAL(iss);
                            errlog(L_PROC|L_INFO,"manage_Proc setting killpending p=!XL pid=!XL @ !%T",p0,p0->pid,p0->killtime);
                        } else {
                            errlog(L_CRITICAL,"manage proc, killing idle, but nothing in activity list!");
                        }
                    }
                } else {
                    errlog(L_CRITICAL,"manage proc, killing idle, but nothing in list!");
                    VMS_SIGNAL(SS$_ABORT);
                }
                unlock_Group(g);
            } else {
                unlock_Group(g);
            }
            lock_Group(0);
            g = g->next;
        }
        unlock_Group(0);

        if (!changed) {
            errlog(L_PROC|L_BABBLE,"manage_Proc() waiting...");
            iss = pthread_mutex_lock(&ProcMgrMutex);
            if (UNIX_ERR(iss)) UNIX_ABORT("manage_proc, mutex_lock");

            iss = pthread_get_expiration_np(&twait, &twake);
            if (UNIX_ERR(iss)) UNIX_ABORT("manage_proc, get_expiration");
            iss = pthread_cond_timedwait(&ProcMgrCond,&ProcMgrMutex,&twake);
            if (UNIX_ERR(iss) && errno != EAGAIN) UNIX_ABORT("manage_proc, cond wait");

            iss = pthread_mutex_unlock(&ProcMgrMutex);
            if (UNIX_ERR(iss)) UNIX_ABORT("manage_proc, mutex_unlock");
            errlog(L_PROC|L_BABBLE,"manage_Proc() signalled to wake...");
        }
    }
    return 0;
}



void
init_manage_Proc()
{
    ROUTINE_NAME("init_manage_Proc");
    int iss;

    errlog(L_PROC|L_TRACE,"init_manage_Proc()");
    iss = pthread_mutex_init(&ProcMgrMutex, pthread_mutexattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("init_manage_proc, mutex_init");

    iss = pthread_cond_init(&ProcMgrCond, pthread_condattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("init_manage_proc, cond_init");

    iss = pthread_create(&ProcMgrThread, pthread_attr_default, &manage_Proc, 0);
    if (UNIX_ERR(iss)) UNIX_ABORT("init_manage_proc, thread_create");

}

void
stop_manage_Proc()
{
    ROUTINE_NAME("stop_manage_Proc");
    int iss;

    errlog(L_PROC|L_TRACE,"stop_manage_Proc()");
    iss = pthread_cancel(ProcMgrThread);
    if (UNIX_ERR(iss)) UNIX_ABORT("stop_manage_proc, thread_cancel");
}

void
kill_all_Procs()
{
    ROUTINE_NAME("kill_all_Procs");
    int iss;
    pProc p;

    errlog(L_PROC|L_TRACE,"kill_all_Procs()");

    lock_Proc(0);

    p = PListHead;
    while (p) {
        stop_Proc(p);
        p = p->next;
    }
    unlock_Proc(0);
}

/* this is a dummy, for lack of something better... */

pGroup
find_Group(longword uic)
{
    ROUTINE_NAME("find_Group");
    pGroup g = GroupHead;
    errlog(L_PROC|L_TRACE,"find_Group(uic=!%U)",uic);

    while (g) {
        if (g->uic == uic) {
            errlog(L_PROC|L_TRACE,"find_Group() returning g=0x!XL",g);
            return g;
        }
        g = g->next;
    }

    errlog(L_PROC|L_ERROR,"find_Group(!%U) unable to find UIC",uic);
    return 0;
}

pGroup
find_named_Group(char *name)
{
    ROUTINE_NAME("find_named_Group");
    pGroup g = GroupHead;
    errlog(L_PROC|L_TRACE,"find_named_group(name='!AZ')",name);

    if (!name) return g;

    while (g) {
        if (tu_strcmp_uc(asciz_pSTRING(g->name), name) == 0) return g;
        g = g->next;
    }
    return 0;
}


pGroup
find_username_Group(char *name)
{
    ROUTINE_NAME("find_username_Group");
    pGroup g = GroupHead;
    errlog(L_PROC|L_TRACE,"find_username_group(name='!AZ')",name);

    if (!name) return 0;

    while (g) {
        if (g->username) {
            if (tu_strcmp_uc(asciz_pSTRING(g->username), name) == 0) return g;
        }
        g = g->next;
    }
    return 0;
}





void
exit_handle_Proc(void)
{
    ROUTINE_NAME("exit_handle_Proc");
    pProc p = PListHead;

    while (p) {
        sys$forcex(&p->pid,0,0);
        p = p->next;
    }
}


void
printall_Group(FILE *fd)
{
    ROUTINE_NAME("printall_Group");
    pGroup g = GroupHead;

    if (!fd) return;
    while (g) {
        print_Group(g,fd);
        g = g->next;
    }
}

void
print_Group(pGroup g, FILE *fd)
{
    ROUTINE_NAME("print_Group");

    if (!g) return;
    if (!fd) return;

    fprintf(fd,"\n-----------------------------------------------\n");
    fprintf(fd,"Group:           %s  @ 0x%08x\n",asciz_pSTRING(g->name),g);
    fprintf(fd,"UIC:             %08x\n",g->uic);
    fprintf(fd,"PRIVS[0]:        %08x\n",g->priv.prv$l_l1_bits);
    fprintf(fd,"PRIVS[1]:        %08x\n",g->priv.prv$l_l2_bits);
    fprintf(fd,"BASE PRIORITY:   %d\n",g->baspri);
    fprintf(fd,"STATUS           %08x\n",g->stsflg);
    fprintf(fd,"PROGRAM:         %s\n",asciz_pSTRING(g->program));
    fprintf(fd,"INPUT:           %s\n",asciz_pSTRING(g->input));
    fprintf(fd,"OUTPUT:          %s\n",asciz_pSTRING(g->output));
    fprintf(fd,"ERROR:           %s\n",asciz_pSTRING(g->error));
    fprintf(fd," #idle           %d\n",g->n_idle);
    fprintf(fd," #busy           %d\n",g->n_busy);
    fprintf(fd," #total          %d\n",g->n_total);
    fprintf(fd," #starting       %d\n",g->n_starting);
    fprintf(fd," #max            %d\n",g->n_max);
    fprintf(fd," #min            %d\n",g->n_min);
    fprintf(fd," #pad_max        %d\n",g->n_pad_max);
    fprintf(fd," #pad_min        %d\n",g->n_pad_min);
    fprintf(fd," #req            %d\n",g->n_req);
    fprintf(fd,"-----------------------------------------------\n");
}
