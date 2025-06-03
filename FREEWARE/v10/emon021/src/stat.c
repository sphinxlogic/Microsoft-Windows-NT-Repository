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
#define DEBUG_STAT

#include "monitor.h"
#include "stat.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <libdef.h>
#if defined (__STDC__) || defined (__DECC)
#include <stdlib.h>
#endif

int	LIB$INSERT_TREE(), LIB$LOOKUP_TREE(), LIB$TRAVERSE_TREE();

/*
 *	variables pour hash-code des circuits
 *	il faut prende un hash dissymetrique sinon les demandes et les reponses
 *	seront toujours en collision
 */
#define HSHSIZE	256	/* nb d'entrées hashées */
#define HASH(p)	(p->tag.from_addr.bytes[5])\
		^(p->tag.dest_addr.bytes[4])\
		^(p->tag.protocol.bytes[1])
static circuit_entry *	hshcirc[HSHSIZE];
static unsigned hshcnt [HSHSIZE];

static source_entry starsource;
static node_entry starnode;
static protocol_entry starproto;

static statfr_unknown_srcs;
static statfr_unknown_dsts;
static statfr_unknown_ends;
static statfr_unknown_protocols;

static const ulong insert_control_flags= 0;
static FILE * fprt;	/* print file pointer */

static ulong
fullbyte(x)
ubyte x;
{
	ulong i;
	sprintf ((char *) &i,"%.1X%.1X",x>>4,x&0xf);
	return i&0xffff;
}

protocol_name_t *
print_ethernet_protocol(pp,buf)
protocol_name_t * buf;
protocol_entry * pp;
{
	if (pp->name[0]==0) {
		ulong proto1= fullbyte (pp->protocol.bytes[1]);
		if (pp->protocol.bytes[0]==0) 
			sprintf ((char *) buf,"%9s %2s","802.3",&proto1);
		else{	ulong proto0= fullbyte (pp->protocol.bytes[0]);
			sprintf ((char *) buf,"%9s-%2s",&proto0,&proto1);
		}
	}
	else sprintf ((char *) buf,"%12.12s",pp->name);
	return buf;
}

node_name_t *
print_ethernet_addr(np,buf)
node_entry *np;
node_name_t *buf;
{
	char buf1[18];
	if (np->name[0]==0)
		if (np->addr.decnet.header==DECNET_HEADER){
			sprintf (buf1,"%d.%d",
					np->addr.decnet.area,
					np->addr.decnet.node);
		}
		else{
			register char * cp= buf1;
			register int i;
			for (i=0; i<6; i++){
				* (ushort *) cp= fullbyte(np->addr.bytes[i]);
			cp[2]= (i==5)? 0 : '-';
				cp+=3;
			}
		}
	else sprintf (buf1,np->name);
	sprintf ( (char *) buf,"%17.17s",buf1);
	return buf;
}

/*	print command:	src; dst; bytes; pkts; 	*/
static
char *
print_circ(cp,buf)
char *buf;
register circuit_entry * cp;
{
	char buf1[18],buf2[18],buf3[13];
	sprintf (buf,"%c %s %s %s %6lu%12lu\n",
		"FP"[cp->pass],
		print_ethernet_addr(cp->srcptr->nodptr, &buf1),
		print_ethernet_addr(cp->dstptr->nodptr, &buf2),
		print_ethernet_protocol(cp->srcptr->prtptr, &buf3),
		cp->nbpackets.total, cp->nbbytes.total);
	return buf;
}

typedef struct {
	ethernet_addr_t addr;
	} nodelabel_t;

static 
boolean
allonode (nodelabel ,nodeaddr)
	nodelabel_t * nodelabel;
	node_entry **nodeaddr;
	{
	register node_entry * ptrnode;
	if (maxnode==node_nb){
		abort_monitor ("systems table overflow");
		return false;
	}
	else{
		
		ptrnode= tabnode+(maxnode++);
		* (nodelabel_t *) &ptrnode->addr = *nodelabel;
		ptrnode->name[0]= 0;	/* init with null name */
		ptrnode->nbspackets.total=0;
		ptrnode->nbrpackets.total=0;
		ptrnode->nbsbytes.total=0;
		ptrnode->nbrbytes.total=0;
		ptrnode->nbspackets.delta=0;
		ptrnode->nbrpackets.delta=0;
		ptrnode->nbsbytes.delta=0;
		ptrnode->nbrbytes.delta=0;
		ptrnode->passsrc= statfr_unknown_srcs;
		ptrnode->passdst= statfr_unknown_dsts;
		ptrnode->passnod= statfr_unknown_ends;
		ptrnode->new= true;
		*nodeaddr= ptrnode;
		return true;
	}
}

