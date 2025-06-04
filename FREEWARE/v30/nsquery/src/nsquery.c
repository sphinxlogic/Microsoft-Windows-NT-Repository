/*
**++
**  FACILITY:	NSQUERY
**
**  ABSTRACT:	The NSQUERY program.
**
**  MODULE DESCRIPTION:
**
**  	This module contains the NSQUERY main program and some supporting
**  routines.
**
**  	NSQUERY is a program that interrogates domain name servers.  It
**  formats a query, sends it to a server (either via UDP or TCP), and
**  decodes and prints out the result.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  25-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	25-SEP-1992 X3.0    Madison 	Initial coding in C.
**  	28-SEP-1992 V3.0    Madison 	Separate out network dependencies.
**  	23-NOV-1992 V3.0-1  Madison 	Fix WKS output.
**  	11-OCT-1993 V3.0-2  Madison 	Slight cleanup, fix for pre-V5.2.
**--
*/
#define NSQ_VERSION 	"V3.0-2"
#define NSQ_COPYRIGHT	"Copyright © 1993, MadGoat Software.  All Rights Reserved."

#ifdef __DECC
#pragma module NSQUERY NSQ_VERSION
#else
#module NSQUERY NSQ_VERSION
#endif

#include "nsquery.h"
#include "ipns_const.h"

#ifdef __DECC
#pragma extern_model save
#pragma extern_model common_block shr
#endif
    char $$$Copyright[]   = NSQ_COPYRIGHT;
#ifdef __DECC
#pragma extern_model restore
#endif

/*
** Forward declarations
*/
    unsigned int main(void);
    static void Show_RR(struct dsc$descriptor_s *, int, unsigned char **);
    static void name_unpack(unsigned char **, char *, int);
    static void check_inverse_query(char *);

/*
** External references
*/
    extern void Print(char *, ...);
    extern unsigned int cli_present(char *);
    extern unsigned int cli_get_value(char *, char *, int);
    extern unsigned int get_logical(char *, char *, int);
    extern struct dsc$descriptor *address_to_string(unsigned int);
    extern unsigned int network_init(void *);
    extern unsigned int network_open(void *, unsigned int *, int, int);
    extern unsigned int network_close(void *);
    extern unsigned int network_send(void *, void *, int);
    extern void         network_lookup(char *, struct QUE *);
    extern void         network_local_hostname(char *, int);
    extern unsigned int network_response(void *, void *, int, short *, TIME *);

/*
** Message codes
*/
#pragma nostandard
    globalvalue unsigned int CLI$_NEGATED, CLI$_PRESENT;
    globalvalue unsigned int NSQ__TRYNS, NSQ__NSOPNERR, NSQ__NOCONTACT,
    	NSQ__UNKRC, NSQ__HDRERR, NSQ__IDENTMISMATCH, NSQ__RCVQUERY,
    	NSQ__MSGTRUNC, NSQ__AUTHRESP, NSQ__SMALLBYTLM, NSQ__SMALLMAXBUF,
    	NSQ__NOALLOC, NSQ__NORECURSION, NSQ__SRVNAMERR;
#pragma standard

/*
** External data references
*/
    extern unsigned int nsq_cld();
    extern char *cls_name[], *pro_name[], *wks_name[];
    extern int cls_name_count, pro_name_count, wks_name_count;
    extern unsigned int rc_msg[];
    extern char *type_name[], *class_input[];
    extern int rc_msg_count, type_name_count, class_input_count;
    extern int type_value[], class_value[];

/*
** Header used in DNS queries and responses
*/
    union NSMSG {
    	unsigned int lw;
    	struct {
    	    short id;
    	    int recursion_desired	: 1;
    	    int truncation	    	: 1;
    	    int authoritative   	: 1;
    	    int opcode  	    	: 4;
    	    int response	    	: 1;
    	    int rcode   	    	: 4;
    	    int unused  	    	: 3;
    	    int recursion_avail 	: 1;
    	} hdr;
    };

/*
** Local data
*/
    static unsigned char nsbuf[4096];	/* query/response buffer */

