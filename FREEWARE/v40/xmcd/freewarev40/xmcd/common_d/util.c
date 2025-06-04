/*
 *   util.c - Common utility routines for xmcd, cda and libdi.
 *
 *   xmcd  - Motif(tm) CD Audio Player
 *   cda   - Command-line CD Audio Player
 *   libdi - CD Audio Player Device Interface Library
 *
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
static char *_util_c_ident_ = "@(#)util.c	6.52 98/10/27";
#endif

#include "common_d/appenv.h"
#include "common_d/util.h"

#if !defined(USE_SELECT) && !defined(USE_POLL) && \
    !defined(USE_NAP) && !defined(USE_USLEEP)
#define USE_SELECT	/* Make USE_SELECT the default if not specified */
#endif

#ifdef USE_SELECT
#include <sys/time.h>
#ifdef _AIX
#include <sys/select.h>
#endif
#endif

#ifdef __VMS
#include <fscndef.h>
STATIC int		context;
#endif


extern appdata_t	app_data;
extern FILE		*errfp;


STATIC uid_t		ouid = 30001;	/* Default to something safe */
STATIC gid_t		ogid = 30001;	/* Default to something safe */
STATIC struct utsname	un;		/* utsname */

/*
 * Data used by util_text_reduce()
 */
STATIC int		excnt,
			delcnt,
			*exlen,
			*dellen;
STATIC char		**exclude_words;
STATIC char		*delete_words[] = {
	"\\n", "\\r", "\\t"
};


/***********************
 *   public routines   *
 ***********************/


/*
 * util_init
 *	Initialize the libutil module.  This should be called before
 *	the calling program does a setuid.
 *
 * Args:
 *	Nothing
 *
 * Return:
 *	Nothing
 */
void
util_init(void)
{
	/* Save original uid and gid */
	ouid = getuid();
	ogid = getgid();

	/* Set uname */
	if (uname(&un) < 0) {
		DBGPRN(errfp, "uname(2) failed (errno=%d)\n", errno);
	}
}


/*
 * util_start
 *	Start up the libutil module.
 *
 * Args:
 *	Nothing
 *
 * Return:
 *	Nothing
 */
void
util_start(void)
{
	int	n,
		i;
	char	*p,
		*q;
	char	c;

	/* Initializations for util_text_reduce() */
	p = app_data.exclude_words;
	n = 1;
	if (p != NULL) {
		for (; *p != '\0'; p++) {
			if (isspace(*p)) {
				n++;
				while (isspace(*p))
					p++;
			}
		}
	}
	excnt = n;

	exclude_words = (char **) MEM_ALLOC(
		"exclude_words",
		excnt * sizeof(char *)
	);
	if (exclude_words == NULL) {
		fprintf(errfp, "Out of virtual memory.\n");
		exit(1);
	}

	p = app_data.exclude_words;
	if (p == NULL)
		exclude_words[0] = "";
	else {
		n = 0;
		for (q = p; *q != '\0'; q++) {
			if (!isspace(*q))
				continue;
			c = *q;
			*q = '\0';
			exclude_words[n] = (char *) MEM_ALLOC(
				"exclude_words[n]",
				strlen(p) + 1
			);
			if (exclude_words[n] == NULL) {
				fprintf(errfp, "Out of virtual memory.\n");
				exit(1);
			}
			(void) strcpy(exclude_words[n], p);
			n++;
			*q = c;
			p = q + 1;
			while (isspace(*p))
				p++;
			q = p;
		}
		exclude_words[n] = (char *) MEM_ALLOC(
			"exclude_words[n]",
			strlen(p) + 1
		);
		if (exclude_words[n] == NULL) {
			fprintf(errfp, "Out of virtual memory.\n");
			exit(1);
		}
		(void) strcpy(exclude_words[n], p);
	}

	exlen = (int *) MEM_ALLOC("exlen", sizeof(int) * excnt);
	if (exlen == NULL) {
		fprintf(errfp, "Out of virtual memory.\n");
		exit(1);
	}

	for (i = 0; i < excnt; i++)
		exlen[i] = strlen(exclude_words[i]);

	delcnt = sizeof(delete_words) / sizeof(char *);

	dellen = (int *) MEM_ALLOC("dellen", sizeof(int) * delcnt);
	if (dellen == NULL) {
		fprintf(errfp, "Out of virtual memory.\n");
		exit(1);
	}

	for (i = 0; i < delcnt; i++)
		dellen[i] = strlen(delete_words[i]);
}


/*
 * util_get_ouid
 *	Get original user ID
 *
 * Args:
 *	Nothing
 *
 * Return:
 *	Original uid value.
 */
uid_t
util_get_ouid(void)
{
	return (ouid);
}


/*
 * util_get_ogid
 *	Get original group ID
 *
 * Args:
 *	Nothing
 *
 * Return:
 *	Original gid value.
 */
