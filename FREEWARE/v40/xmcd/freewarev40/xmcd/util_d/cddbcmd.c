/*
 *   cddbcmd - Connect to a CDDB server site and execute a simple CDDB
 *             server command.
 *
 *   Usage:	cddbcmd [-m <cddbp | http>] [-p proxyhost[:port]]
 *			[-h serverhost[:port]] [-c cgipath] [-l level]
 *			[-d] command ...
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
static char *_cddbcmd_c_ident_ = "@(#)cddbcmd.c	6.16 98/10/08";
#endif

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>

#ifdef _AIX
#include <sys/select.h>
#endif

#if !defined(USE_TERMIOS) && !defined(USE_TERMIO) && !defined(USE_SGTTY)
#define USE_TERMIOS     /* Make USE_TERMIOS the default if not specified */
#endif

#ifdef USE_TERMIOS
#include <termios.h>
#endif
#ifdef USE_TERMIO
#include <termio.h>
#endif
#ifdef USE_SGTTY
#include <sgtty.h>
#endif

#include "common_d/patchlevel.h"


#define CLIENT		"xmcd"
#define CGIPATH		"/~cddb/cddb.cgi"
#define DFLHOST		"cddb.cddb.com"

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN	64
#endif
#ifndef MAXUSERNAMELEN
#define MAXUSERNAMELEN	16
#endif

#define CMD_BUFLEN	1024

#define PROTO_CDDBP	1
#define PROTO_HTTP	2

#define PORT_CDDBP	888
#define PORT_HTTP	80

#define CDDB_PROTO	3

#ifdef SOCKS
#define SOCKSINIT(x)	SOCKSinit(x)
#define SOCKET		socket
#define CONNECT		Rconnect
#else
#define SOCKSINIT(x)
#define SOCKET		socket
#define CONNECT		connect
#endif  /* SOCKS */

extern char		*ttyname(),
			*getenv();
extern int		optind;
extern char		*optarg;

int			server_connect(char *, int, char *, int, char *,
				       int, char *, int);

int			debug,
			proto,
			serverport,
			proxyport,
			cddbproto;
char			user[MAXUSERNAMELEN],
			myhost[MAXHOSTNAMELEN],
			*auth_buf,
			*serverhost,
			*proxyhost,
			*cgipath,
			cmd[CMD_BUFLEN];


/*
 * onalrm
 *	SIGALRM signal handler
 *
 * Args:
 *	signo - signal number (unused)
 *
 * Return:
 *	Nothing
 */
/*ARGSUSED*/
void
onalrm(int signo)
{
	(void) signal(SIGALRM, SIG_DFL);
}


/*
 * my_fgets
 *	Read a string of up to length len from the specified stream.
 *
 * Args:
 *	buf - string buffer
 *	len - maximum length to read in bytes
 *	fp - input stream
 *
 * Return:
 *	Pointer to buf, or NULL on EOF or error.
 */
char *
my_fgets(char *buf, int len, FILE *fp)
{
	char	*ret;

	/* Just use the stdio version */
	ret = fgets(buf, len, fp);

	if (debug)
		(void) fprintf(stderr, "<- %s", buf);

	return (ret);
}


/*
 * my_fputs
 *	Write a string to the specified stream.
 *
 * Args:
 *	buf - string buffer
 *	fp - output stream
 *
 * Return:
 *	Number of bytes written, or -1 on error.
 */
int
my_fputs(char *buf, FILE *fp)
{
	int	fd,
		wlen,
		tot,
		ret;
	char	*p;

	if (debug)
		(void) fprintf(stderr, "-> %s", buf);

	fd = fileno(fp);
	p = buf;
	wlen = strlen(p);
	tot = 0;
	while (wlen > 0) {
		if ((ret = write(fd, p, wlen)) < 0)
			return -1;
		p += ret;
		tot += ret;
		wlen -= ret;
	}

	if (tot == 0) {
		(void) fprintf(stderr, "write error\n");
		return 0;
	}

	return (tot);
}


