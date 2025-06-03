/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Miscellaneous routines
**
**  MODULE DESCRIPTION:
**
**  	Various utility routines needed by various other NEWSRDR
**  modules.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  05-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	05-SEP-1992 V1.0    Madison 	Initial coding.
**  	15-FEB-1993 V1.0-1  Madison 	Make compatible with pre-VMS V5.2.
**  	05-APR-1993 V1.0-2  Madison 	Got DST adjustment wrong!
**  	17-APR-1993 V1.1    Madison 	Add Check_Connection.
**  	23-APR-1993 V1.1-1  Madison 	get_logical changes.
**  	22-MAY-1993 V1.1-2  Madison 	Fix loss of keydefs after msg prompting.
**  	24-MAY-1992 V1.1-3  Madison 	Fix weekday computation in Make_Date.
**  	15-AUG-1993 V1.1-4  Madison 	Don't edit infspec if it's null.
**  	21-SEP-1993 V1.1-5  Madison 	EDT$EDIT requires input file name.
**  	16-MAY-1994 V1.1-6  Madison 	Slight mod to locase().
**  	17-MAY-1994 V1.2    Madison 	Thread_Match routine.
**--
*/
#define MODULE_MISC
#include "newsrdr.h"
#include "globals.h"
#ifdef __GNUC__
#include <vms/jpidef.h>
#include <vms/lnmdef.h>
#include <vms/psldef.h>
#else
#include <jpidef.h>
#include <lnmdef.h>
#include <psldef.h>
#include <smg$routines.h>
#endif
#include <signal.h>

/*
**  Forward declarations
*/
    void make_temp_fspec(char *, int);
    void make_nntp_dtstring(TIME *, char *, int);
    unsigned int Compose_Message(char *, char *, int, int *);
    void upcase(char *);
    void locase(char *);
    void Format_Header(struct HDR *, char *, int, int *);
    void Make_Date(TIME *, char *, int);
    int Yes_Answer(char *, int);
    unsigned int cli_get_value(char *, char *, int);
    unsigned int cli_present(char *);
    unsigned int get_logical(char *, char *);
    unsigned int get_system_logical(char *, char *);
    int streql_case_blind(char *, char *);
    int strneql_case_blind(char *, char *, int);
#ifdef __GNUC__
    void queue_insert(struct QUE *, struct QUE *);
    int  queue_remove(struct QUE *, void **);
#endif

    unsigned int find_image_symbol(char *, char *, void *);
    static unsigned int x_find_image_symbol(struct dsc$descriptor *,
    	    	    	    struct dsc$descriptor *, void *);
    void insert_header(char *, struct HDR *, int);
    unsigned int table_parse(void *, void *, void *);
    void Check_Connection(void);
    int Thread_Match(char *, char *);
/*
**  External references
*/
    extern unsigned int Set_Current_Group(struct GRP *);
    EXTERN struct GRP *curgroup;

/*
**++
**  ROUTINE:	make_temp_fspec
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Builds a file-spec for a temporary file from the current
**  PID and date/time.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	make_temp_fspec(char *fspec, int fspec_size)
**
**  fspec:  	file specification, write only, by reference (ASCIZ string)
**  fspec_size:	integer, read only, by value
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
void make_temp_fspec(char *fspec, int fspec_size) {

    struct dsc$descriptor tdsc;
    TIME time;
    unsigned int pid, jpi_pid=JPI$_PID;
    unsigned short len;
    $DESCRIPTOR(ctrstr, "SYS$SCRATCH:NEWS_!XL-!XL-!XL.TMP");

    lib$getjpi(&jpi_pid, 0, 0, &pid);
    sys$gettim(&time);
    INIT_SDESC(tdsc, fspec_size, fspec);
    sys$fao(&ctrstr, &len, &tdsc, pid, time.long2, time.long1);
    *(fspec+len) = '\0';

} /* make_temp_fspec */

