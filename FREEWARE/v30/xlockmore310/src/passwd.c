

#ifndef lint
static char sccsid[] = "@(#)passwd.c	3.10 96/07/20 xlockmore";

#endif

/*-
 * passwd.c - passwd stuff.
 *
 * Copyright (c) 1988-91 by Patrick J. Naughton.
 *
 * Revision History:
 *
 * Changes of David Bagley <bagleyd@hertz.njit.edu>
 * 25-May-96: When xlock is compiled with shadow passwords it will still
 *            work on non shadowed systems.  Marek Michalkiewicz
 *            <marekm@i17linuxb.ists.pwr.wroc.pl>
 * 25-Feb-96: Lewis Muhlenkamp
 *            Added in ability for any of the root accounts to unlock
 *            screen.  Message now gets sent to syslog if root does the
 *            unlocking.
 * 23-Dec-95: Ron Hitchens <ron@utw.com> reorganized.
 * 10-Dec-95: More context handling stuff for DCE thanks to
 *            Terje Marthinussen <terjem@cc.uit.no>.
 * 01-Sep-95: DCE code added thanks to Heath A. Kehoe
 *            <hakehoe@icaen.uiowa.edu>.
 * 24-Jun-95: Extracted from xlock.c, encrypted passwords are now fetched
 *            on start-up to ensure correct operation (except Ultrix).
 */

#include "xlock.h"

extern Bool allowroot;
extern Bool inroot;
extern Bool inwindow;
extern Bool grabmouse;
extern Bool nolock;

#ifdef VMS
#define ROOT "SYSTEM"
#else
#define ROOT "root"
#endif
#if !defined( VMS ) && !defined( SUNOS_ADJUNCT_PASSWD )
#include <pwd.h>
#endif

#if defined( __FreeBSD__ ) || defined( __NetBSD__ ) || defined( __hpux ) || defined( SUNOS_ADJUNCT_PASSWD )
#include <sys/types.h>
#endif

#ifdef MULTIPLE_ROOT
#include <fcntl.h>
#include <errno.h>
#ifdef SYSLOG
#include <syslog.h>
#endif
extern int  errno;

void        get_multiple(struct passwd *);
void        set_multiple();

#define BUFMAX 1024		/* Maximum size of pipe buffer */

/* Linked list to keep track of everyone that's authorized * to unlock the
   screen. */
struct pwln {
	char       *pw_name;
	char       *pw_passwd;
	struct pwln *next;
};
typedef struct pwln pwlnode;
typedef struct pwln *pwlptr;

pwlptr      pwll, pwllh = (pwlptr) NULL;
extern pwlptr pwllh;

/* Function that creates and initializes a new node that * will be added to
   the linked list. */
pwlptr
new_pwlnode(void)
{
	pwlptr      pwl;

	if ((pwl = (pwlptr) malloc(sizeof (pwlnode))) == 0)
		return ((pwlptr) ENOMEM);

	pwl->pw_name = (char *) NULL;
	pwl->pw_passwd = (char *) NULL;
	pwl->next = (pwlptr) NULL;

	return (pwl);
}
#endif

#ifdef ultrix
#include <auth.h>
#endif

#ifdef OSF1_ENH_SEC
#include <sys/types.h>
#include <sys/security.h>
#include <prot.h>
#endif

#if defined( __linux__ ) && defined( __ELF__ ) && !defined( HAS_SHADOW )
/* Linux may or may not have shadow passwords, so it is best to make the same
   binary work with both shadow and non-shadow passwords. It's easy with the
   ELF libc since it has getspnam() and there is no need for any additional
   libraries like libshadow.a.  This is a quick hack; it probably should be
   done properly in the Imakefile.  */
#define HAS_SHADOW
#endif

#if defined( __linux__ ) && defined( HAS_SHADOW ) && defined( HAS_PW_ENCRYPT )
/* Deprecated - long passwords have known weaknesses.  Also, pw_encrypt is
   non-standard (requires libshadow.a) while everything else you need to
   support shadow passwords is in the standard (ELF) libc.  */
#define crypt pw_encrypt
#endif

#ifdef HAS_SHADOW
#ifndef __hpux
#include <shadow.h>
#endif
#endif

#ifdef SUNOS_ADJUNCT_PASSWD
#include <sys/label.h>
#include <sys/audit.h>
#include <pwdadj.h>
#define passwd passwd_adjunct
#define pw_passwd pwa_passwd
#define getpwnam(_s) getpwanam(_s)
#define pw_name pwa_name
#define getpwuid(_s) getpwanam(cuserid(NULL))
#endif /* SUNOS_ADJUNCT_PASSWD */

/* System V Release 4 redefinitions of BSD functions and structures */
#if !defined( SHADOW ) && (defined( SYSV ) || defined( SVR4 ))

