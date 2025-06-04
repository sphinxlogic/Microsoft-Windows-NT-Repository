/* utility to add a nickname field to a sequential file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "qi.h"

#define CHUNK 10

typedef struct {
    char real[20];
    char nick[120];
} nn_str;

nn_str *names;

void make_nickname(FILE *, char *, char *);
int nickname(char *, char *);
void *my_realloc(void *, int);


main(int argc, char *argv[])
{
    FILE *in, *out, *nick;
    char record[512], *cp;
    char filename[256], nickfile[256], name[DATA_SIZE + 1], id[ID_SIZE + 1];
    int no_nick = False, count = 0, ind, length, max_length, nind = 0, table_size = 0;

    if (argc < 2) {
        printf("Usage: add_nickname input [nicknames] [output]\n");
        exit(1);
    }

    if ((in = fopen(argv[1], "r")) == NULL) {
        perror("Can't open input file");
        exit(3);
    }
    if (argc > 2)
        strcpy(nickfile, argv[2]);
    else
        strcpy(nickfile, "nicknames.txt");

    if ((nick = fopen(nickfile, "r")) == NULL) {
        perror("Can't open nicknames file");
        exit(3);
    }

    if (argc > 3)
        strcpy(filename, argv[3]);
    else {
        strcpy(filename, argv[1]);
        if (cp = strchr(filename, ';'))
            *cp = '\0';
    }

    if ((out = fopen(filename, "w", "rfm=var", "rat=cr")) == NULL) {
        perror("Can't open output file");
        exit(5);
    }


    printf("Add nicknames from %s to %s\n", argv[1], filename);

    /* load up the nicknames */
    while (fgets(record, sizeof(record), nick)) {
        if (record[0] == '\n') continue;    /* skip blank lines */
        if (record[0] == '#') continue;    /* skip comments */
        cp = strchr(record, '\n');
        *cp = '\0';
        cp = strchr(record, ' ');
        *cp = '\0';
        do cp++; while (*cp == ' ');
        if (nind == table_size) {
            names = (nn_str *) my_realloc((nn_str *) names,
                                         (nind + CHUNK) * sizeof(nn_str));
            table_size += CHUNK;
        }
        strcpy(names[nind].real, record);
        strcpy(names[nind++].nick, cp);
    }

    strncpy(name, "", sizeof(name));
    strncpy(id, "", sizeof(id));

    while (fgets(record, sizeof(record), in)) {
        if (strcmp(record, "\n") == 0)
            continue;                        /* skip blank lines */

        if ((++count % 1000) == 0) printf("%d\r", count);

        if (strncmp(id, record, ID_SIZE)) {  /* if new id, */
            if (no_nick)         /* and the old id had no nickname field */
                make_nickname(out, id, name);
            strncpy(id, record, ID_SIZE);    /* record new id */
            strncpy(name, "", sizeof(name));
            no_nick = True;                 /* no nickname field yet */
        }

        if (strncmp(NAME_FIELD, record + ID_SIZE, FIELD_SIZE) == 0)
            strcpy(name, record + ID_SIZE + FIELD_SIZE + SEQ_SIZE + ATTR_SIZE);
        no_nick &= (strncmp(NICKNAME_FIELD, record + ID_SIZE, FIELD_SIZE) != 0);

        fputs(record, out);                  /* copy record */
    }
    if (no_nick)
        make_nickname(out, id, name);

    fclose(in);
    fclose(out);

}


char *new_string(char *str)
{
    char *ptr;

    ptr = (char *) malloc(strlen(str) + 1);
    strcpy(ptr, str);
    return (ptr);
}


void make_nickname(FILE *fp, char *id, char *name)
{
    char newrec[DAT_RECORD_SIZE + 1], dest[DAT_RECORD_SIZE + 1];
    char *cp, *cp2, *cp3;
    int nickname_found = False;

    if (strlen(name) == 0)               /* anything there? */
        return;
    for (cp = name; *cp; cp++) {
        if (iscntrl(*cp))  *cp = ' ';    /* convert tabs to spaces */
        *cp = _tolower(*cp);             /* force lowercase */
    }
    while ((strlen(name) > 0) &&
           (name[strlen(name)-1] == ' '))
        name[strlen(name)-1] = '\0';     /* remove trailing blanks */

    strcat(name, " ");              /* name ends with a space */
    sprintf(newrec, "%s%s%0*d%0*d", id, NICKNAME_FIELD,
            SEQ_SIZE, 0, ATTR_SIZE, 0);
    cp = strchr(name, ',');         /* commas are very helpful */
    if (cp == NULL)
        cp = strchr(name, ' ');     /* settle for space */
    cp++;                           /* skip comma or space */
    while(cp2 = strchr(cp, ' ')) {  /* break at space boundary */
        *cp2 = '\0';
        while (*cp == ' ') cp++;    /* skip leading spaces */
        if ((strlen(cp) >= MIN_KEYWORD) &&
            (nickname(dest, cp)))  {   /* if nickname found */
                strcat(newrec, dest);
                strcat(newrec, " ");
                nickname_found = True;
        }
        cp = cp2 + 1;
    }
    if (nickname_found)
        fprintf(fp, "%s\n", newrec);
}


int nickname(char *dest, char *name)
{
    int ind;

    for (ind = 0; strlen(names[ind].real); ind++)
        if (strcmp(names[ind].real, name) == 0) {
            strcpy(dest, names[ind].nick);
            return True;
        }
    return False;
}


void *my_realloc(void *mem, int size)
{
    if (mem == (void *) 0)
        return ((void *) malloc(size));
    else
        return((void *) realloc(mem, size));
}