/*
**++
**  ROUTINE:	make_nntp_dtstring
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Makes a date/time string suitable for the NEWGROUPS NNTP command.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	make_nntp_dtstring(TIME *dt_stamp, char *str, int str_size)
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
void make_nntp_dtstring(TIME *dt_stamp, char *str, int str_size) {

    short dtnum[7], len;
    struct dsc$descriptor sdsc;
    unsigned int status;
    $DESCRIPTOR(ctrstr, "!2ZW!2ZW!2ZW !2ZW!2ZW!2ZW");

    sys$numtim(dtnum, dt_stamp);
    dtnum[0] %= 100;
    INIT_SDESC(sdsc, str_size, str);
    sys$fao(&ctrstr, &len, &sdsc, dtnum[0], dtnum[1], dtnum[2],
    	    dtnum[3], dtnum[4], dtnum[5]);
    *(str+len) = '\0';

} /* make_nntp_dtstring */

/*
**++
**  ROUTINE:	Compose_Message
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Message composition.  Prompts user for message or fires
**  up a text editor.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Compose_Message(char *infspec, char *outspec, int use_editor,
**  	    	    	    	int *is_temp)
**
**  infspec:	file_spec, read only, by reference (ASCIZ string)
**  outspec:	file_spec, write only, by reference (ASCIZ string)
**  use_editor:	boolean, read only, by value
**  is_temp:	boolean, write only, by reference
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
unsigned int Compose_Message(char *infspec, char *outspec, int use_editor,
    	    	    	    	    int *is_temp) {

    struct dsc$descriptor sdsc, sdsc2;
    unsigned int status;

    if (*infspec && !use_editor) {
    	strcpy(outspec, infspec);
    	*is_temp = 0;
    	return SS$_NORMAL;
    }

    make_temp_fspec(outspec, FSPEC_SIZE);
    *is_temp = 1;

/*
** If we're not using an editor, then just prompt the user for the
** message text.
*/
    if (!use_editor) {
    	char tmp[STRING_SIZE];
    	struct dsc$descriptor tdsc;
    	short len;
    	unsigned int unit, kbid;
    	$DESCRIPTOR(prompt, "> ");

    	status = file_create(outspec, &unit, 0, 0);
    	if (!OK(status)) {
    	    lib$signal(NEWS__CREATERR, 2, strlen(outspec), outspec, status);
    	    return status;
    	}
    	put_output("");
    	put_output("Enter message text; type CTRL/Z when done.");
    	put_output("");
    	INIT_SDESC(tdsc, sizeof(tmp), tmp);
    	Finish_Paged_Output(2);
    	smg$create_virtual_keyboard(&kbid);
    	while (OK(smg$read_composed_line(&kbid, 0, &tdsc, &prompt, &len))) {
    	    file_write(unit, tmp, len);
    	}
    	file_close(unit);
    	smg$delete_virtual_keyboard(&kbid);
    	Pager_Init(1);
    	status = SS$_NORMAL;
    } else {

/*
** We're supposed to fire up a text editor.
*/
    	char edfspec[STRING_SIZE], edsymbol[STRING_SIZE];
    	unsigned int (*editrtn)();

    	put_output("");
    	put_output("Starting editor...");

/*
** Spawned?
*/
    	if (news_prof.spawnedit) {
    	    if (news_prof.captive) {
    	    	lib$signal(NEWS__SPAWNERR, 0, NEWS__CAPTIVE, 0);
    	    	return NEWS__SPAWNERR;
    	    }
    	    sprintf(edsymbol, "%s \"%s\" %s", news_prof.editorname,
    	    	infspec, outspec);
    	    Finish_Paged_Output(2);
    	    INIT_SDESC(sdsc, strlen(edsymbol), edsymbol);
    	    status = lib$spawn(&sdsc);
    	    Pager_Init(1);
    	    if (!OK(status)) {
    	    	lib$signal(NEWS__SPAWNERR, 0, status);
    	    }
    	} else {
/*
** Callable
*/
    	    strcpy(edfspec, news_prof.editorname);
    	    strcat(edfspec, "SHR");
    	    strcpy(edsymbol, news_prof.editorname);
    	    strcat(edsymbol, "$EDIT");
    	    INIT_SDESC(sdsc, strlen(edfspec), edfspec);
    	    INIT_SDESC(sdsc2, strlen(edsymbol), edsymbol);
    	    status = lib$find_image_symbol(&sdsc, &sdsc2, &editrtn);
    	    if (!OK(status)) {
    	    	lib$signal(NEWS__EDITERR, 4, sdsc2.dsc$w_length, edsymbol,
    	    	    	    	sdsc.dsc$w_length, edfspec, status);
    	    	return status;
    	    }
    	    Finish_Paged_Output(2);
    	    INIT_SDESC(sdsc, strlen(infspec), infspec);
    	    INIT_SDESC(sdsc2, strlen(outspec), outspec);
    	    if (strncmp(news_prof.editorname, "EDT", 3) == 0) {
    	    	$DESCRIPTOR(scratch, "SYS$SCRATCH:");
    	    	$DESCRIPTOR(nl, "_NLA0:");
    	    	status = (*editrtn)((sdsc.dsc$w_length == 0 ? 
    	    	    	(struct dsc$descriptor *) &nl : &sdsc), &sdsc2, 0, &scratch);
    	    } else {
    	    	status = (*editrtn)((sdsc.dsc$w_length == 0 ? 0 : &sdsc), &sdsc2);
    	    }
    	    Pager_Init(1);
    	}

    }

