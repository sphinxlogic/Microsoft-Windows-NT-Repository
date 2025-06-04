/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
 *                %% \___________________________________%% \
 *                %% |                                   %%  \
 *                %% |       reset_passwd.c  c2001       %%   \
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
 *  Permits suitably priv'ed user to change password for a specified
 *  user. Caller need not enter current password, but must enter new
 *  password twice to complete successfully. In the event target user 
 *  is DISUSER'ed, this image will exit with approriate message, and
 *  target password remains unchanged.
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define VERSION "V1.15"
#pragma module reset_passwd VERSION   /* assume DecC V5 or higher */

#define PRV$M_SYSPRV 0x10000000
#define PRV$M_BYPASS 0x20000000
#define RMS$_RNF 0x182b2
#define SS$_PWDNOTVER 0x0003888A

#include <stdio.h>
#include <string.h>
#include <descrip.h>
#include <ssdef.h>
#include <jpidef.h>
#include <libdef.h>
#include <lnmdef.h>
#include <iodef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <uaidef.h>

typedef struct {            /* terminal characteristics block */
  unsigned char class, term;
  unsigned short width;
  unsigned bchar : 24;
  unsigned char page;
  unsigned int xchar;
} CHARBLK;

typedef struct {            /* read iosb structure */
    short status;
    short length;
    short termchr;
    short termsiz;
} IOSB;

char    CmdVerb[50];        /* foreign symbol used to invoke this image */
char    PwdMinGrp[12];      /* string value from logical PWDMINGROUP */
char    ImageUser[33];      /* username of invoking user */
char    TargetUser[33];     /* username of target user */
char    Terminal[32];       /* current terminal device */
char    flagstr[80];        /* symbolic list of target UAF flags */
char    PwdBuf[32];         /* buffer for user entered passwords */
int     CurPriv[2];
int     flags;              /* UAF flags for target user */
int     iochan;             /* channel for TT: */
int     MinGrp;             /* binary value from PWDMINGROUP logical */
int     status;
int     Uic, member, group;

struct  {short len, code; int *bufadr, *retlen;} JpiItem[4];
struct  {short len, code; int *bufadr, *retlen;} LnmItem[3];
struct  {short len, code; int *bufadr, *retlen;} UaiItem[4];

CHARBLK newmode, savmode;
IOSB    iosb;

