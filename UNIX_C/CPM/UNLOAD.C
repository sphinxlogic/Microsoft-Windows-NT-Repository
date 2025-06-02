/*
 * unload [files ...]
 */

#include <stdio.h>
#include <ctype.h>
#define SIZE 16

main(argc,argv)
char *argv[];
{
	FILE *f;

	if(argc > 1)
		while(--argc) {
			if( (f = fopen(argv[argc], "r")) == NULL) {
				perror(argv[argc]);
				exit(1);
			}
			printf("%s;\n",argv[argc]);
			unload(f);
			fclose(f);
		}
	else
		unload(stdin);
}

unsigned char check;

unload(f)
FILE *f;
{
	char buf[SIZE];
	register int n,i;
	int addr = 0x100;

	while((n = fread(buf, 1, SIZE, f)) > 0) {
		check = 0;
		putchar(':');
		putbyte(n);
		putbyte(addr>>8);
		putbyte(addr&0xff);
		putbyte(0);
		addr += n;
		for(i = 0; i<n; i++)
			putbyte(buf[i]);
		putbyte( (-check) & 0xff);
		putchar('\n');
	}
	printf(":00010001FE\n");
}

static char hexcode[] = "0123456789ABCDEF";

putbyte(b)
register int b;
{
	putchar( hexcode[(b>>4) & 0xf] );
	putchar( hexcode[b & 0xf] );
	check += (unsigned char) (b & 0xff);
}
