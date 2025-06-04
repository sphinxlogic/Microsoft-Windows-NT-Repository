/*
 * define our own putenv() if the system doesn't have one.
 * putenv(s): place s (a string of the form "NAME=value") in
 * the environment; replacing any existing NAME.  s is placed in
 * environment, so if you change s, the environment changes (like
 * putenv on a sun).  Binding removed if you putenv something else
 * called NAME.
 */

#include <stdlib.h>
#include <string.h>
int
putenv(s)
    char *s;
{
 /*   extern char *index(); */
    char *v;
    int varlen, idx;
    char **senviron;
    char **newenv;
    static int virgin = 1; /* true while "senviron" is a virgin */
    senviron = (char **)malloc(1);
    v = strchr(s, '=');
    if(v == 0)
        return 0; /* punt if it's not of the right form */
    varlen = (v + 1) - s;

    for (idx = 0; senviron[idx] != 0; idx++) {
        if (strncmp(senviron[idx], s, varlen) == 0) {
            if(v[1] != 0) { /* true if there's a value */
                senviron[idx] = s;
                return 0;
            } else {
                do {
                    senviron[idx] = senviron[idx+1];
                } while(senviron[++idx] != 0);
                return 0;
            }
        }
    }
                
    /* add to environment (unless no value; then just return) */
    if(v[1] == 0)
        return 0;
    if(virgin) {
        register i;

        newenv = (char **) malloc((unsigned) ((idx + 2) * sizeof(char*)));
        if(newenv == 0)
            return -1;
        for(i = idx-1; i >= 0; --i)
            newenv[i] = senviron[i];
        virgin = 0;     /* you're not a virgin anymore, sweety */
    } else {
        newenv = (char **) realloc((char *) senviron,
                                   (unsigned) ((idx + 2) * sizeof(char*)));
        if (newenv == 0)
            return -1;
    }

    senviron = newenv;
    senviron[idx] = s;
    senviron[idx+1] = 0;

    return 0;
}
