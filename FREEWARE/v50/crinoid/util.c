/*
 *  utility routines
 *
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <ssdef.h>
#include <starlet.h>
#include <builtins.h>
#include <psldef.h>
#include <dvidef.h>
#include <jpidef.h>
#include <lnmdef.h>
#include <uaidef.h>
#include <rmsdef.h>
#include <lib$routines.h>
#include "errlog_client.h"
#include "util.h"

#define TOHEX(i) ((i) < 0x0a ? '0'+(i) : 'A' + (i) - 0x0A)

void
lc(char *s)
{
    if (!s) return;
    while (*s) {
        *s = tolower(*s);
        s++;
    }
}

void
uc(char *s)
{
    if (!s) return;
    while (*s) {
        *s = toupper(*s);
        s++;
    }
}

void
printable(char *s)
{
    if (!s) return;
    while (*s) {
        *s &= 0x7f;
        if (*s < ' ') *s = '.';
        if (*s == 0x7f) *s = '.';
        s++;
    }
}

pMBX
new_MBX(char *lname, int size)
{
    ROUTINE_NAME("new_MBX");
    int iss;
    word length;
    longword unit;
    $DESCRIPTOR(d_lname, "");
    pItemList i;
    IOSB iosb;
    pMBX m;

    m = (pMBX) malloc(sizeof(MBX));
    if (!m) VMS_ABORT(SS$_INSFMEM,"new_MBX: malloc failed");

    m->chan = 0;
    m->unit = 0;
    m->d_name.dsc$a_pointer = m->name;
    m->d_name.dsc$w_length  = 0;
    m->d_name.dsc$b_dtype   = DSC$K_DTYPE_T;
    m->d_name.dsc$b_class   = DSC$K_CLASS_S;

/*  special case ... make a MBX struct, but that's all... */

    if (size < 0) {
        if (lname) {
            tu_strcpy(m->name, lname);
            m->d_name.dsc$w_length = tu_strlen(lname);
        }
        return m;
    } else if (size == 0) {
        size = DEFAULT_MAILBOX_SIZE;
    }
    m->size = size;

    if (lname) {
        d_lname.dsc$w_length  = tu_strlen(lname);
        d_lname.dsc$a_pointer = lname;
    }

    iss = sys$crembx(0,&m->chan,size,DEFAULT_MAILBOX_QUOTA,0,
                     PSL$C_USER,(lname ? &d_lname : 0));
    if (VMS_ERR(iss)) VMS_ABORT(iss,"new_MBX: crembx error");


    i = new_ItemList(2);
    i = add_Item(i, DVI$_UNIT, &unit, sizeof(unit), 0);
    i = add_Item(i, DVI$_DEVNAM, m->name, 64, &length);
    length = 0;

    iss = sys$getdviw(0, m->chan, 0, i, &iosb, 0, 0, 0);
    destroy_ItemList(i);
    if (VMS_ERR(iss)) VMS_ABORT(iss,"new_MBX: getdvi");
    iss = iosb.status;
    if (VMS_ERR(iss)) VMS_ABORT(iss,"new_MBX: getdvi");

    m->unit = unit;
    m->d_name.dsc$w_length  = length;
    m->name[length] = '\0';


    return m;
}

