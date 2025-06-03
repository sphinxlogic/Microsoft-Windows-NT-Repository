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
#include	<file.h>
#include	<ctype.h>
#include	<descrip.h>
#include	<stdio.h>
#include	<string.h>

#if defined (__STDC__) || defined (__DECC)
#include	<stdlib.h>
#include 	<unixio.h>
#endif

#include 	"types.h"
#include	"monitor"
#include	"stat"
#include	"trace.h"
#include	"filter.h"

int	LIB$FIND_IMAGE_SYMBOL();

static ethernet_protocol_t analprotocol;

struct 	trace_opt_st trace_opt;

static	struct {boolean eve,rtl;} anal_opt;
static	analfile, outfile;
static	ushort link1,link2;
static  analyze_routine_t * analyze_rtn;

static
void put_line (buf,len)
char * buf;
int len;
{
	if (istrue(trace_opt.fil)) write (outfile,buf,len);
	else emon$put_with_scroll (buf,len);
}

typedef struct {ushort dst,src;} link_t;
typedef struct {unsigned seg:12; unsigned ack:4;} ack_t;
typedef struct {unsigned seg:12; unsigned rfu:4;} seg_t;

static 
ubyte * ack_nak(ucp, ucpe, bufack)
ubyte * ucp, * ucpe;
char * bufack;
{
	ack_t ack;
	do{
		ack= *(ack_t *) ucp;
		ucp+= sizeof ack;
		switch (ack.ack){
			case 0: sprintf (bufack," seg=%d",ack.seg); break;
			case 1: sprintf (bufack," seg=%d,noack",ack.seg); break;
			case 8: sprintf (bufack," ACK=%d",ack.seg); break;
			case 9: sprintf (bufack," NAK=%d",ack.seg); break;
			case 10: sprintf (bufack," XACK=%d",ack.seg); break;
			case 11: sprintf (bufack," XNAK=%d",ack.seg); break;
			default: sprintf (bufack," Unknown ack #%d=%d",ack.ack,ack.seg); break;
		}
		bufack += strlen (bufack);
	} while ( (ack.ack & 8) && (ucp<ucpe) );
	return ucp;
}

#define LINEHEADSIZE (2*(sizeof (node_name_t)-1)+ (sizeof(protocol_name_t) -1))

static ubyte * 
getascic (ucp,ucpe,buf,maxlen)
ubyte *ucp, *ucpe;
char * buf;
int maxlen;
{
	ubyte count= *ucp++;
	if ( (ucp+count > ucpe) || (count > maxlen) )
		strcpy (buf,"Illegal counted string");
	else{
		sprintf (buf,"%.*s", count, ucp);
		ucp += count;
	}
	return ucp;

}

static ubyte *
getobj (ucp,ucpe,buf)
char * buf;
ubyte *ucp, *ucpe;
{
	ubyte objnum= ucp[1];
	switch (*ucp){
	case 0:{
		sprintf (buf,"#%d",objnum);
		ucp += 2;
		break;
	}
	case 1: {
		char buf1[255];
		ucp= getascic (ucp+2,ucpe,buf1,16);
		sprintf (buf, "%d=%s", objnum, buf1);
		break;
	 }
	case 2:{
		char buf1[255];
		ulong uic= *( ulong *) ucp;
		ucp= getascic (ucp+6,ucpe,buf1,12);
		sprintf (buf, "%d=%8x %s", objnum, uic, buf1);
		break;
	 }
	default: {
		strcpy (buf,"unrecognizeable object");
	 }
	}
	return ucp;
}

static char * 
get_dc_reason(reason,buf)
ushort reason;
char * buf;
{
	switch (reason){
		case 0: strcpy (buf,"Normal disconnection"); break;
		case 1: strcpy (buf,"Insufficient resources"); break;
		case 9:	strcpy (buf,"abort");break;
		case 38: strcpy (buf,"Partner exited");
		case 41: strcpy (buf,"No link terminate");break;
		case 42: strcpy (buf,"Disconnect confirm");break;
		default: sprintf (buf,"unknown reason #%d",reason);
	}
	return buf;
}

