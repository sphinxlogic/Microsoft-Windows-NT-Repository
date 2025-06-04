/*
    PIPE DATA FROM ONE MBX TO ANOTHER

*/

#include <stdlib.h>
#include <string.h>

#include <ssdef.h>
#include <lib$routines.h>
#include <starlet.h>

#define  __PIPE2_PRIVATE
#include "pipe2.h"
#include "errlog_client.h"

pPipe2
Pipe_new(pMBX in, pMBX out)
{
    ROUTINE_NAME("Pipe_new");
    int size;
    pPipe2 p = malloc(sizeof(Pipe2));
    errlog(L_PIPE|L_TRACE,"Pipe_new(pMBXin:!XL pMBXout:!XL) = !XL",in, out, p);
    if (!p) {
        errlog(L_CRITICAL,"Pipe_new: failed malloc");
        goto error;
    }

    p->in_mbx = p->out_mbx = 0;
    p->inQ = p->outQ = 0;
    p->buffer = 0;
    p->bcount = 0;

    size = 0;
    if (in) {
        size = in->size;
    } else if (out) {
        size = out->size;
    }

    if (in) {
        p->in_mbx = in;
    } else {
        p->in_mbx = new_MBX(0,size);
        errlog(L_PIPE|L_BABBLE,"Pipe_new: in_mbx !XL",p->in_mbx);
        if (!p->in_mbx) {
            errlog(L_CRITICAL,"Pipe_new: failed creating in_mbx");
            goto error;
        }
    }

    if (out) {
        p->out_mbx = out;
    } else {
        p->out_mbx = new_MBX(0,size);
        errlog(L_PIPE|L_BABBLE,"Pipe_new: out_mbx !XL",p->out_mbx);
        if (!p->out_mbx) {
            errlog(L_CRITICAL,"Pipe_new: failed creating out_mbx");
            goto error;
        }
    }

    p->inQ = MbxQ_new(p->in_mbx, MBXQ$_READ|MBXQ$M_NOSTART, &Pipe_AST);
    errlog(L_PIPE|L_BABBLE,"Pipe_new: inQ !XL",p->inQ);
    if (!p->inQ) {
        errlog(L_CRITICAL,"Pipe_new: failed creating inQ");
        goto error;
    }
    p->inQ->extra = p;

    p->outQ = MbxQ_new(p->out_mbx, MBXQ$_WRITE|MBXQ$M_RETRY, 0);
    errlog(L_PIPE|L_BABBLE,"Pipe_new: outQ !XL",p->outQ);
    if (!p->outQ) {
        errlog(L_CRITICAL,"Pipe_new: failed creating outQ");
        goto error;
    }
    MbxQ_start(p->inQ);
    return p;

error:
    if (p) {
        if (p->in_mbx && !in) destroy_MBX(p->in_mbx);
        if (p->out_mbx && !out) destroy_MBX(p->in_mbx);
        if (p->inQ) MbxQ_destroy(p->inQ);
        if (p->outQ) MbxQ_destroy(p->outQ);
        free(p);
    }
    errlog(L_CRITICAL,"Pipe_new: ERROR creating pipe");
    return 0;
}


static void
Pipe_AST(pMbxE e)
{
    ROUTINE_NAME("Pipe_AST");
    int iss = pMbxE_status(e);
    pMbxQ   q = e->queue;
    pPipe2  p = q->extra;
    char *slog;

    errlog(L_PIPE|L_TRACE,"Pipe_AST Queue: !XL, id: !SL  Pipe:!XL, status:!XL)",q,e->id,p,iss);
    if (iss == SS$_ENDOFFILE) {
        if (p->buffer && p->bcount) {
            errlog(L_PIPE|L_BABBLE,"Pipe_AST: transmitting !SL bytes at EOF", p->bcount);
            MbxQ_write(p->outQ,p->buffer,p->bcount);
            if (islogging(L_PIPE|L_DEBUG) && (slog = dumpstring(p->buffer, p->bcount))) {
                errlog(L_PIPE|L_DEBUG,"Pipe_AST: T:'!AZ'",slog);
                free(slog);
            }

            p->bcount = 0;
        }
        errlog(L_PIPE|L_BABBLE,"Pipe_AST: transmitting EOF");
        MbxQ_write(p->outQ, 0, -1);
    } else if (iss == SS$_LINKDISCON || iss == SS$_CANCEL || iss == SS$_ABORT) {
        if (p->buffer && p->bcount) {
            errlog(L_PIPE|L_BABBLE,"Pipe_AST: transmitting !SL bytes at DISCON", p->bcount);
            MbxQ_write(p->outQ,p->buffer,p->bcount);
            if (islogging(L_PIPE|L_DEBUG) && (slog = dumpstring(p->buffer, p->bcount))) {
                errlog(L_PIPE|L_DEBUG,"Pipe_AST: T:'!AZ'",slog);
                free(slog);
            }

            p->bcount = 0;
        }
        Pipe_shutdown(p);
    } else if (VMS_ERR(iss)) {
        VMS_SIGNAL(iss);
    } else {
        if (p->buffer) {
            int   m, n = e->len;
            char *t = e->buf;

            errlog(L_PIPE|L_BABBLE,"Pipe_AST: appending !SL bytes",n);
            while (n > 0) {
                m = (n + p->bcount > p->out_mbx->size) ? p->out_mbx->size - p->bcount : n;
                memcpy(p->buffer+p->bcount, t, m);
                p->bcount += m;
                t += m;
                n -= m;

                if (p->bcount == p->out_mbx->size || (p->bcount && memchr(p->buffer, 0x0a, p->bcount))) {
                    iss = MbxQ_write(p->outQ, p->buffer, p->bcount);
                    errlog(L_PIPE|L_BABBLE,"Pipe_AST: transmitting !UL bytes, iss=!XL",p->bcount,iss);
                    if (islogging(L_PIPE|L_DEBUG) && (slog = dumpstring(p->buffer, p->bcount))) {
                        errlog(L_PIPE|L_DEBUG,"Pipe_AST: T:'!AZ'",slog);
                        free(slog);
                    }

                    if (VMS_ERR(iss)) {
                        errlog(L_PIPE|L_WARNING,"Pipe_AST: error transmitting (!XL), shutdown pipe",iss);
                        Pipe_shutdown(p);
                    }
                    p->bcount = 0;
                }
            }
        } else  {
            iss = MbxQ_write(p->outQ, e->buf, e->len);
            errlog(L_PIPE|L_BABBLE,"Pipe_AST: transmitting !UL bytes, iss=!XL",e->len,iss);
            if (VMS_ERR(iss)) {
                errlog(L_PIPE|L_WARNING,"Pipe_AST: error transmitting (!XL), shutdown pipe",iss);
                Pipe_shutdown(p);
            }
            if (islogging(L_PIPE|L_DEBUG) && (slog = dumpstring(e->buf, e->len))) {
                errlog(L_PIPE|L_DEBUG,"Pipe_AST: T:'!AZ'",slog);
                free(slog);
            }


        }
    }
    MbxQ_dispose(e);
}

