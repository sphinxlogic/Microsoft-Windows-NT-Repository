/* qi_make - convert a sequential file to a format suitable for a qi build */

/* 1.0 1993/08/30 release for beta testing */
/* 1.1 1993/09/13 added long input records & fields, "\n" token in fields */
/* 1.2 1993/10/06 add virtual fields */
/* 1.3 1993/10/22 added error message for long records */
/* 1.4 1994/04/24 (loh@millsaps) add capitalize options 2 (uc), 3 (lc) */
/* 1.5 1994/05/18 warn about short records and pad the record with spaces */
/* 1.6 1994/07/19 fixed field overlap checking, blank lines in parameter file */
/* 2.0 1994/07/22 revamp virtual field processing to use printf formating */
/* 2.1 1994/08/09 change handling of comma in cap(), insert a space if needed */
/* 3.0 1994/12/08 preload virtual field -1 (ID number) in case it is needed */
/* 3.1 1995/07/26 fix problem parsing virtual field numbers containing zeros */
/* 3.2 1995/10/20 apply blank and bz rules to virtual fields too */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "qi.h"

#define MAX_PARAMS 100
#define MAX_FIELD_SIZE 300

int recsz;

struct param {
    int pos;
    int size;
    int field;
    int cap;
    int bz;
    char *fmt;
    char *args;
} params[MAX_PARAMS + 1];

struct vparam {
    int valid;
    char *str;
} vfields[MAX_PARAMS];

void cap(int, char *);
void trim(char *);
int read_params(char *);
void build_str(char **, char *, char *, char *);
char *newcpy(char *);
int is_zeros(char *);


main(int argc, char *argv[])
{

    FILE *in, *out;
    char *record, *field, *new_field;
    char filename[256], temp[256], *cp, *fld;
    int pidx, base = 0, id, index = 1, ridx, recno = 0, sequence, vfield_num;

    if (argc < 3) {
        printf("Usage: make input output [base]\n");
        exit(10002);
    }

    strcpy(filename, argv[1]);
    if (cp = strchr(filename, '.')) *cp = '\0';
    strcat(filename, ".qi_make");

    if (read_params(filename) == FALSE) {
        sprintf(temp, "can't open parameter file %s", filename);
        perror(temp);
        exit(10004);
    }
    record = (char *) malloc(recsz);
    field = (char *) malloc(MAX_FIELD_SIZE);
    new_field = (char *) malloc(MAX_FIELD_SIZE);

    /* check for record definition overlap */
    strncpy(record, "", recsz);
    for (pidx = 0; params[pidx].size > -1; pidx++)
        for (ridx = 0; ridx < params[pidx].size; ridx++) {
            if (record[params[pidx].pos + ridx] > '\0')
                printf("Record overlap at %d in field %d\n",
                       params[pidx].pos + ridx, params[pidx].field);
            record[params[pidx].pos + ridx] = 'X';
        }

    if ((in = fopen(argv[1], "r")) == NULL) {
        perror("Can't open input file");
        exit(10006);
    }
    if ((out = fopen(argv[2], "w", "rfm=var", "rat=cr")) == NULL) {
        perror("Can't open output file");
        exit(10008);
    }
    if (argc == 4)
        base = atoi(argv[3]);

    printf("Convert qi data from %s to %s\n", argv[1], argv[2]);

    for (pidx = 0; params[pidx].size > -1; pidx++)
        if ((params[pidx].field == -1) && (strlen(params[pidx].fmt) > 0)) {
            base = atoi(params[pidx].fmt);
            break;
        }

    printf("ID base set to %d\n", base);

    for (ridx = 0; ridx < recsz; ridx++)  /* init record[] */
        record[ridx] = ' ';
    record[recsz - 1] = '\0';
    while (fgets(record, recsz, in)) {
        recno++;
        if (record[recsz - 2] != '\n') {
            printf("Record %d size was defined as %d, and is actually ",
                   recno, recsz - 2);
            cp = strchr(record, '\n');
            if (cp == NULL) {
               printf("something larger\n");
               exit(10010);   /* stop here */
            }
            else {
               printf("%d\n", cp - record);
               *cp = ' ';   /* drop in a space and keep going */
            }
        }
        id = index++ + base;
        if ((index % 100) == 0) printf("%d\r", index);

        /* fill all virtual fields for this record */
        for (pidx = 0; params[pidx].size > -1; pidx++) {
            if (params[pidx].field > -1) continue;
            strncpy(field, "", MAX_FIELD_SIZE);
            if ((params[pidx].pos > -1) && (params[pidx].size > 0))
                strncpy(field, record + params[pidx].pos, params[pidx].size);
            if (params[pidx].cap) cap(params[pidx].cap, field);
            trim(field);
            vfield_num = params[pidx].field * -1;
            if ((params[pidx].field == -1) && (params[pidx].size > 0))
                id = atoi(field) + base;   /* field -1 is id field */
            else {
                if ((strlen(field) == 0) || 
                    (params[pidx].bz && is_zeros(field))) /* blank the zeros? */
                    vfields[vfield_num].str = newcpy("");
                else
                    build_str(&vfields[vfield_num].str, params[pidx].fmt,
                              field, params[pidx].args);
                vfields[vfield_num].valid = (vfields[vfield_num].str != NULL);
            }
        }

        /* remember the ID field in case it's used */
        vfields[1].str = (char *) calloc(ID_SIZE + 1, sizeof(char)); 
        sprintf(vfields[1].str, "%d", id);
        vfields[1].valid = True;

        /* write all the actual fields for this record */
        for (pidx = 0; params[pidx].size > -1; pidx++) {
            if (params[pidx].field < 0) continue;  /* skip virtual fields */
            strncpy(field, "", MAX_FIELD_SIZE);
            strncpy(field, record + params[pidx].pos, params[pidx].size);
            if (params[pidx].cap) cap(params[pidx].cap, field);
            trim(field);
            if ((params[pidx].size == 0) || /* constant field or */
                ((strlen(field) > 0) &&     /* field contains data and */
                 (!params[pidx].bz ||       /* it's not zeros that should */
                  !is_zeros(field)))) {     /* be blanked */
                build_str(&new_field, params[pidx].fmt, field, params[pidx].args);
                sequence = 0;
                fld = new_field;
                for (;;) {
                    if (cp = strstr(fld, "\\n"))
                        *cp = '\0';
                    if (strlen(fld) > 0)    /* don't write blank records */
                        fprintf(out, "%0*d%0*d%0*d%0*d%s\n",
                            ID_SIZE, id,
                            FIELD_SIZE, params[pidx].field,
                            SEQ_SIZE, sequence++, ATTR_SIZE, 0, fld);
                    if (cp == NULL)
                        break;
                    fld = cp + 2;
                }
                free(new_field);
            }
        }
        for (ridx = 0; ridx < recsz; ridx++)  /* clear the record */
            record[ridx] = ' ';
        record[recsz - 1] = '\0';
    }
    fclose(in);
    fclose(out);
}


