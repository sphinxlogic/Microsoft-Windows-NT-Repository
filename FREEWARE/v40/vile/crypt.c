/*	Crypt:	Encryption routines for MicroEMACS
 *		written by Dana Hoggatt and Daniel Lawrence
 *
 * $Header: /usr/build/vile/vile/RCS/crypt.c,v 1.23 1998/09/07 21:17:52 tom Exp $
 *
 */

#ifdef STANDALONE
# ifndef OPT_ENCRYPT
#  define OPT_ENCRYPT 1
# endif
# define UINT	unsigned int
# define ULONG	unsigned long
#else
# include	"estruct.h"
# include	"edef.h"
#endif

#if	OPT_ENCRYPT
static	int	mod95 (int val);

#ifdef STANDALONE

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
/* assume ANSI C */
#define HAVE_STDLIB_H 1
#endif

#include <sys/types.h>		/* sometimes needed to get size_t */

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if STDC_HEADERS || HAVE_STRING_H
# include <string.h>
  /* An ANSI string.h and pre-ANSI memory.h might conflict.  */
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif /* not STDC_HEADERS and HAVE_MEMORY_H */
#else /* not STDC_HEADERS and not HAVE_STRING_H */
# if HAVE_STRINGS_H
#  include <strings.h>
  /* memory.h and strings.h conflict on some systems */
  /* FIXME: should probably define memcpy and company in terms of bcopy,
     et al here */
# endif
#endif /* not STDC_HEADERS and not HAVE_STRING_H */

#include <stdio.h>
#include <ctype.h>

extern	int	ue_makekey (char *key, UINT len);
extern	void	ue_crypt (char *bptr, UINT len);

static void
failed(const char *s)
{
	perror(s);
	exit(1);
}


static void
filecrypt(FILE *ifp, char *key, int mailmode)
{

    char buf[5000];
    char *p;

    ue_crypt((char *)0, 0);
    ue_crypt(key, strlen(key));

    while (1) {
	p = fgets( buf, sizeof(buf), ifp);
	if (!p) {
	    if (ferror(ifp))
		failed("reading file");
	    if (feof(ifp))
		break;
	}
	if (mailmode) {
	    if (strcmp(buf, "\n") == 0)
		mailmode = 0;
	}
	if (!mailmode)
	    ue_crypt(buf, strlen(buf));
	if (fputs(buf, stdout) == EOF)
	    failed("writing stdout");
    }

    if (fflush(stdout))
	failed("flushing stdout");


}

int
main(int argc, char **argv)
{
	char key[256];
	char *prog = argv[0];
	int mailmode = 0;

	key[0] = 0;

	/* -m for mailmode:  leaves headers intact (up to first blank line)
	 * then crypts the rest.  i use this for encrypting mail messages
	 * in mh folders.  */
	if (argc > 1 && strcmp(argv[1], "-m") == 0) {
	    mailmode = 1;
	    argc -= 1;
	    argv += 1;
	}
	if (argc > 2 && strcmp(argv[1], "-k") == 0) {
	    if (strlen(argv[2]) > sizeof(key) - 1) {
		fprintf(stderr, "%s: excessive key length\n", prog);
		exit(1);
	    }
	    (void)strncpy(key, argv[2], sizeof(key));
	    (void)memset(argv[2], '.', strlen(argv[2]));
	    key[sizeof(key)-1] = '\0';
	    argc -= 2;
	    argv += 2;
	}
	if (argc > 1 && argv[1][0] == '-') {
	    fprintf(stderr,
		"usage: %s [-m] [-k crypt-key] [file ...]\n"
		"  where crypt-key will be prompted for if not specified\n"
		"  and -m will force \"mail-mode\", which leaves text before\n"
		"  the first empty line of a file (i.e. headers) intact.\n",
		prog
	    );
	    exit(1);
	}
	if (!key[0]) {
#if HAVE_GETPASS
	    char *userkey;
	    userkey = getpass("Enter key: ");

	    /* HACK -- the linux version of getpass is not
	     * interruptible.  this means there's no way to abort
	     * a botched passwd.  until this problem goes away (and
	     * i do believe it's a bug, and i'm not planning on
	     * providing our own getpass()) we'll just see if the last
	     * char of the entered key is ^C, and consider it an abort
	     * if so.  yes, this should really be the INTR char.
	     */
	    if (userkey[strlen(userkey)-1] == 3) {
		fprintf(stderr,"Aborted\n");
		exit(1);
	    }
	    (void)strncpy(key, userkey, sizeof(key));
	    (void)memset(userkey, '.', strlen(userkey));
#else
	    fprintf(stderr,
		"usage: %s [-m] -k crypt-key [file ...]\n"
		"  where '-k crypt-key' is required (no prompting on this system)\n"
		"  and -m will force \"mail-mode\", which leaves text before\n"
		"  the first empty line of a file (i.e. headers) intact.\n",
		prog
	    );
	    exit(1);
#endif
	}

	if (argc > 1) {
		int n;
		for (n = 1; n < argc; n++) {
			FILE *fp = fopen(argv[n], "r");
			if (fp == 0)
				failed(argv[n]);
			filecrypt(fp, key, mailmode);
			(void)fclose(fp);
		}
	} else {
		filecrypt(stdin, key, mailmode);
	}
	exit(0);
}

