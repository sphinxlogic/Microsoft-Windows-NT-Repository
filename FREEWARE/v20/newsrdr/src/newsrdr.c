/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	NEWSRDR main module.
**
**  MODULE DESCRIPTION:
**
**  	Main routine for NEWSRDR.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, 1994 MADGOAT SOFTWARE. ALL RIGHTS RESERVED.
**
**  CREATION DATE:  01-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	01-SEP-1992 X4.0    Madison 	Initial coding (conversion from BLISS).
**  	17-SEP-1992 V4.0    Madison 	Make it official.
**  	22-SEP-1992 V4.0-1  Madison 	Fix Display_Announce logic.
**  	22-SEP-1992 V4.0-2  Madison 	Workaround for strange $FAO happenings.
**  	22-SEP-1992 V4.0-3  Madison 	Fix STORE problem.
**  	28-SEP-1992 V4.0-4  Madison 	Fix EXTRACT problem, profile problem.
**  	29-SEP-1992 V4.0-5  Madison 	Fix mail long-lines problem.
**  	30-SEP-1992 V4.0-6  Madison 	Fix yesterday's fix.
**  	07-OCT-1992 V4.0-7  Madison 	Fix REPLY/TO problem, SET SEEN problem.
**  	08-OCT-1992 V4.0-8  Madison 	SHOW VERSION, Parse_List_Reply stuff.
**  	13-OCT-1992 V4.0-9  Madison 	Fix Set_Initial_Groups.
**  	16-OCT-1992 V4.0-10 Madison 	Fix line count in sig file processing.
**  	21-OCT-1992 V4.0-11 Madison 	Fix looping problem in READ/SUBJECT.
**  	07-DEC-1992 V4.0-12 Madison 	Move exit handler decl to after update.
**  	23-DEC-1992 V4.0-13 Madison 	Posting errors were not being handled properly.
**  	10-FEB-1993 V4.0-14 Madison 	Fix PAGER routines to work from batch jobs.
**  	15-FEB-1993 V4.0-15 Madison 	Remove all !AZ references.
**  	15-FEB-1993 V4.1    Madison 	Add SET SEEN/CROSS, /SUBJECT.
**  	17-FEB-1993 V4.1-1  Madison 	Fix up LIB$STOPs in SERVER_xxx modules.
**  	24-FEB-1993 V4.1-2  Madison 	Fix DIRECTORY output problem.
**  	01-MAR-1993 V4.1-3  Madison 	Expand name_conversion support.
**  	17-MAR-1993 V4.1-4  Madison 	Fix REPLY/FOLLOWUP_TO.
**  	23-MAR-1993 V4.1-5  Madison 	Fix FORWARD problem.
**  	05-APR-1993 V4.1-6  Madison 	Fix DST adjustment in date generation.
**  	08-APR-1993 V4.1-7  Madison 	Cmd locasing fix; REPLY/DIST fix.
**  	12-APR-1993 A4.2    Madison 	Add remaining lines to paged displays.
**  	16-APR-1993 B4.2    Madison 	Add support for faking NEWGROUPS.
**  	17-APR-1993 C4.2    Madison 	Add auto-reconnect.
**  	23-APR-1993 D4.2    Madison 	Add DISALLOW_POSTING logical.
**  	14-MAY-1993 V4.2-1  Madison 	Fix wildcard group name processing.
**  	17-MAY-1993 V4.2-2  Madison 	Fix directory paged displays.
**  	19-MAY-1993 V4.2-3  Madison 	Fix Parse_Range bug.
**  	22-MAY-1993 V4.2-4  Madison 	Work around MAIL$ bug in VMS T6.0.
**  	23-MAY-1993 V4.2-5  Madison 	Allow SET SEEN/THREAD with article ranges.
**  	24-MAY-1993 V4.2-6  Madison 	Fix weekday computation in Make_Date.
**  	25-MAY-1993 V4.2-7  Madison 	Fix partial update problem.
**  	31-MAY-1993 V4.2-8  Madison 	Fix looping bug in cont_readsubj.
**  	03-JUN-1993 V4.2-9  Madison 	Got time check backwards in server check.
**  	22-JUN-1993 A4.3    Madison 	SET AUTO_SAVE_PROFILE.
**  	08-JUL-1993 B4.3    Madison 	Let user specify names of keep files.
**  	22-SEP-1993 V4.3    Madison 	New POST, REPLY qualifiers. XHDR. Bug fixes.
**  	24-SEP-1993 V4.4    Madison 	More use of XHDR.
**  	26-SEP-1993 V4.4-1  Madison 	Fix ACCVIO in condition handling on AXP.
**  	28-SEP-1993 V4.4-2  Madison 	Fix header allocation problem.
**  	29-SEP-1993 V4.4-3  Madison 	Found another bug lurking in the XHDR code.
**  	29-SEP-1993 V4.4-4  Madison 	Fix POST/KEYWORDS.
**  	30-SEP-1993 V4.4-5  Madison 	Fix for cmd_dir, another XHDR fix.
**  	02-OCT-1993 V4.4-6  Madison 	Found another bug in ARTICLE.
**  	05-OCT-1993 V4.4-7  Madison 	Fix bug in FORWARD To: prompting.
**  	09-OCT-1993 V4.5    Madison 	All sorts of new stuff.
**  	14-OCT-1993 V4.5-1  Madison 	INN MODE READER support.
**  	21-OCT-1993 V4.5-2  Madison 	Fix for slow DIRECTORY/UNSEEN.
**  	22-OCT-1993 V4.5-3  Madison 	Fix null-subject problem in cmd_dir.
**  	26-OCT-1993 V4.5-4  Madison 	Another XHDR fix.
**  	26-OCT-1993 V4.5-5  Madison 	Pager, CMD_TABLE fixes.
**  	28-OCT-1993 V4.5-6  Madison 	Fix SET SEEN/SUBJECT, directory widths.
**  	10-NOV-1993 V4.5-7  Madison 	ACCVIO server_check fix, etc.
**  	23-NOV-1993 V4.5-8  Madison 	Fix up globals.
**  	01-DEC-1993 V4.5-9  Madison 	CAPTIVE flag fix for AXP/VMS.
**  	05-DEC-1993 V4.5-10 Madison 	Have EXTRACT check validity of articles.
**  	15-JAN-1994 V4.5-11 Madison 	More EXTRACT fixes.
**  	16-MAR-1994 V4.6    Madison 	SET IGNORE/MESSAGE_ID.
**  	28-MAR-1994 V4.6-1  Madison 	Check Copy_File status on saves.
**  	13-APR-1994 V4.7    Madison 	Start of some XOVER support.
**  	28-APR-1994 V4.7-1  Madison 	Rework hole checking.
**  	29-APR-1994 V4.7-2  Madison 	Fixed XOVER support broken in V4.7-1.
**  	16-MAY-1994 X4.8    Madison 	Add mixed-case group name support,
**  	    	    	    	    	    support for settable reply-prefix.
**  	22-MAY-1994 Y4.8    Madison 	Profile/newsrc writing improvements.
**  	23-MAY-1994 Y4.8-1  Madison 	Add /NEWGROUP_ACTION qualifier.
**  	25-MAY-1994 V4.8    Madison 	Removed SET_HANDLER stuff.
**  	16-JUN-1994 V4.8-1  Madison 	Fixed skipped-articles-after-update problem.
**  	06-JUL-1994 V4.8-2  Madison 	XOVER, NEWSRC, profile-writing fixes.
**  	21-SEP-1994 V4.8-3  Madison 	autosave fix, glist fix.
**  	08-DEC-1994 V4.8-4  Madison 	Fixed References: header formatting.
**  	09-JAN-1995 V4.8-5  Madison 	Fixed XHDR ACCVIO.
**  	20-JUN-1995 V4.8-6  Madison 	Keypad fix, increased default hdr cache.
**--
*/
#define NR_VERSION	"V4.8-6"
#define NR_COPYRIGHT	"Copyright © 1993, 1994, 1995  MadGoat Software.  All Rights Reserved."

