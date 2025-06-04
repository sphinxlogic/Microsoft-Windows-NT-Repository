/*
 *   cddb - CD Database Management Library
 *
 *   Copyright (C) 1993-1998  Ti Kan
 *   E-mail: ti@amb.org
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#ifndef LINT
static char *_cddb_c_ident_ = "@(#)cddb.c	6.182 98/10/27";
#endif

#ifdef __VMS
typedef char *	caddr_t;
#endif

#include "common_d/appenv.h"
#include "common_d/patchlevel.h"
#include "common_d/util.h"
#include "cddb_d/cddbp.h"
#include "cddb_d/cddb.h"


#define MAX_ENV_LEN		(STR_BUF_SZ * 16)

#define CDDB_MIN(a,b)		(((a) > (b)) ? (b) : (a))
#define CDDB_MAX(a,b)		(((a) > (b)) ? (a) : (b))

#define STATCODE_1ST(p)		((p)[0])
#define STATCODE_2ND(p)		((p)[1])
#define STATCODE_3RD(p)		((p)[2])
#define STATCODE_CHECK(p)	\
	(STATCODE_1ST(p) != '\0' && isdigit(STATCODE_1ST(p)) && \
	 STATCODE_2ND(p) != '\0' && isdigit(STATCODE_2ND(p)) && \
	 STATCODE_3RD(p) != '\0' && isdigit(STATCODE_3RD(p)))


/* Time interval to run workproc while waiting for I/O */
#define CDDB_WAIT_SEC		0
#define CDDB_WAIT_USEC		200000


extern appdata_t	app_data;
extern FILE		*errfp;


STATIC cddb_client_t	*cddb_clinfo;		/* Client info */
STATIC cddb_path_t	*cddb_pathhead,		/* CDDB path list head */
			*cddb_pathptr;		/* CDDB path list ptr */
STATIC cddb_linkopts_t	*cddb_linkhead;		/* Link options list head */
STATIC cddb_proxy_t	cddb_proxy;		/* HTTP Proxy server info */
STATIC pid_t		child_pid;		/* Child pid */
STATIC bool_t		ischild;		/* Child process */
STATIC char		*auth_buf,		/* Proxy auth buf pointer */
			curfile[FILE_PATH_SZ],	/* Current disc info file */
			http_hellostr[MAXHOSTNAMELEN + STR_BUF_SZ],
			http_extinfo[MAXHOSTNAMELEN + STR_BUF_SZ];
						/* http mode handshake */


/***********************
 *  internal routines  *
 ***********************/


/*
 * onalrm
 *	Signal handler for SIGALRM
 *
 * Args:
 *	signo - The signal number
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
onalrm(int signo)
{
	(void) signal(SIGALRM, onalrm);
	DBGPRN(errfp, "SIGALRM received\n");
}


/*
 * onterm
 *	Signal handler for SIGTERM
 *
 * Args:
 *	signo - The signal number
 *
 * Return:
 *	Nothing.
 */
/*ARGSUSED*/
STATIC void
onterm(int signo)
{
	DBGPRN(errfp, "SIGTERM received\n");
	exit(0);
}


/*
 * cddb_skip_whitespace
 *	Given a character string, return a pointer to the position
 *	where the first non-whitespace (not space or tab) occurs.
 *
 * Args:
 *	str - The input string
 *
 * Return:
 *	Pointer to the next non-whitespace in str.  NULL is returned if the
 *	input string is NULL.
 */
STATIC char *
cddb_skip_whitespace(char *str)
{
	if (str == NULL)
		return NULL;

	while (*str == ' ' || *str == '\t')
		str++;

	return (str);
}


/*
 * cddb_line_filter
 *	Given a string, process it such that becomes only one
 *	single line (i.e., filter out all text including and
 *	following any newline character).  A newline character
 *	is denotes as the characters '\' and 'n' (NOT '\n').
 *
 * Arg:
 *	str - The string to be processed
 *
 * Return:
 *	Nothing.  The input string may be modified.
 */
STATIC void
cddb_line_filter(char *str)
{
	if (str == NULL)
		return;

	for (; *str != '\0'; str++) {
		if (*str == '\\' && *(str+1) == 'n') {
			*str = '\0';
			break;
		}
	}
}


/*
 * cddb_strcat
 *	Concatenate two text strings with special handling for newline
 *	and tab character translations.
 *
 * Args:
 *	s1 - The first text string and destination string.
 *	s2 - The second text string.
 *
 * Return:
 *	Pointer to the resultant string, or NULL if failed.
 */
STATIC char *
cddb_strcat(char *s1, char *s2)
{
	int	n;
	char	*cp = s1;
	bool_t	proc_slash;

	if (s1 == NULL || s2 == NULL)
		return NULL;

	/* Concatenate two strings, with special handling for newline
	 * and tab characters.
	 */
	proc_slash = FALSE;
	n = strlen(s1);
	s1 += n;
	if (n > 0 && *(s1 - 1) == '\\') {
		proc_slash = TRUE;	/* Handle broken escape sequences */
		s1--;
	}

	for (; *s2 != '\0'; s1++, s2++) {
		if (*s2 == '\\') {
			proc_slash = TRUE;
			s2++;
		}

		if (proc_slash) {
			proc_slash = FALSE;

			switch (*s2) {
			case 'n':
				*s1 = '\n';
				break;
			case 't':
				*s1 = '\t';
				break;
			case '\\':
				*s1 = '\\';
				break;
			default:
				*s1++ = '\\';
				*s1 = *s2;
				break;
			}
		}
		else
			*s1 = *s2;
	}
	*s1 = '\0';

	return (cp);
}


/*
 * cddb_http_xlat
 *	String translation for special meta-characters on the HTTP command
 *	buffer.
 *
 * Args:
 *	s1 - Input string
 *	s2 - Output string
 *
 * Return:
 *	Nothing.
 */
STATIC void
cddb_http_xlat(char *s1, char *s2)
{
	char	*p,
		*q;

	for (p = s1, q = s2; *p != '\0'; p++) {
		switch (*p) {
		case '?':
		case '=':
		case '+':
		case '&':
		case ' ':
		case '%':
			(void) sprintf(q, "%%%02X", *p);
			q += 3;
			break;
		default:
			*q = *p;
			q++;
			break;
		}
	}
	*q = '\0';
}


/*
 * cddb_sum
 *	Convert an integer to its text string representation, and
 *	compute its checksum.  Used by cddb_discid to derive the
 *	disc ID.
 *
 * Args:
 *	n - The integer value.
 *
 * Return:
 *	The integer checksum.
 */
STATIC int
cddb_sum(int n)
{
	int	ret;

	/* For backward compatibility this algorithm must not change */
	for (ret = 0; n > 0; n /= 10)
		ret += n % 10;

	return (ret);
}


/*
 * cddb_waitio
 *	Check if read data is pending.  If no data and a workproc is
 *	specified, then call workproc.  Keep waiting until there is
 *	data or until timeout.
 *
 * Args:
 *	fd - File descriptor on which we are expecting data
 *	do_workproc - Whether we should run workproc while waiting
 *	tmout - Timeout interval (in seconds).  If set to 0, no
 *		timeout will occur.
 *
 * Return:
 *	TRUE - There is data pending.
 */
STATIC bool_t
cddb_waitio(int fd, bool_t do_workproc, int tmout)
{
#ifndef SYNCHRONOUS
	int		ret;
	fd_set		rfds;
	fd_set		efds;
	struct timeval	to;
	time_t		start,
			now;

	if (tmout > 0)
		(void) time(&start);

	do {
		FD_ZERO(&efds);
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);

		to.tv_sec = CDDB_WAIT_SEC;
		to.tv_usec = CDDB_WAIT_USEC;
#ifdef __hpux
		ret = select(fd+1, (int *) &rfds, NULL, (int *) &efds, &to);
#else
		ret = select(fd+1, &rfds, NULL, &efds, &to);
#endif
		if (tmout > 0 && ret == 0) {
			(void) time(&now);
			if ((now - start) > tmout) {
				/* Timeout */
				DBGPRN(errfp, "Timed out waiting for data.\n");
				ret = -1;
			}
		}

		if (ret == 0 && do_workproc && cddb_clinfo->workproc != NULL)
			cddb_clinfo->workproc(cddb_clinfo->arg);

	} while (ret == 0);

	errno = 0;
	return (ret > 0 ? TRUE : FALSE);
#else
	return TRUE;
#endif	/* SYNCHRONOUS */
}


/*
 * cddb_getc
 *	Return a character from the file stream.  Perform buffered
 *	read from file if necessary.
 *
 * Args:
 *	fp - Pointer to the cddb_file_t structure.
 *	do_workproc - set to TRUE if the workproc should be invoked
 *		while waiting for I/O.
 *
 * Return:
 *	The character, or -1 on EOF or failure.
 */
STATIC int
cddb_getc(cddb_file_t *fp, bool_t do_workproc)
{
	int		tmout;
	static int	i = 0;

	/* Do some work every 10 characters read */
	if ((i % 10) == 0 && do_workproc && cddb_clinfo->workproc != NULL)
		cddb_clinfo->workproc(cddb_clinfo->arg);

	if (++i > 100)
		i = 0;	/* Reset count */

	if (fp->pos == fp->cnt) {
		/* Wait for data */
		if (fp->type == CDDB_REMOTE_CDDBP ||
		    fp->type == CDDB_REMOTE_HTTP)
			tmout = app_data.cddb_timeout;
		else
			tmout = 0;

		if (!cddb_waitio(fp->fd, do_workproc, tmout))
			return -1;

		/* Load cache */
		fp->cnt = read(fp->fd, fp->cache, CDDB_CACHESZ);
		if (fp->cnt <= 0) {
			fp->cnt = fp->pos;
			return -1;
		}
		fp->pos = 1;
	}
	else
		fp->pos++;

	return ((int) fp->cache[fp->pos-1]);
}


/*
 * cddb_gets
 *	Read a line of text from the file.  The read terminates
 *	when it encounters a newline character, EOF, or if len
 *	characters have been read.
 *
 * Args:
 *	fp - The CDDB file pointer obtained via cddb_open()
 *	buf - Pointer to return string buffer
 *	len - Maximum number of characters to read
 *	do_workproc - set to TRUE if the workproc should be invoked
 *		while waiting for I/O.
 *
 * Return:
 *	TRUE - Successfully read a line of text
 *	FALSE - Reached EOF or read failure
 */
STATIC bool_t
cddb_gets(cddb_file_t *fp, char *buf, int len, bool_t do_workproc)
{
	int	tot = 0,
		c = 0;

	/* Sanity check */
	if (fp->magic != CDDB_FMAGIC)
		return FALSE;

	while ((c = cddb_getc(fp, do_workproc)) > 0) {
		*buf = (char) c;

		tot++;
		buf++;
		len--;

		if (c == '\n') {
			/* Translate CR-LF into just LF */
			if (*(buf-2) == '\r') {
				*(buf-2) = '\n';
				*(buf-1) = '\0';
			}
			break;
		}

		if (len <= 0)
			break;
	}
	*buf = '\0';
	return ((bool_t) (tot > 0));
}


/*
 * cddb_puts
 *	Write a line of text to the file.
 *
 * Args:
 *	fp - The CDDB file pointer obtained via cddb_open()
 *	buf - Pointer to string buffer
 *
 * Return:
 *	TRUE - Success
 *	FALSE - Failure
 */
STATIC bool_t
cddb_puts(cddb_file_t *fp, char *buf)
{
	int	i,
		n,
		len;

	/* Sanity check */
	if (fp->magic != CDDB_FMAGIC)
		return FALSE;

	if (fp->type != CDDB_LOCAL)
		/* Remote writes not supported */
		return FALSE;

	len = strlen(buf);

	for (i = 0; i < len; i++) {
		fp->cache[fp->pos] = (unsigned char) *buf;

		fp->pos++;
		buf++;

		if (fp->pos == CDDB_CACHESZ) {
			/* Flush cache */
			n = write(fp->fd, fp->cache, fp->pos);

			if (n < 0 || n != fp->pos) {
				fp->pos = 0;
				return FALSE;
			}

			fp->pos = 0;
		}
	}

	return TRUE;
}


/*
 * cddb_remote_open
 *	Open a connection to the remote CDDB server.
 *
 * Args:
 *	fp - Pointer to the cddb_file_t structure
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
cddb_remote_open(cddb_file_t *fp)
{
#ifdef NOREMOTE
	return FALSE;
#else
	struct hostent		*hp;
	struct in_addr		ad;
	struct sockaddr_in	sin;
	char			*conhost;
	unsigned short		conport;

	if (fp->type == CDDB_REMOTE_HTTP && app_data.http_proxy) {
		/* Use proxy server */
		conhost = cddb_proxy.host;
		conport = (unsigned short) cddb_proxy.port;
	}
	else {
		/* Direct connect */
		conhost = fp->pathp->host;
		conport = (unsigned short) fp->pathp->port;
	}

	sin.sin_port = util_bswap16(conport);
	sin.sin_family = AF_INET;

	/* Set timeout in case remote server is unavailable */
	(void) signal(SIGALRM, onalrm);
	(void) alarm(app_data.cddb_timeout);

	/* Find server host address */
	if ((hp = gethostbyname(conhost)) != NULL) {
		(void) memcpy((char *) &sin.sin_addr, hp->h_addr, hp->h_length);
	}
	else {
		if ((ad.s_addr = inet_addr(conhost)) != -1) {
			(void) memcpy((char *) &sin.sin_addr,
				      (char *) &ad.s_addr, sizeof(ad.s_addr));
		}
		else {
			/* Cancel alarm */
			(void) alarm(0);

			DBGPRN(errfp, "Unknown host!\n");
			return FALSE;
		}
	}

	/* Open socket */
	if ((fp->fd = SOCKET(AF_INET, SOCK_STREAM, 0)) < 0) {
		if (app_data.debug)
			perror("Socket failed");

		/* Cancel alarm */
		(void) alarm(0);

		return FALSE;
	}

	/* Connect to server */
	if (CONNECT(fp->fd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		if (app_data.debug)
			perror("Connect failed");

		/* Cancel alarm */
		(void) alarm(0);

		(void) close(fp->fd);
		return FALSE;
	}

	/* Cancel alarm */
	(void) alarm(0);

	DBGPRN(errfp, "Opened %sconnection: %s:%d\n",
		(fp->type == CDDB_REMOTE_HTTP && app_data.http_proxy) ?
			"proxy " : "",
		conhost, conport);

	return TRUE;
#endif	/* NOREMOTE */
}


/*
 * cddb_remote_cmd
 *	Send a command to the remote CDDB server
 *
 * Args:
 *	fp - The CDDB file pointer obtained via cddb_open()
 *	buf - Pointer to command string buffer
 *	buflen - Size of the command string buffer
 *
 * Return:
 *	Pointer to the three-digit status code string
 */
STATIC char *
cddb_remote_cmd(cddb_file_t *fp, char *buf, int buflen)
{
	int		len,
			tot,
			ret;
	char		*p,
			*q;
	bool_t		valid_resp,
			proxyauth_fail;
	static char	code[4];

	p = buf;
	len = strlen(p);
	tot = 0;

	/* Send command to server */
	DBGPRN(errfp, "-> %s", p);

	while (len > 0) {
		if ((ret = write(fp->fd, p, len)) < 0) {
			DBGPRN(errfp, "Command write failed: errno = %d\n",
				errno);
			return NULL;
		}

		p += ret;
		tot += ret;
		len -= ret;
	}

	if (tot == 0) {
		DBGPRN(errfp, "Command write failed\n");
		return NULL;
	}

	/* Read server response to command */
	buf[0] = '\0';
	valid_resp = proxyauth_fail = FALSE;
	while (cddb_gets(fp, buf, buflen, FALSE)) {
		p = cddb_skip_whitespace(buf);
		DBGPRN(errfp, "<- %s", p);

		if (app_data.http_proxy && strncmp(p, "HTTP/", 5) == 0) {
			q = strchr(p+1, ' ');
			if (q != NULL && isdigit(*(q+1)) &&
			    (atoi(q+1) == HTTP_PROXYAUTH_FAIL)) {
				proxyauth_fail = TRUE;
			}
		}

		if (STATCODE_CHECK(p)) {
			valid_resp = TRUE;
			break;
		}
	}

	if (proxyauth_fail) {
		/* Need proxy authorization */
		DBGPRN(errfp, "Proxy authorization failed.\n");
		(void) sprintf(code, "%c%c%c",
			       STAT_GEN_CLIENT,
			       STAT_SUB_CLOSE,
			       STAT_QURY_AUTHFAIL);
		return (code);
	}
	if (!valid_resp) {
		/* Server error */
		DBGPRN(errfp, "Invalid CDDB server response.\n");
		return NULL;
	}

	(void) strncpy(code, p, 3);
	code[3] = '\0';

	return (code);
}


