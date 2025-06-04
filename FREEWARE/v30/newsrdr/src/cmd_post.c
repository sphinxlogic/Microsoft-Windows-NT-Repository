/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Commands related to posting articles.
**
**  MODULE DESCRIPTION:
**
**  	This module contains the routines that implement the POST,
**  MAIL, and REPLY commands.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, 1994 MADGOAT SOFTWARE. 
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  04-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	04-SEP-1992 V1.0    Madison 	Initial coding.
**  	17-SEP-1992 V1.0-1  Madison 	Fix follwup-to: poster check.
**  	22-SEP-1992 V1.0-2  Madison 	Fix STORE mix-up.
**  	30-SEP-1992 V1.0-3  Madison 	Missing header separator in FWD/STORE.
**  	01-OCT-1992 V1.0-4  Madison 	Must lower-case group names on REPLY.
**  	07-OCT-1992 V1.0-5  Madison 	cli_get_value("OK"...)?  Duh.
**  	23-DEC-1992 V1.0-6  Madison 	Fix Post_Article return-code check.
**  	23-MAR-1993 V1.0-7  Madison 	Fix file_read reference.
**  	08-APR-1993 V1.0-8  Madison 	Fix REPLY/DISTRIBUTION.
**  	12-APR-1993 V1.1    Madison 	Get_Article_* changes.
**  	08-JUL-1993 V1.2    Madison 	Let user specify name of keep file.
**  	02-AUG-1993 V1.2-1  Madison 	Fix minor CANCEL bug.
**  	21-SEP-1993 V1.3    Madison 	Add /APPROVED, /EXPIRES qualifiers.
**  	29-SEP-1993 V1.3-1  Madison 	Fix /KEYWORDS qualifier.
**  	05-OCT-1993 V1.3-2  Madison 	Fix handling of null To: in cmd_forward.
**  	15-JAN-1994 V1.3-3  Madison 	get_article_body has changed.
**  	28-MAR-1994 V1.3-4  Madison 	Check Copy_File status.
**  	16-MAY-1994 V1.4    Madison 	Customizable reply prefix.
**  	19-MAY-1994 V1.4-1  Madison 	Fix broken mail check.
**  	08-DEC-1994 V1.4-2  Madison 	References are separated by blanks!
**--
*/
#include "newsrdr.h"
#include "globals.h"

    EXTERN struct GRP *curgroup;
    EXTERN int        cur_artnum;

#ifdef __GNUC__
#define RMS$_EOF ((unsigned int) rms$_eof)
    extern unsigned int rms$_eof();
#else
#pragma nostandard
    globalvalue unsigned int RMS$_EOF;
#pragma standard
#endif

    extern struct GRP *Find_Group(char *);
    extern int Post_Article(struct QUE *, char *, int);
    extern unsigned int csl_parse(char *, struct QUE *);
    extern int Mail_Message(struct QUE *, char *, char *, int, int, int);
    extern unsigned int Get_Article_Hdrs(struct GRP *, int, void *, int, int *);
    extern unsigned int Get_Article_Body(int, char *, char *, int *, int);
    extern unsigned int Parse_ToList(char *, struct QUE *, int);
    extern void Make_Return_Address(char *, int);
    extern unsigned int Copy_File(char *, char *, char *, int);
    

/*
**++
**  ROUTINE:	cmd_post
**
**  FUNCTIONAL DESCRIPTION:
**
**  	POST command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_post()
**
**  IMPLICIT INPUTS:	Many.
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
unsigned int cmd_post() {

    char tmp[STRING_SIZE], ngstr[STRING_SIZE], inspec[FSPEC_SIZE];
    char outspec[FSPEC_SIZE], dist[STRING_SIZE], fupto[STRING_SIZE];
    char *ngp, *dcp, *fcp;
    struct HDR *h;
    struct QUE hdrq;
    int is_temp, edit_flag, go_ahead, use_sigfile;
    unsigned int status;

    hdrq.head = hdrq.tail = &hdrq;
    ngstr[0] = '\0';
    ngp = ngstr;

/*
** The server told us whether we could post or not when we connected.
*/
    if (!news_cfg.postingok) {
    	lib$signal(NEWS__NOPOSTING, 0);
    	return SS$_NORMAL;
    }

/*
** Get the group(s) we're posting to.  Assume current group if none
** given.
*/
    while (OK(cli_get_value("GRPNAM", tmp, sizeof(tmp)))) {
    	struct GRP *g;
    	locase(tmp);
    	g = Find_Group(tmp);
    	if (g == NULL) {
    	    lib$signal(NEWS__NOSUCHGRP, 2, strlen(tmp), tmp);
    	    return SS$_NORMAL;
    	}

    	if (ngp != ngstr) *ngp++ = ',';
    	strcpy(ngp, g->grpnam);
    	ngp+= strlen(g->grpnam);

    }
    *ngp = '\0';

    if (ngp == ngstr) {
    	if (curgroup == NULL) {
    	    lib$signal(NEWS__NOCURGROUP, 0);
    	    return SS$_NORMAL;
    	} else {
    	    strcpy(ngstr, curgroup->grpnam);
    	    lib$signal(NEWS__USINGCURGROUP, 2, strlen(curgroup->grpnam),
    	    	    	curgroup->grpnam);
    	}
    }

    insert_header(ngstr, hdrq.tail, NEWS_K_HDR_NEWSGROUPS);

/*
** Get the article subject
*/
    tmp[0] = '\0';
    status = cli_present("SUBJECT");
    if (status == CLI$_PRESENT) {
    	status = cli_get_value("SUBJECT", tmp, sizeof(tmp));
    } else if (status != CLI$_NEGATED) {
    	status = get_cmd(tmp, sizeof(tmp), "Subject: ");
    }

    if (tmp[0]) insert_header(tmp, hdrq.tail, NEWS_K_HDR_SUBJECT);

