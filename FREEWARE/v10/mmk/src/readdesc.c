/*
**++
**  FACILITY:	MMK
**
**  ABSTRACT:	Description file reader
**
**  MODULE DESCRIPTION:
**
**  	This module contains the read_description routine and its
**  supporting routines.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1992-1995, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  20-AUG-1992
**
**  MODIFICATION HISTORY:
**
**  	20-AUG-1992 V1.0    Madison 	Initial coding.
**  	01-SEP-1992 V1.1    Madison 	Comments.
**  	02-APR-1993 V1.2    Madison 	Don't strip comments off commands.
**  	04-JUN-1993 V1.3    Madison 	Support for .INCLUDE files.
**  	22-OCT-1993 V1.3-1  Madison 	Trim trailing blanks off lines.
**  	12-DEC-1993 V1.4    Madison 	Add Fill_In_Missing_Sources.
**  	01-JUL-1994 V1.5    Madison 	Support for CMS.
**  	06-JUL-1994 V1.5-1  Madison 	Add filename to default filespecs.
**  	14-JUL-1994 V1.6    Madison 	Update for prefixed inferences.
**  	15-JUL-1994 V1.6-1  Madison 	Fix broken CMS fallback logic.
**  	29-DEC-1994 V1.6-2  Madison 	Allow for comment lines with leading blanks.
**  	12-JAN-1995 V1.6-3  Madison 	Defer target-libmod dependencies.
**--
*/
#ifdef __DECC
#pragma module READDESC "V1.6-3"
#else
#ifndef __GNUC__
#module READDESC "V1.6-3"
#endif
#endif
#include <ctype.h>
#include "mmk.h"
#include "globals.h"
#pragma nostandard
#include "mmk_msg.h"
#pragma standard
#include <rmsdef.h>

    struct IO {
    	struct IO *flink, *blink;
    	char *linebuf, *stripbuf;
    	unsigned int unit;
    	int maxlen, current_line;
    	char filespec[256];
    };

/*
** Forward declarations
*/
    void Read_Description(char *, char *, int);
    static void strip_comments(char *, char *);
    static void Process_Deferred_Dependencies(void);
    static void Fill_In_Missing_Sources(void);

/*
** External references
*/
    extern void parse_descrip(char *, int, unsigned int *, int *, int, char *);
    extern unsigned int file_open();
    extern unsigned int file_read();
    extern unsigned int file_close();
    extern unsigned int file_exists(char *, char *);
    extern unsigned int file_get_filespec();
    extern struct SFX *find_suffix(char *);
    extern struct RULE *find_rule(char *, char *);
    extern struct RULE *scan_rule_list(struct RULE *, char *, int);
    extern struct DEPEND *find_dependency(struct OBJECT *, int);
    extern int extract_filename(char *, char *);
    extern struct OBJECT *mem_get_object(void);
    extern void mem_free_object(struct OBJECT *);
    extern void mem_free_depend(struct DEPEND *);
    extern struct OBJECT *Find_Object(struct OBJECT *);
    extern void Insert_Object(struct OBJECT *);
    extern struct OBJREF *mem_get_objref(void);
    extern int prefix_match(char *, char *);
    extern unsigned int cms_fetch_file(char *);


