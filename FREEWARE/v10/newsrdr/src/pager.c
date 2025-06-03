/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Paged-output routines.
**
**  MODULE DESCRIPTION:
**
**  	Output routines for NEWSRDR, mainly supporting page-at-a-time
**  display.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, 1994  MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  08-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	08-SEP-1992 V1.0    Madison 	Initial coding.
**  	13-OCT-1992 V1.0-1  Madison 	Trim trailing blanks on error msgs.
**  	10-FEB-1993 V1.1    Madison 	Allow for non-terminal output device.
**  	12-APR-1993 V1.2    Madison 	Put remaining lines in Press... msg.
**  	17-APR-1993 V1.3    Madison 	Add connection check.
**  	21-SEP-1993 V1.4    Madison 	Add get_cmd_noecho.
**  	26-OCT-1993 V1.4-1  Madison 	Fix for double-header pages.
**  	02-MAR-1994 V1.4-2  Madison 	Fix second-header bug in Paged_Output_Done.
**  	13-APR-1994 V1.4-3  Madison 	Fix "1 lines remaining"
**  	19-MAY-1994 V1.4-4  Madison 	Fix missing blank line before prompt.
**--
*/
#define MODULE_PAGER
#include "newsrdr.h"
#include <varargs.h>
#ifdef __GNUC__
#include <vms/smgdef.h>
#include <vms/iodef.h>
#include <vms/dcdef.h>
#include <vms/ttdef.h>
#else
#include <smgdef.h>
#include <iodef.h>
#include <dcdef.h>
#include <ttdef.h>
#include <smg$routines.h>
#endif

/*
** Forward declarations
*/
    unsigned int Pager_Init(int);
    unsigned int Begin_Paged_Output(char *, char *);
    int          Put_Paged(char *, int);
    int          Paged_Output_Done(void);
    static int   Flush_Paged_Output(void);
    void         Discard_Paged_Output(void);
    void         Finish_Paged_Output(int);
    unsigned int put_output(char *);
    unsigned int put_output_counted(char *, int);
    unsigned int put_output_nocc(char *);
    unsigned int put_output_dx(struct dsc$descriptor *);
    unsigned int put_errmsg(struct dsc$descriptor *);
    unsigned int get_cmd(char *, int, char *);
    unsigned int get_cmd_noecho(char *, int, char *);
    static unsigned int get_cmd_common(char *, int, char *, int);
    unsigned int get_cmd_dx(struct dsc$descriptor *, struct dsc$descriptor *,
    	    	    	    	short *);
    unsigned int put_paged_help(char *, char *);
    unsigned int Define_Key(char *, int);
    static unsigned int ctrl_c_handler(void);


    static char ctrlchrs[] = {1,2,3,4,5,6,7,8,
            10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
            28,29,30,31,127,128,129,130,131,132,133,134,135,136,137,
            138,139,140,141,142,143,144,145,146,147,148,149,150,
            151,152,153,154,155,156,157,158,159,160,255};
    static struct dsc$descriptor ctrls, spaces;

    GLOBAL int pb_rows, pb_cols;
    GLOBAL int User_Interrupt = 0;

    static unsigned int pbid, vdid, kbid, ktid;
    static char hdrstr[STRING_SIZE], hdr2str[STRING_SIZE];
    static int cur_row, erase_page;
    static short ttchn;
    static struct QUE waitq;
    static int is_terminal = 0;
    static int Need_Extra_Blank_Line = 0;

    extern unsigned int lbr$output_help();