#else /* STANDALONE */

int
ue_makekey(			/* make encryption key */
char	*key,			/* where to write key */
UINT	len)
{
	register int status;	/* return status */
	int odisinp = disinp;	/* original value of disinp */
	char	temp[NPAT];

	/* turn command input echo off */
	disinp = FALSE;

	/* get the string to use as an encryption string */
	temp[0] = EOS;
	status = mlreply("Encryption String: ", temp, len-1);
	disinp = odisinp;

        if (status == TRUE) {
		(void)strcpy(key, temp);

		/* and encrypt it */
		ue_crypt((char *)0, 0);
		ue_crypt(key, strlen(key));
	}
	mlerase();		/* clear it off the bottom line */
	return(status);
}

/* ARGSUSED */
int
ue_setkey(		/* reset encryption key of current buffer */
int f GCC_UNUSED,	/* default flag */
int n GCC_UNUSED)	/* numeric argument */
{
	register int s = ue_makekey(curbp->b_key, NPAT);

	if (s == FALSE) {
		if (curbp->b_key[0] != EOS) {
			s = mlyesno("Discard encryption key");
			if (s == TRUE) {
				curbp->b_key[0] = EOS;
				make_local_b_val(curbp, MDCRYPT);
				set_b_val(curbp, MDCRYPT, FALSE);
				curwp->w_flag |= WFMODE;
			}
		}
	}
	return (s);
}

#endif /* STANDALONE */

