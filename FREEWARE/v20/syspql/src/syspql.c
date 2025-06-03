#define SYSINIT	/* defined if used as a loadable image */
/**/

#ifdef __alpha
#ifdef SYSINIT
/*	fixed with axpsys06_061	*/
/*#error "Does not work in SYSINIT on alpha/VMS: hang in boot, ast not delivered"
/**/
#else
#error	"Does not work on an alpha/VMS running system (crash in FIL$OPENFILE)."
#endif
#endif

#include <iodef.h>
#include <pqldef.h>
#include <ssdef.h>
#ifndef SYSINIT
#include <stdio.h>
#endif

#include "syspql.h"

#ifdef SYSINIT
#define SYS$CMEXEC(a,b) a()
void consprintf (char *);
#else
int SYS$CMEXEC();
#endif

static char outbuf[132];

static void
doprint()
{
#ifdef SYSINIT
	consprintf (outbuf);
#else
	puts(outbuf);
#endif
	outbuf[0]= 0;
}
static	void
printstr(char * str)
{
	char * cp= outbuf, *cps= str;
	while (*cp) cp++;
	while (*cp++ = *cps++);	
}
static	void
printdec (unsigned long val)
{
	char resbuf[20];
	unsigned long resval= val;
	int	i= sizeof resbuf -1;

	resbuf[i--]= 0;
	do {	resbuf[i--] = '0'+ resval % 10;
		resval /= 10;
	} while (resval != 0);
	printstr (resbuf+i+1);
}

static	void
printhex(unsigned long val)
{
static const char hexstr[]= "0123456789ABCDEF";
	char bufres[9];
	int resval= val;
	int i;

	bufres[sizeof bufres -1] = 0;
	for (i=0; i<=7; i++) {
		bufres[7-i]= hexstr[resval & 0xF];
		resval >>= 4;
	}
	printstr(bufres);
}

static void
printini(char * str)
{
	outbuf[0]= 0;
	printstr ("%SYSPQL-");
	printstr (str);
	printstr (", ");
}

static void
my_strcpy (char *dest, const char * src)
{
	char *cpd=dest;
	const char *cps=src;
	do *cpd++= *cps++;
	while (*cps);
}

extern pqlent * PQL$AR_SYSPQL;
extern unsigned long PQL$GL_SYSPQLLEN;
extern vmsvers SYS$GQ_VERSION;
static parambuf modbuf;
static int openst,readst, accessst;
static u32 channel;
static u16 iosb[4];
static u32 filelbn;	/* = 0 => no update is possible */

static read_file()
{
static	char	pqlfile[]="[SYS$LDR]SYSPQL.DAT";
	struct	{unsigned long len; void * addr;} filedesc,rtrvdesc;
	int	status, FIL$OPENFILE(), FIL$RDWRTLBN(), check_nomove();
	unsigned char filhdr[512], ixfhdr[512];
	unsigned long statblk[2];
	unsigned long rtrvlen;
	struct {unsigned long count, lbn;} rtrvarray[10];

	filedesc.addr = pqlfile;
	filedesc.len  = sizeof pqlfile -1;
	rtrvdesc.addr = &rtrvarray;
	rtrvdesc.len  = sizeof rtrvarray;
	status= FIL$OPENFILE (&channel, &filedesc, ixfhdr, filhdr, statblk,
			     &rtrvlen, &rtrvdesc, 1 /*nocache*/);
	openst= status;

/*	if (status&1) {	/* se proteger contre un MOVEFILE catastrophique */
/*	status= access_file(channel, filhdr); 		/* si on doit ecrire */
/*		accessst= status;			/* => use XQP open */
/*	}	/* pas possible dans SYSINIT */
/**/
	if (status&1) {
		if (rtrvlen==0) status= SS$_ENDOFFILE;
		else {
/*			status= SYS$QIOW(0, channel, IO$_READVBLK, iosb, 0, 0,
/*					 &modbuf, sizeof modbuf, 1, 0, 0, 0);
/*			if (status&1) status= iosb[0];
/**/
			if (check_nomove(filhdr)) filelbn= rtrvarray[0].lbn;
			status= FIL$RDWRTLBN (channel, rtrvarray[0].lbn,
				              &modbuf, IO$_READLBLK, sizeof modbuf);
/**/
		}
	}
	readst= status;
	return status;
	
}

static int
update_file()
{
	int status,FIL$RDWRTLBN();

	status= FIL$RDWRTLBN (channel, filelbn,
		              &modbuf, IO$_WRITELBLK, sizeof modbuf);
	return status;
}

static int
close_file()
{
	int status, SYS$DASSGN();
	status= SYS$DASSGN (channel);
	return status;
}