/*
**++
**  ROUTINE:	Pager_Init
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Initializes stuff used by this module.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Pager_Routine(int semi)
**
**  semi:   boolean, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int Pager_Init(int semi) {

    struct {
    	unsigned char class, type;
    	unsigned short width;
    	unsigned int ch : 24;
    	unsigned char page;
    } smbuf;
    unsigned int status, pbflags=SMG$M_KEEP_CONTENTS;
    unsigned int kpflags=SMG$M_KEYPAD_APPLICATION, eight=8;
    $DESCRIPTOR(sysout, "SYS$OUTPUT:");
    $DESCRIPTOR(keydefs, "NEWSRDR_KEYDEFS");
    $DESCRIPTOR(keydefs_default, "SYS$LOGIN:.NRKEY");

/*
** semi is set if we only need to "semi-"initialize, such as after a
** SPAWN returns
*/
    if (!semi) {
    	status = sys$assign(&sysout, &ttchn, 0, 0);
    	if (!OK(status)) lib$stop(status);
    	status = sys$qiow(0, ttchn, IO$_SENSEMODE, 0, 0, 0,
    	    &smbuf, sizeof(smbuf), 0, 0, 0, 0);
    	if (OK(status)) {
    	    is_terminal = smbuf.class == DC$_TERM;
    	    if (OK(status) && is_terminal) {
    	    	if (!(smbuf.ch & TT$M_WRAP)) {
    	    	    smbuf.ch |= TT$M_WRAP;
    	    	    sys$qiow(0, ttchn, IO$_SETMODE, 0, 0, 0,
    	    	    	&smbuf, sizeof(smbuf), 0, 0, 0, 0);
    	    	}
    	    }
    	}
    	if (is_terminal) {
    	    status = smg$create_pasteboard(&pbid, 0, &pb_rows, &pb_cols,
    	    	    &pbflags);
    	    if (!OK(status)) lib$stop(status);
    	    if (pb_rows < 6) pb_rows = 6;
    	} else {
    	    sys$dassgn(ttchn);
    	    ttchn = 0;
    	    pb_cols = 132;
    	}

    	smg$create_key_table(&ktid);
    	smg$load_key_defs(&ktid, &keydefs, &keydefs_default);
    	smg$create_virtual_keyboard(&kbid);

    	INIT_SDESC(ctrls, sizeof(ctrlchrs), ctrlchrs);
    	INIT_SDESC(spaces, sizeof(ctrlchrs), 0);
    	spaces.dsc$a_pointer = malloc(sizeof(ctrlchrs));
    	memset(spaces.dsc$a_pointer, ' ', spaces.dsc$w_length);
    }

    smg$set_keypad_mode(&kbid, &kpflags);

    if (is_terminal) {
    	smg$set_out_of_band_asts(&pbid, &eight, ctrl_c_handler);
    }

    erase_page = 1;
    waitq.head = waitq.tail = &waitq;
    hdrstr[0] = '\0';
    Need_Extra_Blank_Line = 0;

    return SS$_NORMAL;

} /* Pager_Init */    

/*
**++
**  ROUTINE:	Begin_Paged_Output
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Sets up the header string for an upcoming paged output
**  sequence.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Begin_Paged_Output(char *hdr [,char *hdr2])
**
**  hdr:    ASCIZ_string, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int Begin_Paged_Output(char *hdr, char *hdr2) {

    int argcnt;

    va_count(argcnt);

    strcpy(hdrstr, hdr);
    if (strlen(hdrstr) > pb_cols) {
    	*(hdrstr+pb_cols) = '\0';
    }

    if (argcnt == 2 && hdr2 != 0) {
    	strcpy(hdr2str, hdr2);
    	if (strlen(hdr2str) > pb_cols) {
    	    *(hdr2str+pb_cols) = '\0';
    	}
    } else hdr2str[0] = '\0';

    erase_page = 1;

    return SS$_NORMAL;

} /* Begin_Paged_Output */

