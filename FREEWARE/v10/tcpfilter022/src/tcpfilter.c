/*	001	19-jul-1993 gg:	add msg if table has been replaced */
/*	002	08-jun-1994 gg: add continued lines */
#include <assert.h>
#include <ctype.h>
#include <file.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include armdef
#include rms
#include descrip

#pragma nostandard
#pragma nomember_alignment

typedef struct	{unsigned long rq;
		 unsigned long owner;
		 unsigned long reserved;
		 unsigned short prot;
		 unsigned short loadopt;
	} header_t;
#define LOADOPT$M_OWNER	1
#define LOADOPT$M_PROT	2

static header_t header;


typedef struct {unsigned long addr, mask;
		unsigned short port;
		unsigned short index;
		variant_union {
			variant_struct{
				unsigned short size;
				unsigned short type;
				} struct$$1;
			unsigned long status;
			
		} union$$1;
	}	acte_t;


typedef struct data_st {
		struct data_st * flink, *blink;
		unsigned short type;
		unsigned char size;
		variant_union {
			unsigned char buf[4096];
			struct {
				unsigned short fid[3];
				unsigned long access;
				char devnam[NAM$C_DVI];
			} struct_image;
		} union$$1;
	}	data_t;


#pragma member_alignment
#pragma standard

static struct list_st {struct list_st * next;
		      data_t	* flink, *blink;
		      acte_t	acte;
		     } * act_list= NULL;

static int	loadtype;
static char * errmsgptr ;

int	LOADCODE (int size, void * addr, ...);

static int listsz;	/* size of list */
static int datasz;	/* size of headers */


#define MAXWORD 18

typedef char word_t[MAXWORD+1];

static char line[256];
static int  linsiz;
static int  linidx;
static char curchar;
static FILE * cfg;
static int  init_ok= 1;


static jmp_buf error_buf;

static	void
longjmp_mess (jmp_buf buf, char * errmsg)
{
	errmsgptr= errmsg;
	longjmp (buf, 1);
}

static open_cfg()
{
static const $DESCRIPTOR (tab_d,"LNM$FILE_DEV");
static const $DESCRIPTOR (log_d,"TCP_SECURITY$CFG");
	int status, SYS$TRNLNM();
	char * dna;

	status= SYS$TRNLNM (0, &tab_d, &log_d, 0, 0);
	if (status&1) dna= "dna=.dat";
	else	      dna= "dna=SYS$INPUT:.dat";
	cfg=fopen("tcp_security$cfg", "r", dna, "ctx=rec");
}

static cfg_error(message)
char * message;
{
	printf ("%s\n%*s%s\n",line,linidx+2,"! ",message);
	init_ok=0;
}

static int skipspace();

static 
char *
read_cfg()
{
	char * cp, *cpcom;

	linidx=0;

	if (fgets (line, sizeof line, cfg) == NULL) return NULL;

	cp= strchr (line, '\n');
	if (cp == NULL) {
		cfg_error ("Line too long");
		cp= line;
	}
	else	if ((cpcom= strchr (line, '!'))!=NULL) cp= cpcom;
	*cp= '\0';
	linsiz= cp - line;
	curchar= line[0];
	if (linsiz>0) skipspace();
	return line;
}

static char 
get_char()
{
	char c;

	if (linidx == linsiz) return '\0';
	c= line[++linidx];
	while ( (c=='-') && (linidx == linsiz) ) {
		if (read_cfg() == NULL) c= '\0';
		else c= line[linidx++];
	}
	curchar= c;
	return c;
}

static
skipspace()
{
	while (isspace(curchar)) (void)get_char();
}