/*
** Do we use the signature file?
*/
    status = cli_present("SIGNATURE");
    use_sigfile = (news_prof.autosigpost && status != CLI$_NEGATED) ||
    	(!news_prof.autosigpost && status == CLI$_PRESENT);

/*
** Set the distribution header
*/
    status = cli_present("DISTRIBUTION");
    if (status == CLI$_PRESENT) {
    	char dist[STRING_SIZE], *dcp = dist;
    	while (OK(cli_get_value("DISTRIBUTION", tmp, sizeof(tmp)))) {
    	    if (dcp != dist) *dcp++ = ',';
    	    locase(tmp);
    	    strcpy(dcp, tmp);
    	    dcp += strlen(tmp);
    	}
    	*dcp = '\0';
    	insert_header(dist, hdrq.tail, NEWS_K_HDR_DISTRIBUTION);
    }

/*
** Set the Followup-to header.
*/
    status = cli_present("FOLLOWUP_TO");
    if (status == CLI$_PRESENT) {
    	int fuptoerr = 0;
    	float afew = 3.0;
    	char fupto[STRING_SIZE], *fcp = fupto;
    	while (OK(cli_get_value("FOLLOWUP_TO", tmp, sizeof(tmp)))) {
    	    if (fcp != fupto) *fcp++ = ',';
    	    locase(tmp);
    	    if (strcmp(tmp, "poster") != 0) {
    	    	if (Find_Group(tmp) == 0) {
    	    	    lib$signal(NEWS__NOSUCHGRP, 2, strlen(tmp), tmp);
    	    	    fuptoerr = 1;
    	    	    continue;
    	    	}
    	    }
    	    strcpy(fcp, tmp);
    	    fcp += strlen(tmp);
    	}
    	*fcp = '\0';
    	if (*fupto != '\0') {
    	    insert_header(fupto, hdrq.tail, NEWS_K_HDR_FOLLOWUP_TO);
    	}
    	if (fuptoerr) lib$wait(&afew);
    }

/*
** Set the Approved: header.
*/
    status = cli_present("APPROVED");
    if (status == CLI$_PRESENT) {
    	Make_Return_Address(tmp, sizeof(tmp));
    	insert_header(tmp, hdrq.tail, NEWS_K_HDR_APPROVED);
    }

/*
** Set the Expires: header.
*/
    status = cli_present("EXPIRES");
    if (status == CLI$_PRESENT) {
    	struct dsc$descriptor tdsc;
    	TIME expiry;
    	cli_get_value("EXPIRES", tmp, sizeof(tmp));
    	INIT_SDESC(tdsc, strlen(tmp), tmp);
    	status = lib$convert_date_string(&tdsc, &expiry);
    	if (OK(status)) {
    	    Make_Date(&expiry, tmp, sizeof(tmp));
    	    insert_header(tmp, hdrq.tail, NEWS_K_HDR_EXPIRES);
    	}
    }

/*
** Set the Summary: header.
*/
    status = cli_present("SUMMARY");
    if (status == CLI$_PRESENT) {
    	cli_get_value("SUMMARY", tmp, sizeof(tmp));
    	insert_header(tmp, hdrq.tail, NEWS_K_HDR_SUMMARY);
    }

/*
** Set the Keywords: header.
*/
    status = cli_present("KEYWORDS");
    if (status == CLI$_PRESENT) {
    	char kw[STRING_SIZE], *fcp = kw;
    	while (OK(cli_get_value("KEYWORDS", tmp, sizeof(tmp)))) {
    	    if (fcp != kw) *fcp++ = ',';
    	    locase(tmp);
    	    strcpy(fcp, tmp);
    	    fcp += strlen(tmp);
    	}
    	*fcp = '\0';
    	if (*kw != '\0') {
    	    insert_header(kw, hdrq.tail, NEWS_K_HDR_KEYWORDS);
    	}
    }

/*
** Get the input file name, if one was given.
*/
    inspec[0] = outspec[0] = '\0';
    if (cli_present("INPFIL") == CLI$_PRESENT) {
    	cli_get_value("INPFIL", inspec, sizeof(inspec));
    } else if (cli_present("FILE") == CLI$_PRESENT) {
    	cli_get_value("FILE", inspec, sizeof(inspec));
    }

/*
** Compose_Message prompts the user for message text, or fires
** up the text editor.
*/
    status = cli_present("EDIT");
    edit_flag = (news_prof.edit && status != CLI$_NEGATED) ||
    	    	(status == CLI$_PRESENT);
    status = Compose_Message(inspec, outspec, edit_flag, &is_temp);

/*
** If all went ok, give the user a chance to back out.  If she says
** it's OK, then post away.
*/
    if (!OK(status)) {
    	lib$signal(NEWS__COMPOSERR, 0, status);
    } else if (!outspec[0]) {
    	lib$signal(NEWS__CANCELLED, 0);
    } else {
    	strcpy(tmp, "Ready to post to: ");
    	strcat(tmp, ngstr);
    	put_output(tmp);
    	go_ahead = Yes_Answer("Okay to proceed?", 1);
    	if (go_ahead) go_ahead = OK(Post_Article(&hdrq, outspec, use_sigfile));
    	if (!go_ahead && is_temp) {
    	    go_ahead = !Yes_Answer("Keep file containing message?", 0);
    	}
    	if (go_ahead && is_temp) file_delete(outspec);
    	else if (is_temp) {
    	    strcpy(tmp, "Save as [");
    	    strcat(tmp, outspec);
    	    strcat(tmp, "]: ");
    	    status = get_cmd(inspec, sizeof(inspec)-1, tmp);
    	    if (OK(status) && inspec[0] != '\0') {
    	    	status = Copy_File(outspec, inspec, dist, 0);
    	    	if (OK(status)) file_delete(outspec);
    	    	else {
    	    	    lib$signal(NEWS__WRITERR, 2, strlen(inspec), inspec, status);
    	    	    strcpy(dist, outspec);
    	    	}
    	    } else {
    	    	strcpy(dist, outspec);
    	    }
    	    strcpy(tmp, "Message retained in file ");
    	    strcat(tmp, dist);
    	    put_output(tmp);
    	}
    }

