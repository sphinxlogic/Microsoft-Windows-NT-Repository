#module DEVIL$TEXT "V5.0-000"
/****************************************************************************
!*                                                                          *
!*  COPYRIGHT (c) 1983, 1984, 1985 BY                                       *
!*  NIHON DIGITAL EQUIPMENT CORPORATION, TOKYO, JAPAN.                      *
!*  ALL RIGHTS RESERVED.                                                    *
!*                                                                          *
!*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED   *
!*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE   *
!*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER   *
!*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY   *
!*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY   *
!*  TRANSFERRED.                                                            *
!*                                                                          *
!*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE   *
!*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT   *
!*  CORPORATION.                                                            *
!*                                                                          *
!*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS   *
!*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.                 *
!*                                                                          *
!*                                                                          *
!***************************************************************************/
/*++
! FACILITY: DEVIL Version 4.5-000
!
! FUNCTIONAL DESCRIPTION:
!
! ENVIRONMENT: VAX/VMS
!
! AUTHOR: Inasawa, Yuichi       CREATION DATE: 22-May-1985
!
! MODIFIED BY:
!
!
! V6.0-1:
!	antonietta	Tue Jul 20 17:49:38 1993
!	Don't put LS3R if current state is LS2R-ed at beginning of line
!	(for LN80).
!
! V5.4-2:
!	antonietta	Mon Dec 10 17:18:11 1990
!	Restore default line pitch value (for LN03)
!
! V5.2-000
!	antonietta	Jun 16 1989
!	1) Set left margin same as V4.5 on Kprint/Ln03 in source file DEVTEXT.C
!	2) Dissabele kanji code conversion (2 ku -> 10 ku) on Kprint/LBP
!
! V4.5-000
!	Y. Kozono	28-Nov-1986
!	LN03 support
--*/

#include "jsy$devil:devildef.h"

static struct FUNC {
	int (*dev_setup)();
	int (*put_init)();
	int (*put_reset)();
	int (*put_space)();
	int (*put_ascii)();
	int (*put_kana)();
	int (*put_kanji)();
	int (*put_keisen)();
	int (*put_init_text)();
	int (*put_vertical)();
	int (*put_horizontal)();
	int (*put_char_attr)();
	int (*put_chapter_bar)();
	int (*box_from)();
	int (*box_to)();
	int (*set_font)();
	int (*set_line_pitch)();
	int (*set_char_pitch)();
	int (*skip_pitch)();
	int (*buff_char)();
	int (*buff_string)();
	int (*buff_output)();
    } func;

static struct {
	unsigned    v_doublewidth	    : 1;
	unsigned    v_doublehight_add_line  : 1;
	unsigned    v_overstrike	    : 1;
	unsigned    v_vertical		    : 1;
	int	    l_char_set;
	int	    l_input_kana_shift;
    } devil$txt;

#define CHAR_SET_ASCII		0
#define CHAR_SET_KANA		1
#define CHAR_SET_KANJI		2
#define FORM_STR(str)		devil$buff_string(str, (sizeof str) - 1)
#define STR_CSI			"\233"
#define JSY$C_EOS		0xFFFF
#define KEISEN_DEC_BEG		0xFE21
#define KEISEN_JIS_BEG		0xA8A1

#define ON			1
#define OFF			0

#define TAB			'\t'
#define CR			'\r'
#define LF			'\n'
#define FORM_FEED		'\f'
#define ESC			'\033'

static unsigned char vertical_table[2][94] =

/* 1-ku     0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
/* A */       {{0,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* B */     0,  3,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  6,  7,  0,
/* C */     0,  8,  9, 10, 11, 12,  0,  0,  0,  0, 13, 14, 15, 16, 17, 18,
/* D */    19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,  0,  0,  0,  0,
/* E */     0, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* F */     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},

/* 4,5-ku   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
/* A */        {1,  0,  2,  0,  3,  0,  4,  0,  5,  0,  0,  0,  0,  0,  0,
/* B */     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* C */     0,  0,  0, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* D */     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* E */     0,  0,  0,  6,  0,  7,  0,  8,  0,  0,  0,  0,  0,  0,  9,  0,
/* F */     0,  0,  0,  0,  0, 11, 12,  0,  0,  0,  0,  0,  0,  0,  0}};

#define STR_DEC_INIT		"\233100;100 B\233m\033|"
#define STR_DEC_LINE_PITCH	"z"
#define STR_DEC_CHAR_PITCH	"w"

#define STR_DEC_ASCII		"\033(B" /* set G0 to ASCII char set */
#define STR_DEC_JIS		"\033(J" /* set G0 to JIS char set */
#define STR_DEC_LETTER		"\2332\"z" /* set writing mode to letter quality */
#define STR_DEC_HIGH_SPEED	"\2330\"z" /* set writing mode to high speed */
#define STR_DEC_SLANT		"\2333m" /* set writing mode to right slant */
#define STR_DEC_HORIZONTAL	"\23310m" /* change to the horizontal mode */
#define STR_DEC_VERTICAL	"\23311m" /* change to the vertical mode */
#define STR_DEC_RESET		"\2330;1!r" /* restore hardware characteristics */
#define STR_DEC_SHIFT_KANJI	"\033|"
#define STR_DEC_SHIFT_KANA	"\033}"

static unsigned char dec_lp_tab[] = { 1,  3,  2,  1,  6,  5,  4};
static unsigned char dec_ll_tab[] = { 6, 12,  8,  6,  4,  3,  2};
static unsigned char dec_cp_tab[] = {   3,    3,   10,    2,    1,    7,   11,    6,    5};
static double        dec_cc_tab[] = {13.3, 13.3, 12.9, 12.0, 10.0,  6.7,  6.4,  6.0,  5.0};

static dec_setup()
{
    if (devil$mod.l_device_type == DEVIL$_LA80)
	devil$txt.v_doublehight_add_line = 0;
    else
	devil$txt.v_doublehight_add_line = 0;

    if (devil$mod.l_line_pitch >= sizeof dec_lp_tab) devil$mod.l_line_pitch = 0;
    if (devil$mod.l_char_pitch >= sizeof dec_cp_tab) devil$mod.l_char_pitch = 0;

    if (devil$mod.l_lines_per_inch <= 0)
	devil$mod.l_lines_per_inch = dec_ll_tab[devil$mod.l_line_pitch];
    else
    if (devil$mod.l_lines_per_inch > 8) {
	devil$mod.l_line_pitch     = 1;
	devil$mod.l_lines_per_inch = 12;
    }
    else
	switch (devil$mod.l_lines_per_inch) {
	case 1:
	case 2:
	    devil$mod.l_line_pitch     = 6;
	    devil$mod.l_lines_per_inch = 2;
	    break;
	case 3:
	    devil$mod.l_line_pitch     = 5;
	    devil$mod.l_lines_per_inch = 3;
	    break;
	case 4:
	    devil$mod.l_line_pitch     = 4;
	    devil$mod.l_lines_per_inch = 4;
	    break;
	case 5:
	case 6:
	    devil$mod.l_line_pitch     = 3;
	    devil$mod.l_lines_per_inch = 6;
	    break;
	case 7:
	case 8:
	    devil$mod.l_line_pitch     = 2;
	    devil$mod.l_lines_per_inch = 8;
	    break;
	}

    devil$mod.l_chars_per_inch = dec_cc_tab[devil$mod.l_char_pitch];
    devil$mod.l_pitch_per_line = 720 / devil$mod.l_lines_per_inch;

    if (devil$mod.l_keisen_width >= 2)
    	devil$mod.l_keisen_width = 2;
    else
    	devil$mod.l_keisen_width = 1;
}

static dec_init()
{
    FORM_STR(STR_DEC_INIT);

    dec_set_line_pitch();
    dec_set_char_pitch();

    /*
    ** set G0 to ASCII char set
    */
    if (devil$mod.v_ascii)
	FORM_STR(STR_DEC_ASCII);
    /*
    ** set G0 to JIS char set
    */
    if (devil$mod.v_jis)
	FORM_STR(STR_DEC_JIS);
    /*
    ** set writing mode to letter quality
    */
    if (devil$mod.v_letter)
	FORM_STR(STR_DEC_LETTER);
    /*
    ** set writing mode to high speed
    */
    if (devil$mod.v_high_speed)
	FORM_STR(STR_DEC_HIGH_SPEED);
    /*
    ** set writing mode to right slant
    */
    if (devil$mod.v_slant)
	FORM_STR(STR_DEC_SLANT);
}

static dec_set_line_pitch()
{
    FORM_STR(STR_CSI);
    devil$buff_decimal(dec_lp_tab[devil$mod.l_line_pitch]);
    FORM_STR(STR_DEC_LINE_PITCH);
}

static dec_set_char_pitch()
{
    FORM_STR(STR_CSI);
    devil$buff_decimal(dec_cp_tab[devil$mod.l_char_pitch]);
    FORM_STR(STR_DEC_CHAR_PITCH);
}

static dec_reset()
{
    FORM_STR(STR_DEC_INIT);
    FORM_STR(STR_DEC_RESET);  /* restore hardware characteristics */
}

static dec_chapter_bar()
{
    int i;

    for (i = 0; i < 80; i ++)
	devil$buff_char('=');
}

static dec_space(n)
int n;
{
    if (devil$txt.l_char_set == CHAR_SET_KANA) FORM_STR(STR_DEC_SHIFT_KANJI);
    devil$text_set_writing_mode(0);
    for (; n > 0; n --) devil$buff_char(' ');
}

static dec_ascii(ch)
unsigned ch;
{
    if (devil$txt.l_char_set == CHAR_SET_KANA) FORM_STR(STR_DEC_SHIFT_KANJI);
    devil$text_set_writing_mode(devil$mod.v_vertical);
    devil$buff_char(ch);
}

