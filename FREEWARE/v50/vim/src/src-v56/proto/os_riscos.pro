/* Prototypes.
 * Stolen from os_unix_pro.h
 */
void mch_write __ARGS((char_u *s, int len));
int mch_inchar __ARGS((char_u *buf, int maxlen, long wtime));
int mch_char_avail __ARGS((void));
long_u mch_avail_mem __ARGS((int special));
void mch_delay __ARGS((long msec, int ignoreinput));
void mch_suspend __ARGS((void));
void mch_windinit __ARGS((void));
int mch_check_win __ARGS((int argc, char **argv));
int mch_input_isatty __ARGS((void));
int mch_can_restore_title __ARGS((void));
int mch_can_restore_icon __ARGS((void));
void mch_settitle __ARGS((char_u *title, char_u *icon));
void mch_restore_title __ARGS((int which));
int mch_get_user_name __ARGS((char_u *s, int len));
void mch_get_host_name __ARGS((char_u *s, int len));
long mch_get_pid __ARGS((void));
int mch_dirname __ARGS((char_u *buf, int len));
int mch_FullName __ARGS((char_u *fname, char_u *buf, int len, int force));
int mch_isFullName __ARGS((char_u *fname));
long mch_getperm __ARGS((char_u *name));
int mch_setperm __ARGS((char_u *name, long perm));
void mch_hide __ARGS((char_u *name));
int mch_isdir __ARGS((char_u *name));
void mch_windexit __ARGS((int r));
void mch_settmode __ARGS((int tmode));
void mch_setmouse __ARGS((int on));
int mch_screenmode __ARGS((char_u *arg));
int mch_get_winsize __ARGS((void));
void mch_set_winsize __ARGS((void));
int mch_call_shell __ARGS((char_u *cmd, int options));
void mch_breakcheck __ARGS((void));
int mch_expandpath __ARGS((struct growarray *gap, char_u *path, int flags));
int mch_expand_wildcards __ARGS((int num_pat, char_u **pat, int *num_file, char_u ***file, int flags));
int mch_has_wildcard __ARGS((char_u *p));
int mch_remove __ARGS((char_u *file));

char_u *mch_munge_fname(char_u *fname);
void mch_read_filetype(char_u *file);
void mch_set_filetype(char_u *file, char_u *type);
int mch_check_filetype(char_u *fname, char_u *type);
