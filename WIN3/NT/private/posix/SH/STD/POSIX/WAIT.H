/*
 * POSIX <sys/wait.h>
 */
#if __STDC__
#define	ARGS(args)	args
#else
#define	ARGS(args)	()
#endif

#ifndef sparc
typedef int pid_t;		/* belong in sys/types.h */
#endif

#ifdef sparc
# include "/usr/include/sys/wait.h"
#else

/* waitpid options */
#define WNOHANG		1	/* don't hang in wait */
#define WUNTRACED	2	/* tell about stopped, untraced children */

#define	WSTOPPED	0x7F	/* process is stopped */

#define WIFSTOPPED(x)	(((x)&0xFF) == 0x7F)
#define WIFSIGNALED(x)	(((x)&0xFF) != 0x7F && ((x)&0x7F) != 0)
#define WIFEXITED(x)	(((x)&0xFF) != 0x7F && ((x)&0x7F) == 0)
#define	WIFCORED(x)	(!!((x)&0x80)) /* non-standard */
#define	WEXITSTATUS(x)	((x)>>8&0xFF)
#define	WTERMSIG(x)	((x)&0x7F)
#define	WSTOPSIG(x)	((x)>>8&0xFF)

pid_t wait ARGS((int *statp));
#if _BSD
pid_t wait3 ARGS((int *statp, int options, Void *));
/* todo: does not emulate pid argument */
#define	waitpid(pid, sp, opts)	wait3(sp, opts, (Void*)NULL)
#else
pid_t waitpid ARGS((pid_t pid, int *statp, int options));
#endif

#endif /* sparc */