/**********
 *
 *	ue_crypt - in place encryption/decryption of a buffer
 *
 *	(C) Copyright 1986, Dana L. Hoggatt
 *	1216, Beck Lane, Lafayette, IN
 *
 *	When consulting directly with the author of this routine,
 *	please refer to this routine as the "DLH-POLY-86-B CIPHER".
 *
 *	This routine was written for Dan Lawrence, for use in V3.8 of
 *	MicroEMACS, a public domain text/program editor.
 *
 *	I kept the following goals in mind when preparing this function:
 *
 *	    1.	All printable characters were to be encrypted back
 *		into the printable range, control characters and
 *		high-bit characters were to remain unaffected.  this
 *		way, encrypted would still be just as cheap to
 *		transmit down a 7-bit data path as they were before.
 *
 *	    2.	The encryption had to be portable.  The encrypted
 *		file from one computer should be able to be decrypted
 *		on another computer.
 *
 *	    3.	The encryption had to be inexpensive, both in terms
 *		of speed and space.
 *
 *	    4.	The system needed to be secure against all but the
 *		most determined of attackers.
 *
 *	For encryption of a block of data, one calls crypt passing
 *	a pointer to the data block and its length. The data block is
 *	encrypted in place, that is, the encrypted output overwrites
 *	the input.  Decryption is totally isomorphic, and is performed
 *	in the same manner by the same routine.
 *
 *	Before using this routine for encrypting data, you are expected
 *	to specify an encryption key.  This key is an arbitrary string,
 *	to be supplied by the user.  To set the key takes two calls to
 *	ue_crypt().  First, you call
 *
 *		ue_crypt(NULL, vector)
 *
 *	This resets all internal control information.  Typically (and
 *	specifically in the case on MICRO-emacs) you would use a "vector"
 *	of 0.  Other values can be used to customize your editor to be
 *	"incompatible" with the normally distributed version.  For
 *	this purpose, the best results will be obtained by avoiding
 *	multiples of 95.
 *
 *	Then, you "encrypt" your password by calling
 *
 *		ue_crypt(pass, strlen(pass))
 *
 *	where "pass" is your password string.  Crypt() will destroy
 *	the original copy of the password (it becomes encrypted),
 *	which is good.  You do not want someone on a multiuser system
 *	to peruse your memory space and bump into your password.
 *	Still, it is a better idea to erase the password buffer to
 *	defeat memory perusal by a more technical snooper.
 *
 *	For the interest of cryptologists, at the heart of this
 *	function is a Beaufort Cipher.  The cipher alphabet is the
 *	range of printable characters (' ' to '~'), all "control"
 *	and "high-bit" characters are left unaltered.
 *
 *	The key is a variant autokey, derived from a weighted sum
 *	of all the previous clear text and cipher text.  A counter
 *	is used as salt to obliterate any simple cyclic behavior
 *	from the clear text, and key feedback is used to assure
 *	that the entire message is based on the original key,
 *	preventing attacks on the last part of the message as if
 *	it were a pure autokey system.
 *
 *	Overall security of encrypted data depends upon three
 *	factors:  the fundamental cryptographic system must be
 *	difficult to compromise; exhaustive searching of the key
 *	space must be computationally expensive; keys and plaintext
 *	must remain out of sight.  This system satisfies this set
 *	of conditions to within the degree desired for MicroEMACS.
 *
 *	Though direct methods of attack (against systems such as
 *	this) do exist, they are not well known and will consume
 *	considerable amounts of computing time.  An exhaustive
 *	search requires over a billion investigations, on average.
 *
 *	The choice, entry, storage, manipulation, alteration,
 *	protection and security of the keys themselves are the
 *	responsibility of the user.
 *
 **********/

void
ue_crypt(
register char *bptr,	/* buffer of characters to be encrypted */
register UINT len)	/* number of characters in the buffer */
{
	register int cc;	/* current character being considered */

	static ULONG key = 0;	/* 29 bit encipherment key */
	static int salt = 0;	/* salt to spice up key with */

	if (!bptr) {		/* is there anything here to encrypt? */
		key = len;	/* set the new key */
		salt = len;	/* set the new salt */
		return;
	}
	while (len--) {		/* for every character in the buffer */

		cc = *bptr;	/* get a character out of the buffer */

		/* only encipher printable characters */
		if ((cc >= ' ') && (cc <= '~')) {

/**  If the upper bit (bit 29) is set, feed it back into the key.  This
	assures us that the starting key affects the entire message.  **/

			key &= 0x1FFFFFFFL;	/* strip off overflow */
			if (key & 0x10000000L) {
				key ^= 0x0040A001L;	/* feedback */
			}

/**  Down-bias the character, perform a Beaufort encipherment, and
	up-bias the character again.  We want key to be positive
	so that the left shift here will be more portable and the
	mod95() faster   **/

			cc = mod95((int)(key % 95) - (cc - ' ')) + ' ';

/**  the salt will spice up the key a little bit, helping to obscure
	any patterns in the clear text, particularly when all the
	characters (or long sequences of them) are the same.  We do
	not want the salt to go negative, or it will affect the key
	too radically.  It is always a good idea to chop off cyclics
	to prime values.  **/

			if (++salt >= 20857) {	/* prime modulus */
				salt = 0;
			}

/**  our autokey (a special case of the running key) is being
	generated by a weighted checksum of clear text, cipher
	text, and salt.   **/

			key = key + key + cc + *bptr + salt;
		}
		*bptr++ = cc;	/* put character back into buffer */
	}
	return;
}

static int mod95(register int val)
{
	/*  The mathematical MOD does not match the computer MOD  */

	/*  Yes, what I do here may look strange, but it gets the
		job done, and portably at that.  */

	while (val >= 9500)
		val -= 9500;
	while (val >= 950)
		val -= 950;
	while (val >= 95)
		val -= 95;
	while (val < 0)
		val += 95;
	return (val);
}
#endif
