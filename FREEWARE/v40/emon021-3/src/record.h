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
#include <stdio.h>
#include "types.h"
#pragma nostandard
noshare boolean recording;
noshare FILE * frecord;

boolean start_record (char * filename);
void stop_record (void);
void write_record (void * buffer, int bufsize);

typedef struct {vmstime_t time; ulong type, version;} rec_header;

#define REC_COUNTERS	1
#define REC_COUNTERS_VERS 1
typedef struct {rec_header header; 
		ulong	cdbr,	/* circuit blocks received */
			cbrc,	/* circuit bytes received */
		     	cubu,	/* circuit user buffer unavailable */
			lsbu,	/* line system buffer unavailable */
			llbe,	/* line local buffer unavailable */
			lovr,	/* line data overrun */
			lrfl,	/* line receive failure */
			lsfl,	/* line send failure */
			lcdc	/* line carrier detect check failure */
			;} rec_counters_t;

#define REC_MONPROTOCOL 2
#define REC_MONPROTOCOL_VERS 1
typedef struct {rec_header header;
		unsigned long dbyte;
		unsigned long dpacket;
		ethernet_protocol_t proto;
		} rec_monprotocol_t;

#define REC_MONMULTICAST 3
#define REC_MONMULTICAST_VERS 1
#pragma standard
