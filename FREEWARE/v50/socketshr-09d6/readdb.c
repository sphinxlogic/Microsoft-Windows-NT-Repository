/*	readdb.c							*/
/*	V1.0			24-Oct-1994	IfN/Mey			*/
/*				03-Dec-1998	J. Malmberg		*/
/*				    *	TRUE/FALSE not in GCC		*/
/*				12-Dec-1999	J. Malmberg
 *				    *		Need <string.h>.  DECC will not
 *						let unsigned and signed mix.
 *				05-Nov-2000	T. Dickey
 *						Use socketprv.h header,
 *						ANSIfy'd functions.
 */
/*+
 * Read inet databases (services, protocols)
 *-
 */

#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#define SERVNULL ((struct servent *)0)
#define PROTONULL ((struct protoent *)0)

#include "[-]socketprv.h"

/*
 * **-stredit
 */
#define	STR_TRIM	0x0001
#define	STR_COMPRESS	0x0002
#define	STR_COLLAPSE	0x0004
#define	STR_UPCASE	0x0010
#define	STR_LOWERCASE	0x0020
#define	STR_UNCOMMENT	0x0040
static char *
_stredit(char *dst, char *src, int flag, char *cchar)
{
	register char *p;
	char *cp;
	char *p1;
	char last;
	register char c;

	last = '\0';
	if (dst == NULL) return(NULL);
	p = dst;
	p1 = dst;
	if (src == NULL) goto stredit_fin;

	for(;*src != '\0';src++) {
		c = *src;
		if ( (flag&STR_UNCOMMENT)  &&  cchar != NULL ) {
			cp = cchar;
			while(*cp != '\0') {
				if (c == *cp++) goto stredit_fin;
			}
		}
		if (flag&STR_COLLAPSE  &&  (c == ' ' || c == '\t')) {
			continue;
		}
		if (flag&STR_COMPRESS) {
			if (c == '\t') c = ' ';
			if (c == ' '  &&  last == ' ') continue;
		}
		if (flag&STR_TRIM) {
			if (c == ' ' || c == '\t') {
				if (p == dst) continue;
			}
			else {
				p1 = p + 1;
			}
		}
		else {
			p1 = p + 1;
		}
		if (flag&STR_UPCASE) *p++ = toupper(c);
		else if (flag&STR_LOWERCASE) *p++ = tolower(c);
		else *p++ = c;
		last = c;
	}
stredit_fin:
	*p1 = '\0';
	return(dst);
}


/*
 * **-strelement
 *
 *	(char *)p = strelement(char *element, char *string, int n, char *sep);
 *
 * Find the 'n'th element in 'string'. Elements are separated by the
 * character 'sep'. The element is returned in 'element'.
 * strelement() returns a pointer to the element.
 * If the element does not exist, NULL is returned.
 * The first element is 0. The 0'th element is always present and equal
 * to 'string' if no character 'sep' is present.
 */
static char *
_strelement(char *t, char *s, int n, char *d)
{
	int i = 0;
	char *p;
	char *q = NULL;

	if (s == NULL || d == NULL) return(NULL);

	if (n == 0) q = s;

	for(p = s; *p != '\0'; p++) {
		if (*p == *d) {
			i++;
			if (q == NULL) {
				if (i == n) q = p+1;
			}
			else {
				break;
			}
		}
	}
	if (q == NULL) return(NULL);
	if (t != NULL) {
		strncpy(t, q, p-q);
		t[p-q] = '\0';
	}
	return(q);
}



