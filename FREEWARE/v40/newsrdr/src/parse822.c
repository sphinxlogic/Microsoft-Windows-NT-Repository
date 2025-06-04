/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	RFC822 address handling routines
**
**  MODULE DESCRIPTION:
**
**  	This module contains routine PARSE822.
**
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1992, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  25-JUN-1992
**
**  MODIFICATION HISTORY:
**
**  	25-JUN-1992 V1.0    Madison 	Initial coding.
**  	07-AUG-1992 V1.1    Madison 	Update for Alpha.
**  	04-SEP-1992 V1.2    Madison 	Add full-address parameter.
**  	10-DEC-1992 V1.2-1  Madison 	LIB$TABLE_PARSE now in lib$routines.h.
**--
*/
#include "newsrdr.h"
#include "tpadef.h"

    extern int parse822_state(), parse822_key();

#define TPA_C_LENGTH	(TPA$C_LENGTH0+20)
#define TPA_K_COUNT 	(TPA$K_COUNT0+5)

    struct TPABLK {
    	struct tpadef tpa0;
    	int	*tpa_l_adrlen;
    	char    *tpa_l_adrbuf;
    	int	*tpa_l_namlen;
    	char    *tpa_l_nambuf;
    	int	*tpa_l_nomore;
    };
/*
** The following are codes used in both PARSE822_TABLES.MAR
** and in routine parse822_store below.  They must match!!
*/
#define P8_K_STORE_MBOX	231422
#define P8_K_APPND_MBOX	861324
#define P8_K_STORE_NAME	318417
#define P8_K_NO_MORE	627409

/*
**++
**  ROUTINE:	parse822
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Takes a string containing a comma-separated list of RFC822-type
**  	addresses and extracts the address part and the "personal name".
**
**  	--N.B.--  RFC822 Group lists are _NOT_ handled!!!
**
**  	--N.B.--  NOT Reentrant!!
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	parse822(char *instr, void *contxt, char **full, char **adr, char **nam)
**
** instr:   char_string, character string (ASCIZ), read only, by reference
** contxt:  arbitrary, longword (unsigned), modify, by reference
** full:    pointer, longword (unsigned), write only, by reference
** adr:	    pointer, longword (unsigned), write only, by reference
** nam:	    pointer, longword (unsigned), write only, by reference
**
**  >> Set contxt to 0 before first call; routine will iterate over entire
**  >> string, updating contxt during processing.
**
**  IMPLICIT INPUTS:	tmpbuf (static buffer)
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**  	SS$_NORMAL: 	normal successful completion.
**  	SS$_ENDOFFFILE:	no more addresses were found in the list.
**  	Other values from LIB$TPARSE/LIB$TABLE_PARSE are possible.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int parse822(char *instr, char **contxt, char **fulladr, char **adr,
    	    	    	    	    	    	    	    	char **nam) {
    
    struct TPABLK tpablk;
    int status, adrlen, namlen, nomore, inq, ina, inc;
    char *cp, *fcp;
    static char adrbuf[STRING_SIZE+1], nambuf[STRING_SIZE+1];
    static char fulbuf[STRING_SIZE+1];

    if (*contxt == ((char *) -1)) return SS$_ENDOFFILE;
    if (*contxt == NULL) *contxt = instr;

/*
** Locate the next address in the comma-separated list, trimming blanks
** and such.
*/
    fcp = fulbuf;
    for (cp = *contxt; isspace(*cp); cp++);
    inq = ina = inc = 0;
    while (*cp) {
    	if (inq) {
    	    if (*cp == '"') inq = 0;
    	    if (*cp == '\\') *fcp++ = *cp++;
    	    *fcp++ = *cp++;
    	} else if (inc) {
    	    if (*cp == ')') inc = 0;
    	    if (*cp == '"') inq = 1;
    	    if (*cp == '\\') *fcp++ = *cp++;
    	    *fcp++ = *cp++;
    	} else if (ina) {
    	    if (*cp == '>') ina = 0;
    	    if (*cp == '"') inq = 1;
    	    if (*cp == '\\') *fcp++ = *cp++;
    	    *fcp++ = *cp++;
    	} else {
    	    if (*cp == ',') break;
    	    if (*cp == '"') inq = 1;
    	    if (*cp == '<') ina = 1;
    	    if (*cp == '(') inc = 1;
    	    if (*cp == '\\') *fcp++ = *cp++;
    	    *fcp++ = *cp++;
    	}
    }

/*
** Update context for next call
*/
    if (*cp) {
    	*contxt = cp + 1;
    } else {
    	*contxt = (char *) -1;
    }
    while ((fcp > fulbuf) && isspace(*(fcp-1))) fcp--;
    *fcp = '\0';
    if (fulladr) *fulladr = fulbuf;

