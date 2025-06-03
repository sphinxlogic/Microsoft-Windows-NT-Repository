/* interact (using select) - give user keyboard control

Written by: Don Libes, NIST, 2/6/90

Design and implementation of this program was paid for by U.S. tax
dollars.  Therefore it is public domain.  However, the author and NIST
would appreciate credit if this program or parts of it are used.

$Revision: 1.2 $
$Date: 1992/02/21 18:47:50 $

*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include "term.h"
#include <errno.h>
#ifdef EXTERN_ERRNO
extern int errno;
#endif
#include <signal.h>
#include <ctype.h>
#ifndef NO_STRING_H
#include <string.h>
#endif
#ifdef HPUX
#include <unistd.h>
#include <sys/ptyio.h>
#endif
#ifdef SYSV4
#include <unistd.h>
#endif
#ifdef _AIX
/* AIX has some unusual definition of FD_SET */
#include <sys/select.h>
#endif
#include "tcl.h"
#include "translate.h"
#include "global.h"
#include "command.h"

#define min(x,y)	(((x)<(y))?(x):(y))

#ifndef FD_SET
#define FD_SET(fd,fdset)	(fdset)->fds_bits[0] |= (1<<(fd))
#define FD_CLR(fd,fdset)	(fdset)->fds_bits[0] &= ~(1<<(fd))
#define FD_ZERO(fdset)		(fdset)->fds_bits[0] = 0
#define FD_ISSET(fd,fdset)	(((fdset)->fds_bits[0]) & (1<<(fd)))
#ifndef AUX2
typedef struct fd_set {
	long fds_bits[1];
	/* any implementation so pathetic as to not define FD_SET will just */
	/* have to suffer with only 32 bits worth of fds */
} fd_set;
#endif
#endif

int escape();

static int maxfds;

struct keymap {
	char *keys;
	char *action;
	char fast;	/* if true, skip tty mode changing, spawn_id check,
			etc */
};

static char EmsgBadSpawnId[] = "bad spawn id (process died earlier?) - returning to script control";
static char EmsgProcDied[] = "process died - returning to script control\r\n";

/* special pattern that signifies the expect interpreter itself */
#define INTERPRETER_ACTION	"interpreter"

/* results of in_keymap */
#define MATCH		1	/* first *match_length chars match */
#define CAN_MATCH	2	/* might match in future, i.e., valid prefix */
#define CANT_MATCH	3	/* the first *match_length chars can not */
				/* possibly match */
/* This function accepts user keystrokes and returns one of the above values */
/* describing whether the keystrokes match a key sequence, and could or */
/* can't if more characters arrive */
/* The function assigns a matching keymap if there is a match */

/* The basic idea of how this works is it does a smart sequential search. */
/* It is optimized (if you can call it that) towards a small number of */
/* key mappings, but still works well for large maps, since no function */
/* calls are made, and we stop as soon as there is a single-char mismatch, */
/* and go on to the next one.  A hash table or compiled DFA probably would */
/* not buy very much here for most maps. */
int
in_keymap(keys,length,keymap,km_len,km_entry,match_length)
char *keys;		/* characters actually read from stream */
int length;		/* # of keys */
struct keymap *keymap;	/* key map table */
int km_len;		/* # of key mappings */
struct keymap **km_entry;	/* key map that matched */
int *match_length;	/* number of chars that matched */
{
	int i;		/* which keymap */
	char *j;	/* which character in stream to test */
	char *k;	/* which character in keymap to test */
	char *last_char = keys+length;	/* actually one past last char */

	/* a tiny tweak that should help master output which typically */
	/* is length and has no key maps.  This avoids the second half */
	/* of this procedure, which otherwise mindlessly iterates on */
	/* each character. */
	if (km_len == 0) {
		*match_length = length;
		return(CANT_MATCH);
	}
        
	for (i = 0;i<km_len;i++) {
	    for (j = keys, k=keymap[i].keys;; j++,k++) {
		/* if we hit the end of this map, we must've matched! */
		if (*k == 0) {
			*km_entry = &keymap[i];
			*match_length = j-keys;
			return(MATCH);
		}

		/* if we ran out of user-supplied characters, and still */
		/* haven't matched, it might match if the user supplies more */
		/* characters next time */
		if (j == last_char) return(CAN_MATCH);

		/* skip to next key entry, if characters don't match */
		if ((*j & 0x7f) != *k) break;
	    }
        }

	/* at this point, we know that there are no matches beginning at the */
	/* first character.  Start looking for characters that can be */
	/* discarded.  We do this by comparing each incoming character to */
	/* the first character of each keymap.  If equal, we return the */
	/* number of characters skipped so far, and let us be called again */
	/* to start the matching procedure at the beginning of the input. */

	/* can certainly skip 1st char, else would've matched above */
	for (j=keys+1;j != last_char;j++) {
		for (i=0;i<km_len;i++) {
		/* if characters have arrived which don't match, but the */
		/* next one might, return to let characters be flushed */
			if (*j == keymap[i].keys[0]) {
				*match_length = j-keys;
				return(CANT_MATCH);
			}
		}
	}

	*match_length = length;
	return(CANT_MATCH);
}

