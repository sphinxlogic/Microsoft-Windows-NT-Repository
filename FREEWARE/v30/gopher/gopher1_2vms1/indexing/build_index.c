/* index sequential files, producing .idx and .sel files */
/* Bruce Tanner - Cerritos College */

/*
Version History:

1.0  05/10/93 Original attempt
2.0  06/20/93 Create indexed files directly, add keyword count field
2.1  07/08/93 Change the file name for NOISE_WORDS
2.2  07/08/93 Move the range end (end_pos) to before the terminator
2.2jlw 07/14/93 - JLW added length spec to dash, added additional topic
                  divider keywords
2.3  07/19/93 Set multi-buffer, multi-block counts, read-ahead, write-behind
              and deferred write; noticeably increased performance
2.4  07/26/93 Removed index name, added CLI$ interface, added /TOPIC
2.4jlw 07/27/93 fixed version retention, which was broken
2.5  07/27/93 Selector strings forced to lowercase; use a good copy
2.6  07/29/93 revamp /TOPIC syntax to include text, size, exclude
2.7  07/30/93 make SIZE=n pad as well as truncate field width
2.8  08/03/93 take wildcard input file names, add /OUTPUT, /VERSION
2.9  08/05/93 JLW changed filename sizes from 80 to 256 characters
2.10 08/05/93 add check for max number of topics, reformat code
2.11 08/24/93 JLW added specific statuses for exit errors
2.12 10/01/93 add /NODEFAULT_TOPIC to omit topics that have no topic keyword
2.13 11/03/93 add /LINK to generate .link file instead of .idx/.sel
2.14 11/15/93 add /NOISE=file to specify the noise words file
2.15 11/17/93 add /TOPIC=(position), /FIELD=(position, size), /PUNCTUATION
2.16 11/18/93 fix illegal strcpy for AXP, add /MAX_TOPICS
2.17 11/21/93 make load_noise friendlier, add /NOPUNCTUATION support
2.18 11/27/93 add /MINIMUM_WORD, /COUNT_WORDS
2.19 11/30/93 fix broken /TOPIC
2.20 03/20/94 sort words, add /LINK=SORT, /SEQUENTIAL, remove /COUNT_WORDS
2.21 04/29/94 add /NONUMBERS
2.22 06/23/94 add /TOPIC=(offset) /TOPIC=(position=0)
2.23 06/24/94 add /TOPIC=(end)
2.24 06/27/94 add /CANDIDATE, /KEYWORD=(text,end,exclude)
2.24a 06/29/94 replaced VAXC-specific "#include foo" declarations with
		more portable "#include <foo.h>" (so DECC won't balk).
2.25 08/04/94 fix /TOPIC=END not matching
2.26 09/15/94 /KEYWORD=END=foo stopped at end of line if 'foo' wasn't found
2.27 09/27/94 change get_text() to return updated pointer to fix mangled text
3.0  09/29/94 redo parsing routines, add /SPECIFICATION, /TOPIC=BREAK
3.1  10/10/94 add /SELECTOR, don't index selector line
3.2  10/17/94 change /SELECTOR to /SELECTOR=(TEXT,END,BOTH)
*/
 
#include <ssdef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <rms.h>
#include <descrip.h>
#include <climsgdef.h>
#include <lib$routines.h>
#include <starlet.h>

#define CHUNK 100     /* increment to expand table of words */
#define DESC_SIZE 500  /* maximum size of a topic description */
#define SELECTOR_SIZE 100 /* maximum size of a selector (minus description) */
#define TOPIC_SIZE 20 /* maximum number of topics to list */
#define PUNCT_CHARS  "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"
#define DEFAULT_POS 0 /* column to look for topic by default; 0 = anywhere */
#define MAX_INPUT_LINE 500

typedef struct {
    char *text;
    char *found;
    char *end;
    int  pos;
    int  size;
    int  offset;
    int  exclude;
    int  force_break;
    int  used;
} topic_str;

typedef struct {
    char *name;
    int  state;
    char *value;
} switch_str;

typedef struct dsc$descriptor_s dsc;

int words_index, words_size;
char **words;
int noise_index, noise_size;
int candidate_index, candidate_size;
char **noise;
char **candidate;
int max_word, max_topic, max_count, sequential;
char *idx_record, *prev_keyword;
char sel_record[DESC_SIZE + SELECTOR_SIZE + 100];
int keyword_index;
FILE *spc;
switch_str switches[50];
topic_str topics[TOPIC_SIZE];
topic_str keywords[TOPIC_SIZE];
char *selector_spec;

int cli$dcl_parse();
int cli$get_value();
int cli$present();

void build_words(char *, char *, int);
void test_words(char *, char *, int, topic_str *);
void expand_table(char ***, int *);
void write_words(FILE *, FILE *, struct RAB *, struct RAB*, int *, int *,
                 char *, topic_str *);
void load_words(char *, char *, char ***, int *, int *);
int is_noise(char *, int, int);
int is_candidate(char *, int);
int is_punct(char, char *);
int is_spaces(char *, int, int);
dsc *descr(char *string);
void parse_topic(char *, topic_str *);
void parse_keyword(char *, topic_str *);
void *my_realloc(void *, int);
void index_commands();
int find_str(char *, char *);
void parse_commands(dsc *, switch_str[]);
int switch_present(char *);
char *switch_value(char *);