/*
** Clean up
*/
    while (queue_remove(hdrq.head, &h)) mem_freehdr(h);

    return SS$_NORMAL;

} /* cmd_post */

/*
**++
**  ROUTINE:	cmd_mail
**
**  FUNCTIONAL DESCRIPTION:
**
**  	MAIL command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_mail()
**
**  IMPLICIT INPUTS:	Many.
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
unsigned int cmd_mail() {

    char tmp[STRING_SIZE], subjstr[STRING_SIZE], tostr[STRING_SIZE];
    char infspec[FSPEC_SIZE], outspec[FSPEC_SIZE];
    char *cp;
    struct HDR *h;
    struct QUE hdrq, destq;
    int edit_flag, is_temp, go_ahead, use_sigfile, len;
    unsigned int status;

    hdrq.head = hdrq.tail = &hdrq;
    destq.head = destq.tail = &destq;

/*
** Build the to list, if it was given on the command.  Otherwise,
** prompt the user for addresses.
*/
    while (OK(cli_get_value("TO", tmp, sizeof(tmp)))) {
    	insert_header(tmp, destq.tail, 0);
    }
    if (destq.head == &destq) {
    	status = get_cmd(tmp, sizeof(tmp), "To: ");
    	len = strlen(tmp);
    	while (len > 0 && isspace(tmp[len-1])) len--;
    	if (len == 0) {
    	    status = RMS$_EOF;
    	} else {
    	    tmp[len] = '\0';
    	    for (cp = tmp; *cp && isspace(*cp); cp++) len--;
    	    if (len == 0) status == RMS$_EOF;
    	    else status = csl_parse(cp, &destq);
    	}
    	if (!OK(status)) {
    	    while(queue_remove(destq.head, &h)) mem_freehdr(h);
    	    if (status != RMS$_EOF) {
    	    	lib$signal(NEWS__MAILERR, 0, NEWS__ADDRSYNTX, 0);
    	    }
    	    return SS$_NORMAL;
    	}
    }

/*
** Build the To: header for the message.
*/    
    cp = tostr;
    for (h = destq.head; h != (struct HDR *) &destq; h = h->flink) {
    	if (cp != tostr) *cp++ = ',';
    	strcpy(cp, h->str);
    	cp += strlen(h->str);
    }
    *cp = '\0';

/*
** Set up the Subject: header
*/
    status = cli_present("SUBJECT");
    if (status == CLI$_PRESENT) {
    	status = cli_get_value("SUBJECT", subjstr, sizeof(subjstr));
    } else if (status != CLI$_NEGATED) {
    	status = get_cmd(subjstr, sizeof(subjstr), "Subject: ");
    	if (!OK(status)) return SS$_NORMAL;
    }

/*
** Should we use a signature file?
*/
    status = cli_present("SIGNATURE");
    use_sigfile = (news_prof.autosigmail && status != CLI$_NEGATED) ||
    	    	  (!news_prof.autosigmail && status == CLI$_PRESENT);

/*
** If the user specified a file to be mailed, then use it.
*/
    infspec[0] = '\0';
    cli_get_value("FSPEC", infspec, sizeof(infspec));
    status = cli_present("EDIT");
    edit_flag = (news_prof.edit && status != CLI$_NEGATED) ||
    	    	(status == CLI$_PRESENT);

/*
** Use Compose_Message to enter the message or edit it, or whatever.
*/
    outspec[0] = '\0';
    status = Compose_Message(infspec, outspec, edit_flag, &is_temp);

/*
** If the compose went ok, give the user one more chance to back out.
** If ok by her, then mail it.
*/
    if (!OK(status)) {
    	lib$signal(NEWS__COMPOSERR, 0, status);
    } else if (!outspec[0]) {
    	lib$signal(NEWS__CANCELLED, 0);
    } else {
    	strcpy(tmp, "Ready to mail to: ");
    	strcat(tmp, tostr);
    	put_output(tmp);
    	go_ahead = Yes_Answer("Okay to proceed?", 1);
    	if (go_ahead) {
    	    int copy_self;
    	    status = cli_present("SELF");
    	    copy_self = (status == CLI$_PRESENT) ||
    	    	(news_prof.csmail && status != CLI$_NEGATED);
    	    go_ahead = OK(Mail_Message(&destq, subjstr, outspec, use_sigfile,
    	    	    	    copy_self, 0));
    	}

/*
** If the user decided not to mail the message, ask her if she wants
** to keep the message contents.  Otherwise, blow it away (if it was
** a file we created.
*/
    	if (!go_ahead && is_temp) {
    	    go_ahead = !Yes_Answer("Keep file containing message?", 0);
    	}
    	if (go_ahead && is_temp) file_delete(outspec);
    	else if (is_temp) {
    	    strcpy(tmp, "Save as [");
    	    strcat(tmp, outspec);
    	    strcat(tmp, "]: ");
    	    status = get_cmd(infspec, sizeof(infspec)-1, tmp);
    	    if (OK(status) && infspec[0] != '\0') {
    	    	status = Copy_File(outspec, infspec, tostr, 0);
    	    	if (OK(status)) file_delete(outspec);
    	    	else {
    	    	    lib$signal(NEWS__WRITERR, 2, strlen(infspec), infspec, status);
    	    	    strcpy(tostr, outspec);
    	    	}
    	    } else {
    	    	strcpy(tostr, outspec);
    	    }
    	    strcpy(tmp, "Message retained in file ");
    	    strcat(tmp, tostr);
    	    put_output(tmp);
    	}

    }

