
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
 *                %% \___________________________________%% \
 *                %% |                                   %%  \
 *                %% |              PERSONA              %%   \
 *                %% |          utility.c  c2001         %%    \
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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "version.h"
#pragma module utility VERSION

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
#include <clidef.h>
#include <climsgdef.h>
#include <ossdef.h>         /* defines for get/set_security calls */
#include <descrip.h>        /* Descriptor Definition Macros */
#include "defs.h"

#define SS$_INVPWD 2        /* can't find a definition, so I made one up */

extern struct _IoBuff {
    buff PtdReadBuf,
         PtdWrtBuf;
} _align (PAGE) IoBuff;

extern unsigned long inAdr[2];

extern struct  ITEM DviItem[2];     /* Itemlist for $GETDVI info */
extern struct  ITEM JpiUser[76];     /* Itemlist for invoking user */
extern struct  ITEM LnmItem[3];     /* Itemlist for $TRNLNM call */
extern struct  ITEM MbxItem[2];     /* Itemlist for Termination mailbox device */
extern struct  ITEM UaiItem[4];     /* Itemlist for Target user uaf info */

extern struct IOSB  DviIosb;        /* iosb for $GETDVI */
extern struct IOSB  JpiIosb;        /* iosb for $GETJPI */
extern struct IOSB  MbxIosb;        /* iosb for mailbox */
extern struct IOSB  PtdIosb;        /* iosb for PTD device */
extern struct IOSB  QioIosb;        /* iosb for $QIO(W) */
extern struct IOSB  TtdIosb;        /* iosb for TT: device */

extern short LogStat;               /* boolean determines if entry log successful */
extern short MbxChan;               /* Channel for termination mailbox */
extern short NewProt;               /* Protection for terms during this image */
extern short OldProt;               /* Protection of orig terminal */
extern short PtdChan;               /* Channel for Pseudo Terminal */
extern short TimLen;
extern short TtdChan;               /* Channel for original terminal */

extern char CharBuf[80];
extern char CmdVerb[50];            /* foreign symbol invoking this image */
extern char CurTime[28];
extern char LogFileSpec[80];        /* filespec for logfile */
extern char NewUser[16];            /* Target user name */
extern char NewUserTmp[14];         /* temp string for new username */
extern char OldProcN[18];           /* Process name of invoking user */
extern char OldUser[16];            /* Invoking user name */
extern char PrcName[18];            /* Process name of detached pseudoterm */
extern char TtdName[32];            /* Original TT name */

extern int  OldPid;                 /* process id for original process */
extern int  NewPid;
extern int  NewTTChar[3];           /* Target terminal characteristics */
extern int  OldTTChar[3];           /* Invoking terminal characteristics */
extern int  CurPriv[2];             /* Image process privs */
extern int  ImagePriv[2];           /* Installed image privs */
extern int  StsFlg;                 /* runtime options for created detached process */
extern int  status;

struct  ITEM OssItem[2];            /* Itemlist for $GET_SECURITY info */

char ClassName[]="DEVICE";          /* Name of object class */
char PwdBuf[32];                    /* User entered password string */

int flags = OSS$M_RELCTX;           /* Mask specifying processing options           */
int context;                        /* Context variable for multiple calls          */
int acmode;                         /* Access mode to be used in object protection  */

$DESCRIPTOR(Dsc_Class, ClassName);   /* Descriptor for object class */
$DESCRIPTOR(Dsc_Prn, PrcName);       /* process name string */
$DESCRIPTOR(Dsc_Tt, "TT:");          /* originating terminal device */

extern void ExitHndlr();
extern void PtdReadAst();
extern void TtdReadAst();
extern void CharChangedAst();

extern FILE *logfp;                /* file pointer for logfile */