main(int argc, char *argv[])
{
    FILE  *src, *lnk;
    char  *cp, *cp2, *ptr, desc[DESC_SIZE + 1], src_line[MAX_INPUT_LINE];
    static char cli_input[256], punctuation[128], temp_punct[128];
    static char value[20], file_arg[256], file_spec[256], out_name[256];
    static char spec_name[256], spec_line[270];
    char  orig_line[MAX_INPUT_LINE], lc_line[MAX_INPUT_LINE], spaces[DESC_SIZE + 1];
    int   start_pos, end_pos, status, index, context = 0;
    enum  {none, para, dash, hex, equal, line, whole, field} type = none;
    int   dash_len = 0, ind, minimum_word, where;
    int   hex_value, field_pos = 1, field_size;
    short leng;
    char  *dashes = NULL;
    struct FAB idxfab, selfab;
    struct RAB idxrab, selrab;
    struct XABKEY idxxab, selxab;
    $DESCRIPTOR(input_dsc, cli_input);
    $DESCRIPTOR(file_dsc, file_arg);
    $DESCRIPTOR(file_spec_dsc, file_spec);
    $DESCRIPTOR(out_dsc, out_name);
    $DESCRIPTOR(punct_dsc, temp_punct);
    $DESCRIPTOR(value_dsc, value);
    $DESCRIPTOR(spec_dsc, spec_name);
    $DESCRIPTOR(spec_line_dsc, spec_line);

    for (index = 0; index < TOPIC_SIZE; index++) {
        topics[index].text = NULL;
        topics[index].end = NULL;
        topics[index].found = NULL;
        topics[index].pos = 0;
        topics[index].size = 0;
        topics[index].offset = 0;
        topics[index].exclude = 0;
        topics[index].used = 0;
        topics[index].force_break = 0;

        keywords[index].text = NULL;
        keywords[index].end = NULL;
        keywords[index].found = NULL;
        keywords[index].pos = 0;
        keywords[index].size = 0;
        keywords[index].offset = 0;
        keywords[index].exclude = 0;
        keywords[index].used = 0;
        keywords[index].force_break = 0;
    }

    status = lib$get_foreign(&input_dsc, 0, &leng, 0);

    for (ind = leng; ind >= 0; ind--)
        cli_input[ind+6] = cli_input[ind];
    strncpy(cli_input, "index ", 6);            
    input_dsc.dsc$w_length = leng+6;

    status = cli$dcl_parse(&input_dsc, index_commands);

    if (status != CLI$_NORMAL)  /* error in parse, exit */
        exit(7);

    if ((cli$present(descr("file")) & 1) == 0) {
        printf("Usage: index document\n");
        printf(" /CANDIDATES=file    specify a file of words for index candidates\n");
        printf(" /CHARACTER=n        text separated by control character 'n'\n");
        printf(" /DASH=n             text separated n dashes (default 3)\n");
        printf(" /[NO]DEFAULT_TOPIC  keep [discard] topics [not] matched by /TOPIC\n");
        printf(" /EQUAL=n            text separated n equals (default 80)\n");
        printf(" /FF                 text separated by form feeds\n");
        printf(" /FIELD=(position,size)  specify topic break on field\n");
        printf(" /KEYWORD=(text, end, exclude) specify indexing range\n");
        printf(" /LINE               each line is separate text entry\n");
        printf(" /LINK[=SORT]        generate .link file instead of .idx,.sel files\n");
        printf(" /MAX_TOPICS=n       maximum size of topic ID field (default 4)\n");
        printf(" /MINIMUM_WORD=n     define minimum word to index (default 3)\n");
        printf(" /NOISE=file         specify a file of words to omit in the index\n");
        printf(" /NONUMBERS          omit all numbers from the index\n");
        printf(" /OUTPUT=file        override name of index/selection files\n");
        printf(" /PARAGRAPH          text separated by blank lines\n");
        printf(" /PUNCTUATION=\"...\"  specify the characters that separate words\n");
        printf(" /SEQUENTIAL         create sequential files (.seqidx, .seqsel)\n");
        printf(" /SPECIFICATION=file specify a file of switches\n");
        printf(" /TOPIC=(text,end,position,size,offset,exclude)  specify topic names\n");
        printf(" /[NO]VERSION        keep [discard] document version in selection\n");
        printf(" /WHOLE              whole file is one text entry\n");
        printf(" /WORD_LENGTH=n      maximum size of index key (default 20)\n");
        exit(1);
    }

    if (cli$present(descr("specification")) & 1) { 
        status = cli$get_value(descr("specification"), &spec_dsc, &leng);
        spec_name[leng] = '\0';
        if ((spc = fopen(spec_name, "r")) == NULL) {
            printf("Can't read spec file %s\n", spec_name);
            exit(13);
        }
        /* parse every line of the spec file */
        while (fgets(spec_line, sizeof(spec_line), spc)) {
            if ((spec_line[0] == '\n') || (spec_line[0] == '#') ||
                (spec_line[0] == '!'))
                continue;                 /* skip blank and comment lines */
            ptr = strchr(spec_line, '\n');
            if (ptr) *ptr = '\0';
            leng = strlen(spec_line);
            for (ind = leng; ind >= 0; ind--)
                spec_line[ind+6] = spec_line[ind];
            strncpy(spec_line, "index ", 6);            
            spec_line_dsc.dsc$w_length = leng+6;

            parse_commands(&spec_line_dsc, switches);
        }
    }

    parse_commands(&input_dsc, switches);

    if (switch_present("paragraph"))
        type = para;
    if (switch_present("FF")) {
        type = hex;                      /* /FF same as /character=12 */
        hex_value = '\f';
    }
    if (switch_present("character")) {
        hex_value = atoi(switch_value("character"));
        type = hex;
    }
    if (switch_present("whole"))
        type = whole;
    if (switch_present("line"))
        type = line;
    if (switch_present("dash")) {
        dash_len = atoi(switch_value("dash"));
        type = dash;
    }
    if (switch_present("equal")) {
        dash_len = atoi(switch_value("equal"));
        type = equal;
    }
    if (switch_present("word_length")) {
        max_word = atoi(switch_value("word_length"));
    }
    if (switch_present("field")) {
        type = field;
        field_pos = atoi(switch_value("field.position"));
        field_size = atoi(switch_value("field.size"));
    }
    strcpy(punctuation, PUNCT_CHARS);  /* default for /punctuation */
    if (switch_present("punctuation")) {
        strcpy(temp_punct, switch_value("punctuation"));
        if (temp_punct[0] == '"') {  /* if quoted string */
            strncpy(punctuation, temp_punct+1, leng-2);
            punctuation[leng-2] = '\0';
        }
        else if (strlen(temp_punct) > 0)
            strcpy(punctuation, temp_punct);
    }
    else {  /*  /nopunctuation="$" means exclude $ from punct chars */
        if (cp = switch_value("punctuation"))
            strcpy(temp_punct, cp);
        else
            strcpy(temp_punct, "");
        if (temp_punct[0] == '"') {   /* if quoted string */
            strcpy(temp_punct, temp_punct+1);
            temp_punct[leng-2] = '\0';
        }
        for (cp = temp_punct; *cp; cp++) {
            cp2 = strchr(punctuation, *cp);
            if (cp2)
                strcpy(cp2, cp2+1);  /* remove character from punctuation */
        }
    }
    if (switch_present("max_topics")) {
        max_topic = max_count = atoi(switch_value("max_topics"));  /* query assumes topic = count */
        if (max_topic > 9) {
            printf("/MAX_TOPICS specifies the number of digits in the topic number field.\n");
            printf("A 32 bit system cannot handle integers greater than 9 digits.\n");
            exit(9);
        }
    }
    if (switch_present("minimum_word"))
        minimum_word = atoi(switch_value("minimum_word"));

    sequential = (switch_present("sequential"));

    strcpy(file_arg, switch_value("file"));  /* get source */
    file_dsc.dsc$w_length = (short) strlen(file_arg);  /* set the descriptor length */

    strncpy(file_spec, "", sizeof(file_spec));  /* clear out file_spec */
    status = lib$find_file(&file_dsc, &file_spec_dsc, &context, 0, 0, 0, 0);
    if ((status & 1) == 0) {
        perror("lib$find_file failed");
        exit(11);
    }
    ptr = strchr(file_spec, ' ');
    if (ptr)
        *ptr = '\0';            /* chop off trailing spaces */

    strcpy(out_name, file_spec);    /* make copy for output spec */

    if (switch_present("output"))   /* if /output, overwrite out_name */
        strcpy(out_name, switch_value("output"));

    words_size = words_index = 0;    /* no words yet */
    words = NULL;
    noise_size = noise_index = 0;    /* no noise yet */
    noise = NULL;
    candidate_size = candidate_index = 0;    /* no candidate yet */
    candidate = NULL;
    if (!switch_present("link")) {
        load_words("noise", punctuation, &noise, &noise_size, &noise_index);
        load_words("candidates", punctuation, &candidate, &candidate_size, &candidate_index);
    }

    dashes = (char *)malloc(dash_len+1);
    memset((void *)dashes, (type==dash)?'-':'=', dash_len);
    dashes[dash_len] = '\0';

    memset((void *) spaces, ' ', DESC_SIZE); /* make spaces for padding topic */
    spaces[DESC_SIZE] = '\0';

    idx_record = (char *) calloc(max_word + max_count + max_topic + 1,
                                 sizeof(char));

    prev_keyword = (char *) calloc(max_word + 1, sizeof(char));

    ptr = strrchr(out_name, '.');  /* just get file name */
    if (ptr) *ptr = '\0';
    if (sequential)
        strcat(out_name, ".seqidx");
    else
        strcat(out_name, ".idx");

    idxfab = cc$rms_fab;
    idxfab.fab$l_alq = 100;
    idxfab.fab$b_bks = 3;
    idxfab.fab$w_deq = 25;
    idxfab.fab$b_fac = FAB$M_PUT;
    idxfab.fab$l_fna = out_name;
    idxfab.fab$b_fns = strlen(out_name);
    idxfab.fab$l_fop = FAB$M_CBT | FAB$M_DFW;
    idxfab.fab$w_mrs = max_word + max_topic + max_count;
    if (sequential)
        idxfab.fab$b_org = FAB$C_SEQ;
    else
        idxfab.fab$b_org = FAB$C_IDX;
    idxfab.fab$b_rat = FAB$M_CR;
    idxfab.fab$b_rfm = FAB$C_FIX;
    idxfab.fab$b_shr = FAB$M_NIL;
    idxfab.fab$l_xab = (char *) &idxxab;

    idxrab = cc$rms_rab;
    idxrab.rab$l_fab = (struct FAB *) &idxfab;
    idxrab.rab$b_krf = 0;
    if (sequential)
        idxrab.rab$b_rac = RAB$C_SEQ;
    else
        idxrab.rab$b_rac = RAB$C_KEY;
    idxrab.rab$l_rbf = idx_record;
    idxrab.rab$w_rsz = max_word + max_topic + max_count;
    idxrab.rab$l_ubf = idx_record;
    idxrab.rab$w_usz = max_word + max_topic + max_count;
    idxrab.rab$b_mbf = 20;
    idxrab.rab$l_rop = RAB$M_RAH | RAB$M_WBH;

    idxxab = cc$rms_xabkey;
    idxxab.xab$b_dtp = XAB$C_STG;
    idxxab.xab$b_flg = XAB$M_DAT_NCMPR | XAB$M_IDX_NCMPR;
    idxxab.xab$w_pos0 = 0;
    idxxab.xab$b_siz0 = max_word + max_topic;
    idxxab.xab$b_ref = 0;

    if (!switch_present("link")) {
        if (((status = sys$create(&idxfab)) & 1) != SS$_NORMAL)
            lib$stop(status);
        if (((status = sys$connect(&idxrab)) & 1) != SS$_NORMAL)
            lib$stop(status);
    }

    ptr = strrchr(out_name, '.');  /* just get file name */
    if (ptr) *ptr = '\0';
    if (sequential)
        strcat(out_name, ".seqsel");
    else
        strcat(out_name, ".sel");

    selfab = cc$rms_fab;
    selfab.fab$l_alq = 10;
    selfab.fab$b_bks = 3;
    selfab.fab$w_deq = 5;
    selfab.fab$b_fac = FAB$M_PUT;
    selfab.fab$l_fna = out_name;
    selfab.fab$b_fns = strlen(out_name);
    selfab.fab$l_fop = FAB$M_CBT | FAB$M_DFW;
    selfab.fab$w_mrs = max_topic + DESC_SIZE + SELECTOR_SIZE;
    if (sequential)
        selfab.fab$b_org = FAB$C_SEQ;
    else
        selfab.fab$b_org = FAB$C_IDX;
    selfab.fab$b_rat = FAB$M_CR;
    selfab.fab$b_rfm = FAB$C_VAR;
    selfab.fab$b_shr = FAB$M_NIL;
    selfab.fab$l_xab = (char *) &selxab;

    selrab = cc$rms_rab;
    selrab.rab$l_fab = (struct FAB *) &selfab;
    if (sequential)
        selrab.rab$b_rac = RAB$C_SEQ;
    else
        selrab.rab$b_rac = RAB$C_KEY;
    selrab.rab$l_rbf = sel_record;
    selrab.rab$b_mbf = 20;
    selrab.rab$l_rop = RAB$M_RAH | RAB$M_WBH;

    selxab = cc$rms_xabkey;
    selxab.xab$b_dtp = XAB$C_STG;
    selxab.xab$b_flg = XAB$M_DAT_NCMPR | XAB$M_IDX_NCMPR;
    selxab.xab$w_pos0 = 0;
    selxab.xab$b_siz0 = max_topic;
    selxab.xab$b_ref = 0;

    if (!switch_present("link")) {
        if (((status = sys$create(&selfab)) & 1) != SS$_NORMAL)
            lib$stop(status);
        if (((status = sys$connect(&selrab)) & 1) != SS$_NORMAL)
            lib$stop(status);
    }

    if (switch_present("link")) {
        ptr = strrchr(out_name, '.');
        if (ptr) *ptr = '\0';
        strcat(out_name, ".link");
        lnk = fopen(out_name, "w", "mbc=50", "mbf=20");
        if (!switch_present("link.sort"))
            fprintf(lnk, "Sortdir=False\n\n");
    }

    for (;;) {  /* process all files in input spec, first one already found */

        if ((src = fopen(file_spec, "r", "mbc=50", "mbf=20")) == NULL) { 
            printf("Can't read input file %s\n", file_spec);
            exit(3);
        }
        printf("Building index for %s\n", file_spec);

        start_pos = ftell(src);          /* init start position */
        strncpy(desc, "", DESC_SIZE + 1);
        
        while (fgets(src_line, sizeof(src_line), src)) {
            /* if the first character of the line is the hex value, end topic */
            if ((src_line[0] == hex_value) && (type == hex)) {
                write_words(src, lnk, &selrab, &idxrab, &start_pos,
                            &end_pos, desc, topics);
                continue;
            }
            ptr = strchr(src_line, '\n');
            if (ptr) *ptr = '\0';              /* remove newline */
            for (ptr = src_line; *ptr; ptr++)
                if (iscntrl(*ptr))  *ptr = ' ';  /* convert tabs to spaces */
            while ((strlen(src_line) > 0) &&
                   (src_line[strlen(src_line)-1] == ' '))
                src_line[strlen(src_line)-1] = '\0';/* remove trailing blanks */
            strcpy(orig_line, src_line);    /* copy before forcing lower case */
            for (ptr = src_line; *ptr; ptr++)
                *ptr = _tolower(*ptr);           /* force lowercase */
            strcpy(lc_line, src_line);      /* copy with leading blanks */
            for (ptr = src_line; *ptr; ptr++)
                if (*ptr > ' ') break;  /* find first non-blank char */
            strcpy(src_line, ptr);      /* remove leading blanks */
        
            /* break on dashes */
            if (((type == equal) || (type == dash)) &&
                (strncmp(orig_line, dashes, dash_len) == 0)) {
                write_words(src, lnk, &selrab, &idxrab, &start_pos,
                            &end_pos, desc, topics);
                continue;
            }
            /* breakon paragraph */
            if ((type == para) && (strlen(src_line) == 0)) {
                write_words(src, lnk, &selrab, &idxrab, &start_pos,
                            &end_pos, desc, topics);
                continue;
            }
            /* break on non-empty field */
            if ((type == field) && !is_spaces(orig_line, field_pos, field_size)) {
                write_words(src, lnk, &selrab, &idxrab, &start_pos,
                            &end_pos, desc, topics);
                start_pos = end_pos;   /* don't skip over line with field break */
            }
            /* save the first line by default */ 
            if ((switch_present("default_topic")) && (strlen(desc) == 0))
                strncpy(desc, orig_line, DESC_SIZE);

            if (switch_present("selector.text") &&
                (where = find_str(lc_line, switch_value("selector.text")))) {
                selector_spec = (char *) my_realloc((char *) selector_spec,
                                         strlen(orig_line) + 1);
                strcpy(selector_spec, orig_line + where - 1 +
                       strlen(switch_value("selector.text")));
                if (switch_present("selector.end") &&  /* if selector=end given */
                    (where = find_str(selector_spec, switch_value("selector.end"))))
                    selector_spec[where - 1] = '\0';  /* mark selector end */
                while (*selector_spec == ' ')  /* remove leading spaces */
                    strcpy(selector_spec, selector_spec + 1);
                cp = strchr(selector_spec, ' ');  /* stop on space */
                if (cp) *cp = '\0';
                continue;                    /* do not index this line */
            }
            else if (selector_spec == NULL)
                selector_spec = (char *) calloc(1, sizeof(char)); /* make empty spec */

            for (index = 0; topics[index].used; index++) { /* apply topic rules */
                where = topics[index].pos;      /* where text is found */
                /* if text matches the source text and position */
                if (
                    ((topics[index].pos > 0) && topics[index].text
                     && strncmp(lc_line + topics[index].pos - 1,
                            topics[index].text, strlen(topics[index].text)) == 0)
                    ||  /* or position = 0 and text is found _somewhere_ */
                    ((topics[index].pos == 0)
                     && (where = find_str(lc_line, topics[index].text)))
                    || /* or no text given but position and size field is non-blank */
                    (!topics[index].text &&
                     !is_spaces(orig_line, topics[index].pos, topics[index].size))
                   ) {
                    /* if topic matches and requested a break, do it */
                    if (topics[index].force_break) {
                        write_words(src, lnk, &selrab, &idxrab, &start_pos,
                                    &end_pos, desc, topics);
                        start_pos = end_pos;   /* don't skip over topic line */
                    }
                    if (topics[index].exclude)
                        strcpy(orig_line + where - 1,
                               orig_line + where - 1 + strlen(topics[index].text));
                    topics[index].found = (char *) my_realloc((char *) topics[index].found,
                                         (topics[index].size ? topics[index].size : strlen(orig_line))
                                         + 1);
                    if (topics[index].size > 0) {
                        strncpy(topics[index].found, orig_line
                                + where - 1 + topics[index].offset,
                                topics[index].size);
                        topics[index].found[topics[index].size] = '\0';
                        strncat(topics[index].found, spaces,
                                topics[index].size - strlen(topics[index].found));
                    }
                    else {
                        strcpy(topics[index].found, orig_line
                               + where - 1 + topics[index].offset);
                        if (where = find_str(topics[index].found, topics[index].end))
                            topics[index].found[where - 1] = '\0';    /* terminate the found string */
                    }
                break;     /* a line satisfies only one topic rule */
                }
            }
            if (!switch_present("link"))
                test_words(src_line, punctuation, minimum_word, keywords);
            end_pos = ftell(src);  /* end_pos points before any terminator */
            if (type == line)
                write_words(src, lnk, &selrab, &idxrab, &start_pos,
                            &end_pos, desc, topics);
        }
        
        /* in case file doesn't end with a terminator */
        write_words(src, lnk, &selrab, &idxrab, &start_pos, &end_pos,
                    desc, topics);
        fclose(src);
        if (switch_present("link"))
            fclose(lnk);
        status = lib$find_file(&file_dsc, &file_spec_dsc, &context, 0, 0, 0, 0);
        if ((status & 1) == 0) {
            lib$find_file_end(&context);
            break;
        }
        ptr = strchr(file_spec, ' ');
        if (ptr) *ptr = '\0';            /* chop off trailing spaces */
    }
    sys$close(&selfab);
    sys$close(&idxfab);
}