pMBX
new_MBX_setquota(char *lname, int size, int quota)
{
    ROUTINE_NAME("new_MBX_setquota");
    int iss;
    word length;
    longword unit;
    $DESCRIPTOR(d_lname, "");
    pItemList i;
    IOSB iosb;
    pMBX m;

    m = (pMBX) malloc(sizeof(MBX));
    if (!m) VMS_ABORT(SS$_INSFMEM,"new_MBX_setquota, malloc failure");

    m->chan = 0;
    m->unit = 0;
    m->d_name.dsc$a_pointer = m->name;
    m->d_name.dsc$w_length  = 0;
    m->d_name.dsc$b_dtype   = DSC$K_DTYPE_T;
    m->d_name.dsc$b_class   = DSC$K_CLASS_S;

/*  special case ... make a MBX struct, but that's all... */

    if (size < 0) {
        if (lname) {
            tu_strcpy(m->name, lname);
            m->d_name.dsc$w_length = tu_strlen(lname);
        }
        return m;
    } else if (size == 0) {
        size = DEFAULT_MAILBOX_SIZE;
    }
    m->size = size;

    if (lname) {
        d_lname.dsc$w_length  = tu_strlen(lname);
        d_lname.dsc$a_pointer = lname;
    }

    iss = sys$crembx(0,&m->chan,size,quota,0,PSL$C_USER,(lname ? &d_lname : 0));
    if (VMS_ERR(iss)) VMS_ABORT(iss,"new_MBX_setquota, crembx");


    i = new_ItemList(2);
    i = add_Item(i, DVI$_UNIT, &unit, sizeof(unit), 0);
    i = add_Item(i, DVI$_DEVNAM, m->name, 64, &length);
    length = 0;

    iss = sys$getdviw(0, m->chan, 0, i, &iosb, 0, 0, 0);
    destroy_ItemList(i);
    if (VMS_ERR(iss)) VMS_ABORT(iss,"new_MBX_setquota, getdvi");
    iss = iosb.status;
    if (VMS_ERR(iss)) VMS_ABORT(iss,"new_MBX_setquota, getdvi");

    m->unit = unit;
    m->d_name.dsc$w_length  = length;
    m->name[length] = '\0';


    return m;
}





pMBX
assign_new_MBX(char *dev, pMBX devmbx)
{
    ROUTINE_NAME("assign_new_MBX");
    int iss;
    word length;
    longword unit, size;
    pSTRING d_dev;
    pItemList i;
    IOSB iosb;
    pMBX m;

    m = (pMBX) malloc(sizeof(MBX));
    if (!m) VMS_ABORT(SS$_INSFMEM,"assign_new_MBX, malloc failure");

    m->chan = 0;
    m->unit = 0;
    m->d_name.dsc$a_pointer = m->name;
    m->d_name.dsc$w_length  = 0;
    m->d_name.dsc$b_dtype   = DSC$K_DTYPE_T;
    m->d_name.dsc$b_class   = DSC$K_CLASS_S;

    d_dev = new_STRING(dev);

    iss = sys$assign(d_dev, &m->chan, 0, (devmbx ? &devmbx->d_name : 0));
    destroy_STRING(d_dev);
    if (VMS_ERR(iss)) {
        pSTRING s = errormsg(iss);
        errlog(L_ERROR,"assign_new_MBX: sys$assign err 0x!XL (!AS) for !AZ",iss,s,dev);
        destroy_STRING(s);
        free(m);
        return 0;
    }

    i = new_ItemList(3);
    i = add_Item(i, DVI$_UNIT, &unit, sizeof(unit), 0);
    i = add_Item(i, DVI$_DEVNAM, m->name, 64, &length);
    i = add_Item(i, DVI$_DEVBUFSIZ, &size, sizeof(size), 0);
    length = 0;

    iss = sys$getdviw(0, m->chan, 0, i, &iosb, 0, 0, 0);
    destroy_ItemList(i);
    if (VMS_ERR(iss)) VMS_ABORT(iss,"assign_new_MBX, getdvi");
    iss = iosb.status;
    if (VMS_ERR(iss)) VMS_ABORT(iss,"assign_new_MBX, getdvi");

    m->unit = unit;
    m->d_name.dsc$w_length  = length;
    m->name[length] = '\0';
    m->size = size;

    return m;
}

pMBX
destroy_MBX(pMBX m)
{
    ROUTINE_NAME("destroy_MBX");
    int iss;

    if (m->chan != 0) {
        iss = sys$dassgn(m->chan);
        if (VMS_ERR(iss)) VMS_ABORT(iss,"destroy_MBX, dassgn");
        m->chan = 0;
    }

    free(m);
    return 0;
}

int
device_unit(word chan)
{
    ROUTINE_NAME("device_unit");
    pItemList i = new_ItemList(1);
    IOSB iosb;
    int iss, unit;

    i = add_Item(i, DVI$_UNIT, &unit, sizeof(unit), 0);

    iss = sys$getdviw(0, chan, 0, &i, &iosb, 0, 0, 0);
    destroy_ItemList(i);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = iosb.status;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    return unit;
}

