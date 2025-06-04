/*									*/
/*	Copyright (©) Ecole Nationale Supérieure des Télécommunications */
/*									*/
/*	EMON: moniteur ethernet	(V2.1-3)				*/
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
#include	assert
#include	ctype
#include	file
#include	unixio
#include	descrip
#include	"types.h"
#include	"trace.h"

typedef char node_name_t[18];
typedef char proto_name_t [13];

#pragma member_alignment save
#pragma nomember_alignment

typedef struct {
		unsigned mbz:2;	/* must be 0 */
		unsigned R:1;	/* High Reliability */
		unsigned T:1;	/* High Throughput */
		unsigned D:1;	/* Low Delay */
		unsigned precedence:3; /* priority 0: normal; 7: high priority*/
	}	precedence_t;

typedef struct {
		unsigned len:4;
		unsigned vers:4;
		precedence_t tos;	/* type of service */
		unsigned short	total_length;
		unsigned short	ident;
		unsigned short frgctl;	/* fragment control: format net: offH:4; flags:4; offL:8 */
		unsigned char time, proto;
		unsigned short checksum;
		unsigned char from[4];
		unsigned char to[4];
		unsigned char options[255];
		} ip_t;
#define IP_ICMP 1
#define IP_TCP 6
#define IP_UDP 17
#define IP_AUXS 127
#define IP_REXEC 128
#define IP_RAWIP 255

typedef struct	{
		unsigned short	source, destination;
		unsigned long seqnum, acknum;
		unsigned res:4;
		unsigned off:4;
		unsigned fin:1;
		unsigned syn:1;
		unsigned rst:1;
		unsigned psh:1;
		unsigned ack:1;
		unsigned urg:1;
		unsigned resf6:1;
		unsigned resf7:1;
		unsigned short window;
		unsigned short checksum;
		unsigned short urgent;
		} tcp_t;

typedef struct	{
		unsigned short	source, destination;
		unsigned short  length, checksum;
	        } udp_t;

typedef struct	{
		unsigned char	type, code;
		unsigned short	checksum;
		variant_union {
			unsigned char gw[4];
			unsigned long lw1;
			variant_struct {
				unsigned short identifier, sequence;
			} icmp$$struct1;
		} icmp$$union_1;
		unsigned char data[1];
		} icmp_t;

#pragma member_alignment restore
		
#define	IP_HEADER_LENGTH (sizeof (ip_t))

#define TCP_HEADER_LENGTH 8
#define	TIM_SIZE 11
#define PROTO_SIZE (sizeof (proto_name_t) -1)
#define NOD_SIZE (sizeof (node_name_t) -1)

static	struct trace_opt_st trace_opt;
static	pb_columns;
static	char	ipstr [120];
static	char	tcpstr [60];
static	char	udpstr [60];
static	char	icmpdefstr [60];
static	char	icmp3str [60];
static	char	icmp4str [60];
static	char	icmp5str [60];
static	char	icmp11str [60];
static	char	icmp12str [60];
static	int	iphdrsize;
static	int	tcphdrsize;
static	int	udphdrsize;
static	int	icmpdefhdrsize;
static	int	icmp3hdrsize;
static	int	icmp4hdrsize;
static	int	icmp5hdrsize;
static	int	icmp11hdrsize;
static	int	icmp12hdrsize;

static boolean	ipfrom_f;	/* filter ip according to source address */
static boolean	ipto_f;		/* filter ip according to destination address */
static boolean	icmp_f,		/* filter ip accoring to ICMP packet type */
		icmp_redirect,
		icmp_unreachable,
		icmp_timestamp,
		icmp_echo,
		icmp_quench,
		icmp_other;
static boolean	ip_bothways;
static boolean	ipproto_f;	/* ...................... protocol */
static boolean  socket_f;
static boolean  full;
static unsigned char ipfrom[4];
static unsigned char ipto[4];
static unsigned	ipproto_n;
static enum {check_from, check_to, check_both,
		check_any1, check_any2} socket_checktype;
static unsigned short socket_nf;	/* en format network */
static unsigned short socket_nt;	/* en format network */

