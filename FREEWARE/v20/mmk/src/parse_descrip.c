/*
**++
**  FACILITY:	MMK
**
**  ABSTRACT:	Description file parser.
**
**  MODULE DESCRIPTION:
**
**  	This module contains routine PARSE_DESCRIP.
**
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT � 1992-1995, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  20-AUG-1992
**
**  MODIFICATION HISTORY:
**
**  	20-AUG-1992 V1.0    Madison 	Initial coding.
**  	29-SEP-1992 V1.1    Madison 	Support OBJREFs.
**  	12-OCT-1992 V1.1-1  Madison 	Fix some dependency rule parsing problems.
**  	02-APR-1993 V1.2    Madison 	Support '-' cmd pfx, comments in cmds.
**  	29-APR-1993 V1.2-1  Madison 	Fix dependency target handling.
**  	04-JUN-1993 V1.3    Madison 	Support for new directives.
**  	28-OCT-1993 V1.3-1  Madison 	Fix behavior of .SUFFIXES directive,
**  	    	    	    	    	    symbol definitions.
**  	12-DEC-1993 V1.4    Madison 	Support multiple dependencies for
**  	    	    	    	    	    single object.
**  	15-DEC-1993 V1.4-1  Madison 	Fix @-, -@ in action lines.
**  	03-MAR-1994 V1.4-2  Madison 	Resolve symbols on all lines.
**  	04-APR-1994 V1.4-3  Madison 	Don't resolve symbols on _all_ lines (!)
**  	08-APR-1994 V1.4-4  Madison 	Fix space-sep lists on lhs of dep rules.
**  	28-JUN-1994 V1.4-5  Madison 	Link object module to file in target spec.
**  	12-JUL-1994 V1.5    Madison 	Fix conditionals, add :: support.
**  	14-JUL-1994 V1.6    Madison 	Add prefixing on inference rules.
**  	22-AUG-1994 V1.6-1  Madison 	Remove LIBMOD check on dependency creation.
**  	18-OCT-1994 V1.6-2  Madison 	Don't free line until after we signal error!
**  	02-DEC-1994 V1.7    Madison 	Add line numbers to syntax error msgs.
**  	28-DEC-1994 V1.8    Madison 	Allow multiple dependencies for a target
**  	    	    	    	    	  as long as only one has an action list.
**  	    	    	    	    	  Add deferred symbol references.
**  	    	    	    	    	  Add .IF directive.
**  	10-JAN-1995 V1.8-1  Madison 	"defined" means set to non-null value.
**  	11-JAN-1995 V1.8-2  Madison 	Clear current_deplist on rule or .FIRST/.LAST.
**  	21-JUN-1995 V1.8-3  Madison 	Change to Define_Symbol.
**  	22-JUN-1995 V1.8-4  Madison 	Fix for CMD_TEXT ACCVIO.
**--
*/
#ifdef __DECC
#pragma module PARSE_DESCRIP "V1.8-4"
#else
#ifndef __GNUC__
#module PARSE_DESCRIP "V1.8-4"
#endif
#endif
#include "mmk.h"
#include "globals.h"
#include "tpadef.h"

#pragma nostandard
#include "mmk_msg.h"
#pragma standard

/*
** TPARSE context block.  A basic TPARSE block plus some
** extras of our own.
*/

#define TPA_C_LENGTH	(TPA$C_LENGTH0+16)
#define TPA_K_COUNT 	(TPA$K_COUNT0+4)

    struct TPABLK {
    	struct tpadef tpa0;
    	char    	  *tpa_l_stringbase;
    	char    	  *tpa_l_upbase;
    	unsigned int	  *tpa_l_unit;
    	int 	    	  *tpa_l_maxlen;
    };

/*
** Forward declarations.
*/
    void parse_descrip(char *, int, unsigned int *, int *, int, char *);
    int parse_store(struct TPABLK *);
    static void make_objrefs(struct QUE *, struct QUE *);
    static void copy_objrefs(struct OBJREF *, struct QUE *);