void
device_name(word chan, char *name)
{
    ROUTINE_NAME("device_name");
    pItemList i;
    IOSB iosb;
    int iss;
    word length;

    i = new_ItemList(1);
    i = add_Item(i, DVI$_DEVNAM, name, 64, &length);

    iss = sys$getdviw(0, chan, 0, &i, &iosb, 0, 0, 0);
    destroy_ItemList(i);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = iosb.status;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    name[length]='\0';
}

int
device_size(word chan)
{
    ROUTINE_NAME("device_size");
    pItemList i;
    IOSB iosb;
    int iss;
    word size;

    i = new_ItemList(1);
    i = add_Item(i, DVI$_DEVBUFSIZ, &size, sizeof(size), 0);

    iss = sys$getdviw(0, chan, 0, &i, &iosb, 0, 0, 0);
    destroy_ItemList(i);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = iosb.status;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    return (int) size;
}



longword
get_pflquota(longword pid)
{
    ROUTINE_NAME("get_pflquota");
    int iss;
    longword quota = 0;
    IOSB iosb;
    pItemList i;
    pPrivs p0 = Current_Privs();
    pPrivs p;

    i = new_ItemList(1);
    i = add_Item(i, JPI$_PAGFILCNT, &quota, sizeof(quota), 0);

    if (pid && !p0->prv$v_world) {
        p  = new_Privs();
        p->prv$v_world = 1;
        Set_Privs(p);
    }

    iss = sys$getjpiw(0,&pid,0,i,&iosb,0,0);

    if (pid && !p0->prv$v_world) {
        Reset_Privs(p);
        destroy_Privs(p);
    }
    destroy_Privs(p0);

    destroy_ItemList(i);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = iosb.status;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    return quota;
}

longword
get_bytcnt(longword pid)
{
    ROUTINE_NAME("get_bytcnt");
    int iss;
    longword quota = 0;
    IOSB iosb;
    pItemList i;

    i = new_ItemList(1);
    i = add_Item(i, JPI$_BYTCNT, &quota, sizeof(quota), 0);
    iss = sys$getjpiw(0,&pid,0,i,&iosb,0,0);
    destroy_ItemList(i);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = iosb.status;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    return quota;
}

longword
get_bytlm(longword pid)
{
    ROUTINE_NAME("get_bytlm");
    int iss;
    longword quota = 0;
    IOSB iosb;
    pItemList i;

    i = new_ItemList(1);
    i = add_Item(i, JPI$_BYTLM, &quota, sizeof(quota), 0);
    iss = sys$getjpiw(0,&pid,0,i,&iosb,0,0);
    destroy_ItemList(i);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = iosb.status;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    return quota;
}

longword
get_pid(void)
{
    ROUTINE_NAME("get_pid");
    int iss;
    longword pid;
    pItemList i;
    IOSB iosb;

    i = new_ItemList(1);
    i = add_Item(i, JPI$_PID, &pid, sizeof(pid), 0);
    iss = sys$getjpiw(0,0,0,i,&iosb,0,0);
    destroy_ItemList(i);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = iosb.status;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    return pid;
}



pSTRING
new_STRING(char *s)
{
    ROUTINE_NAME("new_STRING");
    pSTRING p;
    static $DESCRIPTOR(d_null, "");

    p = (pSTRING) malloc(sizeof(STRING));
    if (!p) VMS_SIGNAL(SS$_INSFMEM);

    *p = d_null;
    p->dsc$w_length = tu_strlen(s);

    p->dsc$a_pointer = (char *) malloc(p->dsc$w_length+1);
    if (!p->dsc$a_pointer) VMS_SIGNAL(SS$_INSFMEM);

    strcpy(p->dsc$a_pointer, s);
    return p;
}

pSTRING
new_STRING2(int size)
{
    ROUTINE_NAME("new_STRING2");
    pSTRING p;
    static $DESCRIPTOR(d_null, "");

    p = (pSTRING) malloc(sizeof(STRING));
    if (!p) VMS_SIGNAL(SS$_INSFMEM);

    *p = d_null;
    p->dsc$w_length = size;

    p->dsc$a_pointer = (char *) malloc(size+1);
    if (!p->dsc$a_pointer) VMS_SIGNAL(SS$_INSFMEM);
    memset(p->dsc$a_pointer, 0, size+1);

    return p;
}

