/*
**++
**  FACILITY:	MMK
**
**  ABSTRACT:	Symbol substitution routines
**
**  MODULE DESCRIPTION:
**
**  	This module contains routines that deal with the
**  MMS global and local symbol lists.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT � 1992, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  21-AUG-1992
**
**  MODIFICATION HISTORY:
**
**  	21-AUG-1992 V1.0    Madison 	Initial coding.
**  	01-SEP-1992 V1.1    Madison 	Comments.
**  	29-SEP-1992 V1.2    Madison 	Separate out command-line symbol defs.
**  	14-OCT-1992 V1.2-1  Madison 	Support use of DCL symbols.
**  	02-JUN-1993 V1.3    Madison 	Support use of $*, $<, etc.
**  	07-JUN-1993 V1.4    Madison 	Add MMS$CHANGED_LIST.
**  	16-SEP-1993 V1.4-1  Madison 	Fix make symbol substitution list.
**  	17-OCT-1993 V1.5    Madison 	Allow $(var:subst-rule) notation.
**  	17-OCT-1993 V1.6    Madison 	Symbol setup for libfile deletion.
**  	28-OCT-1993 V1.6-1  Madison 	Fix redefinition of cmdline symbols.
**  	02-DEC-1993 V1.6-2  Madison 	Allow symbol refs inside symbol refs.
**  	12-DEC-1993 V1.7    Madison 	Support for $(MMS) macro.
**  	02-MAR-1994 V1.7-1  Madison 	Fix non-resolvable specials.
**  	14-APR-1994 V1.7-2  Madison 	Fix MMS$CHANGED_LIST creation.
**  	01-JUL-1994 V1.8    Madison 	CMS support.
**  	22-JUL-1994 V1.9    Madison 	Add MMS$TARGET_FNAME.
**  	17-AUG-1994 V1.9-1  Madison 	Fix Define_Symbol for "MMS" override.
**  	22-AUG-1994 V1.9-2  Madison 	Remove KILL_INTERMEDIATES stuff.
**  	02-DEC-1994 V2.0    Madison 	Add default CMS generation stuff.
**  	21-JUN-1995 V2.1    Madison 	Revamp for MMS parity.
**  	03-OCT-1995 V2.1-1  Madison 	Fix handling of nested symbol refs.
**--
*/
#ifdef __DECC
#pragma module SYMBOLS "V2.1-1"
#else
#ifndef __GNUC__
#module SYMBOLS "V2.1-1"
#endif
#endif
#include "mmk.h"
#include "globals.h"
#include <string.h>
#include <ctype.h>

/*
** Forward declarations
*/
    struct SYMBOL *Lookup_Symbol(char *);
    void Define_Symbol(SYMTYPE, char *, char *, int);
    int Resolve_Symbols(char *, int, char **, int *, int);
    void Clear_Local_Symbols(void);
    void Create_Local_Symbols(struct DEPEND *, struct OBJREF *, struct QUE *);
    static void apply_subst_rule(char *, char *, char **, int *);

#define min(a,b) (((a) < (b)) ? (a) : (b))

    static struct SYMTABLE dcl_symbols;
    static int dcl_symbols_inited = 0;

/*
** External references
*/

    extern void upcase(char *);
    extern char *find_char(char *, char *, char);
    extern void mem_free_symbol(struct SYMBOL *);
    extern struct SYMBOL *mem_get_symbol(void);
    extern int extract_name(char *, char *);
    extern int extract_filename(char *, char *);
    extern int make_object_name(char *, struct OBJECT *);
    extern int strneql_case_blind(char *, char *, int);
    extern struct RULE *find_rule(char *, char *);
    extern unsigned int cms_parse_name(char *, char *, int, int *,
    	    	char *, int, int *, int);

/*
**++
**  ROUTINE:	Lookup_Symbol
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Locates a symbol by name.
**
**  RETURNS:	pointer to struct SYMBOL
**
**  PROTOTYPE:
**
**  	Lookup_Symbol(char *name)
**
**  IMPLICIT INPUTS:	all symbol tables
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	non-0: symbol found
**  	    0: symbol not found
**
**  SIDE EFFECTS:   	None.
**
**--
*/
struct SYMBOL *Lookup_Symbol(char *name) {

