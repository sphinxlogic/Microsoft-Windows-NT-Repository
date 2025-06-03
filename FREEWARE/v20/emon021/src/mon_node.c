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

#include <stdio.h>

#include <descrip.h>

static struct {boolean delta;} mon_node_opt;

static mon_node_update()
{
	register circuit_entry *cp;
	node_name_t buf1,buf2;
	protocol_name_t buf3;
	ushort i;
static	const char mon_node_lined[]="%s%s%s%12d%9d.%2d ";
static	const char mon_node_linea[]="%s%s%s%12d%12d";
#define MON_NODE_LINE_SIZE 2*sizeof (node_name_t)+ sizeof (protocol_name_t)-3+2*12
	char buf[MON_NODE_LINE_SIZE+1];

	if (isfalse(upd_stat())) return;

	emon$begin_pasteboard_update ();
	status_update();
	emon$display_home (&line_coord);
	for (i=maxcircuit ,cp= tabcircuit; i-- ; cp++){
	   if istrue(mon_node_opt.delta)
		sprintf (buf,mon_node_lined,
			print_ethernet_addr (cp->srcptr->nodptr, &buf1),
			print_ethernet_addr (cp->dstptr->nodptr, &buf2),
			print_ethernet_protocol (cp->dstptr->prtptr, &buf3),
				cp->nbbytes.delta *interval / 8 ,
				cp->nbpackets.delta / interval,
				((cp->nbpackets.delta*100)/ interval) %100);
	   else
		sprintf (buf,mon_node_linea,
			print_ethernet_addr (cp->srcptr->nodptr, &buf1),
			print_ethernet_addr (cp->dstptr->nodptr, &buf2),
			print_ethernet_protocol (cp->dstptr->prtptr, &buf3),
				cp->nbbytes.total,
				cp->nbpackets.total);
	   emon$put_line (buf, MON_NODE_LINE_SIZE);
	}
	emon$end_pasteboard_update ();
	emon$flush_buffer();
}

boolean
cmd_monitor_node()
{
	ethernet_addr_t node_addr;
	protocol_entry 	*proto;
	node_entry 	*node, *none;
	int		stat_flags;

	(void) emon$enable_buf(true);

	if isfalse (get_filter(&node, &none, &proto, &stat_flags))
			return false;

	mon_node_opt.delta= isfalse(clipresent("total"));

	if (clipresent ("listen")) 
			init_loc_stat (NULL, node, proto, NULL, stat_flags);
	else if (clipresent ("talk"))
			init_loc_stat (node, NULL, proto, NULL, stat_flags);
	else 		init_loc_stat (node, NULL, proto, NULL, stat_flags | LOC_STAT_BOTHWAYS);

	if (clipresent("initialize")){
		emon$erase_display (&line_coord);
		stat_reset_circuit_database();
	}

	if istrue (mon_node_opt.delta)
		emon$title 
("             From               To    Protocol       Bit/s      Pkts/s");
	else    emon$title
("             From               To    Protocol       Bytes        Pkts");

	emon$paste_display (&line_coord);

	init_monitor (mon_node_update);

	monitor (loc_stat);

}
