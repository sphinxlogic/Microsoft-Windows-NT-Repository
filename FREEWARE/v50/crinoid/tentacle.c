/*
    A single tentacle of the CRINOID
*/

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stat.h>
#include <signal.h>

#include <ssdef.h>
#include <starlet.h>
#include <libdef.h>
#include <lnmdef.h>
#include <iodef.h>
#include <descrip.h>
#include <psldef.h>
#include <jpidef.h>
#include <dvidef.h>
#include <impdef.h>
#include <LIB$ROUTINES.H>       /* DECC */

#include "EXTERN.h"
#include "perl.h"

#include "mbxq.h"
#include "pipe2.h"
#include "msg.h"
#include "util.h"
#include "tentacle.h"
#include "perlxsi.c"
#include "errlog_client.h"

int decc$stat(char *file, struct stat *buffer);

static longword             MasterPID = 0;
static int                  Heartbeats_Pending = 0;
static int                  work_in_progress;
static pENV                 EnvBase = 0;
static pSCRIPT              Script_Base = NULL;
static int                  Script_Max  = 5;
static int                  Script_Count= 0;
static pMbxQ                StatusQ;
static pMbxQ                CommandQ;
static longword             StatusEF;
static RQE                  GotMsgQueueHead = {0, 0};
#define GotMsgQ             (&GotMsgQueueHead)
static PerlInterpreter      *script_perl = 0;
#define MASTER_SCRIPT       "CRINOID_home:oyster."
#define MASTER_OPT1         "-ICRINOID_root:[lib]"
#define HEARTBEAT_TIMEOUT   30
#define DEFAULT_LOG_LEVEL   L_ERROR



/*
 *  status goes back to a mailbox on SYS$OUTPUT
 *      note that we don't have an input, until we create one...
 */

