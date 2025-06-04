/*
 *		F P A R S E . C
 *
 * C-callable equivalent of the DCL F$PARSE lexical function.
 */

/*)LIBRARY
*/

#ifdef DOCUMENTATION

title	fparse	Parse VMS file specification a la DCL's F$PARSE
index		Parse VMS file specification a la DCL's F$PARSE

synopsis

	char *
	fparse(file_name,default_name,related_name,fields,options)
	char *file_name;			/* Optional	*/
	char *default_name;			/* Optional	*/
	char *related_name;			/* Optional	*/
	char *fields;				/* Optional	*/
	char *options;				/* Optional	*/

	extern noshare int fparse_dev;
	extern noshare int fparse_sdc;
	extern noshare int fparse_sts;
	extern noshare int fparse_stv;
	extern noshare int fparse_fnb;

description

	fparse() provides a simple interface to the SYS$PARSE() service.  Its
	interface is based on the DCL F$PARSE() lexical function.

	All strings are null-terminated.  Parameters can be omitted by passing
	NULL; trailing parameters may be omitted completely.

	Fields are:

	    "node"		node name
	    "device"		device name
	    "directory"		directory name
	    "name"		file name
	    "type"		file type
	    "version"		file version number

	Options to control the parse:

	    "noconceal"		Don't conceal device name
	    "pwd"		Leave actual DECnet password in place
	    "synchk"		No I/O - check syntax only
	    "ofp"		Do an output file parse

	Any number of field names and options may be specified in any order,
	separated by commas.  Only the first one or two characters of field
	and option names are checked (enough to identify the name uniquely).
	All field and options names must be given in lower case only.

	If the fields parameter is NULL or omitted, all fields are returned,
	except that the node name is included only if it appears explicitly.
	(This is the underlying behavior of SYS$PARSE.)

	The value returned is the address of the null-terminated expanded
	specification.  fparse calls malloc() to reserve space for the string;
	you should free it with free() when you are done with it.

	NULL is returned on either an RMS parse error, or an erroneous field
	parameter name or option.

	The RMS status is is available in symbol fparse_sts; for a bad field
	or option, it will contain SS$_BADPARAM.  If malloc() returns NULL,
	so does fparse(), but in this case fparse_sts will contain contain
	RMS$_NORMAL.  Other values returned as a result of the parse are
	available as follows:

	FAB$L_DEV	fparse_dev
	FAB$L_SDC	fparse_sdc
	FAB$L_STS	fparse_sts
	FAB$L_STV	fparse_stv
	NAM$L_FNB	fparse_fnb

	Wild-card context for the fab used is always discarded by fparse();
	it cannot be used to set up the control blocks for a call to $SEARCH.
	Certain additional RMS fields - FAB$W_DID, for example - are not
	available.

examples

	In determining the actual file specifications to use on a command
	line such as LINK/EXE=exefile objfile, the default type for "objfile"
	is .OBJ.  The default file name for "exefile" is the file name of
	"objfile", and the default type is .EXE.

	If the (char *) variable "objfile" points to the object file name from
	the command line, and the (char *) "exefile" points to the exe file
	from the command line.  Then to expand these into file names for calls
	to open() or fopen():

		objfile = fparse(objfile,".OBJ");
		exefile = fparse(exefile,".EXE",objfile,NULL,"ofp");

	(The "ofp" option was specified to avoid copying anything but the
	name and type from the object file - the exe file should go, for
	example, to the current default directory, not the object file's
	directory.)

	To find only the device and directory of the object file:

		dir = fparse(objfile,".OBJ",NULL,"device,directory");

	This could as well have been written:

		dir = fparse(objfile,".OBJ",NULL,"de,di");

bugs

	Since the various fparse_... fields are statically allocated, the
	code is not reentrant.

	Neither fparse(), nor any approach that relies on the string values
	of the previous files in a parse, can properly handle related file
	specifications involving search lists.

authors

	Bob Messenger; re-written by Jerry Leichter

#endif

