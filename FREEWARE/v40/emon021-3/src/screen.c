/*									*/
/*	Copyright (©) Ecole Nationale Supérieure des Télécommunications */
/*									*/
/*	EMON: moniteur ethernet	(V2.1-3)				*/
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
#include "monitor"

#include <descrip.h>
#include <smgdef.h>

int	SMG$CREATE_PASTEBOARD(), SMG$CREATE_VIRTUAL_DISPLAY(),
	SMG$PUT_LINE(),SMG$SET_CURSOR_ABS(),
	SMG$CREATE_VIRTUAL_KEYBOARD(), SMG$SET_KEYPAD_MODE();
int	LIB$SIGNAL();

#define xposinit 6
#define yposinit 1
#define errposinit 23
#define maxerrlines 5
#define RECALL_SIZE 20

boolean	screen_inited;

display_t	/* linepos, colpos, lines, cols, id, pasted */
	status_coord= 	{ 1, 1, 4,132, 0, false},
	title_coord= 	{ 5, 1, 1,132, 0, false},
	text_coord=   	{ 6, 1,15,20, 0, false},
	display_coord=	{ 6,21,17,132, 0, false},
	scroll_coord= 	{ 6, 1,17,132, 0, false},
	param_coord=	{ 1, 2,23,79, 0, false},
	tab_coord=	{15, 1, 9,80, 0, false},
	line_coord=	{ xposinit, yposinit,40,320, 0, false},
	error_coord=	{ errposinit, 1, maxerrlines,80, 0, false},
	command_coord=	{24, 1, 1,132, 0, false};
ulong recall_size= RECALL_SIZE;

static void
emon$$create_display (ulong * display_id, display_t * coord)
{
	int status;

	status= SMG$CREATE_VIRTUAL_DISPLAY (&coord->lenx, &coord->leny, 
					    &coord->id);
	*display_id= coord->id;
	if isfalse (status) {
		vmsmsgbuf stsbuf;
		emon$print_error ("Emon error %s returned by CREATE_PASTE",
				  emon$translate_status (stsbuf, status) );
		LIB$SIGNAL (status);
	}
}

init_screen()
{
static	const $DESCRIPTOR(stat_1,
	"ethernet  blocks     buffer errors     data    ----failures----");
static	const $DESCRIPTOR(stat_2,
	"bit rate received  user system device overrun receive send C.D.");
static	const $DESCRIPTOR(cir_brc,"Ext. bytes received");
static	const $DESCRIPTOR(lin_dbr,"     Blocks received");
static	const $DESCRIPTOR(lin_mbr,"Mlt. blocks received");
static	const $DESCRIPTOR(lin_brc,"     Bytes received");
static	const $DESCRIPTOR(lin_mrc,"Mlt. bytes received");
static	const $DESCRIPTOR(lin_dbs,"     Blocks sent");
static	const $DESCRIPTOR(lin_mbs,"Mlt. blocks sent");
static	const $DESCRIPTOR(lin_bsn,"     Bytes sent");
static	const $DESCRIPTOR(lin_msn,"Mlt. bytes sent");
static	const $DESCRIPTOR(lin_lrc,"Loc. bytes received");
static	const kpd_mode= SMG$M_KEYPAD_APPLICATION;
	unsigned long status;
	long	data_display_posx;

	status= SMG$CREATE_PASTEBOARD (&paste_id,0,&pb_rows,&pb_columns);
	if isfalse(status) return status;

	/* on cree d'abord error_id pour pouvoir afficher tous les messages */
	/* attention: on ne mappe qu'une ligne en bas d'écran pour error */

	error_coord.posx = pb_rows - command_coord.lenx - 1;
	error_coord.leny = pb_columns;
	error_xpos_init = error_coord.posx;
	emon$$create_display (&error_id, &error_coord);

	status_coord.leny= pb_columns;
	emon$$create_display (&status_id, &status_coord);
	emon$paste_display (&status_coord);

	status= SMG$SET_CURSOR_ABS (&status_id,&un,&un);
	if isfalse(status) return status;
	status= SMG$PUT_LINE (&status_id,&stat_1);
	if isfalse(status) return status;
	status= SMG$PUT_LINE (&status_id,&stat_2);
	if isfalse(status) return status;


	title_coord.posx= status_coord.posx+ status_coord.lenx;
	emon$$create_display (&title_id, &title_coord);

	data_display_posx= title_coord.posx+ title_coord.lenx;

	text_coord.posx= data_display_posx;
	text_coord.leny= pb_columns;
	text_coord.lenx= pb_rows - command_coord.lenx- data_display_posx;
	emon$$create_display (&text_id, &text_coord);

	emon$display_home (&text_coord);
	status= SMG$PUT_LINE (&text_id,&cir_brc);
	if isfalse(status) return status;
	status= SMG$PUT_LINE (&text_id,&lin_dbr);
	if isfalse(status) return status;
	status= SMG$PUT_LINE (&text_id,&lin_mbr);
	if isfalse(status) return status;
	status= SMG$PUT_LINE (&text_id,&lin_dbs);
	if isfalse(status) return status;
	status= SMG$PUT_LINE (&text_id,&lin_mbs);
	if isfalse(status) return status;
	status= SMG$PUT_LINE (&text_id,&lin_brc);
	if isfalse(status) return status;
	status= SMG$PUT_LINE (&text_id,&lin_mrc);
	if isfalse(status) return status;
	status= SMG$PUT_LINE (&text_id,&lin_bsn);
	if isfalse(status) return status;
	status= SMG$PUT_LINE (&text_id,&lin_msn);
	if isfalse(status) return status;
	status= SMG$PUT_LINE (&text_id,&lin_lrc);
	if isfalse(status) return status;

	display_coord.posx= data_display_posx;
	display_coord.leny= pb_columns;
	display_coord.lenx= pb_rows - status_coord.lenx;
	emon$$create_display (&display_id, &display_coord);

	line_coord.posx = data_display_posx;
	emon$$create_display (&line_id, &line_coord);

	scroll_coord.leny= pb_columns;
	scroll_coord.lenx= pb_rows - (data_display_posx-1);
	emon$$create_display (&scroll_id, &scroll_coord);

	emon$$create_display (&tab_id, &tab_coord);

	emon$$create_display (&param_id, &param_coord);

	command_coord.posx = pb_rows - command_coord.lenx;
	command_coord.leny = pb_columns;
	emon$$create_display (&command_id, &command_coord);

	status= SMG$CREATE_VIRTUAL_KEYBOARD(&keybd_id, 0, 0, 0, &recall_size);
	if isfalse(status) return status;
	status= SMG$SET_KEYPAD_MODE (&keybd_id, &kpd_mode);
	if isfalse(status) return status;
	emon$enable_buf(false);
	screen_inited= true;

	return status;
}
