
#include <std.h>

/* intel conversion program
	converts standard input to intel format onto standard output 
			example   cat bfile | intel > ifile
 */

#define CBCONST 1+2+4+2+2+1
#define BYTESPLIN 16
#define CBUFSIZ BYTESPLIN * 2 + CBCONST
#define ENDCODE 1
#define DATASTART 9
#define STARTADDR 0x100


char cbuf[CBUFSIZ];
int cbufi;
int cnum_dbytes;	/* number of data bytes */
int caddr;			/* starting load address */
int caddrsav;
int cchecksum;		/* checksum for record */
int ctype;			/* type of record 0=data 1=end */
int cbremain;		/* bytes remaining to fill record */

main()
{
int i;
char c;

init();
while (i = read(STDIN, &c, 1))
		use(c);
flush();
}


use(byte)
	char byte;
{
cchecksum += byte;
caddr++;
cnum_dbytes++;
chartohex(byte, &cbuf[cbufi]);
cbufi += 2;
if (!--cbremain) dorecord();
}

dorecord()
{
int i;

docount();
doaddr();
dorecordtype();
dochecksum();
cbuf[cbufi++] = '\n';
if ( ((i = write(STDOUT, cbuf, cbufi)) < 0) || (i != cbufi) )
	printf("Unable to write STDOUT\n");
clear();
}


init()
{
cbuf[0] = ':';
cbufi = DATASTART; 	/* third byte begins data */
cbremain = BYTESPLIN; 	/* byte remaining to fill record */
cnum_dbytes = 0;
caddr = STARTADDR;
caddrsav = caddr;
cchecksum = 0;
ctype = 0;
}

flush()
{
if (cnum_dbytes != 0)
	dorecord();
caddr = 0;
caddrsav = 0;
ctype = ENDCODE;
dorecord();
}

clear()
{
cbufi = DATASTART;	/* third byte begins data */
cbremain = BYTESPLIN;
cnum_dbytes = 0;
cchecksum = 0;
caddrsav = caddr;
}

/* ------------------------------------- */

chartohex(c)
	char c;
{
puthex(c, &cbuf[cbufi]);
}

docount()
{
char c;
c = cnum_dbytes;
puthex(c, &cbuf[1]);
}

doaddr()
{
char c;
c = (caddrsav >> 8)  & 0xff;
puthex(c, &cbuf[3]);
c = caddrsav & 0xff;
puthex(c, &cbuf[5]);
}

dorecordtype()
{
char c;
c = ctype;
puthex(c, &cbuf[7]);
}

dochecksum()
{
char ck;
cchecksum += (cnum_dbytes + (caddrsav & 0xff) + ((caddrsav >> 8) & 0xff) ) ;
ck = -cchecksum;
puthex(ck, &cbuf[cbufi]);
cbufi += 2;
}

puthex(c, where)
char c, *where;
{
char d;
char e;
#define HEXCHAR d + ( d > 9 ? 'A'-10 : '0' ) 

d = (c >> 4) & 0xf;
*(where++) = HEXCHAR ;
d = c & 0xf;
*where = HEXCHAR ;
}