/*
** If we were successful, then look for the actual existence of
** the output file.
*/
    if (OK(status)) {
    	unsigned int ffctx=0, status;
    	struct dsc$descriptor str;
    	INIT_SDESC(sdsc, strlen(outspec), outspec);
    	INIT_DYNDESC(str);
    	status = lib$find_file(&sdsc, &str, &ffctx);
    	if (OK(status)) {
    	    lib$find_file_end(&ffctx);
    	    strncpy(outspec, str.dsc$a_pointer, str.dsc$w_length);
    	    *(outspec+str.dsc$w_length) = '\0';
    	} else {
    	    *outspec = '\0';
    	}
    	str$free1_dx(&str);
    }

    Check_Connection();

    return status;

} /* Compose_Message */

/*
**++
**  ROUTINE:	upcase
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Converts a string to upper case.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	upcase(char *str)
**
**  str:    character string, modify, by reference (ASCIZ)
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
void upcase(char *s) {

    register char *cp;

    for (cp = s; *cp; cp++) if (islower(*cp)) *cp = toupper(*cp);

} /* upcase */

/*
**++
**  ROUTINE:	locase
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Converts a string to lower case, but only if it doesn't
**  already contain lower case letters.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	locase(char *str)
**
**  str:    character string, modify, by reference (ASCIZ string)
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
void locase(char *s) {

    register char *cp;

    for (cp = s; *cp; cp++) if (islower(*cp)) return;
    for (cp = s; *cp; cp++) if (isupper(*cp)) *cp = tolower(*cp);

} /* locase */

/*
**++
**  ROUTINE:	Format_Header
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Takes a header that is in TLV format and formats it as an
**  ordinary string (bascially converting the tag into the appropriate
**  message header).
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Format_Header(struct HDR *hdr, char *str, int str_size, int *retlen)
**
**  hdr:    HDR structure, read only, by reference
**  str:    character string, write only, by reference (ASCIZ)
**  str_size: integer, read only, by value
**  retlen: integer, write only, by reference
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
void Format_Header(struct HDR *hdr, char *str, int str_size, int *retlen) {

    static char *hdrtxt[] = {
    	"From: ", "Date: ", "Newsgroups: ", "Subject: ", "Message-ID: ",
    	"Path: ", "Reply-To: ", "Sender: ", "Followup-To: ", "Expires: ",
    	"References: ", "Control: ", "Distribution: ", "Organization: ",
    	"Keywords: ", "Summary: ", "Approved: ", "Lines: ", "Xref: "};
    int len, hlen, copylen;


    len = strlen(hdr->str);
    if (hdr->code == NEWS_K_HDR_OTHER) {
    	copylen = len < str_size-1 ? len : str_size-1;
   	strncpy(str, hdr->str, copylen);
    	*(str+copylen) = '\0';
    } else {
    	hlen = strlen(hdrtxt[hdr->code-1]);
    	hlen = hlen < str_size-1 ? hlen : str_size-1;
    	strncpy(str, hdrtxt[hdr->code-1], hlen);
    	str_size -= hlen;
    	copylen = len < str_size-1 ? len : str_size-1;
    	strncpy(str+hlen, hdr->str, copylen);
    	*(str+hlen+copylen) = '\0';
    	if (retlen) *retlen = hlen + copylen;
    }

} /* Format_Header */

