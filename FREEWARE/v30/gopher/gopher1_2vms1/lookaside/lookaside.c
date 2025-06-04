/* Edit a file's Access Control List, accessing *only* GOPHER Lookaside
    entries.  Use callable EDT as the editor				*/
/* J.Lance Wilkinson, Penn State University Library Computing Services	*/

/* usage: lookaside [filename]	[options]				*/

/* 1.0 07/19/93 Original attempt */
/* 1.1 07/21/93 Added config file, eliminated journalling */
/* 1.2 07/23/93 Added new tuple entries Head=, Foot=, RHead=, RFoot= */
/* 1.3 11/02/93 Added command line options */
/* 1.4 11/03/93 Added /Clear command line switch */
/* 1.5 11/08/93 Fixed configuration bug introduced in 1.3   */
/* 1.6 11/20/93 Made system string.h explicit */

#include "gsgopherobj.h"	    /* GS_* */
#include <sys$library:string.h>	    /* C RTL string functions */
#include <ctype.h>		    /* isspace(), etc.	*/
#include <stdio.h>		    /* NULL, stdin, etc... */
#include <atrdef.h>		    /* VMS File Attributes definitions */
#include <descrip.h>		    /* string descriptor defs. */
#include <ssdef.h>		    /* error codes, etc. */
#include <rms.h>		    /* RMS NAM defs */
#include <acedef.h>		    /* ACE defs */
#include <acldef.h>		    /* ACL defs */
#include <dvidef.h>		    /* For determining SYS$INPUT device class */
#include <dcdef.h>
#include <unixlib.h>

int DEBUG = 0;
int access_tuple;
int interactive;
int clear_buf = 0;
int clt = 0;
char *argv_tuple[GS_TUPLE_CNT];
char *keyword[GS_TUPLE_CNT] = { GS_NAME, GS_TYPE, GS_PATH, GS_HOST, 
				    GS_PORT, GS_NUMB, GS_HDDN, GS_ACCS,
				    GS_HEAD, GS_FOOT, GS_RHEAD, GS_RFOOT };
$DESCRIPTOR(dsc$sys_input, "SYS$INPUT");

#define CTX_CLOSED	-1
#define CTX_OPEN	1
#define CTX_MORE_LKS	2
#define CTX_EOF_LKS	3
#define CTX_EOF_ACL	4
#define CTX_MORE_CMD	5

struct bpv_arg	    /* Bound Procedure Value */
{
    int *routine$;  /* Pointer to routine */
    int env$;	    /* Pointer to environment */
} ;

struct item	    /* Itemlist entry */
{
    short   buflen;
    short   code;
    char    *bufadr;
    int	    unused;
} ;
static	char	*g$ace[GS_TUPLE_CNT];

#define ACL_BUFFER_SIZE ATR$S_READACL /* ACL buffer size
                                         (512 is an XQP limit) */

static	struct FAB acl_fab;	/* storage for a FAB */
static	struct FAB lks_fab;	/* storage for another FAB */
static	struct RAB lks_rab;	/* storage for a RAB */
static	struct XABPRO acl_xab;	/* storage for an XABPRO */
static	struct NAM acl_nam;	/* storage for a NAM */

static	char	*acl_ptr=NULL;
static	char	*acl_new=NULL;
static	char	lookaside[256];

int strncasecomp(const char *s, const char *, register size_t);

void
ZapCRLF(inputline)
  char *inputline;
{
     char *cp;

     cp = strchr(inputline, '\r');    /* Zap CR-LF */
     if (cp != NULL)
	  *cp = '\0';
     else {
	  cp = strchr(inputline, '\n');
	  if (cp != NULL)
	       *cp = '\0';
     }
}