/*
** Clean up
*/
    while (queue_remove(destq.head, &h)) mem_freehdr(h);

    return SS$_NORMAL;

} /* cmd_mail */

/*
**++
**  ROUTINE:	cmd_reply
**
**  FUNCTIONAL DESCRIPTION:
**
**  	REPLY command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_reply()
**
**  IMPLICIT INPUTS:	Many.
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
unsigned int cmd_reply() {

    struct QUE *hdrq, newhq, destq;
    char diststr[STRING_SIZE], groupstr[STRING_SIZE];
    char subjstr[STRING_SIZE], msgidstr[STRING_SIZE], fuptostr[STRING_SIZE];
    char newfupto[STRING_SIZE], refstr[STRING_SIZE], tmp[STRING_SIZE];
    char infspec[FSPEC_SIZE], outspec[FSPEC_SIZE];
    struct HDR *hdr;
    char *cp, *fromstr;
    int edit_flag, extract, post_reply, mail_reply, go_ahead, is_temp;
    int use_sigp, use_sigm;
    unsigned int status;

    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }
    if (cur_artnum == 0) {
    	lib$signal(NEWS__NOCURART, 0);
    	return SS$_NORMAL;
    }

/*
** Replies are a little bit complicated, since they can be both posted
** and mailed.  Let's find out what we're supposed to be doing.
*/

    post_reply = cli_present("POST") == CLI$_PRESENT;
    mail_reply = cli_present("MAIL") == CLI$_PRESENT;
    if (!(post_reply || mail_reply)) {
    	post_reply = news_prof.replypost;
    	mail_reply = news_prof.replymail;
    }

/*
** Check to make sure postings are OK (from the banner message when we
** first conneted.
*/
    if (post_reply && !news_cfg.postingok) {
    	lib$signal(NEWS__NOPOSTING, 0);
    	return SS$_NORMAL;
    }


/*
** We need information from the current article's headers.  Some of
** it is also settable by the user.
*/
    *groupstr = *subjstr = *fuptostr = *infspec = *outspec = *tmp =
    	    *diststr = *refstr = *newfupto = '\0';

    destq.head = destq.tail = &destq;

    if (post_reply) {
    	cp = groupstr;
    	while (OK(cli_get_value("GRPNAM", tmp, sizeof(tmp)))) {
    	    struct GRP *g;
    	    locase(tmp);
    	    g = Find_Group(tmp);
    	    if (g == NULL) {
    	    	lib$signal(NEWS__NOSUCHGRP, 2, strlen(tmp), tmp);
    	    	return SS$_NORMAL;
    	    }
    	    if (cp != groupstr) *cp++ = ',';
    	    strcpy(cp, tmp);
    	    cp += strlen(tmp);
    	}
    	*cp = '\0';
    }

    if (*groupstr) strcpy(fuptostr, groupstr);

    if (mail_reply) {
    	if (cli_present("TO") == CLI$_PRESENT) {
    	    while (OK(cli_get_value("TO", tmp, sizeof(tmp)))) {
    	    	insert_header(tmp, destq.tail, 0);
    	    }
    	}
    }

    status = Get_Article_Hdrs(curgroup, cur_artnum, &hdrq, 0, 0);
    if (!OK(status)) {
    	lib$signal(NEWS__REPLYERR, 0, status);
    	return SS$_NORMAL;
    }

    for (hdr = hdrq->head; hdr != (struct HDR *) hdrq; hdr = hdr->flink) {
    	switch (hdr->code) {

    	case NEWS_K_HDR_FROM:
    	    	fromstr = hdr->str;
    	    	break;
    	case NEWS_K_HDR_REPLY_TO:
    	    	if (mail_reply && destq.head == &destq) {
    	    	    Parse_ToList(hdr->str, &destq, 0);
    	    	}
    	    	break;
    	case NEWS_K_HDR_NEWSGROUPS:
    	    	if (!*groupstr) strcpy(groupstr, hdr->str);
    	    	break;
    	case NEWS_K_HDR_SUBJECT:
    	    	strcpy(subjstr, hdr->str);
    	    	break;
    	case NEWS_K_HDR_FOLLOWUP_TO:
    	    	if (!*fuptostr) strcpy(fuptostr, hdr->str);
    	    	break;
    	case NEWS_K_HDR_DISTRIBUTION:
    	    	strcpy(diststr, hdr->str);
    	    	break;
    	case NEWS_K_HDR_REFERENCES:
    	    	strcpy(refstr, hdr->str);
    	    	break;
    	case NEWS_K_HDR_MESSAGE_ID:
    	    	strcpy(msgidstr, hdr->str);
    	    	break;
    	}
    }

/*
** If the article has a Followup-To: poster header, then we're supposed
** to mail the reply only.
*/
    if (post_reply) {
    	if (streql_case_blind(fuptostr, "poster")) {
    	    post_reply = 0;
    	    mail_reply = 1;
    	    lib$signal(NEWS__MUSTMAIL, 0);
    	}
    }

/*
** Build the list of recipients if mailing.
*/
    if (mail_reply && destq.head == &destq) {
    	Parse_ToList(fromstr, &destq, 0);
    	if (destq.head == &destq) {
    	    lib$signal(NEWS__REPLYERR, 0, NEWS__NOREPLYADDR, 0);
    	    return SS$_NORMAL;
    	}
    }