static getword(word)
word_t word;
{
	register char * cp= word;
	register int i;
static const char valid_chr []= 
"0123456789ABCDEFGHIJKILMNOPQRSTUVWXYZabcdefghijkilmnopqrstuvwxyz.-_$";
	skipspace();

	if (curchar == '"')
		for (i= MAXWORD; (i--);){
			char c = get_char();
			if (c=='\0') {
				linidx+= MAXWORD-i;
				longjmp_mess(error_buf,"Unterminated string");
			}
			if (c!='"') *cp++= c;
			else {
				c= get_char();
				if (c!='"') break;
				else *cp++= c;
			}
		}
	else		
		for (i=MAXWORD; i-- && (strchr(valid_chr, curchar)!=NULL)
				    && (curchar !=0);  (void) get_char())
			*cp++= (isupper (curchar))
					? _tolower(curchar)
					: curchar;

	if (i==MAXWORD-1) longjmp_mess (error_buf,"Word expected");
	if (isalnum(curchar)){
			linidx-= MAXWORD;
			longjmp_mess (error_buf,"Word too long");
	}
	*cp=0;
}

static getoct()
{
	register int i=0;

	skipspace();
	if (isdigit(curchar))
		do	{ int n= curchar-'0';
			if (n>8) longjmp_mess (error_buf, "Invalid octal digit"); 
			i=8*i+n;
			(void) get_char();
		} while (isdigit(curchar));
	else longjmp_mess (error_buf,"Number expected");
	return i;
}

static getnumber()
{
	register int i=0;

	skipspace();
	if (isdigit(curchar))
		do
			i=10*i+(curchar-'0');
		while (isdigit(get_char()));
	else longjmp_mess (error_buf,"Number expected");
	return i;
}

static skipchar (c)
char c;
{
	if (curchar==c) (void) get_char();
	else{
		static char error_txt[20];
		sprintf (error_txt,"'%c' expected",c);
		longjmp_mess (error_buf,error_txt);
	}
}

static getkeyword(list)
char ** list;
{
	word_t word;
	register int i=0;

	getword(word);
	while ( (list[i]!=NULL) && (strcmp(list[i],word)) ) i++;
	if (list[i]==NULL){
		static char error_txt[256];

		linidx-= strlen(word);
		sprintf (error_txt,"Expecting one of keywords %s",list[0]);
		i= 1;
		while (list[i]!=0){
			strcat (error_txt,",");
			strcat (error_txt,list[i++]);
		}
		longjmp_mess (error_buf,error_txt);
	}
	return i;
}

static void
check_status(int status, char * mess)
{
static	int vector[]= {2, 0, 0};
	int SYS$PUTMSG();

	if (status&1) return;
	vector[1]= status;
	SYS$PUTMSG (vector, 0, 0, 0);
	longjmp_mess (error_buf, mess);
}

static unsigned long
get_inaddr()
{
	unsigned char internet [4];
	int i;
	int n;
	for (i=0; i<4; i++) {
		n= getnumber();
		if ( (n<0) || (n>255) )
			longjmp_mess (error_buf, "invalid internet address");
		internet [i]= n;
		if (i!=3) skipchar ('.');
	}
	return * (long *) internet;
}

static unsigned short
get_inport()
{
	unsigned char port[2];
	int i;
	unsigned long n;

	n= getnumber();
	if ( (n<=0) || (n>65535) )
		longjmp_mess (error_buf, "invalid internet port number");

	port[0]= n>>8;
	port[1]= n & 255;
	return * (unsigned short *) port;
}