static ubyte
unknownnspmess(buf,ucp,ucpe)
char * buf;
ubyte *ucp, *ucpe;
{
	int linepos= LINEHEADSIZE;
	sprintf(buf," Unrecognized NSP message");
	emon$dump (ucp, ucpe-ucp, buf, &linepos, 0, false);
	return linepos;
}

static getsrv (srv,buf)
ubyte srv;
char * buf;
{
	switch ( (srv>>2) & 3 ){
		case 0: sprintf (buf," No flow control"); break;
		case 1: sprintf (buf," Segment flow control"); break;
		case 2: sprintf (buf," Message flow control"); break;
		default: {
			sprintf (buf," Illegal flow control mode #%d",(srv>>2) & 3);
		}
	}
	if ( (srv & 0xf3) != 1) {
		char buf1[40];
		sprintf (buf1," Illegal service field %2x",srv);
		strcat (buf,buf1);
	}
}

static getinfo (info,buf)
ubyte info;
char * buf;
{
	switch (info){
		case 0: sprintf (buf," NSP version 3.2"); break;
		case 1: sprintf (buf," NSP version 3.1"); break;
		case 2: sprintf (buf," NSP version 4.0"); break;
		default: {
			char buf1[40];
			sprintf (buf1," unknown NSP version #%d",info);
			strcat (buf,buf1);
		}
	}
}

