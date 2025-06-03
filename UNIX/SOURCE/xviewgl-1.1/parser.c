#include "patchlevel.h"
/* Written by Brad Daniels, February 1991

   This code is provided as is, with no warrantees, express or implied.
   I believe this code to be free of encumbrance, and offer it to the
   public domain.  I ask, however, that this paragraph and my name be
   retained in any modified versions of the file you may make, and
   that you notify me of any improvements you make to the code.

   Brad Daniels (daniels@bigred.enet.dec.com)
*/
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef STDLIB
#include <stdlib.h>
#endif
#include "my_types.h"
#include "parser.h"
#include "actions.h"
#include "libutils.h"
#include "utils.h"

MY_FILE *program_fp;
char program_fname[FILENAME_MAX];
int line_no=0;
static struct prog_loc lastloc;
BOOLEAN trace=FALSE;

struct cmd_entry {
    char cmd_name[CMDLEN_MAX];
    BOOLEAN (*cmd)(/*int argc, my_va_list ap, char *err_msg*/);
} cmds[] = {
    { "box", cmd_box },	/* Draw a box */
    { "break", cmd_break },
    { "call", cmd_call },
    { "cfade", cmd_cfade },	/* Fade in image */
    { "cfree", cmd_cfree },	/* Clear clip reg. */
    { "cgetbuf", cmd_cgetbuf },
    { "chgcolor", cmd_chgcolor },	/* Change EGA cmap */
    { "circle", cmd_circle },	/* Draw an ellipse */
    { "cload", cmd_cload },	/* Load clip reg */
    { "closegl", cmd_closegl },
    { "clearscr", cmd_clearscr },	/* Clear the screen */
    { "color", cmd_color },	/* Set primary/2ndary colors */
    { "cycle", cmd_cycle },	/* Cycle through colors */
    { "data", cmd_data },
    { "databegin", cmd_databegin },	/* Start reading from labelled data. */
    { "dataend", cmd_dataend },	/* End of labelled dat.  Should never be executed. */
    { "dataskip", cmd_dataskip },
    { "dfree", cmd_dfree },
    { "dload", cmd_dload },
    { "edge", cmd_edge },	/* Execute command */
    { "else", cmd_else },
    { "endif", cmd_endif },
    { "exec", cmd_exec },	/* Execute command */
    { "exit", cmd_exit },	/* Finish processing */
    { "ffree", cmd_ffree },	/* Free font space */
    { "fgaps", cmd_fgaps },	/* Set font spacing */
    { "fload", cmd_fload },	/* Load a font */
    { "float", cmd_float },	/* Fly preserving bg */
    { "font", cmd_font },
    { "fly", cmd_fly  },	/* Move pic, trash bg */
    { "font", cmd_font  },	/* Something to do with fonts; not sure */
    { "fstyle", cmd_fstyle },
    { "getcolor", cmd_getcolor },
    { "gosub", cmd_gosub },
    { "goto", cmd_goto },
    { "if", cmd_if },
    { "ifkey", cmd_ifkey },
    { "ifmem", cmd_ifmem },
    { "ifmouse", cmd_ifmouse },
    { "ifvideo", cmd_ifvideo },
    { "int", cmd_int },
    { "line", cmd_line },	/* Draw line */
    { "link", cmd_link },	/* Link to new file */
    { "local", cmd_local },
    { "loop", cmd_loop },	/* Loop to last mark */
    { "mark", cmd_mark },	/* Mark loop start */
    { "merge", cmd_merge },
    { "mode", cmd_mode },
    { "mouse", cmd_mouse },
    { "move", cmd_move },
    { "noise", cmd_noise },
    { "note", cmd_note },
    { "offset", cmd_offset },
    { "opengl", cmd_opengl },
    { "out", cmd_out },
    { "palette", cmd_palette },
    { "pan", cmd_pan },
    { "pfade", cmd_pfade },
    { "pfree", cmd_pfree },
    { "pgetbuf", cmd_pgetbuf },
    { "pload", cmd_pload },
    { "pnewbuf", cmd_pnewbuf },
    { "point", cmd_point },
    { "poke", cmd_poke },
    { "pokel", cmd_pokel },
    { "pokew", cmd_pokew },
    { "position", cmd_position },
    { "psave", cmd_psave },
    { "psetbuf", cmd_psetbuf },
    { "putdff", cmd_putdff },
    { "putup", cmd_putup },
    { "rect", cmd_box },	/* Synonym for box??? */
    { "resetgl", cmd_resetgl },
    { "resetscr", cmd_resetscr },
    { "return", cmd_return },
    { "revpage", cmd_revpage },
    { "set", cmd_set },
    { "send", cmd_send },
    { "setcolor", cmd_setcolor },
    { "setpage", cmd_setpage },
    { "setrgb", cmd_setrgb },
    { "setupscr", cmd_setupscr },
    { "spread", cmd_spread },
    { "text", cmd_text },
    { "tile", cmd_tile },
    { "timer", cmd_timer },
    { "tran", cmd_tran },
    { "video", cmd_video },
    { "wait", cmd_waitkey },	/* Alias for waitkey? */
    { "waitkey", cmd_waitkey },
    { "when", cmd_when },	/* Def clipping window */
    { "window", cmd_window },	/* Def clipping window */
    { "", NULL }
};