/*
**++
**  ROUTINE:	Put_Paged
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Puts out a string that should be considered for the paged output
**  sequence.  The lines that the string takes up are counted and saved
**  if it cannot be displayed on the current page.
**
**  RETURNS:	boolean
**
**  PROTOTYPE:
**
**  	Put_Paged(char *str, int)
**
**  str:    ASCIZ_string, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    0:	string would not fit on page
**  	    1:	string fit on page
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int Put_Paged(char *str, int xlines) {

    char tmp[STRING_SIZE], tmp2[STRING_SIZE], *cp;
    struct HDR *w;
    int lines, maxlines, remaining, l, x, i, len;
    unsigned int status, return_status;

    if (!is_terminal) {
    	put_output(str);
    	return;
    }

    if (erase_page) {
    	smg$erase_pasteboard(&pbid);
    	cur_row = 1;
    	if (*hdrstr) {
    	    put_output_nocc(hdrstr);
    	    if (*hdr2str) put_output_nocc(hdr2str);
    	    put_output("");
    	}
    	erase_page = 0;
    }

    if (!Flush_Paged_Output()) return 0;

    x = 0;
    for (cp = str+strlen(str); cp > str && isspace(*(cp-1)); cp--) x = 1;
    if (x) {
    	strncpy(tmp, str, cp-str);
    	*(tmp+(cp-str)) = '\0';
    	cp = tmp;
    } else {
    	cp = str;
    }
    len = strlen(cp);
    if (*cp == '\014') {
    	put_output("");
    	if (xlines >= 0) {
    	    sprintf(tmp, "[%d line%s remaining, press RETURN for more...]",
    	    	    	    xlines + 1, (xlines == 0 ? "" : "s"));
    	    put_output(tmp);
    	} else {
    	    put_output("[Press RETURN for more...]");
    	}
    	erase_page = 1;
    	if (len > 1) {
    	    *cp = ' ';
    	    insert_header(cp, waitq.tail, 0);
    	}
    	return 0;
    }
    lines = len / pb_cols + 1;
    if (len > 0 && (len % pb_cols == 0)) lines = (lines-1 > 1 ? lines-1 : 1);
    maxlines = pb_rows-cur_row-4;
    if (lines <= maxlines) {
    	put_output(cp);
    	return_status = 1;
    } else {
    	remaining = len;
    	for (i = 0; i < lines; i++) {
    	    l = pb_cols < remaining ? pb_cols : remaining;
    	    remaining -= l;
    	    if (i < maxlines) {
    	    	memcpy(tmp2, cp+(i*pb_cols), l);
    	    	*(tmp2+l) = '\0';
    	    	put_output(tmp2);
    	    } else {
    	    	w = mem_gethdr(l+1);
    	    	memcpy(w->str, cp+(i*pb_cols), l);
    	    	*(w->str+l) = '\0';
    	    	queue_insert(w, waitq.tail);
    	    }
    	}

    	put_output("");
    	if (xlines >= 0) {
    	    sprintf(tmp, "[%d line%s remaining, press RETURN for more...]",
    	    	    	xlines + 1, (xlines == 0 ? "" : "s"));
    	    put_output(tmp);
    	} else {
    	    put_output("[Press RETURN for more...]");
    	}
    	erase_page = 1;
    	return_status = 0;
    }

    return return_status;

} /* Put_Paged */

/*
**++
**  ROUTINE:	Paged_Output_Done
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Checks to see if we've finished any pending paged output,
**  going to the next screen page if necessary.
**
**  RETURNS:	boolean
**
**  PROTOTYPE:
**
**  	Paged_Output_Done()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    0:	not done
**  	    1:	done
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int Paged_Output_Done() {

    if (!is_terminal) return 1;

    if (erase_page) {
    	erase_page = 0;
    	smg$erase_pasteboard(&pbid);
    	cur_row = 1;
    	if (*hdrstr) {
    	    put_output_nocc(hdrstr);
    	    if (*hdr2str) put_output_nocc(hdr2str);
    	    put_output("");
    	}
    }

    return Flush_Paged_Output();

} /* Paged_Output_Done */

/*
**++
**  ROUTINE:	Flush_Paged_Output
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Flushes any waiting pageable output, if it will fit on
**  the current page.
**
**  RETURNS:	boolean
**
**  PROTOTYPE:
**
**  	Flush_Paged_Output()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    0:	did not fit on page
**  	    1:	fit ok on page
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static int Flush_Paged_Output() {

    struct HDR *w;

    while (queue_remove(waitq.head, &w)) {
    	if (cur_row >= pb_rows-4) {
    	    queue_insert(w, &waitq);
    	    put_output("");
    	    put_output("[Press RETURN for more...]");
    	    erase_page = 1;
    	    return 0;
    	}
    	put_output(w->str);
    	mem_freehdr(w);
    }

    return 1;

} /* Flush_Paged_Output */

