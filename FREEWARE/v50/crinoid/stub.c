/***********************************************************************\
 *                                                                     *
 *  STUB                                                               *
 *      This is a "stub" detached process, used to impersonate         *
 *      a user sufficiently to start the REAL detached process         *
 *      in their account.                                              *
 *                                                                     *
 *      communicates with CRINOID to set process parameters.           *
 *                                                                     *
\***********************************************************************/

#ifndef NO_PERSONA
#define HAS_PERSONA
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include <ssdef.h>
#include <iodef.h>
#include <starlet.h>
#include <lib$routines.h>
#include <jpidef.h>
#include <prcdef.h>
#include <impdef.h>
#include <uaidef.h>

#include "util.h"
#include "msg.h"

#define VMS_OK(s)    (((s)&1)!=0)
#define VMS_ERR(s)   (((s)&1)==0)

void     set_mortality(int timeout);
void     death_AST(void);
#ifndef HAS_PERSONA
void     set_username(pSTRING user);
pSTRING  get_username(void);
longword getuic(pSTRING username);
#endif



static pMBX             commandMBX;
static pMBX             statusMBX;
#define MAX_LIFETIME    (5*60)          /* 5 MINUTES */


int
main(void)
{
    ROUTINE_NAME("Stub_main");
    int iss, len;
    struct MBXMSG msg;
    IOSB iosb;
    longword myPID, MasterPID, procPID;
    Privs mypriv, procpriv;
    longword procflags = 0, baspri = 2, persona, uic = 0;
    char * quota = 0;
    pSTRING username, procname = 0, loginout, program, output, error;
    word termMBX = 0;
    char myname[16];
    pSTRING my_username;

    myPID = get_pid();
    sprintf(myname,"STUB%08x",myPID);
    iss = sys$setprn(new_STRING(myname));

    loginout = new_STRING("SYS$SYSTEM:LOGINOUT.EXE");
    error    = new_STRING("");
    procpriv.prv$l_l1_bits = 0;
    procpriv.prv$l_l2_bits = 0;
    procpriv.prv$v_netmbx = 1;
    procpriv.prv$v_tmpmbx = 1;

    statusMBX = assign_new_MBX("SYS$OUTPUT",0);
    if (!statusMBX) VMS_SIGNAL(SS$_INSFMEM);
    output  = &statusMBX->d_name;

    commandMBX= new_MBX(0, MAX_MSG);

    set_mortality(MAX_LIFETIME);
    msg.code = MSG$_STUBREADY;
    msg.unit = commandMBX->unit;
    iss = sys$qiow(0, statusMBX->chan, IO$_WRITEVBLK, &iosb, 0, 0, &msg, 4, 0, 0, 0, 0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = iosb.status;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    MasterPID = iosb.dvispec;

    while (1) {
        iss = sys$qiow(0, commandMBX->chan, IO$_READVBLK, &iosb, 0, 0, &msg, sizeof(msg), 0, 0, 0, 0);
        if (VMS_ERR(iss)) break;
        iss = iosb.status;
        if (VMS_ERR(iss)) break;
        if (iosb.dvispec == MasterPID) {
            len = iosb.count - 4 ;
            switch (msg.code) {
                case MSG$_NETSHUT:
                case MSG$_ABORT:
                    iss = SS$_ABORT;
                    VMS_SIGNAL(iss);
                    goto done;

                case MSG$_USERNAME:
                    username = new_STRING2(len);
                    strncpy(asciz_pSTRING(username),msg.info,len);
                    asciz_pSTRING(username)[len] = 0;
#ifndef HAS_PERSONA
                    uic = getuic(username);
#endif
                    break;

                case MSG$_PROCPRIVS:
                    memcpy(&procpriv, msg.info, sizeof(longword));
                    break;

                case MSG$_PROCQUOTA:
                    quota = malloc(len);
                    memcpy(quota, msg.info, len);
                    break;

                case MSG$_PROCFLAGS:
                    memcpy(&procflags, msg.info, sizeof(longword));
                    break;

                case MSG$_PROCNAME:
                    procname = new_STRING2(len);
                    strncpy(asciz_pSTRING(procname),msg.info,len);
                    asciz_pSTRING(procname)[len] = 0;
                    break;

                case MSG$_PROCPROG:
                    program = new_STRING2(len);
                    strncpy(asciz_pSTRING(program),msg.info,len);
                    asciz_pSTRING(program)[len] = 0;
                    break;

                case MSG$_PROCPRIOR:
                    memcpy(&baspri, msg.info, sizeof(longword));
                    break;

                case MSG$_PROCTMBX:
                    memcpy(&termMBX, msg.info, sizeof(word));
                    break;

                case MSG$_PROCSTART:
                    procflags |= PRC$M_DETACH;

                    mypriv.prv$l_l1_bits = 0;
                    mypriv.prv$l_l2_bits = 0;
                    mypriv.prv$v_detach = 1;
                    mypriv.prv$v_sysprv = 1;
#ifndef HAS_PERSONA
                    mypriv.prv$v_cmkrnl = 1;
#endif
                    iss = sys$setprv(1,&mypriv,0,0);
                    if (VMS_ERR(iss))
                    {
                        VMS_SIGNAL(iss);
                        goto done;
                    }

#ifdef HAS_PERSONA
#if (__VMS_VER >= 72000000 && defined(__ALPHA))
                    iss = sys$persona_create(&persona, username, 0, 0, 0);
#else
                    iss = sys$persona_create(&persona, username, 0);
#endif
                    if (VMS_ERR(iss))
                    {
                        VMS_SIGNAL(iss);
                        goto done;
                    }

#if (__VMS_VER >= 72000000 && defined(__ALPHA))
                    iss = sys$persona_assume(&persona, IMP$M_ASSUME_SECURITY|IMP$M_ASSUME_ACCOUNT|IMP$M_ASSUME_JOB_WIDE,0,0);
#else
                    iss = sys$persona_assume(&persona, IMP$M_ASSUME_SECURITY|IMP$M_ASSUME_ACCOUNT|IMP$M_ASSUME_JOB_WIDE);
#endif
                    if (VMS_ERR(iss))
                    {
                        VMS_SIGNAL(iss);
                        goto done;
                    }
#else
                    my_username = get_username();
                    set_username(username);
#endif
                    mypriv.prv$l_l1_bits = 0;
                    mypriv.prv$l_l2_bits = 0;
                    mypriv.prv$v_detach = 1;
                    iss = sys$setprv(1,&mypriv,0,0);
                    if (VMS_ERR(iss))
                    {
                        VMS_SIGNAL(iss);
                        goto done;
                    }

                    iss = sys$creprc(&procPID,
                                     loginout,                  /* image  */
                                     program,                   /* input  */
                                     output,                    /* output */
                                     error,                     /* error  */
                                     &procpriv,                 /* privs  */
                                     quota,                     /* quotas */
                                     procname,                  /* name */
                                     baspri,                    /* prior */
                                     uic,                       /* uic */
                                     termMBX,                   /* termmbxunit*/
                                     procflags);                /* flags */

                    if (VMS_ERR(iss))
                    {
                        VMS_SIGNAL(iss);
                        goto done;
                    }

#ifdef HAS_PERSONA
#if (__VMS_VER >= 72000000 && defined(__ALPHA))
                    persona = ISS$C_ID_NATURAL;
                    iss = sys$persona_assume(&persona, 0, 0, 0);
#else
                    persona = 1;
                    iss = sys$persona_assume(&persona, 0);
#endif
#else
                    mypriv.prv$l_l1_bits = 0;
                    mypriv.prv$l_l2_bits = 0;
                    mypriv.prv$v_cmkrnl = 1;
                    iss = sys$setprv(1,&mypriv,0,0);
                    if (VMS_ERR(iss))
                    {
                        VMS_SIGNAL(iss);
                        goto done;
                    }

                    set_username(my_username);
#endif
                    msg.code = MSG$_STARTPID;
                    msg.unit = 0;
                    memcpy(msg.info, &procPID, sizeof(longword));
                    iss = sys$qiow(0, statusMBX->chan, IO$_WRITEVBLK, &iosb, 0, 0, &msg, 4+sizeof(longword), 0, 0, 0, 0);
                    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
                    goto done;
            }
        }
    }
done:
    return iss;
}


void
set_mortality(int timeout)
{
    ROUTINE_NAME("set_mortality");
    int iss;
    longword t_expire[2];

    sec2vms(timeout, t_expire);
    iss = sys$setimr(0,t_expire, &death_AST, 0, 0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
}

void
death_AST(void)
{
    ROUTINE_NAME("death_AST");
    int iss;
    iss = sys$delprc(0,0);
    iss = sys$cancel(commandMBX->chan);
}


#ifndef HAS_PERSONA

#include "pcbjibdef.h"

#pragma message save
#pragma message disable (GLOBALEXT)
globalvalue char **   CTL$GL_PCB;
globalvalue char *    CTL$T_USERNAME;
#pragma message restore


pSTRING
get_username(void)
{
    ROUTINE_NAME("get_username");
    pItemList i;
    IOSB iosb;
    char buf[12];
    int j, iss;

    i = new_ItemList(1);
    i = add_Item(i, JPI$_USERNAME, buf, 12, 0);
    iss = sys$getjpiw(0,0,0,i,&iosb,0,0);
    destroy_ItemList(i);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = iosb.status;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    for (j = 11; j >= 0 && buf[j] == ' '; j--)
        buf[j] = '\0';

    return new_STRING(buf);
}

longword
getuic(pSTRING user)
{
    ROUTINE_NAME("getuic");
    pSTRING username;
    pItemList i;
    IOSB iosb;
    longword uic;
    int iss;

    username = new_STRING(asciz_pSTRING(user));
    uc(asciz_pSTRING(username));

    i = new_ItemList(1);
    i = add_Item(i, UAI$_UIC, &uic, sizeof(uic), 0);
    iss = SYS$GETUAI(0,0,username, i, 0,0,0);
    destroy_ItemList(i);
    destroy_STRING(username);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    return uic;
}

char username_global[JIB$S_USERNAME];
int  ustore(void);

void
set_username(pSTRING user)
{
    ROUTINE_NAME("set_username");
    int j, iss;
    char *p;

    for (j = 0; j < 12; j++) username_global[j] = ' ';

    p = asciz_pSTRING(user);
    for (j = 0; j < JIB$S_USERNAME && *p; j++, p++) {
        username_global[j] = toupper(*p);
    }

    iss = SYS$CMKRNL(&ustore,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

}

int
ustore(void)
{
    ROUTINE_NAME("ustore");
    char *pPCB, *pJIB;
    char *jibuser;
    char *pCTL;
    int j;

    pPCB = *CTL$GL_PCB;
    memcpy(&pJIB, pPCB+PCB$L_JIB, sizeof(char *));
    jibuser = pJIB+JIB$T_USERNAME;
    pCTL = CTL$T_USERNAME;

    memcpy(jibuser,username_global,JIB$S_USERNAME);
    memcpy(pCTL,username_global,JIB$S_USERNAME);

    return 1;
}

#endif
