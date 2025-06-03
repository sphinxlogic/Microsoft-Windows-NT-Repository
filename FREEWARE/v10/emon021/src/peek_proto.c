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
#include "$sdl:nmadef"
#include "$sdl:stardefqz.h"	/* for XMDEF */
#include "emon.h"

#include <descrip.h>
#include <dvidef.h>
#include <iodef.h>
#include <ssdef.h>

static boolean	(* ni_call)();
static iosb_t	start_iosb= {0,0,0};
static ushort	ni_chan= 0;
static long	mon_old_priority= -1;

ulong param_id;

boolean ni_check_iosb();

static ni_error(status,text)
ulong status;
char *text;
{
	printf ("fatal error: %s",text);
	exit (status);
}



boolean
stop_port(rtn)
boolean (* rtn)();
{
	void stop_controller();

	if (isfalse(start_iosb.iosb_status)) return false;
	start_iosb.iosb_status= false;
	ni_call= rtn;
	stop_controller();
	return true;
}

static int ni_exit();

static ulong exit_status;
static struct {	ulong link;
		ulong handler;
		ulong arg_num;
		ulong sts_addr;
		} desblk= {0,ni_exit,1,&exit_status};

assign_port(port)
char * port;
{
	static const $DESCRIPTOR (deuna_desc,"XEA0:");
	static const $DESCRIPTOR (deqna_desc,"XQA0:");
	static const $DESCRIPTOR (debnt_desc,"ETA0:");
	static const $DESCRIPTOR (desva_desc,"SVA0:");
	static const $DESCRIPTOR (deesa_desc,"ESA0:");
	struct dsc$descriptor * tab_desc[]=
			{&deuna_desc,
			 &deqna_desc,
			 &debnt_desc,
			 &desva_desc,
			 &deesa_desc};
	unsigned long status;
	int i;
	struct dsc$descriptor dev_desc= {strlen (port), 0, 0, port};

	status= SYS$ASSIGN (&dev_desc,&ni_chan,0,0);

	if (status&1){
		static char devnam[8];
		static unsigned short devnam_len;
		static const struct {unsigned short buflen,bufcod;
			             unsigned long retadr,retlen,listend;}
			itmlst =
				{sizeof devnam,DVI$_DEVNAM,&devnam,&devnam_len,
				 0};
		iosb_t iosb;

		SYS$GETDVIW (0,ni_chan,0,&itmlst,&iosb,0,0,0);
		printf ("l'unite allouee est %.*s\n",devnam_len,devnam);
	}
	else{
		ni_error (status,"Assignation DEUNA impossible");
	}
}

deassign_port()
{
	ulong status;
	status= SYS$DASSGN (ni_chan);
	if (status&1) ni_chan=0;
	else	ni_error (status,"deassignation NI device impossible");
}


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
		case NMA$C_PCLI_ACC: np= "Sharability"; break;
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
				default: sprintf (buf,"format #%d",u);
				np= buf;
			}
			break;
		case NMA$C_PCLI_PTY:
			sprintf (buf,"%2.0X-%2.0X",u&0xff, u>>8);
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
		default: sprintf (buf,"%lu",u);
			np= buf;
	}
	return np;
}

boolean
start_port()
{
	char buf[512];
	$DESCRIPTOR (buffer_desc,&buf);

	unsigned long status;

	{
		register int i,j;
		iosb_t iosb;

		/* start IOSB contains garbage ...dont check */

		SYS$QIOW (0,ni_chan,IO$_SENSEMODE|IO$M_CTRL,&iosb,0,0
			 ,0,&buffer_desc,0,0,0,0);
		SMG$SET_CURSOR_ABS (&param_id,&1,&1);
		for (i=0; i<iosb.iosb_length;){
			char buf1[256], buf2[20];
			$DESCRIPTOR (desc1,buf1);
			register unsigned short paramtype;
			char * np;

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
			puts (buf1);
		}
	}
}

static
void
stop_controller()
{
	ulong status;
	iosb_t iosb;

	status= SYS$QIOW (0,ni_chan,IO$_SETMODE|IO$M_CTRL|IO$M_SHUTDOWN,
			  &iosb,0,0,
			  0,0,0,0,0,0);

	if (mon_old_priority>=0) {
		SYS$SETPRI (0,0,mon_old_priority,0);
		mon_old_priority= -1;
	}

	if (status&iosb.iosb_status&1) ;
	else{
		ni_error ( (status&1)? (ulong) iosb.iosb_status : status,
			   "Erreur dans la routine Stop Port");
	}
}

static
int
ni_exit(status)
int status;
{
	stop_port(0);
	deassign_port();
	return status;
}

void
get_circuit_counters()
{

	struct {double time;
		iosb_t iosb;
		char data [1484];
		} buffer;
	$DESCRIPTOR (data_desc,buffer.data);

	unsigned long status;

	status= SYS$QIOW (0,ni_chan,
			  IO$_SENSEMODE|IO$M_RD_COUNT|IO$M_CLR_COUNT,
		          &buffer.iosb,0,0,
		          0,&data_desc,0,0,0,0);
	if (status&buffer.iosb.iosb_status&1){
		int i,j,size;
		for (i=0,size=buffer.iosb.iosb_length; size; i+=j,size-=j){
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
					emon$put_error ("counter #%d=%lu",
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
	$DESCRIPTOR (data_desc,buffer.data);

	unsigned long status;

	status= SYS$QIOW (0,ni_chan,
/*		  IO$_SENSEMODE|IO$M_RD_COUNT|IO$M_CLR_COUNT|IO$M_CTRL,	*/
/*			don't zero counters for system-wide counters */
			  IO$_SENSEMODE|IO$M_RD_COUNT|IO$M_CTRL,
		          &buffer.iosb,0,0,
		          0,&data_desc,0,0,0,0);
	if (status&buffer.iosb.iosb_status&1){
		int i,j,size;
		for (i=0,size=buffer.iosb.iosb_length; size; i+=j,size-=j){
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
					emon$put_error("line counter #%d=%lu",
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

emon$put_error (mess)
char * mess;
{
	printf ("Error: %s\n", mess);
}

emon$put_more_error (mess)
char * mess;
{
	printf ("Error: %s\n", mess);
}

main()
{
	char port [255];
	scanf ("%s",port);
	assign_port(port);
	start_port();
	deassign_port();
}