main(int argc, char *argv[])                /* Main entry point */
{
    int	    status;
    int	    nc;
    char    c;
    char    *cp;
    char    *ConfigFile;
    char    *FileName;
    struct  
	dsc$descriptor_s
	    input_file, command_file;
    int	    acl_fileio(int *code, int *stream,
		    struct dsc$descriptor *record, struct dsc$descriptor *rhb);
globalvalue edt$m_nocreate;
globalvalue edt$m_nocommand;
globalvalue edt$m_nooutput;
globalvalue edt$m_nojournal;
globalvalue edt$m_command;
globalvalue edt$m_recover;
globalvalue edt$_inpfilnex;
    int	    options;
    FILE    *gdcfile;
    char    inputline[256];
    char    *token;
    char    *restofline;
static 
    struct 
	 bpv_arg 
	    fileio_bpv = {(int *)acl_fileio, 0};

    lib$getdvi(&DVI$_DEVCLASS,0,&dsc$sys_input, &interactive, 0, 0);
    interactive = (interactive == DC$_TERM);

    for (status=0; status<GS_TUPLE_CNT;
	    argv_tuple[status] = g$ace[status] = NULL, status++);
    FileName = NULL;
    ConfigFile = (char *)malloc(sizeof(char)*256);
    *ConfigFile = '\0';
    access_tuple = LinkTuple(GS_ACCS,NULL);
/**
 *	    Right here we have to parse the entire command line, picking off
 *	    the filename, if any, and collecting tuple-specific headers if
 *	    any.
 **/
    for (nc=1; nc<argc; ++nc)
    {
	switch(*argv[nc])
	{
    case '/':	if (DEBUG)
		    printf("\tDebug: %s\n",argv[nc]);
		argv[nc]++;
		c = toupper(*argv[nc]);
		switch(c)
		{
	case 'N':   /*	    /NoInteractive, /NoConfig, /NoDebug, /Nxxx	*/
		    if (strncasecomp(argv[nc],"NOCONFIG",8)==0) {
			if (strlen(ConfigFile))
			    printf("\tConfiguration Discarded\n");
			free(ConfigFile);
			ConfigFile = NULL;
			break;
		    }
		    else
		    if (strncasecomp(argv[nc],"NODEBUG",7)==0) {
			if (DEBUG)
			    printf("\tDEBUG MODE Stopped%s\n",
				(interactive)?", Interactive":"");
			DEBUG = 0;
			break;
		    }
		    else
		    if (strncasecomp(argv[nc],"NOINTERACTIVE",13)==0) {
			if (interactive)
			    printf("\tNo Interactive Editing\n");
			interactive = 0;
			break;
		    }
		    else
			goto n_something;
	case 'C':   /*	    /Config=filename		*/
		    if (strncasecomp(argv[nc],"CONFIG",6)==0) {
			cp = strchr(argv[nc],'=');
			if (cp) {
			    if (!ConfigFile)
				ConfigFile = (char *)malloc(sizeof(char)*256);
			    strcpy(ConfigFile, ++cp);
			}
		    }
		    else if (strncasecomp(argv[nc],"CLEAR",5)==0) {
			clear_buf = 1;
			if (DEBUG)
			    printf("\tDebug: Existing lookasides ignored\n");
		    }
		    else
			goto eh_bad_opt;
		    break;
	case 'D':   /*	    /DEBUG			*/
		    if (strncasecomp(argv[nc],"DEBUG",5)!=0)
			goto eh_bad_opt;
		    if (!DEBUG)
			printf("\tDEBUG MODE Started%s\n",
				(interactive)?", Interactive":"");
		    DEBUG = 1;
		    break;
    default:
    n_something:    if ((status=LinkTuple(argv[nc],NULL))>-1) {
			if (argv_tuple[status])
			    printf("\tIgnoring prior /%s%s\n",keyword[status],
				argv_tuple[status]+strlen(keyword[status]));
			argv_tuple[status] = argv[nc];
			clt++;
			break;
		    }
		    
    eh_bad_opt:	    printf("Eh? \"%s\"\n", argv[nc]);
		    printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
		    "$ LOOKASIDE filename   /[NO]Config=gopher_server_config_file",
		    "			    /tuple_entry=\"tuple entry value\"",
		    "	Default:    LOOKASIDE",
		    "	tuple_entries:",
		    "\tName=\"Name for the item\"",
		    "\tType=1-character type code",
		    "\tPath=\"server-specific selector string\"",
		    "\tPort=TCP/IP port number",
		    "\tHost=DNS host name",
		    "\tNumb=sort placement weight",
		    "\tHidden",
		    "\tAccess={default|DNS site} [!]read [!]browse [!]search",
		    "\tHead=document header \"string\" or filename",
		    "\tFoot=document footer \"string\" or filename",
		    "\tRHead=document range header \"string\" or filename",
		    "\tRFoot=document range footer \"string\" or filename");
		    return(1);
		}
		break;
    default:	if (FileName) {
		    printf("\t%s already specified!\n", FileName);
		    return(1);
		}
		FileName = argv[nc];
	}
     }
/**/
    if (!FileName) {                  /* Verify that a file name was specified */
	if (interactive) {
	    FileName = (char *)malloc(sizeof(char)*256);
	    printf("\nEnter File Specification: "),status=scanf("%s",FileName);
	    fflush(stdin);
	    if (status==EOF)
		return(1);
	    }
	else {
	    printf("\tMust specify a filename to edit\n");
	    return(2);
	}
    }
