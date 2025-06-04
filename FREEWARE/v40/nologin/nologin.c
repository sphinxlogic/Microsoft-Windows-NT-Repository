/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
 *                %% \___________________________________%% \
 *                %% |                                   %%  \
 *                %% |          nologin.c  c1998         %%   \
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
 *  Copyright (C) 1998 Lyle W. West, All Rights Reserved.
 *  Permission is granted to copy and use this program so long as [1] this
 *  copyright notice is preserved, and [2] no financial gain is involved
 *  in copying the program.  This program may not be sold as "shareware"
 *  or "public domain" software without the express, written permission
 *  of the author.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "version.h"
#module nologin VERSION

#include <stdio.h>
#include <rms.h>
#include <descrip.h>
#include <ssdef.h>
#include <jpidef.h>
#include <syidef.h>
#include <libdef.h>
#include <clidef.h>
#include <climsgdef.h>
#include <stdlib.h>
#include <uaidef.h>


#define PRV$M_SYSPRV 0x10000000
#define PRV$M_BYPASS 0x20000000

globalvalue NOLOGIN_CLD;

char    cmdverb[80];        /* foreign symbol used to invoke this image */
char    comfile[80];        /* filespec of .com file to get sysuaf.lis */
char    defdevstr[33];      /* users default device string */
char    defdirstr[65];      /* users default directory string */
char    owner[34];          /* users owner string */
char    ufiltstr[34];       /* filter string for USER_FILTER string */
char    user[14];           /* user record string */
char    *ptr1;              /* general purpose character pointer */
char    *ptr2;              /* general purpose character pointer */

long    LastIlogin[2];      /* binary date for last interactive login */
long    LastNlogin[2];      /* binary date for last noninteractive login */
long    ThisDate[2];        /* binary date for today */
long    TimeDelta[2];       /* binary delta time */

short   Idays;              /* number of days since interactive login */
short   Ndays;              /* number of days since non-interactive login */
short   NumIdleDays;        /* delta time check value for user account tests */
short   NumUsrAcnts;        /* number of user accounts >= NumIdleDays */
short   LoginStruct[7];     /* $numtime output structure for login date */
short   TodayStruct[7];     /* $numtime output structure for today */
short   iDeltaStruct[7];    /* $numtime output structure for idelta time */
short   nDeltaStruct[7];    /* $numtime output structure for ndelta time */
short   noninter_flg;       /* if true, display days since last non-inter */
short   owner_flg;          /* if true, display owner string */
short   stats_flg;          /* if true, display statistics info */
short   ufilt_flg;          /* if true, display only valid filtered users */

int     iosb[2];
int     status;

FILE *uafcom_fp;            /* file pointer for create list file */
FILE *output_fp;            /* file pointer to output log file */
FILE *uaflst_fp;            /* file pointer to SYSUAF.LIS file */

struct  {short len, code; int *bufadr, *retlen;} UaiItem[6];


void GetUafList()
{
    char    outfile[12];
#ifdef __DECC
    int     flags = 0x1c4;
#else
    int     flags = 0;
#endif
    int     taskpid = 0;
    $DESCRIPTOR (dsc_com, comfile);
    $DESCRIPTOR (dsc_out, outfile);

    sprintf(comfile, "%s", "sys$scratch:create_uaflis.com");
    sprintf(outfile, "%s", "NLA0:");
    status = UafListCom();
    dsc_com.dsc$w_length = strlen(comfile);
    dsc_out.dsc$w_length = strlen(outfile);
    status = lib$spawn(0,       /* command line */
             &dsc_com,          /* input file */
             &dsc_out,          /* output file */
             &flags,            /* flags */
             0,                 /* process name */
             &taskpid,          /* process id */
             0,                 /* completion status */
             0,                 /* event flag num */
             0,                 /* AST address */
             0,                 /* AST argument */
             0,                 /* prompt string */
             0);                /* cli */

    if(status != SS$_NORMAL) {
        printf("%%%s-E-SPAWN, LIB$SPAWN failed to create SYSUAF.LIS\n\n",
            cmdverb);
        exit(status);
    }
}


