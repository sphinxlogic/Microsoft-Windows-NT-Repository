/* $Id: strutils.h,v 1.6 1995/10/22 21:41:14 tom Exp $ */

/* scanint.c */
char * scanint (char *string, int *int_);

/* strabbr.c */
int strabbr (char *tst_, char *ref_, int cmplen, int abbr);

/* strclip.c */
int strclip (char *inout, char *clip);

/* strform2.c */
void strform2 (char *out, int maxout, char *s1, int len1, char *s2, int len2);
int strform1 (char *out, int maxout, char *s1, int len1);

/* strlcpy.c */
char *strlcpy (char *optr, char *iptr);

/* strnull.c */
char *strnull(char *c_);

/* strscale.c */
void strscale (char *co_, int col, int len);

/* strskps.c */
char *strskps(char *c_);

/* strtrim.c */
int strtrim(char *c_);

/* strucpy.c */
char *strucpy(char *optr, char *iptr);

/* strvcmp.c */
int strvcmp(char *ref_, char *tst_, int len);

/* strvcpy.c */
void strvcpy(char *co_, char *ci_);

/* strwcmp.c */
int strwcmp(char *wild_, char *tame_);