static unsigned short
ntohs (unsigned short netshort)
{
	return ((netshort&0xFF)<<8) | (netshort >>8);
}
#define htons ntohs

static unsigned long
ntohl (unsigned long netlong)
{
#pragma member_alignment save
#pragma nomember_alignment
	union magic_un {unsigned char bytes[4];
	       		unsigned long value;
              	       } magic, arg;
#pragma member_alignment restore
	arg.value= netlong;
	magic.bytes[0] = arg.bytes[3];
	magic.bytes[1] = arg.bytes[2];
	magic.bytes[2] = arg.bytes[1];
	magic.bytes[3] = arg.bytes[0];
	return magic.value;
}

/* check if a packet is eligible for dump; socket_f must be true */

static boolean
check_socket (src, dst)
unsigned short src,dst;
{
	switch (socket_checktype) {
	case check_from:
		return src==socket_nf;
	case check_to:
		return dst==socket_nt;
	case check_both:
		return (src==socket_nf) && (dst==socket_nt);
	case check_any1:
		return (src==socket_nf) || (dst==socket_nf);
	case check_any2:
		return ((src==socket_nf) && (dst==socket_nt))
		     ||((dst==socket_nf) && (src==socket_nt));
	default:;
	}
	/* y a un bug */
	return true;
}

static void
dump_data (socknum, bufadr, bufsiz, editbuf, inipos)
unsigned short	socknum;	/* dest socket */
unsigned char * bufadr;		/* @ of data */
int		bufsiz;		/* size of data */
char *		editbuf;	/* @ of buffer */
int		inipos;		/* position initiale du buffer */
{
	int	linepos= 0;	/* empty edit buffer */
	switch (ntohs (socknum)) {
	case 520:{	/* RIP */
		struct { unsigned char command, version;
			 unsigned short reserved;
			 struct netarray_st {
				 unsigned short nettype;
				 unsigned char netadr[14];
				 unsigned long netdist;
				} netarray[1];
			} * rip_msg= bufadr;
		struct netarray_st * np;
		char errmsg[80];

		switch (rip_msg->command) {
		case 1:	sprintf (errmsg,"RIP request"); break;
		case 2: sprintf (errmsg,"RIP response"); break;
		default: sprintf (errmsg,"RIP UNKNOWN command %d",
			 rip_msg->command);
		}
		sprintf (editbuf, "%s (version %d) Type          Net  Distance%s",
				   errmsg, rip_msg->version,
				   rip_msg->reserved==0 ? "" :
					" RESERVED HEADER AREA NOT NULL");
		emon$dump_flush (editbuf, strlen (editbuf));
		np= rip_msg->netarray;
		bufsiz -= sizeof rip_msg->command+sizeof rip_msg->version
			 +sizeof rip_msg->reserved;
		linepos= 0;

		while (bufsiz >= sizeof * np) {
			char * type[10];
			char * addr[80];

			memset (editbuf, ' ', inipos);
			if (    (ntohs(np->nettype)==2) &&
				(*(unsigned short *) np->netadr ==0) &&
				(*(unsigned long *) & np->netadr[6] ==0) &&
				(*(unsigned long *) & np->netadr[10] ==0) ) {
				strcpy (type, "INET");
				sprintf (addr, "%d.%d.%d.%d",
				 	np->netadr[2], np->netadr[3],
					np->netadr[4], np->netadr[5]);
			}
			else {
				sprintf (type, "%04X", ntohs(np->nettype));
				sprintf (addr,
	"%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
	np->netadr[0],np->netadr[1],np->netadr[2],np->netadr[3],np->netadr[4],
	np->netadr[5],np->netadr[6],np->netadr[7],np->netadr[8],np->netadr[9],
	np->netadr[10],np->netadr[11],np->netadr[12],np->netadr[13]);
			}
			sprintf (editbuf+inipos, "%4s %15s %lu",
				 type, addr, ntohl(np->netdist));
			emon$dump_flush (editbuf, strlen(editbuf));
			linepos= 0;

			np ++;
			bufsiz -= sizeof *np;
		}
		if (bufsiz!=0) emon$dumpstr ("PACKET END MISCALCULATED",
					      editbuf, &linepos, 0);
		break;
			
	 }	/* end case RIP */

	default: {
		emon$dump (bufadr,	/* adresse zone a dumper */
			   bufsiz,	/* taille zone a dumper */
			   editbuf,	/* adresse buffer */
			   &linepos,	/* indx position libre du buffer */
			   inipos,	/* index de repli */
			   false);	/* options par defaut */
	 }	/* end case DEFAULT */

	}	/* end switch */

	if (linepos !=0) emon$dump_flush (editbuf, linepos);
}