static data_t *
getacl()
{
	int parcnt= 1;
	char aclbuf[10000];
	char *cp;
	data_t data, *entp;
	int entsz;
	struct dsc$descriptor acl_dsc = {0, 0, 0, NULL};
	struct dsc$descriptor ace_dsc = {sizeof data.buf, 0, 0, NULL};
static	const	char accessread[]= "INCOMING";
static	const	char accesswrite[]= "OUTGOING";
static	const	struct dsc$descriptor accessnames[32] = 
		{{sizeof accessread-1, 0, 0, (char *) accessread},
		 {sizeof accesswrite-1, 0, 0, (char *) accesswrite}};
	int	SYS$PARSE_ACL();

	int status;

	ace_dsc.dsc$a_pointer= (char *) data.buf;
	skipspace();
	skipchar ('(');
	aclbuf[0]= '(';
	for (cp=aclbuf+1; (parcnt>0) && (curchar) ; get_char()) {
		switch (curchar) {
		case '(': parcnt++; break;
		case ')': parcnt--; break;
		}
		*cp++ = curchar;
	}
	if (parcnt!=0) longjmp_mess (error_buf, "Invalid ACL syntax");	
	data.type= 0;	/* ACL */
	data.flink= NULL;
	data.blink= NULL;
	acl_dsc.dsc$w_length= cp-aclbuf;
	acl_dsc.dsc$a_pointer= aclbuf;
	status= SYS$PARSE_ACL (&acl_dsc, &ace_dsc, 0, &accessnames, 0, 0);
	if ((status&1)==0) {
		static char errbuf[80];
		sprintf (errbuf, "Error %08X returned from $PARSE_ACL", status);
		longjmp_mess (error_buf, errbuf);
	}
	entsz= sizeof data - sizeof data.buf + data.buf[0];
	entp = malloc (entsz);
	memcpy (entp, &data, entsz);
	entp->size= data.buf[0];
	return entp;
}

static data_t *
getimage()
{
	struct FAB fab;
	struct NAM nam;
        char image[NAM$C_MAXRSS+1];
	char esa[NAM$C_MAXRSS];
	char * cpi= image;
	data_t	* entp, *oldentp=NULL;
	int	status;
	int	SYS$PARSE(),
		SYS$SEARCH();
static	const char * dispatch_line[]= {
		"outgoing",
		"incoming",
		"both",
		"none",
		NULL
	};
static	const char dnm[]= "SYS$SYSTEM:.EXE;";
	unsigned long access;
 
	fab= cc$rms_fab;
        nam= cc$rms_nam;

	skipspace();
	do {
		*cpi++= curchar;
		(void) get_char();
	} while ( (curchar) && !isspace (curchar) );

	fab.fab$b_dns = sizeof dnm-1;
	fab.fab$l_dna = (char *) dnm;
	fab.fab$b_fns = cpi-image;
	fab.fab$l_fna = image;
	fab.fab$l_nam = &nam;
	nam.nam$l_esa = esa;
	nam.nam$b_ess = sizeof esa;

	status= SYS$PARSE (&fab);
	if ( (status&1) == 0) {
		static char errmsg[80];
		sprintf (errmsg, "SYS$PARSE returns %08X", status);
		longjmp_mess (error_buf, errmsg);
		}


        switch (getkeyword (dispatch_line)) {
	case 0: access= ARM$M_WRITE  ; break; 
	case 1: access= ARM$M_READ  ; break;
	case 2: access= ARM$M_READ | ARM$M_WRITE; break;
	case 3: access= 0; break;
	}

	for (;;) {                                   
	    	status= SYS$SEARCH (&fab);
	    	if ( (status&1) == 0) {
	    		static char errmsg[80];

	    		if (status==RMS$_NMF) break;
	    		sprintf (errmsg, "SYS$SEARCH returns %08X", status);
	    		longjmp_mess (error_buf, errmsg);
		}          

	    	entp = malloc ( sizeof entp->flink + sizeof entp->blink +
	    			sizeof entp->type
	    			+sizeof entp->size + sizeof entp->struct_image);

	    	entp->struct_image.fid[0]= nam.nam$w_fid[0];
	    	entp->struct_image.fid[1]= nam.nam$w_fid[1];
	    	entp->struct_image.fid[2]= nam.nam$w_fid[2];
	    	if (nam.nam$t_dvi[1]=='_') {
	    		entp->struct_image.devnam[0]= nam.nam$t_dvi[0] -1;
	    		memcpy (entp->struct_image.devnam+1, nam.nam$t_dvi+2, NAM$C_DVI-2);
	    	}
	    	else	memcpy (entp->struct_image.devnam, nam.nam$t_dvi, NAM$C_DVI);

	    	entp->struct_image.access= access;
	    	entp->size= sizeof entp->struct_image;
	    	entp->type= 1;
    		entp->flink= NULL;
	    	if (oldentp==NULL) {
	    	    	entp->blink= NULL; 
	    		oldentp= entp;
	    	}
	    	else{
	    		entp->blink= oldentp->blink;
	    		oldentp->blink= entp;
	    		oldentp->size += entp->size;
	    	}
	}
	return oldentp;	/* seul le 1er chainon contient la bonne taille */
}

