/*									*/
/*	Copyright (©) Ecole Nationale Supérieure des Télécommunications */
/*									*/
/*	EMON: moniteur ethernet	(V2.1-2)				*/
/*									*/
/*	20-fev-1994:	Guillaume gérard				*/
/*									*/
/*	Ce logiciel est fourni gratuitement et ne peut faire		*/
/*		l'objet d'aucune commercialisation			*/
/*									*/
/*	Aucune garantie d'utilisation, ni implicite,			*/
/*		ni explicite, n'est fournie avec ce logiciel.		*/
/*		Utilisez-le à vos risques et périls			*/
/*									*/
/*	This freeware is given freely and cannot be sold		*/
/*	No warranty is given for reliability of this software		*/
/*	Use at your own risk						*/
/*									*/
#include <ctype.h>
#include <errno.h>
#include <string.h>

#include <descrip.h>
#include <signal.h>
#include <smgdef.h>
#include <ssdef.h>
#include <stsdef.h>
#include <trmdef.h>

#if defined(__STDC__) || defined (__DECC)
#include <stdlib.h>
#endif

#include "monitor"
#include "ni_routines"
#include "stat"
#include "record"

#ifndef SMG$_EOF
#define SMG$_EOF	1213442	/* bug in smgdef.h */
#endif

#ifndef SMG$_FACILITY
#define SMG$_FACILITY	18	/* bug in smgdef.h */
#endif

int	SMG$ERASE_DISPLAY(), SMG$SET_CURSOR_ABS(), SMG$DELETE_PASTEBOARD(),
	SMG$ERASE_LINE(), SMG$PUT_LINE(), SMG$PUT_WITH_SCROLL(), 
	SMG$PUT_CHARS(), SMG$PUT_HELP_TEXT(), SMG$FLUSH_BUFFER(),
	SMG$CONTROL_MODE(), SMG$SET_OUT_OF_BAND_ASTS(),
	SMG$SET_KEYPAD_MODE(), SMG$CANCEL_INPUT(), 
	SMG$READ_LINE(), SMG$READ_STRING(), 
	SMG$RETURN_INPUT_LINE(), SMG$REPLACE_INPUT_LINE(),
	SMG$SAVE_PHYSICAL_SCREEN(), SMG$RESTORE_PHYSICAL_SCREEN(),
	SMG$CREATE_PASTEBOARD(), SMG$CREATE_VIRTUAL_DISPLAY(),
	SMG$CREATE_VIRTUAL_KEYBOARD(), SMG$MOVE_VIRTUAL_DISPLAY(), 
	SMG$PASTE_VIRTUAL_DISPLAY(), SMG$UNPASTE_VIRTUAL_DISPLAY(),
	SMG$REPASTE_VIRTUAL_DISPLAY(), SMG$REPAINT_SCREEN(), 
	SMG$BEGIN_DISPLAY_UPDATE(), SMG$END_DISPLAY_UPDATE(),
	SMG$BEGIN_PASTEBOARD_UPDATE(), SMG$END_PASTEBOARD_UPDATE();

int	LIB$SIGNAL (int, ...);

static	const $DESCRIPTOR (null_d, "");	/* null string */

static ulong exit_status= SS$_NORMAL;
static ulong param_toggle;
static boolean mon_suspend= false;
static boolean mon_collect= false;
static boolean mon_lock= false;
static ulong errpos;
static boolean	errpasted= false;
static ulong clistat;		/* status of last CLI operation */
static boolean statfr_use;	/* used for extended filtering */

static long	title_ypos;

typedef enum {oob_read, oob_monitor, oob_none} outbands;


void
fatal_bug (int status, char * mess)
{
	puts ("\n *** FATAL BUG DETECTED ***");
	puts (mess);
	exit (status);
}

void
signal_bug (int status, char * mess)
{
	puts ("\n *** BUG DETECTED ***");
	puts (mess);
	LIB$SIGNAL (status);
}

static
void
stop_monitor(void)
{
	ulong aststs;
	int SYS$SETAST(), SYS$CANTIM();

	aststs= SYS$SETAST (0);
	SYS$CANTIM (0,0);
	mon_stop++;
	if (aststs==SS$_WASSET) SYS$SETAST(1);
}

void abort_monitor(reason)
char * reason;
{
	stop_monitor();
	stop_port(NULL);
	if (reason) emon$put_error (reason);
}

void exit_monitor(status)
ulong status;
{
	abort_monitor(0);
	mon_exit++;
	exit_status= status;
}

boolean lock_monitor (locked)
boolean locked;
{
	boolean was_locked= mon_lock;
	mon_lock= locked;
	return was_locked;
}

static void
timer_routine(rtn)
boolean (* rtn)();
{
	if (istrue(mon_collect)){
		get_circuit_counters();
		get_line_counters(); /* try to have line ctrs > circuit ctrs */
		if (istrue(mon_lock)){
			(void) (* rtn)();
			init_monitor(NULL);
		}
		else stop_port( ( void (*)() )rtn);
	}
}

void
init_monitor(func)
boolean (* func)();
{
static const $DESCRIPTOR (tim_desc,"0 0:0:8");
static boolean (* savfunc)();
	time_t tim_q;
	int	status, SYS$BINTIM(), SYS$SETIMR();

	status= SYS$BINTIM (&tim_desc,&tim_q);
	if isfalse(status) fatal_bug (status,"init_monitor: $BINTIM");
	if (func==NULL) status= SYS$SETIMR (0, &tim_q, timer_routine, savfunc);
	else {
		savfunc= func;
		status= SYS$SETIMR (0,&tim_q,timer_routine,func);
	}
	if isfalse(status) fatal_bug (status,"init_monitor: $SETIMR");
	mon_stop=0;
}

