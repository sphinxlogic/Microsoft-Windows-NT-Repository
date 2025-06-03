/* datadef.h */
#define ABORTSTRING "\\"
#define ABORTCHAR '\\'

#define MAXMATCHES 17

#define N_BASIC_FIELDS 8
#define OTHER -1

#ifdef UNIX
#ifdef SYS5
#define rindex(a,b) strrchr(a,b)
#define index(a,b) strchr(a,b)
#endif
#ifdef BSD
#include <string.h>
#endif
#endif

typedef enum Basic_Field {

    R_NAME = 0, R_WORK_PHONE, R_HOME_PHONE, R_COMPANY, R_WORK_ADDRESS,
    R_HOME_ADDRESS, R_REMARKS, R_UPDATED

  } BasicField;    
    
extern char *Field_Names[];  
  
/* A Rolodex entry */

typedef struct {

    char *basicfields[N_BASIC_FIELDS];
    int n_others;
    char **other_fields;

  } Rolo_Entry, *Ptr_Rolo_Entry;

  
#define get_basic_rolo_field(n,x) (((x) -> basicfields)[(n)])
#define get_n_others(x) ((x) -> n_others)  
#define get_other_field(n,x) (((x) -> other_fields)[n])
  
#define set_basic_rolo_field(n,x,s) (((x) -> basicfields[(n)]) = (s))
#define set_n_others(x,n) (((x) -> n_others) = (n))
#define incr_n_others(x) (((x) -> n_others)++)
#define set_other_field(n,x,s) ((((x) -> other_fields)[n]) = (s))

typedef struct link {

#ifndef VMS
    Ptr_Rolo_Entry entry;
#else
    Ptr_Rolo_Entry ventry;
#endif
    int matched;
    struct link *prev;
#ifndef M_V7
    struct link *next;
#else
    struct link *lnext;
#endif

  } Rolo_List, *Ptr_Rolo_List;


#ifndef M_V7
#define get_next_link(x) ((x) -> next)
#else
#define get_next_link(x) ((x) -> lnext)
#endif
#define get_prev_link(x) ((x) -> prev)
#ifndef VMS
#define get_entry(x)     ((x) -> entry)
#else
#define get_entry(x)     ((x) -> ventry)
#endif
#define get_matched(x) ((x) -> matched)

#ifndef M_V7
#define set_next_link(x,y) (((x) -> next) = (y))
#else
#define set_next_link(x,y) (((x) -> lnext) = (y))
#endif
#define set_prev_link(x,y) (((x) -> prev) = (y))
#ifndef VMS
#define set_entry(x,y) (((x) -> entry) = (y))
#else
#define set_entry(x,y) (((x) -> ventry) = (y))
#endif
#define set_matched(x) (((x) -> matched) = 1)
#define unset_matched(x) (((x) -> matched) = 0);

extern Ptr_Rolo_List Begin_Rlist;
extern Ptr_Rolo_List End_Rlist;

#define MAXLINELEN 80
#define DIRPATHLEN 100

extern int changed;
extern int name_changed;
extern int reorder_file;
extern int rololocked;
extern int read_only;

extern char *rolo_emalloc();
extern char *malloc();
extern Ptr_Rolo_List new_link_with_entry();
extern char *copystr();
extern int compare_links();
extern char *timestring();
extern char *homedir(), *libdir();
extern char *getenv();
extern char *ctime();
extern char *select_search_string();
extern int in_search_mode;

/*
 * This structure allows documentation and internal access to help files
 */
extern char *hlpfiles[];
