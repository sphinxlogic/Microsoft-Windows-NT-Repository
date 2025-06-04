/* Program Name            : DELTREE2.C                                 */
/*   Original Author       : JLAURET                                    */
/*   Date                  : 1994					*/
/*   Program Description   : Started from a skeleton idea I got		*/
/*                         : from another package, I developped DELTREE	*/
/*                         : a directory tree deletion program ...	*/
/* Revision History at bottom						*/

/* Some DCC pragmas							*/
/*~DollarSign*/


#include "common.h"
#include "deltree_msg.h"


/* Default global variables						*/
#if !defined(__GNUC__)
# pragma nostandard
GLOBALDEF(int,	doconf	,TRUE);
GLOBALDEF(int,	alldir	,FALSE);
GLOBALDEF(int,	dolog	,FALSE);
GLOBALDEF(int,	depth	,0);
GLOBALDEF(int,	rdepth	,0);
GLOBALDEF(int,	count	,0);
GLOBALDEF(int,	total	,0);
GLOBALDEF(int,	dodebug	,FALSE);
# pragma standard
#else
int		doconf	=TRUE;
int		alldir	=FALSE;
int		dolog	=FALSE;
int		depth	=0;
int		rdepth	=0;
int		count	=0;
int		total	=0;
int		dodebug	=FALSE;
#endif

char    	*original;
char		*prgm_nam;
char		*afilin;
char		*save_top;
char		*save_dir;
int		level  = 0;
int		topdir = TRUE;
int		prio   = 4;


/* Function prototype 							*/
static char		*strip(char *);
static int		action(char *);
static int		dochdir(int,char *);
static int		flush_file(char *);
static int		rm_tree(char *);
static unsigned int 	CLI_GET_VALUE(char *,char *);
static unsigned int 	CLI_PRESENT(char *);
static void		cpu_time(int);
static void		exit_lhelp(int);
static unsigned int	get_args(void);
extern void		detach_command(char *,char *);


static clock_t 		init_time;
struct returned_info	info={'\0',0,0};
char 			*privs;
char 			*notif;