/**/
    lookaside[0] = '\0';	    /* Validate Config for lookaside subdir */
    if (!ConfigFile)
	goto no_config;
    if (!strlen(ConfigFile)) {
	strcpy(ConfigFile, "GOPHER_CONFIG");
	if (getenv(ConfigFile) != NULL)
	    strcpy(ConfigFile, getenv(ConfigFile));
	else {
    retry_config:	 
	    if (!interactive)
		goto no_config;
	    printf("\nEnter Configuration File Specification: ");
	    status=scanf("%s",ConfigFile);
	    fflush(stdin);
	    if (status==EOF) {
	    no_config:
		printf("\tCannot collect possible actual lookaside file\n");
		goto done_config;
	    }
	}
    }
    if ((gdcfile = fopen(ConfigFile, "r")) == (FILE *) NULL) {
	  printf("Cannot open file '%s'\n", ConfigFile);
	  if (interactive)
	    goto retry_config;
	  else
	    goto no_config;
     }
     while (fgets(inputline, sizeof(inputline), gdcfile)!= NULL) {
	  ZapCRLF(inputline);
	  if (*inputline == '#' || *inputline == '\0') /** Ignore comments **/
	       continue;
	  cp = strchr(inputline, ':');
	  if (cp == NULL) 
	       continue;
	  *cp = '\0';
	  token      = inputline;
	  if (strncasecomp(token, "LookAside", 9)==0) {
	    restofline = cp+1;
	    while (*restofline == ' ' || *restofline == '\t')
	       restofline++;
	    if ((cp = strchr(restofline, '!')) != NULL) {
		*cp = '\0';
		for(--cp;isspace(*cp)||!isprint(*cp);cp--)    
		    *cp = '\0';
		strcpy(lookaside, restofline);
	    }
	  }
     }
     fclose(gdcfile);
     if (strlen(lookaside)==0) {
	if (interactive)
	    goto retry_config;
	else
	    goto no_config;
     }
done_config:
    acl_fab = cc$rms_fab;		/* Set up the FAB blocks */
    lks_fab = cc$rms_fab;
    lks_rab = cc$rms_rab;		/* Set up the RAB block */
    acl_xab = cc$rms_xabpro;		/* Set up the XAB block */
    acl_nam = cc$rms_nam;		/* Set up the NAM block */

    lks_fab.fab$l_ctx = acl_fab.fab$l_ctx = CTX_CLOSED; /* Files Closed */
    acl_xab.xab$l_aclbuf = NULL;
    acl_xab.xab$w_aclsiz = 0;
    command_file.dsc$b_class = input_file.dsc$b_class = DSC$K_CLASS_S;
    acl_fab.fab$l_fna = input_file.dsc$a_pointer = FileName;
    acl_fab.fab$b_fns = input_file.dsc$w_length  = strlen(FileName);
    if (!(1&(status=sys$open(&acl_fab,0,0)))) {
	printf("\n\nCannot open file %s\n",FileName);
	exit(status);
    }
    sys$close(&acl_fab,0,0);
    command_file.dsc$a_pointer =
    		(char *)malloc(sizeof(char)*(5+strlen(argv[0])));
    strcpy(command_file.dsc$a_pointer, argv[0]);
    cp = strrchr(command_file.dsc$a_pointer,'.');
    strcpy(cp,".ini");
    command_file.dsc$w_length  = strlen(command_file.dsc$a_pointer);
    if (DEBUG) printf("\tDebug: Command file %.*s\n",command_file.dsc$w_length,
				command_file.dsc$a_pointer);
    options = edt$m_nocreate;
    status = EDT$EDIT(&input_file, 0/*out_file*/, &command_file, 
				    0/*jou_file*/, &options, &fileio_bpv, 
					0/*workio*/, 0/*xlate*/);
    if (status != SS$_NORMAL)
	return(0);
    else {
	if (status == edt$_inpfilnex)
	    printf("\n\n%s does not exist\n",FileName);
	return(status);
    }
}
int
acl_fileio(int *code, int *stream, 
	    struct dsc$descriptor *record, struct dsc$descriptor *rhb)
{
/*  status codes */
globalvalue ss$_normal;
globalvalue edt$_nonstdfil;
globalvalue rms$_eof;
globalvalue rms$_normal;
/*  Possible values for *code	*/
globalvalue edt$k_open_input;
#define					EDT$K_OPEN_INPUT		0
globalvalue edt$k_open_output_seq;
#define					EDT$K_OPEN_OUTPUT_SEQ		1
globalvalue edt$k_open_output_noseq;
#define					EDT$K_OPEN_OUTPUT_NOSEQ		2
globalvalue edt$k_open_in_out;
#define					EDT$K_OPEN_IN_OUT		3
globalvalue edt$k_get;
#define					EDT$K_GET			4
globalvalue edt$k_put;
#define					EDT$K_PUT			5
globalvalue edt$k_close_del;
#define					EDT$K_CLOSE_DEL			6
globalvalue edt$k_close;
#define					EDT$K_CLOSE			7
/*  Possible values for *stream	*/
globalvalue edt$k_command_file;
#define					EDT$K_COMMAND_FILE		0
globalvalue edt$k_input_file;
#define					EDT$K_INPUT_FILE		1
globalvalue edt$k_include_file;
#define					EDT$K_INCLUDE_FILE		2
globalvalue edt$k_journal_file;
#define					EDT$K_JOURNAL_FILE		3
globalvalue edt$k_output_file;
#define					EDT$K_OUTPUT_FILE		4
globalvalue edt$k_write_file;
#define					EDT$K_WRITE_FILE		5

#define Codes 8
static	int	codes[Codes] = {edt$k_open_input, edt$k_open_output_seq, 
			edt$k_open_output_noseq, edt$k_open_in_out, 
			edt$k_get, edt$k_put, edt$k_close_del, edt$k_close};
#define Streams 6
static	int	streams[Streams] = {edt$k_command_file, edt$k_input_file,
			    edt$k_include_file, edt$k_journal_file,
			    edt$k_output_file, edt$k_write_file};
	int	CodeNum, StreamNum;
	int	status, g, linktuple;
	char    *g$, *acex$, *acey$;
struct	item	*itmlst;
struct	acedef  *ace$;
static	char    *INacl_ptr, *acex, *acey;
static	int	acl_len, acl_nlen, itmcnt=0;
static	char    sidename[256], buf[512];
static struct
    dsc$descriptor_s 
		dsc$buf = { 0, DSC$K_DTYPE_T, DSC$K_CLASS_S, (char *)0 };

struct	Access
	{
	    struct Access *next$;
	    char    *ace;
	}   *aceA$, *aceL$;
    