#ifdef __DECC
#pragma module NEWSRDR NR_VERSION
#else
#ifndef __GNUC__
#module NEWSRDR NR_VERSION
#endif
#endif

#include "newsrdr.h"
#ifdef __GNUC__
#include <vms/jpidef.h
#include <vms/prvdef.h>
#include <vms/lnmdef.h>
#else
#include <jpidef.h>
#include <prvdef.h>
#include <lnmdef.h>
#endif
#include <signal.h>

#ifdef __DECC
#pragma extern_model save
#pragma extern_model common_block noshr
#endif
    char $$$Copyright[]     = NR_COPYRIGHT;
#ifdef __DECC
#pragma extern_model restore
#endif
    GLOBAL char *$$$Version = NR_VERSION;

/*
** Forward declarations
*/
    unsigned int main();
    static unsigned int Read_Config(void);
    static unsigned int exit_handler();
    static void Display_Announce(void);
    static void Set_Initial_Groups(void);

    GLOBAL unsigned int (*default_action)(); /* Fired when user just hits RETURN */
    GLOBAL unsigned int (*cleanup_action)(); /* Cleanup for non-default action   */
    GLOBAL struct PROF news_prof;   	     /* The user profile                 */
    GLOBAL struct CFG  news_cfg;    	     /* Configuration information        */
    GLOBAL unsigned int image_privs[2];      /* Our image privilege mask         */
    GLOBAL int pager_set_up = 0;             /* PAGER module initialization flag */

    static unsigned int final_status;        /* Final status for exit handler    */
    static struct {                          /* Exit handler block               */
    	unsigned int flink;
    	unsigned int (*exh)();
    	unsigned int argcnt;
    	unsigned int *statusp;
    } exhblk = {0, exit_handler, 1, &final_status};

