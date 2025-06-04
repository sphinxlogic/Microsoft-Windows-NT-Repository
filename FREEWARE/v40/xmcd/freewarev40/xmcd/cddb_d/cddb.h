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
#ifndef __CDDB_H__
#define __CDDB_H__

#ifndef LINT
static char *_cddb_h_ident_ = "@(#)cddb.h	6.53 98/01/02";
#endif

#ifndef NOREMOTE

#ifndef __VMS
/* UNIX */
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#if defined(_AIX) || defined(__QNX__)
#include <sys/select.h>
#endif

#else
/* OpenVMS */
#include <socket.h>
#include <time.h>
#include <in.h>
#include <netdb.h>

#ifndef __FD_SET

/* OpenVMS hack: These are normally defined in socket.h
 * but missing in some versions of OpenVMS.
 */
typedef int		fd_mask;

#ifndef FD_SETSIZE
#define FD_SETSIZE	4096
#endif
#ifndef howmany
#define howmany(x, y)	(((x)+((y)-1))/(y))
#endif
#define NFDBITS		(sizeof(fd_mask) * 8)	/* bits per mask */

typedef struct fd_set {
	fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)	(void) memset((char *)(p), 0, sizeof(*(p)))

#endif	/* __FD_SET */

#endif	/* __VMS */

#else

/* NOREMOTE implies SYNCHRONOUS */
#ifndef SYNCHRONOUS
#define SYNCHRONOUS
#endif

/* NOREMOTE implies no SOCKS */
#ifdef SOCKS
#undef SOCKS
#endif

#endif	/* NOREMOTE */

#ifdef SOCKS
#define SOCKSINIT(x)		SOCKSinit(x)
#define SOCKET			socket
#define CONNECT			Rconnect
#else
#define SOCKSINIT(x)
#define SOCKET			socket
#define CONNECT			connect
#endif	/* SOCKS */


/* CDDB protocol level used */
#define CDDBP_PROTO		1

/* HTTP mode version string */
#define CDDB_HTTP_VER		"HTTP/1.0"

/* HTTP mode default port */
#define HTTP_DFLT_PORT		80

/* HTTP status codes */
#define HTTP_PROXYAUTH_FAIL	407

/* cddbPath and XMCD_CDDBPATH component separator character */
#define CDDBPATH_SEPCHAR	';'

/* DISCID component separator character */
#define CDDBID_SEPCHAR		','

/* CDDB linking parameters */
#define OFFSET_UNKN		0xffffffff
#define OFFSET_THRESH		750

/* CD database type flags */
#define CDDB_INVALID		0
#define CDDB_LOCAL		1
#define CDDB_REMOTE_CDDBP	2
#define CDDB_REMOTE_HTTP	3
#define CDDB_WAIT		4

/* Macros to extract info from cddb_ret_t */
#define CDDB_SET_CODE(stat, arg)	((stat) | (arg) << 16)
#define CDDB_GET_STAT(code)		((code) & 0xffff)
#define CDDB_GET_ARG(code)		((code) >> 16)

/* Error status codes */
#define OPEN_ERR		50
#define CLOSE_ERR		51
#define SETUID_ERR		52
#define READ_ERR		53
#define WRITE_ERR		54
#define LINK_ERR		55
#define FORK_ERR		56
#define KILLED_ERR		57
#define MATCH_ERR		58
#define INCMPL_ERR		59
#define MEM_ERR			60
#define CMD_ERR			61
#define AUTH_ERR		62

/* Max hostname length */
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN  64
#endif


/* Message dialog macros */
#define CDDB_FATAL(msg)		{					\
	if (cddb_clinfo->fatal_msg != NULL && !ischild)			\
		cddb_clinfo->fatal_msg(app_data.str_fatal, (msg));	\
	else {								\
		(void) fprintf(errfp, "Fatal: %s\n", (msg));		\
		exit(1);						\
	}								\
}
#define CDDB_WARNING(msg)	{					\
	if (cddb_clinfo->warning_msg != NULL && !ischild)		\
		cddb_clinfo->warning_msg(app_data.str_warning, (msg));	\
	else								\
		(void) fprintf(errfp, "Warning: %s\n", (msg));		\
}
#define CDDB_INFO(msg)		{					\
	if (cddb_clinfo->info_msg != NULL && !ischild)			\
		cddb_clinfo->info_msg(app_data.str_info, (msg));	\
	else								\
		(void) fprintf(errfp, "Info: %s\n", (msg));		\
}


typedef int		cddb_ret_t;


