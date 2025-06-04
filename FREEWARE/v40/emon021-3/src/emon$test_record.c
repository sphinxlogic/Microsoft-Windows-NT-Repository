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
#include unixio
#include "types"
#define MAXPROTO 256
#define GAP_BITS 38	/* taille en b/w du gap entre 2 trames */
typedef ushort ethernet_protocol_t;

typedef struct {float x, y;} coord_t;
static coord_t nullcoord;

#include "record"

#define SETBUF tcs_setbuf

#include "exttcs"
#include "extag2t"

#include libdtdef

maxdbr;
maxbrc;
duree;

#define DISP_BLOCKS 0
#define DISP_BITRATE 1
#define DISP_BANDWIDTH 2

disptype= DISP_BANDWIDTH;

struct tabproto_t {ulong tdbr,tbrc;
	coord_t old_xy, new_xy;
	ethernet_protocol_t proto;}
	tabproto[MAXPROTO];
nbproto;

vmstime_t start_time,end_time;
gotstart;

struct {rec_header header;
	char rest_of_data[512];} buf;

FILE * recfile;

/*	wrttxt: ecriture d'un texte en mode graphique	*/

static void
wrttxt (cp)
char * cp;
{
	long curch;
	int size= strlen (cp);
	while (size--) {
		curch= (long) *cp++;
		TOUTPT (&curch);
	}
}

/*	get_secs: difference (en secondes) date fournie/date debut */

int get_secs (cur_time)
vmstime_t * cur_time;
{
	vmstime_t vmsduree;
	static unsigned cvt_seconds= LIB$K_DELTA_SECONDS;
	int duree;
	int st;

	LIB$SUB_TIMES (cur_time, &start_time, &vmsduree);
	st= LIB$CVT_FROM_INTERNAL_TIME (&cvt_seconds, &duree, &vmsduree);
	if ((st&1)==0) duree=0;	/* pour le cas ou start_time==cur_time */
	return duree;
}

static trt_counters(buf)
rec_counters_t * buf;
{
	if (buf->cdbr>maxdbr) maxdbr= buf->cdbr;
	if (buf->cbrc>maxbrc) maxbrc= buf->cbrc;
	if (!gotstart) {
		start_time= buf->header.time;
		gotstart++;
	}
	end_time= buf->header.time;
}

static struct tabproto_t *
findproto(buf)
rec_monprotocol_t * buf;
{
	int numproto;
	struct tabproto_t *pp;

	for (numproto=nbproto, pp=tabproto; numproto--; pp++)
		if (buf->proto==pp->proto) return pp;
	return NULL;
}

static void
trt_monproto(buf)
rec_monprotocol_t * buf;
{
	struct tabproto_t *pp;

	end_time= buf->header.time;
	pp= findproto(buf);
	if (pp!=NULL){
			pp->tbrc += buf->dbyte;
			pp->tdbr += buf->dpacket;
	}
	else {
		if (nbproto==MAXPROTO) {
			printf ("Trop de protocoles\n");
			exit (44);
		}
		else {
			pp= &tabproto[nbproto++];
			pp->proto= buf->proto;
			pp->tbrc= buf->dbyte;
			pp->tdbr= buf->dpacket;
		}
	}
}

FILE * recdbg;

static float cur_col;
static float cur_cnt;
static nbcolproto;

static void drawcol()
{
	float cur_tot= 0;
	int numproto;
	struct tabproto_t *pp;

	for (pp= tabproto, numproto=nbproto; numproto--; pp++) {
		cur_tot += pp->new_xy.y;
		pp->new_xy.y= cur_tot;
		if ((pp->old_xy.x!=nullcoord.x) && (pp->old_xy.y!=nullcoord.y)){
			MOVEA (&pp->old_xy.x, &pp->old_xy.y);
			DRAWA (&pp->new_xy.x, &pp->new_xy.y);
		}
		pp->old_xy= pp->new_xy;
	}
}

void draw_counters(buf)
rec_counters_t * buf;
{
	static coord_t old_rcv= {0,0};	/* nb analyse */
	static float old_total= 0;	/* nb recu */


	coord_t new_rcv;
	float new_total;

	if (nbcolproto) drawcol();	/* colonne a terminer */

	new_rcv.x= get_secs (&buf->header.time);
	switch (disptype) {
	case	DISP_BLOCKS:	new_rcv.y= buf->cdbr/8;	
				new_total= new_rcv.y+
					(buf->cubu+buf->lsbu)/8;/* paquets perdus */
				break;
	case	DISP_BITRATE:	new_rcv.y= buf->cbrc;
				new_total= buf->cbrc*(
					(buf->cdbr+buf->cubu+buf->lsbu)
						    /(float)buf->cdbr);
				break;
	case	DISP_BANDWIDTH:	new_rcv.y= buf->cdbr*GAP_BITS+buf->cbrc;
				new_total= buf->cbrc*((buf->cdbr+buf->cubu)
						    /(float)buf->cdbr)
				+GAP_BITS*(buf->cdbr+buf->cubu+buf->lsbu);
				break;
	}

	MOVEA (&old_rcv.x, &old_rcv.y);
	DRAWA (&new_rcv.x, &new_rcv.y);	/* paquets analyses */

	MOVEA (&old_rcv.x, &old_total);
	DRAWA (&new_rcv.x, &new_total);	/* paquets recus */

	old_rcv= new_rcv;
	old_total= new_total;
	nbcolproto= 0;
	cur_col= new_rcv.x;
}