static int 
comparenode (nodelabel,ptrnode)
nodelabel_t * nodelabel;
node_entry * ptrnode;
{
	if (nodelabel->addr.firstpart > ptrnode->addr.firstpart) return 1;
	else 	if (nodelabel->addr.firstpart < ptrnode->addr.firstpart) return -1;
	else	if (nodelabel->addr.secondpart > ptrnode->addr.secondpart) return 1;
	else	if (nodelabel->addr.secondpart < ptrnode->addr.secondpart) return -1;
	else	return 0;

	/*return memcmp (nodelabel,&(ptrnode->addr),6);*/
}

/*	   addnode: add  1 address to node list    	*/
/*	not static, used by initstat & monitor area	*/
/*	returns node ptr if node existed or was created	*/

node_entry *
addnode (addr)
ethernet_addr_t * addr;
{
	node_entry * node;
	ulong i;

	i= LIB$INSERT_TREE (&node_header,addr,&insert_control_flags,
				comparenode,allonode,&node);
	if (i&1) return(node);
	else return 0;
}

static int
printnode(np)
node_entry *np;
{
	node_name_t buf;

	if ((np->nbspackets.total+np->nbrpackets.total)!=0){
		print_ethernet_addr(np, &buf);
		fprintf (fprt, "%c%c%c %s s: %12lu(%6lu) r: %12lu(%6lu)\n",
			"ON"[np->new],"FP"[np->passsrc],"FP"[np->passdst],
			buf,np->nbsbytes.total,np->nbspackets.total,
			    np->nbrbytes.total,np->nbrpackets.total);
	}
	return 1;
}

static boolean
alloprotocol (protocollabel ,nodeaddr)
ethernet_protocol_t * protocollabel;
protocol_entry **nodeaddr;
	{
	register protocol_entry * ptrnode;
	if (maxprotocol==protocol_nb){
		if istrue (ignore.protocol.overflow) {
			counter.protocol.overflow++;
		}
		else	abort_monitor("Protocol table overflow");
		return false;
	}
	else{
		ptrnode= tabprotocol+(maxprotocol++);
		* (ethernet_protocol_t *) &ptrnode->protocol = *protocollabel;
		ptrnode->nbpackets.total=0;
		ptrnode->nbpackets.delta=0;
		ptrnode->nbbytes.total=0;
		ptrnode->nbbytes.delta=0;
		ptrnode->pass= statfr_unknown_protocols;
		ptrnode->new= true;
		*nodeaddr= ptrnode;
		return true;
	}
}

static int 
compareprotocol (protocollabel,ptrnode)
ethernet_protocol_t * protocollabel;
protocol_entry *ptrnode;
{
	return (int) (protocollabel->word - ptrnode->protocol.word);
}

/*	not static, used by initstat */
protocol_entry *
addprotocol (proto)
ethernet_protocol_t * proto;
{
	protocol_entry *node;
	register ulong i;

	i= LIB$INSERT_TREE (&protocol_header, proto, &insert_control_flags,
				compareprotocol, alloprotocol, &node, proto);
	if (i&1) return(node);
	return NULL;
}

static int
printprotocol(pp)
protocol_entry *pp;
{
	protocol_name_t buf;
	if (pp->nbpackets.total!=0)
		fprintf (fprt, "%c%c %s %12lu(%6lu)\n", "ON"[pp->new],
				   "FP"[pp->pass],
				   print_ethernet_protocol(pp, &buf),
		                   pp->nbbytes.total,pp->nbpackets.total);
	return 1;
}

typedef struct {ethernet_addr_t addr;
		ethernet_protocol_t protocol;}
		sourcelabel_t;

