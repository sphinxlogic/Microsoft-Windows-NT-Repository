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
#define LOC_STAT_INITCP   020 /* initialize all fields in entry for called rtn*/
#define LOC_STAT_BOTHWAYS 040 /* bidirectionnal selection */
#define	LOC_STAT_ONEWAY	  0100 /* monodirectional check */
#define LOC_STAT_XTEND	  0200 /* use pass mechanism */

node_entry * addnode (ethernet_addr_t * phy_addr);
source_entry * addsource (ethernet_addr_t * addr, ethernet_protocol_t * proto);
protocol_entry * addprotocol (ethernet_protocol_t * proto);
boolean addprotocol802 (packet_header_t * packet);

int init_loc_stat (node_entry * source, node_entry * dest,
			protocol_entry * proto,
			boolean (* call)(),
			ulong opt);
boolean loc_stat(pckt_t *pp);
boolean glob_stat(pckt_t *pp);
boolean	upd_stat(void);

void statfr_reset(void);
void statfr_apply(void);
void statfr_disable_protocols(void);
void statfr_enable_protocols(void);
void statfr_enable_unknown_protocols(void);
void statfr_disable_srcs(void);
void statfr_enable_srcs(void);
void statfr_enable_unknown_srcs(void);
void statfr_disable_dsts(void);
void statfr_enable_dsts(void);
void statfr_enable_unknown_dsts(void);
void statfr_disable_ends(void);
void statfr_enable_ends(void);
void statfr_enable_unknown_ends(void);
void print_stat(char *);

void stat_reset_circuit_database(void);

node_name_t * 
	print_ethernet_addr (node_entry *np, node_name_t * buf);
protocol_name_t * 
	print_ethernet_protocol (protocol_entry *np, protocol_name_t * buf);
