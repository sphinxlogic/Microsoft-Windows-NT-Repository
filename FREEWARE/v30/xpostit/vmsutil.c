
/* Quick and dirty VMS utility to do all the things normally found on
   VMS systems.
*/

#include <ssdef>
#include <rms>
#include <descrip>
#include <stdio>


/* Macros to initialize VMS descriptors */

#define	$INIT_VMSDSC(dsc, len, addr)		\
	((dsc .dsc$b_class) = DSC$K_CLASS_S,	\
	 (dsc .dsc$b_dtype) = DSC$K_DTYPE_T,	\
	 (dsc .dsc$w_length) = (len),		\
	 (dsc .dsc$a_pointer) = (addr),		\
	 (& (dsc) )				\
	)

#define PARSE$M_NONE			( 0 )
#define PARSE$M_NODE			( 1 << 0 )
#define PARSE$M_DEV			( 1 << 1 )
#define PARSE$M_DIR			( 1 << 2 )
#define PARSE$M_NAME			( 1 << 3 )
#define	PARSE$M_TYPE			( 1 << 4 )
#define PARSE$M_VER			( 1 << 5 )
#define PARSE$M_HIDEPWD			( 1 << 6 )
#define PARSE$M_SYNTAXONLY		( 1 << 7 )
#define PARSE$M_ALL			( -1)


#ifndef NULL
#define NULL 0L
#endif

#ifndef TRUE
#define TRUE 1
#endif

bzero(dest, len)
char	* dest;
int	len;
{
LIB$MOVC5( &0, 0, &0, &len, dest);
}

bcopy(src, dest, len)
char	* src;
char	* dest;
int	len;
{
LIB$MOVC3(& len, src, dest);
}

char * rindex(str, c)
char	* str;
char	c;
{
char	* ptr, * temp_ptr;

for (ptr = str, temp_ptr = NULL; *ptr != '\0'; ptr++) 
    if (*ptr == c) temp_ptr = ptr;
return(temp_ptr);
}

char * index(str, c)
char	* str;
char	c;
{
char	* ptr = str;

for (ptr = str; *ptr != '\0'; ptr++) 
    if (*ptr == c) return(ptr);
return(NULL);
}


#define DIR long

typedef struct direct
    {
    char d_name[256];
    char d_type[128];
    } direct;

static 	long	dir_context;
char	dir_name[512] = "";
struct direct current_file;


DIR	* opendir(d)
char	* d;
{
dir_context = 0;
strcpy(dir_name, d);
return(& dir_context);
}



char * readdir(d)
DIR 	* d;
{
char	buf[512], file_name[512], just_fname[256];
struct dsc$descriptor	dir_dsc, file_dsc;
long    rms_status, status, just_fname_len;
int	i;

$INIT_VMSDSC(file_dsc, 511, file_name);

sprintf(buf,"%s*.*",dir_name);
$INIT_VMSDSC(dir_dsc, strlen(buf),buf);

status = lib$find_file(& dir_dsc, & file_dsc, d, 0, 0, & rms_status, 0);
if ((status != SS$_NORMAL) && (status != RMS$_NORMAL))
    return(0);

bzero(just_fname,sizeof(just_fname));

status = parse_filespec(file_dsc.dsc$w_length, file_name, 255, 
			just_fname, & just_fname_len, PARSE$M_NAME);

if (!status)
    return(0);

for (i=0; i < just_fname_len; i++)
    just_fname[i]=tolower(just_fname[i]);

bzero( & current_file, sizeof(current_file));
strncpy( current_file . d_name, just_fname, just_fname_len);

return(&current_file);
}


closedir(d)
DIR	* d;
{
if (* d != 0)
    lib$find_file_end( d);

* d = 0;
dir_name[0] = '\0';
}


unlink (fname)
char * fname;
{
int status;
struct dsc$descriptor dsc, def_dsc;
$INIT_VMSDSC(dsc, strlen(fname), fname);
$INIT_VMSDSC(def_dsc, strlen(".*;*"), ".*;*");
status = lib$delete_file(& dsc, & def_dsc);
if (status != SS$_NORMAL)
    fprintf(stderr,"Error %d - Can't delete file: %s\n", status, fname);

}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * parse_filespec
 * ------------------
 * Parses and expands a file specification.  Parsing errors are returned.
 *
 *	filespec_len		: int			: read		: value
 *				: (IN) Length of filespec.
 *
 *	filespec_str		: char []		: read		: ref
 *				: (IN) Filespec string.
 *
 *	exp_buf_len		: int			: read		: ref
 *				: (IN) Length of parse buffer.
 *
 *	exp_buf_ptr		: ASCIZ char []		: write		: ref
 *				: (OUT) Parsed filespec information.
 *
 *	out_buf_len		: int			: write		: ref
 *				: (OUT) Actual length parsed information.
 *
 *	flags			: int		: read	: value
 *				: (IN) Flags: (PARSE$M_)
 *				:   HIDEPWD	: hides the password in node
 *				:		:   access string.
 *				:   NODE	: return the node with access
 *				:		:   string and "::".
 *				:   DEV		: return the device with ":".
 *				:   DIR		: return the directory with
 *				:		:   brackets.
 *				:   NAME	: return the file name.
 *				:   TYPE	: return the type with ".".
 *				:   VER		: return the version number
 *				:		:   with ";" or ".".
 *				: SYNTAXONLY	: syntax check which also
 *				:		:   uppercases the filespec.
 *
 *-----------------------------------------------------------------------------
 *
 * value:
 *	TRUE	- valid filespec.
 *	other	- invalid filespec.
 *
 * errors returned:
 *	any errors from RMS.
 *
 * errors signalled:
 *	- any other errors -
 *
 *-----------------------------------------------------------------------------
 */
