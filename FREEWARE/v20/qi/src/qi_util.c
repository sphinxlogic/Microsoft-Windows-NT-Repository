#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ssdef.h>
#include <descrip.h>
#include <time.h>
#include <stdarg.h>
#include <dvidef.h>
#include <dcdef.h>
#include <jpidef.h>
#include <fscndef.h>
#include <starlet.h>
#include <lib$routines.h>
#include <setjmp.h>
#define define_attributes
#define define_modes
#include "qi.h"


char *new_string(context *, char *);
char *lc(context *, char *str);
void chop(char *str);
char *getlogical(context *, char *);
void qilog(context *, int, char *, ...);
int field_number(char *, context *);
Arg *parse_cmd(char *, context *);
Arg *make_arg(context *, char *, int, char *, int, int);
void free_args(Arg *);
void swrite(context *, char *, ...);
void respond(context *, int, char *, ...);
void writestring(int, char *);
char *challenge(context *, int size);
int field_attrib(char *);
int is_local(context *);
void init_dsc(struct dsc$descriptor_s *, char *, int);
void write_result(context *, char *);
void ZapCRLF(char *);
void *check(context *, void *);

extern char *get_value(context *, char*, char*, char *, int);
extern char *get_field(context *, int, char *, int, int);
extern void inet_netnames(int, char *, char *);
extern void crypt_start(char *);
extern char *decrypt(char *, char *);
extern int strcasecmp(char *, char *);
extern int strncasecmp(char *, char *, int);


int quit(char *cmd, context *ctx)
{
    respond(ctx, RESP_ONCE, "200:Bye!");
    return False;
}


/* read a configuration field, optionally forcing lowercase */
char *get_cnf_field(char *ptr, char *field, int lower)
{
    int ind;

    for (ind = 0; (*ptr != '\0') && (*ptr != ':'); ptr++, ind++)
        field[ind] = lower ? _tolower(*ptr) : *ptr;
    field[ind] = '\0';
    if (*ptr == ':') ptr++;  /* skip over terminating ":" */
    return ptr;
}


/* read the configuration file */
int read_fields(char *file, context *ctx)
{
    FILE *cnf;
    char *ptr, line[256], field[128];
    int ind, field_idx;

    for (ind = 0; ind < MAX_FIELD; ind++) {
        ctx->fields[ind].number = NULL;
        ctx->fields[ind].name = NULL;
        ctx->fields[ind].desc = NULL;
        ctx->fields[ind].attrib = 0;
    }

    cnf = fopen(file, "r", "shr=get");
    if (cnf == NULL)
        return False;

    while (fgets(line, sizeof(line), cnf)) {
        ZapCRLF(line);
        ptr = line;
        if ((*ptr == '#') || (*ptr == '\0'))    /* comment or blank? */
            continue;                           /* yes, skip line */
        ptr = get_cnf_field(ptr, field, False);     /* field number */
        field_idx = atoi(field);
        ctx->fields[field_idx].number = new_string(ctx, field);

        ptr = get_cnf_field(ptr, field, FIELD_NAME_LC);      /* field name */
        ctx->fields[field_idx].name = new_string(ctx, field);

        ptr = get_cnf_field(ptr, field, False);     /* field size (ignore) */

        ptr = get_cnf_field(ptr, field, FIELD_NAME_LC);     /* field description */
        ctx->fields[field_idx].desc = new_string(ctx, field);

        ptr = get_cnf_field(ptr, field, False);     /* field option (ignore) */

        for (;;) {
            ptr = get_cnf_field(ptr, field, True);  /* get attribute */
            if (strlen(field) == 0)
                break;                          /* no more attributes */
            ctx->fields[field_idx].attrib |= field_attrib(field);
        }
        if (atoi(ctx->fields[field_idx].number) < 1)
            qilog(NULLCTX, False, "Field \"%s\" has illegal field number",
                  ctx->fields[field_idx].name);
    }

    fclose(cnf);
    return True;
}


void free_fields(context *ctx)
{
    int ind;

    for (ind = 0; ind < MAX_FIELD; ind++) {
        if (ctx->fields[ind].name) free(ctx->fields[ind].name);
        if (ctx->fields[ind].desc) free(ctx->fields[ind].desc);
        if (ctx->fields[ind].number) free(ctx->fields[ind].number);
    }
}


