/* ex.c */

/* Author:
 *	Steve Kirkendall
 *	14407 SW Teal Blvd. #C
 *	Beaverton, OR 97005
 *	kirkenda@cs.pdx.edu
 */


/* This file contains the code for reading ex commands. */

#include "config.h"
#include <ctype.h>
#include "vi.h"

#ifndef isascii
# define isascii(c) !((c)&~0x7f)
#endif

/* This data type is used to describe the possible argument combinations */
typedef short ARGT;
#define FROM	1		/* allow a linespec */
#define	TO	2		/* allow a second linespec */
#define BANG	4		/* allow a ! after the command name */
#define EXTRA	8		/* allow extra args after command name */
#define XFILE	16		/* expand wildcards in extra part */
#define NOSPC	32		/* no spaces allowed in the extra part */
#define	DFLALL	64		/* default file range is 1,$ */
#define DFLNONE	128		/* no default file range */
#define NODFL	256		/* do not default to the current file name */
#define EXRCOK	512		/* can be in a .exrc file */
#define NL	1024		/* if mode!=MODE_EX, then write a newline first */
#define PLUS	2048		/* allow a line number, as in ":e +32 foo" */
#define ZERO	4096		/* allow 0 to be given as a line number */
#define FILES	(XFILE + EXTRA)	/* multiple extra files allowed */
#define WORD1	(EXTRA + NOSPC)	/* one extra word allowed */
#define FILE1	(FILES + NOSPC)	/* 1 file allowed, defaults to current file */
#define NAMEDF	(FILE1 + NODFL)	/* 1 file allowed, defaults to "" */
#define NAMEDFS	(FILES + NODFL)	/* multiple files allowed, default is "" */
#define RANGE	(FROM + TO)	/* range of linespecs allowed */
#define NONE	0		/* no args allowed at all */

/* This array maps ex command names to command codes. The order in which
 * command names are listed below is significant -- ambiguous abbreviations
 * are always resolved to be the first possible match.  (e.g. "r" is taken
 * to mean "read", not "rewind", because "read" comes before "rewind")
 */