static
boolean
allosource (sourcelabel ,nodeaddr)
	sourcelabel_t * sourcelabel;
	source_entry **nodeaddr;
	{
	register source_entry * ptrnode;
	if (maxsource==source_nb){
		abort_monitor("Transport table overflow");
		return false;
	}
	else{
		
		ptrnode= tabsource+(maxsource++);
		* (sourcelabel_t *) &ptrnode->addr = *sourcelabel;
		ptrnode->nbspackets.total=0;
		ptrnode->nbrpackets.total=0;
		ptrnode->nbsbytes.total=0;
		ptrnode->nbrbytes.total=0;
		ptrnode->nbspackets.delta=0;
		ptrnode->nbrpackets.delta=0;
		ptrnode->nbsbytes.delta=0;
		ptrnode->nbrbytes.delta=0;
		*nodeaddr= ptrnode;
		return true;
	}
}

static int 
comparesource (sourcelabel,ptrnode)
sourcelabel_t * sourcelabel;
source_entry * ptrnode;
{
	return memcmp (sourcelabel,&(ptrnode->addr),8);
}

/* not static: used by analyze/trace */

source_entry * 
addsource (addr,protocol)
ethernet_addr_t * addr;
ethernet_protocol_t * protocol;
{
	sourcelabel_t sourcelabel;
	source_entry *node;
	register ulong i;

	sourcelabel.addr= *addr;
	sourcelabel.protocol= *protocol;
	i= LIB$INSERT_TREE (&source_header, &sourcelabel,& insert_control_flags,
				comparesource, allosource, &node);
	if (i&1) return(node);
	return NULL;
}

/*	print command: source; protocol; pkt emis; pkt recus	*/

static
boolean
printsource(sp)
source_entry *sp;
{
	node_name_t buf;
	protocol_name_t bufp;
	fprintf (fprt, "%s %s  s: %12lu(%6lu) r: %12lu(%6lu)\n",
			print_ethernet_addr(sp->nodptr, &buf),
			print_ethernet_protocol (sp->prtptr, &bufp),
			sp->nbsbytes.total,sp->nbspackets.total,
			sp->nbrbytes.total,sp->nbrpackets.total);
	return true;
}

void
print_stat(outf)
char * outf;
{
	circuit_entry * cp;
	int i;
	source_entry * s;
	char buf[80];

	if (outf != NULL) fprt = fopen (outf, "w", "dna=.lis");
	else fprt = stdout;
	if (fprt != NULL) {
		upd_stat();
		for (cp=tabcircuit, i=maxcircuit ; i--; cp++)
			fprintf (fprt, print_circ (cp,buf));
		LIB$TRAVERSE_TREE (&source_header,printsource);
		LIB$TRAVERSE_TREE (&node_header,printnode);
		LIB$TRAVERSE_TREE (&protocol_header,printprotocol);
		if (outf != NULL) fclose (fprt);
	}
	else {	emon$put_error ("Error opening print file");
		emon$put_more_error (strerror (errno, vaxc$errno));
	}
}

/*	update statistics for source, node and protocols	*/

