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
/* #define BUG802V543 */
#define DRIVER_DEBUG 

/*********************************/
#ifdef DRIVER_DEBUG
#define CHECK_802
#endif
#ifdef BUG802V543
#define CHECK_802
#endif

#include "$sdl:nmadef"
#include "$sdl:stardefqz.h"	/* for XMDEF */
#include "monitor.h"
#include "stat.h"

#if defined (__DECC) || defined (__STDC__)
#include <stdlib.h>
#endif

#include <assert.h>
#include <descrip.h>
#include <dvidef.h>
#include <iodef.h>
#include <ssdef.h>
#include <stdio.h>
#include <string.h>

#define SYNC_EFN 33
#define SYNC_EFN_AST 34

static void	(* ni_call)();
static iosb_t start_iosb= {0,0,0};
static ushort	ni_chan= 0;
static long	mon_old_priority= -1;
static	boolean	continue_on_error;	/* internal control flag */
static	boolean	error_stop = false;	/* CLI: stop on every error */

boolean ni_check_iosb();

unsigned long	SMG$SET_CURSOR_ABS(), SMG$PUT_LINE();

static char *
translate_ni_status (errstatus,buf)
unsigned long errstatus;	/* iosb 2nd lw */
char * buf;
{
	int i;

	strcpy (buf,"Unit/line status:");
	if (errstatus&XM$M_STS_ACTIVE) {
		strcat (buf, " ACTIVE");
		errstatus &= ~XM$M_STS_ACTIVE;
	} else 	strcat (buf, " INACTIVE");
	if (errstatus&XM$M_STS_BUFFAIL) {
		strcat (buf, " BUFFAIL");
		errstatus &= ~XM$M_STS_BUFFAIL;
	}
	if (errstatus&XM$M_STS_TIMO) {
		strcat (buf, " TIMO");
		errstatus &= ~XM$M_STS_TIMO;
	}
	for (i=256; i<0x10000; i<<=1)
		if (errstatus&i) {
			char buf1[20];
			sprintf (buf1," 0x#%x", i);
			strcat (buf, buf1);
		}

	if (errstatus&0xFF0000) {
		strcat (buf, "; Error summary:");
		if (errstatus&XM$M_ERR_FATAL) {
			strcat (buf, " FATAL");
			errstatus &= ~XM$M_ERR_FATAL;
		}
		for (i=0x10000; i<0x1000000; i<<=1)
			if (errstatus&i) {
				char buf1[20];
				sprintf (buf1," 0x#%x", i);
				strcat (buf, buf1);
			}
	}

	return buf;
}

static ni_error(status,text)
ulong status;
char *text;
{
	abort_monitor(text);

	if ((status&1)==0) {
		vmsmsgbuf buf;
		emon$put_more_error (emon$translate_status(buf,status));
	}
}

static int ni_exit();

static ulong exit_status;
static struct {	ulong link;
		int ( *handler)();
		ulong arg_num;
		ulong * sts_addr;
		} desblk= {0,ni_exit,1,&exit_status};
unsigned long
assign_port()
{
	static const $DESCRIPTOR (deuna_desc,"XEA0:");
	static const $DESCRIPTOR (deqna_desc,"XQA0:");
	static const $DESCRIPTOR (debnt_desc,"ETA0:");
	static const $DESCRIPTOR (desva_desc,"SVA0:");
	static const $DESCRIPTOR (deesa_desc,"ESA0:");
	static const $DESCRIPTOR (deeza_desc,"EZA0:");
	static const $DESCRIPTOR (emondev_desc,"EMON$DEVICE:");
static const struct dsc$descriptor_s * tab_desc[]=
			{&emondev_desc,
			 &deuna_desc,
			 &deqna_desc,
			 &debnt_desc,
			 &desva_desc,
			 &deesa_desc,
			 &deeza_desc};
	unsigned long status;
	int	SYS$ASSIGN(),SYS$GETDVIW();
	int i;

	if (parameters.dev_desc.dsc$w_length != 0)
		status= SYS$ASSIGN (&parameters.dev_desc,&ni_chan,0,0);
	else	
	  for (i=0; i<(sizeof tab_desc)/sizeof (struct dsc$descriptor *); i++){
		status= SYS$ASSIGN (tab_desc[i],&ni_chan,0,0);
		if (status!=SS$_NOSUCHDEV) break;
	  }

	if (status&1){
		static char devnam[8];
		static unsigned short devnam_len;
		static const struct {unsigned short buflen,bufcod;
			             char * retadr;
				     unsigned short * retlen;
				     unsigned listend;}
			itmlst =
				{sizeof devnam,DVI$_DEVNAM, devnam, &devnam_len,
				 0};
		iosb_t iosb;

		SYS$GETDVIW (0,ni_chan,0,&itmlst,&iosb,0,0,0);
		emon$print_error("l'unite allouee est %.*s",devnam_len,devnam);
	}
	else{
		ni_error (status,"Assignation Ethernet impossible");
	}
	return status;
}

