#include <atrdef.h>
/*#include <fatdef.h>	/* 	sys$lib_c, sigh! */
/*#include <fchdef.h>	/* 	sys$lib_c, sigh! */
#define FCH$M_NOMOVE 0x200000
#include <iodef.h>
#include <pqldef.h>
#include <rms.h>
#include <ssdef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syspql.h"

extern pqlent * PQL$AR_SYSPQL;
extern unsigned long PQL$GL_SYSPQLLEN;
extern vmsvers SYS$GQ_VERSION;
static parambuf modbuf;
static u16 channel;
static int newfile;
static int sysverdif;

static int
open_file()
{
	struct FAB syspql_fab;
static	const char file_name[]= "sys$loadable_images:syspql.dat";
	int status, SYS$CREATE(), SYS$QIOW();

	syspql_fab=cc$rms_fab;
	syspql_fab.fab$l_fna= (char *) file_name;
	syspql_fab.fab$b_fns= sizeof file_name - 1;
	syspql_fab.fab$l_fop= FAB$M_UFO | FAB$M_CIF;
	syspql_fab.fab$b_fac= FAB$M_PUT;
	syspql_fab.fab$b_rfm= FAB$C_UDF;
	syspql_fab.fab$l_alq= 1;
	status= SYS$CREATE (&syspql_fab);
	channel= syspql_fab.fab$l_stv;/* happiness is a warm user mode channel*/
	if (status&1) {
		if (status==RMS$_CREATED) {
			puts ("Created SYS$LOADABLE_IMAGES:SYSPQL.DAT");
			newfile= 1;
			modbuf.nb_param= PARAM_SIZE;
		}
		else {
			u16 iosb[4];
			status= SYS$QIOW (0, channel, IO$_READVBLK, iosb, 0, 0,
					  &modbuf, sizeof modbuf, 1, 0, 0, 0);
			if (status&1) status= iosb[0];
			if (status&1)
			   if ( (modbuf.versmin!=VERS_MIN)
			      ||(modbuf.versmaj!=VERS_MAJ)) {
				puts (
	       "saved SYSPQL.DAT has a bad version number; delete it and retry;"
				     );
				status= SS$_IDMISMATCH;
			   }
			   else if ( (SYS$GQ_VERSION.low != modbuf.sysver.low)
			           ||(SYS$GQ_VERSION.high != modbuf.sysver.high)) {
				puts (
    "SYSPQL.DAT saved VMS version mismatch: minimums loaded from running system"
				     );
				sysverdif=1;
			        }
		}
	}	/* $create ok */
	return status;
}

static int
write_file()
{
	u16 iosb[4];
	int status, SYS$QIOW();
	int canupd;
static	u32	uchar;
#pragma member_alignment save
#pragma nomember_alignment
static	union {
		char bytes[ATR$S_RECATTR];
		struct {
			char rien[8];
			u16  FAT$W_EFBLKH;
			u16  FAT$W_EFBLKL;
			u16  FAT$W_FFBYTE;
			} FAT;
	} fatblk;
static	struct {u16 size, type; p32 addr;} atrlst[3]= 
		{ {ATR$S_UCHAR, ATR$C_UCHAR, &uchar},
		  {ATR$S_RECATTR, ATR$C_RECATTR, &fatblk},
		  {0, 0, NULL}};
#pragma member_alignment restore

	status= SYS$QIOW (0, channel, IO$_ACCESS, iosb, 0, 0, 
			  0, 0, 0, 0, atrlst, 0);
	if (status&1) status= iosb[0];
	canupd= status&1;
	if (canupd==0) {
		printf ("Error reading SYSPQL.DAT attributes, status=%08X\n",
			status);
		}
	status= SYS$QIOW (0, channel, IO$_WRITEVBLK, iosb, 0, 0, 
			  &modbuf, sizeof modbuf, 1, 0, 0, 0);
	if (status&1) status= iosb[0];
	if (status&1&canupd) {
		fatblk.FAT.FAT$W_FFBYTE= sizeof modbuf; /* pretty looking */
		fatblk.FAT.FAT$W_EFBLKL= 1;		/* pretty looking */
		uchar |= FCH$M_NOMOVE;
		status= SYS$QIOW (0, channel, IO$_DEACCESS, iosb, 0, 0, 
				  0, 0, 0, 0, atrlst, 0);
		if (status&1) status= iosb[0];
		canupd= status&1;
		if (canupd==0)
		   printf ("Error writing SYSPQL.DAT attributes, status=%08X\n",
			status);
	}
	if (canupd==0) puts (
		"Please set file SYS$LOADABLE_IMAGES:SYSPQL.DAT /NOMOVE"
			    );
	return status;
}

main()
{
	pqlent * pe;
	int sz;
	int status;
	int debug= 0;

	status= open_file();
	if ((status&1)==0) {
		puts ("Error opening SYS$LOADABLE_IMAGES:SYSPQL.DAT");
		exit (status);
	}

	if (debug) printf ("System PQL length is %d\n", PQL$GL_SYSPQLLEN);

	for (pe= PQL$AR_SYSPQL, sz=PQL$GL_SYSPQLLEN; sz>0; 
			pe++, sz-= sizeof (pqlent)) {
		char	unkcode[20], *cp;
		u32	newval;
		int	badrep, change;
		u32	min_value, stored_value;

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
		default:	sprintf (cp=unkcode, "Code: %d", pe->typ);
		}

		if (pe->typ >= modbuf.nb_param){
			printf ("Illegal PQL parameter %s ignored\n", cp);
			continue;
		}

		stored_value= modbuf.values[pe->typ];
		if (sysverdif||newfile) {
			min_value= pe->val;
			if (newfile) stored_value= min_value;
		}
		else	min_value= modbuf.defvalues[pe->typ];

		do {
		   do {
			int lrep;
			char rep[80];

			printf ("Code:%10s: minimum:%8u; active:%8u; default:%8u :", 
				cp, min_value, pe->val, stored_value);
			gets(rep);
			lrep= strlen(rep);
			badrep=0;	/* assume good rep */
			change= lrep != 0;
			if (change==0) break;
			strcat (rep,"1");
			if (atol(rep)==0) {
				rep[lrep]= 0;
				printf ("Invalid answer\n");
				badrep= 1;
			}
			else{
				rep[lrep]= 0;
				newval= atol(rep);
			}
		   }	while (badrep);

		   if (change) {
			if (pe->val==0) {
				if (pe->typ == PQL$_CPULM){
					   if (newval!=0)
					printf (
		  "SYSPQL quota %s not changed: new value is lower\n", cp);
					continue;
				}
			}
			if (newval >= pe->val) {
				modbuf.values[pe->typ]= (unsigned long) newval;
				modbuf.map |= 1<<pe->typ;
			}
			else{
				printf (
		  "SYSPQL quota %s not changed: new value is lower\n", cp);
				badrep=1;
			}
		   }
		} while (badrep);
	}

	modbuf.versmaj= VERS_MAJ;
	modbuf.versmin= VERS_MIN;
	modbuf.map0inf= 1<<PQL$_CPULM;
	modbuf.nb_param= PARAM_SIZE;

	status= write_file();

	return status;
}