/* allocate new memory for a string */
char *newcpy(char *src)
{
    char *dest;

    dest = (char *) calloc(strlen(src) + 1, sizeof(char));
    strcpy(dest, src);
    return dest;
}


/* build a string with sprintf(); args are -n for vfields[n] or 0 for field */
void build_str(char **dest, char *fmt, char *field, char *args)
{
    char *arg[10];
    char str[MAX_FIELD_SIZE];
    char *cp;
    int ind, aind, vf;

    for (aind = 0; aind < 10; aind++)   /* predefine all args */
        arg[aind] = field;              /* to the default field */

    aind = 0;
    for (cp = args; *cp; ) {
        while (*cp && (*cp == ' ')) cp++;  /* skip spaces */  
        if (*cp && (*cp == ',')) cp++;     /* skip the comma */
        vf = atoi(cp);
        while (*cp && (*cp != ',')) cp++;  /* go to next field */  
        if (vf == 0)
            arg[aind++] = field;
        else
            if (vf < 0) {
                if (vfields[-1 * vf].valid) {
                    arg[aind++] = vfields[-1 * vf].str;
                }
                else {
                    printf("Virtual field %d is invalid\n", vf);
                    *dest = newcpy("");  /* return blank */
                    return;
                }
           }
           else {
                printf("Field %d must be 'virtual' (negative)\n", vf);
                *dest = newcpy("");  /* return blank */
                return;
            }
        }

    if (strlen(fmt) == 0) strcpy(fmt, "%s");  /* default format to %s */

    sprintf(str, fmt, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5],
            arg[6], arg[7], arg[8], arg[9]);
    for (ind = 0; ind < aind; ind++)
        if (arg[ind] && (strlen(arg[ind]) > 0)) break;
    if (aind && (ind == aind))      /* if all args are empty */
        *dest = newcpy("");         /* then clear field */
    else
        *dest = newcpy(str);        /* copy str off of the stack */
}


void strxcpy(char *dest, char *src, int cnt)
{
    if (cnt == 0) return;

    for (;*src; src++) {
        if ((*src != '\\') || (*(src + 1) != ','))  /* converts \, to , */
            *dest++ = *src;
        if (--cnt == 0)
            break;
    }
}