static
anal_nsp(buf, ucp, ucpe)
char * buf;
ubyte *ucp, *ucpe;
{

#define NSPLINEOFFSET	20
#define nspbuf	buf+NSPLINEOFFSET
#define NSPSESOFFSET	20

	char bufmess[256];
	int l1ok, l2ok;
	int linepos;

struct nspheader {ubyte control; link_t link;} * mp;


	if (istrue(anal_opt.eve)	/* filtrage des evenements session */
		&&   (((struct nspheader *) ucp)->control != 0x24)
		 && ((((struct nspheader *) ucp)->control &0x8)==0) ) return 0;

	l1ok= (link1==0) ||(((struct nspheader *) ucp) -> link.src== link1)
			 ||(((struct nspheader *) ucp) -> link.dst== link1);
	l2ok= (link2==0) ||(((struct nspheader *) ucp) -> link.src== link2)
			 ||(((struct nspheader *) ucp) -> link.dst== link2);

	if (   (((struct nspheader *) ucp)->control==0x24)   /* Connect ACK */
	     ||(((struct nspheader *) ucp)->control==0x18) ) /* Connect initiate */
		{
		if ( (!l1ok) && ((link2==0) || (!l2ok)) ) return 0;
		}
	else    if ( (!l1ok) || (!l2ok) ) return 0;

	/*	ok to proceed	print L2 layer message */

	put_line (buf,strlen(buf));

	memset (buf,' ',NSPLINEOFFSET);
	switch (*ucp & 0x8f){
	case	0:{
		mp= (struct nspheader *) ucp;
		switch ( (*ucp)>>4){
		case	0:
		case	2:
		case	4:
		case	6:
			strcpy (nspbuf, "Data MSG");
			sprintf (bufmess," %5d %5d",	mp->link.dst,mp->link.src);
			if (*ucp &0x20) strcat (bufmess," Eom");
			if (*ucp &0x40) strcat (bufmess," Bom");
			break;
		case	1:	strcpy (nspbuf,"Int. MSG");
		case	3:	strcpy (nspbuf,"L.S. MSG");
			sprintf (bufmess," %5d %5d",	mp->link.dst,mp->link.src);
			break;
		default: return unknownnspmess (nspbuf,ucp,ucpe);
		}
		ucp += sizeof (*mp);
		strcat (buf,bufmess);
		ucp= ack_nak (ucp,ucpe,bufmess);
		strcat (buf,bufmess);
		linepos= LINEHEADSIZE;
		emon$dump (ucp, ucpe-ucp, buf, &linepos, 0, false);
		return linepos;
		}
		break;


	case	4:{	/* Ack messages */
struct ackmsg {ubyte control; link_t lnk;} * mp;
			mp= (struct ackmsg *) ucp;
			switch ( (*ucp) >>4){
			case 0: sprintf (nspbuf,"Data ACK %5d %5d ",
					 mp->lnk.dst, mp->lnk.src);
				break;
			case 1: sprintf (nspbuf,"Oth. ACK %5d %5d ",
					 mp->lnk.dst, mp->lnk.src);
				break;
			case 2: sprintf (nspbuf,"Conn ACK %5d      ",mp->lnk.dst); break;
			default: return unknownnspmess(nspbuf,ucp,ucpe);
			}
			if ((*ucp)>>4 <2){
				ucp += sizeof (*mp);
				ucp= ack_nak(ucp,ucpe,bufmess);
				strcat (buf,bufmess);
			}
			else ucp += 2;
			linepos= LINEHEADSIZE;
			emon$dump (ucp, ucpe-ucp, buf, &linepos, 0, false);
			return linepos;
		}
	case	8:{
struct linkctl {ubyte control; link_t lnk;} * mp;
			switch ((*ucp)>>4){
			case 1:	{
				ubyte flags;
struct cnct {ubyte control; link_t link; ubyte srv,info; ushort segsiz;} * mp;
				mp= (struct cnct *) ucp;
				ucp += sizeof * mp;
				sprintf (nspbuf,"Conn ini %11d",mp->link.src);
				if (mp->link.dst != 0) strcat (nspbuf," Illegal dst link");
				getsrv (mp->srv,bufmess);
				strcat (nspbuf,bufmess);
				getinfo (mp->info,bufmess);
				strcat (nspbuf,bufmess);
				sprintf (bufmess, " Seg size=%4d",mp->segsiz);
				strcat (nspbuf,bufmess);
				put_line (buf, strlen(buf));
				memset (buf,' ',NSPLINEOFFSET+NSPSESOFFSET);
				strcpy (nspbuf+NSPSESOFFSET," for object ");
				ucp= getobj (ucp,ucpe,bufmess);
				strcat (nspbuf, bufmess);
				ucp= getobj (ucp,ucpe,bufmess);
				strcat (nspbuf," from object ");
				strcat (nspbuf, bufmess);
				flags= *ucp++;
				if (flags&1){
					ucp= getascic (ucp,ucpe,bufmess,39);
					if (bufmess[0]){
						put_line (buf, strlen(buf));
						memset (buf,' ',NSPLINEOFFSET+NSPSESOFFSET);
						sprintf(nspbuf+NSPSESOFFSET," User=%s",bufmess);
					}
					ucp= getascic (ucp,ucpe,bufmess,39);
					if (bufmess[0]){
						put_line (buf, strlen(buf));
						memset (buf,' ',NSPLINEOFFSET+NSPSESOFFSET);
						sprintf(nspbuf+NSPSESOFFSET," Pass=%s",bufmess);
					}
					ucp= getascic (ucp,ucpe,bufmess,39);
					if (bufmess[0]){
						put_line (buf, strlen(buf));
						memset (buf,' ',NSPLINEOFFSET+NSPSESOFFSET);
						sprintf(nspbuf+NSPSESOFFSET," Acct=%s",bufmess);
					}
				}
				if (flags&2) {
					put_line (buf, strlen(buf));
					memset (buf,' ',NSPLINEOFFSET+NSPSESOFFSET);
					sprintf  (nspbuf+NSPSESOFFSET," Opt. Data (%d bytes) =", *ucp++);
				}
			} break;
			case 2:	{
struct mp82 {ubyte control; link_t link; ubyte srv,info; ushort segsiz;} * mp;
				mp= (struct mp82 *) ucp;
				ucp += sizeof * mp;
				sprintf (nspbuf,"Conn cnf %5d %5d",
					mp->link.dst, mp->link.src);
				getsrv (mp->srv,bufmess);
				strcat (nspbuf,bufmess);
				getinfo (mp->info,bufmess);
				strcat (nspbuf,bufmess);
				sprintf (bufmess, " Seg size=%4d",mp->segsiz);
				strcat (nspbuf,bufmess);
				if (ucp < ucpe) {
					put_line (buf, strlen(buf));
					memset (buf,' ',NSPLINEOFFSET+NSPSESOFFSET);
					sprintf  (nspbuf+NSPSESOFFSET," Opt. Data (%d bytes) =", *ucp++);
				}
			} break;
			case 3:	{
struct	mp83 {ubyte control; link_t link; ushort reason;} * mp;
				mp= (struct mp83 *) ucp;
				ucp += sizeof (* mp);
				sprintf  (nspbuf,"Disc ini %5d %5d reason=%s",mp->link.dst, mp->link.src,
						 get_dc_reason(mp->reason,bufmess));
			} break;
			case 4:{
struct	mp84 {ubyte control; link_t link; ushort reason;} * mp;
				mp= (struct mp84 *) ucp;
				ucp += sizeof (* mp);
				switch (mp->reason){
				default: sprintf (nspbuf,"Disc cnf %5d %5d reason=%s",
						  mp->link.dst, mp->link.src,
						  get_dc_reason(mp->reason,bufmess)); break;
				case 1:  sprintf (nspbuf,"C    T   %5d", mp->link.dst); break;
				case 41: sprintf (nspbuf,"NL   T   %5d %5d",
						mp->link.dst, mp->link.src); break;
				case 42: sprintf (nspbuf,"D    T   %5d %5d",
						mp->link.dst, mp->link.src); break;
				}
			} break;
			case 5:	strcpy (nspbuf,"Start                "); ucp++; break;
			default: return unknownnspmess(nspbuf,ucp,ucpe);
			}
			linepos= LINEHEADSIZE;
			emon$dump (ucp, ucpe-ucp, buf, &linepos, 0, false);
			return linepos;
		}
	default: return unknownnspmess(nspbuf,ucp,ucpe);
	}				
}

