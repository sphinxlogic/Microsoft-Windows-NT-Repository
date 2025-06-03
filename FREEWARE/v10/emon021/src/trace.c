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
#include	<ctype.h>
#include	<file.h>
#include	<stdio.h>
#include	<string.h>

#if defined (__DECC) || defined (__STDC__)
#include	<stdlib.h>
#endif

#include	"monitor.h"
#include	"stat.h"
#include	"trace.h"

#include	<descrip.h>
#include	<ssdef.h>

static	FILE * tracefile;
static	int min_size;
static	analyze_routine_t * analyze_rtn;

struct trace_opt_st trace_opt;
static	void	(*dump_output)()= emon$put_with_scroll;
int	  dump_width;

int	LIB$FIND_IMAGE_SYMBOL();

void
emon$dumpstr(
	char	* str,
	char	* buf,
	int	* curpos,
	int	startpos)
{
	int strsz= strlen (str);
	char *cp= buf+ *curpos;
	char *cps= str;

	for (;;) {		
		int segsize= pb_columns-(cp-buf);
		if ( strsz >  segsize){
			strncpy (cp, cps, segsize);
			emon$put_with_scroll (buf, pb_columns);
			if istrue(trace_opt.trunc) {
				*curpos= pb_columns;
				break;
			}
			memset (buf,' ', startpos);
			cp= buf+startpos;
			cps += segsize;
			strsz -= segsize;
		}
		else{
			strcpy (cp, cps);
			*curpos= cp+strsz-buf;
			break;
		}
	}
}

void
emon$dump_flush (char * cp, int length)
{
	(*dump_output) (cp, length);
}

void
emon$dump(
	void	* ucpv,
	int	nbbytes, 
	char	* buf,
	int	* curpos,
	int	startpos,
	int	dumpmode)
{
static const	char tabhex[]="0123456789ABCDEF";
	unsigned char * ucp= ucpv;
	register char *cp= buf + *curpos;
	char * linebeg= buf+startpos;		/* ou commencer les lignes */
	boolean	ascii= 
		istrue(dumpmode) ? (dumpmode & (DUMP$M_ASCII&~1)) != 0 : trace_opt.ascii;
	boolean	trunc= 
		istrue(dumpmode) ? (dumpmode & (DUMP$M_TRUNC&~1)) != 0 : trace_opt.trunc;
	char errbuf[80];

	if (nbbytes <=0) {
		sprintf (errbuf, "EMON$DUMP: invalid byte count (%d)", nbbytes);
		ascii= true;
		trunc= false;
		ucp= (ubyte *) errbuf;
		nbbytes= strlen (errbuf);
	}
		
	while ( nbbytes-- !=0) {
		if istrue(ascii) {
			if (cp >= buf+dump_width) {
				if istrue(trunc) break;
				(* dump_output) (buf, dump_width);
				memset (buf,' ', startpos);
				cp= linebeg;
			}
			*cp++= (isprint(*ucp))? *ucp : '.';
		}
		else{
			if (cp+3 > buf+dump_width) {
				if istrue (trunc) break;
				(* dump_output) (buf, cp-buf);
				memset (buf,' ', startpos);
				cp= linebeg;
			}
			*cp++= ' ';
			*cp++= tabhex[(*ucp)>>4];
			*cp++= tabhex[(*ucp)&15];
		}
		ucp++;
	}

	*curpos= cp-buf;
}
void
emon$dump_options (rtn, width)
void (*rtn)();
int width;
{
	dump_output= rtn;
	dump_width= width;
}

static char * 
strdup (char * buf)
{
	char * cp;
	cp= malloc (strlen(buf)+1);
	strcpy (cp, buf);
	return cp;
}