static dec_kana(ch)
unsigned ch;
{
    if (devil$txt.l_char_set != CHAR_SET_KANA) FORM_STR(STR_DEC_SHIFT_KANA);
    devil$text_set_writing_mode(devil$mod.v_vertical);
    devil$buff_char(ch);
}

static dec_kanji(ch)
unsigned ch;
{
    if (devil$txt.l_char_set == CHAR_SET_KANA) FORM_STR(STR_DEC_SHIFT_KANJI);
    devil$text_set_writing_mode(devil$mod.v_vertical);
    devil$buff_char(ch);
}

static dec_keisen(ch)
unsigned ch;
{
    if (devil$txt.l_char_set == CHAR_SET_KANA) FORM_STR(STR_DEC_SHIFT_KANJI);
/*+ for jis83 keisen +*/
    if (devil$mod.v_keisen) {
	if (jsy$is_line_drawing(ch))
	    ch = keisen_jis_to_dec(ch);
	ch = keisen_dec_to_jis(ch, devil$mod.l_keisen_width-1);
    }
/*- -*/
    devil$text_set_writing_mode(0);
    devil$buff_char(ch);
}

static dec_horizontal()
{
    FORM_STR(STR_DEC_HORIZONTAL);
}

static dec_vertical()
{
    FORM_STR(STR_DEC_VERTICAL);
}

static dec_set_char_attr(col, old_attr, new_attr)
int      col;
unsigned old_attr;
unsigned new_attr;
{
    int i;

    if ((new_attr & DEVIL$M_OVERSTRIKE) != 0) {
	devil$buff_char(0x08);
	if (devil$txt.l_char_set == CHAR_SET_KANJI)
	    devil$buff_char(0x08);
    }

    if ((old_attr & (DEVIL$M_DOUBLEHIGHT |
		     DEVIL$M_DOUBLEWIDTH)) !=
	(new_attr & (DEVIL$M_DOUBLEHIGHT |
		     DEVIL$M_DOUBLEWIDTH))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_DOUBLEHIGHT) != 0 &&
		(new_attr & DEVIL$M_DOUBLEWIDTH) != 0)
	    FORM_STR("200;200 B");
	else
	if ((new_attr & DEVIL$M_DOUBLEHIGHT) != 0)
	    FORM_STR("200;100 B");
	else
	if ((new_attr & DEVIL$M_DOUBLEWIDTH) != 0)
	    FORM_STR("100;200 B");
	else
	    FORM_STR("100;100 B");
    }

    if ((old_attr & (DEVIL$M_BOLD | DEVIL$M_LIGHT)) !=
	(new_attr & (DEVIL$M_BOLD | DEVIL$M_LIGHT))) {
	if ((old_attr & (DEVIL$M_BOLD)) != (new_attr & (DEVIL$M_BOLD))) {
	    FORM_STR(STR_CSI);
	    if      ((new_attr & DEVIL$M_BOLD)  != 0)	FORM_STR("1m");
	    else if ((new_attr & DEVIL$M_LIGHT) != 0)	FORM_STR("2m");
	    else					FORM_STR("22m");
	}
	else
	if ((old_attr & (DEVIL$M_LIGHT)) != (new_attr & (DEVIL$M_LIGHT))) {
	    FORM_STR(STR_CSI);
	    if      ((new_attr & DEVIL$M_LIGHT)	!= 0)	FORM_STR("2m");
	    else if ((new_attr & DEVIL$M_BOLD)  != 0)	FORM_STR("1m");
	    else					FORM_STR("22m");
	}
    }

    if ((old_attr & (DEVIL$M_ITALIC)) != (new_attr & (DEVIL$M_ITALIC))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_ITALIC) != 0)	FORM_STR("3m");
	else					FORM_STR("23m");
    }

    if ((old_attr & (DEVIL$M_DOUBLEUNDERLINE | DEVIL$M_UNDERLINE)) !=
	(new_attr & (DEVIL$M_DOUBLEUNDERLINE | DEVIL$M_UNDERLINE))) {
	if ((old_attr & (DEVIL$M_DOUBLEUNDERLINE)) !=
	    (new_attr & (DEVIL$M_DOUBLEUNDERLINE))) {
	    FORM_STR(STR_CSI);
	    if      ((new_attr & DEVIL$M_DOUBLEUNDERLINE) != 0)	FORM_STR("21m");
	    else if ((new_attr & DEVIL$M_UNDERLINE)	  != 0) FORM_STR("4m");
	    else						FORM_STR("24m");
	}
	else
	if ((old_attr & (DEVIL$M_UNDERLINE)) !=
	    (new_attr & (DEVIL$M_UNDERLINE))) {
	    FORM_STR(STR_CSI);
	    if      ((new_attr & DEVIL$M_UNDERLINE)	  != 0)	FORM_STR("4m");
	    else if ((new_attr & DEVIL$M_DOUBLEUNDERLINE) != 0)	FORM_STR("21m");
	    else						FORM_STR("24m");
	}
    }

    if ((old_attr & (DEVIL$M_MESH)) != (new_attr & (DEVIL$M_MESH))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_MESH) != 0) FORM_STR("5m");
	else				    FORM_STR("25m");
    }

    if ((old_attr & (DEVIL$M_REVERSE)) != (new_attr & (DEVIL$M_REVERSE))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_REVERSE) != 0)	FORM_STR("7m");
	else					FORM_STR("27m");
    }

    if ((old_attr & (DEVIL$M_VERTICAL)) != (new_attr & (DEVIL$M_VERTICAL))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_VERTICAL) != 0) FORM_STR("11m");
	else					FORM_STR("10m");
    }

/*******************************************************************
    if ((old_attr & (DEVIL$M_SUBSCRIPT |
		     DEVIL$M_SUPERSCRIPT)) !=
	(new_attr & (DEVIL$M_SUBSCRIPT |
		     DEVIL$M_SUPERSCRIPT))) {
	if ((old_attr & DEVIL$M_SUBSCRIPT) != 0) {
	    devil$buff_char(0x8C);
	    for (i = 0; i < col; i ++) devil$buff_char(' ');
	}
	else
	if ((old_attr & DEVIL$M_SUPERSCRIPT) != 0) {
	    devil$buff_char(0x8B);
	    for (i = 0; i < col; i ++) devil$buff_char(' ');
	}

	if ((new_attr & DEVIL$M_SUBSCRIPT) != 0) {
	    devil$buff_char(0x8B);
	    for (i = 0; i < col; i ++) devil$buff_char(' ');
	}
	else
	if ((new_attr & DEVIL$M_SUPERSCRIPT) != 0) {
	    devil$buff_char(0x8C);
	    for (i = 0; i < col; i ++) devil$buff_char(' ');
	}
    }
*******************************************************************/

}

#define STR_NEC_LINE_PITCH	"\033T"
#define STR_NEC_CHAR_PITCH	"\032"
#define STR_NEC_HORIZONTAL	"\033K"
#define STR_NEC_VERTICAL	"\033t"
#define STR_NEC_RESET		"\033H\033T20"

static unsigned char nec_ll_tab[] = {  4,   8,   6,  4,  3};
static unsigned char nec_cp_tab[] = { 'N',  'N',  'E',  'P'};
static double        nec_cc_tab[] = {13.3, 13.3, 12.0, 10.0};

static nec_setup()
{
    devil$txt.v_doublehight_add_line = 0;
    if (devil$mod.l_line_pitch >= sizeof nec_ll_tab) devil$mod.l_line_pitch = 0;
    if (devil$mod.l_char_pitch >= sizeof nec_cp_tab) devil$mod.l_char_pitch = 0;

    if (devil$mod.l_lines_per_inch <= 0)
	devil$mod.l_lines_per_inch = nec_ll_tab[devil$mod.l_line_pitch];
    if (devil$mod.l_image_per_inch <= 0)
	devil$mod.l_image_per_inch = 120;

    devil$mod.l_chars_per_inch = nec_cc_tab[devil$mod.l_char_pitch];
    devil$mod.l_pitch_per_line = 720 / devil$mod.l_lines_per_inch;
}

static nec_init()
{
    nec_set_line_pitch();
    nec_set_char_pitch();
}

static nec_set_line_pitch()
{
    FORM_STR(STR_NEC_LINE_PITCH);
    devil$buff_char('0' + (120 / devil$mod.l_lines_per_inch) / 10);
    devil$buff_char('0' + (120 / devil$mod.l_lines_per_inch) % 10);
}

static nec_set_char_pitch()
{
    FORM_STR(STR_NEC_CHAR_PITCH);
    devil$buff_char(nec_cp_tab[devil$mod.l_char_pitch]);
}

static nec_reset()
{
    FORM_STR(STR_NEC_RESET);
}

static nec_space(n)
int n;
{
    if (n <= 0) return;

    if (n & 1) {
	devil$buff_char( 0 );
	devil$buff_char(' ');
	n --;
    }
    for (; n; n -= 2)
	devil$buff_char(0x2121);
}

static nec_ascii(ch)
unsigned ch;
{
    devil$buff_char(0);
    devil$buff_char(ch);
}

static nec_kanji(ch)
unsigned ch;
{
    devil$buff_char(ch & 0x7f7f);
}

static nec_horizontal()
{
    FORM_STR(STR_NEC_HORIZONTAL);
}

static nec_vertical()
{
    FORM_STR(STR_NEC_VERTICAL);
}