    struct SYMBOL *sym;
    struct QUE    *symq;
    unsigned char *cp;
    struct dsc$descriptor namdsc, valdsc;
    unsigned int status, hash_value;
    int i;
    static struct SYMTABLE *normal_order[] = {
    	&local_symbols, &cmdline_symbols, &global_symbols,
    	&builtin_symbols, &dcl_symbols};
    static struct SYMTABLE *override_order[] = {
    	&local_symbols, &cmdline_symbols, &dcl_symbols,
    	&global_symbols, &builtin_symbols};

    if (!dcl_symbols_inited) {
    	for (i = 0; i < MMK_K_SYMTABLE_SIZE; i++) {
    	    INIT_QUEUE(dcl_symbols.symlist[i]);
    	}
    	dcl_symbols_inited = 1;
    }

    hash_value = 0;
    for (cp = (unsigned char *) name, i = 0; *cp != '\0' && i < 4; cp++, i++) {
    	hash_value |= *cp;
    }
    hash_value &= 0xff;

    for (i = 0; i < sizeof(normal_order)/sizeof(normal_order[0]); i++) {
    	symq = symbol_override ? &override_order[i]->symlist[hash_value]
    	    	    	       : &normal_order[i]->symlist[hash_value];
    	for (sym = symq->head; sym != (struct SYMBOL *) symq; sym = sym->flink) {
    	    if (strcmp(name, sym->name) == 0) return sym;
    	}
    	if ((symbol_override && override_order[i] == &dcl_symbols) ||
    	    	    (!symbol_override && normal_order[i] == &dcl_symbols)) {
    	    INIT_SDESC(namdsc, strlen(name), name);
    	    INIT_DYNDESC(valdsc);
    	    status = lib$get_symbol(&namdsc, &valdsc);
    	    if (OK(status)) {
    	    	sym = mem_get_symbol();
    	    	strcpy(sym->name, name);
    	    	sym->value = malloc(valdsc.dsc$w_length+1);
    	    	memcpy(sym->value, valdsc.dsc$a_pointer, valdsc.dsc$w_length);
    	    	sym->value[valdsc.dsc$w_length] = '\0';
    	    	queue_insert(sym, dcl_symbols.symlist[hash_value].tail);
    	    	str$free1_dx(&valdsc);
    	    	return sym;
    	    }
    	}
    }

    return (struct SYMBOL *) 0;

} /* Lookup_Symbol */

/*
**++
**  ROUTINE:	Define_Symbol
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Creates or re-defines a symbol in the global symbol
**  	table.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Define_Symbol(SYMTYPE symtype, char *name, char *val, int vallen)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	global_symbols
**
**--
*/
void Define_Symbol(SYMTYPE symtype, char *name, char *val, int vallen) {

    struct SYMBOL *sym;
    struct QUE    *symq;
    char upname[MMK_S_SYMBOL+1];
    unsigned char *cp;
    unsigned int hash_value;
    int i;

    strcpy(upname, name);
    upcase(upname);

    hash_value = 0;
    for (cp = (unsigned char *) upname, i = 0; *cp != '\0' && i < 4; cp++, i++) {
    	hash_value |= *cp;
    }
    hash_value &= 0xff;

    switch (symtype) {

    case MMK_K_SYM_LOCAL:
    	symq = &local_symbols.symlist[hash_value];
    	break;
    case MMK_K_SYM_DESCRIP:
    	symq = &global_symbols.symlist[hash_value];
    	break;
    case MMK_K_SYM_CMDLINE:
    	symq = &cmdline_symbols.symlist[hash_value];
    	break;
    case MMK_K_SYM_BUILTIN:
    	symq = &builtin_symbols.symlist[hash_value];
    	break;
    default:
    	symq = 0;   /* this will cause an ACCVIO - should never happen */
    	break;
    }

    for (sym = symq->head; sym != (struct SYMBOL *) symq; sym = sym->flink) {
    	if (strcmp(upname, sym->name) == 0) break;
    }
    if (sym == (struct SYMBOL *) symq) {
    	sym = malloc(sizeof(struct SYMBOL));
    	strcpy(sym->name, upname);
    	queue_insert(sym, symq->tail);
    } else {
    	free(sym->value);
    }

    if (vallen < 0) vallen = strlen(val);
    sym->value = malloc(vallen+1);
    memcpy(sym->value, val, vallen);
    *(sym->value+vallen) = '\0';

} /* Define_Symbol */