int main(int argc, char *argv[])
{
  int	status,gstatus=SS$_NORMAL;
  int	i;
  char	*ptr;
  char	*top;
  char	*dpos;
  char	*safilin;
  char	*sfilin,*filin,*sdev,*dev;

  hack$establish(lib$sig_to_ret);
  status = (int) get_args();
  hack$revert();

  prgm_nam  = argv[0];
  ptr	    = strrchr(prgm_nam,';');
  *ptr	    = '\0';


  /* Let's check those qualifiers/return values				*/
  if( status ==  DELTREE__VERSION ){
    hack$signal(DELTREE__VERSION_msg_hack,2,VERSION,COPYR);
    exit(SS$_NORMAL);
  }

  if( status == CLI$_PRESENT){
    detach_command(afilin,prgm_nam);
    if( info.pid != 0){
      hack$signal(DELTREE__DPRCS_msg_hack,3,info.prcsnam,info.pid,info.prio);
      exit(SS$_NORMAL);
    }
  }

  if(alldir && ! doconf){
    hack$signal(DELTREE__IGNORE_msg_hack,2,"ALL","CONFIRM");
  }



  /* Init timer								*/
  save_dir = NULL;
  cpu_time(0);

  /*  save the current default    					*/
  original = (char *) getcwd('\0',BUFSIZ);
  if(dodebug)hack$signal(DELTREE__DEBUG_msg_hack,2,"root is",original);


  /* Loop over all dir specified on command line			*/
  safilin= afilin;
  dpos	 = strchr(afilin,',');
  do{
    if(dpos) *dpos++  = '\0';

    /* Initialize next pass 						*/
    /* Directory expected without "."					*/
    status	= SS$_NORMAL;
    depth	= 0;
    rdepth	= 0;

    sfilin = filin = (char *) malloc(sizeof(char *)*(strlen(afilin)+1+0));
    sdev   = dev   = (char *) malloc(sizeof(char *)*(strlen(afilin)+1+6));
    if( filin == NULL || dev == NULL){
      chdir(original);
      hack$exit(DELTREE__INSFMEM_msg_hack);
    }
    (void *) strcpy(dev,afilin);
    (void *) strcpy(filin,afilin);

    dev		= strrchr(dev,']');
    filin	= strrchr(filin,']');
    if( dev ){
      /* We had a device specified 					*/
      dev++; 	*dev    = '\0';
      dev	= sdev;
      filin++;
    } else {
      filin	= sfilin;
      (void *) strcpy(sdev,"[]");
      dev	= sdev;
    }


    if( *filin == '\0' ){
      if(dodebug){
	hack$signal(DELTREE__DEBUG_msg_hack,2,
	  "no directory-file specified. Extract from",
	  sfilin);
      }

      filin	= strrchr(sfilin,'.');
      if( filin == NULL){
	/* Dammit !! One of those !! 					*/
	filin	= strrchr(sfilin,'[');
	if( filin == NULL){
	  chdir(original);
	  hack$exit(DELTREE__MISSING_msg_hack);
	}
	filin++;
	(void *) strcpy(sfilin,filin);

	filin	= strrchr(sfilin,']');
	if( filin == NULL){
	  chdir(original);
	  hack$exit(DELTREE__MISSING_msg_hack);
	}
	*filin	= '\0';
	filin	= sfilin;

	dev	= strrchr(dev,'[');
	if( dev == NULL){
	  chdir(original);
	  hack$exit(DELTREE__MISSING_msg_hack);
	}
	*dev = '\0'; dev = sdev;
	(void *) strcat(dev,"[000000]");
      } else {
	/* There is a do somewhere in the path		       		*/
	filin++;  (void *) strcpy(sfilin,filin);
	filin	= strrchr(sfilin,']');
	if( filin == NULL){
	  chdir(original);
	  hack$exit(DELTREE__MISSING_msg_hack);
	}
	*filin	= '\0';
	filin	= sfilin;

	dev	= strrchr(sdev,'.');
	if( dev == NULL){
	  chdir(original);
	  hack$exit(DELTREE__MISSING_msg_hack);
	}
	*dev++	= ']';
	*dev	= '\0';
	dev	= sdev;
      }
    }

#ifdef DEBUG
    if(dodebug){
      char blop1[DIM];
      char blop2[DIM];

      (void *) sprintf(blop1,"final dev=%s ",dev);
      (void *) sprintf(blop2,"dir=%s",filin);
      hack$signal(DELTREE__DEBUG_msg_hack,2,blop1,blop2);
    }
#endif

    /* Path sanity check						*/
    if(     strchr(filin,']') != NULL ||
	    strchr(filin,'[') != NULL ||
	    strchr(filin,':') != NULL ){
      chdir(original);
      hack$exit(DELTREE__MISSING_msg_hack);
    }

    if(	*filin == '-'){
      /* This directory does not have a legitimate name or old-form	*/
      /* with Unix syntax is being used  ...				*/
      chdir(original);
      hack$signal(DELTREE__UGLIX_msg_hack,0);
    }


    if ( chdir(dev) == 0){
      char		*local_dir;

      /* Preceding version of DELTREE allowed path simplication		*/
      top	= strchr(filin,'.');
      if(top)	*top = '\0';

      ALLOCA(local_dir,filin,1+2);
      /* Fix logicals desastrous effects 				*/
      (void *) sprintf(local_dir,"./%s",filin);
      if(dodebug){
	hack$signal(DELTREE__DEBUG_msg_hack,2,"To remove",local_dir);
      }


      /*  if a valid root             					*/
      if (chdir(local_dir) == 0) {

	/*  save the root spec          				*/
	if(dodebug){
	  hack$signal(DELTREE__DEBUG_msg_hack,2,"successful chdir() in",local_dir);
	}

	/* top changes definitions, now the fully specified path	*/
	top = (char *) getcwd('\0',BUFSIZ+5,0);
	ALLOCA(save_top,top,1);
	(void *) strcpy(save_top,top);

#ifdef DEBUG
	if(dodebug){
	  hack$signal(DELTREE__DEBUG_msg_hack,2,"working in",save_top);
	}
#endif

	/* Calculate initial depth					*/
	for(ptr = save_top ; ptr ;){
	  if((ptr = strchr(ptr,'/')) != NULL){
	    depth++,
	    ptr++;
	  }
	}

	/* Fix top with .dir   						*/
	strncat(top,".dir.",BUFSIZ);

	/* Go to root-device						*/
	/*  and let the fun begin       				*/
	chdir("[-]");
	if(dodebug)hack$signal(DELTREE__BEGIN_msg_hack,1,top);


	status = SS$_NORMAL;
	if(status = rm_tree(save_top)){
	  /*  finally, remove the root    				*/
	  if(topdir){
	    status = flush_file(top);
	  } else {
	    hack$signal(DELTREE__PRSRVD_msg_hack,1,save_top);
	  }
	}
	if(gstatus == SS$_NORMAL || gstatus == DELTREE__NOTEXISTS){
	  gstatus = status;
	}
	if(status == DELTREE__PRV) gstatus = status;
	if(count != 0)status = SS$_NORMAL;

	if(dolog){
	  hack$signal(DELTREE__DELETED_msg_hack,1,top);
	  cpu_time(1);
	}
	free(top);
	free(save_top);

      } else {
	hack$signal(DELTREE__NOTFND_msg_hack,2,dev,local_dir);
	if(gstatus == SS$_NORMAL) gstatus = DELTREE__NOTEXISTS;
      }

      free(local_dir);


    } else {
      hack$signal(DELTREE__CHDFAIL_msg_hack,1,dev);
      if(gstatus == SS$_NORMAL) gstatus = DELTREE__NOTATREE;
    }


    /* Free variables. Get ready for next directory in the list if any	*/
    free(sfilin);
    free(sdev);

    if( chdir(original)	!= 0){
      /* The root directory has disappeared				*/
      hack$exit(DELTREE__NOROOT_msg_hack);
    }
    if(rdepth != 0){
      /* Our counter is wrong ...					*/
      if(dodebug) printf("Bad counter %d\n",rdepth);
      hack$exit(DELTREE__BADCOUNT_msg_hack);
    }

    afilin = dpos;
    if(afilin) dpos = strchr(afilin,',');
  } while(afilin);


  /*  go back home                					*/
  chdir(original);
  free(original);
  free(safilin);

  if(dodebug) hack$signal(DELTREE__DEBUG_msg_hack,2,"leaving deltree","");
  return gstatus;
}






