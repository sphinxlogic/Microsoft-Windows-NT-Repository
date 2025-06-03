#include "patchlevel.h"
/* This doesn't handle prototype args within the declaration for a pointer
   to a function type. */
#include <ctype.h>
#include <stdio.h>
#define ARGBUFSIZE 1024
#define my_isalnum(c) ((c=='_') || isalnum(c))
#define my_isalpha(c) ((c=='_') || isalpha(c))

int c;
main() {
int possible=0, braces=0, gotslash=0, gotstar=0, gotnl=0;
    for (c=getchar(); c != -1;) {
	if (gotslash && (c=='*')) {
	    putchar(c);
	    c=getchar();
	    do {
		putchar(c);
		gotstar = (c == '*');
	    } while (((c=getchar()) != -1) && (!gotstar || (c != '/')));
	    continue;
	} else if (gotnl && (c=='#')) {
	    do {
		putchar(c);
		gotslash=(c=='\\');
	    } while (((c=getchar()) != -1) && (gotslash || (c!='\n')));
	}
	gotslash = (c=='/');
	gotnl = (c=='\n');
	if (c=='{') {
	    braces++;
	} else if (c=='}') {
	    braces--;
	} else if (!braces) {
	    if (c==';') {
		possible=0;
	    } else if (c=='(') {
		if (possible) {
		    putchar(c);
		    c=getchar();
		    convert_arglist();
		    possible=0;
		    continue;
		}
	    } else if (my_isalpha(c)) {
		possible=1;
		putchar(c);
		while (((c=getchar()) != -1) && my_isalnum(c))
		    putchar(c);
		continue;
	    }
	}
	putchar(c);
	c=getchar();
    }
    exit(0);
}


convert_arglist()
{
char argbuf[ARGBUFSIZE],argc;
char typebuf[ARGBUFSIZE];
char protobuf[ARGBUFSIZE];
char lastword[ARGBUFSIZE];
int parens=1,len=0,pi,ti,ai,li;
int wordc;
    lastword[0]=0;
    typebuf[0]=0;
    while((c!=-1) && parens) {
	if (c=='(')
	    parens++;
	if (c==')')
	    parens--;
	protobuf[len++]=c;
	c = getchar();
    }
    while ((c != -1) && isspace(c)) {
	protobuf[len++]=c;
	c = getchar();
    }
    protobuf[len]=0;
    if (c == ';') {
	printf(");");
	c = getchar();
	return;
    }
    for (wordc=pi=ti=ai=li=0,parens=1; (pi<len) && parens;) {
	if ((parens==1) && (protobuf[pi]==',')) {
	    sprintf(argbuf+ai,"%s, ",lastword);
	    ai=strlen(argbuf);
	    typebuf[ti++]=';';
	    typebuf[ti++]='\n';
	    pi++;
	    if (wordc==1) {
		printf("%s",protobuf);
		return;
	    }
	    wordc = 0;
	} else {
	    if (protobuf[pi]=='(') parens++;
	    if (protobuf[pi]==')') parens--;
	    if (parens==0) {
		if (ti >0) {
		    typebuf[ti++]=';';
		    typebuf[ti++]='\n';
		}
		pi++;
		break;
	    }
	    typebuf[ti++]=protobuf[pi];
	    li=0;
	    if (my_isalpha(protobuf[pi])) {
		lastword[li++]=protobuf[pi++];
		wordc++;
		while ((pi<len) && my_isalnum(protobuf[pi])) {
		    typebuf[ti++]=protobuf[pi];
		    lastword[li++]=protobuf[pi];
		    pi++;
		}
		lastword[li]=0;
	    } else
		pi++;
	}
    }
    if (wordc<=1) {
	printf("%s",protobuf);
	return;
    }
    typebuf[ti]=0;
    sprintf(argbuf+ai,"%s)\n",lastword);
    printf("%s",argbuf);
    printf("%s",typebuf);
    if (protobuf[pi])
	printf("%s\n",protobuf+pi);
}
