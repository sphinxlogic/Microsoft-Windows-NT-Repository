#include "trydcc.h"
#include <assert.h>
#include <stdlib.h>

#define TWO		1+1
#define BIT2		TWO
#define _min(x,y)	(((x) > (y))? (y) : (x))
#define _changeSign(x,y) x = -x; y = -y

typedef unsigned int bool;
#define FALSE	(0!=0)
#define TRUE	(0==0)

typedef struct {bool fl1:1, fl2:1, fl3:1;} Flags;
typedef enum {AlrdDefId, AlrdQual, ArrExptd, ArrToPtr} ErrNb;
static char *errMsg[] = {
  "...",
  "...",
  "..."
  /*~zif __index() != __extent(ErrNb) "Missing message(s) in 'errMsg[]'" */
};

char *tryAtDcc(void)
{
  unsigned int a, b=0, d=b, *pi=malloc(sizeof(pi));
  const unsigned int *cpi = &a;
  void *pv;
  Flags fl = {TRUE, FALSE /*~zif !__member(fl3) "Incorrect initialization "
	  "of flQual" */};  /* Exemple too simple to be really significant,
			       but will detect added field in struct without
			       corresponding change in initialization. */

  char buf[10], c;
  short s = 32768;
  a = a - TWO;
  a = _min(a++,b);
  if (b = 0);
  else _changeSign(a, b);
  tryAtDcc; b;
  x = 0;
  x++;  /* 'UndecldVar' message not repeated (except if '+zae' option used) */
  c = *errMsg[sizeof(errMsg)];
  if (a<=0 || a-b>d || a&BIT2==BIT2) return &buf[0];
  for(;;)
    c = 128;
  pi = pv = &fl;
  if (pi == NULL) pi = cpi;
  switch (a) {
    case 0: case 1: b=1;
    default: b = 0;}
} 

typedef double Volt, Amp, Watt;
/*~TypeCombination Volt*Amp->Watt */

static int tstTypCmbn(int *pi)
{
  Volt v=0; Amp i=1; Watt p=i*v;

  if (pi == NULL) p = v*v;
  return AlrdQual+ArrToPtr;
}