/* test for start/end of keyword indexing */
void test_words(char *line, char *punct, int minimum_word, topic_str *keywords)
{
    char test_line[MAX_INPUT_LINE], copy_line[MAX_INPUT_LINE];
    int ind, where, retry;

    if (!switch_present("keyword")) {  /* no /keyword= */
        build_words(line, punct, minimum_word);  /* index everything */
        return;
    }

    if ((keyword_index > -1)
        && ((keywords[keyword_index].end == NULL)
            || (strlen(keywords[keyword_index].end) == 0)))
        keyword_index = -1;  /* keyword indexing stops at EOL unless /keyword=end */

    strcpy(test_line, line);        /* copy source line */
    do {
        where = 0;
        retry = FALSE;
        if (keyword_index == -1) {      /* between keywords */
            for (ind = 0; keywords[ind].used; ind++)
                if (where = find_str(test_line, keywords[ind].text))
                    break;
            if (where) {
                keyword_index = ind;          /* record current keyword */
                strcpy(test_line, test_line + where - 1);  /* remove up to keyword */
                if (keywords[ind].exclude)
                    strcpy(test_line, test_line + strlen(keywords[ind].text));
            }
        }
        if (keyword_index > -1)       /* in keyword index */
            if (where = find_str(test_line, keywords[keyword_index].end)) {
                strcpy(copy_line, test_line);
                test_line[where - 1] = '\0';
                build_words(test_line, punct, minimum_word);  /* index contents of line */
                strcpy(test_line, copy_line + where - 1);  /* restart at end word */
                keyword_index = -1;   /* no longer indexing */
                retry = TRUE;        /* check for another keyword */
            }
            else {  /* indexing and no end word found */
                build_words(test_line, punct, minimum_word);  /* index contents of line */
                return;
            }
    } while (retry);
}


