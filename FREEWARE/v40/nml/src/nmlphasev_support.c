/*	logicals affecting the program: */
/*	NMLPHASEV_DNS_DIRECTORY	dns directory of links to use as the node list*/
/*	DECNET_MIGRATE_DIR_SYNONYM same as above, used if the above logical   */
/*			is not defined. If this logical is not defined.	      */
/*			the program uses uses .dna_nodesynonym	*/
/*	NMLPHASEV_IDENTIFICATION	executor identification */
/*	NMLPHASEV_CIRCUIT_NAME		circuit name 		*/
/*								*/
#include <assert.h>
#include <descrip.h>
#include <dnsdef.h>
#include <dnsmsg.h>
#include <lnmdef.h>
#include <net_externals.h>
#include "nfbdef.h"

#include <ctype.h>
#include ssdef
#include stdio
#include stdlib
#include string

#define	NMA$C_NODTY_NR4 5	/* j'ai la flemme d'include nmadef */

int LIB$STOP(int);

static unsigned char nodeaddress [80];	/* 20 seems enough */
static unsigned char * idpointer;
static char addresslen;

struct itmlist	{unsigned short len,typ; void * bufadr, * retadr;};
struct dsc100balles {unsigned short length, mbz; void * addr;};
typedef struct {unsigned short length; char value[80];} asciw;

/*	name: in: logical name string				*/
/* 	buf: out pointer that will receive the buffer address	*/
/*	retlen: will receive the translated length		*/
/*	returns: 1 if the name was translated, else 0		*/
/*	this routine is not reentrant				*/

int	trnlnm (char * name, void ** buf, unsigned short * retlen)
{
static	char	value[256];
static	unsigned short vallen;
static	struct itmlist	trnlst[]= 
			{{sizeof value, LNM$_STRING, value, &vallen},
			 {0, 0, 0, 0}};
static const $DESCRIPTOR (table_desc, "LNM$FILE_DEV");
	int status, SYS$TRNLNM();
	struct dsc100balles name_desc;

	name_desc.addr= name;
	name_desc.length= strlen (name);

	status= SYS$TRNLNM (0, &table_desc, &name_desc, 0, &trnlst);
	if (status&1) {
		* buf= value;
		*retlen= vallen;
		return 1;
	}
	else{
		printf ("$TRLNM %s returns %08X\n", name, status);
		return 0;
	}
}

/*	return != 0 if err */
int strwcpy (char *name, int remsize, void * dest)
{
	int l = strlen (name);
	short * d1p= dest;

	if (l > remsize) return l;
	*d1p= l;
	memcpy (d1p+1, name, l);
	return 0;
}

#define OUTLINK_SZ 8192	/* size of DNS array to fetch links */

typedef struct	{int inited, nextnode;} enumerate_ctx;

static void
enumerate_next(enumerate_ctx * cp)
{
	cp->nextnode= 1;
}