int write_afield(int field_num, context *ctx)
{
    char line[128];
    int aidx;

    if (ctx->fields[field_num].name == NULL)
        return False;

#if FIELD_PARANOIA
    /* if the field is Localpub and the host is not local, skip field */
    if ((ctx->fields[field_num].attrib & ATTR_LOCALPUB) && !is_local(ctx))
        return False;

    /* if the field isn't public, don't make it public knowledge */
    if ((ctx->fields[field_num].attrib &
        (ATTR_PUBLIC | ATTR_DEFAULT | ATTR_LOCALPUB)) == 0)
        return False;
#endif

    sprintf(line, "200:%d:%s:max %d",
            field_num, ctx->fields[field_num].name, DATA_SIZE);
    for (aidx = 0; aidx < MAX_ATTRIBUTES; aidx++)
        if (ctx->fields[field_num].attrib & attributes[aidx].value) {
            strcat(line, " ");
            strcat(line, attributes[aidx].name);
        }
    respond(ctx, RESP_MULT, line);
    respond(ctx, RESP_MULT, "200:%d:%s:%s", field_num,
            ctx->fields[field_num].name, ctx->fields[field_num].desc);
    return True;
}


int fields_cmd(char *cmd, context *ctx)
{
    int fidx, aidx, count = 0;
    char line[256];
    Arg *list, *listp;

    list = listp = parse_cmd(cmd, ctx);

    if (list == NULL)   /* null arg list means all fields */
        for (fidx = 0; fidx < MAX_FIELD; fidx++)
            count += write_afield(fidx, ctx);
    else
        for (; listp; listp = listp->next) {
            fidx = field_number(listp->value, ctx);
            if (fidx > -1)
                count += write_afield(fidx, ctx);
            else
                respond(ctx, RESP_ONCE, "507:Field %s does not exist.",
                        listp->value);
        }

    free_args(list);

/*    respond(ctx, RESP_MULT, "200:%d fields found", count); */
    respond(ctx, RESP_MULT, "200:Ok.");

    if (DEBUG) qilog(ctx, False, "Sent %d field definitions", count);
    return True;
}


id_cmd(char *cmd, context *ctx)
{
    respond(ctx, RESP_ONCE, "200:Thanks, but we don't use ids here.");
    return True;
}


stat_cmd(char *cmd, context *ctx)
{
    if (DEBUG)
        swrite(ctx, "db_status = %d (%X)", ctx->db_status, ctx->db_status);
    if ((ctx->db_status & 1) == SS$_NORMAL)
        respond(ctx, RESP_ONCE, "200:Database ready.");
    else
        respond(ctx, RESP_ONCE, "475:Database unavailable; try later.");
    return True;
}


/* set global mode flags on/off */
int set_cmd(char *cmd, context *ctx)
{
    Arg *list, *listp;
    int index, ind, count;

    list = listp = parse_cmd(cmd, ctx);
    for (; listp; listp = listp->next) {
        for (index = 0, count = 0; index < MAX_MODES; index++)
            if (listp->name && (strncasecmp(listp->name, modes[index].name,
                                           strlen(listp->name)) == 0)) {
                count++;
                ind = index;
            }
        if (count == 1) {
            switch (listp->type & TYPE_MASK) {
            case TYPE_ON:
                if (modes[ind].func == or)
                    ctx->mode |= modes[ind].value;
                else
                    ctx->mode &= modes[ind].value;
                respond(ctx, RESP_ONCE, "200:Done.");
                break;
            case TYPE_OFF:
                ctx->mode &= ~modes[ind].value;
                respond(ctx, RESP_ONCE, "200:Done.");
                break;
            default:
                respond(ctx, RESP_ONCE, "513:Option must be ON or OFF.");
                break;
            }
        }
        else
            respond(ctx, RESP_ONCE, "513:Unknown mode %s",
                   (listp && listp->name) ? listp->name : "");
    }
    if (DEBUG)
        swrite(ctx, "Mode now set to %d", ctx->mode);
    free_args(list);
    return True;
}


