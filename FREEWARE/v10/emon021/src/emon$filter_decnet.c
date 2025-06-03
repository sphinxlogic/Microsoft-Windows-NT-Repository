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
#include <stdio.h>
#include "trace.h"

static	struct {
		boolean rtl, eve;
		} anal_opt;
static	struct  trace_opt_st trace_opt;
typedef char node_name_t[32];

typedef struct	{unsigned short dst, src;} link_t;
typedef struct {unsigned seg:12; unsigned ack:4;} ack_t;
typedef struct {unsigned seg:12; unsigned rfu:4;} seg_t;

static	link1, link2;

#define LINEHEADSIZE	2*(sizeof (node_name_t)-1)

static char * ack_nak(ucp,ucpe,bufack)
ubyte * ucp;
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


static ubyte *
unknownnspmess(buf,ucp,ucpe)
char * buf;
ubyte *ucp, *ucpe;
{
	int linepos;
	sprintf(buf," Unrecognized NSP message: ");
	linepos= strlen (buf);
	emon$dump (ucp, ucpe-ucp, buf, &linepos, 0, DUMP$M_HEX|DUMP$M_TRUNC);
	return linepos;
}

static void
invntwmsg (buf, linepos, ucp, ucpe)
char * buf;
int linepos;
{
	if (linepos!=0) emon$dump_flush (buf, linepos);
	sprintf(buf," Unrecognized NETWORK message");
	linepos= strlen (buf);
	emon$dump (ucp, ucpe-ucp, buf, &linepos, 0, false);
}

static getsrv (srv,buf)
ubyte srv;
char * buf;
{
	switch ( (srv>>2) & 3 ){
		case 0: sprintf (buf," FlowCtl: none"); break;
		case 1: sprintf (buf," FlowCtl: segment"); break;
		case 2: sprintf (buf," FlowCtl: message"); break;
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
	case 0: sprintf (buf," NSPvers: 3.2"); break;
	case 1: sprintf (buf," NSPvers: 3.1"); break;
	case 2: sprintf (buf," NSPvers: 4.0"); break;
	case 3: sprintf (buf," NSPvers: 4.1"); break;
	default:sprintf (buf," unknown NSP version #%d",info);
	}
}