static void
setacl()
{
static const char * dispatch_line[]=
	{"acl",
	 "image",
	 "mask",
	 NULL};
	unsigned long inaddr;
	unsigned long inmask= 0xffffffff;
	unsigned short inport=0;
	data_t * entp=NULL;
	struct list_st * ap, * apr;

	skipspace();
	inaddr= get_inaddr();

	do {
		switch (getkeyword (dispatch_line)){
			case 0: entp= getacl();		break;
			case 1: entp= getimage();	break;
			case 2: inmask= get_inaddr();	break;
			case 3: inport= get_inport();	break;
		}
		skipspace();

	} while (curchar != '\0');

	if (entp==NULL) longjmp_mess (error_buf, "No access specified");
	inaddr= inaddr & inmask;
	inmask= ~inmask;	/* table contains 1's complement of mask */

	for (ap=act_list; ap!=NULL; apr=ap, ap=ap->next)
		if ( (ap->acte.addr==inaddr) &&
		     (ap->acte.mask==inmask) &&
		     (ap->acte.port==inport) &&
		     (ap->acte.type==entp->type) ) {
			data_t * dt= ap->blink;
			dt->flink= entp;
			entp->blink= dt;
			ap->blink= entp;
			datasz -= ((ap->acte.size+3) & 0xfffffffc);
			ap->acte.size+= entp->size;
			datasz += ((ap->acte.size+3) & 0xfffffffc);
			return;
		     }
	ap = malloc (sizeof (struct list_st)) ;
	if (act_list==NULL) act_list= ap;
	else	apr->next= ap;
	ap->next= NULL;
	ap->acte.addr= inaddr;
	ap->acte.mask= inmask;
	ap->acte.port= inport;
	ap->acte.size= entp->size;
	ap->acte.type= entp->type;
	ap->flink= entp;
	ap->blink= entp;
	listsz+= sizeof (acte_t);
	datasz+= (entp->size+3) & 0xfffffffc;
}

setowner()
{
	skipspace();
	if (curchar == '[') {
		unsigned short id[2];
		unsigned grp, mem;

		(void) get_char();
		grp= getoct();
		if ( (grp >= 0x3FFF) || (grp==0) )
			longjmp_mess (error_buf, "Invalid group number");
		skipchar (',');
		mem= getoct();
		if ( (mem >= 0xFFFF) || (mem==0) )
			longjmp_mess (error_buf, "Invalid group number");
		skipchar (']');
		id[0]= grp;
		id[1]= mem;
		header.owner= * (long *) id;
	}
	else {
		word_t ident;
		struct dsc$descriptor ident_d= {0, 0, 0, NULL};
		int status;
		int SYS$ASCTOID();

		getword (ident);
		ident_d.dsc$w_length= strlen (ident);
		ident_d.dsc$a_pointer= ident;
		status= SYS$ASCTOID (&ident_d, &header.owner, 0);
		check_status(status, "$ASCTOID");
		if (header.owner & 0xc000000) {
			longjmp_mess (error_buf, "Invalid identifier");
		}
	}
	header.loadopt |= LOADOPT$M_OWNER;
}

