/*
 * xrwho 1.0
 *
 * By Andrew Herbert                and Robin Humble
 *   <andrewh@cs.monash.edu.au>        <rjh@pixel.maths.monash.edu.au>
 *   <andrew@werple.apana.org.au>
 *
 * Jan '93.
 *
 * build with: gcc -O -s -o xrwho xrwho.c -lXaw -lXmu -lXt -lX11
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <utmp.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include <protocols/rwhod.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Command.h>	
#include <X11/Xaw/Label.h>	
#include <X11/Xaw/Box.h>	
#include <X11/Xaw/Cardinals.h>	
#include <X11/Xaw/Viewport.h>	

#ifdef sun
extern char *sys_errlist[];
#define strerror(e) (sys_errlist[e])
#endif

#ifdef __386BSD__
#define RWHODIR		"/var/rwho"
#else
#define RWHODIR		"/usr/spool/rwho"
#endif
#define MaxSnoops	32	/* maximum snoop users */
#define MaxUtmpEntries	64	/* maximum users for localhost */
#define InsensitiveIdleTime	300	/* one minute max idle time*/
#define OldUserIdleTime	3600	/* one hour user idle time max unless -a specified */
#define HostDeadTime	300	/* time before a host is considered dead */
#define RefreshRate	60000	/* refresh every minute (rate in ms) */

#define MIN(x, y)  ((x)<(y)?(x):(y))

#ifdef Debug
#define Die abort()
#else
#define Die exit(1)
#endif

#ifndef UTMP_FILE
#ifdef _PATH_UTMP
#define UTMP_FILE _PATH_UTMP
#else
#define UTMP_FILE "/etc/utmp"
#endif /* _PATH_UTMP */
#endif /* UTMP_FILE */

#ifdef R3_INTRINSICS
typedef void*	XtPointer;
extern Widget XtAppInitialize();
#endif

extern int gethostname(char *, int);

typedef enum { local, rwho } userinfo_source_t;

/* per-user information */
typedef struct user_struct {
    char name[9];	/* username */
    int	idle;		/* minimum of all the idles for this user */
    int still_here;	/* flag to say the user is still logged in */
    Widget label;	/* the label widget for this user */
    struct user_struct *next;	/* next user */
} user_t;

/* per-host information */
typedef struct {
    Widget box;		/* box widget to contain the labels for this host */
    char host_name[32];
    userinfo_source_t host_file_type;
    char host_filename[256];
    Widget title;	/* the label widget for this host */
    user_t *head;	/* pointer to userlist on this host */
} userinfo_t;

/* temp filename info */
typedef struct host_filestruct {
    char name[256];
    struct host_filestruct *next, *prev;
} host_t;

String fallbacks[] = { 
    "*Label*borderWidth:	0",	/* no border around host or usernames */
    "*vSpace:			2",	/* pack usernames fairly tightly */
    "*host.foreground:		blue",	/* hostname colour */
    "*snoop.foreground:		red",	/* snoop user colour */
    "*font:			-*-helvetica-medium-r-*-*-*-120-*-*-*-*-*-*",
    "*host.font:		-*-helvetica-medium-o-*-*-*-120-*-*-*-*-*-*",
					/* smallish font */
    "*Viewport*allowVert:	True",
    NULL
};

static XrmOptionDescRec options[] = {
    {"-hostfg",	    "*host.foreground",	    XrmoptionSepArg, NULL},
    {"-hostbg",	    "*host.background",	    XrmoptionSepArg, NULL},
    {"-hostfont",   "*host.font",	    XrmoptionSepArg, NULL},
    {"-font",	    "*font",		    XrmoptionSepArg, NULL},
    {"-highlight",  "*snoop.foreground",    XrmoptionSepArg, NULL},
};

Widget	toplevel, box;
int	no_of_hosts;	/* number of remote hosts being monitored */
int	list_long, show_all_hosts, empty_boxes, num_snoops, ring_bell;
char	snoop_user[MaxSnoops][9];
userinfo_t *host_userinfo;

void sort(host_t *);
void init_rwho(Widget);
void process_rwho(void);
void refresh_rwho(XtPointer, XtIntervalId *);
int search_for_user(userinfo_t *, char *, user_t **);
int read_utmp(struct utmp [], int);
int read_rwho(char *, struct whod *);
void remove_tail(char *string);
void syntax(XtAppContext, char *);
void main(int, char **);

