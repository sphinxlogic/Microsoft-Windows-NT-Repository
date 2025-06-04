#ifndef __UTIL_H
#define __UTIL_H

#include "vms_data.h"
#include "CRINOID_types.h"
void        lc(char *s);
void        uc(char *s);
void        printable(char *s);
pMBX        new_MBX(char *lname, int size);
pMBX        new_MBX_setquota(char *lname, int size, int quota);
pMBX        assign_new_MBX(char *dev, pMBX devmbx);
pMBX        destroy_MBX(pMBX m);
int         device_unit(word chan);
void        device_name(word chan, char *name);
int         device_size(word chan);
longword    get_pflquota(longword pid);
longword    get_bytcnt(longword pid);
longword    get_bytlm(longword pid);
longword    get_pid(void);
pSTRING     new_STRING(char *s);
pSTRING     new_STRING2(int size);
pSTRING     concat_STRING(pSTRING s1, pSTRING s2);
pSTRING     destroy_STRING(pSTRING s);
int         tu_strlen(char *str );
char *      tu_strcpy ( char *str1, char *str2 );
char *      tu_strcat ( char *str1, char *str2 );
char *      tu_strncpy ( char *str1, char *str2, int limit );
char *      tu_strnzcpy ( char *str1, char *str2, int limit );
char *      tu_strchr (char *s, char c);
char *      tu_strrchr (char *s, char c);
int         tu_strncmp ( char *str1, char *str2, int maxl );
int         tu_strcmp ( char *str1, char *str2);
int         tu_strcmp_uc ( char *str1, char *str2);
char *      tu_strstr ( char *s1, char *s2 );
int         tu_nmatch( char *s1, char *s2);
pSTRING     translate_logical(char *name);
int         Set_Privs(pPrivs p);
int         Reset_Privs(pPrivs p);
pItemList   new_ItemList(int n);
pItemList   add_Item(pItemList i, word code, void *address, word size, word *retlen);
void        destroy_ItemList(pItemList i);
pSTRING     get_userlogin(char *user);
pPrivs      Current_Privs(void);
pPrivs      Authorized_Privs(void);
pPrivs      Image_Privs(void);
pPrivs      Process_Privs(void);
pPrivs      AND_Privs(pPrivs p1, pPrivs p2);
pPrivs      OR_Privs(pPrivs p1, pPrivs p2);
pPrivs      NOT_Privs(pPrivs p1);
pPrivs      new_Privs(void);
void        destroy_Privs(pPrivs p);
int         sec2vms(int seconds, longword delta[2]);
pSTRING     errormsg(int cond);
int         define_logical(char *name, char *value, char *table);
#ifndef __ALPHA
int         __ATOMIC_INCREMENT_LONG(volatile void *p);
int         __ATOMIC_DECREMENT_LONG(volatile void *p);
#endif
void        my_free(void *pointer, char *routine);
void *      my_malloc(size_t size, char *routine);
void *      my_realloc(void *pointer, size_t size, char *routine);
char *      dumpstring(char *s, int n);

#endif  /* __UTIL_H */
