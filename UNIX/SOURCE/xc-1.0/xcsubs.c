/*	xcsubs.c -- subroutines for XC
	This file uses 4-character tabstops
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>
#include <termio.h>
#ifdef T6000
#include <sys/ioctl.h>
#endif
#include <setjmp.h>
#include "xc.h"

extern jmp_buf erret;

char line[SM_BUFF],	/* Input line */
	 word[SM_BUFF],	/* Parsed word */
	 *wptr, *lptr,	/* Word and line pointers */
	 *tgetstr(), *tgoto();
int	LI,	/* One less than screen length in termcap entry */
	CO;	/* Screen width */
short ospeed;
static char tc[LG_BUFF],	/* termcap buffer */
			tbuf[LG_BUFF], PC, *CD, *CE, *CF, *CL, *CM, *CN, *SO, *SE;

void show();

#define Tgetstr(code) ((s = tgetstr(code,&p)) ? s : "")

#if	!STRSTR		/* For those that do not have strstr() */
/*	Find first occurence of str2 in str1 and return a pointer to it */
char *
strstr(str1, str2)
char *str1, *str2;
{
	register char *Sptr, *Tptr;
	int len = strlen(str1) -strlen(str2) + 1;

	if (*str2)
		for (; len > 0; len--, str1++){
			if (*str1 != *str2)
				continue;

			for (Sptr = str1, Tptr = str2; *Tptr != '\0'; Sptr++, Tptr++)
				if (*Sptr != *Tptr)
					break;

			if (*Tptr == '\0')
				return str1;
		}

	return NIL(char);
}
#endif

#if !DUP2		/* For those that do not have dup2() */
#include <fcntl.h>
dup2(oldfd, newfd)
int oldfd, newfd;
{
	if (fcntl(oldfd, F_GETFL, 0) == -1)		/* Valid file descriptor? */
		return (-1);						/* No, return an error */
	close(newfd);							/* Ensure newfd is closed */
	return (fcntl(oldfd, F_DUPFD, newfd));	/* Dup oldfd into newfd */
}
#endif /* !DUP2	Thanks to Bill Allie CIS: 76703,2061 */

#if !STRDUP	/* For those that do not have strdup() */
char *
strdup(s)
char *s;
{ return strcpy((char *)malloc(strlen(s)+1), s); }
#endif

#if !MEMSET		/* For those that do not have memset() */
char *
memset(dst, chr, len)
char *dst;
register chr, len;
{
	char *d;
	for (d = dst; --len >= 0; *d++ = chr)
		;
	return dst;
}
#endif

void
msecs(t)
long t;
{
	long start;
	struct tms Tbuf;

	start = times(&Tbuf);
	while ((times(&Tbuf)-start) < (t*HZ)/1000)
		;
}

/*	Do the fork call, packaging the error return so that the caller
	need not have code for it.
*/
forkem()
{
	int i;

	if ((i = fork()) < 0){
		S1("XC: Fork failed");
		longjmp(erret,1);
	}
	return i;
}

/*	Throw away all input characters until no more are sent. */
void 
purge(void) { while (readbyte(1) != -1) ; }

/*	Line input routine to be used when the raw terminal mode is in effect. */
void 
getline()
{
	int c, i = 0;
	char *ptr;

	lptr = line;
	memset(line, 0, SM_BUFF);

	while ((c = getchar()) != '\r' && c != '\n'){
		if (c == BS){
			if (i > 0){
				ptr = unctrl(line[--i]);
				line[i] = '\0';
				while (*ptr++ != '\0')
					fputs("\b \b",tfp);
			} else
				beep();
			continue;
		}
		if (c == LK){
			while (i > 0){
				ptr = unctrl(line[--i]);
				while (*ptr++ != '\0')
					fputs("\b \b",tfp);
			}
			memset(line, 0, SM_BUFF);
			continue;
		}
		if (c == ('v' & 0x1f))
			c = getchar();

		line[i++] = c;
		fputs(unctrl(c), tfp);
	}
}

/*	Parse the "line" array for a word */
void 
getword()
{
	char quote, *ptr = word;
	short carat = FALSE, backslash = FALSE;

	wptr = lptr;
	memset(word, 0, SM_BUFF);

	while (isspace(*lptr))
		lptr++;

	if (*lptr == '\0')
		return;

	if (*lptr == '\'' || *lptr == '\"')
		quote = *lptr++;
	else
		quote = '\0';

	for (; *lptr != '\0'; lptr++){
		if (quote){
			if (*lptr == '\0'){
				word[0] = '\0';
				sprintf(Msg,"Unmatched quote: %s", line);
				S;
				s_exit();
			}
			if (*lptr == quote)
				break;
		} else if (!backslash && isspace(*lptr))
			break;

		if (carat)
			*ptr++ = *lptr & 0x1f,
			carat = FALSE;
		else if (backslash)
			*ptr++ = *lptr,
			backslash = FALSE;
		else if (*lptr == '^')
			carat = TRUE;
		else if (*lptr == '\\')
			backslash = TRUE;
		else
			*ptr++ = *lptr;
	}

	lptr++;
}

