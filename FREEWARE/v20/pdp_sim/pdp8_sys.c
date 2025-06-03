/* pdp8_sys.c: PDP-8 simulator interface

   Copyright (c) 1993, 1994, 1995, Robert M Supnik,
   Digital Equipment Corporation
   Commercial use prohibited
*/

#include "pdp8_defs.h"

extern DEVICE cpu_dev;
extern UNIT cpu_unit;
extern DEVICE ptr_dev, ptp_dev;
extern DEVICE tti_dev, tto_dev;
extern DEVICE clk_dev, lpt_dev;
extern DEVICE rk_dev, rx_dev;
extern DEVICE rf_dev;
extern REG cpu_reg[];
extern unsigned short M[];
extern char *get_glyph (char *cptr, char *gbuf, char term);
extern int get_unum (char *cptr, int radix, unsigned int max,
	unsigned int *val);

/* SCP data structures and interface routines

   sim_name		simulator name string
   sim_PC		pointer to saved PC register descriptor
   sim_emax		maximum number of words for examine/deposit
   sim_devices		array of pointers to simulated devices
   sim_stop_messages	array of pointers to stop messages
   sim_load		binary loader
*/

char sim_name[] = "PDP-8";

REG *sim_PC = &cpu_reg[0];

int sim_emax = 4;

DEVICE *sim_devices[] = { &cpu_dev,
	&ptr_dev, &ptp_dev, &tti_dev, &tto_dev,
	&clk_dev, &lpt_dev,
	&rk_dev, &rx_dev, &rf_dev, NULL };

char *sim_stop_messages[] = {
	"Unknown error",
	"Unimplemented instruction",
	"HALT instruction",
	"Breakpoint"  };

/* Binary loader

   Loader format consists of a string of 12-bit words (made up from 7 bit
   characters) between leader and trailer (200).  The last word on tape
   is the checksum.  A word with the "link" bit set is a new origin;
   a character > 0200 indicates a change of field.
*/

int sim_load (FILE *fileref)
{
int rubout, word, low, high, origin, csum, field, state, i;

rubout = state = field = origin = csum = 0;
while ((i = getc (fileref)) != EOF) {
	if (rubout) {
		rubout = 0;
		continue;  }
	if (i == 0377) {
		rubout = 1;
		continue;  }
	if (i > 0200) {
		field = (i & 070) << 9;
		continue;  }
	switch (state) {
	case 0:						/* leader */
		if ((i != 0) && (i != 0200)) state = 1;
		high = i;				/* save as high */
		break;
	case 1:						/* low byte */
		low = i;
		state = 2;
		break;
	case 2:						/* high with test */
		word = (high << 6) | low;
		if (i == 0200) {			/* end of tape? */
			if ((csum - word) & 07777) return SCPE_CSUM;
			return SCPE_OK;  }
		csum = csum + low + high;
		if (word >= 010000) origin = word & 07777;
		else {	if ((field | origin) >= MEMSIZE) return SCPE_NXM;
			M[field | origin] = word & 07777;
			origin = (origin + 1) & 07777;  }
		high = i;
		state = 1;
		break;  }				/* end switch */
	}						/* end while */
return SCPE_FMT;					/* eof? error */
}							/* end sim_loader */

/* Symbol tables */

#define I_V_FL		18				/* flag start */
#define I_M_FL		07				/* flag mask */
#define I_V_NPN		0				/* no operand */
#define I_V_FLD		1				/* field change */
#define I_V_MRF		2				/* mem ref */
#define I_V_IOT		3				/* general IOT */
#define I_V_OP1		4				/* operate 1 */
#define I_V_OP2		5				/* operate 2 */
#define I_V_OP3		6				/* operate 3 */
#define I_NPN		(I_V_NPN << I_V_FL)
#define I_FLD		(I_V_FLD << I_V_FL)
#define I_MRF		(I_V_MRF << I_V_FL)
#define I_IOT		(I_V_IOT << I_V_FL)
#define I_OP1		(I_V_OP1 << I_V_FL)
#define I_OP2		(I_V_OP2 << I_V_FL)
#define I_OP3		(I_V_OP3 << I_V_FL)