/*
**++
**  ROUTINE:	main
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Main program.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	main()
**
**  IMPLICIT INPUTS:	See above.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	See code.
**
**  SIDE EFFECTS:   	See above.
**
**--
*/
unsigned int main() {

    char cmdline[STRING_SIZE], tmp[STRING_SIZE];
    char qhost[STRING_SIZE], nameserver[STRING_SIZE];
    unsigned char *nsbase, *nsp;
    char *cp, *anchor;
    union NSMSG hdr, rcvhdr;
    struct dsc$descriptor sdsc;
    struct ADR *adr;
    struct QUE adrque;
    TIME qident, receive_timeout;
    short len, nslen, qdcount, ancount, nscount, arcount;
    int i, port, use_tcp;
    unsigned int status, netctx;

/*
** Fetch and parse the command line
*/
    strcpy(cmdline, "NSQUERY ");
    INIT_SDESC(sdsc, sizeof(cmdline)-9, cmdline+8);
    status = lib$get_foreign(&sdsc, 0, &len);
    if (!OK(status)) return status;
    INIT_SDESC(sdsc, len+8, cmdline);
    status = cli$dcl_parse(&sdsc, nsq_cld, lib$get_input, lib$get_input);
    if (!OK(status)) return status;

/*
** Get the name we're asking about
*/
    cli_get_value("QNAME", qhost, sizeof(qhost));
    check_inverse_query(qhost);

/*
** Get the name server we're to ask.
*/
    nameserver[0] = '\0';
    if (cli_present("SERVER") == CLI$_PRESENT) {
    	cli_get_value("SERVER", nameserver, sizeof(nameserver));
    }
    if (!nameserver[0]) {
    	status = get_logical("NSQ_SERVER", nameserver, sizeof(nameserver));
    	if (!OK(status)) {
    	    static $DESCRIPTOR(prompt,"_Server: ");
    	    INIT_SDESC(sdsc, sizeof(nameserver)-1, nameserver);
    	    status = lib$get_input(&sdsc, &prompt, &len);
    	    if (!OK(status) || len == 0) {
    	    	return SS$_NORMAL;
    	    } else {
    	    	nameserver[len] = '\0';
    	    }
    	}
    }

/*
** Now translate the name server string into its address(es).
*/
    network_local_hostname(tmp, sizeof(tmp));
    adrque.head = adrque.tail = &adrque;
    network_lookup(nameserver, &adrque);

/*
** Time stamp used as an identifier
*/
    sys$gettim(&qident);

/*
** Set up the buffer pointers.  Reserve the first two bytes for a
** length value in case we're using TCP.
*/
    nsp = nsbase = nsbuf+sizeof(short);
    
/*
** Set up the header...
*/
    hdr.lw = 0;
    hdr.hdr.id = qident.long1 & 0xffff;
    hdr.hdr.recursion_desired = cli_present("RECURSIVE") == CLI$_PRESENT;

/*
** We use a bigger timeout on UDP-based queries if the user requests
** recursion.
*/
    {
    	static $DESCRIPTOR(bigtime,"0 00:00:30");
    	static $DESCRIPTOR(littletime, "0 00:00:15");
    	sys$bintim(hdr.hdr.recursion_desired ? &bigtime : &littletime,
    	    	&receive_timeout);
    }

/*
** Start filling the buffer.  Add the header, counts.
*/
    LONG_PACK(hdr.lw, nsp);
    qdcount = 1;
    BYTE_SWAP_PACK(qdcount, nsp);
    ancount = nscount = arcount = 0;
    BYTE_SWAP_PACK(ancount, nsp);
    BYTE_SWAP_PACK(nscount, nsp);
    BYTE_SWAP_PACK(arcount, nsp);

/*
** Stuff in the query host name
*/

    if (*(qhost+strlen(qhost)-1) != '.') strcat(qhost, ".");
    anchor = qhost;
    while (*anchor) {
    	cp = strchr(anchor, '.');
    	if (cp-anchor > 0) strncpy(tmp, anchor, cp-anchor);
    	tmp[cp-anchor] = '\0';
    	ASCIC_PACK(tmp, nsp);
    	anchor = cp + 1;
    }
    *nsp++ = '\0';

/*
** Get the query type and store in the query buffer
*/

    if (cli_present("TYPE") == CLI$_PRESENT) {
    	cli_get_value("TYPE", tmp, sizeof(tmp));
    } else {
    	strcpy(tmp, "ALL");
    }
    for (i = 0; i < type_name_count; i++) {
    	if (strstr(type_name[i], tmp) == type_name[i]) {
    	    WORD_PACK(type_value[i], nsp);
    	    break;
    	}
    }

/*
** Now for the class
*/

    if (cli_present("CLASS") == CLI$_PRESENT) {
    	cli_get_value("CLASS", tmp, sizeof(tmp));
    } else {
    	strcpy(tmp, "INTERNET");
    }
    for (i = 0; i < class_input_count; i++) {
    	if (strstr(class_input[i], tmp) == class_input[i]) {
    	    WORD_PACK(class_value[i], nsp);
    	    break;
    	}
    }

/*
** Determine the port to which we direct the request (usually 53)
*/

    if (cli_present("PORT") == CLI$_PRESENT) {
    	cli_get_value("PORT", tmp, sizeof(tmp));
    	lib$cvt_dtb(strlen(tmp), tmp, port);
    } else {
    	port = 53;
    }

/*
** Which protocol to use (typically want UDP)
*/

    if (cli_present("PROTOCOL") == CLI$_PRESENT) {
    	cli_get_value("PROTOCOL", tmp, sizeof(tmp));
    	upcase(tmp);
    	use_tcp = strcmp(tmp, "TCP") == 0;
    } else use_tcp = 0;

/*
** Now make the query
*/
    netctx = 0;
    status = network_init(&netctx);
    if (!OK(status)) lib$stop(status);
    while (REMQUE(adrque.head, &adr)) {
    	lib$signal(NSQ__TRYNS, 1, address_to_string(adr->address));
    	status = network_open(&netctx, &(adr->address), port, use_tcp);
    	free(adr);
/*
** For TCP-based queries, we stick the length of the query in the
** first two bytes of the query buffer (in network order)
*/
    	if (use_tcp) {
    	    len = nsp-nsbase;
    	    nsbuf[0] = (len&0xff00)>>8;
    	    nsbuf[1] = len&0x00ff;
    	    nsbase = nsbuf;
    	}
/*
** Send the query
*/
    	if (OK(status)) status = network_send(&netctx, nsbase, nsp-nsbase);
/*
** Get the response
*/
    	if (OK(status)) {
    	    status = network_response(&netctx, nsbuf, sizeof(nsbuf),
    	    	    	    &nslen, &receive_timeout);
    	}
    	if (OK(status)) break;
    	lib$signal(NSQ__NSOPNERR, 0, status);
    }

/*
** All done with the network.  If we couldn't contact any of the
** server's addresses, let the user know.
*/
    network_close(&netctx);
    if (!OK(status) || nslen == 0) {
    	lib$stop(NSQ__NOCONTACT, 0);
    }

/*
** We have a response... decode it
*/
    nsp = nsbuf+sizeof(short);
    LONG_UNPACK(nsp,rcvhdr.lw);
    if (rcvhdr.hdr.id != hdr.hdr.id) {
    	lib$stop(NSQ__HDRERR, 0, NSQ__IDENTMISMATCH, 2, hdr.hdr.id,
    	    rcvhdr.hdr.id);
    }

    if (!rcvhdr.hdr.response) {
    	lib$stop(NSQ__HDRERR, 0, NSQ__RCVQUERY, 0);
    }

    if (rcvhdr.hdr.truncation) {
    	lib$signal(NSQ__MSGTRUNC, 0);
    }
    if (rcvhdr.hdr.authoritative) {
    	lib$signal(NSQ__AUTHRESP, 0);
    }
    if (hdr.hdr.recursion_desired && !rcvhdr.hdr.recursion_avail) {
    	lib$signal(NSQ__NORECURSION, 0);
    }
    if (rc_msg[rcvhdr.hdr.rcode] == NSQ__UNKRC) {
    	lib$signal(NSQ__UNKRC, 1, rcvhdr.hdr.rcode);
    } else {
    	lib$signal(rc_msg[rcvhdr.hdr.rcode], 0);
    }

    Print("");

    BYTE_SWAP_UNPACK(nsp,qdcount);
    BYTE_SWAP_UNPACK(nsp,ancount);
    BYTE_SWAP_UNPACK(nsp,nscount);
    BYTE_SWAP_UNPACK(nsp,arcount);

/*
** Display what we asked for
*/
    for (i = 1; i <= qdcount; i++) {
    	short qtword, qcword;
    	NAME_UNPACK(nsp, qhost);
    	BYTE_SWAP_UNPACK(nsp, qtword);
    	BYTE_SWAP_UNPACK(nsp, qcword);
    	Print("QUERY  #!UL: QName=!AD, QType=!UW, QClass=!UW",
    	    i, strlen(qhost), qhost, qtword, qcword);
    }

/*
** Now display the answers, the authority RR's, and any additional RR's
*/
    if (ancount > 0) {
    	static $DESCRIPTOR(hdr, "ANSWER");
    	Print("");
    	for (i = 1; i <= ancount; i++) Show_RR(&hdr, i, &nsp);
    }
    if (nscount > 0) {
    	static $DESCRIPTOR(hdr, "AUTHOR");
    	Print("");
    	for (i = 1; i <= nscount; i++) Show_RR(&hdr, i, &nsp);
    }
    if (arcount > 0) {
    	static $DESCRIPTOR(hdr, "ADDTNL");
    	Print("");
    	for (i = 1; i <= arcount; i++) Show_RR(&hdr, i, &nsp);
    }
    Print("");

    return SS$_NORMAL;

} /* nsquery */

