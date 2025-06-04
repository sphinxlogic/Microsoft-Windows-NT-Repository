/*  mbx queue / pipe routines  */

#include <stdlib.h>
#include <string.h>

#include <ssdef.h>
#include <iodef.h>
#include <starlet.h>
#include <lib$routines.h>
#include <libdef.h>
#include <builtins.h>

#define __PRIVATE_MBXQ
#include "mbxq.h"
#include "util.h"
#include "errlog_client.h"

static longword         mbxq_ef = 0;

static RQE              ZeroRQE = {0,0};
static RQE              FreeQHead = {0,0};
#define FreeQ           (&FreeQHead)
#define DEFAULT_TIMEOUT 1
#define qName(q)        ((q)->name ? (q)->name : "")
#define qLog(q)         (!((q)->option & MBXQ$M_NOLOG))

void show_quotas(void);

pMbxQ
MbxQ_new(pMBX m, longword option, void (*AST)(pMbxE))
{
    ROUTINE_NAME("MbxQ_new");
    pMbxQ q;
    int iss;

    if (!mbxq_ef) {
        iss = sys$setast(0);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);

        if (!mbxq_ef) {
            iss = lib$get_ef(&mbxq_ef);
            if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        }

        iss = sys$setast(1);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    }

    if (option & MBXQ$M_DISCARD && option & MBXQ$M_RETRY) return 0;
    if (!(option & MBXQ$M_DISCARD) && !(option & MBXQ$M_RETRY))
        option |= MBXQ$M_DISCARD;

    q = malloc(sizeof(MbxQ));
    if (!q) {
        errlog(L_CRITICAL,"MbxQ_new: failed malloc!");
        return 0;
    }

    q->option = option & MBXQ$M_USERFIELD;

    if (!m) {
        m = new_MBX(0,0);
        q->option |= MBXQ$M_MBXCREATED;
    } else {
        if (m->size > MBXQEBUF) {
            errlog(L_CRITICAL,"MbxQ_new: mailbox size too large: !SL",m->size);
            return 0;
        }
    }
    errlog(L_MBXQ|L_INFO,"MbxQ_new, q=!XL, option=!XL !AZ",q,q->option,m->name);

    q->mbx           = m;
    q->msgq          = ZeroRQE;
    q->AST           = AST;
    q->ef            = 0;
    q->inprogress    = 0;
    q->pending       = 0;
    q->bytes         = 0;
    q->extra         = NULL;
    q->shut_ef       = 0;
    q->backlog       = 0;
    q->maxbacklog    = -1;
    q->next_id       = 0;
    q->name          = 0;
    if (q->option & MBXQ$M_DISCARD) q->maxbacklog = 1;

    if (!(q->option & MBXQ$_WRITE) && !(q->option & MBXQ$M_NOSTART))
        MbxQ_queue_read(q);

    return q;
}


static int
MbxQ_write2(pMbxQ q, void *buf, int n)
{
    ROUTINE_NAME("MbxQ_write2");
    int iss, nback;
    pMbxE e;

    if (!q) return SS$_BADPARAM;
    if (n > q->mbx->size) return SS$_MBTOOSML;
    if (q->option & MBXQ$M_SHUT) return SS$_SHUT;

    e = MbxQ_new_entry();
    if (!e) return SS$_INSFMEM;

    e->queue = q;
    if (buf && n > 0)
        memcpy(e->buf, buf, n);
    e->len = n;
    e->id = __ATOMIC_INCREMENT_LONG(&q->next_id);

    nback = __ATOMIC_INCREMENT_LONG(&q->backlog);
    if (q->maxbacklog > 0 && nback > q->maxbacklog) {
        __ATOMIC_DECREMENT_LONG(&q->backlog);
        if (qLog(q)) errlog(L_MBXQ|L_DEBUG,"MbxQ_write2, q:!XL, nback !SL > max, discarding id: !SL",q,nback,e->id);
        iss = lib$insqti(e, FreeQ);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        return SS$_NORMAL;
    }

    iss = lib$insqti(e, &q->msgq);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    iss = _BBSSI(0,&q->pending);
    if (iss==1) {
        if(qLog(q)) errlog(L_MBXQ|L_DEBUG,"MbxQ_write2 q:!XL(!AZ) id=!SL pending status",q,qName(q),e->id);
        return SS$_NORMAL;
    }

    if (qLog(q) && !(e->id % 10)) show_quotas();

    iss = sys$dclast(&MbxQ_ASTw,q,0);
    if(qLog(q)) errlog(L_MBXQ|L_DEBUG,"MbxQ_write2 q:!XL(!AZ) id=!SL DCLAST iss=!XL",q,qName(q),e->id,iss);
    return iss;
}