static struct
{
	char	*name;	/* name of the command */
	CMD	code;	/* enum code of the command */
	void	(*fn)();/* function which executes the command */
	ARGT	argt;	/* command line arguments permitted/needed/used */
}
	cmdnames[] =
{   /*	cmd name	cmd code	function	arguments */
	{"append",	CMD_APPEND,	cmd_append,	FROM+ZERO	},
#ifdef DEBUG
	{"bug",		CMD_DEBUG,	cmd_debug,	RANGE+BANG+EXTRA+NL},
#endif
	{"change",	CMD_CHANGE,	cmd_append,	RANGE		},
	{"delete",	CMD_DELETE,	cmd_delete,	RANGE+WORD1	},
	{"edit",	CMD_EDIT,	cmd_edit,	BANG+FILE1+PLUS	},
	{"file",	CMD_FILE,	cmd_file,	NAMEDF		},
	{"global",	CMD_GLOBAL,	cmd_global,	RANGE+BANG+EXTRA+DFLALL},
	{"insert",	CMD_INSERT,	cmd_append,	FROM		},
	{"join",	CMD_INSERT,	cmd_join,	RANGE		},
	{"k",		CMD_MARK,	cmd_mark,	FROM+WORD1	},
	{"list",	CMD_LIST,	cmd_print,	RANGE+NL	},
	{"move",	CMD_MOVE,	cmd_move,	RANGE+EXTRA	},
	{"next",	CMD_NEXT,	cmd_next,	BANG+NAMEDFS	},
	{"Next",	CMD_PREVIOUS,	cmd_next,	BANG		},
	{"print",	CMD_PRINT,	cmd_print,	RANGE+NL	},
	{"quit",	CMD_QUIT,	cmd_xit,	BANG		},
	{"read",	CMD_READ,	cmd_read,	FROM+ZERO+NAMEDF},
	{"substitute",	CMD_SUBSTITUTE,	cmd_substitute,	RANGE+EXTRA	},
	{"to",		CMD_COPY,	cmd_move,	RANGE+EXTRA	},
	{"undo",	CMD_UNDO,	cmd_undo,	NONE		},
	{"vglobal",	CMD_VGLOBAL,	cmd_global,	RANGE+EXTRA+DFLALL},
	{"write",	CMD_WRITE,	cmd_write,	RANGE+BANG+FILE1+DFLALL},
	{"xit",		CMD_XIT,	cmd_xit,	BANG+NL		},
	{"yank",	CMD_YANK,	cmd_delete,	RANGE+WORD1	},

	{"!",		CMD_BANG,	cmd_shell,	EXRCOK+RANGE+NAMEDFS+DFLNONE+NL},
	{"<",		CMD_SHIFTL,	cmd_shift,	RANGE		},
	{">",		CMD_SHIFTR,	cmd_shift,	RANGE		},
	{"=",		CMD_EQUAL,	cmd_file,	RANGE		},
	{"&",		CMD_SUBAGAIN,	cmd_substitute,	RANGE		},
#ifndef NO_AT
	{"@",		CMD_AT,		cmd_at,		EXTRA		},
#endif

#ifndef NO_ABBR
	{"abbreviate",	CMD_ABBR,	cmd_abbr,	EXRCOK+EXTRA	},
#endif
	{"args",	CMD_ARGS,	cmd_args,	EXRCOK+NAMEDFS	},
#ifndef NO_ERRLIST
	{"cc",		CMD_CC,		cmd_make,	BANG+FILES	},
#endif
	{"cd",		CMD_CD,		cmd_cd,		EXRCOK+NAMEDF	},
	{"copy",	CMD_COPY,	cmd_move,	RANGE+EXTRA	},
#ifndef NO_DIGRAPH
	{"digraph",	CMD_DIGRAPH,	cmd_digraph,	EXRCOK+BANG+EXTRA},
#endif
#ifndef NO_ERRLIST
	{"errlist",	CMD_ERRLIST,	cmd_errlist,	BANG+NAMEDF	},
#endif
	{"ex",		CMD_EDIT,	cmd_edit,	BANG+FILE1	},
	{"map",		CMD_MAP,	cmd_map,	EXRCOK+BANG+EXTRA},
#ifndef NO_MKEXRC
	{"mkexrc",	CMD_MKEXRC,	cmd_mkexrc,	NAMEDF		},
#endif
	{"number",	CMD_NUMBER,	cmd_print,	RANGE+NL	},
	{"put",		CMD_PUT,	cmd_put,	FROM+ZERO+WORD1	},
	{"set",		CMD_SET,	cmd_set,	EXRCOK+EXTRA	},
	{"shell",	CMD_SHELL,	cmd_shell,	NL		},
	{"source",	CMD_SOURCE,	cmd_source,	EXRCOK+NAMEDF	},
	{"tag",		CMD_TAG,	cmd_tag,	BANG+WORD1	},
	{"version",	CMD_VERSION,	cmd_version,	EXRCOK+NONE	},
	{"visual",	CMD_VISUAL,	cmd_visual,	NONE		},
	{"wq",		CMD_WQUIT,	cmd_xit,	NL		},

#ifdef DEBUG
	{"debug",	CMD_DEBUG,	cmd_debug,	RANGE+BANG+EXTRA+NL},
	{"validate",	CMD_VALIDATE,	cmd_validate,	BANG+NL		},
#endif
	{"chdir",	CMD_CD,		cmd_cd,		EXRCOK+NAMEDF	},
#ifndef NO_ERRLIST
	{"make",	CMD_MAKE,	cmd_make,	BANG+NAMEDFS	},
#endif
	{"mark",	CMD_MARK,	cmd_mark,	FROM+WORD1	},
	{"previous",	CMD_PREVIOUS,	cmd_next,	BANG		},
	{"rewind",	CMD_REWIND,	cmd_next,	BANG		},
	{"unmap",	CMD_UNMAP,	cmd_map,	EXRCOK+BANG+EXTRA},
#ifndef NO_ABBR
	{"unabbreviate",CMD_UNABBR,	cmd_abbr,	EXRCOK+WORD1	},
#endif

	{(char *)0}
};