/*
 * cddb_eat_server_output
 *	Read remaining server output and discard
 *
 * Args:
 *	fp - The CDDB file pointer obtained via cddb_open()
 *	buf - Pointer to command string buffer
 *	buflen - Size of the command string buffer
 *
 * Return:
 *	TRUE - Success
 *	FALSE - Failure
 */
STATIC bool_t
cddb_eat_server_output(cddb_file_t *fp, char *buf, int buflen)
{
	char	*p;

	p = cddb_skip_whitespace(buf);

	if (!STATCODE_CHECK(p)) {
		/* Server error */
		DBGPRN(errfp, "Invalid CDDB server response.\n");
		return FALSE;
	}

	while (STATCODE_2ND(p) == STAT_SUB_OUTPUT) {
		if (!cddb_gets(fp, buf, buflen, FALSE)) {
			DBGPRN(errfp, "CDDB server read error!\n");
			return FALSE;
		}

		p = cddb_skip_whitespace(buf);

		DBGPRN(errfp, "<- %s", p);

		if (!STATCODE_CHECK(p)) {
			/* Server error */
			DBGPRN(errfp, "Invalid CDDB server response.\n");
			return FALSE;
		}

		switch (STATCODE_1ST(p)) {
		case STAT_GEN_INFO:
		case STAT_GEN_OK:
		case STAT_GEN_OKCONT:
			/* OK */
			break;

		case STAT_GEN_OKFAIL:
		case STAT_GEN_ERROR:
		case STAT_GEN_CLIENT:
		default:
			/* Error */
			DBGPRN(errfp,
				"CDDB server code '%c%c%c'\n",
				STATCODE_1ST(p),
				STATCODE_2ND(p),
				STATCODE_3RD(p));
			return FALSE;
		}
	}

	return TRUE;
}


/*
 * cddb_remote_banner_ck
 *	Check the remote CDDB server sign-on banner
 *
 * Args:
 *	fp - The CDDB file pointer obtained via cddb_open()
 *	buf - Pointer to command string buffer
 *	buflen - Size of the command string buffer
 *
 * Return:
 *	TRUE - Success
 *	FALSE - Failure
 */
STATIC bool_t
cddb_remote_banner_ck(cddb_file_t *fp, char *buf, int buflen)
{
	char	*p;

	/* Read server start-up banner */
	if (!cddb_gets(fp, buf, buflen, FALSE)) {
		DBGPRN(errfp, "No startup banner on CDDB server!\n");
		return FALSE;
	}

	/* Check sign on banner */
	p = cddb_skip_whitespace(buf);

	DBGPRN(errfp, "<- %s", p);

	if (!STATCODE_CHECK(p)) {
		/* Invalid server sign-on banner */
		DBGPRN(errfp, "Invalid CDDB server response.\n");
		return FALSE;
	}

	switch (STATCODE_1ST(p)) {
	case STAT_GEN_OK:
	case STAT_GEN_OKCONT:
		/* Banner OK: eat remaining lines, if any. */
		return (cddb_eat_server_output(fp, buf, buflen));

	case STAT_GEN_INFO:
	case STAT_GEN_OKFAIL:
	case STAT_GEN_ERROR:
	case STAT_GEN_CLIENT:
	default:
		/* Error */
		DBGPRN(errfp, "CDDB server code '%c%c%c'\n",
			STATCODE_1ST(p), STATCODE_2ND(p), STATCODE_3RD(p));
		return FALSE;
	}
}


/*
 * cddb_cmd_hello
 *	Perform "hello" handshake to remote CDDB server
 *	Used for cddbp only.  This assumes that the connection
 *	is already open upon entering this function.
 *
 * Args:
 *	fp - The CDDB file pointer obtained via cddb_open()
 *	buf - Pointer to command string buffer
 *	buflen - Size of the command string buffer
 *
 * Return:
 *	TRUE - Success
 *	FALSE - Failure
 */
STATIC bool_t
cddb_cmd_hello(cddb_file_t *fp, char *buf, int buflen)
{
	char	*code;

	if (fp->type != CDDB_REMOTE_CDDBP)
		return FALSE;

	/* Send hello */
	(void) sprintf(buf, "%s %.64s %.64s %s v%s%sPL%d\r\n",
		       CDDB_HELLO, cddb_clinfo->user, cddb_clinfo->host,
		       cddb_clinfo->prog, VERSION, VERSION_EXT, PATCHLEVEL);

	if ((code = cddb_remote_cmd(fp, buf, buflen)) == NULL)
		return FALSE;

	switch (STATCODE_1ST(code)) {
	case STAT_GEN_OK:
	case STAT_GEN_OKCONT:
		/* OK: eat remaining lines, if any. */
		return (cddb_eat_server_output(fp, buf, buflen));

	case STAT_GEN_INFO:
	case STAT_GEN_OKFAIL:
	case STAT_GEN_ERROR:
	case STAT_GEN_CLIENT:
	default:
		/* Error */
		DBGPRN(errfp, "CDDB server code '%c%c%c'\n",
			STATCODE_1ST(code),
			STATCODE_2ND(code),
			STATCODE_3RD(code));
		return FALSE;
	}
}


/*
 * cddb_cmd_query
 *	Perform CDDB "query" on remote CDDB server
 *	This assumes that the connection is already open for
 *	both cddbp and http upon entering this function.
 *
 * Args:
 *	fp - The CDDB file pointer obtained via cddb_open()
 *	buf - Pointer to command string buffer
 *	buflen - Size of the command string buffer
 *	dbp - Pointer to the cddb_incore_t structure
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - Success
 *	FALSE - Failure
 */
STATIC bool_t
cddb_cmd_query(
	cddb_file_t	*fp,
	char		*buf,
	int		buflen,
	cddb_incore_t	*dbp,
	curstat_t	*s
)
{
	int		i;
	char		*code,
			*p,
			*p1,
			urlstr[MAXHOSTNAMELEN + FILE_PATH_SZ + 12];
	cddb_match_t	*q,
			*r;

	dbp->category[0] = '\0';

	/* Send query */
	switch (fp->type) {
	case CDDB_REMOTE_CDDBP:
		(void) sprintf(buf, "%s %08x %d",
			       CDDB_QUERY, dbp->queryid, s->tot_trks);

		for (i = 0; i < (int) s->tot_trks; i++) {
			(void) sprintf(buf, "%s %d",
				       buf,
				       s->trkinfo[i].addr + MSF_OFFSET);
		}

		(void) sprintf(buf, "%s %d\r\n", buf,
			       s->trkinfo[(int) s->tot_trks].min * 60 +
			       s->trkinfo[(int) s->tot_trks].sec);
		break;

	case CDDB_REMOTE_HTTP:
		urlstr[0] = '\0';
		if (app_data.http_proxy) {
			(void) sprintf(urlstr, "http://%s:%d",
				       fp->pathp->host, fp->pathp->port);
		}
		(void) sprintf(urlstr, "%s/%s", urlstr, fp->pathp->path);

		(void) sprintf(buf, "GET %s?cmd=cddb+query+%08x+%d",
			       urlstr, dbp->queryid, s->tot_trks);

		for (i = 0; i < (int) s->tot_trks; i++) {
			(void) sprintf(buf, "%s+%d",
				       buf,
				       s->trkinfo[i].addr + MSF_OFFSET);
		}

		(void) sprintf(buf,
			       "%s+%d&%s&proto=%d %s\r\n",
			       buf,
			       s->trkinfo[(int) s->tot_trks].min * 60 +
			 	      s->trkinfo[(int) s->tot_trks].sec,
			       http_hellostr,
			       CDDBP_PROTO,
			       CDDB_HTTP_VER);

		if (app_data.http_proxy && auth_buf != NULL) {
			(void) sprintf(buf,
				       "%sProxy-Authorization: Basic %s\r\n",
				       buf,
				       auth_buf);
		}

		(void) sprintf(buf, "%sHost: %s\r\n%s\r\n",
			       buf,
			       fp->pathp->host,
			       http_extinfo);

		break;
	}

	if ((code = cddb_remote_cmd(fp, buf, buflen)) == NULL)
		return FALSE;

	switch (STATCODE_1ST(code)) {
	case STAT_GEN_OK:
	case STAT_GEN_OKCONT:
		switch (STATCODE_3RD(code)) {
		case STAT_QURY_EXACT:
			p = cddb_skip_whitespace(buf) + 4;
			if (*p == '\0' || (p1 = strchr(p, ' ')) == NULL) {
				/* Invalid server response */
				dbp->category[0] = '\0';
				return FALSE;
			}
			*p1 = '\0';

			/* Get category */
			(void) strncpy(dbp->category, p, FILE_BASE_SZ - 1);
			dbp->category[FILE_BASE_SZ - 1] = '\0';

			return TRUE;

		case STAT_QURY_INEXACT:
			if (STATCODE_2ND(code) != STAT_SUB_OUTPUT)
				/* Invalid server response */
				return FALSE;

			while (cddb_gets(fp, buf, buflen, FALSE)) {
				p = cddb_skip_whitespace(buf);

				DBGPRN(errfp, "<- %s", p);

				if (*p == '\0')
					/* Invalid server response */
					return FALSE;

				if (*p == '.')
					break;

				q = (cddb_match_t *)(void *) MEM_ALLOC(
					"cddb_match_t",
					sizeof(cddb_match_t)
				);
				if (q == NULL)
					/* No memory */
					return FALSE;

				q->dtitle = (char *) MEM_ALLOC(
					"q->dtitle",
					strlen(p)
				);
				if (q->dtitle == NULL) {
					/* No memory */
					MEM_FREE(q);
					return FALSE;
				}

				/* Get category, discid, and disc title */
				if ((p1 = strchr(p, ' ')) == NULL) {
					/* Invalid server response */
					MEM_FREE(q->dtitle);
					MEM_FREE(q);
				}
				*p1 = '\0';

				(void) strncpy(q->category,
					       p, FILE_BASE_SZ - 1);
				q->category[FILE_BASE_SZ - 1] = '\0';

				p = cddb_skip_whitespace(p1 + 1);
				if (*p == '\0') {
					/* Invalid server response */
					MEM_FREE(q->dtitle);
					MEM_FREE(q);
					return FALSE;
				}

				i = sscanf(p, "%x %[^\n]\n",
					&q->discid, q->dtitle);

				if (i < 2) {
					/* Invalid server response */
					MEM_FREE(q->dtitle);
					MEM_FREE(q);
					return FALSE;
				}

				for (r = dbp->matchlist; r != NULL;
				     r = r->next) {
					/* Look for duplicates */
					if (r->discid == q->discid) {
						MEM_FREE(q->dtitle);
						MEM_FREE(q);
						q = NULL;
						break;
					}
					if (strcmp(r->dtitle, q->dtitle) == 0) {
						MEM_FREE(q->dtitle);
						MEM_FREE(q);
						q = NULL;
						break;
					}
				}

				if (q != NULL) {
					/* Add to list */
					q->next = dbp->matchlist;
					dbp->matchlist = q;

					/* Set flag */
					dbp->flags |= CDDB_INEXACT;
				}
			}

			return TRUE;

		case STAT_QURY_NONE:
			return FALSE;
		}
		break;

	case STAT_GEN_CLIENT:
		switch (STATCODE_3RD(code)) {
		case STAT_QURY_AUTHFAIL:
			if (STATCODE_2ND(code) != STAT_SUB_CLOSE)
				/* Invalid server response */
				return FALSE;

			if (app_data.http_proxy) {
				dbp->flags |= CDDB_AUTHFAIL;
				return TRUE;
			}
			/*FALLTHROUGH*/

		default:
			break;
		}
		/*FALLTHROUGH*/

	case STAT_GEN_OKFAIL:
	case STAT_GEN_INFO:
	case STAT_GEN_ERROR:
	default:
		/* Error */
		DBGPRN(errfp, "CDDB server code '%c%c%c'\n",
			STATCODE_1ST(code),
			STATCODE_2ND(code),
			STATCODE_3RD(code));
		return FALSE;
	}

	return FALSE;
}


/*
 * cddb_cmd_read
 *	Perform CDDB "read" on remote CDDB server
 *	This assumes that the connection is already open for
 *	both cddbp and http upon entering this function.
 *
 * Args:
 *	fp - The CDDB file pointer obtained via cddb_open()
 *	buf - Pointer to command string buffer
 *	buflen - Size of the command string buffer
 *	dbp - Pointer to the cddb_incore_t structure.
 *
 * Return:
 *	TRUE - Success
 *	FALSE - Failure
 */
STATIC bool_t
cddb_cmd_read(cddb_file_t *fp, char *buf, int buflen, cddb_incore_t *dbp)
{
	char	*code,
		urlstr[MAXHOSTNAMELEN + FILE_PATH_SZ + 12];

	switch (fp->type) {
	case CDDB_REMOTE_CDDBP:
		(void) sprintf(buf, "%s %s %08x\r\n",
			       CDDB_READ, dbp->category, dbp->queryid);
		break;

	case CDDB_REMOTE_HTTP:
		(void) sprintf(urlstr, "%s%s/%s",
			       app_data.http_proxy ? "http://" : "",
			       app_data.http_proxy ? fp->pathp->host : "",
			       fp->pathp->path);

		(void) sprintf(buf,
			     "GET %s?cmd=cddb+read+%s+%08x&%s&proto=%d %s\r\n",
			       urlstr, dbp->category, dbp->queryid,
			       http_hellostr, CDDBP_PROTO, CDDB_HTTP_VER);

		if (app_data.http_proxy && auth_buf != NULL) {
			(void) sprintf(buf,
				       "%sProxy-Authorization: Basic %s\r\n",
				       buf,
				       auth_buf);
		}

		(void) sprintf(buf, "%sHost: %s\r\n%s\r\n",
			       buf,
			       fp->pathp->host,
			       http_extinfo);

		break;
	}

	if ((code = cddb_remote_cmd(fp, buf, buflen)) == NULL)
		return FALSE;

	switch (STATCODE_1ST(code)) {
	case STAT_GEN_OK:
	case STAT_GEN_OKCONT:
		if (STATCODE_2ND(code) == '1')
			return TRUE;

		return FALSE;

	case STAT_GEN_INFO:
	case STAT_GEN_OKFAIL:
	case STAT_GEN_ERROR:
	case STAT_GEN_CLIENT:
	default:
		/* Error */
		DBGPRN(errfp, "CDDB server code '%c%c%c'\n",
			STATCODE_1ST(code),
			STATCODE_2ND(code),
			STATCODE_3RD(code));
		return FALSE;
	}

	/*NOTREACHED*/
}