gid_t
util_get_ogid(void)
{
	return (ogid);
}


/*
 * util_get_uname
 *	Get the utsname structure for the running system
 *	(See uname(2)).
 *
 * Args:
 *	Nothing
 *
 * Return:
 *	Pointer to the utsname structure.
 */
struct utsname *
util_get_uname(void)
{
	return (&un);
}


/*
 * util_ltobcd
 *	32-bit integer to BCD conversion routine
 *
 * Args:
 *	n - 32-bit integer
 *
 * Return:
 *	BCD representation of n
 */
sword32_t
util_ltobcd(sword32_t n)
{
	return ((n % 10) | ((n / 10) << 4));
}


/*
 * util_bcdtol
 *	BCD to 32-bit integer conversion routine
 *
 * Args:
 *	n - BCD value
 *
 * Return:
 *	integer representation of n
 */
sword32_t
util_bcdtol(sword32_t n)
{
	return ((n & 0x0f) + ((n >> 4) * 10));
}


/*
 * util_stob
 *	String to boolean conversion routine
 *
 * Args:
 *	s - text string "True", "true", "False" or "false"
 *
 * Return:
 *	Boolean value representing the string
 */
bool_t
util_stob(char *s)
{
	if (strcmp(s, "True") == 0 || strcmp(s, "true") == 0 ||
	    strcmp(s, "TRUE") == 0)
		return TRUE;

	return FALSE;
}


/*
 * util_basename
 *	Return the basename of a file path
 *
 * Args:
 *	path - The file path string
 *
 * Return:
 *	The basename string
 */
char *
util_basename(char *path)
{
	char	*p;

	if (path == NULL)
		return NULL;

#ifndef __VMS
	if ((p = strrchr(path, '/')) == NULL)
#else
	if ((p = strrchr(path, ']')) == NULL)
#endif
		return (path);
	
	return (p + 1);
}


/*
 * util_dirname
 *	Return the dirname of a file path
 *
 * Args:
 *	path - The file path string
 *
 * Return:
 *	The dirname string
 */
char *
util_dirname(char *path)
{
	char		*p;
	static char	buf[FILE_PATH_SZ];

	if (path == NULL)
		return NULL;

	if ((int) strlen(path) >= FILE_PATH_SZ)
		/* Error: path name too long */
		return NULL;

	(void) strcpy(buf, path);

#ifndef __VMS
	if ((p = strrchr(buf, '/')) == NULL)
#else
	if ((p = strrchr(buf, ']')) == NULL)
#endif
		return (buf);

#ifdef __VMS
	p++;
#endif

	*p = '\0';

	return (buf);
}


/*
 * util_loginname
 *	Return the login name of the current user
 *
 * Args:
 *	None.
 *
 * Return:
 *	The login name string.
 */
char *
util_loginname(void)
{
#ifndef __VMS
	struct passwd	*pw;
	char		*cp;

	/* Get login name from the password file if possible */
	setpwent();
	if ((pw = getpwuid(ouid)) != NULL) {
		endpwent();
		return (pw->pw_name);
	}
	endpwent();

	/* Try the LOGNAME environment variable */
	if ((cp = (char *) getenv("LOGNAME")) != NULL)
		return (cp);

	/* Try the USER environment variable */
	if ((cp = (char *) getenv("USER")) != NULL)
		return (cp);
#endif
	/* If we still can't get the login name, just set it
	 * to "nobody" (shrug).
	 */
	return ("nobody");
}


/*
 * util_homedir
 *	Return the home directory path of a user given the uid
 *
 * Args:
 *	uid - The uid of the user
 *
 * Return:
 *	The home directory path name string
 */
char *
util_homedir(uid_t uid)
{
#ifndef __VMS
	struct passwd	*pw;
	char		*cp;

	/* Get home directory from the password file if possible */
	setpwent();
	if ((pw = getpwuid(uid)) != NULL) {
		endpwent();
		return (pw->pw_dir);
	}
	endpwent();

	/* Try the HOME environment variable */
	if (uid == ouid && (cp = (char *) getenv("HOME")) != NULL)
		return (cp);

	/* If we still can't get the home directory, just set it to the
	 * current directory (shrug).
	 */
	return (".");
#else
	char		*cp;
	static char	buf[FILE_PATH_SZ];

	if ((cp = (char *) getenv("HOME")) != NULL &&
	    (int) strlen(cp) < sizeof(buf)) {
		(void) strcpy(buf, cp);
		buf[strlen(buf)-1] = '\0';	/* Drop the "]" */
	}
	else
		(void) strcpy(buf, "SYS$DISK:[");

	return (buf);
#endif	/* __VMS */
}


/*
 * util_uhomedir
 *	Return the home directory path of a user given the name
 *
 * Args:
 *	name - The name of the user
 *
 * Return:
 *	The home directory path name string
 */