static FILE *
_open_db(char *filename)
{
	char	path_val[256];
	char	path_log[128];
	int	stat;

	strcpy(path_log,"SOCKETSHR_");
	strcat(path_log, filename);
	_stredit(path_log,path_log,STR_UPCASE,NULL);
	stat = __trnlnm("LNM$FILE_DEV",path_log,"",path_val,256);
	if (stat != 0) {
		strcpy(path_val,"SYS$LIBRARY:");
		strcat(path_val,filename);
		strcat(path_val,".");
	}
	return(fopen(path_val,"r"));
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * struct servent *getservent(void);
 *
 * Description:
 *	returns a servent structure filled in with information about the
 * a service.
 *
 * The routine searches for a file pointed to by SOCKETSHR_SERVICES. If
 * the logical does not exist, SYS$LIBRARY:SERVICES. is used.
 * The format of this file is the standard format from UNIX.
 *
 * The file is opened if neccessary and the next entry is returned.
 *
 * Returns:
 *	Returns the address of a servent structure on success, the NULL pointer
 * on error.
 */

static int serv_stayopen = FALSE;
static FILE* fp_serv = (FILE *)0;
static struct servent ret_servent;
#define MAXSERVALIASES 20
static char *servalias[MAXSERVALIASES];
static char servbuf[256];

struct servent *
socket_getservent(void)
{
	register  char *p;
	int i;
	char *q;

	if (fp_serv == (FILE *)0 ) {
		fp_serv = _open_db("services");
		if (fp_serv == (FILE *)0) return(SERVNULL);
	}
	ret_servent.s_aliases = (char **)0; /* flags invalid struct */
	while(fgets(servbuf, 255, fp_serv)) {
/*
 * Analyse one line from file.
 * Replace trailing LF with NUL.
 * Add a second trailing NUL. This is used later!
 */
		servbuf[strlen(servbuf)-1] = '\0';
		_stredit(servbuf, servbuf,
			STR_TRIM|STR_COMPRESS|STR_UNCOMMENT,"#");
		servbuf[strlen(servbuf)+1] = '\0';
		if (servbuf[0] == '\0') continue;
/*
 * Fill ret_servent from line
 */
		servalias[0] = NULL;
		p = servbuf;
/* name */
		ret_servent.s_name = servbuf;
		while (*p != '\0' && *p != ' ') p++;
		if (*p == '\0') continue;
		*p++ = '\0';
/* port */
		q = p;
		while (*p != '\0' && *p != '/') p++;
		if (*p == '\0') continue;
		*p++ = '\0';
		ret_servent.s_port = htons(atoi(q));
/* protocol */
		ret_servent.s_proto = p;
		while (*p != '\0' && *p != ' ') p++;
		*p++ = '\0';
/*
 * Fill in alias names.
 * p points behind(!) the protocol string. If there are no alias names,
 * p now points to the second NUL...
 */
		for (i=0;;i++) {
			if (*p == '\0') break;
			servalias[i] = p;
			while (*p != '\0' && *p != ' ') p++;
			*p++ = '\0';
		}
		servalias[i] = NULL;
/*
 * We return this entry.
 */
		ret_servent.s_aliases = servalias; /* flags struct valid */
		break;	/* break while loop */
	}
/*
 * Return the struct if valid.
 */
	if (ret_servent.s_aliases == (char **)0) {
		errno = EACCES;
		return(SERVNULL);
	}
	return(&ret_servent);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * int setservent(int stayopen)
 *
 * Description:
 *	Opens and rewinds the file. If the flag stayopen is non-zero, the
 * service database is not closed after each call to getservent().
 *
 * Returns: 0
 */

int socket_setservent(int stayopen)
{
	serv_stayopen = stayopen;
	fclose(fp_serv);
	fp_serv = _open_db("services");
	return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * int endservent(void)
 *
 * Description:
 *	Closes the database.
 *
 * Returns: 0
 */

int socket_endservent(void)
{
	fclose(fp_serv);
	fp_serv = (FILE *)0;
	return(0);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * struct servent *getservbyname(char *name, char *proto);
 *
 * Description:
 *	returns a servent structure filled in with information about the
 * requested service.
 *
 * name  - pointer to character string of service to search for
 * proto - pointer to character string of protocol type desired (tcp, udp)
 *
 * Notes:
 *	The usual services text file is supported.
 *
 * Returns:
 *	Returns the address of a servent structure on success, the NULL pointer
 * on error.
 */

struct servent *socket_getservbyname(char *name, char *proto)
{
struct servent *s;
char **pp;
/*
 * Search from the beginning of the file.
 */
	socket_endservent();
	if (name == NULL) return(SERVNULL);
	while (TRUE) {
		s = socket_getservent();
		if (s == SERVNULL) goto servbyname_return;
		if (proto != NULL  &&  strcmp(proto,s->s_proto) != 0) continue;
		if (strcmp(name,s->s_name) == 0) goto servbyname_return;
		for (pp = s->s_aliases; *pp; pp++) {
			if (strcmp(name,*pp) == 0) goto servbyname_return;
		}
	}
servbyname_return:
	if (!serv_stayopen) {
		socket_endservent();
	}
	return(s);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * struct servent *getservbyport(int port, char *proto);
 *
 * Description:
 *	returns a servent structure filled in with information about the
 * requested service.
 *
 * port  - port of service to search for
 * proto - pointer to character string of protocol type desired (tcp, udp)
 *
 * Notes:
 *	The usual services text file is supported.
 *
 * Returns:
 *	Returns the address of a servent structure on success, the NULL pointer
 * on error.
 */

struct servent *socket_getservbyport(int port, char *proto)
{
struct servent *s;
/*
 * Search from the beginning of the file.
 */
	socket_endservent();
	while (TRUE) {
		s = socket_getservent();
		if (s == SERVNULL) break;
		if (proto != NULL  &&  strcmp(proto,s->s_proto) != 0) continue;
		if (port == s->s_port) break;
	}
	if (!serv_stayopen) {
		socket_endservent();
	}
	return(s);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * struct protoent *getprotoent(void);
 *
 * Description:
 *	returns a protoent structure filled in with information about the
 * a protocol.
 *
 * The routine searches for a file pointed to by SOCKETSHR_PROTOCOLS. If
 * the logical does not exist, SYS$LIBRARY:PROTOCOLS. is used.
 * The format of this file is the standard format from UNIX.
 *
 * The file is opened if neccessary and the next entry is returned.
 *
 * Returns:
 *	Returns the address of a protoent structure on success, the NULL pointer
 * on error.
 */

static int proto_stayopen = FALSE;
static FILE* fp_proto = (FILE *)0;
static struct protoent ret_protoent;
#define MAXPROTOALIASES 20
static char *protoalias[MAXPROTOALIASES];
static char protobuf[256];

struct protoent *
socket_getprotoent(void)
{
	register  char *p;
	int i;
	char *q;

	if (fp_proto == (FILE *)0 ) {
		fp_proto = _open_db("protocols");
		if (fp_proto == (FILE *)0) return(PROTONULL);
	}
	ret_protoent.p_aliases = (char **)0; /* flags invalid struct */
	while(fgets(protobuf, 255, fp_proto)) {
/*
 * Analyse one line from file.
 * Replace trailing LF with NUL.
 * Add a second trailing NUL. This is used later!
 */
		protobuf[strlen(protobuf)-1] = '\0';
		_stredit(protobuf, protobuf,
			STR_TRIM|STR_COMPRESS|STR_UNCOMMENT,"#");
		protobuf[strlen(protobuf)+1] = '\0';
		if (protobuf[0] == '\0') continue;
/*
 * Fill ret_protoent from line
 */
		protoalias[0] = NULL;
		p = protobuf;
/* name */
		ret_protoent.p_name = protobuf;
		while (*p != '\0' && *p != ' ') p++;
		if (*p == '\0') continue;
		*p++ = '\0';
/* protocol */
		q = p;
		while (*p != '\0' && *p != ' ') p++;
		if (*p == '\0') continue;
		*p++ = '\0';
		ret_protoent.p_proto = atoi(q);
/*
 * Fill in alias names.
 * p points behind(!) the protocol string. If there are no alias names,
 * p now points to the second NUL...
 */
		for (i=0;;i++) {
			if (*p == '\0') break;
			protoalias[i] = p;
			while (*p != '\0' && *p != ' ') p++;
			*p++ = '\0';
		}
		protoalias[i] = NULL;
/*
 * We return this entry.
 */
		ret_protoent.p_aliases = protoalias; /* flags struct valid */
		break;	/* break while loop */
	}
/*
 * Return the struct if valid.
 */
	if (ret_protoent.p_aliases == (char **)0) {
		errno = EACCES;
		return(PROTONULL);
	}
	return(&ret_protoent);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * int setprotoent(int stayopen)
 *
 * Description:
 *	Opens and rewinds the file. If the flag stayopen is non-zero, the
 * protocol database is not closed after each call to getprotoent().
 *
 * Returns: 0
 */

int socket_setprotoent(int stayopen)
{
	proto_stayopen = stayopen;
	fclose(fp_proto);
	fp_proto = _open_db("protocols");
	return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * int endprotoent(void)
 *
 * Description:
 *	Closes the database.
 *
 * Returns: 0
 */

int socket_endprotoent(void)
{
	fclose(fp_proto);
	fp_proto = (FILE *)0;
	return(0);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * struct protoent *getprotobyname(char *name);
 *
 * Description:
 *	returns a protoent structure filled in with information about the
 * requested protocol.
 *
 * name  - pointer to character string of protocol to search for
 *
 * Notes:
 *	The usual protocols text file is supported.
 *
 * Returns:
 *	Returns the address of a protoent structure on success, the NULL pointer
 * on error.
 */

struct protoent *socket_getprotobyname(char *name)
{
struct protoent *s;
char **pp;
/*
 * Search from the beginning of the file.
 */
	socket_endprotoent();
	if (name == NULL) return(PROTONULL);
	while (TRUE) {
		s = socket_getprotoent();
		if (s == PROTONULL) goto protobyname_return;
		if (strcmp(name,s->p_name) == 0) goto protobyname_return;
		for (pp = s->p_aliases; *pp; pp++) {
			if (strcmp(name,*pp) == 0) goto protobyname_return;
		}
	}
protobyname_return:
	if (!proto_stayopen) {
		socket_endprotoent();
	}
	return(s);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * struct protoent *getprotobynumber(int proto);
 *
 * Description:
 *	returns a protoent structure filled in with information about the
 * requested protocol.
 *
 * proto - protocol number to search for
 *
 * Notes:
 *	The usual protocols text file is supported.
 *
 * Returns:
 *	Returns the address of a protoent structure on success, the NULL pointer
 * on error.
 */

struct protoent *socket_getprotobynumber(int proto)
{
struct protoent *s;
/*
 * Search from the beginning of the file.
 */
	socket_endprotoent();
	while (TRUE) {
		s = socket_getprotoent();
		if (s == PROTONULL) break;
		if (proto == s->p_proto) break;
	}
	if (!proto_stayopen) {
		socket_endprotoent();
	}
	return(s);
}

#if 0
main(int argc, char *argv[])
{
	struct servoent *s;
	int port = 0;
	char **pp;

	if (argc > 1) port = atoi(argv[1]);

	if (argc == 1) socket_setservent(TRUE);

loop:
	if (argc == 1) {
		s = socket_getservent();
		if (s == SERVNULL) exit(1);
	}
	else if (port == 0) {
		s = socket_getservbyname(argv[1], argv[2]);
	}
	else {
		s = socket_getservbyport(port, argv[2]);
	}
	if (s != SERVNULL ) {
		printf("Name: {%s}, Port: %d, Protocol: {%s}\n",
			s->s_name, s->s_port, s->s_proto);
		for (pp=s->s_aliases; *pp; pp++) {
			printf("  Alias name: {%s}\n",*pp);
		}
	}
	else {
		printf("Service not found!\n");
	}
	if (argc == 1) goto loop;
}
#endif