/*
**++
**  ROUTINE:	Resolve_Symbols
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Performs symbol substitution in a string.  Iterates until no
**  further substitutions are performed.
**
**  Symbol references appear as $(symbol-name) in an MMS description line.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Resolve_Symbols(char *in, int inlen, char **out, int *outlen,
**  	    	    	    int dont_resolve_unknowns)
**
**  The output string is allocated by this procedure using malloc
**  and should be freed by the caller when done.
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
int Resolve_Symbols(char *in, int inlen, char **out, int *outlen,
    	    	    	int dont_resolve_unknowns) {

    char *cp, *inend, *dp, *pp, *tmp, *val, *colp;
    struct SYMBOL *valsym;
    char var[MMK_S_SYMBOL+1];
    int len, curlen, tmplen, first, did_one, free_val, i;
    int resolved_MMS_macro;

    static char SPECIALS[] = "@*<+?%&";
    static char *SPECIAL_VAR[] = {"MMS$TARGET","MMS$TARGET_NAME",
    	    	    	    	  "MMS$SOURCE","MMS$SOURCE_LIST",
    	    	    	    	  "MMS$CHANGED_LIST","MMS$LIB_ELEMENT",
    	    	    	    	  "MMS$CMS_GEN"};

    static char *non_resolvables[] = {"MMS","MMSQUALIFIERS",
    	"MMS$TARGET","MMS$TARGET_NAME","MMS$TARGET_MODULE","MMS$LIB_ELEMENT",
    	"MMS$SOURCE","MMS$SOURCE_LIST","MMS$CHANGED_LIST",
    	"MMS$SOURCE_NAME","MMS$SOURCE_LIST_SPACES","MMS$CHANGED_LIST_SPACES",
    	"MMS$CMS_LIBRARY", "MMS$CMS_ELEMENT", "MMS$CMS_GEN",
    	"MMS$TARGET_FNAME"};

    first = 1;
    resolved_MMS_macro = 0;

    do {

    	did_one = 0;
    	tmp = malloc(tmplen = inlen);
    	cp = in;
    	inend = in+inlen;
    	curlen = 0;

    	while (cp < inend) {

    	    colp = 0;
    	    free_val = 0;
/*
** Look for the beginning of $(...)
*/
    	    dp = find_char(cp, inend, '$');
    	    if (dp == (char *) 0) {
    	    	len = inend-cp;
    	    } else {
    	    	len = dp-cp;
    	    	if ((dp == inend-1) || ((strchr(SPECIALS, *(dp+1)) == 0)
    	    	    	    	    	    && (*(dp+1) != '('))) {
    	    	    len++;
    	    	    dp = (char *) 0;
    	    	} else  if (dp > inend-3 && *(dp+1) == '(') {
    	    	    len++;
    	    	    dp = (char *) 0;
    	    	}
    	    }

/*
** Copy everything up to the "$(" into the output string, expanding its
** size if necessary.
*/
    	    if (curlen+len > tmplen) {
    	    	tmplen = curlen+len+128;
    	    	tmp = realloc(tmp, tmplen);
    	    }
    	    memcpy(tmp+curlen, cp, len);
    	    curlen += len;