char *
util_uhomedir(char *name)
{
#ifndef __VMS
	struct passwd	*pw;

	/* Get home directory from the password file if possible */
	setpwent();
	if ((pw = getpwnam(name)) != NULL) {
		endpwent();
		return (pw->pw_dir);
	}
	endpwent();

	/* If we still can't get the home directory, just set it to the
	 * current directory (shrug).
	 */
	return (".");
#else
	char		*cp;
	static char	buf[FILE_PATH_SZ];

	if ((cp = (char *) getenv("HOME")) != NULL &&
	    (int) strlen(cp) < FILE_PATH_SZ) {
		(void) strcpy(buf, cp);
		buf[strlen(buf)-1] = '\0';	/* Drop the "]" */
	}
	else
		(void) strcpy(buf, "SYS$DISK:[");

	return (buf);
#endif
}


/*
 * util_mkdir
 *	Wrapper for the mkdir() call.
 *
 * Args:
 *	path - The directory path to make
 *	mode - The permissions
 *
 * Return:
 *	TRUE - mkdir succeeded or directory already exists
 *	FALSE - mkdir failed
 */
bool_t
util_mkdir(char *path, mode_t mode)
{
	struct stat	stbuf;

	/*
	 * Make sure directory exists.  If not, create it.
	 */
	if (stat(path, &stbuf) < 0) {
		if (errno == ENOENT) {
			if (mkdir(path, 0777) < 0)
				return FALSE;
		}
		else
			return FALSE;
	}
	else if (!S_ISDIR(stbuf.st_mode))
		return FALSE;

	(void) chmod(path, mode);
	return TRUE;
}


/*
 * util_isexecutable
 *	Verify executability, given a path to a program
 *
 * Args:
 *	path - the absolute path to the program executable
 *
 * Return:
 *	TRUE - It is executable
 *	FALSE - It is not executable
 */
bool_t
util_isexecutable(char *path)
{
#ifdef __VMS
	return TRUE;	/* shrug */
#else
	char		**cp;
	struct group	*gr;
	struct stat	stbuf;

	if (stat(path, &stbuf) < 0 || !S_ISREG(stbuf.st_mode))
		/* Cannot access file or file is not regular */
		return FALSE;

	if (ouid == 0)
		/* Root can execute any file */
		return TRUE;

	if ((stbuf.st_mode & S_IXUSR) == S_IXUSR && ouid == stbuf.st_uid)
		/* The file is executable, and is owned by the user */
		return TRUE;

	if ((stbuf.st_mode & S_IXGRP) == S_IXGRP) {
		if (ogid == stbuf.st_gid)
			/* The file is group executable, and the
			 * user's current gid matches the group.
			 */
			return TRUE;

		setgrent();
		if ((gr = getgrgid(stbuf.st_gid)) != NULL) {
			for (cp = gr->gr_mem; cp != NULL && *cp != '\0'; cp++) {
				/* The file is group executable, and the
				 * user is a member of that group.
				 */
				if (strcmp(*cp, util_loginname()) == 0)
					return TRUE;
			}
		}
		endgrent();
	}

	if ((stbuf.st_mode & S_IXOTH) == S_IXOTH)
		/* The file is executable by everyone */
		return TRUE;

	return FALSE;
#endif	/* __VMS */
}


/*
 * util_checkcmd
 *	Check a command for sanity before running it.
 *
 * Args:
 *	cmd - The command string
 *
 * Return:
 *	TRUE - Command is sane
 *	FALSE - Command is not sane
 */
bool_t
util_checkcmd(char *cmd)
{
#ifdef __VMS
	return TRUE;	/* shrug */
#else
	char	*p,
		*q,
		*r,
		*env,
		*path,
		c,
		c2;
	bool_t	isexe;

	if (cmd == NULL)
		return FALSE;

	/* Cut out just the argv[0] portion */
	c = '\0';
	if ((p = strchr(cmd,' ')) != NULL || (p = strchr(cmd,'\t')) != NULL) {
		c = *p;
		*p = '\0';
	}

	if (cmd[0] == '/') {
		/* Absolute path specified */
		isexe = util_isexecutable(cmd);
	}
	else {
		/* Relative path: walk PATH and look for the executable */
		if ((env = getenv("PATH")) == NULL) {
			if (p != NULL)
				*p = c;
			/* PATH unknown */
			return FALSE;
		}

		isexe = FALSE;

		/* Walk the PATH */
		for (q = env; (r = strchr(q, ':')) != NULL; *r = c2, q = ++r) {
			c2 = *r;
			*r = '\0';

			path = (char *) MEM_ALLOC(
				"checkcmd_path",
				strlen(q) + strlen(cmd) + 2
			);
			if (path == NULL)
				return FALSE;	/* shrug */

			(void) sprintf(path, "%s/%s", q, cmd);

			if (util_isexecutable(path)) {
				isexe = TRUE;
				MEM_FREE(path);
				*r = c2;
				break;
			}

			MEM_FREE(path);
		}
		if (!isexe) {
			/* Check last component in PATH */
			path = (char *) MEM_ALLOC(
				"checkcmd_path",
				strlen(q) + strlen(cmd) + 2
			);
			if (path == NULL)
				return FALSE;	/* shrug */

			(void) sprintf(path, "%s/%s", q, cmd);

			if (util_isexecutable(path))
				isexe = TRUE;

			MEM_FREE(path);
		}
	}

	if (p != NULL)
		*p = c;

	return (isexe);
#endif	/* __VMS */
}