/* set things up for later calls to interact */
void
init_interact()
{
#ifdef HPUX
	maxfds = sysconf(_SC_OPEN_MAX); /* from Ian Hogg, ian@dms.cdc.com */
#elif SYSV4 == 1
	maxfds = sysconf(_SC_OPEN_MAX); /* from Ian Hogg, ian@dms.cdc.com */
#else
	maxfds = getdtablesize();
#endif
}

#define finish(x)	{ status = x; goto done; }

/*ARGSUSED*/
int
cmdInteract(clientData, interp, argc, argv)
ClientData clientData;
Tcl_Interp *interp;
int argc;
char **argv;
{
#ifdef HPUX
	fd_set excep;
	int enable = 1;
#endif
	fd_set rdrs;
	int rc, wc, sc;		/* return codes for various system calls */

	struct keymap *ukey_map = 0;	/* user to process */
	struct keymap *mkey_map = 0;	/* process to user */
	struct keymap *km;	/* ptr for above while parsing */
	int mkeys_mapped = 0;
	int ukeys_mapped = 0;
	int *keys_mapped = &ukeys_mapped;
	int user_in = 0;	/* default stdin */
	int user_out = 1;	/* default stdout */
	extern int exp_dev_tty;
	int master;
	struct f *m = 0, *u = 0;/* pointers to user/master files */
	int all_fast = FALSE;	/* by default, turn off -f */
	char **oldargv = 0;	/* save original argv here if we split it */
	int status = TCL_OK;	/* final return value */
	int i;			/* trusty temp */
	int user_match_in_progress = 0;	/* true if CAN_MATCH was last result */
	int master_match_in_progress = 0;/* ditto */
	char *src, *dest;

	struct timeval inter_timeout_any;
	struct timeval *inter_timeout = 0;
	char *timeout_action = 0;
	int timeout_fast = 0;

	int tty_changed = FALSE;/* true if we had to change tty modes for */
				/* interact to work (i.e., to raw, noecho) */
	int was_raw;
	int was_echo;
	exp_tty tty_old;

	int first_time = TRUE;	/* TRUE if we have not passed any characters
		between user and process in current call to cmdInteract.
		This is just to help with error messages.  Idea is that
		if the process is bad without sending a single character,
		then it was probably a mistake in the script.
		*/

	if (argc == 2 && strchr(argv[1],'\n')) {
		return(exp_eval_with_one_arg(clientData,argc,argv));
	}

	argv++;
	argc--;

	if (argc != 0) {
		/* If flags are used, this will be too large, but it's not */
		/* worth the trouble of making two passes since free doesn't */
		/* care, and we'll  adjust our own count later when we found */
		/* out the real amount */
		if (0 == (ukey_map = (struct keymap *)malloc(argc *
						sizeof(struct keymap)))) {
			tcl_error("malloc failed (%d keymaps)",keys_mapped);
			return(TCL_ERROR);
		}
		for (i=0;i<argc;i++) ukey_map[i].fast = FALSE;
	}

	inter_timeout_any.tv_sec = 0;
	inter_timeout_any.tv_usec = 0;

	km = ukey_map;
	for (;argc>0;argc--,argv++) {
		if (streq(*argv,"-u")) {	/* treat process as user */
			argc--;argv++;
			if (argc < 1) {
				tcl_error("interact: -u needs argument");
				return(TCL_ERROR);
			}
			user_out = user_in = atoi(*argv);
			continue;
		} else if (streq(*argv,"-o")) {	/* apply following patterns */
						/* to opposite side of */
						/* interaction */
			if (mkey_map) {
				tcl_error("interact: too many -o args");
				return(TCL_ERROR);
			}
			/* give rest of user key map to master */
			mkey_map = km;
			keys_mapped = &mkeys_mapped;
			continue;
		} else if (streq(*argv,"-f")) {
			if (argc < 1) {
				tcl_error("interact: -f needs argument");
				return(TCL_ERROR);
			}
			km->fast = TRUE;
			continue;
		} else if (streq(*argv,"-F")) {
			all_fast = TRUE;
			continue;
		} else if (streq(*argv,"-timeout")) {
			argc--;argv++;
			if (argc < 1) {
				tcl_error("interact: -timeout needs action");
				return(TCL_ERROR);
			}

			/* borrow km fast flag */
			if (km->fast) {
				timeout_fast = 1;
				km->fast = 0;	/* reset for next guy */
			}
			if (all_fast) timeout_fast = 1;

			inter_timeout_any.tv_sec = get_timeout();
			inter_timeout = &inter_timeout_any;
			timeout_action = *argv;
			continue;			
		} else {
			if (all_fast) km->fast = TRUE;
			km->keys = *argv;
			/* should really compare to f->umsize, but it's hard */
			/* to get at this point */
			if (BUFSIZ < strlen(km->keys)) {
				tcl_error("key sequence \"%s\"> match_max (%s bytes)",km->keys,BUFSIZ);
				finish(TCL_ERROR);
			}
			argc--;argv++;

			km->action = *argv;
			debuglog("defining key %s, action %s\r\n",
			 km->keys,
			 km->action?(dprintify(km->action))
				   :INTERPRETER_ACTION);

			km++;
			(*keys_mapped)++;
		}
	}

	debuglog("user_out = %d,  user_in = %d\r\n",user_out, user_in);

/* if we are running this using /dev/tty */
/*#define REALTTY (user_in == 0)*/
#define REALTTY (user_in == 0 || (user_in == exp_dev_tty && exp_dev_tty != -1))
	if (REALTTY) {
		tty_changed = tty_raw_noecho(&tty_old,&was_raw,&was_echo);
	}

 restart:
	if (0 == update_master(&master)) finish(TCL_ERROR);

	if (user_in == master) {
		tcl_error("cannot interact with self - set spawn_id to a spawned process");
		finish(TCL_ERROR);
	}

	if ((!(m = fd_to_f(master, "interact"))) || f_adjust(m) ||
	    (!(u = fd_to_f(user_in,"interact"))) || f_adjust(u)) {
		finish(TCL_ERROR);
	}

	FD_ZERO(&rdrs);
#ifdef HPUX
	FD_ZERO(&excep);

	/* Turn on trapping of close, open and ioctl requests from the slave.*/
	ioctl(master, TIOCTRAP, &enable);
#endif

	for (;;) {
		int match;
		int skip;	/* # of chars that have already been printed */
				/* by expect and should be skipped */
		int write_count;/* true # of chars to write after taking */
				/* 'skip' into account */

		if (((m->size == 0) && (u->size == 0))
		    || user_match_in_progress
		    || master_match_in_progress) {
			FD_SET(user_in,&rdrs);
			FD_SET(master,&rdrs);
#ifdef HPUX
			FD_SET(user_in,&excep);
			FD_SET(master,&excep);
/*debuglog("Calling select #1, %d, %ld\r\n", maxfds, rdrs.fds_bits[0]);*/
			sc = select(maxfds,&rdrs,(fd_set *)0,&excep,
#else
			sc = select(maxfds,&rdrs,(fd_set *)0,(fd_set *)0,
#endif
						inter_timeout);
			if (-1 == sc) {
				/* window refreshes trigger EINTR, ignore */
				if (errno == EINTR) continue;
				if (errno != EBADF) {
					/* not prepared to handle anything else */
					errorlog("select: %s\r\n",sys_errlist[errno]);
					bye(-1);
				}
				fd_close(master);
				if (first_time) {
					tcl_error(EmsgBadSpawnId);
					finish(TCL_ERROR);
				} else {
					debuglog(EmsgProcDied);
					finish(TCL_OK);
				}
			} else if (sc == 0) {	/* timeout */
				int te;

				if (!timeout_action) {
					nflog("\r\n",1);
					te = escape(interp);
				} else {
					te = Tcl_Eval(interp,timeout_action,0,(char **)0);
				}
				if (!timeout_fast && REALTTY) tty_changed =
				   tty_raw_noecho(&tty_old,&was_raw,&was_echo);
				switch (te) {
				case TCL_BREAK:
				case TCL_CONTINUE:
					finish(te);
				case TCL_RETURN_TCL:
					finish(TCL_RETURN);
				case TCL_RETURN:
					finish(TCL_OK);
				case TCL_OK:
					/* god knows what the user might */
					/* have done to us in the way of */
					/* closed fds, so .... */
					if (timeout_fast) continue;
					else goto restart;
				default:
					finish(TCL_ERROR);
				}
			}
		}

		if (m->size == 0 || master_match_in_progress) {
			/* received from master, send to user */

#ifdef HPUX
			/*
			 * Exceptional condition from slave process on pty.
			 * This could be either a close or an open or an ioctl
			 * request that is not handled by the termio driver on
			 * the slave side of the pty.
			 *
			 * On HP-UX this is the only way to tell that the other
			 * process exited.  We set up trapping above to trap
			 * close actions on the pty.  When the process on the
			 * slave side exits the file descriptor is closed.
			 * On BSD4.3 and Sys5 a read would return
			 * zero bytes read, so this code is not needed.
			 *
			 * This code provided by Bob Proulx and Jeff
			 * Okamoto of HP
			 */
			if (FD_ISSET(master, &excep)) {

				struct request_info ioctl_info;
				if (ioctl(master,TIOCREQCHECK,&ioctl_info) < 0) {
					debuglog("ioctl error on TIOCREQCHECK: %d", errno);
					break;
				}
				if (ioctl_info.request == TIOCCLOSE) {

/* The process on the slave side closed the pty.  This probably means that it
 exited.  It could be that the process just closed the file descriptor.
 The close is not blocked on the slave side.  If the master side is closed
 the handshake will be completed automatically.  Since there is no need to
 complete the handshake we return here.
 */
					break;	/* EOF from slave process. */
				}
/* Handshake the ioctl or the open request.  We don't know what the request
 was but we return it as successfully completed even though we did nothing.
 This was probably some type of modem control which would mean something on a
 real tty but is meaningless on a pty.  The application is faked-out and that
 is the purpose of this code anyway.
 */
				if (ioctl(master, TIOCREQSET, &ioctl_info) < 0)
					debuglog("ioctl error on TIOCREQSET after ioctl or open on slave: %d", errno);
			}
#endif /*HPUX*/

			/* received from master, send to user */
			if (FD_ISSET(master, &rdrs)) {
/*NEW*/				if (0 >= (rc = read(master, m->buffer+m->size,
						m->msize-m->size))) {
					debuglog("read(spawn_id=%d) = %d, errno = %d\r\n",master,rc,errno);
					/* don't smash errno in prev. line */
					fd_close(master);
					if (first_time) {
						tcl_error(EmsgBadSpawnId);
						finish(TCL_ERROR);
					} else {
						debuglog(EmsgProcDied);
						finish(TCL_OK);
					}
				}
				FD_CLR(master,&rdrs);

				/* got a char from master, so... */
				first_time = FALSE;
				m->size += rc;

				/* Process is sending us I/O.  Always send */
				/* user but sometimes printify and surround */
				/* with diagnostics. */
				if (is_debugging || debugfile) {
					int len; char *p;
					debuglog("process typed {");
					m->buffer[m->size] = 0;
					p = printify(m->buffer);
					len = strlen(p);
					if (is_debugging) {
						logn(p,len);
						write(user_out,p,len);
						if (user_out != 1)
							write(1,p,len);
					} else {
						if (logfile)
							fwrite(m->buffer,1,rc,logfile);
						fwrite(p,1,len,debugfile);
						write(user_out,m->buffer,rc);
					}
					/* want to send to both stdout/err */
					/* but don't want user to see twice! */
					debuglog("}\r\n");
				}
			}
	 	} /* else if (m->size == 0 */

		if (m->size != 0) {
			int te;	/* result of Tcl_Eval */
			struct keymap *map;

			master_match_in_progress = FALSE;
			switch (in_keymap(m->buffer,m->size,mkey_map,
					mkeys_mapped,&map,&match)) {
			case MATCH:
				m->size -= match;

				/* handle chars left over from expect that */
				/* shouldn't be printed again */
				if (m->printed) {
					if (m->printed > match) {
						m->printed -= match;
					} else  m->printed = 0;
				}

				if (m->size != 0) {
				   memcpy(m->buffer,m->buffer+match,m->size);
				}
				m->buffer[m->size] = '\0';

				if (!map->fast && tty_changed)
					tty_set(&tty_old,was_raw,was_echo);
				if (!map->action) {
					nflog("\r\n",1);
					te = escape(interp);
				} else {
					te = Tcl_Eval(interp,map->action,0,(char **)0);
				}
				if (!map->fast && REALTTY) tty_changed =
				   tty_raw_noecho(&tty_old,&was_raw,&was_echo);
				switch (te) {
				case TCL_BREAK:
				case TCL_CONTINUE:
					finish(te);
				case TCL_RETURN_TCL:
					finish(TCL_RETURN);
				case TCL_RETURN:
					finish(TCL_OK);
				case TCL_OK:
					/* god knows what the user might */
					/* have done to us in the way of */
					/* closed fds, so .... */
					if (map->fast) continue;
					else goto restart;
				default:
					finish(TCL_ERROR);
				}
			case CAN_MATCH:
				master_match_in_progress = TRUE;
				/* force user keystrokes to be seen */
				/* necessary, only because we check master's */
				/* first, and if in the middle of a possible */
				/* master match, logic above directs us here */
/*NEW*/				goto user_to_master;	/* was "continue"*/
			case CANT_MATCH:
				break;
			}

			/* assert (case == CANT_MATCH) */

			m->buffer[m->size] = 0;

/* If expect has left characters in buffer, it has already echoed them to the
 screen, thus we must prevent them being rewritten.  Unfortunately this gives
 the possibility of matching chars that have already been output, but we do so
 since the user could have avoided it by flushing the output buffers directly.
 Hopefully, this code should still be fast for when m->printed == 0
*/
			skip = 0;
			write_count = match;
			if (m->printed) {
				skip = min(match,m->printed);
				m->printed -= skip;
				write_count -= skip;
			}

		       if (write_count) {/* sorry about the formatting */
			logn(m->buffer + skip,write_count);
			wc = write(user_out,m->buffer + skip,write_count);
			if (wc <= 0) { /* user disappeared */
				fd_close(user_out);
				if (first_time) {
					tcl_error("user died");
					finish(TCL_ERROR);
				} else {
					debuglog("user went away or closed stdin\r\n");
					finish(TCL_OK);
				}
			}
		       }

			m->size -= match;
			if (m->size != 0) {
			   memcpy(m->buffer,m->buffer+match,m->size);
			}
			m->buffer[m->size] = '\0';
		}

/*NEW*/user_to_master:
		/* chars received from user, send to master */
		if (u->size == 0 || user_match_in_progress) {
		    if (FD_ISSET(user_in, &rdrs)) {
			if (0 >= (rc = read(user_in,	u->buffer + u->size,
							u->msize - u->size))) {
				errorlog("user sent EOF or disappeared\r\n");
				bye(-1);
			}
			/* avoid another function call if possible */
			if (debugfile || is_debugging) {
				u->buffer[u->size + rc] = 0;
				debuglog("user typed {%s}\r\n",
					printify(u->buffer + u->size));
			}

			u->size += rc;
			FD_CLR(user_in,&rdrs);
		    } else continue;
		}

		if (u->size != 0) {
			int te;	/* result of Tcl_Eval */
			struct keymap *map;

			user_match_in_progress = FALSE;
			switch (in_keymap(u->buffer,u->size,ukey_map,
					ukeys_mapped,&map,&match)) {
			case MATCH:
				u->size -= match;

				/* handle chars left over from expect that */
				/* shouldn't be printed again */
				if (u->printed) {
					if (u->printed > match) {
						u->printed -= match;
					} else  u->printed = 0;
				}

				if (u->size != 0) {
				   memcpy(u->buffer,u->buffer+match,u->size);
				}
				u->buffer[u->size] = '\0';

				if (!map->fast && tty_changed)
					tty_set(&tty_old,was_raw,was_echo);
				if (!map->action) {
					nflog("\r\n",1);
					te = escape(interp);
				} else {
					te = Tcl_Eval(interp,map->action,0,(char **)0);
				}
				if (!map->fast && REALTTY) tty_changed =
				   tty_raw_noecho(&tty_old,&was_raw,&was_echo);
				switch (te) {
				case TCL_BREAK:
				case TCL_CONTINUE:
					finish(te);
				case TCL_RETURN_TCL:
					finish(TCL_RETURN);
				case TCL_RETURN:
					finish(TCL_OK);
				case TCL_OK:
					/* god knows what the user might */
					/* have done to us in the way of */
					/* closed fds, so .... */
					if (map->fast) continue;
					else goto restart;
				default:
					finish(TCL_ERROR);
				}
			case CAN_MATCH:
				user_match_in_progress = TRUE;
				continue;
			case CANT_MATCH:
				break;
			}

			/* assert (case == CANT_MATCH) */

			u->buffer[u->size] = 0;

/* If expect has left characters in buffer, it has already echoed them */
/* to the screen, thus we must prevent them being rewritten.  Unfortunately */
/* this gives the possibility of matching chars that have already been */
/* output, but we do so since the user could have avoided it by flushing the */
/* output buffers directly. */
/* Hopefully, this code should still be fast for when m->printed == 0 */
			skip = 0;
			write_count = match;
			if (u->printed) {
				skip = min(match,u->printed);
				u->printed -= skip;
				write_count -= skip;
			}

		       if (write_count) {/* sorry about the formatting */
			wc = write(master,u->buffer + skip,write_count);
			if (wc <= 0) { /* process disappeared */
				fd_close(master);
				if (first_time) {
					tcl_error("process died");
					finish(TCL_ERROR);
				} else {
					debuglog("process went away or closed stdin\r\n");
					finish(TCL_OK);
				}
			}
			first_time = FALSE;
		       }

			u->size -= match;
			if (u->size != 0) {
			   memcpy(u->buffer,u->buffer+match,u->size);
			}
			u->buffer[u->size] = '\0';
		}
	}
 done:
	if (tty_changed) tty_set(&tty_old,was_raw,was_echo);
	if (ukey_map) free((char *)ukey_map);
	if (oldargv) free((char *)argv);

	/* restore lowercase buffer for expect */
	/* do it here rather than upon entrance to expect because */
	/* likelihood is that interact is called very infrequently */
	/* and when called it rarely leaves any characters in buffers */
	/* following code assumes buffers are always null-terminated */
	/* which both expect and interact guarantee, and they are the only */
	/* ones that ever manipulate buffer */
	if (m && m->buffer) {
		for (src = m->buffer, dest = m->lower;*src;src++,dest++) {
			*dest = isupper(*src)?tolower(*src):*src;
		}
	}
	if (u && u->buffer) {
		for (src = u->buffer, dest = u->lower;*src;src++,dest++) {
			*dest = isupper(*src)?tolower(*src):*src;
		}
	}


	return(status);
}

static struct timeval zerotime = {0, 0};
static struct timeval anytime = {0, 0};	/* can be changed by user */

/* returns TCL_OK or TCL_ERROR */
/* if TCL_OK && *master2 && (*n2 == 0), then timed out */
int
ready(masters,n,masters2,n2,timeout)
int *masters;
int n;
int *masters2;	/* out variable */
int *n2;	/* out variable */
int *timeout;
{
	int i;	/* index into in-array */
	int j;	/* index into out-array */
	fd_set rdrs;
	
	struct timeval *t = NULL;

	if (*timeout >= 0) {
		t = &anytime;
		t->tv_sec = *timeout;
	}

	*timeout = 0;	/* says that we've done all the waiting */

 restart:
	FD_ZERO(&rdrs);
	for (i = 0;i < n;i++) {
		FD_SET(masters[i],&rdrs);
	}

	if (-1 == select(maxfds,&rdrs,(fd_set *)0,(fd_set *)0,t)) {
		/* window refreshes trigger EINTR, ignore */
		if (errno == EINTR) goto restart;
		else if (errno == EBADF) {
		    /* someone is rotten */
		    for (i=0;i<n;i++) {
			fd_set suspect;
			FD_ZERO(&suspect);
			FD_SET(masters[i],&suspect);
			if (-1 == select(maxfds,&suspect,
			    		(fd_set *)0,(fd_set *)0,&zerotime)) {
				tcl_error("invalid spawn_id (%d)\r",masters[i]);
				return(TCL_ERROR);
			}
		   }
	        } else {
			/* not prepared to handle anything else */
			tcl_error("select: %s\r",sys_errlist[errno]);
			return(TCL_ERROR);
		}
	}

	j=0;
	for (i=0;i<n;i++) {
		if (FD_ISSET(masters[i],&rdrs)) {
			masters2[j] = masters[i];
			if (TCL_ERROR == f_adjust(masters2[j] + fs))
				return(TCL_ERROR);
			j++;
			if (j==*n2) break;
		}
	}
	*n2 = j;	/* returns number of fds set */
	return(TCL_OK);
}

void
usleep(usec)
long usec;		/* microseconds */
{
	struct timeval t;

	t.tv_sec = usec/1000000L;
	t.tv_usec = usec%1000000L;
	(void) select(1, (fd_set *)0, (fd_set *)0, (fd_set *)0, &t);
}