/*
** If we found "$(", locate the closing ")" and extract the symbol name,
** then look it up and stuff the resulting value into the output string.
*/
    	    if (dp == (char *) 0) {
    	    	cp += len;
    	    } else {
    	    	int is_special = 0;
    	    	int pct;
    	    	dp++;
    	    	if (*dp == '(') {
    	    	    dp++;
    	    	    pp = find_char(dp, inend, ')');
/*
**  Check for embedded symbol references, then
**  check for $(var:<sfx>=<sfx>)
*/
    	    	    if (pp != (char *) 0) {
    	    	    	colp = find_char(dp, pp, '$');
    	    	    	if (colp != 0) if (colp < pp-1 && (*(colp+1) == '('
    	    	    	    	|| strchr(SPECIALS, *(colp+1)) != 0)) {
    	    	    	    tmp[curlen++] = '$';
    	    	    	    cp += len + 1;
    	    	    	    dp = colp = pp = 0;
    	    	    	    continue;
    	    	    	}
    	    	    	colp = find_char(dp, pp, ':');
    	    	    	if (colp != 0) {
    	    	    	    char *cp;
    	    	    	    for (cp = colp; isspace(*(cp-1)); cp--);
    	    	    	    len = min(cp-dp, MMK_S_SYMBOL);
    	    	    	} else {
    	    	    	    len = min(pp-dp,MMK_S_SYMBOL);
    	    	    	}
    	    	    	strncpy(var, dp, len);
    	    	    	var[len] = '\0';
    	    	    	upcase(var);
    	    	    }
    	    	} else {
    	    	    pp = strchr(SPECIALS, *dp);
    	    	    if (pp != 0) {
    	    	    	strcpy(var, SPECIAL_VAR[pp-SPECIALS]);
    	    	    	pp = dp;
    	    	    	is_special = 1;
    	    	    }
    	    	}
    	    	if (pp != 0) {
    	    	    valsym = Lookup_Symbol(var);
    	    	    if (valsym != (struct SYMBOL *) 0) {
    	    	    	did_one = 1;
    	    	    	if (strcmp(valsym->name, "MMS") == 0) resolved_MMS_macro = 1;
    	    	    	if (colp != 0) {
    	    	    	    apply_subst_rule(valsym->value, colp+1, &val, &len);
    	    	    	    free_val = 1;
    	    	    	} else {
    	    	    	    val = valsym->value;
    	    	    	    len = strlen(val);
    	    	    	}
    	    	    	cp = pp + 1;
    	    	    } else {
/*
** If dont_resolve_unknowns is set and we didn't find the symbol in the
** symbol table, just copy the symbol reference into the output string.
** Otherwise, the symbol reference resolves to a null string.
**
** When dont_resolve_unknowns is set to 2, we resolve unknowns unless they
** are on the special "non_resolvables" list.
*/
    	    	    	if (dont_resolve_unknowns == 1) {
    	    	    	    len = 1;
    	    	    	    val = is_special ? dp-1 : dp-2;
    	    	    	    cp = is_special ? dp : dp-1;
    	    	    	} else {
    	    	    	    if (dont_resolve_unknowns == 2) {
    	    	    	    	for (i = 0; i < sizeof(non_resolvables)/
    	    	    	    	    	    	sizeof(non_resolvables[0]); i++) {
    	    	    	    	    if (strcmp(var, non_resolvables[i]) == 0) break;
    	    	    	    	}
    	    	    	    	if (i < sizeof(non_resolvables)/
    	    	    	    	    	    	sizeof(non_resolvables[0])) {
    	    	    	    	    len = 1;
    	    	    	    	    val = is_special ? dp-1 : dp-2;
    	    	    	    	    cp = is_special ? dp : dp-1;
    	    	    	    	} else {
    	    	    	    	    len = 0;
    	    	    	    	    val = dp;
    	    	    	    	    cp = pp + 1;
    	    	    	    	}
    	    	    	    } else {
    	    	    	    	len = 0;
    	    	    	    	val = dp;
    	    	    	    	cp = pp + 1;
    	    	    	    }
    	    	    	}
    	    	    }
    	    	} else {
    	    	    len = 1;
    	    	    val = dp-2;
    	    	    cp = dp-1;
    	    	}
    	    	if (curlen+len > tmplen) {
    	    	    tmplen = curlen+len+128;
    	    	    tmp = realloc(tmp, tmplen);
    	    	}
    	    	memcpy(tmp+curlen, val, len);
    	    	curlen += len;
    	    	if (free_val) free(val);
    	    }
    	}

    	if (!first) free(in);
    	first = 0;

    	if (did_one) {
    	    in = tmp;
    	    inlen = curlen;
    	}

    } while (did_one);

    *out = tmp;
    *outlen = curlen;

    return resolved_MMS_macro;

} /* Resolve_Symbols */

/*
**++
**  ROUTINE:	Clear_Local_Symbols
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Deletes all of the symbols in the local symbol table.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Clear_Local_Symbols()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	local_symbols
**
**--
*/
void Clear_Local_Symbols() {

    struct SYMBOL *sym;
    int i;

    for (i = 0; i < MMK_K_SYMTABLE_SIZE; i++) {
    	while (queue_remove(local_symbols.symlist[i].head, &sym)) {
    	    mem_free_symbol(sym);
    	}
    }

} /* Clear_Local_Symbols */