pSTRING
concat_STRING(pSTRING s1, pSTRING s2)
{
    ROUTINE_NAME("concat_STRING");
    pSTRING p;
    int size = strlen_pSTRING(s1)+strlen_pSTRING(s2);

    p = new_STRING2(size);
    if (!p) VMS_SIGNAL(SS$_INSFMEM);

    memcpy(asciz_pSTRING(p), asciz_pSTRING(s1), strlen_pSTRING(s1));
    memcpy(asciz_pSTRING(p)+strlen_pSTRING(s1), asciz_pSTRING(s2), strlen_pSTRING(s2));
    return p;
}








pSTRING
destroy_STRING(pSTRING p)
{
    ROUTINE_NAME("destroy_STRING");
    int iss, length;

    if (!p) return 0;
    if (p->dsc$a_pointer) free(p->dsc$a_pointer);
    free(p);
    return 0;
}


/**************************************************************************/
/*
 * thread-reentrant versions of string routines
 */
int tu_strlen(char *str )
{ int i = 0; char *p; for ( p = str; *p++; i++ ); return i; }

char *tu_strcpy ( char *str1, char *str2 )
{ char *p, *d, c;
    d = str1;
    for ( p = str2; (c = *p++); *d++ = c );
    *d = '\0';
    return str1;
}

char *tu_strcat ( char *str1, char *str2 )
{ char *p, *d, c;
    for ( d = str1; *d; d++);
    for ( p = str2; (c = *p++); *d++ = c );
    *d = '\0';
    return str1;
}


char *tu_strncpy ( char *str1, char *str2, int limit )
{ char *p, *d, c; int i;
    p = str2; d = str1;
    for ( i = limit; i > 0; --i ) {
        c = *p++; *d++ = c; if ( !c ) break;
    }
    return str1;
}
char *tu_strnzcpy ( char *str1, char *str2, int limit )
{ char *p, *d, c; int i;
   /* Assume str1 buffer is 1 larger than limit */
    p = str2; d = str1;
    for ( i = limit; (i > 0) && (c = *p++); --i ) {
        *d++ = c;
    }
    *d = '\0';
    return str1;
}

char *
tu_strrchr(char *s, char c)
{
    char *r;

    if (c == 0) return s+tu_strlen(s);

    for (r = 0; *s; s++)
        if (*s == c) r = s;
    return r;
}

char *
tu_strchr(char *s, char c)
{
    for (; *s != c; s++)
        if (!*s) return 0;
    return s;
}

int tu_strncmp ( char *str1, char *str2, int maxl )
{ int i=0; char c1, c2;
    for ( i = 0; i < maxl; i++ ) {
        c1 = str1[i];
        if (c1 != str2[i]) return c1 < str2[i] ? -1 : 1;
        if (c1 == '\0') return  str2[i] ? 1: 0; /* str2 is longer */
    }
    return 0;           /* equal to maxl chars. */
}

int tu_strcmp ( char *str1, char *str2)
{ int i=0; char c1, c2;
    while (1) {
        c1 = str1[i];
        if (c1 != str2[i]) return c1 < str2[i] ? -1 : 1;
        if (c1 == '\0') return  str2[i] ? 1: 0; /* str2 is longer */
        i++;
    }
    return 0;           /* equal to maxl chars. */
}

int tu_strcmp_uc ( char *str1, char *str2)
{ int i=0; char c1, c2;
    while (1) {
        c1 = toupper(str1[i]);
        if (c1 != toupper(str2[i])) return c1 < toupper(str2[i]) ? -1 : 1;
        if (c1 == '\0') return  str2[i] ? 1: 0; /* str2 is longer */
        i++;
    }
    return 0;           /* equal to maxl chars. */
}