BOOLEAN get_prog_loc(struct prog_loc *loc)
{
    loc->line = line_no;
    return !my_fgetpos(program_fp, &loc->filepos);
}

BOOLEAN set_prog_loc(struct prog_loc *loc)
{
    line_no = loc->line;
    return !my_fsetpos(program_fp, &loc->filepos);
}

BOOLEAN find_data(struct prog_loc *loc)
{
char cmd[132];
int i;

    get_prog_loc(loc);
    while (my_fgets(cmd,132,program_fp) != NULL) {
	line_no++;
	preprocess_line(cmd);
	i = strspn(cmd," \t");
	if (!strncmp(cmd+i,"data",4) && !isalpha(*(cmd+i+4))) {
	    set_prog_loc(loc);
	    while (i--) my_getc(program_fp);
	    get_prog_loc(loc);
	    return TRUE;
	}
	get_prog_loc(loc);
    }
    return FALSE;
}

static struct label_ent *labels=NULL;

static void add_label(char *label)
{
struct label_ent *newl, *l, *lastl=NULL;
int r;
    for (l = labels,r = -1; (l != NULL) && ((r = strcmp(label, l->label)) >0);
	 lastl = l, l = l->next);
    if (r != 0) {
	newl = malloc(sizeof(*newl));
	newl->label = strcpy(malloc(strlen(label)+1),label);
	newl->next = l;
	if (lastl == NULL)
	    labels = newl;
	else
	    lastl->next = newl;
	memcpy(&newl->pos,&lastloc,sizeof(lastloc));
    }
}

struct label_ent *lookup_label(char *label)
{
struct label_ent *l;
int r;

    for (l=labels; (l!=NULL) && ((r = strcmp(label,l->label)) > 0);
	 l = l->next);
    if (r == 0)
	return l;
    else
	return NULL;
}

static void add_all_labels()
{
struct prog_loc save_pos;
char cmd[132],*lp,*rp;

    get_prog_loc(&save_pos);
    rewind_program();
    line_no=1;
    get_prog_loc(&lastloc);
    while (my_fgets(cmd,132,program_fp) != NULL) {
	line_no++;
	preprocess_line(cmd);
	if (strrchr(cmd,':') != NULL) {
	    /* Add label */
	    rp=strrchr(cmd,':');
	    lp=strtok(cmd,":");
	    rp--;
	    while (isspace(*rp)) *(rp--) = 0;
	    while (isspace(*lp)) lp++;
	    add_label(lp);
	}
	get_prog_loc(&lastloc);
    }
    set_prog_loc(&save_pos);
}

BOOLEAN find_label_location(char *label, struct prog_loc *pos, char *err_msg)
{
struct label_ent *l;

    l = lookup_label(label);
    if (l == NULL) {
	add_all_labels();
	if ((l = lookup_label(label)) == NULL) {
	    sprintf(err_msg,"Label %s not found",label);
	    return FALSE;
	}
    }
    *pos = l->pos;
    return TRUE;
}