/* list help topics */
void list_help(context *ctx, char *name, int topic)
{
    int status, heading = 0, ctext = 0;
    char *cp, line[100], line2[100], file_spec[100], help_name[100];
    struct fscndef scan_list[] = {{(short) 0, (short) FSCN$_NAME, (long) 0},
                                  {(short) 0, (short) 0, (long) 0}};
    struct dsc$descriptor_s help_dsc, file_dsc;

    init_dsc(&help_dsc, help_name, sizeof(help_name)-1);
    init_dsc(&file_dsc, file_spec, sizeof(file_spec)-1);
    strncpy(line, "", sizeof(line));
    sprintf(help_name, "%s[%s]*.HELP", cp=getlogical(ctx, HELP_LIB), name);
    free(cp);
    if (DEBUG)
        swrite(ctx, "Scan for %s", help_name);
    help_dsc.dsc$w_length = (short) strlen(help_name);
    for (;;) {
        strncpy(file_spec, "", sizeof(file_spec));
        status = lib$find_file(&help_dsc, &file_dsc, &ctext, 0, 0, 0, 0);
        if ((status & 1) == 0) break;  /* no more files */
        if (heading++ == 0)
            respond(ctx, RESP_MULT, "200:%d: These \"%s\" help topics are%s available:",
                    topic, name, topic ? " also" : "");
        status = sys$filescan(&file_dsc, scan_list, 0);

        sprintf(line2, " %-20.*s", scan_list[0].fscn$w_length,
                (char *) scan_list[0].fscn$l_addr);
        strcat(line, line2);
        if (strlen(line) > 50) {
            respond(ctx, RESP_MULT, "200:%d:%s", topic, line);
            strncpy(line, "", sizeof(line));
        }
    }
    if (strlen(line))
        respond(ctx, RESP_MULT, "200:%d:%s", topic, line);
    lib$find_file_end(&ctext);
    if (heading == 0)
        respond(ctx, RESP_ONCE, "500:No help available.");
}


/* give some help */
int help_cmd(char *cmd, context *ctx)
{
    FILE *fd;
    Arg *list;
    int ind, status, topic = 0, heading = 0;
    char *ln, line[100], file_name[100];

    list = parse_cmd(cmd, ctx);

    if (list == NULL)
        list_help(ctx, "native", topic++);      /* list native topics */
    else if (list->next == NULL) {
        list_help(ctx, list->value, topic++);   /* list help topics */
        if (strcasecmp(list->value, "native"))  /* if this wasn't native, */
            list_help(ctx, "native", topic++);  /* list native topics also */
    }
    else {
        sprintf(file_name, "%s[%s]%s.HELP", ln=getlogical(ctx, HELP_LIB),
                list->value, list->next->value);
        free(ln);
        if (DEBUG)
            swrite(ctx, "Reading %s", file_name);
        if ((fd = fopen(file_name, "r", "shr=get")) == NULL) {
            respond(ctx, RESP_ONCE, "500:No help available for %s %s.",
                    list->value, list->next->value);
            free_args(list);
            return True;
        }
        else {
            while (fgets(line, sizeof(line), fd)) {
                ZapCRLF(line);
                respond(ctx, RESP_MULT, "200:%d:%s", topic, line);
            }
            fclose(fd);
            topic++;
        }
    }
    respond(ctx, RESP_MULT, "200:Ok.");
    free_args(list);
    return True;
}


/* return some arbitrary site info */
int site_cmd(char *cmd, context *ctx)
{
    FILE *fd;
    char *ln, line[128];
    int count = 0;

    respond(ctx, RESP_MULT, "200:%d:version:%s", count++, VERSION);
    if ((fd = fopen(ln=getlogical(ctx, SITEINFO_NAME), "r", "shr=get")) != NULL) {
        while (fgets(line, sizeof(line), fd)) {
            ZapCRLF(line);
            respond(ctx, RESP_MULT, "200:%d:%s", count++, line);
        }
        fclose(fd);
    }
    free(ln);
    if ((ln=getlogical(ctx, MAILDOMAIN_NAME)) && strlen(ln))
        respond(ctx, RESP_MULT, "200:%d:maildomain:%s", count++, ln);
    free(ln);
#ifdef MAIL_FIELD
    if (ctx->fields[atoi(MAIL_FIELD)].name)
        respond(ctx, RESP_MULT, "200:%d:mailfield:%s", count++,
            ctx->fields[atoi(MAIL_FIELD)].name);
#endif
#ifdef MAIL_BOX
    if (ctx->fields[atoi(MAIL_BOX)].name)
        respond(ctx, RESP_MULT, "200:%d:mailbox:%s", count++,
            ctx->fields[atoi(MAIL_BOX)].name);
#endif
    respond(ctx, RESP_MULT, "200:Ok.");
    return True;
}