/*
**++
**  ROUTINE:	Read_Description
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Reads in a description file and forms lines that will be
**  parsed by parse_descrip.  Handles continuation lines, strips
**  comments.
**
**  	Although rules files and description files use identical
**  syntax, the third argument is a flag to indicate that this is
**  a rules file, since error handling is different in that case.
**
**  RETURNS:	void (errors are signaled)
**
**  PROTOTYPE:
**
**  	Read_Description(char *fspec, char *defspec, int rules_file)
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
void Read_Description(char *fspec, char *defspec, int rules_file) {

    struct QUE ioque;
    struct IO *io;
    char resspec[256];
    char *buf, *xbuf;
    unsigned int status, unit, bufsize, len;
    int continuation, maxlen;

    while (1) {
    	if (*fspec == '\0') {
    	    status = file_open("DESCRIP",&unit,"SYS$DISK:[]DESCRIP.MMS",resspec, &maxlen);
    	    if (!OK(status)) {
    	    	status = file_open("MAKEFILE.",&unit,"SYS$DISK:[]MAKEFILE.",resspec, &maxlen);
    	    }
    	} else {
    	    status = file_open(fspec, &unit, defspec, resspec, &maxlen);
    	}
    	if (OK(status)) break;
    	if (rules_file || !use_cms) break;
/*
**  Description files can be fetched from CMS
*/
    	if (*fspec == '\0') {
    	    status = cms_fetch_file("DESCRIP.MMS");
    	    if (!OK(status)) status = cms_fetch_file("MAKEFILE.");
    	} else {
    	    status = cms_fetch_file(fspec);
    	}
    	if (!OK(status)) {
    	    status = RMS$_FNF;
    	    break;
    	}
    }

    if (!OK(status)) {
    	if (rules_file) {
    	    lib$signal(MMK__NOOPNRUL, 1, fspec, status);
    	    if (OK(exit_status)) exit_status = MMK__NOOPNRUL;
    	} else {
    	    lib$signal(MMK__NOOPNDSC, 1, fspec, status);
    	}
    	return;
    }

    if (do_log) {
    	lib$signal((rules_file ? MMK__OPENRULE : MMK__OPENDESC), 1, resspec);
    }

    ioque.head = ioque.tail = &ioque;
    io = malloc(sizeof(struct IO));
    io->unit = unit;
    io->maxlen = maxlen;
    io->linebuf = malloc(io->maxlen+1);
    io->stripbuf = malloc(io->maxlen+1);
    io->current_line = 0;
    strcpy(io->filespec, resspec);
    queue_insert(io, ioque.tail);
    buf = (char *) 0;
    bufsize = 0;
    unit = 0;

    while (queue_remove(ioque.head, &io)) {
    	while (OK(file_read(io->unit, io->linebuf, io->maxlen+1, &len))) {
    	    io->current_line++;
    	    while (len > 0 && isspace(io->linebuf[len-1])) len--;
    	    io->linebuf[len] = '\0';
	    if (continuation && isspace(*io->linebuf)) {
    	    	char *cp;
    	    	for (cp = io->linebuf; *cp; cp++) if (!isspace(*cp)) break;
    	    	strip_comments(io->stripbuf, cp-1);
    	    	xbuf = io->stripbuf;
    	    } else if (isspace(*io->linebuf)) {
    	    	char *cp;
    	    	for (cp = io->linebuf; *cp; cp++) if (!isspace(*cp)) break;
    	    	if (*cp == '!' || *cp == '#') *io->linebuf = '\0';
    	    	xbuf = cp - 1;
    	    } else {
    	    	strip_comments(io->stripbuf, io->linebuf);
    	    	xbuf = io->stripbuf;
    	    }
    	    if (xbuf[strlen(xbuf)-1] == '-' ||
		    xbuf[strlen(xbuf)-1] == '\\') {
    	    	continuation = 1;
    	    	xbuf[strlen(xbuf)-1] = '\0';
    	    } else continuation = 0;

    	    if (strlen(xbuf) > 0) {
    	    	if (bufsize > 0) {
    	    	    buf = realloc(buf, bufsize+strlen(xbuf));
    	    	    strcpy(buf+bufsize-1,xbuf);
    	    	    bufsize+=strlen(xbuf);
    	    	} else {
    	    	    bufsize = strlen(xbuf)+1;
    	    	    buf = malloc(bufsize);
    	    	    strcpy(buf, xbuf);
    	    	}
    	    }
    	    if (!continuation && bufsize > 1) {
    	    	parse_descrip(buf, bufsize-1, &unit, &maxlen, io->current_line, io->filespec);
    	    	free(buf);
    	    	bufsize = 0;
    	    	if (unit) {
    	    	    queue_insert(io, &ioque);
    	    	    io = malloc(sizeof(struct IO));
    	    	    io->unit = unit;
    	    	    io->maxlen = maxlen;
    	    	    io->linebuf = malloc(io->maxlen+1);
    	    	    io->stripbuf = malloc(io->maxlen+1);
    	    	    io->current_line = 0;
    	    	    file_get_filespec(unit, io->filespec, sizeof(io->filespec));
    	    	    unit = 0;
    	    	}
    	    }
    	}

    	if (bufsize > 0) {
    	    parse_descrip(buf, bufsize-1, &unit, &maxlen, io->current_line, io->filespec);
    	    free(buf);
    	    if (unit) {
    	    	struct IO *io;
    	    	io = malloc(sizeof(struct IO));
    	    	io->unit = unit;
    	    	io->maxlen = maxlen;
    	    	io->linebuf = malloc(io->maxlen+1);
    	    	io->stripbuf = malloc(io->maxlen+1);
    	    	io->current_line = 0;
    	    	file_get_filespec(unit, io->filespec, sizeof(io->filespec));
    	    	unit = 0;
    	    	queue_insert(io, &ioque);
    	    }
    	}

    	file_close(io->unit);
    	free(io->linebuf);
    	free(io->stripbuf);
    	free(io);
    }

    Process_Deferred_Dependencies();
    Fill_In_Missing_Sources();

}