static const int masks[] = {
 07777, 07707, 07000, 07000,
 07416, 07571, 017457 };

static const char *opcode[] = {
 "SKON", "ION", "IOF", "SRQ",
 "GTF", "RTF", "SGT", "CAF",
 "RPE", "RSF", "RRB", "RFC", "RFC RRB",
 "PCE", "PSF", "PCF", "PPC", "PLS",
 "KCF", "KSF", "KCC", "KRS", "KIE", "KRB",
 "TLF", "TSF", "TCF", "TPC", "SPI", "TLS",
 "SBE", "SPL", "CAL",
 "CLEI", "CLDI", "CLSC", "CLLE", "CLCL", "CLSK",
 "CINT", "RDF", "RIF", "RIB",
 "RMF", "SINT", "CUF", "SUF",
 "ADCL", "ADLM", "ADST", "ADRB",
 "ADSK", "ADSE", "ADLE", "ADRS",
 "DCMA", "DMAR", "DMAW",
 "DCIM", "DSAC", "DIML", "DIMA",
 "DFSE", "DFSC", "DISK", "DMAC",
 "DCXA", "DXAL", "DXAC",
 "PSKF", "PCLF", "PSKE",
 "PSTB", "PSIE", "PCLF PSTB", "PCIE",
 "LWCR", "CWCR", "LCAR",
 "CCAR", "LCMR", "LFGR", "LDBR",
 "RWCR", "CLT", "RCAR",
 "RMSR", "RCMR", "RFSR", "RDBR",
 "SKEF", "SKCB", "SKJK", "SKTR", "CLF",
 "DSKP", "DCLR", "DLAG",
 "DLCA", "DRST", "DLDC", "DMAN",
 "LCD", "XDR", "STR",
 "SER", "SDN", "INTR", "INIT",
 "DTRA", "DTCA", "DTXA", "DTLA",
 "DTSF", "DTRB", "DTLB",
 "CIF", "CDF", "CIF CDF",
 "AND", "TAD", "DCA", "ISZ", "JMS", "JMP", "IOT",
 "NOP", "NOP2", "NOP3", "SWAB", "SWBA",
 "STL", "GLK", "STA", "LAS", "CIA",
 "BSW", "RAL", "RTL", "RAR", "RTR", "RAL RAR", "RTL RTR",
 "SKP", "SNL", "SZL",
 "SZA", "SNA", "SZA SNL", "SNA SZL",
 "SMA", "SPA", "SMA SNL", "SPA SZL",
 "SMA SZA", "SPA SNA", "SMA SZA SNL", "SPA SNA SZL",
 "SCL", "MUY", "DVI", "NMI", "SHL", "ASR", "LSR",
 "SCA", "SCA SCL", "SCA MUY", "SCA DVI",
 "SCA NMI", "SCA SHL", "SCA ASR", "SCA LSR",
 "ACS", "MUY", "DVI", "NMI", "SHL", "ASR", "LSR",
 "SCA", "DAD", "DST", "SWBA",
 "DPSZ", "DPIC", "DCIM", "SAM",
 "CLA", "CLL", "CMA", "CML", "IAC",			/* encode only */
 "CLA", "OAS", "HLT",
 "CLA", "MQA", "MQL",
 NULL, NULL, NULL, NULL,				/* decode only */
 NULL  };
 