static int
enumerate_nodes(asciw * stringname, enumerate_ctx *cp)
{
static 	const char dnsdir[]= ".dna_nodesynonym";
static	asciw nodename;
static	char opaquedir[256];
static	struct dsc100balles 	value_desc= {sizeof nodename.value, 0, 
						  & nodename.value},
		 		link_desc= {0, 0, NULL};

static	struct itmlist	dnslst[]= 
	{{0, dns$_directory, &opaquedir, 0},
	 {OUTLINK_SZ , dns$_outsoftlinks, NULL , &link_desc.length},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
			dnsresolve[]= 
	{{0, dns$_fromsimplename, nodename.value, 0},
	 {sizeof stringname->value, dns$_tostringname, 0, 0},
	 {0, 0, 0, 0}};


static	struct {unsigned long status;
		unsigned char x1,x2;
		unsigned short reserved;} dnsb;

	int SYS$DNSW(), DNS$REMOVE_FIRST_SET_VALUE();
	int	status;
	int	fini=0;

	if (cp->inited==0) {
static	struct itmlist	dnsparse[]= 
		{{sizeof dnsdir -1, dns$_fromstringname,(void *) dnsdir, 0},
		 {sizeof opaquedir, dns$_tofullname, 0, 0},
		 {0, 0, 0, 0}};

		assert ( (OUTLINK_SZ & 0xffff) == OUTLINK_SZ); /* in a word */
		dnsparse[1].bufadr= & opaquedir;
		dnsparse[1].retadr= & dnslst[0].len;
		dnslst[1].retadr= & link_desc.length;
		dnslst[2].len= 0;
		dnslst[2].typ= 0;
		dnslst[2].bufadr= 0;
		if (link_desc.addr == NULL) {
			link_desc.addr= malloc (OUTLINK_SZ);
			dnslst[1].bufadr= link_desc.addr;
		}
		value_desc.addr= nodename.value;

		if (trnlnm ("NMLPHASEV_DNS_DIRECTORY", &dnsparse[0].bufadr,
						 &dnsparse[0].len)==0)
			(void) trnlnm ("DECNET_MIGRATE_DIR_SYNONYM",
							   &dnsparse[0].bufadr,
							   &dnsparse[0].len);

		status= SYS$DNSW (0, dns$_parse_fullname_string, dnsparse, 0, 0, 0);
		if ((status&1)==0) {
			printf ("Erreur dans $DNS parse\n");
			LIB$STOP (status);
		}
		else printf ("$DNS parse ok: status: %08X\n", status);
		status=SYS$DNSW (0, dns$_enumerate_softlinks, &dnslst, &dnsb,
				  0, 0);
		printf ("$DNS enumerate: status=%08x; dnsb status: %08X len= %hd\n",
				status, dnsb.status, link_desc.length);
		cp->inited= status&1;
		cp->nextnode= 1;
	}

	if (cp->nextnode)
	   while (cp->inited) {
		status= DNS$REMOVE_FIRST_SET_VALUE (
						&link_desc, 
						&value_desc, &nodename.length,
						0, 0, 
						&link_desc, &link_desc.length);
		if (status&1) {
			dnslst[2].len= nodename.length;
			dnslst[2].typ= dns$_contextvarname;
			dnslst[2].bufadr= nodename.value;
			break;
		}
		else	if (dnsb.status==DNS$_MOREDATA) {
				printf ("DNS$REMOVE... =>%08X\n", status);
				status=SYS$DNSW (0, dns$_enumerate_softlinks, 
						 &dnslst, &dnsb,
						 0, 0);
				printf ("Status: %08x; dnsb status: %08X len= %hd\n",
					status, dnsb.status, link_desc.length);
				if ((status&1) == 0) cp->inited= 0; /* fini. */
			}
			else {
				status= dnsb.status;
				fini=1;
				break;
			}
	   }	/* while */
	/* if nextnode */

	if (cp->inited) {
		dnsresolve[0].len= nodename.length;
		dnsresolve[1].bufadr= &stringname->value;
		dnsresolve[1].retadr= &stringname->length;
		status= SYS$DNSW (0, dns$_simple_opaque_to_string,
					  &dnsresolve, 0, 0, 0);
		if ((status&1)==0) {
			printf ("Error $DNS (opaque_to_string)\n");
			LIB$STOP(status);
		}
		cp->nextnode= 0;
	}

	if (fini || ((status&1)==0)) stringname->length= 0;
	return status;
}

void printhex (void *start, int len)
{
	int i;
	unsigned char *upt= start;
	for (i=0; i<len; i++) printf ("%s%02X", (i==0) ? "" : "-", upt[i]);
}

#pragma member_alignment save
#pragma nomember_alignment
void print_towerset (void * ptset, int size)
{
	struct { char flag; unsigned short ofarrlen,mbarrlen;
		 unsigned short offsets[255];} * pthdr= ptset;
	int i;

	if ((pthdr->flag&1) == 0) {
		printf ("Invalid tower set\n");
		return;
	} else
	   for (i=0; 2*i<pthdr->ofarrlen; i++) {
		struct { char flag; char stamp[14];
			 unsigned short twlen, npair;
			 unsigned char val[2048];} * ptmemb;
		ptmemb= (void *) (pthdr->offsets[i] + (char *) ptset);

		if ((ptmemb->flag&1) == 0)
			printf ("Invalid tower #%d\n", i);
		else {
		   int j;
		   struct {unsigned short len; unsigned char val[2048];} *pit;
		   pit= (void *) ptmemb->val;
		   for (j= 0; j<ptmemb->npair; j++) {
			unsigned char proto;
			printf ("protocol: ");
			printhex (pit->val, pit->len);
			printf ("\n");
			if (pit->len == 1)
				proto= pit->val[0];
			else	proto= 0;
			pit= (void *) (2+ pit->len + (char *) pit);
			printf ("address: ");
			printhex (pit->val, pit->len);
			printf ("\n");
			if ((proto==6)  /* pas trouve de constante symbolique */
			    && (addresslen==0)) {
				unsigned char area;
				area=	pit->val[pit->len-8] +
					256*pit->val[pit->len-9];
				if ( (area > 0) && (area < 64) ) {
					if ( (pit->val[pit->len-7]==0xAA)
					  && (pit->val[pit->len-6]==0)
					  && (pit->val[pit->len-5]==4)
					  && (pit->val[pit->len-4]==0)
					  && (pit->val[pit->len-1]==32)/*NSP*/){
						memcpy (nodeaddress, pit->val,
							pit->len);
						idpointer=
							nodeaddress+pit->len-9;
						addresslen= pit->len;
					}
				}
			}
			pit= (void *) (2+ pit->len + (char *) pit);
		   } /* address tower scan */
		}
		printf ("\n");
	   }	/* offset array scan */
}
#pragma member_alignment restore

int
nml$v_getidp()
{
	int status;
	int SYS$IPCW();
static	struct NET$IPCBDEF ipcb;
	unsigned char outputlst[2048];
	struct {unsigned short len,typ; unsigned char val[2048];} * itlst;
	int len;

	ipcb.IPCB$R_OUTPUTLSTDESC.IPCB$R_OUTPUTLST_DESC_FIELDS.IPCB$A_OUTPUTLST_POINTER= (void *) outputlst;
	ipcb.IPCB$R_OUTPUTLSTDESC.IPCB$R_OUTPUTLST_DESC_FIELDS.IPCB$W_OUTPUTLST_LENGTH= sizeof outputlst;
	status= SYS$IPCW (0, IPC$K_FC_ENUMERATE_LOCAL_TOWERS, &ipcb, 0, 0);
	if ((status&1)==0) exit (status);
	len= ipcb.IPCB$W_RET_OUTPUTLST_LENGTH;
	itlst= (void *) &outputlst;
	while (len>0) {
		printf ("len: %d; code:%d\n", itlst->len, itlst->typ);
		switch (itlst->typ) {
		case NET$K_TAG_SOURCETOWERSET :
				print_towerset (itlst->val, itlst->len);
				break;
		default: ;
		}
		len -= itlst->len;
		itlst= (void *) (itlst->len + (char *) itlst);
	}
	if (len < 0) printf ("Illegal output list length\n");
	if (addresslen) {
		printf ("NSP address: ");
		printhex (nodeaddress, addresslen);
		printf ("\n");
	}
	return ipcb.IPCB$L_STATUS;
}

int
nml$V_nametoaddr(int name_len, char * name, unsigned short * phIVaddr)
{
	int status;
	int SYS$IPCW();
static	struct NET$IPCBDEF ipcb;
	unsigned char outputlst[2048];
	struct {unsigned short len,typ; unsigned char val[2048];} * itlst;
	unsigned short addr;
	int len;
static	struct {unsigned short len,typ;} templatelst= 
		{sizeof templatelst, NET$K_TAG_IV_ADDRESS};
static	struct {unsigned short len,typ; char ph_IV_name[256];} inputlst= 
		{sizeof inputlst, NET$K_TAG_NODESYNONYM, ""};

	if (name_len > sizeof inputlst.ph_IV_name) return 0;
	memcpy (inputlst.ph_IV_name, name, name_len);
	inputlst.len= 4+name_len;

 ipcb.IPCB$R_OUTPUTLSTDESC.IPCB$R_OUTPUTLST_DESC_FIELDS.IPCB$A_OUTPUTLST_POINTER
		= (void *) outputlst;
 ipcb.IPCB$R_OUTPUTLSTDESC.IPCB$R_OUTPUTLST_DESC_FIELDS.IPCB$W_OUTPUTLST_LENGTH
		= sizeof outputlst;
 ipcb.IPCB$R_INPUTLSTDESC.IPCB$R_INPUTLST_DESC_FIELDS.IPCB$A_INPUTLST_POINTER
		= (void *) &inputlst;
 ipcb.IPCB$R_INPUTLSTDESC.IPCB$R_INPUTLST_DESC_FIELDS.IPCB$W_INPUTLST_LENGTH
		= inputlst.len;
 ipcb.IPCB$R_TEMPLATELSTDESC.IPCB$R_TEMPLATELST_DESC_FIELDS.IPCB$A_TEMPLATELST_POINTER
		= (void *) &templatelst;
 ipcb.IPCB$R_TEMPLATELSTDESC.IPCB$R_TEMPLATELST_DESC_FIELDS.IPCB$W_TEMPLATELST_LENGTH
		= sizeof templatelst;

	status= SYS$IPCW (0, IPC$K_FC_BACKTRANSLATE, &ipcb, 0, 0);
	if ((status&1)==0) LIB$STOP (status);
	len= ipcb.IPCB$W_RET_OUTPUTLST_LENGTH;
	itlst= (void *) &outputlst;
	while (len>0) {
		printf ("len: %d; code:%d\n", itlst->len, itlst->typ);
		switch (itlst->typ) {
		case NET$K_TAG_IV_ADDRESS:
			addr= itlst->val[0] + (itlst->val[1]<<8);
			printf ("Address: %hu\n", addr);
			*phIVaddr= addr;
			break;
		default:printf ("Value returned is not in template list\n");
		}
		len -= itlst->len;
		itlst= (void *) (itlst->len + (char *) itlst);
	}

	if (len < 0) printf ("Illegal output list length\n");
	return ipcb.IPCB$L_STATUS;
}

static
int
nml$V_addrtoname (unsigned short addr, unsigned short * namelen,
				char * name)
{
	int status;
	int SYS$IPCW();
static	struct NET$IPCBDEF ipcb;
	unsigned char outputlst[2048];
	struct {unsigned short len,typ; unsigned char val[2048];} * itlst;
	int len;
static	struct {unsigned short len,typ;} templatelst= 
		{sizeof templatelst, NET$K_TAG_NODESYNONYM};
static	struct {unsigned short len,typ, ph_IV_addr;} inputlst= 
		{sizeof inputlst, NET$K_TAG_IV_ADDRESS, 0};

 ipcb.IPCB$R_OUTPUTLSTDESC.IPCB$R_OUTPUTLST_DESC_FIELDS.IPCB$A_OUTPUTLST_POINTER
		= (void *) outputlst;
 ipcb.IPCB$R_OUTPUTLSTDESC.IPCB$R_OUTPUTLST_DESC_FIELDS.IPCB$W_OUTPUTLST_LENGTH
		= sizeof outputlst;
 ipcb.IPCB$R_INPUTLSTDESC.IPCB$R_INPUTLST_DESC_FIELDS.IPCB$A_INPUTLST_POINTER
		= (void *) &inputlst;
 ipcb.IPCB$R_INPUTLSTDESC.IPCB$R_INPUTLST_DESC_FIELDS.IPCB$W_INPUTLST_LENGTH
		= sizeof inputlst;
 ipcb.IPCB$R_TEMPLATELSTDESC.IPCB$R_TEMPLATELST_DESC_FIELDS.IPCB$A_TEMPLATELST_POINTER
		= (void *) &templatelst;
 ipcb.IPCB$R_TEMPLATELSTDESC.IPCB$R_TEMPLATELST_DESC_FIELDS.IPCB$W_TEMPLATELST_LENGTH
		= sizeof templatelst;

	inputlst.ph_IV_addr= addr;
	status= SYS$IPCW (0, IPC$K_FC_BACKTRANSLATE, &ipcb, 0, 0);
	if ((status&1)==0) LIB$STOP (status);
	len= ipcb.IPCB$W_RET_OUTPUTLST_LENGTH;
	itlst= (void *) &outputlst;
	while (len>0) {
		printf ("len: %d; code:%d\n", itlst->len, itlst->typ);
		switch (itlst->typ) {
		case NET$K_TAG_NODESYNONYM:
			printf ("Synonym: %.*s\n", itlst->len-4,
						   itlst->val);
			* namelen= itlst->len-4;
			memcpy (name, itlst->val, *namelen);
			break;
		default:printf ("Value returned is not in template list\n");
		}
		len -= itlst->len;
		itlst= (void *) (itlst->len + (char *) itlst);
	}

	if (len < 0) printf ("Illegal output list length\n");
	return ipcb.IPCB$L_STATUS;
}

static
int
nml$V_addrtoaddr (unsigned short addr, unsigned short * outaddr)
{
	int status;
	int SYS$IPCW();
static	struct NET$IPCBDEF ipcb;
	unsigned char outputlst[2048];
	struct {unsigned short len,typ; unsigned short val;} * itlst;
	int len;
static	struct {unsigned short len,typ;} templatelst= 
		{sizeof templatelst, NET$K_TAG_IV_ADDRESS};
static	struct {unsigned short len,typ, ph_IV_addr;} inputlst= 
		{sizeof inputlst, NET$K_TAG_IV_ADDRESS, 0};

 ipcb.IPCB$R_OUTPUTLSTDESC.IPCB$R_OUTPUTLST_DESC_FIELDS.IPCB$A_OUTPUTLST_POINTER
		= (void *) outputlst;
 ipcb.IPCB$R_OUTPUTLSTDESC.IPCB$R_OUTPUTLST_DESC_FIELDS.IPCB$W_OUTPUTLST_LENGTH
		= sizeof outputlst;
 ipcb.IPCB$R_INPUTLSTDESC.IPCB$R_INPUTLST_DESC_FIELDS.IPCB$A_INPUTLST_POINTER
		= (void *) &inputlst;
 ipcb.IPCB$R_INPUTLSTDESC.IPCB$R_INPUTLST_DESC_FIELDS.IPCB$W_INPUTLST_LENGTH
		= sizeof inputlst;
 ipcb.IPCB$R_TEMPLATELSTDESC.IPCB$R_TEMPLATELST_DESC_FIELDS.IPCB$A_TEMPLATELST_POINTER
		= (void *) &templatelst;
 ipcb.IPCB$R_TEMPLATELSTDESC.IPCB$R_TEMPLATELST_DESC_FIELDS.IPCB$W_TEMPLATELST_LENGTH
		= sizeof templatelst;

	inputlst.ph_IV_addr= addr;
	status= SYS$IPCW (0, IPC$K_FC_BACKTRANSLATE, &ipcb, 0, 0);
	if ((status&1)==0) LIB$STOP (status);
	len= ipcb.IPCB$W_RET_OUTPUTLST_LENGTH;
	itlst= (void *) &outputlst;
	while (len>0) {
		printf ("len: %d; code:%d\n", itlst->len, itlst->typ);
		switch (itlst->typ) {
		case NET$K_TAG_IV_ADDRESS:
			printf ("address: %hds\n", itlst->val);
			* outaddr= itlst->val;
			break;
		default:printf ("Value returned is not in template list\n");
		}
		len -= itlst->len;
		itlst= (void *) (itlst->len + (char *) itlst);
	}

	if (len < 0) printf ("Illegal output list length\n");
	return ipcb.IPCB$L_STATUS;
}

typedef struct {
	unsigned short	length;
	char		bufadr[256];
	}	nmastring;

/*	in: logname: logical name to translate	*/
/*	in: remsize: number of bytes available in buf, including count size */
/*	in: buf: address of parameter */
/*	returns	1: not enough bytes to store the result */
/*		0: enough bytes (length=0 if the translation fails) */
static int
nmalog (char * logname, unsigned remsize, nmastring * buf)
{
	int status;
	void * value;
	unsigned short len;

	status= trnlnm (logname, &value, &len);
	if ((status&1)==0) len=0;
	if ((len + sizeof buf->length) <= remsize) {
		buf->length= len;
		memcpy (&buf->bufadr, value, len);
		printf ("nmalog: value= [%*s]\n", buf->length, value);
		return 0;	/* no overflow */
	}
	else	return 1;
}

typedef struct {unsigned short length,pouet; void * addr;} * qiopx;
typedef struct {unsigned short status, count; unsigned long diag;} nmliosb;

#define cksize(n) {if (remsize < n) {printf ("%d<%d\n",remsize, n); \
					ovf= 1; break;}}