static void
MbxQ_ASTw(pMbxQ q)
{
    ROUTINE_NAME("MbxQ_ASTw");
    int iss, nback;
    pMbxE e;

    iss = lib$remqhi(&q->msgq, &e);
    if (iss == LIB$_QUEWASEMP) {
        iss = _BBCCI(0,&q->pending);
        return;
    }
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    if (e->len >= 0)
        iss = sys$qio(0,q->mbx->chan,IO$_WRITEVBLK|IO$M_NORSWAIT,&e->iosb,&MbxQ_ASTwc,e,e->buf,e->len,0,0,0,0);
    else {
        if (q->option & MBXQ$M_NETMBX) {
             iss = sys$qio(0,q->mbx->chan,IO$_DEACCESS|IO$M_SYNCH,&e->iosb,&MbxQ_ASTwc,e,0,0,0,0,0,0);
        } else {
             iss = sys$qio(0,q->mbx->chan,IO$_WRITEOF|IO$M_NORSWAIT,&e->iosb,&MbxQ_ASTwc,e,0,0,0,0,0,0);
        }
    }
    nback = __ATOMIC_DECREMENT_LONG(&q->backlog);
    if (qLog(q)) errlog(L_MBXQ|L_DEBUG,"MbxQ_ASTw, q=!XL(!AZ) id=!SL qio iss=!XL len=!SL nback=!SL",q,qName(q),e->id,iss,e->len,nback);

    if (VMS_ERR(iss)) {show_quotas(); VMS_SIGNAL(iss);}
    __ATOMIC_INCREMENT_LONG(&q->inprogress);
}



static void
MbxQ_ASTwc(pMbxE e)
{
    ROUTINE_NAME("MbxQ_ASTwc");
    int iss;
    longword option, inprog;
    pMbxQ q = e->queue;
    int loglev = L_MBXQ|L_DEBUG;

    inprog = __ATOMIC_DECREMENT_LONG(&q->inprogress) - 1;

    iss = e->iosb.status;
    if (VMS_OK(iss)) q->bytes += e->iosb.count;
    if (qLog(q) && islogging(loglev)) {
        if (e->len >= 0) {
            char *s = dumpstring(e->buf, e->len > 10 ? 10 : e->len);
            errlog(loglev,"MbxQ_ASTwc  q=!XL(!AZ) id = !SL iss = !XL count=!SL total=!SL !AZ",q,qName(q),e->id,iss,e->iosb.count,q->bytes,s);
            free(s);
        } else {
            errlog(loglev,"MbxQ_ASTwc  q=!XL(!AZ) id = !SL iss = !XL count=!SL total=!SL <EOF>",q,qName(q),e->id,iss,e->iosb.count,q->bytes);
        }
    }

    if (iss == SS$_ENDOFFILE || iss == SS$_LINKDISCON || iss == SS$_LINKABORT) {
        MbxQ_ASTws(q);
    } else if (iss == SS$_CANCEL || iss == SS$_ABORT) {
        _BBSSI(MBXQ$V_SHUT, &q->option);
        if (q->shut_ef) sys$setef(q->shut_ef);
    } else if (VMS_ERR(iss)) {
        VMS_SIGNAL(iss);
    } else if (e->iosb.count != e->len && e->len >= 0)    /* incomplete write */
        VMS_SIGNAL(SS$_ABORT);

    iss = sys$setef(mbxq_ef);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    if (q->AST) (q->AST)(e);
    iss = lib$insqti(e, FreeQ);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    option = q->option;
    if (qLog(q)) errlog(L_MBXQ|L_DEBUG,"MbxQ_ASTwc, q=!XL(!AZ) option=!XL",q,qName(q),iss, q->option);
    if (option & MBXQ$M_SHUT && option & MBXQ$M_DELETEPENDING && inprog == 0) {
        if (option & MBXQ$M_MBXCREATED) {
            if (qLog(q)) errlog(L_MBXQ|L_INFO,"MbxQ_ASTwc, destroying mbx",q,qName(q));
            destroy_MBX(q->mbx);
        }
        if (q->name) free(q->name);
        free(q);
    } else {
        MbxQ_ASTw(q);
    }
}


