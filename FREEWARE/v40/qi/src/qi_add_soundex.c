/* utility to add a soundex field to a sequential file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "qi.h"

void make_soundex(FILE *, char *, char *);
extern char *soundex(char *, char *, int);

main(int argc, char *argv[])
{
    FILE *in, *out;
    char record[512], *cp;
    char filename[256], name[DATA_SIZE + 1], id[ID_SIZE + 1];
    int no_soundex = False, count = 0;

    if (argc < 2) {
        printf("Usage: add_soundex input [output]\n");
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

    printf("Add soundex from %s to %s\n", argv[1], filename);

    strncpy(name, "", sizeof(name));
    strncpy(id, "", sizeof(id));

    while (fgets(record, sizeof(record), in)) {
        if (strcmp(record, "\n") == 0)
            continue;                        /* skip blank lines */

        if ((++count % 500) == 0) printf("%d\r", count);

        if (strncmp(id, record, ID_SIZE)) {  /* if new id, */
            if (no_soundex)         /* and the old id had no soundex field */
                make_soundex(out, id, name);
            strncpy(id, record, ID_SIZE);    /* record new id */
            strncpy(name, "", sizeof(name));
            no_soundex = True;               /* no soundex field yet */
        }

        if (strncmp(NAME_FIELD, record + ID_SIZE, FIELD_SIZE) == 0)
            strcpy(name, record + ID_SIZE + FIELD_SIZE + SEQ_SIZE + ATTR_SIZE);
        no_soundex &= (strncmp(SOUNDEX_FIELD, record + ID_SIZE, FIELD_SIZE) != 0);

        fputs(record, out);                  /* copy record */
    }
    if (no_soundex)
        make_soundex(out, id, name);

    fclose(in);
    fclose(out);
}


void make_soundex(FILE *fp, char *id, char *line)
{
    char newrec[DAT_RECORD_SIZE + 1], dest[SOUNDEX_SIZE + 1];
    char *cp, *cp2, *cp3;

    if (strlen(line) == 0)               /* anything there? */
        return;
    for (cp = line; *cp; cp++) {
        if (iscntrl(*cp))  *cp = ' ';    /* convert tabs to spaces */
        if (*cp == ',') strcpy(cp, cp+1); /* remove commas */
#if NAME_HACK
        if (*cp == '\'') strcpy(cp, cp+1); /* squeeze out apostrophe */
#endif
        *cp = _tolower(*cp);             /* force lowercase */
    }
    while ((strlen(line) > 0) &&
           (line[strlen(line)-1] == ' '))
        line[strlen(line)-1] = '\0';     /* remove trailing blanks */

    strcat(line, " ");              /* line ends with a space */
    sprintf(newrec, "%s%s%0*d%0*d", id, SOUNDEX_FIELD,
            SEQ_SIZE, 0, ATTR_SIZE, 0);
    cp = line;
    while(cp2 = strchr(cp, ' ')) {  /* break at space boundary */
        *cp2 = '\0';
        if (strlen(cp) >= MIN_KEYWORD) {
            strcat(newrec, soundex(dest, cp, SOUNDEX_SIZE));
            strcat(newrec, " ");
        }
#if NAME_HACK
        if (cp3 = strchr(cp, '-')) {
            *cp3 = '\0';
            if (strlen(cp) >= MIN_KEYWORD) {
                strcat(newrec, soundex(dest, cp, SOUNDEX_SIZE));
                strcat(newrec, " ");
            }
            if (strlen(cp3 + 1) >= MIN_KEYWORD) {
                strcat(newrec, soundex(dest, cp3 + 1, SOUNDEX_SIZE));
                strcat(newrec, " ");
            }
        }
#endif
        cp = cp2 + 1;
    }
    fprintf(fp, "%s\n", newrec);
}
