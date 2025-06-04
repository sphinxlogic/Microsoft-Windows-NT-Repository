/* CC/prefix_library_entries=ALL_ENTRIES - 
               RWMBX+ALPHA$LIBRARY:SYS$LIB_C/LIBRARY */ 
/* LINK/SYSEXE RWMBX */ 
/* $RWMBX:==$RQS$EXE:RWMBX */ 
 
 
#include <ssdef.h> 
#include <stdio.h> 
#include <DDBDEF.H> 
#include <LNMSTRDEF> 
#include <MBXDEF.H> 
#include <UCBDEF.H> 
#include <STDLIB.H> 
#include <signal.h> 
#include <descrip.h> 
#include <starlet.h> 
 
 
FILE *fout; char c; 
 
globalvalue DDB **IOC$GL_DEVLIST; 
DDB *ddb_ptr = NULL;		/* Device Data Bloc ptr  */ 
MB_UCB *ucb_ptr = NULL;		/* Mail Box _ Unit Control Bloc ptr */ 
LNMB *lnmb_ptr = NULL;          /* Logical NaMe Bloc ptr */ 
int search = TRUE; 
int dummy_arg_list[2] = {0,0};	/* dummy arglist to cmkrnl */ 
int status = 0; 
int mbx_num = 0; 
int interval = 1; 
int onlyzero = 0; 
int nozero = 0; 
 
struct { 
        char logname[129];			/* logical name */ 
        unsigned short int mbx$w_unit;		/* device unit number */ 
        struct _ucb *mbx$l_ucb;			/* addr of current ucb */ 
	struct _ucb *mbx$l_link;                /* addr of next ucb */ 
        struct _lnmb *mbx$l_logadr;		/* Logical Name Bloc ptr */ 
	unsigned int mbx$l_readerrefc;		/* read channels */ 
        unsigned int mbx$l_writerrefc;		/* write channels*/ 
        unsigned int mbx$l_refc;		/* process reference count */ 
        unsigned short int mbx$w_devbufsiz;	/* max msg size */ 
        unsigned short int mbx$w_iniquo;	/* initial mailbox buffer size */ 
        unsigned short int mbx$w_bufquo;	/* remaining mailbox buffer size */ 
        unsigned short int mbx$w_msgcnt;	/* no# write requests qued */ 
        unsigned int mbx$l_mb_readqfl;		/* absolute que of read IRPs */ 
        unsigned int mbx$l_mb_readqbl;	 
        unsigned int mbx$l_opcnt;		/* no# completed operations */ 
       } mbxdat = {0}; 
 
#pragma noinline( get_mbx_info) 
 
