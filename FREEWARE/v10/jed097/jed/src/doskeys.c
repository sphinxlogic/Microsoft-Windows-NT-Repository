/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#define PC_NULL	"^@^C"
#define PC_UP	"^@H"
#define PC_DN	"^@P"
#define PC_RT	"^@M"
#define PC_LT	"^@K"
#define PC_PGUP	"^@I"
#define PC_PGDN	"^@Q"
#define PC_INS	"^@R"
#define PC_DEL	"^@S"
#define PC_END	"^@O"
#define PC_HOME	"^@G"

#define PC_F1	"^@;"
#define PC_F2	"^@<"
#define PC_F3	"^@="
#define PC_F4	"^@>"
#define PC_F5	"^@?"
#define PC_F6	"^@@"
#define PC_F7	"^@A"
#define PC_F8	"^@B"
#define PC_F9	"^@C"
#define PC_F10	"^@D"

#define PC_ALT_F1	"^@h"
#define PC_ALT_F2	"^@i"
#define PC_ALT_F3	"^@j"
#define PC_ALT_F4	"^@k"
#define PC_ALT_F5	"^@l"
#define PC_ALT_F6	"^@m"
#define PC_ALT_F7	"^@n"
#define PC_ALT_F8	"^@o"
#define PC_ALT_F9	"^@p"
#define PC_ALT_F10	"^@q"

#define PC_SHIFT_F1	"^@T"
#define PC_SHIFT_F2	"^@U"
#define PC_SHIFT_F3	"^@V"
#define PC_SHIFT_F4	"^@W"
#define PC_SHIFT_F5	"^@X"
#define PC_SHIFT_F6	"^@Y"
#define PC_SHIFT_F7	"^@Z"
#define PC_SHIFT_F8	"^@["
#define PC_SHIFT_F9	"^@\\"
#define PC_SHIFT_F10	"^@]"

SLang_define_key1(PC_DEL, (VOID *) delete_char_cmd, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_NULL, (VOID *) set_mark_cmd, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_LT, (VOID *) previous_char_cmd, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_UP, (VOID *) previous_line_cmd, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_DN, (VOID *) next_line_cmd, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_RT, (VOID *) next_char_cmd, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_PGUP, (VOID *) pageup_cmd, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_PGDN, (VOID *) pagedown_cmd, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_HOME, (VOID *) bol, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_END, (VOID *) eol_cmd, SLKEY_F_INTRINSIC, Global_Map);

SLang_define_key1("^K^@M", (VOID *) scroll_left, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1("^K^@K", (VOID *) scroll_right, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1("^K^@H", (VOID *) bob, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1("^K^@P", (VOID *) eob, SLKEY_F_INTRINSIC, Global_Map);

/* wordperfect type stuff */
SLang_define_key1(PC_F1, (VOID *) kbd_quit, SLKEY_F_INTRINSIC, Global_Map);
/* SLang_define_key1(PC_F2, (VOID *) search_forward_cmd, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_SHIFT_F2, (VOID *) search_backward_cmd, SLKEY_F_INTRINSIC, Global_Map); */
SLang_define_key1(PC_F4, (VOID *) indent_line, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_ALT_F5, (VOID *) set_mark_cmd, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_SHIFT_F4, (VOID *) narrow_paragraph, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_SHIFT_F6, (VOID *) center_line, SLKEY_F_INTRINSIC, Global_Map);
SLang_define_key1(PC_F7, (VOID *) exit_jed, SLKEY_F_INTRINSIC, Global_Map);
