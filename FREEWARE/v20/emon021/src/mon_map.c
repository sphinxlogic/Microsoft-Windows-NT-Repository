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
#include "monitor.h"
#include "stat.h"

#include <stdio.h>
#include <string.h>

#if defined (__STDC__) || defined (__DECC)
#include <stdlib.h>
#endif

#include <descrip.h>

int	SMG$SET_CURSOR_ABS(),
	SMG$INSERT_CHARS(),
	SMG$PUT_CHARS();

#define	MAP$M_SEND	01
#define	MAP$M_RECEIVE	02

static counter_t * monnodetraf;
static boolean mon_total, mon_packets;

static
const struct dsc$descriptor *
map_name(map_num)
int map_num;
{
static	char buf[9];
static	const struct dsc$descriptor buf_d={sizeof buf -1, 0, 0, buf};
	if (tabmap[map_num].name[0])
		sprintf (buf,"%8.8s",tabmap[map_num].name);
	else sprintf (buf,"map #%-3d", map_num);
	return &buf_d;
}

static
const struct dsc$descriptor *
local_name(seq_num)
int seq_num;
{
	return map_name(tabmap[seq_num].remap);
}

static local_update()
{
	register circuit_entry * cp= tabcircuit;
	register int isend, irecv, i;
	int	numline;	/* no register since deferencedJpassed by addr*/
	int max_used;
	register counter_t * ap;

	if (isfalse(upd_stat())) return 0;

	emon$begin_pasteboard_update();
	status_update();

	for (ap=monnodetraf,isend=0; isend<max_map; isend++)
		if (tabmap[isend].flags && MAP$M_SEND){
			int ireceive;
			for (ireceive=max_map; ireceive--; ap++->delta=0);
		}
		else ap+= max_map;

	for (cp, i= maxcircuit; i--; cp++){
		ushort sm= cp->srcptr->nodptr->map_num;
		ushort dm= cp->dstptr->nodptr->map_num;
		if ( (sm>=max_map) || (dm>=max_map) ){
			emon$put_error("Invalid node number found");
			return 0;
		}
		tabmap[sm].flags|= MAP$M_SEND;
		tabmap[dm].flags|= MAP$M_RECEIVE;
		ap= monnodetraf+dm+max_map*sm;
		if (istrue(mon_packets)) ap->delta+= cp->nbpackets.delta;
		else			 ap->delta+= cp->nbbytes.delta;
	}

	for (ap=monnodetraf,isend=0; isend<max_map ; isend++)
	{
		if (tabmap[isend].flags & MAP$M_SEND){
			int ireceive;
			for (ireceive=max_map; ireceive--; ap++)
					ap->total+= ap->delta;
		} else ap+= max_map;
	}

	for (i=0,max_used=0; i<max_map; i++)
		if (tabmap[i].flags&&(MAP$M_SEND|MAP$M_RECEIVE) )
					tabmap[max_used++].remap= i;

	for (isend=0, numline=1; isend++<=max_used;) {
static 		char buf[9];
static 		struct dsc$descriptor buf_d={sizeof buf -1,0,0, buf};
		register counter_t * ip= monnodetraf+max_map*tabmap[(isend-2)].remap;

		SMG$SET_CURSOR_ABS (&line_id, &numline, &un);
		if (isend==1){
static 			const $DESCRIPTOR (from_to,"From\\to ");
			SMG$PUT_CHARS (&line_id,&from_to);
			for (irecv=0; irecv<max_used; irecv++)
				if (!(tabmap[irecv].flags & MAP$M_RECEIVE) ) continue;
				else SMG$PUT_CHARS (&line_id,local_name(irecv));
			numline++;
			continue;
		}
		if (!(tabmap[isend-2].flags & MAP$M_SEND)) continue;
		numline++;
		SMG$PUT_CHARS (&line_id,local_name(isend-2));
		for (irecv=0; irecv<max_used; irecv++){
			if (!(tabmap[irecv].flags & MAP$M_RECEIVE) ) continue;
			if (istrue(mon_total))
			     sprintf (buf,"%8d",ip[tabmap[irecv].remap].total);
			else sprintf (buf,"%8d",ip[tabmap[irecv].remap].delta);
			SMG$PUT_CHARS (&line_id,&buf_d);
		}
	}
	emon$end_pasteboard_update();
	emon$flush_buffer();
}

cmd_showmap()
{
	node_entry * np;
	int nbnode;
	int num_map;
	static const posnod= 9;

	emon$begin_pasteboard_update ();
	emon$erase_display (&line_coord);
	emon$paste_display (&line_coord);
	for (np= tabnode, nbnode= maxnode; nbnode--; np++){
		node_name_t bufn;
		ulong line_number;

static		struct dsc$descriptor bufn_d= {8,0,0, NULL};

		bufn_d.dsc$a_pointer= bufn+sizeof bufn-9;
		print_ethernet_addr (np, &bufn);
		line_number= np->map_num + 1;
		SMG$INSERT_CHARS (&line_id, &bufn_d, &line_number, &posnod);
		SMG$PUT_CHARS (&line_id,map_name(np->map_num),0,&un);
	}
	emon$end_pasteboard_update ();
}

cmd_map()
{
	cli_buf_t buf;
	ulong cli_map_num;
	ethernet_addr_t node_addr;

	getclistring ("line",buf);
	cli_map_num=atol (buf);
	if ( (cli_map_num<0) || (cli_map_num>=max_map) ){
		emon$put_error("Invalid map number");
		return 0;
	}
	getclistring ("node",buf);
	if (find_node(buf,&node_addr))
		addnode(&node_addr)->map_num= cli_map_num;
	else
		emon$put_error("Node not found");

}

cmd_monitor_map()
{
	node_entry *from, *to;
	protocol_entry * proto;
	int stat_flags;
	int tabsize= max_map*max_map*sizeof (counter_t);

	(void) emon$enable_buf(true);
	monnodetraf= malloc (tabsize);
	memset (monnodetraf,0,tabsize);

	if (clipresent ("initialize")) emon$erase_display (&line_coord);

	emon$paste_display (&line_coord);
	init_monitor(local_update);

	mon_total= clipresent ("total");
	mon_packets= clipresent ("packets");

	if istrue (get_filter(&from, &to, &proto, &stat_flags))
		if (proto==0) monitor (glob_stat);
		else {	/* from & to interdits dans le .cld */
			init_loc_stat (0,0,proto,0,0);
			monitor (loc_stat);
		}

	free (monnodetraf);
}
