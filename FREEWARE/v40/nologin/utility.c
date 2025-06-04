
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
 *                %% \___________________________________%% \
 *                %% |                                   %%  \
 *                %% |          utility.c  c1998         %%   \
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
#module utility VERSION

#include <stdio.h>
#include <ctype.h>
#include <descrip.h>
#include <ssdef.h>
#include <jpidef.h>
#include <syidef.h>
#include "version.h"

#define PRV$M_SYSPRV 0x10000000
#define PRV$M_BYPASS 0x20000000

extern char cmdverb[80];
extern char comfile[80];
extern int  NumIdleDays;
extern int  status;
extern int  iosb[2];
extern short noninter_flg;
extern short owner_flg;

extern FILE *uafcom_fp;            /* file pointer for create list file */
extern FILE *output_fp;            /* file pointer to output log file */
extern FILE *uaflst_fp;            /* file pointer to SYSUAF.LIS file */


globalvalue NOLOGIN_CLD;

int UafListCom()
{

    if ((uafcom_fp = fopen(comfile, "w")) == NULL) {
        printf("%%%s-E-OPEN, Cannot open '%s'\n\n", cmdverb, comfile);
        exit(1);
    }
    fprintf(uafcom_fp, "$ set default sys$system:\n");
    fprintf(uafcom_fp, "$ mcr authorize list /brief\n");
    fclose(uafcom_fp);
    return(1);
}

void DispStats()
{
    char    AscTime[32];
    char    NodeName[16];
    short   tim_len;
    $DESCRIPTOR (dsc_time, AscTime);
    struct  {short len, code; int *bufadr, *retlen;} SyiItem[2];

    status = sys$asctim(&tim_len, &dsc_time, 0, 0);
    AscTime[tim_len] = '\0';

    SyiItem[0].len = sizeof(NodeName);
    SyiItem[0].code = SYI$_NODENAME;
    SyiItem[0].bufadr = &NodeName;
    SyiItem[0].retlen = &tim_len;
    SyiItem[1].len = 0;
    SyiItem[1].code = 0;

    status = sys$getsyiw(0, 0, 0, &SyiItem, &iosb, 0, 0);
    NodeName[tim_len] = '\0';
    fprintf(output_fp, "\n\n\tNode %s on %s\n", NodeName, AscTime);
    fprintf(output_fp, "\tInactivity Period: %d days\n\n", NumIdleDays);
    fprintf(output_fp, "\n %-13s", "Username");
    if(owner_flg) fprintf(output_fp, "%-21s", "Owner");
    fprintf(output_fp, "%4c ", 0X49);
    if(noninter_flg) fprintf(output_fp, "%4c ", 0X4e);
    fprintf(output_fp, " Default\n\n");
}

int CheckPrv()
{
    int ProcPriv[2] = 0;
    struct  {short len, code; int *bufadr, *retlen;} JpiItem[2];

    JpiItem[0].len = sizeof(ProcPriv);
    JpiItem[0].code = JPI$_PROCPRIV;
    JpiItem[0].bufadr = &ProcPriv;
    JpiItem[0].retlen = 0;
    JpiItem[1].len = 0;
    JpiItem[1].code = 0;

    status = sys$getjpiw(0, 0, 0, &JpiItem, &iosb, 0, 0);
    if(ProcPriv[0] & PRV$M_SYSPRV || ProcPriv[0] & PRV$M_BYPASS) return;
    lib$stop(36); /* SS$_NOPRIV */
}


version()
{
    printf("\n\n\t\t%s by Lyle W West\n", cmdverb);
#ifdef __DECC
    printf("\t\tVersion: %s (DecC)\n", VERSION);
#else
    printf("\t\tVersion: %s (VaxC)\n", VERSION);
#endif
    printf("\t\tLast Modified: %s\n", __DATE__);
    printf("\t\tRequired Privs: SYSPRV (or BYPASS)\n\n");
    exit(1);
}