int get_mbx_info() 
  { 
  unsigned int namelen; 
  char *src_name_ptr, *dst_name_ptr; 
 
  mbxdat.mbx$l_ucb = mbxdat.mbx$l_link;			/* make next ucb the current ucb */ 
  ucb_ptr = (MB_UCB*) mbxdat.mbx$l_ucb; 
  mbxdat.mbx$l_link = ucb_ptr->ucb$r_ucb.ucb$l_link;	/* update next ucb ptr */ 
 
  mbxdat.logname[0] = '\0'; 
  lnmb_ptr = ucb_ptr->ucb$l_logadr; 
  if (lnmb_ptr) 
    { 
    src_name_ptr = &lnmb_ptr->lnmb$t_name;    dst_name_ptr = mbxdat.logname; 
    for (namelen=0; namelen<lnmb_ptr->lnmb$l_namelen; namelen++) 
      *dst_name_ptr++ = *src_name_ptr++; 
    *dst_name_ptr = '\0'; 
    } 
  mbxdat.mbx$w_unit = ucb_ptr->ucb$r_ucb.ucb$w_unit; 
  mbxdat.mbx$l_readerrefc = ucb_ptr->ucb$l_mb_readerrefc; 
  mbxdat.mbx$l_writerrefc = ucb_ptr->ucb$l_mb_writerrefc; 
  mbxdat.mbx$l_refc = ucb_ptr->ucb$r_ucb.ucb$l_refc; 
  mbxdat.mbx$w_devbufsiz = ucb_ptr->ucb$r_ucb.ucb$w_devbufsiz;     /* max msg size */ 
  mbxdat.mbx$w_iniquo = ucb_ptr->ucb$r_ucb.ucb$w_iniquo; 
  mbxdat.mbx$w_bufquo = ucb_ptr->ucb$r_ucb.ucb$w_bufquo; 
  mbxdat.mbx$w_msgcnt = ucb_ptr->ucb$r_ucb.ucb$w_msgcnt; 
  mbxdat.mbx$l_opcnt  = ucb_ptr->ucb$r_ucb.ucb$l_opcnt; 
  return(1); 
} 
 
 
shombx() 
  { 
  char timeStr[24]; 
  $DESCRIPTOR( timeDesc, timeStr); 
  unsigned stat; 
 
 
  /* check for zero (-z) or nozero (-n) filters */ 
  if (onlyzero && mbxdat.mbx$w_msgcnt) return;		/* only display mbx if NO writes qued */ 
  if (nozero && !mbxdat.mbx$w_msgcnt) return;           /* only display mbx if writes qued */ 
 
 
  fprintf( fout, "************************************************\n"); 
  if (mbx_num) { 
      stat = sys$asctim(0, &timeDesc, 0,0); 
      timeStr[23] = 0; 
      fprintf( fout, "\t\t\t%s\n", timeStr); 
      } 
 
  if (mbxdat.logname[0]) 
    fprintf( fout, "%s", mbxdat.logname); 
  fprintf( fout, "\t\tMBA%d\n", mbxdat.mbx$w_unit); 
  fprintf( fout, "            No# Processes: %d\n", mbxdat.mbx$l_refc); 
  fprintf( fout, "      Read/Write Channels: %d/%d\n", mbxdat.mbx$l_readerrefc, mbxdat.mbx$l_writerrefc); 
  fprintf( fout, "             Max Msg Size: %d\n", mbxdat.mbx$w_devbufsiz); 
  fprintf( fout, "      Initial Buffer Size: %d\n", mbxdat.mbx$w_iniquo); 
  fprintf( fout, "    Remaining Buffer Size: %d\n", mbxdat.mbx$w_bufquo); 
  fprintf( fout, "          No# Writes Qued: %d\n", mbxdat.mbx$w_msgcnt); 
  fprintf( fout, "     Completed Operations: %d\n", mbxdat.mbx$l_opcnt); 
  } 
 
 
main( int argc, char *argv[] ) 
{ 
  fout = stdout; 
  while (--argc >0) { 
    c = **++argv; 
    switch (c) { 
      case '>': 
        if ((fout = fopen( ++*argv, "w" )) == NULL) { 
          printf( "Unable to open output file %s for writing.\n", *argv); 
          exit(1); 
        } 
        break; 
 
      case '<': 
        mbx_num = atoi( ++*argv); 
        break; 
 
      case '-': 
        ++*argv;	/* get off '-' */ 
        if (**argv == 's') { 
          interval = atoi( ++*argv); 
	  printf( "sleep interval is %d seconds.\n", interval); 
          } 
        else 
	  if (**argv == 'z') { onlyzero =1;} 
	  else 
	    if (**argv == 'n') { nozero =1;} 
            else { 
              printf( "Offensive command line argument: '%s'\n", *argv); 
              sys$exit(1); 
              } 
        break; 
 
      default: 
        printf( "Offensive command line argument: '%s'\n", *argv); 
        argc = 0; 
        sys$exit(1); 
        break; 
    } 
  } 
 
 
  ddb_ptr = *IOC$GL_DEVLIST; 
 
  do	/* find device data block for mailboxes */ 
    { 
    if ((ddb_ptr->ddb$b_name_len == 3) && (ddb_ptr->ddb$t_name_str[0] == 'M') && 
        (ddb_ptr->ddb$t_name_str[1] == 'B') && (ddb_ptr->ddb$t_name_str[2] == 'A'))  
       search = FALSE; 
    else 
       { 
       ddb_ptr = (DDB *) ddb_ptr->ddb$l_link; 
       } 
    }  while (search); 
 
  mbxdat.mbx$l_link = (UCB *) ddb_ptr->ddb$l_ucb;	/* setup, first mbx ucb to forward ptr */ 
  do 
    { 
    status = SYS$CMKRNL( get_mbx_info, &dummy_arg_list); 
    if (status != SS$_NORMAL) 
      SYS$EXIT( status); 
 
    if (lnmb_ptr) 
      { 
      if ((mbx_num==0) || (mbx_num==mbxdat.mbx$w_unit))  shombx(); 
      } 
 
    if ((mbxdat.mbx$l_link==NULL) && (mbx_num)) { 
      mbxdat.mbx$l_link = (UCB *) ddb_ptr->ddb$l_ucb; 
        if (interval) 
          sleep( interval); 
      } 
    } while(mbxdat.mbx$l_link);			/* until no more UCBs */ 
  SYS$EXIT( status); 
} 