void GetUserInfo()
{
    char    prntbuf[132];
    int     numchrs = 0;
    short   prntflg = 0;
    short   tmp_len;
    $DESCRIPTOR(dsc_user, user);
    
    dsc_user.dsc$w_length = strlen(user);
    status = sys$getuai(0, 0, &dsc_user, &UaiItem, 0, 0, 0);

    ptr1 = &owner;
    tmp_len = *ptr1;
    ptr1++;
    strcpy(owner, ptr1);
    owner[tmp_len] = '\0';
    if(strlen(owner) > 20) owner[20] = '\0';
    ptr1 = &defdevstr;
    tmp_len = *ptr1;
    ptr1++;
    strcpy(defdevstr, ptr1);
    defdevstr[tmp_len] = '\0';

    ptr1 = &defdirstr;
    tmp_len = *ptr1;
    ptr1++;
    strcpy(defdirstr, ptr1);
    defdirstr[tmp_len] = '\0';

    numchrs = sprintf(prntbuf, "%-13s", user);
    if(owner_flg) sprintf(prntbuf+numchrs, "%-21s", owner);
    status = sys$numtim(&LoginStruct, &LastIlogin);
    numchrs = strlen(prntbuf);
    if(LoginStruct[0] == 1858) {
        prntflg++;
        sprintf(prntbuf+numchrs, "%4s ", "-");
    }
    else {
        status = lib$sub_times(&ThisDate, &LastIlogin, &TimeDelta);
        status = sys$numtim(&iDeltaStruct, &TimeDelta);
        if(iDeltaStruct[2] >= NumIdleDays) {
            prntflg++;
            if(iDeltaStruct[2] >= 9999) iDeltaStruct[2] = 9999;
            sprintf(prntbuf+numchrs, "%4d ", iDeltaStruct[2]);
        }
        else {
            numchrs = strlen(prntbuf);
            sprintf(prntbuf+numchrs, "%4c ", 0x20);
        }
    }
    numchrs = strlen(prntbuf);

    if(noninter_flg) {
        status = sys$numtim(&LoginStruct, &LastNlogin);
        if(LoginStruct[0] == 1858) {
            prntflg++;
            sprintf(prntbuf+numchrs, "%4s ", "-");
            numchrs = strlen(prntbuf);
        }
        else {
            status = lib$sub_times(&ThisDate, &LastNlogin, &TimeDelta);
            status = sys$numtim(&nDeltaStruct, &TimeDelta);
            if(nDeltaStruct[2] >= NumIdleDays) {
                prntflg++;
                if(nDeltaStruct[2] >= 9999) nDeltaStruct[2] = 9999;
                sprintf(prntbuf+numchrs, "%4d ", nDeltaStruct[2]);
                numchrs = strlen(prntbuf);
            }
            else {
                sprintf(prntbuf+numchrs, "%4c ", 0x20);
                numchrs = strlen(prntbuf);
            }
        }
    }
    sprintf(prntbuf+numchrs, " %s%s", defdevstr, defdirstr);
    if(prntflg) {
        fprintf(output_fp, " %s\n", prntbuf);
        NumUsrAcnts++;
    }
}

