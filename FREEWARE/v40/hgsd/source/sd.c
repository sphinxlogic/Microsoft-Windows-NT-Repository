#define module_name	SD
#define module_ident	"V2.3"
/*
**  Program:	SD
**
**  Author:	Hunter Goatley
**		Copyright © 1997,1998 Hunter Goatley.  All rights reserved.
**
**  Date:	March 14, 1997
**
**		TWILIGHT ZONE ALERT: the original date of SD.MAR:
**
**;=	Purpose:	Improved SET DEFAULT command			=
**;=	System:		VAX 11/750  VAX/VMS v4.5			=
**;=	Date:		14-MAR-1987					=
**
**		It was rewritten 10 years to the day, completely by accident!
**		Weeeeeeiiiiirrrrrrd.
**
**  Modified by:
**
**	V2.3		Hunter Goatley		31-AUG-1998 20:47
**		For "SD *", show stack only if "*" is only thing given.
**		This allows things like "SD *X" to go to "[FOX]".
**
**		Changed "MADGOAT" to "HG" in logicals.
**
**	V2.2		Hunter Goatley		 1-MAR-1998 01:56
**		Added support for setting the DCL prompt to the final
**		directory spec.  If the spec is too long, the beginning
**		of the directory name is replaced with "*".  (Note: this
**		support is only included when the symbol PROMPT is defined.)
**
**	V2.1		Hunter Goatley		27-AUG-1997 16:21
**		For fao_unknown, go ahead and do the $GETMSG.
**
**	V2.0-1		Hunter Goatley		17-MAR-1997 08:53
**		Added support for "<" and ">" to move to the previous and
**		next alphabetical directory, respectively.  Also, if
**		already at top-level directory, move to [000000] if we
**		go up again ("@@" will always go to [000000]).  Finally,
**		added "&" to "un-conceal" directory specs.  Finally finally,
**		expanded the stack from 10 to 16 entries (0--F).
**
**	V2.0		Hunter Goatley		14-MAR-1997 15:43
**		Rewrite of original MACRO32 program, with enhancements.
**
*/
#ifdef __DECC
#pragma module module_name module_ident
#else
#module module_name module_ident
#endif

#include <lib$routines.h>
#include <str$routines.h>
#include <starlet.h>
#include <descrip.h>
#include <rmsdef.h>
#include <lnmdef.h>
#include <string.h>
#include <libdef.h>
#include <libclidef.h>
#include <hlpdef.h>
#include <ssdef.h>
#include <stdio.h>
#include <prvdef.h>
#ifdef __DECC
#include <fabdef.h>
#include <namdef.h>
#else
#include <fab.h>
#include <nam.h>
#endif

/* $SETDDIR isn't defined in the include files for VAX C or DEC C on VAX */
#ifndef __alpha
int sys$setddir();
#else
#ifndef sys$setddir
int sys$setddir();
#endif
#endif
unsigned int lbr$output_help();

#define STACK_LIMIT	15
#define SD_M_CHANGED	1	/* Default changed */
#define SD_M_UPDATE	2	/* Update stack */
#define SD_M_UPDCHG	3	/* CHANGED and UPDATE */

#define errchk(x)    if (!(x & 1)) return(x)

struct _itmlst {
	unsigned short length;
	unsigned short code;
	char *bufadr;
	unsigned long *retlen;
	} trnlnm_itmlst[3];

/* FORWARD ROUTINE */
int new_angle (char *, int, char);
int up (char *, int);
int down (char *, int);
int top (void);
int over (char *, int);
int back (void);
int no_conceal (void);
int main_or_log (char *, int);
int update_stack (struct dsc$descriptor_s *, int);
int swap_stack (void);
int back_stack (char *, int);
int list_stack (void);
int print_error (int);
#ifdef PROMPT
int set_dir_prompt(struct dsc$descriptor_s *);
#endif /* PROMPT */

/* Define some global static variables */

$DESCRIPTOR (sys$disk, "SYS$DISK");
$DESCRIPTOR (lnm$process_table, "LNM$PROCESS_TABLE");
$DESCRIPTOR (sd_topic, "SD");
$DESCRIPTOR (prev_lnm, "SD_PREV");
$DESCRIPTOR (this_default, "SYS$DISK:");
$DESCRIPTOR (help_library, "HG_SD_HLB");
$DESCRIPTOR (sd_sp_symbol, "SD_SP");
$DESCRIPTOR (dot_dir, ".DIR");
$DESCRIPTOR (star_dot_dir, "*.DIR");
$DESCRIPTOR (up_dir, "[-]");
$DESCRIPTOR (faostr, "  !AS!AS");
$DESCRIPTOR (fao_stack, "#!1XB  !AS");
$DESCRIPTOR (stack_msg, "\012  \033[7m SD's Directory stack \033[m\015\012");
$DESCRIPTOR (fao_dnr, "\012\033[1;7m  Device !AD not available  \033[m\012");
$DESCRIPTOR (fao_dnf, "\012\033[1;7m  No such directory !AD on !AD  \033[m\012");
$DESCRIPTOR (fao_dir, "\012\033[1;7m  Invalid directory spec: !AS  \033[m\012");
$DESCRIPTOR (fao_unknown, "\012\033[1;7m  Error: !AS  \033[m\012");
$DESCRIPTOR (execute_com,
	"$ if f$search(\042SD_ENTER.COM\042).nes.\042\042 then @SD_ENTER.COM");

unsigned long help_flags = HLP$M_PROMPT;
char sd_slot_buff[] = "SD_SLOT0";
char sys$login[] = "SYS$LOGIN:";
char root_dir[] = ":[000000]";