/*
**++
**  ROUTINE:	Show_RR
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Unpacks and displays a resource record.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Show_RR(struct dsc$descriptor *hdr, int number, unsigned char **bufptr)
**
**  hdr:    char_string, read only, by descriptor
**  number: integer, read only, by value
**  bufptr: byte pointer, modify, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void Show_RR(struct dsc$descriptor_s *xhdr, int n, unsigned char **nspp) {

    unsigned char *nsp, *nsp_save;
    char name[STRING_SIZE], classname[STRING_SIZE];
    char name2[STRING_SIZE], name3[STRING_SIZE];
    struct dsc$descriptor cldsc, ndsc;
    short type, class, rdlength;
    int i, j;
    TIME ttl;

    nsp = *nspp;

    NAME_UNPACK(nsp, name);
    INIT_SDESC(ndsc, strlen(name), name);
    WORD_UNPACK(nsp, type);
    BYTE_SWAP_UNPACK(nsp, class);
    if (class >= cls_name_count) {
    	sprintf(classname, "%d", class);
    } else {
    	strcpy(classname, cls_name[class]);
    }
    INIT_SDESC(cldsc, strlen(classname), classname);
    TIME_UNPACK(nsp, ttl);
    BYTE_SWAP_UNPACK(nsp, rdlength);
    nsp_save = nsp;

    switch(type) {

    case IPNS_TYPE_A: {
    	unsigned int address;
    	LONG_UNPACK(nsp, address);
    	Print("!AS #!UL: !AS, A, !AS, !13%D, Addr=!AS", xhdr, n, &ndsc,
    	    &cldsc, &ttl, address_to_string(address));
    	break;
    }

    case IPNS_TYPE_NS: {
    	NAME_UNPACK(nsp, name2);
    	Print("!AS #!UL: !AS, NS, !AS, !13%D, NS=!AD", xhdr, n, &ndsc,
    	    &cldsc, &ttl, strlen(name2), name2);
    	break;
    }

    case IPNS_TYPE_CNAME: {
    	NAME_UNPACK(nsp, name2);
    	Print("!AS #!UL: !AS, CNAME, !AS, !13%D, CName=!AD", xhdr, n,
    	    &ndsc, &cldsc, &ttl, strlen(name2), name2);
    	break;
    }

    case IPNS_TYPE_PTR: {
    	NAME_UNPACK(nsp, name2);
    	Print("!AS #!UL: !AS, PTR, !AS, !13%D, Ptr=!AD", xhdr, n, &ndsc,
    	    &cldsc, &ttl, strlen(name2), name2);
    	break;
    }

    case IPNS_TYPE_HINFO: {
    	ASCIC_UNPACK(nsp, name2);
    	ASCIC_UNPACK(nsp, name3);
    	Print("!AS #!UL: !AS, HINFO, !AS, !13%D, CPU=!AD, OS=!AD", xhdr, n,
    	    &ndsc, &cldsc, &ttl, strlen(name2), name2, strlen(name3), name3);
    	break;
    }

    case IPNS_TYPE_MX: {
    	short pref;
    	BYTE_SWAP_UNPACK(nsp, pref);
    	NAME_UNPACK(nsp, name2);
    	Print("!AS #!UL: !AS, MX, !AS, !13%D, Pref=!UW, Exch=!AD", xhdr, n,
    	    &ndsc, &cldsc, &ttl, pref, strlen(name2), name2);
    	break;
    }

    case IPNS_TYPE_MF: {
    	NAME_UNPACK(nsp, name2);
    	Print("!AS #!UL: !AS, MF, !AS, !13%D, Agent=!AD", xhdr, n, &ndsc,
    	    &cldsc, &ttl, strlen(name2), name2);
    	break;
    }

    case IPNS_TYPE_MB: {
    	NAME_UNPACK(nsp, name2);
    	Print("!AS #!UL: !AS, MB, !AS, !13%D, BoxHost=!AD", xhdr, n, &ndsc,
    	    &cldsc, &ttl, strlen(name2), name2);
    	break;
    }

    case IPNS_TYPE_MG: {
    	NAME_UNPACK(nsp, name2);
    	Print("!AS #!UL: !AS, MG, !AS, !13%D, GrpHost=!AD", xhdr, n, &ndsc,
    	    &cldsc, &ttl, strlen(name2), name2);
    	break;
    }

    case IPNS_TYPE_MR: {
    	NAME_UNPACK(nsp, name2);
    	Print("!AS #!UL: !AS, MR, !AS, !13%D, NewName=!AD", xhdr, n, &ndsc,
    	    &cldsc, &ttl, strlen(name2), name2);
    	break;
    }

    case IPNS_TYPE_MINFO: {
    	NAME_UNPACK(nsp, name2);
    	NAME_UNPACK(nsp, name3);
    	Print("!AS #!UL: !AS, MINFO, !AS, !13%D, Resp=!AD, Errs=!AD", xhdr, n, &ndsc,
    	    &cldsc, &ttl, strlen(name2), name2, strlen(name3), name3);
    	break;
    }

    case IPNS_TYPE_TXT: {
    	int len;
    	short i;
    	Print("!AS #!UL: !AS, TXT, !AS, !13%D", xhdr, n, &ndsc, &cldsc, &ttl);
    	len = rdlength;
    	while (len > 0) {
    	    ASCICN_UNPACK(nsp, name2, i);
    	    Print("    !AD", i, name2);
    	    len -= i+1;
    	}
    	break;
    }
    case IPNS_TYPE_SOA: {
    	int serial;
    	TIME refresh, retry, expire, minimum;
    	NAME_UNPACK(nsp, name2);
    	NAME_UNPACK(nsp, name3);
    	Print("!AS #!UL: !AS, SOA, !AS, !13%D", xhdr, n, &ndsc, &cldsc, &ttl);
    	Print("    Primary NS: !AD!/    Responsible: !AD",
    	    strlen(name2), name2, strlen(name3), name3);
    	LONG_REVERSE_UNPACK(nsp, serial);
    	TIME_UNPACK(nsp, refresh);
    	TIME_UNPACK(nsp, retry);
    	TIME_UNPACK(nsp, expire);
    	TIME_UNPACK(nsp, minimum);
    	Print("    Serial=!UL, Refresh=!13%D, Retry=!13%D", serial,
    	    	    &refresh, &retry);
    	Print("    Expire=!13%D, Minimum=!13%D", &expire, &minimum);
    	break;
    }

    case IPNS_TYPE_WKS: {
    	char wkname[STRING_SIZE], tmp[STRING_SIZE];
    	char proname[STRING_SIZE], *cp;
    	int address, mapsize, wksn;
    	unsigned char proto, wksbyte;

    	LONG_UNPACK(nsp, address);
    	Print("!AS #!UL: !AS, WKS, !AS, !13%D, Addr=!AS", xhdr, n, &ndsc,
    	    &cldsc, &ttl, address_to_string(address));
    	BYTE_UNPACK(nsp, proto);
    	if (proto >= pro_name_count) {
    	    sprintf(proname,"%d",proto);
    	} else {
    	    strcpy(proname, pro_name[proto]);
    	}
    	sprintf(tmp, "  Protocol %s: /", proname);
    	cp = tmp+strlen(tmp);
    	mapsize = rdlength-5;
    	for (i = 1; i <= mapsize; i++) {
    	    BYTE_UNPACK(nsp,wksbyte);
    	    for (j = 0; j < 8; j++) {
    	    	if (wksbyte&(1<<j)) {
    	    	    wksn = i * 8 - j - 1;
    	    	    if (wksn >= wks_name_count) {
    	    	    	if (wksn == 243) {
    	    	    	    strcpy(wkname, "SUR-MEAS");
    	    	    	} else if (wksn == 245) {
    	    	    	    strcpy(wkname, "LINK");
    	    	    	} else {
    	    	    	    sprintf(wkname, "%d", wksn);
    	    	    	}
    	    	    } else {
    	    	    	strcpy(wkname, wks_name[wksn]);
    	    	    }
    	    	    if (cp-tmp+strlen(wkname) > 75) {
    	    	    	*cp = '\0';
    	    	    	Print(tmp);
    	    	    	sprintf(tmp, "    /%s/", wkname);
    	    	    	cp = tmp+strlen(tmp);
    	    	    } else {
    	    	    	strcpy(cp, wkname);
    	    	    	cp += strlen(wkname);
    	    	    	*cp++ = '/';
    	    	    }
    	    	}
    	    }
    	}
    	if (cp != tmp) {
    	    *cp = '\0';
    	    Print(tmp);
    	}
    	break;
    }

    default: {
    	Print("!AS #!UL: !AS, Type=!UW, !AS, !13%D (unknown type)",
    	    xhdr, n, &ndsc, ((type&0xff00)>>8)+((type&0x00ff)<<8),
    	    &cldsc, &ttl);
    	break;
    }

    } /* switch */

    *nspp = nsp_save + rdlength;

} /* Show_RR */

