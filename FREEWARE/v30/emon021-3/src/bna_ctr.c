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

#define ECRB$C_BNA_XMTDEFER_NODG 1	/*     no datagram available	*/
#define ECRB$C_BNA_XMTDEFER_MAPFAIL 2	/*     failed to map buffer	*/
#define ECRB$C_BNA_XMTDEFER_SNDFAIL 3	/*     send datagram failed	*/

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
	ECRB$W_BNA_NOBDT,		/* no. of BDT unavailable	*/
	unsigned long
	ECRB$L_BNA_MAX_PCSPINTIM;	/* (reserved)	*/

	unsigned short
	ECRB$AW_BNA_FQECNT[bna$c_nofreeq];/* no. of free queue empty interrupts*/

	} debna;