/*
 * util_runcmd
 *	Set uid and gid to the original user and spawn an external command.
 *
 * Args:
 *	cmd - Command string.
 *	workproc - Function to call when waiting for child process,
 *		   or NULL if no workproc.
 *	workarg - Argument to pass to workproc.
 *
 * Return:
 *	The exit status of the command.
 */
int
util_runcmd(char *cmd, void (*workproc)(int), int workarg)
{
	int		ret;
#ifndef __VMS
	pid_t		cpid;
	waitret_t	stat_val;

	if (!util_checkcmd(cmd))
		return EXE_ERR;

	/* Fork child to invoke external command */
	switch (cpid = FORK()) {
	case 0:
		/* Child process */
		break;

	case -1:
		/* Fork failed */
		perror("util_runcmd: fork() failed");
		return EXE_ERR;

	default:
		/* Parent process: wait for child to exit */
		while ((ret = WAITPID(cpid, &stat_val, 0)) != cpid) {
			if (ret < 0) {
				DBGPRN(errfp, "waitpid() failed (errno=%d)\n",
				       errno);
				return 0;
			}

			/* If a workproc is defined, run it */
			if (workproc != NULL)
				(*workproc)(workarg);
		}

		if (WIFEXITED(stat_val))
			ret = WEXITSTATUS(stat_val);
		else
			ret = EXE_ERR;

		DBGPRN(errfp, "\nCommand exit status %d\n", ret);
		return (ret);
	}

	DBGPRN(errfp, "\nSetting uid to %d, gid to %d\n",
		(int) util_get_ouid(), (int) util_get_ogid());

	/* Force uid and gid to original setting */
	if (setuid(util_get_ouid()) < 0 || setgid(util_get_ogid()) < 0)
		exit(errno);
#endif	/* __VMS */

	/* Do the command */
	DBGPRN(errfp, "\nCommand: [%s]\n", cmd);
	ret = system(cmd);

#ifdef __VMS
	DBGPRN(errfp, "\nCommand exit status %d\n", ret);
	return (ret);
#else
	stat_val = (waitret_t) ret;

	if (WIFEXITED(stat_val))
		ret = WEXITSTATUS(stat_val);
	else
		ret = EXE_ERR;

	exit(ret);
	/*NOTREACHED*/
#endif	/* __VMS */
}


/*
 * util_isqrt
 *	Fast integer-based square root routine
 *
 * Args:
 *	n - The integer value whose square-root is to be taken
 *
 * Return:
 *	Resultant square-root integer value
 */
int
util_isqrt(int n)
{
	int	a, b, c, as, bs;

	a = 1;
	b = 1;
	while (a <= n) {
		a = a << 2;
		b = b << 1;
	}
	as = 0;
	bs = 0;
	while (b > 1 && n > 0) {
		a = a >> 2;
		b = b >> 1;
		c = n - (as | a);
		if (c >= 0) {
			n = c;
			as |= (a << 1);
			bs |= b;
		}
		as >>= 1;
	}

	return (bs);
}


/*
 * util_blktomsf
 *	CD logical block to MSF conversion routine
 *
 * Args:
 *	blk - The logical block address
 *	ret_min - Minute (return)
 *	ret_sec - Second (return)
 *	ret_frame - Frame (return)
 *	offset - Additional logical block address offset
 *
 * Return:
 *	Nothing.
 */
void
util_blktomsf(
	word32_t	blk,
	byte_t		*ret_min,
	byte_t		*ret_sec,
	byte_t		*ret_frame,
	word32_t	offset)
{
	*ret_min = (blk + offset) / FRAME_PER_SEC / 60;
	*ret_sec = ((blk + offset) / FRAME_PER_SEC) % 60;
	*ret_frame = (blk + offset) % FRAME_PER_SEC;
}


/*
 * util_msftoblk
 *	CD MSF to logical block conversion routine
 *
 * Args:
 *	min - Minute
 *	sec - Second
 *	frame - Frame
 *	ret_blk - The logical block address (return)
 *	offset - Additional logical block address offset
 *
 * Return:
 *	Nothing.
 */
void
util_msftoblk(
	byte_t		min,
	byte_t		sec,
	byte_t		frame,
	word32_t	*ret_blk,
	word32_t	offset)
{
	*ret_blk = FRAME_PER_SEC * (min * 60 + sec) + frame - offset;
}