#ifdef LESS_THAN_AIX3_2
struct passwd {
	char       *pw_name;
	char       *pw_passwd;
	uid_t       pw_uid;
	gid_t       pw_gid;
	char       *pw_gecos;
	char       *pw_dir;
	char       *pw_shell;
};

#endif /* LESS_THAN_AIX3_2 */

#ifdef HPUX_SECURE_PASSWD
#define seteuid(_eu) setresuid(-1, _eu, -1)
#define passwd s_passwd
#define getpwnam(_s) getspwnam(_s)
#define getpwuid(_u) getspwuid(_u)
#endif /* HPUX_SECURE_PASSWD */

#endif /* defined( SYSV ) || defined( SVR4 ) */

#ifdef VMS
#include <uaidef.h>
#define VMS_PASSLENGTH 9
static short uai_salt, root_salt;
static char hash_password[VMS_PASSLENGTH], hash_system[VMS_PASSLENGTH];
static char root_password[VMS_PASSLENGTH], root_system[VMS_PASSLENGTH];
static char uai_encrypt, root_encrypt;

struct ascid {
	short       len;
	char        dtype;
	char        class;
	char       *addr;
};

static struct ascid username, rootuser;

struct itmlst {
	short       buflen;
	short       code;
	long        addr;
	long        retadr;
};

#endif /* VMS */

#ifdef HP_PASSWDETC
#include <sys/wait.h>
#endif /* HP_PASSWDETC */

#ifdef AFS
#include <afs/kauth.h>
#include <afs/kautils.h>
#endif /* AFS */

char        user[PASSLENGTH];

#ifndef ultrix
static char userpass[PASSLENGTH];
static char rootpass[PASSLENGTH];

#ifdef VMS
static char root[] = ROOT;

#endif
#endif /* !ultrix */

#ifdef DCE_PASSWD
static int  usernet, rootnet;
static int  check_dce_net_passwd(char *, char *);

#endif

#ifdef KERBEROS
#include <krb.h>
#include <sys/types.h>
#include <sys/param.h>
static int
krb_check_password(struct passwd *, char *)
#endif

	extern char *crypt(const char *, const char *);

	static void
	            getUserName(void)
{

#ifdef VMS
	(void) strcpy(user, cuserid(NULL));
#else /* !VMS */
#ifdef HP_PASSWDETC

/* 
 * The PasswdEtc libraries have replacement passwd functions that make
 * queries to DomainOS registries.  Unfortunately, these functions do
 * wierd things to the process (at minimum, signal handlers get changed,
 * there are probably other things as well) that cause xlock to become
 * unstable.
 *
 * As a (really, really sick) workaround, we'll fork() and do the getpw*()
 * calls in the child, and pass the information back through a pipe.
 */
	struct passwd *pw;
	int         pipefd[2], n, total = 0, stat_loc;
	pid_t       pid;

	pipe(pipefd);

	if ((pid = fork()) == 0) {
		close(pipefd[0]);
		pw = getpwuid(getuid());
		write(pipefd[1], pw->pw_name, strlen(pw->pw_name));
		close(pipefd[1]);
		_exit(0);
	}
	if (pid < 0)
		error("%s: could not get user password (fork failed)\n");
	close(pipefd[1]);

	while ((n = read(pipefd[0], &(user[total]), 50)) > 0)
		total += n;

	wait(&stat_loc);

	if (n < 0)
		error("%s: could not get user name (read failed)\n");
	user[total] = 0;

	if (total < 1)
		error("%s: could not get user name (lookups failed)\n");

#else /* !HP_PASSWDETC */
#ifdef OSF1_ENH_SEC
	struct pr_passwd *pw;

	/*if ((pw = getprpwuid(getuid())) == NULL) */
	if ((pw = getprpwuid(starting_ruid())) == NULL)
		error("%s: could not get user name.\n");
	(void) strcpy(user, pw->ufld.fd_name);

#else /* !OSF1_ENH_SEC */

	struct passwd *pw;

	if ((pw = (struct passwd *) getpwuid(getuid())) == NULL)
		error("%s: could not get user name.\n");
	(void) strcpy(user, pw->pw_name);

#if MULTIPLE_ROOT
	get_multiple(pw);
#endif

#endif /* !OSF1_ENH_SEC */
#endif /* !HP_PASSWDETC */
#endif /* !VMS */
}

#ifdef HAS_SHADOW
static int
passwd_invalid(char *passwd)
{
	int         i = strlen(passwd);

	return (i == 1 || i == 2);
}
#endif