BOOLEAN save_prog_info(struct prog_info_rec *pi, char *err_msg)
{
    memcpy(&pi->lastloc,&lastloc,sizeof(lastloc));
    if (!get_prog_loc(&pi->save_loc)) {
	sprintf(err_msg,"File positioning failed while saving context: %s",
		strerror(errno));
	return FALSE;
    }
    strcpy(pi->save_fname,program_fname);
    pi->save_labels = labels;
    labels = NULL;
    pi->save_fp = program_fp;
    line_no=0;
    pi->prog_index = prog_index;
    return TRUE;
}

BOOLEAN restore_prog_info(struct prog_info_rec *pi, char *err_msg)
{
struct label_ent *l,*lastl;

    program_fp = pi->save_fp;
    if (!set_prog_loc(&pi->save_loc)) {
	sprintf(err_msg,"File positioning failed while restoring context: %s",
		strerror(errno));
	return FALSE;
    }
    strcpy(program_fname,pi->save_fname);

    for (lastl=NULL,l=labels; (l!=NULL); l = l->next){
	free(l->label);
	if (lastl != NULL)
	    free(lastl);
	lastl = l;
    }
    if (lastl != NULL)
	free(lastl);
    labels = pi->save_labels;
    memcpy(&lastloc,&pi->lastloc,sizeof(lastloc));
    prog_index = pi->prog_index;
    return TRUE;
}

BOOLEAN exec_line(char *cmd, char *err_msg)
{
struct cmd_entry *a;
char *lp,*last_arg,*rp,buf[MAXARGS*5],*bp;
BOOLEAN got_dash;
int argno, argc, i, re;
my_va_list ap;

    line_no++;
    if (trace)
	fprintf(stderr,"%d: %s\n",line_no,cmd);
    preprocess_line(cmd);
    if (((rp=strrchr(cmd,':')) != NULL) && (strrchr(rp,'"')==NULL)) {
	/* Add label */
	while (isspace(*cmd)) cmd++;
	bp=rp=strchr(cmd,':');
	lp=strtok(cmd,":");
	while (--bp,isspace(*bp)) *bp=0;
	rp++;
	while (rp && isspace(*rp)) rp++;
	/* If we got here as the result of a goto, lastloc will not point
	   to the right place to create a label.  However, since we got
	   here by a goto, the label must already exist so it doesn't matter. */
	add_label(lp);
	if (*rp)
	    cmd=rp;
	else
	    return get_prog_loc(&lastloc);
    }
    get_prog_loc(&lastloc);
    if ((rp=strrchr(cmd,'\r')) != NULL) {
	/* Trash carriage return */
	*rp=0;
    }
    lp=tokenize(cmd);
    if (lp==NULL) return TRUE;
    for (a=cmds; (a->cmd != NULL) && (strcmp(lp,a->cmd_name)); a++);
    if (a->cmd == NULL) {
	sprintf(err_msg,"Invalid command \"%s\"",lp);
	return FALSE;
    }
    got_dash=FALSE;
    argno=argc=0;
    bp=buf;
    while ((lp=tokenize(NULL)) != NULL) {
	if (*lp == '"') {
	    lp++;
	    last_arg=lp;
	    set_arg(ap,argc++,lp);
	} else if (got_dash) {
	    re=atol(lp);
	    for (i=atol(last_arg)+1; i <= re; i++) {
		sprintf(bp,"%d",i);
		set_arg(ap,argc++,bp);
		bp += strlen(bp)+1;
	    }
	    got_dash=FALSE;
	} else if (((rp=strchr(lp,'-')) != NULL) &&
		   (isdigit(*lp) || (rp == lp) && (*(rp+1) == 0))) {
	    if ((rp == lp) && (*(rp+1) == 0)) {
		got_dash=TRUE;
	    } else if (rp != lp) {
		*rp++='\0';
		re=atol(rp);
		for (i=atol(lp); i <= re; i++) {
		    sprintf(bp,"%d",i);
		    set_arg(ap,argc++,bp);
		    bp += strlen(bp)+1;
		}
	    } else {
		last_arg=lp;
		set_arg(ap,argc++,lp);
	    }
	} else if (*lp == '@') {
	    if ((rp=read_data(err_msg)) == NULL) {
		return FALSE;
	    }
	    strcpy(bp,rp);
	    set_arg(ap,argc++,bp);
	    bp += strlen(bp)+1;
	} else {
	    last_arg=lp;
	    set_arg(ap,argc++,lp);
	}
    }
    return a->cmd(argc, ap, err_msg);
}
