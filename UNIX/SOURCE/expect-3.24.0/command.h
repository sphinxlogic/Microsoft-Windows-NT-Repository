/* command.h - definitions for expect commands

Written by: Don Libes, NIST, 2/6/90

Design and implementation of this program was paid for by U.S. tax
dollars.  Therefore it is public domain.  However, the author and NIST
would appreciate credit if this program or parts of it are used.
*/

void tcl_error();
int getmaster();
char *get_var();
int get_timeout();
extern int master;
struct f *update_master();
extern int default_match_max;

extern int loguser;	/* shared by cmdLoguser, cmdExpect and cmdSpawn */

/* special version of log for non-null-terminated strings which */
/* never need printf-style formatting. */
#define logn(buf,length)  { \
			  if (logfile) fwrite(buf,1,length,logfile); \
			  if (debugfile) fwrite(buf,1,length,debugfile); \
			  }

#define USER_SPAWN_ID		0
#define USER_SPAWN_ID_LIT	"0"
/* only defined on FD_VALID descriptors */
#define is_user(x)	((x == 0) && (fs[x].flags & FD_USER))

/* fd flags */
#define FD_VALID	0x1	/* valid entry in fds table */
#define FD_CLOSED	0x2	/* has been closed */
#define FD_USER		0x4	/* stdin or stdout */
#define FD_SYSWAITED	0x8	/* wait() has been called */
#define FD_USERWAITED	0x10	/* expect's wait has been called */

/* each process is associated with a 'struct f'.  An array of these ('fs') */
/* keeps track of all processes.  They are indexed by the true fd to the */
/* master side of the pty */
struct f {
	int pid;
	char *buffer;	/* input buffer */
	char *lower;	/* input buffer in lowercase */
	int size;	/* current size of data */
	int msize;	/* size of buffer */
	int umsize;	/* user view of size of buffer */
	int flags;	/* see above */
	int wait;	/* raw status from wait() */
	int printed;	/* # of characters written to stdout (if logging on) */
			/* but not actually returned via a match yet */
};
extern struct f *fs;

struct f *fd_to_f();
int f_adjust();


/* data structure for saving results of expect_before/after */
struct expect_special {
	char *prefix;		/* command plus blank to shove in front */
				/* of args upon finding 1 arg and recursing */
	struct ecase *ecases;
	int ecount;		/* count of cases */
	int *masters;
	int mcount;		/* number of masters */
	int me;
};
