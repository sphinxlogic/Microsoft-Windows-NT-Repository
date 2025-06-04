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
#include "monitor.h"
#include "stat.h"
#include "$sdl:nmadef"

#include <assert.h>
#include <ctype.h>
#include <file.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#if defined (__STDC__) || defined (__DECC)
#include <stdlib.h>
#define VOID_PTR (void *)
#else
#define VOID_PTR
#endif

#define MAXWORD 18

typedef char word_t[MAXWORD+1];

static char line[80];
static int  linsiz;
static int  linidx;
static FILE * cfg;

static boolean built_done=false;
static boolean init_ok=true;

static jmp_buf error_buf;
static const char * errmsg;

static error (const char * mess)
{
	errmsg= mess;
	longjmp (error_buf, 1);
}

static open_cfg()
{
	cfg=fopen("emon$cfg", "r", "dna=.cfg","ctx=rec");
}

static cfg_error(message)
char * message;
{
	printf ("%s\n%*s%s\n",line,linidx+2,"! ",message);
	init_ok=false;
}

static skipspace()
{
	for (;isspace(line[linidx]);) linidx++;
	if (line[linidx]=='!') linsiz=linidx;
}

/*	nb of bytes in line, 0 if EOF, -1 if error */

static int read_cfg()
{
	char * cp;
	cp= fgets (line, sizeof line-1, cfg);

	if (cp==NULL) {
		if feof(cfg) return 0;
		else	return -1;
	}

	linsiz= strlen (cp);
	if (linsiz==sizeof line) {
		cfg_error ("line too long");
		read_cfg();
	}

	linidx=0;
	if (linsiz>0){
		char * cp;
		line[linsiz]=0;
		if (line[linsiz-1]=='\n') line[linsiz-1]=' ';
		skipspace();
		if (linsiz==0) {
			linsiz++;
			linidx++;
		}
	}
	return linsiz;
}

static getword(word)
word_t word;
{
	register char * cp= word;
	register int i;
static const char valid_chr []= 
"0123456789ABCDEFGHIJKILMNOPQRSTUVWXYZabcdefghijkilmnopqrstuvwxyz.-_$";
	skipspace();
	if (line[linidx]=='"')
		for (i= MAXWORD; (i--);){
			linidx++;
			if (line[linidx]==0){
				linidx+= MAXWORD-i;
				error ("Unterminated string");
			}
			if (line[linidx]!='"') *cp++= line[linidx];
			else {
				linidx++;
				if (line[linidx]!='"') break;
				else *cp++= line[linidx++];
			}
		}
	else		
		for (i=MAXWORD; i-- && (strchr(valid_chr, line[linidx])!=NULL);
		 	linidx++)
			*cp++= (isupper (line[linidx]))
					? _tolower(line[linidx])
					: line[linidx];

	if (i==MAXWORD-1) error("Word expected");
	if (isalnum(line[linidx])){
			linidx-= MAXWORD;
			error ("Word too long");
	}
	*cp=0;
}

static getnumber()
{
	register int i=0;

	skipspace();
	if (isdigit(line[linidx]))
		do
			i=10*i+(line[linidx++]-'0');
		while (isdigit(line[linidx]));
	else error ("Number expected");
	return i;
}

static getxbyte()
{
	int i;
	if ( isxdigit(line[linidx++]) && isxdigit(line[linidx++]) )
		sscanf (line+linidx-2,"%2x",&i);
	else {
		linidx--;
		error ("Hex byte expected");
	}
	return i;
}

static skipchar (c)
char c;
{
	if (line[linidx]==c) linidx++;
	else{
		static char error_txt[20];
		sprintf (error_txt,"'%c' expected",c);
		error (error_txt);
	}
}

static getkeyword(list)
word_t * list[];
{
	word_t word;
	register int i=0;

	getword(word);
	while ( (list[i]!=NULL) && (strcmp((char *) list[i],word)) ) i++;
	if (list[i]==NULL){
		static char error_txt[256];

		linidx-= strlen(word);
		sprintf (error_txt,"Expecting one of keywords %s",list[0]);
		i= 1;
		while (list[i]!=NULL){
			strcat (error_txt,",");
			strcat (error_txt, (char *) list[i++]);
		}
		error (error_txt);
	}
	return i;
}

