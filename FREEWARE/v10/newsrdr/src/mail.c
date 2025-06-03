/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Mail routines.
**
**  MODULE DESCRIPTION:
**
**  	This module contains mail-related routines for NEWSRDR.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  09-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	09-SEP-1992 V1.0    Madison 	Initial coding.
**  	29-SEP-1992 V1.0-1  Madison 	For mail, wrap lines > 255 chars.
**  	30-SEP-1992 V1.0-2  Madison 	Fix 0-length line bug in ystdy's fix.
**  	03-OCT-1992 V1.0-3  Madison 	Misplaced break crept in from -1, -2.
**  	13-OCT-1992 V1.0-4  Madison 	Work around $FAO !AZ problem.
**  	16-OCT-1992 V1.0-5  Madison 	Fix signature file line count.
**  	15-FEB-1993 V1.0-6  Madison 	Make compatible with pre-VMS V5.2.
**  	01-MAR-1993 V1.1    Madison 	Add full_convert support.
**  	17-MAR-1993 V1.1-1  Madison 	Make more record-length-proof.
**  	12-APR-1993 V1.2    Madison 	Add Get_Mail_Personal_Name.
**  	23-APR-1993 V1.2-1  Madison 	Add force_proto option for mail.
**  	22-MAY-1993 V1.2-2  Madison 	Use temp file for mail msgs.
**  	31-AUG-1993 V1.2-3  Madison 	MAIL$USER_GET_INFO returns longword,
**  	    	    	    	    	 not word, pre-V6.1.
**--
*/
#include "newsrdr.h"
#include "globals.h"
#ifdef __GNUC__
#include <vms/maildef.h>
#include <vms/jpidef.h>
#else
#include <maildef.h>
#include <jpidef.h>
#endif

    extern unsigned int mail$send_begin(), mail$send_add_attribute();
    extern unsigned int mail$send_add_bodypart(), mail$send_add_address();
    extern unsigned int mail$send_message(), mail$send_end();
    extern unsigned int mail$user_begin(), mail$user_get_info();
    extern unsigned int mail$user_end();

/*
**++
**  ROUTINE:	Make_Return_Address
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Builds the current user's return address for mail purposes.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Make_Return_Address(char *buf, int bufsize)
**
** buf: character string, write only, by reference (ASCIZ)
** bufsize: integer, read only, by value
**
**  IMPLICIT INPUTS:	news_cfg, news_prof.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int Make_Return_Address(char *buf, int bufsize) {

    char tmp[STRING_SIZE];
    struct dsc$descriptor bdsc;
    short blen;
    unsigned int status;

    INIT_SDESC(bdsc, bufsize-1, buf);
    if (news_cfg.namcnv) {
    	struct dsc$descriptor u, str;
    	int two = 2, len;
    	INIT_SDESC(u, strlen(news_cfg.username), news_cfg.username);
    	INIT_DYNDESC(str);

/*
**  Check for full name conversion (username->full address)
*/
    	if (news_cfg.fnmcvt) {
    	    status = (*news_cfg.fnmcvt)(&news_cfg.namctx, &two, &u, &str);
    	    if (OK(status)) {
    	    	if (!*news_prof.personalname) {
    	    	    blen = str.dsc$w_length < bufsize ?
    	    	    	    	str.dsc$w_length : bufsize-1;
    	    	    memcpy(buf, str.dsc$a_pointer, blen);
    	    	} else {
    	    	    static $DESCRIPTOR(ctrstr, "!AS (!AD)");
    	    	    sys$fao(&ctrstr, &blen, &bdsc, &str,
    	    	    	strlen(news_prof.personalname), news_prof.personalname);
    	    	}
    	    	buf[blen] = '\0';
    	    	str$free1_dx(&str);
    	    	return SS$_NORMAL;
    	    }
    	}
    	    	
    	(*news_cfg.namcvt)(&news_cfg.namctx, &two, &u, &str);
    	len = str.dsc$w_length > STRING_SIZE-1 ? STRING_SIZE-1 :
    	    	    str.dsc$w_length;
    	strncpy(tmp, str.dsc$a_pointer, len);
    	*(tmp+len) = '\0';
    	str$free1_dx(&str);
    } else {
    	strcpy(tmp, news_cfg.username);
    	locase(tmp);
    }

    if (news_cfg.bangaddr) {
    	if (!*news_prof.personalname) {
    	    static $DESCRIPTOR(ctrstr, "!AD!!!AD");
    	    sys$fao(&ctrstr, &blen, &bdsc, strlen(news_cfg.mailnode),
    	    	    news_cfg.mailnode, strlen(tmp), tmp);
    	} else {
    	    static $DESCRIPTOR(ctrstr, "!AD!!!AD (!AD)");
    	    sys$fao(&ctrstr, &blen, &bdsc, strlen(news_cfg.mailnode),
    	    	    news_cfg.mailnode, strlen(tmp), tmp,
    	    	    strlen(news_prof.personalname), news_prof.personalname);
    	}
    } else {
    	if (!*news_prof.personalname) {
    	    static $DESCRIPTOR(ctrstr, "!AD@!AD");
    	    sys$fao(&ctrstr, &blen, &bdsc, strlen(tmp), tmp,
    	    	    strlen(news_cfg.mailnode), news_cfg.mailnode);
    	} else {
    	    static $DESCRIPTOR(ctrstr, "!AD@!AD (!AD)");
    	    sys$fao(&ctrstr, &blen, &bdsc, strlen(tmp), tmp,
    	    	strlen(news_cfg.mailnode), news_cfg.mailnode,
    	    	strlen(news_prof.personalname), news_prof.personalname);
    	}
    }
    *(buf+blen) = '\0';

    return SS$_NORMAL;

} /* Make_Return_Address */

