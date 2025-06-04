#module KPRI$PARSE "V4.2-000"
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
! FACILITY: KPRI Version 4.0-006
!
! FUNCTIONAL DESCRIPTION:
!
! ENVIRONMENT: VAX/VMS
!
! AUTHOR: Inasawa, Yuichi       CREATION DATE: 27-Apr-1985
!
! MODIFIED BY:
!
--*/

#include descrip
#include "kpridef.h"

#define MAX_CMD_LEN	31

unsigned jsy$ch_gnext();

static $DESCRIPTOR(sd_b,	    "B");
static $DESCRIPTOR(sd_blank,	    "BLANK");
static $DESCRIPTOR(sd_comment,	    "COMMENT");
static $DESCRIPTOR(sd_font_pitch,   "FONT_PITCH");
static $DESCRIPTOR(sd_font_size,    "FONT_SIZE");
static $DESCRIPTOR(sd_font_type,    "FONT_TYPE");
static $DESCRIPTOR(sd_header,	    "HEADER");
static $DESCRIPTOR(sd_h,	    "H");
static $DESCRIPTOR(sd_horizontal,   "HORIZONTAL");
static $DESCRIPTOR(sd_i,	    "I");
static $DESCRIPTOR(sd_italic,	    "ITALIC");
static $DESCRIPTOR(sd_lm,	    "LM");
static $DESCRIPTOR(sd_pitch,	    "PITCH");
static $DESCRIPTOR(sd_s,	    "S");
static $DESCRIPTOR(sd_six,	    "SIX");
static $DESCRIPTOR(sd_sixel,	    "SIXEL");
static $DESCRIPTOR(sd_skip,	    "SKIP");
static $DESCRIPTOR(sd_slant,	    "SLANT");
static $DESCRIPTOR(sd_sp,	    "SP");
static $DESCRIPTOR(sd_spacing,	    "SPACING");
static $DESCRIPTOR(sd_u,	    "U");
static $DESCRIPTOR(sd_upright,	    "UPRIGHT");
static $DESCRIPTOR(sd_v,	    "V");
static $DESCRIPTOR(sd_vertical,	    "VERTICAL");

struct CMD_ENTRY {
	int def_value;
	};

static struct CMD_ENTRY cmd_table[] = {
	{0},	/* UNKNOWN	    */
	{1},	/* BLANK	    */
	{0},	/* FONT_PITCH	    */
	{0},	/* FONT_SIZE	    */
	{0},	/* FONT_TYPE	    */
	{0},	/* COMMENT	    */
	{0},	/* HEADER	    */
	{0},	/* HORIZONTAL	    */
	{0},	/* ITALIC	    */
	{0},	/* LEFT_MARGIN	    */
	{0},	/* PITCH	    */
	{1},	/* SKIP		    */
	{1},	/* SPACING	    */
	{0},	/* SIXEL	    */
	{0},	/* UPRIGHT	    */
	{0},	/* VERTICAL	    */
	{0}	/* END OF COMMAND   */
	};

struct PARSE_ENTRY {
	char cmd;
	struct dsc$descriptor_s *cmd_str;
	};

static struct PARSE_ENTRY parse_table[] = {
	{CMD_BLANK,	    &sd_b},
	{CMD_BLANK,	    &sd_blank},
	{CMD_COMMENT,	    &sd_comment},
	{CMD_FONT_PITCH,    &sd_font_pitch},
	{CMD_FONT_SIZE,     &sd_font_size},
	{CMD_FONT_TYPE,     &sd_font_type},
	{CMD_HORIZONTAL,    &sd_h},
	{CMD_HEADER,	    &sd_header},
	{CMD_HORIZONTAL,    &sd_horizontal},
	{CMD_ITALIC,	    &sd_i},
	{CMD_ITALIC,	    &sd_italic},
	{CMD_LEFT_MARGIN,   &sd_lm},
	{CMD_PITCH,	    &sd_pitch},
	{CMD_SIXEL,	    &sd_six},
	{CMD_SIXEL,	    &sd_sixel},
	{CMD_SKIP,	    &sd_s},
	{CMD_SKIP,	    &sd_skip},
	{CMD_ITALIC,	    &sd_slant},
	{CMD_SPACING,	    &sd_sp},
	{CMD_SPACING,	    &sd_spacing},
	{CMD_UPRIGHT,	    &sd_u},
	{CMD_UPRIGHT,	    &sd_upright},
	{CMD_VERTICAL,	    &sd_v},
	{CMD_VERTICAL,	    &sd_vertical},
	{CMD_UNKNOWN,	    0}
	};

