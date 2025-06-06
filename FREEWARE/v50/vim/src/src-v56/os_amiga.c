/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * os_amiga.c
 *
 * Amiga system-dependent routines.
 */

#include "vim.h"

#ifdef Window
# undef Window	/* Amiga has its own Window definition */
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif

#undef TRUE		/* will be redefined by exec/types.h */
#undef FALSE

#ifndef LATTICE
# include <exec/types.h>
# include <exec/exec.h>
# include <libraries/dos.h>
# include <libraries/dosextens.h>
# include <intuition/intuition.h>
#else
# include <proto/dos.h>
# include <libraries/dosextens.h>
# include <proto/intuition.h>
# include <proto/exec.h>
#endif

#include <exec/memory.h>

#include <dos/dostags.h>	    /* for 2.0 functions */
#include <dos/dosasl.h>

#if defined(LATTICE) && !defined(SASC) && !defined(NO_ARP)
# include <libraries/arp_pragmas.h>
#endif

/*
 * At this point TRUE and FALSE are defined as 1L and 0L, but we want 1 and 0.
 */
#undef	TRUE
#define TRUE (1)
#undef	FALSE
#define FALSE (0)

#ifndef AZTEC_C
static long dos_packet __ARGS((struct MsgPort *, long, long));
#endif
static int lock2name __ARGS((BPTR lock, char_u *buf, long   len));
static void out_num __ARGS((long n));
static struct FileInfoBlock *get_fib __ARGS((char_u *));
static int sortcmp __ARGS((char **a, char **b));

static BPTR		raw_in = (BPTR)NULL;
static BPTR		raw_out = (BPTR)NULL;
static int		close_win = FALSE;  /* set if Vim opened the window */

struct IntuitionBase	*IntuitionBase = NULL;
#ifndef NO_ARP
struct ArpBase		*ArpBase = NULL;
#endif

static struct Window	*wb_window;
static char_u		*oldwindowtitle = NULL;

#ifndef NO_ARP
int			dos2 = FALSE;	    /* Amiga DOS 2.0x or higher */
#endif
int			size_set = FALSE;   /* set to TRUE if window size was set */

    void
win_resize_on()
{
    OUT_STR_NF("\033[12{");
}

    void
win_resize_off()
{
    OUT_STR_NF("\033[12}");
}

    void
mch_write(p, len)
    char_u  *p;
    int	    len;
{
    Write(raw_out, (char *)p, (long)len);
}

/*
 * mch_inchar(): low level input funcion.
 * Get a characters from the keyboard.
 * If time == 0 do not wait for characters.
 * If time == n wait a short time for characters.
 * If time == -1 wait forever for characters.
 *
 * Return number of characters read.
 */
    int
