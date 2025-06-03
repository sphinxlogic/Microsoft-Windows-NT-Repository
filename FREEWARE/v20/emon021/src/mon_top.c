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

int	SMG$BEGIN_PASTEBOARD_UPDATE(),
	SMG$END_PASTEBOARD_UPDATE(),
	SMG$FLUSH_BUFFER(),
	SMG$PUT_CHARS(),
	SMG$PUT_LINE(),
	SMG$SET_CURSOR_ABS(),
	SMG$SET_CURSOR_REL();
	
static
boolean
mon_top_update()
{

	static const char circuit_line[]="%2d%% %s%s%s%8lu%6lu";
#define	CIRCUIT_LINE_SIZE \
		2*(sizeof (node_name_t) -1) + sizeof (protocol_name_t)-1+18
	ulong totbytes,totpackets,maxbytes,maxpackets;
	char buf[CIRCUIT_LINE_SIZE+1];
static	struct dsc$descriptor buf_d= {CIRCUIT_LINE_SIZE, 0, 0, NULL};
	protocol_name_t bufp;
	node_name_t bufs,bufd;
	register int i;
	int numline=1;

	if (isfalse(upd_stat())) return false;

	SMG$BEGIN_PASTEBOARD_UPDATE(&paste_id);
	status_update();

	buf_d.dsc$a_pointer= buf;

	/*** top protocol ***/
	{
		register protocol_entry * pp, *ppp, *ppb;

		totbytes= 0;
		totpackets= 0;
		maxbytes= 0;
		maxpackets= 0;

		for (pp= tabprotocol; pp < tabprotocol+maxprotocol; pp++){
			if (pp->nbbytes.delta >= maxbytes){
				maxbytes= pp->nbbytes.delta;
				ppb= pp;
			}
			if (pp->nbpackets.delta >= maxpackets){
				maxpackets= pp->nbpackets.delta;
				ppp= pp;
			}
			totbytes   += pp->nbbytes.delta;
			totpackets += pp->nbpackets.delta;
		}

		if (totbytes==0) {
			totbytes++;	/* éviter un divide by 0 */
			totpackets++;	/* éviter un divide by 0 */
		}

		sprintf (buf,circuit_line,
				(100*ppb->nbbytes.delta)/totbytes,
				blank_node_name,
				blank_node_name,
				print_ethernet_protocol(ppb, &bufp),
				ppb->nbbytes.delta,
				ppb->nbpackets.delta/8);
		SMG$PUT_CHARS (&line_id, &buf_d, (numline++,&numline),&un);
		sprintf (buf,circuit_line,
				(100*ppb->nbpackets.delta)/totpackets,
				blank_node_name,
				blank_node_name,
				print_ethernet_protocol(ppp, &bufp),
				ppp->nbbytes.delta,
				ppp->nbpackets.delta/8);
		SMG$PUT_CHARS (&line_id, &buf_d, (numline++,&numline),&un);
	}
	SMG$SET_CURSOR_REL (&line_id,&un); numline++;

	/*** top circuit ***/
	{
		register circuit_entry * cp, *cpp, *cpb;
		for (cp= tabcircuit, maxbytes=0, maxpackets=0; 
		     cp < tabcircuit+maxcircuit;
		     cp++){
			if (cp->nbbytes.delta >= maxbytes){
				maxbytes= cp->nbbytes.delta;
				cpb= cp;
			}
			if (cp->nbpackets.delta >= maxpackets){
				maxpackets= cp->nbpackets.delta;
				cpp= cp;
			}
		}
		sprintf (buf,circuit_line,
				(100*cpb->nbbytes.delta)/totbytes,
				print_ethernet_addr(cpb->srcptr->nodptr, &bufs),
				print_ethernet_addr(cpb->dstptr->nodptr, &bufd),
				print_ethernet_protocol(cpb->dstptr->prtptr, &bufp),
				cpb->nbbytes.delta,
				cpb->nbpackets.delta/8
			);
		SMG$PUT_CHARS (&line_id, &buf_d, (numline++, &numline), &un);
		sprintf (buf,circuit_line,
				(100*cpp->nbpackets.delta)/totpackets,
				print_ethernet_addr(cpp->srcptr->nodptr, &bufs),
				print_ethernet_addr(cpp->dstptr->nodptr, &bufd),
				print_ethernet_protocol(cpp->dstptr->prtptr, &bufp),
				cpp->nbbytes.delta,
				cpp->nbpackets.delta/8
			);
		SMG$PUT_CHARS (&line_id, &buf_d, (numline++, &numline), &un);
	}
	SMG$SET_CURSOR_REL (&line_id,&un); numline++;

	/*** top transport ***/
	{
		register source_entry * sp, *sppr, *spbr, * spps, *spbs;
		ulong	maxsbytes=0, maxrbytes=0, maxspackets=0, maxrpackets=0;

		for (i=maxsource, sp= tabsource; i--;sp++){
			if (sp->nbrbytes.delta >= maxrbytes){	/* <= garantit*/
				maxrbytes= sp->nbrbytes.delta;	/* l'init. */
				spbr= sp;
			}
			if (sp->nbrpackets.delta >= maxrpackets){
				maxrpackets= sp->nbrpackets.delta;
				sppr= sp;
			}
			if (sp->nbsbytes.delta >= maxsbytes){
				maxsbytes= sp->nbsbytes.delta;
				spbs= sp;
			}
			if (sp->nbspackets.delta >= maxspackets){
				maxspackets= sp->nbspackets.delta;
				spps= sp;
			}
		}

		/* listener */
		sprintf (buf,circuit_line,
				(100*spbr->nbrbytes.delta)/totbytes,
				blank_node_name,
				print_ethernet_addr(spbr->nodptr, &bufs),
				print_ethernet_protocol(spbr->prtptr, &bufp),
				spbr->nbrbytes.delta,
				spbr->nbrpackets.delta/8);
		SMG$PUT_CHARS (&line_id, &buf_d, (numline++,&numline),&un);
		sprintf (buf,circuit_line,
				(100*sppr->nbrpackets.delta)/totpackets,
				blank_node_name,
				print_ethernet_addr(sppr->nodptr, &bufs),
				print_ethernet_protocol(sppr->prtptr, &bufp),
				sppr->nbrbytes.delta,
				sppr->nbrpackets.delta/8);
		SMG$PUT_CHARS (&line_id, &buf_d, (numline++,&numline),&un);
		SMG$SET_CURSOR_REL (&line_id,&un); numline++;

		/* talker */
		sprintf (buf,circuit_line,
				(100*spbs->nbsbytes.delta)/totbytes,
				print_ethernet_addr(spbs->nodptr, &bufs),
				blank_node_name,
				print_ethernet_protocol(spbs->prtptr, &bufp),
				spbs->nbsbytes.delta,
				spbs->nbspackets.delta/8);
		SMG$PUT_CHARS (&line_id, &buf_d, (numline++,&numline),&un);
		sprintf (buf,circuit_line,
				(100*spps->nbspackets.delta)/totpackets,
				print_ethernet_addr(spps->nodptr, &bufs),
				blank_node_name,
				print_ethernet_protocol(spps->prtptr, &bufp),
				spps->nbsbytes.delta,
				spps->nbspackets.delta/8);
		SMG$PUT_CHARS (&line_id, &buf_d, (numline++,&numline),&un);
	}
	SMG$SET_CURSOR_REL (&line_id,&un); numline++;

	/*** top node ***/
	{
		register node_entry * np, * npps, *nppr, * npbs,* npbr;
		ulong	maxsbytes=0, maxrbytes=0, maxspackets=0, maxrpackets=0;

		for (i=maxnode, np=tabnode; i--;np++){
			if (np->nbrbytes.delta >= maxrbytes){
				maxrbytes= np->nbrbytes.delta;
				npbr= np;
			}
			if (np->nbrpackets.delta >= maxrpackets){
				maxrpackets= np->nbrpackets.delta;
				nppr= np;
			}
			if (np->nbsbytes.delta >= maxsbytes){
				maxsbytes= np->nbsbytes.delta;
				npbs= np;
			}
			if (np->nbspackets.delta >= maxspackets){
				maxspackets= np->nbspackets.delta;
				npps= np;
			}
		}

		/* listener */
		sprintf (buf,circuit_line,
				(100*npbr->nbrbytes.delta)/totbytes,
				blank_node_name,
				print_ethernet_addr(npbr, &bufs),
				blank_protocol_name,
				npbr->nbrbytes.delta,
				npbr->nbrpackets.delta/8);
		SMG$PUT_CHARS (&line_id, &buf_d, (numline++,&numline),&un);
		sprintf (buf,circuit_line,
				(100*nppr->nbrpackets.delta)/totpackets,
				blank_node_name,
				print_ethernet_addr(nppr, &bufs),
				blank_protocol_name,
				nppr->nbrbytes.delta,
				nppr->nbrpackets.delta/8);
		SMG$PUT_CHARS (&line_id, &buf_d, (numline++,&numline),&un);

		/* talker */
		sprintf (buf,circuit_line,
				(100*npbs->nbsbytes.delta)/totbytes,
				print_ethernet_addr(npbs, &bufs),
				blank_node_name,
				blank_protocol_name,
				npbs->nbsbytes.delta,
				npbs->nbspackets.delta/8);
		SMG$SET_CURSOR_REL (&line_id,&un); numline++;
		SMG$PUT_CHARS (&line_id, &buf_d, (numline++,&numline),&un);
		sprintf (buf,circuit_line,
				(100*nppr->nbspackets.delta)/totpackets,
				print_ethernet_addr(npps, &bufs),
				blank_node_name,
				blank_protocol_name,
				npps->nbsbytes.delta,
				npps->nbspackets.delta/8);
		SMG$PUT_CHARS (&line_id, &buf_d, (numline++,&numline),&un);
	}

	SMG$END_PASTEBOARD_UPDATE (&paste_id);
	SMG$FLUSH_BUFFER (&paste_id);
	return true;
}

