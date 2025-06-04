/* index qi data file */
/* Bruce Tanner - Cerritos College */

/* 1.0  1993/08/14 Start with build_index */
/* 1.1  1993/08/30 Make fopen failure more explicit */
/* 1.2  1993/09/04 Move soundex creation outside */
/* 1.3  1993/10/01 Add /[no]update */
/* 1.4  1993/10/08 Change /[no]update to /merge[=replace] */
/* 1.5  1994/04/21 Add /delete */
/* 1.6  1994/04/27 Fix /merge=replace, delete old indexes on update */
/* 1.7  1994/05/27 Index file wouldn't get written without /DATA */
/* 2.0  1994/08/01 Index email with and without domain, make DECC happy */
/* 2.1  1994/09/19 Add new /update */
/* 3.0  1995/01/15 Index full name if INDEX_FULL_NAME is on in qi.h */
/* 3.1  1995/06/27 INDEX_FULL_NAME should only apply to names */

#include <ssdef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <rms.h>
#include <descrip.h>
#include <climsgdef.h>
#include <assert.h>
#include <starlet.h>
#include <lib$routines.h>
#define define_attributes
#include "qi.h"


char idx_record[IDX_RECORD_SIZE + 1];
char dat_record[DAT_RECORD_SIZE + 1];
char in_record[DAT_RECORD_SIZE + 1];                                
char idx_key[IDX_KEY_SIZE + 1];
char dat_key[DAT_KEY_SIZE + 1];
int  field_attrib[MAX_FIELD];
int mode = 0, index_flag, flags = 0;
struct FAB idxfab, datfab;
struct RAB idxrab, datrab;
struct XABKEY idxxab, idxxab2, datxab;

/* mode */
#define CREATE 1
#define MERGE 2
#define UPDATE 4
#define DELETE 8

/* flags */
#define DATA 1
#define INDEX 1
#define ALREADY_INDEXED 2

void read_fields(char *);
void index_words(char *, struct RAB *);
struct dsc$descriptor_s *descr(char *);
void build_commands();
void find_rec(char *);
void delete_range(int, int, int);
void delete_rec(char *);
void init_dsc(struct dsc$descriptor_s *, char *, int);

extern int cli$present();
extern int cli$dcl_parse();
extern int cli$get_value();
extern int sor$pass_files();
extern int sor$begin_sort();
extern int sor$sort_merge();
extern int sor$return_rec();
extern int sor$end_sort();
extern int strcasecmp(char *, char *);

