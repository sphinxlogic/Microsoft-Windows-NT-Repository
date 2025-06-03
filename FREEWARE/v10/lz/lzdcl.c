/*
 * DCL parser for lz... stuff (swings both ways!)
 *
 * 12-Sep-93  dp	Terminate string with \0 in cli_saveval()
 * 14-Sep-93  dp	Add record-size option to /MODE=FIXED
 *
 */
#include "lz.h"
#include climsgdef
#define	DYNAMIC_STRING(name)			\
    struct dsc$descriptor_d name = {		\
	0, DSC$K_DTYPE_T, DSC$K_CLASS_D, NULL	\
    }

typedef struct {
	char		*key;		/* Option name			*/
	flag		bit;		/* Sets this bit		*/
} KEYWORD;

static char		errname[257];	/* Error text stored here	*/
static $DESCRIPTOR(err, errname);	/* descriptor for error text	*/

extern char	*vms_etext();

static DYNAMIC_STRING(command_line);
static DYNAMIC_STRING(cli_command);
static DYNAMIC_STRING(cli_prompt);
static DYNAMIC_STRING(result);		/* cli_getvalue result datum	*/
static $DESCRIPTOR(prompt_compress, "Compress: ");
static $DESCRIPTOR(prompt_decompress, "Decompress: ");

static int intresult;			/* cli_getintval result */

globalref int		*lz_dcl_table;	/* Different table for cmp/dcmp	*/
extern int		lib$get_input();
char			*cli_savevalue();

/*
 * Note: these tables are inclusive (for both lzcomp and lzdcmp).
 * The code thus must accept "missing" element errors.
 */

KEYWORD	key_export[] = {
 { "VMS",		EXPORT_VMS		},
 { "UNIX",		EXPORT_UNIX		},
 { "BLOCK",		EXPORT_BLOCK		},
 { "HEADER",		EXPORT_HEADER		},
 { "ENDMARKER",		EXPORT_ENDMARKER	},
 { NULL,		0x0000			}
};

KEYWORD key_method[] = {
 { "LZW",		METHOD_LZ		},
 { NULL,		0x0000			}
};

KEYWORD key_mode[] = {
 { "TEXT",		MODE_TEXT		},
 { "BINARY",		MODE_BINARY		},
 { "FIXED",		MODE_FIXED		},
 { "DELTA",		MODE_DELTA		},
 { NULL,		0x0000			}
};

KEYWORD key_show[] = {
 { "ALL",		SHOW_PROGRESS | SHOW_STATISTICS	 | SHOW_FDL },
 { "PROGRESS",		SHOW_PROGRESS		},
 { "STATISTICS",	SHOW_STATISTICS		},
 { "FDL",		SHOW_FDL		},
 { "DEBUG",		SHOW_DEBUG		},
 { "DEBUG_SERIOUS",	SHOW_SERIOUS_DEBUG	},
 { "DEBUG_IO",		SHOW_IO_DEBUG		},
 { NULL,		0x0000			}
};

