/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#ifndef __DAVIS_SYSDEP_H__
#define __DAVIS_SYSDEP_H__

#include <stdio.h>
#ifndef sequent
# include <stdlib.h>
#else
extern int chdir(char *); 
#endif

#ifndef VMS
extern volatile int KeyBoard_Quit;
#endif
extern int Ignore_User_Abort;

#ifdef msdos
#define MAX_INPUT_BUFFER_LEN 40
#else
#define MAX_INPUT_BUFFER_LEN 1024
#endif

extern int Input_Buffer_Len;
extern unsigned char Input_Buffer[MAX_INPUT_BUFFER_LEN];

extern unsigned char sys_getkey(void);

extern void init_tty(void);
extern void reset_tty(void);
extern void sys_suspend(void);
extern int my_getkey(void);
extern int input_pending(int *);
extern void flush_input(void);
extern void ungetkey_string(char *, int);
extern void buffer_keystring (char *, int);
extern void ungetkey(int *);
extern void sys_resume(void);
extern void get_term_dimensions(int *, int *);
extern int sys_delete_file(char *);
extern int sys_chmod(char *, int, int *, short *, short *);
extern unsigned long sys_file_mod_time(char *);

/* expand the file in a canonical way and return a pointer to a 
   static area which should be regarded volatile */
extern char *expand_filename(char *);

/* return pointer to place filname starts in path */
extern char *extract_file(char *);

extern int sys_findfirst(char *);
extern int sys_findnext(char *);

unsigned long sys_file_mod_time(char *file);
extern unsigned long  sys_time(void);
extern char *get_time(void);
extern int Meta_Char;
extern int DEC_8Bit_Hack;
extern int Abort_Char;
extern void map_character(int *, int *);
extern int make_directory(char *);
extern int make_random_number(int *, int *);
extern int delete_directory(char *);
extern int ch_dir(char *);
extern void deslash(char *);
extern char *slash2slash (char *);

#if defined(msdos) || defined(__os2__)
extern int NumLock_Is_Gold;
#endif

#ifdef __os2__
extern int IsHPFSFileSystem(char *);
#endif

#ifdef __GO32__
extern void i386_check_kbd(void);
#endif

#ifdef pc_system
#define SCREEN_HEIGHT 25
extern int PC_Alt_Char;
extern char *msdos_pinhead_fix_dir(char *);
extern int sys_System (char *);
#else
#define SCREEN_HEIGHT 24
#endif

#ifdef sequent
# define strchr index
extern char *my_strstr();
#endif

#ifdef unix
extern void enable_flow_control(int *);
#endif

#ifdef VMS
void vms_cancel_exithandler(void);
extern int vms_send_mail(char *, char *);
extern int vms_get_help (char *, char *);
#endif



#ifdef VMS
#endif
extern char *get_cwd(void);

#endif /* __DAVIS_SYSDEP_H__ */
