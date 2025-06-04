/*
 * configuration parameters for qi and friends
 */

#define KEYWORD_SIZE 30         /* number of chars in keyword field */
#define FIELD_SIZE 2            /* number of chars in field identifier */
#define ID_SIZE 9               /* number of chars in entity identifier */
#define IDX_RECORD_SIZE (KEYWORD_SIZE + FIELD_SIZE + ID_SIZE)
#define IDX_KEY_SIZE    (KEYWORD_SIZE + FIELD_SIZE + ID_SIZE)
#define IDX_KEY2_SIZE   (FIELD_SIZE + ID_SIZE)

#define SEQ_SIZE 2              /* number of chars in sequence number field */
#define ATTR_SIZE 1             /* number of characters in attribute field */
#define DATA_SIZE 120           /* maximum number of chars in data field */
#define DAT_KEY_SIZE    (ID_SIZE + FIELD_SIZE + SEQ_SIZE)
#define DAT_OH_SIZE     (DAT_KEY_SIZE + ATTR_SIZE) /* non-data (overhead) size */
#define DAT_RECORD_SIZE (DAT_OH_SIZE + DATA_SIZE)  /* max record size */

#define PRIMARY_KEY   0         /* symbolic key-of-reference values */
#define SECONDARY_KEY 1

#define MAX_FIELD 100           /* 10 ** FIELD_SIZE */
#define MAX_SEQ 100             /* 10 ** SEQ_SIZE */

#define MIN_KEYWORD 2           /* set to 2 to exclude initials in index */
                                /* set to 1 to index initials */
                                /* set to 0 to include blank records */

#define MAX_RECORDS 30          /* don't display more than this many records */
#define MAX_CHANGES 1           /* don't change more than this many records */
#define MAX_DELETES 1           /* don't delete more than this many records */

/* field numbers hardcoded into build and qi */

#define NAME_FIELD     "01"     /* 'name' field */
#define ALIAS_FIELD    "02"     /* 'alias' field */
#define NICKNAME_FIELD "03"     /* 'nickname' field */
#define EMAIL_FIELD    "04"     /* email address field */
#define SOUNDEX_FIELD  "05"     /* soundex value field */
#define EXPORT_FIELD   "06"     /* export email field */
#define TYPE_FIELD     "90"     /* entry type field */
#define HERO_FIELD     "91"     /* 'hero' field */
#define PROXY_FIELD    "92"     /* proxy owner field */
#define PASSWORD_FIELD "93"     /* login password field */
#define HOLDING_FIELD  "94"     /* holding proxies field */
#define CONTROL_FIELD  "95"     /* control ID number field */
#define ID_FIELD       "ID"     /* dummy string to indicate id field */

#define MAX_INPUT 2048          /* max size of protocol command */

#define LOCAL_DOMAIN_NAME "CSO_DOMAIN" /* logical name for local domain */
#define SITEINFO_NAME "CSO_SITEINFO"   /* logical name for siteinfo file */
#define CONFIG_NAME "CSO_CONFIG"       /* logical name for config file */
#define INDEX_NAME "CSO_INDEX"         /* logical name for index file */
#define DATA_NAME "CSO_DATA"           /* logical name for data file */
#define LOG_NAME "CSO_LOG"             /* logical name for log file */
#define HELP_LIB "CSO_HELPLIB"         /* logical name for help file directory */
#define MAILDOMAIN_NAME "CSO_MAILDOMAIN" /* logical name for mail domain */

/* .cnf file structure definition */

typedef struct {
    char *number;
    char *name;
    char *desc;
    long attrib;                /* maximum of <bits in long> attributes */
} Fields;

/* field attributes */

#define ATTR_INDEXED     1
#define ATTR_LOOKUP      2
#define ATTR_PUBLIC      4
#define ATTR_FORCEPUB    8
#define ATTR_DEFAULT    16
#define ATTR_UNIQUE     32
#define ATTR_ENCRYPTED  64
#define ATTR_LOCALPUB  128
#define ATTR_CHANGE    256
#define ATTR_URL       512
#define ATTR_EMAIL    1024
#define ATTR_IMAGE    2048