/*
 * do_echo
 *	Turn off/on echo mode.  It is assumed that the program starts
 *	with echo mode enabled.
 *
 * Args:
 *	fp - File pointer
 *	mode - 1 = enable echo, 0 = disable echo
 *
 * Return:
 *	Nothing.
 */
void
do_echo(FILE *fp, int mode)
{
	int			fd = fileno(fp);
#ifdef USE_TERMIOS
	struct termios		tio;

	(void) tcgetattr(fd, &tio);

	if (mode)
		tio.c_lflag |= ECHO;
	else
		tio.c_lflag &= ~ECHO;

	(void) tcsetattr(fd, TCSADRAIN, &tio);
#endif	/* USE_TERMIOS */

#ifdef USE_TERMIO
	struct termio		tio;

	(void) ioctl(fd, TCGETA, &tio);

	if (mode)
		tio.c_lflag |= ECHO;
	else
		tio.c_lflag &= ~ECHO;

	(void) ioctl(fd, TCSETAW, &tio);
#endif	/* USE_TERMIO */

#ifdef USE_SGTTY
	struct sgttyb		tio;

	(void) ioctl(fd, TIOCGETP, &tio);

	if (mode)
		tio.sg_flags |= ECHO;
	else
		tio.sg_flags &= ~ECHO;

	(void) ioctl(fd, TIOCSETP, &tio);
#endif	/* USE_SGTTY */
}


/*
 * Data used by b64encode
 */
char	b64map[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
	'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '+', '/'
};

#define B64_PAD		'='


/*
 * b64encode
 *	Base64 encoding function
 *
 * Args:
 *	ibuf - Input buffer pointer
 &	len - Length of data in input buffer
 &	obuf - Output buffer pointer
 *	brklines - Whether the encoded output should be broken
 *		   up into multiple lines (i.e., newlines are
 *		   inserted every 64 characters in accordance
 *		   to RFC 1521
 *
 *	It is assumed that the caller has pre-allocated an output
 *	buffer large enough to hold the encoded data, which should
 *	be 33% larger than the input data length (i.e., for every
 *	three bytes of input, there will be four bytes of output).
 *
 * Return:
 *	Nothing.
 */
void
b64encode(unsigned char *ibuf, int len, unsigned char *obuf, int brklines)
{
	int		i, j, k, n,
			c[4];
	unsigned char	sbuf[4];

	for (i = k = 0; (i + 3) <= len; i += 3, ibuf += 3) {
		c[0] = ((int) ibuf[0] >> 2);
		c[1] = ((((int) ibuf[0] & 0x03) << 4) |
			(((int) ibuf[1] & 0xf0) >> 4));
		c[2] = ((((int) ibuf[1] & 0x0f) << 2) |
			(((int) ibuf[2] & 0xc0) >> 6));
		c[3] = ((int) ibuf[2] & 0x3f);

		for (j = 0; j < 4; j++)
			*obuf++ = b64map[c[j]];

		if (brklines && ++k == 16) {
			k = 0;
			*obuf++ = '\n';
		}
	}

	if (i < len) {
		n = len - i;
		(void) strncpy((char *) sbuf, (char *) ibuf, n);
		for (j = n; j < 3; j++)
			sbuf[j] = (unsigned char) 0;

		n++;
		ibuf = sbuf;
		c[0] = ((int) ibuf[0] >> 2);
		c[1] = ((((int) ibuf[0] & 0x03) << 4) |
			(((int) ibuf[1] & 0xf0) >> 4));
		c[2] = ((((int) ibuf[1] & 0x0f) << 2) |
			(((int) ibuf[2] & 0xc0) >> 6));
		c[3] = ((int) ibuf[2] & 0x3f);

		for (j = 0; j < 4; j++)
			*obuf++ = (j < n) ? b64map[c[j]] : B64_PAD;

		if (brklines && ++k == 16)
			*obuf++ = '\n';
	}

	if (brklines)
		*obuf++ = '\n';

	*obuf = '\0';
}