/* Client information structure */
typedef struct {
	char	prog[FILE_PATH_SZ];		/* Client program name */
	char	host[MAXHOSTNAMELEN];		/* Client host name */
	char	user[STR_BUF_SZ];		/* Client user name */
	bool_t	(*isdemo)(void);		/* Demo mode func */
	curstat_t * (*curstat_addr)(void);	/* Current status structure */
	void	(*fatal_msg)(char *, char *);	/* Fatal message popup func */
	void	(*warning_msg)(char *, char *);	/* Warning message popup func */
	void	(*info_msg)(char *, char *);	/* Info message popup func */
	void	(*workproc)(int);		/* Function to run while
						 * waiting for I/O
						 */
	int	arg;				/* Argument to workproc */

} cddb_client_t;


/* CDDB entry match list structure */
typedef struct dbmatch {
	char		category[FILE_BASE_SZ];	/* CD category */
	word32_t	discid;			/* Disc ID */
	char		*dtitle;		/* Disc title of match */
	struct dbmatch	*next;			/* Link to next match */
} cddb_match_t;


/* CDDB link options structure */
typedef struct lnopts {
	char		*dtitle;		/* Disc title */
	char		idstr[9];		/* Disc id string */
	word32_t	offset;			/* Distance measure */
	struct lnopts	*next;			/* Link to next item */
} cddb_linkopts_t;


#define CDDB_INEXACT	0x0001			/* CDDB close match found */
#define CDDB_AUTHFAIL	0x0002			/* Proxy auth failed */
#define CDDB_CHANGED	0x0003			/* In-core CDDB was edited */
#define CDDB_AUTOSAVE	0x0004			/* An autosave was done */


/* In-core CD database info structure */
typedef struct {
	word32_t	discid;			/* Magic disc ID */
	word32_t	queryid;		/* Disc ID for CDDB access */
	int		type;			/* CDDB_LOCAL or CDDB_REMOTE */
	int		flags;			/* Flags */
	int		revision;		/* Revision */
	char		*dtitle;		/* Disc title */
	char		*trklist[MAXTRACK];	/* Track title list */
	char		*extd;			/* Extended disc info */
	char		*extt[MAXTRACK];	/* Extended track info */
	char		*sav_extt[MAXTRACK];	/* Bkup extended track info */
	char		*playorder;		/* Track play order */
	char		*dbfile;		/* Path to database file */
	char		category[FILE_BASE_SZ];	/* CD category */
	cddb_linkopts_t	*idlist;		/* CDDB disc ID list */
	cddb_match_t	*matchlist;		/* CDDB match list */
	cddb_match_t	*match_cur;		/* Running ptr for matchlist */
} cddb_incore_t;


#define CDDB_FMAGIC	0xcddbf			/* Magic number */
#define CDDB_CACHESZ	1024			/* File I/O cache size */


/* CDDB path list structure */
typedef struct dbpath {
	int		type;			/* CDDB_LOCAL,
						 * CDDB_REMOTE_CDDBP or
						 * CDDB_REMOTE_HTTP
						 */
	char		*host;			/* Host name or IP number */
	int		port;			/* Remote CDDB port */
	char		*path;			/* Directory path */
	struct dbpath	*next;			/* Link to next path */
} cddb_path_t;


/* CDDB file handle structure */
typedef struct {
	int		magic;			/* Magic number */
	int		fd;			/* File descriptor */
	int		rw;			/* O_RDONLY, O_WRONLY */
	int		pos;			/* Cache position */
	int		cnt;			/* Cache character count */
	unsigned char	*cache;			/* I/O cache */
	int		type;			/* CDDB_LOCAL or CDDB_REMOTE */
	cddb_path_t	*pathp;			/* Pointer to cddb_path_t */
} cddb_file_t;


/* CDDB proxy server information structure */
typedef struct {
	char		host[MAXHOSTNAMELEN];	/* Host name */
	int		port;			/* Port number */
} cddb_proxy_t;


/* Public functions */
extern void		cddb_init(cddb_client_t *);
extern void		cddb_halt(curstat_t *);
extern cddb_ret_t	cddb_load(cddb_incore_t *, curstat_t *);
extern cddb_ret_t	cddb_save(cddb_incore_t *, curstat_t *);
extern cddb_ret_t	cddb_link(cddb_incore_t *, char *, char *);
extern cddb_ret_t	cddb_send(cddb_incore_t *, curstat_t *);
extern void		cddb_clear(cddb_incore_t *, curstat_t *s, bool_t);
extern void		cddb_load_cancel(void);
extern word32_t		cddb_discid(curstat_t *);
extern char		*cddb_category(char *);
extern void		cddb_set_auth(char *, char *);
extern cddb_path_t	*cddb_pathlist(void);
extern bool_t		cddb_init_linkopts(cddb_incore_t *, curstat_t *);
extern void		cddb_free_linkopts(void);
extern cddb_linkopts_t	*cddb_linkopts(void);
extern bool_t		cddb_rmt_support(void);
extern void		cddb_curfileupd(cddb_incore_t *);
extern bool_t		cddb_issync(void);
extern void		cddb_dump_incore(cddb_incore_t *, curstat_t *s);

#endif	/* __CDDB_H__ */

