#ifndef NO_IDENT
static char *Id = "$Id: dclarg.c,v 1.8 1995/10/27 23:05:48 tom Exp $";
#endif

/*
 * Title:	dclarg.c
 * Author:	T.E.Dickey
 * Created:	24 May 1984
 * Last update:
 *		27 Oct 1995, suppress DNF-error so "create directory" works.
 *		18 Mar 1995, prototyped
 *		05 Dec 1989, patched out VMS-bug (PATCH_DEC89)
 *		20 Mar 1989, bypass VMS bug which returns occasional illegal
 *			     status code.
 *		11 Jul 1988, permit '-' in filenames.
 *		30 Sep 1985, added this comment!
 *		23 Sep 1985, added this comment!
 *		08 May 1985, added this comment!
 *		27 Apr 1985, added visible-test for sid here to confuse thieves.
 *		28 Jan 1985, the search here is done only if 'cpy_dft' is set,
 *			     to provide (temporary?) support for FLIST/COPY.
 *			     Patch: remove search ultimately, since this does
 *			     not provide support for multiple-outputs.
 *		17 Jan 1985, options on command-keyword should be always set
 *			     with mfld=0.  That way, we consistently number
 *			     "CMD/OPT A,B" and "CMD A/OPT,B".
 *		26 Dec 1984, added NAM-block stuff to reduce re-parsing.
 *		15 Dec 1984, added 'cpy_dft' argument.
 *		14 Oct 1984, fixed bug in option-value parsing
 *		10 Sep 1984, use "rmsinit"
 *		06 Sep 1984, filespec may begin with '_'
 *		26 Aug 1984, cleanup buffer sizes
 *		06 Aug 1984, added 'dclarg_keyw' entrypoint
 *		04 Aug 1984, test tabs/spaces as blanks, misc.
 *		01 Aug 1984, corrected default-field propagation
 *		24 Jul 1984, added 'cmd_arg' flag
 *		13 Jul 1984, fixes for "[x/x]", unparsed names.
 *		04 Jul 1984, option keyword is a/n + '_'
 *		27 Jun 1984, added ',' check to support UIC-directories.
 *		26 Jun 1984, to add '_from', '_size' data for error reporting
 *		25 Jun 1984, to add 'dclarg_text' procedure
 *		19 Jun 1984, to add '$' to name-characters
 *		28 May 1984
 *
 * Function:	This procedure splits a DCL-like command string into a series
 *		of strings in a linked list.  The objective is to permit the
 *		calling program to perform simple syntax analysis of a string
 *		to detect (and parse) options, and filenames, particularly in
 *		setting up SPAWN-calls.  It does some of the functions of
 *		DEC's DCL:
 *
 *		   (a)	Punctuation is used to parse the string.  DCLARG tests
 *			for '/' (to begin option fields) and parentheses (to
 *			group items for propagating defaults) and commas (also
 *			for defaulting).
 *
 *		   (b)	The parsed list is returned as a linked list.  No
 *			attempt is made to decode option values; these are
 *			part of the same string as the "/" options.
 *
 *		No tests are made for the fancier features of DCL (quoted
 *		names, or nested parentheses).  Nor is any attempt made to
 *		default filetypes, etc., in option-strings which may be
 *		filenames.
 *
 *		By observing the VMS TYPE and COPY programs, the following
 *		rules for defaulting have been adopted:
 *
 *		   (a)	The initial default is used until overridden by
 *			explicit use (e.g., CMD A.TMP overrides ".LIS").
 *		   (b)	If an output is used, it defaults by the first
 *			name returned by SYS$SEARCH from the first input
 *			specification (e.g., COPY A.*,B C will default C's
 *			filetype to the first filetype found for A).
 *		   (c)	The above rule, however, applies only if the first
 *			parameter is a list.  If it is not, use as default
 *			the expanded string (from SYS$PARSE).  This permits
 *			support for "COPY A.* B" (i.e., "B" inherits a wildcard
 *			in the filetype).
 *
 * Parameters:	inp_	=> string containing DCL to parse
 *		dft_	=> string defining defaults for filenames in list
 *			(e.g., for PRINT, assume ".LIS").  It may not
 *			specify the directory, since this defaults in a list.
 *		cmd_arg	=  A count of the number of leading tokens which are
 *			command verbs.  Normally this is 1 or 0 (the verb was
 *			already absorbed).  It is "2" in the special case of
 *			functions (BROWSE) called from FLIST, since the
 *			procedure 'argvdcl' does not know about this special
 *			case.  The counter specifies the number of tokens to
 *			pass before treating them as filenames (and propagating
 *			defaults, and related-name fields).
 *		cpy_dft = A flag which is TRUE if we use the defaulting rule
 *			for COPY.  The output file in this case does not
 *			inherit the input's directory name, but (implicitly)
 *			the current default device/directory.
 *
 * Returns:	A pointer to the resulting linked-list of DCLARG structures.
 *		If the list is blank, the pointer is null.  No direct error
 *		reporting is performed from this routine; entries containing
 *		errors have a nonzero '.dcl_stat' entry (-1 if non-RMS).
 *
 * Entry:	dclarg:		Main entry of this module
 *		dclarg_text:	Allocate/reallocate DCLARG-link
 *		dclarg_init:	Initialize 'dclarg', 'dclarg_make'
 *		dclarg_spec:	Skip string pointer past filespec
 *		dclarg_keyw:	Skip string pointer past option keyword.
 *
 * Patch:	This procedure does not properly detect such syntax errors as
 *
 *			COMMAND/NAME.TYP (should be illegal delimiter '.')
 *			COMMAND,NAME (can be tested by 'dclinx (p, 0, 1)')
 *			COMMAND	FILE.TYP.TYP  (except as an illegal filename)
 */

