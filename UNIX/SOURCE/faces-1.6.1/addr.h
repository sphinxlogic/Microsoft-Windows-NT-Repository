
/*  @(#)addr.h 1.2 91/04/15
 *
 *  Copyright (c) 1986, 1987, 1988, 1989, 1990, 1991, by:
 *
 *      Bill Nesheim
 *      Daniel Karrenberg
 *      David Herron
 *      Rich Salz
 *      John Mackin
 *
 *  All rights reserved.
 *
 *  Permission is given to distribute these sources, as long as the
 *  copyright messages are not removed, and no monies are exchanged.
 *
 *  No responsibility is taken for any errors inherent either
 *  to the comments or the code of this program, but if reported
 *  (see README file), then an attempt will be made to fix them.
 */

/*
 *  Mail address data structures
 */

#define	NEED_BZERO
#define NSUBDOM		20      	/* # of subdomain names in domain */

typedef unsigned int MALLOCT;		/* Parameter to malloc		*/

/*
**  An address.
*/
typedef struct _addr {
    struct _addr	*next;		/* next address in list		*/
    struct _dom		*route;		/* route icl. destination domain */
    struct _dom		*destdom;	/* destination domain		*/
    char		*localp;	/* RFC local part		*/
    char		*name;		/* Comment name			*/
    char		*group;		/* Group (List) phrase		*/
    char		*comment;	/* () comment phrase		*/
    char		*error;		/* error text if not NULL	*/
} Addr;


/*
**  A domain.
*/
typedef struct _dom {
    struct _dom		*next;		/* next domain (f.i. in route)	*/
    char		*sub[NSUBDOM];	/* subdomain strins		*/
    char		**top;		/* toplevel domain		*/
} Dom;


extern Addr	*newAddr();		/* Create a new address		*/
extern Dom	*newDom();		/* Create a new domain		*/
extern Addr	*adrlist;
extern Addr	*errlist;
static char	*Amalloc();
extern char	*strcpy();
extern char	*strncpy();
extern char	*strcat();

/* SHUT UP! */
#define Strcpy		(void)strcpy
#define Strncpy		(void)strncpy
#define Strcat		(void)strcat
#define Sprintf		(void)sprintf