/*
**  External references
*/
    EXTERN int User_Interrupt;               /* User pressed CTRL/C              */

#ifdef __GNUC__
#define RMS$_EOF ((unsigned int) rms$_eof)
    extern unsigned int rms$_eof();
#else
#pragma nostandard
    globalvalue unsigned int RMS$_EOF;
#pragma standard
#endif

    extern unsigned int cli$dispatch(void);
    extern unsigned int newsrdr_cld(), cmd_table();
    extern unsigned int cli_error_filter();
    extern void Read_Profile(void);
    extern unsigned int Set_Initial_Group(struct GRP *, struct dsc$descriptor *);
    extern struct GRP *Find_Group(char *);
    extern struct GRP *Find_Group_Wild(char *, unsigned int *);
    extern void Traverse_Finish(unsigned int *);
    extern void cmd_initialize(void);
    extern unsigned int cmd_exit(void);
    extern unsigned int cmd_update(void);
    extern unsigned int cmd_readnextnew(void);
    extern unsigned int Do_Full_Update(int);
    extern void Make_Return_Address(char *, int);
    extern void Clear_ArtInProg(void);
    extern void Article_ExH(void);
    extern void Write_Profile(void);
    extern void print_clup(void);
    extern void Check_New_Groups(int);

/*
**++
**  ROUTINE:	main
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Main program.  Initializes things.  Parses the NEWSRDR command,
**  Contacts the NNTP server.  Identifies new groups and does a new article
**  check.  Goes into main command loop.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	main
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion.
**
**  SIDE EFFECTS:   	Many.
**
**--
*/
unsigned int main() {

    unsigned int jpi_imagpriv=JPI$_IMAGPRIV, jpi_procpriv=JPI$_PROCPRIV;
    unsigned int procprivs[2];
    $DESCRIPTOR(newsrdr_cmd, "NEWSRDR ");
    struct dsc$descriptor str;
    unsigned int status, reply_code, update_type;
    int finish_up, ng_action;
    char tmp[STRING_SIZE];

/*
** Turn off all image privileges, except NETMBX.  We shouldn't need
** any privileges to do anything, unless the foreign mail protocol
** we send messages through needs them.
*/
    lib$getjpi(&jpi_imagpriv, 0, 0, image_privs);
    lib$getjpi(&jpi_procpriv, 0, 0, procprivs);
    image_privs[0] &= ~procprivs[0];
    image_privs[1] &= ~procprivs[1];
    image_privs[0] &= ~PRV$M_NETMBX;
    sys$setprv(0, image_privs, 0, 0);

/*
** Our all-purpose condition handler is below.  It filters any signals
** coming from CLI$ routines.  Other errors it prints out using our
** output routines.
*/
    lib$establish(cli_error_filter);

/*
** Parse the command.
*/
    INIT_DYNDESC(str);
    lib$get_foreign(&str);
    str$prefix(&str, &newsrdr_cmd);
    status = cli$dcl_parse(&str, newsrdr_cld, lib$get_foreign, lib$get_foreign);
    if (!OK(status)) lib$stop(NEWS__CMDERR, 0, status);

/*
** Determine the update type.  FULL means use NNTP LIST (default); PARTIAL
** means use NNTP GROUP commands for each subscribed group.  BOTH means
** do full followed by partial update (the GROUP command returns more
** accurate information about each newsgroup).
*/

    update_type = 2;
    status = cli_present("UPDATE");
    if (status == CLI$_PRESENT) {
    	status = cli_get_value("UPDATE", tmp, sizeof(tmp));
    	if (OK(status)) {
    	    static char *both={"BOTH"}, *full={"FULL"};
    	    upcase(tmp);
    	    if (strstr(both, tmp) == both) update_type = 3;
    	    else if (strstr(full, tmp) != full) update_type = 1;
    	}
    } else if (status == CLI$_NEGATED) update_type = 0;

/*
**  Check to see if newgroup_action is to be overridden.
*/
    ng_action = -1;
    if (cli_present("NEWGROUP_ACTION") == CLI$_PRESENT) {
    	cli_get_value("NEWGROUP_ACTION", tmp, sizeof(tmp));
    	if (tmp[0] == 'S') ng_action = NG_K_SUBSCRIBE;
    	else if (tmp[0] == 'P') ng_action = NG_K_PROMPT;
    	else ng_action = NG_K_NONE;
    }

/*
** Initialize stuff
*/
    Read_Config();

    pager_set_up = Pager_Init(0);

    Display_Announce();

    Read_Profile();
/*
** Connect to server, and check for new groups.
*/
    lib$signal(NEWS__CONNECTING, 2, strlen(news_cfg.server_name),
    	    	    news_cfg.server_name);
    server_connect(news_cfg.server_name);

/*
**  If posting isn't disallowed by logical name NEWSRDR_DISALLOW_POSTING,
**  check to see if it's OK with the server.
*/
    server_get_reply(SRV__ECHO, &reply_code, 0, 0, 0);
    if (reply_code != NNTP__HELOPOSTOK && reply_code != NNTP__HELONOPOST) {
    	server_disconnect();
    	lib$stop(NEWS__NOSERVICE, 2, strlen(news_cfg.server_name),
    	    news_cfg.server_name);
    }

    if (news_cfg.postingok && reply_code == NNTP__HELONOPOST) {
    	news_cfg.postingok = 0;
    }

/*
**  INN's server uses a MODE READER command to determine whether or not the
**  client is a reader or a peer news system.
*/
    if (OK(get_logical("NEWSRDR_INN_SERVER", tmp))) {
    	if (strchr("TtYy1", tmp[0]) != 0) {
    	    server_send("MODE READER");
    	    server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);
    	}
    }

    if (news_prof.profread) Check_New_Groups(ng_action);