static void
MbxQ_ASTws(pMbxQ q)
{
    ROUTINE_NAME("MbxQ_ASTws");
    int iss;
    pMbxE e;

    if (qLog(q)) errlog(L_MBXQ|L_INFO,"MbxQws, q=!XL(!AZ) writeshut option=!XL",q,qName(q),q->option);
    _BBSSI(MBXQ$V_DISCARD, &q->option);
    _BBCCI(MBXQ$V_RETRY, &q->option);

    while (1) {
        iss = lib$remqhi(&q->msgq, &e);
        if (iss == LIB$_QUEWASEMP) break;
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);

        iss = lib$insqti(e, FreeQ);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    }

    if (q->inprogress != 0) {
        if (qLog(q)) errlog(L_MBXQ|L_INFO,"MbxQ_ASTws, q=!XL(!AZ) inprogress, cancelling",q,qName(q));
        iss = sys$cancel(q->mbx->chan);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    } else {
        _BBSSI(MBXQ$V_SHUT, &q->option);
        if (q->shut_ef) sys$setef(q->shut_ef);
        if (q->option & MBXQ$M_DELETEPENDING) {
            if (q->option & MBXQ$M_MBXCREATED) {
                if (qLog(q)) errlog(L_MBXQ|L_INFO,"MbxQ_ASTws, q=!XL(!AZ) destroying mbx",q,qName(q));
                destroy_MBX(q->mbx);
            }
            if (q->name) free(q->name);
            free(q);
        }
    }
}


static int
MbxQ_queue_read(pMbxQ q)
{
    ROUTINE_NAME("MbxQ_queue_read");
    int iss, size;
    pMbxE e;

    if (q->option & MBXQ$M_SHUT) {
        if (qLog(q)) errlog(L_MBXQ|L_DEBUG,"Mbxq_queue_read, q=!XL(!AZ) not queued, shut",q,qName(q));
        return SS$_SHUT;
    }

    e = MbxQ_new_entry();
    if (!e) return SS$_INSFMEM;

    e->queue = q;
    e->id = __ATOMIC_INCREMENT_LONG(&q->next_id);
    size = q->mbx->size;
    iss = sys$qio(0,q->mbx->chan, IO$_READVBLK, &e->iosb, &MbxQ_ASTr, e, e->buf, size, 0, 0, 0, 0);
    if (qLog(q)) errlog(L_MBXQ|L_DEBUG,"Mbxq_queue_read, q=!XL(!AZ) id=!SL qioiss=!XL",q,qName(q),e->id,iss);
    if (VMS_ERR(iss)) {show_quotas(); VMS_SIGNAL(iss);}
    return iss;
}

