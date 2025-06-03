/*
 * Define structures needed for user-defined rule file translation.
 */
typedef struct info_blk *info_ptr;
typedef int map_callout	( info_ptr blk, char *translation, int maxlen, 
	access_info acc, char **case_preserved, int cp_size );
struct info_blk {

    info_ptr next;			/* succeeding block in chain */
    int code;				/* Entry type. */
    int pat_len;			/* pattern length, not including '*' */
    int val_len;			/* length of value string */
    int wildcard;			/* if nonzero, pattern ends in '*' 
					 * if >1, value ends in wildcard. */
    char *pattern;			/* Substring to find in source */
    char *value;			/* Replacement value for substring */
    char *name;	 			/* Rule name for logging */
    map_callout *usermap;		/* auxillary map routine */
};

struct callback_vector {
    int maj_version, min_version;
    int *trace_level;
    int *reentrant_c_rtl, *reentrant_vms_rtl;
    int (*putlog)(int, char*, ...);
    void *(*tf_open)();
    int (*tf_close)(), (*tf_read)(), (*tf_last_error)();
    int (*tf_getline)(), (*tf_header_info)();
};
#define DYN_VERSION_MAJOR 1
#define DYN_VERSION_MINOR 1
/*
 * Define contants for rule names and translation results.
 */
#define IDENT_MAP_RULE_MAP 1
#define IDENT_MAP_RULE_PASS 2
#define IDENT_MAP_RULE_FAIL 3
#define IDENT_MAP_RULE_REDIRECT 4
#define IDENT_MAP_RULE_EXEC 5
#define IDENT_MAP_RULE_USERMAP 6
#define IDENT_MAP_RULE_PROTECT 7
#define IDENT_MAP_RULE_DEFPROT 8
#define IDENT_MAP_LOOP -1
#define IDENT_MAP_TRANSLATED -1		/* Continue with next rule */
#define IDENT_MAP_FAILED 0
#define IDENT_MAP_OK_PASS 1		/* Pass rule matched */
#define IDENT_MAP_OK_REDIRECT 2		/* Redirect rule matched */
#define IDENT_MAP_OK_EXEC 3
#define IDENT_MAP_OK_USERMAP 4