deassign_port()
{
	ulong status;
	ulong	SYS$DASSGN();

	status= SYS$DASSGN (ni_chan);
	if (status&1) ni_chan=0;
	else	ni_error (status,"deassignation NI device impossible");
}


static pckt_t * pckt_id, * membufaddr= 0;

static
char * 
nomparam(param_number)
int param_number;
{
	char * np;
	switch(param_number){
		case NMA$C_PCLI_FMT: np= "Packet format"; break;
		case NMA$C_PCLI_BFN: np= "Nb of buffers"; break;
		case NMA$C_PCLI_BUS: np= "User buffer size"; break;
		case NMA$C_PCLI_BSZ: np= "Device buffer size"; break;
		case NMA$C_PCLI_PTY: np= "Protocol type"; break;
		case NMA$C_PCLI_PID: np= "Protocol identifier"; break;
		case NMA$C_PCLI_ACC: np= "Shareability"; break;
		case NMA$C_PCLI_MLT: np= "Enable all multicasts"; break;
		case NMA$C_PCLI_PAD: np= "Port padding"; break;
		case NMA$C_PCLI_DCH: np= "Data chaining"; break;
		case NMA$C_PCLI_PRM: np= "Promiscuous mode"; break;
		case NMA$C_PCLI_EKO: np= "Echo mode"; break;
		case NMA$C_PCLI_CRC: np= "CRC generation"; break;
		case NMA$C_PCLI_PHA: np= "Physical address"; break;
		case NMA$C_PCLI_HWA: np= "Hardware address"; break;
		case NMA$C_PCLI_MCA: np= "Multicast address"; break;
		case NMA$C_PCLI_ILP: np= "Internal loopback"; break;
		case NMA$C_PCLI_CON: np= "Controller mode"; break;
		case NMA$C_PCLI_MBS: np= "Maximum buffer size"; break;
		case NMA$C_PCLI_RES: np= "Automatic line restart"; break;
		case NMA$C_PCLI_CCA: np= "Can change address"; break;
		case NMA$C_PCLI_APC: np= "Allow promiscuous client"; break;
		case NMA$C_PCLI_MED: np= "Communication medium"; break;
		case NMA$C_PCLI_SRV: np= "Driver service"; break;
		case NMA$C_PCLI_SAP: np= "Service Access Point"; break;
		case NMA$C_PCLI_GSP: np= "Group SAP"; break;
		case NMA$C_PCLI_CNM: np= "Client name"; break;
		case NMA$C_PCLI_PNM: np= "Port name"; break;
		default:{
			static char buf[20];
			sprintf (buf,"parameter #%4d ",param_number);
			np= buf;
		}
	}
	return np;
}

static
char * 
valparam(param_number,u)
int param_number;
ulong u;
{
	static char buf[20];
	char * np;

	switch(param_number){
		case NMA$C_PCLI_FMT:
			switch (u){
				case NMA$C_LINFM_ETH: np= "ethernet"; break;
				case NMA$C_LINFM_802: np= "ISO 802.3"; break;
				case NMA$C_LINFM_802E: np= "ISO 802.3E"; break;
				default: sprintf (buf,"format #%d",u);
				np= buf;
			}
			break;
		case NMA$C_PCLI_PTY:
			sprintf (buf,"%02X-%02X",u&0xff, u>>8);
			np= buf;
			break;
		case NMA$C_PCLI_ACC:
			switch(u){
				case NMA$C_ACC_SHR: np= "shareable"; break;
				case NMA$C_ACC_LIM: np= "shareable limited"; break;
				case NMA$C_ACC_EXC: np= "exclusive"; break;
				default: sprintf (buf,"shr #%d",u);
				np= buf;
			} break;
		case NMA$C_PCLI_MLT:
		case NMA$C_PCLI_PAD:
		case NMA$C_PCLI_DCH:
		case NMA$C_PCLI_PRM:
		case NMA$C_PCLI_EKO:
		case NMA$C_PCLI_CRC:
		case NMA$C_PCLI_ILP:
		case NMA$C_PCLI_CCA:
		case NMA$C_PCLI_APC:
			switch(u){
				case NMA$C_STATE_ON: np= "on"; break;
				case NMA$C_STATE_OFF: np= "off"; break;
				default: sprintf (buf,"illegal boolean #%d",u);
				np= buf;
			} break;
		case NMA$C_PCLI_RES:
			switch(u){
				case NMA$C_LINRES_DIS: np= "disabled"; break;
				case NMA$C_LINRES_ENA: np= "enabled"; break;
				default: sprintf (buf,"mode #%d",u);
				np= buf;
			} break;
		case NMA$C_PCLI_CON: 
			switch(u){
				case NMA$C_LINCN_NOR: np= "normal"; break;
				case NMA$C_LINCN_LOO: np= "loopback"; break;
				default: sprintf (buf,"mode #%d",u);
				np= buf;
			} break;
		case NMA$C_PCLI_MED: 
			switch(u){
				case NMA$C_LINMD_CSMACD: np= "CSMA/CD"; break;
				case NMA$C_LINMD_FDDI: np= "FDDI"; break;
				case NMA$C_LINMD_CI: np= "CI"; break;
				case NMA$C_LINMD_TR: np= "Token ring"; break;
				default: sprintf (buf,"medium type #%d",u);
				np= buf;
			} break;
		default: sprintf (buf,"%lu",u);
			np= buf;
	}
	return np;
}