void
monitor(stat_func)
boolean (* stat_func)();
{
	int SYS$SETAST();

	(void) SYS$SETAST(0);		/* Disable AST's */
	while (!mon_stop){
		if (istrue(start_port()))
		if (istrue(mon_suspend)){
			mon_suspend= false;
		}
		else{
			get_line_counters();	/* initialize line counters */
		}
		mon_collect= true;
		SYS$SETAST(1);	/* Enable AST's */
		ni_read (stat_func);
		mon_collect= false;
		if (!mon_stop && isfalse(mon_suspend)) init_monitor(0);
	}
	(void) SYS$SETAST(1);		/* Enable AST's (Sécurité) */
}

/*	mise d'un titre sur la derniere ligne de l'ecran "status"	*/
void 
emon$title(mess)
const char * mess;
{
	struct dsc$descriptor buf_d= {0,0,0,0};

	buf_d.dsc$w_length= strlen(mess);
	buf_d.dsc$a_pointer= (char *) mess;

	emon$display_home (&title_coord);
	SMG$ERASE_LINE(&title_id);
	SMG$PUT_LINE (&title_id, &buf_d);

	emon$paste_display (&title_coord);
}

/*	AST routine	*/

static void
suspend_monitor()
{
	int SYS$SETAST();

	if (isfalse(mon_lock) &&
	    istrue(mon_collect)){
		SYS$SETAST(0);
		mon_suspend= true;
		stop_port(NULL);
	}
}

static interrupt_handler()
{
	suspend_monitor();	
}

static const ulong control_char_mask= (1<<('C'-'@'))|	/* command */
				      (1<<('U'-'@'))|	/* up	*/
				      (1<<('D'-'@'))|	/* down */
				      (1<<('R'-'@'))|	/* right */
				      (1<<('L'-'@'))|	/* left */
				      (1<<('P'-'@'))|	/* toggle parameter window */
				      (1<<('W'-'@'))|	/* refresh */
				      (1<<('Y'-'@'))|	/* debug */
				      (1<<('Z'-'@'));	/* exit */
	/* controls useable while read in progress */
static const ulong control_char_mask_read= (1<<('W'-'@'));
static const ulong control_char_mask_none= 0;

typedef struct {ulong SMG$L_PASTEBOARD_ID, SMG$L_ARG;
		ubyte SMG$B_CHARACTER;}	char_int$$param;

void monitor_int(char_int$$param * int_ptr)
{
	if (int_ptr->SMG$L_PASTEBOARD_ID==paste_id){
		switch (int_ptr->SMG$B_CHARACTER+'@'){
		boolean savbuf;
		default:
			abort_monitor("Illegal int argument (char)");
			break;
		case 'Z':	mon_exit++;
		case 'C':
			mon_lock= false;
			interrupt_handler();
			stop_monitor();
			break;
		case 'U':	/*	UP	*/
			if isfalse(line_coord.pasted) break;
			suspend_monitor();
			savbuf= emon$enable_buf(true);
			line_xpos -= pb_rows/2;
			SMG$REPASTE_VIRTUAL_DISPLAY (&line_id,
						     &paste_id,
						     &line_xpos,
						     &line_ypos,
						     &status_id);
			emon$flush_buffer();
			emon$enable_buf(savbuf);
			break;
		case 'D':	/*	DOWN	*/
			if isfalse(line_coord.pasted) break;
			suspend_monitor();
			savbuf= emon$enable_buf(true);
			line_xpos += pb_rows/2;
			if (line_xpos>line_coord.posx) 
						line_xpos=line_coord.posx;
			SMG$REPASTE_VIRTUAL_DISPLAY (&line_id,
						     &paste_id,
						     &line_xpos,
						     &line_ypos,
						     &status_id);
			emon$flush_buffer();
			emon$enable_buf(savbuf);
			break;
		case 'L':	/*	LEFT	*/
			if isfalse(line_coord.pasted) break;
			suspend_monitor();
			savbuf= emon$enable_buf(true);
			line_ypos -= pb_columns/2;
			emon$begin_pasteboard_update();
			SMG$REPASTE_VIRTUAL_DISPLAY (&line_id,
						     &paste_id,
						     &line_xpos,
						     &line_ypos,
						     &status_id);
			title_ypos -= pb_columns/2;
			SMG$REPASTE_VIRTUAL_DISPLAY (&title_id,
						     &paste_id,
						     &title_coord.posx,
						     &title_ypos,
						     &status_id);
			emon$end_pasteboard_update();
			emon$flush_buffer();
			emon$enable_buf(savbuf);
			break;
		case 'R':	/*	RIGHT	*/
			if isfalse(line_coord.pasted) break;
			suspend_monitor();
			savbuf= emon$enable_buf(true);
			line_ypos += pb_columns/2;
			if (line_ypos>line_coord.posy) 
						line_ypos= line_coord.posy;
			emon$begin_pasteboard_update();
			SMG$REPASTE_VIRTUAL_DISPLAY (&line_id,
						     &paste_id,
						     &line_xpos,
						     &line_ypos,
						     &status_id);
			title_ypos += pb_columns/2;
			if (title_ypos>title_coord.posy) 
						title_ypos= title_coord.posy;
			SMG$REPASTE_VIRTUAL_DISPLAY (&title_id,
						     &paste_id,
						     &title_coord.posx,
						     &title_ypos,
						     &status_id);
			emon$end_pasteboard_update();
			emon$flush_buffer();
			emon$enable_buf(savbuf);
			break;
		case 'P':
			suspend_monitor();
			savbuf= emon$enable_buf(true);
			if (1&(param_toggle++))
				emon$unpaste_display (&param_coord);
			else
				emon$paste_display (&param_coord);
			emon$flush_buffer();
			emon$enable_buf(savbuf);
			break;
		case 'W':
			suspend_monitor();
			savbuf= emon$enable_buf(true);
			SMG$REPAINT_SCREEN(&paste_id);
			emon$flush_buffer();
			emon$enable_buf(savbuf);
			break;
		case 'Y':
			{
				suspend_monitor();
				LIB$SIGNAL(SS$_DEBUG);
				break;
			}
		}
		SMG$CANCEL_INPUT (&keybd_id);
	}
	else{
		abort_monitor("Illegal int argument (paste id)");
	}
}