/*
** Parse function codes.  Must match counterparts in PARSE_TABLE.MAR.
*/
#define PRS_K_CHECK_COND     0
#define PRS_K_CMD_INIT	     1
#define PRS_K_SYM_INIT	     2
#define PRS_K_DEP_INIT	     3
#define PRS_K_DIR_SFX	     4
#define PRS_K_DIR_FIRST	     5
#define PRS_K_DIR_LAST	     6
#define PRS_K_RULE_INIT	     7
#define PRS_K_DIR_RHS	     8
#define PRS_K_RULE_NEWSFX    9
#define PRS_K_RULE_SFX	    10
#define PRS_K_CMD_NOECHO    11
#define PRS_K_CMD_TEXT	    12
#define PRS_K_SYM2DEP	    13
#define PRS_K_SYM_VALUE	    14
#define PRS_K_DEP_TRGAPP    15
#define PRS_K_DEP_RHS	    16
#define PRS_K_RULE_END      17
#define PRS_K_DIR_IFDEF     18
#define PRS_K_DIR_ELSE      19
#define PRS_K_DIR_ENDIF	    20
#define PRS_K_SYM2DEP2	    21
#define PRS_K_CMD_IGNERR    22
#define	PRS_K_DIR_SILENT    23
#define PRS_K_DIR_IGNORE    24
#define PRS_K_DIR_DEFAULT   25
#define PRS_K_DIR_INCLUDE   26
#define PRS_K_CMD_BOTHNI    27
#define PRS_K_DEP_TRGAPP2   28
#define PRS_K_DEP_DC	    29
#define PRS_K_RULE_INIPFX   30
#define PRS_K_RULE_NEWPFX   31
#define PRS_K_DIR_IFLHS	    32
#define PRS_K_DIR_IFEQL	    33
#define PRS_K_DIR_IFNEQ	    34
#define PRS_K_DIR_IFRHS	    35

#ifdef __GNUC__
#define LIB$_SYNTAXERR ((unsigned int) lib$_syntaxerr)
    extern int lib$_syntaxerr();
#else
#pragma nostandard
    globalvalue unsigned int LIB$_SYNTAXERR;
#pragma standard
#endif

/*
** The parse table (from PARSE_TABLE.MAR)
*/
    extern int parse_state(), parse_key();

/*
** .IFDEF context block.  Used for tracking when we're in and out
** of .IFDEF/.ELSE/.ENDIF constructs in description files.
*/

    static struct IF {
    	struct IF *flink, *blink;
    	int do_it, in_else;
    } ifque = {&ifque,&ifque,0,0};

    static int just_did_rule = 0;

/*
** External references.
*/

    extern struct CMD *mem_get_cmd(void);
    extern struct DEPEND *mem_get_depend(void);
    extern struct SYMBOL *mem_get_symbol(void);
    extern struct RULE *mem_get_rule(void);
    extern struct OBJECT *mem_get_object(void);
    extern struct OBJREF *mem_get_objref(void);
    extern void mem_free_object(struct OBJECT *);
    extern void mem_free_objref(struct OBJREF *);
    extern void mem_free_rule(struct RULE *);
    extern void mem_free_symbol(struct SYMBOL *);
    extern void Resolve_Symbols(char *, int, char **, int *, int);
    extern void Define_Symbol(SYMTYPE, char *, char *, int);
    extern struct SYMBOL *Lookup_Symbol(char *);
    extern void Parse_Objects(void *, int, struct QUE *, int);
    extern struct SFX *find_suffix(char *);
    extern struct RULE *find_rule(char *, char *);
    extern unsigned int file_exists(char *, char *);
    extern unsigned int file_open(char *, unsigned int *, char *, char *, int *);
    extern void upcase(char *);
    extern int strneql_case_blind(char *, char *, int);
    extern void Build_Suffix_List(char *, int);
    extern struct OBJECT *Find_Object(struct OBJECT *);
    extern void Insert_Object(struct OBJECT *);
    extern struct DEPEND *find_dependency(struct OBJECT *, int);
    extern int make_object_name(char *, struct OBJECT *);

