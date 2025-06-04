/*
 *  definitions for CRINOID
 *
 */
#ifndef __CRINOID_H
#define __CRINOID_H

#include "vms_data.h"
#include "util.h"
#include "CRINOID_types.h"
#include "mbxq.h"


int     do_online(pMbxE e);
void    do_offline(pMbxE e);
void    do_idle(pMbxE e);
int     send_message(pMbxQ q, word code, word unit, void *s, int n);
void    messageAST(pMbxE e);
void    init_threads();
void    stop_threads();
void *  thread_code(void *pNum);
void    thread_handle(pMbxE e);
void    do_connect(pTCB tc, pMbxE e);
pGroup  net_select_group();
void    net_sendENV(pProc p);
pNDB    net_connect(pMbxE e);
void    net_disconnect(pNDB);
void    net_readENV();
void    clean_TCB(pTCB tc);
pTCB    destroy_TCB(pTCB tc);
pTCB    new_TCB(int ThreadID);
void    destroy_ENV(pENV e);
pENV *  get_ENVBase();
void    new_ENV(void);
pENV    add_ENV(char *name, char *value, int state);
pENV    find_ENV(char *name);
void    get_main_params(void);
void    get_id2_params(void);
void    get_path_params(void);
void    get_header_params(void);
void    get_input(void);
pTCB    get_current_TCB();
int     netread(char *s, int lmax);
void    netwrite2(pMbxQ q, char *s);
void    netwrite(char *s);
pSTRING fetch_value(char *tag);
char *  pop_token(char **s);
char * net_unescape_string ( char *string, int *length );
void   net_runscript(pProc p);
void   load_translation ( char *name, char *tag, char *arg );
void   destroy_TCB2(void *tc);
void   do_disconnect(pMbxE e);
int    local_script(void);
pENV   parse_input(pSTRING s);
static void Exit_handler(void);
int    send_logical(pProc p, char *name);
int    do_heartbeat(pMbxE e);
int    do_stubstart(pMbxE e);
int    do_startpid(pMbxE e) ;
void   shutdown_server(void);
pSTRING extract_user(char *input, char *prefix);
void    movedown_level(void);
void    broadcast_loglevel(int level);
void    StartLogger(void);
void    StartLogger_queue_lock(void);
void    StartLogger_AST(int flag);
void    StartLoggerProc(void);
void    StopLogger(void);
void    NewLog(void);
int     ReadMainConfig (char *file);
void    do_pendabort(pMbxE e);
void    delay(int secs);
pNDB    destroy_NDB(pNDB ndb);
void    queue_ATE(word unit, int type, int ticks);
void    dequeue_ATE(word unit, int type);
void*   ATE_processor(void *pNum);
void    init_ATE_Proc(void);
void    stop_ATE_Proc(void);
pSTRING url_remainder(char *input, char *prefix, int usermode);
void    queue_special(word code, word unit);

#endif
