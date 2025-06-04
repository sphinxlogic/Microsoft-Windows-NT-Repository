/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
 *                %% \___________________________________%% \
 *                %% |                                   %%  \
 *                %% |          pwdexp.c  c1997          %%   \
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
 *  Show dates a specified user changed password and when that password
 *  will expire. Additionally, if user has been DISUSER'ed also display
 *  that information. If caller is not sufficiently priveliged, then exit.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define VERSION "V1.11"
#module misc VERSION

#define PRV$M_SYSPRV 0x10000000
#define PRV$M_BYPASS 0x20000000
#define RMS$_RNF 98994

#include <stdio.h>
#include <descrip.h>
#include <ssdef.h>
#include <jpidef.h>
#include <libdef.h>
#include <uaidef.h>

char    *month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char    user[32];

long    PasswdChange[2];
long    PasswdLife[2];
long    PasswdExpire[2];
long    PasswdDelta[2];

short   ChangeStruct[7];
short   ExpireStruct[7];
short   DeltaStruct[7];

int     flags;
int     iosb[2];
int     status;
struct  {short len, code; int *bufadr, *retlen;} JpiItem[2];
struct  {short len, code; int *bufadr, *retlen;} UaiItem[3];
$DESCRIPTOR(dsc_user, user);




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
    char flagstr[32];

    UaiItem[0].len = 4;
    UaiItem[0].code = UAI$_FLAGS;
    UaiItem[0].bufadr = &flags;
    UaiItem[0].retlen = 0;
    UaiItem[1].len = 0;
    UaiItem[1].code = 0;

    dsc_user.dsc$w_length = strlen(user);
    status = sys$getuai(0, 0, &dsc_user, &UaiItem, 0, 0, 0);
    if(UAI$M_DISACNT & flags) {
        strcpy(flagstr, "DISUSER");
        printf("\t%10s: %s\n\n", "Flags", flagstr);
        exit();
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: DispMsg
 * Description: Generic error exit routine. Caller places error status into
 *              global variable 'status' which is used as input to switch
 *              statement to generate exit message. Message style is similar
 *              to VMS error messages. but specific to 'Pwdexp.c'
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void DispMsg(int errnum)
{
    char msgbuf[132];

    switch(errnum) {
        case SS$_NOSUCHUSER:
        case RMS$_RNF:
            sprintf(msgbuf, "%%PWDEXP-E-NOTUSER, no such user available\n");
            break;

        case SS$_NOTALLPRIV:
            sprintf(msgbuf, "%%PWDEXP-E-NOTPRIV, insufficient process privs\n");
            break;

        default:
            sprintf(msgbuf, "%%PWDEXP-E-NOMSG, VMS error 0x%08X\n",
                    status);
    }
    printf("%s\n\n", msgbuf);
    exit();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: CheckPrv
 * Description: Verify person executing this image has sufficient privs
 *              to access SYSUAF.DAT to collect required info. If not,
 *              display message regarding insufficient privs and exit.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int CheckPrv()
{
    int ProcPriv[2] = 0;
    int stat;

    JpiItem[0].len = sizeof(ProcPriv);
    JpiItem[0].code = JPI$_PROCPRIV;
    JpiItem[0].bufadr = &ProcPriv;
    JpiItem[0].retlen = 0;
    JpiItem[1].len = 0;
    JpiItem[1].code = 0;

    stat = sys$getjpiw(0, 0, 0, &JpiItem, &iosb, 0, 0);
    if(ProcPriv[0] & PRV$M_SYSPRV || ProcPriv[0] & PRV$M_BYPASS) return;
    DispMsg(SS$_NOTALLPRIV);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Function: main
 * Description: 
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main(int argc, char *argv[])
{
    short   chg_len;
    short   date_len;
    short   life_len;
    short   user_len;

    if(argc != 2) {
        puts("Usage: pwdexp username\n");
        sys$exit(1);
    }
    CheckPrv();
    dsc_user.dsc$a_pointer = user;
    dsc_user.dsc$w_length = strlen(argv[1]);
    strcpy(user, argv[1]);
    status = str$upcase(&dsc_user, &dsc_user);

    UaiItem[0].len = 8;
    UaiItem[0].code = UAI$_PWD_DATE;
    UaiItem[0].bufadr = &PasswdChange;
    UaiItem[0].retlen = &chg_len;
    UaiItem[1].len = 8;
    UaiItem[1].code = UAI$_PWD_LIFETIME;
    UaiItem[1].bufadr = &PasswdLife;
    UaiItem[1].retlen = &life_len;
    UaiItem[2].len = 0;
    UaiItem[2].code = 0;

    status = sys$getuai(0, 0, &dsc_user, &UaiItem, 0, 0, 0);
    if(status == SS$_NORMAL) {
        printf("\n\t  Username: %s\n", user);
        status = sys$numtim(&ChangeStruct, &PasswdChange);
        if(!ChangeStruct[0]) {  /* pre-expired passwd */
            printf("\tPassChange: Unknown\n");
            printf("\tPassExpire: PreExpired\n");
            ChkFlags();
            printf("\n");
            exit(1);
        }
        if(ChangeStruct[0] == 1858) printf("\tPassChange: None\n");
        else {
            printf("\tPassChange: %2d-%s-%d\n", ChangeStruct[2],
                    month[ChangeStruct[1]-1], ChangeStruct[0]);
        }
        if(!PasswdLife[1]) printf("\tPassExpire: None\n");
        else {
            status = lib$add_times(&PasswdChange, &PasswdLife, &PasswdExpire);
            if((status) == LIB$_NORMAL) {
                status = sys$numtim(&DeltaStruct, &PasswdLife);
                status = sys$numtim(&ExpireStruct, &PasswdExpire);
                printf("\tPassExpire: %2d-%s-%d (%d days)\n",
                        ExpireStruct[2], month[ExpireStruct[1]-1], 
                        ExpireStruct[0], DeltaStruct[2]);
            }
        }
        ChkFlags();
        printf("\n");
        exit(1);
    }
    else DispMsg(status);
}