#ifdef define_attributes
struct attr_struct {
    char *name;
    int value;
} attributes[] = {{"Indexed", ATTR_INDEXED},
                  {"Lookup", ATTR_LOOKUP},
                  {"Public", ATTR_PUBLIC},
                  {"Forcepub", ATTR_FORCEPUB},
                  {"Default", ATTR_DEFAULT},
                  {"Unique", ATTR_UNIQUE},
                  {"Encrypted", ATTR_ENCRYPTED},
                  {"Localpub", ATTR_LOCALPUB},
                  {"Change", ATTR_CHANGE},
                  {"URL", ATTR_URL},
                  {"Email", ATTR_EMAIL},
                  {"Image", ATTR_IMAGE}};

#define MAX_ATTRIBUTES (sizeof(attributes) / sizeof(struct attr_struct))
#endif

/* default mode of server */
/* (soundex was confusing people) */
#define DEFAULT_MODE WILD_MODE + LOG_MODE + APPROX_MODE

/* 'set' command definitions */
#define APPROX_MODE  1   /* perform approximate match if exact match fails */
#define SOUNDEX_MODE 2   /* perform soundex match if exact and approx fail */
#define DEBUG_MODE   4   /* print debugging information */
#define LOG_MODE     8   /* log activity */
#define RECORD_MODE 16   /* log responses */
#define WILD_MODE   32   /* match any part of non-indexed fields */
#define REMOTE_MODE 64   /* treat the client as remote even if it isn't */

#ifdef define_modes
struct mode_struct {
    char *name;
    unsigned int value;
    enum {and, or} func;
} modes[] = {{"approximate", APPROX_MODE, or},
             {"soundex", SOUNDEX_MODE, or},
             {"debug", DEBUG_MODE, or},
/*           {"log", LOG_MODE, or},         let's not allow users to do this */
             {"record", RECORD_MODE, or},
             {"wild", WILD_MODE, or},
             {"exact",  ~(APPROX_MODE | SOUNDEX_MODE), and},
             {"remote", REMOTE_MODE, or}};

#define MAX_MODES (sizeof(modes) / sizeof(struct mode_struct))
#endif

/*
 * OK, embedding a local variable (ctx) in a macro is not too cool,
 * but 'mode' was global and these are used all over the place
 */
#define APPROX (ctx->mode & APPROX_MODE)
#define SOUNDEX (ctx->mode & SOUNDEX_MODE)
#define DEBUG (ctx->mode & DEBUG_MODE)

/* single data record attributes (encoded '0'-'7') in data record */
#define FIELD_ATTR_CHANGED  1     /* record changed by user */
#define FIELD_ATTR_CLEAR  128     /* value to clear the 'changed' bit */  	

#define FIELD_ATTR_MASK     6     /* mask for next 4 attributes */
#define FIELD_ATTR_NONE     0     /* use default field attributes */
#define FIELD_ATTR_SUPPRESS 2     /* record not public */
#define FIELD_ATTR_PUBLIC   4     /* record forced public */
#define FIELD_ATTR_LOCAL    6     /* record local only */

#ifdef define_field_attributes
struct field_attr_struct {
    char *name;
    int mask;
    int value;
} field_attributes[] = {{"Suppress", FIELD_ATTR_MASK, FIELD_ATTR_SUPPRESS},
                        {"Public", FIELD_ATTR_MASK, FIELD_ATTR_PUBLIC},
                        {"Local", FIELD_ATTR_MASK, FIELD_ATTR_LOCAL},
                        {"None", FIELD_ATTR_MASK, FIELD_ATTR_NONE},
                        {"Default", FIELD_ATTR_MASK, FIELD_ATTR_NONE},   /* alias for none */
                        {"Unchanged", FIELD_ATTR_CHANGED, FIELD_ATTR_CLEAR},
                        {"Clear", FIELD_ATTR_CHANGED, FIELD_ATTR_CLEAR}};  /* alias for unchanged */

#define MAX_FIELD_ATTRIBUTES (sizeof(field_attributes) / sizeof(struct field_attr_struct))
#endif

#define True  1
#define False 0

#define SOUNDEX_SIZE 4  /* only 4 characters may generate too many matches */

/* command line data struct */

typedef struct astruct {
    int element;
    int type;
    char *name;
    int field;
    char *value;
    struct astruct *prev;
    struct astruct *next;
} Arg;

