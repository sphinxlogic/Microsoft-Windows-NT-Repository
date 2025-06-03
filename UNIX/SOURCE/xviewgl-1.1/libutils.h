/* Warning:  stdio.h *must* be include before my_types.h if this file is
	     also included.  This file needs symbols defined in stdio.h,
	     but if it were to explicitly include stdio.h,  FILENAME_MAX
	     would be redefined in some cases. */
#ifndef LIBUTILS_INCLUDED
#define LIBUTILS_INCLUDED
#include "patchlevel.h"
#include "my_types.h"

typedef struct {
    FILE *fp;
    int bytesleft;
} MY_FILE;

typedef struct {
    fpos_t pos;
    int bytesleft;
} my_fpos_t;

extern struct lib_file_entry {
    long offset;
    char name[13];
} *lib_files;
extern int lib_filecnt;
extern int directory_mode;
extern int prog_index;

BOOLEAN init_lib(char *s, char *script_name, char *err_msg);
MY_FILE *find_file(char *file);
MY_FILE *reopen_prog(char *file, char *mode);
void rewind_program();
int my_fgetc(MY_FILE *fp);
char *my_fgets(char *s, int n, MY_FILE *fp);
int my_read(MY_FILE *fp, void *buf, int n);
int my_fgetpos(MY_FILE *fp, my_fpos_t *pos);
int my_fsetpos(MY_FILE *fp, my_fpos_t *pos);
int my_read(MY_FILE *fp, void *buf, int n);
void my_fclose(MY_FILE *fp);
#define my_getc(fp_) (directory_mode ? \
			(getc((fp_)->fp)) : \
			(((fp_)->bytesleft-->0)?getc((fp_)->fp):-1))
#define my_ungetc(c,fp_) (directory_mode ? \
			(((c)==-1)?(c):ungetc((c),(fp_)->fp)) : \
			(((c)==-1)?(((fp_)->bytesleft=0),-1) : \
				   ((fp_)->bytesleft++,ungetc((c),(fp_)->fp))))
#endif