/*
 * cddb_do_remote_query
 *	Initiate a CDDB query operation with the remote server.
 *
 * Args:
 *	fp - Pointer to the cddb_file_t structure
 *	dbp - Pointer to the cddb_incore_t structure
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
cddb_do_remote_query(cddb_file_t *fp, cddb_incore_t *dbp, curstat_t *s)
{
#ifdef NOREMOTE
	return FALSE;
#else
	char	*buf;

	if (!cddb_remote_open(fp))
		return FALSE;

	/* Allocate command buffer */
	if ((buf = (char *) MEM_ALLOC("cddb_cmdbuf", CDDBP_CMDLEN)) == NULL) {
		(void) close(fp->fd);
		DBGPRN(errfp, "Closed %sconnection: %s\n",
			(fp->type == CDDB_REMOTE_HTTP && app_data.http_proxy) ?
			"proxy " : "",
			(fp->type == CDDB_REMOTE_HTTP && app_data.http_proxy) ?
			cddb_proxy.host : fp->pathp->host);
		CDDB_FATAL(app_data.str_nomemory);
		return FALSE;
	}

	switch (fp->pathp->type) {
	case CDDB_REMOTE_CDDBP:
		/* Verify server sign-on banner */
		if (!cddb_remote_banner_ck(fp, buf, CDDBP_CMDLEN)) {
			(void) close(fp->fd);
			DBGPRN(errfp, "Closed %sconnection: %s\n",
				(fp->type == CDDB_REMOTE_HTTP &&
				 app_data.http_proxy) ? "proxy " : "",
				(fp->type == CDDB_REMOTE_HTTP &&
				 app_data.http_proxy) ?
					cddb_proxy.host : fp->pathp->host);
			MEM_FREE(buf);
			return FALSE;
		}

		/* Perform hello handshake */
		if (!cddb_cmd_hello(fp, buf, CDDBP_CMDLEN)) {
			(void) close(fp->fd);
			DBGPRN(errfp, "Closed %sconnection: %s\n",
				(fp->type == CDDB_REMOTE_HTTP &&
				 app_data.http_proxy) ? "proxy " : "",
				(fp->type == CDDB_REMOTE_HTTP &&
				 app_data.http_proxy) ?
					cddb_proxy.host : fp->pathp->host);
			MEM_FREE(buf);
			return FALSE;
		}
		/* Query server about current CD */
		if (!cddb_cmd_query(fp, buf, CDDBP_CMDLEN, dbp, s)) {
			(void) close(fp->fd);
			DBGPRN(errfp, "Closed %sconnection: %s\n",
				(fp->type == CDDB_REMOTE_HTTP &&
				 app_data.http_proxy) ? "proxy " : "",
				(fp->type == CDDB_REMOTE_HTTP &&
				 app_data.http_proxy) ?
					cddb_proxy.host : fp->pathp->host);
			MEM_FREE(buf);
			return FALSE;
		}

		/* Don't close the connection yet - more commands to send */
		break;

	case CDDB_REMOTE_HTTP:
		/* Query server about current CD */
		if (!cddb_cmd_query(fp, buf, CDDBP_CMDLEN, dbp, s)) {
			(void) close(fp->fd);
			fp->fd = -1;
			DBGPRN(errfp, "Closed %sconnection: %s\n",
				(fp->type == CDDB_REMOTE_HTTP &&
				 app_data.http_proxy) ? "proxy " : "",
				(fp->type == CDDB_REMOTE_HTTP &&
				 app_data.http_proxy) ?
					cddb_proxy.host : fp->pathp->host);
			MEM_FREE(buf);
			return FALSE;
		}

		/* Close here because HTTP only allows one transaction
		 * per connection.  fp->fd is not set to -1, though,
		 * so that the error checking code in cddb_open() doesn't
		 * fail.
		 */
		(void) close(fp->fd);
		DBGPRN(errfp, "Closed %sconnection: %s\n",
			(fp->type == CDDB_REMOTE_HTTP && app_data.http_proxy) ?
				"proxy " : "",
			(fp->type == CDDB_REMOTE_HTTP && app_data.http_proxy) ?
				cddb_proxy.host : fp->pathp->host);
		break;
	}

	/* Succeeded */
	MEM_FREE(buf);

	return TRUE;
#endif	/* NOREMOTE */
}


/*
 * cddb_putentry
 *	Write one information item into a database file.  Used by
 *	cddb_write to update the CD database file.
 *
 * Args:
 *	fp - CDDB file pointer
 *	idstr - The information identifier keyword text string
 *	entry - The information text string
 *
 * Return:
 *	Nothing.
 */
STATIC bool_t
cddb_putentry(cddb_file_t *fp, char *idstr, char *entry)
{
	int	i,
		n;
	char	*cp,
		tmpbuf[STR_BUF_SZ];

	if (entry == NULL) {
		/* Null entry */
		(void) sprintf(tmpbuf, "%s=\n", idstr);
		if (!cddb_puts(fp, tmpbuf))
			return FALSE;
	}
	else {
		/* Write entry to file, splitting into multiple lines
		 * if necessary.  Special handling for newline and tab
		 * characters.
		 */
		cp = entry;

		do {
			(void) sprintf(tmpbuf, "%s=", idstr);
			if (!cddb_puts(fp, tmpbuf))
				return FALSE;

			n = CDDB_MIN((int) strlen(cp), STR_BUF_SZ);

			for (i = 0; i < n; i++, cp++) {
				switch (*cp) {
				case '\n':
					if (!cddb_puts(fp, "\\n"))
						return FALSE;
					break;
				case '\t':
					if (!cddb_puts(fp, "\\t"))
						return FALSE;
					break;
				case '\\':
					if (!cddb_puts(fp, "\\\\"))
						return FALSE;
					break;
				default:
					(void) sprintf(tmpbuf, "%c", *cp);
					if (!cddb_puts(fp, tmpbuf))
						return FALSE;
					break;
				}
			}

			if (!cddb_puts(fp, "\n"))
				return FALSE;

			if (*cp == '\0')
				break;

		} while (n == STR_BUF_SZ);
	}

	return TRUE;
}


/*
 * cddb_add_pathent
 *	Add a cddb path list component.
 *
 * Args:
 *	path - The path component string
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
cddb_add_pathent(char *path)
{
	int		n;
	char		*cp,
			*cp2;
	cddb_path_t	*pp;

	if (*path == '\0' || *path == '\n')
		return TRUE;

	pp = (cddb_path_t *)(void *) MEM_ALLOC(
		"cddb_path_t",
		sizeof(cddb_path_t)
	);
	if (pp == NULL) {
		CDDB_FATAL(app_data.str_nomemory);
		return FALSE;
	}

	(void) memset(pp, 0, sizeof(cddb_path_t));

	if (cddb_pathhead == NULL) {
		cddb_pathhead = cddb_pathptr = pp;
	}
	else {
		cddb_pathptr->next = pp;
		cddb_pathptr = pp;
	}

	/* Determine CDDB type */
	if (strncmp(path, "cddbp://", 8) == 0) {
		/* Syntax: cddbp://host.domain[:port] */
		pp->type = CDDB_REMOTE_CDDBP;
		path += 8;
	}
	else if (strncmp(path, "http://", 7) == 0) {
		/* Syntax: http://host.domain[:port]/cgipath */
		pp->type = CDDB_REMOTE_HTTP;
		path += 7;
	}
	else if (strncmp(path, "file://", 7) == 0) {
		/* Syntax: file://dirpath */
		pp->type = CDDB_LOCAL;
		path += 7;
	}
	else if (path[0] == '@') {
		/* Syntax: @host.domain[:port] (old style for compatibility) */
		pp->type = CDDB_REMOTE_CDDBP;
		path += 1;
	}
	else {
		/* Syntax: dirpath */
		pp->type = CDDB_LOCAL;
	}

	/* Parse the rest of the line */
	switch (pp->type) {
	case CDDB_REMOTE_CDDBP:
	case CDDB_REMOTE_HTTP:
#ifdef NOREMOTE
		pp->type = CDDB_INVALID;
#else

		if ((cp2 = strchr(path, '/')) != NULL)
			*cp2 = '\0';

		if ((cp = strchr(path, ':')) != NULL)
			*cp = '\0';

		/* Get port number, if specified */
		if (cp != NULL) {
			if (!isdigit(*(cp+1)))
				pp->type = CDDB_INVALID;
			else
				pp->port = atoi(cp+1);
		}
		else {
			if (pp->type == CDDB_REMOTE_HTTP)
				pp->port = HTTP_DFLT_PORT;
			else
				pp->port = app_data.cddb_port;
		}

		/* Remote host name */
		n = strlen(path);
		if (n >= MAXHOSTNAMELEN) {
			CDDB_FATAL(app_data.str_longpatherr);
			return FALSE;
		}
		pp->host = (char *) MEM_ALLOC("pp->host", n + 1);
		if (pp->host == NULL) {
			CDDB_FATAL(app_data.str_nomemory);
			return FALSE;
		}

		(void) strcpy(pp->host, path);

		if (cp != NULL)
			*cp = ':';

		/* For http mode, get CGI path */
		if (pp->type == CDDB_REMOTE_HTTP) {
			if (cp2 == NULL) {
				/* No CGI path specified: error */
				pp->type = CDDB_INVALID;
			}
			else {
				n = strlen(cp2+1);
				if (n >= FILE_PATH_SZ) {
					CDDB_FATAL(app_data.str_longpatherr);
					return FALSE;
				}
				pp->path = (char *) MEM_ALLOC(
					"pp->path",
					n + 1
				);
				if (pp->path == NULL) {
					CDDB_FATAL(app_data.str_nomemory);
					return FALSE;
				}

				(void) strcpy(pp->path, cp2+1);
			}
		}

		if (cp2 != NULL)
			*cp2 = '/';
#endif	/* NOREMOTE */

		break;

	case CDDB_LOCAL:
		n = strlen(path);
		if (path[0] == '/') {
			/* Absolute local path name */
			pp->path = (char *) MEM_ALLOC("pp->path", n + 1);
			if (pp->path == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				return FALSE;
			}

			(void) strcpy(pp->path, path);
		}
		else if (path[0] == '~') {
			/* Perform tilde expansion a la [ck]sh */
			if (path[1] == '/') {
				cp2 = util_homedir(util_get_ouid());

				pp->path = (char *) MEM_ALLOC(
					"pp->path",
					n + strlen(cp2)
				);
				if (pp->path == NULL) {
					CDDB_FATAL(app_data.str_nomemory);
					return FALSE;
				}

				(void) sprintf(pp->path, "%s%s", cp2, &path[1]);
			}
			else if (path[1] == '\0') {
				cp2 = util_homedir(util_get_ouid());

				pp->path = (char *) MEM_ALLOC(
					"pp->path",
					strlen(cp2) + 1
				);
				if (pp->path == NULL) {
					CDDB_FATAL(app_data.str_nomemory);
					return FALSE;
				}

				(void) strcpy(pp->path, cp2);
			}
			else {
				cp = strchr(path, '/');
				if (cp == NULL) {
					cp2 = util_uhomedir(&path[1]);
					pp->path = (char *) MEM_ALLOC(
						"pp->path",
						strlen(cp2) + 1
					);
					if (pp->path == NULL) {
						CDDB_FATAL(
							app_data.str_nomemory
						);
						return FALSE;
					}

					(void) strcpy(pp->path, cp2);
				}
				else {
					*cp = '\0';
					cp2 = util_uhomedir(&path[1]);
					pp->path = (char *) MEM_ALLOC(
						"pp->path",
						n + strlen(cp2)
					);
					if (pp->path == NULL) {
						CDDB_FATAL(
							app_data.str_nomemory
						);
						return FALSE;
					}

					(void) sprintf(pp->path, CONCAT_PATH,
						       cp2, cp+1);
				}
			}
		}
		else {
			/* Relative local path name */
			pp->path = (char *) MEM_ALLOC(
				"pp->path",
				n + strlen(app_data.libdir) + 7
			);
			if (pp->path == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				return FALSE;
			}

			(void) sprintf(pp->path, REL_DBDIR_PATH,
				       app_data.libdir, path);
		}

		/* Make sure that the whole path name + discid can
		 * fit in a FILE_PATH_SZ buffer.  Also, make sure the
		 * category name is less than FILE_BASE_SZ.
		 */
		if ((int) strlen(pp->path) >= (FILE_PATH_SZ - 12) ||
		    (int) strlen(util_basename(pp->path)) >= FILE_BASE_SZ) {
			CDDB_FATAL(app_data.str_longpatherr);
			return FALSE;
		}

		break;
	}

	return TRUE;
}


/*
 * cddb_add_linkent
 *	Add an entry to the link-search list in sorted order.  Used
 *	by cddb_init_linkopts.
 *
 * Args:
 *	dtitle - Disc artist/title text string
 *	idstr - A text string representation of the magic number
 *	offset - A measure how good the track addresses match
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
cddb_add_linkent(char *dtitle, char *idstr, word32_t offset)
{
	int		n;
	cddb_linkopts_t	*p,
			*q,
			*r;

	if (dtitle == NULL || idstr == NULL)
		/* Paranoia */
		return FALSE;

	p = (cddb_linkopts_t *)(void *) MEM_ALLOC(
		"cddb_linkopts_t",
		sizeof(cddb_linkopts_t)
	);
	if (p == NULL) {
		CDDB_FATAL(app_data.str_nomemory);
		return FALSE;
	}
	p->dtitle = (char *) MEM_ALLOC("p->dtitle", strlen(dtitle) + 1);
	if (p->dtitle == NULL) {
		CDDB_FATAL(app_data.str_nomemory);
		return FALSE;
	}

	(void) strcpy(p->idstr, idstr);
	(void) strcpy(p->dtitle, dtitle);
	p->offset = offset;

	if (cddb_linkhead == NULL) {
		/* This is the first element */
		cddb_linkhead = p;
		p->next = NULL;
	}
	else {
		/* Add to list in sorted order */
		for (q = cddb_linkhead, r = NULL; q != NULL; q = q->next) {
			if ((n = util_strcasecmp(dtitle, q->dtitle)) == 0) {
				/* Already in list: no need to add */
				MEM_FREE(p->dtitle);
				MEM_FREE((char *) p);
				return TRUE;
			}
			else if (n < 0) {
				/* Track timings outside of criteria:
				 * just sort alphabetically.
				 */
				break;
			}
			r = q;
		}
		if (r == NULL) {
			p->next = cddb_linkhead;
			cddb_linkhead = p;
		}
		else {
			p->next = r->next;
			r->next = p;
		}
	}

	return TRUE;
}


/*
 * cddb_add_ident
 *	Given a disc ID string, add it to the list of disc IDs.
 *
 * Args:
 *	dbp - Pointer to the cddb_incore_t structure
 *	idstr - The disc ID string
 *
 * Return:
 *	Nothing
 */
STATIC void
cddb_add_ident(cddb_incore_t *dbp, char *id)
{
	word32_t	tmpid;
	cddb_linkopts_t	*l;

	/* Sanity check */
	if (strlen(id) != 8 || sscanf(id, "%x", &tmpid) != 1)
		/* Invalid id: do nothing */
		return;

	if (tmpid == dbp->discid)
		/* This is our own id: do nothing */
		return;

	/* Add to list */
	l = (cddb_linkopts_t *)(void *) MEM_ALLOC(
		"cddb_linkopts_t",
		sizeof(cddb_linkopts_t)
	);
	if (l == NULL) {
		CDDB_FATAL(app_data.str_nomemory);
		return;
	}

	(void) strcpy(l->idstr, id);
	l->dtitle = dbp->dtitle;
	l->offset = 0;
	l->next = dbp->idlist;
	dbp->idlist = l;
}


/*
 * cddb_add_idlist
 *	Add disc IDs in the list string to the in-core list of disc IDs.
 *
 * Args:
 *	dbp - Pointer to the cddb_incore_t structure
 *	idstr - The disc ID list string
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
STATIC bool_t
cddb_add_idlist(cddb_incore_t *dbp, char *idstr)
{
	char	*p,
		*q;

	if (idstr == NULL || *idstr == '\0')
		return FALSE;

	p = idstr;
	while ((q = strchr(p, CDDBID_SEPCHAR)) != NULL) {
		*q = '\0';
		cddb_add_ident(dbp, p);
		p = q + 1;
	}
	cddb_add_ident(dbp, p);

	return TRUE;
}


/*
 * cddb_free_idlist
 *	Free the idlist in the incore CDDB structure.
 *
 * Args:
 *	dbp - Pointer to the cddb_incore_t structure
 *
 * Return:
 *	Nothing
 */
STATIC void
cddb_free_idlist(cddb_incore_t *dbp)
{
	cddb_linkopts_t	*p,
			*q;

	for (p = q = dbp->idlist; p != NULL; p = q) {
		q = p->next;
		MEM_FREE(p);
	}
	dbp->idlist = NULL;
}


/*
 * cddb_open
 *	Open CD database entry and return a file descriptor.  If
 *	the open is for reading, this routine will search for the
 *	appropriate entry.  Currently, opens for reading supports
 *	local database directories as well as internet remote
 *	databases, but writing is supported on local directories only.
 *
 *	The dbp->discid and dbp->queryid field should be filled prior
 *	to calling this function.
 *
 * Args:
 *	dbp - Pointer to the cddb_incore_t structure
 *	s - Pointer to the curstat structure
 *	rw - Open for read or write (O_RDONLY, O_WRONLY, etc.)
 *
 * Return:
 *	The CDDB file pointer, or NULL on failure.
 */
