/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */

#include <stdio.h>

#include "config.h"
#include "display.h"
#include "slang.h"

void (*tt_goto_rc)(int, int)  		= SLtt_goto_rc;
void (*tt_begin_insert)(void)  		= SLtt_begin_insert;
void (*tt_end_insert)(void) 	 	= SLtt_end_insert;
void (*tt_del_eol)(void)  		= SLtt_del_eol;
void (*tt_delete_nlines)(int)  		= SLtt_delete_nlines;
void (*tt_delete_char)(void)  		= SLtt_delete_char;
void (*tt_erase_line)(void)  		= SLtt_erase_line;
void (*tt_tt_normal_video)(void)  	= SLtt_normal_video;
void (*tt_cls)(void)  			= SLtt_cls;
void (*tt_beep)(void)  			= SLtt_beep;
void (*tt_reverse_index)(int) 		= SLtt_reverse_index;
void (*tt_smart_puts)(unsigned short *, unsigned short *, int, int)  = SLtt_smart_puts;
void (*tt_write_string)(char *)  	= SLtt_write_string;
void (*tt_putchar)(char)  		= SLtt_putchar;
void (*tt_init_video)(void)  		= SLtt_init_video;
void (*tt_reset_video)(void)  		= SLtt_reset_video;
void (*tt_normal_video)(void)  		= SLtt_normal_video;
void (*tt_set_scroll_region)(int, int)  = SLtt_set_scroll_region;
void (*tt_reset_scroll_region)(void)  	= SLtt_reset_scroll_region;
void (*tt_get_terminfo)(void)  		= SLtt_get_terminfo;
void (*tt_set_color)(int, char *, char *, char *) = SLtt_set_color;

#ifndef pc_system
void (*tt_set_color_esc)(int, char *)	= SLtt_set_color_esc;
void (*tt_wide_width)(void)  		= SLtt_wide_width;
void (*tt_narrow_width)(void)  		= SLtt_narrow_width;
void (*tt_enable_cursor_keys)(void)  	= SLtt_enable_cursor_keys;
void (*tt_set_term_vtxxx)(int *)  	= SLtt_set_term_vtxxx;
#endif

int *tt_Ignore_Beep  		= &SLtt_Ignore_Beep;
int *tt_Use_Ansi_Colors  	= &SLtt_Use_Ansi_Colors;
int *tt_Term_Cannot_Scroll  	= &SLtt_Term_Cannot_Scroll;
int *tt_Term_Cannot_Insert  	= &SLtt_Term_Cannot_Insert;
int *tt_Screen_Rows  		= &SLtt_Screen_Rows;
int *tt_Screen_Cols  		= &SLtt_Screen_Cols;

#ifndef pc_system
int *tt_Baud_Rate  		= &SLtt_Baud_Rate;
#endif

void flush_output (void)
{
   SLtt_flush_output ();
}