static
void
anal_decnet(cp, qp, buf)
circuit_entry * cp;
pckt_t * qp;
char * buf;
{
	register ubyte * ucp= &qp->buffer[2], *ucpe;
	char bufmess[80];
	struct dsc$descriptor buf_d= {0, 0, 0, 0};
	char *msgtype;
	int linepos;

	buf_d.dsc$a_pointer= buf;
	ucpe= ucp+ (* (ushort *) & qp->buffer);

	if (*ucp&0x80) {
		int nbpad= 0x7f & *ucp;
		ucp+= nbpad;
		if (trace_opt.pad){
			sprintf (bufmess," %d pad byte%s",nbpad,(nbpad>1)?"s":"");
			strcat (buf,bufmess);
		}
	}
	if (*ucp&1){		/* control byte */
		if (isfalse(anal_opt.rtl)) return;
		switch (*ucp++){
			case	1:	msgtype= " Start         "; break;
			case	3:	msgtype= " Verification  "; break;
			case	5:	msgtype= " Test          "; break;
			case	7:	msgtype= " Routing       "; break;
			case	9:	msgtype= " Area routing  "; break;
			case   11:	msgtype= " Router hello  "; break;
			case   13:	msgtype= " End node hello"; break;
			default:	sprintf (bufmess," Unknown message type #%d", ucp[-1]);
				msgtype= bufmess;
		}
		strcat (buf,msgtype);
		linepos= LINEHEADSIZE;
		emon$dump (ucp,ucpe-ucp, buf, &linepos, 0, false);
		buf_d.dsc$w_length= linepos;
	}
	else{

#define TR3$V_MSG_CTL	0
#define TR3$M_MSG_CTL	(1<<0)
#define	TR3$V_MSG_RTH	1
#define	TR3$M_MSG_RTH	(1<<1)
#define	TR4$V_RTFLG_LNG	2
#define	TR4$M_RTFLG_LNG	(1<<2)
#define	TR4$V_RTFLG_VER	6
#define	TR4$M_RTFLG_VER	(1<<6)

typedef	struct	{ubyte d_area,d_subarea;
	 ethernet_addr_t d_id;
         ubyte s_area,s_subarea;
	 ethernet_addr_t s_id;
	 ubyte	nl2;
	 ubyte	visit_ct;
	 struct{
		unsigned	sclass_metr:1;
		unsigned	sclass_1:1;
		unsigned	sclass_ls:1;
		unsigned	sclass_suba:1;
		unsigned	sclass_bc:1;
		unsigned	sclass_57:3;
	} s_class;
	ubyte	pt;
	}	tr4$lngmsg_t;

		if (*ucp & TR3$M_MSG_RTH)
			if (*ucp & TR4$M_RTFLG_VER){
			}
			else{	/* route header	*/
				if (*ucp & TR4$M_RTFLG_LNG){	/*long*/
					register tr4$lngmsg_t * mp= 
						(tr4$lngmsg_t *) ucp+1;
					ucp += 1+sizeof (tr4$lngmsg_t);
					if (ucp > ucpe){
						linepos= 0;
						emon$dump (&qp->buffer[2],
							   ucpe- &qp->buffer[2],
							   buf,
							   &linepos, 0, false);
					}
					sprintf (bufmess, " route msg from %d.%d to %d.%d visits=%d",
						mp->s_id.decnet.area,mp->s_id.decnet.node,
						mp->d_id.decnet.area,mp->d_id.decnet.node,
						mp->visit_ct);
					if ( (* (ubyte *) &mp->s_class) | (mp->s_area) | (mp->d_area)
					     |(mp->s_subarea) | (mp->s_subarea) |(mp->pt)){
						linepos= 0;
						emon$dump (&qp->buffer[2],
							   ucpe- &qp->buffer[2],
							   buf,
							   &linepos, 0, false);
					}
					strcat (buf,bufmess);
				}
				else{				/* short */
					linepos= 0;
					emon$dump (&qp->buffer[2],
						   ucpe- &qp->buffer[2],
						   buf,
						   &linepos,
						   0, 
						   false);
				}
				buf_d.dsc$w_length= anal_nsp (buf,ucp,ucpe);
			/*	dump data	*/
			}	/* end route header */
	}/*	end data message */
	if (buf_d.dsc$w_length) put_line (buf,buf_d.dsc$w_length);
}

