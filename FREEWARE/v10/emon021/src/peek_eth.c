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
#include "types.h"

#include <descrip.h>
#include <dvidef.h>
#include <iodef.h>
#include <ssdef.h>

static iosb_t	start_iosb= {0,0,0};
static ushort	ni_chan= 0;

static ni_error(status,text)
ulong status;
char *text;
{
	printf ("fatal error: %s",text);
	exit (status);
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
	} else 	strcat (buf, " NOT active");
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

assign_port(port)
char * port;
{
	unsigned long status;
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
		case NMA$C_PCLI_DUP: np= "Duplex mode"; break;
		case NMA$C_PCLI_PRO: np= "Protocol"; break;
		case NMA$C_PCLI_RTT: np= "Retransmit timer"; break;
		case NMA$C_PCLI_NMS: np= "DMP/DMF sync chars"; break;
		case NMA$C_PCLI_MNTL: np= "Maintenance loopback mode"; break;
		case NMA$C_PCLI_FRA: np= "Framing address (BISYNC)"; break;
		case NMA$C_PCLI_STI1: np= "State info 1st longword"; break;
		case NMA$C_PCLI_STI2: np= "State info 2nd longword"; break;
		case NMA$C_PCLI_TMO: np= "Time out for CTS"; break;
		case NMA$C_PCLI_MCL: np= "Clear modem after deassign"; break;
		case NMA$C_PCLI_SYC: np= "Synchronization char"; break;
		case NMA$C_PCLI_BPC: np= "Bits per character"; break;
		case NMA$C_PCLI_LNS: np= "Line speed"; break;
		case NMA$C_PCLI_SWI: np= "Switch allowed"; break;
		case NMA$C_PCLI_HNG: np= "Hangup allowed"; break;
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

		status= SYS$QIOW (0,ni_chan,IO$_SENSEMODE|IO$M_CTRL,&iosb,0,0
			 ,0,&buffer_desc,0,0,0,0);

		if ((status&1)==0) ni_error (status, "Sende mode failed");

		ni_check_iosb (&iosb);

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

char *
counter_name(val)
int val;
{
	switch (val){
	case NMA$C_CTCIR_ZER: return "Seconds since zeroed";
	case NMA$C_CTCIR_DBR: return "Data blocks received";
	case NMA$C_CTCIR_BRC: return "Bytes received";
	case NMA$C_CTCIR_MBY: return "Multicast blocks received";
	case NMA$C_CTCIR_LBE: return "Local buffer errors";
	case NMA$C_CTCIR_UBU: return "User buffer unavailable";
	case NMA$C_CTCIR_DBS: return "Data blocks sent";
	case NMA$C_CTCIR_MNE: return "Unknown multicast address for protocol";
	case NMA$C_CTCIR_BSN: return "Bytes sent";
	case NMA$C_CTCIR_SIE: return "Selection intervals elapsed";
	case NMA$C_CTCIR_DEI: return "Data errors inbound";
	case NMA$C_CTCIR_DEO: return "Data errors outbound";
	case NMA$C_CTCIR_RRT: return "Remote reply timeouts";
	case NMA$C_CTCIR_LRT: return "Local reply timeouts";
	case NMA$C_CTCIR_RBE: return "Remote buffer errors";
	case NMA$C_CTCIR_SLT: return "Selection timeouts";
	default:{
		static errbuf[20];
		sprintf (errbuf, "counter #%d", val);
		return errbuf;
	 }
	}
}

void
get_circuit_counters()
{

	struct {double time;
		iosb_t iosb;
		char data [1484];
		} buffer;

	struct dsc$descriptor data_desc
		= {sizeof buffer.data, 0, 0, &buffer.data};

	unsigned long status;

	status= SYS$QIOW (0,ni_chan,
			  IO$_SENSEMODE|IO$M_RD_COUNT/*|IO$M_CLR_COUNT*/,
		          &buffer.iosb,0,0,
		          0,&data_desc,0,0,0,0);

	if ((status&1) == 0) ni_error (status, "Error getting device counters");

	ni_check_iosb (&buffer.iosb);

	if (status&buffer.iosb.iosb_status&1){
		int i,j,size;
		for (i=0,size=buffer.iosb.iosb_length; size; i+=j,size-=j){
			unsigned long countval, mapval;
			unsigned short partyp= *(short *) &buffer.data[i];
			i+=2;
			size-=2;
			if ((partyp&0x8000)==0) {
				emon$put_error ("Error parsing item list");
				break;
			}
			/* siz */
			switch (partyp&0x6000){
				case 0x6000: j= 4;
					countval= *(unsigned long *) &buffer.data[i];
					break;
				case 0x4000: j= 2;
					countval= *(unsigned short *) &buffer.data[i];
					break;
				case 0x2000: j= 1;
					countval= buffer.data[i];
					break;
			}
			if (partyp&0x1000) {
				mapval= *(unsigned short *) &buffer.data[i+j];
				j += 2; /* hw map */
				printf ("%12d %s 0x%x\n",countval, 
					counter_name(partyp&0xfff), mapval);
			} else
				printf ("%12d %s\n",countval, 
					counter_name(partyp&0xfff));
		}/* end for */
	}
	else{
		ni_error (status&1? (ulong) buffer.iosb.iosb_status : status,
		          "error launching read circuit counters QIO");
	}
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

main (argc, argv)
int argc;
char **argv;
{
	if (argc>1) assign_port (argv[1]);
	else{
		char port [255];
		gets (port);
		assign_port(port);
	}
	start_port();
	get_circuit_counters();
	deassign_port();
}