/*
 * util_delayms
 *	Suspend execution for the specified number of milliseconds
 *
 * Args:
 *	msec - The number of milliseconds
 *
 * Return:
 *	Nothing.
 */
void
util_delayms(unsigned long msec)
{
#ifdef USE_SELECT
	struct timeval	to;

	to.tv_sec = (long) msec / 1000;
	to.tv_usec = ((long) msec % 1000) * 1000;

	(void) select(0, NULL, NULL, NULL, &to);
#else
#ifdef USE_POLL
	(void) poll(NULL, 0, (int) msec);
#else
#ifdef USE_NAP
	(void) nap((long) msec);
#else
#ifdef USE_USLEEP
	(void) usleep((long) msec * 1000);
#else
	/* shrug: Rounded to the nearest second, with a minimum of 1 second */
	if (msec < 1000)
		(void) sleep(1);
	else
		(void) sleep(((unsigned int) msec + 500) / 1000);
#endif	/* USE_USLEEP */
#endif	/* USE_NAP */
#endif	/* USE_POLL */
#endif	/* USE_SELECT */
}


/*
 * util_strcasecmp
 *	Compare two strings a la strcmp(), except it is case-insensitive.
 *
 * Args:
 *	s1 - The first text string.
 *	s2 - The second text string.
 *
 * Return:
 *	Compare value.  See strcmp(3).
 */
int
util_strcasecmp(char *s1, char *s2)
{
	char	*buf1,
		*buf2,
		*p;
	int	ret;

	if (s1 == NULL || s2 == NULL)
		return 0;

	/* Allocate tmp buffers */
	buf1 = (char *) MEM_ALLOC("strcasecmp_buf1", strlen(s1)+1);
	buf2 = (char *) MEM_ALLOC("strcasecmp_buf2", strlen(s2)+1);
	if (buf1 == NULL || buf2 == NULL) {
		fprintf(errfp, "Error: %s\n", app_data.str_nomemory);
		exit(1);
	}

	/* Convert both strings to lower case and store in tmp buffer */
	for (p = buf1; *s1 != '\0'; s1++, p++)
		*p = (char) ((isupper(*s1)) ? tolower(*s1) : *s1);
	*p = '\0';
	for (p = buf2; *s2 != '\0'; s2++, p++)
		*p = (char) ((isupper(*s2)) ? tolower(*s2) : *s2);
	*p = '\0';

	ret = strcmp(buf1, buf2);

	MEM_FREE(buf1);
	MEM_FREE(buf2);

	return (ret);
}


/*
 * util_strcasecmp
 *	Compare two strings a la strncmp(), except it is case-insensitive.
 *
 * Args:
 *	s1 - The first text string.
 *	s2 - The second text string.
 *	n - number of characters to compare.
 *
 * Return:
 *	Compare value.  See strncmp(3).
 */
int
util_strncasecmp(char *s1, char *s2, int n)
{
	char	*buf1,
		*buf2,
		*p;
	int	ret;

	if (s1 == NULL || s2 == NULL)
		return 0;

	/* Allocate tmp buffers */
	buf1 = (char *) MEM_ALLOC("strncasecmp_buf1", strlen(s1)+1);
	buf2 = (char *) MEM_ALLOC("strncasecmp_buf2", strlen(s2)+1);
	if (buf1 == NULL || buf2 == NULL) {
		fprintf(errfp, "Error: %s\n", app_data.str_nomemory);
		exit(1);
	}

	/* Convert both strings to lower case and store in tmp buffer */
	for (p = buf1; *s1 != '\0'; s1++, p++)
		*p = (char) ((isupper(*s1)) ? tolower(*s1) : *s1);
	*p = '\0';
	for (p = buf2; *s2 != '\0'; s2++, p++)
		*p = (char) ((isupper(*s2)) ? tolower(*s2) : *s2);
	*p = '\0';

	ret = strncmp(buf1, buf2, n);

	MEM_FREE(buf1);
	MEM_FREE(buf2);

	return (ret);
}


/*
 * Data used by b64encode
 */
STATIC char	b64map[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
	'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '+', '/'
};

#define B64_PAD		'='


/*
 * util_b64encode
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
util_b64encode(byte_t *ibuf, int len, byte_t *obuf, bool_t brklines)
{
	int	i, j, k, n,
		c[4];
	byte_t	sbuf[4];

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
 * util_text_reduce
 *	Reduce a text string to become suitable for use in a keyword search
 *	operation.
 *
 * Args:
 *	str - The input text string
 *
 * Return:
 *	The output text string.  The string buffer is allocated internally
 *	and should be freed by the caller via MEM_FREE().  If an error
 *	occurs, NULL is returned.
 */
