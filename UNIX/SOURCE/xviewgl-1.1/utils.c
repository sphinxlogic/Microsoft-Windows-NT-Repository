#include "patchlevel.h"
/* Written by Brad Daniels, February 1991

   This code is provided as is, with no warrantees, express or implied.
   I believe this code to be free of encumbrance, and offer it to the
   public domain.  I ask, however, that this paragraph and my name be
   retained in any modified versions of the file you may make, and
   that you notify me of any improvements you make to the code.

   Brad Daniels (daniels@bigred.enet.dec.com)
*/
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#ifdef STDLIB
#include <stdlib.h>
#endif
#include "my_types.h"
#include "utils.h"
#include "libutils.h"

char *tokenize(char *cmd)
{
static char *nextc;
char *tok;
    if (cmd!=NULL)
	nextc=cmd;
    while (*nextc && isspace(*nextc)) nextc++;
    if ((*nextc == '\0') || (*nextc == '\032'))
	return NULL;
    if (*nextc == ',') {
	nextc++;
	return "-1";
    }
    if (*nextc == '"') {
	tok = nextc++;
	while ((*nextc != '"') && (*nextc != '\032') && (*nextc)) nextc++;
	if (*nextc=='"') {
	    *(nextc++) = 0;
	}
    } else {
	tok=nextc;
	while (*nextc && (*nextc != ',') && (*nextc != '\032')
	       && !isspace(*nextc)) nextc++;
	if (*nextc) {
	    *(nextc++) = 0;
	}
    }
    while (*nextc && isspace(*nextc)) nextc++;
    if (*nextc==',') {
	nextc++;
    }
    return tok;
}

char *downcase(char *s)
{
char *p=s;
    while (*p) {
	*p = my_tolower(*p);
	p++;
    }
    return s;
}

char *preprocess_line(char *s)
{
char *p=s;
    while (*p) {
	if ((*p == '\r')  || (*p == '\032') || (*p == ';')) {
	    /* Strip comment, EOF char, or carriage return */
	    *p = 0;
	} if (*p=='"') {
	    p++;
	    while (*p && (*p != '"')) p++;
	    if (*p) p++;
	} else {
	    *p = my_tolower(*p);
	    p++;
	}
    }
    return s;
}

char *extend(char *s, char *ext)
{
char *p = strrchr(s,'/');
    if (p==NULL)
	p = strchr(s,'.');
    else
	p = strchr(p,'.');
    if (p==NULL)
	return strcat(s,ext);
    else
	return s;
}