boolean
emon$enable_buf (buf_enabled)
boolean buf_enabled;
{
	static ulong pb_mode= SMG$M_MINUPD|SMG$M_BUF_ENABLED;
	ulong	old_mode;

	if (istrue(buf_enabled)) pb_mode |= SMG$M_BUF_ENABLED;
	else pb_mode &= ~SMG$M_BUF_ENABLED;
	SMG$CONTROL_MODE (&paste_id,&pb_mode,&old_mode);
	return (old_mode&SMG$M_BUF_ENABLED)&&1; 
}

static void 
set_outbands(outbands mode)
{
	int status;
	vmsmsgbuf stsbuf;
	const unsigned long * maskaddr;

	switch(mode) {
	case oob_read:	maskaddr= &control_char_mask_read; break;
	case oob_monitor: maskaddr= &control_char_mask;	   break;
	case oob_none:	maskaddr= &control_char_mask_none; break;
	}

	status= SMG$SET_OUT_OF_BAND_ASTS(&paste_id, maskaddr, monitor_int);
	if isfalse (status) emon$print_error ("Emon error %s returned by SETBAND",
					       emon$translate_status(stsbuf,
								    status));
}

void
emon$put_error (mess)
const char * mess;
{

	if (istrue(errpasted)) emon$put_more_error (mess);
	else{
		struct dsc$descriptor buf_d= {0, 0, 0, 0};

		buf_d.dsc$a_pointer= (char *) mess;
		buf_d.dsc$w_length= strlen (mess);
		errpos= error_xpos_init;
		SMG$SET_CURSOR_ABS(&error_id,&un,&un);
		SMG$PUT_LINE (&error_id,&buf_d);
		emon$paste_display (&error_coord);
		errpasted= true;
	}
}

#include <stdarg.h>

void
emon$print_error (const char * line,...)
{
	char	buf[512];
	va_list ap;
	int sts, SYS$SETAST();

	va_start (ap,line);
	sts= SYS$SETAST (0);
	(void) vsprintf(buf, line, ap);
	va_end (ap);
	emon$put_error (buf);
	if (sts==SS$_WASSET) (void) SYS$SETAST (1);
}

void
emon$put_more_error (mess)
const char * mess;
{
	struct dsc$descriptor buf_d= {0, 0, 0, 0};

	buf_d.dsc$a_pointer= (char *) mess;
	buf_d.dsc$w_length= strlen (mess);	
	if (errpos+error_coord.lenx>error_xpos_init) errpos-= 1;
	SMG$MOVE_VIRTUAL_DISPLAY 
			(&error_id, &paste_id, &errpos, &error_coord.posy);
	SMG$PUT_LINE (&error_id,&buf_d);
}

void
emon$put_cerror (mess)
const char * mess;
{
	emon$put_error (mess);
	emon$put_more_error (strerror (errno,vaxc$errno));
}

