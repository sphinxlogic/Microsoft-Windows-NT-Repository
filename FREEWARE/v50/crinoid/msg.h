/**************************************************************************\
 *                                                                        *
 *  MAILBOX MESSAGE CODES                                                 *
 *                                                                        *
 *                                                                        *
\**************************************************************************/
#ifndef __MSG_H
#define __MSG_H

#include <msgdef.h>

#define  MSG$_BUSY         1024
#define  MSG$_ENVDATA      (MSG$_BUSY+1)
#define  MSG$_IDLE         (MSG$_BUSY+2)
#define  MSG$_LOGLEV       (MSG$_BUSY+3)
#define  MSG$_ERRLOG       (MSG$_BUSY+4)
#define  MSG$_HEARTBEAT    (MSG$_BUSY+5)
#define  MSG$_LUBDUB       (MSG$_BUSY+6)
#define  MSG$_STUBREADY    (MSG$_BUSY+7)
#define  MSG$_USERNAME     (MSG$_BUSY+8)
#define  MSG$_PROCPRIVS    (MSG$_BUSY+9)
#define  MSG$_PROCQUOTA    (MSG$_BUSY+10)
#define  MSG$_PROCFLAGS    (MSG$_BUSY+11)
#define  MSG$_STARTPID     (MSG$_BUSY+12)
#define  MSG$_PROCSTART    (MSG$_BUSY+13)
#define  MSG$_PROCPROG     (MSG$_BUSY+14)
#define  MSG$_PROCPRIOR    (MSG$_BUSY+15)
#define  MSG$_PROCTMBX     (MSG$_BUSY+16)
#define  MSG$_PROCNAME     (MSG$_BUSY+17)
#define  MSG$_PENDABORT    (MSG$_BUSY+18)
#define  MSG$_LOGQLOCK     (MSG$_BUSY+19)
#define  MSG$_LOGSTART     (MSG$_BUSY+20)
/* this must come last !! */
#define  MSG$_MAXMSG       (MSG$_BUSY+19)

#ifdef WANT_MSG_TEXT
struct MSGTEXT {
    int code;
    char *text;
};

#define MSGDEF(c,t)  {c,t}

static struct MSGTEXT messages[] = {
    MSGDEF(MSG$_DELPROC   , "DELETE PROCESS"),
    MSGDEF(MSG$_DEVOFFLIN , "DEVICE OFFLINE"),
    MSGDEF(MSG$_TRMHANGUP , "TERMINAL HANG UP"),
    MSGDEF(MSG$_DEVONLIN  , "DEVICE ONLINE"),
    MSGDEF(MSG$_ABORT     , "PARTNER ABORTED LINK"),
    MSGDEF(MSG$_CONFIRM   , "CONNECT CONFIRM"),
    MSGDEF(MSG$_CONNECT   , "INBOUND CONNECT INITIATE"),
    MSGDEF(MSG$_DISCON    , "PARTNER DISCONNECTED - HANGUP"),
    MSGDEF(MSG$_EXIT      , "PARTNER EXITED PREMATURELY"),
    MSGDEF(MSG$_INTMSG    , "INTERRUPT MESSAGE - UNSOLICITED DATA"),
    MSGDEF(MSG$_PATHLOST  , "NFW - PATH LOST TO PARTNER"),
    MSGDEF(MSG$_PROTOCOL  , "PROTOCOL ERROR"),
    MSGDEF(MSG$_REJECT    , "CONNECT REJECT"),
    MSGDEF(MSG$_THIRDPARTY, "THIRD PARTY DISCONNECT"),
    MSGDEF(MSG$_TIMEOUT   , "CONNECT TIMEOUT"),
    MSGDEF(MSG$_NETSHUT   , "Network shutting down"),
    MSGDEF(MSG$_NODEACC   , "Node has become accessible"),
    MSGDEF(MSG$_NODEINACC , "Node has become inaccessible"),
    MSGDEF(MSG$_EVTAVL    , "Events are available to EVL"),
    MSGDEF(MSG$_EVTRCVCHG , "Event receiver database change"),
    MSGDEF(MSG$_INCDAT    , "X25 INCOMING DATA"),
    MSGDEF(MSG$_RESET     , "X25 CIRCUIT RESET"),

    MSGDEF(MSG$_BUSY     , "BUSY"),
    MSGDEF(MSG$_ENVDATA  , "ENVDATA"),
    MSGDEF(MSG$_IDLE     , "IDLE"),
    MSGDEF(MSG$_LOGLEV   , "LOGLEV"),
    MSGDEF(MSG$_ERRLOG   , "ERRLOG"),
    MSGDEF(MSG$_HEARTBEAT, "HEARTBEAT"),
    MSGDEF(MSG$_LUBDUB   , "LUBDUB"),
    MSGDEF(MSG$_STUBREADY, "STUBREADY"),
    MSGDEF(MSG$_USERNAME , "USERNAME"),
    MSGDEF(MSG$_PROCPRIVS, "PROCPRIVS"),
    MSGDEF(MSG$_PROCQUOTA, "PROCQUOTA"),
    MSGDEF(MSG$_PROCFLAGS, "PROCFLAGS"),
    MSGDEF(MSG$_STARTPID , "STARTPID"),
    MSGDEF(MSG$_PROCSTART, "PROCSTART"),
    MSGDEF(MSG$_PROCPROG , "PROCPROG"),
    MSGDEF(MSG$_PROCPRIOR, "PROCPRIOR"),
    MSGDEF(MSG$_PROCTMBX , "PROCTMBX"),
    MSGDEF(MSG$_PROCNAME , "PROCNAME"),
    MSGDEF(MSG$_PENDABORT, "PENDABORT"),
    MSGDEF(MSG$_LOGQLOCK , "LOGQLOCK"),
    MSGDEF(MSG$_LOGSTART , "LOGSTART")
};

#endif /*WANT_MSG_TEXT*/


#endif
