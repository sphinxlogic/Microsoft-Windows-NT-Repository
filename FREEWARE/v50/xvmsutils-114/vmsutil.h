#include <stdio.h>  
#include <string.h>

#if __VMS_VER < 70000000
int               bcmp (char *src, char *dest, unsigned long int length);
char *            getlogin();
int               pclose(FILE *fp);
FILE              *popen(char *cmd, char *mode);
int               putenv(char *s);
long              random();
double            drand48(void);
int               srandom(unsigned int new_seed);
int               strcasecmp(char *s1, char *s2);
int               strncasecmp(char *s1, char *s2, int n);
int               unlink(char *filename);
unsigned long int waitpid(unsigned long int pid, int *statusp, int flags);
#endif
#ifndef bcopy
void bcopy(char *src, char *dest, unsigned long int length);
#endif
#ifndef bzero
void bzero(char *src, unsigned long int lenght);
#endif
#if !defined(__GNUC__) && (__VMS_VER < 70000000) 
char *index( char *string, char *c);
char *rindex( char *string, char *c);
#endif
void              Fatal_Error(char *msg, ...);
int               CreateLogical(char *name, char *table, char *definition,
                                int accmode);
int               GetLogical(char *logical_name, char *outbuf);
int               GetLogicalInd(char *logical_name);
int               GetLogicalTab(char *logical_name, char *table_name, 
                                char *outbuf);
#if __VMS_VER < 60200000
int               getopt(int argc, char **argv, char *opts);
#else
#include <unistd.h>
#endif
int               ioctl (int d, int request, char *argp);
char              *strdup(const char *s1);
int               usleep(unsigned usec);
