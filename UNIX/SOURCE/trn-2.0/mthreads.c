/* $Id: mthreads.c,v 1.2 92/01/11 16:05:25 usenet Exp $
**
** $Log:	mthreads.c,v $
 * Revision 1.2  92/01/11  16:05:25  usenet
 * header twiddling, bug fixes
 * 
** Revision 4.4.3.1  1991/11/22  04:12:15  davison
** Trn Release 2.0
** 
*/

/* mthreads.c -- for making and updating a discussion-thread database
**
** We use the active file to read the high/low counts for each newsgroup
** and compare them with the corresponding values in a file called active2
** (which we created to keep the database high/lows in one place).  If they
** don't match, we read/update/write the group's thread file and move on.
** If the active2 file is missing or corrupted, it will be repaired in the
** normal course of operation.
**
** Usage:  mthreads [-d[MM]] [-e[HHMM]] [-s[hsec]] [-aDfknv] [hierarchy_list]
*/

#include "EXTERN.h"
#include "common.h"
#include "threads.h"
#ifdef SERVER
#include "server.h"
#endif
#include "INTERN.h"
#include "mthreads.h"

#ifdef TZSET
#include <time.h>
#else
#include <sys/time.h>
#include <sys/timeb.h>
#endif

#if !defined(FTRUNCATE) && !defined(CHSIZE)
# ifdef F_FREESP
#  define MYCHSIZE
# else
#  define MVTRUNC
# endif
#endif

#ifdef USESYSLOG
#include <syslog.h>
#else
FILE *fp_log;
#endif

FILE *fp_tmp, *fp_active, *fp_active2, *fp_active2w = Nullfp;
bool eof_active = FALSE, eof_active2 = FALSE;
long first, last, first2, last2;
char ch, ch2;

struct stat filestat;

char buf[LBUFLEN+1];

static char line[256];
static char line2[256];

char fmt_active2[] = "%s %010ld %07ld %c\n";

char *filename;

typedef struct _active_line {
    struct _active_line *link;
    char *name;
    long last;
    long first;
    char type;
} ACTIVE_LINE;

#define Nullact Null(ACTIVE_LINE*)

ACTIVE_LINE *line_root = Nullact, *last_line = Nullact, *pline = Nullact;

bool force_flag = FALSE, kill_mthreads = FALSE, no_processing = FALSE;
bool add_new = FALSE, rebuild = FALSE, zap_thread = FALSE;
bool acttimes_flag = FALSE, grevious_error;
bool initializing = TRUE;
int daemon_delay = 0, log_verbosity = 0, debug = 0, slow_down = 0;
long expire_time = 0;
char *hierarchy_list = NULL;
long truncate_len = -1;

char nullstr[1] = "";

extern int locked, cron_locking;

#ifdef TZSET
time_t tnow;
#else
struct timeb ftnow;
#endif

#define TIMER_FIRST 1
#define TIMER_DEFAULT (10 * 60)

int processed_groups = 0, added_groups = 0, removed_groups = 0;
int action;

#define NG_DEFAULT	0
#define NG_MATCH	1
#define NG_SKIP		2

#ifdef SERVER
char *server;
#else
time_t last_modified = 0;
#endif

SIGRET alarm_handler(), int_handler();
bool makethreads ANSI((void));
void log_startup ANSI((void));
void log_stats ANSI((void));

