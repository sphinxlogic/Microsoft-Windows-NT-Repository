/* interact (with only one process) - give user keyboard control

Written by: Don Libes, NIST, 2/6/90

Design and implementation of this program was paid for by U.S. tax
dollars.  Therefore it is public domain.  However, the author and NIST
would appreciate credit if this program or parts of it are used.
*/

/* This file exists for deficient versions of UNIX that lack select,
poll, or some other multiplexing hook.  Instead, this code uses two
processes per spawned process.  One sends characters from the spawnee
to the spawner; a second send chars the other way.

This will work on any UNIX system.  The only sacrifice is that it
doesn't support multiple processes.  Eventually, it should catch
SIGCHLD on dead processes and do the right thing.  But it is pretty
gruesome to imagine so many processes to do all this.  If you change
it successfully, please mail back the changes to me.  - Don
*/

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#ifndef M_XENIX
#include <sys/wait.h>
#else
#ifndef WEXITSTATUS
#define WEXITSTATUS(stat) (((*((int *) &(stat))) >> 8) & 0xff)
#endif
  /* <sys/wait.h> is not available on SCO XENIX 386 -- pf@artcom0.north.de */
#endif

#include <ctype.h>
#include <setjmp.h>
#include <errno.h>
#include "tcl.h"
#include "translate.h"
#include "global.h"
#include "command.h"
#include "term.h"

#define min(x,y)	(((x)<(y))?(x):(y))

int escape();

struct keymap {
	char *keys;
	char *action;
	char fast;	/* if true, skip tty mode changing, spawn_id check,
			etc */
};

static jmp_buf env;		/* for interruptable read() */
static int reading;		/* while we are reading */
				/* really, while "env" is valid */
static int deferred_interrupt = FALSE;	/* if signal is received, but not */
				/* in i_read record this here, so it will */
				/* be handled next time through i_read */

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
}

/* interruptable read */
static int
i_read(fd,buffer,length)
int fd;
char *buffer;
int length;
{
	int cc = -2;

	if (deferred_interrupt) return(cc);

	if (0 == setjmp(env)) {
		reading = TRUE;
		cc = read(fd,buffer,length);
	}
	reading = FALSE;
	return(cc);
}


void sigchld_handler()
{
	if (reading) longjmp(env,1);

	deferred_interrupt = TRUE;
}

/* exit status for the child process created by cmdInteract */
#define USER_DIED	-2
#define PROCESS_DIED	-3