/*
**++
**  ROUTINE:	Make_Date
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Makes an RFC822-type date/time string from a VMS date/time stamp.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Make_Date(TIME *timadr, char *fmt_date, int fd_size)
**
**  timadr: 	date_time, read only, by reference
**  fmt_date:	character string, write only, by reference (ASCIZ)
**  fd_size:	integer, read only, by value
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
void Make_Date(TIME *timadr, char *fmt_date, int fd_size) {

    enum {YEAR, MONTH, DAY, HOUR, MINUTE, SECOND};
    TIME xtime, time2;
    struct dsc$descriptor fddsc;
    short timbuf[7], len;
    unsigned int status;
    int dst, dow;
    $DESCRIPTOR(ctrstr, "!AD, !2ZW !AD !ZW !2ZW:!2ZW:!2ZW GMT");
    char *dowstr = "Sun Mon Tue Wed Thu Fri Sat ";
    char *monstr = "    Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec ";

    if (timadr == NULL) {
    	sys$gettim(&xtime);
    } else {
    	xtime = *timadr;
    }

    status = sys$numtim(timbuf, &xtime);
    if (!OK(status)) {
    	lib$signal(status);
    	return;
    }
    status = lib$day_of_week(&xtime, &dow);
    if (!OK(status)) {
    	lib$signal(status);
    	return;
    }
    dow %= 7;

    if (news_cfg.dst) {
    	dst = 0;
    	switch (timbuf[MONTH]) {

    	case 4:
    	    if (timbuf[DAY]-dow > 0) {
    	    	if (dow == 0) dst = (timbuf[HOUR] >= 3);
    	    	else dst = 1;
    	    } else dst = 0;
    	    break;

    	case 10:
    	    if (timbuf[DAY]-dow > 24) {
    	    	if (dow == 0) dst = (timbuf[HOUR] == 0);
    	    	else dst = 0;
    	    } else dst = 1;
    	    break;

    	case 5:
    	case 6:
    	case 7:
    	case 8:
    	case 9:
    	    dst = 1;
    	    break;

    	default:
    	    dst = 0;
    	    break;
    	}
    }

    if (news_cfg.neggmtoff) {
    	lib$add_times(&xtime, &news_cfg.gmtoffset, &time2);
    } else {
    	lib$sub_times(&xtime, &news_cfg.gmtoffset, &time2);
    }

    if (news_cfg.dst && dst) {
    	static $DESCRIPTOR(s, "0 01:00:00");
    	static TIME onehr;
    	static int did1hr = 0;
    	TIME time3;

    	if (!did1hr) {sys$bintim(&s, &onehr); did1hr=1;}
    	lib$sub_times(&time2, &onehr, &time3);
    	time2 = time3;
    }
    sys$numtim(timbuf, &time2);
    lib$day_of_week(&time2, &dow);
    dow %= 7;

    INIT_SDESC(fddsc, fd_size, fmt_date);
    len = 0;
    status = sys$fao(&ctrstr, &len, &fddsc, 3, dowstr+(dow*4),
    	    	timbuf[DAY], 3, monstr+(timbuf[MONTH]*4),
    	    	timbuf[YEAR], timbuf[HOUR], timbuf[MINUTE], timbuf[SECOND]);
    if (!OK(status)) lib$signal(status);
    *(fmt_date+len) = '\0';

    return;

} /* Make_Date */

