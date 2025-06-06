/*
 *  Project   : tin - a Usenet reader
 *  Module    : header.c
 *  Author    : Urs Janssen <urs@tin.org>
 *  Created   : 1997-03-10
 *  Updated   : 1997-03-19
 *  Copyright : (c) Copyright 1997-99 by Urs Janssen
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made.
 */

#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */
#ifndef TNNTP_H
#	include "tnntp.h"
#endif /* !TNNTP_H */

#ifndef INDEX_DAEMON
	static const char * get_full_name (void);
	static const char * get_user_name (void);
#endif /* !INDEX_DAEMON */


/* find hostname */
const char *
get_host_name (
	void)
{
	char *ptr;
	static char hostname[MAXHOSTNAMELEN+1]; /* need space for '\0' */

	hostname[0]='\0';

#ifdef HAVE_GETHOSTBYNAME
	gethostname(hostname, sizeof(hostname));
#else
#	if defined(M_AMIGA) || defined(M_OS2)
	if ((ptr = getenv("NodeName")) != (char *) 0)
		strncpy(hostname, ptr, MAXHOSTNAMELEN);
#	else
#		if defined(WIN32)
	if ((ptr = getenv("COMPUTERNAME")) != (char *) 0)
		strncpy(hostname, ptr, MAXHOSTNAMELEN);
#		endif /* WIN32 */
#	endif /* M_AMIGA || M_OS2 */
#endif /* HAVE_GETHOSTBYNAME */
#ifdef HAVE_SYS_UTSNAME_H
	if (! *hostname)
		strcpy(hostname, system_info.nodename);
#endif /* HAVE_SYS_UTSNAME_H */
	if (! *hostname) {
		if ((ptr = getenv("HOST")) != (char *) 0)
			strncpy (hostname, ptr, MAXHOSTNAMELEN);
		else {
			if ((ptr = getenv("HOSTNAME")) != (char *) 0)
				strncpy (hostname, ptr, MAXHOSTNAMELEN);
			else
				hostname[0]='\0';
		}
	}
	hostname[MAXHOSTNAMELEN]='\0';
	return (hostname);
}