boolean
upd_stat()
{
	register circuit_entry *cp;	/* circuit pointer */
	register source_entry *sp;
	register protocol_entry *pp;
	register node_entry *np;
	register counter_t *ap;
	register int i;

/*	zero deltas	*/


	/* sources: node/protocol */

	for (sp=tabsource,i=maxsource; i--; sp++){
		sp->nbrpackets.delta=0;
		sp->nbspackets.delta=0;
		sp->nbrbytes.delta=0;
		sp->nbsbytes.delta=0;
	}

	/* protocols */

	for (pp=tabprotocol,i=maxprotocol; i--; pp++){
		pp->nbpackets.delta=0;
		pp->nbbytes.delta=0;
	}

	/* nodes */

	for (np=tabnode,i=maxnode; i--; np++){
		np->nbrpackets.delta=0;
		np->nbspackets.delta=0;
		np->nbrbytes.delta=0;
		np->nbsbytes.delta=0;
	}

	/* decnet phase IV areas */

	for (ap=tabareatraf,i=area_nb; i; )
	{
		register int j;
		if (tabarea[area_nb-(i--)].used)
			for (j=area_nb; j--; ap++->delta=0);
		else ap+= area_nb;
	}

/*	 scan circuit table 	*/

	for (cp=tabcircuit,i=maxcircuit ; i--; cp++){
		register ulong
sa=(cp->source_addr.decnet.header==DECNET_HEADER)? cp->source_addr.decnet.area: 0,
da=(cp->dest_addr.decnet.header==DECNET_HEADER)?   cp->dest_addr.decnet.area: 0;
		if ( (sa>=area_nb) || (da>=area_nb) ) {
			char buf[64];
			sprintf (buf,"Found out of range area (%d)",
				(sa>=area_nb)? sa : da);
			abort_monitor(buf);
			return false;
		}
		
		tabarea[sa].used++;
		tabarea[da].used++;
		tabareatraf[sa+area_nb*da].delta+= cp->nbbytesctr;

/*	update	'total' and 'delta' fields in circuit entry	*/

		cp->nbpackets.delta= cp->nbpacketsctr;
		cp->nbpacketsctr=0;
		cp->nbbytes.delta= cp->nbbytesctr;
		cp->nbbytesctr=0;
		cp->nbpackets.total+= cp->nbpackets.delta;
		cp->nbbytes.total+= cp->nbbytes.delta;

	/* get all fields to determine if this circuit entry is countable */

		sp= cp->srcptr;
		if (sp==NULL){
			sp= addsource (&cp->source_addr, &cp->protocol);
			if (sp==NULL) return false;
			cp->srcptr=sp;
		}

		if (sp->nodptr==NULL){
			sp->nodptr= addnode (&cp->source_addr);
			if (sp->nodptr==NULL) return false;
		}

		pp= sp->prtptr;
		sp= cp->dstptr;
		if (sp==NULL){
			sp= addsource (&cp->dest_addr, &cp->protocol);
			if (sp==NULL) return false;
			cp->dstptr= sp;
		}

		if (sp->nodptr==NULL){
			sp->nodptr= addnode (&cp->dest_addr);
			if (sp->nodptr==NULL) return false;
		}

	/*	il faut le protocole pour determiner si le paquet est valide */

		if (pp==NULL) {
			pp= cp->srcptr->prtptr;
			if (pp==NULL) {
				pp=addprotocol (&sp->protocol);
				if (pp==NULL) return false;
				sp->prtptr= pp;
			}
			cp->srcptr->prtptr= pp;
		}

		cp->pass= ((cp->srcptr->nodptr->passsrc
			   &cp->dstptr->nodptr->passdst)
			  | cp->srcptr->nodptr->passnod
			  | cp->dstptr->nodptr->passnod)
			 & pp->pass;

	/* if the circuit is valid after filtering, count bytes and packets */

		if istrue (cp->pass) {
			sp= cp->srcptr;
			sp->nbspackets.delta+= cp->nbpackets.delta;
			sp->nbsbytes.delta+= cp->nbbytes.delta;

			sp= cp->dstptr;
			sp->nbrpackets.delta+= cp->nbpackets.delta;
			sp->nbrbytes.delta+= cp->nbbytes.delta;
		}

	} /* circuit scan */

/*	pass source information down to node and protocol table	*/

	for (sp=tabsource, i=maxsource; i--; sp++) {

		/* fully initialize source nbrpackets and nbspackets */

		sp->nbrpackets.total+=sp->nbrpackets.delta;
		sp->nbspackets.total+=sp->nbspackets.delta;
		sp->nbrbytes.total+= sp->nbrbytes.delta;
		sp->nbsbytes.total+= sp->nbsbytes.delta;

		/* update NODE entry from source entry */

		np= sp->nodptr;
		if (np==NULL){
			np= addnode(&sp->addr);
			if (np==NULL) return false;	/* plus de place */
			sp->nodptr= np;
		}
		np->nbrpackets.total+=	sp->nbrpackets.delta;
		np->nbspackets.total+=	sp->nbspackets.delta;
		np->nbrbytes.total+=	sp->nbrbytes.delta;
		np->nbsbytes.total+= 	sp->nbsbytes.delta;
		np->nbrpackets.delta+= 	sp->nbrpackets.delta;
		np->nbspackets.delta+= 	sp->nbspackets.delta;
		np->nbrbytes.delta+= 	sp->nbrbytes.delta;
		np->nbsbytes.delta+= 	sp->nbsbytes.delta;

		/* update PROTOCOL entries from source entries */

		pp= sp->prtptr;
		if (pp==NULL){	/* ca peut arriver, on ne traite que les cas
				   d'exception dans la routine circuits */
			pp= addprotocol(&sp->protocol);
			if (pp==NULL) return false;	/* plus de place */
			sp->prtptr= pp;
		}
		/* only count once packets in protocol entries... */
		pp->nbpackets.total+=	sp->nbspackets.delta;
		pp->nbbytes.total+=	sp->nbsbytes.delta;
		pp->nbpackets.delta+= 	sp->nbspackets.delta;
		pp->nbbytes.delta+= 	sp->nbsbytes.delta;

	}	/*	end source scan		*/

	for (ap=tabareatraf,i=area_nb; i; ap+= area_nb)
	{
		register int j;
		register counter_t * a2p;
		if (tabarea[area_nb-(i--)].used)
			for (j=area_nb,a2p= tabareatraf+(i*area_nb); j--; a2p++)
				a2p->total+= a2p->delta;
	}
	return true;
}