    for (StreamNum=0; streams[StreamNum]!=*stream && StreamNum < Streams;
							StreamNum++);
    switch(StreamNum)
    {
    case EDT$K_INPUT_FILE:			    break;
    case EDT$K_OUTPUT_FILE:			    break;
    case EDT$K_JOURNAL_FILE:			    break;
    case EDT$K_COMMAND_FILE:			    break;
    default:	return(EDT$FILEIO(code, stream, record, rhb));
    }
    for (CodeNum=0; codes[CodeNum]!=*code && CodeNum < Codes ; CodeNum++);
    switch(CodeNum)
    {
    case EDT$K_OPEN_INPUT:
    case EDT$K_OPEN_IN_OUT:
	    if (StreamNum==EDT$K_COMMAND_FILE)
		return(EDT$FILEIO(code, stream, record, rhb));
	    if (StreamNum==EDT$K_JOURNAL_FILE)
		return(ss$_normal);
	    acl_fab.fab$l_fna = record->dsc$a_pointer;	/* Set up the file name */
	    acl_fab.fab$b_fns = record->dsc$w_length;   /* for RMS to open */
	    acl_fab.fab$l_dna = rhb->dsc$a_pointer;
	    acl_fab.fab$b_dns = rhb->dsc$w_length;
	    acl_fab.fab$l_nam = &acl_nam;
	    acl_nam.nam$l_rsa = (char *)malloc(sizeof(char)*255);
	    acl_nam.nam$b_rss = 255;
	    status = sys$open(&acl_fab, 0, 0);	/* Open the file */
	    if (!(status&1)) {			/* Signal $OPEN errors */
		lib$signal(acl_fab.fab$l_sts, acl_fab.fab$l_stv);
		return(ss$_normal);
	    }
	    acl_fab.fab$l_ctx = CTX_OPEN;	    /* Document OPEN */
	    if (strlen(lookaside)) {
		char *cp;
		*(acl_nam.nam$l_rsa+acl_nam.nam$b_rsl) = '\0';
		strcpy(sidename, acl_nam.nam$l_rsa);
		if (cp=strchr(sidename, ']'))
		    *cp = '\0';
		else
		    goto lookaside_setup;
		strcat(sidename, ".");
		strcat(sidename, lookaside); 
		strcat(sidename, "]");
		strcat(sidename, strchr(acl_nam.nam$l_rsa, ']')+1);
		if (cp=strchr(sidename, ';'))
		    *cp = '\0';
		else
		    goto lookaside_setup;
		lks_fab.fab$l_fna = sidename;
		lks_fab.fab$b_fns = strlen(sidename);   
		status = sys$open(&lks_fab, 0, 0);	/* Open the lookaside file */
		if (status&1) {
		    lks_fab.fab$l_ctx = CTX_OPEN;	/* Lookaside Open */
		    lks_rab.rab$l_fab = &lks_fab;
		    status = sys$connect(&lks_rab);
		    if (status&1)
			lks_rab.rab$b_rac = RAB$C_SEQ;
		}
	    }
    lookaside_setup:
	    if (DEBUG)	printf("\tDebug EDT$K_OPEN: %s\n",
		    (CodeNum==EDT$K_OPEN_INPUT)?"EDT$K_OPEN_INPUT"
				    :"EDT$K_OPEN_IN_OUT");
	    return(ss$_normal);

    case EDT$K_OPEN_OUTPUT_SEQ:
    case EDT$K_OPEN_OUTPUT_NOSEQ:
		/*  Since we don't actually output to a file... */
	    if (StreamNum==EDT$K_COMMAND_FILE)
		return(EDT$FILEIO(code, stream, record, rhb));
	    return(ss$_normal);

    case EDT$K_GET:
	if (StreamNum==EDT$K_JOURNAL_FILE)
	    return(rms$_eof);
	if (StreamNum==EDT$K_COMMAND_FILE) {
	    if (interactive)
		return(EDT$FILEIO(code, stream, record, rhb));
	    sprintf(buf,"exit");
	    dsc$buf.dsc$w_length = strlen(buf);
	    dsc$buf.dsc$a_pointer = buf;
	    str$copy_dx(record, &dsc$buf);
	    if (DEBUG)
		printf("\tDebug EDT$K_GET(EDT$K_COMMAND_FILE): %s\n",buf);
	    return(ss$_normal);
	}
	if (clear_buf) {
	    clear_buf = 0;
	    lks_fab.fab$l_ctx = CTX_EOF_ACL;
	    if (DEBUG)
		printf("\tDebug EDT$K_GET: skipping lookaside file, ACLs\n");
	}
	switch(lks_fab.fab$l_ctx)   /* Test lookaside context */
	{
	case CTX_OPEN:		/* Supply commentary header for the lookaside file */
	    lks_fab.fab$l_ctx = CTX_MORE_LKS;	    /* Continue lookaside */
	    sprintf(buf,"#from lookaside file %s #",sidename);
	    dsc$buf.dsc$w_length = strlen(buf);
	    dsc$buf.dsc$a_pointer = buf;
	    str$copy_dx(record, &dsc$buf);
	    if (DEBUG)	printf("\tDebug EDT$K_GET: %s\n",buf);
	    return(ss$_normal);
	case CTX_MORE_LKS:	/* Continue reading the lookaside file */
	    lks_rab.rab$l_ubf = buf;
	    lks_rab.rab$w_usz = sizeof(buf);
	    if ((status = sys$get(&lks_rab))==rms$_normal) {
		dsc$buf.dsc$w_length = lks_rab.rab$w_rsz;
		dsc$buf.dsc$a_pointer = lks_rab.rab$l_ubf;
		str$copy_dx(record, &dsc$buf);
		if (DEBUG)
		    printf("\tDebug EDT$K_GET: %.*s\n",lks_rab.rab$w_rsz,
						lks_rab.rab$l_ubf);
		return(ss$_normal);
	    }
	    /*  Fall thru and do ACL setup *only* */
	case CTX_CLOSED:	/* No Lookaside file present */
	    lks_fab.fab$l_ctx = CTX_EOF_LKS;	    /* EOF on lookaside */
	/*
	    Now that we have exhausted lookaside entries in any lookaside
	    file, we check for ACL entries in the original file.
	    Connect the XAB to the FAB block, allocate some ACL buffer
	    and perform an initial $DISPLAY.
	*/
	    acl_fab.fab$l_xab = (char *) &acl_xab;
	    INacl_ptr = acl_xab.xab$l_aclbuf =
	    		(char *)malloc(sizeof(char)*ACL_BUFFER_SIZE);
	    acl_xab.xab$w_aclsiz = ACL_BUFFER_SIZE;
	    acl_fab.fab$l_ctx = CTX_MORE_LKS;
	    acl_xab.xab$l_aclctx = 0;
	    memset(acl_xab.xab$l_aclbuf, 0, ACL_BUFFER_SIZE);
	    status = sys$display(&acl_fab, 0, 0);
	    if (!(status&1)) {
		lks_fab.fab$l_ctx = CTX_EOF_ACL;    /* EOF on ACLs   */
		if (DEBUG)  printf("\tDebug EDT$K_GET: <EOF ACLs>\n");
		goto eof_acls;
	    }
	    sprintf(buf,"#Gopher ACE's from %s #",acl_nam.nam$l_rsa);
	    dsc$buf.dsc$w_length = strlen(buf);
	    dsc$buf.dsc$a_pointer = buf;
	    str$copy_dx(record, &dsc$buf);
	    if (DEBUG)	printf("\tDebug EDT$K_GET: %s\n",buf);
	    return(ss$_normal);
	case CTX_EOF_LKS:		/* Read in the ACL entries */
	/*  While we actually have an ACL and the ACL lookup was correct, we
	    run through the ACL list within the ACL buffer and format each ACE.
	    Since the buffer is relatively small, keep '$DISPLAY'ing the file 
	    until there is no more ACL entries to find.  The first $DISPLAY
	    was done when the file was $OPENed, the next $DISPLAY reads 
	    XAB$L_ACLCXT to get subsequent ACL entries for the buffer.
        */
	    while ((acl_xab.xab$w_acllen != 0)) {
		while((*INacl_ptr != 0) &&
		      ((INacl_ptr - (char *)acl_xab.xab$l_aclbuf) 
			    < ACL_BUFFER_SIZE)) {
			    /* The first byte is the size of the ACL */
		    ace$ = (struct acedef *)INacl_ptr;
		    INacl_ptr += *INacl_ptr;   /* Get the next ACL entry */
		    if ((ace$->ace$b_type==ACE$C_INFO) &&
			(strncmp((char *)&(ace$->ace$l_access),
				GOPHER_ACE,strlen(GOPHER_ACE))==0)) {
		    /* 
			This is one of our GOPHER Lookaside Application ACE's
		    */
			itmcnt++;
			if (acl_ptr==NULL) {
			    acex = acl_ptr =
			    	(char *)malloc(sizeof(char)*ACL_BUFFER_SIZE);
			    acl_len = ACL_BUFFER_SIZE;
			    memset(acl_ptr, 0, ACL_BUFFER_SIZE);
			}
			g = *((char *)ace$);
			if ((acex+g) > (acl_ptr+acl_len)) {
			    g = acex - acl_ptr;
			    acl_ptr = (char *)realloc(acl_ptr,
						acl_len+ACL_BUFFER_SIZE);
			    acl_len += ACL_BUFFER_SIZE;
			    acex = acl_ptr + g;
			    memset(acex, 0, (acl_len - (acex-acl_ptr)));
			    g = *((char *)ace$);
			}
			memcpy(acex, (char *)ace$, g);
			acex += g;
			g = ace$->ace$b_size - GOPHER_ACESZ;
			g$ = ((char *)&(ace$->ace$l_access)) 
					+ strlen(GOPHER_ACE);
			sprintf(buf,"%.*s",g, g$);
			dsc$buf.dsc$w_length = strlen(buf);
			dsc$buf.dsc$a_pointer = buf;
			str$copy_dx(record, &dsc$buf);
			if (DEBUG)  printf("\tDebug EDT$K_GET: %s\n",buf);
			return(ss$_normal);
		    }
		}
		memset(acl_xab.xab$l_aclbuf, 0, ACL_BUFFER_SIZE);
		status = sys$display(&acl_fab, 0, 0);/* Get the next ACL block */
		if (!(status&1)) break;
		if (!(acl_xab.xab$l_aclsts&1)) break;
	    }
	    acl_fab.fab$l_ctx = CTX_EOF_ACL;	/* EOF on ACL's    */
	    /*  Fall thru and do CMD setup *only* */
	case CTX_EOF_ACL:	/* Supply commentary header for command line args */
	eof_acls:
	    if (clt) {
		lks_fab.fab$l_ctx = CTX_MORE_CMD;   /* Continue cmd line args*/
		clt = 0;
		sprintf(buf,"#from command line #");
		dsc$buf.dsc$w_length = strlen(buf);
		dsc$buf.dsc$a_pointer = buf;
		str$copy_dx(record, &dsc$buf);
		if (DEBUG)  printf("\tDebug EDT$K_GET: %s\n",buf);
		return(ss$_normal);
	    }
	    else {
		if (DEBUG)  printf("\tDebug EDT$K_GET: <EOF>\n");
		return(rms$_eof);   /* No command line entries to append    */
	    }
	case CTX_MORE_CMD:	    /* Continue appending command line entries */
		for (; !argv_tuple[clt] && (clt < GS_TUPLE_CNT); clt++);
		if (clt < GS_TUPLE_CNT) {
		    dsc$buf.dsc$w_length = strlen(argv_tuple[clt]);
		    dsc$buf.dsc$a_pointer = argv_tuple[clt];
		    str$copy_dx(record, &dsc$buf);
		    if (DEBUG)
			printf("\tDebug EDT$K_GET: %s\n",argv_tuple[clt]);
		    clt++;
		    return(ss$_normal);	    /* Supply command line entry */
		}
		if (DEBUG)  printf("\tDebug EDT$K_GET: <EOF>\n");
		return(rms$_eof);	    /*	Finally at the end! */
	}
    case EDT$K_PUT:
	    if (StreamNum==EDT$K_COMMAND_FILE)
		return(EDT$FILEIO(code, stream, record, rhb));
	    if (StreamNum==EDT$K_JOURNAL_FILE)
		return(ss$_normal);
	    g = dsc$buf.dsc$w_length = sizeof(buf) - GOPHER_ACESZ - 1;
	    dsc$buf.dsc$a_pointer = buf + GOPHER_ACESZ;
	    str$copy_dx(&dsc$buf,record);
	    if (g > record->dsc$w_length)
		g = record->dsc$w_length;
	    buf[g + GOPHER_ACESZ] = '\0';
	    while (buf[strlen(buf)-1]==' ')
		buf[strlen(buf)-1] = '\0';
	    if (-1==(linktuple=LinkTuple(dsc$buf.dsc$a_pointer,NULL)))
		return(ss$_normal);	    /* Ignore non link-tuple elements */
	    if (DEBUG)	printf("\tDebug EDT$K_PUT: %s\n",buf);

	    /*
		This is a link tuple entry; now build an ACE from this line 
	    */
	    itmcnt++;
	    ace$ = (struct acedef *)buf;
	    ace$->ace$b_type = ACE$C_INFO;
	    ace$->ace$w_flags = 0;
	    memcpy((char *)&ace$->ace$l_access,GOPHER_ACE,strlen(GOPHER_ACE));
	    ace$->ace$b_size = strlen(dsc$buf.dsc$a_pointer) + GOPHER_ACESZ;
	    if (acl_new==NULL) {
		acey = acl_new = (char *)malloc(sizeof(char)*ACL_BUFFER_SIZE);
		acl_nlen = ACL_BUFFER_SIZE;
		memset(acl_new, 0, ACL_BUFFER_SIZE);
	    }
	    g = *((char *)ace$);
	    if ((acey+g+sizeof(char *)) > (acl_new+acl_nlen)) {
		g = acey - acl_new;
		acl_new = (char *)realloc(acl_new, acl_nlen+ACL_BUFFER_SIZE);
		acl_nlen += ACL_BUFFER_SIZE;
		acey = acl_new + g;
		memset(acey, 0, (acl_nlen -(acey-acl_new)));
		g = *((char *)ace$);
	    }
	    /* 
		Record this link tuple element from the edit buffer.
		If "ACCESS=", record into a linked list of multiple
		access elements; otherwise discard any earlier versions
		of the same element, and keep this one.
	    */
	    linktuple=LinkTuple(NULL,ace$);
	    if (linktuple==access_tuple) {
		aceA$ = (struct Access *)acey;
		aceA$->next$ = NULL;
		if (!aceA$->ace)
		    aceA$->ace = (char *)malloc(sizeof(char)*g);
		else if (strlen(aceA$->ace) < g)
		    realloc((char *)aceA$->ace, sizeof(char)*g);
		memcpy(aceA$->ace, (char *)ace$, g);
		if (g$ace[linktuple]==NULL)
		    g$ace[linktuple] = (char *)aceA$;
		else {
		    for (aceL$ = (struct Access *)g$ace[linktuple];
			    aceL$->next$; aceL$ = aceL$->next$);
			aceL$->next$ = aceA$;
		}
		acey += g + sizeof(struct Access *);
	    }
	    else {
		memcpy(acey, (char *)ace$, g);
		g$ace[linktuple] = acey;
		acey += g;
	    }
	    return(ss$_normal);
    case EDT$K_CLOSE_DEL:	    /* Close/delete -> Close	*/
    case EDT$K_CLOSE:
	    if (StreamNum==EDT$K_COMMAND_FILE)
		return(EDT$FILEIO(code, stream, record, rhb));
	    if (StreamNum == EDT$K_JOURNAL_FILE)
		return(ss$_normal);
	    if (StreamNum == EDT$K_OUTPUT_FILE) {
	    /*
		Build the SYS$CHANGE_ACL() item list.  First build Delete
		entry items for the old ACE's which are not still found
		in the edit buffer.  There will be at most itmcnt (+1) 
		item list entries.
	    */
		if (DEBUG) printf("\tDebug EDT$K_CLOSE: EDT$K_OUTPUT_FILE\n");

		itmlst = (struct item *)malloc(sizeof(struct item)
						*(itmcnt + 1));
		itmcnt=0;
		if (acl_ptr==NULL)
		    goto    done_delete_list;
		for (acex$ = acl_ptr;
			((*acex$ != 0) && ((acex$ - acl_ptr) < acl_len));
			    acex$ += *acex$) {
		/*
		    This entry might exactly match an entry from the edit 
		    buffer; if so, just keep the original and ignore the new
		    (but unchanged) entry; otherwise add the original ACE to 
		    the delete list; always add original ACE's which aren't
		    in the edit buffer anymore to the delete list.
		*/
			linktuple = LinkTuple(NULL, (struct acedef *)acex$);
			if ((linktuple==-1) || (g$ace[linktuple]==NULL))
			    goto delete_item;
			/* Easiest to just delete prior "ACCESS=" entries */
			if (linktuple==access_tuple)
			    goto delete_item;
			if (memcmp(acex$, g$ace[linktuple], *acex$)==0) {
			    /* Keep the original, ignore unchanged new one  */
			    g$ace[linktuple] = NULL;
			}
			else
			{
			    /*  Build Delete item			    */
			delete_item:
			    itmlst[itmcnt].code = ACL$C_DELACLENT;
			    itmlst[itmcnt].bufadr = acex$;
			    itmlst[itmcnt].buflen = *acex$;
			    itmcnt++;
			}
		}
		done_delete_list:
	    /*
		Now for each non-null g$ace[] entry, build an Add item.
	    */
		for (linktuple=0; linktuple < GS_TUPLE_CNT; linktuple++) {
		    if ((acex$ = g$ace[linktuple])!=NULL) {
			if (linktuple==access_tuple) {
			    for (aceL$ = (struct Access *)g$ace[linktuple];
				    aceL$; aceL$ = aceL$->next$) {
				/* Build "ACCESS=" Add item */
				acex$ = aceL$->ace;
				itmlst[itmcnt].code = ACL$C_ADDACLENT;
				itmlst[itmcnt].bufadr = acex$;
				itmlst[itmcnt].buflen = *acex$;
				itmcnt++;
			    }
			}
			else {
			    /* Build non-"ACCESS=" Add item */
			    itmlst[itmcnt].code = ACL$C_ADDACLENT;
			    itmlst[itmcnt].bufadr = acex$;
			    itmlst[itmcnt].buflen = *acex$;
			    itmcnt++;
			}
		    }
		}
		g = itmlst[itmcnt].buflen = itmlst[itmcnt].code = 0;
		itmcnt = ACL$C_FILE;
		dsc$buf.dsc$a_pointer = acl_nam.nam$l_rsa;
		dsc$buf.dsc$w_length = strlen(acl_nam.nam$l_rsa);
		status = SYS$CHANGE_ACL(0, &itmcnt, &dsc$buf, itmlst, 0, 0, &g);
		if (acl_ptr != NULL) /* Free bufs when closing output file */
		    free(acl_ptr);
		if (acl_new != NULL)
		    free(acl_new);
	    }
	    if (lks_fab.fab$l_ctx != -1) {
		sys$close(&lks_fab, 0, 0);
		lks_fab.fab$l_ctx = -1;	    /* User CTX=-1 Closed */
	    }
	    if (acl_xab.xab$l_aclbuf != NULL) {
		free(acl_xab.xab$l_aclbuf);
		acl_xab.xab$l_aclbuf = NULL;
		acl_xab.xab$w_aclsiz = 0;
	    }
	    if (acl_fab.fab$l_ctx != -1) {
		sys$close(&acl_fab, 0, 0);
		acl_fab.fab$l_ctx = -1;	    /* User CTX=-1 Closed */
	    }
	    return(ss$_normal);
    default:	return(EDT$FILEIO(code, stream, record, rhb));
    }
}