int
syspql(int debug)
{
	pqlent * pe;
	int sz;
	int status;
	int rewrite= 0;

	if (okassumes()) {
		printini ("F-NOASSUMES");
		printstr ("Assumes don't hold");
		doprint();
		return SS$_BUGCHECK;
	}

	status= SYS$CMEXEC(read_file,0);

	if (debug) {
		printini ("I-STATUS");
		printstr ("open :");
		printhex (openst);
		printstr ("; read: ");
		printhex (readst);
		doprint();
	}

	if ((status&1) == 0) {
		printini ("E-NOFILE");
		printstr ("Error opening SYSPQL.DAT, status=");
		printhex (status);
		doprint();
		return status;
	}

	if (debug) {
		printini ("I-DEBUG");
		printstr ("System PQL length is ");
		printdec (PQL$GL_SYSPQLLEN);
		doprint();
	}

	if ( (modbuf.versmaj != VERS_MAJ) || (modbuf.versmin > VERS_MIN) )
	{
		printini ("F-BADVERS");
		printstr ("SYSPQL.DAT version mismatch");
		doprint();
		return SS$_IDMISMATCH;
	}

	modbuf.map0inf |= 1<<PQL$_CPULM;	/* security */

	if ( (modbuf.sysver.low != SYS$GQ_VERSION.low)
	   ||(modbuf.sysver.high != SYS$GQ_VERSION.high) ) {
		rewrite= 1;
		modbuf.sysver= SYS$GQ_VERSION;
		modbuf.mapdef= 0;
	}

	for (pe= PQL$AR_SYSPQL, sz=PQL$GL_SYSPQLLEN; sz>0; 
			pe++, sz-= sizeof (pqlent)) {
		char unkcode[20], *cp;

		if ((pe->typ > modbuf.nb_param) ||
		    (pe->typ >= 8*sizeof modbuf.map)) {
			printini ("W-INVPQL");
			printstr ("Invalid system PQL parameter number ");
			printdec (pe->typ);
			printstr ("Skipped");
			doprint();
			continue;
		}

		switch (pe->typ) {
		case PQL$_ASTLM:	cp= "ASTLM"; break;
		case PQL$_BIOLM:	cp= "BIOLM"; break;
		case PQL$_BYTLM:	cp= "BYTLM"; break;
		case PQL$_CPULM:	cp= "CPULIM"; break;
		case PQL$_DIOLM:	cp= "DIOLM"; break;
		case PQL$_FILLM:	cp= "FILLM"; break;
		case PQL$_PGFLQUOTA:	cp= "PGFLQUOTA"; break;
		case PQL$_PRCLM:	cp= "PRCLM"; break;
		case PQL$_TQELM:	cp= "TQELM"; break;
		case PQL$_WSQUOTA:	cp= "WSQUOTA"; break;
		case PQL$_WSDEFAULT:	cp= "WSDEFAULT"; break;
		case PQL$_ENQLM:	cp= "ENQLM"; break;
		case PQL$_WSEXTENT:	cp= "WSEXTENT"; break;
		case PQL$_JTQUOTA:	cp= "JTQUOTA"; break;
		default:	outbuf[0]= 0;
				printstr ("Code: ");
				printdec (pe->typ);
				my_strcpy (unkcode, outbuf);
		}

		if (debug) {
			printini ("I-DEBUG");
			printstr ("Code: ");
			printstr (cp);
			printstr ("; Value: ");
			printdec (pe->val);
			doprint();
		}

		if ( ((1<<pe->typ) & modbuf.mapdef) == 0) {
			rewrite= 1;
			modbuf.mapdef |= 1<<pe->typ;
			modbuf.values[modbuf.nb_param+pe->typ]= pe->val;
			printini ("I-ADDEFPAR");
			printstr ("Added ");
			printstr (cp);
			printstr (" to default parameter list");
			doprint();
		}

		if ( pe->val != modbuf.values[modbuf.nb_param+pe->typ]) {
			rewrite= 1;
			modbuf.values[modbuf.nb_param+pe->typ]= pe->val;
			printini ("I-UPDDEFPAR");
			printstr ("Updated default parameter ");
			printstr (cp);
			doprint();
		}

		if ( ((1<<pe->typ) & modbuf.map) 		/* present */
		  && (modbuf.values[pe->typ] != pe->val)){ /* and different*/
			int underflow;

			if ( (1<<pe->typ)&modbuf.map0inf )
				if (pe->val==0) underflow= 1;
				else	if (modbuf.values[pe->typ]!=0)
					      underflow= modbuf.values[pe->typ]
							   < pe->val;
					else  underflow= 0;
			else underflow= modbuf.values[pe->typ] < pe->val;

			if (underflow) {
				printini ("W-SETMIN");
				printstr ("Parameter ");
				printstr (cp);
				printstr (" set to minimum");
			}
			else {
				pe->val= modbuf.values[pe->typ];
				printini ("I-RAISED");
				printstr ("Parameter ");
				printstr (cp);
				printstr (" value raised to ");
				printdec (modbuf.values[pe->typ]);
			}
			doprint();
		}
	}

	if (rewrite && (status&1)){
		if (filelbn!=0) {
			status= SYS$CMEXEC(update_file, 0);
			if (status&1) {
				printini ("I-UPDATED");
				printstr ("Updated SYSPQL.DAT");
			}
			else	{
				printini ("E-ERRUPD");
				printstr ("Error updating SYSPQL.DAT, status=");
				printhex (status);
			}
		}
		else {
			printini ("W-NOTUPD");
			printstr ("SYSPQL.DAT not updated, file is not set /NOMOVE");
		}
		doprint();
	}
	if (status&1) status= SYS$CMEXEC(close_file, 0);
	return status;
}