static emon$get_input(desc,prompt,rlen)
struct dsc$descriptor * desc, * prompt;
ushort * rlen;
{
static const	ulong modifiers=TRM$M_TM_NORECALL;
	ulong	status;
	ushort	code;
	char	recall_line[132],firstline[132];
	struct	dsc$descriptor recall_d= {0, 0, 0, 0};
	struct	dsc$descriptor firstline_d= {sizeof firstline-1, 0, 0, 0};
	struct	dsc$descriptor bufdesc= *desc;
	int	numline=0;
	int	inputsize= command_coord.leny-prompt->dsc$w_length;
	int	startcol;

	recall_d.dsc$a_pointer= recall_line;
	firstline_d.dsc$a_pointer= firstline;

	SMG$ERASE_DISPLAY (&command_coord);
	emon$paste_display (&command_coord);
	set_outbands(oob_read);
	SMG$RETURN_INPUT_LINE (&keybd_id, &firstline_d, 0, &un, 
			       &firstline_d.dsc$w_length);
	firstline[firstline_d.dsc$w_length]= 0;
	bufdesc.dsc$w_length--;	/* one spare byte for asciZ */

	for (;;) {
		status= SMG$SET_CURSOR_ABS (&command_id, &un, &un);
		if isfalse(status) break;
		startcol= prompt->dsc$w_length+recall_d.dsc$w_length;
		status= SMG$ERASE_LINE  (&command_id, &startcol);
		if isfalse(status) break;
		status= SMG$READ_STRING (&keybd_id, &bufdesc, prompt,
					 &inputsize, &modifiers, 0, 0,
					 rlen, &code, &command_id, &recall_d);
		if (code==SMG$K_TRM_CTRLB) code= SMG$K_TRM_UP; /* TTDRIVER */
		if ( istrue(status) &&
		     ((code==SMG$K_TRM_UP) || (code==SMG$K_TRM_DOWN)) ){
			bufdesc.dsc$a_pointer[*rlen]= '\0';
	/* une ligne n'est entree dans le recall buffer que si elle est */
	/* differente de la precedente ET de taille non nulle */
			while ((*rlen>1) && (bufdesc.dsc$a_pointer[*rlen]==' '))
				*rlen--;
			if ((*rlen!=0) && 
			     strcmp (firstline,bufdesc.dsc$a_pointer)) {
				status= SMG$REPLACE_INPUT_LINE (&keybd_id);
				if isfalse(status) break;
			}
			if (code==SMG$K_TRM_UP)
				if (numline>=recall_size){
					recall_d.dsc$w_length= 0;
					continue;
				} else	numline++;
			else
				if  (numline==1) {
					recall_d.dsc$w_length=0;
					continue;
				} else numline--;
			recall_d.dsc$w_length= sizeof recall_line-1;
			status= SMG$RETURN_INPUT_LINE (&keybd_id,
						       &recall_d,
						       0,
						       &numline,
						       &recall_d.dsc$w_length);
			if istrue(status) continue;
		}
		break;
	}			
	
/**/
/*	status= SMG$READ_COMPOSED_LINE (&keybd_id, &key_id, desc,
/*					prompt, rlen, &command_id);
/**/
	if (status==SMG$_EOF) {
		mon_exit= true;
		status= 1;
	}

	if ( isfalse(status) && isfalse(mon_exit)	/* suite a pbs SMG */
		&& ($VMS_STATUS_FAC_NO(status) == SMG$_FACILITY)) {
		emon$put_error ("error reading command input");
		exit (status);
	}

	set_outbands(oob_monitor);
	if istrue (errpasted) {
		emon$unpaste_display (&error_coord);
		errpasted= false;
	}
	emon$unpaste_display (&command_coord);
	return status;
}


void
status_update()
{
	static unsigned long bitrate;
	static const struct {ulong posy,posx;}	cnt_pos={3,1}, 
						abs_pos= {4,1},
						date_pos={1,70};
	static const char bufline[]="%8lu%8lu%7lu%7lu%7lu%8lu%8lu%5lu%5lu";
	static char buf[8+8+7+7+7+8+8+5+5+1];
	static struct dsc$descriptor buf_d= {sizeof buf, 0, 0, 0};
	static unsigned long maxbitrate=0;
	int	SYS$ASCTIM(), SYS$GETTIM();

	buf_d.dsc$a_pointer= buf;
	emon$begin_display_update (&status_coord);
	bitrate=delta_line_counters.BRC+
/*		delta_line_counters.MBY+	*/
		delta_line_counters.BSN+
/*		delta_line_counters.MSN+	*/
		 (ETHERNET_LINK_LAYER_OVERHEAD)*
			(delta_line_counters.DBR
/*+			 delta_line_counters.MBL	*/
+			 delta_line_counters.DBS
/*+			 delta_line_counters.MBS	*/);
	if (bitrate>maxbitrate) maxbitrate=bitrate;
	SYS$ASCTIM (&buf_d,&buf_d,0,1);
	SMG$PUT_CHARS (&status_id, &buf_d, &date_pos.posy, &date_pos.posx);

	buf_d.dsc$w_length= sizeof buf -1;
	sprintf (buf,bufline,
				bitrate,
				delta_circuit_counters.DBR,
				delta_circuit_counters.UBU,
				delta_line_counters.SBU,
			     delta_line_counters.LBE,delta_line_counters.OVR,
			     delta_line_counters.RFL,delta_line_counters.SFL,
			     delta_line_counters.CDC);
	SMG$PUT_CHARS (&status_id, &buf_d, &cnt_pos.posy, &cnt_pos.posx);

	sprintf (buf,bufline,
				maxbitrate,
				cumulated_circuit_counters.DBR,
				cumulated_circuit_counters.UBU,
				cumulated_line_counters.SBU,
			        cumulated_line_counters.LBE,cumulated_line_counters.OVR,
				cumulated_line_counters.RFL,cumulated_line_counters.SFL,
				cumulated_line_counters.CDC);
	SMG$PUT_CHARS (&status_id, &buf_d, &abs_pos.posy, &abs_pos.posx);
	emon$end_display_update (&status_coord);

	if istrue (recording) {
		rec_counters_t rec;
			rec.cbrc= delta_circuit_counters.BRC;
			rec.cdbr= delta_circuit_counters.DBR;
			rec.cubu= delta_circuit_counters.UBU;
			rec.lsbu= delta_line_counters.SBU;
			rec.llbe= delta_line_counters.LBE;
			rec.lovr= delta_line_counters.OVR;
			rec.lrfl= delta_line_counters.RFL;
			rec.lsfl= delta_line_counters.SFL;
			rec.lcdc= delta_line_counters.CDC;
			rec.header.version= REC_COUNTERS_VERS;
			rec.header.type= REC_COUNTERS;
			SYS$GETTIM (&rec.header.time);
			write_record (&rec, sizeof rec);
	}
}