STATIC cddb_file_t *
cddb_open(cddb_incore_t *dbp, curstat_t *s, int rw)
{
	cddb_file_t	*fp;
	cddb_path_t	*pp;
	char		dbfile[FILE_PATH_SZ];

	/* Allocate cddb_file_t */
	fp = (cddb_file_t *)(void *) MEM_ALLOC(
		"cddb_file_t",
		sizeof(cddb_file_t)
	);
	if (fp == NULL) {
		CDDB_FATAL(app_data.str_nomemory);
		return NULL;
	}

	/* Allocate storage for I/O cache */
	fp->cache = (unsigned char *) MEM_ALLOC("fp->cache", CDDB_CACHESZ);
	if (fp->cache == NULL) {
		CDDB_FATAL(app_data.str_nomemory);
		return NULL;
	}

	fp->magic = CDDB_FMAGIC;
	fp->rw = rw;
	fp->pos = fp->cnt = 0;

	if (rw & O_WRONLY) {
		/* Open file */
		if ((fp->fd = open(dbp->dbfile, rw)) < 0) {
			MEM_FREE(fp->cache);	
			MEM_FREE(fp);	
			return NULL;
		}

		fp->type = CDDB_LOCAL;
	}
	else {
		DBGPRN(errfp, "\nCDDB QUERY discid=%08x queryid=%08x:\n",
			dbp->discid, dbp->queryid);

		/* Loop through all database paths */
		for (pp = cddb_pathhead; pp != NULL; pp = pp->next) {
			switch (pp->type) {
			case CDDB_LOCAL:
				DBGPRN(errfp, "Local CDDB %s\n", pp->path);

				(void) sprintf(dbfile, CDDBFILE_PATH,
					       pp->path, dbp->queryid);

				if ((fp->fd = open(dbfile, rw)) < 0)
					break;

				fp->type = pp->type;
				fp->pathp = pp;

				/* Record the category */
				(void) strcpy(dbp->category,
					      cddb_category(pp->path));

				/* Record the path to the database file. */
				dbp->dbfile = (char *)
					MEM_ALLOC("dbp->dbfile", strlen(dbfile) + 1);
				if (dbp->dbfile == NULL) {
					CDDB_FATAL(app_data.str_nomemory);
					return NULL;
				}
				(void) sprintf(dbp->dbfile, CDDBFILE_PATH,
						pp->path, dbp->discid);

				/* If discid != queryid, perform a link */
				if (dbp->discid != dbp->queryid) {
					(void) cddb_link(dbp,
							 dbfile, dbp->dbfile);
				}

				DBGPRN(errfp, "Found Local: %s\n", dbfile);
				break;

			case CDDB_REMOTE_CDDBP:
			case CDDB_REMOTE_HTTP:
				if (app_data.cddb_rmtdsbl) {
					/* Remote queries disabled */
					DBGPRN(errfp,
						"%s http://%s:%d/%s SKIPPED\n",
						"Remote CDDB",
						pp->host, pp->port, pp->path);
					fp->fd = -1;
					break;
				}

				if (pp->type == CDDB_REMOTE_HTTP) {
					/* http method */
					DBGPRN(errfp,
						"%s http://%s:%d/%s\n",
						"Remote CDDB",
						pp->host, pp->port, pp->path);
				}
				else {
					/* cddbp method */
					DBGPRN(errfp,
						"%s cddbp://%s:%d\n",
						"Remote CDDB",
						pp->host, pp->port);
				}

				fp->type = pp->type;
				fp->pathp = pp;

				if (!cddb_do_remote_query(fp, dbp, s))
					fp->fd = -1;

				break;

			case CDDB_INVALID:
			default:
				fp->fd = -1;
				break;
			}

			if (fp->fd >= 0)
				/* Found a CDDB entry */
				break;
		}

		if (fp->fd < 0) {
			MEM_FREE(fp->cache);	
			MEM_FREE(fp);	
			return NULL;
		}
	}

	return (fp);
}


/*
 * cddb_close
 *	Close the file that was previously opened via cddb_open().
 *
 * Args:
 *	fp - CDDB file pointer
 *
 * Return:
 *	TRUE - Success
 *	FALSE - Failure
 */
STATIC bool_t
cddb_close(cddb_file_t *fp)
{
	int	n;

	/* Sanity check */
	if (fp->magic != CDDB_FMAGIC)
		return FALSE;

	if ((fp->rw & O_WRONLY) && fp->pos > 0) {
		/* Flush write cache */
		n = write(fp->fd, fp->cache, fp->pos);

		if (n < 0 || n != fp->pos) {
			fp->pos = 0;
			return FALSE;
		}
	}

	/* Close file */
	if (close(fp->fd) < 0)
		return FALSE;

	if (fp->type == CDDB_REMOTE_CDDBP || fp->type == CDDB_REMOTE_HTTP) {
		DBGPRN(errfp, "Closed %sconnection: %s\n",
			(fp->type == CDDB_REMOTE_HTTP && app_data.http_proxy) ?
				"proxy " : "",
			(fp->type == CDDB_REMOTE_HTTP && app_data.http_proxy) ?
				cddb_proxy.host : fp->pathp->host);
	}

	/* Free cache storage */
	MEM_FREE(fp->cache);

	/* Free cddb_file_t structure */
	MEM_FREE(fp);

	return TRUE;
}


#ifdef __VMS

/*
 * cddb_read0
 *	Read CDDB data and update incore structures.
 *
 * Args:
 *	fp - CDDB file pointer
 *	dbp - Pointer to the cddb_incore_t structure
 *	s - Pointer to curstat_t structure.
 *
 * Return:
 *	TRUE - Success
 *	FALSE - Failure
 */
STATIC bool_t
cddb_read0(cddb_file_t *fp, cddb_incore_t *dbp, curstat_t *s)
{
	int	pos,
		revision,
		bufsz = CDDBP_CMDLEN;
	char	*buf,
		*tmpbuf;
	bool_t	remote;

	/* Initialize */
	dbp->type = CDDB_WAIT;
	dbp->revision = 0;
	errno = 0;

	if ((buf = (char *) MEM_ALLOC("read0_buf", bufsz)) == NULL) {
		CDDB_FATAL(app_data.str_nomemory);
		return FALSE;
	}

	if (dbp->flags & CDDB_INEXACT || dbp->flags & CDDB_AUTHFAIL) {
		dbp->type = fp->type;
		if (fp->type == CDDB_REMOTE_CDDBP) {
			/* Close down connection */
			(void) sprintf(buf, "%s\r\n", CDDB_QUIT);
			(void) cddb_remote_cmd(fp, buf, bufsz);
		}
		MEM_FREE(buf);
		return TRUE;
	}

	if ((tmpbuf = (char *) MEM_ALLOC("read0_tmpbuf", bufsz)) == NULL) {
		CDDB_FATAL(app_data.str_nomemory);
		return FALSE;
	}

	switch (fp->type) {
	case CDDB_REMOTE_HTTP:
		/* Open a new connection */
		if (!cddb_remote_open(fp))
			return FALSE;

		errno = 0;
		/*FALLTHROUGH*/

	case CDDB_REMOTE_CDDBP:
		remote = TRUE;

		/* Set up for remote CDDB server read */
		if (!cddb_cmd_read(fp, buf, bufsz, dbp)) {
			dbp->type = CDDB_INVALID;
			MEM_FREE(buf);
			MEM_FREE(tmpbuf);
			return FALSE;
		}
		break;

	default:
		break;
	}

	/* Read first line of database entry */
	if (!cddb_gets(fp, buf, bufsz, TRUE)) {
		dbp->type = CDDB_INVALID;
		MEM_FREE(buf);
		MEM_FREE(tmpbuf);
		return FALSE;
	}

	if (remote) {
		DBGPRN(errfp, "<- [CDDB data ...]\n");
	}

	/* Database file signature check */
	if (strncmp(buf, "# xmcd ", 7) != 0) {
		/* Not a supported database file */
		dbp->type = CDDB_INVALID;
		MEM_FREE(buf);
		MEM_FREE(tmpbuf);
		return FALSE;
	}

	/* Read the rest of the database entry */
	while (cddb_gets(fp, buf, bufsz, TRUE)) {
		/* Comment line */
		if (buf[0] == '#') {
			/* Concatenated cddb file */
			if (strncmp(buf, "# xmcd ", 7) == 0)
				break;

			/* Get CDDB file revision, if specified */
			if (sscanf(buf, "# Revision: %d\n", &revision) == 1)
				dbp->revision = revision;

			continue;
		}

		/* End-of-data reached */
		if (buf[0] == '.') {
			if (remote) {
				DBGPRN(errfp, "<- %s", buf);
			}
			break;
		}

		buf[strlen(buf)-1] = '\n';

		/* Disc IDs */
		if (sscanf(buf, "DISCID=%[^\n]\n", tmpbuf) > 0) {
			(void) cddb_add_idlist(dbp, tmpbuf);
			continue;
		}

		/* Disk title */
		if (sscanf(buf, "DTITLE=%[^\n]\n", tmpbuf) > 0) {
			cddb_line_filter(tmpbuf);

			if (dbp->dtitle == NULL) {
				dbp->dtitle = (char *) MEM_ALLOC(
					"dbp->dtitle",
					strlen(tmpbuf) + 1
				);
				if (dbp->dtitle != NULL)
					dbp->dtitle[0] = '\0';
			}
			else {
				dbp->dtitle = (char *) MEM_REALLOC(
					"dbp->dtitle",
					dbp->dtitle,
					strlen(dbp->dtitle) +
						strlen(tmpbuf) + 1
				);
			}

			if (dbp->dtitle == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				break;
			}

			(void) cddb_strcat(dbp->dtitle, tmpbuf);
			continue;
		}

		/* Track title */
		if (sscanf(buf, "TTITLE%u=%[^\n]\n", &pos, tmpbuf) >= 2) {
			if (pos >= (int) (dbp->discid & 0xff))
				continue;

			cddb_line_filter(tmpbuf);

			if (dbp->trklist[pos] == NULL) {
				dbp->trklist[pos] = (char *) MEM_ALLOC(
					"dbp->trklist[n]",
					strlen(tmpbuf) + 1
				);
				if (dbp->trklist[pos] != NULL)
					dbp->trklist[pos][0] = '\0';
				
			}
			else {
				dbp->trklist[pos] = (char *) MEM_REALLOC(
					"dbp->trklist[n]",
					dbp->trklist[pos],
					strlen(dbp->trklist[pos]) +
						strlen(tmpbuf) + 1
				);
			}

			if (dbp->trklist[pos] == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				break;
			}

			(void) cddb_strcat(dbp->trklist[pos], tmpbuf);
			continue;
		}

		/* Disk extended info */
		if (sscanf(buf, "EXTD=%[^\n]\n", tmpbuf) > 0) {
			if (dbp->extd == NULL) {
				dbp->extd = (char *) MEM_ALLOC(
					"dbp->extd",
					strlen(tmpbuf) + 1
				);
				if (dbp->extd != NULL)
					dbp->extd[0] = '\0';
			}
			else {
				dbp->extd = (char *) MEM_REALLOC(
					"dbp->extd",
					dbp->extd,
					strlen(dbp->extd) +
						strlen(tmpbuf) + 1
				);
			}

			if (dbp->extd == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				break;
			}

			(void) cddb_strcat(dbp->extd, tmpbuf);
			continue;
		}

		/* Track extended info */
		if (sscanf(buf, "EXTT%u=%[^\n]\n", &pos, tmpbuf) >= 2) {
			if (pos >= (int) (dbp->discid & 0xff))
				continue;

			if (dbp->extt[pos] == NULL) {
				dbp->extt[pos] = (char *) MEM_ALLOC(
					"dbp->extt[n]",
					strlen(tmpbuf) + 1
				);

				if (dbp->extt[pos] != NULL)
					dbp->extt[pos][0] = '\0';
			}
			else {
				dbp->extt[pos] = (char *) MEM_REALLOC(
					"dbp->extt[n]",
					dbp->extt[pos],
					strlen(dbp->extt[pos]) +
						strlen(tmpbuf) + 1
				);
			}

			if (dbp->extt[pos] == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				break;
			}

			(void) cddb_strcat(dbp->extt[pos], tmpbuf);
			continue;
		}

		/* Play order */
		if (sscanf(buf, "PLAYORDER=%[^\n]\n", tmpbuf) > 0) {
			if (s->program || s->shuffle)
				/* Play program or shuffle already in
				 * progress, do not override it.
				 */
				continue;

			if (dbp->playorder == NULL) {
				dbp->playorder = (char *) MEM_ALLOC(
					"dbp->playorder",
					strlen(tmpbuf) + 1
				);
				if (dbp->playorder != NULL)
					dbp->playorder[0] = '\0';
			}
			else {
				dbp->playorder = (char *) MEM_REALLOC(
					"dbp->playorder",
					dbp->playorder,
					strlen(dbp->playorder) +
						strlen(tmpbuf) + 1
				);
			}

			if (dbp->playorder == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				break;
			}

			(void) cddb_strcat(dbp->playorder, tmpbuf);
			continue;
		}
	}

	/* Update database type */
	dbp->type = (errno != 0) ? CDDB_INVALID : fp->type;

	/* If CDDB file revision is not specified or < 0, set it to 1 */
	if (dbp->revision <= 0)
		dbp->revision = 1;

	MEM_FREE(buf);
	MEM_FREE(tmpbuf);

	if (fp->type == CDDB_REMOTE_CDDBP) {
		/* Close down connection */
		(void) sprintf(buf, "%s\r\n", CDDB_QUIT);
		(void) cddb_remote_cmd(fp, buf, bufsz);
	}

	/* For CDDB_REMOTE_HTTP, let caller handle last close via
	 * cddb_close().
	 */

	return (dbp->type != CDDB_INVALID);
}

#else	/* __VMS */

/*
 * cddb_read1
 *	Read CDDB data and write into pipe.  Used by child process.
 *
 * Args:
 *	fp - CDDB file pointer
 *	pfd - Pipe descriptor to write CDDB data into
 *	dbp - Pointer to the cddb_incore_t structure
 *
 * Return:
 *	TRUE - Success
 *	FALSE - Failure
 */
STATIC bool_t
cddb_read1(cddb_file_t *fp, cddb_file_t *pfp, cddb_incore_t *dbp)
{
	int		revision,
			bufsz = CDDBP_CMDLEN;
	cddb_match_t	*p;
	char		*buf,
			*lbuf;
	bool_t		remote;

	/* Initialize */
	dbp->revision = 0;
	errno = 0;

	if ((buf = (char *) MEM_ALLOC("read1_buf", bufsz)) == NULL)
		return FALSE;

	/* CDDB category */
	if (dbp->category[0] != '\0') {
		(void) sprintf(buf, "CATEGORY=%s\n", dbp->category);
		if (!cddb_puts(pfp, buf)) {
			MEM_FREE(buf);
			return FALSE;
		}
	}

	/* Flags */
	(void) sprintf(buf, "FLAGS=%x\n", dbp->flags);
	if (!cddb_puts(pfp, buf)) {
		MEM_FREE(buf);
		return FALSE;
	}

	remote = FALSE;

	switch (fp->type) {
	case CDDB_LOCAL:
		/* CDDB file path */
		if (dbp->dbfile != NULL) {
			(void) sprintf(buf, "FILEPATH=%s\n", dbp->dbfile);
			if (!cddb_puts(pfp, buf)) {
				MEM_FREE(buf);
				return FALSE;
			}
		}
		break;

	case CDDB_REMOTE_HTTP:
	case CDDB_REMOTE_CDDBP:
		remote = TRUE;

		/* Match entry list */
		for (p = dbp->matchlist; p != NULL; p = p->next) {
			lbuf = (char *) MEM_ALLOC(
				"lbuf",
				strlen(p->dtitle) + FILE_BASE_SZ + 24
			);
			if (lbuf == NULL) {
				MEM_FREE(buf);
				return FALSE;
			}

			(void) sprintf(lbuf, "RMATCH=%s %08x %s\n",
				       p->category, p->discid, p->dtitle);

			if (!cddb_puts(pfp, lbuf)) {
				MEM_FREE(lbuf);
				MEM_FREE(buf);
				return FALSE;
			}

			MEM_FREE(lbuf);
		}
		break;

	default:
		break;
	}

	if (dbp->flags & CDDB_INEXACT || dbp->flags & CDDB_AUTHFAIL) {
		if (fp->type == CDDB_REMOTE_CDDBP) {
			/* Close down connection */
			(void) sprintf(buf, "%s\r\n", CDDB_QUIT);
			(void) cddb_remote_cmd(fp, buf, bufsz);
		}
		MEM_FREE(buf);
		return TRUE;
	}

	if (remote) {
		/* For http: Open a new connection */
		if (fp->type == CDDB_REMOTE_HTTP &&
		    !cddb_remote_open(fp)) {
			MEM_FREE(buf);
			return FALSE;
		}

		errno = 0;

		/* Set up for remote CDDB server read */
		if (!cddb_cmd_read(fp, buf, bufsz, dbp)) {
			MEM_FREE(buf);
			return FALSE;
		}
	}

	/* Read database entry */
	if (!cddb_gets(fp, buf, bufsz, FALSE)) {
		MEM_FREE(buf);
		return FALSE;
	}

	if (remote) {
		DBGPRN(errfp, "<- [CDDB data ...]\n");
	}

	/* Database file signature check */
	if (strncmp(buf, "# xmcd ", 7) != 0) {
		/* Not a supported database file */
		MEM_FREE(buf);
		return FALSE;
	}

	/* Read the database file */
	while (cddb_gets(fp, buf, bufsz, FALSE)) {
		/* End-of-data reached */
		if (buf[0] == '.') {
			if (remote) {
				DBGPRN(errfp, "<- %s", buf);
			}
			break;
		}

		/* Comment line */
		if (buf[0] == '#') {
			/* Concatenated cddb file */
			if (strncmp(buf, "# xmcd ", 7) == 0)
				break;

			/* Get CDDB file revision, if specified */
			if (sscanf(buf, "# Revision: %d\n", &revision) == 1)
				dbp->revision = revision;

			continue;
		}

		buf[strlen(buf)-1] = '\n';

		/* Write CDDB data into pipe */
		if (!cddb_puts(pfp, buf)) {
			MEM_FREE(buf);
			return FALSE;
		}
	}

	/* CDDB type */
	dbp->type = (errno != 0) ? CDDB_INVALID : fp->type;
	(void) sprintf(buf, "CDDBTYPE=%d\n", dbp->type);
	if (!cddb_puts(pfp, buf)) {
		MEM_FREE(buf);
		return FALSE;
	}

	/* Revision */
	if (dbp->revision <= 0)
		dbp->revision = 1;
	
	(void) sprintf(buf, "REVISION=%d\n", dbp->revision);
	if (!cddb_puts(pfp, buf)) {
		MEM_FREE(buf);
		return FALSE;
	}

	if (fp->type == CDDB_REMOTE_CDDBP) {
		/* Close down connection */
		(void) sprintf(buf, "%s\r\n", CDDB_QUIT);
		(void) cddb_remote_cmd(fp, buf, bufsz);
	}

	/* For CDDB_REMOTE_HTTP, let caller handle last close via
	 * cddb_close().
	 */

	return (dbp->type != CDDB_INVALID);
}


