#include "common.h"

#define DELTREE_SIGNAL_ROUTINE_HEADER 1
#include "deltree_msg.h"

#include <prcdef.h>
#include <prvdef.h>
#include <jpidef.h>

#ifdef __GNUC__
#define __PRV(a,b,c)  a.b.c
#else
#define __PRV(a,b,c)  a.c
#endif


#define	IMAGE			"SYS$SYSTEM:LOGINOUT.EXE"

extern struct returned_info 	info;

extern int			topdir;
extern int			prio;
extern int			dodebug;
extern int			dolog;
extern char 			*privs;
extern char			*notif;


void detach_command(char *afilin,char *prgm_nam)
{
  char		*scrdev;
  char		*flnm,*errnm,*outnm,*sflnm,*prcsnm;
  FILE		*fo;

  unsigned long pid=0;
  unsigned int	status;
  int		i,j,len;

  int		jpi_authpri  = JPI$_AUTHPRI;
  int		authpri;

  void		my_fprintf(FILE *fo,char *frmt,int Num, ...);
  char		*hostname();
  union prvdef	prvs;

  unsigned long  int	curtime[2];
  unsigned short int	timlen;
  char    		curtime_str[40]="\0";

  $DESCRIPTOR(T_image, IMAGE);

  DDESCR(T_name);
  DDESCR(T_input);
  DDESCR(T_output);
  DDESCR(T_error);
  DDESCR(curtime_str_d);


  scrdev  = getenv("SYS$SCRATCH");
  ALLOCA(sflnm,scrdev,15+3+1);
  ALLOCA(flnm ,scrdev,15+3+1);
  ALLOCA(errnm,scrdev,15+3+1);
  ALLOCA(outnm,scrdev,15+3+1);

  j	= (int) getpid();

  AGAIN:
  i	= 0;
  do{
    sprintf(sflnm,"%sDtree%.8X%d",scrdev,j,i);
    strcpy(flnm,sflnm);
    strcat(flnm,".COM");
    i++;
  } while( (fo = fopen(flnm,"r")) != NULL && i < MAXDT);
  fclose(fo);
  if( i == MAXDT){
    fprintf(stderr,"\n DELTREE :: Maximum allowed detached DELTREE (%d) exceeded.",MAXDT);
    fprintf(stderr," CTRL/C and\n  $ delete %sDtree%.8X*.COM;*\n or wait",scrdev,j);
    fprintf(stderr," 10 scdes (will try again).\n\n");
    (void) sleep(10);
    goto AGAIN;
  }
  free(scrdev);

  /* Fix file names. Sflnm will be used for job name 			*/
  strcpy(errnm,sflnm);
  strcpy(outnm,sflnm);
  strcat(errnm,".ERR");
  strcat(outnm,".LOG");
  prcsnm = strrchr(sflnm,']');
  prcsnm++;

  if( (fo = fopen(flnm,"w")) != NULL){
    /* Start with some information					*/
    curtime_str_d.dsc$a_pointer = (void *) curtime_str;
    curtime_str_d.dsc$w_length  = (int) sizeof(curtime_str)/sizeof(char);

    sys$gettim(curtime);
    sys$asctim(&timlen, &curtime_str_d, curtime, 0);
    curtime_str[timlen] = '\0';


    my_fprintf(fo,"%s\n%s%s\n%s\n%s\n%s\n%s%s\"\n%s%s\"\n%s\n%s\n",12,
      "$ !+",
      "$ ! DELTREE Command File created on ",curtime_str,
      "$ !-",
      "$ WRITE SYS$OUTPUT \"\"",
      "$ WRITE SYS$OUTPUT \"\t-----------------------------------\"",
      "$ WRITE SYS$OUTPUT \"\tDELTREE Version ",VERSION,
      "$ WRITE SYS$OUTPUT \"\t",COPYR,
      "$ WRITE SYS$OUTPUT \"\t-----------------------------------\"",
      "$ ver = f$verify(0)");

    my_fprintf(fo,"%s%s\n%s\n%s\n%s\n",5,
      "$ SET DEFAULT ",(char *) getcwd('\0',BUFSIZ),
      "$ WRITE SYS$OUTPUT \"\"",
      "$ WRITE SYS$OUTPUT \"Current directory is \",f$environment(\"DEFAULT\")",
      "$ WRITE SYS$OUTPUT \"\"");

    my_fprintf(fo,"$ DELTREE :== $%s\n",1,prgm_nam);
    my_fprintf(fo,"$ ON ERROR THEN GOTO END\n",0);


    /* Messy preceding method can be simplified				*/
    if( privs != NULL)
      my_fprintf(fo,"$ SET PROCESS/PRIVILEGE=(%s)\n",1,privs);

    /* Beware of /CONFIRM presence. Override it	in all cases		*/
    /* + supports only some ketwords in DETACH mode.			*/
    my_fprintf(fo,"$ DELTREE",0);
    if(dolog)	my_fprintf(fo,"/LOG",0);
    if(dodebug)	my_fprintf(fo,"/DEBUG",0);
    if(!topdir)	my_fprintf(fo,"/NOTOP",0);

    my_fprintf(fo,"/NOCONFIRM %s\n%s\n%s\n%s\n",4,afilin,
      "$ WRITE SYS$OUTPUT \"\"",
      "$ END:",
      "$ SSTATUS = $STATUS");
      
    if(dodebug){
      my_fprintf(fo,"%s %s;*\n",2,
	"$ ! DELETE/NOLOG/NOCONFIRM",flnm);
    } else {
      my_fprintf(fo,"%s %s;*\n",2,
	"$ DELETE/NOLOG/NOCONFIRM",flnm);
    }  


    /* Treat the new NOTIFY qualifier					*/
    if(notif != NULL){
      /* DCL trap of the error message done only with in NOTIFY mode	*/
      my_fprintf(fo,"%s\n%s\n%s\n%s\n%s\n%s\n%s\n",7,
	"$ SEVER   = F$MESSAGE(SSTATUS,\"SEVERITY\")",
	"$ IF SEVER.nes.\"%%S\"",
	"$ THEN",
	"$  ERROR = \"Error :: \"+F$MESSAGE(SSTATUS)",
	"$ ELSE",
	"$  ERROR = \"done\"",
	"$ ENDIF");

      if( strcmp(notif,"MAIL")==0){
	my_fprintf(fo,
	  "$ MAIL/SUBJECT=\"$ DELTREE %s  ''ERROR' (%s)\" NL0: %s\n",2,
	  afilin,hostname(),cuserid(0));
      } else {
	DDESCR(symb_name);
	DDESCR(symb_val);
	IDESCR(symb_name,"SEND");

	status = lib$get_symbol(&symb_name, &symb_val);

	if( OK(status) ){
	  char *q;

	  /* I know, looks superfluous but GCC is peaky			*/
	  q = (char *) (symb_val.dsc$a_pointer+symb_val.dsc$w_length);
	  *q= '\0';
	  
	  my_fprintf(fo,"$ SEND :== %s ! Got from Shell Symbol\n",1,symb_val.dsc$a_pointer);
	} else if( getenv("MULTINET") != NULL){
	  /* Automatic support for Multinet TCPIP			*/
	  my_fprintf(fo,"$ SEND :== MULTINET SEND ! Multinet Support\n",0);
	}
	my_fprintf(fo,
	  "$ SEND %s \"$ DELTREE %s  ''ERROR'\"\n",2,
	  cuserid(0),afilin);
      }
    }
    my_fprintf(fo,"$EXIT\n",0);
    fclose(fo);



    /* Small priority check						*/
    status = lib$getjpi((void *)&jpi_authpri, 0, 0, (void *) &authpri, 0, 0);
    if( OK(status) ){
      if( authpri < prio){
	prio = authpri;
	hack$signal(DELTREE__NOTSET_msg_hack,1,authpri);
      }
    } else {
      hack$signal(DELTREE__CHECKFAIL_msg_hack,1,"authorized priority");
    }

    /* OK. I'll set TMPMBX and NETMBX all the time			*/
    memset( &prvs, 0, sizeof( prvs ) );
    __PRV(prvs,prv$r_prvdef_bits0,prv$v_tmpmbx) = 1;
    __PRV(prvs,prv$r_prvdef_bits0,prv$v_netmbx) = 1;


    IDESCR( T_name  , prcsnm);
    IDESCR( T_input , flnm);
    IDESCR( T_output, outnm);
    IDESCR( T_error , errnm);

    if( dodebug){
      fprintf(stderr,"deltree :: Debug. Process created as follows\n");
      fprintf(stderr," Pid      |%d|\n",pid);
      fprintf(stderr," Image    |%s|\n",T_image.dsc$a_pointer);
      fprintf(stderr," Input    |%s|\n",T_input.dsc$a_pointer);
      fprintf(stderr," Output   |%s|\n",T_output.dsc$a_pointer);
      fprintf(stderr," Error    |%s|\n",T_error.dsc$a_pointer);
      fprintf(stderr," Name     |%s|\n",T_name.dsc$a_pointer);
      fprintf(stderr," Priority |%d|\n",prio);
    }


    status = sys$creprc( &pid, &T_image, &T_input, &T_output, &T_error,
      &prvs, 0, &T_name, &prio, 0,  0, PRC$M_DETACH);

    if( !OK(status) ){
      if(dodebug){
	hack$exit(DELTREE__CREATFAIL_msg_hack);
      } else {
	(void) delete(flnm);
	exit(status);
      }
    } else {
      strcpy(info.prcsnam,prcsnm);
      info.pid = pid;
      info.prio= prio;
      free(errnm);	free(outnm);
      free(flnm);	free(sflnm);
      return;
    }
  } else {
    hack$exit(DELTREE__IOERROR_msg_hack);
  }
  return;
}

char *hostname(void)
{
  char *ptr;

  /* YES, I like the brakets ...		*/
  if( (ptr = getenv("MULTINET_HOST_NAME")) == (char *) NULL){
    if( (ptr = getenv("UCX$INET_HOST")) == (char *) NULL){
      if( (ptr = getenv("INTERNET_HOST_NAME")) == (char *) NULL){
	if( (ptr = getenv("TCPWARE_DOMAINNAME")) == (char *) NULL){
	  if( (ptr = getenv("ARPANET_HOST_NAME")) == (char *) NULL){
	    if( (ptr = getenv("ARPANET_HOST_NAME")) == (char *) NULL){
	      ptr = getenv("SYS$NODE");
	    }
	  }
	}
      }
    }
  }
  return ptr;  
}


#include <stdarg.h>
void my_fprintf(FILE *fo,char *frmt,int Num, ...)
{
  char buf[XDIM];
  va_list args;

  va_start(args,Num);
  (void) vsprintf(buf,frmt,args);
  if( fprintf(fo,buf) == -1){
    hack$exit(DELTREE__IOERROR_msg_hack);
  }
}


