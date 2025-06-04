#ifndef __MBXQ_H
#define __MBXQ_H

#include "vms_data.h"
#include "CRINOID_types.h"


#define MBXQ$_READ            0x00000000
#define MBXQ$_WRITE           0x00000001

#define MBXQ$V_DISCARD        1
#define MBXQ$V_RETRY          2
#define MBXQ$V_SEGMENT        3
#define MBXQ$V_NOSTART        4
#define MBXQ$V_NOLOG          5
#define MBXQ$V_NETMBX         6

#define MBXQ$M_DISCARD        (1<<MBXQ$V_DISCARD)
#define MBXQ$M_RETRY          (1<<MBXQ$V_RETRY  )
#define MBXQ$M_SEGMENT        (1<<MBXQ$V_SEGMENT)
#define MBXQ$M_NOSTART        (1<<MBXQ$V_NOSTART)
#define MBXQ$M_NOLOG          (1<<MBXQ$V_NOLOG)
#define MBXQ$M_NETMBX         (1<<MBXQ$V_NETMBX)

#ifdef __PRIVATE_MBXQ
#define MBXQ$V_MAXUSER        MBXQ$V_NETMBX
#define MBXQ$M_USERFIELD      ((1<<(MBXQ$V_MAXUSER+1))-1)

#define MBXQ$V_MBXCREATED     31
#define MBXQ$V_DELETEPENDING  30
#define MBXQ$V_SHUT           29

#define MBXQ$M_MBXCREATED     (1<<MBXQ$V_MBXCREATED    )
#define MBXQ$M_DELETEPENDING  (1<<MBXQ$V_DELETEPENDING )
#define MBXQ$M_SHUT           (1<<MBXQ$V_SHUT          )

#endif

pMbxQ          MbxQ_new(pMBX m, longword option, void (*AST)(pMbxE));
int            MbxQ_write(pMbxQ q, void *buf, int n);
pMbxE          MbxQ_read(pMbxQ q);
void           MbxQ_dispose(pMbxE e);
int            MbxQ_shut(pMbxQ q);
int            MbxQ_shut2(pMbxQ q);
pMbxQ          MbxQ_destroy(pMbxQ q);
int            MbxQ_timeout(pMbxQ q, int secs);
int            MbxQ_start(pMbxQ q);
pMbxE          MbxQ_read2(pMbxQ q, int timeout);
int            MbxQ_bytes(pMbxQ q);
void           MbxQ_setbacklog(pMbxQ q, int n);
void           MbxQ_setname(pMbxQ q, char *name);
pMbxE          MbxQ_new_entry(void);
int            MbxQ_flushed(pMbxQ q, int n, int timeout);

#ifdef __PRIVATE_MBXQ
static void    MbxQ_ASTw(pMbxQ q);
static void    MbxQ_ASTwc(pMbxE e);
static void    MbxQ_ASTws(pMbxQ q);
static void    MbxQ_ASTr(pMbxE e);
static int     MbxQ_queue_read(pMbxQ q);
static int     MbxQ_write2(pMbxQ q, void *buf, int n);
#endif

#define pMbxE_size(e)     ((e)->len)
#define pMbxE_buf(e)      ((e)->buf)
#define pMbxE_status(e)   ((e)->iosb.status)

#endif