/* set current alias */
int login_cmd(char *cmd, context *ctx)
{
    Arg *list;
    char *ap, protocol_challenge[CHALLENGE_SIZE + 5];

    if (ctx->login_mode == MODE_LOGIN) {      /* logout first */
        strncpy(ctx->login_alias, "", sizeof(ctx->login_alias));
        ctx->login_id = -1;
        ctx->login_mode = MODE_ANONYMOUS;
    }

    list = parse_cmd(cmd, ctx);
    if ((list == NULL) || (list->next) || (list->type != TYPE_VALUE)) {
        free_args(list);
        respond(ctx, RESP_ONCE, "599:Syntax error");
        return True;
    }
    if ((ap = get_value(ctx, list->value, ALIAS_FIELD, ALIAS_FIELD, 0)) == NULL) {
        free_args(list);
        respond(ctx, RESP_ONCE, "500:Alias does not exist");
        return True;
    }
    strcpy(ctx->login_alias, ap);
    ctx->login_id = atoi(get_value(ctx, list->value, ALIAS_FIELD, ID_FIELD, 0));
    free_args(list);
    if (get_field(ctx, ctx->login_id, PASSWORD_FIELD, 0, True) == NULL) {
        respond(ctx, RESP_ONCE, "500:Password does not exist");
        return True;
    }
    strcpy(ctx->login_challenge, ap = challenge(ctx, CHALLENGE_SIZE));
    free(ap);
    strcpy(protocol_challenge, "301:");
    strcat(protocol_challenge, ctx->login_challenge);
    respond(ctx, RESP_LITERAL, protocol_challenge);
    ctx->login_mode = MODE_PASSWORD;
    return True;
}


int clear_cmd(char *cmd, context *ctx)
{
    char *cp, *pw;

    cp = strchr(cmd, ' ');             /* skip verb */
    if (cp == NULL) {
        respond(ctx, RESP_ONCE, "599:Syntax error");
        strncpy(ctx->login_alias, "", sizeof(ctx->login_alias));
        ctx->login_id = -1;
        ctx->login_mode = MODE_ANONYMOUS;
        return True;
    }
    for (;*cp && (*cp == ' '); cp++);  /* skip spaces after verb */
    if ((pw = get_field(ctx, ctx->login_id, PASSWORD_FIELD, 0, True)) == NULL) {
        respond(ctx, RESP_ONCE, "500:Password does not exist");
        strncpy(ctx->login_alias, "", sizeof(ctx->login_alias));
        ctx->login_id = -1;
        ctx->login_mode = MODE_ANONYMOUS;
        return True;
    }
    if (strcmp(cp, pw) == 0) {
        respond(ctx, RESP_ONCE, "200:%s:Password accepted", ctx->login_alias);
        ctx->login_mode = MODE_LOGIN;
        crypt_start(pw);
    }
    else {
        respond(ctx, RESP_ONCE, "500:Login failed");
        ctx->login_mode = MODE_ANONYMOUS;
        strncpy(ctx->login_alias, "", sizeof(ctx->login_alias));
        ctx->login_id = -1;
    }
    return True;
}


int answer_cmd(char *cmd, context *ctx)
{
    char *cp, *cp2, decrypted[128];

    cp = strchr(cmd, ' ');             /* skip verb */
    if (cp == NULL) {
        respond(ctx, RESP_ONCE, "599:Syntax error");
        strncpy(ctx->login_alias, "", sizeof(ctx->login_alias));
        ctx->login_id = -1;
        ctx->login_mode = MODE_ANONYMOUS;
        return True;
    }
    for (;*cp && (*cp == ' '); cp++);  /* skip spaces after verb */
    crypt_start(get_field(ctx, ctx->login_id, PASSWORD_FIELD, 0, True));
    decrypt(decrypted, cp);
    if ((cp2 = strchr(decrypted, '\r')) || (cp2 = strchr(decrypted, '\n')))
        *cp2 = '\0';  /* truncate at cr or lf */
    if (DEBUG)
        swrite(ctx, ">>%s\n decrypted into\n>>%s\n compared with\n>>%s",
               cp, decrypted, ctx->login_challenge);
    if (strcmp(decrypted, ctx->login_challenge) == 0) {
        respond(ctx, RESP_ONCE, "200:%s:Password accepted", ctx->login_alias);
        ctx->login_mode = MODE_LOGIN;
    }
    else {
        respond(ctx, RESP_ONCE, "500:Login failed");
        strncpy(ctx->login_alias, "", sizeof(ctx->login_alias));
        ctx->login_id = -1;
        ctx->login_mode = MODE_ANONYMOUS;
    }
    return True;
}