/*
**++
**  ROUTINE:	Yes_Answer
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Asks the user a yes-or-no question.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Yes_Answer(char *prompt, int defans)
**
**  prompt: character_string, read only, by reference (ASCIZ)
**  defans: boolean, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    1: Answer was (or defaulted to) "yes".
**  	    0: Answer was (or defaulted to) "no".
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int Yes_Answer(char *prompt, int defans) {

    char *ansstr[2] = {"no", "yes"};
    char tmp[STRING_SIZE], ans[STRING_SIZE], *cp;
    struct dsc$descriptor adsc, pdsc;
    int answer;
    short len;

    if (defans >= 0) {
    	sprintf(tmp, "%s [%s]: ", prompt, (defans ? "yes" : "no"));
    } else {
    	strcpy(tmp, prompt);
    	strcat(tmp, " ");
    }
    INIT_SDESC(pdsc, strlen(tmp), tmp);
    INIT_SDESC(adsc, sizeof(ans), ans);

    while (1) {
    	lib$get_input(&adsc, &pdsc, &len);
    	while (len > 0 && isspace(ans[len-1])) len--;
    	if (len == 0) return defans;
    	ans[len] = '\0';
    	locase(ans);
    	for (cp = ans; isspace(*cp); cp++);
    	for (answer = 0; answer < 2; answer++) {
    	    if (strstr(ansstr[answer], cp) == ansstr[answer]) return answer;
    	}
    	put_output("Please answer YES or NO.");
    }

    return 0; /* should not get here */

} /* Yes_Answer */

/*
**++
**  ROUTINE:	cli_get_value
**
**  FUNCTIONAL DESCRIPTION:
**
**  	C Interface to CLI$GET_VALUE.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cli_get_value (char *argname, DESCRIP *arg)
**
** argname: ASCIZ_string, read only, by reference
** arg:	    char_string, write only, by descriptor (dynamic)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**  	All those from CLI$PRESENT and CLI$GET_VALUE.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cli_get_value(char *argname, char *arg, int argsize) {

    struct dsc$descriptor argnamd, argd;
    unsigned short arglen;
    int status;

    INIT_SDESC(argnamd, strlen(argname), argname);
    INIT_SDESC(argd, argsize-1, arg);
    status = cli$present(&argnamd);
    if ($VMS_STATUS_SUCCESS(status)) {
    	status = cli$get_value(&argnamd, &argd, &arglen);
    	if (OK(status)) *(arg+arglen) = '\0';
    }
    return status;
}

/*
**++
**  ROUTINE:	cli_present
**
**  FUNCTIONAL DESCRIPTION:
**                               
**  	C Interface to CLI$PRESENT.
**
**  RETURNS:	cond_value, intword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cli_present (char *argname)
**
** argname: ASCIZ_string, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**  	All those from CLI$PRESENT.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cli_present(char *argname) {

    struct dsc$descriptor argnamd;

    INIT_SDESC(argnamd, strlen(argname), argname);
    return cli$present(&argnamd);
}

/*
**++
**  ROUTINE:	get_logical
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Translates a logical name using a standard LNM$FILE_DEV search.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	get_logical(char *lnm, char *str)
**
**  lnm:    logical_name, read only, by reference (ASCIZ string)
**  str:    character string, write only, by reference (ASCIZ string)
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
unsigned int get_logical(char *lnm, char *str) {

    ITMLST lnmlst[2];
    struct dsc$descriptor lnmdsc;
    short slen;
    unsigned int status;
    static unsigned int attr=LNM$M_CASE_BLIND;
    static $DESCRIPTOR(tabnam, "LNM$FILE_DEV");

    ITMLST_INIT(lnmlst[0], LNM$_STRING, STRING_SIZE-1, str, &slen);
    ITMLST_INIT(lnmlst[1], 0, 0, 0, 0);
    INIT_SDESC(lnmdsc, strlen(lnm), lnm);
    status = sys$trnlnm(&attr, &tabnam, &lnmdsc, 0, (str ? lnmlst : 0));
    if (OK(status)) *(str+slen) = '\0';
    return status;

} /* get_logical */