boolean
glob_stat(p)
register pckt_t *p;
{
	int   hshindex= HASH(p);

	register unsigned long pcktsize=
		ETHERNET_LINK_LAYER_OVERHEAD+p->iosb.iosb_count;

	register circuit_entry ** entadrloc= hshcirc+hshindex;
	register circuit_entry * q;

	for (;;){
		q= *entadrloc;	
		if (q==NULL)	/* hash link non initialisé */
			if (maxcircuit==circuit_nb){
				abort_monitor("circuit table overflow");
				return false;
			}
			else {
				q=tabcircuit+(maxcircuit++);
				*(packet_ident_t *) &(q->dest_addr)= *(packet_ident_t *) &p->tag;
				*entadrloc= q;	/* maj hshlnk */
				q->nbpacketsctr=1;
				q->nbbytesctr= pcktsize;
				q->nbbytes.total=0;
				q->nbpackets.total=0;
				q->hshlnk=NULL;
				q->srcptr=NULL;
				q->dstptr=NULL;
				q->pass= true;	/* si on est la c'est que ca passe */
				hshcnt[hshindex]++;	/* debug */
				break;
			}
		if (equ_tag ( (packet_ident_t *) &q->dest_addr,&p->tag)){
			q->nbpacketsctr++;
			q->nbbytesctr+= pcktsize;
			break;
		}
	entadrloc= &q->hshlnk;
	}

#ifdef DEBUG_STAT
	if (q->protocol.word==0) abort();
#endif /* DEBUG_STAT */
}

static struct {boolean source,dest,protocol,call,initcp,bothways,xtend;} local_opt;
static ethernet_addr_t		local_source;
static ethernet_addr_t		local_dest;
static ethernet_protocol_t	local_protocol;
static boolean (* local_call) ();

init_loc_stat(source,dest,protocol,call,opt)
node_entry * source, * dest;
protocol_entry * protocol;
boolean (* call)();
ulong	opt;
{
	if (source!=NULL){
		local_opt.source= true;
		local_source= source->addr;
	} else local_opt.source= false;
	if (dest!=NULL){
		local_opt.dest= true;
		local_dest= dest->addr;
	} else local_opt.dest= false;
	if (protocol!=NULL){
		local_opt.protocol= true;
		local_protocol= protocol->protocol;
	} else local_opt.protocol= false;
	if (call!=NULL){
		local_opt.call= true;
		local_call= call;
	} else local_opt.call= false;

	local_opt.initcp= (boolean) ((opt & LOC_STAT_INITCP)!=0);
	local_opt.bothways= (boolean) ((opt & LOC_STAT_BOTHWAYS)!=0);
	local_opt.xtend= (boolean) ((opt & LOC_STAT_XTEND)!=0);

	if istrue (local_opt.xtend) {
		local_opt.initcp= true;
		local_opt.protocol= false;
		local_opt.source= false;
		local_opt.dest= false;
	}
}