static
boolean
format_ip_header (	ip_t 	* ip,		/* R adresse paquet IP */
			char 	* buf,		/* R adresse buffer resultat */
			int 	* ipbufsize,	/* M index fin du buffer */
			int	inipos)		/* R decalage pour repli */
{
	node_name_t fromaddr, toaddr;	/* pour le cadrage */
	char	protoname[20];
	int	linesize= * ipbufsize;
	int	optsize;
#define	IPHEAD_START 40
	switch (ip->proto) {
	case IP_ICMP:	strcpy (protoname,"ICMP"); break;
	case IP_TCP:	strcpy (protoname,"TCP"); break;
	case IP_UDP: strcpy (protoname,"UDP"); break;
	case IP_RAWIP: strcpy (protoname,"RAW IP"); break;
	default: sprintf (protoname,"proto #0d%d", ip->proto);
	}

	sprintf (fromaddr,"%d.%d.%d.%d",ip->from[0], ip->from[1], ip->from[2],
				        ip->from[3]);
	sprintf (toaddr,"%d.%d.%d.%d",ip->to[0], ip->to[1], ip->to[2],
				        ip->to[3]);

	sprintf (buf+*ipbufsize, ipstr, fromaddr, toaddr, protoname, ip->time, 
			ntohs(ip->ident),
			ip->vers, ip->len,
			ip->tos.precedence, ip->tos.D, ip->tos.T, ip->tos.R,
			ntohs(ip->frgctl) >> 12, ntohs(ip->frgctl)&0xfff,
			ntohs(ip->checksum) );

	linesize= *ipbufsize+iphdrsize;

	/* options stuff */

	optsize= 4*ip->len- ((char *) &ip->options - (char *) ip);

	if (optsize != 0) {
		unsigned char * ucp1= &ip->options;
		char bufopt[255];
		int	savsize, savpos;

		emon$dumpstr ("Options: ", buf, &linesize, 20);
		savpos= linesize;
		savsize= optsize;

		while (optsize>0) {
			sprintf (bufopt, "C: %02X; S:%3d", ucp1[0], ucp1[1]);
			emon$dumpstr (bufopt, buf, &linesize, 20);
			optsize -= ucp1[1];
			if (optsize <0) break;
			emon$dump (ucp1+2, ucp1[1]-2, buf, &linesize, 20, true);
			ucp1 += ucp1[1];
		}
		if (optsize < 0) {
			linesize= savpos;
			emon$dump (&ip->options, savsize, &linesize, buf, 20, 
				   DUMP$M_HEX);
		}
		
	}
	* ipbufsize= linesize;
}