/*
 * http_set_auth
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
http_set_auth(char *name, char *passwd)
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

	if ((buf = (char *) malloc(n1)) == NULL) {
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}

	(void) sprintf(buf, "%s:%s", name, passwd);

	(void) memset(name, 0, i);
	(void) memset(passwd, 0, j);

	if (auth_buf != NULL)
		free(auth_buf);

	if ((auth_buf = (char *) malloc(n2)) == NULL) {
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}

	/* Base64 encode the name/password pair */
	b64encode(
		(unsigned char *) buf,
		strlen(buf),
		(unsigned char *) auth_buf,
		0
	);

	(void) memset(buf, 0, n1);
	free(buf);
}


/*
 * get_proxy_auth
 *	Ask the user to enter user and password for proxy authorization.
 *
 * Args:
 *	None.
 *
 * Return:
 *	0 on success
 *	errno on failure
 */
int
get_proxy_auth(int retrycnt)
{
	FILE		*ttyfp;
	char		*ttypath,
			*name,
			*pass;
	char		input[64];

	if ((ttypath = ttyname(2)) == NULL)
		ttypath = "/dev/tty";
	if ((ttyfp = fopen(ttypath, "r+")) == NULL) {
		if (debug)
			fprintf(stderr, "Cannot open /dev/tty.\n");
		return EFAULT;
	}

	if (retrycnt == 0)
		(void) fprintf(ttyfp, "Proxy Authorization is required.\n");
	else {
		(void) fprintf(ttyfp, "Proxy Authorization failed.");
		if (retrycnt < 3)
			(void) fprintf(ttyfp, "  Try again.\n");
		else {
			(void) fprintf(ttyfp, "  Too many tries.\n\n");
			(void) fflush(ttyfp);
			(void) fclose(ttyfp);
			return EFAULT;
		}
	}

	(void) fprintf(ttyfp, "%s\n\n",
		"Please enter your proxy user name and password.");

	/* Get user name */
	(void) fprintf(ttyfp, "Username: ");
	if (fgets(input, 63, ttyfp) == NULL) {
		(void) fprintf(ttyfp, "\n");
		(void) fflush(ttyfp);
		return 0;
	}
	input[strlen(input)-1] = '\0';
	if (input[0] == '\0')
		return 0;

	name = (char *) malloc(strlen(input) + 1);
	if (name == NULL) {
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}
	(void) strcpy(name, input);
	(void) memset(input, 0, 64);
	(void) fflush(ttyfp);

	/* Get password */
	(void) fprintf(ttyfp, "Password: ");
	do_echo(ttyfp, 0);
	if (fgets(input, 63, ttyfp) == NULL) {
		do_echo(ttyfp, 1);
		(void) fprintf(ttyfp, "\n");
		(void) fflush(ttyfp);
		return 0;
	}
	do_echo(ttyfp, 1);
	input[strlen(input)-1] = '\0';
	if (input[0] == '\0')
		pass = NULL;
	else {
		pass = (char *) malloc(strlen(input) + 1);
		if (pass == NULL) {
			fprintf(stderr, "Out of memory.\n");
			exit(1);
		}
		(void) strcpy(pass, input);
		(void) memset(input, 0, 64);
	}
	(void) fflush(ttyfp);

	http_set_auth(name, pass == NULL ? "" : pass);

	free(name);
	free(pass);

	(void) fprintf(ttyfp, "\n\n");
	(void) fflush(ttyfp);
	(void) fclose(ttyfp);
	return 0;
}


/*
 * http_cmd
 *	Perform user-specified operation via http
 *
 * Args:
 *	cmd - The command string
 *	proxyhost - proxy server host name
 *	serverhost - CDDB server host name
 *	serverport - CDDB server TCP port number
 *	cgipath - The CGI path
 *	fp - socket stream
 *	buf - command and response buffer
 *	len - command and response buffer length
 *	depth - recursion depth
 *
 * Return:
 *	0 on success
 *	errno on failure
 */