/* break line into words and save them in words[] */

void build_words(char *line, char *punct, int minimum_word)
{
    char *cp, *cp2;

    for (cp = line; *cp; cp++)      /* convert punctuation to spaces */
        if (is_punct(*cp, punct)) *cp = ' ';

    strcat(line, " ");              /* line ends with a space */
    cp = line;
    while(cp2 = strchr(cp, ' ')) {  /* break at space boundary */
        *cp2 = '\0';
        if (strlen(cp) > max_word)
            printf("Truncating %d character word (%s) to %d characters\n",
                   strlen(cp), cp, max_word);
        if ((strlen(cp) > 0) &&
            (((candidate_size == 0) &&
             !is_noise(cp, noise_size, minimum_word)) ||
            (candidate_size && is_candidate(cp, candidate_size)))) {
            if (words_index == words_size)  /* table full */
                expand_table(&words, &words_size);
            strncpy(words[words_index++], cp, max_word);
        }
        cp = cp2 + 1;
    }
}


/* expand *table[] by CHUNK elements of max_word characters */

void expand_table(char ***table, int *size)
{
    int ind;

    *table = (char **) my_realloc((char **) *table, (*size + CHUNK) * sizeof(char *));
    for (ind = 0; ind < CHUNK; ind++)
        (*table)[*size + ind] = (char *) calloc(max_word + 1, sizeof(char));
    *size += CHUNK;
}