static void
MbxQ_ASTr(pMbxE e)
{
    ROUTINE_NAME("MbxQ_ASTr");
    pMbxE e2;
    pMbxQ q = e->queue;
    int iss;

    iss = e->iosb.status;
    if (VMS_OK(iss)) q->bytes += e->iosb.count;

    if (iss == SS$_CANCEL || iss == SS$_ABORT || iss == SS$_LINKABORT || iss == SS$_LINKDISCON) {
        if (qLog(q)) errlog(L_MBXQ|L_INFO,"MbxQ_ASTr, q=!XL(!AZ) status=!XL option=!XL shutting down",q,qName(q),iss, q->option);
        _BBSSI(MBXQ$V_SHUT, &q->option);
        if (q->shut_ef) sys$setef(q->shut_ef);
        iss = lib$insqti(e, FreeQ);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        if (q->option & MBXQ$M_DELETEPENDING) {
            while (1) {
                iss = lib$remqhi(&q->msgq, &e2);
                if (iss == LIB$_QUEWASEMP) break;
                if (VMS_ERR(iss)) VMS_SIGNAL(iss);
                iss = lib$insqti(e2,FreeQ);
                if (VMS_ERR(iss)) VMS_SIGNAL(iss);
            }
            if (q->option & MBXQ$M_MBXCREATED) {
                if (qLog(q)) errlog(L_MBXQ|L_INFO,"MbxQ_ASTr, q=!XL(!AZ) mailbox destroyed",q,qName(q));
                destroy_MBX(q->mbx);
            }
            if (q->name) free(q->name);
            free(q);
        }
        return;
    } else if (VMS_ERR(iss) && iss != SS$_ENDOFFILE)
        VMS_SIGNAL(iss);

    e->len = e->iosb.count;

    iss = sys$setef(mbxq_ef);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    iss  = MbxQ_queue_read(q);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    if (qLog(q) && islogging(L_MBXQ|L_DEBUG)) {
        if (e->len >= 0) {
            char *s = dumpstring(e->buf, e->len > 10 ? 10: e->len);
            errlog(L_MBXQ|L_DEBUG,"MbxQ_ASTr, q = 0x!XL(!AZ), id=!SL len=!SL !AZ",q,qName(q),e->id,e->len,s);
            free(s);
        } else {
            errlog(L_MBXQ|L_DEBUG,"MbxQ_ASTr, q = 0x!XL(!AZ), id=!SL len=!SL <EOF>",q,qName(q),e->id,e->len);
        }
    }
    if (q->AST) {
        (q->AST)(e);
    } else {
        iss = lib$insqti(e, &q->msgq);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    }
}

pMbxE
MbxQ_read(pMbxQ q)
{
    ROUTINE_NAME("MbxQ_read");
    int iss;
    pMbxE e;

    iss = lib$remqhi(&q->msgq, &e);
    if (iss == LIB$_QUEWASEMP) {
        return 0;
    }
    return e;
}

pMbxE
MbxQ_read2(pMbxQ q, int timeout)
{
    ROUTINE_NAME("MbxQ_read2");
    int iss;
    pMbxE e = 0;
    longword ef = 0, retry = 1, mask, time[2], idum;

    while (1) {
        iss = sys$clref(mbxq_ef);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);

        iss = lib$remqhi(&q->msgq, &e);
        if (iss == LIB$_QUEWASEMP && !retry) {
            e = 0;
            break;
        }
        if (iss == LIB$_QUEWASEMP) {
            e = 0;
            if (!ef) {
                iss = lib$get_ef(&ef);
                if (VMS_ERR(iss)) VMS_SIGNAL(iss);
                sec2vms(timeout,time);
                mask = 1<<(ef&0x1F) | 1<<(mbxq_ef&0x1F);
            }
            iss = sys$setimr(ef, time, 0, ef, 0);
            if (VMS_ERR(iss)) VMS_SIGNAL(iss);
            iss = sys$wflor(ef,mask);
            if (VMS_ERR(iss)) VMS_SIGNAL(iss);

            iss = sys$readef(ef,&idum);
            if (VMS_ERR(iss)) VMS_SIGNAL(iss);
            if (iss == SS$_WASCLR) {
                iss = sys$cantim(ef,0);
                if (VMS_ERR(iss)) VMS_SIGNAL(iss);
            } else if (iss == SS$_WASSET) {
                retry = 0;
            }
        } else if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        else break;
    }

    if (ef) {
        iss = lib$free_ef(&ef);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    }
    return e;
}