static int opc_val[] = {
 06000+I_NPN, 06001+I_NPN, 06002+I_NPN, 06003+I_NPN,
 06004+I_NPN, 06005+I_NPN, 06006+I_NPN, 06007+I_NPN,
 06010+I_NPN, 06011+I_NPN, 06012+I_NPN, 06014+I_NPN, 06016+I_NPN,
 06020+I_NPN, 06021+I_NPN, 06022+I_NPN, 06024+I_NPN, 06026+I_NPN,
 06030+I_NPN, 06031+I_NPN, 06032+I_NPN, 06034+I_NPN, 06035+I_NPN, 06036+I_NPN,
 06040+I_NPN, 06041+I_NPN, 06042+I_NPN, 06044+I_NPN, 06045+I_NPN, 06046+I_NPN,
 06101+I_NPN, 06102+I_NPN, 06103+I_NPN,
 06131+I_NPN, 06132+I_NPN, 06133+I_NPN, 06135+I_NPN, 06136+I_NPN, 06137+I_NPN,
 06204+I_NPN, 06214+I_NPN, 06224+I_NPN, 06234+I_NPN,
 06244+I_NPN, 06254+I_NPN, 06264+I_NPN, 06274+I_NPN,
 06530+I_NPN, 06531+I_NPN, 06532+I_NPN, 06533+I_NPN,
 06534+I_NPN, 06535+I_NPN, 06536+I_NPN, 06537+I_NPN,
 06601+I_NPN, 06603+I_NPN, 06605+I_NPN,
 06611+I_NPN, 06612+I_NPN, 06615+I_NPN, 06616+I_NPN,
 06621+I_NPN, 06622+I_NPN, 06623+I_NPN, 06626+I_NPN,
 06641+I_NPN, 06413+I_NPN, 06415+I_NPN,
 06661+I_NPN, 06662+I_NPN, 06663+I_NPN,
 06664+I_NPN, 06665+I_NPN, 06666+I_NPN, 06667+I_NPN,
 06701+I_NPN, 06702+I_NPN, 06703+I_NPN,
 06704+I_NPN, 06705+I_NPN, 06706+I_NPN, 06707+I_NPN,
 06711+I_NPN, 06712+I_NPN, 06713+I_NPN,
 06714+I_NPN, 06715+I_NPN, 06716+I_NPN, 06717+I_NPN,
 06721+I_NPN, 06722+I_NPN, 06723+I_NPN, 06724+I_NPN, 06725+I_NPN,
 06741+I_NPN, 06742+I_NPN, 06743+I_NPN,
 06744+I_NPN, 06745+I_NPN, 06746+I_NPN, 06747+I_NPN,
 06751+I_NPN, 06752+I_NPN, 06753+I_NPN,
 06754+I_NPN, 06755+I_NPN, 06756+I_NPN, 06757+I_NPN,
 06761+I_NPN, 06762+I_NPN, 06764+I_NPN, 06766+I_NPN,
 06771+I_NPN, 06772+I_NPN, 06774+I_NPN,
 06201+I_FLD, 06202+I_FLD, 06203+I_FLD,
 00000+I_MRF, 01000+I_MRF, 02000+I_MRF, 03000+I_MRF,
 04000+I_MRF, 05000+I_MRF, 06000+I_IOT,
 07000+I_NPN, 07400+I_NPN, 07401+I_NPN, 07431+I_NPN, 07447+I_NPN,
 07120+I_NPN, 07204+I_NPN, 07240+I_NPN, 07604+I_NPN, 07041+I_NPN,
 07002+I_OP1, 07004+I_OP1, 07006+I_OP1,
 07010+I_OP1, 07012+I_OP1, 07014+I_OP1, 07016+I_OP1,
 07410+I_OP2, 07420+I_OP2, 07430+I_OP2,
 07440+I_OP2, 07450+I_OP2, 07460+I_OP2, 07470+I_OP2,
 07500+I_OP2, 07510+I_OP2, 07520+I_OP2, 07530+I_OP2,
 07540+I_OP2, 07550+I_OP2, 07560+I_OP2, 07570+I_OP2,
 07403+I_OP3, 07405+I_OP3, 07407+I_OP3,
 07411+I_OP3, 07413+I_OP3, 07415+I_OP3, 07417+I_OP3,
 07441+I_OP3, 07443+I_OP3, 07445+I_OP3, 07447+I_OP3,
 07451+I_OP3, 07453+I_OP3, 07455+I_OP3, 07457+I_OP3,
 017403+I_OP3, 017405+I_OP3, 0174017+I_OP3,
 017411+I_OP3, 017413+I_OP3, 017415+I_OP3, 017417+I_OP3,
 017441+I_OP3, 017443+I_OP3, 017445+I_OP3, 017447+I_OP3,
 017451+I_OP3, 017453+I_OP3, 017455+I_OP3, 017457+I_OP3,
 07200+I_OP1, 07100+I_OP1, 07040+I_OP1, 07020+I_OP1, 07001+I_OP1,
 07600+I_OP2, 07404+I_OP2, 07402+I_OP2,
 07601+I_OP3, 07501+I_OP3, 07421+I_OP3,
 07000+I_OP1, 07400+I_OP2, 07401+I_OP3, 017401+I_OP3,
 -1 };