int main(int argc, char** argv, char** env)
{
    ROUTINE_NAME("Tentacle_main");
    int iss, net_shut, need_init = 1;
    longword time[2];
    pMbxE e;
    pMM m;
    pMBX commandMBX;
    pMBX statusMBX;
    pSTRING loglev;

    errlog_prefix("TENTACLE");
    errlog(L_CRITICAL,"starting up");

    loglev = translate_logical("TENTACLE_LOGLEVEL");
    errlog_level(loglev ? atoi(asciz_pSTRING(loglev)) : DEFAULT_LOG_LEVEL);
    if (loglev) destroy_STRING(loglev);

    signal(SIGABRT, SIG_IGN);

    statusMBX = assign_new_MBX("SYS$OUTPUT",0);
    errlog(L_BABBLE,"Status MBX  opened, unit !UW",statusMBX->unit);
    if (!statusMBX) VMS_SIGNAL(SS$_INSFMEM);
    StatusQ = MbxQ_new(statusMBX, MBXQ$_WRITE|MBXQ$M_RETRY, &status_AST);
    if (!StatusQ) VMS_SIGNAL(SS$_INSFMEM);

    commandMBX= new_MBX(0, MAX_MSG);
    errlog(L_BABBLE,"Command MBX opened, unit !UW",commandMBX->unit);

    iss = lib$get_ef(&StatusEF);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = sys$clref(StatusEF);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = sec2vms(30, time);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    init_ENV();
    init_perl();
    need_init = 0;

    iss = sys$setimr(StatusEF, time, 0, 0, 0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    send_status(MSG$_DEVONLIN,commandMBX->unit,0,0);

    iss = sys$waitfr(StatusEF);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    errlog(L_BABBLE,"Response from DEVONLINE, MasterPID:!XL",MasterPID);
    if (MasterPID == 0) return SS$_ABORT;

    CommandQ = MbxQ_new(commandMBX, MBXQ$_READ, &command_AST);
    if (!CommandQ) VMS_SIGNAL(SS$_INSFMEM);

    errlog(L_BABBLE,"starting to process commands");

    heartbeat_AST();
    net_shut = 0;

    while (!net_shut) {
        iss = lib$remqhi(GotMsgQ, &e);
        if (iss == LIB$_QUEWASEMP) {
            errlog(L_BABBLE,"Entering hibernation at !%T",0);
            iss = sys$hiber();
            if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        } else if (VMS_ERR(iss)) {
            errlog(L_ERROR,"Error on command dequeue, status=!XL",iss);
            VMS_SIGNAL(iss);
        } else {
            m = (pMM) e->buf;
            errlog(L_BABBLE,"Message, code = !UL (!AZ) from PID=!XL",m->code,msg_text(m->code),e->iosb.dvispec);
            if (e->iosb.dvispec != MasterPID) {
                errlog(L_ERROR,"Unsolicited message from PID=!XL",e->iosb.dvispec);
            } else {
                switch (m->code) {
                    case MSG$_CONNECT:
                        if (need_init) init_perl();
                        need_init = 1;
                        handle_connection();
                        init_ENV();
                        send_status(MSG$_IDLE,commandMBX->unit,0,0);
                        break;
                    case MSG$_ABORT:
                        errlog(L_INFO,"ABORT message received");
                        init_ENV();
                        send_status(MSG$_IDLE,commandMBX->unit,0,0);
                        break;
                    case MSG$_NETSHUT:
                        errlog(L_INFO,"NETSHUT message received");
                        net_shut = 1;
                        break;
                    case MSG$_ENVDATA:
                        do_envdata(e);
                        break;
                    case MSG$_ERRLOG:
                        do_errlog(e);
                        break;
                    case MSG$_LOGLEV:
                        do_loglev(e);
                        break;
                }
            }
            MbxQ_dispose(e);
        }
    }
    return SS$_NORMAL;
}




void
command_AST(pMbxE e)
{
    ROUTINE_NAME("command_AST");
    int iss;
    word chan;
    pMM m = (pMM) e->buf;

    errlog(L_BABBLE,"Command_AST fired, iss=0x!XW code=!UW (!AZ)",e->iosb.status, m->code, msg_text(m->code));

    iss = e->iosb.status;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);      /* I guess */

    switch (m->code) {
            case MSG$_LUBDUB     :
                Heartbeats_Pending--;
                MbxQ_dispose(e);
                return;
            case MSG$_ABORT      :
                if (work_in_progress) {
                    errlog(L_ERROR,"SIGABRT raised");
                    raise(SIGABRT);
                }
                break;
            case MSG$_DISCON     :
            case MSG$_EXIT       :
            case MSG$_PATHLOST   :
            case MSG$_PROTOCOL   :
            case MSG$_THIRDPARTY :
            case MSG$_TIMEOUT    :
            case MSG$_NETSHUT    :
                VMS_SIGNAL(SS$_ABORT);
    }

    iss = lib$insqti(e, GotMsgQ);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    iss = sys$wake(0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
}


int
send_status(word code, word unit, char *s, int n)
{
    ROUTINE_NAME("send_status");
    int iss;
    MM M;
    int length = sizeof(M.code) + sizeof(M.unit);

    M.code = code;
    M.unit = unit;
    errlog(L_BABBLE,"returning status code=!UW (!AZ) unit=!UW",code, msg_text(code), unit);

    if (s && n > 0 && n < 255) {
        M.info[0] = n;
        memcpy(&M.info+1, s, n);
        length += n + 1;
    }

    return MbxQ_write(StatusQ, &M, length);
}

void
status_AST(pMbxE e)
{
    ROUTINE_NAME("status_AST");
    if (MasterPID == 0) {
        MasterPID = e->iosb.dvispec;
        sys$setef(StatusEF);
    }
}


int
handle_connection()
{
    ROUTINE_NAME("handle_connection");
    pENV e;
    char dev[100];
    pMBX m_in = 0, m_out = 0;
    pPipe2 p_stdin  = 0;
    pPipe2 p_stdout = 0;
    pPipe2 p_stderr = 0;

    errlog(L_INFO,"CONNECT message received, running Perl script");

    e = find_ENV("CRINOID:>PERL");
    if (!e) {
        tu_strcpy(dev, "NL:");
    } else {
        m_in   = assign_new_MBX(e->value,0);
        if (!m_in) goto done;
        m_out  = new_MBX(0,m_in->size);
        p_stdin = Pipe_new(m_in,m_out);
        sprintf(dev,"_MBA%d:",m_out->unit);
    }
    freopen(dev,"rb",stdin);
    errlog(L_BABBLE,"stdin remapped to !AZ", dev);
    m_in = m_out = 0;

    e = find_ENV("CRINOID:<PERL");
    if (!e) {
        tu_strcpy(dev, "NL:");
    } else {
        m_out    = assign_new_MBX(e->value,0);
        if (!m_out) goto done;
        m_in     = new_MBX(0,m_out->size);
        p_stdout = Pipe_new(m_in,m_out);
        Pipe_pack(p_stdout);
        sprintf(dev,"_MBA%d:",m_in->unit);
    }
    freopen(dev,"wb",stdout,"ctx=bin");
    errlog(L_BABBLE,"stdout remapped to !AZ", dev);
    m_in = m_out = 0;

    e = find_ENV("CRINOID:2<PERL");
    if (!e) {
        tu_strcpy(dev, "NL:");
    } else {
        m_out    = assign_new_MBX(e->value,0);
        m_in     = new_MBX(0,m_out->size);
        p_stderr = Pipe_new(m_in,m_out);
        sprintf(dev,"_MBA%d:",m_in->unit);
    }
    freopen(dev,"wb",stderr,"ctx=bin");
    errlog(L_BABBLE,"stderr remapped to !AZ", dev);
    m_in = m_out = 0;

    setbuf(stdin,NULL);
    /*setbuf(stdout,NULL);  */
    setbuf(stderr,NULL);
    clearerr(stdin);

    work_in_progress = 1;
    exec_perl();
    signal(SIGABRT, SIG_IGN);
    work_in_progress = 0;

    fflush(stdout);
    fsync(fileno(stdout));
    Pipe_flush(p_stdout);
    printf("</DNETCGI>");

    fflush(stdout);
    fsync(fileno(stdout));
    fflush(stderr);
    fsync(fileno(stderr));

done:
    if (m_in)    destroy_MBX(m_in);
    if (m_out)    destroy_MBX(m_out);
    if (p_stdin) Pipe_destroy(p_stdin);
    if (p_stdout) Pipe_destroy2(p_stdout);
    if (p_stderr) Pipe_destroy2(p_stderr);
    return 1;
}

int
init_perl(void)
{
    ROUTINE_NAME("init_perl");
    int iss, curscripttime;
    char *embed_argv[] = {0, MASTER_OPT1, MASTER_SCRIPT};
    static int scripttime = -1;
    struct stat st;

    curscripttime = (decc$stat(MASTER_SCRIPT, &st) != -1) ? st.st_mtime : 0;
    if (script_perl && curscripttime == scripttime) return SS$_NORMAL;
    scripttime = curscripttime;

    if (script_perl) {
        errlog(L_INFO,"Master script changed, reloading perl interpreter");
        perl_destruct(script_perl);
        perl_free(script_perl);
    }
    script_perl = perl_alloc();
    perl_construct(script_perl);
    if (!script_perl) VMS_SIGNAL(SS$_INSFMEM);

    iss = perl_parse(script_perl, xs_init,
        sizeof(embed_argv)/sizeof(char *), embed_argv, NULL);
    if (iss != 0 && VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = perl_run(script_perl);
    return iss;
}


int
exec_perl(void)
{
    ROUTINE_NAME("exec_perl");
    int iss;
    pENV e1, e2;
    char *args[] = {0, 0, 0};

    e1 = find_ENV("SCRIPT_BINDIR");
    e2 = find_ENV("SCRIPT_BARE_NAME");
    if (e1) args[0] = e1->value;
    if (e2) args[1] = e2->value;
    errlog(L_BABBLE,"running !AZ/!AZ",e1->value,e2->value);

    construct_perlENV();
    iss = perl_call_argv("main::RunScript",G_DISCARD | G_EVAL, args);
    return 1;
}


void
send_perlENV(char *name, char *value)
{
    ROUTINE_NAME("send_perlENV");
    dSP;
    PUSHMARK(sp);
    XPUSHs(sv_2mortal(newSVpv(name,0)));
    XPUSHs(sv_2mortal(newSVpv(value,0)));
    PUTBACK;
    perl_call_pv("SetSafeENV",G_DISCARD);
    errlog(L_BABBLE, "$PerlENV{'!AZ'} = '!AZ'",name,value);
}

int
construct_perlENV(void)
{
    ROUTINE_NAME("construct_perlENV");
    pENV e = EnvBase;

    while (e) {
        if (e->state) {
            send_perlENV(e->name,e->value);
        }
        e = e->next;
    }
    return 1;
}

pENV
add_ENV(char *name, char *value, int state)
{
    ROUTINE_NAME("add_ENV");
    char *qp;
    pENV p;

    if (p = find_ENV(name)) {
        qp = p->value;
        p->value = malloc(strlen(value)+1);
        if (!p->value) {
            if (qp) free(qp);
            return 0;
        }
        strcpy(p->value,value);
        if (qp) free(qp);
    } else {
        p = (pENV) malloc(sizeof(ENV));
        if (!p) return 0;
        p->name = malloc(strlen(name)+1);
        if (!p->name) {
            free(p);
            return 0;
        }
        strcpy(p->name, name);

        p->value = malloc(strlen(value)+1);
        if (!p->value) {
            if (p->name) free(p->name);
            free(p);
            return 0;
        }
        strcpy(p->value,value);
        p->next = EnvBase;
        EnvBase = p;
    }
    p->state = state;
    return p;
}

pENV
find_ENV(char *name)
{
    ROUTINE_NAME("find_ENV");
    pENV p = EnvBase;

    while (p) {
        if (strcmp(p->name,name) == 0) return p;
        p = p->next;
    }
    return 0;
}

void
clear_ENV(void)
{
    ROUTINE_NAME("clear_ENV");
    pENV p, qp = EnvBase;


    while (p=qp) {
        qp = p->next;
        if (p->name) free(p->name);
        if (p->value) free(p->value);
        free(p);
    }
    EnvBase = 0;
}

void
init_ENV(void)
{
    ROUTINE_NAME("init_ENV");
    int j;
    static char *default_blank[] = {
        "SUBFUNCTION",
        "SERVER_SOFTWARE", "SERVER_NAME", "SERVER_PROTOCOL",
        "SERVER_PORT", "REQUEST_METHOD", "PATH_INFO",
        "PATH_TRANSLATED", "SCRIPT_NAME", "SCRIPT_PATH",
        "QUERY_STRING", "REMOTE_USER", "REMOTE_ADDR", "REMOTE_PORT",
        "REMOTE_HOST", "AUTH_TYPE", "REMOTE_IDENT", "CONTENT_TYPE",
        "CONTENT_LENGTH", NULL};

    clear_ENV();

    for (j = 0; default_blank[j] != NULL; j++) {
        add_ENV(default_blank[j],"",0);
    }

    add_ENV("GATEWAY_INTERFACE", "CGI/1.0", 1);
}

void
dump_ENV(FILE *fp)
{
    ROUTINE_NAME("dump_ENV");
    pENV p = EnvBase;

    while (p) {
        fprintf(fp,"Env{\"%s\"}(%d) = \"%s\"\n",p->name,p->state,p->value);
        p = p->next;
    }
}


void
do_errlog(pMbxE e)
{
    ROUTINE_NAME("do_errlog");

}

void
do_loglev(pMbxE e)
{
    ROUTINE_NAME("do_loglev");
    pMM m = (pMM) e->buf;
    unsigned level;

    memcpy(&level, m->info, sizeof(level));

    errlog_level(level);
}


void
do_envdata(pMbxE e)
{
    ROUTINE_NAME("do_envdata");
    pMM  m     = (pMM) e->buf;
    char *s    = m->info;
    char *send = (char *) m + e->iosb.count;
    char *p, *q;
    int n;
    static char *cname, *cval;
    static int   lname,  lval, doing_name;

    if (m->unit == 0) {
        init_ENV();
        doing_name = 1;
        cname = malloc(1); *cname = '\0';
        cval  = malloc(1); *cval  = '\0';
        lname = lval = 0;
    }

    while (s < send) {
        p = s;
        while (*p && p < send) p++;
        n = p-s;
        if (doing_name) {
            q = malloc(lname+n+1);
            strcpy(q,cname);
            free(cname);
            cname = q;
            strncat(cname,s,n);
            cname[n] = '\0';
            lname += n;
            if (!*p) doing_name = 0;
        } else {
            q = malloc(lval+n+1);
            strcpy(q, cval);
            free(cval);
            cval = q;
            strncat(cval,s,n);
            cval[n] = '\0';
            lval += n;
            if (!*p) {
                add_ENV(cname,cval,1);
                free(cname);
                free(cval);
                cname = malloc(1); *cname = '\0';
                cval  = malloc(1); *cval  = '\0';
                lname = lval = 0;
                doing_name = 1;
            }
        }
        s = p + 1;
    }
}



void
heartbeat_AST(void)
{
    ROUTINE_NAME("heartbeat_AST");
    int iss;
    static int initialized = 0;
    static longword t_expire[2];

    if (Heartbeats_Pending > 0) {
        errlog(L_CRITICAL,"Heartbeat stopped, exiting");
        fake_netshut();
        return;
    }

    if (!initialized) {
        sec2vms(HEARTBEAT_TIMEOUT, t_expire);
        initialized = 1;
    }

    iss = sys$setimr(0,t_expire, &heartbeat_AST, 0, 0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    Heartbeats_Pending++;
    send_status(MSG$_HEARTBEAT,0,0,0);
}

void
fake_netshut(void)
{
    ROUTINE_NAME("fake_netshut");
    int iss;
    static MbxE E;
    pMbxE e = &E;
    pMM   m = (pMM) e->buf;

    e->iosb.dvispec = MasterPID;
    m->code = MSG$_NETSHUT;

    iss = lib$insqhi(e, GotMsgQ);
    iss = sys$wake(0,0);
}
