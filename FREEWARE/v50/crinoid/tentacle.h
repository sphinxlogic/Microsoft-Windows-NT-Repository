/*
 *  definitions for TENTACLE
 *
 */
#ifndef __TENTACLE_H
#define __TENTACLE_H

#include "vms_data.h"
#include "CRINOID_types.h"


typedef struct SCRIPT {
    char * script;
    PerlInterpreter *perl;
    time_t time_last;
    struct SCRIPT *next;
    struct SCRIPT *last;
} *pSCRIPT;


pENV        add_ENV(char *name, char *value, int state);
pENV        find_ENV(char *name);
void        clear_ENV(void);
void        init_ENV(void);
void        dump_ENV(FILE *fp);
int         construct_perlENV(void);
void        send_perlENV(char *name, char *value);
int         init_perl(void);
int         exec_perl(void);
int         send_status(word code, word unit, char *s, int n);
int         queue_read(word chan);
int         handle_connection();
void        status_AST(pMbxE e);
void        command_AST(pMbxE e);
void        do_errlog(pMbxE e);
void        do_loglev(pMbxE e);
void        do_envdata(pMbxE e);
void        upcase(char *s);
void        heartbeat_AST(void);
void        fake_netshut(void);

#endif