static boolean	init_port_done= false;

static	struct	dsc$descriptor port_desc={0, 0, 0, 0};

static boolean
init_port()
{
#pragma	member_alignment save
#pragma nomember_alignment
    struct nma_parameter {unsigned short param_typ; unsigned long param_val;};
	struct nma_parameter
/**/
/*	ligne obligatoire pour taille buffer pointe par P5 */
/*	cf: ethernet-cmn_routines.mar	*/
/*	bien que, sous alpha, il faille supprimer cette ligne .... */
		nma_fmt= {NMA$C_PCLI_FMT,NMA$C_LINFM_ETH},/* ethernet format */
/**/
/*	la ligne suivante peut etre supprimee lors du demarrage */
	/* selon les versions de vms et des drivers ethernet, cette */
	/* ligne est: inutile, necessaire, imperative, ou a ne pas mettre */
/*	la ligne suivante peut etre supprimee lors du demarrage */
		nma_nopad= {NMA$C_PCLI_PAD,NMA$C_STATE_OFF},	/* no padding */
/**/
/*		pour memoire, a priori ne servent pas	*/
		nma_acc= {NMA$C_PCLI_ACC,NMA$C_ACC_EXC},    /* exclusive mode */
		nma_eko= {NMA$C_PCLI_EKO,NMA$C_STATE_ON};     /* echo mode on */

static	struct { struct {struct nma_parameter	bfn,prm,bus,res;} hdr;
		 struct nma_parameter items[3];
	       }
		itmlst=
		{{
		 {NMA$C_PCLI_BFN,0},
		 {NMA$C_PCLI_PRM,NMA$C_STATE_ON},	/* promiscuous mode */
		 {NMA$C_PCLI_BUS,1500},
		 {NMA$C_PCLI_RES,NMA$C_LINRES_ENA} },	/* restart enabled */
		 {{0,0},
		  {0,0},
		  {0,0}}
		};
#pragma	member_alignment restore

	unsigned long status, SYS$QIOW();
	iosb_t	iosb;
	int	itemnumber= 0;

	/* initialize port parameters */

	port_desc.dsc$a_pointer= (char *) &itmlst;
	port_desc.dsc$w_length=	 sizeof itmlst.hdr;

	itmlst.hdr.bfn.param_val= parameters.buffers;

	/* set default known values */

	/*	if nopad is specified, then add PAD OFF parameter */


	if ( (parameters.nopad== NMA$C_STATE_ON) &&
	     (parameters.fmt != NMA$C_LINFM_802E) )
		itmlst.items[itemnumber++]= nma_nopad;

	if (parameters.eko== NMA$C_STATE_ON)		/* enable echo */
		itmlst.items[itemnumber++]= nma_eko;

	if (parameters.fmt >= 0){			/* enable echo */
		nma_fmt.param_val= parameters.fmt;
		itmlst.items[itemnumber++]= nma_fmt;
	}

	port_desc.dsc$w_length+= itemnumber* sizeof (struct nma_parameter);

	status= SYS$QIOW (SYNC_EFN,ni_chan,IO$_SETMODE|IO$M_CTRL,
				  &iosb,0,0,0,&port_desc,0,0,0,0);

	if istrue(status){
		if isfalse(iosb.iosb_status) {
			char buf[80];
			ni_error (iosb.iosb_status, "error in SETMODE port IOSB");
			if (iosb.iosb_status==SS$_BADPARAM){
				char * cp;

				cp= nomparam(iosb.iosb_devdep);
				sprintf (buf,"Bad parameter value, %s",cp);
			}
			else	translate_ni_status (iosb.iosb_devdep, buf);

			emon$put_more_error (buf);
			return false;
		}
	}
	else{
		ni_error (status, "Error in SETMODE port QIO");
		return false;
	}

	return true;
}