#include	<starlet.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<rms.h>
#include	<ctype.h>
#include	<string.h>

#include	"bool.h"
#include	"crt.h"
#include	"dclarg.h"
#include	"rmsinit.h"

/*
 * Forward declarations and external procedures:
 */
static	DCLARG	*dclarg_make (DCLARG *last_, char *s_, char *cmd, int from, int size, struct NAM *nam_);
static	int	dclarg_bracket (char c);
static	int	dclarg_name (char c);
static	void	dclarg__copy (char *dft, struct NAM *nam_, int cpy_dft);
static	void	dclarg_init (void);

#define	SPC(s)	while (isspace(*s))	s++

#define	FROM	inp_, (j_-inp_), (i_-j_)

#define	STRNCPY(out,in,xlen)	{strncpy(out, in, len = (xlen));\
				 out[len] = EOS;}

static	DCLARG	*first_;	/* Pointer to linked-list		*/
static	unsigned status;	/* RMS error code, or 0 (ok) or -1 (own)*/
static	int	mfld;		/* Increment between grouped names	*/
static	int	sfld;		/* Increment after ","			*/

static	char	err_null[] = "null file specification";
static	char	err_parm[] = "invalid parameter delimiter";

DCLARG	*dclarg(char *inp_, char *dft_, int cmd_arg, int cpy_dft)
{
	DCLARG	*arg_	= 0;
	struct	FAB	fab;
	struct	NAM	nam,	nam2;
	int	len,			/* (misc) string length		*/
		use_dna2 = FALSE;	/* TRUE if 'dna2[]' is default	*/
	char	rsa	[NAM$C_MAXRSS],	/* resultant string (SYS$SEARCH)*/
		esa	[NAM$C_MAXRSS],	/* expanded string (SYS$PARSE)	*/
		dna	[NAM$C_MAXRSS],	/* next 'sfld' default string	*/
		dna2	[NAM$C_MAXRSS],	/* next 'mfld' default if list	*/
		dna3	[NAM$C_MAXRSS],	/* next 'mfld' default if single*/
		cmdtok	[NAM$C_MAXRSS],
		*i_	= inp_,		/* beginning of item to parse	*/
		*j_,			/* end+1 of item to parse	*/
		*k_;			/* miscellaneous pointer	*/

	if (!dft_)	dft_ = "";
	strcpy (dna, dft_);
	strcpy (dna2,dft_);
	strcpy (dna3,dft_);

	dclarg_init ();
	for (;;)
	{
	    SPC(i_);	/* Ignore blanks between tokens */

	    j_ = i_;
	    if (*i_ == EOS)
		return (first_);
	    /*
	     * Options begin with "/", and are followed by:
	     *	(a) an optional alphanumeric+'_' name, then
	     *	(b) an optional (if name given) "="
	     *	(c) an option-value if "=", perhaps enclosed in
	     *	    "(" and ")".
	     *
	     * SET-commands may use an "=" without a preceding "/"
	     * (e.g., "SET PROTECTION=(OWN:RE)").
	     */
	    else if (isopt(*i_))
	    {
		if (*i_ == '/')
		{
		    i_++;
		    SPC(i_);
		    i_ = dclarg_keyw (i_);
		    k_ = i_;
		    SPC(k_);
		    if (isopt2(*k_))	i_ = k_;
		}
		if (isopt2(*i_))
		{
		    i_++;
		    SPC(i_);
		    if (*i_ == '(')
		    {
			while (*i_ && (*i_ != ')'))	i_++;
			if (*i_ == ')')			i_++;
		    }
		    else
			i_ = dclarg_spec (i_, "-");
		}
		status = 0;
		STRNCPY(esa, j_, i_ - j_);
		arg_ = dclarg_make (arg_, esa, FROM, (struct NAM *)0);
	    }
	    /*
	     * File names contain a mixture of ".", ":", ";", alphanumeric
	     * characters, and "[", "]" to delimit directories.  Many
	     * DCL-oriented programs permit a list of filenames separated
	     * by comma to indicate that the directory defaults.  Finally,
	     * wildcard characters "*" and "%" are used.
	     *
	     * Perform enough parsing here to pass along the defaults to
	     * the calling program.
	     */
	    else if (dclarg_name(*i_))
	    {
		i_ = dclarg_spec (i_, 0);
		if (cmd_arg > 0)
		{
		    STRNCPY(cmdtok, j_, i_ - j_);
		    arg_ = dclarg_make (arg_, cmdtok, FROM, (struct NAM *)0);
		}
		else
		{
		    if (mfld <= 0)	mfld = 1, sfld = 0;
		    rmsinit_fab (&fab, &nam,
					(sfld	? dna
						: (use_dna2 ? dna2
							    : dna3)), 0);
		    fab.fab$l_fna	= j_;
		    fab.fab$b_fns	= i_-j_;

		    rmsinit_nam (&nam, rsa, esa);
		    status = sys$parse(&fab);

		    /*
		     * If we're referring to a directory that doesn't exist,
		     * the parse will return a directory-not-found (something
		     * that we'd normally expect from sys$search).  That
		     * breaks the "create" command.
		     */
		    if (status == RMS$_DNF)
		    	status = 0;

		    /*
		     * Use the most recent name as a default name for the next
		     * parse:
		     */
		    STRNCPY(dna, esa, nam.nam$b_esl);
		    nam2 = nam;		/* Save to use in 'dclarg_make' */

		    /*
		     * If this was the first item in any list, (sfld==0),
		     * obtain the parsed string, and the first-found from a
		     * search for the defaults 'dna3' and 'dna2', respectively.
		     */
		    if (sfld == 0)
		    {
			dclarg__copy (dna3, &nam, cpy_dft);
			if (cpy_dft)
			{
			    sys$search (&fab);
			    dclarg__copy (dna2, &nam, cpy_dft);
			}
			use_dna2 = FALSE;
		    }

		    /*
		     * Make a new DCLARG-entry:
		     */
		    arg_ = dclarg_make (arg_,
				(nam.nam$b_esl ? dna : j_), FROM, &nam2);
		}
	    }
	    else
	    {
		while (!dclarg_name(*i_) && !isopt(*i_))	i_++;
		status = -1;
		arg_ = dclarg_make (arg_, err_parm, FROM, (struct NAM *)0);
	    }

	    SPC(i_);
	    if (*i_ == ',')
	    {
		use_dna2 = cpy_dft; 	/* COPY-output inherits 1st input */
		i_++; sfld++;
		SPC(i_);
		j_ = i_;
		while (isspace(*i_) || *i_ == ',')	i_++;
		if (j_ != i_ || isopt(*i_))
		{
		    status = -1;
		    arg_ = dclarg_make (arg_, err_null, FROM, (struct NAM *)0);
		}
	    }
	    else if (!isopt(*i_))
	    {
		strcpy (dna, dft_);
		if (cmd_arg <= 0)
		{
		    mfld++;
		    sfld = 0;
		}
	    }

	    if (cmd_arg > 0)
		cmd_arg--;	/* (On zero, look for filenames) */
	}
}