int logout_cmd(char *cmd, context *ctx)
{
    strncpy(ctx->login_alias, "", sizeof(ctx->login_alias));
    ctx->login_id = -1;
    ctx->login_mode = MODE_ANONYMOUS;
    respond(ctx, RESP_ONCE, "200:Done.");
    return True;
}


char *challenge(context *ctx, int size)
{
    char *ptr, *base;

    base = (char *) check(ctx, calloc(size + 1, sizeof(char)));
    for (ptr = base; size; ptr++, size--)
        *ptr = (rand() & 0x3f) + 0x21;
    return base;
}


/* force string to lowercase */
char *lc(context *ctx, char *str)
{
    char *cp, *dest;

    dest = (char *) check(ctx, calloc(DATA_SIZE, sizeof(char)));
    for (cp = dest;; cp++) {
        *cp = tolower(*str++);
        if (*cp == '\0') break;
    }
    return dest;
}


/* remove trailing spaces from string */
void chop(char *str)
{
    while (str[strlen(str)-1] == ' ')
        str[strlen(str)-1] = '\0';
}


/* return the attribute value for the given field name */
int field_attrib(char *str)
{
    int ind;

    for (ind = 0; ind < MAX_ATTRIBUTES; ind++) {
        if (strcasecmp(str, attributes[ind].name) == 0)
            return attributes[ind].value;
    }
    return 0;  /* no match = no bits */
}


/* return the field_number for the given field name */
int field_number(char *str, context *ctx)
{
    int ind;

    for (ind = 0; ind < MAX_FIELD; ind++)
        if (ctx->fields[ind].name &&
            (strcasecmp(str, ctx->fields[ind].name) == 0))
            return atoi(ctx->fields[ind].number);
    return -1;  /* no field number */
}


/* get a token as part of the 'field=value' clause */
/* return pointer to terminator */
char *get_token(char *cp, char *dp)
{
    int in_quote = False;

    if (*cp) {
        while (isspace(*cp)) cp++; /* skip space */
        while (*cp && (in_quote || ((*cp != ' ') && (*cp != '=')))) {
            if (*cp == '"') {
                in_quote = !in_quote;
                cp++;
            }
            else if (in_quote || (*cp != ','))
                *dp++ = *cp++;
            else
                cp++;   /* skip comma unless quoted */
        }
        while (isspace(*cp)) cp++; /* skip space */
    }
    *dp = '\0';
    return cp;
}


/* cmd = 'verb [field=]value ...' */
Arg *parse_cmd(char *cmd, context *ctx)
{
    int index = 0, in_return = 0, prev_field = -1;
    char *cp, token[128];
    Arg *start = NULL, *end = NULL;

    cp = strchr(cmd, ' ');  /* skip verb */
    while (cp) {
        index++;
        cp = get_token(cp, token);
        if (strlen(token) == 0)
            return start;
        if (start == NULL)
            start = end = make_arg(ctx, NULL, -1, NULL, 0, 0);
        else {
            end->next = make_arg(ctx, NULL, -1, NULL, 0, 0);
            end->next->prev = end;
            end = end->next;
        }
        if (*cp == '=') {
            end->name = new_string(ctx, token);
            prev_field = end->field = field_number(token, ctx);
            end->type |= TYPE_NAME | TYPE_EQUAL;
            cp = get_token(++cp, token);
        }
        if (strlen(token)) {
            end->value = new_string(ctx, token);
            end->type |= TYPE_VALUE;
            end->element = index;
        }
        if (strcasecmp(token, "return") == 0) {  /* check for special names */
            end->type = TYPE_RETURN;
            in_return = True;
        } else if (strcasecmp(token, "make") == 0)
            end->type = TYPE_MAKE;
        else if (strcasecmp(token, "set") == 0)
            end->type = TYPE_SET;
        else if (strcasecmp(token, "on") == 0)
            end->type |= TYPE_ON;
        else if ((strcasecmp(token, "off") == 0)  ||
                 (strcasecmp(token, "of") == 0))
            end->type |= TYPE_OFF;
        if (end->field == -1)       /* if there were no field name given */
            if (in_return)
                /* try the field value as a field name */
                end->field = field_number(token, ctx);
            else
                /* inhereit the field number of the previous field */
                end->field = prev_field;
        if (DEBUG)
            swrite(ctx, "Parse %s (field %d) type %d value %s",
                    end->name ? end->name : "", end->field, end->type,
                    end->value ? end->value : "");
    }    
    return start;  /* should only get here on null list */
}


