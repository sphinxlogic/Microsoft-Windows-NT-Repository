/* $Id: threads.h,v 1.2 92/01/11 16:09:13 usenet Exp $
**
** $Log:	threads.h,v $
 * Revision 1.2  92/01/11  16:09:13  usenet
 * header twiddling, bug fixes
 * 
** Revision 4.4.3.1  1991/11/22  04:12:21  davison
** Trn Release 2.0
** 
*/

#define DB_VERSION	2

typedef char		BYTE;
typedef short		WORD;
typedef long		LONG;

#define ROOT_ARTICLE	0x0001		/* article flag definitions */
#define HAS_XREFS	0x0004		/* article has an xref line */

typedef struct Article {
    ART_NUM num;
    char *id;
    struct Domain *domain;
    struct Subject *subject;
    struct Author *author;
    struct Article *parent, *children, *siblings;
    struct Root *root;
    struct Article *id_link;
    time_t date;
    WORD child_cnt;
    WORD flags;
    WORD seq;
} ARTICLE;

typedef struct Domain {
    char *name;
    ARTICLE *ids;
    struct Domain *link;
} DOMAIN;

typedef struct Author {
    struct Author *link;		/* this link MUST be first */
    char *name;
    WORD seq;
    WORD count;
} AUTHOR;

typedef struct Subject {
    struct Subject *link;		/* this link MUST be first */
    char *str;
    WORD seq;
    WORD count;
} SUBJECT;

typedef struct Root {
    struct Root *link;			/* this link MUST be first */
    ARTICLE *articles;
    SUBJECT *subjects;
    ART_NUM root_num;
    WORD thread_cnt;
    WORD subject_cnt;
    WORD seq;
} ROOT;

typedef struct {
    LONG root_num;
    WORD articles;
    WORD thread_cnt;
    WORD subject_cnt;
    WORD pad_hack;
} PACKED_ROOT;

typedef struct {
    LONG num;
    LONG date;
    WORD subject, author;
    WORD flags;
    WORD child_cnt;
    WORD parent;
    WORD padding;
    WORD siblings;
    WORD root;
} PACKED_ARTICLE;

typedef struct Total {
    LONG first, last;
    LONG string1;
    LONG string2;
    WORD root;
    WORD article;
    WORD subject;
    WORD author;
    WORD domain;
    WORD pad_hack;
} TOTAL;

typedef struct {
    BYTE l[sizeof (LONG)];
    BYTE w[sizeof (WORD)];
    BYTE version;
    BYTE pad_hack;
} BMAP;

EXT bool word_same, long_same;
EXT BMAP my_bmap, mt_bmap;

char *thread_name ANSI((char *));
void mybytemap ANSI((BMAP *));
void wp_bmap(), lp_bmap(), swap_bmaps();