/*
**++
**  ROUTINE:	get_system_logical
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Translates an executive-mode logical name out of LNM$SYSTEM.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	get_system_logical(char *lnm, char *str)
**
**  lnm:    logical_name, read only, by reference (ASCIZ string)
**  str:    character string, write only, by reference (ASCIZ)
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
unsigned int get_system_logical(char *lnm, char *str) {

    ITMLST lnmlst[2];
    struct dsc$descriptor lnmdsc;
    short slen;
    unsigned int status;
    static unsigned int attr=LNM$M_CASE_BLIND, acmode=PSL$C_EXEC;
    static $DESCRIPTOR(tabnam, "LNM$SYSTEM");

    ITMLST_INIT(lnmlst[0], LNM$_STRING, STRING_SIZE-1, str, &slen);
    ITMLST_INIT(lnmlst[1], 0, 0, 0, 0);
    INIT_SDESC(lnmdsc, strlen(lnm), lnm);
    status = sys$trnlnm(&attr, &tabnam, &lnmdsc, &acmode, (str ? lnmlst : 0));
    if (OK(status)) *(str+slen) = '\0';
    return status;

} /* get_system_logical */

/*
**++
**  ROUTINE:	streql_case_blind
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Compares two ASCIZ strings for equality, in a case-blind
**  fashion.
**
**  RETURNS:	boolean
**                    
**  PROTOTYPE:
**
**  	streql_case_blind(char *str1, char *str2)
**
**  str1:   character string, read only, by reference (ASCIZ)
**  str2:   character string, read only, by reference (ASCIZ)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	0:  	strings are not equal (case-blind)
**  	non-0:	strings are equal (case-blind)
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int streql_case_blind(char *s1, char *s2) {

    register unsigned char c1, c2;

    while (1) {
        c1 = islower(*s1) ? toupper(*s1) : (*s1);
        c2 = islower(*s2) ? toupper(*s2) : (*s2);
        if (c1 != c2) break;
        if (c1 == '\0') return 1;
        s1++; s2++;
    }

    return 0;
}

/*
**++
**  ROUTINE:	strneql_case_blind
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Counted case-blind string equality function.
**
**  RETURNS:	boolean
**
**  PROTOTYPE:
**
**  	strneql_case_blind(char *str1, char *str2, int len)
**
**  str1: character string, read only, by reference (ASCIZ)
**  str2: character string, read only, by reference (ASCIZ)
**  len:  integer, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    0:	strings are not equal up to the specified length (case-blind)
**  	non-0:	strings are equal up to the specified length (case-blind)
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int strneql_case_blind(char *s1, char *s2, int len) {

    register unsigned char c1, c2;

    while (len > 0) {
        c1 = islower(*s1) ? toupper(*s1) : (*s1);
        c2 = islower(*s2) ? toupper(*s2) : (*s2);
        if (c1 != c2) return 0;
        if (c1 == '\0') return 1;
        s1++; s2++;
    	len--;
    }

    return 1;
}

/*
**++
**  ROUTINE:	queue_insert
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Inserts an entry into a queue.  The QUE structure isn't
**  required, as long as the first two longwords of the structure
**  being used is a queue header.
**
**  USED ONLY WITH GNU C.  VAX C and DEC C use the appropriate builtins.
**  	    	    	   See NEWSRDR.H for more information.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	queue_insert(item, pred)
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
#ifdef __GNUC__
void queue_insert(struct QUE *item, struct QUE *pred) {

    item->head = pred->head;
    item->tail = pred;
    (pred->head)->tail = item;
    pred->head = item;
}
#endif

/*
**++
**  ROUTINE:	queue_remove
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Removes an entry from a queue, if there is one.
**
**  USED ONLY WITH GNU C.  VAX C and DEC C use the appropriate builtins.
**  	    	    	   See NEWSRDR.H for more information.
**
**  RETURNS:	int
**
**  PROTOTYPE:
**
**  	queue_remove(struct QUE *entry, void **addr);
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	1: Remove successful.
**  	0: No entry to remove.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
#ifdef __GNUC__
int queue_remove(struct QUE *entry, void **addr) {

    if (entry->head == entry) return 0;
    (entry->tail)->head = entry->head;
    (entry->head)->tail = entry->tail;
    *addr = entry;
    return 1;
}
#endif

/*
**++
**  ROUTINE:	find_image_symbol
**
**  FUNCTIONAL DESCRIPTION:
**
**  	C interface to LIB$FIND_IMAGE_SYMBOL.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	find_image_symbol(char *image, char *symbol, void *symval)
**
**  image:  file_spec, read only, by reference (ASCIZ string)
**  symbol: character string, read only, by reference (ASCIZ)
**  symval: unspecified, write only, by reference
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
unsigned int find_image_symbol(char *image, char *symbol, void *symval) {

    struct dsc$descriptor idsc, sdsc;

    lib$establish(lib$sig_to_ret);
    INIT_SDESC(idsc, strlen(image), image);
    INIT_SDESC(sdsc, strlen(symbol), symbol);
    return x_find_image_symbol(&idsc, &sdsc, symval);

} /* find_image_symbol */