void main(int argc, char **argv)
{
    int i;
    XtAppContext app_con;
    Widget viewport;

    toplevel = XtAppInitialize(&app_con, "Xrwho", options, XtNumber(options),
       (Cardinal *)&argc, argv, fallbacks, NULL, ZERO);

#ifdef Debug
/* don't work too well boss:
XtAppSetErrorHandler(app_con, abort);
*/
#endif

    list_long = 0;
    show_all_hosts = 0;
    empty_boxes = 0;
    num_snoops = 0;
    ring_bell = 0;

    /*
     * parse optional arguments
     */
    for (i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-a"))
	    list_long = 1;
	else if (!strcmp(argv[i], "-l"))
	    show_all_hosts = 1;
	else if (!strcmp(argv[i], "-b"))
	    empty_boxes = 1;
	else if (!strcmp(argv[i], "-bell"))
	    ring_bell = 1;
	else if (!strcmp(argv[i], "-snoop")) {
	    char *name;

            while (i+1 < argc && argv[i+1][0] != '-') {
		name = strtok(argv[++i], ",");
		do {
		    strncpy(snoop_user[num_snoops], name, 8);
		    snoop_user[num_snoops][8] = 0;	/* 8 chars max for usernames */
		    num_snoops++;
		} while ((name = strtok(NULL, ",")) && (num_snoops < MaxSnoops));
	    }
	}
	else
	    syntax(app_con, argv[0]);
    }

    viewport = XtCreateManagedWidget("viewport", viewportWidgetClass, toplevel, NULL, ZERO);
    box = XtCreateManagedWidget("box", boxWidgetClass, viewport, NULL, ZERO);

    init_rwho(box);	/* initialise the rwho stuff */
    process_rwho();	/* create label widgets for all current users */

    XtRealizeWidget(toplevel);
    XtAppAddTimeOut(XtWidgetToApplicationContext(toplevel), RefreshRate,
	refresh_rwho, NULL);

    XtAppMainLoop(app_con);
} /* end of main() */


void init_rwho(Widget top_box)
{
    DIR		*dir;
    struct dirent *dirent;
    int		i, host_in_whod;
    char hostname[64], filename[256];
    Arg		args[1];
    userinfo_t	*hui;
    host_t *head, *file, *tmp;


    if (gethostname(hostname, 64)) {
	/* no hostname, downer */
        perror("xrwho - hostname()");
	exit(1);
    }
    remove_tail(hostname);

    /* move to rwhod spool area so we can read files in later on */
    if (chdir(RWHODIR)) {
	char *msg;

	if (errno == EACCES)
	    /* the directory exists, but we can't read it */
	    msg = "ho hum";
	else
	    msg = "check the RWHODIR #define";

	fprintf(stderr, "xrwho - can't cd to %s (%s) - %s\n",
	    RWHODIR, strerror(errno), msg);

	if (errno != EACCES)
	    exit(1);
    }

    dir = opendir(".");
    if (!dir) {
#ifdef sun
	fprintf(stderr, "xrwho: can't open %s\n", RWHODIR);
#else
	fprintf(stderr, "xrwho: can't open %s (%s)\n", RWHODIR, strerror(errno));
#endif
	exit(1);
    }

    i = 0;
    host_in_whod = 0;

    /* init linked list for filenames */
    head = (host_t *)NULL;

    while (dirent = readdir(dir))
	if (!strncmp(dirent->d_name, "whod.", 5)) {
	    struct stat	stat_info;
	    int		stat_failed;

#ifdef Debug
printf("host rwho file %d: %s\n", i, file->name);
#endif
	    strcpy(filename, dirent->d_name + 5);
	    remove_tail(filename);

	    /* check whether we can read it, etc */
	    stat_failed = stat(dirent->d_name, &stat_info);

	    /* search for hostname in whod files, and check whether the file
	       has been updated in the past 12 hours - otherwise indicative
	       of a dead entry for the host we're running on */
	    if (!strcmp(hostname, filename)) {
		if (stat_failed && stat_info.st_mtime > (time(NULL) - 60*60*12))
		    host_in_whod = 1;	/* rwho file is OK */
		else
		    continue;		/* too old to be valid, so ignore */
	    }
	    if (stat_failed)
		continue;

	    /* malloc space for whod filename */
	    if (head == (host_t *)NULL) {
		head = (host_t *)malloc(sizeof(host_t));
		if (head == NULL) {
		    fprintf(stderr, "xrwho: can't malloc\n");
		    Die;
		}
		file = head;
		file->prev = (host_t *)NULL;
		file->next = (host_t *)NULL;
	    }
	    else {
		file->next = (host_t *)malloc(sizeof(host_t));
		if (file->next == NULL) {
		    fprintf(stderr, "xrwho: can't malloc\n");
		    Die;
		}
		file->next->prev = file;
		file->next->next = (host_t *)NULL;
		file = file->next;
	    }

	    /* read filename */
	    strcpy(file->name, dirent->d_name);

	    i++;
	}
    closedir(dir);
    no_of_hosts = i;

    /*
     * sort hostnames into alphabetical order
     */
    if (no_of_hosts)
	sort(head);

    /*
     * if host machine not in whod list then use utmp instead
     */
    if (!host_in_whod)
	no_of_hosts++;

#ifdef Debug
if (!host_in_whod)
    printf("No rwho file for localhost - using utmp instead\n");
#endif

    /*
     * allocate and zero the userinfo array
     * assume number of hosts isn't going to change that often
     */
    host_userinfo = (userinfo_t *)malloc(no_of_hosts*sizeof(userinfo_t));
    if (host_userinfo == NULL) {
	fprintf(stderr, "xrwho: can't malloc\n");
	Die;
    }

    /*
     * create enclosing boxes for each host, and set hostnames as labels
     */
    i = 0;
    for (file = head; file != (host_t *)NULL; file = file->next) {

	hui = &(host_userinfo[i]);
	hui->host_file_type = rwho;
	hui->head = (user_t *)NULL;

	/* set the host file name */
	strcpy(hui->host_filename, file->name);

	/* set the hostname, removing trailing dots */
	strcpy(hui->host_name, hui->host_filename + 5);
	remove_tail(hui->host_name);

	hui->box = XtCreateManagedWidget(hui->host_name,
	    boxWidgetClass, top_box, NULL, ZERO);

	hui->title = XtCreateManagedWidget("host",
	    labelWidgetClass, hui->box, NULL, ZERO);
	XtSetArg(args[0], XtNlabel, hui->host_name);
	XtSetValues(hui->title, args, ONE);

	i++;
    }

    /*
     * add a widget for localhost if it's not in the whod files
     */
    if (!host_in_whod) {
	hui = &(host_userinfo[no_of_hosts - 1]);
	hui->host_file_type = local;
	hui->head = (user_t *)NULL;

	strcpy(hui->host_name, hostname);

	hui->box = XtCreateManagedWidget(hui->host_name,
	    boxWidgetClass, top_box, NULL, ZERO);

	hui->title = XtCreateManagedWidget("host",
	    labelWidgetClass, hui->box, NULL, ZERO);
	XtSetArg(args[0], XtNlabel, hui->host_name);
	XtSetValues(hui->title, args, ONE);
    }

    /* free tmp filename info */
    file = head;
    while (file != (host_t *)NULL) {
	tmp = file->next;
	free(file);
	file = tmp;
    }
} /* end of init_rwho() */


