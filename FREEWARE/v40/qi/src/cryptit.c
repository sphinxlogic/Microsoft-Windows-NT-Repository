/***********************************************************************/
/* pure dummy. Does nothing. */

#define ROTORSZ 256
#define MASK 0377

#ifdef VMS
#include <types.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <signal.h>
#include <stdio.h>
#include <ctype.h>
/*# include <sgtty.h>
#include <strings.h> */
#include <string.h>

char  t1[ROTORSZ];
char  t2[ROTORSZ];
char  t3[ROTORSZ];

int   Encrypting = 0;
static int ccnt, nchars, n1, n2;
char   *Visible();
int   crypt_init();
int   decode();
int   encode();
int   threecpy();

crypt_start(pass)
char   *pass;
{
 return;
}


char   *
decrypt(to, from)
char   *to;
char   *from;
{
  char  scratch[4096];
  char   *sp;
  int   count;
  *to = *from;
  return (0);
}

decrypt_end()
{
  Encrypting = 0;
}

#define DEC(c)  (((unsigned char)(c) - '#') & 077)

decode(to, from)
unsigned char *to;
unsigned char *from;
{
  int   n;
  int   chars;

  *from = *to;
}

crypt_init(pw)
char   *pw;
{
}

encryptit(to, from)
char   *to;
char   *from;
{
  *from=*to;
  return *to;
}
#define ENC(c) (((unsigned char)(c) & 077) + '#')

encode(out, buf, n)       /* output a line of binary (up to 45 chars)
		 * in */
int   n;            /* a readable format, converting 3 chars to 4  */
char   *buf;
char   *out;
{
  return(n);
}

threecpy(to, from)
unsigned char *to;
unsigned char *from;
{
 return;
}
