/*
 * @(#)argproc.c 1.0 89/02/01		Mark Pizzolato (mark@infopiz.uucp)	
 */

#ifndef lint
char argproc_version[] = "@(#)argproc.c VMS uucp Version infopiz-1.0";
#endif

#include "includes.h"		/* System include files, system dependent */


/*
 * getredirection() is intended to aid in porting C programs
 * to VMS (Vax-11 C) which does not support '>' and '<'
 * I/O redirection, along with a command line pipe mechanism
 * using the '|' AND background command execution '&'.
 * The piping mechanism will probably work with almost any 'filter' type
 * of program.  With suitable modification, it may useful for other
 * portability problems as well.
 *
 * Author:  Mark Pizzolato	mark@infopiz.UUCP
 */
struct list_item
    {
    struct list_item *next;
    char *value;
    };

int
getredirection(ac, av)
int		*ac;
char		***av;
/*
 * Process vms redirection arg's.  Exit if any error is seen.
 * If getredirection() processes an argument, it is erased
 * from the vector.  getredirection() returns a new argc and argv value.
 * In the event that a background command is requested (by a trailing "&"),
 * this routine creates a background subprocess, and simply exits the program.
 *
 * Warning: do not try to simplify the code for vms.  The code
 * presupposes that getredirection() is called before any data is
 * read from stdin or written to stdout.
 *
 * Normal usage is as follows:
 *
 *	main(argc, argv)
 *	int		argc;
 *    	char		*argv[];
 *	{
 *		getredirection(&argc, &argv);
 *	}
 */
{
    int			argc = *ac;	/* Argument Count	  */
    char		**argv = *av;	/* Argument Vector	  */
    char		*ap;   		/* Argument pointer	  */
    int	       		j;		/* argv[] index		  */
    extern int		errno;		/* Last vms i/o error 	  */
    int			item_count = 0;	/* Count of Items in List */
    struct list_item 	*list_head = 0;	/* First Item in List	    */
    struct list_item	*list_tail;	/* Last Item in List	    */
    char 		*in = NULL;	/* Input File Name	    */
    char 		*out = NULL;	/* Output File Name	    */
    char 		*outmode = "w";	/* Mode to Open Output File */
    int			cmargc = 0;    	/* Piped Command Arg Count  */
    char		**cmargv = NULL;/* Piped Command Arg Vector */
    stat_t		statbuf;	/* fstat buffer		    */

    /*
     * First handle the case where the last thing on the line ends with
     * a '&'.  This indicates the desire for the command to be run in a
     * subprocess, so we satisfy that desire.
     */
    ap = argv[argc-1];
    if (0 == strcmp("&", ap))
	exit(background_process(--argc, argv));
    if ('&' == ap[strlen(ap)-1])
	{
	ap[strlen(ap)-1] = '\0';
	exit(background_process(argc, argv));
	}
    /*
     * Now we handle the general redirection cases that involve '>', '>>',
     * '<', and pipes '|'.
     */
    for (j = 0; j < argc; ++j)
	{
	if (0 == strcmp("<", argv[j]))
	    {
	    if (j+1 >= argc)
		{
		errno = EINVAL;
		perror("No input file");
		exit(EXIT_ERR);
		}
	    in = argv[++j];
	    continue;
	    }
	if ('<' == *(ap = argv[j]))
	    {
	    in = 1 + ap;
	    continue;
	    }
	if (0 == strcmp(">", ap))
	    {
	    if (j+1 >= argc)
		{
		errno = EINVAL;
		perror("No output file");
		exit(EXIT_ERR);
		}
	    out = argv[++j];
	    continue;
	    }
	if ('>' == *ap)
	    {
	    if ('>' == ap[1])
		{
		outmode = "a";
		if ('\0' == ap[2])
		    out = argv[++j];
		else
		    out = 2 + ap;
		}
	    else
		out = 1 + ap;
	    continue;
	    }
	if (0 == strcmp("|", argv[j]))
	    {
	    if (j+1 >= argc)
		{
		errno = EPIPE;
		perror("No command to Pipe to");
		exit(EXIT_ERR);
		}
	    cmargc = argc-(j+1);
	    cmargv = &argv[j+1];
	    argc = j;
	    continue;
	    }
	if ('|' == *(ap = argv[j]))
	    {
	    ++argv[j];
	    cmargc = argc-j;
	    cmargv = &argv[j];
	    argc = j;
	    continue;
	    }
	expand_wild_cards(ap, &list_head, &list_tail, &item_count);
	}
    /*
     * Allocate and fill in the new argument vector, Some Unix's terminate
     * the list with an extra null pointer.
     */
    argv = *av = calloc(item_count+1, sizeof(char *));
    for (j = 0; j < item_count; ++j, list_head = list_head->next)
	argv[j] = list_head->value;
    *ac = item_count;
    if (cmargv != NULL)
	{
	char subcmd[1024];
	static char *pipe_and_fork();

	if (out != NULL)
	    {
	    errno = EINVAL;
	    perror("Invalid '|' and '>' specified");
	    exit(EXIT_ERR);
	    }
	strcpy(subcmd, cmargv[0]);
	for (j = 1; j < cmargc; ++j)
	    {
	    strcat(subcmd, " \"");
	    strcat(subcmd, cmargv[j]);
	    strcat(subcmd, "\"");
	    }
	out = pipe_and_fork(subcmd);
	outmode = "wb";
	}
	
    /* Check for input from a pipe (mailbox) */

    if(fstat(0, &statbuf) == 0){
	if(strncmp(statbuf.st_dev, "MB", 2) == 0 || 
	    strncmp(statbuf.st_dev, "_MB", 3) == 0){

	    /* Input from a pipe, reopen it in binary mode to disable	*/
	    /* carriage control processing.				*/

	    if (in != NULL){
		errno = EINVAL;
		perror("Invalid '|' and '<' specified");
		exit(EXIT_ERR);
		}
	    freopen(statbuf.st_dev, "rb", stdin);
	    }
	}
    else {
	perror("fstat failed");
	exit(EXIT_ERR);
	}
    if ((in != NULL) && (NULL == freopen(in, "r", stdin, "mbc=32", "mbf=2")))
	{
	perror(in);    	       	/* Can't find file		*/
	exit(EXIT_ERR);		/* Is a fatal error		*/
	}
    if ((out != NULL) && (NULL == freopen(out, outmode, stdout, "mbc=32", "mbf=2")))
	{	
	perror(ap);		/* Error, can't write or append	*/
	exit(EXIT_ERR);		/* Is a fatal error		*/
	}
#ifdef DEBUG
    fprintf(stderr, "Arglist:\n");
    for (j = 0; j < *ac;  ++j)
	fprintf(stderr, "argv[%d] = '%s'\n", j, argv[j]);
#endif
}