/*
 * Remove trailing part of a string of the form "a.b.c.d", leaving just "a"
 */

void remove_tail(char *string)
{
    if (string = strchr(string, '.'))
	*string = '\0';
}


/*
 * search_for_user
 *	Scans a host's userlist for a particular name, returning the
 *	array index if found, or -1 if not.
 */

int search_for_user(userinfo_t *hui, char *name, user_t **found)
{
    user_t *i;

    for (i = hui->head; i != (user_t *)NULL; i = i->next)
	if (!strcmp(name, i->name)) {
	    *found = i;
	    return 0;
	}

    *found = (user_t *)NULL;
    return -1;
}


/*
 * sort
 *     sort filenames into alphabetical order by straight insertion
 */
void sort(host_t *head) {
    host_t *i, *j;
    char a[256];

    for (j = head->next; j != (host_t *)NULL; j = j->next) {

	strcpy(a, j->name);

	i = j->prev;
	while ((i->prev != (host_t *)NULL) && (strcmp(i->name, a) > 0)) {
	    strcpy(i->next->name, i->name);
	    i = i->prev;
	}

	if ((i == head) && (strcmp(i->name, a) > 0)) {
		strcpy(i->next->name, i->name);
		strcpy(i->name, a);
	}
	else
	    strcpy(i->next->name, a);
    }

#ifdef Debug
printf("\nhosts in order:\n");
for (j = head; j != (host_t *)NULL; j = j->next)
    printf("  %s", j->name);
printf("\n");
#endif

}


/*
 * read_utmp
 *	Read in a utmp file; returns the number of users in the file.
 */

int read_utmp(struct utmp utmp[], int max_entries)
{
    int fd, bytes;

    fd = open(UTMP_FILE, O_RDONLY);
    if (fd < 0) {
#ifdef Debug
	fprintf(stderr, "xrwho: can't open %s\n", UTMP_FILE);
#endif
	return 0;
    }
    bytes = read(fd, utmp, max_entries*sizeof(struct utmp));
    close(fd);
    return bytes/sizeof(struct utmp);
}