boolean
loc_stat(p)
register pckt_t *p;
{
	register int hshindex;
	register unsigned long pcktsize;
	circuit_entry ** entadrloc;
	register circuit_entry * q;

	if (istrue(local_opt.source)){
		if (neq_addr (&p->tag.from_addr,&local_source) )
			if (istrue(local_opt.bothways)){
				if (neq_addr(&p->tag.dest_addr,&local_source))
					goto testiosb;
				else if (istrue(local_opt.dest) && neq_addr(&p->tag.from_addr,&local_dest) )
					goto testiosb;
			}
			else		goto testiosb;
		else 
			if (istrue (local_opt.dest) && neq_addr(&p->tag.dest_addr,&local_dest) )
					goto testiosb;
	}
	else if (istrue(local_opt.dest) && neq_addr(&p->tag.dest_addr,&local_dest))
					goto testiosb;

	if (istrue(local_opt.protocol)){
		if ( p->tag.protocol.word != local_protocol.word )
					 goto testiosb;
	}

	hshindex= HASH(p);
	pcktsize= ETHERNET_LINK_LAYER_OVERHEAD+p->iosb.iosb_count;
	entadrloc= hshcirc+hshindex;
	for (;;){
		q = *entadrloc;
		if (q==NULL)
			if (maxcircuit==circuit_nb){
				abort_monitor("circuit table overflow");
				return false;
			}
			else {
				q=tabcircuit+maxcircuit;
				*(packet_ident_t *) &(q->dest_addr)= *(packet_ident_t *) &p->tag;
				q->hshlnk=NULL;
				hshcnt[hshindex]++;
				if istrue (local_opt.initcp) {
					protocol_entry *pp;
					q->srcptr=addsource (&q->source_addr,&q->protocol);
					if (q->srcptr==NULL) break;
					pp= addprotocol (&q->srcptr->protocol);
					if (pp==NULL) break;
					q->srcptr->prtptr= pp;
					q->srcptr->nodptr= addnode (&q->source_addr);
					if (q->srcptr->nodptr==NULL) break;
					q->dstptr=addsource (&q->dest_addr,&q->protocol);
					if (q->dstptr==NULL) break;
					q->dstptr->nodptr= addnode (&q->dest_addr);
					if (q->dstptr->nodptr==NULL) break;
					q->dstptr->prtptr= pp;
					q->pass= ((q->srcptr->nodptr->passsrc
						  &q->dstptr->nodptr->passdst)
						 | q->srcptr->nodptr->passnod
						 | q->dstptr->nodptr->passnod)
						& pp->pass;
				}
				else {
					q->srcptr=NULL;
					q->dstptr=NULL;
					q->pass= true;	/* gros mensonge */
				}
				*entadrloc= q;	/* maj hshlnk */

				q->nbpacketsctr=1;
				q->nbbytesctr= pcktsize;
				q->nbbytes.total=0;
				q->nbpackets.total=0;

				maxcircuit++;	/* validate circuit entry */
				break;
			}
		if (equ_tag( (packet_ident_t *)&q->dest_addr,&p->tag)){
			q->nbpacketsctr++;
			q->nbbytesctr+= pcktsize;
			break;
		}
	entadrloc= &q->hshlnk;
	}

	if istrue (local_opt.xtend) {
		if isfalse(q->pass) goto testiosb;
	}
	else {
		if (istrue(local_opt.initcp)) {
			register source_entry * sp= q->srcptr;
			if (sp==0) {
				sp=q->srcptr=addsource (&q->source_addr,&q->protocol);
				sp->nodptr= addnode (&sp->addr);
				sp->prtptr= addprotocol (&sp->protocol);
			}
			sp= q->dstptr;
			if (sp==0) {
				sp=q->dstptr=addsource (&q->dest_addr,&q->protocol);
				sp->nodptr= addnode (&sp->addr);
				sp->prtptr= addprotocol (&sp->protocol);
			}
		}
	}

	if istrue (local_opt.call) 
		return (* local_call)(p,q);
	else	return true;

testiosb:
	if ( (p->iosb.iosb_devdep != 0x800) && istrue (local_opt.call))
		return (* local_call) (p,0);
	else	return false;
}

boolean
stat$show_hash()	/* debug */
{
	unsigned * p;

	for (p=hshcnt; p < hshcnt+HSHSIZE; p += 16)
		printf ("%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d\n",
			p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],
			p[8],p[9],p[10],p[11],p[12],p[13],p[14],p[15]);
	return true;
}

	/* routines pour traiter le 802E */
	/* en fait on se contente de remplacer ces protocoles par des */
	/* protocoles ethernet de valeur < 1500 */