static void
init_display()
{
static const $DESCRIPTOR (topproto,  "      ***       Top protocol     ***");
static const $DESCRIPTOR (topcirc,   "      ***       Top circuit      ***");
static const $DESCRIPTOR (toptptlstn,"      *** Top transport listener ***");
static const $DESCRIPTOR (toptpttlk, "      ***  Top transport talker  ***");
static const $DESCRIPTOR (toplisten, "      ***       Top listener     ***");
static const $DESCRIPTOR (toptalk,   "      ***        Top talker      ***");
static const struct dsc$descriptor_s *init[]= 
			{&topproto,&topcirc,&toptptlstn,&toptpttlk,&toplisten,
			 &toptalk};
static const $DESCRIPTOR(maxblock,
"                                                                  (by block)");
static const $DESCRIPTOR(maxbyte,
"                                                                   (by byte)");
	int i;

	SMG$SET_CURSOR_ABS (&line_id,&un,&un);
	for (i=0; i< sizeof init/sizeof (char *); i++){
		SMG$PUT_LINE (&line_id,init[i]);
		SMG$PUT_LINE (&line_id,&maxbyte);
		SMG$PUT_LINE (&line_id,&maxblock);
	}
}

boolean
cmd_monitor_top()
{
static	const char header[]=
"%              From                To      Protocol Bits/sec Pckts/sec";
	boolean savbuf= emon$enable_buf (true);
	protocol_entry *proto;
	node_entry *from, *to;
	int	stat_flags;

	if isfalse (get_filter (&from, &to, &proto, &stat_flags)) return false;

	emon$title (header);
	init_display();

	emon$paste_display (&line_coord);
	init_monitor (mon_top_update);
	monitor(glob_stat);	/* pour l'instant... */
	(void) emon$enable_buf (savbuf);
}