kpri$parse(buf, len, cmd, sign, num, str, omitted)
unsigned char *buf;
int            len;
int           *cmd;
int           *sign;
int           *num;
struct dsc$descriptor *str;
int           *omitted;
{
    unsigned char *org;
    unsigned char *end;
    unsigned char *top;
    unsigned char *las;
    unsigned char cmd_buf[MAX_CMD_LEN];
    int           cmd_len;
    $DESCRIPTOR(cmd_str, cmd_buf);
    int           idx;
    unsigned      chr;

    *cmd = CMD_UNKNOWN;
    *num = -1;
    *omitted = 1;

    cmd_len = 0;
    org = buf;
    end = buf + len;
    buf ++;
    top = buf;

    if (jsy$ch_gchar(buf, end) == '!') {
	*cmd = CMD_COMMENT;
	return 1;
    }

    while ((chr = jsy$ch_gnext(&buf, end)) != JSY$C_EOS) {
	if (chr >='a' && chr <= 'z')
	    chr = chr - ('a' - 'A');
	if (chr != '_' && (chr < 'A' || chr > 'Z')) break;
	if (cmd_len < MAX_CMD_LEN)
	    cmd_buf[cmd_len++] = chr;
	top = buf;
    }
    cmd_str.dsc$w_length = cmd_len;

    for (idx = 0; parse_table[idx].cmd != CMD_UNKNOWN; idx++) {
	if (str$compare_eql(&cmd_str, parse_table[idx].cmd_str) == 0) {
	    *cmd = parse_table[idx].cmd;
	    break;
	}
    }

    if (*cmd == CMD_UNKNOWN) {
	invalid_command(org, len);
	return 0;
    }

    while (chr == ' ' || chr == TAB) {
	top = buf;
	chr = jsy$ch_gnext(&buf, end);
    }

    if (*cmd == CMD_SIXEL) {
	las = top;
	while (chr != ' ' && chr != TAB && chr != JSY$C_EOS) {
	    las = buf;
	    chr = jsy$ch_gnext(&buf, end);
	}
	if (las > top && (*top == '"' || *top == '\'')) top ++;
	if (las > top && (*(las - 1) == '"' || *(las - 1) == '\'')) las --;
	ots$scopy_r_dx(las - top, top, str);

	return 1;
    }

    *sign = 0;
    if (chr == '+' || chr == '-') {
	if (chr == '+') *sign =  1;
		   else *sign = -1;
	do {
	    chr = jsy$ch_gnext(&buf, end);
	}
	while (chr == ' ' || chr == TAB);
    }
    if (chr >= '0' && chr <= '9') {
	*num = 0;
	do {
	    *num = *num * 10 + (chr - '0');
	    chr = jsy$ch_gnext(&buf, end);
	}
	while (chr >= '0' && chr <= '9');
	*omitted = 0;
    }
    else
	*num = cmd_table[*cmd].def_value;

    return 1;
}

static invalid_command(str, len)
unsigned char *str;
int            len;
{
    int msg_vec[10];

    msg_vec[0] = 4;
    msg_vec[1] = KPRI$_INVALID_COMMAND;
    msg_vec[2] = 2;
    msg_vec[3] = len;
    msg_vec[4] = str;
    sys$putmsg(&msg_vec, 0, 0, 0);
}
