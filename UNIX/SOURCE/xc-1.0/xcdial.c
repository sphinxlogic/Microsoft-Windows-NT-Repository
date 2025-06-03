/*	xcdial.c -- dialing directory module for XC
	This file uses 4-character tabstops
	Author: Steve Manes 8/26/88
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <termio.h>
#include <fcntl.h>
#include "xc.h"

#define NAME	25	/* display for system name */
#define NUMBER	22	/*	  "		"  phone number */
#define BPS	 	 5	/*	  "		"  speed */
#define PROTO	 3	/*	  "		"  protocol */
#define SCRIPT	14	/*	  "		"  script name */

static FILE *dirf;
static long pages[57];	/* offsets into phonefile */
static short dirnum, thispage, lastpage;
static char *last_nbr = NIL(char);
static char format[46];
extern short s_flag;
extern void cl_end(), newbmask();

struct {
	char *speed;
} speed[] = {
	"300",
	"600",
	"1200",
	"2400",
	"4800",
	"9600",
#ifdef B19200
	"19200",
#endif
#ifdef B38400
	"38400",
	"57600",
#endif
	NIL(char)
};

struct {
	char *proto;
} proto[] = {
	"8N1",
	"7E2",
	"7O2",
	NIL(char)
};

/*	show a single, formatted dialdir entry.
	check its format integrity as we go
*/
static void 
showentry(choice, entry)
short choice;
char *entry;
{
	char name[NAME +1], num[NUMBER +1],
		bps[BPS +1], prot[PROTO+1], script[SCRIPT +1];
	char *s;
	int i, j;

	s = entry;

	/* get phone number */
	while (isspace(*s))
		s++;

	for (i=0; i < NUMBER && !isspace(*s); i++, s++)
		num[i] = *s;
	num[i]='\0';

	/* get name */
	while (!isspace(*s))
		s++;
	while (isspace(*s))
		s++;
	for (i=0; i < NAME && *s != '\t' && *s != '\n'; i++, s++)
		name[i] = *s;
	name[i] = '\0';

	/* get bps */
	memset(bps,0,BPS+1);
	if (s = strstr(entry, "BPS=")){
		s += 4;
		for (i=0; i < BPS && isdigit(*s); i++, s++)
			bps[i] = *s;
		for (i = 0, j = 0 ; speed[i].speed ; i++)
				if (!strcmp(bps,speed[i].speed))
					j++;
		if (!j){
			beep();
			sprintf(Msg,"Invalid BPS= for '%s'",name);
			S;
			return;
		}
	}

	memset(prot,0,PROTO+1);
	if (s = strstr(entry, "PROTO=")){
		s += 6;
		for (i=0; i < PROTO && isalnum(*s); i++, s++)
			prot[i] = *s;
		prot[i]='\0';
		uc_word(prot);
		for (i=0, j=0; proto[i].proto; i++)
			if (!strcmp(prot,proto[i].proto))
				j++;
		if (!j){
			beep();
			sprintf(Msg,"Invalid PROT= for '%s'",name);
			S;
			return;
		}
	}

	memset(script,0, SCRIPT+1);
	if (s = strstr(entry, "SCRIPT=")){
		s += 7;
		for (i=0; i < SCRIPT && !isspace(*s); i++, s++)
			script[i] = *s;
	}
	fprintf(tfp, format, choice, name, num, script, bps, prot);
}

/* scroll directory at current filepos */
static void
scroll_dir()
{
	short i;
	char buf[120];

	mode(OLDMODE);
	ttgoto(4, 0);
	cur_off();
	cl_end();

	fseek(dirf, pages[thispage], 0),
	dirnum = thispage * (LI - 6);
	for (i=0; i < LI - 6; i++){
		if (!fgets(buf, 120, dirf)){
			lastpage = thispage;
			break;
		}
		showentry(++dirnum, buf);
	}

	pages[thispage + 1] = ftell(dirf);
	if (!fgets(buf, 120, dirf))
		lastpage = thispage;
	cur_on();
	mode(NEWMODE);
}

/* Dial a phone number, using proper format and delay. */
void
xcdial(s)
char *s;
{
	char buffer[SM_BUFF];

	if (last_nbr)
		free(last_nbr);

	last_nbr = strdup(s);

	sprintf(buffer, DIALSTR, s);
	send_string(buffer);
}

