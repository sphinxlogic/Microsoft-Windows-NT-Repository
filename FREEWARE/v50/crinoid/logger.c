/*
    standalone process to do mailbox -> logfile

 */
#include <stdio.h>
#include <string.h>
#include <ssdef.h>
#include <starlet.h>
#include <iodef.h>
#include <stdlib.h>
#include <libdef.h>
#include <LIB$ROUTINES.H>       /* DECC */
#include <lnmdef.h>
#include <psldef.h>
#include <prvdef.h>
#include <lckdef.h>
#include <descrip.h>
#include <rms.h>

#include "vms_data.h"
#include "util.h"
#include "errlog_client.h"

#define MAXMSG          DEFAULT_MAILBOX_SIZE
#define MBXQUOTA        ((DEFAULT_MAILBOX_SIZE+8)*10)
#define LOGMBX          "CRINOIDLOG_MBX"
#define ERRMBX          "CRINOIDLOG_STDERR_MBX"
#define COMMAND_LOGICAL "CRINOIDLOG_COMMAND"
#define FILE_LOGICAL    "CRINOIDLOG_FILE"

#define TYPE_ERRLOG     1
#define TYPE_STDERR     2
#define TYPE_TIMER      3

typedef struct LOGNTRY    Log;
typedef struct LOGNTRY*  pLog;
typedef struct _LOGMSG    LogMsg;
typedef struct _LOGMSG*  pLogMsg;

struct LOGNTRY {
    int type;
    word chan;
    pLogMsg msg;
};

struct _LOGMSG {
    RQE  q;                     /* put here to keep alignment right ! */
    IOSB iosb;
    int  type;
    char buffer[MAXMSG+1];
    char _pad[sizeof(RQE)-(sizeof(IOSB)+sizeof(int)+MAXMSG+1)%sizeof(RQE)];
};

#define INITIAL_MSGS    100
static LogMsg init_msg[INITIAL_MSGS];

static RQE              LogMSGQHead = {0,0};
#define MsgQ           (&LogMSGQHead)
static RQE              FreeQHead = {0,0};
#define FreeQ           (&FreeQHead)

typedef struct LNMDEF    LNM;
typedef struct LNMDEF*  pLNM;

struct LNMDEF {
    pSTRING name;
    pSTRING equiv;
    pSTRING table;
    byte    acmode;
};

static Log             ltimer;
static Log             lerrlog;
static Log             lstderr;
static struct FAB      outfab;
static struct RAB      outrab;
static longword        t_expire[2];
static char            logfile[256];
static longword        ef = 4;

static LSB logger_main_lsb;
static LSB logger_sub_lsb;
static     $DESCRIPTOR(d_lock,LOG_MAIN_LOCK);
static     $DESCRIPTOR(d_sublock,LOG_SUB_LOCK);

#define LOGFLUSHTIME   20
#define LOGFILE        "LOGGER_OUTPUT.LOG"

int     initialize(void);
int     set_timer(pLog p);
int     logentry(pLogMsg m);
void    queue_logmbx_read(pLog p);
void    logger_AST(pLog p);
int     check_for_stop();
pLNM    translate_logical2(char *name);
int     define_logical(char *name, char *value, char *table);
int     openlog(void);
int     closelog(void);
pLNM    destroy_LNM(pLNM l);


int
main()
{
    ROUTINE_NAME("Logger_main");
    int iss, activity=0, do_flush;
    int dirty = 0;
    int run = 1;
    pSTRING log;
    pLogMsg m;

    errlog_lock();      /* no logging while we're logging! */

    log = translate_logical(FILE_LOGICAL);
    strcpy(logfile, log ? asciz_pSTRING(log): LOGFILE);
    if (log) destroy_STRING(log);

    iss = initialize();
    if (VMS_ERR(iss)) return iss;

    while (run) {
        while (1) {
            iss = lib$remqti(MsgQ, &m);
            if (iss == LIB$_QUEWASEMP) break;
            if (VMS_ERR(iss)) VMS_SIGNAL(iss);

            if (m->type == TYPE_TIMER) {
                do_flush = dirty && !activity;
                activity = 0;
                run = !check_for_stop();
            } else {
                activity = 1;
                dirty++;
                logentry(m);
                if (VMS_ERR(iss)) VMS_SIGNAL(iss);
            }
            do_flush |= dirty > 10;

            if (do_flush) {
                iss = sys$flush(&outrab);
                if (VMS_ERR(iss)) VMS_SIGNAL(iss);
                dirty = 0;
            }

            iss = lib$insqhi(m, FreeQ);
            if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        }
        if (run) {
            iss = sys$clref(ef);
            if (VMS_ERR(iss)) VMS_SIGNAL(iss);

            iss = sys$waitfr(ef);
            if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        }
    }
    iss = closelog();
    return iss;
}