/*
** Command processing initialization - inits the default_action stuff.
*/
    cmd_initialize();

/*
** Our let's-go-home flag
*/
    finish_up = 0;

/*
** Do the newsgroup update.  For first-time users (no profile exists),
** do a full update and set up the initial groups.  For other users,
** the update is controlled by what they put on the NEWSRDR command.
*/
    if (news_prof.profread && (update_type != 0)) {
    	lib$signal(NEWS__UPDATING, 0);
    	switch (update_type) {
    	    case 1: cmd_update(); break;
    	    case 2: Do_Full_Update(0); break;
    	    case 3: Do_Full_Update(1); cmd_update(); break;
    	}
    } else if (!news_prof.profread) {
    	Do_Full_Update(1);
    	Set_Initial_Groups();
    	lib$signal(NEWS__UPDATING, 0);
    	cmd_update();
    }
/*
** Set up exit handler (below) to write our profile back out when
** we're through.
*/
    sys$dclexh(&exhblk);

/*
** Main command loop.
*/
    while (!finish_up) {
    	char *cp;

    	put_output("");
    	User_Interrupt = 0;
    	status = get_cmd(tmp+6, sizeof(tmp)-6, "News> ");
/*
** The only non-OK status we expect here is RMS$_EOF -- i.e., EXIT.
*/
    	if (!OK(status)) {
    	    if (cleanup_action != (unsigned int (*)()) 0) (*cleanup_action)();
    	    cmd_exit();
    	    break;
    	}

/*
** Trim blanks from the command
*/
    	for (cp = tmp+strlen(tmp); cp > tmp+6 && isspace(*(cp-1)); cp--);
    	*cp = '\0';
    	for (cp = tmp+6; *cp && isspace(*cp); cp++);

/*
** If it's a null command, perform the default_action routine.  If there
** is no default action, use READ/NEXT/NEW.
**
** Otherwise, if it's all digits then assume it's an article number to
** be read; if it's just a group name, then assume they want to go to
** that group.
*/
    	if (*cp == '\0') {
    	    if (default_action == (unsigned int (*)()) 0) {
    	    	finish_up = cmd_readnextnew() == NEWS__ALLDONE;
    	    } else {
    	    	finish_up = (*default_action)() == NEWS__ALLDONE;
    	    }
    	} else {
    	    struct dsc$descriptor tmpdsc;
    	    $DESCRIPTOR(Prompt, "News> ");
    	    if (strspn(cp, "0123456789") == strlen(cp)) {
    	    	cp -= 5;
    	    	memcpy(cp, "READ ", 5);
    	    } else if (strchr(cp, ' ') == NULL) {
    	    	locase(cp);
    	    	if (Find_Group(cp) == 0) {
    	    	    unsigned int ctx;
    	    	    struct GRP *g;

    	    	    if (strchr(cp, '*') != 0 || strchr(cp, '%') != 0) {
    	    	    	ctx = 0;
    	    	    	while ((g = Find_Group_Wild(cp, &ctx)) != 0) {
    	    	    	    if (g->subscribed) {
    	    	    	    	Traverse_Finish(&ctx);
    	    	    	    	break;
    	    	    	    }
    	    	    	}
    	    	    	if (g != 0) {
    	    	    	    strcpy(tmp, "GROUP ");
    	    	    	    strcat(tmp, g->grpnam);
    	    	    	    cp = tmp;
    	    	    	}
    	    	    }

    	    	} else {
    	    	    cp -= 6;
    	    	    memcpy(cp, "GROUP ", 6);
    	    	}
    	    }

/*
** Parse the command and perform the specified action.  If the parse is
** successful or we need to exit due to an end-of-file, perform the
** cleanup action first (if any).
*/
    	    INIT_SDESC(tmpdsc, strlen(cp), cp);
    	    status = cli$dcl_parse(&tmpdsc, cmd_table, get_cmd_dx, get_cmd_dx,
    	    	&Prompt);
    	    if (status == RMS$_EOF) {
    	    	if (cleanup_action != (unsigned int (*)()) 0) (*cleanup_action)();
    	    	cmd_exit();
    	    	break;
    	    } else {
    	    	if (!OK(status)) lib$signal(NEWS__CMDERR, 0, status);
    	    	else {
    	    	    if (cleanup_action != (unsigned int (*)()) 0)
    	    	    	(*cleanup_action)();
    	    	    finish_up = cli$dispatch() == NEWS__ALLDONE;
    	    	}
    	    }
    	}
    }