/*	filter: 	2 routines	*/
/*	clear_filter_status: efface la partie d'ecran ou est affiche le filtre*/
/*	get_filter: trouve les filtres et remplit les champs de l'appelant    */
/*			puis les affiche dans l'ecran 'status'		      */
static	const struct {unsigned long y, x;}
		proto_pos= {2,70},
		from_pos= {3,65},
		to_pos= {4,65};
void
clear_filter_status()
{
	SMG$SET_CURSOR_ABS (&status_id, &proto_pos.y, &proto_pos.x);
	SMG$PUT_LINE (&status_id,&null_d);
	SMG$SET_CURSOR_ABS (&status_id, &from_pos.y, &from_pos.x);
	SMG$PUT_LINE (&status_id,&null_d);
	SMG$SET_CURSOR_ABS (&status_id, &to_pos.y, &to_pos.x);
	SMG$PUT_LINE (&status_id,&null_d);
}

protocol_entry *
find_protocol(protocol_name,addr)
char * protocol_name;
ethernet_protocol_t * addr;
{
	int i;
	for (i=0; i<maxprotocol; i++)
		if (!strcmp (protocol_name,tabprotocol[i].name)){
			* addr= tabprotocol[i].protocol;
			return tabprotocol+i;
		};
	return NULL;
}

static protocol_entry *
get_proto_lst (buf)
cli_buf_t * buf;
{
static	protocol_entry unkproto;
	protocol_entry * pp;
	ethernet_protocol_t protocol;
	int count= 0;
	enum {all, none, undef} glob_init= undef;/* ordre impose par boolean */

	do {
		char * nn = (char *) buf;
		if (nn[0]=='~') {
			switch (glob_init) {
			case all:   break;
			case none:  emon$put_error ("Invalid protocol list");
				    return NULL;
			case undef: statfr_enable_protocols();
				    glob_init= all;
				    statfr_use= true;
			}
			nn++;
		}
		else {
			switch (glob_init) {
			case none:  break;
			case all:   emon$put_error ("Invalid protocol list");
				    return NULL;
			case undef: statfr_disable_protocols();
				    glob_init= none;
			}
		}

		if ((pp=find_protocol(nn,& protocol))!=NULL){
			pp->pass= (boolean) glob_init;
			if (++count>1) statfr_use= true;
		}
		else	if (!strcmp ( (char *) buf, "unknown")){
				statfr_enable_unknown_protocols();
				statfr_use= true;
				strcpy (unkproto.name,"unknown");
				pp= & unkproto;
			}
		else{
			emon$put_error ("Unknown protocol");
			return NULL;
		}

	} while istrue(getclistring ("PROTOCOL", (char *) buf));

	return pp;
}

typedef enum {FROM_LIST, TO_LIST, END_LIST, BTW_LIST} list_type;

static boolean
get_node_lst (buf, src, fnode, snode)
cli_buf_t * buf;
list_type src;
node_entry **fnode, **snode;	/* first node, second node */
{
static	node_entry unknode;
	node_entry * pp, *pp1, *pp2;
	ethernet_addr_t node;
	int count= 0;
	enum {all, none, undef} glob_init= undef;
static	const	char * cli_kwds[]= {"FROM","TO","END_ADDRESS","BETWEEN"};

	pp1= NULL;
	pp2= NULL;

	do {
		char *nn= (char *) buf;
		if (nn[0]=='~') {
			switch (glob_init) {
			case all: break;
			case none:  emon$put_error ("Invalid node list");
				   return false;
			case undef:switch (src) {
				    case FROM_LIST: statfr_enable_srcs();
						break;
				    case TO_LIST:   statfr_enable_dsts();
						break;
				    case END_LIST:	statfr_enable_ends();
						break;
				    case BTW_LIST:	statfr_enable_srcs();
							statfr_enable_dsts();
				    }
				    glob_init= all;
				    statfr_use= true;
			}
			nn++;
		}
		else {
			switch (glob_init) {
			case none: break;
			case all:  emon$put_error ("Invalid node list");
				   return false;
			case undef:switch (src) {
				    case FROM_LIST:	statfr_disable_srcs();
						break;
				    case TO_LIST:	statfr_disable_dsts();
						break;
				    case END_LIST:	statfr_disable_ends();
						break;
				    case BTW_LIST:	statfr_disable_srcs();
							statfr_disable_dsts();
				    }
				    glob_init= none;
			}
		}

		if ((pp=find_node(nn, &node))!=NULL){
			switch (src) {
			case FROM_LIST:	pp->passsrc= (boolean) glob_init; break;
			case TO_LIST:	pp->passdst= (boolean) glob_init; break;
			case END_LIST:  pp->passnod= (boolean) glob_init; break;
			case BTW_LIST:	pp->passsrc= (boolean) glob_init; 
				        pp->passsrc= (boolean) glob_init;
			}
			if (++count>1)
				if (src==BTW_LIST) {
					if (count>2) statfr_use= true;
					else	pp2= pp;	/* hack */
				}
				else statfr_use= true;
			else	pp1= pp;
		}
		else	if (!strcmp ( (char *) buf,"unknown")){
				switch (src) {
				case FROM_LIST: statfr_enable_unknown_srcs();
					break;
				case TO_LIST:   statfr_enable_unknown_dsts();
					break;
				case END_LIST:	statfr_enable_unknown_ends();
					break;
				case BTW_LIST:	statfr_enable_unknown_srcs();
						statfr_enable_unknown_dsts();
				}
				statfr_use= true;
				strcpy (unknode.name,"unknown");
				pp= & unknode;
			}
		else{
			switch (src){
			case	FROM_LIST: emon$put_error ("Unknown source");
					break;
			case	TO_LIST: emon$put_error ("Unknown destination");
					break;
			case	END_LIST: emon$put_error ("Unknown end node");
					break;
			case	BTW_LIST: emon$put_error ("Unknown node");
					break;
			}
			return false;
		}

	} while istrue(getclistring (cli_kwds[src], (char *) buf));

	*fnode= pp1;
	if (snode!=NULL) *snode= pp2;

	return true;
}