/*
** Subject header for new article
*/
    status = cli_present("SUBJECT");
    if (status == CLI$_NEGATED) {
    	*subjstr = '\0';
    } else if (status == CLI$_PRESENT) {
    	cli_get_value("SUBJECT", subjstr, sizeof(subjstr));
    } else {
    	if (streql_case_blind(subjstr, "(none)")) {
    	    *subjstr = '\0';
    	} else if (!strneql_case_blind(subjstr, "Re:", 3)) {
    	    memmove(subjstr+4, subjstr, strlen(subjstr)+1);
    	    memcpy(subjstr, "Re: ", 4);
    	}
    }

/*
** Signature file?
*/
    status = cli_present("SIGNATURE");
    use_sigp = (news_prof.autosigpost && status != CLI$_NEGATED) ||
    	       (status == CLI$_PRESENT);
    use_sigm = (news_prof.autosigmail && status != CLI$_NEGATED) ||
    	       (status == CLI$_PRESENT);

/*
** Distribution header for article
*/
    status = cli_present("DISTRIBUTION");
    if (status == CLI$_PRESENT) {
    	cp = diststr;
    	while (OK(cli_get_value("DISTRIBUTION", tmp, sizeof(tmp)))) {
    	    if (cp != diststr) *cp++ = ',';
    	    locase(tmp);
    	    strcpy(cp, tmp);
    	    cp += strlen(tmp);
    	}
    	*cp = '\0';
    }

/*
** Followup-to header for new article.
*/
    status = cli_present("FOLLOWUP_TO");
    if (status == CLI$_PRESENT) {
    	int fuptoerr = 0;
    	float afew = 3.0;
    	cp = newfupto;
    	while (OK(cli_get_value("FOLLOWUP_TO", tmp, sizeof(tmp)))) {
    	    if (cp != newfupto) *cp++ = ',';
    	    locase(tmp);
    	    if (strcmp(tmp, "poster") != 0) {
    	    	if (Find_Group(tmp) == 0) {
    	    	    lib$signal(NEWS__NOSUCHGRP, 2, strlen(tmp), tmp);
    	    	    fuptoerr = 1;
    	    	    continue;
    	    	}
    	    }
    	    strcpy(cp, tmp);
    	    cp += strlen(tmp);
    	}
    	*cp = '\0';
    	if (fuptoerr) lib$wait(&afew);
    }

/*
** Let's see if this is a REPLY/EXTRACT.  If so, we get the current
** article's contents, stick the reply-prefix on the front of each line, and
** use that as the input file for Compose_Message.
*/
    status = cli_present("EDIT");
    edit_flag = (status == CLI$_PRESENT) ||
    	    	    (news_prof.edit && status != CLI$_NEGATED);
    extract = cli_present("EXTRACT") != CLI$_NEGATED;
    edit_flag |= extract;

    if (extract) {
    	char temp0[FSPEC_SIZE], temp1[FSPEC_SIZE];
    	unsigned int runit, wunit;
    	int len;

    	make_temp_fspec(temp0, sizeof(temp0));
    	make_temp_fspec(infspec, sizeof(infspec));
    	status = Get_Article_Body(cur_artnum, temp0, temp1, 0, 0);
    	if (!OK(status)) {
    	    lib$signal(NEWS__REPLYERR, 0, status, 0);
    	    return SS$_NORMAL;
    	}
    	status = file_open(temp1, &runit, 0, 0, 0);
    	if (!OK(status)) {
    	    file_delete(temp1);
    	    lib$signal(NEWS__REPLYERR, 0, status);
    	    return SS$_NORMAL;
    	}
    	status = file_create(infspec, &wunit, 0, infspec);
    	if (!OK(status)) {
    	    file_close(runit);
    	    file_delete(temp1);
    	    return SS$_NORMAL;
    	}
    	strcpy(tmp, "In article ");
    	strcat(tmp, msgidstr);
    	strcat(tmp, ", ");
    	if (mail_reply && !post_reply) {
    	    strcat(tmp, "you write:");
    	} else {
    	    strcat(tmp, fromstr);
    	    strcat(tmp, " writes:");
    	}
    	file_write(wunit, tmp, strlen(tmp));
    	memcpy(tmp, news_prof.reply_prefix, news_prof.reply_prefix_len);
    	while (OK(file_read(runit, tmp+news_prof.reply_prefix_len,
    	    	    	    	sizeof(tmp)-news_prof.reply_prefix_len-1, &len))) {
    	    file_write(wunit, tmp, len+news_prof.reply_prefix_len);
    	}
    	file_close(wunit);
    	file_close(runit);
    	file_delete(temp1);

    }

/*
** Now use Compose_Message for entry/editing of reply text.
*/
    *outspec = '\0';
    status = Compose_Message(infspec, outspec, edit_flag, &is_temp);
    if (!OK(status) || !*outspec) {
    	if (!OK(status)) {
    	    lib$signal(NEWS__COMPOSERR, 0, status);
    	} else {
    	    lib$signal(NEWS__CANCELLED);
    	}
    	if (*outspec) file_delete(outspec);
    	if (extract) file_delete(infspec);
    	return SS$_NORMAL;
    }