/*
**++
**  ROUTINE:	Discard_Paged_Output
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Just throws away any pending pageable output.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Discard_Paged_Output()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void Discard_Paged_Output() {

    struct HDR *w;
    while (queue_remove(waitq.head, &w)) mem_freehdr(w);

} /* Discard_Paged_Output */

/*
**++
**  ROUTINE:	Finish_Paged_Output
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Cleanup routine for this module.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Finish_Paged_Output(int alldone)
**
**  alldone:	boolean, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void Finish_Paged_Output(int alldone) {

    unsigned int zero = 0;

    Discard_Paged_Output();
    if (alldone) {
    	if (is_terminal) smg$set_out_of_band_asts(&pbid, &zero, 0);
    	if (alldone == 1) {
    	    smg$delete_pasteboard(&pbid, &zero);
    	    smg$delete_virtual_keyboard(&kbid);
    	    if (ttchn) sys$dassgn(ttchn);
    	}
    }

} /* Finish_Paged_Output */

/*
**++
**  ROUTINE:	put_output
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Puts out an ASCIZ string.  Updates our record of
**  cursor position, in case we were called as part of paged output.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	put_output(char *str)
**
**  str:    ASCIZ_string, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int put_output(char *str) {

    char tmp[STRING_SIZE];
    struct dsc$descriptor tdsc, sdsc;
    int len, lines;
    
    len = strlen(str);
    if (len > STRING_SIZE) len = STRING_SIZE;
    INIT_SDESC(tdsc, len, tmp);
    INIT_SDESC(sdsc, len, str);
    str$translate(&tdsc, &sdsc, &spaces, &ctrls);

    if (!is_terminal) return lib$put_output(&tdsc);

    lines = len / pb_cols + 1;
    if (len > 0 && (len % pb_cols == 0)) {
    	lines = (lines-1 > 1 ? lines-1 : 1);
    	Need_Extra_Blank_Line = 1;
    } else {
    	if (len == 0 && Need_Extra_Blank_Line) {
    	    sys$qiow(0, ttchn, IO$_WRITEVBLK, 0, 0, 0, "\r\n", 2, 0,
    	    	    0x8d000000, 0, 0);
    	}
    	Need_Extra_Blank_Line = 0;
    }

    cur_row = cur_row+lines > pb_rows ? pb_rows : cur_row+lines;

    return sys$qiow(0, ttchn, IO$_WRITEVBLK, 0, 0, 0,
    	tmp, len, 0, 0x8d010000, 0, 0);

} /* put_output */

/*
**++
**  ROUTINE:	put_output_counted
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Just like put_output, but takes the address of the string plus
**  a count.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	put_output_counted(char *str, int len)
**
**  str:    character string, read only, by reference
**  len:    integer, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int put_output_counted(char *str, int len) {

    char tmp[STRING_SIZE];
    struct dsc$descriptor tdsc, sdsc;
    int lines;
    
    if (len > STRING_SIZE) len = STRING_SIZE;
    INIT_SDESC(tdsc, len, tmp);
    INIT_SDESC(sdsc, len, str);
    str$translate(&tdsc, &sdsc, &spaces, &ctrls);

    if (!is_terminal) return lib$put_output(&tdsc);

    lines = len / pb_cols + 1;
    if (len > 0 && (len % pb_cols == 0)) {
    	lines = (lines-1 > 1 ? lines-1 : 1);
    	Need_Extra_Blank_Line = 1;
    } else {
    	if (len == 0 && Need_Extra_Blank_Line) {
    	    sys$qiow(0, ttchn, IO$_WRITEVBLK, 0, 0, 0, "\r\n", 2, 0,
    	    	    0x8d000000, 0, 0);
    	}
    	Need_Extra_Blank_Line = 0;
    }

    cur_row = cur_row+lines > pb_rows ? pb_rows : cur_row+lines;

    return sys$qiow(0, ttchn, IO$_WRITEVBLK, 0, 0, 0,
    	tmp, len, 0, 0x8d010000, 0, 0);

} /* put_output_counted */

