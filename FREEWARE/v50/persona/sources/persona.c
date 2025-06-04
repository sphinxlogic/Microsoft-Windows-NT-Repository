
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *
 *
 *                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
 *                %% \___________________________________%% \
 *                %% |                                   %%  \
 *                %% |             PERSONA               %%   \
 *                %% |          persona.c  c2001         %%    \
 *                %% |            Lyle W. West           %%    |
 *                %% |                                   %%    |
 *                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    |
 *                \                                        \   |
 *                 \                                        \  |
 *                  \                                        \ |
 *                   \________________________________________\|
 *
 *
 *
 *  Copyright (C) 2001 Lyle W. West, All Rights Reserved.
 *  Permission is granted to copy and use this program so long as [1] this
 *  copyright notice is preserved, and [2] no financial gain is involved
 *  in copying the program.  This program may not be sold as "shareware"
 *  or "public domain" software without the express, written permission
 *  of the author.
 *
 *
 * FACILITY: PERSONA
 *
 * ABSTRACT:  This program allows a privileged user to obtain an interactive
 *            session on behalf of another user.  The privileged user inherits
 *            the complete environment of the target user including DCL symbols,
 *            PROCESS/JOB/GROUP logical names, process quotas, privileges and
 *            rights identifiers as well as any other stuff that is set up in
 *            the target user's LOGIN.COM
 *
 * ENVIRONMENT: User mode 
 *
 * SECURITY: Install with CMKRNL, DETACH, SYSPRV, AUDIT
 *
 * Usage:    $ PERSONA username
 *
 *           PERSONA is defined to be a foreign symbol and
 *           username is a valid userid on the system.
 *
 * OTHER:    Requires the FTA psuedo terminal driver. PERSONA is a C
 *           rewrite and enhancement of the GLOGIN utility written by
 *           Anthony C. McCracken, Northern Arizona University, in 1992.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "version.h"
#pragma module persona VERSION

#include <stdio.h>          /* Standard IO definitions */
#include <ssdef.h>          /* System Service return codes */
#include <iodef.h>          /* I/O Definition codes */
#include <ttdef.h>          /* basic terminal characteristics */
#include <tt2def.h>         /* extended terminal characteristics */
#include <lnmdef.h>         /* logical name defs */
#include <impdef.h>         /* Persona Definitions */
#include <string.h>         /* currently for strlen() */
#include <jpidef.h>         /* Job/Process Information item codes */
#include <dvidef.h>         /* Device Information item codes */
#include <uaidef.h>         /* User Authorization Information item codes */
#include <prvdef.h>         /* Priv mask defs */
#include <ptddef.h>         /* PseudoTerminal Definitions */
#include <accdef.h>         /* Accounting record definitions */
#include <prcdef.h>         /* Process Creation flags */
#include <clidef.h>         /* Cli definitions */
#include <climsgdef.h>
#include <ossdef.h>         /* defines for get/set_security calls */
#include <descrip.h>        /* Descriptor Definition Macros */
#include "defs.h"           /* local defs */

struct _IoBuff {
    buff PtdReadBuf,
         PtdWrtBuf;
} _align (PAGE) IoBuff;

unsigned long inAdr[2];

struct  ITEM DviItem[2];    /* Itemlist for $GETDVI info */
struct  ITEM JpiUser[7];    /* Itemlist for invoking user */
struct  ITEM LnmItem[3];    /* Itemlist for $TRNLNM call */
struct  ITEM MbxItem[2];    /* Itemlist for Termination mailbox device */
struct  ITEM UaiItem[4];    /* Itemlist for Target user uaf info */

struct IOSB  DviIosb;       /* iosb for $GETDVI */
struct IOSB  JpiIosb;       /* iosb for $GETJPI */
struct IOSB  MbxIosb;       /* iosb for mailbox */
struct IOSB  PtdIosb;       /* iosb for PTD device */
struct IOSB  QioIosb;       /* iosb for $QIO(W) */
struct IOSB  TtdIosb;       /* iosb for TT: device */