/*	parse	/FROM, /TO, /END_ADDRESS, /BETWEEN, and /PROTOCOL switches */
/*	sanity checks done by CLI	*/

boolean
get_filter (from, to, proto, stat_flags)
node_entry ** from, **to;
protocol_entry **proto;
int * stat_flags;
{
	cli_buf_t buf;
	protocol_entry * pp;
	node_entry *np;
	char smgbuf[80];
	struct dsc$descriptor buf_d= {0, 0, 0, 0};
	ethernet_addr_t from_addr, to_addr;

	statfr_reset();
	statfr_use= false;
	*stat_flags= 0;

	SMG$SET_CURSOR_ABS (&status_id, &proto_pos.y, &proto_pos.x);
	if (getclistring("PROTOCOL",buf)) {
		*proto=get_proto_lst (buf);
		if (*proto!=NULL) {
			struct dsc$descriptor buf_d= {0, 0, 0, 0};

			buf_d.dsc$a_pointer= (*proto)->name;
			buf_d.dsc$w_length= strlen ( (*proto)->name);
			SMG$PUT_LINE (&status_id, &buf_d);
		}
		else return false;
	}
	else {
		*proto= NULL;
		statfr_enable_protocols();
		SMG$PUT_LINE (&status_id, &null_d);
	}

	buf_d.dsc$a_pointer= smgbuf;

	SMG$SET_CURSOR_ABS (&status_id, &from_pos.y, &from_pos.x);
	if (getclistring("END_ADDRESS", buf)){
		*to= NULL;	/* pour ancienne interface */
		if istrue(get_node_lst (buf, END_LIST, from, NULL)) {
			sprintf (smgbuf, "End %s", (*from)->name);
			buf_d.dsc$w_length= strlen (smgbuf);
			SMG$PUT_LINE (&status_id, &buf_d);
			* stat_flags|= LOC_STAT_BOTHWAYS|LOC_STAT_ONEWAY;
		}
		else return false;
	}
	else {
		statfr_disable_ends();	/* defeat end mechanism */

		if (getclistring("BETWEEN", buf)){
			if istrue (get_node_lst (buf, BTW_LIST, from, to)) {
				sprintf (smgbuf, "End %s", (*from)->name);
				buf_d.dsc$w_length= strlen (smgbuf);
				SMG$PUT_LINE (&status_id, &buf_d);
				* stat_flags|= LOC_STAT_BOTHWAYS;
			}
			else return false;
		}
		else{
			if (getclistring("FROM",buf)){
			   if istrue(get_node_lst (buf, FROM_LIST, from, NULL)) {
					sprintf (smgbuf, "From %s", (*from)->name);
					buf_d.dsc$w_length= strlen (smgbuf);
					SMG$PUT_LINE (&status_id, &buf_d);
			   }
			   else return false;
			}
			else {
				*from= NULL;
				statfr_enable_srcs();
				SMG$PUT_LINE (&status_id, &null_d);
			}
	
			SMG$SET_CURSOR_ABS (&status_id, &to_pos.y, &to_pos.x);
	
			if (getclistring("TO",buf)){
			   if istrue (get_node_lst (buf, TO_LIST, to, NULL)) {
					sprintf (smgbuf, "To   %s", (*to)->name);
					buf_d.dsc$w_length= strlen (smgbuf);
					SMG$PUT_LINE (&status_id, &buf_d);
			   }
			   else return false;
			}
			else {
				*to= NULL;
				statfr_enable_dsts();
				SMG$PUT_LINE (&status_id, &null_d);
			}
		}
	}

	if istrue (statfr_use)	* stat_flags |= LOC_STAT_XTEND;
	statfr_apply();

	return true;
}

void
emon$put_with_scroll (buf,len)
char * buf;
int len;
{
	struct dsc$descriptor buf_d={0, 0, 0, 0};

	buf_d.dsc$a_pointer= buf;
	buf_d.dsc$w_length= len;
	SMG$PUT_WITH_SCROLL (&scroll_id, &buf_d);
	*buf= 0;
}

void
emon$put_line (buf,len)
char * buf;
int len;
{
	struct dsc$descriptor buf_d={0, 0, 0, 0};
	int status;

	buf_d.dsc$a_pointer= buf;
	buf_d.dsc$w_length= len;
	status= SMG$PUT_LINE (&line_id, &buf_d);
	if isfalse (status) signal_bug (status, "error in emon$put_line");
}

void
emon$paste_display (cp)
display_t * cp;
{
	unsigned long status;

	cp->pasted= true;

	status= SMG$PASTE_VIRTUAL_DISPLAY (&cp->id, &paste_id,
					   &cp->posx, &cp->posy);
	if isfalse (status) {
		vmsmsgbuf stsbuf;
		emon$print_error ("Emon error %s returned by PASTE",
				  emon$translate_status (stsbuf, status) );
		LIB$SIGNAL (status);
	}
}