/* -------------------------------------------------------------------- */
/*  remove tree - call shell$from_vms to remove files  			*/
/* -------------------------------------------------------------------- */
int rm_tree(char *s)
{

  if(doconf && (rdepth==0 || alldir)){
    char	ans;
    char	*stripped;

#ifdef DEBUG
    if(dodebug){
      hack$signal(DELTREE__DEBUG_msg_hack,2,"RmTree",s);
      printf("\n%s\n",save_dir);
    }
#endif
#if !defined(__GNUC__)
    /* This is a bug related to mix usage $signal() fprintf()		*/
    /* printf() message overlaps. I don't konw how to fix it yet ...	*/
    fprintf(stdout,"\n");
#endif
    stripped = strip(s);
    fprintf(stdout,"DELTREE %s%s ? ",save_dir==NULL?"/":save_dir,stripped);

    fscanf(stdin,"%s",&ans);
    ans = tolower(ans);
    if (ans != 'y'){
      count++;
      return FALSE;
    }
  }

  /*  move to target dir      						*/
  if( dochdir(1,s) == 0){

    /* blow everything (?) away.					*/
    /* Actually the .*;* remains so we have to treat them separatly	*/
    if(dodebug)hack$signal(DELTREE__DEBUG_msg_hack,2,"flush in",s);

    shell$from_vms(".*;*",action,1);
    shell$from_vms("*.*;*",action,1);

    if(dodebug){
      hack$signal(DELTREE__DEBUG_msg_hack,2,"endFlush in",s);
    }

    /*  move back one							*/
    dochdir(-1,"[-]");
    return TRUE;
  } else {
    return FALSE;
  }
}


/* -------------------------------------------------------------------- */
/*  this routine is reponsible for file deletion			*/
/* -------------------------------------------------------------------- */
int action(char *name)
{
  register int	sts;
  char		*ps;


  /*  is this a directory?  						*/
  if ((ps=strstr(name,".DIR.")) != '\0' || (ps=strstr(name,".dir.")) != '\0'){
    *ps = '\0';
    sts = rm_tree(name);

    *ps = '.';
    if(sts==1) sts = flush_file(name);
  } else {
    /*  it's a file, so							*/
    sts = flush_file(name);
  }
  return 1;
}


int	flush_file(char *name)
{
  register int	stat;
  register int	sts;

  stat	= SS$_NORMAL;

  sts 	= chmod(name,0777);
  if(sts != 0){
    if(dolog)hack$signal(DELTREE__NOPROT_msg_hack,1,name);
    stat = DELTREE__PRV;
  }

#ifdef DEBUG
  if(dodebug)hack$signal(DELTREE__DEBUG_msg_hack,2,"deleting",name);
#endif
  sts	= delete(name);
  if(sts != 0 ){
    if(dolog)hack$signal(DELTREE__NODELE_msg_hack,2,name);
    if(stat == SS$_NORMAL)stat = DELTREE__NOTATREE;
  } else {
    /* If the file belong to another user, stat =			*/
    /* DELTREE__PRV will be set. But we may still be able		*/
    /* to delete the file if the protection or ACL are			*/
    /* set properly. i.e. we need to reset status ...			*/
    stat	= SS$_NORMAL;
    total++;
  }
  return stat;
}