int power(int base, int exp)
{
    int result;

    result = base;
    while (--exp)
        result *= base;
    return result;
}


/* qsort compare routine */
int compare (const void *str1, const void *str2)
{
    return(strcmp(*(char **) str1, *(char **) str2));
}

/* write out the index entries */
write_index(int db_index, struct RAB *idxptr)
{
    int ind, status, dup_count;

    /* write out the words */
    /* sort keys for counts and $put performance */
    qsort(words, words_index, sizeof(char *), compare);
    dup_count = 1;
    strcpy(prev_keyword, words[0]);
    for (ind = 1; ind < words_index; ind++)
        if (strcmp(words[ind], prev_keyword) == 0)
            dup_count++;
        else {
            sprintf(idx_record, "%-*s%0*d%0*d",
                max_word, prev_keyword,
                max_topic, db_index,
                max_count, dup_count);
            status = sys$put(idxptr);
            if ((status & 1) != SS$_NORMAL)
                lib$stop(status);
            strcpy(prev_keyword, words[ind]);
            dup_count = 1;
        }
    /* write out the last word */
    sprintf(idx_record, "%-*s%0*d%0*d",
            max_word, prev_keyword,
            max_topic, db_index,
            max_count, dup_count);
    status = sys$put(idxptr);
    if ((status & 1) != SS$_NORMAL)
        lib$stop(status);
}

void reset_topic(FILE *src, char *desc, int *start_pos, topic_str *topics)
{
    int ind;

    strncpy(desc, "", sizeof(desc));
    *start_pos = ftell(src);          /* init start position */
    for (ind = 0; ind < words_index; ind++)  /* clear out words[] */
        *words[ind] = '\0';
    words_index = 0;
    for (ind = 0; ind < TOPIC_SIZE; ind++)
        if (topics[ind].found)
            *topics[ind].found = '\0';
}