static void
draw_monproto(buf)
rec_monprotocol_t * buf;
{
	struct tabproto_t * pp;
	coord_t new_xy;

	nbcolproto++;

	pp= findproto (buf);
	new_xy.x= cur_col;
	switch (disptype) {
	case	DISP_BLOCKS:	new_xy.y= buf->dpacket/8; break;
	case	DISP_BITRATE:	new_xy.y= buf->dbyte; break;
	case	DISP_BANDWIDTH:	new_xy.y= buf->dbyte+GAP_BITS*buf->dpacket; break;
	}
	pp->new_xy= new_xy;
}

void init_graph()
{
	static long debut= 0;
	float xmax,ymax;
	float xmin= 0.0, ymin= 0.0;
	static	int baud=9600/10;
	static  int termtype=5;

	fseek (recfile, debut, 0);
	recdbg= fopen ("trace.dat","w");
	xmax= get_secs (&end_time);
	switch (disptype){
	case DISP_BLOCKS:	ymax= maxdbr/8; break;
	case DISP_BITRATE:	ymax= maxbrc; break;
	case DISP_BANDWIDTH:	ymax= maxbrc+GAP_BITS*maxdbr; break;
	}
	INITT (&baud, &termtype);
	AXES (&xmin, &xmax, &ymin, &ymax);
}

fin_graph()
{
	int ibidon;
	int un= 1;
	int numproto;
	struct tabproto_t * pp;
	static struct {ulong x,y;} posnom= {3800,550};

	if (nbcolproto) drawcol();

	for (numproto= nbproto, pp=tabproto; numproto--; pp++) {
		char sbuf [20];
		struct {unsigned l1:4, l2:4, l3:4, l4:4;}
			*prp= &pp->proto;

		MOVEA (&pp->old_xy.x, &pp->old_xy.y);
		DRWABS (&posnom.x, &posnom.y);
		ANMODE();
		sprintf (sbuf," %x%x-%x%x",prp->l2, prp->l1, prp->l4, prp->l3);
		wrttxt (sbuf);
		posnom.y += 100;
	}
	ANMODE();
	TINPUT (&ibidon);
	FINITT(&un, &un);
	puts ("\033[?38l");
}

/* on met les proto les moins utilises en bas */

static void
trie_proto()
{
	int i,j;

	for (i=0; i<nbproto-1; i++)
		for (j=i+1; j<nbproto; j++)
			if (tabproto[j].tbrc < tabproto[i].tbrc) {
				struct tabproto_t inter= tabproto[j];
				tabproto[j]= tabproto[i];
				tabproto[i]= inter;
			}
}

static void
affiche()
{
	int reclen;

	trie_proto();
	init_graph();
	while (reclen= read (fileno(recfile), &buf, sizeof buf) > 0) {
		switch (buf.header.type) {
			case REC_COUNTERS:	draw_counters (&buf); break;
			case REC_MONPROTOCOL:	draw_monproto (&buf); break;
			default: {
				ANMODE();
				fprintf (stderr, "Illegal record type %d\n",
							buf.header.type);
				exit (44);
			}
		}
	}
	fin_graph();
}

static void
direexp()
{
	char bidon[255];
	do {
		printf ("Il y a %d protocoles\n\naffichage (1:blk 2:bit 3:B/W) ?", nbproto);
		gets (bidon);
		disptype= atol (bidon) -1;
	}	while ( (disptype<0) || (disptype>2) );
}

main()
{
	int reclen;	/* length of record read */

	recfile= fopen ("emon.rec","rb");
	if (recfile==NULL) {
		perror ("open record");
		exit (44);
	}

	while (reclen= read (fileno(recfile), &buf, sizeof buf) > 0) {
		switch (buf.header.type) {
			case REC_COUNTERS:	trt_counters (&buf); break;
			case REC_MONPROTOCOL:	trt_monproto (&buf); break;
			default: 
				fprintf (stderr, "Illegal record type %d\n",
							buf.header.type);
				exit (44);
		}
	}
	if (reclen) {
		perror ("read record file");
		exit (44);
	}
	else{
		direexp();
		affiche();
	}
}
