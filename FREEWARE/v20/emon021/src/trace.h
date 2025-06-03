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
#include "types.h"

struct 	trace_opt_st {
		boolean data,	/* display frame data */
			trunc,	/* truncate display instead of wrapping */
			ascii,	/* display data in ascii rather than in HEX */
			pad,	/* first 2 bytes are length (DEC) */
			fil,	/* output file is present */
			tim,	/* TIME field is present in display */
			iosb,	/* IOSB devdep has been displayed */
			trail,	/* 6 last P5 bytes displayed (int DBG) */
			size,	/* frames are selected according to size */
			err,	/* display frames with erroneous IOSB */
			filter,	/* use user defined filter to parse frame */
			debug,	/* filter debug requested */
			full,	/* /FULL qualifier for a filter */
			brief;	/* /BRIEF qualifier for a filter */
		};

struct levelfilter_st {
		char * from;	/* from name (Default: NULL) */
		char * to;	/* to name (Default: NULL) */
		boolean bothways;/* true if bothways (end1= from, end2=to) */
		char * protocol;/* protocol name */
		char * filter;	/* yet another filter name (default: NULL)*/
		char ** display;/* display list (NULL terminated) */
		char ** events; /* event list (NULL terminated) */
		};

#pragma member_alignment save
#pragma nomember_alignment
#pragma nostandard

struct	ethernet_packet_st {
		unsigned long time[2];
		unsigned short iosb_status;
		unsigned short iosb_length;
		unsigned long iosb_devdepend;
		unsigned char destination[6];
		unsigned char source[6];
		unsigned short protoeth;	/* valid ONLY for ethernet */
				/* if 802* then ntohs(protoeth) <=1500 */
		unsigned char type;
#define	FMT_ETH 0	/* ethernet format */
#define FMT_802 1	/* 802 format */
#define FMT_802E 2	/* 802 Extended format */
		variant_union {
			variant_struct {
				variant_union {
					variant_struct {
						unsigned char dsap802;
						unsigned char ssap802;
						} emon$$struct1;
					unsigned short dssap802;
				} emon$$union1;
				unsigned short ctl802;
				unsigned char emon$$fill1;
			} emon$$union2;
			unsigned char proto802e[5];
		} emon$$union3;
		unsigned char data[1500];
		};

#pragma standard
#pragma member_alignment restore

typedef boolean analyze_routine_t (struct ethernet_packet_st * pkt,
				   char * editbuf,
				   int curbufpos);
typedef boolean init_routine_t	(struct trace_opt_st * opt,
				 struct levelfilter_st * l4,
				 struct levelfilter_st * l3,
				 int width,
				void print_routine(char * buf, int len));
typedef void	cleanup_routine_t	(void);

void	emon$dump (
		void * buf,	    /* addr of data to be dumped */
		int  buflen,	    /* size of data to be dumped */
		char * editbuf,	    /* starting addr of edit buffer */
		int  * editbufpos,  /* current position in edit buffer (inout)*/
		int  startpos,	    /* where to start after a line wrap */
		int  dumpmode	    /* change default dump mode */
		);
#define DUMP$M_HEX	1	    /* default: HEX, TRUNC */
#define	DUMP$M_ASCII	3
#define DUMP$M_TRUNC	5
#define DUMP$M_NOTRUNC	1
void	emon$dumpstr (
		char * msg,         /* addr of string to be output */
		char * editbuf,	    /* starting addr of edit buffer */
		int  * editbufpos,  /* current position in edit buffer (inout)*/
		int  startpos);	    /* where to start after a line wrap */
void	emon$dump_flush (	/* write a dump buffer */
		char * buf,	    /* starting position of buffer */
		int  length);	    /* size of above buffer */
void	emon$put_error (const char * message);
void	emon$put_more_error (const char * message);
void	emon$print_error (const char * ctrstr, ...);
