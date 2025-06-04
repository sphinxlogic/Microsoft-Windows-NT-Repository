/*
    send logging information to error logger mailbox

*/
#include <ssdef.h>
#include <agndef.h>
#include <starlet.h>
#include <iodef.h>
#include <LIB$ROUTINES.H>       /* DECC */
#include <STR$ROUTINES.H>       /* DECC */
#include <lnmdef.h>
#include <libdef.h>
#include <descrip.h>
#include <builtins.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


#include "vms_data.h"
#include "errlog_client.h"
#include "util.h"

#ifdef DEBUG_MEMORY
#undef free
#undef malloc
#undef realloc
#endif

#define ERRLOG_INITIAL 0
#define ERRLOG_OPEN    1

typedef struct PendingQE    Pending;
typedef struct PendingQE*  pPending;

struct PendingQE {
    RQE  q;      /* put here to keep alignment right ! */
    pSTRING msg;
};

static RQE              PendQHead = {0,0};
#define PendQ           (&PendQHead)

static longword         active_AST  = 0;
static word             errlog_chan;
static longword         locked       = 0;
static longword         initializing = 0;
static pSTRING          prefix       = 0;
static unsigned int     log_level = 2;
static int              errlog_state = ERRLOG_INITIAL;
#define LOGMBX          "CRINOIDLOG_MBX"
static $DESCRIPTOR(d_mbx, LOGMBX);
#define MAXMSG          256
#define RETRY_DELAY     10

static void errlog_init(void);
static void errlog_AST(pPending p);

int errlog_level(unsigned int l)
{
    if (l == L_NULLTYPE) return log_level;
    errlog(0,"Changing logging level from !XL to !XL", log_level, l);
    return log_level = l;
}


static void
errlog_init(void)
{
    int iss;
    pSTRING mbx;
    $DESCRIPTOR(table,"LNM$FILE_DEV");
    ItemList itm[2];
    char equiv[256];
    word length;

    iss = _BBSSI(0,&initializing);
    if (iss == 1) return;
    if (errlog_state == ERRLOG_OPEN) goto done;

    itm[0].code                  = LNM$_STRING;
    itm[0].address               = equiv;
    itm[0].length                = sizeof(equiv);
    itm[0].return_length_address = &length;
    itm[1].code                  = 0;
    itm[1].address               = 0;
    itm[1].length                = 0;
    itm[1].return_length_address = 0;

    iss = sys$trnlnm(0,&table,&d_mbx,0,(char *)itm);
    if (iss == SS$_NOLOGNAM) goto done;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    iss = sys$assign(&d_mbx, &errlog_chan, 0, 0, AGN$M_WRITEONLY);

    if (VMS_OK(iss)) errlog_state = ERRLOG_OPEN;

    iss = _BBSSI(0,&active_AST);
    if (iss !=1) {
        iss = sys$dclast(&errlog_AST,0,0);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    }

done:
    (void) _BBCCI(0,&initializing);
    if (errlog_state == ERRLOG_OPEN)
        errlog(0,"Starting error logging at level !XL", log_level);
}


static void
errlog_AST(pPending p)
{
    int iss;
    static IOSB iosb;
    static longword time[2];
    static time_initted = 0;

    if (p) {
        if (iosb.status == SS$_NOREADER) {
            iss = lib$insqhi(p, PendQ);
            if (VMS_ERR(iss)) VMS_SIGNAL(iss);
            iss = sys$dassgn(errlog_chan);
            if (VMS_ERR(iss)) VMS_SIGNAL(iss);

            errlog_state = ERRLOG_INITIAL;
            active_AST = 0;

            if (!time_initted) {
                time_initted = 1;
                sec2vms(RETRY_DELAY, time);
            }
            iss = sys$setimr(0,time,&errlog_init,0,0);
            if (VMS_ERR(iss)) VMS_SIGNAL(iss);
            return;
        }
        if (VMS_ERR(iosb.status)) VMS_SIGNAL(iosb.status);
        free(p->msg->dsc$a_pointer);
        free(p->msg);
        free(p);
    }
    active_AST = 0;

    if (errlog_state != ERRLOG_OPEN) return;

    iss = lib$remqhi(PendQ, &p);
    if (iss == LIB$_QUEWASEMP) return;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    iss = sys$qio(0,errlog_chan,IO$_WRITEVBLK|IO$M_NOW|IO$M_NORSWAIT|IO$M_READERCHECK,
          &iosb, &errlog_AST, p, asciz_pSTRING(p->msg), strlen_pSTRING(p->msg), 0,0,0,0);

    active_AST = 1;
    if (VMS_OK(iss)) return;

    if (iss == SS$_MBFULL) {
        iss = lib$insqhi(p, PendQ);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        if (!time_initted) {
            time_initted = 1;
            sec2vms(RETRY_DELAY, time);
        }
        iss = sys$setimr(0,time,&errlog_AST,0,0);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        return;
    } else if (iss == SS$_NOREADER) {
        iss = lib$insqhi(p, PendQ);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        iss = sys$dassgn(errlog_chan);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);

        errlog_state = ERRLOG_INITIAL;
        active_AST = 0;

        if (!time_initted) {
            time_initted = 1;
            sec2vms(RETRY_DELAY, time);
        }
        iss = sys$setimr(0,time,&errlog_init,0,0);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        return;
    }
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    active_AST = 0;
    free(p->msg->dsc$a_pointer);
    free(p->msg);
    free(p);
}