/* arg type field definitions */
#define TYPE_NAME   1
#define TYPE_VALUE  2
#define TYPE_EQUAL  4
#define TYPE_RETURN 8          /* query ... return ... */
#define TYPE_ON    16          /* set ... = on */
#define TYPE_OFF   32          /* set ... = off */
#define TYPE_MAKE  64          /* change ... make ... */
#define TYPE_SET  128          /* change ... set ... */
#define TYPE_MASK (TYPE_ON + TYPE_OFF)

#define NAME_HACK 1  /* mangle names a bit: (1) compress out apostrophes */
                     /* (2) index both parts of a hyphenated name, */
                     /* i.e. cartwright-chickenwood is also indexed as */
                     /* cartwright and chickenwood */

#define INDEX_FULL_NAME 1	/* write an index for the full name as well */
				/* as each word of the name */

#define EXEC_LOGICALS 1  /* force all logical names to be exec mode */

#define MAX_BAD 10   /* number of bad or blank commands to cause a shutdown */

/* modes defined for login_mode */

#define MODE_ANONYMOUS 1  /* anonymous mode (default) */
#define MODE_LOGIN     2  /* logged in to an alias */
#define MODE_PASSWORD  4  /* waiting for response to challenge */

#define CHALLENGE_SIZE 40 /* size of challenge string */

typedef struct rnode {
    char *message;
    struct rnode *next;
} resp_node;

#define RESP_ONCE    0    /* only respond with the first status message */
#define RESP_MULT    1    /* always respond with this status message */
#define RESP_LITERAL 2    /* do not interpolate string */
#define RESP_RESET   3    /* flush out the status messages and reset */

#include <rms.h>                /* RAB, FAB referenced here */
#include <descrip.h>            /* dsc$descriptor_a referenced here */
#include <setjmp.h>		/* jmp_buf referenced */

/* define a one dimension array descriptor */
typedef struct {
    struct dsc$descriptor_a a;  /* the array */
    struct {
        struct dsc$descriptor_d *dsc$a_a0;
        long dsc$l_m;
    } m;                /* multiplier */
    struct {
        long dsc$l_l;
        long dsc$l_u;
    } b;                /* bounds */
} onedim;

/* context of server instance */
typedef struct {
    int mode;             /* global mode flags should be first */
    int login_mode;       /* logged in or not */
    int db_status;        /* status of database */
    int interactive;      /* interactive or not */
    int socket;           /* network socket */
    int login_id;         /* id of logged in entry */
    int next_id;          /* id returned by 'select' command */
    int bad_cmd;          /* number of bad commands in a row */
    int keyword_size;     /* size of index file keyword field */
    int field_size;       /* size of index file field number field */
    int id_size;          /* size of index file id field */
    resp_node *root;      /* pending responses */
    onedim *results;      /* API result storage */
    Fields fields[MAX_FIELD];  /* field attributes */
    jmp_buf env;          /* longjump buffer */
    struct FAB idxfab, datfab;
    struct RAB idxrab, datrab;
    struct XABKEY idxxab, idxxab2;
    char idx_record[IDX_RECORD_SIZE + 1];
    char idx_key[IDX_KEY_SIZE + 1];
    char dat_record[DAT_RECORD_SIZE + 1];
    char dat_key[DAT_KEY_SIZE + 1];
    char login_alias[KEYWORD_SIZE + 1];  /* current login */
    char login_challenge[CHALLENGE_SIZE + 1];
} context;

#define NULLCTX ((context *) 0)

#define NotRet(x) ((x->type & (TYPE_RETURN | TYPE_MAKE | TYPE_SET)) == 0)


#define EOL  "\n"  /* "\r\n" for telnet, "\n" for TurboGopher */

/*
 * ALIAS_FIELD of an entry that has a CONTROL_FIELD; this control field
 * will be used automatically as the 'select' command argument when no entry
 * has been 'select'ed
 */
#define DEFAULT_SELECT "control"

/*
 * It appears that some clients expect all the fields to be known and
 * need this turned off.  If you don't care and don't want any knowledge
 * of non-public or non-localpub fields to get out, turn this on.
 */
#define FIELD_PARANOIA 0

/* Do we want to force lowercase field names and descriptions,
 * or leave them alone? 
 */
#define FIELD_NAME_LC 0   /* 0 = leave alone, 1 = force lowercase */

/* Site info - everything else is in CSO_SITEINFO */
#define MAIL_FIELD  ALIAS_FIELD     /* field that has public name */
#define MAIL_BOX    EMAIL_FIELD     /* field that has real email address */

#define VERSION "VMS qi V3.09"

