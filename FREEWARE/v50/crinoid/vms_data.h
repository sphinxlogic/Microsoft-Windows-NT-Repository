/*
 *
 *  vms data structures
 *
 */

#ifndef __VMS_DATA_H
#define __VMS_DATA_H

#include <descrip.h>
typedef unsigned char   byte;
typedef unsigned short  word;
typedef unsigned long   longword;
typedef signed   long   signed_longword;

typedef struct dsc$descriptor_s   STRING;
typedef struct dsc$descriptor_s* pSTRING;
typedef struct _IOSB               IOSB;

#define asciz_pSTRING(p)   ((p)->dsc$a_pointer)
#define strlen_pSTRING(p)  ((p)->dsc$w_length)

#pragma member_alignment save
#pragma nomember_alignment
struct _IOSB {
    word      status;
    word      count;
    longword  dvispec;
};


struct ITEM_LIST_3 {
    word length;
    word code;
    void * address;
    word * return_length_address;
};

typedef struct ITEM_LIST_3    ItemList;
typedef struct ITEM_LIST_3*  pItemList;
#define NULL_ITEM      {0,0,0,0}
#define ITEM(a,b,c,d)  {(a),(b),(c),(d)}
#define ITEM_CODE(a)         (a)->code
#define ITEM_LENGTH(a)       (a)->length
#define ITEM_ADDR(a)         (a)->address
#define ITEM_RETLENADDR(a)   (a)->return_length_address

struct _lksb {
    word        status;
    word        _reserved;
    longword    id;
    byte        value[16];
};

typedef struct _lksb    LSB;
typedef struct _lksb * pLSB;

#pragma nomember_alignment quadword

struct SRQP {
    longword relptr[2];
};
typedef struct SRQP RQE;

#pragma nomember_alignment

#include <prvdef.h>
typedef union prvdef     Privs;
typedef union prvdef*   pPrivs;

#define VMS_OK(s)    (((s)&1)!=0)
#define VMS_ERR(s)   (((s)&1)==0)
#define UNIX_OK(s)   (s==0)
#define UNIX_ERR(s)  (s!=0)
#define UNIX_ABORT(s) {perror(s); lib$signal(CRINOID_SYSSRVERR,2,__FILE__,__LINE__,SS$_ABORT);}
#define VMS_ABORT(s,msg)  {errlog(L_ERROR,"!AZ, Error 0x!XL",msg,s); lib$signal(CRINOID_SYSSRVERR,2,__FILE__,__LINE__,s);}

#pragma message save
#pragma message disable globalext
globalvalue int CRINOID_SYSSRVERR;
#pragma message restore

#define VMS_SIGNAL(s)   lib$signal(CRINOID_SYSSRVERR,2,__FILE__,__LINE__,s)
#define RMS_SIGNAL(s,v) lib$signal(CRINOID_SYSSRVERR,2,__FILE__,__LINE__,s,v)


#define DEFAULT_MAILBOX_SIZE     1024
#define DEFAULT_MAILBOX_QUOTA    (4*DEFAULT_MAILBOX_SIZE)
#define MAX_MAILBOX_SIZE         4096
#endif