main(int argc, char *argv[])
{

    char  cli_input[256], file_arg[256], file_spec[256];
    char  idx_name[256], dat_name[256], value[20], attr[ATTR_SIZE + 1];
    char  *ptr, field[DATA_SIZE + 1], id[ID_SIZE + 1], seq[SEQ_SIZE + 1];
    char  dat_copy[DAT_RECORD_SIZE + 1];
    int   status, status2, ind, start, end, context = 0, count = 0;
    short leng, lrl = DAT_RECORD_SIZE;
    short key_buffer[] = { 1, DSC$K_DTYPE_T, 0, 0, DAT_KEY_SIZE };
    struct dsc$descriptor_s input_dsc, file_dsc, file_spec_dsc, idx_dsc;
    struct dsc$descriptor_s in_dsc, value_dsc;

    init_dsc(&input_dsc, cli_input, sizeof(cli_input)-1);
    init_dsc(&file_dsc, file_arg, sizeof(file_arg)-1);
    init_dsc(&file_spec_dsc, file_spec, sizeof(file_spec)-1);
    init_dsc(&idx_dsc, idx_name, sizeof(idx_name)-1);
    init_dsc(&value_dsc, value, sizeof(value)-1);
    init_dsc(&in_dsc, in_record, sizeof(in_record)-1);

    status = lib$get_foreign(&input_dsc, 0, &leng, 0);

    for (ind = leng; ind >= 0; ind--)
        cli_input[ind+6] = cli_input[ind];
    strncpy(cli_input, "build ", 6);
    input_dsc.dsc$w_length = leng+6;

    status = cli$dcl_parse(&input_dsc, build_commands, lib$get_input);

    if (status != CLI$_NORMAL)  /* error in parse, exit */
        exit(1);

    status = cli$get_value(descr("file"), &file_dsc, &leng);  /* get source */

    if ((status & 1) == 0) {
        printf("Usage: build data_file /switches\n");
        printf("Switches:\n");
        printf("/create (default) create new output files\n");
        printf("/delete           delete range of ID before merge\n");
        printf("/merge            merge input with existing output files\n");
        printf("      =replace    update duplicate records\n");
        printf("/update           batch update records\n");
        printf("/start=n          (used with /delete, /update) ignore entry ID below n\n");
        printf("/end=n            (used with /delete, /update) ignore entry ID above n\n");
        printf("/config=<file>    config file name (defaults to <data_file>.cnf)\n");
        printf("/output=<file>    create/update index file (defaults to <data_file>.index)\n");
        printf("/data             create/update <output>.data file\n");
        exit(3);
    }

    status = lib$find_file(&file_dsc, &file_spec_dsc, &context, 0, 0, 0, 0);
    ptr = strchr(file_spec, ' ');
    if (ptr) *ptr = '\0';            /* chop off trailing spaces */
    strcpy(idx_name, file_spec);    /* make copy for output spec */

    if (cli$present(descr("output")) & 1) { /* if /output, overwrite out_name */
        status = cli$get_value(descr("output"), &idx_dsc, &leng);
        idx_name[leng] = '\0';
    }

    if (cli$present(descr("create")) & 1)
        mode = CREATE;
    if (cli$present(descr("merge")) & 1)
        mode = MERGE;
    if (cli$present(descr("update")) & 1)
        mode = UPDATE;
    if (cli$present(descr("delete")) & 1)
        mode |= DELETE;

    if (cli$present(descr("start")) & 1) {
        status = cli$get_value(descr("start"), &value_dsc, &leng);
        value[leng] = '\0';
        start = atoi(value);
    }

    if (cli$present(descr("end")) & 1) {
        status = cli$get_value(descr("end"), &value_dsc, &leng);
        value[leng] = '\0';
        end = atoi(value);
    }

    /* /MERGE or /DELETE or /UPDATE implies /DATA */
    if ((cli$present(descr("DATA")) & 1) || (mode & (MERGE | DELETE | UPDATE)))
        flags |= DATA;

    ptr = strrchr(idx_name, '.');  /* just get file name */
    if (ptr) *ptr = '\0';
    strcat(idx_name, ".INDEX");

    idxfab = cc$rms_fab;
    idxfab.fab$b_bks = 6;
    idxfab.fab$b_fac = FAB$M_GET | FAB$M_PUT | FAB$M_DEL;
    idxfab.fab$l_fna = idx_name;
    idxfab.fab$b_fns = strlen(idx_name);
    idxfab.fab$l_fop = FAB$M_CBT | FAB$M_DFW;
    idxfab.fab$w_mrs = IDX_RECORD_SIZE;
    idxfab.fab$b_org = FAB$C_IDX;
    idxfab.fab$b_rat = FAB$M_CR;
    idxfab.fab$b_rfm = FAB$C_FIX;
    idxfab.fab$b_shr = FAB$M_NIL;
    idxfab.fab$l_xab = (char *) &idxxab;

    idxrab = cc$rms_rab;
    idxrab.rab$l_fab = &idxfab;
    idxrab.rab$b_krf = 0;
    idxrab.rab$b_ksz = IDX_KEY_SIZE;
    idxrab.rab$l_kbf = idx_key;
    idxrab.rab$b_rac = RAB$C_KEY;
    idxrab.rab$l_rbf = idx_record;
    idxrab.rab$w_rsz = IDX_RECORD_SIZE;
    idxrab.rab$l_ubf = idx_record;
    idxrab.rab$w_usz = IDX_RECORD_SIZE;
    idxrab.rab$b_mbf = 20;
    idxrab.rab$l_rop = RAB$M_RAH | RAB$M_WBH;

    idxxab = cc$rms_xabkey;
    idxxab.xab$b_dtp = XAB$C_STG;
    idxxab.xab$b_flg = XAB$M_IDX_NCMPR;
    idxxab.xab$w_pos0 = 0;
    idxxab.xab$b_siz0 = IDX_KEY_SIZE;
    idxxab.xab$b_ref = PRIMARY_KEY;
    idxxab.xab$l_nxt = (char *) &idxxab2;

    idxxab2 = cc$rms_xabkey;
    idxxab2.xab$b_dtp = XAB$C_STG;
    idxxab2.xab$b_flg = XAB$M_IDX_NCMPR | XAB$M_DUP;
    idxxab2.xab$w_pos0 = KEYWORD_SIZE + FIELD_SIZE;
    idxxab2.xab$b_siz0 = ID_SIZE;
    idxxab2.xab$w_pos1 = KEYWORD_SIZE;
    idxxab2.xab$b_siz1 = FIELD_SIZE;
    idxxab2.xab$b_ref = SECONDARY_KEY;

    strcpy(dat_name, idx_name);
    ptr = strrchr(dat_name, '.');  /* just get file name */
    if (ptr) *ptr = '\0';
    strcat(dat_name, ".DATA");

    datfab = cc$rms_fab;
    datfab.fab$b_bks = 9;
    datfab.fab$b_fac = FAB$M_GET | FAB$M_PUT |FAB$M_DEL |  FAB$M_UPD;
    datfab.fab$l_fna = dat_name;
    datfab.fab$b_fns = strlen(dat_name);
    datfab.fab$l_fop = FAB$M_CBT | FAB$M_DFW;
    datfab.fab$w_mrs = DAT_RECORD_SIZE;
    datfab.fab$b_org = FAB$C_IDX;
    datfab.fab$b_rat = FAB$M_CR;
    datfab.fab$b_rfm = FAB$C_VAR;
    datfab.fab$b_shr = FAB$M_NIL;
    datfab.fab$l_xab = (char *) &datxab;

    datrab = cc$rms_rab;
    datrab.rab$l_fab = &datfab;
    datrab.rab$b_krf = 0;
    datrab.rab$b_ksz = DAT_KEY_SIZE;
    datrab.rab$l_kbf = dat_key;
    datrab.rab$b_rac = RAB$C_KEY;
    datrab.rab$l_rbf = dat_record;
    datrab.rab$w_rsz = DAT_RECORD_SIZE;
    datrab.rab$l_ubf = dat_record;
    datrab.rab$w_usz = DAT_RECORD_SIZE;
    datrab.rab$b_mbf = 20;
    datrab.rab$l_rop = RAB$M_RAH | RAB$M_WBH;

    datxab = cc$rms_xabkey;
    datxab.xab$b_dtp = XAB$C_STG;
    datxab.xab$b_flg = XAB$M_DAT_NCMPR | XAB$M_IDX_NCMPR;
    datxab.xab$w_pos0 = 0;
    datxab.xab$b_siz0 = DAT_KEY_SIZE;
    datxab.xab$b_ref = PRIMARY_KEY;


    /* open index file */
    if (mode & CREATE) {
        if (((status = sys$create(&idxfab)) & 1) != SS$_NORMAL)
            lib$stop(status);
    }
    else {
        if (((status = sys$open(&idxfab)) & 1) != SS$_NORMAL)
            lib$stop(status);
    }
    if (((status = sys$connect(&idxrab)) & 1) != SS$_NORMAL)
        lib$stop(status);

    /* open data file */
    if (flags & DATA) {
        if (mode & CREATE) {
            if (((status = sys$create(&datfab)) & 1) != SS$_NORMAL)
                lib$stop(status);
        }
        else {
            if (((status = sys$open(&datfab)) & 1) != SS$_NORMAL)
                lib$stop(status);
        }
        if (((status = sys$connect(&datrab)) & 1) != SS$_NORMAL)
            lib$stop(status);
    }

    /* record the fields with Indexed attribute */
    read_fields(file_spec);

    if (mode & DELETE)
        /* delete all the data and index records */
        delete_range(start, end, -1);

    for (;;) {  /* process all files in input spec, first one already found */

        printf("Sorting %s\n", file_spec);

        status = sor$pass_files(&file_spec_dsc);
        if ((status & 1) == 0) {
            printf("Can't read input file %s\n", file_spec);
            exit(status);
        }
        status = sor$begin_sort(key_buffer, &lrl);
        if ((status & 1) == 0)
            exit(status);

        status = sor$sort_merge(0);
        if ((status & 1) == 0)
            exit(status);

        printf("Building index for %s\n", file_spec);

        if (mode & UPDATE) {
            sprintf(dat_key, "%0*d", ID_SIZE, start);
            datrab.rab$b_ksz = strlen(dat_key);
            datrab.rab$l_rop = RAB$M_KGE;
            datrab.rab$b_rac = RAB$C_KEY;
            strncpy(dat_record, "", sizeof(dat_record));
            status = sys$get(&datrab);
        }

        while ((status = sor$return_rec(&in_dsc, &leng)) & 1) {
            in_record[leng] = '\0';

            if (strlen(in_record) == 0)
                continue;                 /* skip blank lines */

            if ((++count % 500) == 0)
                printf("%d\r", count);

            strncpy(id, in_record, ID_SIZE);
            id[ID_SIZE] = '\0';
            strncpy(field, in_record + ID_SIZE, FIELD_SIZE);
            field[FIELD_SIZE] = '\0';
            strncpy(seq, in_record + ID_SIZE + FIELD_SIZE, SEQ_SIZE);
            seq[SEQ_SIZE] = '\0';
            index_flag = INDEX;  /* index all indexable records by default */

            if (mode & UPDATE) {
                datrab.rab$b_rac = RAB$C_SEQ;
                /* delete all records between current rec and ID+FIELD of input */
                while (strncmp(dat_record, in_record, ID_SIZE + FIELD_SIZE) < 0) {
                    /* unless it was created online */
                    strncpy(attr, dat_record+DAT_KEY_SIZE, ATTR_SIZE);
                    attr[ATTR_SIZE + 1] = '\0';
                    if ((atoi(attr) & FIELD_ATTR_CHANGED) == 0)
                        delete_rec(dat_record);
                    strncpy(dat_record, "", sizeof(dat_record));
                    status = sys$get(&datrab);
                }
            }

            /* if /DATA requested, write .data file record */
            if (flags & DATA) {
                if (strncmp(dat_record, in_record, DAT_KEY_SIZE)) {
                    strcpy(dat_record, in_record);
                    datrab.rab$w_rsz = strlen(dat_record);
                    status = sys$put(&datrab);
                    find_rec(dat_record);  /* synch data pointer to input */
                }
                else
                    status = RMS$_DUP;   /* spoof a duplicate found */
                if ((status & 7) != 1) {
                    /* $put failed; found duplicate? */
                    if ((status == RMS$_DUP) && (mode & MERGE) &&
                        (cli$present(descr("merge.replace")) & 1)) {
                        /* duplicate found and /merge=replace */
                        strncpy(dat_key, dat_record, DAT_KEY_SIZE);
                        dat_key[DAT_KEY_SIZE + 1] = '\0';
                        datrab.rab$b_ksz = strlen(dat_key);
                        datrab.rab$l_rop = RAB$M_RAH | RAB$M_WBH;
                        datrab.rab$b_rac = RAB$C_KEY;
                        status = sys$find(&datrab);
                        /* replace just this field instance? */
                        if (cli$present(descr("merge.replace.one")) & 1) {
                            if (status == RMS$_NORMAL) {
                                datrab.rab$w_rsz = strlen(dat_record);
                                status = sys$update(&datrab);    /* update the record */
                                if (field_attrib[atoi(field)] & ATTR_INDEXED)
                                    delete_range(atoi(id), -1, atoi(field));
                            }
                        }
                        else {  /* replace all the higher sequence numbers */
                            dat_key[ID_SIZE + FIELD_SIZE] = '\0';  /* find rest of id+field */
                            datrab.rab$l_rop = RAB$M_KGE;
                            datrab.rab$b_rac = RAB$C_KEY;
                            while (((status2 = sys$get(&datrab)) & 1) &&
                                   (strncmp(dat_record, in_record, ID_SIZE) == 0))
                                delete_rec(dat_record);
                        }
                    }

                    /* record already exists, but the data may need updating */
                    if ((status == RMS$_DUP) && (mode & UPDATE)) {
                        strncpy(attr, dat_record+DAT_KEY_SIZE, ATTR_SIZE);
                        attr[ATTR_SIZE + 1] = '\0';
                        /* if the rec is diffrent and hasn't been changed online */
                        if (strcmp(dat_record, in_record) &&
                            (atoi(attr) & FIELD_ATTR_CHANGED) == 0) {
                            strcpy(dat_record, in_record);
                            datrab.rab$w_rsz = strlen(dat_record);
                            status = sys$update(&datrab);    /* update the record */
                            if ((field_attrib[atoi(field)] & ATTR_INDEXED) &&
                                (atoi(seq) == 0)) { /* don't back-delete indexes */
                                    delete_range(atoi(id), -1, atoi(field));
                            }
                        }
                        else { /* already there, but changed; so leave it alone */
                            status = RMS$_NORMAL;  /* keep the error code below happy */
                            index_flag = ALREADY_INDEXED;
                        }
                    }

                    if (status == RMS$_DUP)
                        printf("Duplicate: %s\n", dat_record);
                    if ((status & 7) != 1) {
                        printf("DATA rec (%d chars) %s\n", strlen(dat_record), dat_record);
                        lib$stop(status);
                    }
                }
            }
            strcpy(dat_copy, in_record);

            /* if this is an indexed field, write index record(s) */
            if ((index_flag != ALREADY_INDEXED) && 
                (field_attrib[atoi(field)] & ATTR_INDEXED)) {
                for (ptr = dat_copy; *ptr; ptr++) {
                    if (iscntrl(*ptr))  *ptr = ' ';  /* convert tabs to spaces */
                    *ptr = _tolower(*ptr);           /* force lowercase */
                }
                while ((strlen(dat_copy) > 0) &&
                       (dat_copy[strlen(dat_copy)-1] == ' '))
                    dat_copy[strlen(dat_copy)-1] = '\0';/* remove trailing blanks */
        
                index_words(dat_copy, &idxrab);
            }
            if (mode & UPDATE) {
                datrab.rab$b_rac = RAB$C_SEQ;
                strncpy(dat_record, "", sizeof(dat_record));
                status = sys$get(&datrab);          /* go to next record */
            }
        }

        sor$end_sort(0);

        /* delete any records that exist after the last input record */
        while (status == RMS$_NORMAL) {
            strncpy(id, dat_record, ID_SIZE);
            id[ID_SIZE] = '\0';
            delete_range(atoi(id), atoi(id), -1);
            status = sys$get(&datrab);
        }

        status = lib$find_file(&file_dsc, &file_spec_dsc, &context, 0, 0, 0, 0);
        if ((status & 1) == 0) {
            lib$find_file_end(&context);
            break;
        }
        ptr = strchr(file_spec, ' ');
        if (ptr) *ptr = '\0';            /* chop off trailing spaces */
    }
    if (flags & DATA)
        sys$close(&datfab);
    sys$close(&idxfab);
}


