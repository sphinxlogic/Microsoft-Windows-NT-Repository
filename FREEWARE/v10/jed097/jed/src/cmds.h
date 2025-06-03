/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
extern int jed_buffer_visible (char *b);
extern void blink_match(char);
extern int goto_match(void);
extern int newline(void);
extern int previous_char_cmd(void);
extern int delete_char_cmd(void);
extern int backward_delete_char_cmd(void);
extern int backward_delete_char_untabify(void);
extern int previous_line_cmd(void);
extern int next_line_cmd(void);
extern int next_char_cmd(void);
extern int kill_line(void);
extern void insert_line(Line *line);
extern int double_line(void);
extern int kill_eol_cmd(void);
extern int kill_line_cmd(void);
extern int trim_whitespace(void);
extern int indent_line(void);
extern int transpose_lines(void);
extern int newline_and_indent(void);
extern int brace_bra_cmd(void);
extern int brace_ket_cmd(void);
extern int eol_cmd(void);
extern int c_comment(void);
extern int sys_spawn_cmd(void);
extern int ins_char_cmd(void);
extern int exit_jed(void);
extern int quit_jed(void);
extern int save_some_buffers(void);
extern int pagedown_cmd(void);
extern int pageup_cmd(void);
extern int quoted_insert(void);
extern int scroll_left(void);
extern int scroll_right(void);
extern void indent_to(int);
extern int goto_column1(int *);
extern void goto_column(int *);
extern void insert_whitespace(int *);
extern unsigned char *get_indent(int *);
extern unsigned char *get_current_indent(int *);
extern unsigned int skip_whitespace(void);
extern int looking_at(char *);
extern void skip_chars(char *);
extern void skip_chars1(char *, int);
extern void bskip_chars(char *);
extern void bskip_chars1(char *, int);
#ifndef VMS
extern int Last_Command_Char;
#endif
extern int Blink_Flag;
extern int Indented_Text_Mode;
extern int Kill_Line_Feature;
extern int C_Bra_Newline;
extern int C_Comment_Hint;
#if 0
extern int new_find_matching (int *, int *);
#endif

extern void jed_make_lut (unsigned char *, unsigned char *, unsigned char);