unsigned long sym_table = LIB$K_CLI_GLOBAL_SYM;

char defdev_buff[NAM$C_MAXRSS], defdir_buff[NAM$C_MAXRSS],
	origdef_buff[NAM$C_MAXRSS], parse_file_expanded[NAM$C_MAXRSS],
	work_buff[NAM$C_MAXRSS];
char sd_sp_buff[NAM$C_MAXRSS];
struct dsc$descriptor_s sd_sp = {NAM$C_MAXRSS, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
struct dsc$descriptor_s default_dev = {NAM$C_MAXRSS, DSC$K_DTYPE_T,
		DSC$K_CLASS_S, 0};
struct dsc$descriptor_s default_dir = {NAM$C_MAXRSS, DSC$K_DTYPE_T,
		DSC$K_CLASS_S, 0};
struct dsc$descriptor_s original_default = {NAM$C_MAXRSS, DSC$K_DTYPE_T,
		DSC$K_CLASS_S, 0};
struct dsc$descriptor_s work = {NAM$C_MAXRSS, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
struct dsc$descriptor_s sd_slot = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

struct FAB parse_fab;
struct NAM parse_nam;

int changed = 0;
int watch_for_closing_bracket = 0;
char *p_addr;
unsigned long p_len;


int main(int argc, char **argv)
{

    unsigned long status, retlen;
    char *ptr, *cmd;
    unsigned long len, skip_rangle;

    parse_fab = cc$rms_fab;
    parse_fab.fab$l_fop = FAB$M_NAM;
    parse_fab.fab$l_nam = &parse_nam;

    parse_nam = cc$rms_nam;
    parse_nam.nam$l_esa = parse_file_expanded;
    parse_nam.nam$b_ess = NAM$C_MAXRSS;
    parse_nam.nam$b_nop = NAM$M_SYNCHK;
    
    default_dev.dsc$a_pointer = defdev_buff;
    default_dir.dsc$a_pointer = defdir_buff;
    original_default.dsc$a_pointer = origdef_buff;
    sd_sp.dsc$a_pointer = sd_sp_buff;
    sd_slot.dsc$w_length = sizeof(sd_slot_buff)-1;
    sd_slot.dsc$a_pointer = sd_slot_buff;
    work.dsc$a_pointer = work_buff;

    /*
    **  First, get the current default device and directory.  Use
    **  SYS$PARSE to parse "SYS$DISK:[]".
    */
    parse_fab.fab$l_fna = this_default.dsc$a_pointer;
    parse_fab.fab$b_fns = this_default.dsc$w_length;
    status = sys$parse (&parse_fab, 0, 0);
    errchk(status);

    /*
    **  Copy the initial default device and directory to original_default.
    */
    strncpy (origdef_buff, parse_nam.nam$l_dev, parse_nam.nam$b_dev);
    strncpy (&origdef_buff[parse_nam.nam$b_dev],
		parse_nam.nam$l_dir, parse_nam.nam$b_dir);
    original_default.dsc$w_length = parse_nam.nam$b_dev + parse_nam.nam$b_dir;
    strncpy (work_buff, original_default.dsc$a_pointer,
		original_default.dsc$w_length);
    work.dsc$w_length = original_default.dsc$w_length;

    /*
    **  Get the current stack pointer symbol.  If it's not defined, then
    **  initialize the SD stack symbols.
    */
    status = lib$get_symbol (&sd_sp_symbol, &sd_sp, &sd_sp, &sym_table);
    if (status == LIB$_NOSUCHSYM) {
	int i;
	for (i = 0; i < STACK_LIMIT+1; i++) {
	    lib$set_symbol (&sd_slot, &original_default, &sym_table);
	    sd_slot.dsc$a_pointer[sd_slot.dsc$w_length-1] += 1;
	    if (i == 9)
	    	sd_slot.dsc$a_pointer[sd_slot.dsc$w_length-1] += 7;
	}
	/*
	**  Now initialize the stack pointer to '0'.
	*/
	*sd_sp.dsc$a_pointer = '0';
	sd_sp.dsc$w_length = 1;
	lib$set_symbol (&sd_sp_symbol, &sd_sp, &sym_table);
    }

    /*
    **  If nothing was given on the command line, just print the current
    **  default.
    */
    if (argc == 1) {
	work_buff[0] = work_buff[1] = ' ';
	strncpy (&work_buff[2], original_default.dsc$a_pointer,
			original_default.dsc$w_length);
	work.dsc$w_length = original_default.dsc$w_length + 2;
	return(lib$put_output (&work));
    }

    /*
    **  If the argument begins with "?", then just do the help and return.
    */
    if (argv[1][0] == '?')
	return(lbr$output_help (&lib$put_output, 0, &sd_topic, &help_library,
		&help_flags, &lib$get_input));

    /*
    **  If the argument begins with "*", then just show the stack and return
    */
    if ((argc == 2) && (argv[1][0] == '*') && (argv[1][1] == '\0')) {
	list_stack();
	return(SS$_NORMAL);
	}

    /*
    **  Some VMS internals here: the DEC C RTL keeps the argv values
    **  in a buffer, separated by nulls, with a null separating them
    **  from the argv[0] value (the image name):
    **
    **		parm1.parm2.parm3..image   (where "." is "NULL")
    **
    **  Use that knowledge to step through the entire command line,
    **  processing SD commands as we go.
    */

    p_addr = argv[1];
    p_len  = (unsigned long) argv[0]-1;	/* Length is 1 minus image name address */
    cmd = argv[1];

    while (*cmd != '\0') {
	ptr = cmd + 1;
	len = 1;
	skip_rangle = (*cmd == '<');
	for (;;) {
	    if ((*ptr == '^') ||
		((!skip_rangle && *ptr == '>')) ||
		(*ptr == '#') ||
		(*ptr == '%') ||
		(*ptr == '@') ||
		(*ptr == '[') ||
		(*ptr == '<') ||
		(*ptr == '&') ||
		(*ptr == '\\') ||
		(*ptr == '.') ||
		(*ptr == '~') ||
		(*ptr == '\0'))
		    break;
		len++;
		ptr++;
	    }	
	    
	p_len -= len;
	p_addr = ptr;

	if (*ptr == '\0') {
	    p_addr++;
	    p_len--;
	    }

#ifdef DEBUG
	{
	    char buff[255];

	    strncpy (buff, cmd, len);
	    buff[len] = '\0';
	    printf("Command is '%s'\n", buff);
	}
#endif

	/* Process the command */
	switch (*cmd) {
	    case '^' :
	    case '-' :	{ up (cmd, len); break;}
	    case '.' :	{ down (cmd, len); break;}
	    case '%' :	{ update_stack (&original_default, 1); break;}
	    case '[' :	{ new_angle (cmd, len, ']'); break;}
	    case '<' :	{ new_angle (cmd, len, '>'); break;}
	    case '@' :	{ top (); break;}
	    case '>' :	{ over (cmd, len); break;}
	    case '&' :	{ no_conceal (); break;}
	    case '\\' :	{ back (); break;}
	    case '#' :	{ back_stack (cmd, len); break;}
	    case '~' :	{ main_or_log (sys$login, strlen(sys$login)); break; }
	    default:	{ main_or_log (cmd, len); break; }
	    }

	cmd = p_addr;

    }

    status = SS$_NORMAL;

    if (changed) {
	parse_fab.fab$l_fna = work.dsc$a_pointer;
	parse_fab.fab$b_fns = work.dsc$w_length;
	parse_nam.nam$b_nop &= ~NAM$M_SYNCHK;
	status = sys$parse (&parse_fab, 0, 0);

	if (status & 1) {
	    /*
	    **  See if the device actually changed before changing the
	    **  logical.
	    */
	    if ((default_dev.dsc$w_length != parse_nam.nam$b_dev) ||
		(strncmp (work.dsc$a_pointer, default_dev.dsc$a_pointer,
			default_dev.dsc$w_length) != 0))
		{
		default_dev.dsc$w_length = parse_nam.nam$b_dev;
		default_dev.dsc$a_pointer = parse_nam.nam$l_dev;
		status = lib$set_logical (&sys$disk, &default_dev);
		}
	    default_dir.dsc$w_length = parse_nam.nam$b_dir;
	    default_dir.dsc$a_pointer = parse_nam.nam$l_dir;
	    status = sys$setddir (&default_dir, 0, 0);
	    }

	/* Define SD_PREV: to point to our original default */
	lib$set_logical (&prev_lnm, &original_default);

	}	/* if (changed) */

    if (status & 1) {
	work_buff[0] = work_buff[1] = ' ';
	work.dsc$w_length = parse_nam.nam$b_dev + parse_nam.nam$b_dir;
	strncpy (&work_buff[2], parse_nam.nam$l_dev, work.dsc$w_length);
	work.dsc$w_length += 2;		/* Include blanks! */
	lib$put_output (&work);
	work.dsc$w_length -= 2;		/* Now make work descriptor point */
	work.dsc$a_pointer += 2;	/* just to final string w/o blanks */
					/* (Used by UPDATE_STACK!) */
	update_stack (&work, 0);	/* Update the stack */
#ifdef PROMPT
	set_dir_prompt (&work);
#endif /* PROMPT */
	}
    else
	print_error (status);

    lib$do_command (&execute_com);
    return (SS$_NORMAL);

}


/*
**  Routine:	new_angle
**
**  Desc:	Routine that's called when a new directory spec is
**		encountered via "[" or "<".  The new directory is
**		just copied to work buffer as a new top-level directory.
**
*/
int new_angle (char *cmd, int len, char close)
{
    char *x;

    /*
    **  If the command length is 1, user entered "[.xxxx", which will be
    **  handled by down().  Just return in that case.
    **
    **  Special check: if "<" is the command, peek ahead to see if the
    **  next command is ".".  If it's not, then treat this as a request
    **  to move back to the previous alphabetical directory.
    */
    if (len == 1)
	if ((close == '>') && (p_len == 0) || (*p_addr != '.'))
	    return (back());
	else
	    return (SS$_NORMAL);

    changed = SD_M_UPDCHG;		/* Set changed flag */

    x = strchr (work.dsc$a_pointer, ':') + 1;
    strncpy (x, cmd, len);
    work.dsc$w_length = (x - work.dsc$a_pointer) + len;

    if (cmd[len-1] != close)
	work.dsc$a_pointer[work.dsc$w_length++] = close;

    return (SS$_NORMAL);  
}


/*
**  Routine:	main_or_log
**
**  Desc:	Parse a logical or top-level directory name.
**
*/
int main_or_log (char *cmd, int len)
{
    char *x;
    int status;

    changed = SD_M_UPDCHG;		/* Set changed flag */

    /*
    **  Let $PARSE do most of our work.  Use the current work spec as the
    **  default so that any parsing is done relative to that spec.
    */
    parse_fab.fab$l_fna = cmd;
    parse_fab.fab$b_fns = len;
    parse_fab.fab$b_dns = work.dsc$w_length;
    parse_fab.fab$l_dna = work.dsc$a_pointer;
    status = sys$parse (&parse_fab, 0, 0);
    parse_fab.fab$b_dns = 0;
    parse_fab.fab$l_dna = 0;

    /*
    **  If an error occurred, so ahead and act like we're going to use
    **  this one so an error can be reported (or it'll be corrected).
    */
    if (!(status & 1)) {
	top();
	return (over (cmd, len));
	}

    /*
    **  Regardless of whether or not the parse was good, go ahead and use
    **  the returned device and directory as the new work spec.
    */
    strncpy (work.dsc$a_pointer, parse_nam.nam$l_dev, parse_nam.nam$b_dev);
    strncpy (&work.dsc$a_pointer[parse_nam.nam$b_dev],
		parse_nam.nam$l_dir, parse_nam.nam$b_dir);
    work.dsc$w_length = parse_nam.nam$b_dev + parse_nam.nam$b_dir;

    if (status & 1) {
	/*
	**  Handle case like "dev:dir" (without "[]") by checking to
	**  see if there was a filename in the parsed spec.
	*/
	if (parse_nam.nam$b_name != 0) {
	    top();
	    return (over (parse_nam.nam$l_name, parse_nam.nam$b_name));
	    }
	}

    return (SS$_NORMAL);  
}


/*
**  Routine:	top
**
**  Desc:	Move to the top of the current directory tree.
**
*/
int top (void)
{
    char bracket;
    char *ptr;
    int x;
    int status;

    changed = SD_M_UPDCHG;		/* Set changed flag */

    bracket = work.dsc$a_pointer[work.dsc$w_length-1];
    work.dsc$a_pointer[work.dsc$w_length] = '\0';

    ptr = strchr (work.dsc$a_pointer, '.');
    if (ptr != NULL) {
	*ptr++ = bracket;
	work.dsc$w_length = ptr - work.dsc$a_pointer;
	}
    else {
	ptr = strchr (work.dsc$a_pointer, ':');
	strncpy (ptr, root_dir, strlen(root_dir));
	work.dsc$w_length = strlen(root_dir) + ptr - work.dsc$a_pointer;
	}

    return (SS$_NORMAL);  
}


/*
**  Routine:	over
**
**  Desc:	Move over to another directory at the same level.  Will
**		either move to specified directory or to next alphabetical
**		directory.
**
*/
int over (char *cmd, int len)
{
    char bracket;
    char buff[NAM$C_MAXRSS];
    char *ptr, *tmp;
    int x, tmplen, bufflen;
    int status;
    int context = 0;
    struct dsc$descriptor_d dir = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
    struct dsc$descriptor_s str = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

    changed = SD_M_UPDCHG;		/* Set changed flag */

    tmp = cmd;
    tmplen = len;
    if (*cmd == '>') {
	tmp++;
	tmplen--;
	}

    ptr = work.dsc$a_pointer + work.dsc$w_length;
    bracket = *--ptr;		/* Get the closing "]" or ">" */

    /*  Work backwards looking for "." or ":" */
    while ((*--ptr != '.') && (*ptr != ':'));

    /*
    **  Copy our work default, minus the lowest-level directory, to a
    **  new work buffer to use for searches.
    */
    bufflen = ptr - work.dsc$a_pointer;
    strncpy (buff, work.dsc$a_pointer, bufflen);

    if (*ptr == ':') {
	strcpy (&buff[bufflen], root_dir);
	bufflen += sizeof(root_dir)-1;
	}
    else
	buff[bufflen++] = bracket;

    str.dsc$a_pointer = buff;
    str.dsc$w_length = bufflen;

    if (*ptr == ':')
	ptr += 2;	/* Move past ":[" or ":<" */
    else
	ptr++;		/* Move past "." */

    /*
    **  There are two possibilities when moving over a directory:
    **
    **	    - we move to the specified directory, using wildcard matching
    **	    - no directory is given and we move to the next alphabetical dir
    */
    if (tmplen == 0) {
	/*
	**  No directory was specified, so find the next alphabetical
	**  directory and move to it.
	*/
	struct dsc$descriptor_d newdir = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
	struct dsc$descriptor_d firstdir = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
	struct dsc$descriptor_s thisdir = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
	int i, first = 1;
	char *ptr2;

	/*
	**  Let thisdir point to the current directory (uppercased).
	*/
	thisdir.dsc$w_length = work.dsc$w_length -
				(ptr - work.dsc$a_pointer) - 1;
	thisdir.dsc$a_pointer = ptr;
	/* Make sure it's all uppercase */
	for (i = 0, ptr2 = ptr; i < thisdir.dsc$w_length; i++, ptr2++)
	    if (*ptr2 >= 'a' && *ptr2 <= 'z')
		*ptr2 = *ptr2 & 0xDF;

	/*
	**  Look for a directory that alphabetically comes behind the
	**  current directory.
	*/
	str$concat (&dir, &str, &star_dot_dir);		/* Search for *.DIR */
	do {
	    status = lib$find_file (&dir, &newdir, &context, 0, 0, 0, 0);
	    if (status & 1) {
		parse_fab.fab$l_fna = newdir.dsc$a_pointer;
		parse_fab.fab$b_fns = newdir.dsc$w_length;
		sys$parse (&parse_fab, 0, 0);
		if (first) {	/* Save this one if it's the first found */
		    i = parse_nam.nam$b_name;
		    str$copy_r (&firstdir, &i, parse_nam.nam$l_name);
		    first = 0;
		    }
		if ((parse_nam.nam$b_name == thisdir.dsc$w_length) &&
		    (strncmp(thisdir.dsc$a_pointer, parse_nam.nam$l_name,
				 thisdir.dsc$w_length) == 0))
		    break;
	    }
	}
	while (status & 1);

	/*
	**  If we exited the loop because we found the current directory,
	**  do one more FIND to get the next directory.
	*/
	if (status & 1)
	     status = lib$find_file (&dir, &dir, &context, 0, 0, 0, 0);

	/*
	**  If the returned status was "No more files", then we need to
	**  use the first directory returned.  Otherwise, DIR already
	**  describes the next directory.
	*/
	if (status == RMS$_NMF) {	/* No more files?  Use first one */
	    str$free1_dx (&dir);
	    dir.dsc$w_length = firstdir.dsc$w_length;
	    dir.dsc$a_pointer = firstdir.dsc$a_pointer;
	    status = SS$_NORMAL;
	    }
	}
    else {
	/*
	**  See if the directory exists by searching for "name.DIR".
	*/
	str$copy_r (&dir, &tmplen, tmp);
	str$concat (&dir, &str, &dir, &dot_dir);
	status = lib$find_file (&dir, &dir, &context, 0, 0, 0, 0);

	/*
	**  If it doesn't exist, find a possible match using "name*.DIR".
	*/
	if (!(status & 1)) {
	    str$copy_r (&dir, &tmplen, tmp);
	    str$concat (&dir, &str, &dir, &star_dot_dir);
	    status = lib$find_file (&dir, &dir, &context, 0, 0, 0, 0);
	    }
	}

    if (status & 1) {
	/*
	**  There was a matching directory found, so parse the name
	**  out and use that as the directory name.
	*/
	parse_fab.fab$l_fna = dir.dsc$a_pointer;
	parse_fab.fab$b_fns = dir.dsc$w_length;
	sys$parse (&parse_fab, 0, 0);
	tmplen = parse_nam.nam$b_name;
	tmp = parse_nam.nam$l_name;
    }
    lib$find_file_end (&context);
    str$free1_dx (&dir);

    strncpy (ptr, tmp, tmplen);
    ptr += tmplen;
    *ptr++ = bracket;
    work.dsc$w_length = ptr - work.dsc$a_pointer;

    return (SS$_NORMAL);  
}


/*
**  Routine:	back
**
**  Desc:	Move to the previous alphabetical directory at same level.
**
*/
int back (void)
{
    char bracket;
    char buff[NAM$C_MAXRSS];
    char *ptr, *tmp;
    int x, tmplen, bufflen;
    int status;
    int context = 0;
    struct dsc$descriptor_d dir = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
    struct dsc$descriptor_s str = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
    static char root_dir[] = ":[000000]";
    struct dsc$descriptor_d newdir = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
    struct dsc$descriptor_d prevdir = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
    struct dsc$descriptor_s thisdir = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
    int i, first = 1;
    char *ptr2;

    changed = SD_M_UPDCHG;		/* Set changed flag */

    tmp = 0;
    tmplen = 0;

    ptr = work.dsc$a_pointer + work.dsc$w_length;
    bracket = *--ptr;		/* Get the closing "]" or ">" */

    /*  Work backwards looking for "." or ":" */
    while ((*--ptr != '.') && (*ptr != ':'));

    /*
    **  Copy our work default, minus the lowest-level directory, to a
    **  new work buffer to use for searches.
    */
    bufflen = ptr - work.dsc$a_pointer;
    strncpy (buff, work.dsc$a_pointer, bufflen);

    if (*ptr == ':') {
	strcpy (&buff[bufflen], root_dir);
	bufflen += sizeof(root_dir)-1;
	}
    else
	buff[bufflen++] = bracket;

    str.dsc$a_pointer = buff;
    str.dsc$w_length = bufflen;

    if (*ptr == ':')
	ptr += 2;	/* Move past ":[" or ":<" */
    else
	ptr++;		/* Move past "." */


    /*
    **  Let thisdir point to the current directory (uppercased).
    */
    thisdir.dsc$w_length = work.dsc$w_length - (ptr - work.dsc$a_pointer) - 1;
    thisdir.dsc$a_pointer = ptr;
    /* Make sure it's all uppercase */
    for (i = 0, ptr2 = ptr; i < thisdir.dsc$w_length; i++, ptr2++)
	if (*ptr2 >= 'a' && *ptr2 <= 'z')
		*ptr2 = *ptr2 & 0xDF;

    /*
    **  Look for a directory that alphabetically comes behind the
    **  current directory.
    */
    str$concat (&dir, &str, &star_dot_dir);		/* Search for *.DIR */
    do {
	status = lib$find_file (&dir, &newdir, &context, 0, 0, 0, 0);
	if (status & 1) {
		parse_fab.fab$l_fna = newdir.dsc$a_pointer;
		parse_fab.fab$b_fns = newdir.dsc$w_length;
		sys$parse (&parse_fab, 0, 0);
		if ((parse_nam.nam$b_name == thisdir.dsc$w_length) &&
		    (strncmp(thisdir.dsc$a_pointer, parse_nam.nam$l_name,
				 thisdir.dsc$w_length) == 0)) {
		    if (!first)
			break;
		    }
		i = parse_nam.nam$b_name;
		str$copy_r (&prevdir, &i, parse_nam.nam$l_name);
		first = 0;
	}
    }
    while (status & 1);

    str$free1_dx (&dir);
    tmplen = prevdir.dsc$w_length;
    tmp = prevdir.dsc$a_pointer;

    lib$find_file_end (&context);

    strncpy (ptr, tmp, tmplen);
    ptr += tmplen;
    *ptr++ = bracket;
    work.dsc$w_length = ptr - work.dsc$a_pointer;

    str$free1_dx (&prevdir);

    return (SS$_NORMAL);  
}


/*
**  Routine:	up
**
**  Desc:	Move up a directory level.
**
*/
int up (char *cmd, int len)
{
    char bracket;
    char *ptr;
    int x;
    int status;

    changed = SD_M_UPDCHG;		/* Set changed flag */

    ptr = work.dsc$a_pointer + work.dsc$w_length;
    bracket = *--ptr;		/* Get the closing "]" or ">" */

    /*  Work backwards looking for "." */
    while ((*--ptr != '.') && (ptr > work.dsc$a_pointer));

    /*  If we found a ".", replace it with our bracket and update length */
    if (ptr != work.dsc$a_pointer) {
	*ptr++ = bracket;
	work.dsc$w_length = ptr - work.dsc$a_pointer;
	}
    else {
	/* At the top, so move to [000000] */
	ptr = strchr (work.dsc$a_pointer, ':');
	strncpy (ptr, root_dir, strlen(root_dir));
	work.dsc$w_length = strlen(root_dir) + ptr - work.dsc$a_pointer;
	}


    return (SS$_NORMAL);  
}


/*
**  Routine:	down
**
**  Desc:	Move down a directory level.  Check to see if the specified
**		subdirectory exists.  If not, do a search for a match and
**		use it.
*/
int down (char *cmd, int len)
{
    char bracket;
    char *ptr;
    char tmp[NAM$C_MAXRSS];
    int x, tmplen, context = 0;
    int status;
    
    struct dsc$descriptor_d dir = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
    changed = SD_M_UPDCHG;		/* Set changed flag */

    if (len == 1)
	return (main_or_log (sys$login, strlen(sys$login)));

    strncpy (tmp, cmd, len);
    tmplen = len;

    if ((tmp[len-1] == ']') || (tmp[len-1] == '>'))
	--tmplen;

    /*
    **  See if the directory exists by searching for "name.DIR".
    */
    x = len - 1;
    str$copy_r (&dir, &x, &cmd[1]);
    str$concat (&dir, &work, &dir, &dot_dir);
    status = lib$find_file (&dir, &dir, &context, 0, 0, 0, 0);

    /*
    **  If it doesn't exist, find a possible match using "name*.DIR".
    */
    if (!(status & 1)) {
	x = len - 1;
	str$copy_r (&dir, &x, &cmd[1]);
	str$concat (&dir, &work, &dir, &star_dot_dir);
	status = lib$find_file (&dir, &dir, &context, 0, 0, 0, 0);
    }

    if (status & 1) {
	/*
	**  There was a matching directory found, so parse the name
	**  out and use that as the subdirectory name.
	*/
	parse_fab.fab$l_fna = dir.dsc$a_pointer;
	parse_fab.fab$b_fns = dir.dsc$w_length;
	sys$parse (&parse_fab, 0, 0);
	tmp[0] = '.';
	strncpy (&tmp[1], parse_nam.nam$l_name, parse_nam.nam$b_name);
	tmplen = parse_nam.nam$b_name+1;
    }
    lib$find_file_end (&context);
    str$free1_dx (&dir);

    /*
    **  Copy the string in tmp[] to the work buffer.  If the subdirectory
    **  doesn't exist, we still copy it in so that the user can see the
    **  directory spec that doesn't exist.
    */
    ptr = work.dsc$a_pointer + work.dsc$w_length;
    bracket = *--ptr;		/* Get the closing "]" or ">" */

    strncpy (ptr, tmp, tmplen);
    ptr += tmplen;
    *ptr++ = bracket;

    work.dsc$w_length = ptr - work.dsc$a_pointer;

    return (SS$_NORMAL);  
}


/*
**  Routine:	no_conceal
**
**  Desc:	Unconceal the current work directory (translate all
**		logicals and handle hidden directory specs).
*/
int no_conceal (void)
{
    char *src, *dst, *end;
    char ch;
    int status;
    
    changed = SD_M_UPDCHG;		/* Set changed flag */

    parse_fab.fab$l_fna = work.dsc$a_pointer;
    parse_fab.fab$b_fns = work.dsc$w_length;
    parse_nam.nam$b_nop |= NAM$M_NOCONCEAL;
    sys$parse (&parse_fab, 0, 0);
    parse_nam.nam$b_nop &= ~NAM$M_NOCONCEAL;

    /*
    **  Copy the string in the NAM buffer to the work buffer.
    **
    **  We need to recognize and handle these formats:
    **
    **		dev:[dir.][dir.dir]
    **		dev:[dir.][000000]
    **
    */
    src = parse_nam.nam$l_dev;
    dst = work.dsc$a_pointer;
    end = src + parse_nam.nam$b_dev + parse_nam.nam$b_dir;

    while (src < end) {
	*dst++ = (ch = *src++);
	/*  Have we found ".]["? */
	if ((ch == '.') &&
	   ((src[0] == ']') || (src[0] == '>')) &&
	   ((src[1] == '[') || (src[1] == '<'))) {
	    /*
	    **  We found the hidden directory spec, so skip over the
	    **  "][" pair and see if we have "000000." or "000000]".
	    **  If we do, don't copy that string to the work buffer.
	    */
	    src += 2;
	    if ((src[0] == '0') &&
		(src[1] == '0')  &&
		(src[2] == '0')  &&
		(src[3] == '0')  &&
		(src[4] == '0')  &&
		(src[5] == '0')  &&
		((src[6] == '.') || (src[6] == ']') || (src[6] == '>'))) {
		src += 6;	/* Skip over "000000" */
		dst--;		/* Back up over the "." so src[6] is copied */
		}
	    }
	}

    work.dsc$w_length = dst - work.dsc$a_pointer;

    return (SS$_NORMAL);  
}


/*
**  Routine:	update_stack
**
**  Desc:	Update the stack by pushing the new default.
**
*/
int update_stack (struct dsc$descriptor_s *str, int push_current)
{
    char slot_no;
    int status;

    if (!push_current) {
	if (!(changed & SD_M_UPDATE))	/* Do nothing if UPDATE not set */
	    return (SS$_NORMAL);

	/* Do nothing if the default spec didn't actually change */
	if ((str->dsc$w_length == original_default.dsc$w_length) &&
	    (strncmp (str->dsc$a_pointer, original_default.dsc$a_pointer,
			str->dsc$w_length) == 0))
	    return (SS$_NORMAL);
	}

    /*
    **  Get the current slot number and increment it.
    */
    slot_no = (char) *sd_sp.dsc$a_pointer;
    if (slot_no == 'F')
	slot_no = '0';
    else
	if (slot_no == '9')
	    slot_no = 'A';
	else
	    slot_no++;

    sd_slot.dsc$a_pointer[sd_slot.dsc$w_length-1] = slot_no;
    sd_sp.dsc$a_pointer[0] = slot_no;

    /*
    **  Now update SD_SLOTn and SD_SP.
    */
    status = lib$set_symbol (&sd_slot, str, &sym_table);
    if (status & 1)
	status = lib$set_symbol (&sd_sp_symbol, &sd_sp, &sym_table);

    return (status);  
}


/*
**  Routine:	swap_stack
**
**  Desc:	Swap the contents of positions 0 and 1 in the stack.
**
*/
int swap_stack (void)
{
    char slot_no;
    int status;
    char prev_slot_no;
    char slot_buff[NAM$C_MAXRSS];
    struct dsc$descriptor_s slot_spec = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

    changed = SD_M_CHANGED;	/* Default changed, but don't update stack */

    /*
    **  Get the current slot number and update it.
    */
    prev_slot_no = slot_no = (char) *sd_sp.dsc$a_pointer;
    sd_slot.dsc$a_pointer[sd_slot.dsc$w_length-1] = slot_no;

    slot_spec.dsc$w_length = NAM$C_MAXRSS;
    slot_spec.dsc$a_pointer = slot_buff;
    status = lib$get_symbol (&sd_slot, &slot_spec, &slot_spec);

    /*
    **  Decrement the slot number.
    */
    if (slot_no == 'A')
	slot_no = '9';
    else
	if (slot_no == '0')
	    slot_no = 'F';
	else
	    slot_no--;

    sd_slot.dsc$a_pointer[sd_slot.dsc$w_length-1] = slot_no;
    work.dsc$w_length = NAM$C_MAXRSS;
    status = lib$get_symbol (&sd_slot, &work, &work);

    status = lib$set_symbol (&sd_slot, &slot_spec, &sym_table);
    sd_slot.dsc$a_pointer[sd_slot.dsc$w_length-1] = prev_slot_no;
    status = lib$set_symbol (&sd_slot, &work, &sym_table);

    return (status);  
}


/*
**  Routine:	back_stack
**
**  Desc:	Move to the specified stack slot number.
**
*/
int back_stack (char *cmd, int len)
{
    char slot_no;
    int status;
    char prev_slot_no;
    char slot_buff[NAM$C_MAXRSS];
    struct dsc$descriptor_s slot_spec = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

    if (len > 2)
	return (SS$_NORMAL);

    if (len == 1)
	return (swap_stack());

    if ((cmd[1] >= 'a') && (cmd[1] <= 'z'))
	cmd[1] &= 0xDF;

    slot_no = cmd[1] - '0';
    if (cmd[1] >= 'A')
	slot_no -= 7;
    if ((slot_no < 0) || (slot_no > STACK_LIMIT))
	return (SS$_NORMAL);

    prev_slot_no = (char) *sd_sp.dsc$a_pointer;
    if (prev_slot_no >= 'A')
	prev_slot_no -= 7;
    prev_slot_no -= '0';
    /* Calculate actual stack position (SD_SP - #) */
    prev_slot_no -= slot_no;
    if (prev_slot_no < 0)
	prev_slot_no += STACK_LIMIT + 1;

    if (prev_slot_no > 9) prev_slot_no += 7;
    sd_slot.dsc$a_pointer[sd_slot.dsc$w_length-1] = prev_slot_no + '0';

    changed = SD_M_UPDCHG;	/* Default changed, but don't update stack */

    work.dsc$w_length = NAM$C_MAXRSS;
    status = lib$get_symbol (&sd_slot, &work, &work);

    return (status);  
}


/*
**  Routine:	list_stack
**
**  Desc:	Display the stack.
**
*/
int list_stack (void)
{
    char fao_buff[NAM$C_MAXRSS];
    int slot_no, count;
    struct dsc$descriptor_s faoout = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
    int status;

    faoout.dsc$a_pointer = fao_buff;
    lib$put_output (&stack_msg);

    /*
    **  Get the current slot number.
    */
    slot_no = (char) *sd_sp.dsc$a_pointer;
    if (slot_no >= 'A')
	slot_no -= 7;
    slot_no -= '0';
    count = 0;
    
    for (count = 0; count <= STACK_LIMIT; count++) {
	work.dsc$w_length = NAM$C_MAXRSS;
	faoout.dsc$w_length = NAM$C_MAXRSS;

	sd_slot.dsc$a_pointer[sd_slot.dsc$w_length-1] = slot_no + '0';
	if (slot_no > 9)
	    sd_slot.dsc$a_pointer[sd_slot.dsc$w_length-1] += 7;
	status = lib$get_symbol (&sd_slot, &work, &work);
	sys$fao (&fao_stack, &faoout, &faoout, count, &work);
	lib$put_output (&faoout);

	slot_no--;
	if (slot_no < 0)
	    slot_no = STACK_LIMIT;
	}

    faoout.dsc$w_length = 0;
    lib$put_output (&faoout);

    return (status);  
}


/*
**  Routine:	print_error
**
**  Desc:	Print error. ;-)
**
*/
int print_error (int status)
{
    char fao_buff[NAM$C_MAXRSS];
    char unk_buff[256];
    struct dsc$descriptor_s faoout = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
    struct dsc$descriptor_s unkmsg = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

    faoout.dsc$w_length = sizeof(fao_buff);
    faoout.dsc$a_pointer = fao_buff;
    unkmsg.dsc$w_length = sizeof(unk_buff);
    unkmsg.dsc$a_pointer = unk_buff;

    switch (status) {
	case RMS$_DNF:
	    sys$fao (&fao_dnf, &faoout, &faoout, parse_nam.nam$b_dir,
		parse_nam.nam$l_dir, parse_nam.nam$b_dev, parse_nam.nam$l_dev);
	    break;
	case RMS$_DNR:
	case RMS$_DEV:
	    sys$fao (&fao_dnr, &faoout, &faoout, parse_nam.nam$b_dev,
				parse_nam.nam$l_dev);
	    break;
	case RMS$_DIR:
	case RMS$_SYN:
	    sys$fao (&fao_dir, &faoout, &faoout, &work);
	    break;
	default:
	    sys$getmsg (status, &unkmsg.dsc$w_length, &unkmsg, 1, 0);
	    sys$fao (&fao_unknown, &faoout, &faoout, &unkmsg);
	}

    lib$put_output (&faoout);

    return (SS$_NORMAL);

}


#ifdef PROMPT

int exec_set_dir_prompt (char *str)
{
   char *prc, *sptr, *dptr;
   long *q;
   char x;

#ifdef __DECC
#pragma nostandard
#endif

globalref CTL$AG_CLIDATA;
globalvalue PPD$L_PRC;
globalvalue PRC_B_PROMPTLEN;
globalvalue PRC_G_PROMPT;

#ifdef __DECC
#pragma standard
#endif

   /*
    *  Point to the PRC region of the PPD.
    */
   q = (long *)((char *)&CTL$AG_CLIDATA + PPD$L_PRC);
   prc = (char *) *q;

   /*
    *  Store the prompt length (+3) as a single byte.
    */
   x = (char) strlen(str);
   dptr = prc + PRC_B_PROMPTLEN;
   *dptr = x + 3;	/* Have to include <NUL>_ */

   /*
    *  Now copy the actual string to the prompt buffer.
    */
   for (sptr = str, dptr = prc + PRC_G_PROMPT; x; x--)
        *dptr++ = *sptr++;

   return(SS$_NORMAL);
}

int set_dir_prompt(struct dsc$descriptor_s *str)
{
    char work[36];
    char *sptr, *dptr;
    int x;
    unsigned long arg[2];
    unsigned long int privmsk[2] = {PRV$M_CMEXEC, 0};
    static $DESCRIPTOR(lnm, "HG_SD_SET_PROMPT");
    static $DESCRIPTOR(lnmtable, "LNM$DCL_LOGICAL");

    if (!(sys$trnlnm(0, &lnmtable, &lnm, 0, 0) & 1))
	return (SS$_NORMAL);

    /*
    **  Enable CMEXEC privilege; if not enabled, just return.
    */
    if (!(sys$setprv(1, &privmsk, 0, 0) & 1))
	return (SS$_NORMAL);

    /*
    **  If the directory spec is longer than 31 characters, replace
    **  the beginning part of the directory name with "*":
    **
    **		HG_SRC:[HGSD.SOURCE] -> HG_SRC:*D.SOURCE]
    **
    */
    if (str->dsc$w_length > 31) {

	/* Copy the device name to the work buffer */

	for (sptr = str->dsc$a_pointer, dptr = work; *sptr != ':';)
	   *dptr++ = *sptr++;

	*dptr++ = ':';
	*dptr++ = '*';

	/*
	**  Now copy the source string backwards from the end until we reach
	**  the "*" added above.
	*/
	sptr = str->dsc$a_pointer + str->dsc$w_length - 1;
	x = 31 - (dptr - work);
	for (dptr = &work[30]; x != 0; x--)
	   *dptr-- = *sptr--;
	work[31] = ' ';
	work[32] = '\0';
    } else {
	/*
	**  String is <= 31 chars, so just copy it to the work buffer.
	*/
	for (sptr = str->dsc$a_pointer, dptr = work, x = str->dsc$w_length;
		x; x--)
	    *dptr++ = *sptr++;
	*dptr++ = ' ';
	*dptr++ = '\0';
	}
 
    arg[0] = 1;
    arg[1] = (unsigned long) &work;
    sys$cmexec(&exec_set_dir_prompt,&arg);

    return(SS$_NORMAL);

}
#endif /* PROMPT */
