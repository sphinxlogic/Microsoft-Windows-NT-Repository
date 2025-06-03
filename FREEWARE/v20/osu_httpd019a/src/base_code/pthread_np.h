/*
 * This include file handles fixups needed for the DECthreads implementation
 * of the pthread library definitions.
 */
#ifdef _GNUC_
#define pthread_self pthread_self_hidden
#endif
#include <pthread.h>
#ifdef __DECC
#include <reentrancy.h>
#endif
/*
 * For some reason, the re-defines of malloc and free don't have correspondin
 * entries in the shareable images, so undefine them.
 */
#if defined(VAXC) || defined(_GNUC_)
#ifdef malloc
#undef malloc
#endif
#ifdef free
#undef free
#endif
#ifdef realloc
#undef realloc
#endif
#endif
/*
 * If the c routines ever become thread-reentrant, null out these definitions.
 * As an intermediate step, we've made these dependant upon a global flag.
 */
int http_reentrant_c_rtl;
#ifdef VMS
#define LOCK_C_RTL if ( !http_reentrant_c_rtl ) pthread_lock_global_np();
#define UNLOCK_C_RTL if ( !http_reentrant_c_rtl ) pthread_unlock_global_np();
#else
#define LOCK_C_RTL
#define UNLOCK_C_RTL
#define LOCK_VMS_RTL
#define UNLOCK_VMS_RTL
#endif

int http_reentrant_vms_rtl;
#ifdef VMS
#define LOCK_VMS_RTL 
#define UNLOCK_VMS_RTL
#endif
/*
 * Set symbol that tserver_tcp.h uses to detect if pthread headers known.
 */
#define loaded_pthread_np 1
/*
 * Workaround GCCs problem with structure returned by pthread_self() function.
 */
#ifdef _GNUC_
void pthread_self ( pthread_t * );
static pthread_t gcc_pthread_self() {
    pthread_t temp;
    pthread_self ( &temp );
    return temp;
}
#define pthread_self gcc_pthread_self
#endif