boolean
get_levelfilter (struct levelfilter_st * plf, int level)
{
	char varname[32];
	cli_buf_t buf;
	struct link {struct link * ptr; cli_buf_t buf;} * root= NULL;
	int numlink;
	int sizlink;
	
	plf->from = NULL;
	plf->to = NULL;
	plf->bothways= false;
	plf->protocol= NULL;
	plf->filter= NULL;
	plf->events= NULL;
	plf->display= NULL;

	sprintf (varname, "L%d.FROM", level);
	if (getclistring (varname,buf))	plf->from = strdup (buf);
	sprintf (varname, "L%d.TO", level);
	if (getclistring (varname,buf))	plf->to = strdup (buf);
	sprintf (varname, "L%d.END_ADDRESS", level);
	if (getclistring (varname,buf)){
		plf->bothways= true;
		plf->from = strdup (buf);
	}
	if (getclistring (varname,buf))	plf->to = strdup (buf);
	if (getclistring (varname,buf))	{
		emon$put_error ("Only 2 ends can be specified");
		free (plf->from);
		free (plf->to);
		return false;
	}
	sprintf (varname, "L%d.PROTOCOL", level);
	if (getclistring (varname, buf)) plf->protocol= strdup (buf);
	sprintf (varname, "L%d.FILTER", level);
	if (getclistring (varname, buf)) plf->filter= strdup (buf);
	sprintf (varname, "L%d.EVENTS", level);
	sizlink= 0;
	numlink = 0;
	while (getclistring (varname, buf)) {
		struct link * lp = malloc (sizeof (struct link));
		strcpy (lp->buf,buf);
		lp->ptr= root;
		root= lp;
		numlink++;
		sizlink += strlen (buf);
	}
	if (numlink != 0) {
		char ** p;
		char * q;
		plf->events= malloc ( (numlink+1) * sizeof (char *)
				      + numlink + sizlink);
		p= (char **) plf->events + numlink;
		q= (char *) (p+1);/* space after pointers is used by the */
		* p = NULL;	  /* string themselves. */
		while (root != NULL ) {
			struct link * r;
			strcpy (q, root->buf);
			*--p= q;
			q+= strlen(q) +1;
			r= root;
			root= root->ptr;
			free (r);
		}
	}

	sprintf (varname, "L%d.DISPLAY", level);
	sizlink= 0;
	numlink = 0;
	while (getclistring (varname, buf)) {
		struct link * lp = malloc (sizeof (struct link));
		strcpy (lp->buf,buf);
		lp->ptr= root;
		root= lp;
		numlink++;
		sizlink += strlen (buf);
	}
	if (numlink != 0) {
		char ** p;
		char * q;
		plf->display= malloc ( (numlink+1) * sizeof (char *)
				      + numlink + sizlink);
		p= (char **) plf->display + numlink;
		q= (char *) p+1;
		* p = NULL;
		while (root != NULL ) {
			struct link * r;
			strcpy (q, root->buf);
			*--p= q;
			q+= strlen(q) +1;
			r= root;
			root= root->ptr;
			free (r);
		}
	}
	return true;
}

void
free_levelfilter (struct levelfilter_st * plf)
{
	free (plf->from);
	free (plf->to);
	free (plf->filter);
	free (plf->protocol);
	free (plf->events);
	free (plf->display);
}

static boolean
trace_update()
{
	upd_stat();
	status_update();
	return true;
}

static
boolean
trace_print (p,e)
pckt_t * p;
circuit_entry *e;
{
	static const char traceline[]="%s%s%s%5d";
#define	LINEHEADSIZE 2*sizeof (node_name_t)+ sizeof (protocol_name_t) -3 +5
	char buf[255];
	int linesize;
	node_name_t source_buf,dest_buf;
	protocol_name_t proto_buf;
	ulong	enbast;
	int	SYS$ASCTIM(), SYS$SETAST();

	register ushort nbbytes= p->iosb.iosb_count;
	register ubyte *ucp= (ubyte *) &p->buffer;

	if (e==0) {	/* status message */
		if istrue (trace_opt.fil) fwrite (p, 
						 sizeof p->time+sizeof p->iosb,
						 1,
						 tracefile);
		return false;
	}

	if istrue (trace_opt.size) {
		if (nbbytes < min_size) return false;
	}

	if istrue (trace_opt.err) {
		if istrue (p->iosb.iosb_status) return;
	}

	if (istrue(trace_opt.pad)){
		if (nbbytes<2 ||
		    ( (nbbytes= * (ushort *) ucp) > p->iosb.iosb_count)){
			abort_monitor("Invalid padding");
			return false;
		} else ucp += 2;
	}

	if (istrue(trace_opt.fil)){
		fwrite ( p, 
			(istrue(trace_opt.pad)? nbbytes+2 : nbbytes) 
			     +sizeof p->time + sizeof p->iosb + sizeof p->tag,
			1,
			tracefile);
		return true;
	}

	enbast= SYS$SETAST(0);

	if (istrue(trace_opt.tim)){
		ushort timlen;
		struct dsc$descriptor buf_d= {0, 0, 0, 0};

		buf_d.dsc$w_length= sizeof buf;
		buf_d.dsc$a_pointer= (char *) buf;
		SYS$ASCTIM (&timlen, &buf_d, &p->time, 1);
		sprintf (buf+timlen,"%s%s%s%5d",
			print_ethernet_addr(e->srcptr->nodptr, &source_buf),
			print_ethernet_addr(e->dstptr->nodptr, &dest_buf),
			print_ethernet_protocol(e->srcptr->prtptr, &proto_buf),
			nbbytes);

		linesize= LINEHEADSIZE+timlen;
	}
	else{
		sprintf (buf,"%s%s%s%5d",
			print_ethernet_addr(e->srcptr->nodptr, &source_buf),
			print_ethernet_addr(e->dstptr->nodptr, &dest_buf),
			print_ethernet_protocol(e->srcptr->prtptr, &proto_buf),
			nbbytes);
	
		linesize= LINEHEADSIZE;
	}

	if istrue(trace_opt.iosb) {
		sprintf (buf+linesize," %8.8x",p->iosb.iosb_devdep);
		linesize += 9;
	}

	if istrue(trace_opt.trail) {
		sprintf (buf+linesize," [%2x%2x%2x%2x%2x%2x]",
				p->tag.padto20bytes[0],
				p->tag.padto20bytes[1],
				p->tag.padto20bytes[2],
				p->tag.padto20bytes[3],
				p->tag.padto20bytes[4],
				p->tag.padto20bytes[5]);
		linesize+= 15;
	}

	if (isfalse(p->iosb.iosb_status)) {
		vmsmsgbuf erbuf;

		emon$translate_status (erbuf, p->iosb.iosb_status);
		strcpy (buf+linesize, erbuf);
		linesize += strlen (erbuf);
	}

	/* we must cast the pointer into the simpler packet model */
	/* for the user interface */

	if istrue (trace_opt.filter) {
		if isfalse ( (*analyze_rtn) (
			 (struct ethernet_packet_st *) p, buf, linesize)
						){
			abort_monitor ("Filter requested abort");
			if (enbast==SS$_WASSET) SYS$SETAST(1);
			return false;
		}
		if (enbast==SS$_WASSET) SYS$SETAST(1);
		return true;
	}

	if (istrue(trace_opt.data)) 
		emon$dump (ucp, nbbytes, buf, &linesize, 20, false);

	emon$put_with_scroll (buf, linesize);

	if (enbast==SS$_WASSET) SYS$SETAST(1);
}