boolean
start_port()
{
	char buf[1024];
	struct dsc$descriptor buffer_desc= {0, 0, 0, 0};
	ulong N= parameters.qionb;
	unsigned long status, SYS$DCLEXH(), SYS$SETPRI(), SYS$QIOW();

assert	(sizeof (packet_header_t) == 20);

	buffer_desc.dsc$w_length= sizeof buf;
	buffer_desc.dsc$a_pointer=  buf;
	
/*	on se reserve de l'espace supplementaire pour commencer sur une */
/*	frontière de page ...	*/
	if (membufaddr==0){
		SYS$DCLEXH (&desblk);
		membufaddr= malloc (parameters.qionb*sizeof (pckt_t)+511);
		pckt_id= (pckt_t *) ((511 + (ulong) membufaddr) &~0x1ff);
	}

	if isfalse (init_port_done) {
		if isfalse(init_port()) return false;
		init_port_done= true;
	}

	/* work at specified priority */

	SYS$SETPRI (0,0,parameters.priority,&mon_old_priority);

	status= SYS$QIOW (SYNC_EFN,ni_chan,IO$_SETMODE|IO$M_CTRL|IO$M_STARTUP,
				  &start_iosb,0,0,0,&port_desc,0,0,0,0);

	if (istrue(status)){
		if (istrue(start_iosb.iosb_status)){
			register int i,j;
			iosb_t iosb;

		/* start IOSB contains garbage ...dont check */

			SYS$QIOW (SYNC_EFN,ni_chan,IO$_SENSEMODE|IO$M_CTRL,
				  &iosb,0,0,
				 0, &buffer_desc,0,0,0,0);
			SMG$SET_CURSOR_ABS (&param_id,&1,&1);
			for (i=0; i<iosb.iosb_count;){
				char buf1[256];
				struct dsc$descriptor desc1= {0, 0, 0, 0};
				register unsigned short paramtype;
				char * np;

				desc1.dsc$w_length= sizeof buf1;
				desc1.dsc$a_pointer=  buf1;
				paramtype= (long) * (unsigned short *) &buf[i];
				i+= 2;
				np= nomparam(paramtype&0xfff);
				if (paramtype&0x1000){
					unsigned short len;
					char * cp;

					len= *(unsigned short *) &buf[i];
					i+=2;
					sprintf (buf1,"| %s ",np);
					for (;len--;){
						char buf2[3];
						unsigned char c=buf[i++];
						sprintf (buf2,"%X%X",c>>4,c&15);
						strcat (buf1,buf2);
						if (len) strcat (buf1,"-");
					}
				}
				else{
					char *vp= valparam(
						paramtype&0xfff,
						* (long *) &buf[i]);

					sprintf (buf1,"| %s %s",  np, vp);
					i+=4;
				}
				desc1.dsc$w_length= strlen(buf1);
				SMG$PUT_LINE (&param_id,&desc1);
			}
			if (iosb.iosb_status != SS$_NORMAL) {
				vmsmsgbuf errtxt;
				struct dsc$descriptor err_d= {0, 0, 0, 0};

				err_d.dsc$a_pointer= errtxt;
				(void) emon$translate_status (errtxt,
							      iosb.iosb_status);
				err_d.dsc$w_length= strlen (errtxt);
				SMG$PUT_LINE (&param_id, &err_d);
			}
		}
		else
		 if (start_iosb.iosb_status!=0) { /* si pas stop_port intempestif */

			char buf[80];

			sprintf (buf,
				"Error in start iosb: stat=%x len=%x devdep=%x",
				start_iosb.iosb_status,
				start_iosb.iosb_count,
				start_iosb.iosb_devdep);
			abort_monitor(buf);
			if (start_iosb.iosb_status==SS$_BADPARAM){
				char * cp;

				cp= nomparam(start_iosb.iosb_devdep);
				sprintf (buf,"Bad parameter value, %s",cp);
				emon$put_more_error (buf);
			}
			exit_monitor (start_iosb.iosb_status);
		}
	}
	else{
		ni_error (status,"Error sending start QIO.");
	}

	continue_on_error= istrue (start_iosb.iosb_status)
			   && isfalse(error_stop);

	return istrue(start_iosb.iosb_status);
}