int  OldTTChar[3];          /* Invoking terminal characteristics */
int  NewTTChar[3];          /* Target terminal characteristics */
int  CurPriv[2];            /* Image process privs */
int  ImagePriv[2];          /* Installed image privs */
int  status;                /* global status return info */
int  indx;                  /* index value */
int  CliStat;               /* status value for cli calls */
int  Ignore;                /* Ignore identifier checking (undocumented) */
int  ImpFlags;              /* flags for $PERSONA_CREATE call */
int  ImpHandle;
int  MbxUnit;               /* termination mailbox unit number (MBAxxx) */
int  NewPid;                /* process id for detached process */
int  OldPid;                /* process id for original process */
int  StsFlg;                /* runtime options for created detached process */
int  UicBufO;               /* uic of original user */
int  UicBufN;               /* uic of new user (fetched, but not used) */
int  UaiFlags;              /* uai flags for target user */

short PtdChan;              /* Channel for Pseudo Terminal */
short TtdChan;              /* Channel for original terminal */
short MbxChan;              /* Channel for termination mailbox */
short OldProt;              /* Protection of orig terminal */
short NewProt;              /* Protection for terms during this image */
short CvLen;                /* Length of command verb */
short FlagLen;              /* size in bytes of uaiflags */
short PrvLen;               /* size of CurPriv */
short TimLen;               /* Length of current time string */
short UicLen;               /* size of UIC value (UicBuf) */
short UserLen;              /* size of username buffer (NewUser) */
short NameLen;
short LogFlag;              /* boolean determines if logging enabled */
short LogStat;              /* boolean determines if entry log successful */

char CmdVerb[50];           /* foreign symbol used to invoke this image */
char CliStr[50];            /* buffer to store cli info */
char CurTime[28];           /* buffer for current time string */
char EscStr[16];            /* clr screen and home cursor str */
char ImgStr[50];            /* image name str for loginout.exe */
char LogFileSpec[80];       /* filespec for logfile */
char MbxBuff[90];           /* Mailbox accounting string buffer */
char NewProcN[18];          /* New invoking process name */
char NewUser[16];           /* Target user name */
char NewUserTmp[14];        /* temp string for new username */
char OldProcN[18];          /* Process name of invoking user */
char OldUser[16];           /* Invoking user name */
char PrcName[18];           /* Process name of detached pseudoterm */
char TtdName[32];           /* Original TT name */
char PtdName[32];           /* Resultant Pseudoterminal device name */
char *ptr1;                 /* general string pointer */

FILE *logfp;                /* file pointer for logfile */

extern void ExitHndlr();
extern int  GetLnm();
extern void MbxAst();
extern void PtdReadAst();
extern void QioReadAst();
extern void TtdReadAst();

        /* DESBLK - control block for an exit handler */

static int ExitStatus;
static struct {
    int flink;
    int (*hand_address) ();
    int NumOfArgs;
    int *ReasonForExit;
} ExitBlock = {0, ExitHndlr, 1, &ExitStatus};

globalvalue PERSONA_CLD;