static void
clean_up_after_child(master)
int master;
{
/* should really be recoded using the common wait code in command.c */
	int wait_status;

	/* first, find out why it died */
	(void) wait(&wait_status);
	if (WEXITSTATUS(wait_status) == USER_DIED) bye(0);
	/* else process died */
	deferred_interrupt = FALSE;
	fd_close(master);
	master = -1;
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
	int skip;	/* # of chars that have already been printed */
			/* by expect and should be skipped */
	int write_count;/* true # of chars to write after taking */
			/* 'skip' into account */
	int pid;
	int rc, wc;	/* return codes for various system calls */
	int match;

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

	km = ukey_map;
	for (;argc>0;argc--,argv++) {
		if (streq(*argv,"-u")) {	/* treat process as user */
			tcl_error("expect not compiled with multiprocess support");
			/* select a different INTERACT_TYPE in Makefile */
			return(TCL_ERROR);
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

/* restart: (from multiprocess code) */
	if (0 == update_master(&master)) finish(TCL_ERROR);

	if (master == user_out) {
		tcl_error("cannot interact with self - set spawn_id to a spawned process");
		finish(TCL_ERROR);
	}

	if ((!(m = fd_to_f(master, "interact"))) || f_adjust(m) ||
	    (!(u = fd_to_f(user_in,"interact"))) || f_adjust(u)) {
		finish(TCL_ERROR);
	}

	deferred_interrupt = FALSE;
	flush_streams();

	if (-1 == (pid = fork())) {
		tcl_error("fork: %s",sys_errlist[errno]);
		finish(TCL_ERROR);
	}
	if (pid == 0) { /* child */
		fd_close(0);
		for (;;) {

		if (m->size == 0 || master_match_in_progress) {
			/* if (io pending on master ...  { */
				/* read from master, send to user */
				if (0 >= (rc = read(master, m->buffer+m->size,
						m->msize-m->size))) {
					debuglog("read(spawn_id=%d) = %d, errno = %d\r\n",master,rc,errno);
					/* don't smash errno in prev. line */
					debuglog("process went away or sent eof\r\n");
					exit(PROCESS_DIED);
				}
	
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
/*			}*/
			}

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
					}
					else m->printed = 0;
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
				if (!map->fast) tty_changed =
				   tty_raw_noecho(&tty_old,&was_raw,&was_echo);
				switch (te) {
				case TCL_BREAK:
				case TCL_CONTINUE:
				case TCL_RETURN:
				case TCL_RETURN_TCL:
					exit(0);
				case TCL_OK:
					/* god knows what the user might */
					/* have done to us in the way of */
					/* closed fds, so .... */
					if (map->fast) continue;
					else goto restart;
				default:
					exit(USER_DIED);
				}
			case CAN_MATCH:
				master_match_in_progress = TRUE;
				continue;
			case CANT_MATCH:
				break;
			}

			/* assert (case == CANT_MATCH) */

			m->buffer[m->size] = 0;

/* If expect has left characters in buffer, it has already echoed them */
/* to the screen, thus we must prevent them being rewritten.  Unfortunately */
/* this gives the possibility of matching chars that have already been */
/* output, but we do so since the user could have avoided it by flushing the */
/* output buffers directly. */
/* Hopefully, this code should still be fast for when m->printed == 0 */
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
	   }
	} else { /* parent */
		debuglog("fork = %d\r\n",pid);
#ifdef SIGCHLD
		signal(SIGCHLD,sigchld_handler);
#else
		signal(SIGCLD,sigchld_handler);
#endif
	restart:
		tty_changed = tty_raw_noecho(&tty_old,&was_raw,&was_echo);

		while (TRUE) {
		/* chars received from user, send to master */
		if (u->size == 0 || user_match_in_progress) {
/*		    if (FD_ISSET(user_in, &rdrs)) {*/
			if (-2 == (rc = read(user_in,	u->buffer + u->size,
							u->msize - u->size))) {
				debuglog("child died\r\n");
				clean_up_after_child(master);
				break;
			} else if (rc == 0 /* eof */ || rc == -1 /* error */) {
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
/*		    } else continue;*/
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
				if (!map->fast) tty_changed =
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
				debuglog("process went away or closed stdin\r\n");
				clean_up_after_child(master);
				if (first_time) {
					tcl_error("process died");
					finish(TCL_ERROR);
				} else {
					debuglog("process went away or closed stdin\r\n");
					finish(TCL_OK);
				}
			}
			first_time = FALSE;
			flush_streams();
		       }

			u->size -= match;
			if (u->size != 0) {
			   memcpy(u->buffer,u->buffer+match,u->size);
			}
			u->buffer[u->size] = '\0';
		}
	}
	} /* ugly indentation to make it easier to keep in sync with */
	  /* other versions */

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

	/*
	Unlike the other implementation of interact, we can't just lowerize
	the buffer.  The reason is that since the child is writing the buffer,
	the parent never sees that it has been emptied - so instead just zero
	it.  This means that the transition from 'interact' to 'expect' can
	potentially drop things on the floor, but in practice, this shouldn't
	actually affect 99.9% of applications.
	*/
	if (m && m->buffer) {
		m->size = 0;
		m->buffer[0] = '\0';
#if 0
		for (src = m->buffer, dest = m->lower;*src;src++,dest++) {
			*dest = isupper(*src)?tolower(*src):*src;
		}
#endif
	}

	if (u && u->buffer) {
		for (src = u->buffer, dest = u->lower;*src;src++,dest++) {
			*dest = isupper(*src)?tolower(*src):*src;
		}
	}

	return(status);
}

/*ARGSUSED*/
int ready(masters,n,masters2,n2,timeout)
int *masters;
int n;
int *masters2;	/* out variable */
int *n2;	/* out variable */
int *timeout;
{
	if (n > 1) {
		tcl_error("expect not compiled with multiprocess support");
		/* select a different INTERACT_TYPE in Makefile */
		return(TCL_ERROR);
	}

	masters2[0] = masters[0];
	*n2 = 1;

	/* since this implementation can't wait for 0 seconds, bump it back */
	/* to 1 (prepare for upcoming read) */
	if (*timeout == 0) *timeout = 1;

	/* if timer should be turned off, do so */
	if (*timeout == -1) *timeout = 0;

	return(TCL_OK);
}


/* There is no portable way to do sub-second sleeps on such a system, so */
/* do the next best thing (without a busy loop) and fake it: sleep the right */
/* amount of time over the long run.  Note that while "subtotal" isn't */
/* reinitialized, it really doesn't matter for such a gross hack as random */
/* scheduling pauses will easily introduce occasional one second delays. */
void
usleep(usec)
long usec;		/* microseconds */
{
	static subtotal = 0;
	int seconds;

	subtotal += usec;
	if (subtotal < 1000000) return;
	seconds = subtotal/1000000;
	subtotal = subtotal%1000000;
	sleep(seconds);
}