void
get_circuit_counters()
{

	struct {double time;
		iosb_t iosb;
		char data [1484];
		} buffer;
	unsigned long status, SYS$QIOW();
	struct dsc$descriptor data_desc = {0, 0, 0, 0};


	data_desc.dsc$w_length= sizeof buffer.data;
	data_desc.dsc$a_pointer= buffer.data;

	status= SYS$QIOW (SYNC_EFN,ni_chan,	/* read circuit counters */
			  IO$_SENSEMODE|IO$M_RD_COUNT|IO$M_CLR_COUNT,
		          &buffer.iosb,0,0,
		          0,&data_desc,0,0,0,0);
	if (status&buffer.iosb.iosb_status&1){
		int i,j,size;
		for (i=0,size=buffer.iosb.iosb_count; size; i+=j,size-=j){
			unsigned long countval;
			unsigned short partyp= *(short *) &buffer.data[i];
			i+=2;
			size-=2;
			j= (partyp&0x3000)? 4 : 2;/* 2=>lw 1=>hw+map 0=>hw*/
			if (partyp&0x2000)
				countval= *(unsigned long *) &buffer.data[i];
			else    countval= *(unsigned short *) &buffer.data[i];
			switch (partyp&0xfff){
				case NMA$C_CTCIR_ZER: break;
				case NMA$C_CTCIR_DBR:
					delta_circuit_counters.DBR= countval;
					cumulated_circuit_counters.DBR+= countval;
					break;
				case NMA$C_CTCIR_BRC:
					delta_circuit_counters.BRC= countval;
					cumulated_circuit_counters.BRC+= countval;
					break;
				case NMA$C_CTCIR_MBY:
					delta_circuit_counters.MBY= countval;
					cumulated_circuit_counters.MBY+= countval;
					break;
				case NMA$C_CTCIR_LBE:
					delta_circuit_counters.LBE= countval;
					cumulated_circuit_counters.LBE+= countval;
					break;
				case NMA$C_CTCIR_DBS:
					delta_circuit_counters.DBS= countval;
					cumulated_circuit_counters.DBS+= countval;
					break;
				case NMA$C_CTCIR_MNE:
					delta_circuit_counters.MNE= countval;
					cumulated_circuit_counters.MNE+= countval;
					break;
				case NMA$C_CTCIR_BSN:
					delta_circuit_counters.BSN= countval;
					cumulated_circuit_counters.BSN+= countval;
					break;
				case NMA$C_CTCIR_UBU:
					delta_circuit_counters.UBU= countval;
					cumulated_circuit_counters.UBU+= countval;
					break;
				default:
					emon$print_error ("counter #%d=%lu",
						partyp&0xfff,countval);
			}
		}/* end for */
	}
	else{
		ni_error (status&1? (ulong) buffer.iosb.iosb_status : status,
		          "error launching read circuit counters QIO");
	}
}

void
get_line_counters()
{

	struct {double time;
		iosb_t iosb;
		char data [1484];
		} buffer;
	struct dsc$descriptor data_desc = {0, 0, 0, 0};
	unsigned long status, SYS$QIOW();

	data_desc.dsc$w_length= sizeof buffer.data;
	data_desc.dsc$a_pointer= buffer.data;

	status= SYS$QIOW (SYNC_EFN,ni_chan,	/* read controller counters */
/*		  IO$_SENSEMODE|IO$M_RD_COUNT|IO$M_CLR_COUNT|IO$M_CTRL,	*/
/*			don't zero counters for system-wide counters */
			  IO$_SENSEMODE|IO$M_RD_COUNT|IO$M_CTRL,
		          &buffer.iosb,0,0,
		          0,&data_desc,0,0,0,0);
	if (status&buffer.iosb.iosb_status&1){
		int i,j,size;
		for (i=0,size=buffer.iosb.iosb_count; size; i+=j,size-=j){
			unsigned long countval;
			unsigned short partyp= *(short *) &buffer.data[i];
			i+=2;
			size-=2;
			j= (partyp&0x3000)? 4 : 2;
			if (partyp&0x2000)
				countval= *(unsigned long *) &buffer.data[i];
			else    countval= *(unsigned short *) &buffer.data[i];
			switch (partyp&0xfff){
				case NMA$C_CTLIN_ZER: break;
				case NMA$C_CTLIN_DBR:
		delta_line_counters.DBR= countval-cumulated_line_counters.DBR;
					cumulated_line_counters.DBR = countval;
					break;
				case NMA$C_CTLIN_BRC:
		delta_line_counters.BRC= countval-cumulated_line_counters.BRC;
					cumulated_line_counters.BRC = countval;
					break;
				case NMA$C_CTLIN_MBL:
		delta_line_counters.MBL= countval-cumulated_line_counters.MBL;
					cumulated_line_counters.MBL = countval;
					break;
				case NMA$C_CTLIN_RFL:
		delta_line_counters.RFL= countval-cumulated_line_counters.RFL;
					cumulated_line_counters.RFL = countval;
					break;
				case NMA$C_CTLIN_MBY:
		delta_line_counters.MBY= countval-cumulated_line_counters.MBY;
					cumulated_line_counters.MBY = countval;
					break;
				case NMA$C_CTLIN_OVR:
		delta_line_counters.OVR= countval-cumulated_line_counters.OVR;
					cumulated_line_counters.OVR = countval;
					break;
				case NMA$C_CTLIN_LBE:
		delta_line_counters.LBE= countval-cumulated_line_counters.LBE;
					cumulated_line_counters.LBE = countval;
					break;
				case NMA$C_CTLIN_DBS:
		delta_line_counters.DBS= countval-cumulated_line_counters.DBS;
					cumulated_line_counters.DBS = countval;
					break;
				case NMA$C_CTLIN_MBS:
		delta_line_counters.MBS= countval-cumulated_line_counters.MBS;
					cumulated_line_counters.MBS = countval;
					break;
				case NMA$C_CTLIN_BSM:
		delta_line_counters.BSM= countval-cumulated_line_counters.BSM;
					cumulated_line_counters.BSM = countval;
					break;
				case NMA$C_CTLIN_BS1:
		delta_line_counters.BS1= countval-cumulated_line_counters.BS1;
					cumulated_line_counters.BS1 = countval;
					break;
				case NMA$C_CTLIN_BID:
		delta_line_counters.BID= countval-cumulated_line_counters.BID;
					cumulated_line_counters.BID = countval;
					break;
				case NMA$C_CTLIN_BSN:
		delta_line_counters.BSN= countval-cumulated_line_counters.BSN;
					cumulated_line_counters.BSN = countval;
					break;
				case NMA$C_CTLIN_MSN:
		delta_line_counters.MSN= countval-cumulated_line_counters.MSN;
					cumulated_line_counters.MSN = countval;
					break;
				case NMA$C_CTLIN_SFL:
		delta_line_counters.SFL= countval-cumulated_line_counters.SFL;
					cumulated_line_counters.SFL = countval;
					break;
				case NMA$C_CTLIN_CDC:
		delta_line_counters.CDC= countval-cumulated_line_counters.CDC;
					cumulated_line_counters.CDC = countval;
					break;
				case NMA$C_CTLIN_UFD:
		delta_line_counters.UFD= countval-cumulated_line_counters.UFD;
					cumulated_line_counters.UFD = countval;
					break;
				case NMA$C_CTLIN_SBU:
		delta_line_counters.SBU= countval-cumulated_line_counters.SBU;
					cumulated_line_counters.SBU = countval;
					break;
				case NMA$C_CTLIN_UBU:
		delta_line_counters.UBU= countval-cumulated_line_counters.UBU;
					cumulated_line_counters.UBU = countval;
					break;
				default:
					emon$print_error("line counter #%d=%lu",
						partyp&0xfff,countval);
			}
		}/*for */
	}
	else{
		ni_error (status&1? (ulong) buffer.iosb.iosb_status : status,
		          "error launching read line counters QIO");
	}
}

