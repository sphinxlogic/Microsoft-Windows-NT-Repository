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

#include <string.h>
#if defined(__DECC) || defined (__STDC__)
#include <stdlib.h>
#endif

#include <assert.h>
#include <descrip.h>
#include <ssdef.h>

int	SMG$PUT_LINE();

#define DEBUG

static	rec_monprotocol_t * rec;/* record structure */
static	float * peak_xcast;	/* valeurs de pointe des multicast (Kb/s) */
static	float peak_bw;		/* peak bandwidth (including inter-gap) */

static	duration;		/* time since this program was started */
static	node_name_t total1;
static	protocol_name_t total2;

	/* routine called every sample end */

static mon_update()
{
	register source_entry *sp;
	protocol_entry * pp;
	node_name_t bufn;
	protocol_name_t bufp;
	ushort i;
static  const char mon_line[]=
		"%s%s%8.1f%3d%%%5.0f%3d%%%7.0f%3d%%%7.0f%3d%%%8.1g%7.2f";
static  const char mon_line1[]=	/* small values */
		"%s%s%8.1f%3d%%%5.1f%3d%%%7.1g%3d%%%7.1g%3d%%%8.1g%7.2f";
#define MON_LINE_SIZE sizeof (node_name_t)-1 + sizeof (protocol_name_t) -1\
		 +8+3+1+6+2+1+8+2+1+8+2+1+8+7

	char buf[MON_LINE_SIZE+100];
static	struct dsc$descriptor buf_d= {MON_LINE_SIZE,0,0,NULL};
	counter_t	totbytes, totpackets, totxbytes, totxpackets;
	float xbw;
	int	SYS$GETTIM();

	if (isfalse(upd_stat())) return;	/* update stats */

	buf_d.dsc$a_pointer= buf;

	emon$begin_pasteboard_update ();	/* begin display update */
	status_update();			/* status line update */

	/* account for total packets/bytes received during sampling */

	duration += interval;

	totbytes.delta= 0;	/* total data packets/bytes delta/total */
	totpackets.delta= 0;
	totbytes.total= 0;
	totpackets.total= 0;

	totxbytes.delta= 0;	/* total xcst packets/bytes delta/total */
	totxpackets.delta= 0;
	totxbytes.total= 0;
	totxpackets.total= 0;
	xbw =0;

		/* sum all the bytes/packets delta/total received */

	for (i=maxprotocol ,pp= tabprotocol; i-- ; pp++){
		totbytes.delta+= pp->nbbytes.delta;
		totpackets.delta+= pp->nbpackets.delta;
		totbytes.total+= pp->nbbytes.total;
		totpackets.total+= pp->nbpackets.total;
	}

		/* ensure a nice display if no packets received */

	if (totpackets.delta==0) {
		if (totpackets.total==0) totpackets.total=1;
		totpackets.delta=1;
		if (totbytes.total==0) totbytes.total=1;
		totbytes.delta=1;
	}
		
	/* on affiche le bebe */

	emon$display_home (&line_coord);	/* start of display */

	for (i=0 ,sp= tabsource; i < maxsource ; sp++, i++) {
		if (isfalse(sp->addr.bytes[0])) continue; /* pas multicast */
/*		impression seulement si le protocole a vu des paquets */
		if (sp->nbrpackets.total)
			if istrue(recording) {
				SYS$GETTIM (&rec->header.time);
				rec->dbyte= sp->nbrbytes.delta;
				rec->dpacket= sp->nbrpackets.delta;
				write_record (rec, sizeof *rec);
			}

			else{
				float bw;	/* bandwidth en Kb/s */
				float avepkt= (float) sp->nbrpackets.total / duration;

				totxbytes.delta+= sp->nbrbytes.delta;
				totxpackets.delta+= sp->nbrpackets.delta;
				totxbytes.total+= sp->nbrbytes.total;
				totxpackets.total+= sp->nbrpackets.total;

				bw= (sp->nbrbytes.delta+ETHERNET_LINK_LAYER_OVERHEAD*sp->nbrpackets.delta)/1E3;
				xbw += bw;
				if (bw>peak_xcast[i]) peak_xcast[i]= bw;
				if (peak_xcast[i] > 1e4) 
					abort_monitor ("après upd_stat");

				sprintf (buf, (avepkt>=10) ? mon_line : mon_line1, 
					print_ethernet_addr (sp->nodptr, &bufn),
					print_ethernet_protocol (sp->prtptr, &bufp),
					(float)sp->nbrbytes.delta/ interval,
					(100*sp->nbrbytes.delta)/totbytes.delta,
					(float)sp->nbrpackets.delta / interval,
					(100*sp->nbrpackets.delta)/totpackets.delta,
					(float) sp->nbrbytes.total / duration,
					(sp->nbrbytes.total>40000000)
						? sp->nbrbytes.total/(1+totbytes.total/100)
						: (100*sp->nbrbytes.total)/totbytes.total,
					avepkt,
					(sp->nbrpackets.total>40000000)
						? sp->nbrpackets.total/(1+totpackets.total/100)
						: (100*sp->nbrpackets.total)/totpackets.total,
					bw,
					peak_xcast[i]
				);
#ifdef DEBUG
		assert (strlen(buf) <= MON_LINE_SIZE);
#endif
				SMG$PUT_LINE (&line_id, &buf_d);
			}
		}

	if (xbw > peak_bw) peak_bw= xbw;
	sprintf (buf, mon_line, total1, total2, 
			(float) totxbytes.delta / interval,
			(100*totxbytes.delta) / totbytes.delta,
			(float)totxpackets.delta / interval,
			(100*totxpackets.delta)/totpackets.delta,
			(float) totxbytes.total / duration,
			(totxbytes.total>40000000)
				? totxbytes.total/(1+totbytes.total/100)
				: (100*totxbytes.total)/totbytes.total,	
			(float) totxpackets.total / duration,
			(totxpackets.total>40000000)
				? totxpackets.total/(1+totpackets.total/100)
				: (100*totxpackets.total)/totpackets.total,
			xbw,
			peak_bw
	);

#ifdef DEBUG
	assert (strlen(buf) <= MON_LINE_SIZE);
#endif

	SMG$PUT_LINE (&line_id, &buf_d);
	emon$end_pasteboard_update();
	emon$flush_buffer();
}