/*
** Now actually build the headers for the new article/mail message, then
** get confirmation from the user.
*/
    newhq.head = newhq.tail = &newhq;

    if (post_reply) {
    	if (!*fuptostr) strcpy(fuptostr, groupstr);
    	insert_header(fuptostr, newhq.tail, NEWS_K_HDR_NEWSGROUPS);
    	if (*subjstr) insert_header(subjstr, newhq.tail, NEWS_K_HDR_SUBJECT);
    	if (*diststr) insert_header(diststr, newhq.tail, NEWS_K_HDR_DISTRIBUTION);
    	if (*newfupto) insert_header(newfupto, newhq.tail, NEWS_K_HDR_FOLLOWUP_TO);
    	if (*refstr) {
    	    if (strlen(refstr) < 255+strlen(msgidstr)) {
    	    	strcat(refstr, " ");
    	    	strcat(refstr, msgidstr);
    	    }
    	    insert_header(refstr, newhq.tail, NEWS_K_HDR_REFERENCES);
    	} else {
    	    insert_header(msgidstr, newhq.tail, NEWS_K_HDR_REFERENCES);
    	}
/*
** Set the Approved: header.
*/
    	status = cli_present("APPROVED");
    	if (status == CLI$_PRESENT) {
    	    Make_Return_Address(tmp, sizeof(tmp));
    	    insert_header(tmp, newhq.tail, NEWS_K_HDR_APPROVED);
    	}

/*
** Set the Expires: header.
*/
    	status = cli_present("EXPIRES");
    	if (status == CLI$_PRESENT) {
    	    struct dsc$descriptor tdsc;
    	    TIME expiry;
    	    cli_get_value("EXPIRES", tmp, sizeof(tmp));
    	    INIT_SDESC(tdsc, strlen(tmp), tmp);
    	    status = lib$convert_date_string(&tdsc, &expiry);
    	    if (OK(status)) {
    	    	Make_Date(&expiry, tmp, sizeof(tmp));
    	    	insert_header(tmp, newhq.tail, NEWS_K_HDR_EXPIRES);
    	    }
    	}
/*
** Set the Summary: header.
*/
    	status = cli_present("SUMMARY");
    	if (status == CLI$_PRESENT) {
    	    cli_get_value("SUMMARY", tmp, sizeof(tmp));
    	    insert_header(tmp, newhq.tail, NEWS_K_HDR_SUMMARY);
    	}

/*
** Set the Keywords: header.
*/
    	status = cli_present("KEYWORDS");
    	if (status == CLI$_PRESENT) {
    	    char kw[STRING_SIZE], *fcp = kw;
    	    while (OK(cli_get_value("KEYWORDS", tmp, sizeof(tmp)))) {
    	    	if (fcp != kw) *fcp++ = ',';
    	    	locase(tmp);
    	    	strcpy(fcp, tmp);
    	    	fcp += strlen(tmp);
    	    }
    	    *fcp = '\0';
    	    if (*kw != '\0') {
    	    	insert_header(kw, newhq.tail, NEWS_K_HDR_KEYWORDS);
    	    }
    	}

    	strcpy(tmp, "Ready to post to: ");
    	strcat(tmp, fuptostr);
    	put_output(tmp);

    }

    if (mail_reply) {
    	struct HDR *h;
    	strcpy(tmp, "Ready to mail to: ");
    	cp = tmp+18;
    	for (h = destq.head; h != (struct HDR *) &destq; h = h->flink) {
    	    if (cp != tmp+18) {*cp++ = ','; *cp++ = ' ';}
    	    strcpy(cp, h->str);
    	    cp += strlen(h->str);
    	}
    	put_output(tmp);
    }

    go_ahead = Yes_Answer("Ok to proceed?", 1);

/*
** If the user says it's OK, then do it.
*/
    if (go_ahead) {
    	if (post_reply) go_ahead = OK(Post_Article(&newhq, outspec, use_sigp));
    	if (mail_reply && go_ahead) {
    	    int copy_self;
    	    status = cli_present("SELF");
    	    copy_self = (status == CLI$_PRESENT) ||
    	    	(news_prof.csreply && status != CLI$_NEGATED);
    	    go_ahead = OK(Mail_Message(&destq, subjstr, outspec, use_sigm,
    	    	    	copy_self, 0));
    	}
    }


/*
** Find out if we should keep or delete the message file.
*/
    if (extract) file_delete(infspec);

    if (!go_ahead && is_temp) {
    	go_ahead = !Yes_Answer("Keep file containing message?", 0);
    }
    if (go_ahead && is_temp) file_delete(outspec);
    else if (is_temp) {
    	strcpy(tmp, "Save as [");
    	strcat(tmp, outspec);
    	strcat(tmp, "]: ");
    	status = get_cmd(infspec, sizeof(infspec)-1, tmp);
    	if (OK(status) && infspec[0] != '\0') {
    	    status = Copy_File(outspec, infspec, refstr, 0);
    	    if (OK(status)) file_delete(outspec);
    	    else {
    	    	lib$signal(NEWS__WRITERR, 2, strlen(infspec), infspec, status);
    	    	strcpy(refstr, outspec);
    	    }
    	} else {
    	    strcpy(refstr, outspec);
    	}
    	strcpy(tmp, "Message retained in file ");
    	strcat(tmp, refstr);
    	put_output(tmp);
    }

/*
** Clean up
*/
    while (queue_remove(newhq.head, &hdr)) mem_freehdr(hdr);
    while (queue_remove(destq.head, &hdr)) mem_freehdr(hdr);

    return SS$_NORMAL;

} /* cmd_reply */