/* write out Gopher command, write out words */
void write_words(FILE *src, FILE *lnk, struct RAB *selptr, struct RAB *idxptr,
                 int *start_pos, int *end_pos, char *desc, topic_str *topics)
{
    static int db_index = 0;
    int ind, status;
    char filename[256], *ptr, temp_desc[512] = "";

    keyword_index = -1;             /* stop indexing at end of section */
    if (!switch_present("link")) {
        if (words_index == 0) {
            reset_topic(src, desc, start_pos, topics);
            return;      /* no words to write */
        }
        if ((db_index + 2) >= power(10, max_topic)) {
            printf("You have reached %d topics in this index\n", db_index);
            printf("Please re-index with /MAX_TOPIC larger than %d\n", max_topic);
            exit(5);
        }
    }
    fgetname(src, filename);
    if (!switch_present("version")) {  /* if /noversion */
        ptr = strchr(filename, ';');  /* get rid of version number */
        if (ptr) *ptr = '\0';
    }
    for(ptr = filename; *ptr; ptr++)
        *ptr = _tolower(*ptr);  /* force filename lowercase */
    for (ind = 0; ind < TOPIC_SIZE; ind++)
        if (topics[ind].found && (strlen(topics[ind].found) > 0)) {
            if (strlen(temp_desc) > 0)
                strcat(temp_desc, " ");
            strcat(temp_desc, topics[ind].found);
        }
    if (strlen(temp_desc) > 0)
        strncpy(desc, temp_desc, DESC_SIZE);

    if (strlen(desc) > 0) {  /* no description, no index */
        if (switch_present("selector.text") && selector_spec) {
            if (!switch_present("selector.both") && /* selectors only */
                (strlen(selector_spec) == 0)) {     /* and no selector found */
                reset_topic(src, desc, start_pos, topics);  /* reset topic stuff */
                return;                             /* and quit */
            }
            if (switch_present("link")) {
                fprintf(lnk, "Name=%s\nType=%c\n", desc, *selector_spec);
                fprintf(lnk, "Path=%s\n", selector_spec+1);
                fprintf(lnk, "Port=+\nHost=+\n\n");
            }
            else {
                sprintf(sel_record, "%0*d%c%s\t%s",
                    max_topic, ++db_index, *selector_spec, desc, selector_spec+1);
                selptr->rab$w_rsz = strlen(sel_record);
                if (((status = sys$put(selptr)) & 1) != SS$_NORMAL)
                    lib$stop(status);
                write_index(db_index, idxptr);
            }
            strcpy(selector_spec, "");  /* reset the current selector */
        }
        if (!switch_present("selector.text") || switch_present("selector.both")) {
            if (!switch_present("link")) {
                /* write out the selector */
                sprintf(sel_record, "%0*d0%s\tR%d-%d-%s",
                    max_topic, ++db_index, desc, *start_pos, *end_pos, filename);
                selptr->rab$w_rsz = strlen(sel_record);
                if (((status = sys$put(selptr)) & 1) != SS$_NORMAL)
                    lib$stop(status);
                write_index(db_index, idxptr);
            }
            else {
                fprintf(lnk, "Name=%s\nType=0\n", desc);
                fprintf(lnk, "Path=R%d-%d-%s\n", *start_pos, *end_pos, filename);
                fprintf(lnk, "Port=+\nHost=+\n\n");
            }
        }
        printf("%s\n", desc);
    }
    reset_topic(src, desc, start_pos, topics);  /* clear words[], topics */
}


/* read in a file of words */

void load_words(char *name, char *punct, char ***table, int *table_size, int *table_index)
{
    FILE *nf;
    char *cp, *cp2, line[MAX_INPUT_LINE];
    static char file_name[256];
    short leng;
    int status;
    $DESCRIPTOR(name_dsc, file_name);

    if (switch_present(name)) { 
        strcpy(file_name, switch_value(name));
        if ((nf = fopen(file_name, "r")) == NULL) {
            printf("Can't read data file %s\n", file_name);
            return;
        }
    }
    else if (strcmp(name, "noise"))
        return;
    else if ((nf = fopen("_noise_words", "r", "dna = gopher_root:[000000].dat")) == NULL)
        return;

    while (fgets(line, sizeof(line), nf)) {
        cp = strchr(line, '\n');
        if (cp) *cp = '\0';               /* remove newline */
        for (cp = line; *cp; cp++) {
            if (is_punct(*cp, punct) || iscntrl(*cp))
                *cp = ' ';            /* convert punctuation, tabs to spaces */
            *cp = _tolower(*cp);          /* force lowercase */
        }
        while ((strlen(line) > 0) &&
               (line[strlen(line)-1] == ' '))
            line[strlen(line)-1] = '\0';  /* remove trailing blanks */
        for (cp = line; *cp; cp++)
            if (*cp > ' ') break;         /* find first non-blank char */
        strcpy(line, cp);                 /* remove leading blanks */

        strcat(line, " ");                /* line ends with a space */
        cp = line;
        while(cp2 = strchr(cp, ' ')) {    /* break at space boundary */
            *cp2 = '\0';
            if (strlen(cp) > 0) {
                if (*table_index == *table_size)  /* table full */
                    expand_table(table, table_size);
                strcpy((*table)[(*table_index)++], cp);
            }
            cp = cp2 + 1;
        }
    }

    fclose(nf);
}

/* see if a char is punctuation */

int is_punct(char ch, char *punct)
{
    char *ptr;

    for (ptr = punct; *ptr; ptr++)
        if (*ptr == ch)
            return TRUE;
    return FALSE;
}


/* see if field is spaces */

int is_spaces(char *line, int pos, int size)
{
    int index;

    if (strlen(line) < pos)
        return(TRUE);
    for (index = 0; index < size; index++)
        if (!isspace(line[pos + index - 1]))
            return(FALSE);
    return(TRUE);
}


/* see if the word is noise */

int is_noise(char *word, int size, int minimum_word)
{
    int ind;

    if (strlen(word) < minimum_word)  /* simple heuristic saves lots of noise entries */
        return(TRUE);
    if ((!switch_present("numbers")) && isdigit(*word))
        return(TRUE); 
    for(ind = 0; ind < size; ind++) {
        if (noise[ind] == NULL) return (FALSE);
        if (strcmp(noise[ind], word) == 0)
            return (TRUE);
    }
    return (FALSE);
}