/*	verifie l'etat du port	*/

boolean 
ni_check_iosb (iosb_ptr)
iosb_t * iosb_ptr;
{
	int i;
	char buf[132];
	unsigned long errstatus= iosb_ptr->iosb_devdep;

	if (errstatus==XM$M_STS_ACTIVE) return true;

	strcpy (buf,"Unit and line status:");
	if (errstatus&XM$M_STS_ACTIVE) {
		strcat (buf, " ACTIVE");
		errstatus &= ~XM$M_STS_ACTIVE;
	}
	if (errstatus&XM$M_STS_BUFFAIL) {
		strcat (buf, " BUFFAIL");
		errstatus &= ~XM$M_STS_BUFFAIL;
	}
	if (errstatus&XM$M_STS_TIMO) {
		strcat (buf, " TIMO");
		errstatus &= ~XM$M_STS_TIMO;
	}
	for (i=256; i<0x10000; i<<=1)
		if (errstatus&i) {
			char buf1[20];
			sprintf (buf1," 0x#%x", i);
			strcat (buf, buf1);
		}
	emon$put_error (buf);
	strcpy (buf, "Error summary:");
	if (errstatus&XM$M_ERR_FATAL) {
		strcat (buf, " FATAL");
		errstatus &= ~XM$M_ERR_FATAL;
	}
	for (i=0x10000; i<0x1000000; i<<=1)
		if (errstatus&i) {
			char buf1[20];
			sprintf (buf1," 0x#%x", i);
			strcat (buf, buf1);
		}
	emon$put_error (buf);
	if (errstatus&0xff) {
		sprintf (buf, "reserved first byte not null: 0x%x",
			 errstatus&0xff);
		emon$put_error(buf);
	}
	if (errstatus>>24) {
		sprintf (buf, "reserved last byte not null: 0x%x",
			 errstatus>>24);
		emon$put_error(buf);
	}

	return false;
}


/* stop controller AST routine: private routine	*/
/* stop the ethernet promiscuous channel */
/* qio's pending will or SS$_CANCEL, or SS$_ABORT */

static
void
stop_controller()
{
	ulong status, SYS$QIOW(), SYS$SETPRI();
	iosb_t iosb;

	status= SYS$QIOW (SYNC_EFN_AST, ni_chan,
			  IO$_SETMODE|IO$M_CTRL|IO$M_SHUTDOWN,
			  &iosb,0,0,
			  0,0,0,0,0,0);

	if (mon_old_priority>=0) {	/* reset normal priority */
		SYS$SETPRI (0,0,mon_old_priority,0);
		mon_old_priority= -1;
	}

	if (status&iosb.iosb_status&1) ;
	else{
		ni_error ( (status&1)? (ulong) iosb.iosb_status : status,
			   "Erreur dans la routine Stop Port");
	}
}