void find_rec(char *rec)
{
    int status;

    strncpy(dat_key, rec, DAT_KEY_SIZE);
    dat_key[DAT_KEY_SIZE + 1] = '\0';
    datrab.rab$b_ksz = strlen(dat_key);
    datrab.rab$l_rop = 0;
    datrab.rab$b_rac = RAB$C_KEY;
    strncpy(dat_record, "", sizeof(dat_record));
    status = sys$get(&datrab);
}


void write_index(char *cp, char *field, char *id, struct RAB *idxptr)
{
    int status;

    if (strlen(cp) > KEYWORD_SIZE)
        printf("Truncating %d character word \"%s\" to %d characters\n",
               strlen(cp), cp, KEYWORD_SIZE);
    if (strlen(cp) >= MIN_KEYWORD) {
        sprintf(idx_record, "%-*.*s%s%s",
                KEYWORD_SIZE, KEYWORD_SIZE, cp, field, id);
        strncpy(idx_key, idx_record, KEYWORD_SIZE + FIELD_SIZE);
        idxptr->rab$b_ksz = KEYWORD_SIZE + FIELD_SIZE;
        if ((field_attrib[atoi(field)] & ATTR_UNIQUE) &&
            ((status = sys$get(idxptr)) & 1))  /* unique record found? */
                printf("Omit duplicate unique record: %s\n", idx_record);
        else {
            idxptr->rab$w_rsz = IDX_RECORD_SIZE;
            if (((status = sys$put(idxptr)) & 1) == 0)
                if (status == RMS$_DUP)
                    printf("Duplicate index: %s\n", idx_record);
                else
                    lib$stop(status);
        }
    }
}