/*
**++
**  ROUTINE:	cmd_forward
**
**  FUNCTIONAL DESCRIPTION:
**
**  	FORWARD and STORE commands.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_forward()
**
**  IMPLICIT INPUTS:	Many.
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
unsigned int cmd_forward() {

    struct QUE *hdrq, destq;
    char subjstr[STRING_SIZE], infspec[FSPEC_SIZE], outspec[FSPEC_SIZE];
    char tmp[STRING_SIZE], tostr[STRING_SIZE], bodyfile[FSPEC_SIZE];
    struct HDR *h, *hdr;
    char *cp;
    int edit_flag, is_temp, go_ahead, use_sigfile, artnum, store_cmd;
    unsigned int runit, wunit, status;

    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }

/*
** Initialize and check to see if we are FORWARDing or STORing.
*/
    *subjstr = *infspec = *outspec = *tmp = *tostr = '\0';
    cli_get_value("$VERB", tmp, sizeof(tmp));
    store_cmd = strncmp(tmp, "STOR", 4) == 0;

    destq.head = destq.tail = &destq;

/*
** Which article?
*/
    if (cli_present("ARTNUM") == CLI$_PRESENT) {
    	cli_get_value("ARTNUM", tmp, sizeof(tmp));
    	lib$cvt_dtb(strlen(tmp), tmp, &artnum);
    } else if (cur_artnum == 0) {
    	    lib$signal(NEWS__NOCURART, 0);
    	    return SS$_NORMAL;
    } else {
    	artnum = cur_artnum;
    }

/*
** If this is a STORE, then we send to ourselves.  Otherwise, we send
** to whoever the user tells us to.
*/
    if (!store_cmd) {
    	while (OK(cli_get_value("TO", tmp, sizeof(tmp)))) {
    	    insert_header(tmp, destq.tail, 0);
    	}
    	if (destq.head == &destq) {
    	    status = get_cmd(tmp, sizeof(tmp), "To: ");
    	    if (OK(status)) {
    	    	int len = strlen(tmp);
    	    	while (len > 0 && isspace(tmp[len-1])) len--;
    	    	if (len == 0) return SS$_NORMAL;
    	    	tmp[len] = '\0';
    	    } else return SS$_NORMAL;
    	    status = csl_parse(tmp, &destq);
    	    if (!OK(status)) {
    	    	while (queue_remove(destq.head, &hdr)) mem_freehdr(hdr);
    	    	if (status != RMS$_EOF) lib$signal(NEWS__FWDERR, 0, NEWS__ADDRSYNTX, 0);
    	    	return SS$_NORMAL;
    	    }
    	}
    	status = cli_present("SUBJECT");
    	if (status == CLI$_PRESENT) {
    	    status = cli_get_value("SUBJECT", subjstr, sizeof(subjstr));
    	} else if (status != CLI$_NEGATED) {
    	    status = get_cmd(subjstr, sizeof(subjstr), "Subject: ");
    	    if (!OK(status)) return SS$_NORMAL;
    	}
    }

/*
** We need the headers early so we can get the Subject header out.
*/
    status = Get_Article_Hdrs(curgroup, artnum, &hdrq, 0, 0);
    if (!OK(status)) {
    	lib$signal(NEWS__FWDERR, 0, status);
    	return SS$_NORMAL;
    }
    if (store_cmd) {
    	for (hdr = hdrq->head; hdr != (struct HDR *) hdrq; hdr = hdr->flink) {
    	    if (hdr->code == NEWS_K_HDR_SUBJECT) {
    	    	strcpy(subjstr, hdr->str);
    	    	break;
    	    }
    	}
    }

/*
** Build the message to be sent
*/
    make_temp_fspec(infspec, sizeof(infspec));
    status = file_create(infspec, &wunit, 0, infspec);
    if (!OK(status)) {
    	lib$signal(NEWS__FWDERR, 0, status);
    	return SS$_NORMAL;
    }

    for (hdr = hdrq->head; hdr != (struct HDR *) hdrq; hdr = hdr->flink) {
    	int len;
    	Format_Header(hdr, tmp, sizeof(tmp), &len);
    	file_write(wunit, tmp, len);
    }
    file_write(wunit, "", 0);
    file_close(wunit);

    make_temp_fspec(tmp, sizeof(tmp));
    status = Get_Article_Body(artnum, tmp, bodyfile, 0, 0);
    if (!OK(status)) {
    	file_delete(infspec);
    	while (queue_remove(destq.head, &hdr)) mem_freehdr(hdr);
    	lib$signal(NEWS__FWDERR, 0, status, 0);
    	return SS$_NORMAL;
    }

/*
** Append the article body to the message.
*/

    status = Copy_File(bodyfile, infspec, 0, 1);
    file_delete(bodyfile);
    if (!OK(status)) {
    	file_delete(infspec);
    	while (queue_remove(destq.head, &hdr)) mem_freehdr(hdr);
    	lib$signal(NEWS__FWDERR, 0, status);
    	return SS$_NORMAL;
    }

/*
** Signature?
*/
    if (store_cmd) {
    	use_sigfile = 0;
    } else {
    	status = cli_present("SIGNATURE");
    	use_sigfile = (status == CLI$_PRESENT) ||
    	    	      (news_prof.autosigmail && status != CLI$_NEGATED);
    }


    cp = tostr;
    for (h = destq.head; h != (struct HDR *) &destq; h = h->flink) {
    	if (cp != tostr) {*cp++ = ','; *cp++ = ' ';}
    	strcpy(cp, h->str);
    	cp += strlen(h->str);
    }
    *cp = '\0';

/*
** FORWARDs can be edited
*/
    if (store_cmd) {
    	edit_flag = 0;
    } else {
    	status = cli_present("EDIT");
    	edit_flag = (status == CLI$_PRESENT) ||
    	    	    (news_prof.edit && status != CLI$_NEGATED);
    }

    status = Compose_Message(infspec, outspec, edit_flag, &is_temp);