/*
** Say good-night to the server.
*/

    server_send("QUIT");
    server_get_reply(SRV__ECHO, &reply_code, 0, 0, 0);
    server_disconnect();

    return SS$_NORMAL;

}  /* main */

/*
**++
**  ROUTINE:	Read_Config
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Sets up the news_cfg structure, which consists of items
**  of information that aren't directly settable by the user (at
**  least, not within NEWSRDR).  
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Read_Config()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion.
**
**  SIDE EFFECTS:   	Changes news_cfg.
**
**--
*/
static unsigned int Read_Config() {

    ITMLST jpilst[3];
    char tmp[STRING_SIZE];
    unsigned int prcprv[2], status;
    unsigned short tlen;
    int ok;

    memset(&news_cfg, 0, sizeof(struct CFG));
    news_cfg.bangpath = news_cfg.newgroups = news_cfg.dopath = 1;

    news_cfg.postingok = !OK(get_logical("NEWSRDR_DISALLOW_POSTING", tmp));

    ITMLST_INIT(jpilst[0], JPI$_USERNAME, sizeof(tmp), tmp, &tlen);
    ITMLST_INIT(jpilst[1], JPI$_PROCPRIV, sizeof(prcprv), prcprv, 0);
    ITMLST_INIT(jpilst[2], 0, 0, 0, 0);
    status = sys$getjpiw(0, 0, 0, jpilst, 0, 0, 0);
    if (!OK(status)) return status;
    while (isspace(tmp[tlen-1])) tlen--; tmp[tlen] = '\0';
    strcpy(news_cfg.username, tmp);

    status = get_system_logical("NEWSRDR_SERVER", news_cfg.server_name);
    if (!OK(status) || (prcprv[0]&PRV$M_SETPRV) || (prcprv[0]&PRV$M_SYSNAM)) {
    	status = get_logical("NEWSRDR_SERVER", news_cfg.server_name);
    }
    if (!OK(status)) return status;

    status = get_system_logical("NEWSRDR_NODE_NAME", news_cfg.node_name);
    if (!OK(status)) {
    	status = get_hostname(news_cfg.node_name, sizeof(news_cfg.node_name));
    }
    if (!OK(status)) return status;

    if (!OK(get_system_logical("NEWSRDR_MAIL_NODE", news_cfg.mailnode))) {
    	strcpy(news_cfg.mailnode, news_cfg.node_name);
    }

    get_logical("NEWSRDR_ORGANIZATION", news_cfg.org_name);
    get_logical("NEWSRDR_MAIL_PROTOCOL", news_cfg.mail_proto);
    get_logical("NEWSRDR_PATH_STRING", news_cfg.pathstr);

    ok = 1;
    if (OK(get_system_logical("NEWSRDR_DISABLE_USER_REPLY_TO", tmp))) {
    	ok = strchr("TtYy1", tmp[0]) == NULL;
    }
    if (ok) ok = OK(get_logical("NEWSRDR_REPLY_TO", news_cfg.reply_to));
    if (!ok) Make_Return_Address(news_cfg.reply_to, sizeof(news_cfg.reply_to));

    if (OK(get_logical("NEWSRDR_BANG_PATH", tmp))) {
    	news_cfg.bangpath = strchr("TtYy1", tmp[0]) != NULL;
    }
    if (OK(get_logical("NEWSRDR_BANG_ADDRESS", tmp))) {
    	news_cfg.bangaddr = strchr("TtYy1", tmp[0]) != NULL;
    }
    if (OK(get_logical("NEWSRDR_DO_MESSAGE-ID", tmp))) {
    	news_cfg.genmsgid = strchr("TtYy1", tmp[0]) != NULL;
    }
    if (OK(get_logical("NEWSRDR_DO_DATE", tmp))) {
    	news_cfg.gendate = strchr("TtYy1", tmp[0]) != NULL;
    }
    if (OK(get_logical("NEWSRDR_DO_NEWGROUPS", tmp))) {
    	news_cfg.newgroups = strchr("TtYy1", tmp[0]) != NULL;
    }
    if (!news_cfg.newgroups && OK(get_logical("NEWSRDR_FAKE_NEWGROUPS", tmp))) {
    	news_cfg.newgroups = strchr("TtYy1", tmp[0]) == NULL ? 0 : 2;
    }

    if (OK(get_logical("NEWSRDR_DO_PATH", tmp))) {
    	news_cfg.dopath = strchr("TtYy1", tmp[0]) != NULL;
    }

    if (OK(get_logical("NEWSRDR_NO_XHDR", tmp))) {
    	news_cfg.xhdr = (strchr("TtYy1", tmp[0]) != NULL) ? -1 : 0;
    }

    if (OK(get_logical("NEWSRDR_NO_XOVER", tmp))) {
    	news_cfg.xover = (strchr("TtYy1", tmp[0]) != NULL) ? -1 : 0;
    }

    if (OK(get_logical("NEWSRDR_HEADER_CACHE_SIZE", tmp))) {
    	if (!OK(lib$cvt_dtb(strlen(tmp), tmp, &news_cfg.cachesize))) {
    	    news_cfg.cachesize = 512;
    	}
    } else news_cfg.cachesize = 512;

    if (OK(get_logical("NEWSRDR_GMT_OFFSET", tmp))) {
    	char *cp;
    	struct dsc$descriptor tmpdsc;
    	if (!isdigit(tmp[0])) {
    	    news_cfg.neggmtoff = tmp[0] == '-';
    	    cp = &tmp[1];
    	} else cp = tmp;
    	INIT_SDESC(tmpdsc, strlen(cp), cp);
    	sys$bintim(&tmpdsc, &news_cfg.gmtoffset);
    } else {
    	$DESCRIPTOR(est, "0 05:00:00");
    	sys$bintim(&est, &news_cfg.gmtoffset);
    	news_cfg.neggmtoff = 1;
    	news_cfg.dst = 1;
    }

    if (OK(get_logical("NEWSRDR_US_DST_ZONE", tmp))) {
    	news_cfg.dst = strchr("TtYy1", tmp[0]) != NULL;
    }

    if (OK(get_logical("NEWSRDR_CHARACTER_CONVERSION", tmp))) {
    	status = find_image_symbol("NEWSRDR_CHARACTER_CONVERSION",
    	    	    "LOCAL_TO_NETWORK", &news_cfg.chrlton);
    	if (OK(status)) {
    	    status = find_image_symbol("NEWSRDR_CHARACTER_CONVERSION",
    	    	    "NETWORK_TO_LOCAL", &news_cfg.chrntol);
    	}
    	news_cfg.chrcnv = OK(status);
    }

    if (OK(get_system_logical("NEWSRDR_NAME_CONVERSION", tmp))) {

    	unsigned int (*init)();

    	status = find_image_symbol("NEWSRDR_NAME_CONVERSION",
    	    	    "INIT", &init);
    	if (OK(status)) {
    	    status = find_image_symbol("NEWSRDR_NAME_CONVERSION",
    	    	    "CONVERT", &news_cfg.namcvt);
    	}
    	if (OK(status)) {
    	    (void) find_image_symbol("NEWSRDR_NAME_CONVERSION",
    	    	    "FULL_CONVERT", &news_cfg.fnmcvt);
    	}
    	if (OK(status)) {
    	    status = find_image_symbol("NEWSRDR_NAME_CONVERSION",
    	    	    "CLEANUP", &news_cfg.namclup);
    	}
    	if (OK(status)) status = (*init)(&news_cfg.namctx);
    	news_cfg.namcnv = OK(status);
    }

    if (OK(get_system_logical("NEWSRDR_ADDRESS_CONVERSION", tmp))) {

    	unsigned int (*init)();

    	status = find_image_symbol("NEWSRDR_ADDRESS_CONVERSION",
    	    	    "INIT", &init);
    	if (OK(status)) {
    	    status = find_image_symbol("NEWSRDR_ADDRESS_CONVERSION",
    	    	    "CONVERT", &news_cfg.adrcvt);
    	}
    	if (OK(status)) {
    	    status = find_image_symbol("NEWSRDR_ADDRESS_CONVERSION",
    	    	    "CLEANUP", &news_cfg.adrclup);
    	}
    	if (OK(status)) status = (init)(&news_cfg.adrctx);
    	news_cfg.adrcnv = OK(status);
    }

    return SS$_NORMAL;

} /* Read_Config */