int
LinkTuple(char *t, struct acedef *ace)
{
    int	lt;

    if (ace != NULL)
	t = (char *)&ace->ace$l_access + strlen(GOPHER_ACE);
    for (lt=0; lt < GS_TUPLE_CNT; lt++)
	if (strncasecomp(t, keyword[lt], strlen(keyword[lt]))==0)
	    return(lt);
    return(-1);
}

/*
 * Copyright (c) 1987 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Modified for use on VMS by Earl Fogel, University of Saskatchewan
 * Computing Services, January 1992
 */

typedef unsigned char u_char;

/*
 * This array is designed for mapping upper and lower case letter
 * together for a case independent comparison.  The mappings are
 * based upon ascii character sequences.
 */
static const u_char charmap[] = {
	'\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
	'\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
	'\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
	'\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
	'\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
	'\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
	'\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
	'\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
	'\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
	'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
	'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
	'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
	'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
	'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
	'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
	'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
	'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
	'\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
	'\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
	'\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
	'\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
	'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
	'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
	'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
	'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

int
strncasecomp(s1, s2, n)
	const char *s1, *s2;
	register size_t n;
{
	if (n != 0) {
		register const u_char *cm = charmap,
				*us1 = (const u_char *)s1,
				*us2 = (const u_char *)s2;

		do {
			if (cm[*us1] != cm[*us2++])
				return (cm[*us1] - cm[*--us2]);
			if (*us1++ == '\0')
				break;
		} while (--n != 0);
	}
	return (0);
}
