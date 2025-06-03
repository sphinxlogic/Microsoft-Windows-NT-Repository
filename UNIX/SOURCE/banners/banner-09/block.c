#include <stdio.h>
#include <string.h>

#define LINES		  10
#define BUFFERSIZE	1024

extern char	*data[][LINES];

char		buffer[LINES][BUFFERSIZE+1];

int min(a,b)
int	a,
	b;
{
    if (a>b)
	return(b);
    return(a);
}

main(argc,argv)
int	argc;
char	**argv;
{
    short	i,
		j,
		k,
		l,
		m,
		idx,
		idx2,
		al,
		sw,
		tmp,
		tmp2;
    char	*p,
		*ptr;

    while (--argc) {
	argv++;
/*
	for (i=0; i<LINES; i++) {
	    p = *argv;
	    while (*p != '\0')
		printf("%s ",data[*p++][i]);
	    printf("\n");
	}
*/
	p = *argv;
	idx = 0;
	idx2 = 0;
	for (i=0; i<LINES; i++)
	    for (j=0; j<BUFFERSIZE; j++)
		buffer[i][j] = ' ';
	while (*p != '\0') {
	    al = strlen(data[*p][0]);
	    sw = al;
	    l = 0; m = 0;
	    for (i=0; i<LINES; i++) {
		for (j=1; j<=idx2 && buffer[i][idx-j]==' '; j++);
		for (k=0; *p!=' ' && k<al && data[*p][i][k]==' '; k++);
		sw = min(sw,j+k);
		if (i) {
		    sw = min(sw,j+m);
		    sw = min(sw,l+k);
		}
		l = j; m = k;
	    }
	    if (sw)
		--sw;
	    tmp = idx + al - sw;
	    tmp2 = idx-sw+1;
	    for (i=0; i<LINES; i++)
		for (j=0; j<min(al,BUFFERSIZE-tmp); j++)
		    if (data[*p][i][j]!=' ')
			buffer[i][tmp2+j]=data[*p][i][j];
	    idx2 = ( *p==' ' ? 0 : tmp);
	    idx = tmp + 1;
	    p++;
	    if (idx>=BUFFERSIZE) {
		idx = BUFFERSIZE;
		*p = '\0';
	    }
	}
	for (i=0; i<LINES; i++) {
	    buffer[i][idx] = '\0';
	    printf("%s\n",buffer[i]);
	}
    }
}
