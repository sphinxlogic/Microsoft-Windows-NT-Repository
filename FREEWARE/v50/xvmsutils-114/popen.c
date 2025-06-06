#ifndef lint
static char    *RCSid = "$Id: vms.c,v 1.2 1995/03/31 13:24:29 drd Exp $";
#endif

/* drop in popen() / pclose() for VMS
 * originally written for port of perl to vms
 */

static int something_in_this_file;

 
/* (to aid porting) - how are errors dealt with */

#define ERROR(msg) { fprintf(stderr, "%s\nFile %s line %d\n", msg, __FILE__, __LINE__); }
#define FATAL(msg) { fprintf(stderr, "%s\nFile %s line %d\n", msg, __FILE__, __LINE__); exit(EXIT_FAILURE); }

 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dvidef.h>
#include <syidef.h>
#include <jpidef.h>
#include <ssdef.h>
#include <descrip.h>
#include <signal.h>
#include <lib$routines.h>
#include <starlet.h>

#define _cksts(call) \
  if (!(sts=(call))&1) FATAL("Internal error") else {}

static void
create_mbx(unsigned short int *chan, struct dsc$descriptor_s *namdsc)
{
	static unsigned long int mbxbufsiz;
		long int syiitm = SYI$_MAXBUF, dviitm = DVI$_DEVNAM;
	unsigned long sts;  /* for _cksts */
  
  if (!mbxbufsiz) {
    /*
     * Get the SYSGEN parameter MAXBUF, and the smaller of it and the
     * preprocessor consant BUFSIZ from stdio.h as the size of the
     * 'pipe' mailbox.
     */

    _cksts(lib$getsyi(&syiitm, &mbxbufsiz, 0, 0, 0, 0));
    if (mbxbufsiz > BUFSIZ) mbxbufsiz = BUFSIZ; 
  }
  _cksts(sys$crembx(0,chan,mbxbufsiz,mbxbufsiz,0,0,0));

  _cksts(lib$getdvi(&dviitm, chan, NULL, NULL, namdsc, &namdsc->dsc$w_length));
  namdsc->dsc$a_pointer[namdsc->dsc$w_length] = '\0';

}  /* end of create_mbx() */

struct pipe_details
{
    struct pipe_details *next;
    FILE *fp;
    int pid;
    unsigned long int completion;
};

static struct pipe_details *open_pipes = NULL;
static $DESCRIPTOR(nl_desc, "NL:");
static int waitpid_asleep = 0;

static void
popen_completion_ast(unsigned long int unused)
{
  if (waitpid_asleep) {
    waitpid_asleep = 0;
    sys$wake(0,0);
  }
}

FILE *
popen(char *cmd, char *mode)
{
    char mbxname[64];
    unsigned short int chan;
    unsigned long int flags=1;  /* nowait - gnu c doesn't allow &1 */
    struct pipe_details *info;
    unsigned long sts;                            
    struct dsc$descriptor_s namdsc;
    struct dsc$descriptor_s cmddsc;

    namdsc.dsc$w_length= strlen(mbxname);
    namdsc.dsc$b_dtype= DSC$K_DTYPE_T;
    namdsc.dsc$b_class= DSC$K_CLASS_S;
    namdsc.dsc$a_pointer= mbxname;

    cmddsc.dsc$w_length= 0;
    cmddsc.dsc$b_dtype= DSC$K_DTYPE_T;
    cmddsc.dsc$b_class= DSC$K_CLASS_S;
    cmddsc.dsc$a_pointer= 0;

    if (!(info=malloc(sizeof(struct pipe_details))))
    {
    	ERROR("Cannot malloc space");
    	return NULL;
    }

    info->completion=0;  /* I assume this will remain 0 until terminates */
        
    /* create mailbox */
    create_mbx(&chan,&namdsc);

    /* open a FILE* onto it */
    info->fp=fopen(mbxname, mode);

    /* give up other channel onto it */
    _cksts(sys$dassgn(chan));

    if (!info->fp)
        return NULL;
        
    cmddsc.dsc$w_length=strlen(cmd);
    cmddsc.dsc$a_pointer=cmd;

    if (strcmp(mode,"r")==0) {
      _cksts(lib$spawn(&cmddsc, &nl_desc, &namdsc, &flags,
                     0  /* name */, &info->pid, &info->completion,
                     0, popen_completion_ast,0,0,0,0));
    }
    else {
      _cksts(lib$spawn(&cmddsc, &namdsc, 0 /* sys$output */, &flags,
                     0  /* name */, &info->pid, &info->completion));
    }

    info->next=open_pipes;  /* prepend to list */
    open_pipes=info;
        
    return info->fp;
}

int pclose(FILE *fp)
{
    struct pipe_details *info, *last = NULL;
    unsigned long int abort = SS$_TIMEOUT, retsts;
    unsigned long sts;
    
    for (info = open_pipes; info != NULL; last = info, info = info->next)
        if (info->fp == fp) break;

    if (info == NULL)
      /* get here => no such pipe open */
      FATAL("pclose() - no such pipe open ???");

    if (!info->completion) { /* Tap them gently on the shoulder . . .*/
      _cksts(sys$forcex(&info->pid,0,&abort));
      sleep(1);
    }
    if (!info->completion)  /* We tried to be nice . . . */
      _cksts(sys$delprc(&info->pid));
    
    fclose(info->fp);
    /* remove from list of open pipes */
    if (last) last->next = info->next;
    else open_pipes = info->next;
    retsts = info->completion;
    free(info);

    return retsts;
}  /* end of pclose() */


/* sort-of waitpid; use only with popen() */
/*{{{unsigned long int waitpid(unsigned long int pid, int *statusp, int flags)*/
unsigned long int
waitpid(unsigned long int pid, int *statusp, int flags)
{
    struct pipe_details *info;
    unsigned long int abort = SS$_TIMEOUT;
    unsigned long sts;
    
    for (info = open_pipes; info != NULL; info = info->next)
        if (info->pid == pid) break;

    if (info != NULL) {  /* we know about this child */
      while (!info->completion) {
        waitpid_asleep = 1;
        sys$hiber();
      }

      *statusp = info->completion;
      return pid;
    }
    else {  /* we haven't heard of this child */
      $DESCRIPTOR(intdsc,"0 00:00:01");
      unsigned long int ownercode = JPI$_OWNER, ownerpid, mypid;
      unsigned long int interval[2];

      _cksts(lib$getjpi(&ownercode,&pid,0,&ownerpid,0,0));
      _cksts(lib$getjpi(&ownercode,0,0,&mypid,0,0));
      if (ownerpid != mypid)
        FATAL("pid not a child");

      _cksts(sys$bintim(&intdsc,interval));
      while ((sts=lib$getjpi(&ownercode,&pid,0,&ownerpid,0,0)) & 1) {
        _cksts(sys$schdwk(0,0,interval,0));
        _cksts(sys$hiber());
      }
      _cksts(sts);

      /* There's no easy way to find the termination status a child we're
       * not aware of beforehand.  If we're really interested in the future,
       * we can go looking for a termination mailbox, or chase after the
       * accounting record for the process.
       */
      *statusp = 0;
      return pid;
    }
                    
}  /* end of waitpid() */