int errlog(unsigned int level, char *format, ...)
{
    va_list ap;
    int iss, args, i, header, fao_param[32];
    int status = SS$_NORMAL;
    int l;
    unsigned q;
    word length1, length2;
    static $DESCRIPTOR(hcontrol1,"!+{!6XL:!SL}");
    static $DESCRIPTOR(hcontrol2,"[!AS !6XL:!SL]");
    char a_mess1[MAXMSG+1], a_mess2[MAXMSG+1];
    $DESCRIPTOR(message1,"");
    $DESCRIPTOR(message2,"");
    $DESCRIPTOR(control2,"");
    pSTRING control, message = 0;
    pPending p = 0;

    if (locked) return SS$_NORMAL;
    if (errlog_state == ERRLOG_INITIAL) {
        errlog_init();
    }
    if ((level & L_LEVMASK)  > (log_level & L_LEVMASK)) goto done;
    if ((level & L_ALLTYPES) != 0 && (level & log_level & L_ALLTYPES) == 0) goto done;

    q = level >> L_MINTYPE;
    l = level &  L_LEVMASK;
    p = malloc(sizeof(Pending));
    if (!p) goto done;

    message1.dsc$a_pointer = a_mess1;
    message1.dsc$w_length  = sizeof(a_mess1)-1;

    control = prefix ? &hcontrol2 : &hcontrol1;
    iss = sys$fao(control, &length1, &message1, prefix, q, l);
    if (VMS_ERR(iss)) {status =  iss; goto done;}
    message1.dsc$w_length = length1;
    a_mess1[length1] = '\0';

    control2.dsc$a_pointer = format;
    control2.dsc$w_length  = strlen(format);

    message2.dsc$a_pointer = a_mess2;
    message2.dsc$w_length  = sizeof(a_mess2)-1;

    va_start(ap,format);

    va_count(args);
    for (i = 0; i < args-1; i++) fao_param[i] = va_arg(ap,int);
    va_end(ap);
    iss = sys$faol(&control2, &length2, &message2, fao_param);
    if (VMS_ERR(iss)) {status = iss; goto done;}
    message2.dsc$w_length = length2;
    a_mess2[length2] = '\0';

    message = malloc(sizeof(STRING));
    if (!message) {status = SS$_INSFMEM; goto done;}
    message->dsc$w_length = message1.dsc$w_length + message2.dsc$w_length;
    message->dsc$a_pointer = malloc(message->dsc$w_length+1);
    if (!message->dsc$a_pointer) {status = SS$_INSFMEM; goto done;}
    strcpy(message->dsc$a_pointer, message1.dsc$a_pointer);
    strcat(message->dsc$a_pointer, message2.dsc$a_pointer);

    p->msg = message;
    iss = lib$insqti(p,PendQ);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    iss = _BBSSI(0,&active_AST);
    if (iss !=1) {
        iss = sys$dclast(&errlog_AST,0,0);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    }

done:
    if (VMS_ERR(status)) {
        if (message) free(message);
        if (p) free(p);
    }
    return status;
}


int
islogging(unsigned int level)
{
    if (locked) return 0;
    if (errlog_state != ERRLOG_OPEN) return 0;
    if ((level & L_LEVMASK)  > (log_level & L_LEVMASK)) return 0;
    if ((level & L_ALLTYPES) != 0 && (level & log_level & L_ALLTYPES) == 0) return 0;
    return 1;
}


#define WANT_MSG_TEXT
#include "msg.h"

char *
msg_text(int code)
{
    int j;
    static char unknown[] = "Unknown MSG code";
    if (code <= 0 || code >= MSG$_MAXMSG) return unknown;

    for (j = 0; j < sizeof(messages)/sizeof(struct MSGTEXT); j++) {
        if (messages[j].code == code) return messages[j].text;
    }
    return unknown;
}

#define TOHEX(i) ((i) < 0x0a ? '0'+(i) : 'A' + (i) - 0x0A)

int
logdump(int level, char *header, void *buf, int size)
{
    int j, l;
    char c, *p, *pb, cbuf[17], bbuf[60];

    if (locked) return SS$_NORMAL;
    if (errlog_state == ERRLOG_INITIAL)
        errlog_init();
    if (errlog_state != ERRLOG_OPEN)
        return SS$_SHUT;
    if ((level & L_LEVMASK)  > (log_level & L_LEVMASK)) return 1;
    if ((level & L_ALLTYPES) != 0 && (level & log_level & L_ALLTYPES) == 0) return 1;

    pb = (char *) buf;
    while (size > 0) {
        l = (size > 16)? 16 : size;

        p = bbuf;
        for (j = 0; j < l; j++) {
            c = *pb++;
            *p++ = TOHEX((c>>4) & 0x0F);
            *p++ = TOHEX(c & 0x0F);
            *p++ = ' ';
            cbuf[j] = (c == 0)? '.' : c;
        }
        *p++ = '\0';
        cbuf[l] = '\0';
        printable(cbuf);
        errlog(level,"!AZ [!AZ] |!AZ|", header, bbuf, cbuf);

        size -= l;
    }
    return SS$_NORMAL;
}



void
errlog_lock(void)
{
    locked = 1;
}

void
errlog_unlock(void)
{
    locked = 0;
}


void
errlog_prefix(char *s)
{
    if (prefix) prefix = destroy_STRING(prefix);
    prefix = new_STRING(s);
}


void
errlog_flush(void)
{
    int iss;
    pPending p;

    if (errlog_state != ERRLOG_OPEN) return;

    while (1) {
        iss = lib$remqhi(PendQ, &p);
        if (iss == LIB$_QUEWASEMP) return;
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);

        iss = sys$qio(0,errlog_chan,IO$_WRITEVBLK|IO$M_NOW|IO$M_NORSWAIT|IO$M_READERCHECK,
              0, 0, 0, asciz_pSTRING(p->msg), strlen_pSTRING(p->msg), 0,0,0,0);
    }
}