static int
nmlshow (iosb, p1, p2, p3, p4)
nmliosb * iosb;
qiopx p1, p2, p4;
unsigned short * p3;
{
	struct NFBDEF *np= p1->addr;
	int * itmptr;
	int	idsize,
		remid,
		remsize,
		ovf=0,
		notfirstcall,
		ent_cnt,
		svcount,
		srchkey;
	void *	ctx_ptr=  ((char *) p2->addr + p2->length - NFB$C_CTX_SIZE);
	char * p4buf= p4->addr;
	short * keyaddr = (void *) ((char *) p2->addr + 4); /* first key only */
	int	shoknode= 0;

	notfirstcall= * (long *) ctx_ptr;

	if ( (np->NFB$B_DATABASE!=NFB$C_DB_NDI)
	   &&(np->NFB$B_DATABASE!=NFB$C_DB_LNI)	) {
		iosb->status= SS$_ILLCNTRFUNC;
		iosb->diag= NFB$_ERR_DB;
		return SS$_NORMAL;
	}

	if (np->NFB$L_SRCH_KEY != NFB$C_WILDCARD) {
		if ( (np->NFB$B_OPER != NFB$C_OP_EQL) ) {
			iosb->status= SS$_ILLCNTRFUNC;
			iosb->diag= NFB$_ERR_OPER;
			return SS$_NORMAL;
		}
		switch (np->NFB$L_SRCH_KEY) {
		case	NFB$C_NDI_NNA:
		case	NFB$C_NDI_TAD:
		case	NFB$C_NDI_ADD:	break;
		default:iosb->status= SS$_NOSUCHDEV;
			iosb->diag= np->NFB$L_SRCH_KEY;
			printf ("Bad search key %d\n", np->NFB$L_SRCH_KEY);
			return SS$_NORMAL;
		}
	}

	if ( (np->NFB$R_FLAGS_OVERLAY.NFB$B_FLAGS ==3) /* mult, errcont */
	   &&(np->NFB$B_DATABASE == NFB$C_DB_NDI)
	   &&(np->NFB$L_SRCH_KEY == NFB$C_WILDCARD) ) {
			shoknode=1;
	}
	else
		if (np->NFB$L_SRCH2_KEY != NFB$C_WILDCARD) {
			if ( (np->NFB$B_OPER2 != NFB$C_OP_EQL) ) {
				iosb->status= SS$_ILLCNTRFUNC;
				iosb->diag= NFB$_ERR_OPER2;
				return SS$_NORMAL;
			}
			switch (np->NFB$L_SRCH2_KEY) {
			default:iosb->status= SS$_ILLCNTRFUNC;
				iosb->diag= NFB$_ERR_SRCH2;
				return SS$_NORMAL;
			}
		}

	idsize= p1->length-NFB$K_LENGTH;
	if ( (idsize % sizeof *itmptr) != 0) {
		iosb->status= SS$_ILLCNTRFUNC;
		iosb->diag= NFB$_ERR_P1;
		return SS$_NORMAL;
	}

	idsize /= sizeof *itmptr;
	remsize= p4->length;
	ent_cnt= 0;

	do {
	    if (shoknode) {
static		asciw	nodename;
		int status;
		char * cp;

		status= enumerate_nodes(& nodename, ctx_ptr);
		if ((status&1) == 0) {
			return status;
		}
		if (nodename.length==0) break;
		keyaddr= (void *) & nodename;
		srchkey= NFB$C_NDI_NNA;
	        svcount= iosb->count;
		for (cp=nodename.value  ; cp < nodename.value+nodename.length
					;cp++)
			if (islower (*cp)) * cp=toupper (*cp);
	    }
	    else srchkey= np->NFB$L_SRCH_KEY;

	    for (itmptr= (void *) &np->NFB$L_FLDID, remid= idsize; 
			remid>0 ; 
			remid--) {
		int curitm= *itmptr++;
		int typ= (curitm>>16) &3;

		printf ("Working on item %08X\n", curitm);
		if (curitm == NFB$C_ENDOFLIST) break;

		if (typ == NFB$C_TYP_STR) {
			cksize (2);
			* (short *) p4buf = 0;
		}
		else{
			cksize (4);
			* (long *) p4buf = 0;
		}

		switch (curitm) {
		case	NFB$C_LNI_ADD:
					nml$V_addrtoaddr( 0, (void *) p4buf);
					break;
		case	NFB$C_LNI_NAM:
					cksize (8);
				nml$V_addrtoname ( 0, (void *) p4buf, p4buf+2);
					break;
		case	NFB$C_LNI_ETY:
					*(long *) p4buf = NMA$C_NODTY_NR4;
					break;
		case	NFB$C_LNI_NVE:	/* nsp version (3 bytes */
					ovf= strwcpy ("421",remsize, p4buf);
					break;
		case	NFB$C_LNI_MVE:	/* management version (3 bytes */
					ovf= strwcpy ("503",remsize, p4buf);
					break;
		case	NFB$C_LNI_RVE:	/* routing version (3 bytes */
					ovf= strwcpy ("300",remsize, p4buf);
					break;
		case	NFB$C_LNI_IDE:
					ovf= nmalog (
						"NMLPHASEV_IDENTIFICATION",
						remsize, (nmastring *) p4buf);
					if ((ovf == 0) && 
					    (* (short *) p4buf == 0) )
						ovf= strwcpy ("GG was here",
							      remsize,
							      p4buf);
					break;
		case	NFB$C_LNI_IDP:{
					int i;
					unsigned char * ucp;
					char *cp;
					

					nml$v_getidp();
					addresslen -= 9;
					cksize (2*addresslen);
					for (i=addresslen, ucp=nodeaddress,
					 			 cp= p4buf+2;
									 i>0; 
								   i--, ucp++){
						sprintf (cp, "%02X", *ucp);
						cp += 2;
					}
					* (short *) p4buf= addresslen*2;
					break;
					}

		case	NFB$C_NDI_TAD:
			switch (srchkey) {
			case	NFB$C_NDI_NNA:
				if (1&(nml$V_nametoaddr (* keyaddr,
						         (char *)keyaddr+2,
						         (void *)p4buf))==0) {
					iosb->status= SS$_ENDOFFILE;
					return SS$_NORMAL;
				}
				break;
			case	NFB$C_NDI_ADD:
				if (1&(nml$V_addrtoaddr (* keyaddr,
						         (void *)p4buf))==0) {
					iosb->status= SS$_ENDOFFILE;
					return SS$_NORMAL;
				}
				break;
			}
			break;
		case	NFB$C_NDI_NNA:
					cksize (8);
			switch (srchkey) {
			case	NFB$C_NDI_NNA: 
					memcpy (p4buf, keyaddr, *keyaddr+2);
					break;
			case	NFB$C_NDI_ADD:
					if ((nml$V_addrtoname ( *keyaddr,
								(void *) p4buf,
								p4buf+2)
							&1)==0){
						iosb->status= SS$_ENDOFFILE;
						return SS$_NORMAL;
					}
			}
			break;
		case	NFB$C_NDI_NLI:
				ovf= nmalog ("NMLPHASEV_CIRCUIT_NAME",
					     remsize, (nmastring *) p4buf);
		default:;
		}

		if (ovf) break;

		if (typ==NFB$C_TYP_STR){
			iosb->count += 2+ * (unsigned short *) p4buf;
			remsize -= 2+ * (unsigned short *) p4buf;
			p4buf += 2 + * (unsigned short *) p4buf;
		}
		else{
			iosb->count += 4;
			remsize -= 4;
			p4buf +=  4;
		}
	    } /* for */

	    if (ovf==0){
		ent_cnt++;
		enumerate_next(ctx_ptr);
	    }

	} while (shoknode && (ovf==0));

	*p3= iosb->count;

	if (ovf) {
		iosb->count= svcount;
		iosb->status= SS$_BUFFEROVF;
	}
	else	if (shoknode && notfirstcall)
			iosb->status= SS$_ENDOFFILE;
		else	iosb->status= SS$_NORMAL;

	* (unsigned long *) p2->addr= ent_cnt;
	iosb->diag= 0;
	return SS$_NORMAL;
}

int
nml$phasevqio (iosb, p1, p2, p3, p4)
nmliosb * iosb;
qiopx p1, p2, p4;
unsigned short * p3;
{
	struct NFBDEF *np= p1->addr;
	/* no error checking done on parameters length */
	printf ("Function %d; database: %d\n",	np->NFB$B_FCT,
						np->NFB$B_DATABASE);
	iosb->count= 0;
	if (np->NFB$B_FCT==NFB$C_FC_SHOW) {
		nmlshow (iosb, p1, p2, p3, p4);
	}
	else {
		iosb->status= SS$_ILLCNTRFUNC;
		iosb->diag= NFB$_ERR_FCT;
	}
	return SS$_NORMAL;
}