int
lzdcl(argc, argv)
int		argc;
char		*argv[];
{
	register int		i;
	register int		status;
	struct dsc$descriptor_s	temp;
	extern int		ignore_dcl_error();

#ifdef CLIDBG
	for (i = 1; i < argc; i++) {
	    printf("%2d: %s\n", i, argv[i]);
	}
#endif
	descriptor(&temp, (is_compress) ? "COMPRESS " : "DECOMPRESS ");
	if (argc <= 1) {
	    status = lib$get_foreign(	/* Read the command line	*/
		&command_line,
		(is_compress) ? &prompt_compress : &prompt_decompress);
	    if (status == RMS$_EOF)
		exit(status);
	    else if (status != SS$_NORMAL)
		lib$stop(status);
	    str$concat(&cli_prompt, &temp, &command_line);
	}
	else {
	    for (i = 1; i < argc; i++) {
		str$append(&cli_command, &temp);
		descriptor(&temp, argv[i]);
		str$append(&cli_command, &temp);
		descriptor(&temp, " ");
	    }
	}
#ifdef CLIDBG
	printf("command: \"%.*s\"\n",
	    cli_command.dsc$w_length, cli_command.dsc$a_pointer);
#endif
	VAXC$ESTABLISH(ignore_dcl_error);
	status = cli$dcl_parse(		/* Parse the DCL		*/
		&cli_command,		/* Input from lib$get_foreign	*/
		&lz_dcl_table,		/* Parsing table		*/
		&lib$get_input,	    	/* Gets a required parameter	*/
		&lib$get_input,	    	/* Gets a continuation prompt	*/
		&cli_prompt);
#ifdef CLIDBG
	printf("cli$dcl_parse returns %s\n", vms_etext(status));
#endif
	if ((status & STS$M_SUCCESS) != 0) {
	    status = SS$_NORMAL;
	    /*
	     * Process all arguments
	     */
	    infilename = cli_savevalue("INPUT");
	    if (cli_present("OUTPUT") == CLI$_PRESENT)
		outfilename = cli_savevalue("OUTPUT");
	    if (cli_present("BITS") == CLI$_PRESENT &&
		  cli_getintval("BITS") & STS$M_SUCCESS) {
		maxbits = intresult;
		if (maxbits < MIN_BITS) {
		    fprintf(stderr, "Illegal bits value %d\n", maxbits);
		    lib$signal(CLI$_IVVALU);
		    status = CLI$_IVVALU;
		}
	    }
	    cli_list("METHOD", &method, key_method);
	    cli_list("EXPORT", &export, key_export);
	    if ((export & EXPORT_UNIX) != 0)
		export &= ~EXPORT_VMS;
	    cli_list("MODE",   &mode,   key_mode);
	    cli_list("SHOW",   &show,   key_show);
	    if (!is_compress && 
		  cli_present("FIXED") == CLI$_PRESENT &&
		    cli_getintval("FIXED") & STS$M_SUCCESS) {
		fblksiz = intresult;
		if (fblksiz <= 0) {
		    fprintf(stderr, "Illegal record size %d\n", fblksiz);
		    lib$signal(CLI$_IVVALU);
		    status = CLI$_IVVALU;
		}
	    }
	}
	str$free1_dx(&command_line);
	str$free1_dx(&cli_command);
	str$free1_dx(&cli_prompt);
	str$free1_dx(&result);
	if (mode & (MODE_BINARY | MODE_FIXED))
	    mode &= ~MODE_TEXT;		/* non-text: remove preset default */
	return (status);
}

cli_list(what, flag_word, keytable)
char			*what;
flag			*flag_word;
KEYWORD			*keytable;
{
	register int		key_status;
	register int		status;
	register KEYWORD	*kp;

	switch(cli_present(what)) {
	case CLI$_NEGATED:
#ifdef CLIDBG
	    printf("found %s (negated)\n", what);
#endif
	    *flag_word = 0;
	    break;

	case CLI$_PRESENT:
	    for (kp = keytable; kp->key != NULL; kp++) {
		key_status = cli_present(kp->key);
		switch (key_status) {
		case CLI$_PRESENT:
		    *flag_word |= kp->bit;
#ifdef CLIDBG
		    printf("found %s %s\n", what, kp->key);
#endif
		    break;

		case CLI$_NEGATED:
		    *flag_word &= ~kp->bit;
#ifdef CLIDBG
		    printf("found %s %s (negated)\n", what, kp->key);
#endif
		    break;
		}
	    }
	    break;

	default:
	    break;
	}
}

int
cli_present(what)
char		*what;
/*
 * TRUE if the argument is present in the command line
 */
{
	struct dsc$descriptor_s	parm;

	descriptor(&parm, what);
	return(cli$present(&parm));
}

descriptor(descr, what)
register struct dsc$descriptor_s	*descr;
char					*what;
/*
 * Turn a C string into a (static) descriptor.
 */
{
	descr->dsc$w_length = strlen(what);
	descr->dsc$b_class = DSC$K_CLASS_S;
	descr->dsc$b_dtype = DSC$K_DTYPE_T;
	descr->dsc$a_pointer = what;
}

int
cli_getvalue(what)
char		*what;
/*
 * Get the value (storing it in result).  Return the status.
 * Note the following (successful) statuses:
 *	CLI$_COMMA		Another entry (in the list) may be read
 *	SS$_NORMAL		The last -- or only -- value.
 * A non-successful status is signaled.
 */
{
	struct dsc$descriptor_s	parm;
	int			status;

	descriptor(&parm, what);
	if (((status = cli$get_value(&parm, &result)) & STS$M_SUCCESS) == 0)
	    lib$signal(status);
	return (status);
}

