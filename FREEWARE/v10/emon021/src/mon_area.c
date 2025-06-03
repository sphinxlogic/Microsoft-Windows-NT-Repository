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
#include <string.h>

#if defined (__DECC) || defined (_STDC_)
#include <stdlib.h>
#endif

#include <descrip.h>
int	SMG$BEGIN_PASTEBOARD_UPDATE(),
	SMG$END_PASTEBOARD_UPDATE(),
	SMG$FLUSH_BUFFER(),
	SMG$SET_CURSOR_ABS(),
	SMG$PUT_CHARS(),
	SMG$ERASE_DISPLAY(),
	SMG$PUT_LINE();

static struct select_t {ubyte used,remap;} * tabusednode;
static counter_t * monnodetraf;
static ushort area_mon;
static ethernet_addr_t decnet_addr;
static boolean mon_total, mon_packets;

static const struct dsc$descriptor_s *
local_name(seq_num)
int seq_num;	/* rang du noeud a trouver */
{
static	char buf[9];
static  const struct dsc$descriptor_s buf_d={sizeof buf-1,0,0,buf};
static 	const $DESCRIPTOR (other_desc,"Other   ");
	if (seq_num==0) return &other_desc;
	else{
		node_name_t bufn;
		decnet_addr.decnet.node= tabusednode[seq_num].remap;
		print_ethernet_addr(addnode(&decnet_addr), &bufn);
		sprintf (buf,"%-8.8s",bufn+sizeof bufn-sizeof buf);
		return &buf_d;
	}	
}

static boolean
local_update()
{
	register circuit_entry * cp= tabcircuit;
	int	i;	/* cannot register, passed by address */
	register int j;
	int max_used;
	register counter_t * ap;

	if (isfalse(upd_stat())) return false;

	SMG$BEGIN_PASTEBOARD_UPDATE(&paste_id);
	status_update();

	for (ap=monnodetraf,i=0; i<max_addr; )
	{
		if (tabusednode[i++].used)
			for (j=max_addr; j--; ap++->delta=0);
		else ap+= max_addr;
	}

	for (cp, i= maxcircuit; i--; cp++){
		ushort sn=((cp->source_addr.decnet.header==DECNET_HEADER) && (cp->source_addr.decnet.area==area_mon))? 
			cp->source_addr.decnet.node : 0;
		ushort dn=((cp->dest_addr.decnet.header==DECNET_HEADER) && (cp->dest_addr.decnet.area==area_mon))?
			cp->dest_addr.decnet.node : 0;
		if ( (sn>=max_addr) || (dn>=max_addr) ){
			emon$put_error("Invalid node number found");
			return false;
		}
		tabusednode[sn].used= 1;
		tabusednode[dn].used= 1;
		ap= monnodetraf+dn+max_addr*sn;
		if (istrue(mon_packets)) ap->delta+= cp->nbpackets.delta;
		else			 ap->delta+= cp->nbbytes.delta;
	}

	for (ap=monnodetraf,i=0; i<max_addr ; ap+= max_addr)
	{
		register int j;
		register counter_t * a2p;
		if (tabusednode[i++].used)
			for (j=max_addr, a2p= ap; j--; a2p++)
				a2p->total+= a2p->delta;
	}

	for (i=0,max_used=0; i<max_addr; i++)
		if (tabusednode[i].used) tabusednode[max_used++].remap= i;

	for (i=0 ; i++<=max_used;) {
		static char buf[9];
		static struct dsc$descriptor buf_d={sizeof buf -1,0,0,buf};
		register counter_t * ip= monnodetraf+max_addr*tabusednode[i-2].remap;
		SMG$SET_CURSOR_ABS (&line_id, &i, &un);
		if (i==1){
			static const $DESCRIPTOR (from_to,"From\\to ");
			register j;
			SMG$PUT_CHARS (&line_id,&from_to);
			for (j=0; j<max_used; j++)
				SMG$PUT_CHARS (&line_id,local_name(j));
			continue;
		}
		SMG$PUT_CHARS (&line_id,local_name(i-2));
		for (j=0; j<max_used; j++){
			if (istrue(mon_total))
			     sprintf (buf,"%8d",ip[tabusednode[j].remap].total);
			else sprintf (buf,"%8d",ip[tabusednode[j].remap].delta);
			SMG$PUT_CHARS (&line_id,&buf_d);
		}
	}
	SMG$END_PASTEBOARD_UPDATE(&paste_id);
	SMG$FLUSH_BUFFER (&paste_id);
}

boolean
cmd_monitor_area()
{
	int tabsize= max_addr*(sizeof (struct select_t)+ 
			      max_addr*sizeof (counter_t));
	cli_buf_t buf;
	node_entry * from, * to;
	protocol_entry * proto;
	int flags;

	(void) emon$enable_buf (true);

	getclistring ("area_nb",buf);
	area_mon= atol (buf);
	if ( (area_mon <= 0) || (area_mon >= area_nb)) {
		emon$put_error ("Invalid area number");
		return false;
	}

	decnet_addr.decnet.header= DECNET_HEADER;
	decnet_addr.decnet.area= area_mon;

	if (clipresent ("initialize")) SMG$ERASE_DISPLAY (&line_id);
	tabusednode= malloc (tabsize);
	monnodetraf= (counter_t *)
		    ((char *) tabusednode) + max_addr*sizeof (struct select_t);
	memset (tabusednode,0,tabsize);

	emon$paste_display (&line_coord);
	init_monitor(local_update);

	mon_total= clipresent ("total");
	mon_packets= clipresent ("packets");

	if (istrue (get_filter(&from, &to, &proto, &flags)))
		if (proto==0) monitor (glob_stat);
		else {	/* from et to interdits par le CLD */
			init_loc_stat (0,0,proto,0,0);
			monitor (loc_stat);
		}

	free (tabusednode);
}
