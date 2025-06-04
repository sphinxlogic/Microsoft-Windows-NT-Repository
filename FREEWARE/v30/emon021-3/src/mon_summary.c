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
#include	"monitor.h"
#include	"stat.h"

#include	<stdio.h>
#include	<string.h>

#include	<descrip.h>
int		SMG$PUT_LINE(),
		SMG$SET_CURSOR_REL();

static boolean
display_update()
{
	char buf[132];
static	struct dsc$descriptor buf_desc={24,0,0,NULL};
	static const char countline[]="%12lu%12lu";


	if isfalse(upd_stat()) return false;
	buf_desc.dsc$a_pointer= buf;

	emon$begin_pasteboard_update();
	status_update();
	emon$display_home (&display_coord);

/*		bytes received			*/
	sprintf (buf,countline,cumulated_circuit_counters.BRC,
			            delta_circuit_counters.BRC
		);
	SMG$PUT_LINE (&display_id,&buf_desc);
/*		local node traffic		*/
/*		data blocks received		*/
	sprintf (buf,countline ,cumulated_line_counters.DBR,
				delta_line_counters.DBR);
	SMG$PUT_LINE (&display_id,&buf_desc);
/*		multicast blocks received	*/
	sprintf (buf,countline ,cumulated_line_counters.MBL,
				delta_line_counters.MBL);
	SMG$PUT_LINE (&display_id,&buf_desc);
/*		data blocks sent		*/
	sprintf (buf,countline,	cumulated_line_counters.DBS,
				delta_line_counters.DBS);
	SMG$PUT_LINE (&display_id,&buf_desc);
/*		multicast blocks sent		*/
	sprintf (buf,countline ,cumulated_line_counters.MBS,
				delta_line_counters.MBS);
	SMG$PUT_LINE (&display_id,&buf_desc);
/*		bytes received			*/
	sprintf (buf,countline ,cumulated_line_counters.BRC,
				delta_line_counters.BRC);
	SMG$PUT_LINE (&display_id,&buf_desc);
/*		multicast blocks received	*/
	sprintf (buf,countline ,cumulated_line_counters.MBY,
				delta_line_counters.MBY);
	SMG$PUT_LINE (&display_id,&buf_desc);
/*		bytes sent			*/
	sprintf (buf,countline ,cumulated_line_counters.BSN,
				delta_line_counters.BSN);
	SMG$PUT_LINE (&display_id,&buf_desc);
/*		multicast bytes sent		*/
	sprintf (buf,countline ,cumulated_line_counters.MSN,
				delta_line_counters.MSN);
	SMG$PUT_LINE (&display_id,&buf_desc);
	sprintf (buf, "%12ld%12ld",delta_line_counters.DBR-
				   delta_circuit_counters.UBU-
				   delta_circuit_counters.DBR,
				   delta_line_counters.BRC+
				 18*(delta_circuit_counters.UBU+
				     delta_circuit_counters.DBR)
				 -delta_circuit_counters.BRC);
	SMG$PUT_LINE (&display_id,&buf_desc);

	emon$display_home( &tab_coord);

/*		D E C N E T    D A T A						*/
	{
static		ubyte indexes[64],mon_maxareanb=0;
static		const char * statlin="%8lu";
static		char buft[100];
static 		const struct dsc$descriptor buft_desc={40,0,0,buft};
		register int i,j;
		register char *cp;

		for (i=area_nb, j=0; i--; ) if (tabarea[i].used) j++;
		if (j>mon_maxareanb){
static			struct dsc$descriptor	buf_d;

			cp= buft;
			strcpy (cp," From\\To");
			for (i=0,j=0; i<area_nb; i++)
				if (tabarea[i].used){
					cp+= 8;
					indexes[j++]= i;
					sprintf (cp,"  %6.6s",tabarea[i].name);
				}
			buf_d.dsc$w_length= cp-buft+8;
			buf_d.dsc$a_pointer=  buft;
			SMG$PUT_LINE (&tab_id,&buf_d);
			mon_maxareanb=j;
		}
		else SMG$SET_CURSOR_REL(&tab_id,&un);
		for (i=0; i<mon_maxareanb;i++){
			sprintf (buft,"%6.6s  ",tabarea[indexes[i]].name);
			cp=buft+8;
			for (j=0; j<mon_maxareanb; j++){
				sprintf (cp,statlin,
			 tabareatraf[indexes[i]*area_nb+indexes[j]].delta/1000);
				cp+= 8;
			}
			SMG$PUT_LINE(&tab_id,&buft_desc);
		}
/*				P R O T O C O L S 				*/
		{
			char buf[256];
static			struct dsc$descriptor	buf_d;
			static ushort maxpp=0;
			register protocol_entry * pp;

			buf_d.dsc$a_pointer= buf;
			if (maxprotocol>maxpp){
				protocol_name_t bufp;
				cp= buf;
				for (pp=tabprotocol,i=maxprotocol; i--; pp++)
					if (pp->nbbytes.total){
						sprintf (cp,"%8.8s",
						print_ethernet_protocol (pp, &bufp)+
							sizeof bufp-8-1);
						cp+= 8;
					}
				buf_d.dsc$w_length= cp-buf;
				SMG$PUT_LINE(&tab_id,&buf_d);
				maxpp= maxprotocol;
			}
			else SMG$SET_CURSOR_REL (&tab_id,&un);

			cp= buf;
			for (i=maxprotocol,pp=tabprotocol; i--; pp++)
				if (pp->nbbytes.total){
					sprintf (cp,statlin,pp->nbbytes.delta/1000);
					cp+=8;
				}

			buf_d.dsc$w_length= cp-buf;
			SMG$PUT_LINE (&tab_id,&buf_d);	
		}
		{
			register circuit_entry * p, * maxent;
			char buf[80];
			node_name_t buf1,buf2;
			protocol_name_t buf3;
static			struct dsc$descriptor	buf_d;
			ulong max;
			ulong nbbytes=0, nbpackets=0, mtbytes=0, mtpackets=0;

			buf_d.dsc$a_pointer= buf;

			for (p=tabcircuit,maxent=0,max=0,i=maxcircuit; i--; p++){
				nbpackets += p->nbpackets.delta;
				if (istrue(p->dest_addr.bytes[0])){
					mtpackets+= p->nbpackets.delta;
					mtbytes+= p->nbbytes.delta;
				}
				nbbytes += p->nbbytes.delta;
				if (p->nbbytes.delta>max){
					max= p->nbbytes.delta;
					maxent=p;
				}
			}

			if (maxent)
			sprintf (buf,"%s %s %s %12lu %6lu",
			    print_ethernet_addr (maxent->srcptr->nodptr, &buf1),
			    print_ethernet_addr (maxent->dstptr->nodptr, &buf2),
			 print_ethernet_protocol(maxent->srcptr->prtptr, &buf3),
				maxent->nbbytes.delta,maxent->nbpackets.delta);
			else strcpy (buf,"No data packets received");
			buf_d.dsc$w_length=strlen(buf);
			SMG$PUT_LINE (&tab_id,&buf_d);

			sprintf (buf,"counted: %8d bytes %8d packets (incl Xcst %d %d)",
				nbbytes,nbpackets,mtbytes,mtpackets);
			buf_d.dsc$w_length=strlen(buf);
			SMG$PUT_LINE (&tab_id,&buf_d);
		}
	}
	emon$end_pasteboard_update();
	emon$flush_buffer();
}

void cmd_monitor_summary()
{
	boolean save_buf= emon$enable_buf(true);
	emon$begin_pasteboard_update();
	emon$paste_display (&text_coord);
	emon$paste_display (&display_coord);
	emon$paste_display (&tab_coord);
	emon$title ("         Summary                    ");
	init_monitor(display_update);
	emon$end_pasteboard_update();
	monitor (glob_stat);
	emon$unpaste_display (&text_coord);
	emon$unpaste_display (&display_coord);
	emon$unpaste_display (&tab_coord);
	(void) emon$enable_buf(save_buf);
}