/* --------------------------------------------------------------------	*/
/* Other routines							*/
/* --------------------------------------------------------------------	*/

int dochdir(int i,char *st)
{
  rdepth += i;
  depth  += i;

  /* 10 = 8 + 2 ; 2 comes from "/" at beginning of disk name, "/" between
    disk name and first directory from 000000				*/

#ifdef DEBUG
  if(dodebug){
    char buffer[DIM];

    (void *) sprintf(buffer,"chdir() if %d%%%d && %d (%d)",depth,MAXDEPTH,i,doconf);
    hack$signal(DELTREE__DEBUG_msg_hack,2,buffer,"");
  }
#endif

  if( (depth % MAXDEPTH) == 0 && i > 0 ){
    /* Solve it with a recursive loop					*/

#ifdef DEBUG
    if(dodebug){
      char buffer[DIM];

      (void *) sprintf(buffer,"level=%d depth encountered. %d (%d)\n",level,dodebug,doconf);
      hack$signal(DELTREE__DEBUG_msg_hack,2,buffer,"");
    }
#endif

    if(level == 0){
      $DESCRIPTOR(logical,DEEPDR);
      $DESCRIPTOR(parent,"LNM$PROCESS_TABLE");
      $DESCRIPTOR(value,"");

      char		direct[XDIM];
      unsigned long 	trans;
      unsigned int  	status;
      char	*expectedcd;
      char	*ptr,*disk;
      char	*tmp1,*tmp2;
      char	*local_dir;
      char	*env;
      int	len;

      direct[0] = '\0';

      if(dolog || dodebug)hack$signal(DELTREE__DEEPDIR_msg_hack,1,st);
#ifdef DEBUG
      if(dodebug)hack$signal(DELTREE__DEBUG_msg_hack,2,"Directory String",st);
#endif

      /* Copy directory, skip first slash, change to dots		*/
      ALLOCA(ptr,st,0);
      (void *) strcpy(ptr,st+1);

      for(disk = ptr ; (disk = strchr(disk,'/')) != NULL ; disk++){
	*disk = '.';
      }


      /* Get disk and directory name					*/
      ALLOCA(disk,ptr,1);
      (void *) strcpy(disk,ptr);
#ifdef DEBUG
      if(dodebug)hack$signal(DELTREE__DEBUG_msg_hack,2,"Copying",disk);
#endif
      tmp1	= strchr(disk,'.');
      tmp2	= strrchr(disk,'.');
      if(tmp1 == NULL || tmp2 == NULL){
	chdir(original);
	hack$exit(DELTREE__BUGCHK_msg_hack);
      }
      *tmp1	= '\0';
      *tmp2	= '\0';
      tmp2++;

      if( (env = getenv(disk)) == NULL_C){
	/* was not a logical						*/
	if(dodebug)hack$signal(DELTREE__GETENV_msg_hack,1,"(null)");
	len	= strlen(disk);
	(void *) strcpy(direct,disk);
	direct[len++] = ':';
	direct[len++] = '[';
	direct[len]   = '\0';
	tmp1++;
	strncat(direct,tmp1,BUFSIZ);
#ifdef DEBUG
	if(dodebug)hack$signal(DELTREE__DEBUG_msg_hack,2,"Now have",direct);
#endif
      } else {
	/* Was a logical						*/
	(void *) strcpy(direct,env);
	if(dodebug)hack$signal(DELTREE__GETENV_msg_hack,1,direct);
	direct[strlen(direct)-1] = '\0';

	/* Strip out an eventual .000000. if any			*/
	if( *(tmp1+1)	== '0' &&
	    *(tmp1+2)	== '0' &&
	    *(tmp1+3)	== '0' &&
	    *(tmp1+4)	== '0' &&
	    *(tmp1+5)	== '0' &&
	    *(tmp1+6)	== '0' ){
	    tmp1	= tmp1+7;
	}
	/* Skip first dot						*/
	tmp1++;
	(void *) strncat(direct,tmp1,BUFSIZ);
      }
      /* strcat() did not work here					*/
      len		= strlen(direct);
      direct[len++]	= '.';
      direct[len++]	= ']';
      direct[len]	= '\0';


#ifdef DEBUG
      if(dodebug){
	char buffer[DIM];

	(void *) sprintf(buffer,"%d",len);

	hack$signal(DELTREE__DEBUG_msg_hack,2,"Ptr  is",ptr);
        hack$signal(DELTREE__DEBUG_msg_hack,2,"Disk is",disk);
        hack$signal(DELTREE__DEBUG_msg_hack,2,"Dir  is",tmp2);
        hack$signal(DELTREE__DEBUG_msg_hack,2,"Dirf is",direct);
        hack$signal(DELTREE__DEBUG_msg_hack,2,"Len  is",buffer);
      }
#endif


      /* Define the logical						*/
      /* Sets logical, change directory, recursively delete
	files and come back						*/
      value.dsc$a_pointer	= (void *) direct;
      value.dsc$w_length	= len;

      trans	= LNM$M_CONCEALED | LNM$M_TERMINAL;
#ifdef DEBUG
      if(dodebug)hack$signal(DELTREE__DEBUG_msg_hack,2,"Setting logical to",direct);
#endif
      status = lib$set_logical(&logical, &value, &parent, &trans);
      if( !OK(status) ) hack$exit(DELTREE__SLOGFAIL_msg_hack);


      /* Just in case ...						*/
      ALLOCA(expectedcd,st,1);
      (void *)	strcpy(expectedcd,st);

#ifdef DEBUG
      if(dodebug)hack$signal(DELTREE__DEBUG_msg_hack,2,"expectedcd is",expectedcd);
#endif

      if( chdir(DEEPCD) == 0){
	level++;
	ALLOCA(local_dir,tmp2,1+2);
	(void *) sprintf(local_dir,"./%s",tmp2);

	ALLOCA(save_dir,st,1);
	(void *)strcpy(save_dir,st);
	tmp1	= strrchr(save_dir,'/');
	tmp1++; *tmp1= '\0';


	if(! rm_tree(local_dir) ){
	  if(dolog)hack$signal(DELTREE__DEEPFAIL_msg_hack,0);
	} else {
	  if(dodebug)hack$signal(DELTREE__DEEPOK_msg_hack,0);
	}
	level--;

	free(local_dir);
	free(save_dir);
	save_dir = NULL;
      } else {
	hack$signal(DELTREE__CHDFAIL_msg_hack,1,DEEPDF);
      }
      if(dodebug)hack$signal(DELTREE__DEBUG_msg_hack,2,"****","done");

      /* Note that we need to pretend that we changed directory 	*/
      /* so old-root is the directory we received			*/
      if(chdir(expectedcd) != 0){
	chdir(original);
	hack$exit(DELTREE__CHDIR_msg_hack);
      }
      free(expectedcd);
      free(ptr);
      free(disk);
      return 0;
    } else {
      if(dolog)hack$signal(DELTREE__TOODEEP_msg_hack,1,st);
      return -1;
    }
  } else {
    if(dodebug)hack$signal(DELTREE__DEBUG_msg_hack,2,"going in",st);
    return chdir(st);
  }
}