/*
**++
**  ROUTINE:	Create_Local_Symbols
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Given a dependency rule, the local symbols for commands
**  invoked under that dependency are defined.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Create_Local_Symbols(struct DEPEND *dependency_rule)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	local_symbols
**
**--
*/
void Create_Local_Symbols(struct DEPEND *dep, struct OBJREF *srcref, struct QUE *chgque) {

    struct OBJREF *obj;
    struct RULE *r;
    char nam[256], *src, *cp;
    int srclen, srcsize, i;

    extract_name(nam, dep->target->name);
    Define_Symbol(MMK_K_SYM_LOCAL, "MMS$TARGET_NAME", nam, -1);
    extract_filename(nam, dep->target->name);
    Define_Symbol(MMK_K_SYM_LOCAL, "MMS$TARGET_FNAME", nam, -1);

    if (dep->target->type == MMK_K_OBJ_LIBMOD) {
    	Define_Symbol(MMK_K_SYM_LOCAL, "MMS$TARGET", dep->target->libfile->name, -1);
    	Define_Symbol(MMK_K_SYM_LOCAL, "MMS$TARGET_MODULE", dep->target->name, -1);
    	Define_Symbol(MMK_K_SYM_LOCAL, "MMS$LIB_ELEMENT", dep->target->name, -1);
    } else {
    	Define_Symbol(MMK_K_SYM_LOCAL, "MMS$TARGET", dep->target->name, -1);
    }
/*
**  If we didn't get an explicit source reference, try and find the "best"
**  one available.  That's the one where there's a rule for building the
**  target from one of the sources, or if there is no such rule, just the
**  first source in the list.
*/

    if (srcref == 0) {
    	for (obj = dep->sources.flink; obj != &dep->sources; obj = obj->flink) {
    	    if (obj->obj->type == MMK_K_OBJ_LIBMOD) {
    	    	r = find_rule(dep->target->sfx, obj->obj->libfile->sfx);
    	    } else {
    	    	r = find_rule(dep->target->sfx, obj->obj->sfx);
    	    }
    	    if (r) break;
    	}
    	if (obj == &dep->sources) obj = dep->sources.flink;
    } else obj = srcref;

/*
**  We still have an out if there were no sources in the list (in which case
**  we just don't define the sources macros.
*/
    	    
    if (obj != &dep->sources) {
    	Define_Symbol(MMK_K_SYM_LOCAL, "MMS$SOURCE", obj->obj->name, -1);
    	if (obj->obj->type == MMK_K_OBJ_CMSFILE) {
    	    char lib[256];
    	    unsigned int status;
    	    status = cms_parse_name(obj->obj->name, lib, sizeof(lib), 0,
    	    	    	    	nam, sizeof(nam), 0, 0);
    	    if (OK(status)) {
    	    	Define_Symbol(MMK_K_SYM_LOCAL, "MMS$CMS_ELEMENT", nam, -1);
    	    	if (lib[0] != '\0') Define_Symbol(MMK_K_SYM_LOCAL, "MMS$CMS_LIBRARY", lib, -1);
    	    }
    	    if (obj->obj->cms_gen[0] != '\0') {
    	    	Define_Symbol(MMK_K_SYM_LOCAL, "MMS$CMS_GEN", obj->obj->cms_gen, -1);
    	    } else if (cms_default_generation[0] != '\0') {
    	    	Define_Symbol(MMK_K_SYM_LOCAL, "MMS$CMS_GEN", cms_default_generation, -1);
    	    } else Define_Symbol(MMK_K_SYM_LOCAL, "MMS$CMS_GEN", "1+", 2);
    	}
    	extract_name(nam, obj->obj->name);
    	Define_Symbol(MMK_K_SYM_LOCAL, "MMS$SOURCE_NAME", nam, -1);
    	srcsize = strlen(obj->obj->name)+256;
    	src = malloc(srcsize);
    	strcpy(src, obj->obj->name);
    	srclen = strlen(obj->obj->name);
    	for (obj = obj->flink; obj != &dep->sources; obj = obj->flink) {
    	    i = make_object_name(nam, obj->obj);
    	    if (srclen + i > srcsize-2) {
    	    	srcsize += i + 256;
    	    	src = realloc(src, srcsize);
    	    }
    	    *(src+(srclen++)) = ',';
    	    strcpy(src+srclen, nam);
    	    srclen += strlen(nam);
    	}
    	Define_Symbol(MMK_K_SYM_LOCAL, "MMS$SOURCE_LIST", src, srclen);
    	for (cp = src; *cp; cp++) if (*cp == ',') *cp = ' ';
    	Define_Symbol(MMK_K_SYM_LOCAL, "MMS$SOURCE_LIST_SPACES", src, srclen);
    	free(src);
    }

/*
**  Now build the changed-source list
*/

    srcsize = 256;
    src = malloc(srcsize);
    srclen = 0;
    for (obj = chgque->head; obj != (struct OBJREF *) chgque; obj = obj->flink) {
    	i = make_object_name(nam, obj->obj);
    	if (srclen + i > srcsize-2) {
    	    srcsize += i + 256;
    	    src = realloc(src, srcsize);
    	}
    	if (srclen > 0) *(src+(srclen++)) = ',';
    	strcpy(src+srclen, nam);
    	srclen += strlen(nam);
    }
    src[srclen] = '\0';
    Define_Symbol(MMK_K_SYM_LOCAL, "MMS$CHANGED_LIST", src, srclen);
    for (cp = src; *cp; cp++) if (*cp == ',') *cp = ' ';
    Define_Symbol(MMK_K_SYM_LOCAL, "MMS$CHANGED_LIST_SPACES", src, srclen);
    free(src);


} /* Create_Local_Symbols */