static unsigned int x_find_image_symbol(struct dsc$descriptor *imgnam,
    	    	    	struct dsc$descriptor *symnam, void *symval) {

    return lib$find_image_symbol(imgnam, symnam, symval);

}

/*
**++
**  ROUTINE:	insert_header
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Makes a HDR structure from a character string and inserts
**  it in a queue.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	insert_header(char *str, struct HDR *pred, int code)
**
**  str:    character string, read only, by reference (ASCIZ)
**  pred:   HDR structure, modify, by reference
**  code:   integer, read only, by value
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
void insert_header(char *str, struct HDR *pred, int code) {

    struct HDR *h;
    int len;

    len = strlen(str)+1;
    h = mem_gethdr(len);
    memcpy(h->str, str, len);
    h->code = code;
    queue_insert(h, pred);

} /* insert_header */

/*
**++
**  ROUTINE:	table_parse
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Glue to LIB$TABLE_PARSE/LIB$TPARSE, automatically using the
**  former if present.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	table_parse(void *tpablk, void *tbl, void *key);
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
unsigned int table_parse(void *tpablk, void *tbl, void *key) {

    static unsigned int (*parse_routine)() = 0;
    unsigned int status;

    if (parse_routine == 0) {
    	status = find_image_symbol("LIBRTL", "LIB$TABLE_PARSE", &parse_routine);
    	if (!OK(status)) {
    	    status = find_image_symbol("LIBRTL", "LIB$TPARSE", &parse_routine);
    	}
    	if (!OK(status)) return status;
    }

    return (*parse_routine)(tpablk, tbl, key);

} /* table_parse */

/*
**++
**  ROUTINE:	Check_Connection
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Verifies that the server connection is still there.  If not,
**  attempts to reconnect and restore our server context.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Check_Connection()
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
void Check_Connection() {

    unsigned int status;
    int reply_code;

    status = server_check();
    if (OK(status)) return;

    server_disconnect();
    server_connect(news_cfg.server_name);
    server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);
    if (curgroup) Set_Current_Group(curgroup);
    return;

} /* Check_Connection */

/*
**++
**  ROUTINE:	Thread_Match
**
**  FUNCTIONAL DESCRIPTION:
**
**  	tbs
**
**  RETURNS:	int
**
**  PROTOTYPE:
**
**  	tbs
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
int Thread_Match(char *m1, char *m2) {

    register char *anchor, *cp;
    char tmp[STRING_SIZE];

    anchor = m2;
    while (*anchor != '\0') {
    	while (*anchor != '<' && *anchor != '\0') anchor++;
    	if (anchor == '\0') break;
    	for (cp = anchor; *cp != '\0' && !isspace(*cp); cp++);
    	if (*cp == '\0') {
    	    if (strstr(m1, anchor) != 0) return 1;
    	    anchor = cp;
    	} else {
    	    strncpy(tmp, anchor, cp-anchor);
    	    tmp[cp-anchor] = '\0';
    	    if (strstr(m1, tmp) != 0) return 1;
    	    anchor = cp + 1;
    	}
    }

    return 0;

} /* Thread_Match */