char *
util_text_reduce(char *str)
{
	int		i,
			lastex,
			*len;
	char		last,
			next,
			*p1,
			*p2,
			*pr,
			**t,
			*newstr;

	if ((newstr = (char *) MEM_ALLOC("text_reduce_newstr",
					 strlen(str) + 1)) == NULL)
		return NULL;

	p1 = str;
	p2 = newstr;
	pr = newstr;

	last = ' ';
	lastex = -1;

	while (*p1 != '\0') {
		next = *p1;

		for (i = 0, t = delete_words, len = dellen; i < delcnt;
		     i++, t++, len++) {
			if (strncmp(p1, *t, *len) == 0) {
				p1 += *len - 1;
				next = ' ';
				break;
			}
		}

		if (!isalnum(last) && !isalnum(next)) {
			p1++;
			continue;
		}

		for (i = 0, t = exclude_words, len = exlen; i < excnt;
		    i++, t++, len++) {
			if (lastex != i && !isalnum(last) &&
			    util_strncasecmp(p1, *t, *len) == 0 &&
			    !isalnum(p1[*len])) {
				p1 += *len;
				lastex = i;
				break;
			}
		}

		if (i < excnt)
			continue;

		if (isalnum(next))
			*p2 = next;
		else
			*p2 = ' ';

		last = next;
		p2++;
		p1++;

		if (isalnum(next)) {
			lastex = -1;
			pr = p2;
		}
	}

	*pr = '\0';
	return (newstr);
}


/*
 * util_cgi_xlate
 *	Translate a keyword string into CGI form.  It substitutes whitespaces
 *	with the proper separator, conversion to lower case, handles non-
 *	alphanumerdic character translations, etc.
 *
 * Args:
 *	str - The input keywords string, separated by whitespace
 *
 * Return:
 *	The output text string.  The string buffer is allocated internally
 *	and should be freed by the caller via MEM_FREE().  If an error
 *	occurs, NULL is returned.
 */
char *
util_cgi_xlate(char *str)
{
	char	*p,
		*q,
		*new_str;

	if ((q = new_str = (char *) MEM_ALLOC("cgi_xlate_newstr",
					      (strlen(str) * 3) + 5)) == NULL)
		return NULL;

	/* Skip leading whitespaces */
	p = str;
	while (isspace(*p))
		p++;

	/* Process the string */
	while (*p != '\0') {
		if (isspace(*p)) {
			/* Skip remaining consecutive white spaces */
			while (isspace(*(++p)))
				;
			if (*p == '\0')
				break;	/* End of string reached */
			else {
				/* Substitute white spaces with separator */
				*q = '+';
				q++;
			}
		}
		else if ((ispunct(*p) &&
			  *p != '_' && *p != '.' &&
			  *p != '*' && *p != '@') ||
			 (*p & 0x80)) {
			/* Need URL-encoding */
			(void) sprintf(q, "%%%02X", (int) (*p));
			q += 3;
			p++;
		}
		else if (isprint(*p)) {
			/* Printable character */
			*q = (char) (isupper(*p) ? tolower(*p) : (*p));
			q++;
			p++;
		}
		else
			p++;
	}
	*q = '\0';

	return (new_str);
}


/*
 * util_bswap16
 *	16-bit little-endian to big-endian byte-swap routine.
 *	On a big-endian system architecture this routines has no effect.
 *
 * Args:
 *	x - The data to be swapped
 *
 * Return:
 *	The swapped data.
 */
word16_t
util_bswap16(word16_t x)
{
#if _BYTE_ORDER_ == _L_ENDIAN_
	word16_t	ret;

	ret  = (x & 0x00ff) << 8;
	ret |= (word16_t) (x & 0xff00) >> 8;
	return (ret);
#else
	return (x);
#endif
}


/*
 * util_bswap24
 *	24-bit little-endian to big-endian byte-swap routine.
 *	On a big-endian system architecture this routines has no effect.
 *
 * Args:
 *	x - The data to be swapped
 *
 * Return:
 *	The swapped data.
 */
word32_t
util_bswap24(word32_t x)
{
#if _BYTE_ORDER_ == _L_ENDIAN_
	word32_t	ret;

	ret  = (x & 0x0000ff) << 16;
	ret |= (x & 0x00ff00);
	ret |= (x & 0xff0000) >> 16;
	return (ret);
#else
	return (x);
#endif
}


/*
 * util_bswap32
 *	32-bit little-endian to big-endian byte-swap routine.
 *	On a big-endian system architecture this routines has no effect.
 *
 * Args:
 *	x - The data to be swapped
 *
 * Return:
 *	The swapped data.
 */
word32_t
util_bswap32(word32_t x)
{
#if _BYTE_ORDER_ == _L_ENDIAN_
	word32_t	ret;

	ret  = (x & 0x000000ff) << 24;
	ret |= (x & 0x0000ff00) << 8;
	ret |= (x & 0x00ff0000) >> 8;
	ret |= (x & 0xff000000) >> 24;
	return (ret);
#else
	return (x);
#endif
}


