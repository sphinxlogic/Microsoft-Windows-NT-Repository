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
#include	"monitor.h"
#include	"stat.h"
#include	"ni_routines.h"

#include	<ctype.h>
#include	<stdio.h>
#include	<string.h>

#if defined (__STDC__) || defined (__DECC)
#include	<stdlib.h>
#endif

#include	<descrip.h>

int	SMG$PUT_LINE();

#define MAXFRSIZE 9999	/* max size of ethernet frame */
#define MAXNBFRSIZE 18	/* max # of gauges */

static boolean	if_check;	/* check ethernet interface */
static boolean mon_abort;
static const int predefsize[MAXNBFRSIZE]= 
	{60,100,128,256,512,1024,1200,MAXFRSIZE};

static struct frsize_t { int
			maxsize,		/* maximum size of frames */
			deltaframes, deltabits,
			totframes,totbits;}
		frsize_tab[MAXNBFRSIZE];

static boolean
mon_update()
{
#define	GAP_SIZE 12
	struct frsize_t * fp;
	char line_buf[80];
static	struct dsc$descriptor buf_d= {sizeof line_buf, 0, 0, NULL};

	int prev_val= 0;
	int grand_tot_bytes= 0;

	upd_stat();
	status_update();

	buf_d.dsc$a_pointer= line_buf;

	emon$begin_pasteboard_update();
	emon$display_home (&line_coord);

	for (fp= frsize_tab; prev_val<MAXFRSIZE; fp++) {
		fp->totbits += fp->deltabits;
		fp->totframes += fp->deltaframes;
		grand_tot_bytes += (fp->totbits+GAP_SIZE*fp->totframes)/100;
		prev_val= fp->maxsize;
	}

	if (grand_tot_bytes==0) grand_tot_bytes=1; /* prot anti jolin */
	prev_val= 0;

	for (fp= frsize_tab; prev_val<MAXFRSIZE; fp++) {
		sprintf (line_buf, "%4d-%4d:%9d%7d%10d%6.2f%7d",
				prev_val, fp->maxsize, 
				fp->deltabits, fp->deltaframes, 
				fp->totbits,
				(float) (GAP_SIZE*fp->totframes+fp->totbits)
					/ (float) grand_tot_bytes,
				fp->totframes);
		fp->deltabits= 0;
		fp->deltaframes= 0;
		prev_val= fp->maxsize+1;
		buf_d.dsc$w_length= strlen (line_buf);
		SMG$PUT_LINE (&line_id, &buf_d);
	}
	emon$end_pasteboard_update();
	return true;
}

static mon_record (p,e)
pckt_t * p;
circuit_entry *e;
{
#define	L2_OVERHEAD	8/*preambule*/+6+6+2/* source/dest/proto */+4 /*CRC*/

	int i;
	register ushort nbbytes= p->iosb.iosb_count+L2_OVERHEAD;

	if (istrue(if_check)) {
		if istrue (mon_abort) return;
		if (isfalse (ni_check_iosb(&p->iosb))){
			mon_abort= true;
			abort_monitor ("Error detected in IOSB");
			return;
		}
	}

	if (e==0) /* status message */ return;

	for (i= 0; frsize_tab[i].maxsize<nbbytes; i++);

	frsize_tab[i].deltabits+= nbbytes;
	frsize_tab[i].deltaframes++;
}

int
cmd_monitor_size()
{
	node_entry * from, * to;
	protocol_entry *proto;
	cli_buf_t buf;
	int i;
	int stat_flags;

	mon_abort= false;

	emon$erase_display (&line_coord);

	for (i=0; i<MAXNBFRSIZE; i++){
		frsize_tab[i].maxsize= predefsize[i];
		frsize_tab[i].totbits= 0;
		frsize_tab[i].totframes= 0;
		frsize_tab[i].deltabits= 0;
		frsize_tab[i].deltaframes= 0;
	}

	if (getclistring ("FRAME",buf)){		/* tableau des fraimes */
		int i=0;
		do {
			if ( (i+1)>= MAXNBFRSIZE ) {
				emon$put_error ("Too many frame sizes");
				break;
			}
		 	frsize_tab[i++].maxsize= atol(buf);
		}
		while (getclistring("FRAME",buf));
		frsize_tab[i].maxsize= MAXFRSIZE;
	}
	frsize_tab[MAXNBFRSIZE-1].maxsize= MAXFRSIZE;

	if_check= (boolean) clipresent ("check");

	if (isfalse(get_filter (&from, &to, &proto, &stat_flags))) return;

	init_loc_stat (from, to, proto, mon_record, stat_flags);

	emon$title("             Frame size statistics ...");
	emon$paste_display (&line_coord);
	init_monitor (mon_update);
	lock_monitor (false);		/* on arrete la collection pendant l'affichage */
	monitor(loc_stat);
}