void
MbxQ_dispose(pMbxE e)
{
    ROUTINE_NAME("MbxQ_dispose");
    int iss;

    iss = lib$insqti(e, FreeQ);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
}


int
MbxQ_shut(pMbxQ q)
{
    ROUTINE_NAME("MbxQ_shut");
    int iss = SS$_NORMAL;

    if (q->option & MBXQ$_WRITE) {
        iss = sys$dclast(&MbxQ_ASTws,q,0);
    } else {
        if (!(q->option & MBXQ$M_SHUT)) {
            if (qLog(q)) errlog(L_MBXQ|L_INFO,"MbxQ_shut, q=!XL(!AZ) cancelling i/o",q,qName(q));
            iss = sys$cancel(q->mbx->chan);
        }
    }
    return iss;
}

int
MbxQ_shut2(pMbxQ q)
{
    ROUTINE_NAME("MbxQ_shut2");
    int iss = SS$_NORMAL;
    if (!q) return SS$_ABORT;

    if (qLog(q)) errlog(L_MBXQ|L_DEBUG,"MbxQ_shut2, q=!XL(!AZ)",q,qName(q));
    iss = lib$get_ef(&q->shut_ef);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = sys$clref(q->shut_ef);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    if (q->option & MBXQ$_WRITE) {
        iss = sys$dclast(&MbxQ_ASTws,q,0);
    } else {
        if (!(q->option & MBXQ$M_SHUT))
            iss = sys$cancel(q->mbx->chan);
    }
    iss = sys$waitfr(q->shut_ef);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = lib$free_ef(&q->shut_ef);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    q->shut_ef = 0;
    return SS$_NORMAL;
}

pMbxQ
MbxQ_destroy(pMbxQ q)
{
    ROUTINE_NAME("MbxQ_destroy");
    int iss;
    pMbxE e;

    _BBSSI(MBXQ$V_DELETEPENDING, &q->option);
    if (!(q->option & MBXQ$M_SHUT)) {
        if (qLog(q)) errlog(L_MBXQ|L_INFO,"MbxQ_destroy, q=!XL(!AZ) have to shut first",q,qName(q));
        MbxQ_shut(q);
    } else {
        if (!(q->option & MBXQ$_WRITE)) {
            while (1) {
                iss = lib$remqhi(&q->msgq, &e);
                if (iss == LIB$_QUEWASEMP) break;
                if (VMS_ERR(iss)) VMS_SIGNAL(iss);
                iss = lib$insqti(e,FreeQ);
                if (VMS_ERR(iss)) VMS_SIGNAL(iss);
            }
        }
        if (q->option & MBXQ$M_MBXCREATED) {
            if (qLog(q)) errlog(L_MBXQ|L_INFO,"MbxQ_destroy, q=!XL(!AZ) destroying mbx",q,qName(q));
            destroy_MBX(q->mbx);
        }
        if (q->name) free(q->name);
        free(q);
    }
    return 0;
}



int
MbxQ_write(pMbxQ q, void *buf, int n)
{
    ROUTINE_NAME("MbxQ_write");
    int iss, len;

    if (!q) return SS$_BADPARAM;
    if (!(q->option & MBXQ$_WRITE)) return SS$_BADPARAM;

    if (n <= q->mbx->size) {
        iss = MbxQ_write2(q, buf, n);
    } else {
        iss = SS$_MBTOOSML;

        if (q->option & MBXQ$M_SEGMENT) {
            while (n > 0) {
                len = n > q->mbx->size ? q->mbx->size : n;
                iss = MbxQ_write2(q, buf, len);
        if (VMS_ERR(iss) && qLog(q)) errlog(L_ERROR,"MbxQ_write2, iss:!XL",iss);
                if (VMS_ERR(iss)) return iss;
                n   -= len;
                buf = (void *) ((char *) buf + len);
            }
        }
    }
    if (VMS_ERR(iss) && qLog(q)) errlog(L_ERROR,"MbxQ_write2, iss:!XL",iss);
    return iss;
}


