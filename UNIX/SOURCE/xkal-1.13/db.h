/*
 *	db.h : External defs for database users.
 *
 *      George Ferguson, ferguson@cs.rochester.edu, 27 Oct 1990.
 *	Version 1.1 - 27 Feb 1991.
 *
 *	$Id: db.h,v 2.1 91/02/28 11:21:15 ferguson Exp $
 */

#ifndef DB_H
#define DB_H

/*
 * The appointment database is a hierarchical data structure. It has an
 * array indexed by dow (and 0 for unspecified dow) of year structures,
 * which in turn each contain a list of month structures, each containing
 * a list day structures, each containing a list of message structures
 * that store the actual reminders.
 *
 * The key to using incompletely-specified patterns is described in
 * lookupEntry().
 */
typedef struct _msgrec {
	int dow,year,month,day;		/* redundant, but useful */
	int hour,mins;
	char *text;
	int system;
	int level;
	struct _msgrec *next;
} Msgrec;
typedef struct _dayrec {
	int day;
	Msgrec *msgs;
	struct _dayrec *next;
} Dayrec;
typedef struct _monrec {
	int mon;
	Dayrec *days;
	struct _monrec *next;
} Monrec;
typedef struct _yearrec {
	int year;
	Monrec *mons;
	struct _yearrec *next;
} Yearrec;

extern Yearrec *Db[8];

extern void initDb(), readDb(), writeDb();
extern Msgrec *addEntry();
extern Boolean deleteEntry();
extern int lookupEntry();

#endif /* DB_H */