int
parse_filespec ( filespec_len, filespec_str, exp_buf_len, exp_buf_ptr, 
			out_buf_len, flags )

int	  	filespec_len;
char		* filespec_str;
int		exp_buf_len;
char		* exp_buf_ptr;
int		* out_buf_len;
int		flags;

{
struct FAB	fab;
struct NAM	nam;
char		buffer [255 + 1], * buf_ptr;
int		buf_len, status, len;

char		* curptr;
int		curlen;

/*........................................................................*/

buf_len = sizeof (buffer) - 1;
buf_ptr = buffer;

if (buf_len > 255)
    buf_len = 255;

/*    Initialize the FAB and NAM structures.    */

fab = cc$rms_fab;
nam = cc$rms_nam;

fab .fab$l_nam = & nam;
fab .fab$b_fns = filespec_len;
fab .fab$l_fna = filespec_str;

if ( flags & PARSE$M_SYNTAXONLY )
    {
    nam . nam$b_nop |= NAM$M_SYNCHK;
    }
else
    {
    nam .nam$b_ess = buf_len;
    nam .nam$l_esa = buf_ptr;
    }

/*    If the caller wants to hide the access string password, do so.    */

if ( !(flags & PARSE$M_HIDEPWD) )
    {
    nam .nam$b_nop |= NAM$M_PWD;
    }

/*    Parse the filespec string.    */

status = SYS$PARSE (& fab, 0, 0);

/* If we are just performing a syntax check then uppercase the string and
   return. */

if ( flags & PARSE$M_SYNTAXONLY )
    {

    * out_buf_len = filespec_len;
    exp_buf_ptr = filespec_str;

    status = TRUE;
    return (status);
    }

/*    Get the strings we want.    */

curptr = exp_buf_ptr;
curlen = 0;

if ( flags & PARSE$M_NODE )
    {
	len = nam .nam$b_node;

	if ( len + curlen > exp_buf_len )
	    {
	    len = exp_buf_len - curlen;
	    }

	LIB$MOVC3( & len, nam .nam$l_node, curptr );

	curptr += nam .nam$b_node;
	curlen += nam .nam$b_node;
    }

if ( flags & PARSE$M_DEV )
    {
	len = nam .nam$b_dev;

	if ( len + curlen > exp_buf_len )
	    {
	    len = exp_buf_len - curlen;
	    }

	LIB$MOVC3( & len, nam .nam$l_dev, curptr );

	curptr += nam .nam$b_dev;
	curlen += nam .nam$b_dev;
    }

if ( flags & PARSE$M_DIR )
    {
	len = nam .nam$b_dir;

	if ( len + curlen > exp_buf_len )
	    {
	    len = exp_buf_len - curlen;
	    }

	LIB$MOVC3( &len, nam .nam$l_dir, curptr );

	curptr += nam .nam$b_dir;
	curlen += nam .nam$b_dir;
    }

if ( flags & PARSE$M_NAME )
    {
	len = nam .nam$b_name;

	if ( len + curlen > exp_buf_len )
	    {
	    len = exp_buf_len - curlen;
	    }

	LIB$MOVC3( &len, nam .nam$l_name, curptr );

	curptr += nam .nam$b_name;
	curlen += nam .nam$b_name;
    }

if ( flags & PARSE$M_TYPE )
    {
	len = nam .nam$b_type;

	if ( len + curlen > exp_buf_len )
	    {
	    len = exp_buf_len - curlen;
	    }

	LIB$MOVC3( &len, nam .nam$l_type, curptr );

	curptr += nam .nam$b_type;
	curlen += nam .nam$b_type;
    }

if ( flags & PARSE$M_VER )
    {
	len = nam .nam$b_ver;

	if ( len + curlen > exp_buf_len )
	    {
	    len = exp_buf_len - curlen;
	    }

	LIB$MOVC3( &len, nam .nam$l_ver, curptr );

	curptr += nam .nam$b_ver;
	curlen += nam .nam$b_ver;
    }

/*    Return length of output.    */

* out_buf_len = curlen;

/*    Return any errors.    */

if (status & 1)
    status = TRUE;

return (status);
}
