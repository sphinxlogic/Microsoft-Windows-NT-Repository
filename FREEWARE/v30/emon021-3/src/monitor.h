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
#include "types"
#include "emon.h"
#pragma nostandard
noshare ulong
	paste_id,
	keybd_id,
	status_id,
	title_id,
	text_id,
	display_id,
	param_id,
	tab_id,
	scroll_id,
	line_id,	/* array status */
	error_id,	/* error messages */
	command_id;	/* command line */

typedef struct {long posx,posy,lenx,leny;
		ulong id;
		boolean pasted;} display_t;

extern display_t 
	status_coord,
	title_coord,
	text_coord,
	display_coord,
	scroll_coord,
	param_coord,
	tab_coord,
	line_coord,
	error_coord,
	command_coord;

extern	ulong	recall_size;

noshare ulong	line_xpos_init,
	line_ypos_init,
	error_xpos_init;

noshare long	line_xpos;	/* line_id horizontal position */
noshare long	line_ypos;	/* line_id vertical position */

ulong	const un=1;

noshare ubyte	mon_stop;
noshare ubyte   mon_exit;
noshare int	interval = 8;	/* intervalle d'echantillonnage */

noshare ulong	pb_rows,	/* nb de lignes du terminal	*/
		pb_columns;	/* nb de colonnes du terminal	*/
extern	boolean	screen_inited;

boolean lock_monitor (boolean locked);	/* do not allow collection stop */
void	init_monitor( boolean (* status_routine)() );
void	abort_monitor(char * reason);	  /*	stop retour niveau commande	*/
void	exit_monitor (ulong status);	/* sortie programme avec status */
void	monitor	( boolean (* statistics_routine)() );/*	do monitor	*/
void	status_update(void);			/* update status display */

#define MAXCLILEN 64

typedef	char cli_buf_t[MAXCLILEN];

boolean	getclistring (const char * parametre, cli_buf_t valeur);
boolean	clipresent (const char * parametre);

/* conversion : nom de protocole -> pointeur vers table et valeur hex */
protocol_entry * find_protocol (protocol_name_t, ethernet_protocol_t *);
/* conversion : nom de noeud -> pointeur vers table et valeur hex */
node_entry * find_node (node_name_t, ethernet_addr_t *);
/* retourne les valeurs a appliquer au filtage de trames */
boolean get_filter (node_entry **, node_entry **, protocol_entry **, int *);

void emon$put_error (const char * mess);	/* error message */
void emon$print_error (const char * ctrl, ...);	/* error message */
void emon$put_more_error (const char * mess);	/* complementary error messages */
void emon$put_cerror (const char * mess);	/* C error: uses errno & vaxc$errno */

typedef char vmsmsgbuf[40];
char * emon$translate_status (vmsmsgbuf retbuf, unsigned long status);

void	emon$title (const char * intitule);	/* title for status display */

void	emon$paste_display (display_t * coord);
void	emon$unpaste_display (display_t * coord);
boolean	emon$enable_buf (boolean buf_enabled);
void	emon$erase_display (display_t * coord);
void	emon$flush_buffer (void);
void	emon$put_with_scroll (char * buffer, int length); /* line in scroll display */
void	emon$put_line (char * buffer, int length); /* line in line display */
void	emon$display_home (display_t * coord);
void	emon$begin_display_update (display_t * coord);
void	emon$end_display_update (display_t * coord);
void	emon$begin_pasteboard_update(void);
void	emon$end_pasteboard_update(void);
#pragma standard