static
void
anal_ldmop(cp, qp, buf)
circuit_entry * cp;
pckt_t *qp;
char * buf;
{
#define	LINEHEADSIZE_MOP 40
	struct dsc$descriptor buf_d= {0, 0, 0, 0};
	int size= * (short *) &qp->buffer[0];
	ubyte *ucp= &qp->buffer[2];
	ubyte *ucpe;
	int linesize;
	
	buf_d.dsc$a_pointer= buf;
	ucpe= ucp+size;
	switch (*ucp++){
	case 2: strcat (buf," L/D memory load"); break;
	case 3: strcat (buf," L/D assistance volunteer"); break;
	case 8: strcat (buf," L/D request program"); break;
	case 10: strcat (buf," L/D request memory load"); break;
	case 20: strcat (buf," L/D parameter load with transfer address"); break;
	default: sprintf (buf+strlen(buf)," Unknown MOP function %#d", ucp[-1]);
	}
	linesize= LINEHEADSIZE_MOP;
	emon$dump (ucp, ucpe-ucp, buf, &linesize, 0, false);
	buf_d.dsc$w_length= linesize;
	if (buf_d.dsc$w_length) put_line (buf,buf_d.dsc$w_length);
	if (size+2 > qp->iosb.iosb_count) put_line ("Illegal frame length",20);
}
static
void
anal_appletalk (cp, qp, buf)
circuit_entry * cp;
pckt_t *qp;
char * buf;
{
#define	LINEHEADSIZE_ATK 40
	static header_done= 0;

#pragma nostandard
	struct ddph {
		struct {unsigned char lap_dest, lap_source, lap_type;} lap;
		variant_union {
		struct {unsigned :6; unsigned ddp1_length:10;
			unsigned char ddp1_dstsck, ddp1_srcsck;
			unsigned char ddp1_proto;} ddp1;
		struct {unsigned :2; unsigned ddp2_hopcnt:4; unsigned ddp2_length:10;
			unsigned short ddp2_checksum;
			unsigned short ddp2_dstnet;
			unsigned short ddp2_srcnet;
			unsigned char ddp2_srcnod, ddp2_dstnod;
			unsigned char ddp2_dstsck, ddp2_srcsck;
			unsigned char ddp2_proto;} ddp2;
		} _ddp_variant;
	} * ap;
#pragma standard

	unsigned char * dp;
	int dg_type, dg_size;
	int linepos;

	if (header_done==0) {
		char header[256];
		int i= strlen(buf);
		header_done++;
		sprintf (header, "%*s   socket     network      node  type", i, "");
		if istrue(trace_opt.fil) put_line (header, strlen (header));
		else	emon$title (header);
	}

	ap= (struct ddph *) &qp->buffer;

	switch (ap->lap.lap_type) {
	case 1: sprintf (buf+strlen(buf), " %3d->%3d %21s",
			 ap->ddp1.ddp1_srcsck, 
			 ap->ddp1.ddp1_dstsck, 
			"");
		dp= (unsigned char *) ap+sizeof ap->lap+sizeof ap->ddp1;
		dg_type= ap->ddp1.ddp1_proto;
		dg_size= ap->ddp1.ddp1_length;
		break;
	case 2: sprintf (buf+strlen(buf), 
			" %3d->%3d %5d->%5d %3d->%3d",
			 ap->ddp2.ddp2_srcsck, 
			 ap->ddp2.ddp2_dstsck, 
			 ap->ddp2.ddp2_srcnet, 
			 ap->ddp2.ddp2_dstnet,  
			 ap->ddp2.ddp2_srcnod, 
			 ap->ddp2.ddp2_dstnod);
		dp= (unsigned char *) ap+sizeof ap->lap+sizeof ap->ddp2;
		dg_type= ap->ddp2.ddp2_proto;
		dg_size= ap->ddp2.ddp2_length;
		break;
	default: sprintf (buf+strlen(buf)," Unknown LAP type %d",
			 ap->lap.lap_type);
		dp= (unsigned char *) ap;
		dg_size= qp->iosb.iosb_count;
		goto dump;
	}
	switch (dg_type) {
	case	1:	strcat (buf," RMTP"); break;
	case	2:	strcat (buf,"  NBP"); 	/* eclatage du paquet NBP */
		{	unsigned tupcnt= *dp & 0xf;
			int ctrl= *dp >>4;
			int nbp_id= dp[1];
			int i;
			unsigned char * tupptr;
			static const char * nbptyp[] = {"unknown nbp control #0", "BrRG","LkUP","LkUP-Reply"};

			if (ctrl <=3) sprintf (buf+strlen(buf), " %s ID=%d", nbptyp[ctrl], nbp_id);
			else sprintf (buf+strlen(buf), " unknown nbp control #%d ID=%d", ctrl, nbp_id);
			put_line(buf, strlen(buf));

			for (tupptr= dp+2,i=1; i<= tupcnt; i++) {
				int net, node, socket, num, obj, type, zonesize;
				char * zone;

				net= * (short *) tupptr;
				tupptr += 2;
				node= *tupptr++;
				socket= *tupptr++;
				num= *tupptr++;
				obj= tupptr[1];
				tupptr += *tupptr+1;
				type= tupptr[1];
				tupptr += *tupptr+1;
				zonesize= *tupptr++;
				zone= (char *) tupptr;
				tupptr+= zonesize;
				sprintf (buf,"%*s Tuple #%2d: net: %d nod: %d sckt: %d enum: %d object: %d type: %d zone: %.*s",-
					32," ",i, net, node, socket, num, obj, type, zonesize, zone);
				put_line(buf, strlen(buf));
			}
			return;
		}				
	case	3:	strcat (buf,"  ATP"); break;
	case	5:	strcat (buf,"  DSP"); break;
	default: sprintf (buf+strlen(buf)," ?%3d",dg_type);
	}
dump:	linepos= LINEHEADSIZE_ATK;
	emon$dump (dp, dg_size, buf, &linepos, 0, false);
	if (linepos) put_line (buf, linepos);
}

