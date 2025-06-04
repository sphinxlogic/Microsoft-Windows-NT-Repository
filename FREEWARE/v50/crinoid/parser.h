/*
        parser data structures, etc.
*/

#ifndef __PARSER_H
#define __PARSER_H

typedef struct PARSE_CMD_STRUCT   Verb;
typedef struct PARSE_CMD_STRUCT * pVerb;
typedef int VerbHandler(pVerb v, char **args, char *errmsg);
typedef int (* pVerbHandler)(pVerb v, char **args, char *errmsg);


struct PARSE_CMD_STRUCT {
        pVerb        next;
        int          nunique;
        int          id;
        char        *verb;
        pVerbHandler handler;
};

/*
        int (*handler)(pVerb v, char **args, char *errmsg);
        extern pVerb    new_Verb(char *verb, int (*handler)(pVerb v, char **args, char *errmsg), pVerb vhead);
*/

#define MAX(a,b)   ((a)>(b)?(a):(b))

extern pVerb    new_Verb(char *verb, int id, pVerbHandler handler, pVerb vhead);
extern void     uniquify_Verb(pVerb v);
extern int      parse_line(pVerb vhead, char *s, char *errmsg);
extern char *   destringify(char **s, char *errmsg, int *result);
extern void     destroy_Verb(pVerb v);
#endif