static protocol802e_entry * hshproto802e[HSHSIZE];	/* 802E protocol hash */
static unsigned hsh802ecnt [HSHSIZE];

boolean
addprotocol802 (tp)
packet_header_t * tp;
{
	unsigned char * protocol802e;
	int hshindex;
	register protocol802e_entry ** entadrloc;
	unsigned short dssap= tp->protocol.word;
	boolean	extend;
static	unsigned char fake_802e[5]= {0, 0, 0, 0, 0};

	/* un paquet 802E est repéré par un dssap et un ssap particulier */
	/* ainsi qu'un octet de contrôle de type UI */

	if  ((dssap!= IEEE802E_DSSAP) || (tp->ctlb!=IEEE802_CTL_UI)){/* 802 */
		if ((tp->ctlb&3) == 3)	tp->hdr802.ctl= tp->ctlb;
		else			tp->hdr802.ctl= tp->ctlw;
		tp->hdr802.dssap= dssap;
		tp->type= PKT_802;

	/* cas normal: DSAP = SSAP => numero de protocole >=0 et <=255 */

		if (tp->hdr802.dsap==tp->hdr802.ssap)	{
			tp->protocol.bytes[1]= tp->hdr802.ssap;
			tp->protocol.bytes[0]= 0;
			return true;
		}

	/* cas anormal: DSAP <> SSAP => numero de protocole 802E */
	/* on simule un protocole 802e de le forme (0,0,0,ssap,dsap) */
	/* sachant que les protocoles 802E de cette forme ne peuvent pas */
	/* exister dans la table que nous allons garnir */

		extend=false;			/* 802 */
		protocol802e= fake_802e;
		protocol802e[3]= tp->hdr802.dsap;
		protocol802e[4]= tp->hdr802.ssap;
	}
	else{
		tp->type= PKT_802E;
		protocol802e= (unsigned char *) &tp->hdr802e.pid;

		/* si nous avons un protocole de la forme: 00-00-00-xx-yy */
		/* alors il s'agit d'un protocole ethernet mis sur 802.3E */

		if ( (* (unsigned short *) protocol802e == 0) 
			&& (protocol802e[2]== 0) ){
			tp->protocol.word= * (short *) &protocol802e[3];
			return true;
		}
	   	extend= true;			/* 802E */
	}

	hshindex= protocol802e[4]^protocol802e[3]^protocol802e[2];
	entadrloc= hshproto802e+hshindex;
                                                          

	/* on cree un protocole bidon */
	/* une table contient les correspondances entre les protocoles 802E */
	/* et un numero de protocole >= 256 et < 1500			    */

	for (;;){
		register protocol802e_entry * q= *entadrloc;
		if (q==NULL)	/* hash link non initialisé */
				/* il faut ajouter un nouveau protocole */
			if (maxprotocol802e==protocol802e_nb) {
				abort_monitor("802E protocol table overflow");
				return false;
			}
			else {	protocol_entry *ethp;
				ethernet_protocol_t ethproto;

				ethproto.word= maxprotocol802e+256;

				ethp= addprotocol (&ethproto);
				if (ethp!=NULL) {
					q=tabprotocol802e+(maxprotocol802e++);
					*entadrloc= q;	/* maj hshlnk */
					q->protocol_low= * (long *) protocol802e;
					q->protocol_high = protocol802e[4];
					q->protoptr= ethp;
					hsh802ecnt[hshindex]++;	/* debug */
assert (sizeof ethp->name > 12); /* pour pouvoir caser le hex du protocole */
					if (ethp->name[0]==0)
					if istrue (extend)
						sprintf (ethp->name,
							"%02X%02X%02X-%02X-%02X",
							protocol802e[0],
							protocol802e[1],
							protocol802e[2],
							protocol802e[3],
							protocol802e[4]);
					else	sprintf (ethp->name,
							"SAP: d%02X/s%02X",
							protocol802e[3],
							protocol802e[4]);
					tp->protocol.word= ethp->protocol.word;
					break;
				}
				else abort_monitor ("Protocol table overflow");
				return false;
			}
		if ( (q->protocol_low == * (long *) protocol802e)
		     && (q->protocol_high == protocol802e[4]) ){
			tp->protocol.word= q->protoptr->protocol.word;
			break;
		}

		/* prochain link de hash */
		entadrloc= &q->hshlnk;	/* PB ?? */
	}
	return true;
}