/* This function parses a search pattern - given a pointer to a / or ?,
 * it replaces the ending / or ? with a \0, and returns a pointer to the
 * stuff that came after the pattern.
 */
char	*parseptrn(ptrn)
	REG char	*ptrn;
{
	REG char 	*scan;

	for (scan = ptrn + 1;
	     *scan && *scan != *ptrn;
	     scan++)
	{
		/* allow backslashed versions of / and ? in the pattern */
		if (*scan == '\\' && scan[1] != '\0')
		{
			scan++;
		}
	}
	if (*scan)
	{
		*scan++ = '\0';
	}

	return scan;
}


/* This function parses a line specifier for ex commands */
char *linespec(s, markptr)
	REG char	*s;		/* start of the line specifier */
	MARK		*markptr;	/* where to store the mark's value */
{
	long		num;
	REG char	*t;

	/* parse each ;-delimited clause of this linespec */
	do
	{
		/* skip an initial ';', if any */
		if (*s == ';')
		{
			s++;
		}

		/* skip leading spaces */
		while (isascii(*s) && isspace(*s))
		{
			s++;
		}
	
		/* dot means current position */
		if (*s == '.')
		{
			s++;
			*markptr = cursor;
		}
		/* '$' means the last line */
		else if (*s == '$')
		{
			s++;
			*markptr = MARK_LAST;
		}
		/* digit means an absolute line number */
		else if (isascii(*s) && isdigit(*s))
		{
			for (num = 0; isascii(*s) && isdigit(*s); s++)
			{
				num = num * 10 + *s - '0';
			}
			*markptr = MARK_AT_LINE(num);
		}
		/* appostrophe means go to a set mark */
		else if (*s == '\'')
		{
			s++;
			*markptr = m_tomark(cursor, 1L, (int)*s);
			s++;
		}
		/* slash means do a search */
		else if (*s == '/' || *s == '?')
		{
			/* put a '\0' at the end of the search pattern */
			t = parseptrn(s);
	
			/* search for the pattern */
			*markptr &= ~(BLKSIZE - 1);
			if (*s == '/')
			{
				pfetch(markline(*markptr));
				if (plen > 0)
					*markptr += plen - 1;
				*markptr = m_fsrch(*markptr, s);
			}
			else
			{
				*markptr = m_bsrch(*markptr, s);
			}
	
			/* adjust command string pointer */
			s = t;
		}
	
		/* if linespec was faulty, quit now */
		if (!*markptr)
		{
			return s;
		}
	
		/* maybe add an offset */
		t = s;
		if (*t == '-' || *t == '+')
		{
			s++;
			for (num = 0; *s >= '0' && *s <= '9'; s++)
			{
				num = num * 10 + *s - '0';
			}
			if (num == 0)
			{
				num = 1;
			}
			*markptr = m_updnto(*markptr, num, *t);
		}
	} while (*s == ';' || *s == '+' || *s == '-');

	return s;
}



/* This function reads an ex command and executes it. */
void ex()
{
	char		cmdbuf[80];
	REG int		cmdlen;
	static long	oldline;
	
	significant = FALSE;
	oldline = markline(cursor);

	while (mode == MODE_EX)
	{
		/* read a line */
		cmdlen = vgets(':', cmdbuf, sizeof cmdbuf);
		if (cmdlen < 0)
		{
			return;
		}

		/* if empty line, assume ".+1" */
		if (cmdlen == 0)
		{
			strcpy(cmdbuf, ".+1");
			qaddch('\r');
			clrtoeol();
		}
		else
		{
			addch('\n');
		}
		refresh();

		/* parse & execute the command */
		doexcmd(cmdbuf);

		/* handle autoprint */
		if (significant || markline(cursor) != oldline)
		{
			significant = FALSE;
			oldline = markline(cursor);
			if (*o_autoprint && mode == MODE_EX)
			{
				cmd_print(cursor, cursor, CMD_PRINT, FALSE, "");
			}
		}
	}
}

