/*      */
#ifndef __PIPE2_H
#define __PIPE2_H


#include "vms_data.h"
#include "CRINOID_types.h"
#include "util.h"
#include "mbxq.h"


pPipe2      Pipe_new(pMBX in, pMBX out);
void        Pipe_shutdown(pPipe2 p);
pPipe2      Pipe_destroy(pPipe2 p);
pPipe2      Pipe_destroy2(pPipe2 p);
void        Pipe_pack(pPipe2 p);
void        Pipe_flush(pPipe2 p);
#ifdef __PIPE2_PRIVATE
static void Pipe_AST(pMbxE e);
#define     PIPE$M_PACK;
#endif


#endif