void
Pipe_shutdown(pPipe2 p)
{
    ROUTINE_NAME("Pipe_shutdown");
    errlog(L_PIPE|L_TRACE,"Pipe_shutdown(pipe:!XL)",p);
    if (!p) return;

    MbxQ_shut(p->inQ);
    MbxQ_shut(p->outQ);
}

pPipe2
Pipe_destroy(pPipe2 p)
{
    ROUTINE_NAME("Pipe_destroy");
    errlog(L_PIPE|L_TRACE,"Pipe_destroy(pipe:!XL)",p);
    if (!p) return 0;

    MbxQ_destroy(p->inQ);
    MbxQ_destroy(p->outQ);
    destroy_MBX(p->in_mbx);
    destroy_MBX(p->out_mbx);
    if (p->buffer) free(p->buffer);
    free(p);
    return 0;
}

/* destroy with flush */
pPipe2
Pipe_destroy2(pPipe2 p)
{
    ROUTINE_NAME("Pipe_destroy2");
    int iss, j, bin, sent;
    char *slog;

    errlog(L_PIPE|L_TRACE,"Pipe_destroy2(pipe:!XL)",p);
    if (!p) return 0;

    MbxQ_shut2(p->inQ);     /* wait for input shutdown */
    bin = MbxQ_bytes(p->inQ);
    MbxQ_destroy(p->inQ);
    destroy_MBX(p->in_mbx);

    if (p->buffer && p->bcount) {
        iss = MbxQ_write(p->outQ, p->buffer, p->bcount);
        errlog(L_PIPE|L_BABBLE,"Pipe_destroy2: transmitting !UL bytes, iss=!XL",p->bcount,iss);
        if (slog = dumpstring(p->buffer, p->bcount)) {
            errlog(L_PIPE|L_DEBUG,"Pipe_AST: T:'!AZ'",slog);
            free(slog);
        }

        if (VMS_ERR(iss)) {
            errlog(L_PIPE|L_WARNING,"Pipe_destroy2: error transmitting (!XL), shutdown pipe",iss);
            Pipe_shutdown(p);
        }
        p->bcount = 0;
    }
    iss = MbxQ_write(p->outQ,0,-1);           /* send EOF */
    errlog(L_PIPE|L_BABBLE,"Pipe_destroy2: transmitting EOF, iss=!XL",iss);


    if ((sent = MbxQ_flushed(p->outQ, bin, 30)) != bin)
        errlog(L_ERROR,"Pipe_destroy2(p=!XL) ERR flushing, sent !SL, need !SL",p,sent,bin);

    MbxQ_destroy(p->outQ);
    destroy_MBX(p->out_mbx);
    if (p->buffer) free(p->buffer);
    free(p);
    return 0;
}

void
Pipe_pack(pPipe2 p)
{
    ROUTINE_NAME("Pipe_pack");
    if (!p) return;
    p->buffer = malloc(p->out_mbx->size);
    p->bcount = 0;

}


void
Pipe_flush(pPipe2 p)
{
    ROUTINE_NAME("Pipe_flush");
    int iss;
    char *slog;

    if (!p) return;
    iss = sys$setast(0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    if (p->buffer && p->bcount) {
        iss = MbxQ_write(p->outQ, p->buffer, p->bcount);
        errlog(L_PIPE|L_BABBLE,"Pipe_flush: transmitting !UL bytes, iss=!XL",p->bcount,iss);
        if (slog = dumpstring(p->buffer, p->bcount)) {
            errlog(L_PIPE|L_DEBUG,"Pipe_flush: T:'!AZ'",slog);
            free(slog);
        }

        if (VMS_ERR(iss)) {
            errlog(L_PIPE|L_WARNING,"Pipe_flush: error transmitting (!XL), shutdown pipe",iss);
            Pipe_shutdown(p);
        }
        p->bcount = 0;
    }
    iss = sys$setast(1);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);


}