/* stop port: entry point; AST routine	*/
/*	stop port, and call rtn after it has been stopped */

boolean
stop_port(rtn)
void (* rtn)();
{
	void stop_controller();

	continue_on_error= false;

	if (isfalse(start_iosb.iosb_status)) return false; /* not started */
	start_iosb.iosb_status= false;
	ni_call= rtn;
	stop_controller();
	return true;
}

/* exit handler: private routine */
/*	stop port, don't call routine after stopping port */

static
int
ni_exit(status)
int status;
{
	continue_on_error = false;
	stop_port(0);
	deassign_port();
	free (membufaddr);
	return status;
}

static boolean	firsterr;
static void prtframe (q)
pckt_t *q;
{
	char buf[256];

	sprintf (buf, 
"Illegal frame received, proto= %02X-%02X, 802 = %02X %02X %02X %02X %02X %02X",
		q->tag.protocol.bytes[0], 
		q->tag.protocol.bytes[1],
		q->tag.padto20bytes[0],
		q->tag.padto20bytes[1],
		q->tag.padto20bytes[2],
		q->tag.padto20bytes[3],
		q->tag.padto20bytes[4],
		q->tag.padto20bytes[5]);
	if istrue (firsterr) {
		emon$put_error(buf);
		firsterr= false;
	}
	else	emon$put_more_error(buf);

	sprintf (buf,
"IOSB: %04X %04X %08X, dest: %02X-%02X-%02X-%02X-%02X-%02X, src: %02X-%02X-%02X-%02X-%02X-%02X",
		q->iosb.iosb_status,
		q->iosb.iosb_count,
		q->iosb.iosb_devdep,
		q->tag.dest_addr.bytes[0],
		q->tag.dest_addr.bytes[1],
		q->tag.dest_addr.bytes[2],
		q->tag.dest_addr.bytes[3],
		q->tag.dest_addr.bytes[4],
		q->tag.dest_addr.bytes[5],
		q->tag.from_addr.bytes[0],
		q->tag.from_addr.bytes[1],
		q->tag.from_addr.bytes[2],
		q->tag.from_addr.bytes[3],
		q->tag.from_addr.bytes[4],
		q->tag.from_addr.bytes[5]);
	emon$put_more_error (buf);
}