#if !defined( ultrix ) && !defined( DCE_PASSWD )
/* This routine is not needed if MULTIPE_ROOT */
static void
getCryptedUserPasswd(void)
{

#ifdef VMS
	struct itmlst il[4];

	il[0].buflen = 2;
	il[0].code = UAI$_SALT;
	il[0].addr = &uai_salt;
	il[0].retadr = 0;
	il[1].buflen = 8;
	il[1].code = UAI$_PWD;
	il[1].addr = &hash_password;
	il[1].retadr = 0;
	il[2].buflen = 1;
	il[2].code = UAI$_ENCRYPT;
	il[2].addr = &uai_encrypt;
	il[2].retadr = 0;
	il[3].buflen = 0;
	il[3].code = 0;
	il[3].addr = 0;
	il[3].retadr = 0;

	username.len = strlen(user);
	username.dtype = 0;
	username.class = 0;
	username.addr = user;

	sys$getuai(0, 0, &username, &il, 0, 0, 0);
#else /* !VMS */
#ifdef HP_PASSWDETC

/* 
 * still very sick, see above
 */
	struct passwd *pw;
	int         pipefd[2], n, total = 0, stat_loc;
	pid_t       pid;

	pipe(pipefd);

	if ((pid = fork()) == 0) {
		close(pipefd[0]);
		pw = getpwuid(getuid());
		write(pipefd[1], pw->pw_passwd, strlen(pw->pw_passwd));
		close(pipefd[1]);
		_exit(0);
	}
	if (pid < 0)
		error("%s: could not get user password (fork failed)\n");
	close(pipefd[1]);

	while ((n = read(pipefd[0], &(userpass[total]), 50)) > 0)
		total += n;

	wait(&stat_loc);

	if (n < 0)
		error("%s: could not get user password (read failed)\n");
	user[total] = 0;

	if (total < 1)
		error("%s: could not get user password (lookups failed)\n");

#else /* !HP_PASSWDETC */
#ifdef OSF1_ENH_SEC
	struct pr_passwd *pw;

	/*if ((pw = getprpwuid(getuid())) == NULL) */
	if ((pw = getprpwuid(starting_ruid())) == NULL)
		error("%s: could not get encrypted user password.\n");
	(void) strcpy(userpass, pw->ufld.fd_encrypt);

#else /* !OSF1_ENH_SEC */

	struct passwd *pw;

#ifdef HAS_SHADOW
	struct spwd *spw;

#endif
	if ((pw = (struct passwd *) getpwuid(getuid())) == NULL)
		error("%s: could not get encrypted user password.\n");
#ifdef HAS_SHADOW
	if ((spw = getspnam(pw->pw_name)) != NULL)
		pw->pw_passwd = spw->sp_pwdp;
	endspent();
	/* Check if there is any chance of unlocking the display later...  */
	/* Program probably needs to be setuid to root.  */
	/* If this is not possible, compile with -DUSE_XLOCKRC */
	if (passwd_invalid(pw->pw_passwd))
		error("%s: it looks like you have shadow passwording.\nContact your administrator.\n");
#endif
	(void) strcpy(userpass, pw->pw_passwd);

#endif /* !OSF1_ENH_SEC */
#endif /* !HP_PASSWDETC */
#endif /* !VMS */
}

static void
getCryptedRootPasswd(void)
{

#ifdef VMS
	struct itmlst il[4];

	il[0].buflen = 2;
	il[0].code = UAI$_SALT;
	il[0].addr = &root_salt;
	il[0].retadr = 0;
	il[1].buflen = 8;
	il[1].code = UAI$_PWD;
	il[1].addr = &root_password;
	il[1].retadr = 0;
	il[2].buflen = 1;
	il[2].code = UAI$_ENCRYPT;
	il[2].addr = &root_encrypt;
	il[2].retadr = 0;
	il[3].buflen = 0;
	il[3].code = 0;
	il[3].addr = 0;
	il[3].retadr = 0;

	rootuser.len = strlen(root);
	rootuser.dtype = 0;
	rootuser.class = 0;
	rootuser.addr = root;

	sys$getuai(0, 0, &rootuser, &il, 0, 0, 0);
#else /* !VMS */
#ifdef HP_PASSWDETC

/* 
 * Still really, really sick.  See above.
 */
	struct passwd *pw;
	int         pipefd[2], n, total = 0, stat_loc;
	pid_t       pid;

	pipe(pipefd);

	if ((pid = fork()) == 0) {
		close(pipefd[0]);
		pw = getpwnam(ROOT);
		write(pipefd[1], pw->pw_passwd, strlen(pw->pw_passwd));
		close(pipefd[1]);
		_exit(0);
	}
	if (pid < 0)
		error("%s: could not get root password (fork failed)\n");
	close(pipefd[1]);

	while ((n = read(pipefd[0], &(rootpass[total]), 50)) > 0)
		total += n;

	wait(&stat_loc);

	if (n < 0)
		error("%s: could not get root password (read failed)\n");
	rootpass[total] = 0;

	if (total < 1)
		error("%s: could not get root password (lookups failed)\n");

#else /* !HP_PASSWDETC */
#ifdef OSF1_ENH_SEC
	struct pr_passwd *pw;

	if ((pw = getprpwnam(ROOT)) == NULL)
		error("%s: could not get encrypted root password.\n");
	(void) strcpy(rootpass, pw->ufld.fd_encrypt);

#else /* !OSF1_ENH_SEC */
	struct passwd *pw;

#ifdef HAS_SHADOW
	struct spwd *spw;

#endif

	if ((pw = (struct passwd *) getpwuid(0)) == NULL)
		error("%s: could not get encrypted root password.\n");
#ifdef HAS_SHADOW
	spw = getspnam(pw->pw_name);
	if (spw)
		pw->pw_passwd = spw->sp_pwdp;
	endspent();
#endif
	(void) strcpy(rootpass, pw->pw_passwd);

#ifdef MULTIPLE_ROOT
	set_multiple();
#endif /* MULTIPLE_ROOT */

#endif /* !OSF1_ENH_SEC */
#endif /* !HP_PASSWDETC */
#endif /* !VMS */
}

