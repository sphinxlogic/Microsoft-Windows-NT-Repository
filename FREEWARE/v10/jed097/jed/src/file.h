/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#ifdef VMS
#include <file.h>
#endif

#include "buffer.h"
#include "vfile.h"

extern int Require_Final_Newline;
extern int read_file(char *);
extern int insert_file(char *);
extern void set_file_modes(void);
extern void auto_save_buffer(Buffer *);
extern void auto_save_all(void);
extern void auto_save(void);
extern int write_region_to_fp(int);
extern int write_region(char *);
extern int read_file_pointer(int);
extern int insert_file_pointer(VFILE *);
extern int append_to_file(char *);
extern int write_file_with_backup(char *, char *);
extern void visit_file(char *, char *);
extern void fixup_dir(char *);
extern char *dir_file_merge(char *, char *);
extern int file_status(char *);
extern int file_changed_on_disk(char *);
extern int file_time_cmp(char *, char *);
extern char *file_type(char *);
extern void check_buffer(Buffer *);
extern void check_buffers(void);
extern void set_file_trans(int *);

#ifdef unix
extern char *expand_link(char *);
#endif