int read_params(char *name)
{
    FILE *fp;
    int ind = 0;
    char *cp, *cp2, val[10], line[256];

    fp = fopen(name, "r");

    if (fp == NULL) return (FALSE);

    while (fgets(line, sizeof(line), fp)) {
        if ((line[0] == '#') || (line[0] == '\n')) continue;
        cp = strchr(line, '\n');
        if (cp) *cp = '\0';
        cp = line;

        /* position */
        cp2 = strchr(cp,',');
        if (cp2 == NULL) {
            printf("Invalid param line: %s\n", line);
            continue;
        }
        strncpy(val, "", sizeof(val));
        strncpy(val, cp, cp2 - cp);
        params[ind].pos = atoi(val) - 1;  /* origin 0 into field */
        cp = cp2 + 1;

        /* size */
        cp2 = strchr(cp,',');
        if (cp2 == NULL) {
            printf("Invalid param line: %s\n", line);
            continue;
        }
        strncpy(val, "", sizeof(val));
        strncpy(val, cp, cp2 - cp);
        params[ind].size = atoi(val);
        cp = cp2 + 1;

        /* field number */
        cp2 = strchr(cp,',');
        if (cp2 == NULL)
            cp2 = strchr(cp, '\0');
        strncpy(val, "", sizeof(val));
        strncpy(val, cp, cp2 - cp);
        params[ind].field = atoi(val);
        cp = cp2 + 1;

        /* capitalize */
        cp2 = strchr(cp,',');
        if (cp2 == NULL)
            cp2 = strchr(cp, '\0');
        strncpy(val, "", sizeof(val));
        strncpy(val, cp, cp2 - cp);
        params[ind].cap = atoi(val);
        cp = *cp2 ? cp2 + 1 : cp2;

        /* blank-when-zero */
        cp2 = strchr(cp,',');
        if (cp2 == NULL)
            cp2 = strchr(cp, '\0');
        strncpy(val, "", sizeof(val));
        strncpy(val, cp, cp2 - cp);
        params[ind].bz = atoi(val);
        cp = *cp2 ? cp2 + 1 : cp2;

        /* format string */
        /* find comma, but skip over '\,' */
        for (cp2 = cp; *cp2; cp2++)
            if ((*cp2 == '\\') && (*(cp2 + 1) == ','))
                cp2++;
            else if (*cp2 == ',')
                break;
        params[ind].fmt = (char *) calloc((cp2 - cp) + 1, sizeof(char));
        strxcpy(params[ind].fmt, cp, cp2 - cp);
        cp = *cp2 ? cp2 + 1 : cp2;

        /* include arg list */
        cp2 = strchr(cp, '\0');
        params[ind].args = (char *) calloc((cp2 - cp) + 1, sizeof(char));
        strxcpy(params[ind].args, cp, cp2 - cp);
        cp = *cp2 ? cp2 + 1 : cp2;

        if (recsz < (params[ind].pos + params[ind].size + 2))  /* \n\0 */
            recsz = (params[ind].pos + params[ind].size + 2);

        if (++ind == MAX_PARAMS) {
            printf("Parameter file contains more than %d rules\n", MAX_PARAMS);
            break;
        }
    }
    params[ind].size = -1;  /* set sentinel .size = -1 */

    fclose(fp);
    return (TRUE);
}

int is_zeros(char *s)
{
    char *cp;

    for (cp = s; *cp; cp++)
        if (*cp != '0')
            return False;
    return True;
}

void trim(char *s)
{
    while ((strlen(s) > 0) && (s[strlen(s)-1] == ' '))
        s[strlen(s)-1] = '\0';
}

void cap(int capcode, char *s)
{
    char *ptr, temp[MAX_FIELD_SIZE];

    strncpy(temp, "", MAX_FIELD_SIZE);
    switch ( capcode ) {

    case 1: /* apply capitalization algorithm */
        *s = _toupper(*s);
        for (ptr = s + 1; *ptr; ptr++) {
            *ptr = _tolower(*ptr);
            if ((*ptr == ',') && (*(ptr+1) != ' ')) {
                strncpy(temp, s, ptr - s);  /* copy up to the comma */
                strcat(temp, ", ");         /* add comma, space */
                strcat(temp, ptr+1);        /* copy rest of string */
                strcpy(s, temp);            /* move new string back to *s */
            }
            if (!isalnum(*(ptr - 1)) && (*(ptr - 1) != '\''))
                *ptr = _toupper(*ptr);
                if ((*(ptr - 1) == 'I') && (*ptr == 'i'))
                    *ptr = _toupper(*ptr);
        }
        break;

    case 2: /* all uppercase */
        for (ptr = s; *ptr; ptr++) *ptr = toupper(*ptr);
        break;

    case 3: /* all lowercase */
        for (ptr = s; *ptr; ptr++ ) *ptr = tolower(*ptr);
        break;

    case 0:	/* ignore - just in case */
        break;

    default: /* ignore bad codes */
        break;

    }
}