#define STR_LBP_INIT		"\2332 I\233100;100 B\033(J"
#define STR_LBP_FONT_RESET	"\2332 K\23396 C\23380y"
#define STR_LBP_ATTR_RESET	"\233m\2330 S"
#define STR_LBP_SLANT		"\2333m"
#define STR_LBP_PORTRATE	"\2330p"
#define STR_LBP_LANDSCAPE	"\2331p"
#define STR_LBP_PART_PAINT	"\233&z"
#define STR_LBP_FULL_PAINT	"\2332&z"
#define STR_LBP_REGISTER1	"\2331;"
#define STR_LBP_REGISTER0	"\2330;"
#define STR_LBP_REGISTER_END	";0&s"		/* ";1&s" Permanent */
#define STR_LBP_OVERLAY1	"\2331;"
#define STR_LBP_OVERLAY0	"\2330;"
#define STR_LBP_OVERLAY_END	"&w"
#define STR_LBP_PITCH_END	" G"
#define STR_LBP_PITCH_RESET	"\233120;54 G"
#define STR_LBP_HORIZONTAL	"\2330 S"
#define STR_LBP_VERTICAL	"\2331 S"
#define STR_LBP_SHIFT_JIS	"\033(J"
#define STR_LBP_SHIFT_KANJI	"\033$@"
#define STR_LBP_CHAPTER_BAR	"\2339;0;3{"
#define STR_LBP_CHAPTER_BAR_END	"\2335}"
#define STR_LBP_RESET		"\033<"		/* "\033c" Full Reset */

static unsigned char lbp_ll_tab[] = { 6, 12,  8,  6,  4,  3,  2,  7};
static unsigned char lbp_cp_tab[] = {  54,   48,   54,   56,   60,   72};
static double        lbp_cc_tab[] = {13.3, 15.0, 13.3, 12.9, 12.0, 10.0};

static lbp_setup()
{
    devil$txt.v_doublehight_add_line = 0;

    if (devil$mod.l_char_pitch >= sizeof lbp_cp_tab) devil$mod.l_char_pitch = 0;
    if (devil$mod.l_line_pitch >= sizeof lbp_ll_tab) devil$mod.l_line_pitch = 0;
    if (devil$mod.l_lines_per_inch <= 0)
	devil$mod.l_lines_per_inch = lbp_ll_tab[devil$mod.l_line_pitch];

    if (devil$mod.l_keisen_width <= 0 || devil$mod.l_keisen_width > 9)
	devil$mod.l_keisen_width = 1;

    devil$mod.l_chars_per_inch = lbp_cc_tab[devil$mod.l_char_pitch];
    devil$mod.l_pitch_per_line = 720 / devil$mod.l_lines_per_inch;

    if (!devil$mod.v_font_pitch) devil$mod.l_font_pitch = 1500;
    if (!devil$mod.v_font_size)  devil$mod.l_font_size  = 96;
    if (!devil$mod.v_font_type)  devil$mod.l_font_type  = 80;
}

static lbp_init()
{
    if (devil$mod.v_reset_overlay_form)
	FORM_STR(STR_LBP_RESET);

    if (devil$mod.v_register_form) {
	FORM_STR(STR_LBP_REGISTER1);
	devil$buff_char('0' + devil$mod.l_register_form - 1);
	FORM_STR(STR_LBP_REGISTER_END);
    }
    else
    if (devil$mod.v_overlay_form) {
	FORM_STR(STR_LBP_OVERLAY1);
	devil$buff_char('0' + devil$mod.l_overlay_form - 1);
	FORM_STR(STR_LBP_OVERLAY_END);
    }

    if (devil$mod.v_landscape) {
    	if (devil$mod.v_full_paint)
	    FORM_STR(STR_LBP_FULL_PAINT);
	FORM_STR(STR_LBP_LANDSCAPE);
    }

    lbp_init_text();

    /*
    ** set writing mode to right slant
    */
    if (devil$mod.v_slant)
	FORM_STR(STR_LBP_SLANT);
}

static lbp_init_text()
{
    FORM_STR(STR_LBP_INIT);

    lbp_set_font(devil$mod.l_font_pitch, 
		 devil$mod.l_font_size,
		 devil$mod.l_font_type);

    FORM_STR(STR_CSI);
    devil$buff_decimal(720 / devil$mod.l_lines_per_inch);
    FORM_STR(";");
    devil$buff_decimal(lbp_cp_tab[devil$mod.l_char_pitch]);
    FORM_STR(STR_LBP_PITCH_END);
}

static lbp_set_font(pitch, size, type)
int pitch;
int size;
int type;
{
    if (pitch < 0 && size < 0 && type < 0) {
	pitch = 1500; 
	size  = 96;
	type  = 80;
    }
    if (pitch >= 0) {
	FORM_STR(STR_CSI);
	FORM_STR("?");
	devil$buff_decimal(pitch);
	FORM_STR(" K");
	devil$mod.l_font_pitch = pitch;
    }

    if (size >= 0) {
	FORM_STR(STR_CSI);
	devil$buff_decimal(size);
	FORM_STR(" C");
	devil$mod.l_font_size = size;
     }

    if (type >= 0) {
	FORM_STR(STR_CSI);
	devil$buff_decimal(type);
        FORM_STR("y");
	devil$mod.l_font_type = type;
    }
}

static lbp_set_line_pitch()
{
    FORM_STR(STR_CSI);
    devil$buff_decimal(720 / devil$mod.l_lines_per_inch);
    FORM_STR(STR_LBP_PITCH_END);
}

static lbp_set_char_pitch()
{
    FORM_STR(STR_CSI);
    FORM_STR(";");
    devil$buff_decimal(lbp_cp_tab[devil$mod.l_char_pitch]);
    FORM_STR(STR_LBP_PITCH_END);
}

static lbp_skip_pitch(pitch)
int pitch;
{
    while (pitch >= devil$mod.l_pitch_per_line) {
	devil$buff_output();
	pitch -= devil$mod.l_pitch_per_line;
    }
    if (pitch > 0) {
	FORM_STR(STR_CSI);
	FORM_STR("11h");	/* SIZE mode */
	FORM_STR(STR_CSI);
	devil$buff_decimal(pitch);
	FORM_STR("e");
	FORM_STR(STR_CSI);
	FORM_STR("11l");	/* CHARACTER mode */
    }
}

static lbp_reset()
{
    FORM_STR(STR_LBP_INIT);
    FORM_STR(STR_LBP_ATTR_RESET);
    FORM_STR(STR_LBP_FONT_RESET);
    FORM_STR(STR_LBP_PITCH_RESET);

    if (devil$mod.v_landscape) {
    	if (devil$mod.v_full_paint)
	    FORM_STR(STR_LBP_PART_PAINT);
	FORM_STR(STR_LBP_PORTRATE);
    }

    if (devil$mod.v_register_form) {
	FORM_STR(STR_LBP_REGISTER0);
	devil$buff_char('0' + devil$mod.l_register_form - 1);
	FORM_STR(STR_LBP_REGISTER_END);
    }
    else
    if (devil$mod.v_overlay_form) {
	FORM_STR("\f");
	FORM_STR(STR_LBP_OVERLAY0);
	devil$buff_char('0' + devil$mod.l_overlay_form - 1);
	FORM_STR(STR_LBP_OVERLAY_END);
    }

}

devil$change_overlay_page(page)
int page;
{
    if (devil$mod.l_device_type == DEVIL$_LN80 && devil$mod.v_overlay_form) {
	FORM_STR(STR_LBP_OVERLAY0);
	devil$buff_char('0' + devil$mod.l_overlay_form - 1);
	FORM_STR(STR_LBP_OVERLAY_END);
	devil$mod.l_overlay_form = page;
	FORM_STR(STR_LBP_OVERLAY1);
	devil$buff_char('0' + devil$mod.l_overlay_form - 1);
	FORM_STR(STR_LBP_OVERLAY_END);
    }
}

static lbp_chapter_bar()
{
    FORM_STR(STR_LBP_CHAPTER_BAR);
    lbp_space(95);
    FORM_STR(STR_LBP_CHAPTER_BAR_END);
}

static lbp_shift_jis(doublewidth)
int doublewidth;
{
    FORM_STR(STR_LBP_SHIFT_JIS);
    FORM_STR(STR_CSI);
    FORM_STR("?");
    devil$buff_decimal(devil$mod.l_font_pitch);
    FORM_STR(" K");
    FORM_STR(STR_CSI);
    FORM_STR(";");
    if (doublewidth)
	devil$buff_decimal(lbp_cp_tab[devil$mod.l_char_pitch]*2);
    else
	devil$buff_decimal(lbp_cp_tab[devil$mod.l_char_pitch]);
    FORM_STR(STR_LBP_PITCH_END);
}

static lbp_shift_kanji(doublewidth)
int doublewidth;
{
    FORM_STR(STR_LBP_SHIFT_KANJI);
    FORM_STR(STR_CSI);
    FORM_STR("?");
    devil$buff_decimal(devil$mod.l_font_pitch / 2);
    FORM_STR(" K");
    FORM_STR(STR_CSI);
    FORM_STR(";");
    if (doublewidth)
	devil$buff_decimal(lbp_cp_tab[devil$mod.l_char_pitch]*4);
    else
	devil$buff_decimal(lbp_cp_tab[devil$mod.l_char_pitch]*2);
    FORM_STR(STR_LBP_PITCH_END);
}

static lbp_space(n)
int n;
{
    devil$text_set_writing_mode(0);
    lbp_shift_jis(0);
#if	0
    dec_space(n);
#else
    /* V6.0-1:
     Don't put LS3R if current state is LS2R-ed at beginning of line.
     Copied from dec_space (). */
    for (; n > 0; n --) devil$buff_char(' ');
#endif	/* 0 */
}

