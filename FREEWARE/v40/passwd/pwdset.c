/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
 *                %% \___________________________________%% \
 *                %% |                                   %%  \
 *                %% |          pwdset.c  c1997          %%   \
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
 *  Copyright (C) 1997 Lyle W. West, All Rights Reserved.
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

#define VERSION "V1.11"
#module pwdset VERSION

#define PRV$M_SYSPRV 0x10000000
#define PRV$M_BYPASS 0x20000000
#define RMS$_RNF 98994
#define SS$_PWDERRVER 276268100

#include <stdio.h>
#include <descrip.h>
#include <ssdef.h>
#include <jpidef.h>
#include <libdef.h>
#include <iodef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <uaidef.h>

typedef struct {
  unsigned char class, term;
  unsigned short width;
  unsigned bchar : 24;
  unsigned char page;
  unsigned int xchar;
} CHARBLK;

typedef struct {    /* read iosb structure */
    short status;
    short length;
    short termchr;
    short termsiz;
} IOSB;

char    Terminal[32];
char    flagstr[80];
char    pwdbuf[32];
int     flags;
int     iochan;
int     status;
struct  {short len, code; int *bufadr, *retlen;} JpiItem[3];
struct  {short len, code; int *bufadr, *retlen;} UaiItem[4];
IOSB    iosb;
$DESCRIPTOR(dsc_dev, "TT");
$DESCRIPTOR(usrnam, "");

CHARBLK newmode, savmode;


void DispMsg(int msgval)
{
    char msgbuf[132];

    switch(msgval) {
        case SS$_NOSUCHUSER:
        case RMS$_RNF:
            sprintf(msgbuf, "%%PWDSET-E-NOUSER, no such user registered\n");
            break;

        case SS$_NOTALLPRIV:
            sprintf(msgbuf, "%%PWDSET-E-NOPRIV, insufficient process privs\n");
            break;

        case SS$_PWDERRVER:
            sprintf(msgbuf, "%%PWDSET-E-PWDERRVER, new password verification error\n");
            break;

        default:
            sprintf(msgbuf, "%%PWDSET-E-NOMSG, VMS error 0x%08X\n", status);
    }
    printf("%s\n", msgbuf);
    exit();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: EchoOff
 * Description: Check login flags for specified user. Currently, only
 *              concerned with DISUSER flag. If this flag bit is set,
 *              display the flag status and exit, else return.
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

void EchoOn()
{
    status = sys$qiow(0, iochan, IO$_SETMODE, &iosb, 0, 0, &savmode,
                      sizeof(savmode), 0, 0, 0, 0);
    status = sys$dassgn(iochan);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Read character string from the keyboard, perform no editing, no echo
 * Purge typeahead buffer at entry.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

TtGets()
{
    status = SYS$QIOW(0, iochan, IO$_READLBLK|IO$M_PURGE, &iosb, 0, 0,
                      &pwdbuf, sizeof(pwdbuf), 0, 0, 0, 0);
    pwdbuf[iosb.length] = '\0';
    return (iosb.length);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: ChkFlags
 * Description: Check login flags for specified user. Currently, only
 *              concerned with DISUSER flag. If this flag bit is set,
 *              display the flag status and exit, else return.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int ChkFlags()
{
    UaiItem[0].len = 4;
    UaiItem[0].code = UAI$_FLAGS;
    UaiItem[0].bufadr = &flags;
    UaiItem[0].retlen = 0;
    UaiItem[1].len = 0;
    UaiItem[1].code = 0;
    status = sys$getuai(0, 0, &usrnam, UaiItem, 0, 0, 0);
    if(status == 0x182b2) DispMsg(status);
    if(UAI$M_DISACNT & flags) {
        strcat(flagstr, "DISUSER");
        printf("%%PWDSET-E-FLAGS, user flag '%s' prevents password change\n\n",
                flagstr);
        exit();
    }
}


int CheckPrv()
{
    int ProcPriv[2] = 0;

    JpiItem[0].len = sizeof(ProcPriv);
    JpiItem[0].code = JPI$_PROCPRIV;
    JpiItem[0].bufadr = &ProcPriv;
    JpiItem[0].retlen = 0;
    JpiItem[1].len = sizeof(Terminal);
    JpiItem[1].code = JPI$_TERMINAL;
    JpiItem[1].bufadr = &Terminal;
    JpiItem[1].retlen = 0;
    JpiItem[2].len = 0;
    JpiItem[2].code = 0;

    status = sys$getjpiw(0, 0, 0, &JpiItem, &iosb, 0, 0);
    if(ProcPriv[0] & PRV$M_SYSPRV || ProcPriv[0] & PRV$M_BYPASS) return;
    DispMsg(SS$_NOTALLPRIV);
}

int main(int argc, char **argv)
{
    char    passw1[32];
    char    passw2[32];

    $DESCRIPTOR(new_pwd, pwdbuf);
    static unsigned char alg;
    static unsigned short salt;
    static unsigned long old_hash[2], new_hash[2], stat;

    if (argc != 2){
        puts("Usage: pwdset username\n\n");
        sys$exit(1);
    }

    CheckPrv();
    usrnam.dsc$a_pointer = argv[1];
    usrnam.dsc$w_length = strlen(argv[1]);
    status = str$upcase(&usrnam, &usrnam);
    ChkFlags();
    EchoOff();
    printf("New Password: ");
    status = TtGets();
    strcpy(passw1, pwdbuf);
    printf("\nVerification: ");
    status = TtGets();
    strcpy(passw2, pwdbuf);
    EchoOn();
    if(strcmp(passw1, passw2)) {
        printf("\n");
        DispMsg(276268100);
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

    status = sys$getuai(0, 0, &usrnam, UaiItem, 0, 0, 0);

    UaiItem[0].len = 8;
    UaiItem[0].code = UAI$_PWD;
    UaiItem[0].bufadr = &new_hash;
    UaiItem[0].retlen = 0;
    UaiItem[1].len = 0;
    UaiItem[1].code = 0;

    new_pwd.dsc$w_length = strlen(pwdbuf);
    status = str$upcase(&new_pwd, &new_pwd);
    status = sys$hash_password(&new_pwd, alg, salt, &usrnam, new_hash);
    status = sys$setuai(0, 0, &usrnam, UaiItem, 0, 0, 0);
    if(status != SS$_NORMAL) sys$exit(status);
    printf("\n");
}