int main()
{
    char MsgBuff[80];                  /* temp string for messages */

        /* Initialize descriptor list */

    $DESCRIPTOR(Dsc_PrnN, NewProcN);    /* new invoking process name */
    $DESCRIPTOR(Dsc_Prn, PrcName);      /* detached pseudoterm process name */
    $DESCRIPTOR(Dsc_Tt, "TT:");         /* originating terminal device */
    $DESCRIPTOR(Dsc_Ttd, TtdName);      /* orig terminal device name string */
    $DESCRIPTOR(Dsc_Ptd, PtdName);      /* pseudo terminal device name string */
    $DESCRIPTOR(Dsc_UserO, OldUser);    /* originating username string */
    $DESCRIPTOR(Dsc_UserT, NewUserTmp); /* target username tmp string */
    $DESCRIPTOR(Dsc_UserN, NewUser);    /* target username string */
    $DESCRIPTOR(Dsc_Cli, CliStr);       /* cli param or qualifier string */
    $DESCRIPTOR(Dsc_Cmd, CmdVerb);      /* foreign invoking symbol name */
    $DESCRIPTOR(Dsc_Img, ImgStr);       /* image string for $creprc */


    sprintf(ImgStr, "SYS$SYSTEM:LOGINOUT.EXE");
    status = cmdlin(&Dsc_Cmd, &CvLen);  /* get invoking foreign cmd str */
    if(status != SS$_NORMAL) exit(status);

        /* make sure cmd verb is uppercase */

    CmdVerb[CvLen] = '\0';
    ptr1 = strchr(CmdVerb,'/');
    if(ptr1) *ptr1 = '\0';
    ptr1 = strchr(CmdVerb,' ');
    if(ptr1) *ptr1 = '\0';
    for(indx = 0; indx < strlen(CmdVerb); indx++)
        CmdVerb[indx] = toupper(CmdVerb[indx]);

    CliStat = InitCli(PERSONA_CLD, "PERSONA");
    if(!CliStat & 1) lib$stop(CliStat);

        /* if user typed /HELP then show help text and exit */

    strcpy(CliStr,"HELP");
    Dsc_Cli.dsc$w_length = strlen(Dsc_Cli.dsc$a_pointer);
    CliStat = cli$present(&Dsc_Cli);
    if(CliStat == CLI$_PRESENT) ShowHelp();

        /* if user typed /VERSION then show versinfo and exit */

    strcpy(CliStr, "VERSION");
    Dsc_Cli.dsc$w_length = strlen(CliStr);
    CliStat = cli$present(&Dsc_Cli);
    if(CliStat == CLI$_PRESENT) version();

        /* if user typed /IGNOREID then set the ignore flag */

    strcpy(CliStr, "IGNOREID");
    Dsc_Cli.dsc$w_length = strlen(CliStr);
    CliStat = cli$present(&Dsc_Cli);
    if(CliStat == CLI$_PRESENT) Ignore = TRUE;

        /* get the target username from command line */

    strcpy(Dsc_Cli.dsc$a_pointer,"USERNAME");
    Dsc_Cli.dsc$w_length = strlen(Dsc_Cli.dsc$a_pointer);
    CliStat = cli$get_value(&Dsc_Cli, &Dsc_UserT, &UserLen);
    status = str$upcase(&Dsc_UserT, &Dsc_UserT);
    if(CliStat == SS$_NORMAL) {
        NewUserTmp[UserLen] = '\0';
    }
    else {  /* if no username, display message and exit */
        printf("\n%%%s-E-NOUSERID,  Usage Syntax: %s Username\n",
            CmdVerb,  CmdVerb);
        printf("\t(type %s /HELP)\n\n", CmdVerb);
        exit(1);
    }
    sprintf(NewUser,"%-12s", NewUserTmp);
    Dsc_UserN.dsc$w_length = strlen(NewUser);

        /* Check the required priv bits for this image. If returned status 
           is SS$_NOTALLPRIV then we exit due to insufficient privs */

    status = ChkPriv();
    if(status != SS$_NORMAL) {
        printf("%%%s-E-NOTALLPRIV, not all requested privileges authorized\n\n",
                CmdVerb);
        exit(1);
    }
    StsFlg = 0;

        /* Verify pseudoterm is installed, exit if not */

    strcpy(PtdName, "FTA0:");
    DviItem[0].buf_size = strlen(PtdName);
    DviItem[0].item_code = DVI$_DEVNAM;
    DviItem[0].buf_addr = &PtdName;
    DviItem[0].buf_len = &NameLen;
    DviItem[1].buf_size = 0;
    DviItem[1].item_code = 0;

    status = sys$getdviw(0, 0, &Dsc_Ptd, &DviItem, &DviIosb, 0, 0, 0);
    if(!(status & 1)) {
        printf("\n%%%s-E-NOSUCHDEV, device FTA0: not found\n\n", CmdVerb);
        exit(1);
    }
        /* check if the username really exists on this system */

    UaiItem[0].buf_size = sizeof(UaiFlags);
    UaiItem[0].item_code = UAI$_FLAGS;
    UaiItem[0].buf_addr = &UaiFlags;
    UaiItem[0].buf_len  = 0;
    UaiItem[1].buf_size = sizeof(UicBufN);
    UaiItem[1].item_code = UAI$_UIC;
    UaiItem[1].buf_addr = &UicBufN;
    UaiItem[1].buf_len  = &UicLen;
    UaiItem[2].buf_size = 0;
    UaiItem[2].item_code = 0;

    Dsc_UserT.dsc$w_length = strlen(NewUserTmp);
    status = sys$getuai(0, 0, &Dsc_UserT, &UaiItem, 0, 0, 0);
    if (status == RMS$_RNF || status == SS$_NOSUCHUSER) {
        printf("\n%%%s-E-NOSUCHUSER, User %s not found\n\n",
                CmdVerb, NewUserTmp);
        exit(1);
    }
    if (status == SS$_NORMAL) {
        if (UaiFlags & UAI$M_DISACNT) {
            printf("\n%%%s-E-DISUSER, The UIC record for %s is disabled\n\n",
                    CmdVerb, NewUserTmp);
            exit(1);
        }
    }
    else lib$stop(status);

        /* get some info about invoking user */

    JpiUser[0].buf_size = sizeof(UicBufO);
    JpiUser[0].item_code = JPI$_UIC;
    JpiUser[0].buf_addr = &UicBufO;
    JpiUser[0].buf_len  = 0;
    JpiUser[1].buf_size = sizeof(OldUser);
    JpiUser[1].item_code = JPI$_USERNAME;
    JpiUser[1].buf_addr = &OldUser;
    JpiUser[1].buf_len  = 0;
    JpiUser[2].buf_size = sizeof(CurPriv);
    JpiUser[2].item_code = JPI$_CURPRIV;
    JpiUser[2].buf_addr = &CurPriv;
    JpiUser[2].buf_len  = 0;
    JpiUser[3].buf_size = sizeof(ImagePriv);
    JpiUser[3].item_code = JPI$_IMAGPRIV;
    JpiUser[3].buf_addr = &ImagePriv;
    JpiUser[3].buf_len  = 0;
    JpiUser[4].buf_size = sizeof(OldPid);
    JpiUser[4].item_code = JPI$_PID;
    JpiUser[4].buf_addr = &OldPid;
    JpiUser[4].buf_len  = 0;
    JpiUser[5].buf_size = sizeof(OldProcN);
    JpiUser[5].item_code = JPI$_PRCNAM;
    JpiUser[5].buf_addr = &OldProcN;
    JpiUser[5].buf_len  = &NameLen;
    JpiUser[6].buf_size = 0;
    JpiUser[6].item_code = 0;

    status = sys$getjpiw(0, 0, 0, &JpiUser, &JpiIosb, 0, 0);
    if(!(status & 1)) lib$stop(status);

        /* Verify invoking user is authorized to become target user.
           If not, display error message and exit. Should the IGNORE
           flag be set, have user enter system manager password */

    if(Ignore) {
        status = PwdChk();
        if(!(status & 1)) {
            printf("%%%s-E-INVPWD, Invalid password entered for SYSTEM account\n",
                    CmdVerb);
            exit(1);
        }
    }
    else {
        status = CheckIdent();
        if(status == SS$_NOSUCHID || status == SS$_IVIDENT) {
            ptr1 = strchr(OldUser, ' ');
            *ptr1 = '\0';
            sprintf(MsgBuff, "\n\n%%%s-E-NOAUTH, User %s not authorized to ",
                    CmdVerb, OldUser);
            sprintf(MsgBuff+strlen(MsgBuff), "assume persona of %s\n\n",
                    NewUserTmp);
            printf("%s", MsgBuff);
            exit(1);
        }
    }

        /* Assign a channel to the original terminal device */

    status = sys$assign(&Dsc_Tt, &TtdChan, 0, 0);
    if(!(status & 1)) lib$stop(status);

        /* Get device name of original terminal device */
 
    DviItem[0].buf_size = sizeof(TtdName);
    DviItem[0].item_code = DVI$_DEVNAM;
    DviItem[0].buf_addr = &TtdName;
    DviItem[0].buf_len = &NameLen;
    DviItem[1].buf_size = 0;
    DviItem[1].item_code = 0;

    status = sys$getdviw(0, TtdChan, 0, &DviItem, &DviIosb, 0, 0, 0);
    if(!(status & 1)) lib$stop(status);

        /* get the device protections of original terminal and save
           for restore on exit. Then set orig term to full access */

    status = GetDevProt(&TtdName, OLDDEV);
    if(!(status & 1)) lib$stop(status);
    status = SetDevProt(&TtdName, NEWDEV);
    if(!(status & 1)) lib$stop(status);

        /* get original terminal characteristics */

    status = sys$qiow(0, TtdChan, IO$_SENSEMODE, &TtdIosb, 0, 0, &OldTTChar,
                      sizeof(OldTTChar), 0, 0, 0, 0);
    if(!(status & 1)) lib$stop(status);

        /* copy old term characteristics to new term characteristics,
           then make changes to new characteristics buffer (there are
           prettier ways to do this, but this method works! */

    NewTTChar[0] = OldTTChar[0];
    NewTTChar[1] = OldTTChar[1];
    NewTTChar[2] = OldTTChar[2];

    NewTTChar[1] |= TT$M_NOECHO;
    NewTTChar[1] &= ~TT$M_WRAP;
    NewTTChar[2] |= TT2$M_PASTHRU;

        /* save new characteristics back to originating terminal */

    status = sys$qio(0, TtdChan, IO$_SETMODE, &TtdIosb, 0, 0, &NewTTChar,
                      sizeof(NewTTChar), 0, 0, 0, 0);
    if(!(status & 1)) lib$stop(status);

        /* Define an exit handler to restore callers original env */

    status = sys$dclexh(&ExitBlock);
    if(!(status & 1)) lib$stop(status);

        /* Check if there is a logfile option enabled. If so, set the
           boolean LogFlag to TRUE */

    status = GetLnm();
    if (status == SS$_NORMAL) LogFlag = TRUE;

        /* Perform persona create and assume for new username */

    ImpFlags = IMP$M_ASSUME_DEFPRIV | IMP$M_ASSUME_DEFCLASS;
    status = sys$persona_create(&ImpHandle, &Dsc_UserN, ImpFlags);
    if(!(status & 1)) lib$stop(status);

    ImpFlags = IMP$M_ASSUME_ACCOUNT | IMP$M_ASSUME_JOB_WIDE |
               IMP$M_ASSUME_SECURITY;
    status = sys$persona_assume(&ImpHandle, ImpFlags);
    if(!(status & 1)) lib$stop(status);

        /* Create FTA device, using the default characteristics */

    inAdr[0] = &IoBuff;
    inAdr[1] = inAdr[0] + sizeof(IoBuff);
#ifdef MYPTD
    status = ptd$create(&PtdChan, 0, &OldTTChar, sizeof(OldTTChar),
                        0, 0, 0, &inAdr);
#else
    status = ptd$create(&PtdChan, 0, 0, 0, 0, 0, 0, &inAdr);
#endif
    if(!(status & 1)) lib$stop(status);

        /* Initialize itemlist to get the device name of the 
           pseudoterminal. Save this string in PtdName */

    DviItem[0].buf_size = sizeof(PtdName);
    DviItem[0].item_code = DVI$_DEVNAM;
    DviItem[0].buf_addr = &PtdName;
    DviItem[0].buf_len = &NameLen;
    DviItem[1].buf_size = 0;
    DviItem[1].item_code = 0;

    status = sys$getdviw(0, PtdChan, 0, &DviItem, &DviIosb, 0, 0, 0);
    if(!(status & 1)) lib$stop(status);
    PtdName[NameLen] = 0;

        /* update the descriptor for PtdName to reflect new FTAx 
           device name length */

    Dsc_Ptd.dsc$w_length = strlen(Dsc_Ptd.dsc$a_pointer);

        /* set pseudoterm protections to full access */

    status = SetDevProt(&PtdName, NEWDEV);
    if(!(status & 1)) lib$stop(status);

        /* Write the terminal name and new user to TT: */

    sprintf(EscStr, "\033[2J\033[H"); /* clr screen, home cursor */
    sprintf(MsgBuff, "%s\n\n%%%s-I-SESINIT, session initiated.",
            EscStr, CmdVerb);
    sprintf(MsgBuff+strlen(MsgBuff), " PsuedoTerm: %s, Userid: %s\n\n",
            PtdName, NewUser);
    status = sys$qiow(0, TtdChan, IO$_WRITEVBLK, &TtdIosb, 0, 0,
                      MsgBuff, strlen(MsgBuff), 0, 0, 0, 0);
    if(!(status & 1)) lib$stop(status);

        /* create a termination mailbox, get mbx unit value and
           set up an ast routine to be called when the mbx is
           written to (at detached process logout) */

    status = sys$crembx(0, &MbxChan, ACC$K_TERMLEN, 0, 0, 0, 0, 0, 0);
    if(!(status & 1)) lib$stop(status);

    MbxItem[0].buf_size = sizeof(MbxUnit);
    MbxItem[0].item_code = DVI$_UNIT;
    MbxItem[0].buf_addr = &MbxUnit;
    MbxItem[0].buf_len = &NameLen;
    MbxItem[1].buf_size = 0;
    MbxItem[1].item_code = 0;

    status = sys$getdviw(0, MbxChan, 0, &MbxItem, &DviIosb, 0, 0, 0);
    if(!(status & 1)) lib$stop(status);

        /* Don't use $QIOW here, we will block forever... */
 
    status = sys$qio(0, MbxChan, IO$_READVBLK, &MbxIosb, &MbxAst, 0,
                      MbxBuff, ACC$K_TERMLEN, 0, 0, 0, 0);
    if(!(status & 1)) lib$stop(status);

        /* load the detached procname string with original pid */

    sprintf(PrcName, "P_%08X", OldPid);
    Dsc_Prn.dsc$w_length = strlen(PrcName);

        /* Create the interactive process using the psuedo terminal.
           The created process will be INTERactive and DETACHed.
           The NOPASSWORD flag will cause the process to login
           without the Userid: and Password: prompts */

    StsFlg = PRC$M_DETACH | PRC$M_INTER | PRC$M_NOPASSWORD;

    status = sys$creprc(&NewPid,        /* new process id */
                        &Dsc_Img,       /* descriptor for loginout */
                        &Dsc_Ptd,       /* input - pseudoterminal */
                        &Dsc_Ptd,       /* output - pseudoterminal */
                        &Dsc_Ptd,       /* error (same as output) */
                        0,              /* mask_privileges */
                        0,              /* new proc quotas */
                        &Dsc_Prn,       /* proc name is invoking user's pid */
                        4,              /* base priority */
                        0,              /* uic of new user */
                        MbxUnit,        /* termination mbx unit number */
                        StsFlg,         /* process creation options */
                        0,              /* itmlst - reserved by Digital */
                        0);             /* Target node (not used) */

    if(!(status & 1)) lib$stop(status);

        /* set original process name to be PERS_xxxxxxxxx where
           xxxxxxxxx is the process id of the detached process */

    sprintf(NewProcN, "PERS_%08X", NewPid);
    Dsc_PrnN.dsc$w_length = strlen(NewProcN);
    status = sys$setprn(&Dsc_PrnN);
    if(!(status & 1)) lib$stop(status);


        /* Delete assumed persona, return to orig environment */

    ImpFlags = 1;       /* 0x1 says return to original user persona */

    status = sys$persona_assume(&ImpFlags, 0, 0);
    if(!(status & 1)) lib$stop(status);

#ifdef ARF
    status = sys$persona_delete(&ImpHandle);
    if(!(status & 1)) lib$stop(status);
#endif

        /* if logging enabled, record time, orig user. target user,
           pid of each, prcnam of each. */

    if(LogFlag) {
        logfp = fopen(LogFileSpec, "a+", "rat=cr", "rfm=var", "shr=upi");
        if(logfp) {
            GetCurTime();
            status = fprintf(logfp,
                "%s Persona entry: User: %12s(%08X), Target: %12s(%08X)\n",
                CurTime, OldUser, OldPid, NewUser, NewPid); 
            LogStat = TRUE;
            status = fclose(logfp);
        }
        else LogFlag = 0;
    }


        /* Post a read to the local terminal -- with an AST */

    status = sys$qio(0, TtdChan, IO$_READVBLK, &TtdIosb, TtdReadAst, 0,
                     &IoBuff.PtdWrtBuf.dat, 1, 0, 0, 0, 0);
    if(!(status & 1)) lib$stop(status);
 
        /* Post PTD read with an AST. Any consecutive PTD read
           will be accomplished within the AST routine. */

    status = ptd$read(0, PtdChan, PtdReadAst, 0, &IoBuff.PtdReadBuf, 508);
    if(!(status & 1)) lib$stop(status);

        /* HIBERnate here forever -- all the rest is done by ASTs. */

    status = sys$hiber();
    if(!(status & 1)) lib$stop(status);
    exit(1);
}