mch_inchar(buf, maxlen, time)
    char_u  *buf;
    int	    maxlen;
    long    time;		/* milli seconds */
{
    int	    len;
    long    utime;
#ifdef AUTOCMD
    static int	once_already = 0;
#endif

    if (time >= 0)
    {
	if (time == 0)
	    utime = 100L;	    /* time = 0 causes problems in DOS 1.2 */
	else
	    utime = time * 1000L;   /* convert from milli to micro secs */
	if (WaitForChar(raw_in, utime) == 0)	/* no character available */
	{
#ifdef AUTOCMD
	    once_already = 0;
#endif
	    return 0;
	}
    }
    else    /* time == -1 */
    {
#ifdef AUTOCMD
	if (once_already == 2)
	    updatescript(0);
	else if (once_already == 1)
	{
	    setcursor();
	    once_already = 2;
	    return 0;
	}
	else
#endif
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk
	 */
	    if (WaitForChar(raw_in, p_ut * 1000L) == 0)
	{
#ifdef AUTOCMD
            if (has_cursorhold() && get_real_state() == NORMAL_BUSY)
            {
                apply_autocmds(EVENT_CURSORHOLD, NULL, NULL, FALSE, curbuf);
                update_screen(VALID);
		once_already = 1;
                return 0;
            }
            else
#endif
		updatescript(0);
	}
    }

    for (;;)	    /* repeat until we got a character */
    {
	len = Read(raw_in, (char *)buf, (long)maxlen);
	if (len > 0)
	{
#ifdef AUTOCMD
            once_already = 0;
#endif
	    return len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    int
mch_char_avail()
{
    return (WaitForChar(raw_in, 100L) != 0);
}

    long_u
mch_avail_mem(special)
    int	    special;
{
    return (long_u)AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY);
}

    void
mch_delay(msec, ignoreinput)
    long    msec;
    int	    ignoreinput;
{
#ifndef LATTICE		/* SAS declares void Delay(UNLONG) */
    void	    Delay __ARGS((long));
#endif

    if (msec > 0)
    {
	if (ignoreinput)
	    Delay(msec / 20L);	    /* Delay works with 20 msec intervals */
	else
	    WaitForChar(raw_in, msec * 1000L);
    }
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    void
mch_suspend()
{
    suspend_shell();
}

#define DOS_LIBRARY	((UBYTE *) "dos.library")

    void
mch_windinit()
{
    static char	    intlibname[] = "intuition.library";

#ifdef AZTEC_C
    Enable_Abort = 0;		/* disallow vim to be aborted */
#endif
    Columns = 80;
    Rows = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    if (raw_in == (BPTR)NULL)
    {
	raw_in = Input();
	raw_out = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	if (!IsInteractive(raw_in))
	    raw_in = raw_out;
	else if (!IsInteractive(raw_out))
	    raw_out = raw_in;
    }

    out_flush();

    wb_window = NULL;
    if ((IntuitionBase = (struct IntuitionBase *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == NULL)
    {
	mch_errmsg("cannot open ");
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_windexit(3);
    }
}

#include <workbench/startup.h>

/*
 * Check_win checks whether we have an interactive window.
 * If not, a new window is opened with the newcli command.
 * If we would open a window ourselves, the :sh and :! commands would not
 * work properly (Why? probably because we are then running in a background
 * CLI). This also is the best way to assure proper working in a next
 * Workbench release.
 *
 * For the -f option (foreground mode) we open our own window and disable :sh.
 * Otherwise the calling program would never know when editing is finished.
 */
#define BUF2SIZE 320	    /* length of buffer for argument with complete path */

    int
mch_check_win(argc, argv)
    int argc;
    char **argv;
{
    int		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[20];
    char_u	    buf2[BUF2SIZE];
    static char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    static char_u   winerr[] = "VIM: Can't open window!\n";
    struct WBArg    *argp;
    int		    ac;
    char	    *av;
    char_u	    *device = NULL;
    int		    exitval = 4;
    struct Library  *DosBase;
    int		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
    if (DosBase = OpenLibrary(DOS_LIBRARY, 37L))
    {
	CloseLibrary(DosBase);
#ifndef NO_ARP
	dos2 = TRUE;
#endif
    }
    else	    /* without arp functions we NEED 2.0 */
    {
#ifdef NO_ARP
	mch_errmsg("Need Amigados version 2.04 or later\n");
	exit(3);
#else
		    /* need arp functions for dos 1.x */
	if (!(ArpBase = (struct ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
	{
	    fprintf(stderr, "Need %s version %ld\n", ArpName, ArpVersion);
	    exit(3);
	}
#endif
    }

/*
 * scan argv[] for the "-f" and "-d" arguments
 */
    for (i = 1; i < argc; ++i)
	if (argv[i][0] == '-')
	{
	    switch (argv[i][1])
	    {
	    case 'f':
		usewin = TRUE;
		break;

	    case 'd':
		if (i < argc - 1)
		    device = (char_u *)argv[i + 1];
		break;
	    }
	}

/*
 * If we were not started from workbench, do not have a '-d' argument and
 * we have been started with an interactive window, use that window.
 */
    if (argc != 0 && device == NULL &&
			  (IsInteractive(Input()) || IsInteractive(Output())))
	return OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    if (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	for (i = -1; i < 3; ++i)
	{
	    if (i >= 0)
		device = constrings[i];
	    if (device && (raw_in = Open((UBYTE *)device,
					   (long)MODE_NEWFILE)) != (BPTR)NULL)
		break;
	}
	if (raw_in == (BPTR)NULL)	/* all three failed */
	{
	    mch_errmsg((char *)winerr);
	    goto exit;
	}
	raw_out = raw_in;
	close_win = TRUE;
	return OK;
    }

    if ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)NULL)
    {
	mch_errmsg("Cannot open NIL:\n");
	goto exit;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     */
    sprintf((char *)buf1, "t:nc%ld", (char *)buf1);
    if ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)NULL)
    {
	mch_errmsg("Cannot create ");
	mch_errmsg((char *)buf1);
	mch_errmsg("\n");
	goto exit;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    if (argc == 0)	/* run from workbench */
	ac = ((struct WBStartup *)argv)->sm_NumArgs;
    else
	ac = argc;
    for (i = 0; i < ac; ++i)
    {
	if (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((struct WBStartup *)argv)->sm_ArgList[i]);
	    if (argp->wa_Lock)
		(void)lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
#ifndef NO_ARP
	    if (dos2)	    /* use 2.0 function */
#endif
		AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2SIZE - 1));
#ifndef NO_ARP
	    else	    /* use arp function */
		TackOn((char *)buf2, argp->wa_Name);
#endif
	    av = (char *)buf2;
	}
	else
	    av = argv[i];

	if (av[0] == '-' && av[1] == 'd')	/* skip '-d' option */
	{
	    ++i;
	    continue;
	}
	if (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (long)strlen(av));
	if (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, " ", 1L);
    }
    Write(fh, "\nendcli\n", 8L);
    Close(fh);

/*
 * Try to open a new cli in a window. If '-d' argument was given try to open
 * the specified device. Then try a 24 line 80 column window.
 * If that fails, try two smaller ones.
 */
    for (i = -1; i < 3; ++i)
    {
	if (i >= 0)
	    device = constrings[i];
	else if (device == NULL)
	    continue;
	sprintf((char *)buf2, "newcli <nil: >nil: %s from %s", (char *)device, (char *)buf1);
#ifndef NO_ARP
	if (dos2)
	{
#endif
	    if (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
		break;
#ifndef NO_ARP
	}
	else
	{
	    if (Execute((UBYTE *)buf2, nilfh, nilfh))
		break;
	}
#endif
    }
    if (i == 3)	    /* all three failed */
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg((char *)winerr);
	goto exit;
    }
    exitval = 0;    /* The Execute succeeded: exit this program */

exit:
#ifndef NO_ARP
    if (ArpBase)
	CloseLibrary((struct Library *) ArpBase);
#endif
    exit(exitval);
    /* NOTREACHED */
    return FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    int
mch_input_isatty()
{
    return TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same.
 */
    void
fname_case(name)
    char_u  *name;
{
    struct FileInfoBlock    *fib;
    size_t		    len;

    fib = get_fib(name);
    if (fib != NULL)
    {
	len = STRLEN(name);
	if (len == strlen(fib->fib_FileName))	/* safety check */
	    mch_memmove(name, fib->fib_FileName, len);
	vim_free(fib);
    }
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    static struct FileInfoBlock *
get_fib(fname)
    char_u *fname;
{
    BPTR		    flock;
    struct FileInfoBlock    *fib;

    if (fname == NULL)	    /* safety check */
	return NULL;
    fib = (struct FileInfoBlock *)malloc(sizeof(struct FileInfoBlock));
    if (fib != NULL)
    {
	flock = Lock((UBYTE *)fname, (long)ACCESS_READ);
	if (flock == (BPTR)NULL || !Examine(flock, fib))
	{
	    vim_free(fib);  /* in case of an error the memory is freed here */
	    fib = NULL;
	}
	if (flock)
	    UnLock(flock);
    }
    return fib;
}

#ifdef WANT_TITLE
/*
 * set the title of our window
 * icon name is not set
 */
    void
mch_settitle(title, icon)
    char_u  *title;
    char_u  *icon;
{
    if (wb_window != NULL && title != NULL)
	SetWindowTitles(wb_window, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  1  Just restore title
 *  2  Just restore icon (which we don't have)
 *  3  Restore title and icon (which we don't have)
 */
    void
mch_restore_title(which)
    int which;
{
    if (which & 1)
	mch_settitle(oldwindowtitle, NULL);
}

    int
mch_can_restore_title()
{
    return (wb_window != NULL);
}

    int
mch_can_restore_icon()
{
    return FALSE;
}
#endif

/*
 * Insert user name in s[len].
 */
    int
mch_get_user_name(s, len)
    char_u  *s;
    int	    len;
{
    *s = NUL;
    return FAIL;
}

/*
 * Insert host name is s[len].
 */
    void
mch_get_host_name(s, len)
    char_u  *s;
    int	    len;
{
    STRNCPY(s, "Amiga", len);
}

/*
 * return process ID
 */
    long
mch_get_pid()
{
    return (long)0;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    int
mch_dirname(buf, len)
    char_u	*buf;
    int		len;
{
    return mch_FullName((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    int
mch_FullName(fname, buf, len, force)
    char_u	*fname, *buf;
    int		len;
    int		force;
{
    BPTR	l;
    int		retval = FAIL;
    int		i;

    *buf = NUL;
    if (fname == NULL)	/* always fail */
	return FAIL;

    /* Lock the file.  If it exists, we can get the exact name. */
    if ((l = Lock((UBYTE *)fname, (long)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (long)len - 1);
	UnLock(l);
	if (mch_isdir(fname))
	    STRCAT(buf, "/");
    }
    else if (force || !mch_isFullName(fname))	    /* not a full path yet */
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	if ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)NULL)
	{
	    retval = lock2name(l, buf, (long)len);
	    UnLock(l);
	    if (retval == OK)
	    {
		i = STRLEN(buf);
		if (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
		    buf[i++] = '/';
		STRNCPY(buf + i, fname, (len - i)); /* concatenate the fname */
	    }
	}
    }
    if (retval == FAIL || *buf == 0 || *buf == ':')
	STRCPY(buf, fname); /* something failed; use the file name */
    return retval;
}

/*
 * return TRUE is fname is an absolute path name
 */
    int
mch_isFullName(fname)
    char_u	*fname;
{
    return (vim_strchr(fname, ':') != NULL);
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    static int
lock2name(lock, buf, len)
    BPTR    lock;
    char_u  *buf;
    long    len;
{
#ifndef NO_ARP
    if (dos2)		    /* use 2.0 function */
#endif
	return ((int)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifndef NO_ARP
    else		/* use arp function */
	return ((int)PathName(lock, (char *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(name)
    char_u	*name;
{
    struct FileInfoBlock    *fib;
    long		    retval = -1;

    fib = get_fib(name);
    if (fib != NULL)
    {
	retval = fib->fib_Protection;
	vim_free(fib);
    }
    return retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    int
mch_setperm(name, perm)
    char_u	*name;
    long	perm;
{
    perm &= ~FIBF_ARCHIVE;		/* reset archived bit */
    return (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    void
mch_hide(name)
    char_u	*name;
{
    /* can't hide a file */
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    int
mch_isdir(name)
    char_u	*name;
{
    struct FileInfoBlock    *fib;
    int			    retval = FALSE;

    fib = get_fib(name);
    if (fib != NULL)
    {
	retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
	vim_free(fib);
    }
    return retval;
}

/*
 * Careful: mch_windexit() may be called before mch_windinit()!
 */
    void
mch_windexit(r)
    int		    r;
{
    if (raw_in)			    /* put terminal in 'normal' mode */
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    out_char('\n');
    if (raw_out)
    {
	if (term_console)
	{
	    win_resize_off();	    /* window resize events de-activated */
	    if (size_set)
		OUT_STR("\233t\233u");	/* reset window size (CSI t CSI u) */
	}
	out_flush();
    }

#ifdef WANT_TITLE
    mch_restore_title(3);	    /* restore window title */
#endif

    ml_close_all(TRUE);		    /* remove all memfiles */

#ifndef NO_ARP
    if (ArpBase)
	CloseLibrary((struct Library *) ArpBase);
#endif
    if (close_win)
	Close(raw_in);
    if (r)
	printf("Vim exiting with %d\n", r); /* somehow this makes :cq work!? */
    exit(r);
}

/*
 * This is a routine for setting a given stream to raw or cooked mode on the
 * Amiga . This is useful when you are using Lattice C to produce programs
 * that want to read single characters with the "getch()" or "fgetc" call.
 *
 * Written : 18-Jun-87 By Chuck McManis.
 */

#define MP(xx)	((struct MsgPort *)((struct FileHandle *) (BADDR(xx)))->fh_Type)

/*
 * Function mch_settmode() - Convert the specified file pointer to 'raw' or
 * 'cooked' mode. This only works on TTY's.
 *
 * Raw: keeps DOS from translating keys for you, also (BIG WIN) it means
 *	getch() will return immediately rather than wait for a return. You
 *	lose editing features though.
 *
 * Cooked: This function returns the designate file pointer to it's normal,
 *	wait for a <CR> mode. This is exactly like raw() except that
 *	it sends a 0 to the console to make it back into a CON: from a RAW:
 */
    void
mch_settmode(tmode)
    int		tmode;
{
    if (dos_packet(MP(raw_in), (long)ACTION_SCREEN_MODE,
					  tmode == TMODE_RAW ? -1L : 0L) == 0)
	mch_errmsg("cannot change console mode ?!\n");
}

/*
 * set screen mode, always fails.
 */
    int
mch_screenmode(arg)
    char_u   *arg;
{
    EMSG("Screen mode setting not supported");
    return FAIL;
}

/*
 * Code for this routine came from the following :
 *
 * ConPackets.c -  C. Scheppner, A. Finkel, P. Lindsay	CBM
 *   DOS packet example
 *   Requires 1.2
 *
 * Found on Fish Disk 56.
 *
 * Heavely modified by mool.
 */

#include <devices/conunit.h>

/*
 * try to get the real window size
 * return FAIL for failure, OK otherwise
 */
    int
mch_get_winsize()
{
    struct ConUnit  *conUnit;
    char	    id_a[sizeof(struct InfoData) + 3];
    struct InfoData *id;

    if (!term_console)	/* not an amiga window */
	return FAIL;

    /* insure longword alignment */
    id = (struct InfoData *)(((long)id_a + 3L) & ~3L);

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    if (size_set)
	OUT_STR("\233t\233u");	/* CSI t CSI u */
    out_flush();

    if (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_window = (struct Window *)id->id_VolumeNode) == NULL)
    {
	/* it's not an amiga window, maybe aux device */
	/* terminal type should be set */
	term_console = FALSE;
	return FAIL;
    }
    if (oldwindowtitle == NULL)
	oldwindowtitle = (char_u *)wb_window->Title;
    if (id->id_InUse == (BPTR)NULL)
    {
	mch_errmsg("mch_get_winsize: not a console??\n");
	return FAIL;
    }
    conUnit = (struct ConUnit *) ((struct IOStdReq *) id->id_InUse)->io_Unit;

    /* get window size */
    Rows = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    if (Rows < 0 || Rows > 200)	    /* cannot be an amiga window */
    {
	Columns = 80;
	Rows = 24;
	term_console = FALSE;
	return FAIL;
    }
    check_winsize();

    return OK;
}

/*
 * try to set the real window size
 */
    void
mch_set_winsize()
{
    if (term_console)
    {
	size_set = TRUE;
	out_char(CSI);
	out_num((long)Rows);
	out_char('t');
	out_char(CSI);
	out_num((long)Columns);
	out_char('u');
	out_flush();
    }
}

/*
 * out_num - output a (big) number fast
 */
    static void
out_num(n)
    long	n;
{
    OUT_STR_NF(tltoa((unsigned long)n));
}

#ifndef AZTEC_C
/*
 * Sendpacket.c
 *
 * An invaluable addition to your Amiga.lib file. This code sends a packet to
 * the given message port. This makes working around DOS lots easier.
 *
 * Note, I didn't write this, those wonderful folks at CBM did. I do suggest
 * however that you may wish to add it to Amiga.Lib, to do so, compile it and
 * say 'oml lib:amiga.lib -r sendpacket.o'
 */

/* #include <proto/exec.h> */
/* #include <proto/dos.h> */
#include <exec/memory.h>

/*
 * Function - dos_packet written by Phil Lindsay, Carolyn Scheppner, and Andy
 * Finkel. This function will send a packet of the given type to the Message
 * Port supplied.
 */

    static long
dos_packet(pid, action, arg)
    struct MsgPort *pid;    /* process indentifier ... (handlers message port) */
    long	    action, /* packet type ... (what you want handler to do)   */
		    arg;    /* single argument */
{
# ifndef NO_ARP
    struct MsgPort	    *replyport;
    struct StandardPacket   *packet;
    long		    res1;

    if (dos2)
# endif
	return DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	/* use 2.0 function */
# ifndef NO_ARP

    replyport = (struct MsgPort *) CreatePort(NULL, 0);	/* use arp function */
    if (!replyport)
	return (0);

    /* Allocate space for a packet, make it public and clear it */
    packet = (struct StandardPacket *)
	AllocMem((long) sizeof(struct StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
    if (!packet) {
	DeletePort(replyport);
	return (0);
    }
    packet->sp_Msg.mn_Node.ln_Name = (char *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_Msg);
    packet->sp_Pkt.dp_Port = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    PutMsg(pid, (struct Message *)packet);	/* send packet */

    WaitPort(replyport);
    GetMsg(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (long) sizeof(struct StandardPacket));
    DeletePort(replyport);

    return (res1);
# endif
}
#endif

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    int
mch_call_shell(cmd, options)
    char_u	*cmd;
    int		options;	/* SHELL_*, see vim.h */
{
    BPTR	mydir;
    int		x;
#ifdef AZTEC_C
    int		use_execute;
    char_u	*shellcmd = NULL;
    char_u	*shellarg;
#endif
    int		retval = 0;

    if (close_win)
    {
	/* if Vim opened a window: Executing a shell may cause crashes */
	EMSG("Cannot execute shell with -f option");
	return -1;
    }

    if (term_console)
	win_resize_off();	    /* window resize events de-activated */
    out_flush();

    if (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    /* set to normal mode */
    mydir = Lock((UBYTE *)"", (long)ACCESS_READ);   /* remember current directory */

#if !defined(AZTEC_C)		    /* not tested very much */
    if (cmd == NULL)
    {
# ifndef NO_ARP
	if (dos2)
# endif
	    x = SystemTags(p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifndef NO_ARP
	else
	    x = Execute(p_sh, raw_in, raw_out);
# endif
    }
    else
    {
# ifndef NO_ARP
	if (dos2)
# endif
	    x = SystemTags((char *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifndef NO_ARP
	else
	    x = Execute((char *)cmd, 0L, raw_out);
# endif
    }
# ifdef NO_ARP
    if (x < 0)
# else
    if ((dos2 && x < 0) || (!dos2 && !x))
# endif
    {
	MSG_PUTS("Cannot execute ");
	if (cmd == NULL)
	{
	    MSG_PUTS("shell ");
	    msg_outtrans(p_sh);
	}
	else
	    msg_outtrans(cmd);
	msg_putchar('\n');
	retval = -1;
    }
# ifdef NO_ARP
    else if (x)
# else
    else if (!dos2 || x)
# endif
    {
	if (x = IoErr())
	{
	    if (!(options & SHELL_SILENT))
	    {
		msg_putchar('\n');
		msg_outnum((long)x);
		MSG_PUTS(" returned\n");
	    }
	    retval = x;
	}
    }
#else	/* else part is for AZTEC_C */
    if (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    else
	use_execute = 0;
    if (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	if (shellcmd == NULL)	    /* out of memory, use Execute */
	    use_execute = 1;
	else
	{
	    shellarg = skiptowhite(shellcmd);	/* find start of arguments */
	    if (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    if (cmd == NULL)
    {
	if (use_execute)
	{
# ifndef NO_ARP
	    if (dos2)
# endif
		x = SystemTags((UBYTE *)p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifndef NO_ARP
	    else
		x = !Execute((UBYTE *)p_sh, raw_in, raw_out);
# endif
	}
	else
	    x = fexecl((char *)shellcmd, (char *)shellcmd, (char *)shellarg, NULL);
    }
    else if (use_execute)
    {
# ifndef NO_ARP
	if (dos2)
# endif
	    x = SystemTags((UBYTE *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifndef NO_ARP
	else
	    x = !Execute((UBYTE *)cmd, 0L, raw_out);
# endif
    }
    else if (p_st & 1)
	x = fexecl((char *)shellcmd, (char *)shellcmd, (char *)shellarg,
							   (char *)cmd, NULL);
    else
	x = fexecl((char *)shellcmd, (char *)shellcmd, (char *)shellarg,
					   (char *)p_shcf, (char *)cmd, NULL);
# ifdef NO_ARP
    if (x < 0)
# else
    if ((dos2 && x < 0) || (!dos2 && x))
# endif
    {
	MSG_PUTS("Cannot execute ");
	if (use_execute)
	{
	    if (cmd == NULL)
		msg_outtrans(p_sh);
	    else
		msg_outtrans(cmd);
	}
	else
	{
	    MSG_PUTS("shell ");
	    msg_outtrans(shellcmd);
	}
	msg_putchar('\n');
	retval = -1;
    }
    else
    {
	if (use_execute)
	{
# ifdef NO_ARP
	    if (x)
# else
	    if (!dos2 || x)
# endif
		x = IoErr();
	}
	else
	    x = wait();
	if (x)
	{
	    if (!(options & SHELL_SILENT))
	    {
		msg_putchar('\n');
		msg_outnum((long)x);
		MSG_PUTS(" returned\n");
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#endif	/* AZTEC_C */

    if (mydir = CurrentDir(mydir))	/* make sure we stay in the same directory */
	UnLock(mydir);
    settmode(TMODE_RAW);		/* set to raw mode */
#ifdef WANT_TITLE
    resettitle();
#endif
    if (term_console)
	win_resize_on();		/* window resize events activated */
    return retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    void
mch_breakcheck()
{
   if (SetSignal(0L, (long)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

/* this routine causes manx to use this Chk_Abort() rather than it's own */
/* otherwise it resets our ^C when doing any I/O (even when Enable_Abort */
/* is zero).  Since we want to check for our own ^C's			 */

#ifdef _DCC
#define Chk_Abort chkabort
#endif

#ifdef LATTICE
void __regargs __chkabort(void);

void __regargs __chkabort(void)
{}

#else
    long
Chk_Abort(void)
{
    return(0L);
}
#endif

/*
 * mch_expandpath() - this code does wild-card pattern matching using the arp
 *		      routines.
 *
 * "pat" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 *
 * This is based on WildDemo2.c (found in arp1.1 distribution).
 * That code's copyright follows:
 *	Copyright (c) 1987, Scott Ballantyne
 *	Use and abuse as you please.
 */

#define ANCHOR_BUF_SIZE (512)
#define ANCHOR_SIZE (sizeof(struct AnchorPath) + ANCHOR_BUF_SIZE)

    int
mch_expandpath(gap, pat, flags)
    struct growarray	*gap;
    char_u		*pat;
    int			flags;		/* EW_* flags */
{
    struct AnchorPath	*Anchor;
    LONG		Result;
    char_u		*starbuf, *sp, *dp;
    int			start_len;
    int			matches;

    start_len = gap->ga_len;

    /* Get our AnchorBase */
    Anchor = (struct AnchorPath *)alloc_clear((unsigned)ANCHOR_SIZE);
    if (Anchor == NULL)
    {
	EMSG("Out of memory");
	return 0;
    }

    Anchor->ap_Strlen = ANCHOR_BUF_SIZE;  /* ap_Length not supported anymore */
#ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	/* allow '.' for current dir */
#else
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	/* allow '.' for current dir */
#endif

#ifndef NO_ARP
    if (dos2)
    {
#endif
	/* hack to replace '*' by '#?' */
	starbuf = alloc((unsigned)(2 * STRLEN(pat) + 1));
	if (starbuf == NULL)
	    goto Return;
	for (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    if (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    else
		*dp++ = *sp;
	}
	*dp = NUL;
	Result = MatchFirst((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ifndef NO_ARP
    }
    else
	Result = FindFirst((char *)pat, Anchor);
#endif

    /*
     * Loop to get all matches.
     */
    while (Result == 0)
    {
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#ifndef NO_ARP
	if (dos2)
#endif
	    Result = MatchNext(Anchor);
#ifndef NO_ARP
	else
	    Result = FindNext(Anchor);
#endif
    }
    matches = gap->ga_len - start_len;

    if (Result == ERROR_BUFFER_OVERFLOW)
	EMSG("ANCHOR_BUF_SIZE too small.");
    else if (matches == 0 && Result != ERROR_OBJECT_NOT_FOUND
			  && Result != ERROR_NO_MORE_ENTRIES)
	EMSG("I/O ERROR");

    /*
     * Sort the files for this pattern.
     */
    if (matches)
	qsort((void *)(((char_u **)gap->ga_data) + start_len),
				  (size_t)matches, sizeof(char_u *), sortcmp);

    /* Free the wildcard stuff */
#ifndef NO_ARP
    if (dos2)
#endif
	MatchEnd(Anchor);
#ifndef NO_ARP
    else
	FreeAnchorChain(Anchor);
#endif

Return:
    vim_free(Anchor);

    return matches;
}

    static int
sortcmp(a, b)
    char **a, **b;
{
    return stricmp(*a, *b);
}

    int
mch_has_wildcard(p)
    char_u *p;
{
#ifdef VIM_BACKTICK
    return (vim_strpbrk(p, (char_u *)"*?[(~#$`") != NULL);
#else
    return (vim_strpbrk(p, (char_u *)"*?[(~#$") != NULL);
#endif
}

/*
 * With AmigaDOS 2.0 support for reading local environment variables
 *
 * Two buffers are allocated:
 * - A big one to do the expansion into.  It is freed before returning.
 * - A small one to hold the return value.  It is kept until the next call.
 */
    char_u *
mch_getenv(var)
    char_u *var;
{
    int		    len;
    UBYTE	    *buf;		/* buffer to expand in */
    char_u	    *retval;		/* return value */
    static char_u   *alloced = NULL;	/* allocated memory */

#ifndef NO_ARP
    if (!dos2)
	retval = (char_u *)getenv((char *)var);
    else
#endif
    {
	vim_free(alloced);
	alloced = NULL;
	retval = NULL;

	buf = alloc(IOSIZE);
	if (buf == NULL)
	    return NULL;

	len = GetVar((UBYTE *)var, buf, (long)(IOSIZE - 1), (long)0);
	if (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    /* if $VIM is not defined, use "vim:" instead */
    if (retval == NULL && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    return retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
/* ARGSUSED */
    int
mch_setenv(var, value, x)
    char *var;
    char *value;
    int	 x;
{
#ifndef NO_ARP
    if (!dos2)
	return setenv(var, value);
#endif

    if (SetVar((UBYTE *)var, (UBYTE *)value, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	return 0;   /* success */
    return -1;	    /* failure */
}