/*
**++
**  ROUTINE:	name_unpack
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Unpacks a domain name, decoding any offset references.
**  This is referenced through the NAME_UNPACK macro in other routines.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	name_unpack(unsigned char **bufptr, char *name, int size)
**
**  bufptr: byte pointer, modify, by reference
**  name:   ASCIZ_string, write only, by reference
**  size:   integer, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static void name_unpack(unsigned char **nspp, char *buf, int bufsize) {

    char seg[STRING_SIZE];
    unsigned char *nsp, *offset;
    char *bufp;
    unsigned short xoffset;

    nsp = *nspp;
    if (!*nsp) {
    	strcpy(buf, ".");
    	*nspp += 1;
    	return;
    }

    bufp = buf;
    while (*nsp) {
    	if (*nsp < 64) {
    	    if (bufp-buf+*nsp < bufsize-2) {
    	    	memcpy(bufp, nsp+1, *nsp);
    	    	bufp += *nsp;
    	    	*bufp++ = '.';
    	    	*bufp = '\0';
    	    }
    	    nsp += *nsp + 1;
    	} else {
    	    BYTE_SWAP_UNPACK(nsp,xoffset);
    	    offset = nsbuf + (xoffset - 0xc000) + sizeof(short);
    	    name_unpack(&offset, seg, sizeof(seg));
    	    strcpy(bufp, seg);
    	    *nspp = nsp;
    	    return;
    	}
    }
    *nspp = nsp + 1;

} /* name_unpack */

/*
**++
**  ROUTINE:	check_inverse_query
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Converts dotted-decimal IP address into the appropriate
**  	.IN-ADDR.ARPA name.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	check_inverse_query(char *host)
**
**  host:   ASCIZ_string, modify, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static void check_inverse_query(char *host) {

    char tmp[STRING_SIZE], tmp2[STRING_SIZE];
    char *cp;

    if (strspn(host, "0123456789.") != strlen(host)) return;

    strcpy(tmp, host);
    tmp2[0] = '\0';
    for (cp = tmp+strlen(tmp)-1; cp > tmp; cp--) {
    	if (*cp == '.') {
    	    strcat(tmp2, cp+1);
    	    strcat(tmp2, ".");
    	    *cp = '\0';
    	}
    }
    strcat(tmp2, tmp);
    strcat(tmp2, ".IN-ADDR.ARPA.");
    strcpy(host, tmp2);

} /* check_inverse_query */
