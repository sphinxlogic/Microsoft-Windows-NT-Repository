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
#include "$sdl:nmadef"
#include "$sdl:stardefqz.h"	/* for XMDEF */
#include "types.h"

#include <dcdef.h>
#include <descrip.h>
#include <dvidef.h>
#include <iodef.h>
#include <ssdef.h>

static iosb_t	start_iosb= {0,0,0};
static ushort	ni_chan= 0;
static int	ni_type;

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
		static devclass;
		static const struct {unsigned short buflen,bufcod;
			             unsigned long retadr,retlen;}
			itmlst[] ={
				{sizeof devnam,DVI$_DEVNAM,&devnam,&devnam_len},
				{sizeof devclass,DVI$_DEVCLASS,&devclass,0},
				{sizeof ni_type,DVI$_DEVTYPE,&ni_type,0},
				{0,0,0,0}};
		iosb_t iosb;

		SYS$GETDVIW (0,ni_chan,0,&itmlst,&iosb,0,0,0);
		printf ("l'unite allouee est %.*s\n",devnam_len,devnam);
		if (devclass!=DC$_SCOM) {
			printf ("Pas un periph de communications");
			exit (SS$_NOSUCHDEV);
		}
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

		for (i=0; i<iosb.iosb_count;){
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
		for (i=0,size=buffer.iosb.iosb_count; size; i+=j,size-=j){
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

read_ctr ()
{
#define	ecr$readintctr 1
	iosb_t	iosb;
	struct { struct
		{unsigned short zerctr;	/* seconds since last zeroed */
		 unsigned long	brcctr,
				bsnctr,
				dbrctr,
				dbsctr,
				mbyctr,
				mblctr,
				bidctr,
				bs1ctr,
				bsmctr;
		unsigned short	sflctr,
				sflmap,
				rflctr,
				rflmap,
				ufdctr,
				ovrctr,
				sbuctr,
				ubuctr;
	/* fin des compteurs "standard" */
		unsigned long	mbsctr,
				msnctr,
				fmt802ectr,
				fmtethctr,
				fmt802ctr;
		unsigned short	lbectr,
				cdcctr,
				iltctr,
				il2ctr,
				rsfctr,
				chlmsg,
				chlerr,
				sidmsg,
				siderr,
				rqcmsg,
				rqcerr;
		} std;
		variant_union {
		struct {
			unsigned long	es_rcvunc,
					es_rcvcha,
					es_badmca,
					es_xmtcop,
					es_xmtoar,
					es_xmtcxb,
					es_xmtiar;
			unsigned short	es_xmttmo,
					es_initmo,
					es_xmtufl,
					es_fatmer,
					es_fatbbl;
		 } esdriver;

#define bna$c_nofreeq 32
	struct {			/* Driver's Internal Counters	*/
	unsigned short
	ECRB$W_BNA_RLIFE,		/* Receiver alive indicator	*/
	ECRB$W_BNA_RLIFESAV,		/*   used for comparison	*/
	ECRB$W_BNA_XLIFE,		/* Transmit alive indicator	*/
	ECRB$W_BNA_FFIRST,		/* no. of FFI auto restart	*/
	ECRB$W_BNA_ADAPERR;		/* no. of fatal adapter error	*/
	unsigned char
	ECRB$W_BNA_ADAPERRCODE[2];	/* Fatal adapter error reason code */

#define ECRB$C_BNA_ADAPERR_INTR	1	/*   error during interrupt servicing */
#define ECRB$C_BNA_ADAPERR_ETYPE 2	/*   fatal error type (PS)	*/
#define ECRB$C_BNA_ADAPERR_PCMD 3	/*   port command failed	*/
#define ECRB$C_BNA_ADAPERR_RETRY 4	/*   initialization retries exhausted */
#define ECRB$C_BNA_ADAPERR_SLFTST 5	/*   self-test timeout		*/
#define ECRB$C_BNA_ADAPERR_INIT 6	/*   init timeout		*/
#define ECRB$C_BNA_ADAPERR_INITFAIL 7	/*   init command failed	*/
#define ECRB$C_BNA_ADAPERR_SHUT 8	/*   shutdown timeout		*/
#define ECRB$C_BNA_ADAPERR_CMDQLOCK 9	/*   command queue locked	*/
#define ECRB$C_BNA_ADAPERR_RSPQLOCK 10	/*   response queue locked	*/
#define ECRB$C_BNA_ADAPERR_FREEQLOCK 11	/*   free queue locked		*/

	unsigned short
	ECRB$W_BNA_OFFLERR;		/* Port offline error code	*/
#define ECRB$C_BNA_OFFLERR_UNTINT 1	/*   Unit initialization failed	*/
#define ECRB$C_BNA_OFFLERR_ADAPRST 2	/*   Adapter restart failed	*/

	unsigned char
	ECRB$B_BNA_DSABL,		/* no. of forced port disable	*/
	ecrb$$reserv_btye;		/* (reserved)			*/

	unsigned short
	ECRB$W_BNA_CMDTMOCNT,		/* no. of command timeout	*/
	ECRB$W_BNA_PCMDTMOCNT,		/* no. of port command timeout	*/
	ECRB$W_BNA_XMTTMOCNT,		/* no. of transmit timeout	*/
	ECRB$W_BNA_RCVTMOCNT,		/* no. of receive timeout	*/
	ECRB$W_BNA_UNDEFTMO,		/* no. of UNDEFINED state timeout */
	ecrb$$reserv_word1[3],		/* (reserved)	*/
					/* (reserved)	*/

	ECRB$W_BNA_XMTERR,		/* no. of fatal transmit errors	*/
	ECRB$W_BNA_LASTXMTERR,		/* last transmit error code	*/
	ECRB$W_BNA_XMTDEFER,		/* no. of deferred transmits	*/

#define ECRB$C_BNA_XMTDEFER_THROTTLE 1	/*     transmitter is full	*/
#define ECRB$C_BNA_XMTDEFER_NODG 2	/*     no datagram available	*/
#define ECRB$C_BNA_XMTDEFER_MAPFAIL 3	/*     failed to map buffer	*/
#define ECRB$C_BNA_XMTDEFER_SNDFAIL 4	/*     send datagram failed	*/

	ECRB$W_BNA_RCVERR,		/* no. of receive errors	*/
	ECRB$W_BNA_RCVINACTIVE,		/* no. of packets with no user	*/
	ECRB$W_BNA_LOGOUTERR,		/* no. of scan logout area errors */
	ECRB$W_BNA_DUPRSPDG;		/* no. of duplicate response datagram */
	unsigned char
	ECRB$B_BNA_ILLRESTART,		/* no. of illegal auto restarts	*/
	ECRB$B_BNA_ILLSTRTPRO;		/* no. of illegal start protocols */

	unsigned short
	ECRB$W_BNA_ASHUTDOWN,		/* no. of asynchronous shutdown	*/
	ECRB$W_BNA_BADSTATE,		/* no. of bad state encountered	*/
	ECRB$W_BNA_UNEXPUNDEF,		/* no. of unexpected UNDEF state */
	ECRB$W_BNA_POWERFAIL,		/* no. of powerfail recovery	*/
	ECRB$W_BNA_FORKINIT,		/* no. of INIT state in fork	*/
	ECRB$W_BNA_CMDQLOCK,		/* no. of command queue lockup	*/
	ECRB$W_BNA_RSPQLOCK,		/* no. of response queue lockup	*/
	ECRB$W_BNA_FREEQLOCK;		/* no. of free queue lockup	*/

	unsigned long
	ECRB$L_BNA_MAX_XMTIME;		/* Max time to xmit pkt (tick)	*/

	unsigned char
	ECRB$B_BNA_XMTIME_TOOLONG,	/* no. of too long to transmit	*/
	ECRB$B_BNA_UCBTMO;		/* no. of UCB timeout		*/
	unsigned short
	ecrb$$w_reserved_2,		/* (reserved)	*/
	ECRB$W_BNA_MAX_IQRETRY,		/* Max no of INSQTI retry so far */
	ECRB$W_BNA_MAX_RQRETRY,		/* Max no of REMQHI retry so far */
	ECRB$W_BNA_IQRETRY,		/* no. of times INSQTI has to retry */
	ECRB$W_BNA_RQRETRY,		/* no. of times REMQHI has to retry */

	ECRB$W_BNA_FREQDELAY,		/* no. of free queue insert delay */
	ecrb$$w_reserved_3,		/* (reserved)	*/
	ECRB$W_BNA_CMDQDELAY,		/* no. of command queue delay	*/
	ECRB$W_BNA_BDTALLOCNUM,		/* no. of BDT entries allocated	*/
	ECRB$W_BNA_NOBUF,		/* no. of receive buffer unavailable */
	ECRB$W_BNA_NOBDT;		/* no. of BDT unavailable	*/
	unsigned long
	ECRB$L_BNA_MAX_PCSPINTIM;	/* (reserved)	*/

	unsigned short
	ECRB$AW_BNA_FQECNT[bna$c_nofreeq];/* no. of free queue empty interrupts*/

	} debna;

		} $$specific_counters;
		char pleindebytes[400];
	   } buffer;
	unsigned long status;

	status= SYS$QIOW (0,ni_chan,
			  IO$_ACCESS,
		          &iosb,0,0,
		          &buffer, sizeof buffer , ecr$readintctr,0,0,0);

	if ((status&1) == 0) ni_error (status, "Error getting device counters");

	ni_check_iosb (&iosb);

	if (status & iosb.iosb_status & 1){
		printf ("Taille: 0d%d (exp:0d%d)\n", iosb.iosb_count,
				 sizeof buffer.std+sizeof buffer.esdriver);
		/* partie standard */
		printf ("Paquets recus:  eth: %d; 802: %d; 802E: %d\n",
				buffer.std.fmtethctr,
				buffer.std.fmt802ctr,
				buffer.std.fmt802ectr);
		printf ("Restart failures: %d\n", buffer.std.rsfctr);
		printf ("Longueur invalide: total:%d; trop court: %d\n",
				buffer.std.iltctr, buffer.std.il2ctr);
		printf ("channel Loopbacks: sent: %d; failures: %d\n",
				buffer.std.chlmsg, buffer.std.chlerr);
		printf ("System ID msgs: sent: %d; failures: %d\n",
				buffer.std.sidmsg, buffer.std.siderr);
		printf ("Request counters msgs: sent: %d; failures: %d\n",
				buffer.std.rqcmsg, buffer.std.rqcerr);
		/* fin de la partie standard */
	switch (ni_type) {
	case	DT$_ES_LANCE:
		printf ("Receives: chained: %d; unchained: %d\n",
			buffer.esdriver.es_rcvunc, buffer.esdriver.es_rcvcha);
	    printf ("transmits: copied: %d; uncopied: %d; area: in %d; out%d\n",
			buffer.esdriver.es_xmtcop, buffer.esdriver.es_xmtcxb,
			buffer.esdriver.es_xmtiar, buffer.esdriver.es_xmtoar);
		printf ("timeouts: XMT: %d; INIT: %d\n",
			buffer.esdriver.es_xmttmo, buffer.esdriver.es_initmo);
		printf ("Transmitter underflows: %d\n",
			buffer.esdriver.es_xmtufl);
		printf	("fatal errors: MERR: %d; BABL: %d\n", 
			buffer.esdriver.es_fatmer, buffer.esdriver.es_fatbbl);
		break;
	case	DT$_ET_DEBNA:
#define	dbuf	buffer.debna
		printf	("buffer allocation: count: %d BDT; failures: BDT: %d; CXB: %d\n",
			  dbuf.ECRB$W_BNA_BDTALLOCNUM,
			  dbuf.ECRB$W_BNA_NOBDT,dbuf.ECRB$W_BNA_NOBUF);
		printf (
      "Timeouts: CMD %d; port CMD: %d; XMT: %D; RCV: %d; UNDEFINED state: %d\n",
			 dbuf.ECRB$W_BNA_CMDTMOCNT,
			 dbuf.ECRB$W_BNA_PCMDTMOCNT,
			 dbuf.ECRB$W_BNA_XMTTMOCNT,
			 dbuf.ECRB$W_BNA_RCVTMOCNT,
			 dbuf.ECRB$W_BNA_UNDEFTMO);
		if (dbuf.ECRB$W_BNA_XMTERR) {
	    		char *msg,errbuf[40];
			switch (dbuf.ECRB$W_BNA_LASTXMTERR) {
			case 9:	/* no reason */ msg= "(no reason)"; break;
			case 10:/* LCAR */ msg= "Carrier lost"; break;
			case 11:/* RTRY */ msg= "Failed after 16 retries";break;
			case 12:/* LCOL */ msg= "Late collision"; break;
			default: sprintf (errbuf, "Unknown reason 0x%X",
					  dbuf.ECRB$W_BNA_LASTXMTERR);
				 msg= errbuf;
			}
			printf ("transmit errors: %d; last error= %s\n",
				dbuf.ECRB$W_BNA_XMTERR, msg);
				
		} else printf ("no transmit errors\n");

		if (dbuf.ECRB$W_BNA_XMTDEFER) {
	    		char *msg,errbuf[40];
			switch (dbuf.ECRB$W_BNA_XMTDEFER>>8) {
			case	ECRB$C_BNA_XMTDEFER_THROTTLE:
				msg= "transmitter is full"; break; 
			case	ECRB$C_BNA_XMTDEFER_NODG: 
				msg= "no datagram available"; break;
			case	ECRB$C_BNA_XMTDEFER_MAPFAIL: 
				msg= "failed to map buffer"; break;
			case	ECRB$C_BNA_XMTDEFER_SNDFAIL: 
				msg= "send datagram failed"; break;
			default: sprintf (errbuf, "Unknown DEFER reason 0x%X",
				dbuf.ECRB$W_BNA_XMTDEFER>>8);
				msg= errbuf;
			}
			printf ("deferred transmits: %d; last error= %s\n",
				dbuf.ECRB$W_BNA_XMTERR & 0xff, msg);
		} else printf ("no deferred transmits\n");

		if (dbuf.ECRB$W_BNA_RCVERR) 
			printf ("receive errors: %d\n", dbuf.ECRB$W_BNA_RCVERR);
		else	printf ("no receive errors\n");

		if (dbuf.ECRB$W_BNA_OFFLERR) {
	    		char *msg,errbuf[40];
			 switch (dbuf.ECRB$W_BNA_OFFLERR) {
			 case ECRB$C_BNA_OFFLERR_UNTINT: 
				msg= "Unit initialization failed"; break;
			 case ECRB$C_BNA_OFFLERR_ADAPRST:
				msg= "Adapter restart failed"; break;
			 default: sprintf (errbuf, "unknown reason 0x%D\n",
					   dbuf.ECRB$W_BNA_OFFLERR);
			}
			printf ("Port offline: reason %s\n", msg);
		}
			
		if (dbuf.ECRB$W_BNA_ADAPERR) {
	    		char *msg,errbuf[40];
	    		switch (dbuf.ECRB$W_BNA_ADAPERRCODE[0]) {
	    		case	ECRB$C_BNA_ADAPERR_INTR: 
	    			msg= "Error during interrupt service"; break;
	    		case	ECRB$C_BNA_ADAPERR_ETYPE:
	    			msg= "fatal error type (PS)"; break;
	    		case	ECRB$C_BNA_ADAPERR_PCMD:
	    			msg= "Port command failed"; break;
	    		case	ECRB$C_BNA_ADAPERR_RETRY:
	    			msg= "initialization retries exhausted"; break;
	    		case	ECRB$C_BNA_ADAPERR_SLFTST:
	    			msg= "self-test timeout"; break;
	    		case	ECRB$C_BNA_ADAPERR_INIT:
	    			msg= "init timeout"; break;
	    		case	ECRB$C_BNA_ADAPERR_INITFAIL:
	    			msg= "init command failed"; break;
	    		case	ECRB$C_BNA_ADAPERR_SHUT:
	    			msg= "shutdown timeout"; break;
	    		case	ECRB$C_BNA_ADAPERR_CMDQLOCK:
	    			msg= "command queue locked"; break;
	    		case	ECRB$C_BNA_ADAPERR_RSPQLOCK:
	    			msg= "response queue locked"; break;
	    		case	ECRB$C_BNA_ADAPERR_FREEQLOCK:
	    			msg= "free queue locked"; break;
	    		default:
	    			sprintf (errbuf, "unknown adapter error 0x%X\n",
	    				 dbuf.ECRB$W_BNA_ADAPERRCODE[0]);
	    			msg= errbuf;
	    		}
	    		printf ("Adapter error count: %d; (last error: %s (%d))\n",
	    			dbuf.ECRB$W_BNA_ADAPERR, 
	    			msg, dbuf.ECRB$W_BNA_ADAPERRCODE[1]);
	    	}
	    	else	printf ("No adapter errors\n");
	printf ("nb of forced port disable: %d\n", dbuf.ECRB$B_BNA_DSABL);
	printf ("nb of FFI auto restarts: %d\n", dbuf.ECRB$W_BNA_FFIRST);
	printf ("nb of illegal auto restarts: %d\n",dbuf.ECRB$B_BNA_ILLRESTART);
	printf ("nb of illegal start protocols: %d\n",dbuf.ECRB$B_BNA_ILLSTRTPRO);
	printf ("nb of Asynchronous shutdown: %d\n",dbuf.ECRB$W_BNA_ASHUTDOWN);
	printf ("nb of bad state encountered: %d\n",dbuf.ECRB$W_BNA_BADSTATE);
	printf ("nb of unexpected UNDEF state: %d\n",dbuf.ECRB$W_BNA_UNEXPUNDEF);
	printf ("nb of powerfail recovery: %d\n",dbuf.ECRB$W_BNA_POWERFAIL);
	printf ("nb of INIT state in fork: %d\n",dbuf.ECRB$W_BNA_FORKINIT);
	printf ("lockup: queues: command: %d; response: %d; free: %d\n",
		dbuf.ECRB$W_BNA_CMDQLOCK,
		dbuf.ECRB$W_BNA_RSPQLOCK,
		dbuf.ECRB$W_BNA_FREEQLOCK);
	printf ("nb of too long to transmit: %d\n",dbuf.ECRB$B_BNA_XMTIME_TOOLONG);
	printf ("nb of UCB timeouts: %d\n",dbuf.ECRB$B_BNA_UCBTMO);
	printf ("nb of INSQTI retries: %d\n",dbuf.ECRB$W_BNA_IQRETRY);
	printf ("nb of REMQHI retries: %d\n",dbuf.ECRB$W_BNA_RQRETRY);
	printf ("life indicators: receiver: %d; saved receiver: %d; transmit: %d\n",
		dbuf.ECRB$W_BNA_RLIFE,
		dbuf.ECRB$W_BNA_RLIFESAV,
		dbuf.ECRB$W_BNA_XLIFE);
		break;
	default:	printf ("Hardware inconnu: #0d%D\n", ni_type);
	}
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
		printf ("Device: "); fflush (stdin);
		gets (port);
		assign_port(port);
	}
	read_ctr();
/*	start_port();
	get_circuit_counters();	*/
	deassign_port();
}
