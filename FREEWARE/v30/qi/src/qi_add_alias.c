/* utility to add the alias field to a sequential file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "qi.h"

#define LIST 499  /* hash base, should be prime */

#define LF_ALIAS 0       /* last first => f-last */
#define FL_ALIAS 0       /* first last => f-last */
#define LF_FULL_ALIAS 1  /* last first => first.last */
#define FL_FULL_ALIAS 0  /* first last => first.last */

typedef struct a_node {
    char *name;
    struct a_node *next;
} alias_node;

alias_node alias_table[LIST];

void make_alias(FILE *, char *, char *);

main(int argc, char *argv[])
{
    FILE *in, *out;
    char record[512], *cp;
    char filename[256], name[DATA_SIZE + 1], id[ID_SIZE + 1];
    int no_alias = False, count = 0, ind, length, max_length;
    alias_node *ptr;

    if (argc < 2) {
        printf("Usage: add_alias input [output]\n");
        exit(1);
    }

    if ((in = fopen(argv[1], "r")) == NULL) {
        perror("Can't open input file");
        exit(3);
    }
    if (argc == 2) {
        strcpy(filename, argv[1]);
        if (cp = strchr(filename, ';'))
            *cp = '\0';
    }
    else
        strcpy(filename, argv[2]);

    if ((out = fopen(filename, "w", "rfm=var", "rat=cr")) == NULL) {
        perror("Can't open output file");
        exit(5);
    }


    printf("Add alias from %s to %s\n", argv[1], filename);

    strncpy(name, "", sizeof(name));
    strncpy(id, "", sizeof(id));

    while (fgets(record, sizeof(record), in)) {
        if (strcmp(record, "\n") == 0)
            continue;                        /* skip blank lines */

        if ((++count % 1000) == 0) printf("%d\r", count);

        if (strncmp(id, record, ID_SIZE)) {  /* if new id, */
            if (no_alias)         /* and the old id had no alias field */
                make_alias(out, id, name);
            strncpy(id, record, ID_SIZE);    /* record new id */
            strncpy(name, "", sizeof(name));
            no_alias = True;                 /* no alias field yet */
        }

        if (strncmp(NAME_FIELD, record + ID_SIZE, FIELD_SIZE) == 0)
            strcpy(name, record + ID_SIZE + FIELD_SIZE + SEQ_SIZE + ATTR_SIZE);
        no_alias &= (strncmp(ALIAS_FIELD, record + ID_SIZE, FIELD_SIZE) != 0);

        fputs(record, out);                  /* copy record */
    }
    if (no_alias)
        make_alias(out, id, name);

    fclose(in);
    fclose(out);

    count = 0;
    max_length = 0;
    for (ind = 0; ind < LIST; ind++) {
        if (alias_table[ind].name) count++;
        length = 0;
        for (ptr = alias_table[ind].next; ptr; ptr = ptr->next)
            max_length = ++length > max_length ? length : max_length;
    }
    printf("Alias table = %d, %d used, longest chain = %d\n",
           LIST, count, max_length);

}


int hash(char *word)
{
    char *cp;
    int total = 0;

    for (cp = word; *cp; cp++)
        total += *cp;
    return (total % LIST);
}


char *new_string(char *str)
{
    char *ptr;

    ptr = (char *) malloc(strlen(str) + 1);
    strcpy(ptr, str);
    return (ptr);
}