/******************************************************************************/
/*	ANALYZE routine							      */
/* this boolean routine will stop the tracer if it returns false              */
/*	messages cannot be output via emon$*error routines		      */
/******************************************************************************/
boolean	/* analyze_routine_t */
EMON$ANALYZE  (struct ethernet_packet_st * pkt, char * buf, int inipos)
{
	int 	linesize=inipos;
	int	ipsize;			/* taille header IP */
	char	bufip[255];		/* buffer pour header ip */
	int	ipbufsize= 0;		/* index car. libre dans bufip */
	unsigned short	nbbytes  = 	pkt->iosb_length;
	unsigned short socknum;		/* socket protocol # */
	unsigned char * ucp=	pkt->data;
	ip_t * ip = ucp;

	if (pkt->protoeth != 0x0008) return true;	/* check right proto */

	if istrue (ipfrom_f)
		if (*(long*) ipfrom != *(long *) ip->from)
		   if istrue (ip_bothways){
			if ( (* (long *) ipto != *(long *) ip->from))
					return true;
		   }  else return true;
	if istrue (ipto_f)
		if (*(long*) ipto != *(long *) ip->to)
		   if istrue (ip_bothways){
			if ( (* (long *) ipfrom != *(long *) ip->to))
					return true;
		   }  else return true;

	if istrue (ipproto_f)
		if (ip->proto != ipproto_n) return true;


	ipsize= 4*ip->len;
	ucp+= ipsize;
	nbbytes= ntohs(ip->total_length)-ipsize;

	if istrue (socket_f)	/* do filter checks before formatting routines*/
		switch (ip->proto){
			case IP_TCP:{
				tcp_t * tcp= ucp;
				if isfalse (check_socket (tcp->source,
							  tcp->destination))
					return true;
				break;
			}
			case IP_UDP:{
				udp_t * udp= ucp;
				if isfalse (check_socket (udp->source,
							  udp->destination))
					return true;
				break;
			}
			case IP_ICMP: return true;
			default:;
		}	/* end switch */
	else
		if istrue (icmp_f)
			if (ip->proto == IP_ICMP) {

			 icmp_t * icmp= ucp;
			 switch (icmp->type) {
			 case	0:
			 case	8: if isfalse(icmp_echo) return true; break;
			 case	3: if isfalse(icmp_unreachable) return true; break;
			 case	4: if isfalse(icmp_quench) return true; break;
			 case	5: if isfalse(icmp_redirect) return true; break;
			 case   13:
			 case   14: if isfalse(icmp_timestamp) return true; break;
			 default:   if isfalse(icmp_other) return true; break;
			 } /* switch */
			}	/* proto = ICMP */
			else return true;

		/* packet is now valid for displaying */

	emon$dump_flush (buf, linesize);	/* flush ethernet header */
	(void) format_ip_header (ip, bufip, & ipbufsize, inipos);/* IP header */

	/* data stuff */

	switch (ip->proto) {
	case IP_TCP:{	tcp_t * tcp= ucp;
			char flags[4*8+1];

			if (ipbufsize!=0) emon$dump_flush (bufip, ipbufsize);
			flags[0]= 0;
			if istrue (tcp->resf7) strcat (flags,"?#7 ");
			if istrue (tcp->resf6) strcat (flags,"?#6 ");
			if istrue (tcp->urg) strcat (flags,"Urg ");
			if istrue (tcp->ack) strcat (flags,"Ack ");
			if istrue (tcp->psh) strcat (flags,"Psh ");
			if istrue (tcp->rst) strcat (flags,"Rst ");
			if istrue (tcp->syn) strcat (flags,"Syn ");
			if istrue (tcp->fin) strcat (flags,"Fin ");
			sprintf (buf, tcpstr,	ntohs(tcp->source),
						ntohs(tcp->destination),
						flags, 
						ntohl(tcp->seqnum),
						ntohl(tcp->acknum));
			linesize= tcphdrsize;
			if istrue(full)
				emon$dump (ucp+12, 4*tcp->off-12, 
				           buf, &linesize, 20, false);
			ucp= ucp+4*tcp->off;
			nbbytes-= 4*tcp->off;	/* IP data size - TCP hdr size*/
			socknum= tcp->destination;
			break;
		}
	case IP_UDP:{	udp_t * udp= ucp;
			int size;

			if (ipbufsize!=0) emon$dump_flush (bufip, ipbufsize);
			size= 	ntohs(udp->length) -8;
			sprintf (buf, udpstr, ntohs(udp->source), 
					      ntohs(udp->destination),
					      size);
			linesize= udphdrsize;
			emon$dump (ucp+4, 4, buf, &linesize, 20, false);
			ucp= ucp+8;
			nbbytes= size;
			socknum= udp->destination;
			break;
		}
	case IP_ICMP:{	icmp_t * icmp= ucp;

			if (ipbufsize!=0) emon$dump_flush (bufip, ipbufsize);
			switch (icmp->type) {
			case	3: {	/* unreachable */
				char * name;
				char bufcode[10];
				ip_t * ip;

				switch (icmp->code) {
				case	0: name= "Net"; break;
				case	1: name= "Host"; break;
				case	2: name= "Protocol"; break;
				case	3: name= "Port"; break;
				case	4: name= "Frag/DF"; break;
				case	5: name= "Source rt"; break;
				default:sprintf (bufcode, "Code #%d",
						 icmp->code);
					name= bufcode;
				}
				sprintf (buf, icmp3str, name);
				ip= icmp->data;
				emon$dump_flush (buf, icmp3hdrsize);
				strcpy (buf, "* ");
				linesize= 2;
				format_ip_header (ip, buf, &linesize, inipos);
				ucp= (char *) ip+4*ip->len;
				nbbytes -= 8+4*ip->len;
				break;
			}
			case	4: {	/* quench */
				ip_t * ip;

				sprintf (buf, icmp4str);
				emon$dump_flush (buf, icmp4hdrsize);
				strcpy (buf, "* ");
				linesize= 2;
				ip= icmp->data;
				format_ip_header (ip, buf, &linesize, inipos);
				ucp= (char *) ip+4*ip->len;
				nbbytes -= 8+4*ip->len;
				break;
			}
			case	5: {	/* redirect */
				char * name;
				char bufcode[10];
				node_name_t gwaddr;
				ip_t * ip;

				switch (icmp->code) {
				case	0: name= "Net"; break;
				case	1: name= "Host"; break;
				case	2: name= "Net/Service"; break;
				case	3: name= "Host/Service"; break;
				default:sprintf (bufcode, "Code #%d",
						 icmp->code);
					name= bufcode;
				}
				sprintf (gwaddr,"%d.%d.%d.%d",
					 icmp->gw[0], icmp->gw[1], icmp->gw[2],
					 icmp->gw[3]);
				sprintf (buf, icmp5str, name, gwaddr);
				ip= icmp->data;
				emon$dump_flush (buf, icmp5hdrsize);
				strcpy (buf, "* ");
				linesize= 2;
				format_ip_header (ip, buf, &linesize, inipos);
				ucp= (char *) ip+4*ip->len;
				nbbytes -= 8+4*ip->len;
				break;
			}
			case	11: {	/* lost */
				char * name;
				char bufcode[10];
				node_name_t gwaddr;
				ip_t * ip;

				switch (icmp->code) {
				case	0: name= "TTL exceeded"; break;
				case	1: name= "Reassembly timeout"; break;
				default:sprintf (bufcode, "Code #%d",
						 icmp->code);
					name= bufcode;
				}
				sprintf (buf, icmp11str, name);
				emon$dump_flush (buf, icmp11hdrsize);
				ip= icmp->data;
				strcpy (buf, "* ");
				linesize= 2;
				format_ip_header (ip, buf, &linesize, inipos);
				ucp= (char *) ip+4*ip->len;
				nbbytes -= 8+4*ip->len;
				break;
			}
			case	12: {	/* erroneous */
				ip_t * ip;

				sprintf (buf, icmp12str);
				emon$dump_flush (buf, icmp12hdrsize);
				ip= icmp->data;
				strcpy (buf, "* ");
				linesize= 2;
				format_ip_header (ip, buf, &linesize, inipos);
				ucp= (char *) ip+4*ip->len;
				nbbytes -= 8+4*ip->len;
				break;
			}
			default:sprintf (buf, icmpdefstr,
					 icmp->type, icmp->code, icmp->lw1);
				linesize= icmpdefhdrsize;
				ucp += 8;
				nbbytes -= 8;
			}
			socknum= 0;
		break;
	}
	default:
		if (ipbufsize!=0) emon$dump_flush (bufip, ipbufsize);
		linesize= 0;
		socknum= 0;
	}

	        /* socknum= socket # of destination */
	     /* ucp= data start; nbbytes= data length */
	/* linesize = formatted size of buf not yet printed */

	if (linesize!=0) emon$dump_flush (buf, linesize);
	linesize= 0;

	inipos= 20;
	if (istrue(trace_opt.data) && (nbbytes > 0) )
		dump_data (socknum, ucp, nbbytes, buf, inipos);

	return true;
}