/* see if the word is candidate */

int is_candidate(char *word, int size)
{
    int ind;

    for(ind = 0; ind < size; ind++) {
        if (candidate[ind] == NULL) return (FALSE);
        if (strcmp(candidate[ind], word) == 0)
            return (TRUE);
    }
    return (FALSE);
}


char *lc(char *str)
{
# define N_STRING 4
    static char strings[N_STRING][MAX_INPUT_LINE];
    static int cur_string = -1;
    char *cp;

    if (++cur_string >= N_STRING) cur_string = 0;
    for (cp = strings[cur_string];; cp++) {
        *cp = tolower(*str++);
        if (*cp == '\0') break;
    }
    return strings[cur_string];
}


/* find where the string starts (origin 1) in record */

int find_str(char *record, char *str)
{
    char *cp, *lcr;

    if ((str == NULL) || (strlen(str) == 0))
        return (0);  /* zero means string not found */
    lcr = lc(record);
    cp = strstr(lcr, lc(str));
    if (cp == NULL) return (0);
    return (cp - lcr + 1);
}


/* descr() creates character descriptor and return the address
of the descriptor to the caller. */
# define N_DESCR 10
static struct dsc$descriptor_s str_desc[N_DESCR];
static int cur_descr = -1;

struct dsc$descriptor_s *descr(char *string)
{
    if(++cur_descr >= N_DESCR) cur_descr = 0;
    str_desc[cur_descr].dsc$w_length=(short)strlen(string);      
    str_desc[cur_descr].dsc$b_dtype=DSC$K_DTYPE_T;   
    str_desc[cur_descr].dsc$b_class=DSC$K_CLASS_S;  
    str_desc[cur_descr].dsc$a_pointer=string;     
    return (&str_desc[cur_descr]);
}


int get_decimal(char *ptr)
{
    do
        ptr++;
    while ((*ptr != '=')       /* skip to the keyword/parameter */
        && (*ptr != ':'));     /* separator character */
    while (isspace(*++ptr));   /* skip spaces */
    return ((atoi(ptr) < 256) ? atoi(ptr) : 256);
}


char *get_text(char **dest, char *ptr)
{
    char *start, *cp;

    do 
        ptr++;
    while ((*ptr != '=')         /* skip to the keyword/parameter */
           && (*ptr != ':'));    /* separator character */
    while (isspace(*++ptr));     /* skip spaces */
    if (*ptr == '"') {           /* if quoted string */
        start = ++ptr;           /* skip over quote */
        for (; *ptr; ptr++) {    /* skip to ending quote */
            if ((*ptr == '"') && (*(ptr+1) == '"')) {  /* doubled quotes? */
                ptr++;           /* yes, skip it */
                continue;
            }
            if (*ptr == '"')     /* un-doubled quote? */
                break;           /* yes, stop here */
        }
    }
    else {                       /* else non-quoted string */
        start = ptr;             /* start of string */
        while (*ptr
               && (*ptr != ' ')
               && (*ptr != ',')
               && (*ptr != '/')
               && (*ptr != ')'))
            ptr++;               /* skip to string terminator */
    }
    *dest = (char *) calloc((ptr - start) + 1, sizeof(char));
    strncpy(*dest, start, ptr - start);
    for (cp = *dest; *cp; *cp++)   /* collapse doubled quotes to single */
        if ((*cp == '"') && (*(cp+1) == '"'))
            strcpy(cp, cp+1);
    return (ptr);
}


/* parse command line for /topic */
void parse_topic(char *line, topic_str *topics)
{
    char *ptr, *start, **dest;
    static int index = -1;

    for (ptr = line; *ptr; ptr++)
        *ptr = _tolower(*ptr);         /* force command line lowercase */
    ptr = line;                        /* point to start of line */
    for (;;) {                         /* search for /topic until end of line */
        if (index == TOPIC_SIZE)
            return;                    /* exit if we can't hold any more */
        ptr = strchr(ptr, '/');        /* search for switch start */
        if (ptr == NULL)
            return;                    /* no more switches */
        while (isspace(*++ptr));       /* skip spaces */
        if (*ptr != 't')               /* topic is unique to one character */
            continue;                  /* not /topic, keep scanning */
        do
            ptr++;
        while ((*ptr != '=')           /* skip to the keyword/parameter */
               && (*ptr != ':'));      /* separator character */
        while (isspace(*++ptr));       /* skip spaces */
        if (*ptr == '(')               /* if start of list */
            while (isspace(*++ptr));   /* skip spaces */
        index++;                       /* next topics structure */
        topics[index].used = TRUE;     /* this topic index is used */
        topics[index].pos = DEFAULT_POS; /* default position */
        topics[index].end = "</";      /* default end to HTML end tag */
        for (;;) {                     /* parse all /topic list elements */
            if (*ptr == 'e')           /* "end" or "exclude" */
                ptr++;                 /* so we match on 'n' or 'x' */
            switch (*ptr) {
                case 't':                        /* text */
                    ptr = get_text(&topics[index].text, ptr);
                    break;
                case 'n':                        /* end */
                    get_text(&topics[index].end, ptr);
                    break;
                case 'p':                        /* position */
                    topics[index].pos = get_decimal(ptr);
                    break;
                case 's':                        /* size */
                    topics[index].size = get_decimal(ptr);
                    break;
                case 'o':                        /* offset */
                    topics[index].offset = get_decimal(ptr);
                    break;
                case 'x':                         /* exclude */
                    topics[index].exclude = TRUE; /* has no parameters */
                    break;
                case 'b':                              /* break */
                    topics[index].force_break = TRUE;  /* has no parameters */
                    break;
            }
            while (*ptr
                   && (*ptr != ' ')              /* skip to end of */
                   && (*ptr != ',')              /* keyword */
                   && (*ptr != '/')              /* switch */
                   && (*ptr != ')'))             /* or parameter */
                ptr++;
            while (*ptr &&
                   ((*ptr <= ' ') ||             /* skip spaces, junk */
                    (*ptr == ',')))              /* list seperators */
                ptr++;
            if (*ptr == '\0')
                return;                          /* end of the line */
            if ((*ptr == ')') || (*ptr == '/'))
                break;                           /* end of the list */
        }                                        /* scan for more list elements */
    }
}