static
void
dump_packet(qp, cp, buf)
pckt_t *qp;
circuit_entry * cp;
char * buf;
{
	int linepos= strlen(buf);

	emon$dump (&qp->buffer, qp->iosb.iosb_count, buf, &linepos, 0, false);
	if (linepos) put_line (buf, linepos);
}

static boolean
anal_packet (qp,cp)
pckt_t * qp;
circuit_entry * cp;
{
	node_name_t	buff,buft;
	protocol_name_t	bufp;
	char bufmess[80];
	if (qp->iosb.iosb_devdep!=0x800){
		sprintf (bufmess,"*** Lost trace records, status=%#X",qp->iosb.iosb_devdep&~0x800);
		put_line (bufmess,strlen(bufmess));
		if (cp==0) return true;
	}

	if ((analprotocol.word==0) || 
	    (analprotocol.word== qp->tag.protocol.word)) {
		char buf[256];

		if (trace_opt.tim){
			struct dsc$descriptor bufmess_d;
			ushort timlen;
			int SYS$ASCTIM();

			bufmess_d.dsc$a_pointer= bufmess;
			bufmess_d.dsc$w_length= sizeof bufmess -1;
			SYS$ASCTIM (&timlen, &bufmess_d, &qp->time, 1);
			bufmess[timlen]= 0;
		}
		else bufmess[0]= 0;

		{
			register source_entry * sp= cp->srcptr;
			if (sp==0) {
				sp=cp->srcptr=addsource (&cp->source_addr,&cp->protocol);
				sp->nodptr= addnode (&sp->addr);
				sp->prtptr= addprotocol (&sp->protocol);
			}
			sp= cp->dstptr;
			if (sp==0) {
				sp=cp->dstptr=addsource (&cp->dest_addr,&cp->protocol);
				sp->nodptr= addnode (&sp->addr);
				sp->prtptr= addprotocol (&sp->protocol);
			}
		}

		sprintf (buf,"%s%s ->%s%s",
			bufmess,
			print_ethernet_addr(cp->srcptr->nodptr, &buff),
			print_ethernet_addr(cp->dstptr->nodptr, &buft),
			print_ethernet_protocol(cp->srcptr->prtptr, &bufp)
			);

	/* we have an extern definition of packets, used only */
	/* for callable images, and simpler. We must cast into the new type */

		if istrue(trace_opt.filter) {
			if isfalse( (* analyze_rtn) (
				(struct ethernet_packet_st *) qp,  buf, strlen(buf)) )
				mon_stop= true;
		}
		else	switch (analprotocol.word){
			case 0x0360:	anal_decnet(cp,qp,buf);	break;
			case 0x0160:	anal_ldmop(cp,qp,buf) ;	break;
			case 0x9b80:	anal_appletalk (cp, qp, buf); break;
			default:	dump_packet(qp,cp,buf);
		}
	}	/* if right protocol	*/
	return true;
}