/* return a unique name */
char *unique(char *name)
{
    static char temp[DATA_SIZE + 1];
    alias_node *ptr, *prev;
    int index, count;

    strcpy(temp, name);
    for (count = 1;;count++) {
        index = hash(temp);
        if (alias_table[index].name == NULL) {     /* no entry for this hash */
            alias_table[index].name = new_string(temp);
            return temp;
        }
        for (ptr = &alias_table[index], prev = (alias_node *) 0;
             ptr; prev = ptr, ptr = ptr->next)
            if (strcmp(ptr->name, temp) == 0)
                break;
        if (ptr == (alias_node *) 0) {
            if (prev) {
                prev->next = (alias_node *) calloc(1, sizeof(alias_node));
                prev->next->name = new_string(temp);
            }
            else {
                alias_table[index].next = (alias_node *) calloc(1, sizeof(alias_node));
                alias_table[index].next->name = new_string(temp);
            }
            return temp;
        }
        printf("Dup: %s\n", temp);
        sprintf(temp, "%s%d", name, count);  /* name was found, make a new one */
    }
}


void make_alias(FILE *fp, char *id, char *name)
{
    char newrec[DAT_RECORD_SIZE + 1], dest[DATA_SIZE + 1];
    char *cp, *cp2, *cp3, alias[DATA_SIZE + 1];

    if (strlen(name) == 0)               /* anything there? */
        return;
    for (cp = name; *cp; cp++)
        if (iscntrl(*cp))  *cp = ' ';    /* convert tabs to spaces */
    while ((strlen(name) > 0) &&
           (name[strlen(name)-1] == ' '))
        name[strlen(name)-1] = '\0';     /* remove trailing blanks */
    for (cp = name; *cp; cp++) {    /* apply any special editing to names */
/*        if (*cp == ',') *cp = ' '; /* remove comma */
        if ((*cp == ' ') && (*(cp+1) == ' ')) strcpy(cp, cp+1); /* mult spaces */
        if ((*cp == '.') && (*(cp+1) == ' ')) strcpy(cp, cp+1); /* St. Foo */
#if NAME_HACK
        if (*cp == '\'') strcpy(cp, cp+1); /* and apostrophe */
#endif
#if LF_ALIAS || FL_ALIAS
        *cp = _tolower(*cp);             /* force lowercase */
#endif
    }
    strncpy(alias, "", sizeof(alias));
#if LF_ALIAS
    if ((cp = strchr(name, ' ')) == NULL)
        cp = strchr(name, '\0');
    alias[0] = *(cp+1);  /* first character of first name */
    alias[1] = '-';      /* dash */
    strncat(alias, name, cp-name);
#endif
#if LF_FULL_ALIAS
    if ((cp = strchr(name, ',')) == NULL)
        if ((cp = strchr(name, ' ')) == NULL)
            cp = strchr(name, '\0');
    *cp = '\0';
    do cp++; while (*cp == ' ');
    if ((cp2 = strchr(cp+1, ' ')) == NULL)
        cp2 = strchr(cp+1, '\0');
    *cp2 = '\0';
    if ((strlen(cp) == 1) || (strncmp(cp, "Jr", 2) == 0)
        || (strncmp(cp, "III", 3) == 0))
        strcpy(alias, cp2+1);       /* first name */
    else
        strcpy(alias, cp);          /* first name */
    strcat(alias, ".");             /* dot */
    strcat(alias, name);            /* last name */
    for (cp = alias; *cp; cp++)
        if (*cp == ' ')
            *cp = '.';              /* changes spaces to dots */
    if (cp = strstr(alias,".Jr"))
        *cp = '\0';
#endif
#if FL_ALIAS
    alias[0] = *name;    /* first character of first name */
    alias[1] = '-';      /* dash */
    if ((cp2 = strchr(cp+1, ' ')) == NULL)
        cp2 = strchr(cp+1, '\0');
    strncat(alias, cp+1, cp2-(cp+1));
#endif
#if FL_FULL_ALIAS
    strcpy(alias, name);            /* copy name */
    for (cp = alias; *cp; cp++)
        if (*cp == ' ') *cp = '.';  /* change spaces to RFC-822 legal separators */
#endif
    fprintf(fp, "%s%s%0*d%0*d%.*s\n", id, ALIAS_FIELD,
            SEQ_SIZE, 0, ATTR_SIZE, 0, KEYWORD_SIZE, unique(alias));
}