/*
**++
**  ROUTINE:	Mail_Message
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Sends out a message using VMS Mail.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Mail_Message(struct QUE *destq, char *subjstr, char *fspec,
**  	    	int use_signature, int copy_self, int just_self)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int Mail_Message(struct QUE *destq, char *subjstr, char *fspec,
    	    	    int use_signature, int copy_self, int just_self) {

    char tmp[STRING_SIZE], *inbuf, tmpfile[FSPEC_SIZE];
    struct dsc$descriptor sdsc;
    ITMLST mailst[4], outlst[1];
    struct HDR *hdr;
    unsigned int status, mailctx, unit, unit2;
    static short mail_to = MAIL$_TO, mail_cc = MAIL$_CC;
    int tlen, bufsize;

    status = file_open(fspec, &unit, 0, 0, &bufsize);
    if (!OK(status)) {
    	lib$signal(NEWS__MAILERR, 0, status);
    	return NEWS__MAILERR;
    }
    inbuf = malloc(bufsize+1);
    if (!inbuf) {
    	lib$signal(NEWS__MAILERR, 0, SS$_INSFMEM);
    	return NEWS__MAILERR;
    }

    mailctx = 0;
    ITMLST_INIT(mailst[0], MAIL$_NOSIGNAL, 0, 0, 0);
    ITMLST_INIT(mailst[1], 0, 0, 0, 0);
    ITMLST_INIT(mailst[2], 0, 0, 0, 0);
    ITMLST_INIT(mailst[3], 0, 0, 0, 0);
    ITMLST_INIT(outlst[0], 0, 0, 0, 0);

    status = mail$send_begin(&mailctx, mailst, outlst);
    if (!OK(status)) {
    	lib$signal(NEWS__MAILERR, 0, status);
    	file_close(unit);
    	return NEWS__MAILERR;
    }

    if (*subjstr) {
    	ITMLST_INIT(mailst[1], MAIL$_SEND_SUBJECT, strlen(subjstr), subjstr, 0);
    	status = mail$send_add_attribute(&mailctx, mailst, outlst);
    	if (!OK(status)) lib$signal(NEWS__MAILERR, 0, status);
    }

    make_temp_fspec(tmpfile, sizeof(tmpfile)-1);
    status = file_create(tmpfile, &unit2, 0, 0);
    if (!OK(status)) {
    	mail$send_end(&outlst, &outlst);
    	lib$signal(NEWS__MAILERR, 0, status);
    	return NEWS__MAILERR;
    }

    while (OK(file_read(unit, inbuf, bufsize, &tlen))) {
    	char *tp;
    	int len;
    	tp = inbuf;
    	while (tlen >= 0) {
    	    len = (tlen > 255 ? 255 : tlen);
    	    status = file_write(unit2, tp, len);
#ifdef notdef
    	    ITMLST_INIT(mailst[1], MAIL$_SEND_RECORD, len, tp, 0);
    	    status = mail$send_add_bodypart(&mailctx, mailst, outlst);
#endif /* notdef */
    	    if (!OK(status)) {
    	    	lib$signal(NEWS__MAILERR, 0, status);
    	    	break;
    	    }
    	    tlen -= len;
    	    tp += len;
    	    if (tlen == 0) break;
    	}
    	if (!OK(status)) break;
    }
    file_close(unit);

    free(inbuf);

    if (use_signature && *news_prof.sigfile) {
    	status = file_open(news_prof.sigfile, &unit, 0, 0, 0);
    	if (!OK(status)) {
    	    lib$signal(NEWS__SIGFERR, 2, strlen(news_prof.sigfile),
    	    	    	news_prof.sigfile, status);
    	} else {
    	    int i;
    	    for (i = 0; i < 8; i++) {
    	    	if (!OK(file_read(unit, tmp, sizeof(tmp)-1, &tlen))) break;
    	    	status = file_write(unit2, tmp, (tlen > 255 ? 255 : tlen));
#ifdef notdef
    	    	ITMLST_INIT(mailst[1], MAIL$_SEND_RECORD, tlen, tmp, 0);
    	    	status = mail$send_add_bodypart(&mailctx, mailst, outlst);
#endif /* notdef */
    	    	if (!OK(status)) {
    	    	    lib$signal(NEWS__SIGFERR, 2, strlen(news_prof.sigfile),
    	    	    	    	news_prof.sigfile, status);
    	    	    break;
    	    	}
    	    }
    	    file_close(unit);
    	}
    }

    file_close(unit2);
    ITMLST_INIT(mailst[1], MAIL$_SEND_FILENAME, strlen(tmpfile), tmpfile, 0);
    status = mail$send_add_bodypart(&mailctx, mailst, outlst);
    if (!OK(status)) lib$signal(NEWS__MAILERR, 0, status);

    if (just_self) {
    	ITMLST_INIT(mailst[1], MAIL$_SEND_USERNAME, strlen(news_cfg.username),
    	    	    	news_cfg.username, 0);
    	ITMLST_INIT(mailst[2], MAIL$_SEND_USERNAME_TYPE, sizeof(mail_to),
    	    	    	&mail_to, 0);
    	status = mail$send_add_address(&mailctx, mailst, outlst);
    	if (!OK(status)) lib$signal(NEWS__MAILERR, 0, status);
    } else {
    	while (queue_remove(destq->head, &hdr)) {
    	    if (news_cfg.adrcnv) {
    	    	struct dsc$descriptor str, sdsc;
    	    	int len;
    	    	INIT_DYNDESC(str);
    	    	INIT_SDESC(sdsc, strlen(hdr->str), hdr->str);
    	    	(*news_cfg.adrcvt)(&news_cfg.adrctx, &sdsc, &str);
    	    	len = str.dsc$w_length > STRING_SIZE-1 ? STRING_SIZE-1 :
    	    	    	str.dsc$w_length;
    	    	strncpy(tmp, str.dsc$a_pointer, len);
    	    	*(tmp+len) = '\0';
    	    } else {
    	    	if (!news_prof.force_proto && (strchr(hdr->str, '@') == NULL &&
    	    	    	strchr(hdr->str, '!') == NULL)) {
    	    	    strcpy(tmp, hdr->str);
    	    	} else {
    	    	    char *cp, tmp2[STRING_SIZE];
    	    	    strcpy(tmp2, hdr->str);
    	    	    for (cp = tmp2; *cp; cp++) {
    	    	    	if (*cp == '"') *cp = '\'';
    	    	    }
    	    	    strcpy(tmp, news_cfg.mail_proto);
    	    	    strcat(tmp, "\"");
    	    	    strcat(tmp, tmp2);
    	    	    strcat(tmp, "\"");
    	    	}
    	    }
    	    ITMLST_INIT(mailst[1], MAIL$_SEND_USERNAME, strlen(tmp), tmp, 0);
    	    ITMLST_INIT(mailst[2], MAIL$_SEND_USERNAME_TYPE, sizeof(mail_to),
    	    	    	    &mail_to, 0);
    	    status = mail$send_add_address(&mailctx, mailst, outlst);
    	    if (!OK(status)) lib$signal(NEWS__MAILERR, 0, status);
    	    mem_freehdr(hdr);
    	}
    }

    if (!just_self && copy_self) {
    	ITMLST_INIT(mailst[1], MAIL$_SEND_USERNAME, strlen(news_cfg.username),
    	    	    	news_cfg.username, 0);
    	ITMLST_INIT(mailst[2], MAIL$_SEND_USERNAME_TYPE, sizeof(mail_cc),
    	    	    	&mail_cc, 0);
    	status = mail$send_add_address(&mailctx, mailst, outlst);
    	if (!OK(status)) lib$signal(NEWS__MAILERR, 0, status);
    }

    status = mail$send_message(&mailctx, outlst, outlst);
    if (!OK(status)) lib$signal(NEWS__MAILERR, 0, status);

    status = mail$send_end(&mailctx, outlst, outlst);
    if (!OK(status)) lib$signal(NEWS__MAILERR, 0, status);

    file_delete(tmpfile);

    if (!OK(status)) return NEWS__MAILERR;

    return SS$_NORMAL;

} /* Mail_Message */

/*
**++
**  ROUTINE:	Get_Mail_Personal_Name
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Retrieves the user's VMS MAIL personal name.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Get_Mail_Personal_Name(char *nam, int siz)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void Get_Mail_Personal_Name(char *buf, int bufsiz) {

    ITMLST inlst[2], outlst[2];
    unsigned int len = 0;
    unsigned int status, ctx;

    *buf = '\0';
    ctx = 0;

    ITMLST_INIT(inlst[0], MAIL$_NOSIGNAL, 0, 0, 0);
    ITMLST_INIT(inlst[1], 0, 0, 0, 0);

    ITMLST_INIT(outlst[0], MAIL$_USER_PERSONAL_NAME,
    	(bufsiz-1 < 255 ? bufsiz-1 : 255), buf, &len);
    ITMLST_INIT(outlst[1], 0, 0, 0, 0);

    status = mail$user_begin(&ctx, inlst, &outlst[1]);
    if (!OK(status)) return;
    status = mail$user_get_info(&ctx, inlst, outlst);
    mail$user_end(&ctx, inlst, &outlst[1]);
    if (OK(status)) buf[len] = '\0';

} /* Get_Mail_Personal_Name */
