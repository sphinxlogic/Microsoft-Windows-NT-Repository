/* Application Programming Interface to the nameserver */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <starlet.h>
#include <ssdef.h>
#include <lib$routines.h>
#include <jpidef.h>
#include <descrip.h>
#include <setjmp.h>
#include "qi.h"

int process(context *, char *);
void free_results(onedim **);

extern void db_open(context *);
extern void db_close(context *);
extern int read_fields(char *, context *);
extern void free_fields(context *);
extern int fields_cmd(char *, context *);
extern char *getlogical(context *, char *);
extern void qilog(context *, int, char *, ...);
extern int query_cmd(char *, context *);
extern int delete_cmd(char *, context *);
extern int change_cmd(char *, context *);
extern int add_cmd(char *, context *);
extern int quit(char *, context *);
extern void respond(context *, int, char *, ...);
extern int id_cmd(char *, context *);
extern int stat_cmd(char *, context *);
extern int set_cmd(char *, context *);
extern int site_cmd(char *, context *);
extern int help_cmd(char *, context *);
extern int login_cmd(char *, context *);
extern int answer_cmd(char *, context *);
extern int clear_cmd(char *, context *);
extern int logout_cmd(char *, context *);
extern int select_cmd(char *,context *);
extern void ZapCRLF(char *);
extern int strcasecmp(char *, char *);
extern context *create_context();


struct verb_struct {
    char *name;
    int mode; /* requires login, etc */
    int (*proc)(char *, context *);
} verbs[] = {
             {"add", MODE_LOGIN, add_cmd},
             {"answer", MODE_PASSWORD, answer_cmd},
             {"change", MODE_LOGIN, change_cmd},
             {"clear", MODE_PASSWORD, clear_cmd},
             {"delete", MODE_LOGIN, delete_cmd},
             {"exit", MODE_ANONYMOUS | MODE_LOGIN, quit},
             {"fields", MODE_ANONYMOUS | MODE_LOGIN, fields_cmd},
             {"help", MODE_ANONYMOUS | MODE_LOGIN, help_cmd},
             {"id", MODE_ANONYMOUS | MODE_LOGIN, id_cmd},
             {"login", MODE_ANONYMOUS | MODE_LOGIN, login_cmd},
             {"logout", MODE_LOGIN, logout_cmd},
             {"ph", MODE_ANONYMOUS | MODE_LOGIN, query_cmd},
             {"query", MODE_ANONYMOUS | MODE_LOGIN, query_cmd},
             {"quit", MODE_ANONYMOUS | MODE_LOGIN, quit},
             {"select", MODE_LOGIN, select_cmd},
             {"set", MODE_ANONYMOUS | MODE_LOGIN, set_cmd},
             {"siteinfo", MODE_ANONYMOUS | MODE_LOGIN, site_cmd},
             {"status", MODE_ANONYMOUS | MODE_LOGIN, stat_cmd},
             {"stop", MODE_ANONYMOUS | MODE_LOGIN, quit},
            };

#define MAX_VERBS (sizeof(verbs) / sizeof(struct verb_struct))

typedef struct dsc$descriptor_s strdsc;