/* break data field into words and write them to index file */

void index_words(char *line, struct RAB *idxptr)
{
    char data[DATA_SIZE + 2], field[FIELD_SIZE + 1], id[ID_SIZE + 1];
    char *cp, *cp2, *cp3;
    int  status;

    strncpy(id, line, ID_SIZE);
    id[ID_SIZE] = '\0';
    strncpy(field, line + ID_SIZE, FIELD_SIZE);
    field[FIELD_SIZE] = '\0';
    strncpy(data, line + ID_SIZE + FIELD_SIZE + SEQ_SIZE + ATTR_SIZE, DATA_SIZE);
    data[DATA_SIZE] = '\0';

    /* special hack to index email with and without domain */
    if ((strcmp(field, EMAIL_FIELD) == 0) && (cp = strchr(data, '@'))) {
        write_index(data, field, id, idxptr);
        *cp = '\0';
    }

    if (strcmp(field, NAME_FIELD) == 0) { /* only edit name field */
#if INDEX_FULL_NAME
        if (strchr(data, ' '))  /* if only one word, will be indexed below */
            write_index(data, field, id, idxptr);  /* write the full name */
#endif
        for (cp = data; *cp; cp++) {    /* apply any special editing to names */
            if (*cp == ',') strcpy(cp, cp+1); /* remove commas */
            if ((*cp == '(') || (*cp == ')') || (*cp == '&'))
                *cp = ' ';   /* misc punctuation */
#if NAME_HACK
            if (*cp == '\'') strcpy(cp, cp+1); /* squeeze out apostrophe */
            if (*cp == '/')  *cp = ' '; /* index both slashed names */
#endif
        }
    }
    strcat(data, " ");              /* line ends with a space */
    cp = data;
    while(cp2 = strchr(cp, ' ')) {  /* break at space boundary */
        *cp2 = '\0';
        write_index(cp, field, id, idxptr);
#if NAME_HACK
        if ((strcmp(field, NAME_FIELD) == 0) &&
            (cp3 = strchr(cp, '-'))) {
            *cp3 = '\0';
            write_index(cp, field, id, idxptr);
            write_index(cp3 + 1, field, id, idxptr);
        }
#endif
        cp = cp2 + 1;
    }
}