static add_item(head, tail, value, count)
struct list_item **head;
struct list_item **tail;
char *value;
int *count;
{
    if (*head == 0)
	{
	if (NULL == (*head = calloc(1, sizeof(**head))))
	    {
	    errno = ENOMEM;
	    perror("");
	    exit(EXIT_ERR);
	    }
	*tail = *head;
	}
    else
	if (NULL == ((*tail)->next = calloc(1, sizeof(**head))))
	    {
	    errno = ENOMEM;
	    perror("");
	    exit(EXIT_ERR);
	    }
	else
	    *tail = (*tail)->next;
    (*tail)->value = value;
    ++(*count);
}

static expand_wild_cards(item, head, tail, count)
char *item;
struct ltem_list **head;
struct ltem_list **tail;
int *count;
{
int expcount = 0;
int context = 0;
int status;
int status_value;
int had_version;
$DESCRIPTOR(filespec, item);
$DESCRIPTOR(defaultspec, "SYS$DISK:[]*.*;");
$DESCRIPTOR(resultspec, "");

    if (strcspn(item, "*%") == strlen(item))
	{
	add_item(head, tail, item, count);
	return;
	}
    resultspec.dsc$b_dtype = DSC$K_DTYPE_T;
    resultspec.dsc$b_class = DSC$K_CLASS_D;
    resultspec.dsc$a_pointer = NULL;
    filespec.dsc$w_length = strlen(item);
    /*
     * Only return version specs, if the caller specified a version
     */
    had_version = strchr(item, ';');
    while (1 == (1&lib$find_file(&filespec, &resultspec, &context,
    				 &defaultspec, 0, &status_value, &0)))
	{
	char *string;
	char *c;

	if (NULL == (string = calloc(1, resultspec.dsc$w_length+1)))
	    {
	    errno = ENOMEM;
	    perror("");
	    exit(EXIT_ERR);
	    }
	strncpy(string, resultspec.dsc$a_pointer, resultspec.dsc$w_length);
	string[resultspec.dsc$w_length] = '\0';
	if (NULL == had_version)
	    *((char *)strrchr(string, ';')) = '\0';
	/*
	 * Be consistent with what the C RTL has already done to the rest of
	 * the argv items and lowercase all of these names.
	 */
	for (c = string; *c; ++c)
	    if (isupper(*c))
		*c = tolower(*c);
	add_item(head, tail, string, count);
	++expcount;
	}
    if (expcount == 0)
	add_item(head, tail, item, count);
    lib$sfree1_dd(&resultspec);
    lib$find_file_end(&context);
}