static getparameter()
{
	static const char * parameter_entity[]=
			{
				"buffers",
				"priority",
				"io",
				"eko",
				"device",
				"format",
				"nopad",
				"802emode",
				NULL
			},
			* onoff[]={"on","off",0},
			* ethfmt[]= {"none","802e","ethernet","802"};
	int parfmt= 0;	/* avoid both format and 802emode */

assert (NMA$C_STATE_ON==0);
assert (NMA$C_STATE_OFF==1);
assert (NMA$C_LINFM_802E==0);
assert (NMA$C_LINFM_ETH==1);
assert (NMA$C_LINFM_802==2);
	switch (getkeyword(&parameter_entity)){
		case 0: parameters.buffers= getnumber(); break;
		case 1: {
			int priority= getnumber();
			if (priority>31) error ("Illegal priority");
			parameters.priority= priority;	
			}break;
		case 2: {
			int qionb= getnumber();
			if (qionb==0) error ("Illegal IO count");
			parameters.qionb= qionb;
			}break;
		case 3: parameters.eko= getkeyword (onoff); break;
		case 4: {
			word_t devnam;
			int devlen;
			getword (&devnam);
			devlen= strlen(devnam);
			if (devlen>sizeof parameters.device)
				error ("illegal device name");
			strncpy (parameters.device,
				 devnam,
				 sizeof parameters.device
				);
			parameters.dev_desc.dsc$w_length= devlen;
			break;
		case 5: parameters.fmt= getkeyword (ethfmt)-1; break;
		case 6: if (parfmt++) error ("'802emode' already specified");
			else 	parameters.nopad= getkeyword (onoff);
			break;
		case 7: if (parfmt++) error ("'format' already specified");
			else	if (getkeyword (onoff)) parameters.fmt=NMA$C_LINFM_802E;
			break;
		}
	}
}

static add_permanent_params()
{
	/* y a plus rien pour l'instant */
}

static alloc_mem()
{
	int circuit_size= circuit_nb*sizeof (circuit_entry);
	int source_size= source_nb*sizeof (source_entry);
	int node_size=	node_nb*sizeof (node_entry);
	int protocol_size= (protocol_nb+protocol802e_nb)*sizeof (protocol_entry);
	int protocol802e_size= protocol802e_nb*sizeof (protocol802e_entry);
	int area_size=	area_nb*(area_nb*sizeof(counter_t) +sizeof (area_entry));
	int map_size=   max_map* sizeof (map_entry);
	char * cp;
	cp= malloc (circuit_size+source_size+node_size+
		    protocol_size+area_size+map_size+protocol802e_size);
	if (cp==NULL) error ("Not enough memory");
	tabcircuit= VOID_PTR cp;
	cp+= circuit_size;
	tabsource= VOID_PTR cp;
	cp+= source_size;
	tabnode= VOID_PTR cp;
	cp+= node_size;
	tabprotocol= VOID_PTR cp;
	cp+= protocol_size;
	tabarea= VOID_PTR cp;
	cp+= area_nb*sizeof (area_entry);
	tabareatraf= VOID_PTR cp;
	cp+= area_size;
	tabmap= VOID_PTR cp;
	cp+= map_size;
	tabprotocol802e= VOID_PTR cp;
	add_permanent_params();
	built_done= true;
}

static buildtable()
{
	static const char * const table_entity[]=
			{"circuit",
			 "transport",
			 "system",
			 "protocol",
			 "area",
			 "address",
			 "map",
			 "802eprotocol",
			 NULL};

	if istrue(built_done) error ("Statement out of order");
	switch (getkeyword(table_entity)){
		case 0: circuit_nb=getnumber(); break;
		case 1: source_nb= getnumber(); break;
		case 2: node_nb=   getnumber(); break;
		case 3: protocol_nb= getnumber(); break;
		case 4: area_nb=   1+getnumber(); break;
		case 5: max_addr= 1+getnumber(); break;
		case 6: max_map=  1+getnumber(); break;
		case 7: { int i=getnumber();
			  if  (i<=1500) protocol802e_nb= i;
			       else error("802e protocol nb must be <1500");
			  break;
			}
	}
}

static buildarea()
{
	ulong	area_num= getnumber();
	word_t	area_name;

	if (area_num>=area_nb)	error ("area number out of range");
	getword(area_name);
	if (strlen (area_name)>sizeof (area_name_t))
				error ("area name too long");
	strncpy (tabarea[area_num].name,area_name,sizeof (area_name_t));
}

static buildnode()
{
	ethernet_addr_t phy_addr;
	word_t name;
	node_entry * np;

	if isfalse(built_done) alloc_mem();
	phy_addr.decnet.header= DECNET_HEADER;
	phy_addr.decnet.area=getnumber();
	skipchar ('.');
	phy_addr.decnet.node= getnumber();
	getword(name);
	if (strlen(name)>6) {
		linidx-= strlen(name);
		error ("Decnet node name too long");
	}
	np= addnode (&phy_addr);
	if (np==0) error ("Cannot add new decnet node");
	np->new= false;
	strncpy (np->name,name,sizeof np->name);
}

getdecnet()
{
	static const char * const decnet_entity[]=
			{"area",
			 "node",
			 NULL};

	switch (getkeyword(decnet_entity)){
	case 0:	buildarea(); break;
	case 1: buildnode(); break;
	}
}

static gethardware()
{
	ethernet_addr_t phy_addr;
	word_t name;
	node_entry * np;
	int i;

	if isfalse(built_done) alloc_mem();
	skipspace();
	phy_addr.bytes[0]= getxbyte();
	for (i=0; ++i<6;){
		skipchar('-');
		phy_addr.bytes[i]= getxbyte();
	}
	getword(name);
	np= addnode (&phy_addr);
	if (np==NULL) error ("Cannot add new hardware node");
	if (np->name[0])
		error ("Duplicate hardware address");
	else{	strncpy (np->name,name,sizeof np->name);
		np->new= false;
	}
}

