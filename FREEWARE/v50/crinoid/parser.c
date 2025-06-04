 /*
        subroutines to parse input lines
*/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"
#include "util.h"

pVerb
new_Verb(char *verb, int id,  int (*handler)(pVerb v, char **args, char *errmsg), pVerb vhead)
{
    ROUTINE_NAME("new_Verb");
    pVerb v;

    v = malloc(sizeof(Verb));
    v->next = vhead;
    v->nunique = 1;
    v->id  = id;
    v->verb = malloc(strlen(verb)+1);
    strcpy(v->verb, verb);
    v->handler = handler;
    return v;
}

void
uniquify_Verb(pVerb v)
{
    ROUTINE_NAME("uniquify_Verb");
    pVerb w;
    char *s1, *s2;
    int n;

    while (v) {
        w = v->next;
        while (w) {
            s1 = v->verb;
            s2 = w->verb;
            n = 1;
            while (*s1 && *s2 && tolower(*s1) == tolower(*s2)) {
                s1++;
                s2++;
                n++;
            }
            v->nunique = MAX(v->nunique, n);
            w->nunique = MAX(w->nunique, n);
            w = w->next;
        }
        v = v->next;
    }
}


int
parse_line(pVerb vhead, char *s, char *errmsg)
{
    ROUTINE_NAME("parse_line");
    pVerb v;
    char **args;
    int maxargs = 16;
    int n, j, status, nargs = 0;

    args = (char **) malloc((maxargs+1)*sizeof(char *));
    if (!args) {
        strcpy(errmsg,"parse_line: malloc fail for args");
        return 0;
    }

    while (1) {
        while (*s && isspace(*s)) s++;
        if (!*s || *s == '#') break;

        if (nargs == maxargs) {
            char **args2;
            int j;

            args2 = (char **) malloc((2*maxargs+1)*sizeof(char *));
            if (!args2) {
                strcpy(errmsg,"parse_line: malloc fail for args");
                for (j = 0; j < nargs; j++) free(args[j]);
                free(args);
                return 0;
            }
            for (j = 0; j < nargs; j++) {
                args2[j] = args[j];
            }
            free(args);
            args = args2;
            maxargs = 2*maxargs;
        }
        if (*s == '"') {
            args[nargs] = destringify(&s, errmsg,&n);
            if (!args[nargs]) {
                    for (j = 0; j < nargs; j++) free(args[j]);
                    free(args);
                    return 0;
            }
            nargs++;
        } else {
            char *p = s;
            while (*p && !isspace(*p)) p++;
            args[nargs] = malloc(p-s+1);
            if (!args[nargs]) {
                strcpy(errmsg,"parse_line: malloc fail for argument");
                for (j = 0; j < nargs; j++) free(args[j]);
                free(args);
                return 0;
            }
            strncpy(args[nargs],s,p-s);
            args[nargs][p-s] = '\0';
            s = *p? p + 1 : p;
            nargs++;
        }
    }

    args[nargs] = 0;
    /* find verb */

    if (!args[0]) {
        free(args);
        return 1;
    }

    v = vhead;
    while (v) {
        char *s1, *s2;
        int nmatch;

        s1 = v->verb;
        s2 = args[0];
        nmatch = 0;
        while (*s1 && *s2 && tolower(*s1) == tolower(*s2)) {
            nmatch++;
            s1++;
            s2++;
        }
        if (nmatch >= v->nunique && !*s2) break;
        v = v->next;
    }

    if (!v) {
        sprintf(errmsg,"parse_line: unknown verb '%s'",args[0]);
        for (j = 0; j < nargs; j++) free(args[j]);
        free(args);
        return 0;
    }

    status = (v->handler)(v,args,errmsg);

    for (j = 0; j < nargs; j++) free(args[j]);
    free(args);
    return status;
}


char *
destringify(char **s, char *errmsg, int *result)
{
    ROUTINE_NAME("destringify");
    char *p = *s+1, *q, *q0;
    int n = 0;

    *result = 0;
    while (*p && *p != '"') {
        if (*p == '\\' && *(p+1) == '"') p++;
        p++;
    }

    q0 = q = malloc(p-*s);
    if (!q) {
        strcpy(errmsg,"destringify: malloc failed");
        return 0;
    }

    p = *s+1;
    while (*p && *p != '"') {
        if (*p != '\\') {
            *q++ = *p++;
        } else {
            p++;
            switch (*p) {
                case '\r':
                case '\n':
                    p++;
                case '\0':
                    break;
                case '0':
                case '1':
                case '2':
                    n = *p++ - '0';
                    if (*p >= '0' && *p <= '7') {
                        n *= 8;
                        n += *p++ - '0';
                        if (*p >= '0' && *p <= '7') {
                            n *= 8;
                            n += *p++ - '0';
                        }
                    }
                    *q++ = n;
                    break;
                case 'r':
                    *q++ = '\r';
                    p++;
                    break;
                case 'n':
                    *q++ = '\n';
                    p++;
                    break;
                case 't':
                    *q++ = '\t';
                    p++;
                    break;
                case 'f':
                    *q++ = '\f';
                    p++;
                    break;
                default:
                    *q++ = *p++;
            }
        }
    }
    *result = (q - q0);
    *s = (*p == '"') ? p+1 : p;
    *q = '\0';
    return q0;
}


void
destroy_Verb(pVerb v)
{
    ROUTINE_NAME("destroy_Verb");
    pVerb v2;
    while (v) {
        if (v->verb) free(v->verb);
        v2 = v;
        v = v->next;
        free(v2);
    }
}