#endif /* !ultrix && !DCE_PASSWD */


/* 
 * we don't allow for root to have no password, but we handle the case
 * where the user has no password correctly; they have to hit return
 * only
 */
int
checkPasswd(char *buffer)
{
	int         done;

#ifdef VMS
	struct ascid password;

	password.len = strlen(buffer);
	password.dtype = 0;
	password.class = 0;
	password.addr = buffer;

	str$upcase(&password, &password);

	sys$hash_password(&password, uai_encrypt, uai_salt,
			  &username, &hash_system);

	hash_password[VMS_PASSLENGTH - 1] = 0;
	hash_system[VMS_PASSLENGTH - 1] = 0;

	done = !strcmp(hash_password, hash_system);
	if (!done && allowroot) {
		sys$hash_password(&password, root_encrypt, root_salt,
				  &rootuser, &root_system);
		root_password[VMS_PASSLENGTH - 1] = 0;
		root_system[VMS_PASSLENGTH - 1] = 0;
		done = !strcmp(root_password, root_system);
	}
#else /* !VMS */

#ifdef DCE_PASSWD
	if (usernet)
		done = check_dce_net_passwd(user, buffer);
	else
		done = !strcmp(userpass, crypt(buffer, userpass));

	if (done)
		return 1;
	if (!allowroot)
		return 0;

	if (rootnet)
		done = check_dce_net_passwd(ROOT, buffer);
	else
		done = !strcmp(rootpass, crypt(buffer, rootpass));
#else /* !DCE_PASSWD */

#ifdef ultrix
	done = ((authenticate_user((struct passwd *) getpwnam(user),
				   buffer, NULL) >= 0) || (allowroot &&
			 (authenticate_user((struct passwd *) getpwnam(ROOT),
					    buffer, NULL) >= 0)));
#else /* !ultrix */

#ifdef AFS
	char       *reason;

	/* check afs passwd first, then local, then root */
	done = !ka_UserAuthenticate(user, "", 0, buffer, 0, &reason);
	if (!done)
#endif /* !AFS */
#ifdef KERBEROS
		if (!strcmp(userpass, "*"))
			done = (krb_check_password((struct passwd *) getpwuid(getuid()), buffer) ||
				(allowroot && !strcmp((char *) crypt(buffer, rootpass), rootpass)));
		else
#endif /* !KERBEROS */
#ifdef MULTIPLE_ROOT
			/* Scan through the linked list until you match a password.  Print
			 * message to log if password match doesn't equal the user.
			 *
			 * This should be changed to allow the user name to be typed in also
			 * to make this more secure.
			 */
			done = 0;
	for (pwll = pwllh; pwll->next; pwll = pwll->next)
		if (!strcmp((char *) crypt(buffer, pwll->pw_passwd), pwll->pw_passwd)) {
#ifdef SYSLOG
			if (strcmp(user, pwll->pw_name) != 0)
				syslog(LOG_NOTICE, "xlock: %s unlocked screen", pwll->pw_name);
#endif
			done = 1;
			break;
		}
#else
			done = ((!strcmp((char *) crypt(buffer, userpass), userpass)) ||
				(allowroot && !strcmp((char *) crypt(buffer, rootpass), rootpass)));
#endif
	/* userpass is used */
	if (!*userpass && *buffer)
		/*
		 * the user has no password, but something was typed anyway.
		 * sounds fishy: don't let him in...
		 */
		done = False;

#endif /* !ultrix */
#endif /* !DCE_PASSWD */
#endif /* !VMS */

	return done;
}