Arg *make_arg(context *ctx, char *name, int field, char *value, int type, int element)
{
    Arg *ptr;

    ptr = (Arg *) check(ctx, malloc(sizeof (Arg)));
    ptr->element = element;
    ptr->type = type;
    ptr->name = name;
    ptr->field = field;
    ptr->value = value;
    ptr->prev = (Arg *) 0;
    ptr->next = (Arg *) 0;
    return ptr;
}
    


void free_args(Arg *ptr)
{
    Arg *next;

    while (ptr) {
        next = ptr->next;
        if (ptr->name) free(ptr->name);
        if (ptr->value) free(ptr->value);
        free(ptr);
        ptr = next;
    }
}


/* copy string into malloc'ed space */

char *new_string(context *ctx, char *str)
{
    char *ptr;

    ptr = (char *) check(ctx, malloc(strlen(str) + 1));
    strcpy(ptr, str);
    return ptr;
}


void swrite(context *ctx, char *fmt, ...)
{
    char    buf[512];
    va_list arg_ptr;

    va_start(arg_ptr, fmt);
    vsprintf(buf, fmt, arg_ptr);
    va_end(arg_ptr);
    if (ctx->mode & RECORD_MODE)    /* dump reponse to the log file */
        qilog(ctx, False, buf);
    if (ctx->interactive)
        printf("%s\r\n", buf);
    else if (ctx->socket == -1)
        write_result(ctx, buf);
    else {
        strcat(buf, EOL);
        writestring(ctx->socket, buf);
    }
}


/* buffer responses until the end of the command */
/* req RESP_ONCE = respond with 1st message */
/* req RESP_MULT = respond with all messages */
/* req RESP_LITERAL = RESP_MULT but no formatting */
/* req RESP_RESET = end of command, flush buffer */
void respond(context *ctx, int req, char *fmt, ...)
{
    resp_node *ptr, *nxt;
    char buf[512], *leader, *cp, *temp;
    int skip = 0;
    va_list arg_ptr;

/*  I'm not sure the RESP_ONCE thing is at all right - or even works */
/*    if ((req == RESP_ONCE) && ctx->root)    /* already have a message? */
/*        return;                             /* yes, we're done */

    if (req == RESP_RESET) {
        for (ptr = ctx->root; ptr; ptr = nxt) {
            /* if this is 200 and next exists and is <= 200, make neg */
            leader = ((atoi(ptr->message+2) == 200) && ptr->next &&
                      (atoi(ptr->next->message+2) <= 200)) ? "-" : "";
            if (!skip) swrite(ctx, ptr->message, leader);
            skip |= ((*leader == '\0') && (atoi(ptr->message+2) >= 200));
            nxt = ptr->next;
            free(ptr->message);
            free(ptr);
        }
        ctx->root = NULL;
        return;
    }
    if (req == RESP_LITERAL) {        /* copy fmt to buf[] duplicating '%' */
        for (cp = buf; *fmt; fmt++) {
          *cp++ = *fmt;
          if (*fmt == '%')
              *cp++ = *fmt;
        }
        *cp = '\0';
    }
    else {                       /* RESP_MULT and RESP_ONCE use vsprintf() */
        va_start(arg_ptr, fmt);
        vsprintf(buf, fmt, arg_ptr);
        va_end(arg_ptr);
    }

    temp = (char *) check(ctx, malloc(strlen(buf) + 4));
    sprintf(temp, "%%s%s", buf);
    if (ctx->root == NULL) {
        ctx->root = (resp_node *) check(ctx, malloc(sizeof(resp_node)));
        ctx->root->message = temp;
        ctx->root->next = NULL;
    }
    else {
        for (ptr = ctx->root; ptr->next; ptr = ptr->next);
        ptr->next = (resp_node *) check(ctx, malloc(sizeof(resp_node)));
        ptr->next->message = temp;
        ptr->next->next = NULL;
    }
}