int
MbxQ_start(pMbxQ q)
{
    ROUTINE_NAME("MbxQ_start");
    if (!q) return SS$_BADPARAM;
    if (q->option & MBXQ$M_SHUT) return SS$_SHUT;
    if (!(q->option & MBXQ$M_NOSTART)) return SS$_NORMAL;

    _BBCCI(MBXQ$V_NOSTART, &q->option);
    return MbxQ_queue_read(q);

}


int MbxQ_bytes(pMbxQ q)
{
    ROUTINE_NAME("MbxQ_bytes");
    int iss;
    int n;

    iss = sys$setast(0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    n = q->bytes;

    iss = sys$setast(1);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    return n;
}


void
MbxQ_setbacklog(pMbxQ q, int n)
{
    ROUTINE_NAME("MbxQ_setbacklog");
    q->maxbacklog = n;
}

void
MbxQ_setname(pMbxQ q, char *name)
{
    ROUTINE_NAME("MbxQ_setname");
    if (q->name) free(q->name);
    q->name = malloc(strlen(name)+1);
    if (!q->name) return;
    strcpy(q->name, name);
}








#include <jpidef.h>

void
show_quotas(void)
{
    ROUTINE_NAME("show_quotas");
    int iss;
    longword pid = get_pid();
    longword astcnt = 0;
    longword biocnt = 0;
    longword bytcnt = 0;
    longword diocnt = 0;
    longword enqcnt = 0;
    longword filcnt = 0;
    longword tqcnt  = 0;
    IOSB iosb;
    pItemList i;

    i = new_ItemList(7);
    i = add_Item(i, JPI$_ASTCNT, &astcnt, sizeof(astcnt), 0);
    i = add_Item(i, JPI$_BIOCNT, &biocnt, sizeof(biocnt), 0);
    i = add_Item(i, JPI$_BYTCNT, &bytcnt, sizeof(bytcnt), 0);
    i = add_Item(i, JPI$_DIOCNT, &diocnt, sizeof(diocnt), 0);
    i = add_Item(i, JPI$_ENQCNT, &enqcnt, sizeof(enqcnt), 0);
    i = add_Item(i, JPI$_FILCNT, &filcnt, sizeof(filcnt), 0);
    i = add_Item(i, JPI$_TQCNT,  &tqcnt,  sizeof(tqcnt), 0);

    iss = sys$getjpiw(0,&pid,0,i,&iosb,0,0);
    destroy_ItemList(i);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = iosb.status;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    errlog(L_MBXQ|L_DEBUG,"Quotas: AST:!SL BIO:!SL BYT:!SL DIO:!SL ENQ:!SL FIL:!SL TQ:!SL",astcnt,biocnt,bytcnt,diocnt,enqcnt,filcnt,tqcnt);
}


pMbxE
MbxQ_new_entry(void)
{
    ROUTINE_NAME("MbxQ_new_entry");
    int iss;
    pMbxE e;

    iss = lib$remqhi(FreeQ, &e);
    if (iss == LIB$_QUEWASEMP) {
        e = malloc(sizeof(MbxE));
    } else if (VMS_ERR(iss)) {
        VMS_SIGNAL(iss);
    }
    return e;
}

int
MbxQ_flushed(pMbxQ q, int n, int timeout)
{
    ROUTINE_NAME("MbxQ_flushed");
    int iss, retry = 1;
    longword ef, mask, time[2], idum;

    if (n <= 0) n = MbxQ_bytes(q);

    iss = lib$get_ef(&ef);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    sec2vms(timeout,time);
    mask = 1<<(ef&0x1F) | 1<<(mbxq_ef&0x1F);
    iss = sys$setimr(ef, time, 0, ef, 0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    while (MbxQ_bytes(q) < n && retry) {
        iss = sys$clref(mbxq_ef);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);

        iss = sys$wflor(ef,mask);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);

        iss = sys$readef(ef,&idum);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        retry = (iss == SS$_WASCLR);
    }

    if (retry) {
        iss = sys$cantim(ef,0);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    }

    iss = lib$free_ef(&ef);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    return MbxQ_bytes(q);
}