int
main(argc, argv)
int  argc;
char *argv[];
{
    int fd;
    long pid;

    while (--argc) {
	if (**++argv == '-') {
	    while (*++*argv) {
		switch (**argv) {
		case 'a':		/* automatically thread new groups */
		    add_new = TRUE;
		    break;
		case 'c':		/* continue trying to lock */
		    cron_locking = TRUE;
		    break;
		case 'D':		/* run in debug mode */
		    debug++;
		    break;
		case 'd':		/* run in daemon mode */
		    if (*++*argv <= '9' && **argv >= '0') {
			daemon_delay = atoi(*argv) * 60;
			while (*++*argv <= '9' && **argv >= '0') {
			    ;
			}
		    } else {
			daemon_delay = TIMER_DEFAULT;
		    }
		    --*argv;
		    break;
		case 'e': {		/* enhanced expire processing */
		    struct tm *ts;
		    long desired;

		    (void) time(&expire_time);
		    ts = localtime(&expire_time);

		    if (*++*argv <= '9' && **argv >= '0') {
			desired = atol(*argv);
			if (desired/100 > 23 || desired%100 > 59) {
			    fprintf(stderr, "Illegal expire time: '%04d'\n",
				desired);
			    exit(1);
			}
			desired = (desired/100)*60 + desired%100;
			while (*++*argv <= '9' && **argv >= '0') {
			    ;
			}
		    } else {
			desired = 30;			/* 0030 = 12:30am */
		    }
		    --*argv;
		    desired -= ts->tm_hour * 60 + ts->tm_min;
		    if (desired < 0) {
			desired += 24 * 60;
		    }
		    expire_time += desired * 60 - ts->tm_sec;
		    break;
		 }
		case 'f':		/* force each group to process */
		    force_flag = TRUE;
		    break;
		case 'k':		/* kill running mthreads */
		    kill_mthreads = TRUE;
		    break;
		case 'n':		/* don't process anything */
		    no_processing = TRUE;
		    break;
		case 's':		/* sleep between articles */
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
		case 't':		/* maintain active.times file */
#ifdef ACTIVE_TIMES
		    if (strEQ(ACTIVE_TIMES, "nntp")) {
			fprintf(stderr, "Ignoring the -t option.\n");
		    } else {
			acttimes_flag = TRUE;
		    }
#else
		    fprintf(stderr, "Ignoring the -t option.\n");
#endif
		    break;
		case 'v':		/* get more verbose in the log file */
		    log_verbosity++;
		    break;
		case 'z':		/* destroy .thread on severe signal */
		    zap_thread = TRUE;
		    break;
		default:
		    fprintf(stderr, "Unknown option: '%c'\n", **argv);
		    exit(1);
		}
	    }
	} else {
	    if (hierarchy_list) {
		fprintf(stderr, "Specify the newsgroups in one comma-separated list.\n");
		exit(1);
	    }
	    hierarchy_list = *argv;
	}
    }

    /* Initialize umask(), file_exp(), etc. */
    mt_init();

    /* If this is a kill request, look for the daemon lock file. */
    if (kill_mthreads) {
	if (!mt_lock(DAEMON_LOCK, SIGTERM)) {
	    fprintf(stderr, "No mthreads daemon is running.\n");
	    wrap_it_up(1);
	}
	fprintf(stderr, "Killed mthreads daemon.\n");
	wrap_it_up(0);
    }

    /* See if an mthreads pass is already going. */
    if (mt_lock(PASS_LOCK, 0) != 0 && !daemon_delay) {
	fprintf(stderr, "mthreads is already running.\n");
	wrap_it_up(1);
    }

#ifdef USESYSLOG
# ifdef LOG_DAEMON
    openlog("mthreads", LOG_PID, USESYSLOG);
# else
    openlog("mthreads", LOG_PID);
# endif
#else
    /* Open our log file */
    filename = file_exp(MTLOG);
    if ((fp_log = fopen(filename, "a")) == Nullfp) {
	fprintf(stderr, "Unable to open `%s'.\n", filename);
	wrap_it_up(1);
    }
#endif

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
    sigset(SIGALRM, SIG_IGN);
#ifdef SERVER
    sigset(SIGPIPE, int_handler);
#endif
#ifdef lint
    alarm_handler();			/* foolishness for lint's sake */
    int_handler(SIGINT);
#endif

    /* Ensure this machine has the right byte-order for the database */
    filename = file_exp(DBINIT);
    if ((fp_tmp = fopen(filename, FOPEN_RB)) == Nullfp
     || fread((char*)&mt_bmap,1,sizeof (BMAP), fp_tmp) < sizeof (BMAP)-1) {
	if (fp_tmp != Nullfp) {
	    fclose(fp_tmp);
	}
     write_db_init:
	mybytemap(&mt_bmap);
	if ((fp_tmp = fopen(filename, FOPEN_WB)) == Nullfp) {
	    log_entry("Unable to create file: `%s'.\n", filename);
	    wrap_it_up(1);
	}
	mt_bmap.version = DB_VERSION;
	fwrite((char*)&mt_bmap, 1, sizeof (BMAP), fp_tmp);
	fclose(fp_tmp);
    } else {
	int i;

	fclose(fp_tmp);
	if (mt_bmap.version != DB_VERSION) {
	    if (mt_bmap.version == DB_VERSION-1) {
		rebuild = TRUE;
		log_entry("Upgrading database to version %d.\n", DB_VERSION);
		goto write_db_init;
	    }
	    log_entry("** Database is not the right version (%d instead of %d) **\n",
		mt_bmap.version, DB_VERSION);
	    wrap_it_up(1);
	}
	mybytemap(&my_bmap);
	for (i = 0; i < sizeof (LONG); i++) {
	    if (my_bmap.l[i] != mt_bmap.l[i]
	     || (i < sizeof (WORD) && my_bmap.w[i] != mt_bmap.w[i])) {
		log_entry("\
** Byte-order conflict -- re-run from a compatible machine **\n\
\t\tor remove the current thread files, including db.init **\n");
		wrap_it_up(1);
	    }
	}
    }

#ifdef SERVER
    if ((server = get_server_name(0)) == NULL) {
	log_entry("Couldn't find name of news server.\n");
	wrap_it_up(1);
    }
#endif

    initializing = FALSE;

    /* If we're not in daemon mode, run through once and quit. */
    if (!daemon_delay) {
	log_startup();
	setbuf(stdout, Nullch);
	extra_expire = (expire_time != 0);
	makethreads();
    } else {
	cron_locking = FALSE;
	if (mt_lock(DAEMON_LOCK, 0) != 0) {
	    fprintf(stderr, "An mthreads daemon is already running.\n");
	    wrap_it_up(1);
	}
	/* For daemon mode, we cut ourself off from anything tty-related and
	** run in the background (involves forks, but no knives).
	*/
	close(0);
	if (open("/dev/null", 2) != 0) {
	    fprintf(stderr, "unable to open /dev/null!\n");
	    wrap_it_up(1);
	}
	close(1);
	close(2);
	dup(0);
	dup(0);
	while ((pid = fork()) < 0) {
	    sleep(2);
	}
	if (pid) {
	    exit(0);
	}
#ifdef TIOCNOTTY
	if ((fd = open("/dev/tty", 1)) >= 0) {
	    ioctl(fd, TIOCNOTTY, (int*)0);
	    close(fd);
	}
#else
	(void) setpgrp();
	while ((pid = fork()) < 0) {
	    sleep(2);
	}
	if (pid) {
	    exit(0);
	}
#endif
	/* Put our pid in the lock file for death detection */
	if ((fp_tmp = fopen(file_exp(MTDLOCK), "w")) != Nullfp) {
	    fprintf(fp_tmp, "%ld\n", (long)getpid());
	    fclose(fp_tmp);
	}
	log_startup();

	sigset(SIGALRM, alarm_handler);

	/* Start timer -- first interval is shorter than all others */
	alarm(TIMER_FIRST);
	for (;;) {
	    pause();		/* let alarm go off */
	    alarm(0);

#ifndef USESYSLOG
	    /* Re-open our log file, if needed */
	    if (!fp_log && !(fp_log = fopen(file_exp(MTLOG), "a"))) {
		wrap_it_up(1);
	    }
#endif
#ifndef SERVER
	    if (stat(file_exp(ACTIVE), &filestat) < 0) {
		log_entry("Unable to stat active file -- quitting.\n");
		wrap_it_up(1);
	    }
#endif
	    if (expire_time && time(Null(time_t*)) > expire_time) {
		expire_time += 24L * 60 * 60;
		extra_expire = TRUE;
	    }
#ifdef SERVER
	    makethreads();	/* NNTP version always compares files */
#else
	    if (extra_expire || filestat.st_mtime != last_modified) {
		last_modified = filestat.st_mtime;
		if (!makethreads()) {
		    last_modified--;
		}
	    }
#endif
	    alarm(daemon_delay);
#ifndef USESYSLOG
	    fclose(fp_log);	/* close the log file while we sleep */
	    fp_log = Nullfp;
#endif
	} /* for */
    }/* if */

    wrap_it_up(0);
    return 0;				/* NOTREACHED */
}