#ifdef __ALPHA
    extern unsigned int lib$table_parse();
#define lib$tparse lib$table_parse
#endif

/*
**++
**  ROUTINE:	parse_descrip
**
**  FUNCTIONAL DESCRIPTION:
**
**  RETURNS:	void (errors are signaled)
**
**  PROTOTYPE:
**
**  	parse_descrip(char *line, int linelen, unsigned int *newu, int *newmaxl)
**
**  IMPLICIT INPUTS:	All the globals.
**
**  IMPLICIT OUTPUTS:	All the globals.
**
**  COMPLETION CODES:
**
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void parse_descrip(char *xline, int xlinelen, unsigned int *newu, int *newmaxl,
    	    	    	int current_line, char *current_file) {
    
    struct TPABLK tpablk;
    char *upline, *line;
    int i, linelen;
    unsigned int status;

/*
** Resolve symbols, leaving untouched any symbols that cannot be
** resolved at this point.
*/
    if (just_did_rule && xlinelen > 0 && isspace(*xline)) {
    	line = xline;
    	linelen = xlinelen;
    } else {
    	Resolve_Symbols(xline, xlinelen, &line, &linelen, 1);
    }

/*
** To be able to match the keywords in the parse table, we must be
** upper case.  For those items that are case sensitive, parse_store
** uses the token's offset from the beginning of the upcase string
** as the offset into the mixed-case string for extracting the token
** value.
*/
    upline = malloc(linelen+1);
    memcpy(upline,line,linelen);
    *(upline+linelen) = '\0';
    upcase(upline);

    memset(&tpablk, 0, TPA_C_LENGTH);

    tpablk.tpa0.tpa$l_count = TPA_K_COUNT;
    tpablk.tpa0.tpa$l_options = TPA$M_BLANKS;
    tpablk.tpa0.tpa$l_stringcnt = linelen;
    tpablk.tpa0.tpa$l_stringptr = upline;
    tpablk.tpa_l_stringbase = line;
    tpablk.tpa_l_upbase = upline;
    tpablk.tpa_l_unit = newu;
    tpablk.tpa_l_maxlen = newmaxl;

    status = lib$tparse(&tpablk, parse_state, parse_key);

    free(upline);
    if (!OK(status) && status != MMK__CONDSKIP)
    	    lib$signal(MMK__PARSERR, 2, linelen, line,
    	    	       MMK__ERRLOC, 3, current_line, strlen(current_file), current_file,
    	    	       status);
    if (line != xline) free(line);

} /* parse_descrip */