/* parse command line for /keyword */
void parse_keyword(char *line, topic_str *keywords)
{
    char *ptr, *start, **dest;
    static int index = -1;

    for (ptr = line; *ptr; ptr++)
        *ptr = _tolower(*ptr);         /* force command line lowercase */
    ptr = line;                        /* point to start of line */
    for (;;) {                         /* search for /keyword until end of line */
        if (index == TOPIC_SIZE)
            return;                    /* exit if we can't hold any more */
        ptr = strchr(ptr, '/');        /* search for switch start */
        if (ptr == NULL)
            return;                    /* no more switches */
        while (isspace(*++ptr));       /* skip spaces */
        if (*ptr != 'k')               /* keyword is unique to one character */
            continue;                  /* not /keyword, keep scanning */
        do
            ptr++;
        while ((*ptr != '=')           /* skip to the keyword/parameter */
               && (*ptr != ':'));      /* separator character */
        while (isspace(*++ptr));       /* skip spaces */
        if (*ptr == '(')               /* if start of list */
            while (isspace(*++ptr));   /* skip spaces */
        index++;                       /* next keywords structure */
        keywords[index].used = TRUE;   /* this keyword index is used */
        for (;;) {                     /* parse all /keyword list elements */
            if (*ptr == 'e')           /* "end" or "exclude" */
                ptr++;                 /* so we match on 'n' or 'x' */
            switch (*ptr) {
                case 't':                        /* text */
                    ptr = get_text(&keywords[index].text, ptr);
                    break;
                case 'n':                        /* end */
                    get_text(&keywords[index].end, ptr);
                    break;
                case 'x':                        /* exclude */
                    keywords[index].exclude = TRUE; /* has no parameters */
                    break;
            }
            while (*ptr
                   && (*ptr != ' ')              /* skip to end of */
                   && (*ptr != ',')              /* keyword */
                   && (*ptr != '/')              /* switch */
                   && (*ptr != ')'))             /* or parameter */
                ptr++;
            while (*ptr &&
                   ((*ptr <= ' ') ||             /* skip spaces, junk */
                    (*ptr == ',')))              /* list seperators */
                ptr++;
            if (*ptr == '\0')
                return;                          /* end of the line */
            if ((*ptr == ')') || (*ptr == '/'))
                break;                           /* end of the list */
        }                                        /* scan for more list elements */
    }
}
                                                       

void parse_switch(char *name, switch_str sw[], int def_sw, char *def_value)
{
    int status, ind;
    short leng;
    static char value[100];
    $DESCRIPTOR(switch_dsc, value);

    for (ind = 0; sw[ind].name; ind++)  /* find end of sw[] */
        if (strcmp(sw[ind].name, name) == 0)  /* or an pre-existing switch */
            break;

    if (sw[ind].name == NULL) {         /* register name first time */
        sw[ind].name = calloc(strlen(name) + 1, sizeof(char));
        strcpy(sw[ind].name, name);
        sw[ind].state = def_sw;         /* and set its default state */
    }

    if (def_sw)    /* if default on, remember if it's turned off */
        sw[ind].state &= (cli$present(descr(name)) & 1);
    else           /* if default off, remember if it's turned on */
        sw[ind].state |= (cli$present(descr(name)) & 1);

    status = cli$get_value(descr(name), &switch_dsc, &leng);
    if (status & 1) {
        value[leng] = '\0';
        /* if this is the first value or a non-default value, save it */
        if ((sw[ind].value == NULL) || strcmp(def_value, value)) {
            if (*value == '"') {  /* remove quotes from quoted string */
              strncpy(value, value + 1, leng - 2);
              value[leng - 2] = '\0';
            }
            sw[ind].value = calloc(strlen(value) + 1, sizeof(char));
            strcpy(sw[ind].value, value);
        }
    }
}


void parse_commands(dsc *input, switch_str sw[])
{
    cli$dcl_parse(input, index_commands);

    parse_switch("candidates", sw, 0, 0);
    parse_switch("character", sw, 0, 0);
    parse_switch("dash", sw, 0, 0);
    parse_switch("default_topic", sw, 1, 0);
    parse_switch("equal", sw, 0, 0);
    parse_switch("ff", sw, 0, 0);
    parse_switch("field", sw, 0, 0);
    parse_switch("field.position", sw, 0, 0);
    parse_switch("field.size", sw, 0, 0);
    parse_switch("file", sw, 0, 0);
    parse_switch("keyword", sw, 0, 0);
    parse_switch("line", sw, 0, 0);
    parse_switch("link", sw, 0, 0);
    parse_switch("link.sort", sw, 0, 0);
    parse_switch("max_topics", sw, 1, "6");
    parse_switch("minimum_word", sw, 1, "3");
    parse_switch("noise", sw, 0, 0);
    parse_switch("numbers", sw, 1, 0);
    parse_switch("output", sw, 0, 0);
    parse_switch("paragraph", sw, 0, 0);
    parse_switch("punctuation", sw, 0, 0);
    parse_switch("selector.text", sw, 0, 0);
    parse_switch("selector.end", sw, 0, 0);
    parse_switch("selector.both", sw, 0, 0);
    parse_switch("sequential", sw, 0, 0);
    parse_switch("version", sw, 1, 0);
    parse_switch("whole", sw, 0, 0);
    parse_switch("word_length", sw, 1, "20");

    if (switch_present("keyword")) {
        keyword_index = -1;
        parse_keyword(input->dsc$a_pointer, keywords); /* fill keywords[] */
    }
 
    parse_topic(input->dsc$a_pointer, topics); /* parse the command line and fill topics */
}


int switch_present(char *name)
{
    int ind;

    for (ind = 0; switches[ind].name; ind++)
        if (strcmp(name, switches[ind].name) == 0)
            break;
    if (switches[ind].name == NULL) return (FALSE);
    return (switches[ind].state);
}
     

char *switch_value(char *name)
{
    int ind;

    for (ind = 0; switches[ind].name; ind++)
        if (strcmp(name, switches[ind].name) == 0)
            break;
    return (switches[ind].value);
}


void *my_realloc(void *mem, int size)
{
    if (mem == (void *) 0)
        return ((void *) malloc(size));
    else
        return((void *) realloc(mem, size));
}