static lbp_ascii(ch)
unsigned ch;
{
    devil$text_set_writing_mode(0);
    if (devil$txt.l_char_set == CHAR_SET_KANJI)
	lbp_shift_jis(devil$txt.v_doublewidth);
    devil$buff_char(ch);
}

static lbp_kanji(ch)
unsigned ch;
{
	/*
	 * V5.2-000: Disable 2ku->10ku kanji code conversion on Kprint/Lbp
	 */
/*++ M002 ++
    if (ch >= 0xa2a1 && ch <= 0xa2fe) ch = 0xaa00 | (ch & 0xff);
  -- M002 --								V5.1 */
    if(!devil$mod.v_plain_lbp)
	    if (ch >= 0xa2a1 && ch <= 0xa2fe) ch = 0xaa00 | (ch & 0xff);

    if (devil$mod.v_vertical) {
	devil$text_set_writing_mode(devil$mod.v_vertical);
	if (ch >= 0xA1A1 && ch <= 0xA1E1 &&
		vertical_table[0][ch - 0xA1A1] > 0)
	    ch = 0xA8C6 + vertical_table[0][ch - 0xA1A1] - 1;
	else
	if (ch >= 0xA4A1 && ch <= 0xA4EE &&
		vertical_table[1][ch - 0xA4A1] > 0)
	    ch = 0xA8E5 + vertical_table[1][ch - 0xA4A1] - 1;
	else
	if (ch >= 0xA5A1 && ch <= 0xA5F6 &&
		vertical_table[1][ch - 0xA5A1] > 0)
	    ch = 0xA8EF + vertical_table[1][ch - 0xA5A1] - 1;
    }
    if (devil$txt.l_char_set != CHAR_SET_KANJI)
	lbp_shift_kanji(devil$txt.v_doublewidth);
    devil$buff_char(ch & 0x7F7F);
}

static lbp_move_cursor(x, y)
int x;
int y;
{
    if (x != 0) {
	FORM_STR(STR_CSI);
	if (x > 0) {
	    devil$buff_decimal(x);
	    FORM_STR("a");
	}
	else {
	    devil$buff_decimal(- x);
	    FORM_STR("j");
	}
    }
    if (y != 0) {
	FORM_STR(STR_CSI);
	if (y > 0) {
	    devil$buff_decimal(y);
	    FORM_STR("e");
	}
	else {
	    devil$buff_decimal(- y);
	    FORM_STR("k");
	}
    }
}

static lbp_box_from(x, y, pos, line, line_width, line_type, mesh, mesh_type)
int x;
int y;
int pos;
int line;
int line_width;
int line_type;
int mesh;
int mesh_type;
{
    if (devil$txt.l_char_set == CHAR_SET_KANJI)
	lbp_shift_jis(0);

    lbp_move_cursor(x, y);
    if (line)
	lbp_box_line_from(line_width, line_type, pos);
    if (mesh)
	lbp_box_mesh_from(mesh_type, pos);
    lbp_move_cursor(- x, - y);

    if (devil$txt.l_char_set != CHAR_SET_KANJI)
	lbp_shift_jis(devil$txt.v_doublewidth);
    else
	lbp_shift_kanji(devil$txt.v_doublewidth);
}

static lbp_box_to(x, y, pos, line, mesh)
int x;
int y;
int pos;
int line;
int mesh;
{
    if (devil$txt.l_char_set == CHAR_SET_KANJI)
	lbp_shift_jis(0);

    lbp_move_cursor(x, y);
    if (line)
	lbp_box_line_to(pos);
    if (mesh)
	lbp_box_mesh_to(pos);
    lbp_move_cursor(- x, - y);

    if (devil$txt.l_char_set != CHAR_SET_KANJI)
	lbp_shift_jis(devil$txt.v_doublewidth);
    else
	lbp_shift_kanji(devil$txt.v_doublewidth);
}

static lbp_box_line_from(width, type, pos)
int width;
int type;
int pos;
{
    FORM_STR(STR_CSI);
    devil$buff_decimal(width);
    FORM_STR(";");
    devil$buff_decimal(type);
    FORM_STR(";");
    devil$buff_decimal(pos);
    FORM_STR("{");
}

static lbp_box_mesh_from(type, pos)
int type;
int pos;
{
    FORM_STR(STR_CSI);
    devil$buff_decimal(type);
    FORM_STR(";");
    devil$buff_decimal(pos);
    FORM_STR("s");
}

static lbp_box_line_to(pos)
int pos;
{
    FORM_STR(STR_CSI);
    devil$buff_decimal(pos);
    FORM_STR("}");
}

static lbp_box_mesh_to(pos)
int pos;
{
    FORM_STR(STR_CSI);
    devil$buff_decimal(pos);
    FORM_STR("r");
}

static lbp_keisen_from(n)
int n;
{
    lbp_box_line_from(devil$mod.l_keisen_width, 
		      devil$mod.l_keisen_line, n);
}

static lbp_keisen_to(n)
int n;
{
    lbp_box_line_to(n);
}

static lbp_keisen(ch)
unsigned ch;
{
    static unsigned char rule[] = {
	    0x25,   0x24,   0x23,   0x26,   0x2b,
	    0x21,   0x27,   0x29,   0x2a,   0x28,   0x22
	};

    if (devil$txt.l_char_set != CHAR_SET_KANJI)
	lbp_shift_kanji(devil$txt.v_doublewidth);
    devil$text_set_writing_mode(0);
    if (devil$mod.v_keisen) {
/*++ M002
!/*+ for jis83 keisen +*
!	if (jsy$is_line_drawing_dec(ch))
!	    ch = keisen_dec_to_jis(ch, devil$mod.l_keisen_width-1);
!/*- -*
--*++*/
	if (jsy$is_line_drawing(ch)) ch = keisen_jis_to_dec(ch);
/*-- M002 --*/

	switch(ch - KEISEN_DEC_BEG) {
	case  0:
	    lbp_keisen_from(3); lbp_keisen_to(4);
	    lbp_keisen_from(4); lbp_keisen_to(1);
	    break;
	case  1:
	    lbp_keisen_from(3); lbp_keisen_to(4);
	    lbp_keisen_from(4); lbp_keisen_to(7);
	    break;
	case  2:
	    lbp_keisen_from(7); lbp_keisen_to(4);
	    lbp_keisen_from(4); lbp_keisen_to(5);
	    break;
	case  3:
	    lbp_keisen_from(1); lbp_keisen_to(4);
	    lbp_keisen_from(4); lbp_keisen_to(5);
	    break;
	case  4:
	    lbp_keisen_from(1); lbp_keisen_to(7);
	    lbp_keisen_from(3); lbp_keisen_to(5);
	    break;
	case  5:
	    lbp_keisen_from(3); lbp_keisen_to(5);
	    break;
	case  6:
	    lbp_keisen_from(1); lbp_keisen_to(7);
	    lbp_keisen_from(4); lbp_keisen_to(5);
	    break;
	case  7:
	    lbp_keisen_from(1); lbp_keisen_to(7);
	    lbp_keisen_from(3); lbp_keisen_to(4);
	    break;
	case  8:
	    lbp_keisen_from(1); lbp_keisen_to(4);
	    lbp_keisen_from(3); lbp_keisen_to(5);
	    break;
	case  9:
	    lbp_keisen_from(4); lbp_keisen_to(7);
	    lbp_keisen_from(3); lbp_keisen_to(5);
	    break;
	case 10:
	    lbp_keisen_from(1); lbp_keisen_to(7);
	    break;
	}
	devil$buff_char(0x2121);
    }
/*++ M002
!    else
!	devil$buff_char(0x2800 | rule[ch - KEISEN_DEC_BEG]);
--*++*/
    else {
	if (kpri$is_line_drawing_dec(ch)) ch = keisen_dec_to_jis(ch,0);
    	ch &= 0x7f7f;
	devil$buff_char(ch);
    }
/*-- M002 --*/
}

static lbp_horizontal()
{
    FORM_STR(STR_LBP_HORIZONTAL);
    if (devil$txt.l_char_set != CHAR_SET_KANJI)
	lbp_shift_jis(devil$txt.v_doublewidth);
    else
	lbp_shift_kanji(devil$txt.v_doublewidth);
}

static lbp_vertical()
{
    FORM_STR(STR_LBP_VERTICAL);
    if (devil$txt.l_char_set != CHAR_SET_KANJI)
	lbp_shift_jis(devil$txt.v_doublewidth);
    else
	lbp_shift_kanji(devil$txt.v_doublewidth);
}