boolean
cmd_monitor_multicast()
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
			rec->header.type= REC_MONMULTICAST;
			rec->header.version= 1;
		}

	if (clipresent ("initialize")){
		emon$erase_display (&line_coord);
	}

	/* alloc & zero an array parallel to the source array */

	peak_xcast= malloc (source_nb*sizeof *peak_xcast);
	for (i=0; i<source_nb; i++) {
		peak_xcast[i]= 0;
		tabsource[i].nbrbytes.total= 0;
		tabsource[i].nbrpackets.total= 0;
	}

	duration= 0;
	peak_bw= 0;
	sprintf (total1, "%*s", sizeof total1 -1, "----- Multicast ");
	sprintf (total2, "%-*s", sizeof total2 -1, "Total -----");

#ifdef DEBUG
	assert (strlen(total1) < sizeof total1);
	assert (strlen(total2) < sizeof total2);
#endif

	(void) emon$enable_buf(true);
	emon$paste_display (&line_coord);
	emon$title 
("  Multicast addr.    Protocol  CUR by/s %   pk/s % AVE by/s %    pk/s  %   Kb/s   peak");
	if istrue (get_filter (&from, &to, &proto, &statflags)) {
		init_monitor (mon_update);
		if ( (from==NULL) && (to==NULL) && (proto==NULL) ) monitor (glob_stat);
		else {
			init_loc_stat (from, to, proto, NULL, statflags);
			monitor (loc_stat);
		}
	}

	free (peak_xcast);

	if istrue(recording) {
		stop_record();
		free (rec);
	}
}