ni_read(stat_routine)
void (* stat_routine)();
{
#pragma nostandard
	globalref time_t EXE$GQ_SYSTIME;
#pragma standard

static	const	packet_header_t null802; /* = 0 because static ... 802.3 */

	ulong status, SYS$QIO(), SYS$SYNCH();
	int nbqio;		/* nb de QIO en cours */
	int N= parameters.qionb;/* nb de QIO simultanées maximum */
	boolean port_shutdown;	/* true if stop because port was shut down */
	register pckt_t *q;
#ifdef DRIVER_DEBUG
	boolean	illframe= false;
#endif
	pckt_t *maxq= pckt_id+parameters.qionb-1; /* adresse du dernier buffer valide*/

#ifdef DRIVER_DEBUG
	firsterr= true;
#endif

	if (isfalse(start_iosb.iosb_status)) return;	/* port stoppé */

	    	/* on lance d'abort N-1 QIO's */

	for (q= pckt_id, nbqio=0 ; nbqio < N-1; q++, nbqio++){
		q->tag.bytes= null802.bytes;			/* 802.3 */
		status= SYS$QIO (32,ni_chan,IO$_READLBLK,&q->iosb,0,0,
				 &q->buffer,sizeof q->buffer,0,0,&q->tag,0);
		if ( (status&1) == 0){
			q= maxq;	/* 1ere QIO a tester */
			break;
		}
	}

	/* invariant ici: q= maxq => QIO a tester = la premiere */

assert (q==maxq);

	    	/* on lance la Nieme QIO, et on boucle */

	if istrue (status)
	    for (;;){
	    	q->tag.bytes= null802.bytes;			/* 802.3 */
	    	status= SYS$QIO (32,ni_chan,IO$_READPBLK,&q->iosb,0,0,
	    	  		 &q->buffer,sizeof q->buffer,0,0,&q->tag,0);
	    	if (status&1){
		   if (q==maxq) q= pckt_id;		/* q pointe sur la */
		   else q++;			    /* dernière QIO testée */
		   SYS$SYNCH (32,&q->iosb);
		   q->time= EXE$GQ_SYSTIME;

		   if (q->iosb.iosb_status&1){
	    		if (  * (long *) q->tag.padto20bytes 
	    		    ||* (short *) & q->tag.padto20bytes[4]){ /* 802.3 */
				if isfalse (addprotocol802 (&q->tag)) break;

#ifdef DRIVER_DEBUG
		if (q->tag.protocol.word==0) {
		illframe= true;
		break;
	}
#endif /* DRIVER_DEBUG */
	    	    	}
#ifdef CHECK_802
			else if ( ((q->tag.protocol.bytes[0]<<8)
				   +q->tag.protocol.bytes[1]) <=1500)
#ifdef DRIVER_DEBUG
				{
					illframe= true;
					break;
				}
#else
				   	q->tag.protocol.word= 0;
#endif
#endif
	    		(* stat_routine)(q);
	    	    }   else 	if (isfalse(start_iosb.iosb_status)
				    || isfalse(continue_on_error))
					break;
				else {
			    		if (  * (long *) q->tag.padto20bytes 
			    		    ||* (short *) & q->tag.padto20bytes[4]){ /* 802.3 */
						if isfalse (addprotocol802 (&q->tag)) break;
			    	    	}
#ifdef CHECK_802
					else if ( ((q->tag.protocol.bytes[0]<<8)
						   +q->tag.protocol.bytes[1]) <=1500)
#ifdef DRIVER_DEBUG
				{
					illframe= true;
					break;
				}
#else
						   	q->tag.protocol.word= 0;
#endif
#endif
		   	 		(* stat_routine)(q);
				}
	       }    else break;

	    }/* for ever */

	    port_shutdown= isfalse (start_iosb.iosb_status);

	    if isfalse(port_shutdown) stop_controller();

	    /* si status = false c'est qu'on n'a pas pu lancer une QIO */
	    /* sinon, c'est qu'on a trouve une QIO qui s'est mal passée */
	    /* si le port n'etait pas stoppé, il le sera par ni_error */
	    /* si port_shutdown, alors SS$_CANCEL et SS$_ABORT sont  */
	    /* normaux dans l'iosb, et SS$_DEVINACT est normal pour QIO */

	    if (istrue(status)) {	/* sortie sur erreur IOSB  */
		if ( isfalse (port_shutdown) ||
#ifdef DRIVER_DEBUG
		     istrue (illframe) ||
#endif
		     ( (q->iosb.iosb_status!=SS$_CANCEL)
		     &&(q->iosb.iosb_status!=SS$_ABORT)) ){
			char buf[132],buf1[80];

#ifdef DRIVER_DEBUG
			if istrue (illframe) prtframe (q);
			else
#endif
			{
			sprintf (buf,
				"Error in read iosb: stat=%x len=%x %s",
				q->iosb.iosb_status,
				q->iosb.iosb_count,
				translate_ni_status (q->iosb.iosb_devdep,buf1)
	    			);
			ni_error (q->iosb.iosb_status,buf);
			}
		}
	    } else {			/* sortie sur erreur QIO */
		if ( isfalse (port_shutdown)||
		          (status!=SS$_DEVINACT)) {
			vmsmsgbuf errbuf;
#ifdef DRIVER_DEBUG
			firsterr= false;
#endif
			ni_error (status,"Error sending read QIO");
			emon$translate_status (errbuf, status);
			emon$put_more_error (errbuf);
		}
	    }

	    /* on est ici avec start_iosb.iosb_status = 0 */
	    /* cas anormal: port_shutdown == false: ya eu un pepin */
	    /* il reste nbqio QIO a attendre */
	    /* q pointe sur la derniere QIO testee */
	    /* cette boucle a ete réécrite séparément */
	    /* pour ne pas ralentir la boucle principale */

	for (; nbqio> 0; nbqio--) {

		if (q==maxq) q= pckt_id;
		else q++;

		SYS$SYNCH (32, &q->iosb);
		q->time= EXE$GQ_SYSTIME;
		if istrue (q->iosb.iosb_status) {
			if (  * (long *) q->tag.padto20bytes
			    ||* (short *) & q->tag.padto20bytes[4]){ /* 802.3 */
				if isfalse (addprotocol802(&q->tag)) break;

#ifdef DRIVER_DEBUG
				if (q->tag.protocol.word==0) {
					prtframe (q);
					break;
				}
#endif /* DRIVER_DEBUG */

	    		}

#ifdef CHECK_802
			else if ( ((q->tag.protocol.bytes[0]<<8)
				   +q->tag.protocol.bytes[1]) <=1500)
#ifdef DRIVER_DEBUG
				{ prtframe (q); continue; }
#else
			   	q->tag.protocol.word= 0;
#endif /* DRIVER_DEBUG */
#endif /* CHECK_802 */

	    		(* stat_routine)(q);
	    	}
		else{
			char buf[80];
			if (q->iosb.iosb_status==SS$_ABORT) continue;
			sprintf (buf,
				"Error in read iosb: stat=%x len=%x stat=%x",
				q->iosb.iosb_status,
				q->iosb.iosb_count,
				q->iosb.iosb_devdep);
			ni_error (q->iosb.iosb_status,buf);
		}
	}

	    /* si ni_call est non nul, alors on appelle la routine */

	if ( ni_call != NULL ) (* ni_call)();
}

boolean
ni$set_error()
{
	error_stop= clipresent ("halt");
	return true;
}