int
initialize(void)
{
    ROUTINE_NAME("initialize");
    int iss, j;
    longword pmask = 0x1100;
    word len, ch_mbx, ch_mbx2;
    pSTRING mbx1 = new_STRING(LOGMBX);
    pSTRING mbx2 = new_STRING(ERRMBX);
    pLNM q;
    pPrivs p0, p1;

    p0 = Current_Privs();
    if (!p0->prv$v_syslck) {
        p1 = new_Privs();
        p1->prv$v_syslck = 1;
        iss = Set_Privs(p1);
        if (VMS_ERR(iss)) VMS_SIGNAL(SS$_NOPRIV);
    }

    /* we can share with CRINOID that started us */
    iss = sys$enqw(0,LCK$K_PRMODE,&logger_main_lsb,LCK$M_SYSTEM,&d_lock,0,0,0,0,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    /* but we want EX mode before other CRINOIDs try to start loggers */
    iss = sys$enq(0,LCK$K_EXMODE,&logger_main_lsb,LCK$M_CONVERT,0,0,0,0,0,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    /* queuing sublock notifies CRINOID we've started, when granted
       we should have the main lock in EX mode too */

    iss = sys$enqw(0,LCK$K_EXMODE,&logger_sub_lsb,LCK$M_SYSTEM,
                  &d_sublock,logger_main_lsb.id,0,0,0,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    /* don't need sublock any more, get rid of it */
    iss = sys$deq(logger_sub_lsb.id,0,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    if (!p0->prv$v_syslck) {
        iss = Reset_Privs(p1);
        if (VMS_ERR(iss)) VMS_SIGNAL(SS$_NOPRIV);
        destroy_Privs(p1);
    }

    /* temp mbx's go into system table where others can find them */

    iss = define_logical("LNM$TEMPORARY_MAILBOX","LNM$SYSTEM","LNM$PROCESS_DIRECTORY");
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    /* need SYSNAM priv to write to the system table */

    p0 = Current_Privs();
    if (!p0->prv$v_sysnam) {
        p1 = new_Privs();
        p1->prv$v_sysnam = 1;
        iss = Set_Privs(p1);
        if (VMS_ERR(iss)) VMS_SIGNAL(SS$_NOPRIV);
    }


    q = translate_logical2(asciz_pSTRING(mbx1));
    if (q) {            /* left over mailbox, take it over */
        iss = sys$assign(mbx1,&ch_mbx,0,0,0);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        destroy_LNM(q);
    } else {           /* create log mailbox */
        iss = sys$crembx(0,&ch_mbx,MAXMSG,MBXQUOTA,pmask,PSL$C_USER,mbx1,0,0);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    }
    lerrlog.chan = ch_mbx;
    lerrlog.type = TYPE_ERRLOG;

    q = translate_logical2(asciz_pSTRING(mbx2));
    if (q) {            /* left over mailbox, take it over */
        iss = sys$assign(mbx2,&ch_mbx2,0,0,0);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        destroy_LNM(q);
    } else {           /* create stderr mailbox */
        iss = sys$crembx(0,&ch_mbx2,MAXMSG,MBXQUOTA,pmask,PSL$C_USER,mbx2,0,0);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    }
    lstderr.chan = ch_mbx2;
    lstderr.type = TYPE_STDERR;

    /* reset privs */

    if (!p0->prv$v_sysnam) {
        Reset_Privs(p1);
        destroy_Privs(p1);
    }
    destroy_Privs(p0);

    /* open the logfile */

    iss = openlog();
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    /* get some message blocks to begin with */

    for (j = 0; j < INITIAL_MSGS; j++) {
        iss = lib$insqhi(&init_msg[j], FreeQ);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    }

    /* set up flush timer ... also used for checking command logical*/

    sec2vms(LOGFLUSHTIME, t_expire);
    ltimer.type = TYPE_TIMER;
    iss = set_timer(&ltimer);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    /* queue the mailbox reads */

    queue_logmbx_read(&lerrlog);
    queue_logmbx_read(&lstderr);

    return SS$_NORMAL;
}


int
set_timer(pLog p)
{
    ROUTINE_NAME("set_timer");
    int iss;
    pLogMsg m;

    iss = lib$remqti(FreeQ, &m);
    if (iss == LIB$_QUEWASEMP) {
        m = malloc(sizeof(LogMsg));
        if (!m) iss = SS$_INSFMEM;
    }
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    p->msg = m;
    m->iosb.status = 0;
    m->iosb.count = 0;

    iss = sys$setimr(0, t_expire, &logger_AST, p, 0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    return SS$_NORMAL;
}



int
logentry(pLogMsg m)
{
    ROUTINE_NAME("logentry");
    int iss, length;
    static $DESCRIPTOR(d_format1,"(!8XL !%D): ");
    static $DESCRIPTOR(d_format2,"(!8XL STDERR !%D): ");
    $DESCRIPTOR(d_buffer,"");
    pSTRING pFormat;
    char header_buffer[100+2*MAXMSG];

    iss = m->iosb.status;
    if (iss != SS$_ENDOFFILE) {
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);

        pFormat = &d_format1;
        if (m->type == TYPE_STDERR) pFormat = &d_format2;

        d_buffer.dsc$a_pointer = header_buffer;
        d_buffer.dsc$w_length  = sizeof(header_buffer);
        length = 0;
        iss = sys$fao(pFormat,&length,&d_buffer,m->iosb.dvispec,0);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);

        memcpy(header_buffer+length, m->buffer, m->iosb.count);

        outrab.rab$w_rsz = length + m->iosb.count;
        outrab.rab$l_rbf = header_buffer;
        iss = sys$put(&outrab);
        if (VMS_ERR(iss)) RMS_SIGNAL(iss, outrab.rab$l_stv);
    }
    return SS$_NORMAL;

}



void
queue_logmbx_read(pLog p)
{
    ROUTINE_NAME("queue_logmbx_read");
    int iss;
    pLogMsg m;

    iss = lib$remqti(FreeQ, &m);
    if (iss == LIB$_QUEWASEMP) {
        m = malloc(sizeof(LogMsg));
        if (!m) iss = SS$_INSFMEM;
    }
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    p->msg = m;
    m->iosb.status = 0;
    m->iosb.count  = 0;

    iss = sys$qio(0,p->chan,IO$_READVBLK,&m->iosb,&logger_AST,p,m->buffer,MAXMSG,0,0,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
}

void
logger_AST(pLog p)
{
    ROUTINE_NAME("logger_AST");
    int iss;
    pLogMsg m = p->msg;

    m->type = p->type;
    if (m->iosb.count > MAXMSG) m->iosb.count = 0;
    m->buffer[m->iosb.count] = '\0';

    iss = lib$insqhi(m, MsgQ);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    p->msg = 0;
    if (p->type != TYPE_TIMER) {
        queue_logmbx_read(p);
    } else {
        set_timer(p);
    }

    iss = sys$setef(ef);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
}

int
check_for_stop()
{
    ROUTINE_NAME("check_for_stop");
    int iss, stop = 0;
    pLNM q;
    pPrivs p0, p1;

    q = translate_logical2(COMMAND_LOGICAL);

    if (q) {
        stop = strcmp(asciz_pSTRING(q->equiv),"STOP") == 0;

        if (strcmp(asciz_pSTRING(q->equiv),"NEWLOG") == 0) {
            iss = closelog();
            if (VMS_ERR(iss)) return 1;
            iss = openlog();
            if (VMS_ERR(iss)) return 1;
        }

        p0 = Current_Privs();
        if (!p0->prv$v_sysnam) {
            p1 = new_Privs();
            p1->prv$v_sysnam = 1;
            iss = Set_Privs(p1);
            if (VMS_ERR(iss)) return 1;
        }

        iss = sys$dellnm(q->table,q->name,&q->acmode);
        if (VMS_ERR(iss)) return 1;
        if (!p0->prv$v_sysnam) {
            Reset_Privs(p1);
            destroy_Privs(p1);
        }
        destroy_Privs(p0);

        destroy_LNM(q);
    }
    return stop;
}



pLNM
translate_logical2(char *name)
{
    ROUTINE_NAME("translate_logical2");
    int iss;
    $DESCRIPTOR(table,"LNM$FILE_DEV");
    pItemList i;
    word l_equ, l_tab;

    pLNM l = malloc(sizeof(LNM));
    if (!l) return 0;

    l->name  = new_STRING(name);
    l->equiv = new_STRING2(256);
    l->table = new_STRING2(256);

    i = new_ItemList(3);
    i = add_Item(i, LNM$_STRING, asciz_pSTRING(l->equiv), strlen_pSTRING(l->equiv), &l_equ);
    i = add_Item(i, LNM$_TABLE,  asciz_pSTRING(l->table), strlen_pSTRING(l->table), &l_tab);
    i = add_Item(i, LNM$_ACMODE, &l->acmode, sizeof(l->acmode), 0);

    iss = sys$trnlnm(0,&table,l->name,0,(char *)i);

    destroy_ItemList(i);
    if (iss == SS$_NOLOGNAM)
        return destroy_LNM(l);

    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    strlen_pSTRING(l->equiv) = l_equ;
    strlen_pSTRING(l->table) = l_tab;
    asciz_pSTRING(l->equiv)[l_equ] = '\0';
    asciz_pSTRING(l->table)[l_tab] = '\0';

    return l;
}



int
openlog(void)
{
    ROUTINE_NAME("openlog");
    int iss;

    /* open the log file */

    outfab = cc$rms_fab;
    outfab.fab$b_fac = FAB$M_PUT;
    outfab.fab$b_shr = FAB$M_SHRGET | FAB$M_UPI;
    outfab.fab$l_fna = logfile;
    outfab.fab$b_fns = strlen(logfile);
    outfab.fab$b_org = FAB$C_SEQ;
    outfab.fab$b_rat = FAB$M_CR;
    outfab.fab$b_rfm = FAB$C_VAR;

    iss = sys$create(&outfab);
    if (VMS_ERR(iss)) RMS_SIGNAL(iss, outfab.fab$l_stv);

    outrab = cc$rms_rab;
    outrab.rab$l_fab = &outfab;
    outrab.rab$l_rop |= RAB$M_WBH;
    outrab.rab$b_mbf = 5;

    iss = sys$connect(&outrab);
    if (VMS_ERR(iss)) RMS_SIGNAL(iss, outrab.rab$l_stv);
    return SS$_NORMAL;
}

int
closelog(void)
{
    ROUTINE_NAME("closelog");
    int iss;

    iss = sys$flush(&outrab);
    if (VMS_ERR(iss)) RMS_SIGNAL(iss, outrab.rab$l_stv);

    iss = sys$disconnect(&outrab);
    if (VMS_ERR(iss)) RMS_SIGNAL(iss, outrab.rab$l_stv);

    iss = sys$close(&outfab);
    if (VMS_ERR(iss)) RMS_SIGNAL(iss, outfab.fab$l_stv);

    return SS$_NORMAL;
}

pLNM
destroy_LNM(pLNM l)
{
    ROUTINE_NAME("destroy_LNM");
    if (!l) return 0;
    if (l->name ) destroy_STRING(l->name);
    if (l->equiv) destroy_STRING(l->equiv);
    if (l->table) destroy_STRING(l->table);
    free(l);
    return 0;
}