/*
 * cddb_read2
 *	Read CDDB data from pipe and update in-core structures.
 *	Used by parent process.
 *
 * Args:
 *	pfd - Pipe descriptor to write CDDB data into.
 *	dbp - Pointer to cddb_incore_t structure.
 *	s - Pointer to curstat_t structure.
 *
 * Return:
 *	TRUE - Success
 *	FALSE - Failure
 */
/*ARGSUSED*/
STATIC bool_t
cddb_read2(cddb_file_t *pfp, cddb_incore_t *dbp, curstat_t *s)
{
	int		bufsz = CDDBP_CMDLEN;
	unsigned int	pos;
	cddb_match_t	*p;
	char		*buf,
			*tmpbuf;

	/* Initialize */
	dbp->type = CDDB_WAIT;
	dbp->revision = 0;

	buf = (char *) MEM_ALLOC("read2_buf", bufsz);
	tmpbuf = (char *) MEM_ALLOC("read2_tmpbuf", bufsz);
	if (buf == NULL || tmpbuf == NULL) {
		CDDB_FATAL(app_data.str_nomemory);
		return FALSE;
	}

	/* Read the database entry */
	while (cddb_gets(pfp, buf, bufsz, TRUE)) {
		/* Disc IDs */
		if (sscanf(buf, "DISCID=%[^\n]\n", tmpbuf) > 0) {
			(void) cddb_add_idlist(dbp, tmpbuf);
			continue;
		}

		/* Disk title */
		if (sscanf(buf, "DTITLE=%[^\n]\n", tmpbuf) > 0) {
			cddb_line_filter(tmpbuf);

			if (dbp->dtitle == NULL) {
				dbp->dtitle = (char *) MEM_ALLOC(
					"dbp->dtitle",
					strlen(tmpbuf) + 1
				);
				if (dbp->dtitle != NULL)
					dbp->dtitle[0] = '\0';
			}
			else {
				dbp->dtitle = (char *) MEM_REALLOC(
					"dbp->dtitle",
					dbp->dtitle,
					strlen(dbp->dtitle) +
						strlen(tmpbuf) + 1
				);
			}

			if (dbp->dtitle == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				break;
			}

			(void) cddb_strcat(dbp->dtitle, tmpbuf);
			continue;
		}

		/* Track title */
		if (sscanf(buf, "TTITLE%u=%[^\n]\n", &pos, tmpbuf) >= 2) {
			if (pos >= (int) (dbp->discid & 0xff))
				continue;

			cddb_line_filter(tmpbuf);

			if (dbp->trklist[pos] == NULL) {
				dbp->trklist[pos] = (char *) MEM_ALLOC(
					"dbp->trklist[n]",
					strlen(tmpbuf) + 1
				);
				if (dbp->trklist[pos] != NULL)
					dbp->trklist[pos][0] = '\0';
				
			}
			else {
				dbp->trklist[pos] = (char *) MEM_REALLOC(
					"dbp->trklist[n]",
					dbp->trklist[pos],
					strlen(dbp->trklist[pos]) +
						strlen(tmpbuf) + 1
				);
			}

			if (dbp->trklist[pos] == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				break;
			}

			(void) cddb_strcat(dbp->trklist[pos], tmpbuf);
			continue;
		}

		/* Disk extended info */
		if (sscanf(buf, "EXTD=%[^\n]\n", tmpbuf) > 0) {
			if (dbp->extd == NULL) {
				dbp->extd = (char *) MEM_ALLOC(
					"dbp->extd",
					strlen(tmpbuf) + 1
				);
				if (dbp->extd != NULL)
					dbp->extd[0] = '\0';
			}
			else {
				dbp->extd = (char *) MEM_REALLOC(
					"dbp->extd",
					dbp->extd,
					strlen(dbp->extd) +
						strlen(tmpbuf) + 1
				);
			}

			if (dbp->extd == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				break;
			}

			(void) cddb_strcat(dbp->extd, tmpbuf);
			continue;
		}

		/* Track extended info */
		if (sscanf(buf, "EXTT%u=%[^\n]\n", &pos, tmpbuf) >= 2) {
			if (pos >= (int) (dbp->discid & 0xff))
				continue;

			if (dbp->extt[pos] == NULL) {
				dbp->extt[pos] = (char *) MEM_ALLOC(
					"dbp->extt[n]",
					strlen(tmpbuf) + 1
				);
				if (dbp->extt[pos] != NULL)
					dbp->extt[pos][0] = '\0';
			}
			else {
				dbp->extt[pos] = (char *) MEM_REALLOC(
					"dbp->extt[n]",
					dbp->extt[pos],
					strlen(dbp->extt[pos]) +
						strlen(tmpbuf) + 1
				);
			}

			if (dbp->extt[pos] == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				break;
			}

			(void) cddb_strcat(dbp->extt[pos], tmpbuf);
			continue;
		}

		/* Play order */
		if (sscanf(buf, "PLAYORDER=%[^\n]\n", tmpbuf) > 0) {
			if (s->program || s->shuffle)
				/* Play program or shuffle already in
				 * progress, do not override it.
				 */
				continue;

			if (dbp->playorder == NULL) {
				dbp->playorder = (char *) MEM_ALLOC(
					"dbp->playorder",
					strlen(tmpbuf) + 1
				);
				if (dbp->playorder != NULL)
					dbp->playorder[0] = '\0';
			}
			else {
				dbp->playorder = (char *) MEM_REALLOC(
					"dbp->playorder",
					dbp->playorder,
					strlen(dbp->playorder) +
						strlen(tmpbuf) + 1
				);
			}

			if (dbp->playorder == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				break;
			}

			(void) cddb_strcat(dbp->playorder, tmpbuf);
			continue;
		}

		/* Database file type */
		if (sscanf(buf, "CDDBTYPE=%d\n", &dbp->type) > 0)
			continue;

		/* Database category */
		if (sscanf(buf, "CATEGORY=%s\n", dbp->category) > 0)
			continue;

		/* Flags */
		if (sscanf(buf, "FLAGS=%x\n", (int *) &dbp->flags) > 0)
			continue;

		/* Database file path */
		if (sscanf(buf, "FILEPATH=%s\n", tmpbuf) > 0) {
			if (dbp->dbfile == NULL) {
				dbp->dbfile = (char *) MEM_ALLOC(
					"dbp->dbfile",
					strlen(tmpbuf) + 1
				);
				if (dbp->dbfile != NULL)
					dbp->dbfile[0] = '\0';
			}
			else {
				dbp->dbfile = (char *) MEM_REALLOC(
					"dbp->dbfile",
					dbp->dbfile,
					strlen(dbp->dbfile) +
						strlen(tmpbuf) + 1
				);
			}

			if (dbp->dbfile == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				break;
			}

			(void) cddb_strcat(dbp->dbfile, tmpbuf);
			continue;
		}

		/* Match list entry */
		if (sscanf(buf, "RMATCH=%[^\n]\n", tmpbuf) > 0) {
			p = (cddb_match_t *)(void *) MEM_ALLOC(
				"cddb_match_t",
				sizeof(cddb_match_t)
			);
			if (p == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				break;
			}

			p->dtitle = (char *) MEM_ALLOC(
				"p->dtitle",
				strlen(tmpbuf)
			);
			if (p->dtitle == NULL) {
				CDDB_FATAL(app_data.str_nomemory);
				break;
			}

			if (sscanf(tmpbuf, "%s %x %[^\n]\n",
				   p->category, &p->discid, p->dtitle) < 3) {
				MEM_FREE(p->dtitle);
				MEM_FREE(p);
				continue;
			}

			/* Add to list */
			p->next = dbp->matchlist;
			dbp->matchlist = p;
			continue;
		}

		/* Revision */
		if (sscanf(buf, "REVISION=%d\n", &dbp->revision) > 0)
			continue;

	}

	if (dbp->type == CDDB_WAIT)
		dbp->type = CDDB_INVALID;

	MEM_FREE(buf);
	MEM_FREE(tmpbuf);

	return (dbp->type != CDDB_INVALID);
}

#endif	/* __VMS */


/*
 * cddb_write
 *	Write the cddb_incore_t structure into the appropriate CD
 *	database file.
 *
 * Args:
 *	fp - CDDB file pointer
 *	dbp - Pointer to the cddb_incore_t structure
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	TRUE - Success
 *	FALSE - Failure
 */
STATIC bool_t
cddb_write(cddb_file_t *fp, cddb_incore_t *dbp, curstat_t *s)
{
	int		i;
	cddb_linkopts_t	*p;
	char		idstr[12],
			tmpbuf[STR_BUF_SZ * 2];

	/* File header */
	(void) sprintf(tmpbuf, "# xmcd CD database file\n# %s\n#\n", COPYRIGHT);
	if (!cddb_puts(fp, tmpbuf))
		return FALSE;

	/* Track frame offsets */
	if (!cddb_puts(fp, "# Track frame offsets:\n"))
		return FALSE;
	for (i = 0; i < (int) s->tot_trks; i++) {
		(void) sprintf(tmpbuf, "#\t%u\n",
			       s->trkinfo[i].addr + MSF_OFFSET);
		if (!cddb_puts(fp, tmpbuf))
			return FALSE;
	}

	/* Disc length */
	(void) sprintf(tmpbuf, "#\n# Disc length: %u seconds\n#\n",
		       s->trkinfo[(int) s->tot_trks].min * 60 +
		       s->trkinfo[(int) s->tot_trks].sec);
	if (!cddb_puts(fp, tmpbuf))
		return FALSE;

	/* Revision */
	(void) sprintf(tmpbuf, "# Revision: %d\n", dbp->revision);
	if (!cddb_puts(fp, tmpbuf))
		return FALSE;

	/* Submitter */
	(void) sprintf(tmpbuf, "# Submitted via: xmcd %s%s PL%d\n#\n",
		       VERSION, VERSION_EXT, PATCHLEVEL);
	if (!cddb_puts(fp, tmpbuf))
		return FALSE;

	/* Disc IDs */
	(void) sprintf(idstr, "%08x", dbp->discid);
	(void) sprintf(tmpbuf, "DISCID=%s", idstr);
	i = 1;
	for (p = dbp->idlist; p != NULL; p = p->next) {
		if (strncmp(p->idstr, idstr, 8) == 0)
			/* This is our own ID, which we already processed */
			continue;

		if (i == 0)
			(void) sprintf(tmpbuf, "DISCID=%s", p->idstr);
		else
			(void) sprintf(tmpbuf, "%s%c%s", tmpbuf,
				       CDDBID_SEPCHAR, p->idstr);

		i++;
		if (i == 8) {
			(void) strcat(tmpbuf, "\n");
			if (!cddb_puts(fp, tmpbuf))
				return FALSE;
			i = 0;
		}
	}
	if (i != 0) {
		(void) strcat(tmpbuf, "\n");
		if (!cddb_puts(fp, tmpbuf))
			return FALSE;
	}

	/* Disc artist/title */
	if (!cddb_putentry(fp, "DTITLE", dbp->dtitle))
		return FALSE;

	/* Track titles */
	for (i = 0; i < (int) s->tot_trks; i++) {
		(void) sprintf(idstr, "TTITLE%u", i);
		if (!cddb_putentry(fp, idstr, dbp->trklist[i]))
			return FALSE;
	}

	/* Extended disc information */
	if (!cddb_putentry(fp, "EXTD", dbp->extd))
		return FALSE;

	/* Extended track information */
	for (i = 0; i < (int) s->tot_trks; i++) {
		(void) sprintf(idstr, "EXTT%u", i);
		if (!cddb_putentry(fp, idstr, dbp->extt[i]))
			return FALSE;
	}

	/* Track program sequence */
	if (!cddb_putentry(fp, "PLAYORDER",
			   s->onetrk_prog ? NULL : dbp->playorder))
		return FALSE;

	return TRUE;
}


/*
 * cddb_setperm
 *	Set the file permissions of the CD database file.  The
 *	dbp->dbfile field must be filled prior to calling this
 *	function.
 *
 * Args:
 *	path - file path name
 *
 * Return:
 *	Nothing
 */
STATIC void
cddb_setperm(char *path)
{
	unsigned int	mode;

	/* Set the database file permissions */
	(void) sscanf(app_data.cddb_filemode, "%o", &mode);

	/* Make sure the file is at least readable to the user just
	 * in case mode is bogus.
	 */
	mode |= S_IRUSR;

	/* Turn off extraneous bits */
	mode &= ~(S_ISUID | S_ISGID | S_IXUSR | S_IXGRP | S_IXOTH);

	/* Set file permission */
	(void) chmod(path, (mode_t) mode);
}


/*
 * cddb_save_ent
 *	Save a CDDB entry
 *
 * Args:
 *	dbp - Pointer to the cddb_incore_t structure
 *	s - Pointer to the curstat_t structure
 *	pp - Pointer to the cddb_path_t structure that corresponds to
 *	     the directory to save to
 *
 * Return:
 *	Nothing
 */
STATIC void
cddb_save_ent(cddb_incore_t *dbp, curstat_t *s, cddb_path_t *pp)
{
	if (dbp->dbfile != NULL)
		MEM_FREE(dbp->dbfile);
	
	dbp->dbfile = (char *) MEM_ALLOC("dbp->dbfile", strlen(pp->path) + 10);
	if (dbp->dbfile == NULL) {
		CDDB_FATAL(app_data.str_nomemory);
		return;
	}
	
	(void) sprintf(dbp->dbfile, CDDBFILE_PATH, pp->path, dbp->queryid);
	
	/* Write to local database */
	if (cddb_save(dbp, s) == 0)
		dbp->flags |= CDDB_AUTOSAVE;
}


/*
 * cddb_autosave
 *	For remote CDDB queries, if the local CDDB contains a
 *	matching directory, save the entry into the local database.
 *
 * Args:
 *	dbp - Pointer to the cddb_incore_t structure
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing
 */