void
emon$unpaste_display (cp)
display_t * cp;
{
	unsigned long status;

	cp->pasted= false;

	status= SMG$UNPASTE_VIRTUAL_DISPLAY (&cp->id, &paste_id);
	if isfalse(status){
		vmsmsgbuf stsbuf;
		emon$print_error ("Emon error %s returned by UNPASTE DISPLAY",
			           emon$translate_status (stsbuf, status) );
		LIB$SIGNAL (status);
	}
}

void
emon$erase_display (display_t * display)
{
	ulong status;

	status= SMG$ERASE_DISPLAY (&display->id);
	if isfalse (status) {
		vmsmsgbuf stsbuf;
		emon$print_error ("Emon error %s returned by ERASE_DISPLAY",
				  emon$translate_status (stsbuf, status) );
		LIB$SIGNAL (status);
	}
}

void
emon$display_home (display_t * display)
{
	ulong status;
	status= SMG$SET_CURSOR_ABS (&display->id, &un, &un);
	if isfalse (status) {
		vmsmsgbuf stsbuf;
		emon$print_error ("Emon error %s returned by SET_CURSOR_ABS",
				  emon$translate_status (stsbuf, status) );
		LIB$SIGNAL (status);
	}
}

void
emon$flush_buffer()
{
	ulong status;
	status= SMG$FLUSH_BUFFER (&paste_id);
	if isfalse (status) {
		vmsmsgbuf stsbuf;
		emon$print_error ("Emon error %s returned by FLUSH_BUFFER",
				  emon$translate_status (stsbuf, status) );
		LIB$SIGNAL (status);
	}
}

void
emon$begin_display_update(display_t * display)
{
	ulong status;
	status= SMG$BEGIN_DISPLAY_UPDATE (&display->id);
	if isfalse (status) {
		vmsmsgbuf stsbuf;
		emon$print_error ("Emon error %s returned by BEGIN_DISPLAY_UPDATE",
				  emon$translate_status (stsbuf, status) );
		LIB$SIGNAL (status);
	}
}

void
emon$end_display_update(display_t * display)
{
	ulong status;
	status= SMG$END_DISPLAY_UPDATE (&display->id);
	if isfalse (status) {
		vmsmsgbuf stsbuf;
		emon$print_error ("Emon error %s returned by END_DISPLAY_UPDATE",
				  emon$translate_status (stsbuf, status) );
		LIB$SIGNAL (status);
	}
}

void
emon$begin_pasteboard_update()
{
	ulong status;
	status= SMG$BEGIN_PASTEBOARD_UPDATE (&paste_id);
	if isfalse (status) {
		vmsmsgbuf stsbuf;
		emon$print_error ("Emon error %s returned by BEGIN_PASTEBOARD_UPDATE",
				  emon$translate_status (stsbuf, status) );
		LIB$SIGNAL (status);
	}
}

void
emon$end_pasteboard_update()
{
	ulong status;
	status= SMG$END_PASTEBOARD_UPDATE (&paste_id);
	if isfalse (status) {
		vmsmsgbuf stsbuf;
		emon$print_error ("Emon error %s returned by END_PASTEBOARD_UPDATE",
				  emon$translate_status (stsbuf, status) );
		LIB$SIGNAL (status);
	}
}

/*	find node by name	*/
/*	returns pointer to node entry */
/*	and node hard addr */

node_entry *
find_node(node_name,addr)
char * node_name;	/* input */
ethernet_addr_t * addr;	/* output */
{
	int i;
	for (i=0; i<maxnode; i++)
		if (!strcmp (node_name,tabnode[i].name)){
			* addr= tabnode[i].addr;
			return tabnode+i;
		};
	return NULL;
}

char *
emon$translate_status (vmsmsgbuf buffer, unsigned long val)
{
	unsigned status;
	unsigned short retlen;
	struct dsc$descriptor bufdesc= {0, 0, 0, 0};
	unsigned getmsg_flags;
	int SYS$GETMSG();

	bufdesc.dsc$a_pointer= buffer;
	bufdesc.dsc$w_length= sizeof (vmsmsgbuf) -1;
	
	/* don't include text; if system only include mnemonic */
	if ( $VMS_STATUS_FAC_NO(val) == 0)	getmsg_flags=0x2;
	else					getmsg_flags=0xE;

	status= SYS$GETMSG (val, &retlen, &bufdesc, getmsg_flags, 0);
	if (isfalse(status) || (status==SS$_MSGNOTFND))
		sprintf (buffer, "Vms code %%X%08X", val);
	else buffer[retlen]= 0;
	return buffer;
}

boolean
getclistring (param, value)
const char * param;
cli_buf_t value;
{
	ulong	status;
	struct dsc$descriptor param_d={0, 0, 0, 0};
	struct dsc$descriptor buf_d={MAXCLILEN, 0, 0, 0};
	int CLI$GET_VALUE();

	param_d.dsc$w_length= strlen(param);
	param_d.dsc$a_pointer= (char *) param;
	buf_d.dsc$a_pointer= value;
	status= CLI$GET_VALUE (&param_d, &buf_d, &buf_d.dsc$w_length);
	if ( (status & 1)==0 ) value[0]= 0;
	else {
		value[buf_d.dsc$w_length]= 0;
		if (value[0]=='"'){
			int i,j=0;
			register char *cp= value;
			for (i= buf_d.dsc$w_length; (i--);){
				j++;
				if (value[j]==0){
					emon$put_error("Unterminated string");
					return 1;
				}
				if (value[j]!='"') *cp++= value[j];
				else {
					j++;
					if (value[j]!='"') {
						*cp=0;
						break;
					}
					else *cp++= value[j++];
				}
			}
		}
		else{
			char * cp;
			for (cp=value; *cp; cp++) *cp= tolower(*cp);
		}
	}
	clistat= status;
	return (boolean) status&1;
}