/*
**++
**  ROUTINE:	exit_handler
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Exit handler for NEWSRDR.  Calls cleanup routines to delete
**  temporary files and such, and writes out the NEWSRDR_PROFILE file.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	exit_handler(unsigned int *final_status)
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
static unsigned int exit_handler(unsigned int *final_status) {

    Clear_ArtInProg();
    Article_ExH();
    Write_Profile();
    print_clup();
    if (news_cfg.namcnv) (*news_cfg.namclup)(&news_cfg.namctx);
    if (news_cfg.adrcnv) (*news_cfg.adrclup)(&news_cfg.namctx);

    return SS$_NORMAL;

} /* exit_handler */

/*
**++
**  ROUTINE:	Display_Announce
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Displays the announcement message, if one has been set up.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Display_Announce()
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
static void Display_Announce() {

    unsigned int status, unit;
    char annfile[FSPEC_SIZE], tmp[STRING_SIZE], *cp;
    int tlen;

    status = get_logical("NEWSRDR_SUPPRESS_COPYRIGHT", tmp);
    if (!OK(status)) {
    	strcpy(tmp, "NEWSRDR ");
    	strcat(tmp, $$$Version);
    	put_output(tmp);
    	put_output($$$Copyright);
    }
    status = get_logical("NEWSRDR_ANNOUNCE", annfile);
    if (!OK(status) || *annfile == '\0') return;

    put_output("");
    if (*annfile == '@') {
    	for (cp = annfile+1; isspace(*cp); cp++);
    	status = file_open(cp, &unit, 0, 0, 0);
    	if (!OK(status)) return;
    	while (OK(file_read(unit, tmp, sizeof(tmp)-1, &tlen))) {
    	    tmp[tlen] = '\0';
    	    put_output(tmp);
    	}
    	file_close(unit);
    } else {
    	put_output(annfile);
    }

} /* Display_Announce */