int main()
{
    char    buffer[132];
    char    clistr[80];
    char    usr_str[16];
    int     clistat;
    int     cv_len = 0;
    int     indx;
    short   equ_len = 0;
    short   ilogin_len;
    short   nlogin_len;
    short   own_len;
    short   usr_len = 0;
    $DESCRIPTOR(dsc_cli, clistr);       /* cli param or qualifier string */
    $DESCRIPTOR(dsc_cmd, cmdverb);      /* foreign invoking symbol name */
    $DESCRIPTOR(dsc_buf, buffer);       /* get number user inactive days */

    UaiItem[0].len = 8;
    UaiItem[0].code = UAI$_LASTLOGIN_I;
    UaiItem[0].bufadr = &LastIlogin;
    UaiItem[0].retlen = &ilogin_len;
    UaiItem[1].len = 8;
    UaiItem[1].code = UAI$_LASTLOGIN_N;
    UaiItem[1].bufadr = &LastNlogin;
    UaiItem[1].retlen = &nlogin_len;
    UaiItem[2].len = sizeof(owner);
    UaiItem[2].code = UAI$_OWNER;
    UaiItem[2].bufadr = &owner;
    UaiItem[2].retlen = &own_len;
    UaiItem[3].len = sizeof(defdevstr);
    UaiItem[3].code = UAI$_DEFDEV;
    UaiItem[3].bufadr = &defdevstr;
    UaiItem[3].retlen = 0;
    UaiItem[4].len = sizeof(defdirstr);
    UaiItem[4].code = UAI$_DEFDIR;
    UaiItem[4].bufadr = &defdirstr;
    UaiItem[4].retlen = 0;
    UaiItem[5].len = 0;
    UaiItem[5].code = 0;


    clistat = cmdlin(&dsc_cmd,&cv_len);  /* get invoking foreign cmd str */
    if(clistat != SS$_NORMAL) exit(clistat);

    cmdverb[cv_len] = '\0';
    ptr1 = strchr(cmdverb,'/');
    if(ptr1) *ptr1 = '\0';
    ptr1 = strchr(cmdverb,' ');
    if(ptr1) *ptr1 = '\0';
    for(indx = 0; indx < strlen(cmdverb); indx++)
        cmdverb[indx] = toupper(cmdverb[indx]);

    clistat = init_cli(NOLOGIN_CLD, "NOLOGIN");
    if((clistat & 1) == 0) exit(1);

    strcpy(clistr,"HELP");
    dsc_cli.dsc$w_length = strlen(dsc_cli.dsc$a_pointer);
    clistat = cli$present(&dsc_cli);
    if(clistat == CLI$_PRESENT) sho_hlp();

    strcpy(clistr, "VERSION");
    dsc_cli.dsc$w_length = strlen(clistr);
    clistat = cli$present(&dsc_cli);
    if(clistat == CLI$_PRESENT) version();

    strcpy(clistr,"NONINTERACTIVE");
    dsc_cli.dsc$w_length = strlen(dsc_cli.dsc$a_pointer);
    clistat = cli$present(&dsc_cli);
    if(clistat == CLI$_PRESENT) noninter_flg = TRUE;
    else noninter_flg = FALSE;

    strcpy(clistr,"NOOWNER");
    dsc_cli.dsc$w_length = strlen(dsc_cli.dsc$a_pointer);
    clistat = cli$present(&dsc_cli);
    if(clistat == CLI$_PRESENT) owner_flg = FALSE;
    else owner_flg = TRUE;

    strcpy(clistr,"IDLEDAYS");
    dsc_cli.dsc$w_length = strlen(dsc_cli.dsc$a_pointer);
    clistat = cli$present(&dsc_cli);
    if(clistat == CLI$_PRESENT) {
        clistat = cli$get_value(&dsc_cli, &dsc_buf, &equ_len);
        if(clistat == SS$_NORMAL) {
            buffer[equ_len] = '\0';
            NumIdleDays = atoi(buffer);
        }
        else exit(1);
    }
    else NumIdleDays = 90;

    strcpy(clistr,"NOSTATISTICS");
    dsc_cli.dsc$w_length = strlen(dsc_cli.dsc$a_pointer);
    clistat = cli$present(&dsc_cli);
    if(clistat == CLI$_PRESENT) stats_flg = FALSE;
    else stats_flg = TRUE;

    strcpy(clistr,"OUTPUT");
    dsc_cli.dsc$w_length = strlen(dsc_cli.dsc$a_pointer);
    clistat = cli$present(&dsc_cli);
    if(clistat == CLI$_PRESENT) {
        clistat = cli$get_value(&dsc_cli, &dsc_buf, &equ_len);
        if(clistat == SS$_NORMAL) {
            buffer[equ_len] = '\0';
            output_fp = fopen(buffer, "w");
        }
    }
    else output_fp = fopen("TT:", "w");

    strcpy(clistr,"USER_FILTER");
    dsc_cli.dsc$w_length = strlen(dsc_cli.dsc$a_pointer);
    clistat = cli$present(&dsc_cli);
    if(clistat == CLI$_PRESENT) {
        clistat = cli$get_value(&dsc_cli, &dsc_buf, &equ_len);
        if(clistat == SS$_NORMAL) {
            buffer[equ_len] = '\0';
            clistat = ParseFilter(&buffer, &ufiltstr);
            if(clistat) ufilt_flg = 1;
            else ufilt_flg = 0;
        }
    }
    else ufilt_flg = 0;

    CheckPrv();         /* if insufficient privs for sysuaf.dat, then exit */
    GetUafList();       /* create current version of sysuaf.lis */
    status = sys$gettim(&ThisDate);
    if(stats_flg) DispStats();
    if ((uaflst_fp = fopen("sys$sysroot:[sysexe]sysuaf.lis", "r")) == NULL) {
        printf("%%RMS-E-FNF, file not found (%s)\n",
            "sys$sysroot:[sysexe]sysuaf.lis");
        exit(1);
    }
    ptr1 = 0;
    while (!ptr1) {
        status = fgets(buffer, sizeof(buffer)-1, uaflst_fp);
        ptr1 = strchr(buffer, '[');
    }
    do {
        ptr2 = &buffer;
        ptr2 += 21;
        status = sscanf(ptr2, "%s", &usr_str);
        ptr2 = strchr(ptr2, ' ');
        if(ptr2) *ptr2  = '\0';
        strcpy(user, usr_str);
        if (ufilt_flg) {
            if ((strncmp(user, ufiltstr, strlen(ufiltstr)) == 0)) {
                GetUserInfo();
            }
        }
        else GetUserInfo();
        status = fgets(buffer, sizeof(buffer)-1, uaflst_fp);
        ptr1 = strchr(buffer, '[');
    } while (ptr1);
    if(stats_flg) fprintf(output_fp, "\n\tTotal of %d user records displayed\n\n",
        NumUsrAcnts);
    fclose(uaflst_fp);
    fclose(output_fp);
    status = remove("sys$sysroot:[sysexe]sysuaf.lis");
    status = remove(comfile);
}