static lbp_set_char_attr(col, old_attr, new_attr)
int      col;
unsigned old_attr;
unsigned new_attr;
{
    if ((new_attr & DEVIL$M_OVERSTRIKE) != 0)
	devil$buff_char(0x08);

    if ((old_attr & (DEVIL$M_DOUBLEHIGHT |
		     DEVIL$M_DOUBLEWIDTH)) !=
	(new_attr & (DEVIL$M_DOUBLEHIGHT |
		     DEVIL$M_DOUBLEWIDTH))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_DOUBLEHIGHT) != 0 &&
		(new_attr & DEVIL$M_DOUBLEWIDTH) != 0)
	    FORM_STR("200;200 B");
	else
	if ((new_attr & DEVIL$M_DOUBLEHIGHT) != 0)
	    FORM_STR("200;100 B");
	else
	if ((new_attr & DEVIL$M_DOUBLEWIDTH) != 0)
	    FORM_STR("100;200 B");
	else
	    FORM_STR("100;100 B");

	if (devil$txt.l_char_set != CHAR_SET_KANJI)
	    lbp_shift_jis((new_attr & DEVIL$M_DOUBLEWIDTH) != 0);
	else
	    lbp_shift_kanji((new_attr & DEVIL$M_DOUBLEWIDTH) != 0);
    }

    if ((old_attr & (DEVIL$M_BOLD | DEVIL$M_LIGHT)) !=
	(new_attr & (DEVIL$M_BOLD | DEVIL$M_LIGHT))) {
	if ((old_attr & (DEVIL$M_BOLD)) != (new_attr & (DEVIL$M_BOLD))) {
	    FORM_STR(STR_CSI);
	    if      ((new_attr & DEVIL$M_BOLD)  != 0)	FORM_STR("1m");
	    else if ((new_attr & DEVIL$M_LIGHT) != 0)	FORM_STR("2m");
	    else					FORM_STR("22m");
	}
	else
	if ((old_attr & (DEVIL$M_LIGHT)) != (new_attr & (DEVIL$M_LIGHT))) {
	    FORM_STR(STR_CSI);
	    if      ((new_attr & DEVIL$M_LIGHT)	!= 0)	FORM_STR("2m");
	    else if ((new_attr & DEVIL$M_BOLD)  != 0)	FORM_STR("1m");
	    else					FORM_STR("22m");
	}
    }

    if ((old_attr & (DEVIL$M_ITALIC)) != (new_attr & (DEVIL$M_ITALIC))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_ITALIC) != 0)	FORM_STR("3m");
	else					FORM_STR("23m");
    }

    if ((old_attr & (DEVIL$M_UNDERLINE)) != (new_attr & (DEVIL$M_UNDERLINE))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_UNDERLINE) != 0)    FORM_STR("4m");
	else					    FORM_STR("24m");
    }

    if ((old_attr & (DEVIL$M_MESH)) != (new_attr & (DEVIL$M_MESH))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_MESH) != 0)	FORM_STR("5m");
	else					FORM_STR("25m");
    }

    if ((old_attr & (DEVIL$M_REVERSE)) != (new_attr & (DEVIL$M_REVERSE))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_REVERSE) != 0)	FORM_STR("7m");
	else					FORM_STR("27m");
    }

    if ((old_attr & (DEVIL$M_WHITE)) != (new_attr & (DEVIL$M_WHITE))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_WHITE) != 0)	FORM_STR("8m");
	else					FORM_STR("28m");
    }

    if ((old_attr & DEVIL$M_VERTICAL) != (new_attr & DEVIL$M_VERTICAL)) {
	if ((new_attr & DEVIL$M_VERTICAL) != 0) lbp_vertical();
	else					lbp_horizontal();
    }

    if ((old_attr & (DEVIL$M_SUBSCRIPT | DEVIL$M_SUPERSCRIPT)) !=
	(new_attr & (DEVIL$M_SUBSCRIPT | DEVIL$M_SUPERSCRIPT))) {
	if      ((old_attr & DEVIL$M_SUBSCRIPT)   != 0) devil$buff_char(0x8C);
	else if ((old_attr & DEVIL$M_SUPERSCRIPT) != 0) devil$buff_char(0x8B);

	if ((old_attr & (DEVIL$M_SUBSCRIPT)) !=
	    (new_attr & (DEVIL$M_SUBSCRIPT))) {
	    if      ((new_attr & DEVIL$M_SUBSCRIPT)   != 0) devil$buff_char(0x8B);
	    else if ((new_attr & DEVIL$M_SUPERSCRIPT) != 0) devil$buff_char(0x8C);
	}
	else
	if ((old_attr & (DEVIL$M_SUPERSCRIPT)) !=
	    (new_attr & (DEVIL$M_SUPERSCRIPT))) {
	    if      ((new_attr & DEVIL$M_SUPERSCRIPT) != 0) devil$buff_char(0x8C);
	    else if ((new_attr & DEVIL$M_SUBSCRIPT)   != 0) devil$buff_char(0x8B);
	}
    }
}

#define STR_LN03_INIT		"\033(J\033+\"0\033|\2332 I"
#define STR_LN03_FONT_RESET	"\233100 C\233100;66 B"
#define STR_LN03_ATTR_RESET	"\233m\2330 S"
#define STR_LN03_REGISTER	"\2200;0;1\"p"
#define STR_LN03_OVERLAY	"\2331'v"
#define STR_LN03_OVERLAY_END	"\2330'v"
#define STR_LN03_PITCH_END	" G"
#define STR_LN03_PITCH_RESET	"\233120;55 G"
#define STR_LN03_FONT		"\2202;10}DBULTN1\234\23310m\233100 C"
#define STR_LN03_HORIZONTAL	"\233?75l"
#define STR_LN03_VERTICAL	"\233?75h"
#define STR_LN03_ASCII		"\033(B"
#define STR_LN03_JIS		"\033(J"
#define STR_LN03_SHIFT_KANJI	"\033|"
#define STR_LN03_SHIFT_KANA	"\033}"
#define STR_LN03_RESET		"\033(B\033+\"0\033|\2332 I\233\233?20 J\233?75l"

static unsigned char ln03_size_a4[]     = { 6, 7};
static unsigned char ln03_size_letter[] = {20, 21};

static unsigned char ln03_ll_tab[] = { 6, 12,  8,  6,  4,  3,  2,  7};
static unsigned char ln03_cp_tab[] = {   55,    48,    55,    60,    72,    86};
static double        ln03_cc_tab[] = {13.04, 15.00, 13.04, 12.00, 10.00,  8.33};
static int	form_converting, initial_flag, change_required;

static ln03_setup()
{
    if (devil$mod.l_keisen_width >= 2)
    	devil$mod.l_keisen_width = 2;
    else
    	devil$mod.l_keisen_width = 1;

    initial_flag = ON;
    form_converting = change_required = OFF;
    devil$txt.v_doublehight_add_line = 0;

    if (devil$mod.l_char_pitch >= sizeof ln03_cp_tab) devil$mod.l_char_pitch = 0;
    if (devil$mod.l_line_pitch >= sizeof ln03_ll_tab) devil$mod.l_line_pitch = 0;
    if (devil$mod.l_lines_per_inch <= 0)
	devil$mod.l_lines_per_inch = ln03_ll_tab[devil$mod.l_line_pitch];

    devil$mod.l_chars_per_inch = ln03_cc_tab[devil$mod.l_char_pitch];
    devil$mod.l_pitch_per_line = 300 / devil$mod.l_lines_per_inch;

    if (!devil$mod.v_font_pitch) devil$mod.l_font_pitch = 1000;
    if (!devil$mod.v_font_size)  devil$mod.l_font_size  = 100;
    if (!devil$mod.v_font_type)  devil$mod.l_font_type  = 66;
}

static ln03_init()
{
/*
    if (devil$mod.v_reset_overlay_form) {
	FORM_STR(STR_LN03_RESET);
	FORM_STR(STR_LN03_FONT_RESET);
    }
*/
    if (devil$mod.v_register_form) {
	FORM_STR(STR_LN03_REGISTER);
	devil$buff_output();
	form_converting = ON;
	return;
    }
    ln03_init_text();
    if (devil$mod.v_ascii)
	FORM_STR(STR_LN03_ASCII);
    if (devil$mod.v_jis)
	FORM_STR(STR_LN03_JIS);
    devil$buff_char(CR);
}

static ln03_init_text()
{
    FORM_STR(STR_LN03_INIT);
    FORM_STR(STR_LN03_FONT);
    ln03_set_font(devil$mod.l_font_pitch, 
		 devil$mod.l_font_size,
		 devil$mod.l_font_type);
}

static ln03_set_font(pitch, size, type)
int pitch;
int size;
int type;
{
    if (pitch < 0 && size < 0 && type < 0) {
	pitch = 1000;
	size  = 100;
	type  = 66;
    }

    devil$mod.l_font_pitch = pitch;
    pitch /= 100;
    FORM_STR(STR_CSI);
    switch (pitch) {
	case 12:
	    devil$buff_decimal(1);
	    break;
	case 15:
	    devil$buff_decimal(2);
	    break;
	case  6:
	    devil$buff_decimal(3);
	    break;
	default:
	    devil$buff_decimal(0);
    }
    FORM_STR(" K");

    devil$mod.l_font_size = size;
    FORM_STR(STR_CSI);
    devil$buff_decimal((size >= 0) ? size : 100);
    devil$buff_char(';');
    switch (type) {
	case 3:
	    devil$buff_decimal(96);
	    break;
	case 2:
	    devil$buff_decimal(83);
	    break;
	default:
	    devil$buff_decimal(66);
    }
    FORM_STR(" B");
}

static ln03_set_pitch()
{
    FORM_STR(STR_CSI);
    devil$buff_decimal(720 / devil$mod.l_lines_per_inch);
    FORM_STR(";");
    devil$buff_decimal(ln03_cp_tab[devil$mod.l_char_pitch]);
    FORM_STR(STR_LN03_PITCH_END);
}

static ln03_reset()
{
    if (devil$mod.v_register_form) {
	form_converting = OFF;
	FORM_STR("\234");
	return;
    }
    FORM_STR(STR_LN03_INIT);
    FORM_STR(STR_LN03_ATTR_RESET);
    FORM_STR(STR_LN03_FONT_RESET);
    FORM_STR(STR_LN03_PITCH_RESET);
    devil$mod.v_landscape = devil$mod.v_vertical = 0;
    devil$mod.l_line_pitch = devil$mod.l_char_pitch = 0;	/* default */
    /*
     * V5.4-2:
     *	Restore default line pitch value
     */
    devil$mod.l_lines_per_inch = ln03_ll_tab[devil$mod.l_line_pitch];
    ln03_horizontal();

    if (devil$mod.v_overlay_form)
	FORM_STR(STR_LN03_OVERLAY_END);
}