/*
**++
**  ROUTINE:	Set_Initial_Groups
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Subscribes a new user to an initial set of newsgroups.
**  The logical name NEWSRDR_INITIAL_GROUPS is translated from the
**  process, job, group, and system logical name tables; the user is
**  subscribed to all newsgroups resulting from each of those translations.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Set_Initial_Groups()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	Changes the group tree.
**
**--
*/
static void Set_Initial_Groups() {

    ITMLST lnmlst[3];
    char lnmbuf[256];
    struct dsc$descriptor tabnam, sdsc;
    $DESCRIPTOR(lognam, "NEWSRDR_INITIAL_GROUPS");
    static char *table[] = {"LNM$PROCESS","LNM$JOB","LNM$GROUP","LNM$SYSTEM"};
    int i, maxidx;
    short len;
    unsigned int status;

    ITMLST_INIT(lnmlst[2],0,0,0,0);
    for (i = 0; i < sizeof(table)/sizeof(char *); i++) {
    	ITMLST_INIT(lnmlst[0],LNM$_MAX_INDEX,sizeof(maxidx),&maxidx,0);
    	ITMLST_INIT(lnmlst[1],0,0,0,0);
    	INIT_SDESC(tabnam,strlen(table[i]),table[i]);
    	status = sys$trnlnm(0, &tabnam, &lognam, 0, lnmlst);
    	if (OK(status)) {
    	    int j;
    	    for (j = 0; j <= maxidx; j++) {
    	    	ITMLST_INIT(lnmlst[0],LNM$_INDEX,sizeof(j),&j,0);
    	    	ITMLST_INIT(lnmlst[1],LNM$_STRING,sizeof(lnmbuf)-1,lnmbuf,&len);
    	    	status = sys$trnlnm(0, &tabnam, &lognam, 0, lnmlst);
    	    	if (OK(status)) {
    	    	    *(lnmbuf+len) = '\0';
    	    	    locase(lnmbuf);
    	    	    INIT_SDESC(sdsc, len, lnmbuf);
    	    	    lib$traverse_tree(&news_prof.gtree, Set_Initial_Group,
    	    	    	    &sdsc);
    	    	}
    	    }
    	}
    }
} /* Set_Initial_Groups */