/* Operate decode

   Inputs:
	inst	=	mask bits
	class	=	instruction class code
	sp	=	space needed?
   Outputs:
	status	=	space needed
*/

int oprf (int inst, int class, int sp)
{
int i, j;

for (i = 0; opc_val[i] >= 0; i++) {			/* loop thru ops */
	j = (opc_val[i] >> I_V_FL) & I_M_FL;		/* get class */
	if ((j == class) && (opc_val[i] & inst)) {	/* same class? */
		inst = inst & ~opc_val[i];		/* mask bit set? */
		printf ((sp? " %s": "%s"), opcode[i]);
		sp = 1;  }  }
return sp;
}

/* Symbolic decode

   Inputs:
	addr	=	current PC
	*val	=	pointer to data
	cflag	=	true if decoding for CPU
	sw	=	switches
   Outputs:
	return	=	status code
*/

int print_sym (unsigned int addr, unsigned int *val, int cflag, int sw)
{
int i, j, ch, sp, inst, disp, mask;
extern int emode;

ch = val[0];
if (sw & SWMASK ('A')) {				/* ASCII? */
	if (ch > 0377) return SCPE_ARG;
	ch = ch & 0177;
	printf ((ch < 040)? "<%03o>": "%c", ch);
	return SCPE_OK;  }
if (sw & SWMASK ('C')) {				/* characters? */
	putchar (((ch >> 6) & 077) + 040);
	putchar ((ch & 077) + 040);
	return SCPE_OK;  }
if (!(sw & SWMASK ('M'))) return SCPE_ARG;

/* Instruction decode */

inst = val[0] | ((emode & 1) << 12);			/* include EAE mode */
for (i = 0; opc_val[i] >= 0; i++) {			/* loop thru ops */
    j = (opc_val[i] >> I_V_FL) & I_M_FL;		/* get class */
    if ((opc_val[i] & 017777) == (inst & masks[j])) {	/* match? */

	switch (j) {					/* case on class */
	case I_V_NPN:					/* no operands */
		printf ("%s", opcode[i]);		/* opcode */
		break;
	case I_V_FLD:					/* field change */
		printf ("%s %-o", opcode[i], (inst >> 3) & 07);
		break;
	case I_V_MRF:					/* mem ref */
		disp = inst & 0177;			/* displacement */
		printf ("%s%s", opcode[i], ((inst & 00400)? " I ": " "));
		if (inst & 0200) {			/* current page? */
			if (cflag) printf ("%-o", (addr & 07600) | disp);
			else printf ("T %-o", disp);  }
		else printf ("%-o", disp);		/* page zero */
		break;
	case I_V_IOT:					/* IOT */
		printf ("%s %-o", opcode[i], inst & 0777);
		break;
	case I_V_OP1:					/* operate group 1 */
		sp = oprf (inst & 0361, j, 0);		/* mask bits */
		if (opcode[i]) printf ((sp? " %s": "%s"), opcode[i]);
		break;
	case I_V_OP2:					/* operate group 2 */
		if (opcode[i]) printf ("%s", opcode[i]);	/* skips */
		oprf (inst & 0206, j, opcode[i] != NULL);	/* mask bits */
		break;	
	case I_V_OP3:					/* operate group 3 */
		sp = oprf (inst & 0320, j, 0);		/* mask bits */
		if (opcode[i]) printf ((sp? " %s": "%s"), opcode[i]);
		break;  }				/* end case */
	return SCPE_OK;  }				/* end if */
	}						/* end for */
return SCPE_ARG;
}

/* Symbolic input

   Inputs:
	*cptr	=	pointer to input string
	addr	=	current PC
	cflag	=	true if parsing for CPU
	*val	=	pointer to output values
   Outputs:
	status	=	error status
*/