/*
**++
**  ROUTINE:	put_output_nocc
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Just like put_output, but disables carriage control for the
**  displayed line.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	put_output_nocc(char *str)
**
**  str:    ASCIZ_string, read only, by reference)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int put_output_nocc(char *str) {

    char tmp[STRING_SIZE];
    struct dsc$descriptor tdsc, sdsc;
    int len, lines;
    
    len = strlen(str);
    if (len > STRING_SIZE) len = STRING_SIZE;
    INIT_SDESC(tdsc, len, tmp);
    INIT_SDESC(sdsc, len, str);
    str$translate(&tdsc, &sdsc, &spaces, &ctrls);

    if (!is_terminal) lib$put_output(&tdsc);

    lines = len / pb_cols + 1;
    if (len > 0 && (len % pb_cols == 0)) lines = (lines-1 > 1 ? lines-1 : 1);
    cur_row = cur_row+lines > pb_rows ? pb_rows : cur_row+lines;

    return sys$qiow(0, ttchn, IO$_WRITEVBLK, 0, 0, 0,
    	tmp, len, 0, 0x8d000000, 0, 0);

} /* put_output_nocc */

/*
**++
**  ROUTINE:	put_output_dx
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Just like put_output, but for a character string passed
**  in by descriptor.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	put_output_dx(struct dsc$descriptor *s)
**
**  s:	char_string, read only, by descriptor
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int put_output_dx(struct dsc$descriptor *s) {

    char *cp;
    short len;

    if (!is_terminal) return lib$put_output(s);

    lib$analyze_sdesc(s, &len, &cp);
    return put_output_counted(cp, len);

} /* put_output_dx */

/*
**++
**  ROUTINE:	put_errmsg
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Just like put_output_dx, but returns a zero as status value
**  for routine.  Used in conjunction with $PUTMSG to do our own
**  error message output.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	put_errmsg(struct dsc$descriptor *s)
**
**  s:	char_string, read only, by descriptor
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    0:	    	Always returned.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int put_errmsg(struct dsc$descriptor *s) {

    char *cp;
    short len;

    lib$analyze_sdesc(s, &len, &cp);
    while (len > 0 && isspace(*(cp+len-1))) len--;
    put_output_counted(cp, len);
    return 0;

} /* put_errmsg */

/*
**++
**  ROUTINE:	get_cmd
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Gets a string from the command (input) stream.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	get_cmd(char *buf, int bufsize, char *prompt)
**
**  buf:    	ASCIZ_string, write only, by reference
**  bufsize:	integer, read only, by value
**  prompt: 	ASCIZ_string, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**  	Others from SMG$READ_COMPOSED_LINE are possible.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int get_cmd(char *buf, int bufsize, char *prompt) {

    return get_cmd_common(buf, bufsize, prompt, 0);

} /* get_cmd */

/*
**++
**  ROUTINE:	get_cmd_noecho
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Gets a string from the command (input) stream without echoing.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	get_cmd_noecho(char *buf, int bufsize, char *prompt)
**
**  buf:    	ASCIZ_string, write only, by reference
**  bufsize:	integer, read only, by value
**  prompt: 	ASCIZ_string, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**  	Others from SMG$READ_COMPOSED_LINE are possible.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int get_cmd_noecho(char *buf, int bufsize, char *prompt) {

    return get_cmd_common(buf, bufsize, prompt, 1);

} /* get_cmd_noecho */