/*
** If there was a problem or the forward was cancelled, clean up and
** get out of here.
*/
    if (!OK(status) || *outspec == '\0') {
    	if (!OK(status)) {
    	    lib$signal(NEWS__COMPOSERR, 0, status);
    	} else {
    	    lib$signal(NEWS__CANCELLED, 0);
    	}
    	if (is_temp && *outspec) file_delete(outspec);
    	file_delete(infspec);
    	while (queue_remove(destq.head, &hdr)) mem_freehdr(hdr);
    	return SS$_NORMAL;
    }

/*
** STOREs are automatic.  Otherwise, get confirmation.
*/
    if (store_cmd) {
    	go_ahead = 1;
    } else {
    	strcpy(tmp, "Ready to mail to: ");
    	strcat(tmp, tostr);
    	put_output(tmp);
    	go_ahead = Yes_Answer("Okay to proceed?", 1);
    }

/*
** If the user says OK, then mail it.
*/
    if (go_ahead) {
    	int copy_self;
    	if (store_cmd) {
    	    copy_self = 0;
    	} else {
    	    status = cli_present("SELF");
    	    copy_self = (status == CLI$_PRESENT) ||
    	    	    (news_prof.csfwd && status != CLI$_NEGATED);
    	}
    	Mail_Message(&destq, subjstr, outspec, use_sigfile, copy_self,
    	    	    	    store_cmd);
    }

/*
** Clean up
*/
    file_delete(infspec);
    if (is_temp) file_delete(outspec);

    while (queue_remove(destq.head, &hdr)) mem_freehdr(hdr);

    return SS$_NORMAL;

} /* cmd_forward */

/*
**++
**  ROUTINE:	cmd_cancel
**
**  FUNCTIONAL DESCRIPTION:
**
**  	CANCEL command.  Builds a "cancel" control message to cancel
**  an article.  It first checks to make sure that the current user posted
**  the article that should be cancelled.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_cancel()
**
**  IMPLICIT INPUTS:	Many.
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
unsigned int cmd_cancel() {

    char tmp[STRING_SIZE];
    struct QUE *hdrqptr, hdrq;
    struct HDR *hdr, *msgid;
    int chkdfrom, artnum;
    unsigned int status;

    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }

/*
** Which article?
*/
    if (cli_present("ARTNUM") == CLI$_PRESENT) {
    	cli_get_value("ARTNUM", tmp, sizeof(tmp));
    	lib$cvt_dtb(strlen(tmp), tmp, &artnum);
    	if (artnum > curgroup->lastavl || artnum < curgroup->frstavl) {
    	    lib$signal(NEWS__OUTRNG, artnum, curgroup->frstavl,
    	    	    curgroup->lastavl);
    	}
    	return SS$_NORMAL;
    } else if (cur_artnum == 0) {
    	lib$signal(NEWS__NOCURART, 0);
    	return SS$_NORMAL;
    } else {
    	artnum = cur_artnum;
    }

/*
** Verify that this user posted the article.  Anyone can do a cancel,
** you know.
*/
    chkdfrom = 0;
    msgid = NULL;
    status = Get_Article_Hdrs(curgroup, artnum, &hdrqptr, 0, 0);
    if (OK(status)) {
    	for (hdr = hdrqptr->head; hdr != (struct HDR *) hdrqptr; hdr = hdr->flink) {
    	    if (hdr->code == NEWS_K_HDR_FROM) {
    	    	struct QUE adrq;
    	    	struct HDR *myfrom, *itsfrom;
    	    	int do_it;

    	    	adrq.head = adrq.tail = &adrq;
    	    	Make_Return_Address(tmp, sizeof(tmp));
    	    	Parse_ToList(tmp, &adrq, 0);
    	    	queue_remove(adrq.head, &myfrom);
    	    	while (queue_remove(adrq.head, &itsfrom)) mem_freehdr(itsfrom);
    	    	Parse_ToList(hdr->str, &adrq, 0);
    	    	do_it = 0;
    	    	while (queue_remove(adrq.head, &itsfrom)) {
    	    	    if (strcmp(myfrom->str, itsfrom->str) == 0) do_it = 1;
    	    	    mem_freehdr(itsfrom);
    	    	}
    	    	if (!do_it) {
    	    	    lib$signal(NEWS__NOCANCEL, 1, artnum, NEWS__NOTYOURS, 4,
    	    	    	strlen(myfrom->str), myfrom->str,
    	    	    	strlen(hdr->str), hdr->str);
    	    	    mem_freehdr(myfrom);
    	    	    return SS$_NORMAL;
    	    	}
    	    	mem_freehdr(myfrom);
    	    	chkdfrom = 1;
    	    	if (msgid != NULL) break;
    	    }
    	    if (hdr->code == NEWS_K_HDR_MESSAGE_ID) {
    	    	msgid = hdr;
    	    	if (chkdfrom) break;
    	    }
    	}
/*
** Everything's ok: build the cancel message and post it.
*/
    	if (hdr != (struct HDR *) hdrqptr) {
    	    hdrq.head = hdrq.tail = &hdrq;
    	    insert_header(curgroup->grpnam, hdrq.tail, NEWS_K_HDR_NEWSGROUPS);
    	    strcpy(tmp, "cancel ");
    	    strcat(tmp, msgid->str);
    	    insert_header(tmp, hdrq.tail, NEWS_K_HDR_CONTROL);
    	    insert_header(tmp, hdrq.tail, NEWS_K_HDR_SUBJECT);
    	    status = Post_Article(&hdrq, 0, 0);
    	    while (queue_remove(hdrq.head, &hdr)) mem_freehdr(hdr);
    	} else {
    	    lib$signal(NEWS__NOCANCEL, 1, artnum);
    	}
    } else {
    	lib$signal(NEWS__NOCANCEL, 1, artnum);
    }

    return SS$_NORMAL;

} /* cmd_cancel */    