int parse_sym (char *cptr, unsigned int addr, int cflag, unsigned int *val)
{
int i, j, k;
unsigned int r;
char gbuf[CBUFSIZE];

cptr = get_glyph (cptr, gbuf, 0);			/* get opcode */
if (gbuf[0] == '\'') {					/* single char? */
	if (gbuf[1] == 0) return SCPE_ARG;
	val[0] = (unsigned int) gbuf[1] | 0200;
	return SCPE_OK;  }
if (gbuf[0] == '"') {					/* string? */
	if (gbuf[1] == 0) return SCPE_ARG;
	val[0] = (((unsigned int) gbuf[1] - 040) & 077) << 6;
	if (gbuf[2]) val[0] = val[0] + (((unsigned int) gbuf[2] - 040) & 077);
	return SCPE_OK;  }

/* Instruction parse */

for (i = 0; (opcode[i] != NULL) && (strcmp (opcode[i], gbuf) != 0) ; i++) ;
if (opcode[i] == NULL) return SCPE_ARG;
val[0] = opc_val[i] & 07777;				/* get value */
j = (opc_val[i] >> I_V_FL) & I_M_FL;			/* get class */

switch (j) {						/* case on class */
case I_V_IOT:						/* IOT */
	cptr = get_glyph (cptr, gbuf, 0);		/* get dev+pulse */
	if (get_unum (gbuf, 8, 0777, &r) != SCPE_OK) return SCPE_ARG;
	val[0] = val[0] | r;
	break;
case I_V_FLD:						/* field */
	for (cptr = get_glyph (cptr, gbuf, 0); gbuf[0] != 0;
		cptr = get_glyph (cptr, gbuf, 0)) {
		for (i = 0; (opcode[i] != NULL) &&
			(strcmp (opcode[i], gbuf) != 0) ; i++) ;
		if (opcode[i] != NULL) {
			k = (opc_val[i] >> I_V_FL) & I_M_FL;
			if (k != j) return SCPE_ARG;
			val[0] = val[0] | (opc_val[i] & 07777);  }
		else {	if (get_unum (gbuf, 8, 07, &r) != SCPE_OK)
				return SCPE_ARG;
			val[0] = val[0] | (r << 3);
			break;  }  }
		break;
case I_V_MRF:						/* mem ref */
	cptr = get_glyph (cptr, gbuf, 0);		/* get next field */
	if (strcmp (gbuf, "I") == 0) {			/* indirect? */
		val[0] = val[0] | 0400;
		cptr = get_glyph (cptr, gbuf, 0);  }
	if ((k = (strcmp (gbuf, "T") == 0)) || (strcmp (gbuf, "Z") == 0)) {
		cptr = get_glyph (cptr, gbuf, 0);
		if (get_unum (gbuf, 8, 0177, &r) != SCPE_OK) return SCPE_ARG;
		val[0] = val[0] | r | (k? 0200: 0);  }
	else {	if (get_unum (gbuf, 8, 07777, &r) != SCPE_OK) return SCPE_ARG;
		if (r <= 0177) val[0] = val[0] | r;
		else if (cflag && (((addr ^ r) & 07600) == 0))
			val[0] = val[0] | (r & 0177) | 0200;
		else return SCPE_ARG;  }
	break;
case I_V_NPN: case I_V_OP1: case I_V_OP2: case I_V_OP3:	/* operates */
	for (cptr = get_glyph (cptr, gbuf, 0); gbuf[0] != 0;
		cptr = get_glyph (cptr, gbuf, 0)) {
		for (i = 0; (opcode[i] != NULL) &&
			(strcmp (opcode[i], gbuf) != 0) ; i++) ;
		k = opc_val[i] & 07777;
		if ((opcode[i] == NULL) || (((k ^ val[0]) & 07000) != 0))
			return SCPE_ARG;
		val[0] = val[0] | k;  }
	break;  }					/* end case */
if (*cptr != 0) return SCPE_ARG;			/* junk at end? */
return SCPE_OK;
}
