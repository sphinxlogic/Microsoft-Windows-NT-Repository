/* $Id: tmpthread.c,v 1.2 92/01/11 16:09:21 usenet Exp $
**
** $Log:	tmpthread.c,v $
 * Revision 1.2  92/01/11  16:09:21  usenet
 * header twiddling, bug fixes
 * 
** Revision 4.4.3.1  1991/11/22  04:12:21  davison
** Trn Release 2.0
** 
*/

/* tmpthread.c -- for creating a temporary discussion-thread file
**
** Usage:  tmpthread [opts] news.group last# first# addmax# /tmp/filename
*/

#include "EXTERN.h"
#include "common.h"
#include "threads.h"
#ifdef SERVER
#include "server.h"
#endif
#include "INTERN.h"
#define TMPTHREAD
#include "mthreads.h"

#ifdef TZSET
#include <time.h>
#else
#include <sys/time.h>
#include <sys/timeb.h>
#endif

struct stat filestat;

char buf[LBUFLEN+1];
char *filename, *group, *tmpname;

int first, last, max, start;
int log_verbosity = 0, debug = 0, slow_down = 0;
int ignore_database = 0, read_from_tmp = 0;

char nullstr[1] = "";

#ifdef XTHREAD
int size;
#else
FILE *fp;
#endif

#ifdef TZSET
time_t tnow;
#else
struct timeb ftnow;
#endif

#ifdef SERVER
char *server;
#endif

SIGRET int_handler();