int
cmd_analyze()
{
	cli_buf_t clibuf;
	node_entry * from, *to;
	protocol_entry * proto;
	int	stat_flags;
	int	status;
	init_routine_t * init_rtn;
	cleanup_routine_t * cleanup_rtn;
	struct	levelfilter_st l3, l4;
	int	analfile;
	pckt_t	filbuf;
	int	reclen;
	unsigned min_size;

	get_common_trace_options ();

	if isfalse(get_levelfilter (&l3, 3)) return false;
	if isfalse(get_levelfilter (&l4, 4)) return false;

	if (trace_opt.filter= getclistring ("filter", clibuf)) {
static const	$DESCRIPTOR (init_key, "EMON$INIT");
static const	$DESCRIPTOR (analyze_key, "EMON$ANALYZE");
static const	$DESCRIPTOR (cleanup_key, "EMON$CLEANUP");
		int status;
		struct dsc$descriptor image_desc= {0, 0, 0, 0};
		char img[255];
		vmsmsgbuf errbuf;

		sprintf (img, "EMON$FILTER_%.240s", clibuf);
		image_desc.dsc$w_length= strlen (img);
		image_desc.dsc$a_pointer= img;

		status= LIB$FIND_IMAGE_SYMBOL (&image_desc,
					 &analyze_key, &analyze_rtn);
		if isfalse(status) {
			emon$put_error (emon$translate_status(errbuf, status));
			free_levelfilter (&l3);
			free_levelfilter (&l4);
			return false;
		}

		status= LIB$FIND_IMAGE_SYMBOL (&image_desc,
					 &init_key, &init_rtn);
		if isfalse(status) init_rtn= NULL;
		status= LIB$FIND_IMAGE_SYMBOL (&image_desc,
					 &cleanup_key, &cleanup_rtn);
		if isfalse(status) cleanup_rtn= NULL;
	}
	else{
		if (l4.from !=NULL) link1= atol (l4.from);
		if (l4.to !=NULL) link1= atol (l4.to);
		if (l4.display!=NULL) {
			char ** cpp;
			for  (cpp= l4.display; *cpp != NULL; cpp++) {
				if (!strcmp (*l4.display, "events"))
					anal_opt.eve= true;
				else if (!strcmp (*l4.display, "routing"))
					anal_opt.rtl= true;
				else emon$print_error ("Unknown event %s",
							  *cpp);
			}
		}
	}

	if (trace_opt.size= getclistring ("min_size", clibuf)) {
		min_size= atol (clibuf);
		if (min_size <=0) {
			emon$put_error ("invalid /MIN_SIZE value");
			return false;
		}
	}

	if (trace_opt.fil= getclistring ("output",clibuf)){
		outfile= creat (clibuf,0660,"rfm=var","dna=.lis","rat=cr");
		if (outfile<0){
			emon$put_cerror ("Cannot open output file");
			return false;
		}
	}
	else	emon$dump_options (emon$put_with_scroll, pb_columns);

	getclistring ("input",clibuf);	/* obligatoire par le CLD */
	analfile= open (clibuf,O_RDONLY,"ctx=rec","dna=.trace");
	if (analfile < 0){
		emon$put_error ("cannot open input file");
		close (outfile);
		return false;
	}

	if (clipresent("initialize")) emon$erase_display (&scroll_coord);

	if (istrue(get_filter (&from, &to, &proto, &stat_flags))) {
		if (proto!=NULL) analprotocol= proto->protocol;

		emon$title("             From               To    Protocol Data...");
		emon$paste_display (&scroll_coord);
		if (istrue(trace_opt.filter) && (init_rtn != NULL)){
			if isfalse ( (*init_rtn) (&trace_opt,
						  &l3, &l4,
						  istrue(trace_opt.fil) 
							? 132 : pb_columns,
						  put_line)) {
				free_levelfilter (&l3);
				free_levelfilter (&l4);
				return false;
			}
		}
		if istrue(trace_opt.fil)
			emon$dump_options (put_line, 132);
		init_loc_stat (from, to, proto, anal_packet,
		      stat_flags | (istrue(trace_opt.fil)? 0: LOC_STAT_INITCP));
		mon_stop= false;
		while (reclen=read(analfile,&filbuf,sizeof filbuf)){
			if (istrue(mon_stop)) break;
			if (reclen==16) anal_packet (&filbuf,0);/* status msg */
			else loc_stat(&filbuf);
		}
		if istrue(trace_opt.fil)
			emon$dump_options (emon$put_with_scroll, pb_columns);
	}

	if (reclen<0) emon$put_error ("error reading file");

	if (istrue(trace_opt.filter) && (cleanup_rtn != NULL)) (*cleanup_rtn)();
	free_levelfilter (&l3);
	free_levelfilter (&l4);

	close (analfile);
	if istrue (trace_opt.fil) close (outfile);
	return true;
}
