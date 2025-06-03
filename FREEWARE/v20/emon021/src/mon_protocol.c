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
#include "monitor"
#include "stat"
#include "record"

#if defined (__DECC) || defined (__STDC__)
#include <stdlib.h>
#endif

#include <descrip.h>
#include <ssdef.h>

int	SMG$SET_CURSOR_ABS(),
	SMG$PUT_LINE(),
	SMG$ERASE_DISPLAY();

static rec_monprotocol_t * rec;
static	float * peak_proto;	/* valeurs de pointe des protocoles */

static	counter_t	totbytes, totpackets;

static mon_update()
{
	register protocol_entry *pp;
	protocol_name_t bufp;
	ushort i;
static  const char mon_line[]="%s%8d%3d%%%6d%3d%%%12d%3d%%%12d%3d%%%5.2f%5.2f";
#define MON_LINE_SIZE sizeof (protocol_name_t)-1 +8+6+4+4+12+12+4+4+5+5
	char buf[MON_LINE_SIZE+1];
static	struct dsc$descriptor buf_d= {MON_LINE_SIZE, 0, 0, NULL};
	int SYS$GETTIM();

	if (isfalse(upd_stat())) return;	/* update stats */

	buf_d.dsc$a_pointer= buf;

	emon$begin_pasteboard_update ();	/* begin display update */
	status_update();			/* status line update */

	/* account for total packets/bytes received during sampling */

	totbytes.delta= 0;
	totpackets.delta= 0;
	totbytes.total= 0;
	totpackets.total= 0;
	for (i=maxprotocol ,pp= tabprotocol; i-- ; pp++){
		totbytes.delta+= pp->nbbytes.delta;
		totpackets.delta+= pp->nbpackets.delta;
		totbytes.total+= pp->nbbytes.total;
		totpackets.total+= pp->nbpackets.total;
	}

	if (totpackets.delta==0) {	/* eviter un divide by 0 */
		totpackets.delta= 1;
		totbytes.delta= 1;
		if (totpackets.total==0) {
			totpackets.total= 1;
			totbytes.total= 1;
		}
	}

	/* on affiche le bebe */

	SMG$SET_CURSOR_ABS (&line_id,&un,&un);
	for (i=0 ,pp= tabprotocol; i < maxprotocol ; pp++, i++)
/*		impression seulement si le protocole a vu des paquets */
		if (pp->nbpackets.total)
			if istrue(recording) {
				SYS$GETTIM (&rec->header.time);
				rec->dbyte= pp->nbbytes.delta;
				rec->dpacket= pp->nbpackets.delta;
				rec->proto= pp->protocol;
				write_record (rec, sizeof *rec);
			}

			else{
				float bw;

				bw= (pp->nbbytes.delta+ETHERNET_LINK_LAYER_OVERHEAD*pp->nbpackets.delta)/1E6;
				if (bw>peak_proto[i]) peak_proto[i]= bw;
				if (peak_proto[i] > 10.0) 
					abort_monitor ("après upd_stat");

				sprintf (buf,mon_line,
					print_ethernet_protocol (pp, &bufp),

					pp->nbbytes.delta,
					(100*pp->nbbytes.delta)/totbytes.delta,
					pp->nbpackets.delta,
					(100*pp->nbpackets.delta)/totpackets.delta,

					pp->nbbytes.total,
					(pp->nbbytes.total>40000000)?
						pp->nbbytes.total/(1+totbytes.total/100):
						(100*pp->nbbytes.total)/totbytes.total,

					pp->nbpackets.total,
					(pp->nbpackets.total>40000000)?
						pp->nbpackets.total/(1+totpackets.total/100):
						(100*pp->nbpackets.total)/totpackets.total,
					bw,
					peak_proto[i]
				);
				SMG$PUT_LINE (&line_id,&buf_d);
			}
	emon$end_pasteboard_update ();
	emon$flush_buffer();
}

boolean
cmd_monitor_protocol()
{
	node_entry * from, * to;
	protocol_entry * proto;
	int statflags;

	cli_buf_t record_file;
	int i;

	if (getclistring ("record",record_file))
		if isfalse (start_record(record_file)) return false;
		else{
			rec= malloc (sizeof *rec);
			rec->header.type= REC_MONPROTOCOL;
			rec->header.version= 1;
		}

	if (clipresent ("initialize")){
		SMG$ERASE_DISPLAY (&line_id);
	}

	peak_proto= malloc (protocol_nb*sizeof *peak_proto);
	for (i=0; i<protocol_nb; i++) peak_proto[i]= 0;

	(void) emon$enable_buf(true);
	emon$paste_display (&line_coord);
	emon$title 
 ("protocol name   bytes -REL- pkts          bytes   -ABS-    pkts    Mb/S peak");
	if istrue (get_filter (&from, &to, &proto, &statflags)) {
		init_monitor (mon_update);
		if ( (from==0) && (to==0) && (proto==0) ) monitor (glob_stat);
		else {
			init_loc_stat (from, to, proto, 0, statflags);
			monitor (loc_stat);
		}
	}

	free (peak_proto);

	if istrue(recording) {
		stop_record();
		free (rec);
	}
}