/*
 * util_lswap16
 *	16-bit big-endian to little-endian byte-swap routine.
 *	On a little-endian system architecture this routines has no effect.
 *
 * Args:
 *	x - The data to be swapped
 *
 * Return:
 *	The swapped data.
 */
word16_t
util_lswap16(word16_t x)
{
#if _BYTE_ORDER_ == _L_ENDIAN_
	return (x);
#else
	word16_t	ret;

	ret  = (x & 0x00ff) << 8;
	ret |= (word16_t) (x & 0xff00) >> 8;
	return (ret);
#endif
}


/*
 * util_lswap24
 *	24-bit big-endian to little-endian byte-swap routine.
 *	On a little-endian system architecture this routines has no effect.
 *
 * Args:
 *	x - The data to be swapped
 *
 * Return:
 *	The swapped data.
 */
word32_t
util_lswap24(word32_t x)
{
#if _BYTE_ORDER_ == _L_ENDIAN_
	return (x);
#else
	word32_t	ret;

	ret  = (x & 0x0000ff) << 16;
	ret |= (x & 0x00ff00);
	ret |= (x & 0xff0000) >> 16;
	return (ret);
#endif
}


/*
 * util_lswap32
 *	32-bit big-endian to little-endian byte-swap routine.
 *	On a little-endian system architecture this routines has no effect.
 *
 * Args:
 *	x - The data to be swapped
 *
 * Return:
 *	The swapped data.
 */
word32_t
util_lswap32(word32_t x)
{
#if _BYTE_ORDER_ == _L_ENDIAN_
	return (x);
#else
	word32_t	ret;

	ret  = (x & 0x000000ff) << 24;
	ret |= (x & 0x0000ff00) << 8;
	ret |= (x & 0x00ff0000) >> 8;
	ret |= (x & 0xff000000) >> 24;
	return (ret);
#endif
}


/*
 * util_dbgdump
 *	Dump a data buffer to screen.
 *
 * Args:
 *	title - Message banner
 *	data - Address of data
 *	len - Number of bytes to dump
 *
 * Return:
 *	Nothing.
 */
void
util_dbgdump(char *title, byte_t *data, int len)
{
	int	i, j, k, n,
		lines;

	if (title == NULL || data == NULL || len <= 0)
		return;

	(void) fprintf(errfp, "\n%s:", title);

	lines = ((len - 1) / 16) + 1;

	for (i = 0, k = 0; i < lines; i++) {
		(void) fprintf(errfp, "\n%04x    ", k);

		for (j = 0, n = k; j < 16; j++, k++) {
			if (k < len)
				(void) fprintf(errfp, "%02x ", *(data + k));
			else
				(void) fprintf(errfp, "-- ");

			if (j == 7)
				(void) fprintf(errfp, " ");
		}

		(void) fprintf(errfp, "   ");

		for (j = 0, k = n; j < 16; j++, k++) {
			if (k < len) {
				(void) fprintf(errfp, "%c",
				    isprint(*(data + k)) ? *(data + k) : '.'
				);
			}
			else
				(void) fprintf(errfp, ".");
		}
	}

	(void) fprintf(errfp, "\n");
}


#ifdef __VMS
/*
 * The following section provide UNIX-like functionality for Digital OpenVMS
 */

/* Function prototypes */
extern void	delete();

#ifdef VMS_USE_OWN_DIRENT

extern int	LIB$FIND_FILE();
extern void	LIB$FIND_FILE_END();
extern void	SYS$FILESCAN();

typedef struct {
	short	length;
	short	component;
	int	address;
	int	term;
} item_list;


/*
 * util_opendir
 *	Emulate a UNIX opendir by clearing the context value, and creating
 *	the wild card search by appending *.* to the path name.
 *	(See opendir(2) on UNIX systems)
 *
 * Args:
 *	path - directory path to open
 *
 * Return:
 *	Pointer to the DIR structure descriptor
 */
DIR *
util_opendir(char *path)
{
	static DIR		dir;
	static struct dirent	ent;

	context = 0;
	(void) sprintf(ent.d_name, "%s*.*", path);
	dir.dd_buf = &ent;
 	return (&dir);
}


/*
 * util_closedir
 *	Emulate a UNIX closedir by call LIB$FIND_FILE_END to close 
 *	the file context.  (End the wild card search)
 *	(See closedir(2) on UNIX systems)
 *
 * Args:
 *	dp - pointer to the directory's DIR structure
 *
 * Return:
 *	Nothing.
 */
void
util_closedir(DIR *dp)
{
	LIB$FIND_FILE_END(&context);
}


/*
 * util_readdir
 *	Emulate a UNIX readdir by calling LIB$FIND_FILE, and SYS$FILESCAN
 *	to return the file name back.
 *	(See readdir(2) on UNIX systems)
 *
 * Args:
 *	dp - pointer to the directory's DIR structure
 *
 * Return:
 *	Pointer to the dirent structure pertaining to a directory entry
 */