boolean
clipresent (param)
const char * param;
{
	int CLI$PRESENT();

	struct dsc$descriptor param_d= {0, 0, 0, 0};
	param_d.dsc$w_length= strlen(param);
	param_d.dsc$a_pointer= (char *) param;
	return (boolean) 1&CLI$PRESENT(&param_d);
}

void cmd_print()
{
	cli_buf_t buffer;

	if (istrue (getclistring ("p1", buffer))) print_stat (buffer);
	else print_stat (NULL);
}

void cmd_help()
{
static	const $DESCRIPTOR (helplib_d, "EMON$HELP");
	cli_buf_t buffer;
	struct dsc$descriptor buffer_d= {0, 0, 0, 0};
	unsigned status;
	vmsmsgbuf stsbuf;

	getclistring ("p1", buffer);
	buffer_d.dsc$a_pointer= buffer;
	buffer_d.dsc$w_length= strlen (buffer);
	emon$paste_display (&scroll_coord);
	emon$display_home (&scroll_coord);
	status= SMG$PUT_HELP_TEXT (&scroll_id,
				   &keybd_id,
				   &buffer_d,
				   &helplib_d);
	emon$unpaste_display (&scroll_coord);
	if isfalse(status) emon$print_error ("Error %s returned by help",
					      emon$translate_status(stsbuf,
								    status));
}

void cmd_spawn()
{
	cli_buf_t command;
	int status;
	int save_id;
	int LIB$SPAWN();
	struct dsc$descriptor command_d= {0, 0, 0, 0};

	getclistring ("command", command);
	SMG$SAVE_PHYSICAL_SCREEN (&paste_id, &save_id);
	SMG$SET_CURSOR_ABS (&error_id, &un, &un);
	set_outbands(oob_none);
	command_d.dsc$a_pointer= command;
	command_d.dsc$w_length= strlen (command);
	LIB$SPAWN (&command_d);
	if (command_d.dsc$w_length) {
		printf ("Type [Return] to return to EMON>");
		gets (command);
	}
	SMG$RESTORE_PHYSICAL_SCREEN (&paste_id, &save_id);
	set_outbands (oob_monitor);
}

void cmd_exit()
{
	mon_exit++;
}

#include chfdef

static 
unsigned int
debug_handler (sigarr,mecharr)
struct	chf$signal_array	*sigarr;
{
	char buf[257];
#pragma nostandard
	globalvalue int CLI$_FACILITY;
	globalvalue int LIB$_KEYNOTFOU;
#pragma standard
	struct dsc$descriptor buf_d;
	ushort len;
	ulong status;
	ulong sigid= sigarr->chf$l_sig_name;
	int SYS$GETMSG();

	if (sigid==LIB$_KEYNOTFOU) return SS$_CONTINUE;
	stop_monitor();
	stop_port(NULL);
	buf_d.dsc$a_pointer= buf;
	buf_d.dsc$w_length= sizeof buf -1;
	status= SYS$GETMSG (sigid,&len,&buf_d,0,0);
	if ( (status&1)==0) return status;
	buf[len]=0;
	emon$put_error(buf);
	if ( $VMS_STATUS_FAC_NO(sigid) != CLI$_FACILITY){
		sleep(1);
		return SS$_RESIGNAL;
	}
	else return SS$_CONTINUE;
}

main()
{
static	const $DESCRIPTOR (emon_prompt,"EMON>");
	void emoncmd();
	ulong	status;
	boolean init_stat();	/* init. config. */
	int	CLI$DCL_PARSE(), CLI$DISPATCH();
	int	init_screen();

	if (init_stat()){
		status= init_screen();
		if istrue (status) status= assign_port();
		if isfalse (status) {
			exit_status= status;
			mon_exit= true;
		}
		else{
			VAXC$ESTABLISH (debug_handler);
			set_outbands (oob_monitor);
		}
		while (!mon_exit){
			line_xpos= line_coord.posx;
			line_ypos= line_coord.posy;
			title_ypos= title_coord.posx;
			status= CLI$DCL_PARSE (0,emoncmd,
				               emon$get_input,
					       emon$get_input,
					       &emon_prompt);
			if istrue(status) {
				clear_filter_status();
				if istrue (line_coord.pasted)
					emon$unpaste_display (&line_coord);
				if istrue (scroll_coord.pasted)
					emon$unpaste_display (&scroll_coord);
				if istrue (title_coord.pasted)
					emon$unpaste_display (&title_coord);
				CLI$DISPATCH();
				emon$enable_buf (false);
				lock_monitor(false);
			}
		}

		SMG$ERASE_DISPLAY (&command_coord);
		emon$paste_display (&command_coord);
		SMG$SET_CURSOR_ABS (&command_id,&un,&un);
		SMG$DELETE_PASTEBOARD(&paste_id,&0);
	}
	else {
		printf("Error during initialization\n");
		exit_status= SS$_BADPARAM;
	}
	return exit_status;
}
