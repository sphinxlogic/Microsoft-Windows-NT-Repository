/************************************************************************\
 *                                                                       *
 *      ROUTINES FOR DEALING WITH SCRIPTS/DIRECTORIES, ETC.              *
 *                                                                       *
 *                                                                       *
 *                                                                       *
 *                                                                       *
 *                                                                       *
\************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <ssdef.h>
#include <starlet.h>
#include <lib$routines.h>
#include <str$routines.h>
#include <strdef.h>

#include "crinoid_types.h"
#include "util.h"
#include "script.h"
#include "errlog_client.h"

static pScript          ScriptHead = 0;
static pScript          ScriptTail = 0;
static pthread_once_t   ScriptOnce = pthread_once_init;
static pthread_mutex_t  ScriptMutex;


/* leave here...private definition */
static void   init_Script(void);


pScript
new_Script(char *wild, int type, char *user)
{
    ROUTINE_NAME("new_Script");
    int iss;
    pScript s;

    s = malloc(sizeof(Script));
    if (!s) VMS_ABORT("new_Script, malloc fail",SS$_INSFMEM);

    s->type = type;
    s->wild = new_STRING(wild);
    uc(asciz_pSTRING(s->wild));
    s->user = user ? new_STRING(user) : 0;
    s->server = 0;
    s->bindir = 0;
    s->next = 0;
    s->flags = 0;
    s->permit = 0;
    s->deny = 0;
    s->warn = 0;

    iss = pthread_once(&ScriptOnce, &init_Script);
    if (UNIX_ERR(iss)) UNIX_ABORT("add_Script: pthread_once");

    iss = pthread_mutex_lock(&ScriptMutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("add_Script: pthread_mutex_lock");

    if (ScriptTail)
        ScriptTail->next = s;

    ScriptTail = s;

    if (!ScriptHead)
        ScriptHead = s;

    iss = pthread_mutex_unlock(&ScriptMutex);
    if (UNIX_ERR(iss)) UNIX_ABORT("add_Script: pthread_mutex_unlock");

    return s;
}

static void
init_Script(void)
{
    ROUTINE_NAME("init_Script");
    int iss;
    iss = pthread_mutex_init(&ScriptMutex,pthread_mutexattr_default);
    if (UNIX_ERR(iss)) UNIX_ABORT("init_Script, pthread_mutex_init");
}


pScript
findwild_Script(char *in, char *server)
{
    ROUTINE_NAME("findwild_Script");
    int iss;
    pScript s = ScriptHead;
    pSTRING in2, ins;

    in2 = new_STRING(in);
    uc(asciz_pSTRING(in2));
    ins = new_STRING(server);
    uc(asciz_pSTRING(ins));

    while (s) {
        if (!s->server || tu_strcmp(asciz_pSTRING(s->server),asciz_pSTRING(ins)) == 0) {
            pthread_lock_global_np();
            iss = str$match_wild(in2, s->wild);
            pthread_unlock_global_np();
            if (iss == STR$_MATCH) break;
        }
        s = s->next;
    }
    destroy_STRING(in2);
    destroy_STRING(ins);
    return s;
}