/* --------------------------------------------------------------------	*/
/* Routines to get the arguments from the command line			*/
/* --------------------------------------------------------------------	*/

unsigned int get_args(void)
{
  unsigned int	status;
  char		what[DIM];

#if defined(__DECC) || defined(__GNUC__)
  extern char	*DELTREE;
#else
  globalref	DELTREE;
#endif

  DDESCR(line_str_d);
  $DESCRIPTOR(command,"DELTREE ");
  $DESCRIPTOR(filin_d,"INFILE");


  lib$get_foreign(&line_str_d);
  str$concat(&line_str_d,&command,&line_str_d);
  status = cli$dcl_parse(&line_str_d,&DELTREE);
  if (! OK(status) ) exit(status);

  if( CLI_PRESENT("HELP") == CLI$_PRESENT)	exit_lhelp(SS$_NORMAL);
  if( CLI_PRESENT("VERSION") == CLI$_PRESENT){
    return DELTREE__VERSION;
  }

  /* Get P1 argument NOW. Do not continue if nothing specified		*/
  if( ! OK( CLI_GET_VALUE("INFILE",what) ) ){
    exit_lhelp(-1);
    hack$exit(DELTREE__NODIR_msg_hack);
  } else {
    ALLOCA(afilin,what,1);
    if( afilin == NULL ) hack$exit(DELTREE__INSFMEM_msg_hack);
    (void *) strcpy(afilin,what);
    while( OK(CLI_GET_VALUE("INFILE",what)) ){
      REALLOCA(afilin,what,1+1);
      (void *) strcat(afilin,",");
      (void *) strcat(afilin,what);
    }
  }


  if( OK(CLI_GET_VALUE("PRIORITY",what)) )	prio = atoi(what);

  status = CLI_PRESENT("DEBUG");
  if( status == CLI$_PRESENT)	dodebug	= TRUE;
  if( status == CLI$_NEGATED)	dodebug	= FALSE;


  status = CLI_PRESENT("LOG");
  if( status == CLI$_PRESENT)	dolog	= TRUE;
  if( status == CLI$_NEGATED)	dolog	= FALSE;


  /* This keyword is the new supported one. DELETE command has		*/
  /* it and therefore /ALLDIR will be possible without major CLD	*/
  /* Modifications.							*/
  status = CLI_PRESENT("TOP");
  if( status == CLI$_PRESENT)	topdir	= TRUE;
  if( status == CLI$_NEGATED)	topdir	= FALSE;

  status = CLI_PRESENT("CONFIRM");
  if( status == CLI$_PRESENT)	doconf	= TRUE;
  if( status == CLI$_NEGATED)	doconf	= FALSE;

  status = CLI_PRESENT("ALLDIR");
  if( status == CLI$_PRESENT)	alldir	= TRUE;
  if( status == CLI$_NEGATED)	alldir	= FALSE;


  /* Privileges list							*/
  if( OK( CLI_GET_VALUE("PRIVILEGES",what) ) ){
    ALLOCA(privs,what,1);
    (void *) strcpy(privs,what);
    while( OK(CLI_GET_VALUE("PRIVILEGES",what)) ){
      REALLOCA(privs,what,1+1);
      (void *) strcat(privs,",");
      (void *) strcat(privs,what);
    }
  } else {
    privs = NULL;
  }

  /* get the NOTIFY method						*/
  if( OK( CLI_GET_VALUE("NOTIFY",what) ) ){
    ALLOCA(notif,what,1);
    (void *) strcpy(notif,what);
  } else {
    notif = NULL;
  }


  /* Detach will now be done outside ...				*/
  return CLI_PRESENT("DETACH");

}