/*
**++
**  ROUTINE:	PARSE_STORE
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Action routine for use with LIB$TPARSE.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	PARSE_STORE  flag, inplen, inp, toklen, tok, char, number,
**  	    	    	usrarg
**
** The first eight arguments are the LIB$TPARSE standard argument block.
** LIB$TABLE_PARSE just passes the address of the context block, rather
** than passing the context fields as the routine arguments.
**
** This routine is non-reentrant.
**
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**  	
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int parse_store(struct TPABLK *tpa) {

    int len, i;
    char *cp, *cp1;
    struct SYMBOL *s;
    unsigned int status;
    static struct CMD *current_cmd;
    static struct SYMBOL *current_sym;
    static struct QUE *current_cmdque, trgque, srcque, refque;
    static struct RULE *current_rule, *r, *xr;
    static struct DEPEND *current_deplist = 0;
    static int current_dirtype;
    static char *sfx_ptr;
    static char *trg_str, *src_str;
    static struct dsc$descriptor iflhs, ifrhs;
    static unsigned int iftype;
    static int trg_str_size, trg_str_len;
    static int double_colon;
    struct OBJECT *obj;
    struct DEPEND *dep;
    struct IF *ifent;

/*
** We handle .IFDEF/.ELSE/.ENDIF sequences first, since we don't want
** to be interpreting stuff that's inside a dead block.
*/

    switch (tpa->tpa0.tpa$l_param) {
    	char symnam[33];

    case PRS_K_CHECK_COND:
    	for (ifent = ifque.flink; ifent != &ifque; ifent = ifent->flink) {
    	    if (!ifent->do_it) return MMK__CONDSKIP;
    	}
    	return SS$_NORMAL;
    	break;

    case PRS_K_DIR_IFDEF:
    	ifent = malloc(sizeof(struct IF));
    	queue_insert(ifent, &ifque);
    	strncpy(symnam, tpa->tpa0.tpa$l_tokenptr, tpa->tpa0.tpa$l_tokencnt);
    	*(symnam+tpa->tpa0.tpa$l_tokencnt) = 0;
    	s = Lookup_Symbol(symnam);
    	ifent->do_it = (s != 0) && (s->value != 0) && (s->value[0] != '\0');
    	ifent->in_else = 0;
    	return SS$_NORMAL;

    case PRS_K_DIR_IFLHS:
    	iflhs.dsc$w_length = tpa->tpa0.tpa$l_tokencnt;
    	iflhs.dsc$a_pointer = tpa->tpa_l_stringbase +
    	    	(tpa->tpa0.tpa$l_tokenptr-tpa->tpa_l_upbase);
    	iflhs.dsc$b_dtype = DSC$K_DTYPE_T;
    	iflhs.dsc$b_class = DSC$K_CLASS_S;
    	return SS$_NORMAL;

    case PRS_K_DIR_IFEQL:
    case PRS_K_DIR_IFNEQ:
    	iftype = tpa->tpa0.tpa$l_param;
    	return SS$_NORMAL;

    case PRS_K_DIR_IFRHS:
    	ifrhs.dsc$w_length = tpa->tpa0.tpa$l_tokencnt;
    	ifrhs.dsc$a_pointer = tpa->tpa_l_stringbase +
    	    	(tpa->tpa0.tpa$l_tokenptr-tpa->tpa_l_upbase);
    	ifrhs.dsc$b_dtype = DSC$K_DTYPE_T;
    	ifrhs.dsc$b_class = DSC$K_CLASS_S;
    	ifent = malloc(sizeof(struct IF));
    	queue_insert(ifent, &ifque);
    	i = str$case_blind_compare(&iflhs, &ifrhs);
    	ifent->do_it = (iftype == PRS_K_DIR_IFEQL) ? !i : i;
    	ifent->in_else = 0;
    	return SS$_NORMAL;

    case PRS_K_DIR_ELSE:
    	ifent = ifque.flink;
    	if (ifent == &ifque) return MMK__ELSENOIF;
    	if (ifent->in_else) return MMK__ELSENOIF;
    	ifent->do_it = !ifent->do_it;
    	ifent->in_else = 1;
    	return SS$_NORMAL;

    case PRS_K_DIR_ENDIF:
    	if (!queue_remove(ifque.flink, &ifent)) return MMK__ENDIFNOIF;
    	free(ifent);
    	return SS$_NORMAL;

    default:
    	break;
    }

    switch (tpa->tpa0.tpa$l_param) {

    case PRS_K_CMD_INIT:
    	current_cmd = mem_get_cmd();
    	break;

    case PRS_K_SYM_INIT:
    	current_sym = mem_get_symbol();
    	Resolve_Symbols(tpa->tpa0.tpa$l_tokenptr, tpa->tpa0.tpa$l_tokencnt,
    	    	    	    &cp, &len, 0);
    	if (len >= sizeof(current_sym->name)) len = sizeof(current_sym->name)-1;
    	strncpy(current_sym->name, cp, len);
    	*(current_sym->name+len) = '\0';
    	free(cp);
    	current_sym->value = (char *) 0;
    	break;

    case PRS_K_DEP_INIT:
    	trg_str = malloc(64);
    	trg_str_len = 0;
    	trg_str_size = 64;
    	double_colon = 0;
    	break;

    case PRS_K_DIR_SFX:
    case PRS_K_DIR_FIRST:
    case PRS_K_DIR_LAST:
    	current_dirtype = tpa->tpa0.tpa$l_param;
    	break;

    case PRS_K_DIR_RHS:
    	switch (current_dirtype) {

    	case PRS_K_DIR_SFX:
    	    Resolve_Symbols((tpa->tpa_l_stringbase+
    	    	(tpa->tpa0.tpa$l_stringptr-tpa->tpa_l_upbase)),
    	    	tpa->tpa0.tpa$l_stringcnt, &cp, &len, 0);
    	    while (len > 0 && isspace(cp[len-1])) len--;
    	    Build_Suffix_List(cp, len);
    	    free(cp);
    	    current_dirtype = 0;
    	    break;
    	case PRS_K_DIR_FIRST:
    	    current_cmdque = (struct QUE *) &do_first;
    	    current_dirtype = 0;
    	    current_deplist = 0;
    	    break;
    	case PRS_K_DIR_LAST:
    	    current_cmdque = (struct QUE *) &do_last;
    	    current_dirtype = 0;
    	    current_deplist = 0;
    	    break;
    	}
    	just_did_rule = 0;
    	break;

    case PRS_K_DIR_SILENT:
    	if (!override_silent) verify = 0;
    	just_did_rule = 0;
    	break;

    case PRS_K_DIR_IGNORE:
    	if (!override_ignore) ignore = 3;
    	just_did_rule = 0;
    	break;

    case PRS_K_DIR_DEFAULT:
    	default_rule = mem_get_rule();
    	current_cmdque = (struct QUE *) &default_rule->cmdque;
    	current_deplist = 0;
    	just_did_rule = 0;
    	break;

    case PRS_K_DIR_INCLUDE:
    	i = tpa->tpa0.tpa$l_stringcnt;
    	cp = tpa->tpa0.tpa$l_stringptr;
    	while (i > 0 && isspace(*cp)) {
    	    cp++; i--;
    	}
    	if (i > 0) {
    	    while (i > 1 && isspace(*(cp+(i-1)))) i--;
    	}
    	if (i > 0) {
    	    char tmp[256];
    	    status = file_open(cp, tpa->tpa_l_unit, "SYS$DISK:[].MMS",
    	    	    	    tmp, tpa->tpa_l_maxlen);
    	    if (OK(status)) {
    	    	if (do_log) lib$signal(MMK__OPENINCL, 1, tmp);
    	    } else {
    	    	lib$signal(MMK__NOOPNINCL, 1, cp, status);
    	    	*(tpa->tpa_l_unit) = 0;
    	    }
    	}
    	just_did_rule = 0;
    	break;

    case PRS_K_RULE_INIT:
    case PRS_K_RULE_INIPFX:
    	current_rule = mem_get_rule();
    	current_deplist = 0;
    	sfx_ptr = current_rule->src;
    	*sfx_ptr++ = '.';
    	*sfx_ptr = '\0';
    	current_cmdque = (struct QUE *) &current_rule->cmdque;
    	if (tpa->tpa0.tpa$l_param == PRS_K_RULE_INIPFX) {
    	    current_rule->srcpfxlen = tpa->tpa0.tpa$l_tokencnt-2;
    	    if (current_rule->srcpfxlen > sizeof(current_rule->srcpfx)-1)
    	    	current_rule->srcpfxlen = sizeof(current_rule->srcpfx)-1;
    	    memcpy(current_rule->srcpfx, tpa->tpa0.tpa$l_tokenptr+1,
    	    	    	current_rule->srcpfxlen);
    	    current_rule->srcpfx[current_rule->srcpfxlen] = '\0';
    	}
    	break;

    case PRS_K_RULE_SFX:
    	*sfx_ptr++ = tpa->tpa0.tpa$b_char;
    	*sfx_ptr   = '\0';
    	break;

    case PRS_K_RULE_NEWSFX:
    	sfx_ptr = current_rule->trg;
    	*sfx_ptr++ = '.';
    	*sfx_ptr = '\0';
    	break;

    case PRS_K_RULE_NEWPFX:
    	current_rule->trgpfxlen = tpa->tpa0.tpa$l_tokencnt-2;
    	if (current_rule->trgpfxlen > sizeof(current_rule->trgpfx)-1)
    	    current_rule->trgpfxlen = sizeof(current_rule->trgpfx)-1;
    	memcpy(current_rule->trgpfx, tpa->tpa0.tpa$l_tokenptr+1,
    	    	    	current_rule->trgpfxlen);
    	current_rule->trgpfx[current_rule->trgpfxlen] = '\0';
    	break;


    case PRS_K_RULE_END:
    	if (current_rule->trg[0] == '\0' ||
    	    current_rule->src[0] == '\0') return LIB$_SYNTAXERR;
    	xr = find_rule(current_rule->trg, current_rule->src);
    	if (xr) {
    	    struct RULE *lastr;
    	    lastr = 0;
    	    for (r = xr; r != 0; r = r->next) {
    	    	if (r->srcpfxlen == current_rule->srcpfxlen &&
    	    	    	r->trgpfxlen == current_rule->trgpfxlen &&
    	    	    	strneql_case_blind(r->srcpfx, current_rule->srcpfx,
    	    	    	    	r->srcpfxlen) &&
    	    	    	strneql_case_blind(r->trgpfx, current_rule->trgpfx,
    	    	    	    	r->trgpfxlen)) {
    	    	    break;
    	    	}
    	    	lastr = r;
    	    }
    	    if (r == 0) {
    	    	lastr->next = current_rule;
    	    	current_rule->next = 0;
    	    } else {
    	    	current_rule->next = r->next;
    	    	if (r == xr) {
    	    	    queue_insert(current_rule, r);
    	    	    queue_remove(r, &r);
    	    	} else {
    	    	    lastr->next = current_rule;
    	    	}
    	    }
    	} else {
    	    queue_insert(current_rule, rules.blink);
    	}
    	just_did_rule = 1;
    	break;

    case PRS_K_CMD_NOECHO:
    	current_cmd->flags |= CMD_M_NOECHO;
    	break;

    case PRS_K_CMD_IGNERR:
    	current_cmd->flags |= CMD_M_IGNERR;
    	break;

    case PRS_K_CMD_BOTHNI:
    	current_cmd->flags |= CMD_M_IGNERR|CMD_M_NOECHO;
    	break;

    case PRS_K_CMD_TEXT:
    	if (current_cmdque == 0) return MMK__ACTNODEPRULE;
    	current_cmd->cmd = malloc(tpa->tpa0.tpa$l_stringcnt+1);
    	strncpy(current_cmd->cmd, (tpa->tpa_l_stringbase+
    	    	(tpa->tpa0.tpa$l_stringptr-tpa->tpa_l_upbase)),
    	    	tpa->tpa0.tpa$l_stringcnt);
    	*(current_cmd->cmd+tpa->tpa0.tpa$l_stringcnt) = '\0';
    	queue_insert(current_cmd, current_cmdque->tail);
    	current_cmd = (struct CMD *) 0;
/*
**  If we just did a dependency line, set up the pointers to the command
**  queue for each dependency processed, and make sure we don't have
**  multiple sets of action lines for multiple single-colon dependencies
**  for one target.
*/
    	while (current_deplist != 0) {
    	    dep = current_deplist;
    	    if (dep->cmdqptr != 0 && !dep->double_colon) {
    	    	char target[256];
    	    	i = make_object_name(target, dep->target);
    	    	lib$signal(MMK__MULACTION, 2, i, target);
    	    	return MMK__MULACTION;
    	    }
    	    dep->cmdqptr = (struct CMD *) current_cmdque;
    	    current_deplist = dep->deplist_flink;
    	}
    	break;

    case PRS_K_SYM2DEP:
    	trg_str_size = 64;
	trg_str = malloc(64);
	strcpy(trg_str, current_sym->name);
	trg_str_len = strlen(current_sym->name);
	mem_free_symbol(current_sym);
    	double_colon = 0;
    	if (tpa->tpa0.tpa$b_char != ',') {
    	    *(trg_str+trg_str_len) = tpa->tpa0.tpa$b_char;
    	    trg_str_len++;
    	}
    	current_sym = (struct SYMBOL *) 0;
	break;

    case PRS_K_SYM2DEP2:
    	trg_str_size = 64;
	trg_str = malloc(64);
	strcpy(trg_str, current_sym->name);
	trg_str_len = strlen(current_sym->name);
    	double_colon = 0;
	mem_free_symbol(current_sym);
    	current_sym = (struct SYMBOL *) 0;
	break;

    case PRS_K_SYM_VALUE:
    	Resolve_Symbols((tpa->tpa_l_stringbase+
    	    	(tpa->tpa0.tpa$l_stringptr-tpa->tpa_l_upbase)),
    	    	tpa->tpa0.tpa$l_stringcnt, &cp, &len, 2);
/*
**  Change deferred symbol references, ${xxx}, into normal symbol
**  references, $(xxx).
*/
    	cp1 = cp;
    	i = len;
    	while (i > 1) {
    	    if (*cp1 == '$' && *(cp1+1) == '{') {
    	    	int j;
    	    	char *cp2;
    	    	for (j = i-2, cp2 = cp1+2; j > 0; cp2++, j--) {
    	    	    if (*cp2 == '}') {
    	    	    	*(cp1+1) = '(';
    	    	    	*cp2 = ')';
    	    	    	i = 1;
    	    	    	break;
    	    	    }
    	    	}
    	    }
    	    cp1++; i--;
    	}
    	Define_Symbol(MMK_K_SYM_DESCRIP, current_sym->name, cp, len);
    	free(current_sym);
    	current_sym = (struct SYMBOL *) 0;
    	just_did_rule = 0;
    	break;

    case PRS_K_DEP_TRGAPP:
    	if (trg_str_len == trg_str_size) {
    	    trg_str_size += 64;
    	    trg_str = realloc(trg_str, trg_str_size);
    	}
    	*(trg_str+trg_str_len) = tpa->tpa0.tpa$b_char;
    	trg_str_len++;
    	break;

    case PRS_K_DEP_TRGAPP2:
    	if (trg_str_len > trg_str_size-2) {
    	    trg_str_size += 64;
    	    trg_str = realloc(trg_str, trg_str_size);
    	}
    	*(trg_str+trg_str_len) = ',';
    	*(trg_str+trg_str_len+1) = tpa->tpa0.tpa$b_char;
    	trg_str_len += 2;
    	break;

    case PRS_K_DEP_DC:
    	double_colon = 1;
    	break;

    case PRS_K_DEP_RHS: {
    	struct OBJREF *o;
    	struct OBJECT *obj2;

    	src_str = malloc(tpa->tpa0.tpa$l_stringcnt+1);
    	strncpy(src_str, (tpa->tpa_l_stringbase+
    	    	(tpa->tpa0.tpa$l_stringptr-tpa->tpa_l_upbase)),
    	    	tpa->tpa0.tpa$l_stringcnt);
    	*(src_str+tpa->tpa0.tpa$l_stringcnt) = '\0';
    	trgque.head = trgque.tail = &trgque;
    	srcque.head = srcque.tail = &srcque;
    	refque.head = refque.tail = &refque;
    	Resolve_Symbols(trg_str, trg_str_len, &cp, &len, 0);
    	Parse_Objects(cp, len, &trgque, 1);
    	free(cp);
    	Resolve_Symbols(src_str, strlen(src_str), &cp, &len, 0);
    	Parse_Objects(cp, len, &srcque, 0);
    	free(cp);
    	make_objrefs(&refque, &srcque);
    	current_cmdque = (struct QUE *) mem_get_cmd();
    	current_cmdque->head = current_cmdque->tail = current_cmdque;
    	current_deplist = 0;
    	while (queue_remove(trgque.head, &obj)) {
    	    if ((obj2 = Find_Object(obj)) == NULL) {
    	    	Insert_Object(obj);
    	    } else {
    	    	mem_free_object(obj);
    	    	obj = obj2;
    	    }
    	    dep = find_dependency(obj, 0);
    	    if (dep == 0) {
    	    	dep = mem_get_depend();
    	    	dep->target = obj;
    	    	dep->double_colon = double_colon;
    	    	queue_insert(dep, dependencies.blink);
    	    } else {
    	    	if ((double_colon && !dep->double_colon) ||
    	    	    	(!double_colon && dep->double_colon)) {
    	    	    lib$signal(MMK__SDCMIX, 1, obj->name);
    	    	    return MMK__SDCMIX;
    	    	}
    	    	if (double_colon) {
    	    	    struct DEPEND *dep2, *dep3;
    	    	    dep2 = mem_get_depend();
    	    	    dep2->target = obj;
    	    	    dep2->double_colon = 1;
    	    	    while (dep->dc_flink != 0) dep = dep->dc_flink;
    	    	    dep->dc_flink = dep2;
    	    	    dep = dep2;
    	    	}
    	    }
    	    copy_objrefs(&dep->sources, &refque);
    	    dep->deplist_flink = current_deplist;
    	    current_deplist = dep;
    	}
    	while (queue_remove(refque.head, &o)) mem_free_objref(o);
    	free(trg_str);
    	trg_str_len = trg_str_size = 0;
    	free(src_str);
    	just_did_rule = 0;
    	break;
    }

    default:
    	lib$signal(MMK__PRSTBLERR, 0);

    }
    return SS$_NORMAL;

} /* parse_store */

