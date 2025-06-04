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
#include <stdio.h>

#define str(x) x>>4, x&0xf
boolean cmd_shownode()
{
	cli_buf_t buf;
	ethernet_addr_t node_addr;
	node_entry * node_ptr;

	getclistring ("node",buf);
	if ( find_node(buf,&node_addr) ){
		printf ("Node %s: ethernet addr %X%X-%X%X-%X%X-%X%X-%X%X-%X%X\n",
			buf,
			str(node_addr.bytes[0]),
			str(node_addr.bytes[1]),
			str(node_addr.bytes[2]),
			str(node_addr.bytes[3]),
			str(node_addr.bytes[4]),
			str(node_addr.bytes[5]));
	}
	else
		emon$put_error("Node not found");

	return true;
}