char *tu_strstr ( char *s1, char *s2 )
{
    char *cand;
    unsigned int i,j;

    for ( cand = s1; *cand; cand++ ) if ( *cand == *s2 ) {
        for ( i = 1; s2[i]; i++ ) {
            if ( cand[i]=='\0' ) return (char *) 0;     /* too short */
            if ( s2[i] != cand[i] ) break;
        }
        if ( s2[i] == '\0' ) return cand;       /* found match */
    }
    if ( !*s2 ) return s1;              /* Zero length input string */
    return (char *) 0;
}

/* returns the number of chars that match */
int tu_nmatch( char *s1, char *s2)
{
    int n = 0;
    if (!s1 || !s2) return 0;

    while (*s1 && *s2) {
        if (*s1++ != *s2++) break;
        n++;
    }
    return n;
}


pSTRING
translate_logical(char *name)
{
    ROUTINE_NAME("translate_logical");
    int iss;
    $DESCRIPTOR(table,"LNM$FILE_DEV");
    pSTRING pname;
    pItemList i;
    char equiv[256];
    word length;

    pname = new_STRING(name);
    i = new_ItemList(1);
    i = add_Item(i, LNM$_STRING, equiv, sizeof(equiv), &length);

    iss = sys$trnlnm(0,&table,pname,0,(char *)i);

    destroy_ItemList(i);
    destroy_STRING(pname);
    if (iss == SS$_NOLOGNAM) return 0;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    equiv[length] = '\0';
    return new_STRING(equiv);
}


int
Set_Privs(pPrivs p)
{
    return sys$setprv(1,p,0,0);
}

int
Reset_Privs(pPrivs p)
{
    return sys$setprv(0,p,0,0);
}


pItemList
new_ItemList(int n)
{
    ROUTINE_NAME("new_ItemList");
    static ItemList null_item = NULL_ITEM;
    pItemList i;

    i = malloc(sizeof(ItemList)*(n+2));
    if (!i) VMS_SIGNAL(SS$_INSFMEM);
    i[0].length = n;
    i[0].code = 0;
    i++;
    i[0] = null_item;
    return i;
}

pItemList
add_Item(pItemList i, word code, void *address, word size, word *retlen)
{
    ROUTINE_NAME("add_Item");
    static ItemList  null_item = NULL_ITEM;
    pItemList ibase = i-1;

    if (ibase->code == ibase->length) {
        ibase->length++;
        ibase = realloc(ibase,  sizeof(ItemList)*(ibase->length+2));
        if (!ibase) VMS_SIGNAL(SS$_INSFMEM);
        i = ibase+1;
    }
    i[ibase->code].code                  = code;
    i[ibase->code].address               = address;
    i[ibase->code].length                = size;
    i[ibase->code].return_length_address = retlen;
    ibase->code++;
    i[ibase->code] = null_item;

    return i;
}

void
destroy_ItemList(pItemList i)
{
    ROUTINE_NAME("destroy_ItemList");
    free(i-1);
}



pSTRING
get_userlogin(char *user)
{
    ROUTINE_NAME("get_userlogin");
    pSTRING name, result;
    int iss;
    pItemList i;
    pPrivs p0, p1;
    struct {
        byte size;
        char s[31];
    } dev;
    struct {
        byte size;
        char s[63];
    } dir;

    i = new_ItemList(2);
    i = add_Item(i, UAI$_DEFDEV, &dev, sizeof(dev), 0);
    i = add_Item(i, UAI$_DEFDIR, &dir, sizeof(dir), 0);

    name = new_STRING(user);
    uc(asciz_pSTRING(name));

    p0 = Current_Privs();
    if (!p0->prv$v_sysprv) {
        p1 = new_Privs();
        p1->prv$v_sysprv = 1;
        Set_Privs(p1);
    }

    iss = sys$getuai(0,0,name,i,0,0,0);
    destroy_STRING(name);
    destroy_ItemList(i);

    if (!p0->prv$v_sysprv) {
        Reset_Privs(p1);
        destroy_Privs(p1);
    }
    destroy_Privs(p0);

    if (iss == RMS$_RNF) return 0;
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    result = new_STRING2(dev.size+dir.size);
    memcpy(asciz_pSTRING(result), dev.s, dev.size);
    memcpy(asciz_pSTRING(result)+dev.size, dir.s, dir.size);
    return result;
}