void
get_common_trace_options()
{

	trace_opt.tim= clipresent ("time");
	trace_opt.data= clipresent ("data");
	trace_opt.trunc= clipresent ("trunc");
	trace_opt.ascii= clipresent ("data.ascii");
	trace_opt.pad= clipresent ("pad");
	trace_opt.iosb= clipresent ("iosb");
	trace_opt.trail= clipresent ("802_trailer");
	trace_opt.err= clipresent ("error");
	trace_opt.debug=clipresent("debug");
	trace_opt.full=clipresent("full");
	trace_opt.brief=clipresent("brief");
}

int
cmd_trace()
{
	cli_buf_t buf;
	node_entry * from, *to;
	protocol_entry * proto;
	int  stat_flags;
	int	status;
	init_routine_t	* init_rtn;
	cleanup_routine_t * cleanup_rtn;
	struct levelfilter_st l3, l4;

	emon$dump_options (emon$put_with_scroll,pb_columns);

	get_common_trace_options();

	if (trace_opt.filter= getclistring ("filter", buf)) {
static const	$DESCRIPTOR (init_key, "EMON$INIT");
static const	$DESCRIPTOR (analyze_key, "EMON$ANALYZE");
static const	$DESCRIPTOR (cleanup_key, "EMON$CLEANUP");
		int status;
		struct dsc$descriptor image_desc= {0, 0, 0, 0};
		char img[255];
		vmsmsgbuf errbuf;

		if isfalse(get_levelfilter (&l3, 3)) return false;
		if isfalse(get_levelfilter (&l4, 4)) return false;

		sprintf (img, "EMON$FILTER_%.240s", buf);
		image_desc.dsc$w_length= strlen (img);
		image_desc.dsc$a_pointer= img;

		status= LIB$FIND_IMAGE_SYMBOL (&image_desc,
					 &analyze_key, &analyze_rtn);
		if isfalse(status) {
			emon$put_error (emon$translate_status(errbuf, status));
			return 0;
		}
		status= LIB$FIND_IMAGE_SYMBOL (&image_desc,
					 &init_key, &init_rtn);
		if isfalse(status) init_rtn= NULL;
		status= LIB$FIND_IMAGE_SYMBOL (&image_desc,
					 &cleanup_key, &cleanup_rtn);
		if isfalse(status) cleanup_rtn= NULL;
	}

	if (trace_opt.size= getclistring ("min_size", buf)) {
		min_size= atol (buf);
		if (min_size <=0) {
			emon$put_error ("invalid /MIN_SIZE value");
			return 0;
		}
	}

	if (trace_opt.fil= getclistring ("output",buf)){
		tracefile= fopen (buf, "wb", "rfm=var","dna=.trace","rat=blk");
		if (tracefile==NULL){
			emon$put_error ("Cannot open trace file");
			return 0;
		}
	}

	if (istrue (get_filter (&from, &to, &proto, &stat_flags))) {
		emon$title("             From               To    Protocol Data...");
		if (clipresent("initialize")) emon$erase_display (&scroll_coord);
		emon$paste_display (&scroll_coord);
		if (istrue (trace_opt.filter) && (init_rtn != NULL)) {
			if isfalse ( (*init_rtn) (&trace_opt,
						  &l3, &l4,
						  dump_width,
						  dump_output)) {
				free_levelfilter (&l3);
				free_levelfilter (&l4);
				return false;
			}
		}
		init_loc_stat (from,to,proto,trace_print,
		      stat_flags | (istrue(trace_opt.fil)? 0: LOC_STAT_INITCP));
		init_monitor(trace_update);
		lock_monitor(true);
		monitor(loc_stat);
	}

	if istrue (trace_opt.filter) {
		if (cleanup_rtn != NULL) (*cleanup_rtn)();
		free_levelfilter (&l3);
		free_levelfilter (&l4);
	}
	if (trace_opt.fil) fclose (tracefile);
}