/*
 * read_rwho
 *	Read in a whod file; returns the number of users in the file.
 */

int read_rwho(char *filename, struct whod *who)
{
    int fd, bytes;

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
#ifdef Debug
	fprintf(stderr, "xrwho: can't open %s/%s\n", RWHODIR, filename);
#endif
	return 0;
    }
    bytes = read(fd, who, sizeof(struct whod));
    close(fd);

    if ((time(NULL) - who->wd_recvtime) > HostDeadTime) {
	/* we haven't heard from host for too long so ignore it */
	return 0;
    }

    return (bytes - 60)/sizeof(struct whoent);	/* number of whoent records */
}


void process_rwho(void)
{
    int		i, n;
    struct whod	who;
    struct utmp	utmp[MaxUtmpEntries];
    Arg		args[1];
    int		j;
    user_t	*k, *prev;
    time_t	time_now = time(NULL);

    /* add labels to the box, one per remote user */
    for (i = 0; i < no_of_hosts; i++) {
	userinfo_t	*hui = &(host_userinfo[i]);

	switch (hui->host_file_type) {
	    case local: n = read_utmp(utmp, MaxUtmpEntries); break;
	    case rwho:	n = read_rwho(hui->host_filename, &who); break;
	}

	/*
	 * set all idle times to infinity, and still_here to false
	 * so that the following update works correctly
	 */
	for (k = hui->head; k != (user_t *)NULL; k = k->next) {
	    k->idle = INT_MAX;
	    k->still_here = 0;
	}

	/*
	 * scan through the utmp-style array for the host, updating idle times
	 * and checking whether users are still logged on
	 */
	for (j = 0; j < n; j++) {
	    char *name;
	    int	idle;

	    switch (hui->host_file_type) {
		case local: {
		    char	tmp[64];
		    struct stat	stat_info;
#ifdef USER_PROCESS
		    /* SYSV-style utmp */
		    if (utmp[j].ut_type != USER_PROCESS)
			continue;	/* ignore everything else */
		    name = utmp[j].ut_user;
#else
		    if (!utmp[j].ut_name)
			continue;
		    name = utmp[j].ut_name;
#endif
		    /* check how long it's been since the user last accessed
		       their tty */
		    strcpy(tmp, "/dev/");
		    if (!stat(strcat(tmp, utmp[j].ut_line), &stat_info))
			idle = time_now - stat_info.st_atime;
		    else {
#ifdef Debug
			fprintf(stderr, "can't stat %s\n", tmp);
#endif
			idle = 0;	/* ignore */
		    }
		    break;
		}
		case rwho:
		    name = who.wd_we[j].we_utmp.out_name;
		    idle = who.wd_we[j].we_idle;
		    break;
	    }

	    /* handle 8-char usernames */
	    name[8] = 0;

#ifdef Debug
printf("checking %s in %s (idle %d seconds)\n", name,
    hui->host_filename, idle);
#endif

	    /* check host_userinfo for this name */
	    if (search_for_user(hui, name, &k) != -1) {
		/* matched existing user */

#ifdef Debug
printf("user %s matched (idle %d)\n", k->name, k->idle);
#endif
		k->idle = MIN(k->idle, idle);
		k->still_here++;
	    }
	    else {
		/* new user */

		/* check whether idle is too long */
		if (list_long || idle < OldUserIdleTime) {
		    int m, snoop;

		    if (hui->head == (user_t *)NULL) {
			/* this is 1st user for this host */
			hui->head = (user_t *)malloc(sizeof(user_t));
			if (hui->head == NULL) {
			    fprintf(stderr, "xrwho: can't malloc\n");
			    Die;
			}
			k = hui->head;
			k->next = (user_t *)NULL;
		    }
		    else {
			k = (user_t *)malloc(sizeof(user_t));
			if (k == NULL) {
			    fprintf(stderr, "xrwho: can't malloc\n");
			    Die;
			}
			k->next = hui->head;
			hui->head = k;
		    }

		    /*
		     * add user to list
		     */
		    strcpy(k->name, name);
		    k->idle = idle;
		    k->still_here = 1;

		    /*
		     * see if the user is snooped upon
		     */
		    snoop = 0;
		    for (m = 0; m < num_snoops; m++) {
			if (!strcmp(snoop_user[m], k->name)) {
			    snoop++;
#ifdef Debug
printf("created snoop user %s from %s\n", snoop_user[m], hui->host_filename);
#endif
			    k->label = XtCreateManagedWidget("snoop",
				labelWidgetClass, hui->box, NULL, ZERO);
			    XtSetArg(args[0], XtNlabel, k->name);
			    XtSetValues(k->label, args, ONE);
			    if (ring_bell)
				XBell(XtDisplay(toplevel), 1);
			}
    		    }

		    if (!snoop) {
			k->label = XtCreateManagedWidget(k->name,
			    labelWidgetClass, hui->box, NULL, ZERO);
#ifdef Debug
printf("created %s from %s\n", k->name, hui->host_filename);
#endif
		    }
		}
	    }
	}

	/* check whether any of the users has logged out, or idle is too long */
	k = hui->head;
	prev = (user_t *)NULL;
	while (k != (user_t *)NULL) {
	    if (!(k->still_here) || (!list_long && (k->idle > OldUserIdleTime))) {
#ifdef Debug
printf("removed %s from %s\n", k->name, hui->host_name);
#endif
		XtDestroyWidget(k->label);

		/* unlink k from the list */
		if (prev == (user_t *)NULL) {
		    /* user is head of list */
		    hui->head = k->next;
		    free(k);
		    k = hui->head;
		}
		else {
		    prev->next = k->next;
		    free(k);
		    k = prev->next;
		}
	    }
	    else {
		prev = k;
		k = k->next;
	    }
	}

	/* set each user as insensitive or sensitive, according to idle time */
	for (k = hui->head; k != (user_t *)NULL; k = k->next) {

#ifdef Debug
printf("user %s: %s\n", k->name,
    k->idle < InsensitiveIdleTime ? "active" : "inactive");
#endif
	    XtSetArg(args[0], XtNsensitive,
		k->idle < InsensitiveIdleTime ? True : False);
	    XtSetValues(k->label, args, ONE);
	}

	/*
	 * want to hide machine title (and possibly box) if there are no users,
	 * and display it again if there are some.
	 */
	if (!show_all_hosts) {
	    Widget wid = empty_boxes ? hui->title : hui->box;

	    if (hui->head != (user_t *)NULL) {
		if (!XtIsManaged(wid)) {
		    XtManageChild(wid);
#ifdef Debug
printf("has users: adding management to %s\n", hui->host_name);
#endif
		}
	    }
	    else {
	    	if (XtIsManaged(wid)) {
		    XtUnmanageChild(wid);
#ifdef Debug
printf("no users: removing management from %s\n", hui->host_name);
#endif
		}
	    }
	}
    } /* end of host loop */

#ifdef Debug
{
    int i;

    printf("at end of loop -- status:\n");
    for (i = 0; i < no_of_hosts; i++) {
	userinfo_t	*hui = &(host_userinfo[i]);
        user_t *k;

        if (hui->head != (user_t *)NULL)
            printf("%s (%s) wid: %ld\n", hui->host_name,
		XtIsManaged(hui->title) ? "managed" : "not managed", hui->title);

        for (k = hui->head; k != (user_t *)NULL; k = k->next) {
	   printf("   %s - idle %d %s (%d) wid: %ld\n", k->name, k->idle,
		k->still_here > 0 ? "here" : "not here", k->still_here, k->label);
        }
    }
}
#endif

}