SIGRET
alarm_handler()
{
    sigset(SIGALRM, alarm_handler);
}

SIGRET
int_handler(sig)
int sig;
{
    static int visits = 0;
    int ret = 0;

    if (++visits > 4) {
	wrap_it_up(1);
    }

#ifndef USESYSLOG
    /* Re-open our log file, if needed */
    if (fp_log || (fp_log = fopen(file_exp(MTLOG), "a")))
#endif
    {
	switch (sig) {
	case SIGTERM:
#ifdef SIGHUP
	case SIGHUP:
#endif
#ifdef SIGQUIT
	case SIGQUIT:
#endif
	    log_entry("halt requested.\n");
	    zap_thread = 0;
	    break;
#ifdef SERVER
	case SIGPIPE:
	    log_entry("broken pipe -- trying to continue.\n");
	    sigset(SIGPIPE, int_handler);
	    return;
#endif
#ifdef SIGBUS
	    case SIGBUS:
#endif
	    case SIGSEGV:
		log_error("** Severe signal: %d **\n", sig);
		/* Destroy offending thread file if requested to do so. */
		if (zap_thread) {
		    unlink(thread_name(line));
		    log_entry("Destroyed thread file for %s\n", line);
		}
		break;
	default:
	    log_entry("interrupt %d received.\n", sig);
	    zap_thread = 0;
	    ret = 1;
	    break;
	}
    }
    if (!daemon_delay) {
	printf("Interrupt %d!\n", sig);
	if (zap_thread) {
	    printf("Destroyed thread file for %s\n", line);
	}
    }

    /* If we're in the middle of writing the new active2 file, finish it. */
    if (fp_active2w) {
	if (*line2) {
	    if (index(line2, ' ')) {
		fputs(line2, fp_active2w);
	    } else {
		fprintf(fp_active2w, fmt_active2, line2, last2, first2, ch2);
	    }
	}
	for (pline = line_root; pline; pline = pline->link) {
	    fprintf(fp_active2w, fmt_active2,
			pline->name, pline->last, pline->first, pline->type);
	}
	if (!eof_active2) {
	    while (fgets(line2, sizeof line2, fp_active2)) {
		fputs(line2, fp_active2w);
	    }
	}
	log_stats();
    }
    wrap_it_up(ret);
}

void
wrap_it_up(ret)
int ret;
{
    mt_unlock(locked);
    exit(ret);
}