$DESCRIPTOR(dsc_dev, "TT");
$DESCRIPTOR(Dsc_Image, ImageUser);
$DESCRIPTOR(Dsc_Usr, TargetUser);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: DispMsg
 * Description: Something has gone ary. Caller has specified which msg
 *              to display based on the contents of global integer 'status'
 *              Display message, then exit
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void DispMsg(int msgval)
{
    char msgbuf[132];

    switch(msgval) {
        case SS$_NOSUCHUSER:
        case RMS$_RNF:
            sprintf(msgbuf, "%%%s-E-NOUSER, no such user\n", CmdVerb);
            break;

        case SS$_NOTALLPRIV:
            sprintf(msgbuf, "%%%s-E-NOPRIV, insufficient process privs\n",
                    CmdVerb);
            break;

        case SS$_NOSUCHID:
            sprintf(msgbuf, "%%%s-E-NOSUCHID, unknown rights identifier\n",
                    CmdVerb);
            break;

        case SS$_IVIDENT:
            sprintf(msgbuf, "%%%s-E-NOAUTH, %s is not authorized to modify password\n",
                    CmdVerb, ImageUser);
            break;

        case SS$_PWDNOTVER:
            sprintf(msgbuf, "%%%s-E-PWDNOTVER, password verification error\n",
                    CmdVerb);
            break;

        case SS$_NOLOGNAM:
        case SS$_NOTRAN:
            sprintf(msgbuf, "%%%s-E-NOTRAN, no translation for logical name PWDMINGROUP\n",
                    CmdVerb); 
            break;

        default:
            sprintf(msgbuf, "%%%s-E-NOMSG, VMS error 0x%08X\n",
                    CmdVerb, status);
    }
    printf("%s\n", msgbuf);
    exit();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: GetLnm
 * Description: Check the SYSTEM logical name table for definition of
 *              PWDMINGROUP. If it exists, the equivalance string is
 *              placed in the global integer MinGrp. If there is not an
 *              equivalence defined, Return status of $TRNLNM call
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int GetLnm()
{
    char    LogLnm[16];            /* target logical name */
    char    LnmTable[32];

    short   PwdMin_len;
    short   LnmTable_len;

    int     LnmStat;

    $DESCRIPTOR(Dsc_lnm, LogLnm);      /* logical name for persona logging */
    $DESCRIPTOR(Dsc_tabnam, LnmTable); /* lnm table to check */

    LnmItem[0].len = sizeof(LnmTable);
    LnmItem[0].code = LNM$_TABLE;
    LnmItem[0].bufadr = &LnmTable;
    LnmItem[0].retlen = &LnmTable_len;
    LnmItem[1].len = sizeof(PwdMinGrp);
    LnmItem[1].code = LNM$_STRING;
    LnmItem[1].bufadr = &PwdMinGrp;
    LnmItem[1].retlen = &PwdMin_len;
    LnmItem[2].len = 0;
    LnmItem[2].code = 0;

    strcpy(LnmTable,"LNM$SYSTEM_TABLE");
    sprintf(LogLnm, "PWDMINGROUP");
    Dsc_lnm.dsc$w_length = strlen(LogLnm);
    Dsc_tabnam.dsc$w_length = strlen(LnmTable);
    LnmStat = sys$trnlnm(0, &Dsc_tabnam, &Dsc_lnm, 0, &LnmItem);
    return(LnmStat);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: EchoOff
 * Description: Use QIOW to save entry terminal settings, then again use
 *              QIOW to set terminal device so user entries will not be
 *              echoed to screen when new password is entered.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void EchoOff()
{
    status = sys$assign(&dsc_dev, &iochan, 0, 0);
    status = sys$qiow(0, iochan, IO$_SENSEMODE, &iosb, 0, 0, &savmode,
                      sizeof(savmode), 0, 0, 0, 0);
    newmode = savmode;
    newmode.bchar |= TT$M_NOECHO;       /* term will be Noecho */
    newmode.bchar &= ~TT$M_ESCAPE;      /* no escape */
    newmode.bchar |= TT$M_NOBRDCST;     /* disable direct broadcast */
    newmode.bchar &= ~TT$M_HOSTSYNC;    /* no host sync */
    newmode.bchar &= ~TT$M_TTSYNC;      /* do sync at subprocess level */

    status = sys$qiow(0, iochan, IO$_SETMODE, &iosb, 0, 0, &newmode,
                      sizeof(newmode), 0, 0, 0, 0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: EchoOn
 * Description: Restore terminal status to same state as it was at the
 *              time this image was activated. Called after selected
 *              users password has been entered.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void EchoOn()
{
    status = sys$qiow(0, iochan, IO$_SETMODE, &iosb, 0, 0, &savmode,
                      sizeof(savmode), 0, 0, 0, 0);
    status = sys$dassgn(iochan);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: TtGets
 * Description: Read character string from the keyboard, perform no
 *              editing, no echo. Purge typeahead buffer at entry.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
TtGets()
{
    status = sys$qiow(0, iochan, IO$_READLBLK|IO$M_PURGE, &iosb, 0, 0,
                      &PwdBuf, sizeof(PwdBuf), 0, 0, 0, 0);
    PwdBuf[iosb.length] = '\0';
    return (iosb.length);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: ChkFlags
 * Description: Check login flags for specified user. Currently, only
 *              concerned with DISUSER flag. If this flag bit is set,
 *              display the flag status and exit, else return. Also, 
 *              checks to see if username is a valid user.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ChkFlags()
{
    UaiItem[0].len = 4;
    UaiItem[0].code = UAI$_FLAGS;
    UaiItem[0].bufadr = &flags;
    UaiItem[0].retlen = 0;
    UaiItem[1].len = 0;
    UaiItem[1].code = 0;
    status = sys$getuai(0, 0, &Dsc_Usr, UaiItem, 0, 0, 0);
    if(status == RMS$_RNF) DispMsg(status);  /* NOSUCHUSER */
    if(UAI$M_DISACNT & flags) {
        strcat(flagstr, "DISUSER");
        printf("%%%s-E-FLAGS, user flag '%s' prevents password change\n\n",
                CmdVerb, flagstr);
        exit();
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: ChkIdent
 * Description: Reads the rights database to verify invoking user has
 *              PWD_SETPASS identifier granted to modify the password of
 *              the target user. If not, call DispMsg with return status
 *              of SS$_IVIDENT else just return
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ChkIdent()
{
    char IdentStr[31];
    unsigned int HolderBin[2] = 0;
#ifdef ARF
    unsigned int IdentBin[2] = 0;
#else
    unsigned int IdentBin = 0;
#endif
    unsigned int HeldBin, context = 0;
    unsigned short count = 0;
    $DESCRIPTOR(Dsc_Id, IdentStr);

        /* get invoking username, convert to binary value */

    status = sys$asctoid ( &Dsc_Image, &HolderBin, 0);
    if(!(status & 1)) lib$stop(status);

        /* convert required identifier string to binary value */

    strcpy(IdentStr, "PWD_SETPASS");
    Dsc_Id.dsc$w_length = strlen(IdentStr);
    status = sys$asctoid(&Dsc_Id, &IdentBin, 0);
    if(!(status & 1)) DispMsg(status);   /* probably SS$_NOSUCHID */

        /* Make repeated calls to SYS$FIND_HELD to return
           all identifiers held by holder (ImageUser) */

    HolderBin[1] = 0;
#ifdef ARF
    IdentBin[1] = 0;
#endif
    while(status == SS$_NORMAL) {
        status = sys$find_held(&HolderBin, &HeldBin, 0, &context);
        if(status == SS$_NORMAL) {
            if(HeldBin == IdentBin) {
                status = sys$finish_rdb(&context); /* rights database context */
                return;
            }
        }
        if(status == SS$_IVIDENT || status == SS$_NOSUCHID) DispMsg(SS$_IVIDENT);
        if(status != SS$_NORMAL) DispMsg(status);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: ChkCaller
 * Description: Get environmental info regarding invoking user, specifically,
 *              the terminal being used, the username of the user to verify
 *              who they are.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ChkCaller()
{
    char *ptr2 = 0;
    short termlen;

    JpiItem[0].len = sizeof(Terminal);
    JpiItem[0].code = JPI$_TERMINAL;
    JpiItem[0].bufadr = &Terminal;
    JpiItem[0].retlen = &termlen;
    JpiItem[1].len = sizeof(ImageUser);
    JpiItem[1].code = JPI$_USERNAME;
    JpiItem[1].bufadr = &ImageUser;
    JpiItem[1].retlen = 0;
    JpiItem[2].len = 0;
    JpiItem[2].code = 0;

    status = sys$getjpiw(0, 0, 0, &JpiItem, &iosb, 0, 0);
    ptr2 = strchr(ImageUser, ' ');
    if(ptr2) {
        *ptr2 = '\0';
        Dsc_Image.dsc$w_length = strlen(ImageUser);
        return;
    }
    DispMsg(SS$_NOTALLPRIV);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: ChkPriv
 * Description: Call LIB$GETJPI to see if we are installed with the required
 *              privs. If so return SS$_NORMAL, else see if user can set the
 *              required privs from their process privs. If not, we have to
 *              return SS$_NOTALLPRIV to indicate this image cannot be run
 *              due to insufficent privs.
 *              (NOTE: if the 'ifdef' statements below are changed to 'ifndef'
 *              runtime status messages for this routine will be displayed).
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ChkPriv()
{
    int EnabFlg = 1;               /* Enable flag for $SETPRV */
    int ImagePriv[2] = 0;          /* privs for installed image */
    int PrivMask[2] = 0;           /* privs we want to check/set */
    int PrivStat = 0;              /* return status */
    int StsFlg;
    int jpicode = JPI$_IMAGPRIV;

    PrivMask[0] = PRV$M_SYSPRV | PRV$M_BYPASS;

    PrivStat = lib$getjpi(&jpicode, 0, 0, &ImagePriv, 0, 0 );
#ifdef ARF
    printf("\nlib$getjpi(PrivStat): %08x, ImagePriv[0]: %08x\n",
             PrivStat, ImagePriv[0]);
#endif
    if(PrivStat != SS$_NORMAL) lib$stop(PrivStat); /* should never execute this */
    if(ImagePriv[0] == PrivMask[0]) return;


        /* Set the required priv bits for this image. If returned status
           is SS$_NOTALLPRIV then we exit due to insufficient privs */

#ifdef ARF
    StsFlg = PRV$M_SYSPRV | PRV$M_BYPASS;

    PrivStat = sys$setprv(EnabFlg, &StsFlg, 0, 0);
#else
    PrivStat = sys$setprv(EnabFlg, &PrivMask, 0, 0);
#endif

#ifdef ARF
    printf("\nsys$setprv(PrivStat): 0X%08x\n\n", PrivStat);
#endif
    if(PrivStat != SS$_NORMAL) DispMsg(SS$_NOTALLPRIV);
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: Main
 * Description: This is the primary function of the tool, to check the
 *              invoking user's privs, valid username, and getting the
 *              new password string and entering into SYSUAF.DAT
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main(int argc, char *argv[])
{
    char    passw1[32];             /* first new password entry string */
    char    passw2[32];             /* second new password entry string */
    char    *ptr1;
    short   CvLen;                  /* Length of command line string */
    int     indx;
    $DESCRIPTOR(Dsc_Cmd, CmdVerb);  /* foreign invoking symbol name */
    $DESCRIPTOR(Dsc_Pwd, PwdBuf);
    static unsigned char alg;       /* target user password algorithm */
    static unsigned short salt;     /* target user password salt */
    static unsigned long old_hash[2], new_hash[2], stat;

    if (argc != 2){
        printf("%%%s-E-NOUSER, Username parameter missing\n\n", CmdVerb);
        sys$exit(1);
    }

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

    strcpy(TargetUser, argv[1]);
    Dsc_Usr.dsc$a_pointer = TargetUser;
    Dsc_Usr.dsc$w_length = strlen(TargetUser);
    status = str$upcase(&Dsc_Usr, &Dsc_Usr);

        /* perform necessary security/target checks */

    ChkCaller();    /* get basic invoking user info */
    ChkFlags();     /* check if target user is DISUSERed */
    ChkPriv();      /* check for required privs (SYSPRV, BYPASS) */
    ChkIdent();     /* check for required identifier (SETPASS) */

        /* above call to ChkFlags verified valid user, get the uic */

    UaiItem[0].len = sizeof(Uic);
    UaiItem[0].code = UAI$_UIC;
    UaiItem[0].bufadr = &Uic;
    UaiItem[0].retlen  = 0;
    UaiItem[1].len = 0;
    UaiItem[1].code = 0;

    status = sys$getuai(0, 0, &Dsc_Usr, UaiItem, 0, 0, 0);
    if(status != SS$_NORMAL) DispMsg(status);
    status = GetLnm();      /* see if required logical is defined */
    if(status != SS$_NORMAL) DispMsg(status);
    member = Uic & 0x1fff;
    group = (Uic & 0x1fff0000) >>16;
    sscanf(PwdMinGrp, "%o", &MinGrp);   /* this value is octal */
    if(group < MinGrp) {
        printf("%%%s-F-NOTAUTH, You are not authorized to modify %s\n\n",
                CmdVerb, argv[1]);
        sys$exit(1);
    }
    EchoOff();
    printf("\nNew Password: ");
    status = TtGets();
    strcpy(passw1, PwdBuf);
    printf("\nVerification: ");
    status = TtGets();
    strcpy(passw2, PwdBuf);
    EchoOn();
    if(strcmp(passw1, passw2)) {        /* password strings do not match */
        printf("\n");
        DispMsg(231562);
    }
    printf("\n");

    UaiItem[0].len = 1;
    UaiItem[0].code = UAI$_ENCRYPT;
    UaiItem[0].bufadr = &alg;
    UaiItem[0].retlen = 0;
    UaiItem[1].len = 2;
    UaiItem[1].code = UAI$_SALT;
    UaiItem[1].bufadr = &salt;
    UaiItem[1].retlen = 0;
    UaiItem[2].len = 0;
    UaiItem[2].code = 0;

    status = sys$getuai(0, 0, &Dsc_Usr, UaiItem, 0, 0, 0);
    if(status != SS$_NORMAL) DispMsg(status);

    UaiItem[0].len = 8;
    UaiItem[0].code = UAI$_PWD;
    UaiItem[0].bufadr = &new_hash;
    UaiItem[0].retlen = 0;
    UaiItem[1].len = 0;
    UaiItem[1].code = 0;

    Dsc_Pwd.dsc$w_length = strlen(PwdBuf);
    status = str$upcase(&Dsc_Pwd, &Dsc_Pwd);
    status = sys$hash_password(&Dsc_Pwd, alg, salt, &Dsc_Usr, new_hash);
    status = sys$setuai(0, 0, &Dsc_Usr, UaiItem, 0, 0, 0);
    if(status != SS$_NORMAL) sys$exit(status);
    else printf("\n%%%s-I-SUCCESS, password succesfully changed\n\n", CmdVerb);
}