void doexcmd(cmdbuf)
	char		*cmdbuf;	/* string containing an ex command */
{
	REG char	*scan;		/* used to scan thru cmdbuf */
	MARK		frommark;	/* first linespec */
	MARK		tomark;		/* second linespec */
	REG int		cmdlen;		/* length of the command name given */
	CMD		cmd;		/* what command is this? */
	ARGT		argt;		/* argument types for this command */
	short		forceit;	/* bang version of a command? */
	REG int		cmdidx;		/* index of command */
	REG char	*build;		/* used while copying filenames */
	int		iswild;		/* boolean: filenames use wildcards? */
	int		isdfl;		/* using default line ranges? */
	int		didsub;		/* did we substitute file names for % or # */


	/* ex commands can't be undone via the shift-U command */
	U_line = 0L;

	/* ignore command lines that start with a double-quote */
	if (*cmdbuf == '"')
	{
		return;
	}

	/* permit extra colons at the start of the line */
	while (*cmdbuf == ':')
	{
		cmdbuf++;
	}

	/* parse the line specifier */
	scan = cmdbuf;
	if (nlines < 1)
	{
		/* no file, so don't allow addresses */
	}
	else if (*scan == '%')
	{
		/* '%' means all lines */
		frommark = MARK_FIRST;
		tomark = MARK_LAST;
		scan++;
	}
	else if (*scan == '0')
	{
		frommark = tomark = MARK_UNSET;
		scan++;
	}
	else
	{
		frommark = cursor;
		scan = linespec(scan, &frommark);
		tomark = frommark;
		if (frommark && *scan == ',')
		{
			scan++;
			scan = linespec(scan, &tomark);
		}
		if (!tomark)
		{
			/* faulty line spec -- fault already described */
			return;
		}
		if (frommark > tomark)
		{
			msg("first address exceeds the second");
			return;
		}
	}
	isdfl = (scan == cmdbuf);

	/* skip whitespace */
	while (isascii(*scan) && isspace(*scan))
	{
		scan++;
	}

	/* if no command, then just move the cursor to the mark */
	if (!*scan)
	{
		cursor = tomark;
		return;
	}

	/* figure out how long the command name is */
	if (isascii(*scan) && !isalpha(*scan))
	{
		cmdlen = 1;
	}
	else
	{
		for (cmdlen = 1;
		     !isascii(scan[cmdlen]) || isalpha(scan[cmdlen]);
		     cmdlen++)
		{
		}
	}

	/* lookup the command code */
	for (cmdidx = 0;
	     cmdnames[cmdidx].name && strncmp(scan, cmdnames[cmdidx].name, cmdlen);
	     cmdidx++)
	{
	}
	argt = cmdnames[cmdidx].argt;
	cmd = cmdnames[cmdidx].code;
	if (cmd == CMD_NULL)
	{
#if OSK
		msg("Unknown command \"%s\"", scan);
#else
		msg("Unknown command \"%.*s\"", cmdlen, scan);
#endif
		return;
	}

	/* if the command ended with a bang, set the forceit flag */
	scan += cmdlen;
	if ((argt & BANG) && *scan == '!')
	{
		scan++;
		forceit = 1;
	}
	else
	{
		forceit = 0;
	}

	/* skip any more whitespace, to leave scan pointing to arguments */
	while (isascii(*scan) && isspace(*scan))
	{
		scan++;
	}

	/* a couple of special cases for filenames */
	if (argt & XFILE)
	{
		/* if names were given, process them */
		if (*scan)
		{
			for (build = tmpblk.c, iswild = didsub = FALSE; *scan; scan++)
			{
				switch (*scan)
				{
				  case '%':
					if (!*origname)
					{
						msg("No filename to substitute for %%");
						return;
					}
					strcpy(build, origname);
					while (*build)
					{
						build++;
					}
					didsub = TRUE;
					break;
	
				  case '#':
					if (!*prevorig)
					{
						msg("No filename to substitute for #");
						return;
					}
					strcpy(build, prevorig);
					while (*build)
					{
						build++;
					}
					didsub = TRUE;
					break;
	
				  case '*':
				  case '?':
#if !(MSDOS || TOS)
				  case '[':
				  case '`':
				  case '{': /* } */
				  case '$':
				  case '~':
#endif
					*build++ = *scan;
					iswild = TRUE;
					break;

				  default:
					*build++ = *scan;
				}
			}
			*build = '\0';
	
			if (cmd == CMD_BANG
			 || cmd == CMD_READ && tmpblk.c[0] == '!'
			 || cmd == CMD_WRITE && tmpblk.c[0] == '!')
			{
				if (didsub)
				{
					if (mode != MODE_EX)
					{
						addch('\n');
					}
					addstr(tmpblk.c);
					addch('\n');
					exrefresh();
				}
			}
			else
			{
				if (iswild && tmpblk.c[0] != '>')
				{
					scan = wildcard(tmpblk.c);
				}
			}
		}
		else /* no names given, maybe assume origname */
		{
			if (!(argt & NODFL))
			{
				strcpy(tmpblk.c, origname);
			}
			else
			{
				*tmpblk.c = '\0';
			}
		}

		scan = tmpblk.c;
	}

	/* bad arguments? */
	if (!(argt & EXRCOK) && nlines < 1L)
	{
		msg("Can't use the \"%s\" command in a %s file", cmdnames[cmdidx].name, EXRC);
		return;
	}
	if (!(argt & (ZERO | EXRCOK)) && frommark == MARK_UNSET)
	{
		msg("Can't use address 0 with \"%s\" command.", cmdnames[cmdidx].name);
		return;
	}
	if (!(argt & FROM) && frommark != cursor && nlines >= 1L)
	{
		msg("Can't use address with \"%s\" command.", cmdnames[cmdidx].name);
		return;
	}
	if (!(argt & TO) && tomark != frommark && nlines >= 1L)
	{
		msg("Can't use a range with \"%s\" command.", cmdnames[cmdidx].name);
		return;
	}
	if (!(argt & EXTRA) && *scan)
	{
		msg("Extra characters after \"%s\" command.", cmdnames[cmdidx].name);
		return;
	}
	if ((argt & NOSPC) && !(cmd == CMD_READ && (forceit || *scan == '!')))
	{
		build = scan;
#ifndef CRUNCH
		if ((argt & PLUS) && *build == '+')
		{
			while (*build && !(isascii(*build) && isspace(*build)))
			{
				build++;
			}
			while (*build && isascii(*build) && isspace(*build))
			{
				build++;
			}
		}
#endif /* not CRUNCH */
		for (; *build; build++)
		{
			if (isspace(*build))
			{
				msg("Too many %s to \"%s\" command.",
					(argt & XFILE) ? "filenames" : "arguments",
					cmdnames[cmdidx].name);
				return;
			}
		}
	}

	/* some commands have special default ranges */
	if (isdfl && (argt & DFLALL))
	{
		frommark = MARK_FIRST;
		tomark = MARK_LAST;
	}
	else if (isdfl && (argt & DFLNONE))
	{
		frommark = tomark = 0L;
	}

	/* write a newline if called from visual mode */
	if ((argt & NL) && mode != MODE_EX && !exwrote)
	{
		addch('\n');
		exrefresh();
	}

	/* act on the command */
	(*cmdnames[cmdidx].fn)(frommark, tomark, cmd, forceit, scan);
}


/* This function executes EX commands from a file.  It returns 1 normally, or
 * 0 if the file could not be opened for reading.
 */
int doexrc(filename)
	char	*filename;	/* name of a ".exrc" file */
{
	int	fd;		/* file descriptor */
	int	len;		/* length of the ".exrc" file */
	char	buf[MAXRCLEN];	/* buffer, holds the entire .exrc file */

	/* open the file, read it, and close */
	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		return 0;
	}
	len = tread(fd, buf, MAXRCLEN);
	close(fd);

	/* execute the string */
	exstring(buf, len);

	return 1;
}

void exstring(buf, len)
	char	*buf;	/* the commands to execute */
	int	len;	/* the length of the string */
{
	char	*cmd;		/* start of a command */
	char	*end;		/* used to search for the end of cmd */

	/* find & do each command */
	for (cmd = buf; cmd < &buf[len]; cmd = end + 1)
	{
		/* find the end of the command */
		for (end = cmd; end < &buf[len] && *end != '\n' && *end != '|'; end++)
		{
		}
		*end = '\0';

		/* do it */
		doexcmd(cmd);
	}
}