/*
 * Revision History
 *	1.0 ??-???-85	 BM	Invention
 *	2.0  6-Nov-86	JSL	Extensive re-write; added options, ability
 *				to specify multiple fields at once.  Clean
 *				up fab context before returning.  Now returns
 *				NULL, not an empty string, on errors, and
 *				makes the various RMS values accessible.
 *	2.1 28-SEP-00	Goatley		Add include for string.h
 */

#include <stdio.h>
#include <rms.h>
#include <ssdef.h>
#include <varargs.h>
#include <string.h>

#define EOS	'\0'

static char *copyn();
extern char *malloc();

noshare int fparse_dev;
noshare int fparse_sdc;
noshare int fparse_sts;
noshare int fparse_stv;
noshare int fparse_fnb;

char *
fparse(va_alist)
va_dcl
{	char		*file;
	char		*defaultn;
	char		*related;
	char		*fields;
	char		*options;
	struct FAB	fab;
	struct NAM	nam;
	struct NAM 	rnam;
	char		*result;
	int		len;
	char		expand_buf[NAM$C_MAXRSS];
	int		nargs;
	va_list		va;
	int		fbits;
#define NODE	 1
#define DEV	 2
#define DIR	 4
#define	NAME	 8
#define TYPE	16
#define	VER	32
#define ALL	(NODE|DEV|DIR|NAME|TYPE|VER)

	/*
	 * Pick up the arguments, forcing all missing ones to NULL
	 */
	va_count(nargs);
	va_start(va);
	file		= (nargs > 0) ? va_arg(va,char *) : NULL;
	defaultn	= (nargs > 1) ? va_arg(va,char *) : NULL;
	related		= (nargs > 2) ? va_arg(va,char *) : NULL;
	fields		= (nargs > 3) ? va_arg(va,char *) : NULL;
	options		= (nargs > 4) ? va_arg(va,char *) : NULL;
	va_end(va);

	/*
	 * Initialize all the blocks for RMS
	 */
	fab = cc$rms_fab;
	fab.fab$l_nam = &nam;
	fab.fab$l_fna = file;
	fab.fab$b_fns = (file == NULL) ? 0 : strlen(file);
	fab.fab$l_dna = defaultn;
	fab.fab$b_dns = (defaultn == NULL) ? 0 : strlen(defaultn);

	nam = cc$rms_nam;
	nam.nam$l_esa = expand_buf;
	nam.nam$b_ess = NAM$C_MAXRSS;
	nam.nam$l_rlf = &rnam;

	rnam = cc$rms_nam;
	rnam.nam$l_rsa = related;
	rnam.nam$b_rsl = (related == NULL) ? 0 : strlen(related);

	fparse_sts = SS$_BADPARAM;		/* Assume failure	*/
	/*
	 * Figure out what fields the user wants returned
	 */
	if (fields == NULL || *fields == EOS)
		fbits = ALL;
	else
	{	fbits = 0;
		while (*fields != EOS)
		{	switch (*fields)
			{
		case 'n':				/* node or name	*/
				if (*++fields == 'o')	/* node		*/
					fbits |= NODE;
				else if (*fields == 'a')/* name		*/
					fbits |= NAME;
				else	return(NULL);	/* n?		*/
				break;
		case 'd':			/* device or directory	*/
				if (*++fields == 'e')	/* device	*/
					fbits |= DEV;
				else if (*fields == 'i')/* directory	*/
					fbits |= DIR;
				else	return(NULL);	/* d?		*/
				break;
		case 't':				/* type		*/
				fbits |= TYPE;
				break;
		case 'v':				/* version	*/
				fbits |= VER;
				break;
		default:				/* Unknown	*/
				return(NULL);
			}
			while (*fields != ',' && *fields != EOS)
				fields++;
			if (*fields != EOS)
				fields++;
		}
	}

	/*
	 * Scan options and set the appropriate RMS bits.
	 */
	if (options != NULL)
	{	while (*options != EOS)
		{	switch(*options)
			{
		case 'n':				/* noconceal	*/
				nam.nam$b_nop |= NAM$M_NOCONCEAL;
				break;

		case 'o':				/* ofp		*/
				fab.fab$l_fop |= FAB$M_OFP;
				break;

		case 'p':				/* pwd		*/
				nam.nam$b_nop |= NAM$M_PWD;
				break;

		case 's':				/* synchk	*/
				nam.nam$b_nop |= NAM$M_SYNCHK;
				break;

		default:				/* unknown	*/
				return(NULL);
			}
			while (*options != ',' && *options != EOS)
				options++;
			if (*options != EOS)
				options++;
		}
	}

	/*
	 * call SYS$PARSE to parse the file name
	 */
	fparse_sts = SYS$PARSE(&fab);
	fparse_dev = fab.fab$l_dev;
	fparse_sdc = fab.fab$l_sdc;
	fparse_stv = fab.fab$l_stv;
	fparse_fnb = nam.nam$l_fnb;
	if (fparse_sts != RMS$_NORMAL)
		return(NULL);

	/*
	 * Calculate the total length of the string we will be returning.
	 * In the process, zero out the lengths of the fields we are not
	 * returning to simplify the later copy code.
	 */
	len = 0;
	if (fbits & NODE)
		len += nam.nam$b_node;
	else	nam.nam$b_node = 0;

	if (fbits & DEV)
		len += nam.nam$b_dev;
	else	nam.nam$b_dev = 0;

	if (fbits & DIR)
		len += nam.nam$b_dir;
	else	nam.nam$b_dir = 0;

	if (fbits & NAME)
		len += nam.nam$b_name;
	else	nam.nam$b_name = 0;

	if (fbits & TYPE)
		len += nam.nam$b_type;
	else	nam.nam$b_type = 0;

	if (fbits & VER)
		len += nam.nam$b_ver;
	else	nam.nam$b_ver = 0;

	/*
	 * Construct the expanded file name
	 */
	if ((result = malloc(len + 1)) != NULL)
	{	char	*p;

		p = copyn(result,nam.nam$l_node,nam.nam$b_node);
		p = copyn(p,nam.nam$l_dev,nam.nam$b_dev);
		p = copyn(p,nam.nam$l_dir,nam.nam$b_dir);
		p = copyn(p,nam.nam$l_name,nam.nam$b_name);
		p = copyn(p,nam.nam$l_type,nam.nam$b_type);
		p = copyn(p,nam.nam$l_ver,nam.nam$b_ver);
		*p++ = EOS;
	}

	/*
	 * Release any space reserved by RMS for wild-card expansion
	 */
	nam.nam$b_nop = NAM$M_SYNCHK;
	fab.fab$b_dns = 0;
	nam.nam$l_rlf = NULL;
	SYS$PARSE(&fab);

	return(result);
}

/*
 * copyn n >= 0 characters from in to out; return (out + n), i.e., the next
 * free character after the characters copied.
 */
static char *
copyn(out,in,n)
char	*out;
char	*in;
int	n;
{
	while (n-- > 0)
		*out++ = *in++;
	return(out);
}

#ifdef TESTING
/*
 * Simple test program
 */
main()
{	char a[200],b[200],c[200],d[200],e[200];
	char *r;

	for (;;)
	{	printf("file name: ");
		gets(a);
		printf("default name: ");
		gets(b);
		printf("related name: ");
		gets(c);
		printf("fields: ");
		gets(d);
		printf("options: ");
		gets(e);
		r = fparse(a,b,c,d,e);
		if (r == NULL)
			printf("[Null]\n");
		else
		{	puts(r);
			free(r);
		}
		printf("sts %X, stv %X, dev %X, sdc %X, fnb %X\n",
			fparse_sts, fparse_stv, fparse_dev, fparse_sdc, 
			fparse_fnb);
		if (fparse_sts != RMS$_NORMAL)
		{	printf("sts: %s\n",vms_etext(fparse_sts));
			if (fparse_stv != 0)
				printf("stv: %s\n",vms_etext(fparse_stv));
		}
	}
}
#endif
