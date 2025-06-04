/* $Id: sysutils.h,v 1.5 1995/10/23 21:56:52 tom Exp $ */

#ifndef SYSUTILS_H
#define SYSUTILS_H

#include "datent.h"

/* syasctim.c */
void	sysasctim (char *buf, DATENT *q_, int ilen);

/* sybintim.c */
int	sysbintim (char *ci_, DATENT *obfr);

/* sydelete.c */
unsigned sysdelete (char *dspec);

/* syfom.c */
void	sysfom (char *co_);

/* sygetmsg.c */
void	sysgetmsg (unsigned status, char *msg, int size_msg);

/* sygetsym.c */
int	sysgetsym (char *co_, char *ci_, int len);

/* syhour.c */
unsigned syshour (DATENT *q_);
void	syshours (char *co_, unsigned q, int len);

/* syrename.c */
unsigned sysrename (char *newspec, char *oldspec);

/* syrights.c */
int	sysrights (int mask, int word_num);

/* sytrnlog.c */
void	systrnlog (char *result, char *tofind);

/* uid2s.c */
char *	vms_uid2s(char *result, unsigned member, unsigned group);

#endif /* SYSUTILS_H */