/*-
 * Functions for DCE authentication
 *
 * Revision History:
 * 21-Aug-95: Added fallback to static password file [HAK]
 * 06-Jul-95: Mods by Heath A. Kehoe <hakehoe@icaen.uiowa.edu> for
 *            inclusion into xlockmore
 * May-95: Created by Phil O'Connell <philo@icaen.uiowa.edu>
 */

#ifdef DCE_PASSWD
#include <pthread.h>
#include <dce/sec_login.h>

static void
initDCE(void)
{
	sec_login_handle_t login_context;
	error_status_t error_status;
	boolean32   valid;
	struct passwd *pwd;

	/* test to see if this user exists on the network registry */
	valid = sec_login_setup_identity((unsigned_char_p_t) user,
			  sec_login_no_flags, &login_context, &error_status);
	if (!valid) {
		switch (error_status) {
			case sec_rgy_object_not_found:
				break;
			case sec_rgy_server_unavailable:
				(void) fprintf(stderr, "%s: the network registry is not available.\n",
					       ProgramName);
				break;
			case sec_login_s_no_memory:
				error("%s: out of memory\n");
				/*NOTREACHED */
			default:
				(void) fprintf(stderr,
					       "%s: sec_login_setup_identity() returned status %d\n",
					    ProgramName, (int) error_status);
				break;
		}

		pwd = getpwnam(user);
		if (!pwd || strlen(pwd->pw_passwd) < 10) {
			error("%s: could not get user password\n");
			/*NOTREACHED */
		}
		usernet = 0;
		(void) strcpy(userpass, pwd->pw_passwd);
	} else
		usernet = 1;

	if (allowroot) {
		valid = sec_login_setup_identity((unsigned_char_p_t) ROOT,
			  sec_login_no_flags, &login_context, &error_status);
		if (!valid) {
			switch (error_status) {
				case sec_rgy_object_not_found:
					break;
				case sec_rgy_server_unavailable:
					(void) fprintf(stderr, "%s: the network registry is not available.\n",
						       ProgramName);
					break;
				case sec_login_s_no_memory:
					error("%s: out of memory\n");
					/*NOTREACHED */
				default:
					(void) fprintf(stderr,
						       "%s: sec_login_setup_identity() returned status %d\n",
					    ProgramName, (int) error_status);
					break;
			}

			pwd = getpwuid(0);
			if (!pwd || strlen(pwd->pw_passwd) < 10) {
				(void) fprintf(stderr,
					       "%s: could not get root password\n", ProgramName);
				allowroot = 0;
			}
			rootnet = 0;
			(void) strcpy(rootpass, pwd->pw_passwd);
		} else
			rootnet = 1;
	}
	pthread_lock_global_np();
}

static char *
error_string(error_status_t error_status)
{
	static char buf[60];

	switch (error_status) {
		case error_status_ok:
			return "no error";
		case sec_rgy_object_not_found:
			return "The principal does not exist";
		case sec_rgy_server_unavailable:
			return "The network registry is not available";
		case sec_login_s_no_memory:
			return "Not enough memory is available to complete the operation";
		case sec_login_s_already_valid:
			return "The login context has already been validated";
		case sec_login_s_default_use:
			return "Can't validate the default context";
		case sec_login_s_acct_invalid:
			return "The account is invalid or has expired";
		case sec_login_s_unsupp_passwd_type:
			return "The password type is not supported";
		case sec_login_s_context_invalid:
			return "The login context itself is not valid";
		default:
			(void) sprintf(buf, "error status #%d", (int) error_status);
			return buf;
	}
}


/* 
 *----------------------------------------------------------------------
 *     Function Created 5/95 to be used with xlock to validate DCE
 *     passwords.  Routine simply returns a (1) if the the variable
 *     PASS is the USER's PASSWORD, else it returns a (0).
 *     Functions used:
 *
 *               sec_login_setup_identity
 *               sec_login_validate_identity
 *               sec_login_certify_identity
 *
 *     where setup_identity obtains the login context for the USER.
 *     This identity is then validated with validate_identity.  Finally,
 *     cerfify_identity is called to make sure that the Security
 *       Server used to set up and validate a login context is legitimate.
 *
 *       Created by Phil O'Connell
 *     philo@icaen.uiowa.edu
 *     Student Programmer
 *
 *-----------------------------------------------------------------------
 */