/*	Make the specified word all lower case */
void 
lc_word(ptr)
char *ptr;
{
	while (*ptr){
		*ptr = tolower(*ptr);
		ptr++;
	}
}

/*	Make the specified word all upper case */
void uc_word(ptr)
char *ptr;
{
	while (*ptr){
		*ptr = toupper(*ptr);
		ptr++;
	}
}

void 
mode(flag)
int flag;
{
	if (flag == NEWMODE)
		ioctl(0, TCSETAF, &newmode);
	else if (flag == SIGMODE)
		ioctl(0, TCSETAF, &sigmode);
	else if (flag == OLDMODE)
		ioctl(0, TCSETAF, &oldmode);
}

beep()
{
	putc(7,tfp);
	return SUCCESS;
}

/*	initialize termcap stuff */
void 
get_ttype()
{
	char *ttytype;
	char *p = tbuf;
	char *s;

	if (!(ttytype = getenv("TERM"))){
		S1("TERM not set");
		exit(6);
	}
	if (tgetent(tc, ttytype) != 1){
		sprintf(Msg,"Can't load %s", ttytype);
		S;
		exit(7);
	}
	ospeed = newmode.c_cflag & CBAUD;
	LI = tgetnum("li") - 1;
	CO = tgetnum("co");
	if (!(s=Tgetstr("pc")))
		PC = '\0';
	else
		PC = *s;
	
	CD = Tgetstr("cd");
	CE = Tgetstr("ce");
	CL = Tgetstr("cl");
	CM = Tgetstr("cm");
	SE = Tgetstr("se");
	SO = Tgetstr("so");
	CF = Tgetstr("CF");
	CN = Tgetstr("CN");
	if (CF && ! CN)
		CN = Tgetstr("CO");
}

/*	putchr() is a routine to pass to tputs() */
void
putchr(int c) { putc(c,tfp); }

void
cls(void) { tputs(CL,LI,putchr); }

void
cur_on(void) { tputs(CN,1,putchr); }

void
cur_off(void) { tputs(CF,1,putchr); }

void
cl_line(void) { tputs(CE,1,putchr); }

void
cl_end(void) { tputs(CD,LI,putchr); }

void 
ttgoto(int row, int col) { tputs(tgoto(CM, col, row),1,putchr); }

void 
drawline(row, col, len)
int row, col, len;
{

	ttgoto(row, col);
	while (len--)
		fputc('-', tfp);
}

void 
show(flag, str)
short flag;
char *str;
{
	if (!flag){
		beep();
		ttgoto(LI,0),
		cl_line(),
		ttgoto(LI,(CO-strlen(str))/2 -1);
	}
	if (flag == 2)
		putc('\n',tfp),
		putc('\r',tfp);
	tputs(SO,1,putchr);
	putc(' ',tfp);
	fputs(str, tfp);
	putc(' ',tfp);
	tputs(SE,1,putchr);
	if (flag > 0)
		putc('\n',tfp),
		putc('\r',tfp);
}

void 
show_abort(void) { S2("USER ABORT"); }

FILE *
isregfile(pathname)
char *pathname;
{
	struct stat statbuf;

	if (stat(pathname,&statbuf) || (statbuf.st_mode & S_IFMT) != S_IFREG)
		return NIL(FILE);
	return fopen(pathname, "r");
}


FILE *
openfile(name)
char *name;
{
	FILE *fp = NIL(FILE);
	char *home, fullname[SM_BUFF], *path, *pathend;
	int pathlen;

	if ((path = getenv("XC_PATH"))){
		while (!fp){
			if (!(pathend = strchr(path, ':')))
				pathlen = strlen(path);
			else
				pathlen = pathend - path;

			sprintf(fullname, "%.*s/%s", pathlen, path, name);
			fp = isregfile(fullname);

			path += pathlen;
			if (*path == '\0')
				break;
			path++;
		}
	}

	if (!fp)
		fp = isregfile(name);
	
	if (!fp){
		if ((home = getenv("HOME")))
			sprintf(fullname, "%s/%s", home, name);
		fp = isregfile(fullname);
	}

	if (!fp){
		sprintf(fullname, "%s/%s", LIBDIR, name);
		fp = isregfile(fullname);
	}

	return fp;
}

/*	Translate the character specified by 'c' to its ASCII display name.
	Note:	This routine is specific to the ASCII character set.
*/
char *
unctrl(c)
int c;
{
	static char buffer[3], buf1[2];

	memset(buffer, 0, 3);
	memset(buf1, 0, 2);

	if (c == 0x7f)
		strcpy(buffer, "^?");
	else {
		if (iscntrl(c))
			strcpy(buffer, "^"),
			c += '@';
		buf1[0] = c;
		strcat(buffer, buf1);
	}

	return buffer;
}
