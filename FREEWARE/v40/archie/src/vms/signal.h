/* signal.h */
#define SIGURG  16
#define SIGTSTP 18
#define SIGCHLD 20
#define SIGIO   23
#define sigmask(m) (1 << ((m)-1))

#ifndef __GNUC__
# include <sys$library:signal.h>
#else /* Gnu C */
# include <gnu_cc_include:[000000]signal.h>
#endif /* not Gnu C */