pPrivs
Current_Privs(void)
{
    ROUTINE_NAME("current_Privs");
    int iss;
    pItemList i;
    IOSB iosb;
    pPrivs p;

    p = new_Privs();
    i = new_ItemList(1);
    i = add_Item(i, JPI$_CURPRIV, p, sizeof(Privs), 0);
    iss = sys$getjpiw(0,0,0,i,&iosb,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    if (VMS_ERR(iosb.status)) VMS_SIGNAL(iosb.status);
    destroy_ItemList(i);
    return p;
}

pPrivs
Authorized_Privs(void)
{
    ROUTINE_NAME("Authorized_Privs");
    int iss;
    pItemList i;
    IOSB iosb;
    pPrivs p;

    p = new_Privs();
    i = new_ItemList(1);
    i = add_Item(i, JPI$_AUTHPRIV, p, sizeof(Privs), 0);
    iss = sys$getjpiw(0,0,0,i,&iosb,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    if (VMS_ERR(iosb.status)) VMS_SIGNAL(iosb.status);
    destroy_ItemList(i);
    return p;
}

pPrivs
Image_Privs(void)
{
    ROUTINE_NAME("Image_Privs");
    int iss;
    pItemList i;
    IOSB iosb;
    pPrivs p;

    p = new_Privs();
    i = new_ItemList(1);
    i = add_Item(i, JPI$_IMAGPRIV, p, sizeof(Privs), 0);
    iss = sys$getjpiw(0,0,0,i,&iosb,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    if (VMS_ERR(iosb.status)) VMS_SIGNAL(iosb.status);
    destroy_ItemList(i);
    return p;
}

pPrivs
Process_Privs(void)
{
    ROUTINE_NAME("Process_Privs");
    int iss;
    pItemList i;
    IOSB iosb;
    pPrivs p;

    p = new_Privs();
    i = new_ItemList(1);
    i = add_Item(i, JPI$_PROCPRIV, p, sizeof(Privs), 0);
    iss = sys$getjpiw(0,0,0,i,&iosb,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    if (VMS_ERR(iosb.status)) VMS_SIGNAL(iosb.status);
    destroy_ItemList(i);
    return p;
}


pPrivs
AND_Privs(pPrivs p1, pPrivs p2)
{
    ROUTINE_NAME("AND_Privs");
    int j;
    pPrivs presult;
    union pu {
        word    w[4];
        Privs   p;
    } *q1, *q2, *qr;

    presult = new_Privs();
    q1 = (union pu *) p1;
    q2 = (union pu *) p2;
    qr = (union pu *) presult;

    for (j = 0; j<4; j++) {
        qr->w[j] = q1->w[j] & q2->w[j];
    }
    return presult;
}

pPrivs
OR_Privs(pPrivs p1, pPrivs p2)
{
    ROUTINE_NAME("OR_Privs");
    int j;
    pPrivs presult;
    union pu {
        word    w[4];
        Privs            p;
    } *q1, *q2, *qr;

    presult = new_Privs();
    q1 = (union pu *) p1;
    q2 = (union pu *) p2;
    qr = (union pu *) presult;

    for (j = 0; j<4; j++) {
        qr->w[j] = q1->w[j] | q2->w[j];
    }
    return presult;
}

pPrivs
NOT_Privs(pPrivs p1)
{
    ROUTINE_NAME("NOT_Privs");
    int j;
    union pu {
        word    w[4];
        Privs            p;
    } *q1, *qr;
    pPrivs presult;

    presult = new_Privs();

    q1 = (union pu *) p1;
    qr = (union pu *) presult;

    for (j = 0; j<4; j++) {
        qr->w[j] = ~q1->w[j];
    }
    return presult;
}

pPrivs
new_Privs(void)
{
    ROUTINE_NAME("new_Privs");
    pPrivs p;

    p = malloc(sizeof(Privs));
    if (!p) VMS_SIGNAL(SS$_INSFMEM);
    p->prv$l_l1_bits = 0;
    p->prv$l_l2_bits = 0;
    return p;
}

void
destroy_Privs(pPrivs p)
{
    ROUTINE_NAME("destroy_Privs");
    free(p);
}


int
sec2vms(int seconds, longword delta[2])
{
    ROUTINE_NAME("sec2vms");
    static $DESCRIPTOR (timeformat, "!UL !UL:!UL:!UL");
    word len;
    int iss, days, hours, mins;
    char tstring[100];
    $DESCRIPTOR(expire,"");
    expire.dsc$a_pointer = tstring;
    expire.dsc$w_length  = sizeof(tstring);

    days = seconds / 86400;
    seconds -= days * 86400;
    hours = seconds / 3600;
    seconds -= hours * 3600;
    mins = seconds / 60;
    seconds -= mins * 60;

    iss = sys$fao(&timeformat, &len, &expire, days, hours, mins, seconds);
    if (VMS_ERR(iss)) return iss;

    expire.dsc$w_length  = len;
    iss = sys$bintim(&expire, delta);
    return iss;
}


pSTRING
errormsg(int cond)
{
    ROUTINE_NAME("errormsg");
    int iss;
    word len;
    pSTRING s = new_STRING2(256);
    pSTRING s2;

    iss = sys$getmsg(cond, &len, s, 0, 0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    asciz_pSTRING(s)[len] = '\0';

    s2 = new_STRING(asciz_pSTRING(s));
    destroy_STRING(s);
    return s2;
}

int
define_logical(char *name, char *value, char *table)
{
    ROUTINE_NAME("define_logical");
    int iss = SS$_INSFMEM;
    pSTRING pname, ptable;
    pItemList i;

    if (!(pname  = new_STRING(name))) goto done;
    if (!(ptable = new_STRING(table))) goto done;

    if (!(i = new_ItemList(1))) goto done;
    i = add_Item(i, LNM$_STRING, value, strlen(value), 0);
    iss = sys$crelnm(0,ptable,pname,0,i);

done:
    if (i)      destroy_ItemList(i);
    if (pname)  destroy_STRING(pname);
    if (ptable) destroy_STRING(ptable);
    return iss;
}


#ifndef __ALPHA
int
__ATOMIC_INCREMENT_LONG(volatile void *p)
{
    int iss, l, *lp = (int *)p;

    iss = sys$setast(0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    l = *lp;
    *lp++;
    iss = sys$setast(1);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    return l;
}


int
__ATOMIC_DECREMENT_LONG(volatile void *p)
{
    int iss, l, *lp = (int *)p;

    iss = sys$setast(0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    l = *lp;
    *lp--;
    iss = sys$setast(1);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    return l;
}

#endif


#ifdef DEBUG_MEMORY
#undef free
#undef malloc
#undef realloc
#endif

void
my_free(void *pointer, char *routine)
{
    errlog(L_MEM|L_INFO,"[!AZ] free(!XL)",routine,pointer);
    free(pointer);
}

void *
my_malloc(size_t size, char *routine)
{
    void * p = malloc(size);
    errlog(L_MEM|L_INFO,"[!AZ] malloc(!UL) = !XL",routine,size,p);
    return p;
}

void *
my_realloc(void *pointer, size_t size, char *routine)
{
    void *p = realloc(pointer,size);
    errlog(L_MEM|L_INFO,"[!AZ] realloc(!XL,!UL) = !XL",routine,pointer,size,p);
    return p;
}



char *
dumpstring(char *s, int n)
{
    ROUTINE_NAME("dumpstring");
    int len = 1+(3*n-1)+1+1+1+n+1+1;
    int j;
    char *p, *p0;

    if (n < 0) return 0;

    p = p0 = malloc(len);
    if (!p) return 0;

    *p = 0;
    if (n == 0) return p;

    *p++ = '[';
    for (j = 0; j < n; j++) {
        char c = s[j];
        *p++ = TOHEX((c>>4) & 0x0F);
        *p++ = TOHEX(c & 0x0F);
        *p++ = ' ';
    }
    *(p-1) = ']';
    *p++ = ' ';
    *p++ = '|';

    for (j = 0; j < n; j++) {
        char c = s[j];
        if (!c) c = '.';
        *p++ = c;
    }
    *p++ = '|';
    *p = 0;
    printable(p0);
    return p0;
}