void qilog(context *ctx, int force, char *fmt, ...)
{
#define RETRIES 40
    FILE    *logfd;
    int     status, pid, count = 0, jpiarg = JPI$_PID;
    char    *cp, *ln, host_name[256], ip_addr[20], NowBuf[26], buf[512];
    time_t  Now;
    float delay = 0.25;
    va_list arg_ptr;

    if ((force == False) && (ctx->mode & LOG_MODE) == 0) /* write to log file? */
        return;                         /* no, quit */

    va_start(arg_ptr, fmt);
    vsprintf(buf, fmt, arg_ptr);
    va_end(arg_ptr);
    host_name[0] = '\0';

    if (ctx->interactive) {
        status = lib$getjpi(&jpiarg, 0, 0, &pid, 0, 0);
        sprintf(host_name, "%X", pid);  /* inlcude pid instead of host */
    }
    else if (ctx != NULLCTX)
        inet_netnames(ctx->socket, host_name, ip_addr);

    while ((count++ < RETRIES) && ((logfd = fopen(ln=getlogical(ctx, LOG_NAME), "a",
                              "dna=qi.log")) == NULL)) {
        cfree(ln);
        if (count >= RETRIES)
            return;
        lib$wait(&delay);
    }
    cfree(ln);
    time(&Now);
    cp = (char *) ctime(&Now);
    ZapCRLF(cp);
    cp = strcpy(NowBuf, cp);
    if (strlen(buf))
        fprintf(logfd, "%s %s %s\n", cp, host_name, buf);
    fclose(logfd);
}


/* return true if the hostname of the socket is in LOCAL_DOMAIN_NAME
 * and the server has not been placed in 'remote' mode
 */
int is_local(context *ctx)
{
    char *cp, host_name[256], temp[256], domain[256], ip_addr[20];

    if (ctx->interactive) return True;            /* interactive is very local */
    inet_netnames(ctx->socket, host_name, ip_addr); /* get host name */
    strcpy(domain, cp=getlogical(ctx, LOCAL_DOMAIN_NAME)); /* our domain name */
    free(cp);
    strcpy(temp, host_name);
    for (;;) {
        if (strcasecmp(temp, domain) == 0) {     /* if they match, */
            if (DEBUG) {
                swrite(ctx, "Node %s is local to %s", host_name, domain);
                swrite(ctx, "Remote-mode = %d", ctx->mode & REMOTE_MODE);
            }
            return !(ctx->mode & REMOTE_MODE); /* return !remote_mode status */
        }
        cp = strchr(temp, '.');              /* they don't; throw out part */
        if (cp == NULL) {                    /* if all parts thrown out, */
            if (DEBUG)
                swrite(ctx, "Node %s is not local to %s", host_name, domain);
            return False;                    /* the host isn't local */
        }
        strcpy(temp, cp + 1);                /* retry with smaller host name */
    }
}


/* realloc memory with fix for VAXC bug */
void *my_realloc(context *ctx, void *mem, int size)
{
    if ((mem == (void *) 0))
        return (void *) check(ctx, malloc(size));
    else
        return (void *) check(ctx, realloc(mem, size));
}


/* check memory allocation and send error if it fails */
void *check(context *ctx, void *value)
{
    if (value == (void *) 0) {
        swrite(ctx, "599: Memory allocation failure");
        longjmp(ctx->env, 1);  /* shutdown the server */
    }
    return value;
}


/* translate an exec mode logical name */
char *getlogical(context *ctx, char *name)
{
#include <psldef.h>
#include <lnmdef.h>
#define RESULT_SIZE 200

    typedef struct {
        short length;
        short item_code;
        char  *bufadr;
        short *ret_len_addr;
    } item_desc;

    struct {
        item_desc string;
        int terminator;
    } trnlst;

    char *result;  /* the object returned */
    short ret_len = 0;
    int acmode = PSL$C_EXEC;
#if EXEC_LOGICALS
    $DESCRIPTOR(table_dsc, "LNM$SYSTEM_TABLE");
#else
    $DESCRIPTOR(table_dsc, "LNM$FILE_DEV");
#endif

    struct dsc$descriptor_s log_dsc;

    result = (char *) check(ctx, calloc(RESULT_SIZE, sizeof(char)));

    init_dsc(&log_dsc, name, strlen(name));
    trnlst.string.bufadr = result;
    trnlst.string.length = RESULT_SIZE;
    trnlst.string.item_code = LNM$_STRING;
    trnlst.string.ret_len_addr = &ret_len;
    trnlst.terminator = 0;

#if EXEC_LOGICALS
    sys$trnlnm(0, &table_dsc, &log_dsc, &acmode, &trnlst);
#else
    sys$trnlnm(0, &table_dsc, &log_dsc, 0, &trnlst);
#endif

    result[ret_len] = '\0';
    return result;
}


