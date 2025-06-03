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
#ifndef __EMON_LOADED
#define	__EMON_LOADED True
#include "types"

#define ETHERNET_LINK_LAYER_OVERHEAD	 8+6+6+2+4+12
/* preambule dest_address source_address type */	/* postambule gap */
#define MIN_ETHERNET_SIZE	46	/* 802.3 taille min. paquet ethernet */
#define MAXCIRCUIT 200
#define MAXSOURCE 70
#define MAXPROTOCOL 20
#define MAXPROTOCOL802E 20
#define	MAXAREA	20
#define MAXNODE 50
#define MAXDECNETNODE	60
#define MAXMAPLINE	40
#define PARAMBUFFERS	40
#define PARAMPRIORITY	15
#define PARAMNBQIO	50

#pragma member_alignment save
#pragma nomember_alignment
#pragma nostandard

typedef union{
		struct{
			ulong header;
			unsigned node:10;
			unsigned area:6;
			} decnet;
		variant_struct {ulong  firstpart;
			ushort secondpart;
			} cmp_data;
		ubyte bytes[6];
		} ethernet_addr_t;
#define DECNET_HEADER 0x400aaL
#define equ_addr(src,dst) (((src)->firstpart==(dst)->firstpart) \
		     && ((src)->secondpart==(dst)->secondpart) )
#define neq_addr(src,dst) (( (src)->firstpart!=(dst)->firstpart ) \
		     || ( (src)->secondpart!=(dst)->secondpart ) )
typedef union{
		ubyte bytes[2];
		ushort word;
		} ethernet_protocol_t;

typedef struct {ethernet_addr_t dest_addr,from_addr;
		ethernet_protocol_t protocol;
		variant_union {
			ubyte padto20bytes[6];
			struct {
				unsigned char hrd80e2$$fill;
				unsigned char pid[5];	/* 802E raw /formatted*/
			} hdr802e;
			variant_union {
			   variant_struct {
				unsigned short ctlw;	/* 802 raw 2 ctl */
				unsigned char hdr802$$fill1[4];
			   } hdr802$$1;
			   variant_struct {
				unsigned char ctlb;	/* 802 raw 1 ctl */
				unsigned char hdremon$$fill[5];
			   } hdr802$$2;
			   variant_struct {
				unsigned char type;	/* any formatted */
				unsigned char hdr802$$$3[5];
			   } hdr802$$3;
			   struct {
				unsigned char hdr802$$$4[6];
			   } bytes;
			} hdr$$802;
			struct {
				unsigned char emon$$type;
				variant_union {
				   variant_struct {
				     unsigned char dsap,ssap;/* 802 formatted */
				   } hdremon1$$$struct;
 				   unsigned short dssap;   /* 802 formatted */
				} hdremon1$$$union;
				unsigned short ctl;	/* 802 formatted */
				unsigned char hdremon1$$fill;
			} hdr802;
		} trail802$$union;
	} packet_header_t;

#define PKT_802 1
#define PKT_802E 2
#define	IEEE802E_DSSAP	0xAAAA
#define IEEE802_CTL_UI	3

typedef struct {ethernet_addr_t dest_addr,from_addr;
		ethernet_protocol_t protocol;
		} packet_ident_t;

typedef struct  {ulong low,high;} time_t;

typedef struct {ulong delta,total;} counter_t;

/*	equ_tag: memes headers de paquets: from, dest, et protocol */

#define equ_tag(src,dst) ( \
			     equ_addr (&(src)->dest_addr,&(dst)->dest_addr)\
			&&   equ_addr (&(src)->from_addr,&(dst)->from_addr)\
			&&   ( (src)->protocol.word == (dst)->protocol.word )\
			)
typedef struct {
		time_t time; 
		iosb_t iosb;
		packet_header_t tag;
		ubyte buffer[1500];
		} pckt_t;	/* 3 pages tout rond! */

#pragma member_alignment restore

typedef	char	protocol_name_t[13];
const	char blank_protocol_name[]="            ";
typedef char	node_name_t[18];
const	char blank_node_name[]="                 ";
typedef char	area_name_t[6];
typedef char	map_name_t[9];

typedef struct {
		ubyte libheader[10];
		ethernet_addr_t addr;
		node_name_t name;
		counter_t nbrbytes,nbrpackets;
		counter_t nbsbytes,nbspackets;
		ushort	map_num;
		ushort	top_num;
		boolean	passsrc;	/* this node valid as source */
		boolean passdst;	/* this node valid as dest */
		boolean passnod;/* this node valid whatever the partner is */
		boolean pass;		/* this node valid for stats */
		boolean new;
		} node_entry;

typedef struct {
		ubyte libheader[10];
		ethernet_protocol_t protocol;
		protocol_name_t name;
		counter_t nbbytes,nbpackets;
		boolean pass;
		boolean new;
		} protocol_entry;

	/* on mappe les protocoles 802E sur les protocoles 1-1500 */
	/* le protocole 0 mappe les trames 802 */