static int
getprot()
{
	word_t prot;
	char * cp;
	int bits= 0;

	skipchar (':');
	getword(prot);
	cp= prot;
	for ( ;*cp; cp++)
		switch (*cp) {
		case 'I':
		case 'i': bits |= 1; break;
		case 'O':
		case 'o': bits |= 2; break;
		default:	longjmp_mess (error_buf, "invalid protection");
	}
	return bits;
}

static void
setprot()
{
static const char * dispatch []= 
	{ "system", "owner", "group", "world", NULL};
	union{
		struct {unsigned system:4;
			unsigned owner:4;
			unsigned group:4;
			unsigned world:4;} bits;
		unsigned short w;
	} prot;

	prot.w= 0;

	do {
		switch (getkeyword (dispatch)) {
		case	0: prot.bits.system= getprot(); break;
		case	1: prot.bits.owner= getprot(); break;
		case	2: prot.bits.group= getprot(); break;
		case	3: prot.bits.world= getprot(); break;
		}
		skipspace();
	} while (curchar != 0);

	header.prot= ~prot.w;
	header.loadopt |= LOADOPT$M_PROT;	
}

static void setload()
{
	int n= getnumber();
	if (n>2) longjmp_mess (error_buf, "Invalid loadcode value");
	else if (n) puts ("***WARNING*** unsupported option");
	loadtype= n;
}
int
init_stat()
{
	int cp;
	open_cfg();
	if (cfg!=NULL){
		while (read_cfg() != NULL){
			if (cp=setjmp(error_buf)) cfg_error(errmsgptr);
			else{
				static char * dispatch_line[]=
					{"access",
					 "owner",
					 "protection",
					 "loadcode",
					 NULL};
	
				if (curchar != 0) {
					switch (getkeyword (dispatch_line)){
						case 0: setacl(); break;
						case 1: setowner(); break;
						case 2: setprot(); break;
						case 3: setload();
					}
					skipspace();
					if (curchar != 0) cfg_error ("rest of line ignored");
				}
			}
		}
		if (ferror(cfg)) perror ("read config");
		fclose (cfg);
	}
	else perror ("Open config");
	return init_ok;
}

static
alloc_mem()
{
	int tabsize= sizeof (header_t) + sizeof (acte_t) + listsz + datasz;
	header_t * hp= malloc (tabsize);
	acte_t * listadr= (acte_t *) ((char *) hp+ sizeof (header_t));
	unsigned char * dataadr= (unsigned char *) listadr+ 
					sizeof (acte_t) + listsz;
	struct list_st * lp;
	int	status;
	unsigned long diag[6];

	assert	(sizeof (header_t) == 16); 
	assert  (sizeof (acte_t) == 16); 

	memset (hp, 0, tabsize);
	memcpy (hp, &header, sizeof header);

	for (lp=act_list ; lp!=NULL; lp= lp->next, listadr++) {
		data_t * dp;

		memcpy (listadr, &lp->acte, sizeof (acte_t));
		/* longword displacement relative to current acte */
		listadr->index = ((unsigned) dataadr - (unsigned) listadr) / 4;
		for (dp=lp->flink; dp!=NULL; dp= dp->flink) {
			memcpy (dataadr, dp->buf, dp->size);
			dataadr += dp->size;
		}
		dataadr = (unsigned char *) (((unsigned long) dataadr+3) & -4L);
	}
	hp->rq=4;

	status= LOADCODE (tabsize, hp, diag, loadtype);
	if ((status&1) == 0) {
		printf ("LOADCODE failed: %08X %08X %08X %08X %08X %08X\n",
			diag[0], diag[1], diag[2], diag[3], diag[4], diag[5]);
		if (diag[3] != (unsigned long) &diag[2]) 
			printf ("Illegal diag data\n");
	}
	if (status==3) printf (
"%%TCPFILTER-I-UPDATED, previous security filter replaced\n");
	return status;
}

main()
{
	if (init_stat()) return alloc_mem();
}