void init_dsc(struct dsc$descriptor_s *dest, char *str, int size)
{
    dest->dsc$w_length = (unsigned short) size;
    dest->dsc$b_dtype = DSC$K_DTYPE_T;
    dest->dsc$b_class = DSC$K_CLASS_S;
    dest->dsc$a_pointer = str;
}


context *create_context()
{
    context *ctx;

    ctx = calloc(1, sizeof(context));

    /* there's not much we can do if this fails... */
    if (ctx == (context *) 0) {
        printf("Context memory allocation failed\n");
        abort();
    }

    ctx->login_mode = MODE_ANONYMOUS;
    ctx->mode = DEFAULT_MODE;
    ctx->login_id = -1;
    ctx->next_id = -1;
    ctx->socket = -1;

    return ctx;
}


/* write a string to ctx->results */
void write_result(context *ctx, char *str)
{
    struct dsc$descriptor_d desc, *elem;

    if (ctx->results == (onedim *) 0) {
        ctx->results = (onedim *) check(ctx, malloc(sizeof(onedim)));
        ctx->results->a.dsc$w_length =  8;                /* len of descriptor */
        ctx->results->a.dsc$b_dtype = DSC$K_DTYPE_DSC;
        ctx->results->a.dsc$b_class = DSC$K_CLASS_A;
        ctx->results->a.dsc$a_pointer = NULL;
        ctx->results->a.dsc$b_scale =  0;
        ctx->results->a.dsc$b_digits = 0;
        ctx->results->a.dsc$b_aflags.dsc$v_fl_coeff = 1;  /* multiplier defined */
        ctx->results->a.dsc$b_aflags.dsc$v_fl_bounds = 1; /* bounds defined */
        ctx->results->a.dsc$b_dimct =  1;                 /* one dimension */
        ctx->results->a.dsc$l_arsize = 0;                 /* array size */
        ctx->results->m.dsc$a_a0 = NULL;
        ctx->results->m.dsc$l_m = 0;                      /* number of elements */
        ctx->results->b.dsc$l_l = 0;                      /* lower bound */
        ctx->results->b.dsc$l_u = 0;                      /* upper bound */
    }

    ctx->results->m.dsc$l_m++;    /* array is one element bigger */
    ctx->results->b.dsc$l_u = ctx->results->m.dsc$l_m - 1;
    ctx->results->a.dsc$l_arsize = ctx->results->a.dsc$w_length * ctx->results->m.dsc$l_m;
    ctx->results->m.dsc$a_a0 = (struct dsc$descriptor_d *) my_realloc(
                               ctx, ctx->results->m.dsc$a_a0,
                               sizeof(desc) * ctx->results->m.dsc$l_m);
    ctx->results->a.dsc$a_pointer = (char *) ctx->results->m.dsc$a_a0;

    desc.dsc$w_length = (short) strlen(str);  /* set up a descriptor */
    desc.dsc$b_dtype = DSC$K_DTYPE_T;
    desc.dsc$b_class = DSC$K_CLASS_D;
    desc.dsc$a_pointer = (char *) check(ctx, calloc(strlen(str) + 1, sizeof(char)));
    strcpy(desc.dsc$a_pointer, str);
    elem = ctx->results->m.dsc$a_a0 + ctx->results->b.dsc$l_u;
    memcpy(elem, &desc, sizeof(desc));  /* copy descriptor to array element */
}


/*
 * ZapCRLF removes all carriage returns and linefeeds from a C-string.
 */

void ZapCRLF(char *inputline)
{
     char *cp;

     cp = strchr(inputline, '\r');    /* Zap CR-LF */
     if (cp != NULL)
	  *cp = '\0';
     else {
	  cp = strchr(inputline, '\n');
	  if (cp != NULL)
	       *cp = '\0';
     }
}