static getprotocol()
{
	ethernet_protocol_t protocol;
	word_t name;
	protocol_entry * pp;

	if isfalse(built_done) alloc_mem();
	skipspace();
	protocol.bytes[0]= getxbyte();
	skipchar ('-');
	protocol.bytes[1]= getxbyte();
	if ( (256*protocol.bytes[0]+protocol.bytes[1]) <= 1500)
			error ("Invalid ethernet protocol value");
	getword(name);
	pp= addprotocol (&protocol);
	if (pp==NULL) 	error("Cannot add new protocol entry");
	strncpy (pp->name,name,sizeof pp->name);
	pp->new= false;
}

static get802protocol()
{
	packet_header_t tag;
	word_t name;
	protocol_entry * pp;
	int	i;

	if isfalse(built_done) alloc_mem();
	skipspace();

	tag.protocol.bytes[0]= getxbyte();
	tag.protocol.bytes[1]= tag.protocol.bytes[0];

	if (tag.hdr802.dsap==0xAA) error ("SNAP sap used by 802E");

	getword(name);
	if isfalse(addprotocol802 (&tag))
				error ("Cannot add new 802 procotol entry");
	/* ici on a la valeur allouee. on recupere alors le pointeur vers */
	/* la zone protocole ethernet */
	pp= addprotocol (&tag.protocol);
	strncpy (pp->name, name, sizeof pp->name);
	pp->new= false;
}

static get802eprotocol()
{
	packet_header_t tag;
	word_t name;
	protocol_entry * pp;
	int	i;

	if isfalse(built_done) alloc_mem();
	skipspace();

	tag.hdr802e.pid[0]= getxbyte();
	for (i=1; i<5; i++) {
		skipchar ('-');
		tag.hdr802e.pid[i]= getxbyte();
	}

		/* ce sont des protocoles ethernet */

	if ( (tag.hdr802e.pid[0]==0) && (tag.hdr802e.pid[1]==0) && (tag.hdr802e.pid[2]==0))
		error ("Invalid 802E protocol value");

	getword(name);
	tag.protocol.word= 0xAAAA;
	tag.ctlb=3;	/* 2 champs necessaires pour 802e */
	if isfalse(addprotocol802 (&tag))
		error ("Cannot add new 802E procotol entry");
	/* ici on a la valeur allouee. on recupere alors le pointeur vers */
	/* la zone protocole ethernet */
	pp= addprotocol (&tag.protocol);
	strncpy (pp->name, name, sizeof pp->name);
	pp->new= false;
}

static getmap()
{
	static const char * const map_entity[]=
			{"number",
			 "node",
			 NULL},
			*name_entity[]=
			{"name",
			NULL};
	ushort map_num;
	word_t map_name, node_name;
	ethernet_addr_t node_addr;
	node_entry *np;

	if isfalse(built_done) alloc_mem();
	switch (getkeyword(map_entity)){
	case 0:
		map_num=getnumber();
		if (map_num>=max_map) error ("Invalid map #");
		getkeyword(name_entity);
		getword(map_name);
		strncpy (tabmap[map_num].name,map_name,sizeof (map_name_t));
		break;
	case 1:
		getword (node_name);
		map_num= getnumber ();
		if (map_num>=max_map) error ("Invalid map #");
		np= find_node (node_name, &node_addr);
		if (np!=NULL) np->map_num= map_num;
		else error ("Unknown node");
	}
}

static void
getignore()
{
static const char * const ignore_types[]= 
			{"protocol",
			 NULL},
		  * const ignore_what[]= 
			{"overflow",
			 NULL};

	switch(getkeyword(ignore_types)) {
	case 0: ignore.protocol.overflow= true;
		getkeyword (ignore_what);
	}
}

boolean
init_stat()
{
	open_cfg();
	if (cfg!=NULL){
		while (read_cfg()>0){
			if (setjmp(error_buf)) cfg_error(errmsg);
			else{
				static const char * const dispatch_line[]=
					{"maximum",
					 "decnet",
					 "hardware",
					 "protocol",
					 "parameter",
					 "map",
					 "802protocol",
					 "802eprotocol",
					 "ignore",
					 NULL};
	
				if (linidx!=linsiz){
					switch (getkeyword (dispatch_line)){
						case 0: buildtable(); break;
						case 1: getdecnet(); break;
						case 2: gethardware(); break;
						case 3: getprotocol(); break;
						case 4: getparameter(); break;
						case 5: getmap(); break;
						case 6: get802protocol(); break;
						case 7: get802eprotocol(); break;
						case 8: getignore(); break;
					}
					skipspace();
					if (linidx!=linsiz) cfg_error ("rest of line ignored");
				}
			}
		}
		if (linsiz<-1) perror ("read config");
		fclose (cfg);
	}
	else perror ("Open config");
	if isfalse(built_done) alloc_mem();
	return init_ok;
}