/*
**++
**  ROUTINE:	make_objrefs
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Duplicates a queue of objects to another queue.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	make_objrefs(struct QUE *destq, struct QUE *srcq);
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
static void make_objrefs(struct QUE *destq, struct QUE *srcq) {

    struct OBJECT *obj, *obj2;
    struct OBJREF *tmp;

    while (queue_remove(srcq->head, &obj)) {
    	obj2 = Find_Object(obj);
    	if (obj2 == NULL) {
    	    Insert_Object(obj);
    	} else {
    	    mem_free_object(obj);
    	    obj = obj2;
    	}
    	tmp = mem_get_objref();
    	tmp->obj = obj;
    	queue_insert(tmp, destq->tail);
    }

} /* make_objrefs */

/*
**++
**  ROUTINE:	copy_objrefs
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Duplicates a queue of object references to another queue.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	copy_objrefs(struct QUE *destq, struct QUE *srcq);
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
static void copy_objrefs(struct OBJREF *destq, struct QUE *srcq) {

    struct OBJREF *tmp, *obj, *o;

    for (obj = srcq->head; obj != (struct OBJREF *) srcq; obj = obj->flink) {
    	for (o = destq->flink; o != destq; o = o->flink)
    	    if (o->obj == obj->obj) break;
    	if (o == destq) {
    	    tmp = mem_get_objref();
    	    tmp->obj = obj->obj;
    	    queue_insert(tmp, destq->blink);
    	}
    }

} /* copy_objrefs */