/* <dclarg__copy>:
 * Copy default-strings from the first file-specification in a main-field.
 * If the 'cpy_dft' flag is set, don't copy the pathname, but begin with the
 * filename.
 */
static
void	dclarg__copy (char *dft, struct NAM *nam_, int cpy_dft)
{
	char	*c_	= nam_->nam$l_node;
	int	len	= nam_->nam$b_ver + (nam_->nam$l_ver - c_),
		adj	= 0;

	if (cpy_dft)
	{
		adj = nam_->nam$l_name - c_;
		len -= adj;
		c_  += adj;
	}
	strncpy (dft, c_, len);
	dft[len] = EOS;
}

/* <dclarg_text>:
 * Allocate (or re-allocate) a 'DCLARG' entry, given the text-buffer to load.
 * This procedure can be used to allocate a new entry (if 'this_' is null), or
 * to reallocate an entry when the text buffer's size has altered.  The status
 * and field indices are unaltered.
 */
DCLARG	*dclarg_text(
	DCLARG	*this_,		/* Entry to (re)allocate	*/
	DCLARG	*last_,		/* Entry containing link to it	*/
	char	*s_,		/* Text-buffer (mustn't be null)*/
	int	uc)		/* TRUE if uppercase		*/
{
	size_t	size	= sizeof(DCLARG) + strlen(s_) + 2;
	char	*text_;

	if (this_)	this_	= realloc (this_, size);
	else		this_	= calloc (1, size);

	this_->dcl_size	= strlen(s_);
	text_	= (char *) this_ + sizeof(DCLARG);

	/*
	 * Strip out blanks from the string as I load it.  (patch: Should look
	 * for quoted strings here.)
	 */
	for (this_->dcl_text = text_; *s_; s_++)
	{
		if (!uc || !isspace(*s_))	*text_++ = *s_;
	}
	*text_ = EOS;

	/*
	 * Do uppercase conversion if needed.  This is used for all filenames
	 * and option strings.  Error messages are left alone.
	 */
	if (uc)
	{
		for (text_ = this_->dcl_text; *text_; text_++)
			*text_ = _toupper(*text_);
	}
	if (last_ != 0)
		last_->dcl_next = (DCLARG *)this_;
	/*
	 * Don't allow empty name ".;" -- trim it off
	 */
	if ((size = strlen(text_ = this_->dcl_text)) > 2)
	{
		if (!strcmp(text_ + size - 2, ".;"))
			text_[size-2] = EOS;
	}
	return (this_);
}