STATIC void
cddb_autosave(cddb_incore_t *dbp, curstat_t *s)
{
	cddb_path_t	*pp;
	char		categ[FILE_PATH_SZ];
	
	if (dbp->type != CDDB_REMOTE_CDDBP && dbp->type != CDDB_REMOTE_HTTP)
		return;

	/* Loop through local database paths and look for
	 * the appropriate category directory.
	 */
	for (pp = cddb_pathhead; pp != NULL; pp = pp->next) {
		if (pp->type == CDDB_LOCAL) {
			(void) strcpy(categ, cddb_category(pp->path));
			if (strcmp(categ, dbp->category) == 0) {
				cddb_save_ent(dbp, s, pp);
				break;
			}
		}
	}

	/* If no category matches, try to use the first local directory */
	if ((dbp->flags & CDDB_AUTOSAVE) == 0) {
		for (pp = cddb_pathhead; pp != NULL; pp = pp->next) {
			if (pp->type == CDDB_LOCAL) {
				cddb_save_ent(dbp, s, pp);
				break;
			}
		}
	}

	if ((dbp->flags & CDDB_AUTOSAVE) == 0) {
		CDDB_WARNING(app_data.str_saverr_write);
	}
}


/*
 * cddb_check_entry
 *	Check if a CDDB file meets adequete criteria for sending
 *	to a central archive server.
 *
 * Args:
 *	dbp - Pointer to the cddb_incore_t structure
 *
 * Return:
 *	TRUE if success
 *	FALSE if failure
 */
bool_t
cddb_check_entry(cddb_incore_t *dbp)
{
	int	i,
		lineno,
		bufsz = STR_BUF_SZ * 2;
	char	*buf,
		*p;
	FILE	*fp;
	bool_t	has_frameoffs = FALSE,
		has_disclen = FALSE,
		has_discid = FALSE,
		has_dtitle = FALSE,
		has_ttitle = FALSE;

	DBGPRN(errfp, "cddb_check_entry: %s\n", dbp->dbfile);

	if ((fp = fopen(dbp->dbfile, "r")) == NULL)
		return FALSE;

	if ((buf = (char *) MEM_ALLOC("check_entry_buf", bufsz)) == NULL) {
		(void) fclose(fp);
		return FALSE;
	}

	for (lineno = 0; fgets(buf, bufsz, fp) != NULL; lineno++) {
		/* Check for CDDB file signature: abort early if not found
		 * at top of file.
		 */
		if (lineno == 0 && strncmp(buf, "# xmcd", 6) != 0) {
			/* Invalid xmcd CDDB file */
			MEM_FREE(buf);
			(void) fclose(fp);
			return FALSE;
		}
		/* Check for track frame offsets */
		if (strncmp(buf, "# Track frame offsets:", 22) == 0) {
			has_frameoffs = TRUE;
			continue;
		}
		/* Check for disc length information */
		if (strncmp(buf, "# Disc length:", 14) == 0) {
			has_disclen = TRUE;
			continue;
		}
		/* Check for Disc ID */
		if (strncmp(buf, "DISCID=", 7) == 0) {
			has_discid = TRUE;
			continue;
		}
		/* Check for Disc artist/title */
		if (strncmp(buf, "DTITLE=", 7) == 0) {
			/* Eat newline */
			i = strlen(buf) - 1;
			if (buf[i] == '\n')
				buf[i] = '\0';

			p = cddb_skip_whitespace(buf + 7);
			if (*p != '\0')
				has_dtitle = TRUE;

			continue;
		}
		/* Check for at least one non-blank track title */
		if (strncmp(buf, "TTITLE", 6) == 0) {
			/* Eat newline */
			i = strlen(buf) - 1;
			if (buf[i] == '\n')
				buf[i] = '\0';

			p = strchr(buf, '=');
			if (p == NULL || !isdigit(*(p-1))) {
				/* Invalid xmcd CDDB file */
				MEM_FREE(buf);
				(void) fclose(fp);
				return FALSE;
			}

			p = cddb_skip_whitespace(p + 1);
			if (*p != '\0')
				has_ttitle = TRUE;

			continue;
		}
	}

	MEM_FREE(buf);
	(void) fclose(fp);

	return (has_frameoffs &&
		has_disclen &&
		has_discid &&
		has_dtitle &&
		has_ttitle);
}


/***********************
 *   public routines   *
 ***********************/


/*
 * cddb_init
 *	Initialize CD database management services
 *
 * Args:
 *	progname - The client program name string
 *	username - The client user login name string
 *
 * Return:
 *	Nothing.
 */
void
cddb_init(cddb_client_t *clp)
{
	char		*cp,
			*path,
			**ap,
			hname[MAXHOSTNAMELEN],
			tmpuser[STR_BUF_SZ],
			tmphost[MAXHOSTNAMELEN];
	struct hostent	*he;

	DBGPRN(errfp, "\nlibcddb: %s %s %s\n\n",
#ifdef SYNCHRONOUS
		"SYNCHRONOUS",
#else
		"ASYNCHRONOUS",
#endif
#ifdef NOREMOTE
		"NOREMOTE",
		""
#else
		"REMOTE",
#ifdef SOCKS
		"SOCKS"
#else
		""
#endif
#endif
	);

	cddb_clinfo = clp;

	/* Sanity check */
	if (clp->prog[0] == '\0')
		(void) strcpy(clp->prog, "unknown");

#ifndef NOREMOTE
	SOCKSINIT(clp->prog);
#endif

	if (clp->host[0] == '\0') {
#ifndef NOREMOTE
		/* Try to determine host name */
		if (gethostname(hname, MAXHOSTNAMELEN-1) < 0 ||
		    (he = gethostbyname(hname)) == NULL) {
			(void) strcpy(clp->host, "unknown");
		}
		else {
			/* Look for a a fully-qualified hostname
			 * (with domain)
			 */
			if (strchr(he->h_name, '.') != NULL) {
				(void) strncpy(clp->host, he->h_name,
					       MAXHOSTNAMELEN-1);
			}
			else {
				for (ap = he->h_aliases; *ap != NULL; ap++) {
					if (strchr(*ap, '.') != NULL) {
					    (void) strncpy(clp->host, *ap,
							   MAXHOSTNAMELEN-1);
					    break;
					}
				}
			}

			if (clp->host[0] == '\0')
				(void) strcpy(clp->host, hname);

			clp->host[MAXHOSTNAMELEN-1] = '\0';
		}
#else
		(void) strcpy(clp->host, "unknown");
#endif
	}

	if (clp->user[0] == '\0')
		(void) strcpy(clp->user, "unknown");

	/* Load XMCD_CDDBPATH environment variable, if specified */
	if ((cp = (char *) getenv("XMCD_CDDBPATH")) != NULL) {
		if ((int) strlen(cp) >= MAX_ENV_LEN) {
			CDDB_FATAL(app_data.str_longpatherr);
			return;
		}
		app_data.cddb_path = (char *) MEM_ALLOC(
			"app_data.cddb_path",
			strlen(cp) + 1
		);
		if (app_data.cddb_path == NULL) {
			CDDB_FATAL(app_data.str_nomemory);
			return;
		}

		(void) strcpy(app_data.cddb_path, cp);
	}

	if (app_data.cddb_path == NULL || app_data.cddb_path[0] == '\0')
		return;

	/* Create the CDDB path list */
	path = app_data.cddb_path;
	while ((cp = strchr(path, CDDBPATH_SEPCHAR)) != NULL) {
		*cp = '\0';

		if (!cddb_add_pathent(path))
			return;

		*cp = CDDBPATH_SEPCHAR;
		path = cp + 1;
	}
	(void) cddb_add_pathent(path);

#ifndef NOREMOTE
	if (app_data.http_proxy) {
		if (app_data.proxy_server == NULL ||
		    app_data.proxy_server[0] == '\0') {
			CDDB_FATAL(app_data.str_proxyerr);
			return;
		}
		if ((cp = strchr(app_data.proxy_server, ':')) != NULL)
			*cp = '\0';

		(void) strncpy(cddb_proxy.host, app_data.proxy_server,
			       MAXHOSTNAMELEN-1);
		cddb_proxy.host[MAXHOSTNAMELEN-1] = '\0';

		if (cp != NULL) {
			cddb_proxy.port = atoi(cp+1);
			*cp = ':';
		}
		else
			cddb_proxy.port = HTTP_DFLT_PORT;
	}

	cddb_http_xlat(cddb_clinfo->user, tmpuser);
	cddb_http_xlat(cddb_clinfo->host, tmphost);

	(void) sprintf(http_hellostr,
		"hello=%.64s+%.64s+%s+v%s%sPL%d",
		tmpuser, tmphost,
		cddb_clinfo->prog, VERSION, VERSION_EXT, PATCHLEVEL);
	(void) sprintf(http_extinfo,
#ifdef NO_MASQUERADE
		"User-Agent: %s/%s%sPL%d\r\nAccept: %s\r\n",
#else
		/* Believe it or not, this is how MS Internet Explorer
		 * does it, so don't blame me...
		 */
		"User-Agent: %s (compatible; %s %s%sPL%d)\r\nAccept: %s\r\n",
		"Mozilla/3.0",
#endif
		cddb_clinfo->prog, VERSION, VERSION_EXT, PATCHLEVEL,
		"text/plain");
#endif
}


/*
 * cddb_halt
 *	Shut down cddb subsystem.
 *
 * Args:
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	Nothing
 */
void
cddb_halt(curstat_t *s)
{
	if (curfile[0] != '\0' && s->devlocked)
		(void) UNLINK(curfile);
}


/*
 * cddb_load
 *	Load CD database entry for the currently inserted CD.
 *	A search will be performed on all database paths in the
 *	cddb path list.
 *
 * Args:
 *	dbp - Pointer to the cddb_incore_t structure
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	return code as defined by cddb_ret_t
 */
cddb_ret_t
cddb_load(cddb_incore_t *dbp, curstat_t *s)
{
#ifdef __VMS
	cddb_file_t	*fp;

	/* Clear flag */
	dbp->flags &= ~(CDDB_INEXACT | CDDB_AUTHFAIL);

	/* Get a CDDB file handle */
	if ((fp = cddb_open(dbp, s, O_RDONLY)) == NULL) {
		/* Database entry does not exist */
		return CDDB_SET_CODE(OPEN_ERR, 0);
	}

	/* Read contents of CD database entry and update incore structure */
	if (!cddb_read0(fp, dbp, s)) {
		/* Invalid CD database contents */
		(void) cddb_close(fp);
		return CDDB_SET_CODE(READ_ERR, 0);
	}

	/* Close file */
	(void) cddb_close(fp);

	if (dbp->flags & CDDB_INEXACT) {
		/* An inexact CDDB match was found */
		return CDDB_SET_CODE(MATCH_ERR, 0);
	}
	else if (dbp->flags & CDDB_AUTHFAIL) {
		/* Proxy authorization failed */
		return CDDB_SET_CODE(AUTH_ERR, 0);
	}

	switch (dbp->type) {
	case CDDB_REMOTE_CDDBP:
	case CDDB_REMOTE_HTTP:
		if (app_data.cddb_rmtautosave)
			/* Auto-save */
			cddb_autosave(dbp, s);
		break;

	default:
		break;
	}

	return 0;

#else	/* __VMS */
	int		pfd[2],
			ret,
			retcode;
	pid_t		cpid;
	waitret_t	stat_val;
	cddb_file_t	*fp,
			*wfp,
			*rfp;

	/* Clear flag */
	dbp->flags &= ~(CDDB_INEXACT | CDDB_AUTHFAIL);

	/* Set up a pipe */
	if (PIPE(pfd) < 0)
		return CDDB_SET_CODE(OPEN_ERR, errno);

	/* Fork child to performs actual I/O */
	switch (cpid = FORK()) {
	case 0:
		/* Child process */
		ischild = TRUE;
		break;

	case -1:
		(void) close(pfd[0]);
		(void) close(pfd[1]);
		return CDDB_SET_CODE(FORK_ERR, errno);

	default:
		/* Parent process */
		child_pid = cpid;

		/* Close un-needed pipe descriptor */
		(void) close(pfd[1]);

		/* Allocate cddb_file_t structures for read pipe */
		rfp = (cddb_file_t *)(void *) MEM_ALLOC(
			"cddb_file_t",
			sizeof(cddb_file_t)
		);
		if (rfp == NULL) {
			CDDB_FATAL(app_data.str_nomemory);
			return CDDB_SET_CODE(OPEN_ERR, 0);
		}

		/* Allocate read cache */
		rfp->cache = (unsigned char *) MEM_ALLOC(
			"rfp->cache",
			CDDB_CACHESZ
		);
		if (rfp->cache == NULL) {
			CDDB_FATAL(app_data.str_nomemory);
			return CDDB_SET_CODE(OPEN_ERR, 0);
		}

		rfp->magic = CDDB_FMAGIC;
		rfp->fd = pfd[0];
		rfp->rw = O_RDONLY;
		rfp->pos = 0;
		rfp->cnt = 0;
		rfp->type = CDDB_LOCAL;	/* It's really a pipe, but that's ok */

		/* Read contents of CD database entry from pipe into
		 * in-core structure
		 */
		(void) cddb_read2(rfp, dbp, s);

		/* Close read pipe */
		(void) cddb_close(rfp);

		if (dbp->flags & CDDB_INEXACT)
			retcode = CDDB_SET_CODE(MATCH_ERR, 0);
		else if (dbp->flags & CDDB_AUTHFAIL)
			retcode = CDDB_SET_CODE(AUTH_ERR, 0);
		else
			retcode = 0;

		/* Parent process: wait for child to exit */
		while ((ret = WAITPID(cpid, &stat_val, 0)) != cpid) {
			if (ret < 0) {
				child_pid = 0;
				stat_val = 0;
				break;
			}

			/* Do some work */
			if (cddb_clinfo->workproc != NULL)
				cddb_clinfo->workproc(cddb_clinfo->arg);
		}

		child_pid = 0;

		if (WIFEXITED(stat_val)) {
			if (WEXITSTATUS(stat_val) == 0) {
				switch (dbp->type) {
				case CDDB_REMOTE_CDDBP:
				case CDDB_REMOTE_HTTP:
					if (app_data.cddb_rmtautosave)
						/* Auto-save */
						cddb_autosave(dbp, s);
					break;

				default:
					break;
				}

				return (retcode);
			}
			else
				return CDDB_SET_CODE(WEXITSTATUS(stat_val), 0);
		}
		else if (WIFSIGNALED(stat_val))
			return CDDB_SET_CODE(KILLED_ERR, WTERMSIG(stat_val));
		else
			return 0;
	}

	/* Exit on receipt of SIGTERM */
	(void) signal(SIGTERM, onterm);

	DBGPRN(errfp, "\nSetting uid to %d, gid to %d\n",
		(int) util_get_ouid(), (int) util_get_ogid());

	/* Force uid and gid to original setting */
	if (setuid(util_get_ouid()) < 0 || setgid(util_get_ogid()) < 0)
		exit(SETUID_ERR);

	/* Close un-needed pipe descriptor */
	(void) close(pfd[0]);

	/* Allocate cddb_file_t structures for write pipe */
	wfp = (cddb_file_t *)(void *) MEM_ALLOC(
		"cddb_file_t",
		sizeof(cddb_file_t)
	);
	if (wfp == NULL)
		exit(MEM_ERR);

	/* Allocate write cache */
	wfp->cache = (unsigned char *) MEM_ALLOC("wfp->cache", CDDB_CACHESZ);
	if (wfp->cache == NULL)
		exit(MEM_ERR);

	wfp->magic = CDDB_FMAGIC;
	wfp->fd = pfd[1];
	wfp->rw = O_WRONLY;
	wfp->pos = 0;
	wfp->cnt = 0;
	wfp->type = CDDB_LOCAL;	/* It's really a pipe, but that's ok */

	/* Get a CDDB file handle */
	if ((fp = cddb_open(dbp, s, O_RDONLY)) == NULL) {
		/* Database entry does not exist */
		(void) cddb_close(wfp);
		exit(OPEN_ERR);
	}

	/* Read contents of CD database entry and write into pipe */
	if (!cddb_read1(fp, wfp, dbp)) {
		/* Invalid CD database contents */
		(void) cddb_close(fp);
		(void) cddb_close(wfp);
		exit(READ_ERR);
	}

	/* Close file */
	(void) cddb_close(fp);
	(void) cddb_close(wfp);

	/* Child exits here */
	exit(0);

	/*NOTREACHED*/
#endif	/* __VMS */
}


/*
 * cddb_save
 *	Save current in-core CD database data into file
 *
 * Args:
 *	dbp - Pointer to the cddb_incore_t structure
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	return code as defined by cddb_ret_t
 */