int
http_cmd(
	char	*cmd,
	char	*proxyhost,
	char	*serverhost,
	int	serverport,
	char	*cgipath,
	FILE	*fp,
	char	*buf,
	int	len,
	int	depth
)
{
	int	n,
		code,
		ret;
	char	*p,
		*q,
		str[CMD_BUFLEN],
		str2[CMD_BUFLEN];

	/* Translate cmd to http CGI format */
	for (p = cmd, q = str; *p != '\0'; q++) {
		if (isspace(*p)) {
			*q = '+';
			while (isspace(*(++p)))
				;
		}
		else
			*q = *p++;
	}
	*q = '\0';

	if (proxyhost != NULL)
		(void) sprintf(str2, "http://%s:%d%s",
			       serverhost, serverport, cgipath);
	else
		(void) strcpy(str2, cgipath);

	(void) sprintf(buf,
	       "GET %s?cmd=%s&hello=%s+%s+%s+v%s%sPL%d&proto=%d HTTP/1.0\r\n",
		       str2, str, user, myhost,
		       CLIENT, VERSION, VERSION_EXT, PATCHLEVEL, CDDB_PROTO);
	n = strlen(buf);
	if (my_fputs(buf, fp) < n)
		return (errno);

#ifdef NO_MASQUERADE
	(void) sprintf(buf, "User-Agent: %s/%s%sPL%d\r\n",
#else
	/* Believe it or not, this is how MS Internet Explorer does it.
	 * so don't blame me...
	 */
	(void) sprintf(buf, "User-Agent: %s (compatible; %s %s%sPL%d)\r\n",
		       "Mozilla/3.0",
#endif
		       CLIENT, VERSION, VERSION_EXT, PATCHLEVEL);
	n = strlen(buf);
	if (my_fputs(buf, fp) < n)
		return (errno);

	(void) sprintf(buf, "Host: %s\r\n", serverhost);
	n = strlen(buf);
	if (my_fputs(buf, fp) < n)
		return (errno);

	(void) strcpy(buf, "Accept: text/plain\r\n");
	n = strlen(buf);
	if (my_fputs(buf, fp) < n)
		return (errno);

	if (proxyhost != NULL && auth_buf != NULL) {
		(void) sprintf(buf, "Proxy-Authorization: Basic %s\r\n",
			       auth_buf);
		n = strlen(buf);
		if (my_fputs(buf, fp) < n)
			return (errno);
	}

	(void) strcpy(buf, "\r\n");
	n = strlen(buf);
	if (my_fputs(buf, fp) < n)
		return (errno);

	/* Check HTTP banner */
	if (my_fgets(buf, len, fp) == NULL)
		return EFAULT;
	if (strncmp(buf, "HTTP/", 5) != 0) {
		(void) fprintf(stderr, "HTTP protocol error.\n");
		return EFAULT;
	}

	/* Check HTTP status code */
	q = strchr(buf, ' ');
	if (q != NULL && isdigit(*(q+1))) {
		int	httpcode,
			class;

		httpcode = atoi(q+1);
		class = httpcode / 100;

		switch (class) {
		case 2:	/* Successful */
			break;
		case 3:	/* Redirection */
			(void) fprintf(stderr,
				       "HTTP redirection status %d\n",
				       httpcode);
			break;
		case 4:	/* Client error */
			if (httpcode == 407 && proxyhost != NULL) {
				/* Proxy authorization failed */
				if ((ret = get_proxy_auth(depth)) != 0)
					return (ret);
				
				/* Close the socket */
				fclose(fp);

				/* Recurse */
				return (server_connect(proxyhost, proxyport,
						       serverhost, serverport,
						       cgipath, cddbproto,
						       cmd, ++depth));
			}
			else
				(void) fprintf(stderr,
					       "HTTP client error: status %d\n",
					       httpcode);
			break;
		case 5:	/* Server error */
			(void) fprintf(stderr,
				       "HTTP server error: status %d\n",
				       httpcode);
			break;
		default:
			(void) fprintf(stderr, "HTTP status %d\n", httpcode);
			break;
		}
	}

	/* Skip the rest of the HTTP junk */
	while (my_fgets(buf, len, fp) != NULL) {
		if (isdigit(buf[0]) && isdigit(buf[1]) && isdigit(buf[2]) &&
		    isspace(buf[3]))
			break;
	}
	if (sscanf(buf, "%d %[^\n]\n", &code, str) < 2) {
		(void) fprintf(stderr, "Unexpected server output:\n");
		(void) fprintf(stderr, "%s\n", buf);
		return EFAULT;
	}

	/* Check code */
	switch (code / 10) {
	case 20:
	case 23:
	case 30:
	case 33:
		/* Print status */
		/* Eat CR-LF */
		for (p = buf + 4; *p != '\0'; p++) {
			if (*p == '\r') {
				*p = '\0';
				break;
			}
		}

		(void) printf("%s\n", buf + 4);
		break;

	case 21:
	case 31:
		/* Print server output */
		while (my_fgets(buf, len, fp) != NULL) {
			if (buf[0] == '.')
				break;

			/* Eat CR-LF */
			for (p = buf + 4; *p != '\0'; p++) {
				if (*p == '\r') {
					*p = '\0';
					break;
				}
			}

			(void) printf("%s\n", buf);
		}
		break;

	case 22:
	case 32:
		/* Read input and send to server */
		while (fgets(buf, len, stdin) != NULL) {
			if (buf[0] == '.')
				break;

			n = strlen(buf);
			if (my_fputs(buf, fp) < n)
				return EFAULT;
		}
		strcpy(buf, ".\n");
		n = strlen(buf);
		if (my_fputs(buf, fp) < n)
			return EFAULT;
		break;

	default:
		(void) fprintf(stderr, "Command code %d: %s", code, buf + 4);
		return EFAULT;
	}

	return 0;
}


/*
 * cddbp_hello
 *	Perform cddbp hello command
 *
 * Args:
 *	fp - socket stream
 *	buf - command and response buffer
 *	len - command and response buffer length
 *
 * Return:
 *	0 on success
 *	errno on failure
 */
int
cddbp_hello(FILE *fp, char *buf, int len)
{
	int	n,
		code;
	char	str[CMD_BUFLEN];

	/* Check server sign on banner */
	if (my_fgets(buf, len, fp) == NULL) {
		(void) fprintf(stderr, "Cannot read server sign-on banner.\n");
		return EFAULT;
	}

	n = sscanf(buf, "%d %[^\n]\n", &code, str);
	if (n < 2) {
		(void) fprintf(stderr, "Server sign on banner unrecognized:\n");
		(void) fprintf(stderr, "%s\n", buf);
		return EFAULT;
	}
	/* Check code */
	if ((code / 100) != 2) {
		(void) fprintf(stderr, "Server sign on error: code %d\n", code);
		return EFAULT;
	}

	/* Send hello handshake */
	(void) sprintf(buf, "cddb hello %s %s %s v%s%sPL%d\r\n",
		       user, myhost, CLIENT, VERSION, VERSION_EXT, PATCHLEVEL);
	n = strlen(buf);
	if (my_fputs(buf, fp) < n)
		return (errno);

	/* Check command status */
	if (my_fgets(buf, len, fp) == NULL)
		return EFAULT;

	if (sscanf(buf, "%d %[^\n]\n", &code, str) < 2) {
		(void) fprintf(stderr, "Unexpected server output:\n");
		(void) fprintf(stderr, "%s\n", buf);
		return EFAULT;
	}
	/* Check code */
	if ((code / 100) != 2) {
		(void) fprintf(stderr, "Hello command error: code %d\n", code);
		return EFAULT;
	}

	return 0;
}


/*
 * cddbp_proto
 *	Perform cddbp proto command
 *
 * Args:
 *	cddbproto - the CDDBP protocol level to change to
 *	fp - socket stream
 *	buf - command and response buffer
 *	len - command and response buffer length
 *
 * Return:
 *	0 on success
 *	errno on failure
 */
int
cddbp_proto(int cddbproto, FILE *fp, char *buf, int len)
{
	int	n,
		code;
	char	str[CMD_BUFLEN];

	(void) sprintf(buf, "proto %d\r\n", cddbproto);
	n = strlen(buf);
	if (my_fputs(buf, fp) < n)
		return (errno);

	/* Check command status */
	if (my_fgets(buf, len, fp) == NULL)
		return EFAULT;

	if (sscanf(buf, "%d %[^\n]\n", &code, str) < 2) {
		(void) fprintf(stderr, "Unexpected server output:\n");
		(void) fprintf(stderr, "%s\n", buf);
		return EFAULT;
	}
	/* Check code */
	if ((code / 100) != 2) {
		(void) fprintf(stderr, "Proto command error: code %d\n", code);
		return EFAULT;
	}

	return 0;
}


/*
 * cddbp_cmd
 *	Perform user-specified command via cddbp
 *
 * Args:
 *	cmd - the command string
 *	fp - socket stream
 *	buf - command and response buffer
 *	len - command and response buffer length
 *
 * Return:
 *	0 on success
 *	errno on failure
 */
int
cddbp_cmd(char *cmd, FILE *fp, char *buf, int len)
{
	int	n,
		code;
	char	*p,
		str[CMD_BUFLEN];

	(void) sprintf(buf, "%s\r\n", cmd);
	n = strlen(buf);
	if (my_fputs(buf, fp) < n)
		return (errno);

	/* Check command status */
	if (my_fgets(buf, len, fp) == NULL)
		return EFAULT;

	if (sscanf(buf, "%d %[^\n]\n", &code, str) < 2) {
		(void) fprintf(stderr, "Unexpected server output:\n");
		(void) fprintf(stderr, "%s\n", buf);
		return EFAULT;
	}

	/* Check code */
	switch (code / 10) {
	case 20:
	case 23:
	case 30:
	case 33:
		/* Print status */
		/* Eat CR-LF */
		for (p = buf + 4; *p != '\0'; p++) {
			if (*p == '\r') {
				*p = '\0';
				break;
			}
		}

		(void) printf("%s\n", buf + 4);
		break;

	case 21:
	case 31:
		/* Print server output */
		while (my_fgets(buf, len, fp) != NULL) {
			if (buf[0] == '.')
				break;

			/* Eat CR-LF */
			for (p = buf + 4; *p != '\0'; p++) {
				if (*p == '\r') {
					*p = '\0';
					break;
				}
			}

			(void) printf("%s\n", buf);
		}
		break;

	case 22:
	case 32:
		/* Read input and send to server */
		while (fgets(buf, len, stdin) != NULL) {
			if (buf[0] == '.')
				break;

			n = strlen(buf);
			if (my_fputs(buf, fp) < n)
				return EFAULT;
		}
		strcpy(buf, ".\n");
		n = strlen(buf);
		if (my_fputs(buf, fp) < n)
			return EFAULT;
		break;

	default:
		(void) fprintf(stderr, "Command code %d: %s", code, buf + 4);
		return EFAULT;
	}

	return 0;
}


/*
 * cddbp_quit
 *	Perform cddbp quit command to terminate connection
 *
 * Args:
 *	fp - socket stream
 *	buf - command and response buffer
 *	len - command and response buffer length
 *
 * Return:
 *	0 on success
 *	errno on failure
 */
int
cddbp_quit(FILE *fp, char *buf, int len)
{
	int	n;

	(void) strcpy(buf, "quit\r\n");
	n = strlen(buf);
	if (my_fputs(buf, fp) < n)
		return (errno);

	/* Eat server output */
	(void) my_fgets(buf, len, fp);

	return 0;
}


/*
 * server_connect
 *	Connect to server and perform commands
 *
 * Args:
 *	proxyhost - The proxy server host name
 *	proxyport - The proxy TCP port number to use
 *	serverhost - The CDDB server host name
 *	serverport - The CDDB server TCP port number to use
 *	cgipath - The CGI path (if http mode)
 *	cddbproto - The CDDBP protocol level
 *	cmd - The suser-specified command string
 *
 * Return:
 *	0 on success
 *	errno on failure
 */
int
server_connect(
	char	*proxyhost,
	int	proxyport,
	char	*serverhost,
	int	serverport,
	char	*cgipath,
	int	cddbproto,
	char	*cmd,
	int	depth
)
{
	struct hostent		*hp;
	struct in_addr		ad;
	struct sockaddr_in	sin;
	char			*buf,
				*host;
	int			port,
				fd,
				ret;
	FILE			*fp;

	if (proxyhost != NULL) {
		host = proxyhost;
		port = proxyport;
	}
	else {
		host = serverhost;
		port = serverport;
	}

	sin.sin_port = htons((unsigned short) port);
	sin.sin_family = AF_INET;

	/* Set timeout in case remote server is unavailable */
	signal(SIGALRM, onalrm);
	alarm(60);

	/* Find server host address */
	if ((hp = gethostbyname(host)) != NULL) {
		memcpy((char *) &sin.sin_addr, hp->h_addr, hp->h_length);
	}
	else {
		if ((ad.s_addr = inet_addr(host)) != -1) {
			memcpy((char *) &sin.sin_addr,
			       (char *) &ad.s_addr, sizeof(ad.s_addr));
		}
		else {
			(void) fprintf(stderr, "Unknown host!\n");
			return EINVAL;
		}
	}

	/* Open socket */
	if ((fd = SOCKET(AF_INET, SOCK_STREAM, 0)) < 0) {
		ret = errno;
		perror("Socket failed");
		return (ret);
	}

	/* Connect to server */
	if (CONNECT(fd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		ret = errno;
		perror("Connect failed");
		close(fd);
		return (ret);
	}

	/* Cancel alarm */
	alarm(0);

	/* Allocate command buffer */
	if ((buf = (char *) malloc(CMD_BUFLEN)) == NULL) {
		(void) fprintf(stderr, "Out of memory.\n");
		close(fd);
		return ENOMEM;
	}

	if ((fp = fdopen(fd, "r+")) == NULL) {
		ret = errno;
		(void) fprintf(stderr, "fdopen of socket failed\n");
		return (ret);
	}

	switch (proto) {
	case PROTO_CDDBP:
		/* Verify server sign-on banner */
		if ((ret = cddbp_hello(fp, buf, CMD_BUFLEN)) != 0) {
			free(buf);
			return (ret);
		}

		/* Set the CDDB protocol level if > 1 */
		if (cddbproto > 1 &&
		    (ret = cddbp_proto(cddbproto, fp, buf, CMD_BUFLEN)) != 0) {
			free(buf);
			return (ret);
		}

		/* Do the designated operation */
		if ((ret = cddbp_cmd(cmd, fp, buf, CMD_BUFLEN)) != 0) {
			free(buf);
			return (ret);
		}

		/* Close the connection */
		if ((ret = cddbp_quit(fp, buf, CMD_BUFLEN)) != 0) {
			free(buf);
			return (ret);
		}
		break;

	case PROTO_HTTP:
		if ((ret = http_cmd(cmd, proxyhost, serverhost, serverport,
				    cgipath, fp, buf, CMD_BUFLEN,
				    depth)) != 0) {
			free(buf);
			return (ret);
		}
		break;

	default:
		return EINVAL;
	}

	/* Succeeded */
	free(buf);
	return 0;
}


/*
 * usage
 *	Print command line usage help
 *
 * Args:
 *	progname - The program name
 *
 * Return:
 *	0 on success
 *	errno on failure
 */
void
usage(char *progname)
{
	(void) fprintf(stderr, "Usage: %s %s\n\t\t%s\n\t\t%s\n",
		       progname,
		       "[-m <cddbp | http>] [-p proxyhost[:port]]",
		       "[-h serverhost[:port]] [-c cgipath] [-l level]",
		       "[-d] command ...");
}


/*
 * main
 *	The main function
 */
int
main(int argc, char **argv)
{
	int		i,
			n,
			ret,
			c;
	uid_t		ouid;
	struct passwd	*pw;
	struct hostent	*he;
	char		*cp,
			**ap;

	serverhost = DFLHOST;
	proxyhost = NULL;
	proto = PROTO_CDDBP;
	cgipath = CGIPATH;
	cddbproto = CDDB_PROTO;
	serverport = -1;
	proxyport = PORT_HTTP;

	while ((c = getopt(argc, argv, "?dm:p:c:l:h:")) != -1) {
		switch (c) {
		case 'm':
			if (strcmp(optarg, "cddbp") == 0)
				proto = PROTO_CDDBP;
			else if (strcmp(optarg, "http") == 0)
				proto = PROTO_HTTP;
			else {
				(void) fprintf(stderr,
					"Invalid arg used with -%c\n", c);
				usage(argv[0]);
				exit(1);
			}
			break;

		case 'p':
			proxyhost = optarg;
			if ((cp = strchr(proxyhost, ':')) != NULL) {
				*cp = '\0';
				proxyport = atoi(++cp);
			}
			break;

		case 'h':
			serverhost = optarg;
			if ((cp = strchr(serverhost, ':')) != NULL) {
				*cp = '\0';
				serverport = atoi(++cp);
			}
			break;

		case 'c':
			cgipath = optarg;
			break;

		case 'l':
			cddbproto = atoi(optarg);
			break;

		case 'd':
			debug = 1;
			break;

		case '?':
			usage(argv[0]);
			exit(0);

		default:
			(void) fprintf(stderr, "Unknown option -%c\n", c);
			usage(argv[0]);
			exit(1);
		}
	}

	if (argc < (optind + 1)) {
		(void) fprintf(stderr, "No server command specified.\n");
		usage(argv[0]);
		exit(1);
	}

	/* Set up command string */
	cmd[0] = '\0';
	n = sizeof(cmd);
	i = optind;
	for (;;) {
		if ((int) (n -= (strlen(argv[i]) + 2)) < 0) {
			(void) fprintf(stderr, "Command string too long.\n");
			exit(1);
		}
		(void) strcat(cmd, argv[i]);
		if (++i == argc)
			break;
		(void) strcat(cmd, " ");
	}

	if (serverport < 0) {
		/* Set default port number */
		if (proto == PROTO_CDDBP)
			serverport = PORT_CDDBP;
		else
			serverport = PORT_HTTP;
	}

	if (proxyhost != NULL && proto == PROTO_CDDBP) {
		(void) fprintf(stderr,
			"The -p option cannot be used in cddbp mode.\n");
		exit(1);
	}

	if (debug)
		(void) fprintf(stderr, "%s=%s %s=%d %s=%s %s=%d\n",
			"serverhost", serverhost,
			"serverport", serverport,
			"proxyhost", (proxyhost == NULL) ? "-" : proxyhost,
			"proxyport", proxyport);

	/* Try to get the host name */
	if (gethostname(myhost, MAXHOSTNAMELEN-1) < 0 ||
	    (he = gethostbyname(myhost)) == NULL) {
		(void) strcpy(myhost, "unknown");
	}
	else {
		/* Look for a a fully-qualified hostname
		 * (with domain)
		 */
		if (strchr(he->h_name, '.') != NULL) {
			(void) strncpy(myhost, he->h_name, MAXHOSTNAMELEN-1);
		}
		else {
			for (ap = he->h_aliases; *ap != NULL; ap++) {
				if (strchr(*ap, '.') != NULL) {
				    (void) strncpy(myhost, *ap,
						   MAXHOSTNAMELEN-1);
				    break;
				}
			}
		}
	}
	myhost[MAXHOSTNAMELEN-1] = '\0';

	ouid = getuid();

	if ((pw = getpwuid(ouid)) != NULL)
		(void) strncpy(user, pw->pw_name, MAXUSERNAMELEN-1);
	else if ((cp = getenv("LOGNAME")) != NULL)
		(void) strncpy(user, cp, MAXUSERNAMELEN-1);
	else if ((cp = getenv("USER")) != NULL)
		(void) strncpy(user, cp, MAXUSERNAMELEN-1);
	else
		(void) strcpy(user, "nobody");

	user[MAXUSERNAMELEN-1] = '\0';

	SOCKSINIT("xmcd");

	ret = server_connect(proxyhost, proxyport,
			     serverhost, serverport,
			     cgipath, cddbproto, cmd, 0);

	exit((ret == 0) ? 0 : errno);
	/*NOTREACHED*/
}