/* Process the active file, creating/modifying the active2 file and
** creating/modifying the thread data files.
*/
bool
makethreads()
{
    register char *cp, *cp2;
    char data_file_open;
    bool update_successful, old_groups, touch_thread;
#ifdef SERVER
    int server_failure = 0;
#endif

    /* See if an mthreads pass is already going. */
    if (!(locked & PASS_LOCK) && mt_lock(PASS_LOCK, 0) != 0) {
	log_entry("unable to get a lock for this pass.\n");
	return FALSE;
    }
#ifdef SERVER
    if (!open_server()) {
	return FALSE;
    }
    put_server("LIST");	/* ask server for the active file */
    get_server(line, sizeof line);
    if (*line != CHAR_OK) {
	log_entry("Unable to get active file from server.\n");
	close_server();
	return FALSE;
    }
    if ((fp_active = fopen(file_exp(ACTIVE1), "w+")) == Nullfp) {
	log_entry("Unable to write the active1 file -- quitting.\n");
	wrap_it_up(1);
    }
    while (1) {
	if (get_server(line, sizeof line) < 0) {
	    log_entry("Server failed to send entire active file.\n");
	    fclose(fp_active);
	    close_server();
	    return FALSE;
	}
	if (*line == '.') {
	    break;
	}
	fputs(line, fp_active);
	putc('\n', fp_active);
    }
    if (ferror(fp_active)) {
	log_entry("Error writing to active1 file.\n");
	fclose(fp_active);
	close_server();
	return FALSE;
    }
    fseek(fp_active, 0L, 0);		/* rewind for read */
#else /* not SERVER */
    if ((fp_active = fopen(file_exp(ACTIVE), "r")) == Nullfp) {
	log_entry("Unable to open the active file.\n");
	wrap_it_up(1);
    }
#endif
    filename = file_exp(ACTIVE2);
    if ((fp_active2w = fopen(filename, "r+")) == Nullfp) {
	if ((fp_active2w = fopen(filename, "w")) == Nullfp) {
	    log_entry("Unable to open the active2 file for update.\n");
	    wrap_it_up(1);
	}
	/* Add existing groups to active.times file with ancient date. */
	old_groups = TRUE;
    } else {
	old_groups = FALSE;
    }
    if ((fp_active2 = fopen(filename, "r")) == Nullfp) {
	log_entry("Unable to open the active2 file.\n");
	wrap_it_up(1);
    }
    if (extra_expire && log_verbosity) {
	log_entry("Using enhanced expiration for this pass.\n");
    }

    /* What time is it? */
#ifdef TZSET
    (void) time(&tnow);
    (void) tzset();
#else
    (void) ftime(&ftnow);
#endif

    eof_active = eof_active2 = FALSE;
    fp_tmp = Nullfp;

    /* Loop through entire active file. */
    for (;;) {
	if (eof_active || !fgets(line, sizeof line, fp_active)) {
	    if (eof_active2 && !line_root) {
		break;
	    }
	    eof_active = TRUE;
	    ch = 'x';
	} else {
	    cp = line + strlen(line) - 1;
	    if (*cp == '\n') {
		*cp = '\0';
	    }
	    if (!(cp = index(line, ' '))) {
		log_entry("** line in 'active' has no space: %s **\n", line);
		continue;
	    }
	    *cp = '\0';
	    if (sscanf(cp+1, "%ld %ld %c", &last, &first, &ch) != 3) {
		log_entry("** digits corrupted in 'active': %s %s **\n",
			line, cp+1);
		continue;
	    }
	    if (last < first - 1) {
		log_entry("** bogus group values in 'active': %s %s **\n",
			line, cp+1);
		continue;
	    }
	}
	if (debug > 1 || log_verbosity > 3) {
	    log_entry("Processing %s:\n", line);
	}
	data_file_open = 0;
	/* If we've allocated some lines in memory while searching for
	** newsgroups (they've scrambled the active file on us), check
	** them first.
	*/
	last_line = Nullact;
	for (pline = line_root; pline; pline = pline->link) {
	    if (eof_active || strEQ(line, pline->name)) {
		strcpy(line2, pline->name);
		free(pline->name);
		first2 = pline->first;
		last2 = pline->last;
		ch2 = pline->type;
		if (last_line) {
		    last_line->link = pline->link;
		} else {
		    line_root = pline->link;
		}
		free(pline);
		break;
	    }
	    last_line = pline;
	}/* for */
	touch_thread = FALSE;

	/* If not found yet, check the active2 file. */
	if (!pline) {
	    for (;;) {
		if (eof_active2 || !fgets(line2, sizeof line2, fp_active2)) {
		    /* At end of file, check if the thread data file exists.
		    ** If so, use its high/low values.  Else, default to
		    ** some initial values.
		    */
		    eof_active2 = TRUE;
		    if (eof_active) {
			break;
		    }
		    strcpy(line2, line);
		    if ((data_file_open = init_data(thread_name(line)))) {
			last2 = total.last;
			first2 = total.first;
			ch2 = 'y';
		    } else {
			total.first = first2 = first;
			if (add_new && (!hierarchy_list
			  || ngmatch(hierarchy_list, line) == NG_MATCH)) {
			    total.last = last2 = first - 1;
			    ch2 = (ch == '=' ? 'x' : ch);
			    touch_thread = TRUE;
			    added_groups += (ch2 != 'x');
			} else {
			    total.last = last2 = last;
			    ch2 = (ch == '=' ? 'X' : toupper(ch));
			}
		    }
		    data_file_open++;		/* (1 == empty, 2 == open) */
#ifdef ACTIVE_TIMES
		    /* Found a new group -- see if we need to log it. */
		    if (acttimes_flag) {
			if (!fp_tmp && !(fp_tmp = fopen(ACTIVE_TIMES, "a"))) {
			    log_entry("unable to append to %s.\n",ACTIVE_TIMES);
			    acttimes_flag = FALSE;
			} else {
			    fprintf(fp_tmp, "%s %ld mthreads@%s\n", line,
				old_groups ? 30010440L : time(Null(time_t)),
				OURDOMAIN);
			}
		    }
#endif
		    break;
		}
		if (!(cp2 = index(line2, ' '))) {
		    log_entry("active2 line has no space: %s\n", line2);
		    continue;
		}
		*cp2 = '\0';
		if (sscanf(cp2+1,"%ld %ld %c",&last2,&first2,&ch2) != 3) {
		    log_entry("active2 digits corrupted: %s %s\n",
			line2, cp2+1);
		    continue;
		}
		/* Check if we're still in-sync */
		if (eof_active || strEQ(line, line2)) {
		    break;
		}
		/* Nope, we've got to go looking for this line somewhere
		** down in the file.  Save each non-matching line in memory
		** as we go.
		*/
		pline = (ACTIVE_LINE*)safemalloc(sizeof (ACTIVE_LINE));
		pline->name = savestr(line2);
		pline->last = last2;
		pline->first = first2;
		pline->type = ch2;
		pline->link = Nullact;
		if (!last_line) {
		    line_root = pline;
		} else {
		    last_line->link = pline;
		}
		*line2 = '\0';
		last_line = pline;
	    }/* for */
	    if (eof_active && eof_active2) {
		break;
	    }
	}/* if !pline */
	if (eof_active) {
	    strcpy(line, line2);
	    if (truncate_len < 0) {
		truncate_len = ftell(fp_active2w);
	    }
	}
	if (rebuild) {
	    unlink(thread_name(line));
	}
	update_successful = FALSE;
	if (hierarchy_list && !add_new) {
	    switch ((action = ngmatch(hierarchy_list, line))) {
	    case NG_MATCH:			/* if unthreaded, add it */
		if (ch2 < 'a' && ch != 'x' && ch != '=') {
		    total.last = last2 = first2 - 1;
		    touch_thread = TRUE;
		    added_groups++;
		}
		break;
	    case NG_SKIP:			/* if threaded, remove it */
		if (ch2 >= 'a') {
		    unlink(thread_name(line));
		    removed_groups++;
		}
		break;
	    }
	} else {
	    action = (ch2 < 'a' ? NG_SKIP : NG_MATCH);
	}
	if (action == NG_DEFAULT || (debug && action == NG_SKIP)) {
	    dont_read_data(data_file_open);	/* skip silently */
	    if (touch_thread) {
		(void) write_data(thread_name(line));
	    }
	} else if (ch == 'x' || ch == '=') {
	    if (!daemon_delay) {		/* skip 'x'ed groups */
		putchar('x');
	    }
	    ch = (action == NG_SKIP ? 'X' : 'x');
	    if ((ch2 >= 'a' && ch2 != 'x') || force_flag) {
		/* Remove thread file if group is newly 'x'ed out */
		unlink(thread_name(line));
	    }
	    update_successful = TRUE;
	    dont_read_data(data_file_open);
	} else if (action == NG_SKIP) {	/* skip excluded groups */
	    if (!daemon_delay) {
		putchar('X');
	    }
	    ch = toupper(ch);
	    if (force_flag) {
		unlink(thread_name(line));
	    }
	    update_successful = TRUE;
	    dont_read_data(data_file_open);
	} else if (no_processing) {
	    if (!daemon_delay) {
		putchar(',');
	    }
	    ch2 = ch;
	    dont_read_data(data_file_open);
	    if (touch_thread) {
		(void) write_data(thread_name(line));
	    }
	} else if (!force_flag && !extra_expire && !rebuild
	 && first == first2 && last == last2) {
	    /* We're up-to-date here.  Skip it. */
	    if (!daemon_delay) {
		putchar('.');
	    }
	    update_successful = TRUE;
	    dont_read_data(data_file_open);
	    if (touch_thread) {
		(void) write_data(thread_name(line));
	    }
	} else {
	    /* Looks like we need to process something. */
#ifdef SERVER
	    if (server_failure == 1) {
		if (!open_server()) {
		    log_entry("failed to re-open server for this pass.\n");
		    server_failure = 2;
		} else {
		    server_failure = 0;
		}
	    }
	    if (!server_failure) {
		sprintf(buf, "GROUP %s", line);
		put_server(buf);		/* go to next group */
		if (get_server(buf, sizeof buf) < 0
		 || *buf != CHAR_OK) {
		    log_error("NNTP failure -- %s.\n", buf);
		    if (strnNE(buf, "400", 3)) {
			close_server();
		    }
		    server_failure = 1;
		}
	    }
	    if (server_failure) {
#else
	    strcpy(cp = buf, line2);
	    while ((cp = index(cp, '.'))) {
		*cp = '/';
	    }
	    filename = file_exp(buf);		/* relative to spool dir */
	    if (chdir(filename) < 0) {
		if (errno != ENOENT) {
		    log_entry("Unable to chdir to `%s'.\n", filename);
		}
#endif
		if (!daemon_delay) {
		    putchar('*');
		}
		dont_read_data(data_file_open);
	    } else {
		filename = thread_name(line);
		/* Try to open the data file only if we didn't try it
		** in the name matching code above.
		*/
		if (!data_file_open--) {	/* (0 == haven't tried yet) */
		    if (!(data_file_open = init_data(filename))) {
			total.last = first - 1;
			total.first = first;
		    }
		}

		if (data_file_open) {		/* (0 == empty, 1 == open) */
		    if (!read_data()) {	/* did read fail? */
			if (debug) {
			    strcpy(buf, filename);
			    cp = rindex(buf, '/') + 1;
			    strcpy(cp, "bad.read");
			    rename(filename, buf);
			}
			data_file_open = init_data(Nullch);
			total.last = first - 1;
			total.first = first;
		    }
		}
		grevious_error = FALSE;
		process_articles(first, last);
		processed_groups++;
		if (!added_count && !expired_count && !touch_thread
		 && last == last2) {
		    (void) write_data(Nullch);
		    if (!daemon_delay) {
			putchar(':');
		    }
		    update_successful = TRUE;
		} else {
		    strcpy(buf, filename);
		    cp = rindex(buf, '/') + 1;
		    strcpy(cp, NEW_THREAD);	/* write data as .new */
		    if (write_data(buf) && !grevious_error) {
			rename(buf, filename);
			added_articles += added_count;
			expired_articles += expired_count;
			if (!daemon_delay) {
			    if (!total.root) {
				putchar('-');
			    } else {
				putchar('#');
			    }
			}
			update_successful = TRUE;
		    } else {
			if (debug) {
			    cp = rindex(filename, '/') + 1;
			    strcpy(cp, "bad.write");
			    rename(buf, filename);
			} else {
			    unlink(buf);	/* blow away bad write */
			    if (grevious_error) {
				unlink(filename); /* blow away the .thread, */
				(void) init_data(Nullch); /* zero totals & */
				(void) write_data(filename); /* write it null */
			    }
			}
			if (!daemon_delay) {
			    putchar('!');
			}
		    }/* if */
		}/* if */
	    }/* if */
	}/* if */
	/* Finally, update the active2 entry for this newsgroup. */
	if (!eof_active) {
	    if (update_successful) {
		fprintf(fp_active2w, fmt_active2, line, last, first, ch);
	    } else {
		fprintf(fp_active2w, fmt_active2, line, last2, first2, ch2);
	    }
	}
	*line2 = '\0';
	/* If we're not out of sync, keep active2 file flushed. */
	if (!line_root) {
	    fflush(fp_active2w);
	}
#ifdef CHECKLOAD
	checkload();
#endif
    }/* for */

#ifdef SERVER
    if (!server_failure) {
	close_server();
    }
#endif
    fclose(fp_active);
    fclose(fp_active2);

    if (truncate_len >= 0) {
#ifdef FTRUNCATE
	if (ftruncate(fileno(fp_active2w), truncate_len) == -1)
#else
#ifdef MVTRUNC
	if (mvtrunc(file_exp(ACTIVE2), truncate_len) == -1)
#else
	if (chsize(fileno(fp_active2w), truncate_len) == -1)
#endif
#endif
	{
	    log_entry("Unable to truncate the active2 file.\n");
	}
	truncate_len = -1;
    }
    fclose(fp_active2w);
    fp_active2w = Nullfp;

    if (fp_tmp) {
	fclose(fp_tmp);
    }
    log_stats();
    processed_groups = added_groups = removed_groups = 0;
    added_articles = expired_articles = 0;

    extra_expire = FALSE;
    rebuild = FALSE;

    mt_unlock(PASS_LOCK);		/* remove single-pass lock */

    return TRUE;
}

#ifdef SERVER
int
open_server()
{
    switch (server_init(server)) {
    case OK_NOPOST:
    case OK_CANPOST:
	return 1;
    case ERR_ACCESS:
	log_entry("Server %s rejected connection -- quitting.\n", server);
	wrap_it_up(1);
    default:
	log_entry("Couldn't connect with server %s.\n", server);
	return 0;
    }
}
#endif

/*
** ngmatch - newsgroup name matching
**
** returns NG_MATCH for a positive patch, NG_SKIP for a negative match,
** and NG_DEFAULT if the group doesn't match at all.
**
** "all" in a pattern is a wildcard that matches exactly one word;
** it does not cross "." (NGDELIM) delimiters.
**
** This matching code was borrowed from C news.
*/

#define ALL "all"			/* word wildcard */

#define NGNEG '!'
#define NGSEP ','
#define NGDELIM '.'

int
ngmatch(ngpat, grp)
char *ngpat, *grp;
{
    register char *patp;		/* point at current pattern */
    register char *patcomma;
    register int depth;
    register int faildeepest = 0, hitdeepest = 0;	/* in case no match */
    register bool negation;

    for (patp = ngpat; patp != Nullch; patp = patcomma) {
	negation = FALSE;
	patcomma = index(patp, NGSEP);
	if (patcomma != Nullch) {
	    *patcomma = '\0';		/* will be restored below */
	}
	if (*patp == NGNEG) {
	    ++patp;
	    negation = TRUE;
	}
	depth = onepatmatch(patp, grp); /* try 1 pattern, 1 group */
	if (patcomma != Nullch) {
	    *patcomma++ = NGSEP;	/* point after the comma */
	}
	if (depth == 0) {		/* mis-match */
	    ;				/* ignore it */
	} else if (negation) {
	    /* record depth of deepest negated matched word */
	    if (depth > faildeepest) {
		faildeepest = depth;
	    }
	} else {
	    /* record depth of deepest plain matched word */
	    if (depth > hitdeepest) {
		hitdeepest = depth;
	    }
	}
    }
    if (hitdeepest > faildeepest) {
	return NG_MATCH;
    } else if (faildeepest) {
	return NG_SKIP;
    } else {
	return NG_DEFAULT;
    }
}

/*
** Match a pattern against a group by looking at each word of pattern in turn.
**
** On a match, return the depth (roughly, ordinal number * k) of the rightmost
** word that matches.  If group runs out first, the match fails; if pattern
** runs out first, it succeeds.  On a failure, return zero.
*/
int
onepatmatch(patp, grp)
char *patp, *grp;
{
    register char *rpatwd;		/* used by word match (inner loop) */
    register char *patdot, *grdot;	/* point at dots after words */
    register char *patwd, *grwd;	/* point at current words */
    register int depth = 0;

    for (patwd = patp, grwd = grp;
	 patwd != Nullch && grwd != Nullch;
	 patwd = patdot, grwd = grdot
    ) {
	register bool match = FALSE;
	register int incr = 20;

	/* null-terminate words */
	patdot = index(patwd, NGDELIM);
	if (patdot != Nullch) {
	    *patdot = '\0';		/* will be restored below */
	}
	grdot = index(grwd, NGDELIM);
	if (grdot != Nullch) {
	    *grdot = '\0';		/* will be restored below */
	}
	/*
	 * Match one word of pattern with one word of group.
	 * A pattern word of "all" matches any group word,
	 * but isn't worth as much.
	 */
#ifdef FAST_STRCMP
	match = STREQ(patwd, grwd);
	if (!match && STREQ(patwd, ALL)) {
	    match = TRUE;
	    --incr;
	}
#else
	for (rpatwd = patwd; *rpatwd == *grwd++;) {
	    if (*rpatwd++ == '\0') {
		match = TRUE;		/* literal match */
		break;
	    }
	}
	if (!match) {
	    /* ugly special case match for "all" */
	    rpatwd = patwd;
	    if (*rpatwd++ == 'a' && *rpatwd++ == 'l'
	     && *rpatwd++ == 'l' && *rpatwd   == '\0') {
		match = TRUE;
		 --incr;
	    }
	}
#endif				/* FAST_STRCMP */

	if (patdot != Nullch) {
	    *patdot++ = NGDELIM;	/* point after the dot */
	}
	if (grdot != Nullch) {
	    *grdot++ = NGDELIM;
	}
	if (!match) {
	    depth = 0;		/* words differed - mismatch */
	    break;
	}
	depth += incr;
    }
    /* if group name ran out before pattern, then match fails */
    if (grwd == Nullch && patwd != Nullch) {
	depth = 0;
    }
    return depth;
}

/* Put our startup options into the log file.
*/
void
log_startup()
{
    char tmpbuf[256];

    strcpy(tmpbuf, "Started mthreads");
    if (cron_locking) {
	strcat(tmpbuf, " -c");
    }
    if (debug) {
	strcat(tmpbuf, " -D");
    }
    if (force_flag) {
	strcat(tmpbuf, " -f");
    }
    if (daemon_delay) {
	sprintf(tmpbuf + strlen(tmpbuf), " -d%d", daemon_delay / 60);
	if (expire_time) {
	    struct tm *ts;

	    ts = localtime(&expire_time);
	    sprintf(tmpbuf + strlen(tmpbuf), " -e%02d%02d",ts->tm_hour,ts->tm_min);
	}
    } else if (expire_time) {
	strcat(tmpbuf, " -e");
    }
    if (slow_down) {
	sprintf(tmpbuf + strlen(tmpbuf), " -s%d", slow_down);
    }
    if (no_processing) {
	strcat(tmpbuf, " -n");
    }
    if (log_verbosity) {
	sprintf(tmpbuf + strlen(tmpbuf), " -v%d", log_verbosity);
    }
    if (zap_thread) {
	strcat(tmpbuf, " -z");
    }
    if (add_new) {
	if (hierarchy_list) {
	    sprintf(tmpbuf + strlen(tmpbuf), " -a %s", hierarchy_list);
	} else {
	    strcat(tmpbuf, " -a all");
	}
    } else if (hierarchy_list) {
	sprintf(tmpbuf + strlen(tmpbuf), " %s (only)", hierarchy_list);
    }
    log_entry("%s\n", tmpbuf);
}

/* Put our statistics into the log file.
*/
void
log_stats()
{
    sprintf(line, "Processed %d group%s:  added %d article%s, expired %d.\n",
	processed_groups, processed_groups == 1 ? nullstr : "s",
	added_articles, added_articles == 1 ? nullstr : "s",
	expired_articles);

    log_entry(line);

    if (!daemon_delay) {
	putchar('\n');
	fputs(line, stdout);
    }
    if (added_groups) {
	sprintf(line, "Turned %d group%s on.\n", added_groups,
		added_groups == 1 ? nullstr : "s");
	log_entry(line);
	if (!daemon_delay) {
	    fputs(line, stdout);
	}
    }
    if (removed_groups) {
	sprintf(line, "Turned %d group%s off.\n", removed_groups,
		removed_groups == 1 ? nullstr : "s");
	log_entry(line);
	if (!daemon_delay) {
	    fputs(line, stdout);
	}
    }
}
/* Generate a log entry with timestamp.
*/
/*VARARGS1*/
void
log_entry(fmt, arg1, arg2, arg3)
char *fmt;
long arg1, arg2, arg3;
{
#ifndef USESYSLOG
    time_t now;
    char *ctime();
#endif

    if (initializing) {
	fprintf(stderr, fmt, arg1, arg2, arg3);
	return;
    }

#ifndef USESYSLOG
    (void) time(&now);
    fprintf(fp_log, "%.12s%c", ctime(&now)+4, daemon_delay ? ' ' : '+');
    fprintf(fp_log, fmt, arg1, arg2, arg3);
    fflush(fp_log);
#else
    syslog(LOG_INFO, fmt, arg1, arg2, arg3);
#endif
}

/* Generate a log entry, with 'E'rror flagging (non-daemon mode), time-stamp,
** and newsgroup name.
*/
/*VARARGS1*/
void
log_error(fmt, arg1, arg2, arg3)
char *fmt;
long arg1;
long arg2;
long arg3;
{
    char fmtbuf[256];

    sprintf(fmtbuf, "%s: %s", line, fmt);
#ifndef USESYSLOG
    log_entry(fmtbuf, arg1, arg2, arg3);
#else
    syslog(LOG_NOTICE, fmtbuf, arg1, arg2, arg3);
#endif
    if (*fmt == '*') {
	grevious_error = TRUE;
	if (!daemon_delay) {
	    putchar('E');
	}
    }
    else {
	if (!daemon_delay) {
	    putchar('e');
	}
    }
}

#ifdef MYCHSIZE
	/* code courtesy of William Kucharski */

int
chsize(fd, length)
int fd;			/* file descriptor */
off_t length;		/* length to set file to */
{
    extern long lseek();
    struct flock fl;

    if (fstat(fd, &filestat) < 0) {
	return -1;
    }
    if (filestat.st_size < length) {	/* extend file length */
	/* Write a 0 byte at the end. */
	if (lseek(fd, length - 1, 0) < 0
	 || write(fd, "", 1) != 1) {
	    return -1;
	}
    } else {
	/* Truncate file at length. */
	fl.l_whence = 0;
	fl.l_len = 0;
	fl.l_start = length;
	fl.l_type = F_WRLCK;		/* write lock on file space */

	/*
	** This relies on the UNDOCUMENTED F_FREESP argument to
	** fcntl(2), which truncates the file so that it ends at the
	** position indicated by fl.l_start.
	**
	** Will minor miracles never cease?
	*/
	if (fcntl(fd, F_FREESP, &fl) < 0) {
	    return -1;
	}
    }
    return 0;
}
#endif

#ifdef MVTRUNC
int
mvtrunc(filename, truncate_len)
char *filename;
long truncate_len;
{
    FILE *fp_in, *fp_out;

    sprintf(line, "%s.new", filename);
    if ((fp_out = fopen(line, "w")) == Nullfp) {
	log_entry("Tried to create active2.new.\n");
	return -1;
    }
    if ((fp_in = fopen(filename, "r")) == Nullfp) {
	fclose(fp_out);
	unlink(line);
	log_entry("Tried to re-open the active2 file.\n");
	return -1;
    }
    while (ftell(fp_out) < truncate_len) {
	if (!fgets(buf, sizeof buf, fp_in)) {
	    break;
	}
	fputs(buf, fp_out);
    }
    sprintf(buf, "%s.old", filename);
    rename(filename, buf);
    rename(line, filename);
    fclose(fp_in);
    fclose(fp_out);

    return 0;
}
#endif

#ifndef RENAME
int
rename(old, new)
char	*old, *new;
{
    struct stat st;

    if (stat(old, &st) == -1) {
	return -1;
    }
    if (unlink(new) == -1 && errno != ENOENT) {
	return -1;
    }
    if (link(old, new) == -1) {
	return -1;
    }
    if (unlink(old) == -1) {
	int e = errno;
	(void) unlink(new);
	errno = e;
	return -1;
    }
    return 0;
}
#endif /*RENAME*/

#ifdef CHECKLOAD

#define FREQCHECK	300
#define SLPLOAD		300
#define UPTIME		"/usr/ucb/uptime"
#define TOOHIGH		5

checkload()
{
    static long lastcheck = 0;
    long time(), i;
    FILE *pp;
    char buf[BUFSIZ];
    register char *cp;
    char *strrchr();

    i = time(Null(time_t*));
    if ((i - lastcheck) < FREQCHECK) {
	return;
    }
    lastcheck = i;

again:
    if ((pp = popen(UPTIME, "r")) == NULL) {
	return;
    }
    if (fgets(buf, BUFSIZ, pp) == NULL) {
	pclose(pp);
	return;
    }
    pclose(pp);
    if ((cp = strrchr(buf, ':')) == NULL) {
	return;
    }
    if (atoi(cp + 2) >= TOOHIGH) {
	sleep(SLPLOAD);
	goto again;
    } else {
	return;
    }
}
#endif