static int
check_dce_net_passwd(char *usr, char *pass)
{
	sec_login_handle_t login_context;
	error_status_t error_status;
	sec_passwd_rec_t password;
	boolean32   reset_password;
	sec_login_auth_src_t auth_src;
	unsigned_char_p_t principal_name;
	boolean32   valid = 0;
	char       *passcpy;
	boolean32   current_context;


	pthread_unlock_global_np();
	/* -------------------- SETUP IDENTITY--------------------------------- */

	principal_name = (unsigned_char_p_t) usr;

	/*
	 * We would rather like to refresh and existing login context instead of
	 * making a new one.
	 */

	sec_login_get_current_context(&login_context, &error_status);
	if (error_status != error_status_ok) {
		current_context = 0;
		(void) fprintf(stderr,
		       "get_current_context failed! Setting up a new one\n");

		valid = sec_login_setup_identity(principal_name, sec_login_no_flags,
					      &login_context, &error_status);

		if (!valid) {
			(void) fprintf(stderr, "sec_login_setup_identity() failed: %s\n",
				       error_string(error_status));
			pthread_lock_global_np();
			return 0;
		}
	} else
		current_context = 1;

/*--------------- VALIDATE IDENTITY ---------------------------------*/

	/* make a copy of pass, because sec_login_validate_identity() will
	   clobber the plaintext password passed to it */
	passcpy = strdup(pass);

	password.key.key_type = sec_passwd_plain;
	password.key.tagged_union.plain = (unsigned char *) passcpy;
	password.pepper = NULL;
	password.version_number = sec_passwd_c_version_none;

	valid = sec_login_validate_identity(login_context, &password, &reset_password, &auth_src, &error_status);

	/* report unusual error conditions */
	if (error_status != error_status_ok &&
	    error_status != sec_rgy_passwd_invalid &&
	    error_status != sec_login_s_already_valid &&
	    error_status != sec_login_s_null_password) {
		(void) fprintf(stderr, "sec_login_validate_identity failed: %s\n",
			       error_string(error_status));
	}
	/* done with the copy of the password */
	(void) free(passcpy);

	/* Refresh the context if we already have one */
	if (current_context) {
		if (!sec_login_refresh_identity(login_context, &error_status)) {
			(void) fprintf(stderr, "sec_login_refresh_identity failed: %s\n",
				       error_string(error_status));
		} else {
			passcpy = strdup(pass);

			password.key.key_type = sec_passwd_plain;
			password.key.tagged_union.plain = (unsigned char *) passcpy;
			password.pepper = NULL;
			password.version_number = sec_passwd_c_version_none;

			/* Have to validate the refreshed context */
			valid = sec_login_validate_identity(login_context,
			&password, &reset_password, &auth_src, &error_status);

			/* report unusual error conditions */
			if (error_status != error_status_ok &&
			    error_status != sec_rgy_passwd_invalid &&
			    error_status != sec_login_s_null_password) {
				(void) fprintf(stderr, "sec_login_validate_identity failed: %s\n",
					       error_string(error_status));
			}
		}
		/* done with the copy of the password */
		(void) free(passcpy);
	}
	/* make sure that the authentication service is not an imposter */
	if (valid) {
		if (!sec_login_certify_identity(login_context, &error_status)) {
			(void) fprintf(stderr, "Authentication service is an imposter!\n");
			/* logoutUser(); */
			valid = 0;
		}
	}
	pthread_lock_global_np();
	return valid;
}

#endif /* DCE_PASSWD */

#ifdef KERBEROS
int
krb_check_password(struct passwd *pwd, char *pass)
{
	char        realm[REALM_SZ];
	char        tkfile[MAXPATHLEN];

	/* find local realm */
	if (krb_get_lrealm(realm, 1) != KSUCCESS)
		(void) strncpy(realm, KRB_REALM, sizeof (realm));

	/* Construct a ticket file */
	(void) sprintf(tkfile, "/tmp/xlock_tkt_%d", getpid());

	/* Now, let's make the ticket file named above the _active_ tkt file */
	krb_set_tkt_string(tkfile);

	/* ask the kerberos server for a ticket! */
	if (krb_get_pw_in_tkt(pwd->pw_name, "", realm,
			      "krbtgt", realm,
			      DEFAULT_TKT_LIFE,
			      pass) == INTK_OK) {
		unlink(tkfile);
		return 1;
	}
	return 0;
}
#endif /* KERBEROS */

#undef passwd
#undef pw_name
#undef pw_passwd
#ifndef SUNOS_ADJUNCT_PASSWD
#include <pwd.h>
#endif
#ifdef MULTIPLE_ROOT

void
get_multiple(struct passwd *pw)
{
	/* This should be the first element on the linked list.
	 * If not, then there could be problems.
	 * Also all memory allocations tend to force an exit of
	 * the program.  This should probably be changed somehow.
	 */
	if (pwllh == (pwlptr) NULL) {
		if ((pwll = new_pwlnode()) == (pwlptr) ENOMEM) {
			perror("new");
			exit(1);
		}
		pwllh = pwll;
	}
	if ((pwll->pw_name = (char *) malloc(strlen(pw->pw_name) + 1)) ==
	    (char *) -1) {
		perror("new");
		exit(1);
	}
	(void) strcpy(pwll->pw_name, pw->pw_name);
	if ((pwll->pw_passwd = (char *) malloc(strlen(pw->pw_passwd) + 1)) ==
	    (char *) -1) {
		perror("new");
		exit(1);
	}
	(void) strcpy(pwll->pw_passwd, pw->pw_passwd);
	if ((pwll->next = new_pwlnode()) == (pwlptr) ENOMEM) {
		perror("new");
		exit(1);
	}
}