sho_hlp()
{
    printf(" \n\n%s Command line arguments:\n\n", cmdverb);
    printf("\n   Parameters\n\n");
    printf("     None\n\n");
    printf("   Qualifiers\n\n");
    printf("    /HELP - if present, %s displays list of qualifiers\n", cmdverb);
    printf("\t\tavailable for %s (this display)\n\n", cmdverb);
    printf("    /IDLEDAYS=number - if present, AND a positive number is \n");
    printf("\t\tentered, the entered number is used for last");
    printf(" login information.\n");
    printf("\t\tA number value is required with /IDLEDAYS qualifier\n");
    printf("\t\tDefault is 90 days.\n\n");
    printf("    /NOOWNER - if present, %s does NOT display ", cmdverb);
    printf("the uaf owner information\n\n");
    printf("    /NONINTERACTIVE - if present %s displays number", cmdverb);
    printf(" of days since\n\t\ta non-interactive login occurred (in");
    printf(" addition to\n\t\tlast interactive login).\n\t\tDefault displays");
    printf(" only interactive logins.\n\n");
    printf("    /NOSTATISTICS - if present, %s does NOT display node,\n",
            cmdverb);
    printf("\t\tdate, heading and summary information. The default is to\n");
    printf("\t\tdisplay this information.\n\n");
    printf("    /OUTPUT=filespec - if present %s output is directed to the",
            cmdverb);
    printf(" selected \n\t\tfile spec, else output is directed to display.\n");
    printf("\t\tA filespec string is required with /OUTPUT qualifier\n\n");
    printf("    /USER_FILTER - if present, %s will parse each username\n",
            cmdverb);
    printf("\t\tfor matches of the entered filter string, per normal VMS\n");
    printf("\t\twild card parsing. The filter string MUST consist of\n");
    printf("\t\talphanumeric characters and '_', '$', '*' characters only.\n");
    printf("\t\tAny other characters will display an error message and\n");
    printf("\t\t%s will exit. The default is not to filter usernames.\n\n",
            cmdverb);
    printf("    /VERSION - if present %s displays version, build", cmdverb);
    printf(" date,\n\t\tand required privs\n\n");
    exit(1);
}

int ParseFilter(char *inpbuf, char *filtbuf)
{
    int done = 0;
    char *ptr3;

    ptr3 = inpbuf;
    while(!done) {
        status = isalnum(*ptr3);
        if (status) ptr3++;
        else {
            switch(*ptr3) {
                case '\0':
                    done = TRUE;
                    break;
                case '$':
                case '_':
                    ptr3++;
                    break;
                case '*':
                    if(ptr3 > inpbuf) {
                        *ptr3 = '\0';
                        done = TRUE;
                        break;
                    }
                default:
                    fprintf(output_fp, "%%%s-E-INVWILD, filter string %s%s\n\n",
                        cmdverb, inpbuf, " contains invalid character");
                    exit(1);
            }
        }
    }
    strcpy(filtbuf, inpbuf);
    return TRUE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *            ****** NOTICE ******
 *
 *    This routine written by Joe Meadows and is used in his
 *    VERB, FILE and FIND programs. Only minor modifications
 *    have been made for use in this program - lww
 *
 *    status = init_cli(table_addr, verb_name)
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

int init_cli(char *table, char *name)
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
    printf("Unexpected error in INIT_CLI, error %d in retrieving $LINE\n");
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
    printf("Unexpected error in INIT_CLI, ");
    printf("verb name wasn't smaller than $line prefix\n");
#else
    printf("\n%%NOLOGIN-E-NEEDFORSYMB, must be invoked via foreign symbol\n\n");
/*  exit(1);  */
#endif 
  return(cli$dcl_parse(&cmd,table)); /* initialize the parse tables */
}

int lib$sig_to_ret();

int cli_get_value(struct dsc$descriptor *s1, struct dsc$descriptor *s2)
{
  lib$establish(lib$sig_to_ret);
  return cli$get_value(s1, s2);
}


