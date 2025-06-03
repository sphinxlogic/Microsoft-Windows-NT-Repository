/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */

extern int set_mark_cmd(void);
extern int push_spot(void);
extern int push_mark(void);
extern void goto_mark(Mark *);
extern int pop_mark(int *);
extern int mark_spot(void);
extern int pop_spot(void);
extern int widen_buffer(Buffer *);
extern int widen(void);
extern int exchange_point_mark(void);
extern int narrow_to_region(void);
extern int yank(void);
extern int check_region(int *);
extern int copy_region_to_buffer(Buffer *);
extern int delete_region(void);
extern int copy_to_pastebuffer(void);
extern int kill_region(void);
extern int insert_rectangle(void);
extern int kill_rectangle(void);
extern int blank_rectangle(void);
extern int open_rectangle(void);
extern int copy_rectangle(void);

extern void goto_user_mark (void);
extern void create_user_mark (void);
extern void free_user_marks (Buffer *);
extern char *user_mark_buffer (void);
extern register_jed_classes (void);