void
set_multiple(void)
{
	/* If you thought the above was sick, then you'll think this is
	 * downright horrific.  This is set up so that a child process
	 * is created to read in the password entries using getpwent(3C).
	 * In the man pages on the HPs, getpwent(3C) has in it the fact
	 * that once getpwent(3C) has opened the password file, it keeps
	 * it open until the process is finished.  Thus, the child
	 * process exits immediately after reading the entire password
	 * file.  Otherwise, the password file remains open the entire
	 * time this program is running.
	 *
	 * I went with getpwent(3C) because it will actually read in
	 * the password entries from the NIS maps as well.
	 */
	struct passwd *pw;
	int         pipefd[2];
	char        buf[BUFMAX], xlen;
	pid_t       cid;

#ifdef HAS_SHADOW
	struct spwd *spw;

#endif

	if (pipe(pipefd) < 0) {
		perror("Pipe Generation");
		exit(1);
	}
	if ((cid = fork()) < 0) {
		perror("fork");
		exit(1);
	} else if (cid == 0) {
		/* child process.  Used to read in password file.  Also checks to
		 * see if the uid is zero.  If so, then writes that to the pipe.
		 */
		register int sbuf = 0;
		char       *cbuf, *pbuf;

		close(pipefd[0]);
		while ((pw = getpwent()) != (struct passwd *) NULL) {
			if (pw->pw_uid != 0)
				continue;
#ifdef HAS_SHADOW
			spw = getspnam(pw->pw_name);
			if (spw)
				pw->pw_passwd = spw->sp_pwdp;
#endif
			if (pw->pw_passwd[0] != '*') {
				xlen = strlen(pw->pw_name);
				if ((sbuf + xlen) >= BUFMAX) {
					if (write(pipefd[1], buf, sbuf) != sbuf)
						perror("write");
					sbuf = 0;
				}
				cbuf = &buf[sbuf];
				*cbuf++ = xlen;
				for (pbuf = pw->pw_name; *pbuf;)
					*cbuf++ = *pbuf++;
				sbuf += xlen + 1;

				xlen = strlen(pw->pw_passwd);
				if ((sbuf + xlen) >= BUFMAX) {
					if (write(pipefd[1], buf, sbuf) != sbuf)
						perror("write");
					sbuf = 0;
				}
				cbuf = &buf[sbuf];
				*cbuf++ = xlen;
				for (pbuf = pw->pw_passwd; *pbuf;)
					*cbuf++ = *pbuf++;
				sbuf += xlen + 1;
			}
		}
		cbuf = &buf[sbuf];
		*cbuf = -1;
		sbuf++;
		if (write(pipefd[1], buf, sbuf) != sbuf)
			perror("write");

		close(pipefd[1]);
		exit(0);
	} else {
		/* parent process.  Does the actual creation of the linked list.
		 * It assumes that everything coming through the pipe are password
		 * entries that are authorized to unlock the screen.
		 */
		register int bufsize = BUFMAX, done = 0, sbuf = BUFMAX,
		            i;
		char       *cbuf, *pbuf;
		pwlptr      pwll;

		close(pipefd[1]);

		if (pwllh == (pwlptr) NULL) {
			if ((pwll = new_pwlnode()) == (pwlptr) ENOMEM) {
				perror("new");
				exit(1);
			}
			pwllh = pwll;
		}
		for (pwll = pwllh; pwll->next; pwll = pwll->next);
		while (!done) {
			if (sbuf >= bufsize) {
				if ((bufsize = read(pipefd[0], buf, BUFMAX)) <= 0)
					perror("read");
				sbuf = 0;
			}
			cbuf = &buf[sbuf];
			xlen = *cbuf++;
			if (xlen < 0) {
				done = 1;
				break;
			}
			sbuf++;
			if (sbuf >= bufsize) {
				if ((bufsize = read(pipefd[0], buf, BUFMAX)) <= 0)
					perror("read");
				sbuf = 0;
			}
			if ((pwll->pw_name = (char *) malloc(xlen + 1)) == (char *) -1)
				break;
			pbuf = pwll->pw_name;
			cbuf = &buf[sbuf];
			for (i = 0; i < xlen; i++) {
				*pbuf++ = *cbuf++;
				sbuf++;
				if (sbuf >= bufsize) {
					if ((bufsize = read(pipefd[0], buf, BUFMAX)) <= 0)
						perror("read");
					sbuf = 0;
					cbuf = buf;
				}
			}
			*pbuf = (char) NULL;

			cbuf = &buf[sbuf];
			xlen = *cbuf++;
			sbuf++;
			if (sbuf >= bufsize) {
				if ((bufsize = read(pipefd[0], buf, BUFMAX)) <= 0)
					perror("read");
				sbuf = 0;
			}
			if ((pwll->pw_passwd = (char *) malloc(xlen + 1)) == (char *) -1)
				break;
			pbuf = pwll->pw_passwd;
			cbuf = &buf[sbuf];
			for (i = 0; i < xlen; i++) {
				*pbuf++ = *cbuf++;
				sbuf++;
				if (sbuf >= bufsize) {
					if ((bufsize = read(pipefd[0], buf, BUFMAX)) <= 0)
						perror("read");
					sbuf = 0;
					cbuf = buf;
				}
			}
			*pbuf = (char) NULL;

			if ((pwll->next = new_pwlnode()) == (pwlptr) ENOMEM)
				break;
			pwll = pwll->next;
		}
		close(pipefd[0]);
	}
}
#endif