static int child_st[2];	/* Event Flag set when child process completes	*/

static short child_chan;/* I/O Channel for Pipe Mailbox			*/

static exit_handler(status)
int *status;
{
short iosb[4];

    if (0 == child_st[0])
	{
#ifdef DEBUG
	fprintf(stderr, "Waiting for Child Process to Finnish . . .\n");
#endif
	fflush(stdout);	    /* Have to flush pipe for binary data to	*/
			    /* terminate properly -- <tp@mccall.com>	*/
	sys$qiow(0, child_chan, IO$_WRITEOF, iosb, 0, 0, 0, 0, 0, 0, 0, 0);
	sys$dassgn(child_chan);
	fclose(stdout);
	sys$synch(0, child_st);
	}
}

#include "syidef"	/* System Information Definitions	*/

static sig_child(chan)
int chan;
{
#ifdef DEBUG
    fprintf(stderr, "Child Completion AST\n");
#endif
    if (child_st[0] == 0)
	child_st[0] = 1;
}

static struct exit_control_block
    {
    struct exit_control_block *flink;
    int	(*exit_routine)();
    int arg_count;
    int *status_address;
    int exit_status;
    } exit_block =
    {
    0,
    exit_handler,
    1,
    &exit_block.exit_status,
    0
    };

static char *pipe_and_fork(cmd)
char *cmd;
{
    $DESCRIPTOR(cmddsc, cmd);
    static char mbxname[64];
    $DESCRIPTOR(mbxdsc, mbxname);
    short iosb[4];
    int status;
    int pid;
    struct
	{
	short dna_buflen;
	short dna_itmcod;
	char *dna_buffer;
	short *dna_retlen;
	int listend;
	} itmlst =
	{
	sizeof(mbxname),
	DVI$_DEVNAM,
	mbxname,
	&mbxdsc.dsc$w_length,
	0
	};
    int mbxsize;
    struct
	{
	short mbf_buflen;
	short mbf_itmcod;
	int *mbf_maxbuf;
	short *mbf_retlen;
	int listend;
	} syiitmlst =
	{
	sizeof(mbxsize),
	SYI$_MAXBUF,
	&mbxsize,
	0,
	0
	};

    cmddsc.dsc$w_length = strlen(cmd);
    /*
     * Get the SYSGEN parameter MAXBUF, and the smaller of it and 2048 as
     * the size of the 'pipe' mailbox.
     */
    if (1 == (1&(vaxc$errno = sys$getsyiw(0, 0, 0, &syiitmlst, iosb, 0, 0, 0))))
	vaxc$errno = iosb[0];
    if (0 == (1&vaxc$errno))
	{
 	errno = EVMSERR;
	perror("Can't get SYSGEN parameter value for MAXBUF");
	exit(EXIT_ERR);
	}
    if (mbxsize > 2048)
	mbxsize = 2048;
    if (0 == (1&(vaxc$errno = sys$crembx(0, &child_chan, mbxsize, mbxsize, 0, 0, 0))))
	{
	errno = EVMSERR;
	perror("Can't create pipe mailbox");
	exit(EXIT_ERR);
	}
    if (1 == (1&(vaxc$errno = sys$getdviw(0, child_chan, 0, &itmlst, iosb,
    					  0, 0, 0))))
	vaxc$errno = iosb[0];
    if (0 == (1&vaxc$errno))
	{
 	errno = EVMSERR;
	perror("Can't get pipe mailbox device name");
	exit(EXIT_ERR);
	}
    mbxname[mbxdsc.dsc$w_length] = '\0';
#ifdef DEBUG
    fprintf(stderr, "Pipe Mailbox Name = '%s'\n", mbxname);
#endif
    if (0 == (1&(vaxc$errno = lib$spawn(&cmddsc, &mbxdsc, 0, &1,
    					0, &pid, child_st, &0, sig_child,
    					&child_chan))))
	{
	errno = EVMSERR;
	perror("Can't spawn subprocess");
	exit(EXIT_ERR);
	}
#ifdef DEBUG
    fprintf(stderr, "Subprocess's Pid = %08X\n", pid);
#endif
    sys$dclexh(&exit_block);
    return(mbxname);
}