cddb_ret_t
cddb_save(cddb_incore_t *dbp, curstat_t *s)
{
	int		ret;
	pid_t		cpid;
	waitret_t	stat_val;
	cddb_file_t	*fp;
	cddb_ret_t	retcode;
	char		dbfile[FILE_PATH_SZ];

	/* Increment revision if in-core entry has been edited */
	if (dbp->flags & CDDB_CHANGED)
		dbp->revision++;

	/* Fork child to perform actual I/O */
	switch (cpid = FORK()) {
	case 0:
		/* Child process */
		ischild = TRUE;
		break;

	case -1:
		return CDDB_SET_CODE(FORK_ERR, errno);

	default:
		/* Parent process: wait for child to exit */
		while ((ret = WAITPID(cpid, &stat_val, 0)) != cpid) {
			if (ret < 0)
				return 0;

			/* Do some work */
			if (cddb_clinfo->workproc != NULL)
				cddb_clinfo->workproc(cddb_clinfo->arg);
		}

		retcode = 0;
		if (WIFEXITED(stat_val))
			retcode = CDDB_SET_CODE(WEXITSTATUS(stat_val), 0);
		else if (WIFSIGNALED(stat_val))
			retcode = CDDB_SET_CODE(KILLED_ERR, WTERMSIG(stat_val));

		/* If discid != queryid, perform a link */
		if (retcode == 0 && dbp->discid != dbp->queryid) {
			(void) sprintf(dbfile, CDDBFILE_PATH,
					util_dirname(dbp->dbfile), dbp->discid);
			(void) cddb_link(dbp, dbp->dbfile, dbfile);
			(void) strcpy(dbp->dbfile, dbfile);
		}
		
		return (retcode);
	}

	DBGPRN(errfp, "\nSetting uid to %d, gid to %d\n",
		(int) util_get_ouid(), (int) util_get_ogid());

	/* Force uid and gid to original setting */
	if (setuid(util_get_ouid()) < 0 || setgid(util_get_ogid()) < 0)
		exit(SETUID_ERR);

	DBGPRN(errfp, "\nWriting CD database file %s\n", dbp->dbfile);

	/* Get a CDDB file handle */
	if ((fp = cddb_open(dbp, s, O_WRONLY | O_TRUNC | O_CREAT)) == NULL)
		exit(OPEN_ERR);

	/* Set CD database file permissions */
	cddb_setperm(dbp->dbfile);

	/* Write to CD database */
	if (!cddb_write(fp, dbp, s))
		exit(WRITE_ERR);

	/* Close file */
	if (!cddb_close(fp))
		exit(CLOSE_ERR);

	/* Child exits here */
	exit(0);

	/*NOTREACHED*/
}


/*
 * cddb_link
 *	Create a link of an existing local CD database entry to another.
 *
 * Args:
 *	origpath - Source directory path string
 *	newpath - target directory path string
 *
 * Return:
 *	return code as defined by cddb_ret_t
 */
cddb_ret_t
cddb_link(cddb_incore_t *dbp, char *origpath, char *newpath)
{
	int		i,
			ret,
			bufsz = STR_BUF_SZ * 2;
	pid_t		cpid;
	waitret_t	stat_val;
	FILE		*rfp,
			*wfp;
	cddb_linkopts_t	*r;
	cddb_ret_t	retcode;
	char		*buf,
			origid[9],
			newid[9];
	bool_t		process_idlist;

	/* Fork child to perform actual I/O */
	switch (cpid = FORK()) {
	case 0:
		/* Child process */
		ischild = TRUE;
		break;

	case -1:
		return CDDB_SET_CODE(FORK_ERR, errno);

	default:
		/* Parent process: wait for child to exit */
		while ((ret = WAITPID(cpid, &stat_val, 0)) != cpid) {
			if (ret < 0)
				return 0;

			/* Do some work */
			if (cddb_clinfo->workproc != NULL)
				cddb_clinfo->workproc(cddb_clinfo->arg);
		}

		retcode = 0;
		if (WIFEXITED(stat_val))
			retcode = CDDB_SET_CODE(WEXITSTATUS(stat_val), 0);
		else if (WIFSIGNALED(stat_val))
			retcode = CDDB_SET_CODE(KILLED_ERR, WTERMSIG(stat_val));

		if (retcode == 0)
			/* Set the query ID back to the disc ID */
			dbp->queryid = dbp->discid;

		return (retcode);
	}

	DBGPRN(errfp, "\nSetting uid to %d, gid to %d\n",
		(int) util_get_ouid(), (int) util_get_ogid());

	/* Force uid and gid to original setting */
	if (setuid(util_get_ouid()) < 0 || setgid(util_get_ogid()) < 0)
		exit(SETUID_ERR);

	DBGPRN(errfp, "\ncddb_link: origpath=%s newpath=%s\n",
		origpath, newpath);

	(void) strncpy(origid, util_basename(origpath), 8);
	(void) strncpy(newid, util_basename(newpath), 8);
	origid[8] = newid[8] = '\0';

	/*
	 * Copy original file to new file, modify the DISCID line
	 */

	/* Open original cddb file for reading */
	if ((rfp = fopen(origpath, "r")) == NULL)
		exit(OPEN_ERR);
	/* Open new file for writing */
	if ((wfp = fopen(newpath, "w")) == NULL)
		exit(OPEN_ERR);

	/*
	 * Set file permissions
	 */
	cddb_setperm(newpath);

	if ((buf = (char *) MEM_ALLOC("link_buf", bufsz)) == NULL)
		exit(MEM_ERR);

	process_idlist = FALSE;
	cddb_free_idlist(dbp);
	while (fgets(buf, bufsz, rfp) != NULL) {
		if (strncmp(buf, "DISCID=", 7) == 0) {
			process_idlist = TRUE;

			/* Zap newline */
			buf[strlen(buf)-1] = '\0';

			/* Build disc id list */
			(void) cddb_add_idlist(dbp, buf + 7);
		}
		else {
			if (process_idlist) {
				process_idlist = FALSE;

				/* Check if the original file's ID is in the
				 * idlist.  If not, add it.
				 */
				for (r = dbp->idlist; r != NULL; r = r->next) {
					if (strcmp(origid, r->idstr) == 0)
						break;
				}
				if (r == NULL)
					cddb_add_ident(dbp, origid);

				/* Pump out the list of disc IDs */
				(void) fprintf(wfp, "DISCID=%s", newid);
				i = 1;
				for (r = dbp->idlist; r != NULL; r = r->next) {
					if (strcmp(newid, r->idstr) == 0)
						continue;

					if (i == 0)
						(void) fprintf(wfp, "DISCID=%s",
							       r->idstr);
					else
						(void) fprintf(wfp, "%c%s",
							       CDDBID_SEPCHAR,
							       r->idstr);

					i++;
					if (i == 8) {
						(void) fprintf(wfp, "\n");
						i = 0;
					}
				}
				if (i != 0)
					(void) fprintf(wfp, "\n");
			}

			(void) fprintf(wfp, "%s", buf);
		}
	}

	(void) fclose(rfp);
	(void) fclose(wfp);

	/*
	 * Write contents of new file back to original file
	 */

	/* Open destination file for reading */
	if ((rfp = fopen(newpath, "r")) == NULL)
		exit(OPEN_ERR);
	/* Open original cddb file for writing */
	if ((wfp = fopen(origpath, "w")) == NULL)
		exit(OPEN_ERR);

	/*
	 * Set file permissions
	 */
	cddb_setperm(origpath);

	while (fgets(buf, bufsz, rfp) != NULL)
		(void) fprintf(wfp, "%s", buf);

	(void) fclose(rfp);
	(void) fclose(wfp);

	/*
	 * Remove new file
	 */
	if (UNLINK(newpath) < 0)
		exit(LINK_ERR);

	/*
	 * Link original file to new file
	 */
	if (LINK(origpath, newpath) < 0)
		exit(LINK_ERR);

	/* Child exits here. */
	exit(0);

	/*NOTREACHED*/
}


/*
 * cddb_send
 *	Send current CD database entry to archive server via e-mail.
 *
 * Args:
 *	dbp - Pointer to the cddb_incore_t structure
 *	s - Pointer to the curstat_t structure
 *
 * Return:
 *	return code as defined by cddb_ret_t
 */
/*ARGSUSED*/
cddb_ret_t
cddb_send(cddb_incore_t *dbp, curstat_t *s)
{
	char		*p,
			*q,
			*cmd,
			*subject;
	int		ret;
#ifndef __VMS
	pid_t		cpid;
	waitret_t	stat_val;

	/* Fork child to perform actual I/O */
	switch (cpid = FORK()) {
	case 0:
		/* Child process */
		ischild = TRUE;
		break;

	case -1:
		return CDDB_SET_CODE(FORK_ERR, errno);

	default:
		/* Parent process: wait for child to exit */
		while ((ret = WAITPID(cpid, &stat_val, 0)) != cpid) {
			if (ret < 0)
				return 0;

			/* Do some work */
			if (cddb_clinfo->workproc != NULL)
				cddb_clinfo->workproc(cddb_clinfo->arg);
		}

		if (WIFEXITED(stat_val))
			return CDDB_SET_CODE(WEXITSTATUS(stat_val), 0);
		else if (WIFSIGNALED(stat_val))
			return CDDB_SET_CODE(KILLED_ERR, WTERMSIG(stat_val));
		else
			return 0;
	}

	DBGPRN(errfp, "\nSetting uid to %d, gid to %d\n",
		(int) util_get_ouid(), (int) util_get_ogid());

	/* Force uid and gid to original setting */
	if (setuid(util_get_ouid()) < 0 || setgid(util_get_ogid()) < 0)
		exit(SETUID_ERR);
#endif	/* __VMS */

	/* Check CD database entry for sanity */
	if (!cddb_check_entry(dbp)) {
#ifdef __VMS
		return (INCMPL_ERR);
#else
		exit(INCMPL_ERR);
#endif
	}

	/* Allocate command buffer */
	cmd = (char *) MEM_ALLOC(
		"send_cmdbuf",
		strlen(app_data.cddb_mailcmd) +
		strlen(app_data.cddb_mailsite) +
		strlen(dbp->category) + STR_BUF_SZ
	);
	subject = (char *) MEM_ALLOC(
		"subject",
		strlen(dbp->category) + STR_BUF_SZ
	);
	if (cmd == NULL || subject == NULL) {
#ifdef __VMS
		CDDB_FATAL(app_data.str_nomemory);
		return (MEM_ERR);
#else
		exit(MEM_ERR);
#endif
	}

	/* Mail command */
	for (p = cmd, q = app_data.cddb_mailcmd; *q != '\0'; p++, q++) {
		if (*q ==  '%') {
			/* Support the special meanings of %S, %A and %F */
			switch (*(q+1)) {
			case 'S':
				/* Mail subject */
				(void) sprintf(subject, "cddb %s %08x",
					       dbp->category[0] == '\0' ?
					       "unknown" : dbp->category,
					       dbp->discid);
				(void) strcpy(p, subject);
				p += strlen(subject) - 1;
				q++;
				break;

			case 'A':
				/* Mail address */
				(void) strcpy(p, app_data.cddb_mailsite);
				p += strlen(app_data.cddb_mailsite) - 1;
				q++;
				break;

			case 'F':
				/* CD database file path */
				(void) strcpy(p, dbp->dbfile);
				p += strlen(dbp->dbfile) - 1;
				q++;
				break;

			default:
				*p++ = *q++;	/* '%' */
				*p = *q;
				break;
			}
		}
		else {
			*p = *q;
		}
	}
	*p = '\0';

	DBGPRN(errfp, "\nSend CDDB: [%s]\n", cmd);

	/* Send the mail */
	if (cddb_clinfo->isdemo != NULL && cddb_clinfo->isdemo()) {
		/* Don't send mail if in demo mode */
		(void) fprintf(errfp, "DEMO mode: mail not sent.\n");
		ret = 0;
	}
	else
		ret = system(cmd);

	MEM_FREE(subject);
	MEM_FREE(cmd);

#ifdef __VMS
	return ((ret == 0 || ret == 1) ? 0 : CMD_ERR);
#else
	/* Child exits here */
	exit((ret == 0) ? 0 : CMD_ERR);
	/*NOTREACHED*/
#endif
}


/*
 * cddb_clear
 *	Clear the in-core cddb_incore_t structure
 *
 * Args:
 *	dbp - Pointer to the cddb_incore_t structure
 *	s - Pointer to the curstat_t structure
 *	reload - Whether this operation is due to a reload of the CDDB
 *		(We don't want to clear a play sequence in this case).
 *
 * Return:
 *	Nothing
 */
/*ARGSUSED*/
void
cddb_clear(cddb_incore_t *dbp, curstat_t *s, bool_t reload)
{
	int		i;
	cddb_match_t	*p,
			*q;

	/* Cancel pending queries */
	cddb_load_cancel();

	dbp->category[0] = '\0';

	if (dbp->dbfile != NULL) {
		MEM_FREE(dbp->dbfile);
		dbp->dbfile = NULL;
	}

	if (dbp->dtitle != NULL) {
		MEM_FREE(dbp->dtitle);
		dbp->dtitle = NULL;
	}

	if (dbp->extd != NULL) {
		MEM_FREE(dbp->extd);
		dbp->extd = NULL;
	}

	for (i = MAXTRACK-1; i >= 0; i--) {
		if (dbp->trklist[i] != NULL) {
			MEM_FREE(dbp->trklist[i]);
			dbp->trklist[i] = NULL;
		}

		if (dbp->extt[i] != NULL) {
			MEM_FREE(dbp->extt[i]);
			dbp->extt[i] = NULL;
		}

		if (dbp->sav_extt[i] != NULL) {
			MEM_FREE(dbp->sav_extt[i]);
			dbp->sav_extt[i] = NULL;
		}
	}

	if (!reload && dbp->playorder != NULL) {
		MEM_FREE(dbp->playorder);
		dbp->playorder = NULL;
	}

	dbp->discid = 0;
	dbp->queryid = 0;
	dbp->revision = 0;
	dbp->type = CDDB_INVALID;
	dbp->flags = 0;

	for (p = q = dbp->matchlist; p != NULL; p = q) {
		q = p->next;
		MEM_FREE(p->dtitle);
		MEM_FREE(p);
	}
	dbp->matchlist = dbp->match_cur = NULL;

	cddb_free_idlist(dbp);
}


/*
 * cddb_load_cancel
 *	Cancel asynchronous CDDB load operation, if active.
 *
 * Args:
 *	None.
 *
 * Return:
 *	nothing.
 */
void
cddb_load_cancel(void)
{
	if (child_pid > 0) {
		/* Kill child process */
		(void) kill(child_pid, SIGTERM);
		child_pid = 0;
	}
}


/*
 * cddb_discid
 *	Compute a magic disc ID based on the number of tracks,
 *	the length of each track, and a checksum of the string
 *	that represents the offset of each track.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	The integer disc ID.
 */
word32_t
cddb_discid(curstat_t *s)
{
	int	i,
		a,
		t = 0,
		n = 0;

	/* For backward compatibility this algorithm must not change */

	a = (int) s->tot_trks;

	for (i = 0; i < a; i++)
		n += cddb_sum((s->trkinfo[i].min * 60) + s->trkinfo[i].sec);

	t = ((s->trkinfo[a].min * 60) + s->trkinfo[a].sec) -
	     ((s->trkinfo[0].min * 60) + s->trkinfo[0].sec);

	return ((n % 0xff) << 24 | t << 8 | s->tot_trks);
}


/*
 * cddb_category
 *	Extract the category from a CDDB file path
 *
 * Args:
 *	path - The path name to a CDDB file.
 *
 * Return:
 *	The category string.
 */
char *
cddb_category(char *path)
{
#ifdef __VMS
	char		*p,
			*q;
	static char	retbuf[FILE_BASE_SZ];

	retbuf[0] = '\0';

	if ((p = strrchr(path, '.')) == NULL ||
	    (q = strrchr(path, ']')) == NULL)
		return (path);

	*q = '\0';
	(void) strncpy(retbuf, p+1, FILE_BASE_SZ - 1);
	retbuf[FILE_BASE_SZ - 1] = '\0';
	*q = ']';

	return (retbuf);
#else
	return (util_basename(path));
#endif
}


/*
 * cddb_set_auth
 *	Set proxy authorization user name and password.
 *
 * Args:
 *	name - Proxy user name
 *	passwd - Proxy password
 *
 * Return:
 *	Nothing.
 */
