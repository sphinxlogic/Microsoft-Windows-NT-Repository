/********************************************************************
 * lindner
 * 3.1
 * 1994/02/20 16:15:46
 * /home/mudhoney/GopherSrc/CVS/gopher+/gopher/form.h,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 92, 93, 94 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: form.h
 * Abstraction of form definition and management functions
 *********************************************************************
 * Revision History:
 * form.h,v
 * Revision 3.1  1994/02/20  16:15:46  lindner
 * New form definition and management functions
 *
 *
 *********************************************************************/

#include "String.h"
#include "DAarray.h"
#include "STAarray.h"
#include "boolean.h"

/*
 * An ITEM is a single UI control type thing..  Examples include labels,
 * choose items, etc..
 */

#define ITEM_UNINIT 0
#define ITEM_LABEL  1
#define ITEM_PROMPT 2
#define ITEM_CHOICE 3
#define ITEM_SELECT 4
#define ITEM_PASSWD 5
#define ITEM_LONG   6

typedef int ITEMtype;

typedef struct ITEM_struct {
     ITEMtype type;
     String   *label;
     String   *response;

     int      chooseitem;
     StrArray *choices;
} ITEM;


#define ITEMsetPrompt(f,l)    (STRset((f)->label,(l)))
#define ITEMgetPrompt(f)      (STRget((f)->label))

#define ITEMgetType(f)       ((f)->type)
#define ITEMsetType(f,t)     ((f)->type = (t))

#define ITEMsetLabel(f,l)    (STRset((f)->label,(l)))
#define ITEMgetLabel(f)      (STRget((f)->label))

#define ITEMgetResponse(f)   (STRget((f)->response))
#define ITEMsetResponse(f,r) (STRset((f)->response,(r)))

#define ITEMgetChoice(f)     ((f)->chooseitem)
#define ITEMsetChoice(f,c)   ((f)->chooseitem=(c))

#define ITEMgetChoiceNum(f,i)   (STAgetText((f)->choices,i))
#define ITEMgetNumChoices(i) (STAgetTop((i)->choices))  

ITEM *ITEMnew();
void  ITEMinit();
ITEM *ITEMcpy();
void  ITEMdestroy();
void  ITEMpushChoice();

/********************************************************************
 * 
 * A FORM is an array of items
 *
 */

typedef DynArray FORM;
#define FORMnew(a)        (DAnew((a),ITEMnew,ITEMinit,ITEMdestroy,ITEMcpy))
#define FORMinit(a)       (DAinit((a)))
#define FORMgetTop(a)     (DAgetTop(a))
#define FORMgetEntry(a,b) (ITEM*)(DAgetEntry(a,b))

#define FORMpush(a,b)     (DApush((DynArray*)(a),(b)))
#define FORMpop(a)        (FORM*)(DApop(a))
#define FORMdestroy(a)    (DAdestroy(a))
#define FORMcpy(a,b)      (DAcpy(a,b))

boolean FORMaddLabel();
boolean FORMaddPrompt();
boolean FORMaddPasswd();
boolean FORMaddChoice();
boolean FORMaddSelect();
char    **FORMdoAskBlock();