/*
**++
**  ROUTINE:	strip_comments
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Strips comments from the end of a string.  A comment begins
**  with either an exclamation point (!) or a pound sign (#).  Quoted
**  strings are handled properly.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	strip_comments(char *dest, char *source)
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
void strip_comments(char *dest, char *source) {

    int quote;
    register char *cp=source, *cp1=dest;

    quote = 0;
    for (cp = source; *cp; cp++) {
    	if (quote) {
    	    if (*cp == '"') quote = !quote;
    	} else {
    	    if (*cp == '!' || *cp == '#') break;
    	}
    	*cp1++ = *cp;
    }
    while (isspace(*(cp1-1))) cp1--;
    *cp1 = 0;
}

/*
**++
**  ROUTINE:	Process_Deferred_Dependencies
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Looks for dependencies that we created for library modules
**  that are targets, and adds them to the main dependencies queue if
**  there aren't dependencies there already for them.  The deferred
**  dependencies list is created by Parse_Objects().
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Process_Deferred_Dependencies()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:   None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static void Process_Deferred_Dependencies () {

    struct DEPEND *dep;

    while (queue_remove(dep_deferred.flink, &dep)) {

    	if (find_dependency(dep->target, 0) == 0) queue_insert(dep, dependencies.blink);
    	else mem_free_depend(dep);

    }

} /* Process_Deferred_Dependencies */

/*
**++
**  ROUTINE:	Fill_In_Missing_Sources
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Add implied sources to dependency rules missing them.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Fill_In_Missing_Sources()
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
static void Fill_In_Missing_Sources(void) {

    struct DEPEND *dep;
    struct OBJECT *obj, *obj2;
    struct OBJREF *o;
    struct RULE *r, *xr;
    struct SFX *s;
    char trgnam[MMK_S_FILE];
    int need_extra_source, trgnamlen;

    for (dep = dependencies.flink; dep != &dependencies; dep = dep->flink) {
    	if (dep->target->type != MMK_K_OBJ_FILE) continue;
    	s = find_suffix(dep->target->sfx);
    	if (s) {
    	    need_extra_source = 1;
    	    for (o = dep->sources.flink; o != &dep->sources; o = o->flink) {
    	    	if (o->obj->type != MMK_K_OBJ_FILE) continue;
    	    	if (find_rule(dep->target->sfx, o->obj->sfx)) {
    	    	    need_extra_source = 0;
    	    	    break;
    	    	}
    	    }
    	    if (need_extra_source) {

    	    	for (; s != &suffixes; s = s->flink) {
    	    	    int check_cms, slen;
/*
**  Don't use the CMS suffixes if we're not using CMS
*/
    	    	    slen = strlen(s->value);
    	    	    check_cms = s->value[slen-1] == '~';
    	    	    if (check_cms && !use_cms) continue;

    	    	    xr = find_rule(dep->target->sfx, s->value);
    	    	    if (xr != 0) {
    	    	    	r = scan_rule_list(xr, dep->target->name, 0);
    	    	    	if (r != 0) {
    	    	    	    obj = mem_get_object();
    	    	    	    trgnamlen = extract_filename(trgnam, dep->target->name);
    	    	    	    memcpy(obj->name, r->srcpfx, r->srcpfxlen);
    	    	    	    memcpy(obj->name+r->srcpfxlen, trgnam, trgnamlen);
    	    	    	    strcpy(obj->name+(r->srcpfxlen+trgnamlen), s->value);
    	    	    	    strcpy(obj->sfx, s->value);
    	    	    	    obj->type = MMK_K_OBJ_FILE;
    	    	    	    if ((obj2 = Find_Object(obj)) == NULL) {
    	    	    	    	Insert_Object(obj);
    	    	    	    } else {
    	    	    	    	mem_free_object(obj);
    	    	    	    	obj = obj2;
    	    	    	    }
    	    	    	    o = mem_get_objref();
    	    	    	    o->obj = obj;
    	    	    	    queue_insert(o, &dep->sources);
    	    	    	    break;
    	    	    	}
    	    	    }
    	    	}
    	    }
    	}
    }
} /* Fill_In_Missing_Sources */