void
cddb_set_auth(char *name, char *passwd)
{
	int	i,
		j,
		n1,
		n2;
	char	*buf;

	if (name == NULL || passwd == NULL)
		return;

	i = strlen(name);
	j = strlen(passwd);
	n1 = i + j + 2;
	n2 = (n1 * 4 / 3) + 8;

	if ((buf = (char *) MEM_ALLOC("set_auth_buf", n1)) == NULL) {
		CDDB_FATAL(app_data.str_nomemory);
		return;
	}

	(void) sprintf(buf, "%s:%s", name, passwd);

	(void) memset(name, 0, i);
	(void) memset(passwd, 0, j);

	if (auth_buf != NULL)
		MEM_FREE(auth_buf);

	if ((auth_buf = (char *) MEM_ALLOC("set_auth_authbuf", n2)) == NULL) {
		CDDB_FATAL(app_data.str_nomemory);
		return;
	}

	/* Base64 encode the name/password pair */
	util_b64encode(
		(byte_t *) buf,
		strlen(buf),
		(byte_t *) auth_buf,
		FALSE
	);

	(void) memset(buf, 0, n1);
	MEM_FREE(buf);
}


/*
 * cddb_pathlist
 *	Return a pointer to the CDDB path list.
 *
 * Args:
 *	None.
 *
 * Return:
 *	A pointer to the path list head, or NULL if the list is empty.
 */
cddb_path_t *
cddb_pathlist(void)
{
	return (cddb_pathhead);
}


/*
 * cddb_init_linkopts
 *	Build a sorted linked list of track titles which are to be
 *	used to present to the user for database search-link.
 *
 * Args:
 *	s - Pointer to the curstat_t structure.
 *
 * Return:
 *	TRUE - success
 *	FALSE - failure
 */
bool_t
cddb_init_linkopts(cddb_incore_t *dbp, curstat_t *s)
{
	int		i,
			n,
			ntrk,
			bufsz = STR_BUF_SZ * 2,
			trk_off[MAXTRACK];
	word32_t	offset,
			trkaddr;
	char		*dbdir,
			*bname,
			*buf,
			tmppath[FILE_PATH_SZ];
	FILE		*fp;
	DIR		*dp;
	struct dirent	*de;
	bool_t		found;

	/* Warning: This code is SYSV-ish.  Porting to other
	 * environment may require some modification here.
	 */

	if (dbp->dbfile == NULL)
		/* Error */
		return FALSE;

	dbdir = util_dirname(dbp->dbfile);
	bname = util_basename(dbp->dbfile);

	if ((dp = OPENDIR(dbdir)) == NULL)
		return FALSE;

	if ((buf = (char *) MEM_ALLOC("init_linkopts", bufsz)) == NULL) {
		CDDB_FATAL(app_data.str_nomemory);
		return FALSE;
	}

	for (n = 0; (de = READDIR(dp)) != NULL; n++) {
		/* Handle some events to avoid GUI freeze-up
		 * (once every 50 loops)
		 */
		if (cddb_clinfo->workproc != NULL && (n % 50) == 0)
			cddb_clinfo->workproc(cddb_clinfo->arg);

		if ((int) strlen(de->d_name) != 8)
			continue;

		/* Find all entries in this directory with the same
		 * number of tracks as this disc.
		 */
		if (strncmp(de->d_name + 6, bname + 6, 2) != 0)
			continue;

		(void) sprintf(tmppath, CONCAT_PATH, dbdir, de->d_name);
		if ((fp = fopen(tmppath, "r")) == NULL)
			continue;
		
		/* Read first line of database file */
		if (fgets(buf, bufsz, fp) == NULL) {
			(void) fclose(fp);
			continue;
		}

		/* Database file signature check */
		if (strncmp(buf, "# xmcd ", 7) != 0) {
			/* Not a supported database file */
			(void) fclose(fp);
			continue;
		}

		(void) memset(trk_off, 0, sizeof(trk_off));
		ntrk = 0;
		found = FALSE;

		while (fgets(buf, bufsz, fp) != NULL) {
			/* Look for track addresses of possible links */
			if (strncmp(buf, "# Track frame offsets", 21) == 0) {
				found = TRUE;
				continue;
			}
			else if (strncmp(buf, "# Disc length", 13) == 0) {
				i = sscanf(buf, "# Disc length: %u seconds\n",
					   &trkaddr);
				if (i > 0) {
					trk_off[ntrk] =
						(trkaddr * FRAME_PER_SEC) -
						s->trkinfo[ntrk].addr -
						MSF_OFFSET;
				}
				else {
					/* File format error */
					ntrk = 0;
				}
				found = FALSE;
				continue;
			}

			if (found &&
			    (i = sscanf(buf, "# %u\n", &trkaddr)) > 0) {
				trk_off[ntrk] =
					trkaddr - s->trkinfo[ntrk].addr -
					MSF_OFFSET;
				ntrk++;
			}

			/* Look for disk title */
			if (strncmp(buf, "DTITLE=", 7) == 0) {
				/* Eat newline */
				i = strlen(buf) - 1;
				if (buf[i] == '\n')
					buf[i] = '\0';
				
				/* Check whether a valid offset can be
				 * calculated (compare # of tracks).
				 */
				if (ntrk == (int) s->tot_trks) {
					/* Compute the average block
					 * number difference per track.
					 */
					offset = 0;
					for (i = 0; i <= ntrk; i++) {
						if (trk_off[i] < 0)
						    trk_off[i] = -trk_off[i];
						offset += trk_off[i];
					}
					offset /= ntrk;
				}
				else {
					/* Track offsets not specified or
					 * not valid in database file.
					 */
					offset = (word32_t) OFFSET_UNKN;
				}

				/* Add to list in sorted order */
				(void) cddb_add_linkent(buf + 7, de->d_name,
							offset);
				break;
			}
		}

		(void) fclose(fp);
	}
	(void) CLOSEDIR(dp);

	MEM_FREE(buf);
	return TRUE;
}


/*
 * cddb_free_linkopts
 *	Dismantle the sorted linked list of track titles created by
 *	cddb_init_linkopts.
 *
 * Args:
 *	Nothing.
 *
 * Return:
 *	Nothing.
 */
void
cddb_free_linkopts(void)
{
	cddb_linkopts_t	*p,
			*q;

	for (p = q = cddb_linkhead; p != NULL; p = q) {
		q = p->next;
		if (p->dtitle != NULL)
			MEM_FREE(p->dtitle);
		MEM_FREE((char *) p);
	}
	cddb_linkhead = NULL;
}


/*
 * cddb_linkopts
 *	Return a pointer to the CDDB linkopts list.
 *
 * Args:
 *	None.
 *
 * Return:
 *	A pointer to the linkopts list head, or NULL if the list is empty.
 */
cddb_linkopts_t *
cddb_linkopts(void)
{
	return (cddb_linkhead);
}


/*
 * cddb_rmt_support
 *	Returns information whether this cddb library supports remote
 *	CDDB servers.
 *
 * Args:
 *	None.
 *
 * Return:
 *	TRUE  - Remote CDDB server supported
 *	FALSE - Remote CDDB server not supported
 */
bool_t
cddb_rmt_support(void)
{
	cddb_path_t	*pp;

	if (app_data.cddb_rmtdsbl)
		return FALSE;

	for (pp = cddb_pathhead; pp != NULL; pp = pp->next) {
		if (pp->type == CDDB_REMOTE_CDDBP ||
		    pp->type == CDDB_REMOTE_HTTP)
			return TRUE;
	}
	return FALSE;
}


/*
 * cddb_curfileupd
 *	Update the current disc info file.
 *
 * Args:
 *	dbp - Pointer to the cddb_incore_t structure.
 *
 * Return:
 *	Nothing
 */
void
cddb_curfileupd(cddb_incore_t *dbp)
{
#ifndef __VMS
	FILE		*fp;
	struct stat	stbuf;
	char		*dtitle,
			*ttitle,
			str[FILE_PATH_SZ * 3];
	static char	prev[FILE_PATH_SZ * 3],
			prev_dtitle[STR_BUF_SZ],
			prev_ttitle[STR_BUF_SZ];
	curstat_t	*s;
	bool_t		playing,
			changed;

	if (!app_data.write_curfile ||			/* disabled */
	    strncmp(app_data.device, "(sim", 4) == 0)	/* demo */
		return;

	if (cddb_clinfo->curstat_addr == NULL)
		return;

	s = cddb_clinfo->curstat_addr();

	if (!s->devlocked)
		/* Don't write curfile if we don't own the device */
		return;

	playing = FALSE;
	dtitle = ttitle = NULL;

	if (s->mode != MOD_NODISC) {
		char	modestr[24];

		dtitle = dbp->dtitle;

		switch (s->mode) {
		case MOD_PLAY:
		case MOD_PAUSE:
		case MOD_A:
		case MOD_AB:
		case MOD_SAMPLE:
			playing = TRUE;

			(void) sprintf(modestr, "Playing track %d",
				       s->cur_trk);

			if (s->cur_trk > 0) {
				int	n;

				n = (int) s->cur_trk - 1;

				if (s->trkinfo[n].trkno != s->cur_trk) {
					for (n = 0; n < MAXTRACK; n++) {
						if (s->trkinfo[n].trkno ==
						    s->cur_trk)
							break;
					}
				}

				ttitle = dbp->trklist[n];
			}
			break;

		case MOD_STOP:
			(void) strcpy(modestr, "Stopped");
			break;

		default:
			modestr[0] = '\0';
			break;
		}

		(void) sprintf(str,
			       "%s\t\t%s\n%s\t%s\n%s\t\t%08x\n%s\t\t%s\n",
			       "Device:", s->curdev == NULL ? "-" : s->curdev,
			       "Category:",
			       dbp->category[0] == '\0' ?
					"<unknown>" : dbp->category,
			       "DiscID:", dbp->discid,
			       "Status:", modestr);
	}
	else {
		(void) sprintf(str,
			       "%s\t\t%s\n%s\t%s\n%s\t\t%s\n%s\t\t%s\n",
			       "Device:", s->curdev == NULL ? "-" : s->curdev,
			       "Category:", "-",
			       "DiscID:", "-",
			       "Status:", "No_Disc");
	}

	changed = FALSE;

	if (playing) {
		if (ttitle == NULL && prev_ttitle[0] != '\0')
			changed = TRUE;
		else if (ttitle != NULL &&
			 strncmp(ttitle, prev_ttitle, STR_BUF_SZ - 1) != 0)
			changed = TRUE;
	}

	if (s->mode != MOD_NODISC) {
		if (dtitle == NULL && prev_dtitle[0] != '\0')
			changed = TRUE;
		else if (dtitle != NULL && 
			 strncmp(dtitle, prev_dtitle, STR_BUF_SZ - 1) != 0)
			changed = TRUE;
	}

	if (strcmp(str, prev) != 0)
		changed = TRUE;

	if (!changed)
		return;

	/* Write to file */
	if (curfile[0] == '\0') {
		if (stat(app_data.device, &stbuf) < 0) {
			DBGPRN(errfp, "\nCannot stat %s\n", app_data.device);
			return;
		}

		(void) sprintf(curfile, "%s/curr.%x",
			       TEMP_DIR, (int) stbuf.st_rdev);
	}

	/* Remove original file */
	if (UNLINK(curfile) < 0 && errno != ENOENT) {
		DBGPRN(errfp, "\nCannot unlink old %s\n", curfile);
		return;
	}

	/* Write new file */
	if ((fp = fopen(curfile, "w")) == NULL) {
		DBGPRN(errfp, "\nCannot open %s for writing\n", curfile);
		return;
	}

	DBGPRN(errfp, "\nWriting current disc info file: %s\n", curfile);

	(void) fprintf(fp, "#\n# Xmcd current CD information\n#\n%s", str);

	if (s->mode != MOD_NODISC) {
		(void) fprintf(fp, "Disc:\t\t%s\n",
			       dtitle == NULL ?
			       "<unknown disc title>" : dtitle);
	}
	else {
		(void) fprintf(fp, "Disc:\t\t-\n");
	}

	if (playing) {
		(void) fprintf(fp, "Track:\t\t%s\n",
			       ttitle == NULL ?
			       "<unknown track title>" : ttitle);
	}
	else {
		(void) fprintf(fp, "Track:\t\t-\n");
	}

	(void) fclose(fp);
	(void) chmod(curfile, 0644);
	
	if (ttitle == NULL)
		prev_ttitle[0] = '\0';
	else {
		(void) strncpy(prev_ttitle, ttitle, STR_BUF_SZ - 1);
		prev_ttitle[STR_BUF_SZ - 1] = '\0';
	}

	if (dtitle == NULL)
		prev_dtitle[0] = '\0';
	else {
		(void) strncpy(prev_dtitle, dtitle, STR_BUF_SZ - 1);
		prev_dtitle[STR_BUF_SZ - 1] = '\0';
	}

	(void) strcpy(prev, str);
#endif	/* __VMS */
}


/*
 * cddb_issync
 *	Returns whether the CDDB subsystem is running in SYNCHRONOUS mode.
 *
 * Args:
 *	None.
 *
 * Return:
 *	TRUE if SYNCHRONOUS
 *	FALSE if not SYNCHRONOUS
 */
bool_t
cddb_issync(void)
{
#ifdef SYNCHRONOUS
	return TRUE;
#else
	return FALSE;
#endif
}


/*
 * cddb_dump_incore
 *	Displays the contents of the cddb_incore_t structure.
 *
 * Args:
 *	dbp - Pointer to the cddb_incore_t structure.
 *	s - Pointer to the curstat_t structure.
 *
 * Returns:
 *	Nothing.
 */
void
cddb_dump_incore(cddb_incore_t *dbp, curstat_t *s)
{
	int		i;
	cddb_match_t	*p;
	cddb_linkopts_t	*q;

	(void) fprintf(errfp,
		       "\nDumping the cddb_incore_t structure at 0x%x:\n",
		       (int) dbp);
	(void) fprintf(errfp, "discid=%08x queryid=%08x type=%d flags=0x%x\n",
		       dbp->discid, dbp->queryid, dbp->type, dbp->flags);
	(void) fprintf(errfp, "revision=%d dbfile=%s category=%s\n",
		       dbp->revision,
		       dbp->dbfile == NULL ? "NULL" : dbp->dbfile,
		       dbp->category);
	(void) fprintf(errfp, "dtitle=%s\n",
		       dbp->dtitle == NULL ? "NULL" : dbp->dtitle);

	for (i = 0; i < (int) s->tot_trks; i++) {
		(void) fprintf(errfp, "trklist[%d]=%s\n", i,
			       dbp->trklist[i] == NULL ?
				    "NULL" : dbp->trklist[i]);
	}

	if (dbp->extd == NULL)
		(void) fprintf(errfp, "extd=NULL\n");
	else
		(void) fprintf(errfp, "extd=\n%s\n", dbp->extd);

	for (i = 0; i < (int) s->tot_trks; i++) {
		if (dbp->extt[i] == NULL)
			(void) fprintf(errfp, "extt[%d]=NULL\n", i);
		else
			(void) fprintf(errfp, "extt[%d]=\n%s\n",
				       i, dbp->extt[i]);
	}

	for (i = 0; i < (int) s->tot_trks; i++) {
		if (dbp->sav_extt[i] == NULL)
			(void) fprintf(errfp, "sav_extt[%d]=NULL\n", i);
		else
			(void) fprintf(errfp, "sav_extt[%d]=\n%s\n", i,
				dbp->sav_extt[i]);
	}

	(void) fprintf(errfp, "playorder=%s\n",
		       dbp->playorder == NULL ? "NULL" : dbp->playorder);

	if (dbp->idlist == NULL)
		(void) fprintf(errfp, "idlist=NULL\n");
	else {
		for (q = dbp->idlist, i = 0; q != NULL; q = q->next, i++) {
			(void) fprintf(errfp, "idlist[%d] ptr=0x%x id=%s\n",
				       i, (int) q, q->idstr);
		}
	}

	if (dbp->matchlist == NULL)
		(void) fprintf(errfp, "matchlist=NULL\n");
	else {
		for (p = dbp->matchlist, i = 0; p != NULL; p = p->next, i++) {
			(void) fprintf(errfp,
				       "matchlist[%d] ptr=0x%x categ=%s ",
				       i, (int) p, p->category);
			(void) fprintf(errfp, "discid=%08x dtitle=%s\n",
				       p->discid, p->dtitle);
		}
	}
	if (dbp->match_cur == NULL)
		(void) fprintf(errfp, "match_cur=NULL\n");
	else
		(void) fprintf(errfp, "match_cur=0x%x\n", (int) dbp->match_cur);
}


