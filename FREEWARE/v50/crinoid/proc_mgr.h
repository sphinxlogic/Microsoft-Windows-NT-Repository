/*
 *
 *  include file for worker process management interface
 *
 */

#ifndef __PROC_MGR_H
#define __PROC_MGR_H

#include "vms_data.h"
#include "CRINOID_types.h"

void    init_Proc(pMBX term_mbx, char* stub);
pProc   new_Proc(pGroup s);
void    start_Proc(pProc p);
void    link_Proc(pProc p);
void    unlink_Proc(pProc p);
pProc   get_Proc(longword pid);
void    destroy_Proc(pProc p);
void    lock_Proc(pProc p);
void    unlock_Proc(pProc p);
longword *  list_Proc(void);
void    stop_Proc(pProc p);
pProc   getidle_Proc(pGroup g);
void    setidle_Proc(pProc p);
void    lock_Group(pGroup g);
void    unlock_Group(pGroup g);
pGroup  new_Group(char *name);
void    setuser_Group(pGroup g, char *user);
void    link_Group(pGroup g);
void    destroy_Group(pGroup g);
void    init_Group(char *program, char *input, char *output, char *error);
void    check_Proc();
void*   manage_Proc(void *arg);
void    init_manage_Proc();
pGroup  find_Group(longword uic);
pGroup  find_named_Group(char *name);
pGroup  find_username_Group(char *name);
void    kill_all_Procs();
void    stop_manage_Proc();
void    exit_handle_Proc(void);
void    printall_Group(FILE *fd);
void    print_Group(pGroup g, FILE *fd);

#endif