struct dirent *
util_readdir(DIR *dp)
{
	int			dir_desc[2],
				desc[2],
				i;
	char 			*p,
				*file[FILE_PATH_SZ];
	item_list		list;
	static struct dirent	ent;

	desc[0] = FILE_PATH_SZ;
	desc[1] = (int) file;

	dir_desc[0] = FILE_PATH_SZ;
	dir_desc[1] = (int) dp->dd_buf->d_name;

	if (LIB$FIND_FILE(dir_desc, desc, &context) & 0x01) {
		list.length = 0;
		list.component = FSCN$_NAME;
		list.address = 0;
		list.term = 0;

		SYS$FILESCAN(desc, &list, 0, 0, 0); 

		p = (char *) list.address;
		p[list.length] = '\0';

		for (p = (char *) list.address; *p != '\0'; p++)
			*p = tolower(*p);

		(void) strcpy(ent.d_name, (char *) list.address);
		return (&ent);
	}
	else
		return NULL;
}

#endif	/* VMS_USE_OWN_DIRENT */


/*
 * util_waitpid
 *	Emulate a UNIX waitpid by doing a wait call
 *	(see waitpid(2) on UNIX systems)
 *
 * Args:
 *	pid - process ID to wait for
 *	statloc - pointer to wait status information
 *	options - wait options
 * Return:
 *	The process ID of the process that caused this call to stop
 *	waiting.
 */
pid_t
util_waitpid(pid_t pid, int *statloc, int options)
{
	pid_t	ret;

	ret = wait(statloc);

	/* Under VMS a vfork() call does not create a child process unless
	 * a real process is created.  In the cases where the child does
	 * not follow the vfork with a system() or exec() call to create
	 * a real subprocess, we need to fake things out.
	 */
	if (ret < 0)
		ret = pid;

	/* VMS returns a 1 for success.  Patch it to zero to
	 * make this function compatible with UNIX.
	 */
	if (*statloc == 1)
		*statloc = 0;

	return (ret);
}


/*
 * util_unlink
 *	Emulate a UNIX unlink call
 *	(See unlink(2) on UNIX systems)
 *
 * Args:
 *	file - file path name to unlink
 *
 * Return:
 *	0  - Success
 *	-1 - Failure
 */
int
util_unlink(char *file)
{
	delete(file);
	return 0;
}


/*
 * util_link
 *	Emulate a UNIX link call by copying FILE1 to FILE2
 *	(See link(2) on UNIX systems)
 *
 * Args:
 *	file1 - source file
 *	file2 - destination file
 *
 * Return:
 *	0  - Success
 *	-1 - Failure
 */
int 
util_link(char *file1, char *file2)
{
	FILE	*fp1,
		*fp2;
	char	buf[STR_BUF_SZ * 2];

	fp1 = fopen(file1, "r");
	fp2 = fopen(file2, "w");

	if (fp1 == NULL || fp2 == NULL)
		return -1;

	while (fgets(buf, sizeof(buf), fp1) != NULL)
		(void) fprintf(fp2, "%s", buf);

	(void) fclose(fp1);	
	(void) fclose(fp2);	

	return 0;	
}

#endif	/* __VMS */

#ifdef MEM_DEBUG
/*
 * For memory allocation debugging
 */


/*
 * util_dbg_malloc
 *	Wrapper for malloc(3).
 */
void *
util_dbg_malloc(char *name, size_t size)
{
	void	*ptr;

	ptr = _MEM_ALLOC(size);
	(void) fprintf(stderr, "Malloc(%s, %d) => 0x%x\n", name, size, ptr);
	return (ptr);
}


/*
 * util_dbg_realloc
 *	Wrapper for realloc(3).
 */
void *
util_dbg_realloc(char *name, void *ptr, size_t size)
{
	void	*nptr;

	nptr = _MEM_REALLOC(ptr, size);
	(void) fprintf(stderr, "Realloc(%s, 0x%x, %d) => 0x%x\n",
			name, ptr, size, nptr);
	return (nptr);
}


/*
 * util_dbg_calloc
 *	Wrapper for calloc(3).
 */
void *
util_dbg_calloc(char *name, size_t nelem, size_t elsize)
{
	void	*ptr;

	ptr = _MEM_CALLOC(nelem, elsize);
	(void) fprintf(stderr, "Calloc(%s, %d, %d) => 0x%x\n",
			name, nelem, elsize, ptr);
	return (ptr);
}


/*
 * util_dbg_free
 *	Wrapper for free(3).
 */
void
util_dbg_free(void *ptr)
{
	(void) fprintf(stderr, "Free(0x%x)\n", ptr);
	_MEM_FREE(ptr);
}

#endif	/* MEM_DEBUG */