static ln03_chapter_bar()
{
    dec_chapter_bar();
}

static ln03_space(n)
int n;
{
    if (devil$txt.l_char_set == CHAR_SET_KANA) FORM_STR(STR_DEC_SHIFT_KANJI);
    for (; n > 0; n --) devil$buff_char(' ');
}

static ln03_ascii(ch)
unsigned ch;
{
    if (devil$txt.l_char_set == CHAR_SET_KANA) FORM_STR(STR_LN03_SHIFT_KANJI);
    devil$buff_char(ch);
}

static ln03_kana(ch)
unsigned ch;
{
    if (devil$txt.l_char_set != CHAR_SET_KANA) FORM_STR(STR_LN03_SHIFT_KANA);
    devil$buff_char(ch);
}

static ln03_kanji(ch)
unsigned ch;
{
    if (devil$txt.l_char_set == CHAR_SET_KANA) FORM_STR(STR_LN03_SHIFT_KANJI);
    devil$buff_char(ch);
}

static ln03_keisen(ch)
unsigned ch;
{
/*++ M002
!/*+ for jis83 keisen +*
!    if (devil$mod.v_keisen)
!	if (jsy$is_line_drawing_dec(ch))
!	    ch = keisen_dec_to_jis(ch, devil$mod.l_keisen_width-1);
!/*- -*
--*++*/
    if (devil$mod.v_keisen) {
	if (kpri$is_line_drawing_dec(ch)) {
	    ch = keisen_dec_to_jis(ch, devil$mod.l_keisen_width-1);
    	}
	else if (jsy$is_line_drawing(ch)) {
	    ch = keisen_jis_to_dec(ch);
	    ch = keisen_dec_to_jis(ch, devil$mod.l_keisen_width-1);
    	}
    }
/*-- M002 --*/
    ln03_kanji(ch);
}

static ln03_horizontal()
{
    if (devil$mod.v_register_form)
	return;
    FORM_STR(STR_LN03_HORIZONTAL);
    ln03_set_size();
}

static ln03_vertical()
{
    if (devil$mod.v_register_form)
	return;
    FORM_STR(STR_LN03_VERTICAL);
    ln03_set_size();
}

static ln03_set_size()
{
    FORM_STR(STR_CSI);
    switch(devil$mod.l_size_type) {
	case DEVIL$V_SIZE_A4:
	    devil$buff_decimal(ln03_size_a4[(devil$mod.v_landscape ^ devil$mod.v_vertical) != 0]);
	    break;
	case DEVIL$V_SIZE_LETTER:
	    devil$buff_char('?');
	    devil$buff_decimal(ln03_size_letter[(devil$mod.v_landscape ^ devil$mod.v_vertical) != 0]);
	    break;
    }
    FORM_STR(" J");
    ln03_set_pitch();

/*++ ++*/
    FORM_STR(STR_CSI);
    FORM_STR("1;167s");
/*-- --*/

    devil$buff_char(CR);
}

static ln03_set_char_attr(col, old_attr, new_attr)
int      col;
unsigned old_attr;
unsigned new_attr;
{
    if ((new_attr & DEVIL$M_OVERSTRIKE) != 0)
	devil$buff_char(0x08);

    if ((old_attr & (DEVIL$M_DOUBLEHIGHT |
		     DEVIL$M_DOUBLEWIDTH)) !=
	(new_attr & (DEVIL$M_DOUBLEHIGHT |
		     DEVIL$M_DOUBLEWIDTH))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_DOUBLEHIGHT) != 0)
	    FORM_STR("200;");
	else
	    FORM_STR("100;");
	if ((new_attr & DEVIL$M_DOUBLEWIDTH) != 0)
	    FORM_STR("132 B");
	else
	    FORM_STR("66 B");
    }

    if ((old_attr & (DEVIL$M_BOLD | DEVIL$M_LIGHT)) !=
	(new_attr & (DEVIL$M_BOLD | DEVIL$M_LIGHT))) {
	if ((old_attr & (DEVIL$M_BOLD)) != (new_attr & (DEVIL$M_BOLD))) {
	    FORM_STR(STR_CSI);
	    if      ((new_attr & DEVIL$M_BOLD)  != 0)	FORM_STR("1m");
	    else if ((new_attr & DEVIL$M_LIGHT) != 0)	FORM_STR("2m");
	    else					FORM_STR("22m");
	}
	else
	if ((old_attr & (DEVIL$M_LIGHT)) != (new_attr & (DEVIL$M_LIGHT))) {
	    FORM_STR(STR_CSI);
	    if      ((new_attr & DEVIL$M_LIGHT)	!= 0)	FORM_STR("2m");
	    else if ((new_attr & DEVIL$M_BOLD)  != 0)	FORM_STR("1m");
	    else					FORM_STR("22m");
	}
    }

    if ((old_attr & (DEVIL$M_ITALIC)) != (new_attr & (DEVIL$M_ITALIC))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_ITALIC) != 0)	FORM_STR("3m");
	else					FORM_STR("23m");
    }

    if ((old_attr & (DEVIL$M_DOUBLEUNDERLINE | DEVIL$M_UNDERLINE)) !=
	(new_attr & (DEVIL$M_DOUBLEUNDERLINE | DEVIL$M_UNDERLINE))) {
	if ((old_attr & (DEVIL$M_DOUBLEUNDERLINE)) !=
	    (new_attr & (DEVIL$M_DOUBLEUNDERLINE))) {
	    FORM_STR(STR_CSI);
	    if      ((new_attr & DEVIL$M_DOUBLEUNDERLINE) != 0)	FORM_STR("21m");
	    else if ((new_attr & DEVIL$M_UNDERLINE)	  != 0) FORM_STR("4m");
	    else						FORM_STR("24m");
	}
	else
	if ((old_attr & (DEVIL$M_UNDERLINE)) !=
	    (new_attr & (DEVIL$M_UNDERLINE))) {
	    FORM_STR(STR_CSI);
	    if      ((new_attr & DEVIL$M_UNDERLINE)	  != 0)	FORM_STR("4m");
	    else if ((new_attr & DEVIL$M_DOUBLEUNDERLINE) != 0)	FORM_STR("21m");
	    else						FORM_STR("24m");
	}
    }

    if ((old_attr & (DEVIL$M_REVERSE)) != (new_attr & (DEVIL$M_REVERSE))) {
	FORM_STR(STR_CSI);
	if ((new_attr & DEVIL$M_REVERSE) != 0)	FORM_STR("7m");
	else					FORM_STR("27m");
    }

    if ((old_attr & DEVIL$M_VERTICAL) != (new_attr & DEVIL$M_VERTICAL)) {
 	change_required = ON;
    }
}

devil$buff_convert(buf, len)
unsigned char	buf[];
int		*len;
{
unsigned char	wrk[4000];
int		wl = *len, i;

    if (devil$mod.l_device_type != DEVIL$_LN03 || form_converting != ON)
	return;
    jsy$ch_move(*len, buf, wrk);
    wrk[wl++] = CR;
    wrk[wl++] = LF;
    while (wl % 3)
	wrk[wl++] = CR;
    for (*len=0,i=0; i<wl; i+=3) {
	buf[(*len)++] = 			    ((wrk[i]   >> 2) & 0x3f)  + 0x3f;
	buf[(*len)++] = (((wrk[i]   << 4) & 0x30) | ((wrk[i+1] >> 4) & 0x0f)) + 0x3f;
	buf[(*len)++] = (((wrk[i+1] << 2) & 0x3c) | ((wrk[i+2] >> 6) & 0x03)) + 0x3f;
	buf[(*len)++] =   (wrk[i+2]       & 0x3f)			      + 0x3f;
    }
}

devil$buff_form_feed()
{
    if (devil$mod.l_device_type != DEVIL$_LN03)
	return;
    if (initial_flag == ON) {
	if (devil$mod.v_overlay_form)
	    FORM_STR(STR_LN03_OVERLAY);
	if (devil$mod.v_vertical) {
	    initial_flag = OFF;
	    devil$text_put_writing_mode(devil$mod.v_vertical);
	}
    }
    else if (change_required == ON) {
	change_required = OFF;
	devil$text_set_writing_mode(devil$mod.v_vertical);
    }
}

#define STR_BRO_LINE_PITCH	"\033J"
#define STR_BRO_CHAR_PITCH	"\034-\034$"
#define STR_BRO_HORIZONTAL	"\034K"
#define STR_BRO_VERTICAL	"\034J"
#define STR_BRO_HANKAKU		"\034^"
#define STR_BRO_RESET		"\034.\033J"

static unsigned char bro_ll_tab[] = {  4,   8,   6,  4,  3};
static unsigned char bro_cp_tab[] = { 108,  108,  144};
static double        bro_cc_tab[] = {13.3, 13.3, 10.0};

static bro_setup()
{
    devil$txt.v_doublehight_add_line = 0;
    if (devil$mod.l_line_pitch >= sizeof bro_ll_tab) devil$mod.l_line_pitch = 0;
    if (devil$mod.l_char_pitch >= sizeof bro_cp_tab) devil$mod.l_char_pitch = 0;

    if (devil$mod.l_lines_per_inch <= 0)
	devil$mod.l_lines_per_inch = bro_ll_tab[devil$mod.l_line_pitch];
    if (devil$mod.l_image_per_inch <= 0)
	devil$mod.l_image_per_inch = 120;

    devil$mod.l_chars_per_inch = bro_cc_tab[devil$mod.l_char_pitch];
    devil$mod.l_pitch_per_line = 720 / devil$mod.l_lines_per_inch;
}

static bro_init()
{
    bro_set_line_pitch();
    bro_set_char_pitch();
}