/*
** Prepare for initial parse attempt
*/
    memset(&tpablk, 0, TPA_C_LENGTH);

    tpablk.tpa0.tpa$l_count = TPA_K_COUNT;
    tpablk.tpa0.tpa$l_options = TPA$M_BLANKS;
    tpablk.tpa0.tpa$l_stringcnt = fcp-fulbuf;
    tpablk.tpa0.tpa$l_stringptr = fulbuf;
    tpablk.tpa_l_adrlen	= &adrlen;
    tpablk.tpa_l_adrbuf = adrbuf;
    tpablk.tpa_l_namlen = &namlen;
    tpablk.tpa_l_nambuf = nambuf;
    tpablk.tpa_l_nomore = &nomore;

    adrlen = namlen = nomore = 0;
    status = table_parse(&tpablk, (void *) parse822_state, (void *) parse822_key);

/*
** Handle the case where the parse failed because the leading phrase
** on the address was syntactically invalid.  This is done fairly
** frequently by UNIX systems.  (Surprise, surprise.)  Just look
** for the opening angle bracket and take it from there.
*/

    if (status == LIB$_SYNTAXERR) {
    	if ((cp = strchr(fulbuf, '<')) == NULL) return LIB$_SYNTAXERR;

    	memset(&tpablk, 0, TPA_C_LENGTH);
    	tpablk.tpa0.tpa$l_count = TPA_K_COUNT;
    	tpablk.tpa0.tpa$l_options = TPA$M_BLANKS;
    	tpablk.tpa0.tpa$l_stringcnt = fcp-cp;
    	tpablk.tpa0.tpa$l_stringptr = cp;
    	tpablk.tpa_l_adrlen = &adrlen;
    	tpablk.tpa_l_adrbuf = adrbuf;
    	tpablk.tpa_l_namlen = &namlen;
    	tpablk.tpa_l_nambuf = nambuf;
    	tpablk.tpa_l_nomore = &nomore;

    	adrlen = namlen = nomore = 0;
    	status = table_parse(&tpablk, (void *) parse822_state, (void *) parse822_key);
    }

    if (!$VMS_STATUS_SUCCESS(status)) return status;

/*
** If the address in the original string had angle brackets, strip them off
** (unless it's a route-address).
*/
    cp = adrbuf;
    if (*cp == '<') {
    	if (*(cp+1) != '@') {
    	    adrlen -= 2;
    	    cp++;
    	}
    }
    *(cp+adrlen) = '\0';
    if (adr) *adr = cp;

/*
** Trim trailing blanks from the "personal name" string
*/
    if (namlen > 0)
    	while (*(nambuf+namlen-1) == ' ') if ((--namlen) == 0) break;
    *(nambuf+namlen) = '\0';
    if (nam) *nam = nambuf;

    return SS$_NORMAL;

} /* parse822 */

/*
**++
**  ROUTINE:	PARSE822_STORE
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Character store routine for use with LIB$TPARSE.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	PARSE822_STORE  flag, inplen, inp, toklen, tok, char, number,
**  	    	    	usrarg, adrlen, adrbuf, namlen, nambuf, nomore
**
** The first eight arguments are the LIB$TPARSE standard argument block.
**
** adrlen:  longword_unsigned, longword (unsigned), modify, by reference
** adrbuf:  char_string, character string, modify, by reference
** namlen:  longword_unsigned, longword (unsigned), modify, by reference
** nambuf:  char_string, character string, modify, by reference
** nomore:  longword_unsigned, longword (unsigned), write only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**  	
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int parse822_store(struct TPABLK *tpa) {

    int len;

    switch (tpa->tpa0.tpa$l_param) {

    case  P8_K_STORE_MBOX :
    	len = min(tpa->tpa0.tpa$l_tokencnt, STRING_SIZE-1);
    	memcpy(tpa->tpa_l_adrbuf, tpa->tpa0.tpa$l_tokenptr, len);
    	*tpa->tpa_l_adrlen = len;
    	*(tpa->tpa_l_adrbuf+len) = '\0';
    	break;

    case  P8_K_APPND_MBOX :
    	len = min(tpa->tpa0.tpa$l_tokencnt, STRING_SIZE-1-*tpa->tpa_l_adrlen);
    	memcpy(tpa->tpa_l_adrbuf+(*tpa->tpa_l_adrlen), tpa->tpa0.tpa$l_tokenptr, len);
    	*tpa->tpa_l_adrlen += len;
    	*(tpa->tpa_l_adrbuf+(*tpa->tpa_l_adrlen)) = '\0';
    	break;

    case P8_K_STORE_NAME :
    	len = min(tpa->tpa0.tpa$l_tokencnt,STRING_SIZE-1); 
   	if (tpa->tpa_l_namlen) *tpa->tpa_l_namlen = len;
    	if (tpa->tpa_l_nambuf) {
    	    memcpy(tpa->tpa_l_nambuf, tpa->tpa0.tpa$l_tokenptr, len);
    	    *(tpa->tpa_l_nambuf+len) = '\0';
    	    }
    	break;

    case P8_K_NO_MORE :
    	*tpa->tpa_l_nomore = 1;
    	break;
    	
    default :
    	return SS$_BADPARAM;

    }

    return SS$_NORMAL;

} /* parse822_store */
