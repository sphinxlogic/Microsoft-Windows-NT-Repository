/*
    global declarations, etc. for error logging client routines

*/
#ifndef __ERRLOG_CLIENT_H

void   errlog_inast(int in);
int    errlog_level(unsigned int l);
int    errlog(unsigned int level, char *control, ...);
int    islogging(unsigned int level);
char * msg_text(int code);
int    logdump(int level, char *header, void *buf, int size);
void   errlog_lock(void);
void   errlog_unlock(void);
void   errlog_prefix(char *s);
void   errlog_flush(void);

#define L_CRITICAL (0)
#define L_ERROR    (1)
#define L_WARNING  (2)
#define L_INFO     (3)
#define L_TRACE    (4)
#define L_BABBLE   (5)
#define L_DEBUG    (6)

#define L_MINTYPE  8
#define L_LEVMASK  ((1<<L_MINTYPE) - 1)
#define L_TYPE(i)  (1<<(i+L_MINTYPE))
#define L_NULLTYPE (~((unsigned int) 0))
#define L_ALLTYPES (~L_LEVMASK)


#define L_MAIN          L_TYPE(0)       /* CRINOID.c */
#define L_NETIO         L_TYPE(1)       /* CRINOID.c */
#define L_LOCKING       L_TYPE(2)       /* proc_mgr.c */
#define L_PROC          L_TYPE(3)       /* proc_mgr.c */
#define L_PIPE          L_TYPE(4)       /* pipe2.c */
#define L_MBXQ          L_TYPE(5)       /* mbxq.c */
#define L_MEM           L_TYPE(6)       /* malloc/free */

#endif  /* __ERRLOG_CLIENT_H */