void
statfr_reset()
{
	int i;
	circuit_entry * q;

	for (q= tabcircuit, i=0; i<maxcircuit; i++, q++) {
		register source_entry * sp= q->srcptr;
		if (sp==NULL) {
			sp=q->srcptr=addsource (&q->source_addr,&q->protocol);
			sp->nodptr= addnode (&sp->addr);
			sp->prtptr= addprotocol (&sp->protocol);
		}
		sp= q->dstptr;
		if (sp==NULL) {
			sp=q->dstptr=addsource (&q->dest_addr,&q->protocol);
			sp->nodptr= addnode (&sp->addr);
			sp->prtptr= addprotocol (&sp->protocol);
		}
		q->pass= false;
	}
}

void
statfr_apply()
{
	int i;
	circuit_entry * q;
	node_entry *np;

	for (q= tabcircuit, i=0; i<maxcircuit; i++, q++) {

		q->pass= q->srcptr->nodptr->passsrc &
			 q->dstptr->nodptr->passdst &
			 q->srcptr->prtptr->pass;
	}

	/* set the pass attribute for nodes */

	for (i= 0, np=tabnode; i<maxnode; i++, np++) np->pass= np->passsrc |
							       np->passdst |
							       np->passnod;
}

void
statfr_disable_protocols()
{
	int i;
	for (i= 0; i<maxprotocol; i++) tabprotocol[i].pass=false;
	statfr_unknown_protocols= false;
}

void
statfr_enable_protocols()
{
	int i;
	for (i= 0; i<maxprotocol; i++) tabprotocol[i].pass=true;
	statfr_unknown_protocols= true;
}

void
statfr_enable_unknown_protocols()
{
	int i;
	for (i= 0; i<maxprotocol; i++) if istrue (tabprotocol[i].new) 
		tabprotocol[i].pass=true;
	statfr_unknown_protocols= true;
}

void
statfr_disable_dsts()
{
	int i;
	for (i= 0; i<maxnode; i++) tabnode[i].passdst=false;
	statfr_unknown_dsts= false;
}

void
statfr_enable_dsts()
{
	int i;
	for (i= 0; i<maxnode; i++) tabnode[i].passdst=true;
	statfr_unknown_dsts= true;
}

void
statfr_enable_unknown_dsts()
{
	int i;
	for (i= 0; i<maxnode; i++) if istrue (tabnode[i].new) 
		tabnode[i].passdst=true;
	statfr_unknown_dsts= true;
}

void
statfr_disable_srcs()
{
	int i;
	for (i= 0; i<maxnode; i++) tabnode[i].passsrc=false;
	statfr_unknown_srcs= false;
}

void
statfr_enable_srcs()
{
	int i;
	for (i= 0; i<maxnode; i++) tabnode[i].passsrc=true;
	statfr_unknown_srcs= true;
}

void
statfr_enable_unknown_srcs()
{
	int i;
	for (i= 0; i<maxnode; i++) if istrue (tabnode[i].new) 
		tabnode[i].passsrc=true;
	statfr_unknown_srcs= true;
}

void
statfr_disable_ends()
{
	int i;
	for (i= 0; i<maxnode; i++) tabnode[i].passnod=false;
	statfr_unknown_ends= false;
}

void
statfr_enable_ends()
{
	int i;
	for (i= 0; i<maxnode; i++) tabnode[i].passnod=true;
	statfr_unknown_ends= true;
}

void
statfr_enable_unknown_ends()
{
	int i;
	for (i= 0; i<maxnode; i++) if istrue (tabnode[i].new) 
		tabnode[i].passnod=true;
	statfr_unknown_ends= true;
}

void
stat_reset_circuit_database()
{
	int i;

	maxcircuit=0;
	for (i=0; i< HSHSIZE; i++) {
		hshcirc[i]= NULL;
		hshcnt[i]= 0;
	}
}