/* do the command in cmddsc; return result in result */
int qi_command(context **ctxp, strdsc *cmddsc, onedim **result)
{
    char *ln, *str, cmd[200];
    int status, jump_status;
    short length;

    if ((ctxp == (context **) 0) || (cmddsc == (strdsc *) 0) ||
        (result == (onedim **) 0))
        return SS$_ACCVIO;  /* need valid pointers */

    if (*ctxp == NULLCTX) {  /* null context, make one */
        *ctxp = create_context();
        (*ctxp)->mode &= ~LOG_MODE;  /* disable logging */

        if (setjmp((*ctxp)->env)) {   /* set up the longjmp buffer */
            free(ln);
            free(*ctxp);
            *ctxp = NULLCTX;
            return SS$_INSFMEM;
        }

        /* load fields */
        if (read_fields(ln=getlogical(*ctxp, CONFIG_NAME), *ctxp) == False) {
            free(ln);
            free(*ctxp);
            *ctxp = NULLCTX;
            return SS$_NODATA;
        }
        free(ln);

        /* initialize randomness */
        srand((int) time(NULL));
        db_open(*ctxp);      /* open database */
    }
    if (*result != (onedim *) 0) {
        free_results(result);      /* free any residual results */
        (*ctxp)->results = (onedim *) 0;
    }
    status = lib$analyze_sdesc(cmddsc, &length, &str);
    if ((status & 1) != SS$_NORMAL)   /* bad descriptor */
        return status;
    if (length > (sizeof(cmd) - 1))   /* descriptor is too big */
        return SS$_BADPARAM;
    strncpy(cmd, str, length);        /* make a copy of the string */
    cmd[length] = '\0';               /* terminate the string */

    jump_status = setjmp((*ctxp)->env);    /* set up the longjmp buffer */

    if ((strlen(cmd) > 0) && (jump_status == 0)) {
        process(*ctxp, cmd);
        *result = (*ctxp)->results;
    }
    if ((strlen(cmd) == 0) || jump_status) {
        db_close(*ctxp);
        free_fields(*ctxp);
        free(*ctxp);
        *ctxp = NULLCTX;
        if (jump_status)  /* came from memory failure */
            return SS$_INSFMEM;
    }
    return SS$_NORMAL;
}


/* process a command stream */

int process(context *ctx, char *cmd)
{
    int status, ind, length;
    char *cp, verb[MAX_INPUT];

    ZapCRLF(cmd);
    qilog(ctx, False, "Cmd: %s", cmd);

    /* put first word of input in verb as lowercase */
    strcpy(verb, cmd);
    cp = strchr(verb, ' ');
    if (cp) *cp = '\0';

    for (ind = 0; ind < MAX_VERBS; ind++)
        if (strcasecmp(verb, verbs[ind].name) == 0)
            break;

    if (ind == MAX_VERBS) {
        qilog(ctx, False, "Unknown command: %s from %s", verb, cmd);
        respond(ctx, RESP_ONCE, "514:Unknown command.");
        respond(ctx, RESP_RESET, ""); /* flush status messages */
        return True;
    }

    if (((verbs[ind].mode & ctx->login_mode) == 0) &&
        (ctx->login_mode == MODE_ANONYMOUS)) {
        qilog(ctx, False, "Not logged in: %s", cmd);
        respond(ctx, RESP_ONCE, "506:Request refused; must be logged in to execute.");
        respond(ctx, RESP_RESET, ""); /* flush status messages */
        return True;
    }

    if (((verbs[ind].mode & ctx->login_mode) == 0) &&
        (ctx->login_mode == MODE_PASSWORD)) {
        qilog(ctx, False, "Not answer or clear: %s", cmd);
        respond(ctx, RESP_ONCE, "523:Expecting 'answer' or 'clear'");
        respond(ctx, RESP_RESET, ""); /* flush status messages */
        return True;
    }

    if ((ind < MAX_VERBS) && (verbs[ind].mode & ctx->login_mode))
            status = (*verbs[ind].proc)(cmd, ctx);

    respond(ctx, RESP_RESET, "");  /* flush status messages */
    return status;
}


void free_results(onedim **result)
{
    struct dsc$descriptor_d *elem;
    int ind, count = 0;

    for (ind = 0; ind < (*result)->m.dsc$l_m; ind++) {
        elem = (*result)->m.dsc$a_a0 + ind;
        cfree(elem->dsc$a_pointer);
    }
    free((*result)->m.dsc$a_a0);
    free(*result);
    *result = (onedim *) 0;
}


/* dummy network routines */
void writestring(int a, char *b) {}

void inet_netnames(int a, char *b, char *c)
{
    int pid, jpiarg = JPI$_PID;

    lib$getjpi(&jpiarg, 0, 0, &pid, 0, 0);

    sprintf(b, "API %X", pid);
}