/* field_num = -1 means all fields */
/* end = -1 means delete all indexes for id/field */

void delete_range(int start, int end, int field_num)
{
    int status;
    char id[ID_SIZE+1], field[FIELD_SIZE+1];

    /* delete data fields */
    datrab.rab$l_rop = RAB$M_KGE;
    datrab.rab$b_rac = RAB$C_KEY;
    if (field_num > -1)
        sprintf(dat_key, "%0*d%0*d", ID_SIZE, start, FIELD_SIZE, field_num);
    else
        sprintf(dat_key, "%0*d", ID_SIZE, start);
    datrab.rab$b_ksz = strlen(dat_key);  /* partial key size */

    while ((end > -1) && (status = sys$get(&datrab)) & 1) {
        datrab.rab$b_rac = RAB$C_SEQ;
        dat_record[ID_SIZE] = '\0';
        if (atoi(dat_record) > end)
            break;
        status = sys$delete(&datrab);
    }

    /* delete index records */
    idxrab.rab$b_krf = SECONDARY_KEY;
    idxrab.rab$l_rop = RAB$M_KGE;
    idxrab.rab$b_rac = RAB$C_KEY;
    if (field_num > -1)
        sprintf(idx_key, "%0*d%0*d", ID_SIZE, start, FIELD_SIZE, field_num);
    else
        sprintf(idx_key, "%0*d", ID_SIZE, start);
    idxrab.rab$b_ksz = strlen(idx_key);  /* partial key size */

    while ((status = sys$get(&idxrab)) & 1) {
        idxrab.rab$b_rac = RAB$C_SEQ;
        idx_record[KEYWORD_SIZE + FIELD_SIZE + ID_SIZE] = '\0';
        strncpy(id, idx_record + KEYWORD_SIZE + FIELD_SIZE, ID_SIZE);
        id[ID_SIZE] = '\0';
        strncpy(field, idx_record + KEYWORD_SIZE, FIELD_SIZE);
        field[FIELD_SIZE] = '\0';
        if ((end > -1) && (atoi(id) > end))
            break;
        if ((end == -1) &&
            ((atoi(id) > start) || (atoi(field) > field_num)))
            break;
        status = sys$delete(&idxrab);
    }
    idxrab.rab$l_rop = RAB$M_RAH | RAB$M_WBH;
    idxrab.rab$b_rac = RAB$C_KEY;
    idxrab.rab$b_krf = PRIMARY_KEY;  /* reset the krf */
}