/* <dclarg_make>:
 * Form a new entry in the linked-list of parsed items
 */
static
DCLARG	*dclarg_make (
	DCLARG	*last_,		/* last entry in DCLARG-list	*/
	char	*s_,		/* option, filename, or message	*/
	char	*cmd,
	int	from,		/* index and length in command	*/
	int	size,		/* index and length in command	*/
	struct	NAM *nam_)	/* => NAM-block, if we use result */
{
	DCLARG	*this_;
	char	bfr[CRT_COLS];	/* Assume command is narrower than screen */

#ifdef	PATCH_DEC89
	if (status == (RMS$_NORMAL & ~7)) {
		status = RMS$_NORMAL;
		flist_log("! dclarg found parse-bug");
	}
#endif	/* PATCH_DEC89 */

	if (status == -1)
	{
		sprintf (bfr, "%s: \\%.8s\\", s_, &cmd[from]);
		s_ = bfr;
	}
	this_ = dclarg_text (0, last_, s_, (status == RMS$_NORMAL));

	this_->dcl_stat = (status == RMS$_NORMAL) ? 0 : status;
	this_->dcl_mfld = mfld;
	this_->dcl_sfld = sfld;
	this_->dcl_from = from;
	this_->dcl_size = size;
	if (nam_)
	{
		this_->dcl$l_fnb	= nam_->nam$l_fnb;
		this_->dcl$b_node	= nam_->nam$b_node;
		this_->dcl$b_dev	= nam_->nam$b_dev;
		this_->dcl$b_dir	= nam_->nam$b_dir;
		this_->dcl$b_name	= nam_->nam$b_name;
		this_->dcl$b_type	= nam_->nam$b_type;
		this_->dcl$b_ver	= nam_->nam$b_ver;
	}
	if (!last_)
		first_ = this_;
	return (this_);
}

/* <dclarg_init>:
 * Initialize the state used in 'dclarg_make' to enable external use of
 * the routine:
 */
static
void	dclarg_init (void)
{
	status = mfld = sfld = 0;
}

/* <dclarg_spec>:
 * Skip a character-pointer past a filespec, option-keyword (including date).
 */
char	*dclarg_spec (char *i_, char *also)
{
	int	brackets = 0;

	if (!also)	also = "$";

	while ( *i_ && (	dclarg_name(*i_)
			||	strrchr(also, *i_)
			||	(*i_ == ',' && brackets) )
		)
	{
		brackets += dclarg_bracket(*i_);
		if (brackets < 0)	brackets = 0;
		i_++;
	}
	return (i_);
}

/* <dclarg_name>:
 * Test a character to see if it is part of a legal VMS wildcard-file spec.
 */
static
int	dclarg_name(char c)
{
	return (isalnum(c)
	||	c == '-'
	||	c == '_'
	||	c == '$'
	|| 	c == '.' || c == ':' || c == ';'
	||	dclarg_bracket (c)
	||	c == '*' || c == '%');
}

/* <dclarg_bracket>:
 * VMS/DCL permits either '<','>' or '[',']' to delimit directory pathname.
 * Test for either.
 */
static
int	dclarg_bracket (char c)
{
	switch (c)
	{
	case '[':
	case '<':
		return (1);
	case ']':
	case '>':
		return (-1);
	default:
		return (0);
	}
}

/* <dclarg_keyw>:
 * Skip a character-pointer past an option keyword only.
 */
char	*dclarg_keyw (char *c_)
{
	while (isalnum(*c_) || *c_ == '_')	c_++;
	return (c_);
}