/*
**++
**  ROUTINE:	apply_subst_rule
**
**  FUNCTIONAL DESCRIPTION:
**
**  	tbs
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
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
static void apply_subst_rule(char *orig, char *rule, char **xval, int *xlen) {

    char lhs[256], rhs[256];
    char *cp, *val, *cp1, *dp;
    int i, len, curlen, add_rhs, lhslen, rhslen;
    
    val = malloc(len = strlen(orig)+256);

    while (isspace(*rule)) rule++;

    cp1 = lhs;
    for (cp = rule; *cp != '\0' && *cp != '=' && *cp != ')' && !isspace(*cp); cp++) *cp1++ = *cp;
    while (isspace(*cp)) cp++;
    *cp1 = '\0';
    lhslen = cp1-lhs;
    cp1 = rhs;
    if (*cp == '=') {
    	for (cp++; isspace(*cp); cp++);
    	while (*cp != '\0' && *cp != ')' && !isspace(*cp)) *cp1++ = *cp++;
    }
    *cp1 = '\0';
    rhslen = cp1-rhs;

    curlen = 0;
    cp = orig;
    while (*cp != '\0') {
    	add_rhs = 0;
    	for (cp1 = cp; *cp1 != '\0' && *cp1 != ',' && !isspace(*cp1); cp1++);
    	dp = find_char(cp, cp1, '.');
    	if (dp == 0) {
    	    if (lhslen == 0) {
    	    	dp = cp1;
    	    	add_rhs = 1;
    	    }
    	} else if (cp1-dp == lhslen) {
    	    add_rhs = strneql_case_blind(dp, lhs, lhslen);
    	}
    	i = add_rhs ? rhslen + (dp-cp) : cp1-cp;
    	if ((curlen + i) >= len) {
    	    len = curlen + i + len + 256;
    	    val = realloc(val, len);
    	}
    	if (add_rhs) {
    	    memcpy(val+curlen, cp, dp-cp);
    	    curlen += (dp-cp);
    	    memcpy(val+curlen, rhs, rhslen);
    	    curlen += rhslen;
    	} else {
    	    memcpy(val+curlen, cp, cp1-cp);
    	    curlen += (cp1-cp);
    	}
    	while (isspace(*cp1)) cp1++;
    	if (*cp1 == '\0') break;
    	if (*cp1 == ',') {
    	    *(val+curlen) = ',';
    	    curlen++;
    	    for (cp = cp1+1; isspace(*cp); cp++);
    	} else {
    	    *(val+curlen) = ' ';
    	    curlen++;
    	    cp = cp1;
    	}
    }

    *xval = val;
    *xlen = curlen;

} /* apply_subst_rule */