/*
 * refresh_rwho
 *	Refresh the rwho display after an interval has expired.
 */

void refresh_rwho(XtPointer client_data, XtIntervalId *id)
{
    /* set it up again */
    XtAppAddTimeOut(XtWidgetToApplicationContext(toplevel), RefreshRate,
	refresh_rwho, NULL);

    /* check what has changed since last time */
    process_rwho();
}


void syntax(XtAppContext app_con, char *call)
{
    XtDestroyApplicationContext(app_con);
    fprintf(stderr, "Usage: %s [-a] [-l | -b] [-snoop <user1>,<user2>,... [-bell] ]\n", call);
    fprintf(stderr, "             [-font <font>] [-hostfont <font>]\n");
    fprintf(stderr, "             [-hostfg <color>] [-hostbg <color>]\n");
    fprintf(stderr, "             [-highlight <color>]\n");
    fprintf(stderr, "           + usual X options.\n");
    fprintf(stderr, "   -a   shows users with long idle times also.\n");
    fprintf(stderr, "   -l   shows hostnames when no users are logged in.\n");
    fprintf(stderr, "   -b   shows empty boxes when no users are logged in.\n");
    fprintf(stderr, "   -snoop <user1>,<user2>,...  highlights particular users.\n");
    fprintf(stderr, "   -bell   beeps when a snooped user arrives.\n");
    fprintf(stderr, "   -highlight   sets the colour of a snooped user (default red).\n");
    exit(1);
}