static
parse_entry(buf)
char *buf;
{
	int i;
	char *s, *t, *nbr, bps[BPS+1], prot[PROTO+1];

	if (s = strchr(buf,'\n'))
		*s = '\0';

	if (s = strstr(buf, "BPS=")){
		s += 4;
		for (i=0; i < BPS && isdigit(*s); i++, s++)
			bps[i] = *s;
		bps[i]='\0';
		if (!mrate(bps)){
			S0("Invalid BPS=");
			return FAILURE;
		}
	}

	if (s = strstr(buf, "PROTO=")){
		s += 6;
		for (i=0; i < PROTO && isalnum(*s); i++, s++)
			prot[i] = *s;
		prot[i]='\0';
		uc_word(prot);
		if (!xc_setproto(prot)){
			S0("Invalid PROTO=");
			return FAILURE;
		}
	}

	cls();
	sprintf(Msg,"Calling %s",buf);
	S;

	if (s = strstr(buf, "PREFIX="))
		s += 7,
		send_string("\r"),
		send_string(s),
		send_string("\r"),
		s -= 7,
		*s = '\0',
		sleep(1);

	while (isspace(*buf) && *buf)
		buf++;

	if (!(*buf))
		return FAILURE;

	for (nbr = buf; !isspace(*buf) && *buf; buf++)
		;

	*buf = '\0';
	xcdial(nbr);

	if (s = strstr(++buf, "SCRIPT=")){
		s += 7;
		t = s;
		while (*t && !isspace(*t))
			t++;
		*t = '\0';
		sprintf(ddsname,"%s",s);
		s_flag = linkflag = TRUE;
	}
	return SUCCESS;
}

static
dial_entry(choice)
short choice;
{
	char buf[120];

	if (!choice)
		return FAILURE;
	rewind(dirf);
	while (choice--){
		if (!fgets(buf, 120, dirf)){
			S0("Nonexistent entry");
			return FAILURE;
		}
	}
	return (parse_entry(buf));
}

static
man_dial()
{
	ttgoto(LI-1, 0);
	cl_end();
	fputs("Number to dial: ",tfp);
	getline();
	if (!line[0])
		return FAILURE;
	return (parse_entry(line));
}

dial_dir()
{
	int i, c;
	char buf[5];

	if (!(dirf = openfile(phonefile))){
		sprintf(Msg,"Phonelist '%s' not found",phonefile);
		S;
		return FAILURE;
	}

	dirnum = thispage = 0;
	lastpage = -1;
	cls();
	drawline(0, 0, CO);
	ttgoto(1,(CO-strlen(phonefile))/2 -1);
	show(-1,phonefile);
	drawline(2, 0, CO);
	ttgoto(3, 0);
	sprintf(format,"     %%-%ds %%%ds %%-%ds %%%ds %%%ds%*s\n\r",
		NAME, NUMBER, SCRIPT, BPS, PROTO,
		CO-NAME-NUMBER-BPS-PROTO-SCRIPT-7, "");
	sprintf(Msg, format, "NAME", "NUMBER", "SCRIPT", "BPS", "PRO");
	show(-1,Msg);
	sprintf(format,"%%3d - %%-%ds %%%ds %%-%ds %%%ds %%%ds\n\r",
		NAME, NUMBER, SCRIPT, BPS, PROTO);
	scroll_dir();
	for (;;){
		ttgoto(LI-1, 0);
		fputs(
		"==>     [#] Dial Entry   [M]anual Dial   [X]it   [N]ext   [P]revious",			tfp);
		ttgoto(LI-1, 4);
		while (1){
			c = toupper(fgetc(stdin));
			if (c == BS)
				continue;
			if (c == 'N' || c == '\n' || c == ' '){
				if (thispage > (int)((1000/(LI-6))-1) || thispage == lastpage)
					S0("Last page");
				else
					thispage++,
					scroll_dir();
				break;
			}
			else if (c == 'P' && dirnum > 1){
				if (!thispage)
					S0("First page");
				else
					thispage--,
					scroll_dir();
				break;
			}
			else if (c == 'X'){
				cls();
				fclose(dirf);
				return FAILURE;
			}
			else if (c == 'M'){
				if (man_dial()){
					fclose(dirf);
					reterm = TRUE;
					return SUCCESS;
				}
				reterm = FALSE;
				break;
			}
			else if (isdigit(c)){
				buf[0] = c;
				fputc(c,tfp);
				for (i=1; i<4; ++i){
					buf[i] = getchar();
					if (buf[i]==BS){
						if (i>0)
							fputs("\b \b",tfp),
							i -= 2;
						else
							i = -1;
						continue;
					}
					fputc(buf[i],tfp);
					if (buf[i]=='\n' || buf[i]=='\r')
						break;
				}
				if (!i){
					reterm = FALSE;
					break;
				}
				buf[++i] = '\0';
				if (dial_entry(atoi(buf))){
					fclose(dirf);
					reterm = TRUE;
					return SUCCESS;
				}
				reterm = FALSE;
				break;
			}
		}
	}
}

redial()
{
	char *s;

	if (!last_nbr){
		S1("REDIAL FAILURE");
		return -1;
	}

	s = strdup(last_nbr);
	xcdial(s);
	free(s);
	return SUCCESS;
}
