#ifndef NSQUERY_H
#define NSQUERY_H 1

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __GNUC__
#include <vms/descrip.h>
#include <vms/stsdef.h>
#include <vms/ssdef.h>
extern int cli$_absent();
#define CLI$_ABSENT ((unsigned int) cli$_absent)
extern int cli$_present();
#define CLI$_PRESENT ((unsigned int) cli$_present)
extern int cli$_negated();
#define CLI$_NEGATED ((unsigned int) cli$_negated)
extern int cli$_comma();
#define CLI$_COMMA ((unsigned int) cli$_comma)
#else
#include <descrip.h>
#include <stsdef.h>
#include <ssdef.h>
#include <lib$routines.h>
#include <str$routines.h>
#include <starlet.h>
#ifdef __DECC
#include <builtins.h>
#else
#pragma builtins
#endif
#pragma nostandard
globalvalue unsigned int CLI$_ABSENT, CLI$_PRESENT, CLI$_NEGATED, CLI$_COMMA;
#endif

#define STRING_SIZE 1024

#define LONG_PACK(x,p) {*(unsigned int *)p = (x); p+= sizeof(unsigned int);}
#define LONG_UNPACK(p,x) {(x)=(*(unsigned int *)(p)); p+= sizeof(unsigned int);}
#define LONG_REVERSE_UNPACK(p,x) {\
    x = (*p<<24)+(*(p+1)<<16)+(*(p+2)<<8)+(*(p+3));\
    p += sizeof(unsigned int);}
#define BYTE_SWAP_PACK(x,p) {*p++ = ((x)&0xff00)>>8; *p++ = ((x)&0xff);}
#define BYTE_SWAP_UNPACK(p,x) {x = (*p<<8)+(*(p+1)); p+=sizeof(short);}
#define WORD_PACK(x,p) {*(short *)p = x; p += sizeof(short);}
#define WORD_UNPACK(p,x) {x = *(short *)p; p+=sizeof(short);}
#define ASCIC_PACK(s,p) {int len; len=strlen(s);\
    	    *p++ = len; memcpy(p, s, len); p+=len;}
#define ASCIC_UNPACK(p,s) {memcpy(s,p+1,*p); *(s+(*p))='\0'; p+=(*p)+1;}
#define ASCICN_UNPACK(p,s,n) {memcpy(s,p+1,(n=(*p))); *(s+(*p))='\0'; p+=(*p)+1;}
#define TIME_UNPACK(p,t) {static int m=(-10000000),z=0; int xtime;\
    	    LONG_REVERSE_UNPACK(p,xtime); lib$emul(&xtime, &m, &z, &t);}
#define NAME_UNPACK(p,s) name_unpack(&p,s,sizeof(s))
#define BYTE_UNPACK(p,x) {x =(*p++);}

/*
** VMS date-time stamp
*/
    typedef struct { unsigned int long1, long2; } TIME;
/*
** item_list_3 item
*/
    typedef struct { short bufsiz, itmcod; void *bufadr, *retlen; } ITMLST;

/*
** Generic queue structure
*/
    struct QUE { void *head, *tail; };
/*
** For forming list of addresses to be contacted
*/
    struct ADR {
    	struct ADR *flink, *blink;
    	unsigned int address;
    };


#define INIT_DYNDESC(str) {str.dsc$w_length = 0; str.dsc$a_pointer = (void *) 0;\
    	    str.dsc$b_class = DSC$K_CLASS_D; str.dsc$b_dtype = DSC$K_DTYPE_T;}
#define INIT_SDESC(str,len,ptr) {str.dsc$w_length=(len);str.dsc$a_pointer=(ptr);\
    	    str.dsc$b_class=DSC$K_CLASS_S; str.dsc$b_dtype=DSC$K_DTYPE_T;}
#define ITMLST_INIT(itm,c,s,a,r) {itm.bufsiz=(s); itm.itmcod=(c);\
    	    itm.bufadr=(void *)(a); itm.retlen=(void *)(r);}
#define OK(x) $VMS_STATUS_SUCCESS(x)

#ifdef __ALPHA
#define INSQUE(item,pred) __PAL_INSQUEL((void *)(pred),(void *)(item))
#define REMQUE(entry,addr) (((struct QUE *)entry)->head == entry ? 0 :\
    	    	    	    (__PAL_REMQUEL((void *)(entry),(void *)(addr)),1))
#else
#define INSQUE(item,pred) _INSQUE(item,pred)
#define REMQUE(entry,addr) (((struct QUE *)entry)->head == entry ? 0 :\
    	    	    	    (_REMQUE(entry,addr),1))
#endif
#endif