background_process(argc, argv)
int argc;
char **argv;
{
char command[2048] = "$";
$DESCRIPTOR(value, command);
$DESCRIPTOR(cmd, "BACKGROUND$COMMAND");
$DESCRIPTOR(null, "NLA0:");
int pid;

    strcat(command, argv[0]);
    while (--argc)
	{
	strcat(command, " \"");
	strcat(command, *(++argv));
	strcat(command, "\"");
	}
    value.dsc$w_length = strlen(command);
    if (0 == (1&(vaxc$errno = lib$set_symbol(&cmd, &value))))
	{
	errno = EVMSERR;
	perror("Can't create symbol for subprocess command");
	exit(EXIT_ERR);
	}
    if (0 == (1&(vaxc$errno = lib$spawn(&cmd, &null, 0, &17, 0, &pid))))
	{
	errno = EVMSERR;
	perror("Can't spawn subprocess");
	exit(EXIT_ERR);
	}
#ifdef DEBUG
    fprintf(stderr, "%s\n", command);
#endif
    fprintf(stderr, "%08X\n", pid);
    return(EXIT_OK);
}

/* got this off net.sources */

#ifdef	VMS
#define	index	strchr
#endif	/*VMS*/

/*
 * get option letter from argument vector
 */
int	opterr = 1,		/* useless, never set or used */
	optind = 1,		/* index into parent argv vector */
	optopt;			/* character checked for validity */
char	*optarg;		/* argument associated with option */

#define BADCH	(int)'?'
#define EMSG	""
#define tell(s)	fputs(progname,stderr);fputs(s,stderr); \
		fputc(optopt,stderr);fputc('\n',stderr);return(BADCH);

getopt(nargc,nargv,ostr)
int	nargc;
char	**nargv,
	*ostr;
{
	static char	*place = EMSG;	/* option letter processing */
	register char	*oli;		/* option letter list index */
	char	*index();
	char	*progname;

	if(!*place) {			/* update scanning pointer */
		if(optind >= nargc || *(place = nargv[optind]) != '-' || !*++place) return(EOF);
		if (*place == '-') {	/* found "--" */
			++optind;
			return(EOF);
		}
	}				/* option letter okay? */
	if ((optopt = (int)*place++) == (int)':' || !(oli = index(ostr,optopt))) {
		if(!*place) ++optind;
#ifdef VMS
                progname = strrchr(nargv[0],']');
#else
                progname = rindex(nargv[0],'/');
#endif
                if (!progname) progname = nargv[0]; else progname++;
		tell(": illegal option -- ");
	}
	if (*++oli != ':') {		/* don't need argument */
		optarg = NULL;
		if (!*place) ++optind;
	}
	else {				/* need an argument */
		if (*place) optarg = place;	/* no white space */
		else if (nargc <= ++optind) {	/* no arg */
			place = EMSG;
#ifdef VMS
                        progname = strrchr(nargv[0],']');
#else
                        progname = rindex(nargv[0],'/');
#endif
                        if (!progname) progname = nargv[0]; else progname++;
			tell(": option requires an argument -- ");
		}
	 	else optarg = nargv[optind];	/* white space */
		place = EMSG;
		++optind;
	}
	return(optopt);			/* dump back option letter */
}