int
main(argc, argv)
int  argc;
char *argv[];
{
    char *cp;

    while (--argc) {
	if (**++argv == '-') {
	    while (*++*argv) {
		switch (**argv) {
		case 'D':
		    debug++;
		    break;
		case 'i':
		    ignore_database++;
		    break;
		case 's':
		    if (*++*argv <= '9' && **argv >= '0') {
			slow_down = atoi(*argv);
			while (*++*argv <= '9' && **argv >= '0') {
			    ;
			}
		    } else {
			slow_down = 1L * 1000 * 1000;
		    }
		    --*argv;
		    break;
		case 't':	/* Use tmpfile w/no byte-order changes */
		    read_from_tmp = 1;
		    break;
		case 'T':	/* Use tmpfile w/possible byte-order changes */
		    read_from_tmp = 2;
		    break;
		case 'v':
		    log_verbosity++;
		    break;
		default:
		    fprintf(stderr, "Unknown option: '%c'\n", **argv);
		    exit(1);
		}
	    }
	} else {
	    break;
	}
    }

    if (argc < 5) {
      give_usage:
	fprintf(stderr,
"Usage:  tmpthread [opts] news.group last# first# addmax# /tmp/filename\n");
	exit(1);
    }

    group = argv[0];
    last = atoi(argv[1]);
    first = atoi(argv[2]);
    max = atoi(argv[3]);
    tmpname = argv[4];

    if (first < 0 || last < first - 1 || max <= 0) {
	goto give_usage;
    }

    /* Initialize umask(), file_exp(), etc. */
    mt_init();

#ifdef SIGHUP
    if (sigset(SIGHUP, SIG_IGN) != SIG_IGN) {
	sigset(SIGHUP, int_handler);
    }
#endif
    if (sigset(SIGINT, SIG_IGN) != SIG_IGN) {
	sigset(SIGINT, int_handler);
    }
#ifdef SIGQUIT
    if (sigset(SIGQUIT, SIG_IGN) != SIG_IGN) {
	sigset(SIGQUIT, int_handler);
    }
#endif
    sigset(SIGTERM, int_handler);
#ifdef SIGBUS
    sigset(SIGBUS, int_handler);
#endif
    sigset(SIGSEGV, int_handler);
#ifdef SIGTTIN
    sigset(SIGTTIN, SIG_IGN);
    sigset(SIGTTOU, SIG_IGN);
#endif

#ifdef lint
    int_handler(SIGINT);
#endif

#ifdef SERVER
    if ((server = get_server_name(0)) == NULL) {
	log_entry("couldn't find name of news server.\n");
	exit(1);
    }
    switch (server_init(server)) {
    case OK_NOPOST:
    case OK_CANPOST:
	break;
    case ERR_ACCESS:
	log_entry("Server %s rejected connection.\n", server);
	exit(1);
    default:
	log_entry("Couldn't connect with server %s.\n", server);
	exit(1);
    }
#endif

    /* See if this machine needs byte-order translation for the database */
    word_same = long_same = TRUE;
    if (read_from_tmp != 1) {
#ifdef XTHREAD
	put_server("XTHREAD DBINIT");
	rawcheck_server(buf, sizeof buf);
	size = rawget_server((char*)&mt_bmap, sizeof (BMAP));
	if (size < sizeof (BMAP) - 1) {
#else
	if ((fp = fopen(file_exp(DBINIT), FOPEN_RB)) == Nullfp
	 || fread((char*)&mt_bmap, 1, sizeof (BMAP), fp) < sizeof (BMAP)-1) {
#endif
	    log_entry("db.init read failed -- assuming no byte-order translations.\n\n");
	    mybytemap(&mt_bmap);
	} else {
	    int i;

	    if (mt_bmap.version != DB_VERSION) {
		log_entry("Thread database is not the right version -- ignoring it.\n");
		ignore_database = 1;
	    }
	    mybytemap(&my_bmap);
	    for (i = 0; i < sizeof (LONG); i++) {
		if (i < sizeof (WORD)) {
		    if (my_bmap.w[i] != mt_bmap.w[i]) {
			word_same = FALSE;
		    }
		}
		if (my_bmap.l[i] != mt_bmap.l[i]) {
		    long_same = FALSE;
		}
	    }
	}
#ifdef XTHREAD
	while (rawget_server(buf, sizeof buf)) {
	    ;		/* trash any extraneous bytes */
	}
#else
	if (fp != Nullfp) {
	    fclose(fp);
	}
#endif
    }

    /* What time is it? */
#ifdef TZSET
    (void) time(&tnow);
    (void) tzset();
#else
    (void) ftime(&ftnow);
#endif

#ifdef SERVER
    sprintf(buf, "GROUP %s", group);
    put_server(buf);
    if (get_server(buf, sizeof buf) < 0 || *buf != CHAR_OK) {
	log_entry("NNTP failure on group `%s'.\n", group);
#else
    strcpy(cp = buf, group);
    while ((cp = index(cp, '.'))) {
	*cp = '/';
    }
    filename = file_exp(buf);		/* relative to spool dir */
    if (chdir(filename) < 0) {
	if (errno != ENOENT) {
	    log_entry("Unable to chdir to `%s'.\n", filename);
	}
#endif
	exit(1);
    } else {
	if (read_from_tmp) {
	    filename = tmpname;
	} else {
#ifdef XTHREAD
	    ignore_database = TRUE;
#else
	    filename = thread_name(group);
#endif
	}
	if (ignore_database || !init_data(filename) || !read_data()) {
	    (void) init_data(Nullch);
	    total.last = first - 1;
	    total.first = first;
	}
	start = total.last + 1;
	if (start < last - max + 1) {
	    start = last - max + 1;
	}
	process_articles(first, last);
	putchar('\n') FLUSH;
	if (!write_data(tmpname)) {
	    exit(1);
	}
    }
    return 0;
}

SIGRET
int_handler(sig)
int sig;
{
    /* Simply bug out with an error flag. */
    printf("interrupt %d!\n", sig) FLUSH;
    exit(1);
}

void
wrap_it_up(ret)
int ret;
{
    exit(ret);
}

/* Generate a "log entry" for the interactive user.
*/
/*VARARGS1*/
void
log_entry(fmt, arg1, arg2)
char *fmt;
long arg1;
long arg2;
{
    printf("tmpthread: ");
    printf(fmt, arg1, arg2);
    fflush(stdout);
}

/* Generate a "log entry", with newsgroup name for the interactive user.
*/
/*VARARGS1*/
void
log_error(fmt, arg1, arg2, arg3)
char *fmt;
long arg1;
long arg2;
long arg3;
{
    log_entry("%s: ", group);
    printf(fmt, arg1, arg2, arg3);
    fflush(stdout);
}
