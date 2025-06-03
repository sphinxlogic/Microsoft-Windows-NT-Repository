/* $Id: mthreads.h,v 1.2 92/01/11 16:05:33 usenet Exp $
**
** $Log:	mthreads.h,v $
 * Revision 1.2  92/01/11  16:05:33  usenet
 * header twiddling, bug fixes
 * 
** Revision 4.4.3.1  1991/11/22  04:12:15  davison
** Trn Release 2.0
** 
*/

#ifdef lint
#include "mt-lint.h"
#endif

#if !defined(TMPTHREAD) && defined(SERVERSPOOL)
# undef SERVER
# undef SPOOL
# define SPOOL SERVERSPOOL
#endif

#define RWBUFSIZ 8192

#define PASS_LOCK 1
#define DAEMON_LOCK 2

EXT char rwbuf[RWBUFSIZ];

EXT TOTAL total;

EXT int added_articles INIT(0);
EXT int expired_articles INIT(0);
EXT int added_count;
EXT int expired_count;
EXT bool extra_expire INIT(FALSE);

EXT char *strings INIT(0);
EXT WORD *subject_cnts INIT(0);
EXT WORD *author_cnts INIT(0);
EXT WORD *ids INIT(0);

EXT SUBJECT **subject_array;
EXT ROOT **root_array;
EXT AUTHOR **author_array;
EXT ARTICLE **article_array;

EXT PACKED_ROOT p_root;
EXT PACKED_ARTICLE p_article;

EXT ROOT *root_root;
EXT AUTHOR *author_root;

#ifndef DOINIT
EXT DOMAIN unk_domain;
#else
DOMAIN unk_domain = {
    ".unknown.", NULL, NULL
};
#endif

int ngmatch ANSI((char *,char *));
int onepatmatch ANSI((char *,char *));

void mt_init ANSI((void));
long mt_lock ANSI((int, int));
void mt_unlock ANSI((int));
void wrap_it_up ANSI((int));
void log_entry();
void log_error();

int read_data ANSI((void));
int write_data ANSI((char *));
void dont_read_data ANSI((int));

void process_articles ANSI((ART_NUM,ART_NUM));

int open_server ANSI((void));
char *file_exp ANSI((char *));
char *savestr ANSI((char *));

#ifndef lint
char *safemalloc ANSI((MEM_SIZE));
void free();
void safefree();
#endif

time_t get_date ANSI((char *,time_t,long));

#define Nullart Null(ARTICLE*)