static bro_set_line_pitch()
{
    FORM_STR(STR_BRO_LINE_PITCH);
    devil$buff_char(120 / devil$mod.l_lines_per_inch);
}

static bro_set_char_pitch()
{
    FORM_STR(STR_BRO_CHAR_PITCH);
    devil$buff_char(bro_cp_tab[devil$mod.l_char_pitch]);
}

static bro_reset()
{
    FORM_STR(STR_BRO_RESET);
    devil$buff_char(0);
}

static bro_space(n)
int n;
{
    if (n <= 0) return;

    if (n & 1) {
	FORM_STR(STR_BRO_HANKAKU);
	devil$buff_char(0x2121);
	n --;
    }
    for (; n; n -= 2)
	devil$buff_char(0x2121);
}

static bro_ascii(ch)
unsigned ch;
{
    FORM_STR(STR_BRO_HANKAKU);
    devil$buff_char(0x7f7f & jsy$chg_rom_full(ch));
}

static bro_kana(ch)
unsigned ch;
{
    FORM_STR(STR_BRO_HANKAKU);
    devil$buff_char(0x7f7f & jsy$chg_kana_full(ch, 1));
}

static bro_kanji(ch)
unsigned ch;
{
    devil$buff_char(ch & 0x7f7f);
}

static bro_keisen(ch)
unsigned ch;
{
    static unsigned char rule[] = {
	    0x64,   0x63,   0x62,   0x65,   0x6a,
	    0x60,   0x66,   0x68,   0x69,   0x67,   0x61
	};

/*+ for jis83 keisen +*/
    if (jsy$is_line_drawing(ch))
	ch = keisen_jis_to_dec(ch);
/*- -*/
    devil$buff_char(0x2800 | rule[ch - KEISEN_DEC_BEG]);
}

static bro_horizontal()
{
    FORM_STR(STR_BRO_HORIZONTAL);
}

static bro_vertical()
{
    FORM_STR(STR_BRO_VERTICAL);
}

static text_setup() 
{
    devil$txt.v_doublehight_add_line = 0;
    devil$mod.l_line_pitch = 0;
    devil$mod.l_char_pitch = 0;

    if (devil$mod.l_lines_per_inch <= 0)
    	devil$mod.l_lines_per_inch = 6;

    devil$mod.l_image_per_inch = 180;
    devil$mod.l_chars_per_inch = 13.3;
    devil$mod.l_pitch_per_line = 720 / devil$mod.l_lines_per_inch;
}

static text_skip_pitch(pitch)
int pitch;
{
    while (pitch > 0) {
	devil$buff_output();
	pitch -= devil$mod.l_pitch_per_line;
    }
}

static text_space(n)
int n;
{
    if (devil$txt.l_char_set == CHAR_SET_KANA) FORM_STR(STR_DEC_SHIFT_KANJI);
    for (; n > 0; n --) devil$buff_char(' ');
}

static text_ascii(ch)
unsigned ch;
{
    if (devil$txt.l_char_set == CHAR_SET_KANA) FORM_STR(STR_DEC_SHIFT_KANJI);
    if (devil$txt.v_doublewidth) ch = jsy$chg_rom_full(ch);
    devil$buff_char(ch);
}

static text_kana(ch)
unsigned ch;
{
    if (devil$txt.l_char_set != CHAR_SET_KANA) FORM_STR(STR_DEC_SHIFT_KANA);
    devil$buff_char(ch);
    if (devil$txt.v_doublewidth) devil$buff_char(' ');
}

static text_kanji(ch)
unsigned ch;
{
    if (devil$txt.l_char_set == CHAR_SET_KANA) FORM_STR(STR_DEC_SHIFT_KANJI);
    if (devil$txt.v_doublewidth) devil$buff_char(' ');
    devil$buff_char(ch);
    if (devil$txt.v_doublewidth) devil$buff_char(' ');
}

static text_keisen(ch)
unsigned ch;
{
    if (devil$txt.l_char_set == CHAR_SET_KANA) FORM_STR(STR_DEC_SHIFT_KANJI);
    if (devil$txt.v_doublewidth) devil$buff_char(' ');
    devil$buff_char(ch);
    if (devil$txt.v_doublewidth) devil$buff_char(' ');
}

static text_set_char_attr(col, old_attr, new_attr)
int      col;
unsigned old_attr;
unsigned new_attr;
{
    if ((new_attr & DEVIL$M_OVERSTRIKE) != 0) {
	devil$buff_char(0x08);
	if (devil$txt.l_char_set == CHAR_SET_KANJI)
	    devil$buff_char(0x08);
    }

    if ((old_attr & (DEVIL$M_BOLD      |
		     DEVIL$M_UNDERLINE |
		     DEVIL$M_MESH      |
		     DEVIL$M_REVERSE)) !=
	(new_attr & (DEVIL$M_BOLD      |
		     DEVIL$M_MESH      |
		     DEVIL$M_UNDERLINE |
		     DEVIL$M_REVERSE))) {
	FORM_STR("\033[m");
	if (new_attr & DEVIL$M_BOLD)      FORM_STR("\033[1m");
	if (new_attr & DEVIL$M_UNDERLINE) FORM_STR("\033[4m");
	if (new_attr & DEVIL$M_MESH)      FORM_STR("\033[5m");
	if (new_attr & DEVIL$M_REVERSE)   FORM_STR("\033[7m");
    }
}

devil$text_put_string(col, buf, len)
int            col;
unsigned char *buf;
int            len;
{
    unsigned char *last;
    unsigned chr;
    unsigned kpri$ch_gnext_shift(); /* nee jsy$ch_gnext_shift(); */

    last = buf + len;
    while ((chr = kpri$ch_gnext_shift(&buf, last, &devil$txt.l_input_kana_shift))
		!= JSY$C_EOS) {
	col = devil$text_put_char(col, chr);
    }
    return col;
}

devil$text_put_char(col, chr)
int      col;
unsigned chr;
{
    int      nex;

    if (chr >= 0x20 && chr <= 0x7E) {
	(*func.put_ascii)(chr);
	devil$txt.l_char_set = CHAR_SET_ASCII;
	if (!devil$txt.v_overstrike) {
	    col ++;
	    if (devil$txt.v_doublewidth) col ++;
	}
    }
    else
    if (chr == TAB) {
	nex = col + 8 - (col & 7);
	(*func.put_space)(nex - col);
	devil$txt.l_char_set = CHAR_SET_ASCII;
	col = nex;
    }
    else
    if (chr < 0x20 || chr >= 0x7F && chr <= 0xA0 || chr == 0xFF) {
	(*func.put_ascii)(chr);
	devil$txt.l_char_set = CHAR_SET_ASCII;
    }
    else
    if (chr >= 0xA1 && chr <= 0xFE) {
	(*func.put_kana)(chr);
	devil$txt.l_char_set = CHAR_SET_KANA;
	if (!devil$txt.v_overstrike) {
	    col ++;
	    if (devil$txt.v_doublewidth) col ++;
	}
    }
    else
    if (jsy$is_line_drawing(chr) || kpri$is_line_drawing_dec(chr)) {
	(*func.put_keisen)(chr);
	devil$txt.l_char_set = CHAR_SET_KANJI;
	if (!devil$txt.v_overstrike) {
	    col += 2;
	    if (devil$txt.v_doublewidth) col += 2;
	}
    }
    else {
	(*func.put_kanji)(chr);
	devil$txt.l_char_set = CHAR_SET_KANJI;
	if (!devil$txt.v_overstrike) {
	    col += 2;
	    if (devil$txt.v_doublewidth) col += 2;
	}
    }
    return col;
}

devil$text_decimal(num)
int num;
{
    char buf[10];
    int  len;

    len = decimal_str(num, buf, sizeof buf);

    devil$text_put_string(0, buf, len);

    return len;
}

devil$buff_decimal(num)
int num;
{
    char buf[10];
    int  len;

    len = decimal_str(num, buf, sizeof buf);

    devil$buff_string(buf, len);

    return len;
}

static decimal_str(num, buf, siz)
int  num;
char buf[];
int  siz;
{
   int  i;

   i = siz;
   do {
      buf[-- i] = '0' + num % 10;
      num /= 10;
   } while (num > 0 && i > 0);
   jsy$ch_move(siz - i, &buf[i], buf);

   return siz - i;
}

/*++ JIS-83 keisen handle ++*/
static	keisen_dec_to_jis(ch, sz)
unsigned	ch;
int		sz;
{
static unsigned	char rule[11][2] = 
	{{ 0xA5, 0xB0 },	/* FE21 ■! вк ие or и░ */
	 { 0xA4, 0xAF },	/* FE22 ■" вк ид or ип */
	 { 0xA3, 0xAE },	/* FE23 ■# вк иг or ио */
	 { 0xA6, 0xB1 },	/* FE24 ■$ вк иж or и▒ */
	 { 0xAB, 0xB6 },	/* FE25 ■% вк ил or и╢ */
	 { 0xA1, 0xAC },	/* FE26 ■& вк иб or им */
	 { 0xA7, 0xB2 },	/* FE27 ■' вк из or и▓ */
	 { 0xA9, 0xB4 },	/* FE28 ■( вк ий or и┤ */
	 { 0xAA, 0xB5 },	/* FE29 ■) вк ик or и╡ */
	 { 0xA8, 0xB3 },	/* FE2A ■* вк ии or и│ */
	 { 0xA2, 0xAD } };	/* FE2B ■+ вк ив or ин */

    return (0xA800 | rule[ch - KEISEN_DEC_BEG][sz]);
}