/* same as delete_range, but just for one record */
void delete_rec(char *record)
{
    int status;

    /* delete data fields */
    datrab.rab$l_rop = RAB$M_KGE;
    datrab.rab$b_rac = RAB$C_KEY;
    strncpy(dat_key, record, DAT_KEY_SIZE);
    datrab.rab$b_ksz = DAT_KEY_SIZE;

    if ((status = sys$get(&datrab)) & 1)
        status = sys$delete(&datrab);

    /* delete index records */
    idxrab.rab$b_krf = SECONDARY_KEY;
    idxrab.rab$l_rop = RAB$M_KGE;
    idxrab.rab$b_rac = RAB$C_KEY;
    strncpy(idx_key, record, ID_SIZE + FIELD_SIZE);
    idxrab.rab$b_ksz = ID_SIZE + FIELD_SIZE;

    while ((status = sys$get(&idxrab)) & 1) {
        idxrab.rab$b_rac = RAB$C_SEQ;
        if (strncmp(idx_record + KEYWORD_SIZE + FIELD_SIZE, idx_key, ID_SIZE)
            || strncmp(idx_record + KEYWORD_SIZE, idx_key + ID_SIZE, FIELD_SIZE))
            break;
        status = sys$delete(&idxrab);
    }
    idxrab.rab$l_rop = RAB$M_RAH | RAB$M_WBH;
    idxrab.rab$b_rac = RAB$C_KEY;
    idxrab.rab$b_krf = PRIMARY_KEY;  /* reset the krf */
}