unsigned int CLI_PRESENT(char *item)
{
  DDESCR(item_d);

  IDESCR(item_d,item);
  return cli$present(&item_d);
}

unsigned int CLI_GET_VALUE (char *item, char *retv)
{
  DDESCR(item_d);
  DDESCR(retv_d);
  unsigned int status;

  IDESCR(item_d,item);

  status = cli$present(&item_d);
  if ( OK(status) ) {
    status = cli$get_value(&item_d, &retv_d);
    if (OK(status)){
      strncpy(retv,retv_d.dsc$a_pointer,retv_d.dsc$w_length);
	*(retv+retv_d.dsc$w_length) = '\0';
    }
  }
  return status;
}

/* Strip the path DEEPDF if exists					*/
char	*strip(char *st)
{
  char *base;
  char *name;

  ALLOCA(base,DEEPDF,1);
  (void *) strcpy(base,DEEPDF);
  name = st;
  for(; *name && tolower(*name) == tolower(*base); name++,base++);
  free(base);
  return name;
}



/* -------------------------------------------------------------------	*/
/*   Original Author       : JLAURET					*/
/*   Date                  : 20-OCT-1995 				*/
/* ----------------------- :                                            */
/*   Purpose of Revision   : Got rid of pointer on int and void now	*/
/* -------------------------------------------------------------------	*/
#if defined(VMS) && (__CRTL_VER < 70000000)
# define CLK_SCALE CLK_TCK
#else
# define CLK_SCALE CLOCKS_PER_SEC
#endif


/* Pointer on integer so we can use it in FORtran 			*/
void cpu_time(int what)
{
  clock_t	lc;

  lc = clock();
  if (what == 0) {
    if(lc < 0)hack$exit(DELTREE__CLKINIT_msg_hack);
    init_time = lc;
  } else {
    char	st[DIM];
    lc = lc - init_time;
    sprintf(st,"%.1f",(float) lc/CLK_SCALE);
    hack$signal(DELTREE__STAT_msg_hack,1,total,st);
  }
}




/* -------------------------------------------------------------------	*/
/* Don't panic. Formated with TEXT2C					*/
/* -------------------------------------------------------------------	*/
void	exit_lhelp(int sts)
{
#include "deltree_help.c"
  if(sts != -1) exit(sts);
}