globalvalue PERSONA_CLD;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  GetLnm - Check the SYSTEM logical name table for definition of
 *           PERSONA_LOG. If it exists, the equivalance string is 
 *           placed in the global character string LogFileSpec.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int GetLnm()
{
    char    LogLnm[16];            /* persona logging logical name */
    char    LnmTable[32];

    short   LogFileSpec_len;
    short   LnmTable_len;

    int     LnmStat;

    $DESCRIPTOR(Dsc_lnm, LogLnm);      /* logical name for persona logging */
    $DESCRIPTOR(Dsc_tabnam, LnmTable); /* lnm table to check */

    LnmItem[0].buf_size = sizeof(LnmTable);
    LnmItem[0].item_code = LNM$_TABLE;
    LnmItem[0].buf_addr = &LnmTable;
    LnmItem[0].buf_len = &LnmTable_len;
    LnmItem[1].buf_size = sizeof(LogFileSpec);
    LnmItem[1].item_code = LNM$_STRING;
    LnmItem[1].buf_addr = &LogFileSpec;
    LnmItem[1].buf_len = &LogFileSpec_len;
    LnmItem[2].buf_size = 0;
    LnmItem[2].item_code = 0;

    strcpy(LnmTable,"LNM$SYSTEM_TABLE");
    sprintf(LogLnm, "PERSONA_LOG");
    Dsc_lnm.dsc$w_length = strlen(LogLnm);
    Dsc_tabnam.dsc$w_length = strlen(LnmTable);
    LnmStat = sys$trnlnm(0, &Dsc_tabnam, &Dsc_lnm, 0, &LnmItem);
    if(LnmStat == SS$_NORMAL) LogFileSpec[LogFileSpec_len] = '\0';
    return(LnmStat);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  ChkPriv - Call LIB$GETJPI to see if we are installed with the required
 *            privs. If so return SS$_NORMAL, else see if user can set the
 *            required privs from their process privs. If not, we have to
 *            return SS$_NOTALLPRIV to indicate this image cannot be run
 *            due to insufficent privs.
 *            (NOTE: if the 'ifdef' statements below are changed to 'ifndef'
 *            runtime status messages for this routine will be displayed).
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int ChkPriv()
{
    int EnabFlg = 1;               /* Enable flag for $SETPRV */
    int PrivMask[2] = 0;
    int PrivStat = 0;
    int jpicode = JPI$_IMAGPRIV;

    PrivMask[0] = PRV$M_SYSPRV | PRV$M_DETACH | PRV$M_CMKRNL | PRV$M_BYPASS;

    PrivStat = lib$getjpi(&jpicode, 0, 0, &ImagePriv, 0, 0 );
#ifdef ARF
    printf("\nlib$getjpi(PrivStat): %08x, ImagePriv[0]: %08x\n",
             PrivStat, ImagePriv[0]);
#endif
    if(PrivStat != SS$_NORMAL) lib$stop(PrivStat);
    if(ImagePriv[0] == PrivMask[0]) return(SS$_NORMAL);


        /* Set the required priv bits for this image. If returned status
           is SS$_NOTALLPRIV then we exit due to insufficient privs */

    StsFlg = PRV$M_SYSPRV | PRV$M_DETACH | PRV$M_CMKRNL | PRV$M_BYPASS;

    PrivStat = sys$setprv(EnabFlg, &StsFlg, 0, 0);
#ifdef ARF
    printf("\nsys$setprv(PrivStat): 0X%08x\n\n", PrivStat);
#endif
    if(PrivStat != SS$_NORMAL) return(SS$_NOTALLPRIV);
    return(SS$_NORMAL);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * EchoOff - Save current terminal status and change temporarily
 *           so as not to echo when the system manager's password
 *           is entered.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void EchoOff()
{
    status = sys$assign(&Dsc_Tt, &TtdChan, 0, 0);
    status = sys$qiow(0, TtdChan, IO$_SENSEMODE, &QioIosb, 0, 0, &OldTTChar,
                      sizeof(OldTTChar), 0, 0, 0, 0);
    NewTTChar[0] = OldTTChar[0];
    NewTTChar[1] = OldTTChar[1];
    NewTTChar[2] = OldTTChar[2];

    NewTTChar[1] |= TT$M_NOECHO;       /* term will be Noecho */
    NewTTChar[1] &= ~TT$M_ESCAPE;      /* no escape */
    NewTTChar[1] |= TT$M_NOBRDCST;     /* disable direct broadcast */
    NewTTChar[1] &= ~TT$M_HOSTSYNC;    /* no host sync */
    NewTTChar[1] &= ~TT$M_TTSYNC;      /* do sync at subprocess level */

    status = sys$qiow(0, TtdChan, IO$_SETMODE, &QioIosb, 0, 0, &NewTTChar,
                      sizeof(NewTTChar), 0, 0, 0, 0);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * EchoOn - Restore terminal status to same state as it was at the
 *          time this image was activated. Called after system manager's
 *          password has been entered.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void EchoOn()
{
    status = sys$qiow(0, TtdChan, IO$_SETMODE, &QioIosb, 0, 0, &OldTTChar,
                      sizeof(OldTTChar), 0, 0, 0, 0);
    status = sys$dassgn(TtdChan);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * GetCurTime - Get current date and time as ascii string and place
 *              resultant string in global buffer 'CurTime'
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void GetCurTime()
{
    int cvtflg = 0;
    char *ptr2 = 0;
    $DESCRIPTOR(dsc_time, CurTime);

    status = sys$asctim(&TimLen, &dsc_time, 0, &cvtflg);
    if(status == SS$_NORMAL) {
        ptr2 = strchr(CurTime, '.');
        if(ptr2) *ptr2 = '\0';
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * TtGets - Read character string from the keyboard, perform no editing,
 *          no echo. Purge typeahead buffer at entry.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int TtGets()
{
    int length;         /* num bytes in typeahead buf */

    status = sys$qiow(0, TtdChan, IO$_READLBLK|IO$M_PURGE, &QioIosb, 0, 0,
                      &PwdBuf, sizeof(PwdBuf), 0, 0, 0, 0);
    length = QioIosb.byte_cnt;
    PwdBuf[length] = '\0';
    return (length);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * PwdChk - Prompt user for system manager password. Do not echo response,
 *          compare response string to existing password. If response is
 *          is positive return SS$_NORMAL, else return SS$_INVPWD
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int PwdChk()
{
    char Alg;
    char UsrNam[] = {"SYSTEM"};
    short Salt;
    long ChkHash[2], CurHash[2];
    $DESCRIPTOR(Dsc_ChkPwd, PwdBuf);
    $DESCRIPTOR(Dsc_User, UsrNam);

    printf("\nEnter System Manager Password: ");
    EchoOff();
    status = TtGets();
    EchoOn();
    printf("\n");

    Dsc_ChkPwd.dsc$a_pointer = PwdBuf;
    Dsc_ChkPwd.dsc$w_length = strlen(PwdBuf);

    UaiItem[0].buf_size = 1;
    UaiItem[0].item_code = UAI$_ENCRYPT;
    UaiItem[0].buf_addr = &Alg;
    UaiItem[0].buf_len = 0;
    UaiItem[1].buf_size = 2;
    UaiItem[1].item_code = UAI$_SALT;
    UaiItem[1].buf_addr = &Salt;
    UaiItem[1].buf_len = 0;
    UaiItem[2].buf_size = 8;
    UaiItem[2].item_code = UAI$_PWD;
    UaiItem[2].buf_addr = &CurHash;
    UaiItem[2].buf_len = 0;
    UaiItem[3].buf_size = 0;
    UaiItem[3].item_code = 0;

    status = str$upcase(&Dsc_ChkPwd, &Dsc_ChkPwd);
    status = sys$getuai(0, 0, &Dsc_User, &UaiItem, 0, 0, 0);

        /* use 'sys$hash_password' to get hash value from user */

    status = sys$hash_password(&Dsc_ChkPwd, Alg, Salt, &Dsc_User, ChkHash);
    if((CurHash[0] != ChkHash[0]) || (CurHash[1] != ChkHash[1])) 
        return(SS$_INVPWD);
    else return (SS$_NORMAL);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * MbxAst - Ast routine called when a write occurs to the termination
 *          mailbox. This event is called as a result of exiting the
 *          detached pseudo terminal process. Simply cancel the SYS$HIBER
 *          call, then exit. ExitHndlr() will handle the remaining items.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MbxAst()
{
    status = sys$dassgn(MbxChan);
    status = sys$wake(&OldPid, 0);  /* as with glogin.mar, this doesn't work */
    status = sys$exit(SS$_NORMAL);  /* this however, does */
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  GetDevProt - Get the word length protection mask for the device
 *               name buffer pointed to by DevName. The ProtType
 *               constant defines where to save retrieved mask.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int GetDevProt(char *DevName, int ProtType)
{
    short length = 0;
    $DESCRIPTOR(Dsc_Term, DevName);  /* Descriptor for object name */

    Dsc_Term.dsc$w_length = strlen(DevName);
    OssItem[0].buf_size = 2;
    OssItem[0].item_code = OSS$_PROTECTION;
    OssItem[0].buf_addr = &NewProt;
    OssItem[0].buf_len  = &length;
    OssItem[1].buf_size = 0;
    OssItem[1].item_code = 0;

    if(ProtType) OssItem[0].buf_addr = &OldProt;
    status = sys$get_security(&Dsc_Class, &Dsc_Term, 0, flags, &OssItem,
                              &context, &acmode);
    if(!(status & 1)) lib$stop(status);
    return(SS$_NORMAL);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  SetDevProt - Set the word length protection mask for the device
 *               name buffer pointed to by DevName. The ProtType
 *               constant defines where the new protection mask
 *               is located. (Ugly, but it works)
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int SetDevProt(char *DevName, int ProtType)
{
    short length = 0;
    $DESCRIPTOR(Dsc_Term, DevName);  /* Descriptor for object name */

    Dsc_Term.dsc$w_length = strlen(DevName);
    OssItem[0].buf_size = 2;
    OssItem[0].item_code = OSS$_PROTECTION;
    OssItem[0].buf_addr = &NewProt;
    OssItem[0].buf_len  = &length;
    OssItem[1].buf_size = 0;
    OssItem[1].item_code = 0;

    if(ProtType) OssItem[0].buf_addr = &OldProt;
    status = sys$set_security(&Dsc_Class, &Dsc_Term, 0, flags, &OssItem,
                              &context, &acmode);
    if(!(status & 1)) lib$stop(status);
    return(SS$_NORMAL);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  ExitHndlr - Exit handler from $DCLEXH declaration. Routine is called
 *              for any exit() call subsequent to SYS$PERSONA_CREATE 
 *              and is used to cleanup terminal environment prior to
 *              actual image rundown.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ExitHndlr(int *exit_status)
{
    status = ptd$delete(PtdChan);
    if(!(status & 1)) lib$stop(status);
    strcpy(PrcName, OldProcN);
    Dsc_Prn.dsc$w_length = strlen(PrcName);
    status = sys$setprn(&Dsc_Prn);
    if(!(status & 1)) lib$stop(status);
    status = sys$dassgn(TtdChan);
    if(!(status & 1)) lib$stop(status);
    status = sys$assign(&Dsc_Tt, &TtdChan, 0, 0);
    if(!(status & 1)) lib$stop(status);
    status = sys$qio(0, TtdChan, IO$_SETMODE, &TtdIosb, 0, 0, &OldTTChar,
                      sizeof(OldTTChar), 0, 0, 0, 0);
    printf("\n\n%%%s-I-SESCOMP, session completed. Terminal: %s, UserId: %s\n\n",
            CmdVerb, TtdName, OldUser);
    status = SetDevProt(&TtdName, OLDDEV);
    if(!(status & 1)) lib$stop(status);
    status = sys$dassgn(TtdChan);
    if(!(status & 1)) lib$stop(status);

        /* if logging enabled, record time, orig user. target user,
           pid of each, prcnam of each. */

    if(LogStat) {
        logfp = fopen(LogFileSpec, "a+", "rat=cr", "rfm=var", "shr=upi");
        if(logfp) {
            GetCurTime();
            status = fprintf(logfp,
                "%s Persona  exit: User: %12s(%08X), Target: %12s(%08X)\n",
                CurTime, OldUser, OldPid, NewUser, NewPid); 
            status = fclose(logfp);
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  TtdReadAst - called whenever a key is pressed on the real keyboard.
 *               It just transfers the keystroke to the psuedo term,
 *               then requeues another read the keyboard.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TtdReadAst()
{
    status = ptd$write(PtdChan, 0, 0, &IoBuff.PtdWrtBuf, 1, 0, 0);
    if(!(status & 1)) lib$stop(status);

    status = sys$qio(0, TtdChan, IO$_READVBLK, 0, TtdReadAst, 0,
                      &IoBuff.PtdWrtBuf.dat, 1, 0, 0, 0, 0);
    if(!(status & 1)) lib$stop(status);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  PtdReadAst - called whenever anything comes in from the psuedo term.
 *               It determines how many characters were read and 
 *               transfers them to the real screen. It then queues
 *               another read to the psuedo terminal.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PtdReadAst()
{
    status = sys$qio(0, TtdChan, IO$_WRITEVBLK, &QioIosb, 0, 0,
                     &IoBuff.PtdReadBuf.dat,
                     IoBuff.PtdReadBuf.length, 0, 0, 0, 0);
    if(!(status & 1)) lib$stop(status);

    status = ptd$read(0, PtdChan, PtdReadAst, 0, &IoBuff.PtdReadBuf, 508);
    if(!(status & 1)) lib$stop(status);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  CharChangedAst - This AST routine will be called anytime an attempt
 *                   is made to change the terminal Characteristics. 
 *                   This will be indicated by printing the string
 *                   the string "--->CHAR" on the terminal screen, but
 *                   is NOT implemented in this application.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CharChangedAst()
{
    char charChangedBuf[] = "--->CHAR\n";

        /* Write to the terminal */

    status = sys$qiow(0, TtdChan, IO$_WRITEVBLK|IO$M_BREAKTHRU, &QioIosb,
                      0, 0, charChangedBuf, strlen(charChangedBuf),
                      0, 0, 0, 0);
    if(!(status & 1)) lib$stop(status);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  CheckIdent - Reads the rights database to verify invoking user has
 *               appropriate identifier granted to assume the persona of
 *               the target user. If not, return SS$_NOSUCHID else
 *               if proper identifier then return SS$_NORMAL.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int CheckIdent()
{
    char name[31];
    char TargIdentStr[31];
    unsigned int HolderBin[2];
    unsigned int TargIdentBin;
    unsigned int HeldBin, context = 0;
    unsigned short count = 0;
    $DESCRIPTOR (Dsc_Name, name);
    $DESCRIPTOR(Dsc_Pers, TargIdentStr);

        /* get invoking username, convert to binary value */

    strcpy(name, OldUser);
    Dsc_Name.dsc$w_length = strlen(name);
    status = sys$asctoid ( &Dsc_Name, &HolderBin, 0);
    if(!(status & 1)) lib$stop(status);

        /* get target username, prefix with PERS_, then
           convert to binary value */

    sprintf(TargIdentStr, "PERS_%s", NewUserTmp);
    Dsc_Pers.dsc$w_length = strlen(TargIdentStr);
    status = sys$asctoid(&Dsc_Pers, &TargIdentBin, 0);
    if(!(status & 1)) return(status);   /* probably SS$_NOSUCHID */

        /* Make repeated calls to SYS$FIND_HELD to return 
           all identifiers held by holder (OldUser) */

    HolderBin[1] = 0;        /* complete the quadword holder */
    while(status == SS$_NORMAL) {
        status = sys$find_held(&HolderBin, &HeldBin, 0, &context);
        if(status == SS$_NORMAL) {
            if(HeldBin == TargIdentBin) {
                status = sys$finish_rdb(&context); /* rights database context */
                return(SS$_NORMAL);
            }
            else count++;
        }
        if((status == SS$_NOSUCHID) && !count) return(status);
    }
    return(status);     /* probably SS$_NOSUCHID with count > 0 */
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  version - User entered the /VERSION qualifier to display the image
 *            version, C compiler type, and required image privileges.
 *            This software application has NOT been built with VaxC
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
version()
{
    printf("\n\n\t\t%s by Lyle W West\n", CmdVerb);
    printf("\t\tVersion: %s (DecC)\n", VERSION);
    printf("\t\tImage Build: %s\n", __DATE__);
    printf("\t\tRequired Privs: AUDIT, SYSPRV, DETACH, CMKRNL, BYPASS\n\n");
    exit(1);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  ShowHelp - User entered the /HELP qualifier to get a list of
 *             parameters and qualifiers available to this image
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
ShowHelp()
{
    printf(" \n\n%s Command line arguments:\n\n", CmdVerb);
    printf("\n   Parameters\n\n");
    printf("     USERNAME - Required. The USERNAME is a valid user in\n");
    printf("\t\tthe User Authorization File. The user account cannot\n");
    printf("\t\tbe disabled. If invalid or disabled, %s exits.\n\n", CmdVerb);
    printf("   Qualifiers\n\n");
    printf("    /HELP - if present, %s displays list of qualifiers\n", CmdVerb);
    printf("\t\tavailable for %s (this display)\n\n", CmdVerb);
    printf("    /IGNOREID - if present, %s ignores identifier requirements.\n",
                CmdVerb);
    printf("\t\tHowever, user is required to enter the system manager's\n");
    printf("\t\tpassword to continue\n\n");
    printf("    /VERSION - if present %s displays version, build", CmdVerb);
    printf(" date,\n\t\tand required privs\n\n");
    exit(1);
}





/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *            ****** NOTICE ******
 *
 *    This routine written by Joe Meadows and is used in his
 *    VERB, FILE and FIND programs. Only minor modifications
 *    have been made for use in this program - lww
 *
 *    status = InitCli(table_addr, verb_name)
 *
 *    This routine allows you to define a command as either a foreign
 *  command (via PROG :== $<wherever>PROG), or as a real command (via the
 *  SET COMMAND command). "table_addr" is the address of the command tables
 *  (you must do a SET COMMAND/OBJECT and link the resulting object file
 *  into your code, then use "globalvalue" to reference the table.
 *  "verb_name" is the name of the verb (a null terminated, uppercase string)
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* "table" is a pointer to the internal CLD
 * "name" is a text string containing the name of the verb
 */

int InitCli(char *table, char *name)
{
  static struct dsc$descriptor cmd = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
  static $DESCRIPTOR(verb,"$VERB");
  static $DESCRIPTOR(line,"$LINE");
  int indx, stat;

  stat = cli_get_value(&verb,&cmd);
  if ((stat & 1) && cmd.dsc$w_length)
  {
    if (strncmp(cmd.dsc$a_pointer,name,cmd.dsc$w_length) == 0)
    {
      return(1);    /* the command must have been properly defined! */
    }
  }

  stat = cli_get_value(&line,&cmd);
  if (!(stat & 1))
  {
    printf("Unexpected error in InitCli, error %d in retrieving $LINE\n",
            stat);
    return(stat);
  }

  /* this code assumes that the verb is shorter than than the foreign
     symbol (which includes device:[dir], so, should be reasonable) */
  for (indx = 0; (indx < cmd.dsc$w_length) &&
              (cmd.dsc$a_pointer[indx] != ' ') &&
              (cmd.dsc$a_pointer[indx] != '/'); ++indx)
    cmd.dsc$a_pointer[indx] = (indx < strlen(name))?name[indx]:' ';

  if (indx < strlen(name))
#ifdef ARF      /* added by lww - 26-Mar-1998 */
    printf("Unexpected error in InitCli, ");
    printf("verb name wasn't smaller than $line prefix\n");
#else
    printf("\n%%%s-E-NOFORSYMB, must be invoked via foreign symbol\n\n",
            CmdVerb);
#endif 
  return(cli$dcl_parse(&cmd,table)); /* initialize the parse tables */
}

int lib$sig_to_ret();

int cli_get_value(struct dsc$descriptor *s1, struct dsc$descriptor *s2)
{
  lib$establish(lib$sig_to_ret);
  return cli$get_value(s1, s2);
}


