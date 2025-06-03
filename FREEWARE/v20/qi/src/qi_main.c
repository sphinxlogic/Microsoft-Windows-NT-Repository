/* qi_main.c - UIUC CCSO nameserver query interpreter */
/* Bruce Tanner - Cerritos College */

/*

Version history:

1.0  1993/08/15 Initial version
1.1  1993/08/25 Add field instance attribute, conditionalize options
1.2  1993/09/08 Soundex is now an indexed explicit field; exact match mode
1.3  1993/09/15 Added ID to index key to remove duplicate index records
2.0  1993/09/16 Add login mode (login, answer, clear, logout)
2.1  1993/09/20 Interactive mode
2.2  1993/11/01 Allow response terminator to be set to \n for TurboGopher
2.3  1993/11/06 Add LocalPub field attribute
2.4  1994/03/29 Add change command, delete command
2.5  1994/04/13 Show field based on Localpub, fix error responses
2.6  1994/04/20 New index key, fix delete_entry(), fix process(), add Help
2.7  1994/05/27 Fix problems with change of unique field
2.8  1994/07/31 Reorganize modules, minor mods for phquery
2.9  1994/08/27 Make validate_match() more stringent
2.10 1994/09/11 Fix problems with query '*'
2.11 1994/10/05 Allow UCX interactive, fix query_cmd accvio
2.12 1994/10/17 Fix problem with encrypted passwords being mangled
3.00 1994/11/23 Add 'select' and 'add' commands
3.01 1994/12/02 Optimize wildcard query clauses, new field default
3.02 1994/12/16 Make most routines reentrant and maintain a context structure
3.03 1994/12/28 Define qi_command API
3.04 1995/01/17 Fix broken decrypt_field, calculate field sizes from database
3.05 1995/01/23 Free memory in respond(), make phquery use exact mode
3.06 1995/03/05 Fix problems with locked records, add per-field attributes
3.07 1995/05/01 Add URL attribute, add remote mode
3.08 1995/07/25 Add Image attribute, netlib support, fix help accvio
3.09 1995/10/07 Add Email attribute, catch malloc failure, missing alias accvio
*/

#include <stdio.h>
#include <stdlib.h>
#include <types.h>
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
#if defined(UCX)
#include <ucx$inetdef.h>
#endif
#ifdef NETLIB
#include <netlib_dir/netlibdef.h>
#endif
#include "qi.h"

int process(context *);

extern void db_open(context *);
extern void db_close(context *);
extern int read_fields(char *, context *);
extern void free_fields(context *);
extern int fields_cmd(char *, context *);
extern char *getlogical(context *, char *);
extern void qilog(context *, int, char *, ...);
extern void respond(context *, int, char *, ...);
extern int query_cmd(char *, context *);
extern int delete_cmd(char *, context *);
extern int change_cmd(char *, context *);
extern int add_cmd(char *, context *);
extern int quit(char *, context *);
extern int id_cmd(char *, context *);
extern int stat_cmd(char *, context *);
extern int set_cmd(char *, context *);
extern int site_cmd(char *, context *);
extern int help_cmd(char *, context *);
extern int login_cmd(char *, context *);
extern int answer_cmd(char *, context *);
extern int clear_cmd(char *, context *);
extern int logout_cmd(char *, context *);
extern int select_cmd(char *, context *);
extern int closenet(int);
extern int readline(int, char *, int);
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
             {"q", MODE_ANONYMOUS | MODE_LOGIN, quit},
             {"f", MODE_ANONYMOUS | MODE_LOGIN, quit}
            };

#define MAX_VERBS (sizeof(verbs) / sizeof(struct verb_struct))