char * get_field(char *ptr, char *field)
{
    int ind;

    for (ind= 0; (*ptr != '\0') && (*ptr != ':'); ptr++, ind++)
        field[ind] = _tolower(*ptr);
    field[ind] = '\0';
    if (*ptr == ':') ptr++;  /* skip over terminating ":" */
    return ptr;
}


void read_fields(char *file)
{
    FILE *cnf;
    char *ptr, config[256], line[256], field[128];
    int ind, field_num;
    short leng;
    struct dsc$descriptor_s config_dsc;

    init_dsc(&config_dsc, config, sizeof(config)-1);
    if (cli$present(descr("configuration")) & 1) { /* if /config */
        cli$get_value(descr("configuration"), &config_dsc, &leng);
        config[leng] = '\0';
    }
    else {                      /* no /config switch */
        strcpy(config, file);
        ptr = strrchr(config, '.');
        if (ptr) *ptr = '\0';
        strcat(config,".cnf");
    }

    for (ind = 0; ind < MAX_FIELD; ind++)
        field_attrib[ind] = 0;                  /* init array */

    if ((cnf = fopen(config, "r", "dna=.cnf")) == NULL) {
        printf("Can't read config file %s\n", config);
        exit(7);
    }

    while (fgets(line, sizeof(line), cnf)) {
        ptr = strchr(line, '\n');
        if (ptr) *ptr = '\0';                   /* remove newline */

        ptr = line;
        if ((*ptr == '#') || (*ptr == '\0'))    /* comment or blank? */
            continue;                           /* yes, skip line */
        ptr = get_field(ptr, field);            /* field number */
        field_num = atoi(field);

        ptr = get_field(ptr, field);            /* field name */
        ptr = get_field(ptr, field);            /* field size */
        ptr = get_field(ptr, field);            /* field description */
        ptr = get_field(ptr, field);            /* field option */

        for (;;) {
            ptr = get_field(ptr, field);	/* get attribute */
            if (strlen(field) == 0)
                break;                          /* no more attributes */

            for (ind = 0; ind < MAX_ATTRIBUTES; ind++)
                if (strcasecmp(field, attributes[ind].name) == 0)
                    field_attrib[field_num] |= attributes[ind].value;
        }
    }

    fclose(cnf);
}

/* descr() creates character descriptor and returns
 * the address of the descriptor to the caller.
 */
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

void init_dsc(struct dsc$descriptor_s *dest, char *str, int size)
{
    dest->dsc$w_length = (unsigned short) size;
    dest->dsc$b_dtype = DSC$K_DTYPE_T;
    dest->dsc$b_class = DSC$K_CLASS_S;
    dest->dsc$a_pointer = str;
}
