/* tag.c */
int do_tag __ARGS((char_u *tag, int type, int count, int forceit, int verbose));
void do_tags __ARGS((void));
int find_tags __ARGS((char_u *pat, int *num_matches, char_u ***matchesp, int flags, int mincount));
void simplify_filename __ARGS((char_u *filename));
int expand_tags __ARGS((int tagnames, char_u *pat, int *num_file, char_u ***file));