int
cli_getintval(what)
char		*what;
/*
 * Get the value as an integer, storing it in intresult.
 * Status return as for cli_getvalue().
 */
{
    	register int	i, c, v;
    	int		status;

    	status = cli_getvalue(what);
    	if ((status & STS$M_SUCCESS) == 0) 
	    return (status);

	intresult = -1;

    	for (v = 0, i = 0; i < result.dsc$w_length; i++) {
	    c = result.dsc$a_pointer[i];
	    if (!isdigit(c)) {
		fprintf(stderr, "Illegal number \"%.*s\"\n",
			result.dsc$w_length, result.dsc$a_pointer);
		lib$signal(CLI$_IVVALU);
		return (CLI$_IVVALU);
	    }
	    v = (v * 10) + (c - '0');
	}

	intresult = v;
	return (status);
}

char *
cli_savevalue(what)
char		*what;
/*
 * Find the value and store it as a C string in malloc'ed memory.
 * Return NULL on errors (which should not happen).
 */
{
	register char		*string;

	if ((cli_getvalue(what) & STS$M_SUCCESS) == 0)
	    return (NULL);
	else {
	    string = malloc(result.dsc$w_length + 1);
	    strncpy(string, result.dsc$a_pointer, result.dsc$w_length);
	    string[result.dsc$w_length] = '\0';   /* !! */
	    return (string);
	}
}

dumpoptions()
/*
 * Debug: dump all option values.
 */
{
	fprintf(stderr, "(Debug) Option dump, bits = %d\n", maxbits);
	fprintf(stderr, "Input file: %s\n",
	    (infilename == NULL) ? "<unspecified>" : infilename);
	fprintf(stderr, "Output filename: %s\n",
	    (outfilename == NULL) ? "<unspecified>" : outfilename);
	option_dump("METHOD", method, key_method);
	option_dump("EXPORT", export, key_export);
	option_dump("MODE",   mode,   key_mode);
	option_dump("SHOW",   show,   key_show);
}

option_dump(what, flag_word, keytable)
char			*what;
flag			flag_word;
KEYWORD			*keytable;
{
	register KEYWORD	*kp;
	register int		first;

	for (first = TRUE, kp = keytable; kp->key != NULL; kp++) {
	    if ((flag_word & kp->bit) == kp->bit) {
		if (first) {
		    fprintf(stderr, " %s=(%s", what, kp->key);
		    first = FALSE;
		}
		else {
		    fprintf(stderr, ", %s", kp->key);
		}
		if (streq(kp->key, "FIXED")) 	/* hacky! */    
		    fprintf(stderr, "=%d", fblksiz);
	    }
	}
	if (first)
	    fprintf(stderr, " %s -- no options specified.\n", what);
	else {
	    fprintf(stderr, ")\n");
	}
}

#ifdef CLIDBG
/*
 * This is in lzvio.c, too.
 */
char *
vms_etext(errorcode)
int		errorcode;
{
	char		*bp;
	short		errlen;		/* Actual text length		*/

	lib$sys_getmsg(&errorcode, &errlen, &err, &15);
	/*
	 * Trim trailing junk.
	 */
	for (bp = &errname[errlen]; --bp >= errname;) {
	    if (isgraph(*bp) && *bp != ' ')
		break;
	}
	bp[1] = EOS;
	return(errname);
}
#endif

#define	CODE(v)	((v) & STS$M_CODE)

ignore_dcl_error(signal_arg, mech_arg)
long int	signal_arg[];
long int	mech_arg[];
/*
 * VMS exception handler.  Needs extension to handle "all" errors.
 */
{
	switch (CODE(signal_arg[1])) {
	case CODE(SS$_UNWIND):			/* Currently unwinding	*/
	    return;

	case CODE(SS$_CONTROLC):		/* CTRL/C trap		*/
	case CODE(SS$_ACCVIO):
	case CODE(SS$_ROPRAND):
	    lib$stop(signal_arg[1]);
	    break;

	default:
#ifdef CLIDBG
	    fprintf(stderr, "\n------signal------\n");
	    fflush(stderr);
#endif
	    signal_arg[0] -= 2;
	    sys$putmsg(signal_arg);
	    signal_arg[0] += 2;
#ifdef CLIDBG
	    fprintf(stderr, "\n------------------\n");
	    fflush(stderr);
#endif
	    break;
	}
	return;
}
