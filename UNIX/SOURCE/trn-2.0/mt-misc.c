/* $Id: mt-misc.c,v 1.2 92/01/11 16:05:06 usenet Exp $
**
** $Log:	mt-misc.c,v $
 * Revision 1.2  92/01/11  16:05:06  usenet
 * header twiddling, bug fixes
 * 
** Revision 4.4.3.1  1991/11/22  04:12:15  davison
** Trn Release 2.0
** 
*/

#include "EXTERN.h"
#include "common.h"
#include "threads.h"
#include "mthreads.h"

char *lib, *rnlib, *mtlib, *spool, *threaddir, *homedir;
int locked = 0, cron_locking = 0;

void
mt_init()
{
    /* Set up a nice friendly umask. */
    umask(002);

    /* Init the directory strings, possibly translating them into real paths */
    homedir = getenv("HOME");
    if (homedir == Nullch) {
	homedir = getenv("LOGDIR");
    }
    spool = savestr(file_exp(SPOOL));
    lib = savestr(file_exp(LIB));
    rnlib = savestr(file_exp(RNLIB));
    mtlib = savestr(file_exp(MTLIB));
    threaddir = savestr(file_exp(THREAD_DIR));
}

/* Make sure we're not already running by creating a lock file. */
long
mt_lock(which_lock, sig)
int which_lock;
int sig;
{
    char buff[LBUFLEN], *filename;
    FILE *fp;

    sprintf(buff, "%s.%ld", file_exp(MTPRELOCK), (long)getpid());
    if ((fp = fopen(buff, "w")) == Nullfp) {
	log_entry("Unable to create lock temporary `%s'.\n", buff);
	wrap_it_up(1);
    }
    fprintf(fp, "%s%ld\n", which_lock == DAEMON_LOCK ? "pid " : nullstr,
	(long)getpid());
    fclose(fp);

    /* Try to link to lock file. */
    if (which_lock == DAEMON_LOCK) {
	filename = file_exp(MTDLOCK);
    } else {
	filename = file_exp(MTLOCK);
    }
  dolink:
    while (link(buff, filename) < 0) {
      long otherpid;
	if ((fp = fopen(filename, "r")) == Nullfp) {
	    log_entry("unable to open %s\n", filename);
	    if (cron_locking) {
		goto Sleep;
	    }
	    unlink(buff);
	    wrap_it_up(1);
	}
	if (fscanf(fp, "%ld", &otherpid) != 1) { 
	    log_entry("unable to read pid from %s\n", filename);
	    fclose(fp);
	    if (cron_locking) {
		goto Sleep;
	    }
	    unlink(buff);
	    wrap_it_up(1);
	}
	fclose(fp);
	if (kill(otherpid, sig) == -1 && errno == ESRCH) {
	    if (unlink(filename) == -1) {
		log_entry("unable to unlink lockfile %s\n", filename);
		unlink(buff);
		wrap_it_up(1);
	    }
	    goto dolink;
	}
	if (cron_locking) {
	  Sleep:
	    sleep(60);
	    continue;
	}
	unlink(buff);
	return otherpid;
    }
    unlink(buff);			/* remove temporary LOCK.<pid> file */
    locked |= which_lock;
    return 0;				/* return success */
}

void
mt_unlock(which_lock)
int which_lock;
{
    which_lock &= locked;
    if (which_lock & PASS_LOCK) {
	unlink(file_exp(MTLOCK));		/* remove single-pass lock */
    }
    if (which_lock & DAEMON_LOCK) {
	unlink(file_exp(MTDLOCK));		/* remove daemon lock */
    }
    locked &= ~which_lock;
}

/* Interpret rn's %x prefixes and ~name expansions without including tons
** of useless source.  NOTE:  names that don't start with '/', '%' or '~'
** are prefixed with the SPOOL directory.  (Note that ~'s don't work yet.)
*/
char *
file_exp(name)
char *name;
{
    static char namebuf[MAXFILENAME];

    if (*name == '/') {	/* fully qualified names are left alone */
	return name;
    }
    switch (name[0]) {
    case '%':			/* interpret certain %x values */
	switch (name[1]) {
	case 'P':
	    strcpy(namebuf, spool);
	    break;
	case 'w':
	    strcpy(namebuf, mtlib);
	    break;
	case 'W':
	    strcpy(namebuf, threaddir);
	    break;
	case 'x':
	    strcpy(namebuf, lib);
	    break;
	case 'X':
	    strcpy(namebuf, rnlib);
	    break;
	default:
	    log_entry("Unknown expansion: %s\n", name);
	    wrap_it_up(1);
	}
	strcat(namebuf, name+2);
	break;
    case '~':
    {
	char *s = name + 1;

	if (!*s || *s == '/') {
	    sprintf(namebuf, "%s%s", homedir, s);
	} else {
	    log_entry("~name expansions not implemented.");
	    wrap_it_up(1);
	}
    }
    default:			/* all "normal" names are relative to SPOOL */
	sprintf(namebuf, "%s/%s", spool, name);
	break;
    }
    return namebuf;
}

#ifndef lint
/* A malloc that bombs-out when memory is exhausted. */
char *
safemalloc(amount)
MEM_SIZE amount;
{
    register char *cp;
    char *malloc();

    if ((cp = malloc(amount)) == Nullch) {
	log_error("malloc(%ld) failed.\n", (long)amount);
	wrap_it_up(1);
    }
    return cp;
}

/* paranoid version of realloc */
char *
saferealloc(where,size)
char *where;
MEM_SIZE size;
{
    char *ptr;
    char *realloc();

    ptr = realloc(where,size?size:1);	/* realloc(0) is NASTY on our system */
    if (ptr == Nullch) {
	log_error("realloc(..., %ld) failed.\n", (long)size);
	wrap_it_up(1);
    }
    return ptr;
}
#endif

/* Create a malloc'ed copy of a string. */
char *
savestr(str)
char *str;
{
    register MEM_SIZE len = strlen(str) + 1;
    register char *newaddr = safemalloc(len);

    bcopy(str, newaddr, (int)len);

    return newaddr;
}

#ifndef lint
/* Free some memory if it hasn't already been freed. */
void
safefree(pp)
char **pp;
{
    if (*pp) {
	free(*pp);
	*pp = Nullch;
    }
}
#endif