/******************************************************************************/
/*	INIT routine							      */
/* this boolean routine will abort the trace request if it returns false      */
/*	messages can be output via emon$put*message routines		      */
/******************************************************************************/
/* init_routine_t */
boolean
EMON$INIT (traceadr, l3f, l4f, screen_width, output_rtn)
struct trace_opt_st * traceadr;
int screen_width;
struct levelfilter_st * l3f, *l4f;
boolean (*output_rtn)();
{
	globalvalue	SS$_DEBUG;

	assert (sizeof (precedence_t) == 1);
	assert (sizeof (short) == 2);
	assert (sizeof (ip_t) == 20 + 255);
	assert (sizeof (struct ethernet_packet_st) == 1536);

	trace_opt= * traceadr;
	pb_columns= screen_width;

	if istrue (trace_opt.debug) LIB$SIGNAL (SS$_DEBUG);

	full= istrue (trace_opt.full);

assert(sizeof (node_name_t) > (4*3+3));	/* 4 octets a 2 chiffres et 3 points */

	sprintf (ipstr, 
"%*s%%%ds%%%ds%%%ds TTL%%4d SEQ%%6d Vers:%%2d len:%%2d Prio:%%d D/T/R:%%d/%%d/%%d flags:%%2d offset:%%4d chk:%%5d",
			istrue (trace_opt.tim)? TIM_SIZE : 0,
			"IP hdr:  ", NOD_SIZE, NOD_SIZE, PROTO_SIZE);
    iphdrsize= PROTO_SIZE+2*NOD_SIZE+ (istrue(trace_opt.tim)?TIM_SIZE:0)+18;
	if istrue(full)	iphdrsize += 65;
assert(strlen(ipstr) < sizeof ipstr);

	sprintf (tcpstr, "%*s%%%dd%%%dd%%24sSEQ:%%11u ACK:%%11u",
			istrue (trace_opt.tim)? TIM_SIZE : 0,
			"TCP hdr:  ", NOD_SIZE, NOD_SIZE);
	tcphdrsize= 2*NOD_SIZE+ (istrue(trace_opt.tim)?TIM_SIZE:0)+24+32;
assert(strlen(tcpstr) < sizeof tcpstr);
	sprintf (udpstr, "%*s%%%dd%%%dd%24sDsize:%%5d",
			istrue (trace_opt.tim)? TIM_SIZE : 0,
			"UDP hdr:  ", NOD_SIZE, NOD_SIZE,"");
	udphdrsize= 2*NOD_SIZE+ (istrue(trace_opt.tim)?TIM_SIZE:0)+24+11;
assert(strlen(udpstr) < sizeof udpstr);
	sprintf (icmpdefstr, "ICMP hdr: type= 0d%%-3u code= 0d%%-3u, lw1= %%08X");
	icmpdefhdrsize= strlen (icmpdefstr);
assert(strlen(icmpdefstr) < sizeof icmpdefstr);
	sprintf (icmp5str, "ICMP redirect %%-%dsgw: %%%ds",
			    (istrue (trace_opt.tim)? TIM_SIZE : 0 )+NOD_SIZE-14,
			    NOD_SIZE-4);
	icmp5hdrsize= 2*NOD_SIZE+ (istrue(trace_opt.tim)?TIM_SIZE:0);
assert(strlen(icmp5str) < sizeof icmp5str);
	sprintf (icmp3str, "ICMP unreachable %%-%ds",
			    (istrue (trace_opt.tim)? TIM_SIZE : 0)+NOD_SIZE-11);
	icmp3hdrsize= NOD_SIZE+ (istrue(trace_opt.tim)?TIM_SIZE:0);
assert(strlen(icmp3str) < sizeof icmp3str);
	sprintf (icmp4str, "%-*%s",
			    (istrue (trace_opt.tim)? TIM_SIZE : 0)+NOD_SIZE,
			    "ICMP source quench");
	icmp4hdrsize= NOD_SIZE+ (istrue(trace_opt.tim)?TIM_SIZE:0);
assert(strlen(icmp4str) < sizeof icmp4str);
	sprintf (icmp11str, "%-*s%%-%ds",
			    (istrue (trace_opt.tim)? TIM_SIZE : 0)+NOD_SIZE,
			    "ICMP datagram lost",
			    NOD_SIZE);
	icmp11hdrsize= 2*NOD_SIZE+ (istrue(trace_opt.tim)?TIM_SIZE:0);
assert(strlen(icmp11str) < sizeof icmp11str);
	sprintf (icmp12str, "%-*s",
			    (istrue (trace_opt.tim)? TIM_SIZE : 0)+NOD_SIZE,
			    "ICMP erroneous datagram");
	icmp12hdrsize= NOD_SIZE+ (istrue(trace_opt.tim)?TIM_SIZE:0);
assert(strlen(icmp12str) < sizeof icmp12str);


	if (l3f->from != NULL){
		sscanf (l3f->from, "%d.%d.%d.%d", &ipfrom[0], &ipfrom[1], 
						       &ipfrom[2], &ipfrom[3]);
		ipfrom_f = true;
	}
	else	ipfrom_f = false;

	if (l3f->to != NULL){
		sscanf (l3f->to, "%d.%d.%d.%d", &ipto[0], &ipto[1], 
						       &ipto[2], &ipto[3]);
		ipto_f = true;
	}
	else	ipto_f = false;
	ip_bothways = l3f->bothways;

	icmp_f= false;
	if (l3f->protocol!=NULL) {
		ipproto_f= true;
		if (!strcmp (l3f->protocol,"tcp")) ipproto_n= IP_TCP;
		else if (!strcmp (l3f->protocol,"udp")) ipproto_n= IP_UDP;
		else if (!strcmp (l3f->protocol,"icmp")) {
			ipproto_n= IP_ICMP;
			if (l4f->events!=NULL) {
				char ** l4e= l4f->events;

				icmp_f= true;
				icmp_echo= false;
				icmp_redirect= false;
				icmp_unreachable= true;
				icmp_timestamp= false;
				icmp_quench= false;
				icmp_other= false;
				while (*l4e!=NULL) {
				 if (!strcmp (*l4e,"echo")) icmp_echo= true;
				 else if (!strcmp (*l4e,"redirect")) icmp_redirect= true;
				 else if (!strcmp (*l4e,"unreachable")) icmp_unreachable= true;
				 else if (!strcmp (*l4e,"quench")) icmp_quench= true;
				 else if (!strcmp (*l4e,"timestamp")) icmp_timestamp= true;
				 else if (!strcmp (*l4e,"other")) icmp_other= true;
				 else {
				   emon$print_error ("Unknown ICMP event %s",
						     *l4e);
				   return false;
				 }
				 l4e++;
				}
			}	/* ICMP EVENT */
		}
		else {
			emon$print_error ("Unknown IP protocol %s", l3f->protocol);
			return false;
		}
	}	/* L3 protocol != NULL */
	else	ipproto_f= false;

	if (l4f->from!=NULL) {
		socket_f= true;
		socket_checktype= check_from;
		sscanf (l4f->from, "%hd", &socket_nf);
		if (socket_nf==0) {
			emon$print_error ("Invalid socket #%s", l4f->from);
			return false;
		}
		else	socket_nf= htons (socket_nf);
	}
	else	socket_f= false;

	if (l4f->to!=NULL) {
		if istrue (socket_f)
			socket_checktype= check_both;
		else 	socket_checktype= check_to;
		socket_f= true;
		sscanf (l4f->to, "%hd", &socket_nt);
		if (socket_nt==0) {
			emon$print_error ("Invalid socket #%s", l4f->to);
			return false;
		}
		else	socket_nt= htons (socket_nt);
	}

	if istrue(l4f->bothways) {
		if (socket_checktype==check_from)
			socket_checktype=check_any1;
		else	socket_checktype=check_any2;
	}

	return true;
}

/******************************************************************************/
/*	CLEANUP routine							      */
/* this boolean routine will cleanup after the tracing is finished	      */
/*	it is not called if the INIT routine failed			      */
/******************************************************************************/
void
EMON$CLEANUP()
{
}