/*
**++
**  ROUTINE:	get_cmd_common
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Gets a string from the command (input) stream.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	get_cmd_common(char *buf, int bufsize, char *prompt)
**
**  buf:    	ASCIZ_string, write only, by reference
**  bufsize:	integer, read only, by value
**  prompt: 	ASCIZ_string, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**  	Others from SMG$READ_COMPOSED_LINE are possible.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static unsigned int get_cmd_common(char *buf, int bufsize, char *prompt, int noecho) {

    struct dsc$descriptor bdsc, pdsc;
    unsigned int status, set_status, term_char_1, old_term_char_1, flags;
    short len;

    if (noecho) {
    	term_char_1 = TT$M_NOECHO;
    	set_status = smg$set_term_characteristics(&pbid, &term_char_1, 0, 0, 0, &old_term_char_1);
    	flags = SMG$M_NORECALL;
    } else {
    	flags = 0;
    }

    INIT_SDESC(bdsc, bufsize-1, buf);
    if (prompt) {
    	INIT_SDESC(pdsc, strlen(prompt), prompt);
    } else {
    	INIT_SDESC(pdsc, 0, buf);
    }
    status = smg$read_composed_line(&kbid, &ktid, &bdsc, &pdsc, &len, 0, &flags);
    if (OK(status)) {
    	*(buf+len) = '\0';
    }

    if (noecho && OK(set_status)) {
    	if (!(old_term_char_1 & TT$M_NOECHO)) {
    	    smg$set_term_characteristics(&pbid, 0, 0, &term_char_1);
    	}
    }

    Check_Connection();

    return status;

} /* get_cmd_common */

/*
**++
**  ROUTINE:	get_cmd_dx
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Like get_cmd, but uses descriptors.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	get_cmd_dx(struct dsc$descriptor *buf, struct dsc$descriptor *prompt,
**  	    	    	short *len)
**
**  buf:    character string, write only, by descriptor
**  prompt: character string, read only, by descriptor
**  len:    word, write only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int get_cmd_dx(struct dsc$descriptor *buf,
    	    	    struct dsc$descriptor *prompt, short *lenp) {

    struct dsc$descriptor bdsc, pdsc;
    unsigned int status;
    short len;
    int count;

    va_count(count);
    status = smg$read_composed_line(&kbid, &ktid, buf, prompt, &len);
    if (OK(status)) {
    	if (count > 2 && lenp != NULL) *lenp = len;
    }

    Check_Connection();

    return status;

} /* get_cmd_dx */

/*
**++
**  ROUTINE:	put_paged_help
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Uses our output routines with LBR$OUTPUT_HELP to display
**  help.  At one time, we actually paged this output, but not
**  any more.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	put_paged_help(char *topic, char *lib)
**
**  topic:  ASCIZ_string, read only, by reference
**  lib:    ASCIZ_string, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int put_paged_help(char *topic, char *lib) {

    struct dsc$descriptor tdsc, ldsc;
    INIT_SDESC(tdsc, strlen(topic), topic);
    INIT_SDESC(ldsc, strlen(lib), lib);
    return lbr$output_help(put_output_dx, 0, &tdsc, &ldsc, 0, get_cmd_dx);

} /* put_paged_help */

/*
**++
**  ROUTINE:	Define_Key
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Uses SMG$DEFINE_KEY to define a function key.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Define_Key(char *defkcmd, int cmdlen)
**
**  defkcmd:	character string, read only, by reference
**  cmdlen: 	integer, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int Define_Key(char *defkcmd, int cmdlen) {

    struct dsc$descriptor dsc;
    INIT_SDESC(dsc, cmdlen, defkcmd);
    return smg$define_key(&ktid, &dsc);

} /* Define_Key */

/*
**++
**  ROUTINE:	ctrl_c_handler
**
**  FUNCTIONAL DESCRIPTION:
**
**  	AST routine invoked when the user types a ctrl/C.  Sets
**  the user_interrupt flag and displays the [Cancel] message.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	ctrl_c_handler()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Always returned.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static unsigned int ctrl_c_handler() {

    static char canmsg[] = "\015\012[Cancel]\015\012";
    User_Interrupt = 1;
    if (is_terminal) {
    	sys$qiow(0, ttchn, IO$_WRITEVBLK, 0, 0, 0, canmsg, 12, 0, 0, 0, 0);
    }
    return SS$_NORMAL;

} /* ctrl_c_handler */