#ifdef USE_XLOCKRC

static void
gpass(void)
{
#include <sys/types.h>
#include <sys/param.h>
#define CPASSLENGTH 14
#define CPASSCHARS 64
	extern char *cpasswd;
	static char saltchars[CPASSCHARS + 1] =
	"abcdefghijklmnopwrstuvwxyzABCDEFGHIJKLMNOPWRSTUVWXYZ1234567890./";
	char        xlockrc[MAXPATHLEN], *home, *getpass();
	FILE       *fp;

	if (cpasswd[0] == '\0') {
		/*
		 * No password given on command line or from database, get from
		 * $HOME/.xlockrc instead.
		 */
		if ((home = getenv("HOME")) == 0) {
			struct passwd *p = getpwuid(getuid());

			if (p == 0) {
				error("%s: Who are you?\n");
				exit(1);
			}
			home = p->pw_dir;
		}
		(void) strncpy(xlockrc, home, MAXPATHLEN);
		(void) strncat(xlockrc, "/.xlockrc", MAXPATHLEN);
		if ((fp = fopen(xlockrc, "r")) == NULL) {
			char        pw[9], salt[2], *pw2;

			if ((fp = fopen(xlockrc, "w")) != NULL)
				(void) fchmod(fileno(fp), 0600);
			while (1) {
				(void) strncpy(pw, getpass("Key: "), sizeof pw);
				pw2 = getpass("Again:");
				if (strcmp(pw, pw2) != 0)
					error("%s: Mismatch, try again\n");
				else
					break;
			}
			/* grab a random printable character that isn't a colon */
			salt[0] = saltchars[LRAND() & (CPASSCHARS - 1)];
			salt[1] = saltchars[LRAND() & (CPASSCHARS - 1)];
			(void) strncpy(userpass, (char *) crypt(pw, salt), CPASSLENGTH);
			if (fp)
				(void) fprintf(fp, "%s\n", userpass);
		} else {
			char        buf[BUFSIZ];

			(void) fchmod(fileno(fp), 0600);
			buf[0] = '\0';
			if ((fgets(buf, sizeof buf, fp) == NULL) ||
			    (!(strlen(buf) == CPASSLENGTH - 1 ||
			       (strlen(buf) == CPASSLENGTH && buf[CPASSLENGTH - 1] == '\n')))) {
				(void) fprintf(stderr, "%s: %s crypted password %s\n", xlockrc,
				       buf[0] == '\0' ? "null" : "bad", buf);
				exit(1);
			}
			buf[CPASSLENGTH - 1] = '\0';
			(void) strncpy(userpass, buf, CPASSLENGTH);
		}
		if (fp)
			(void) fclose(fp);
	} else {
		if (strlen(cpasswd) != CPASSLENGTH - 1) {
			error("%s: bad crypted password %s\n", cpasswd);
			exit(1);
		} else
			(void) strncpy(userpass, cpasswd, CPASSLENGTH);
	}
}

#endif /* USE_XLOCKRC */

void
initPasswd(void)
{
	getUserName();
#if !defined( ultrix ) && !defined( DCE_PASSWD )
	if (!nolock && !inroot && !inwindow && grabmouse) {
#ifdef USE_XLOCKRC
		gpass();
#else
		getCryptedUserPasswd();
#endif
		if (allowroot)
			getCryptedRootPasswd();
	}
#endif /* !ultrix && !DCE_PASSWD */
#ifdef DCE_PASSWD
	initDCE();
#endif
}