/* This program is designed to run as an 'inetd' detached process */
/* But it will also work interactively */
main()
{
    int status, jump_status, proc_mode;
    char *ln;
    context *ctx;
#if defined(MULTINET) || defined(WOLLONGONG)
    unsigned short chan;
    $DESCRIPTOR(sysin_dsc, "SYS$INPUT");
#endif
#ifdef NETLIB
    unsigned short NS_PORT = 105;
    struct SINDEF server_sin;
    unsigned int sinsize = sizeof(struct SINDEF);
#endif /* netlib */

    ctx = create_context();
    status = lib$getjpi(&JPI$_MODE, 0, 0, &proc_mode, 0, 0);
    if ((status & 1) != SS$_NORMAL)
        exit(status);
    ctx->interactive = (proc_mode == JPI$K_INTERACTIVE);

    if (!ctx->interactive) {

#if defined(MULTINET) || defined(WOLLONGONG)
        /* open a channel to an INET device */
        status = sys$assign(&sysin_dsc, &chan, 0, 0, 0);
        if ((status & 1) != SS$_NORMAL) {
            qilog(NULLCTX, False, "Open fail status = %d", status);
            exit(status);
        }
        ctx->socket = (int) chan;
#endif /* MultiNet or Wollongong */

#if defined(UCX)
        /* Try to get the client connection from the AUX server */
        ctx->socket = socket(UCX$C_AUXS, 0, 0);
        if (ctx->socket == -1) {
            qilog(NULLCTX, False, "UCX socket error %X", errno);
            exit(errno);
        }
#endif /* UCX v2+ */

#if defined(NETLIB)
        memset(&server_sin, 0, sinsize);
        server_sin.sin_w_port = netlib_hton_word(&NS_PORT);

        status = netlib_server_setup((void *) &ctx->socket, &server_sin, &sinsize);
        if ((status & 1) != SS$_NORMAL) {
            qilog(NULLCTX, False, "Server setup fail status = %d", status);
            exit(status);
        }
#endif /* NETLIB */

    } /* not interactive */

    if (setjmp(ctx->env)) {   /* set up the longjmp buffer */
        printf("Memory allocation failure");
        exit(6);
    }

    /* load fields */
    if (read_fields(ln=getlogical(ctx, CONFIG_NAME), ctx) == False) {
        free(ln);
        printf("Config file (%s) can't be read\n", CONFIG_NAME);
        exit(4);
    }
    free(ln);

    /* initialize randomness */
    srand((int) time(NULL));

    jump_status = setjmp(ctx->env);    /* set up the longjmp buffer */
    if (jump_status == 0) {
        db_open(ctx);
        while (process(ctx));
    }

    if (!ctx->interactive)
        closenet(ctx->socket);
    db_close(ctx);
    free_fields(ctx);
    free(ctx);
}


/* process a command stream */

int process(context *ctx)
{
    int status, ind, length;
    char *cp, inputline[MAX_INPUT], verb[MAX_INPUT];

    strncpy(inputline, "", MAX_INPUT);
    ctx->bad_cmd = 0;
    if (ctx->interactive) {
        printf("qi> ");
        fgets(inputline, MAX_INPUT, stdin);
        length = strlen(inputline);
    }
    else
        length = readline(ctx->socket, inputline, MAX_INPUT); /** Get the line **/
    ZapCRLF(inputline);
    if (strlen(inputline) > 0)
        qilog(ctx, False, "Cmd: %s", inputline);

    if (length <= 0) {
        qilog(ctx, False, "Remote end shutdown");
        return False;
    }

    if (strlen(inputline) == 0)  /* ignore blank lines */
        return (++ctx->bad_cmd < MAX_BAD);  /* return False if too many null cmds */

    /* put first word of input in verb as lowercase */
    strcpy(verb, inputline);
    cp = strchr(verb, ' ');
    if (cp) *cp = '\0';

    for (ind = 0; ind < MAX_VERBS; ind++)
        if (strcasecmp(verb, verbs[ind].name) == 0)
            break;

    if (ind == MAX_VERBS) {
        qilog(ctx, False, "Unknown command: %s from %s", verb, inputline);
        respond(ctx, RESP_ONCE, "514:Unknown command.");
        respond(ctx, RESP_RESET, ""); /* flush status messages */
        return (++ctx->bad_cmd < MAX_BAD);  /* return False if too many bad cmds */
    }

    if (((verbs[ind].mode & ctx->login_mode) == 0) &&
        (ctx->login_mode == MODE_ANONYMOUS)) {
        qilog(ctx, False, "Not logged in: %s", inputline);
        respond(ctx, RESP_ONCE, "506:Request refused; must be logged in to execute.");
        respond(ctx, RESP_RESET, ""); /* flush status messages */
        return (++ctx->bad_cmd < MAX_BAD);  /* return False if too many bad cmds */
    }

    if (((verbs[ind].mode & ctx->login_mode) == 0) &&
        (ctx->login_mode == MODE_PASSWORD)) {
        qilog(ctx, False, "Not answer or clear: %s", inputline);
        respond(ctx, RESP_ONCE, "523:Expecting 'answer' or 'clear'");
        respond(ctx, RESP_RESET, ""); /* flush status messages */
        return (++ctx->bad_cmd < MAX_BAD);  /* return False if too many bad cmds */
    }

    if ((ind < MAX_VERBS) && (verbs[ind].mode & ctx->login_mode))
            status = (*verbs[ind].proc)(inputline, ctx);

    respond(ctx, RESP_RESET, "");  /* flush status messages */
    return status;
}
