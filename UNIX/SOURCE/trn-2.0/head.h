/* $Id: head.h,v 1.2 92/01/11 16:04:33 usenet Exp $
 *
 * $Log:	head.h,v $
 * Revision 1.2  92/01/11  16:04:33  usenet
 * header twiddling, bug fixes
 * 
 * Revision 4.4  1991/09/09  20:18:23  sob
 * release 4.4
 *
 *
 * 
 */
/* This software is Copyright 1991 by Stan Barber. 
 *
 * Permission is hereby granted to copy, reproduce, redistribute or otherwise
 * use this software as long as: there is no monetary profit gained
 * specifically from the use or reproduction of this software, it is not
 * sold, rented, traded or otherwise marketed, and this copyright notice is
 * included prominently in any copy made. 
 *
 * The author make no claims as to the fitness or correctness of this software
 * for any use whatsoever, and it is provided as is. Any use of this software
 * is at the user's own risk. 
 */

#define HEAD_FIRST 1

/* types of header lines (if only C really believed in enums)
 * (These must stay in alphabetic order at least in the first letter.
 * Within each letter it helps to arrange in increasing likelihood.)
 */

#define PAST_HEADER	0	/* body */
#define SOME_LINE	1	/* unrecognized */
#define ARTID_LINE	2	/* article-i.d. */
#define APPR_LINE	3	/* approved */
#define ACAT_LINE	4	/* ACategory (ClariNet) */
#define ANPA_LINE	5	/* ANPA (ClariNet) */
#define CODES_LINE	6	/* Codes (ClariNet) */
#define DIST_LINE	7	/* distribution */
#define DATE_LINE	8	/* date */
#define RECEIVED_LINE	9	/* date-received */
#define EXPIR_LINE	10	/* expires */
#define FOLLOW_LINE	11	/* followup-to */
#define FROM_LINE	12	/* from */
#define FORM_LINE	13	/* Format (ClariNet) */
#define KEYW_LINE	14	/* keywords */
#define LINES_LINE	15	/* lines */
#define MESSID_LINE	16	/* message-id */
#define NFFR_LINE	17	/* nf-from */
#define NFID_LINE	18	/* nf-id */
#define NGS_LINE	19	/* newsgroups */
#define NOTE_LINE	20	/* Note (ClariNet) */
#define ORG_LINE	21	/* organization */
#define PATH_LINE	22	/* path */
#define POSTED_LINE	23	/* posted */
#define PVER_LINE	24	/* posting-version */
#define PRI_LINE	25	/* Priority (ClariNet) */
#define REPLY_LINE	26	/* reply-to */
#define REFS_LINE	27	/* references */
#define RVER_LINE	28	/* relay-version */
#define SENDER_LINE	29	/* sender */
#define SUMRY_LINE	30	/* summary */
#define SUBJ_LINE	31	/* subject */
#define SLUG_LINE	32	/* Slugword (ClariNet) */
#define XREF_LINE	33	/* xref */
#define XSUP_LINE	34	/* X-Supersedes (ClariNet) */
#define HEAD_LAST	35	/* one more than the last one above */

struct headtype {
    char *ht_name;		/* header line identifier */
#ifdef pdp11
    short ht_minpos;
    short ht_maxpos;
#else
    ART_POS ht_minpos;		/* pointer to beginning of line in article */
    ART_POS ht_maxpos;		/* pointer to end of line in article */
#endif
    char ht_length;		/* could make these into nybbles but */
    char ht_flags;		/* it wouldn't save space normally */
};				/* due to alignment considerations */

#define HT_HIDE 1	/* -h on this line */
#define HT_MAGIC 2	/* do any special processing on this line */

/* This array must stay in the same order as the list above */

#ifndef DOINIT
EXT struct headtype htype[HEAD_LAST];
#else
struct headtype htype[HEAD_LAST] = {
 /* name             minpos   maxpos  length   flag */
    {"BODY",		0,	0,	4,	0		},
    {"unrecognized",	0,	0,	12,	0		},
    {"article-i.d.",	0,	0,	12,	HT_HIDE		},
    {"approved",	0,	0,	8,	HT_HIDE		},
    {"acategory",	0,	0,	9,	HT_HIDE		},
    {"anpa",		0,	0,	4,	HT_HIDE		},
    {"codes",		0,	0,	5,	HT_HIDE		},
    {"distribution",	0,	0,	12,	0		},
#ifdef USETHREADS
    {"date",		0,	0,	4,	HT_MAGIC	},
#else
    {"date",		0,	0,	4,	0		},
#endif
    {"date-received",	0,	0,	13,	0		},
    {"expires",		0,	0,	7,	HT_HIDE|HT_MAGIC},
    {"followup-to",	0,	0,	11,	0		},
    {"from",		0,	0,	4,	HT_MAGIC	},
    {"format",		0,	0,	6,	HT_HIDE		},
    {"keywords",	0,	0,	8,	0		},
    {"lines",		0,	0,	5,	0		},
    {"message-id",	0,	0,	10,	HT_HIDE		},
    {"nf-from",		0,	0,	7,	HT_HIDE		},
    {"nf-id",		0,	0,	5,	HT_HIDE		},
    {"newsgroups",	0,	0,	10,	HT_MAGIC|HT_HIDE},
    {"note",		0,	0,	4,	0,		},
    {"organization",	0,	0,	12,	0		},
    {"path",		0,	0,	4,	HT_HIDE		},
    {"posted",		0,	0,	6,	HT_HIDE		},
    {"posting-version",	0,	0,	15,	HT_HIDE		},
    {"priority",	0,	0,	8,	HT_HIDE		},
    {"reply-to",	0,	0,	8,	HT_HIDE		},
    {"references",	0,	0,	10,	HT_HIDE		},
    {"relay-version",	0,	0,	13,	HT_HIDE		},
    {"sender",		0,	0,	6,	HT_HIDE		},
    {"summary",		0,	0,	7,	0		},
    {"subject",		0,	0,	7,	HT_MAGIC	},
    {"slugword",	0,	0,	8,	HT_HIDE		},
    {"xref",		0,	0,	4,	HT_HIDE		},
    {"x-supersedes",	0,	0,	12,	HT_HIDE		}
};
#endif

#ifdef ASYNC_PARSE
EXT ART_NUM parsed_art INIT(0);
#endif

EXT char in_header INIT(0);		/* are we decoding the header? */

#ifdef CACHESUBJ
    EXT char **subj_list INIT(Null(char **));
#endif

void	head_init ANSI((void));
int	set_line_type ANSI((char *,char *));
void	start_header ANSI((ART_NUM));
bool    parseline ANSI((char *,int,int));
#ifdef ASYNC_PARSE
    int		parse_maybe ANSI((ART_NUM));
#endif
char	*fetchsubj ANSI((ART_NUM,bool_int,bool_int));
char	*fetchlines ANSI((ART_NUM,int));

#ifdef DEBUGGING
void	dumpheader ANSI((char *));
#endif