/*++ JIS-83 keisen handle ++*/
static	keisen_jis_to_dec(ch)
unsigned	ch;
{
static	unsigned char	rule[] = 
 	{ 0x26, 0x2B, 0x23, 0x22, 0x21, 0x24, 0x27, 0x2A, 0x28, 0x29, 0x25, 
 	  0x26, 0x2B, 0x23, 0x22, 0x21, 0x24, 0x27, 0x2A, 0x28, 0x29, 0x25, 
	  0x27, 0x2A, 0x28, 0x29, 0x25, 0x27, 0x2A, 0x28, 0x29, 0x25 };

    return (0xFE00 | rule[ch - KEISEN_JIS_BEG]);
}
/*-- --*/

static dev_noop() {}

devil$text_initialize()
{
    switch (devil$mod.l_device_type) {
    case DEVIL$_LA80:
    case DEVIL$_LA84:
    case DEVIL$_LA86:
	func.dev_setup        = dec_setup;
	func.put_init         = dec_init;
	func.put_reset        = dec_reset;
	func.put_ascii        = dec_ascii;
	func.put_kana         = dec_kana;
	func.put_kanji        = dec_kanji;
	func.put_keisen       = dec_keisen;
	func.put_space        = dec_space;
	func.put_vertical     = dec_vertical;
	func.put_horizontal   = dec_horizontal;
	func.put_init_text    = dev_noop;
	func.put_char_attr    = dec_set_char_attr;
	func.put_chapter_bar  = dec_chapter_bar;
	func.box_from	      = dev_noop;
	func.box_to	      = dev_noop;
	func.set_font         = dev_noop;
	func.set_line_pitch   = dec_set_line_pitch;
	func.set_char_pitch   = dec_set_char_pitch;
	func.skip_pitch       = text_skip_pitch;
	break;
    case DEVIL$_NEC:
	func.dev_setup        = nec_setup;
	func.put_init         = nec_init;
	func.put_reset        = nec_reset;
	func.put_ascii        = nec_ascii;
	func.put_kana         = nec_ascii;
	func.put_kanji        = nec_kanji;
	func.put_keisen       = nec_kanji;
	func.put_space        = nec_space;
	func.put_vertical     = nec_vertical;
	func.put_horizontal   = nec_horizontal;
	func.put_char_attr    = dev_noop;
	func.put_init_text    = dev_noop;
	func.put_chapter_bar  = dev_noop;
	func.box_from	      = dev_noop;
	func.box_to	      = dev_noop;
	func.set_font         = dev_noop;
	func.set_line_pitch   = nec_set_line_pitch;
	func.set_char_pitch   = nec_set_char_pitch;
	func.skip_pitch       = text_skip_pitch;
	break;
    case DEVIL$_LN80:
	func.dev_setup        = lbp_setup;
	func.put_init         = lbp_init;
	func.put_reset        = lbp_reset;
	func.put_ascii        = lbp_ascii;
	func.put_kana         = lbp_ascii;
	func.put_kanji        = lbp_kanji;
	func.put_keisen       = lbp_keisen;
	func.put_space        = lbp_space;
	func.put_vertical     = lbp_vertical;
	func.put_horizontal   = lbp_horizontal;
	func.put_init_text    = lbp_init_text;
	func.put_char_attr    = lbp_set_char_attr;
	func.put_chapter_bar  = lbp_chapter_bar;
	func.box_from	      = lbp_box_from;
	func.box_to	      = lbp_box_to;
	func.set_font         = lbp_set_font;
	func.set_line_pitch   = lbp_set_line_pitch;
	func.set_char_pitch   = lbp_set_char_pitch;
	func.skip_pitch       = lbp_skip_pitch;
	break;
    case DEVIL$_LN03:
	func.dev_setup        = ln03_setup;
	func.put_init         = ln03_init;
	func.put_reset        = ln03_reset;
	func.put_ascii        = ln03_ascii;
	func.put_kana         = ln03_kana;
	func.put_kanji        = ln03_kanji;
	func.put_keisen       = ln03_keisen;
	func.put_space        = ln03_space;
	func.put_vertical     = ln03_vertical;
	func.put_horizontal   = ln03_horizontal;
	func.put_init_text    = ln03_init_text;
	func.put_char_attr    = ln03_set_char_attr;
	func.put_chapter_bar  = ln03_chapter_bar;
	func.box_from	      = dev_noop;
	func.box_to	      = dev_noop;
	func.set_font         = ln03_set_font;
	func.set_line_pitch   = ln03_set_pitch;
	func.set_char_pitch   = ln03_set_pitch;
	func.skip_pitch       = text_skip_pitch;
	break;
    case DEVIL$_BROTHER:
	func.dev_setup        = bro_setup;
	func.put_init         = bro_init;
	func.put_reset        = bro_reset;
	func.put_ascii        = bro_ascii;
	func.put_kana         = bro_kana;
	func.put_kanji        = bro_kanji;
	func.put_keisen       = bro_keisen;
	func.put_space        = bro_space;
	func.put_vertical     = bro_vertical;
	func.put_horizontal   = bro_horizontal;
	func.put_init_text    = dev_noop;
	func.put_char_attr    = dev_noop;
	func.put_chapter_bar  = dev_noop;
	func.box_from	      = dev_noop;
	func.box_to	      = dev_noop;
	func.set_font         = dev_noop;
	func.set_line_pitch   = bro_set_line_pitch;
	func.set_char_pitch   = bro_set_char_pitch;
	func.skip_pitch       = text_skip_pitch;
	break;
    case DEVIL$_TEXT:
	func.dev_setup        = text_setup;
	func.put_init         = dev_noop;
	func.put_reset        = dev_noop;
	func.put_ascii        = text_ascii;
	func.put_kana         = text_kana;
	func.put_kanji        = text_kanji;
	func.put_keisen       = text_keisen;
	func.put_space        = text_space;
	func.put_vertical     = dev_noop;
	func.put_horizontal   = dev_noop;
	func.put_init_text    = dev_noop;
	func.put_char_attr    = text_set_char_attr;
	func.put_chapter_bar  = dec_chapter_bar;
	func.box_from	      = dev_noop;
	func.box_to	      = dev_noop;
	func.set_font         = dev_noop;
	func.set_line_pitch   = dev_noop;
	func.set_char_pitch   = dev_noop;
	func.skip_pitch       = text_skip_pitch;
	break;
    }

    devil$text_setup();
}

devil$text_setup()
{
    (*func.dev_setup)();

    if (devil$mod.al_chars_per_inch != 0)
	*devil$mod.al_chars_per_inch = devil$mod.l_chars_per_inch;
    if (devil$mod.al_lines_per_inch != 0)
	*devil$mod.al_lines_per_inch = devil$mod.l_lines_per_inch;
}

devil$text_put_init()
{
    (*func.put_init)();
    if (devil$mod.l_device_type != DEVIL$_LN03 || initial_flag == ON)
	devil$text_put_writing_mode(devil$mod.v_vertical);
    else
	devil$text_set_writing_mode(devil$mod.v_vertical);
    devil$txt.l_input_kana_shift = 0;
    devil$txt.l_char_set         = CHAR_SET_ASCII;
    devil$txt.v_doublewidth      = 0;
}

devil$text_put_init_text()
{
    (*func.put_init_text)();
}

devil$text_box_from(x, y, pos, line, line_width, line_type, mesh, mesh_type)
int x;
int y;
int pos;
int line;
int line_width;
int line_type;
int mesh;
int mesh_type;
{
    (*func.box_from)(x, y, pos, line, line_width, line_type, mesh, mesh_type);
}

devil$text_box_to(x, y, pos, line, mesh)
int x;
int y;
int pos;
int line;
int mesh;
{
    (*func.box_to)(x, y, pos, line, mesh);
}

devil$text_set_font(pitch, size, type)
int pitch;
int size;
int type;
{
    (*func.set_font)(pitch, size, type);
}

devil$text_set_line_pitch(lines_per_inch, res_lines_per_inch)
int  lines_per_inch;
int *res_lines_per_inch;
{
    devil$mod.l_lines_per_inch = lines_per_inch;
    (*func.dev_setup)();
    if (res_lines_per_inch != 0)
	*res_lines_per_inch = devil$mod.l_lines_per_inch;
    (*func.set_line_pitch)();
}

devil$text_skip_pitch(pitch)
int pitch;
{
    (*func.skip_pitch)(pitch);
}

devil$text_put_reset()
{
    if (func.put_reset != 0)
	(*func.put_reset)();
}

devil$text_put_writing_mode(vertical)
int vertical;
{
    if (devil$txt.v_vertical = vertical)
	(*func.put_vertical)();
    else
	(*func.put_horizontal)();
}

devil$text_set_writing_mode(vertical)
int vertical;
{
    if (vertical != devil$txt.v_vertical)
	devil$text_put_writing_mode(vertical);
}

devil$text_set_char_attr(col, old_attr, new_attr)
int      col;
unsigned old_attr;
unsigned new_attr;
{
    (*func.put_char_attr)(col, old_attr, new_attr);
    devil$txt.v_doublewidth = (new_attr & DEVIL$M_DOUBLEWIDTH) != 0;
    devil$txt.v_overstrike  = (new_attr & DEVIL$M_OVERSTRIKE)  != 0;
    devil$mod.v_vertical    = devil$txt.v_vertical
			    = (new_attr & DEVIL$M_VERTICAL)    != 0;
}

devil$text_put_chapter_bar()
{
    (*func.put_chapter_bar)();
}

devil$text_put_space(space)
int space;
{
    (*func.put_space)(space);
    devil$txt.l_char_set = CHAR_SET_ASCII;
}

devil$text_doublehight_add_line()
{
    return devil$txt.v_doublehight_add_line;
}