#ifdef DOMAIN_NAME
/* find domainname - check DOMAIN_NAME */
const char *
get_domain_name (
	void)
{
	char *ptr;
	char buff[MAXHOSTNAMELEN];
	FILE *fp;
	static char domain[8192];

#	if defined(M_AMIGA)
/*
 * Damn compiler bugs...
 * Without this hack, SASC 6.55 produces a TST.B d16(pc),
 * which is illegal on a 68000
 */
static const char *domain_name_hack = DOMAIN_NAME;
#		undef	DOMAIN_NAME
#		define DOMAIN_NAME domain_name_hack
#	endif /* M_AMIGA */

	domain[0]='\0';

	if (strlen(DOMAIN_NAME))
		strcpy(domain, DOMAIN_NAME);

#	if defined(M_AMIGA)
	if (strchr(domain, ':')) { /* absolute AmigaOS paths contain one, RFC-hostnames don't */
#	else
	if (domain[0] == '/' && domain[1]) {
#	endif /* M_AMIGA */
		/* If 1st letter is '/' read domainname from specified file */
		if ((fp = fopen (domain, "r")) != (FILE *) 0) {
			while (fgets (buff, (int) sizeof (buff), fp) != (char *) 0) {
				if (buff[0] == '#' || buff[0] == '\n')
					continue;

				if ((ptr = strrchr (buff, '\n'))) {
					*ptr = '\0';
					strcpy (domain, buff);
				}
			}
			if (domain[0] == '/') /* file was empty */
				domain[0]='\0';

			fclose (fp);
		} else
			domain[0]='\0';
	}
	domain[MAXHOSTNAMELEN]='\0';
	return (domain);
}
#endif /* DOMAIN_NAME */


#ifdef HAVE_GETHOSTBYNAME
#	define MAXLINELEN   1024
/* find FQDN - gethostbyaddr() */
const char *
get_fqdn (
	const char *host)
{
	FILE *inf;
	char *cp, *domain;
	char line[MAXLINELEN+1];
	char name[MAXHOSTNAMELEN+2];
	static char fqdn[1024];
	struct hostent *hp;
	struct in_addr in;

	*fqdn = '\0';
	domain = NULL;

	name[MAXHOSTNAMELEN] = '\0';
	if (host) {
		if (strchr(host, '.'))
			return host;
		(void) strncpy(name, host, MAXHOSTNAMELEN);
	} else
		if (gethostname(name, MAXHOSTNAMELEN))
			return(NULL);

	if ('0' <= *name && *name <= '9') {
		in.s_addr = inet_addr(name);
		if ((hp = gethostbyaddr((char *)&in.s_addr, 4, AF_INET)))
			in.s_addr = (*hp->h_addr);
		return(hp && strchr(hp->h_name, '.') ? hp->h_name : inet_ntoa(in));
	}
	if ((hp = gethostbyname(name)) && !strchr(hp->h_name, '.'))
		if ((hp = gethostbyaddr(hp->h_addr, hp->h_length, hp->h_addrtype)))
			in.s_addr = (*hp->h_addr);

	sprintf(fqdn,"%s", hp
		? strchr(hp->h_name, '.')
			? hp->h_name
			: inet_ntoa(in)
		: "");
	if (!*fqdn || (fqdn[strlen(fqdn)-1] <= '9')) {
		*fqdn = '\0';
		inf = fopen("/etc/resolv.conf", "r");
		if (inf) {
			while(fgets(line, MAXLINELEN, inf)) {
				line[MAXLINELEN] = '\0';
				(void) str_trim(line);
				if (strncmp(line,"domain ", 7) == 0) {
					domain = line + 7;
					break;
				}
				if (strncmp(line, "search ", 7) == 0) {
					domain = line + 7;
					cp = strchr(domain, ' ');
					if (cp)
						*cp = '\0';
					break;
				}
			}
			if (domain)
				sprintf(fqdn, "%s.%s", name, domain);
			fclose(inf);
		}
	}

	return(fqdn);
}
#endif /* HAVE_GETHOSTBYNAME */


/*
 * Find username & fullname
 */
#ifndef INDEX_DAEMON
void
get_user_info (
	char *user_name,
	char *full_name)
{
	const char *ptr;

	user_name[0] = '\0';
	full_name[0] = '\0';

	if ((ptr = get_full_name()))
		strcpy(full_name, ptr);
	if ((ptr = get_user_name()))
		strcpy(user_name, ptr);
}
#endif /* !INDEX_DAEMON */


#ifndef INDEX_DAEMON
static const char *
get_user_name (
	void)
{
#	if defined (M_AMIGA) || (defined VMS)
	char *p;
#	endif /* M_AMIGA || VMS */
	static char username[128];
	struct passwd *pw;

	username[0] = '\0';
#	if defined (M_AMIGA) || defined (VMS)
	if ((p = getenv ("USER"))) {
		STRCPY (username, p);
#		ifdef VMS
		lower (username);
#		endif /* VMS */
	}
#	else
	pw = getpwuid (getuid ());
	strcpy (username, pw->pw_name);
#	endif /* M_AMIGA || VMS */
	return(username);
}
#endif /* !INDEX_DAEMON */


#ifndef INDEX_DAEMON
static const char *
get_full_name (
	void)
{
	char *p;
	char buf[128];
	char tmp[128];
	static char fullname[128];
	struct passwd *pw;

	fullname[0] = '\0';

	if ((p = getenv ("NAME")) != (char *) 0) {
		strncpy (fullname, p, sizeof (fullname));
		return (fullname);
	}
	if ((p =  getenv ("REALNAME")) != (char *) 0) {
		strncpy (fullname, p, sizeof (fullname));
		return (fullname);
	}

#	ifdef VMS
	strncpy (fullname, fix_fullname(get_uaf_fullname()), sizeof (fullname));
#	else
	pw = getpwuid (getuid ());
	strncpy (buf, pw->pw_gecos, sizeof (fullname));
	if ((p = strchr (buf, ',')))
		*p = '\0';
	if ((p = strchr (buf, '&'))) {
		*p++ = '\0';
		strcpy (tmp, pw->pw_name);
		/* strcpy(tmp, get_user_name()); */
		if (*tmp && *tmp >= 'a' && *tmp <= 'z')
			*tmp = *tmp - 32;
		sprintf (fullname, "%s%s%s", buf, tmp, p);
	} else
		strcpy (fullname, buf);
#	endif /* VMS */
	return (fullname);
}
#endif /* !INDEX_DAEMON */


/*
 * FIXME to:
 * char * build_from(full_name, user_name, domain_name)
 */
/*
 * build From: in 'name <user@host.doma.in>' format
 */
#ifndef INDEX_DAEMON
void
get_from_name (
	char *from_name,
	struct t_group *thisgrp)
{
#	ifdef USE_INN_NNTPLIB
	char *fromhost = GetConfigValue (_CONF_FROMHOST);
#	else
	char *fromhost = NULL;
#	endif /* USE_INN_NNTPLIB */

	if (!(fromhost && *fromhost))
		fromhost = domain_name;

	if (thisgrp && (thisgrp->attribute->from != (char *) 0)) {
		strcpy(from_name, thisgrp->attribute->from);
		return;
	}

	if (*tinrc.mail_address) {
		strcpy(from_name, tinrc.mail_address);
		return;
	}

	sprintf (from_name, ((strpbrk(get_full_name(), "!()<>@,;:\\\".[]")) ? "\"%s\" <%s@%s>" : "%s <%s@%s>"), get_full_name(), get_user_name(), fromhost);
#	ifdef DEBUG
	if (debug == 2)
		error_message ("FROM=[%s] USER=[%s] HOST=[%s] NAME=[%s]", from_name, get_user_name(), domain_name, get_full_name());
#	endif /* DEBUG */

}
#endif /* !INDEX_DAEMON */


/*
 * build_sender()
 * returns *(Full_Name <user@fq.domainna.me>)
 */
#if !defined(INDEX_DAEMON) && !defined(FORGERY)
char *
build_sender (
	void)
{
	const char *ptr;
	static char sender[8192];

	sender[0] = '\0';

	if ((ptr = get_full_name()))
		sprintf (sender, ((strchr(ptr, '.')) ? "\"%s\" " : "%s "), ptr);

	if ((ptr = get_user_name())) {
		strcat(sender, "<");
		strcat(sender, ptr);
		strcat(sender, "@");

#	ifdef HAVE_GETHOSTBYNAME
		if ((ptr = get_fqdn(get_host_name())))
#	else
		if ((ptr = get_host_name()))
#	endif /* HAVE_GETHOSTBYNAME */
		{
			strcat(sender, ptr);
			strcat(sender, ">");
		} else
			return (char *) 0;
	} else
		return (char *) 0;

	return sender;
}
#endif /* !INDEX_DAEMON && !FORGERY */