static anal_nsp(buf, linepos, l3mess, ucp, ucpe)
int linepos;
char * buf, *l3mess;
ubyte *ucp, *ucpe;
{

#define NSPLINEOFFSET	10
#define nspbuf	buf+NSPLINEOFFSET
#define NSPSESSOFFSET	20

	char bufmess[256];
	int l1ok, l2ok;

struct nspheader {ubyte control; link_t link;} *mp;


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

	if (linepos) emon$dump_flush (buf, linepos);
	linepos= 0;
	emon$dump_flush (l3mess, strlen (l3mess));

	memset (buf,' ',NSPLINEOFFSET);
	switch (*ucp & 0x8f){
	case	0:{
		mp= ucp;
		switch ( (*ucp)>>4){
		case	0:
		case	2:
		case	4:
		case	6:
			strcpy (nspbuf, "Data MSG");
			sprintf (bufmess," %5d %5d",	mp->link.src,mp->link.dst);
			if (*ucp &0x20) strcat (bufmess," Eom");
			if (*ucp &0x40) strcat (bufmess," Bom");
			break;
		case	1:	strcpy (nspbuf,"Int. MSG");
		case	3:	strcpy (nspbuf,"L.S. MSG");
			sprintf (bufmess," %5d %5d",	mp->link.src,mp->link.dst);
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
struct {ubyte control; link_t lnk;} * mp;
			mp= ucp;
			switch ( (*ucp) >>4){
			case 0: sprintf (nspbuf,"Data ACK %5d %5d ",
					 mp->lnk.src, mp->lnk.dst);
				break;
			case 1: sprintf (nspbuf,"Oth. ACK %5d %5d ",
					 mp->lnk.src, mp->lnk.dst);
				break;
			case 2: sprintf (nspbuf,"Conn ACK %11d",mp->lnk.dst); break;
			default: return unknownnspmess(nspbuf,ucp,ucpe);
			}

			if ((*ucp)>>4 <2){
				ucp += sizeof (*mp);
				ucp= ack_nak(ucp,ucpe,bufmess);
				strcat (buf,bufmess);
			}
			else ucp += 3;

			linepos= NSPLINEOFFSET+NSPSESSOFFSET;
			if (ucpe!=ucp)
			     emon$dump (ucp, ucpe-ucp, buf, &linepos, 0, false);
			return linepos;
		}
	case	8:{
struct {ubyte control; link_t lnk;} * mp;
			switch ((*ucp)>>4){

			/* connect initiate */
			/* connect retransmit */

			case 1:	
			case 6:	{
				ubyte flags;
struct {ubyte control; link_t link; ubyte srv,info; ushort segsiz;} * mp;
				mp= ucp;
				ucp += sizeof * mp;
				if (mp->link.dst==0)
				    sprintf (nspbuf,"Conn %s%5d      ",
					(mp->control>>4==1) ? "ini " : "RINI",
					 mp->link.src);
				else {
				    sprintf (nspbuf,"Conn %s%5d %5d",
					(mp->control>>4==1) ? "ini " : "RINI",
					 mp->link.src,
					 mp->link.dst);
					 strcat (nspbuf," Illegal dst link");
				}
				getsrv (mp->srv,bufmess);
				strcat (nspbuf,bufmess);
				getinfo (mp->info,bufmess);
				strcat (nspbuf,bufmess);
				sprintf (bufmess, " SegSize=%4d",mp->segsiz);
				strcat (nspbuf,bufmess);
				emon$dump_flush (buf, strlen(buf));
				memset (buf,' ',NSPLINEOFFSET+NSPSESSOFFSET);
				strcpy (nspbuf+NSPSESSOFFSET," for object ");
				ucp= getobj (ucp,ucpe,bufmess);
				strcat (nspbuf, bufmess);
				ucp= getobj (ucp,ucpe,bufmess);
				strcat (nspbuf," from object ");
				strcat (nspbuf, bufmess);
				flags= *ucp++;
				if (flags&1){
					ucp= getascic (ucp,ucpe,bufmess,39);
					if (bufmess[0]){
						emon$dump_flush (buf, strlen(buf));
						memset (buf,' ',NSPLINEOFFSET+NSPSESSOFFSET);
						sprintf(nspbuf+NSPSESSOFFSET," User=%s",bufmess);
					}
					ucp= getascic (ucp,ucpe,bufmess,39);
					if (bufmess[0]){
						emon$dump_flush (buf, strlen(buf));
						memset (buf,' ',NSPLINEOFFSET+NSPSESSOFFSET);
						sprintf(nspbuf+NSPSESSOFFSET," Pass=%s",bufmess);
					}
					ucp= getascic (ucp,ucpe,bufmess,39);
					if (bufmess[0]){
						emon$dump_flush (buf, strlen(buf));
						memset (buf,' ',NSPLINEOFFSET+NSPSESSOFFSET);
						sprintf(nspbuf+NSPSESSOFFSET," Acct=%s",bufmess);
					}
				}

/*	flags & 4	= invokeproxy */
/* 	flags & 8	= proxyUIC */
/*	(flags>>5)&3	= session control version (0=1, 1=2) */

				if (flags&2) {
					emon$dump_flush (buf, strlen(buf));
					memset (buf,' ',NSPLINEOFFSET+NSPSESSOFFSET);
					sprintf  (nspbuf+NSPSESSOFFSET," Opt. Data (%d bytes) =", *ucp++);
				}
			} break;


			/* Connect confirm */

			case 2:	{
struct {ubyte control; link_t link; ubyte srv,info; ushort segsiz;} * mp;
				mp= ucp;
				ucp += sizeof * mp;
				sprintf (nspbuf,"Conn cnf %5d %5d",
					mp->link.src, mp->link.dst);
				getsrv (mp->srv,bufmess);
				strcat (nspbuf,bufmess);
				getinfo (mp->info,bufmess);
				strcat (nspbuf,bufmess);
				sprintf (bufmess, " SegSize=%4d",mp->segsiz);
				strcat (nspbuf,bufmess);
				if (ucp < ucpe) {
					int l= *ucp++;
					if (l) {
					     emon$dump_flush (buf, strlen(buf));
				 	     memset (buf,' ',NSPLINEOFFSET+NSPSESSOFFSET);
					     sprintf  (nspbuf+NSPSESSOFFSET,
						  " Opt. Data (%d bytes) =", l);
					}
				}
			} break;


			/* disconnect initiate */

			case 3:	{
struct	{ubyte control; link_t link; ushort reason;} * mp;
				mp= ucp;
				ucp += sizeof (* mp);
				sprintf  (nspbuf,"Disc ini %5d %5d reason=%s",
						 mp->link.src, mp->link.dst,
						 get_dc_reason(mp->reason,bufmess));
				if (ucp < ucpe) {
					int l= *ucp++;
					if (l) {
					     emon$dump_flush (buf, strlen(buf));
				 	     memset (buf,' ',NSPLINEOFFSET+NSPSESSOFFSET);
					     sprintf  (nspbuf+NSPSESSOFFSET,
						  " Opt. Data (%d bytes) =", l);
					}
				}
			} break;

			/* disconnect confirm */

			case 4:{
struct	{ubyte control; link_t link; ushort reason;} * mp;
				mp= ucp;
				ucp += sizeof (* mp);
				switch (mp->reason){
				case 1:	if (mp->link.src==0) {
					   sprintf (nspbuf,"Disc RSC %11d", 
					   mp->link.dst); break;
				}
				default: sprintf (nspbuf,"Disc     %5d %5d reason=%s",
						  mp->link.dst, mp->link.src,
						  get_dc_reason(mp->reason,bufmess)); 
					 break;
				case 2:  sprintf (nspbuf,"Disc syn %5d %5d", 
					     mp->link.dst, mp->link.src); break;
				case 42: sprintf (nspbuf,"Disc cnf %5d %5d",
					     mp->link.dst, mp->link.src); break;
				}
			 } break;

			/* start circuit */

			case 5:	strcpy (nspbuf,"Start                "); ucp++; 
				break;
			default: return unknownnspmess(nspbuf,ucp,ucpe);
			}
			linepos= strlen (buf);
			if (ucp<ucpe)
				emon$dump (ucp, ucpe-ucp, buf, &linepos, 
					   NSPLINEOFFSET, false);
			return linepos;
		}
	default: return unknownnspmess(nspbuf,ucp,ucpe);
	}				
}

boolean
EMON$ANALYZE (qp, buf, bufpos)
struct ethernet_packet_st * qp;
char * buf;
int bufpos;
{
	register ubyte * ucp, * ucpe;
	char bufmess[80];
	char *msgtype;
	int linepos;

	if (qp->protoeth != 0x0360) return true;

	linepos= bufpos;
	ucp= &qp->data[2];
	ucpe= ucp+ (* (ushort *) & qp->data);

	if (*ucp&0x80) {
		int nbpad= 0x7f & *ucp;
		ucp+= nbpad;
		if (trace_opt.pad){
			sprintf (bufmess, " %d pad byte%s",nbpad,(nbpad>1)?"s":"");
			emon$dumpstr (bufmess, buf, &linepos, 0);
		}
	}

	if (*ucp&1){		/* control byte */
		if (isfalse(anal_opt.rtl)) return true;
		if (linepos!=0) emon$dump_flush(buf, linepos);
		linepos= 0;
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
		sprintf (buf,msgtype);
		linepos= strlen(buf);
		emon$dump (ucp,ucpe-ucp, buf, &linepos, 0, false);
		linepos= 0;
	}
	else{
		char l3mess[256];

#define TR3$V_MSG_CTL	0
#define TR3$M_MSG_CTL	(1<<0)
#define	TR3$V_MSG_RTH	1
#define	TR3$M_MSG_RTH	(1<<1)
#define	TR4$V_RTFLG_LNG	2
#define	TR4$M_RTFLG_LNG	(1<<2)
#define	TR4$V_RTFLG_VER	6
#define	TR4$M_RTFLG_VER	(1<<6)

	 struct phIVaddr {unsigned node:10;
			  unsigned area:6;};

typedef	struct{	 ubyte d_area,d_subarea;
	 	 unsigned char fromhdr[4];
		 struct phIVaddr s_id;

	         ubyte s_area,s_subarea;
	 	 unsigned char tohdr[4];
		 struct phIVaddr d_id;
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
	} tr4$lngmsg_t;

		if (*ucp & TR3$M_MSG_RTH)
			if (*ucp & TR4$M_RTFLG_VER){
						invntwmsg (buf, linepos,
							   &qp->data[2], ucpe);
						return false;
			}
			else{	/* route header	*/
				if (*ucp & TR4$M_RTFLG_LNG){	/*long*/
					register tr4$lngmsg_t * mp= ucp+1;
					ucp += 1+sizeof (tr4$lngmsg_t);
					if (ucp > ucpe) {
						invntwmsg (buf, linepos,
							   &qp->data[2], ucpe);
						return false;
					}
					sprintf (l3mess, " route msg from %d.%d to %d.%d visits=%d",
						mp->d_id.area,mp->d_id.node,
						mp->s_id.area,mp->s_id.node,
						mp->visit_ct);
					if ( (* (ubyte *) &mp->s_class) | (mp->s_area) | (mp->d_area)
					     |(mp->s_subarea) | (mp->s_subarea) |(mp->pt)){
						invntwmsg (buf, linepos,
							   &qp->data[2], ucpe);
						return false;
					}
				}
				else{				/* short */
						invntwmsg (buf, linepos,
							   &qp->data[2], ucpe);
						return false;
				}
				linepos= anal_nsp (buf,linepos,l3mess,ucp,ucpe);
			/*	dump data	*/
			}	/* end route header */
			else {
						invntwmsg (buf, linepos,
							   &qp->data[2], ucpe);
						return false;
			}
	}/*	end data message */
	if (linepos!=0) emon$dump_flush (buf, linepos);
	return true;
}

boolean
EMON$INIT(struct trace_opt_st * trptr,
		struct levelfilter_st *l3, 
		struct levelfilter_st *l4,
		int width,
		void (* flush_rtn)())
{
	trace_opt= *trptr;

	if (l4->from !=NULL) link1= atol (l4->from);
	if (l4->to !=NULL) link1= atol (l4->to);
	if (l4->display!=NULL) {
		char ** cpp;
		for  (cpp= l4->display; *cpp != NULL; cpp++) {
			if (!strcmp (*l4->display, "events"))
				anal_opt.eve= true;
			else if (!strcmp (*l4->display, "routing"))
				anal_opt.rtl= true;
			else{
				emon$print_error ("Unknown event %s",
						  *cpp);
				return false;
			}
		}
	}

	return true;
}

void
EMON$CLEANUP()
{
}