typedef	struct	protocol802e_entry_st {
		unsigned long protocol_low;
		unsigned char protocol_high;
		protocol_entry * protoptr;
		struct protocol802e_entry_st *hshlnk;
		} protocol802e_entry;

typedef struct 	source_entry_st {
		ubyte libheader[10];
		ethernet_addr_t addr;
		ethernet_protocol_t protocol;
		protocol_entry * prtptr;
		node_entry * nodptr;
		counter_t nbrbytes,nbrpackets;
		counter_t nbsbytes,nbspackets;
		}	source_entry;

typedef struct circuit_entry_st {
		ethernet_addr_t dest_addr,source_addr;
		ethernet_protocol_t protocol;
		struct source_entry_st * srcptr, * dstptr;
		counter_t nbbytes,nbpackets;
		struct circuit_entry_st * hshlnk;
		ulong nbbytesctr, nbpacketsctr;
		boolean pass;
		}	circuit_entry;

typedef struct {
		area_name_t name;
		ulong	used;
		}	area_entry;

typedef struct {
		map_name_t name;
		ubyte	flags;
		ushort	remap;
		}	map_entry;

noshare circuit_entry * tabcircuit;
noshare ushort 	maxcircuit=0;
noshare ushort	circuit_nb=MAXCIRCUIT;
noshare source_entry * source_header=0;		/* node+proto, listen+talk */
noshare source_entry *	tabsource;
noshare ushort	maxsource=0;
noshare ushort  source_nb=MAXSOURCE;
noshare protocol_entry * protocol_header=0;	/* protocols listen+talk */
noshare protocol_entry * tabprotocol;
noshare ushort	maxprotocol=0;
noshare ushort  protocol_nb=MAXPROTOCOL;
noshare node_entry * node_header=0;		/* nodes listen+talk */
noshare node_entry * tabnode;
noshare ushort	maxnode=0;
noshare ushort  node_nb=MAXNODE;
noshare area_entry * tabarea;			/* area names */
noshare ushort 	area_nb=MAXAREA;
noshare counter_t *tabareatraf;			/* array of area traffic */
noshare ushort  max_addr=MAXDECNETNODE+1;	/* 'maximum address' for all areas +1 */
noshare map_entry * tabmap;			/* map names */
noshare ushort	max_map=MAXMAPLINE+1;		/* maximum number of lines in map +1 */
noshare protocol802e_entry * protocol802e_header= 0;
noshare protocol802e_entry * tabprotocol802e;
noshare ushort maxprotocol802e= 0;
noshare ushort protocol802e_nb=MAXPROTOCOL802E;
noshare struct {
		struct {
			boolean overflow;
		} protocol;
	} ignore;	/* = all false since 0 == false and variable is static*/
noshare struct {
		struct {
			unsigned long overflow;
		} protocol;
	} counter;	/* = all 0 since variable is static*/

typedef struct {ulong 	DBR,	/* Data Blocks Received */
			MBL,	/* Multicast Blocks received */
			RFL,	/* Receive FaiLures */
			BRC,	/* Bytes Received */
			MBY,	/* Multicast Bytes received */
			OVR,	/* data OVeRrun */
			LBE,	/* Local Buffer Errors */
			DBS,	/* Data Blocks Sent */
			MBS,	/* Multicast Blocks Sent */
			BSM,	/* Blocks Sent Multiple collision */
			BS1,	/* Blocks Sent 1 collision */
			BID,	/* Blocks Initially Deffered */
			BSN,	/* Bytes SeNt */
			MSN,	/* Multicast bytes SeNt */
			SFL,	/* Send FaiLures */
			CDC,	/* Carrier Detect Check */
			UFD,	/* Unrecognized Frame Destination */
			SBU,	/* System Buffer Unavailable */
			UBU;	/* User Buffer Unavailable */
		}
	line_counters;
noshare line_counters cumulated_line_counters={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
noshare line_counters delta_line_counters;

typedef struct {ulong DBR,BRC,MBY,LBE,DBS,MNE,BSN,UBU;}
		circuit_counters;
noshare circuit_counters cumulated_circuit_counters={0,0,0,0,0,0,0,0};
noshare circuit_counters delta_circuit_counters;

	/* parametres du moniteur */

noshare struct	{ulong	buffers;
	 ulong  qionb;
	 ulong  eko;
	 ulong 	priority;
	 int	fmt;
	 char	device[14];
	 struct { ushort dsc$w_length, nothing; char * dsc$a_pointer;} dev_desc;
	 ulong  nopad;
	}
	parameters= {	PARAMBUFFERS,
			PARAMNBQIO,
			1 /* off */,
			PARAMPRIORITY,
			1 /* off */,
			"",
			{0,0, (char *) &parameters.device},
			1 /* off */
		    };
#pragma standard
#endif
