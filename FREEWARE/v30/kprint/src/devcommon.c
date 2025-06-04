#module DEVIL$COMMON "V4.5-000"
#define __MODULE__ "DEVIL$COMMON"
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
! FACILITY: Device Library Version 4.5-000
!
! FUNCTIONAL DESCRIPTION:
!
! ENVIRONMENT: VAX/VMS
!
! AUTHOR: Inasawa, Yuichi       CREATION DATE: 22-May-1985
!
! MODIFIED BY:
!
! V5.2-000
!	antonietta	Jun 16 1989
!	Dissabele kanji code conversion (2 ku -> 10 ku) on Kprint/LBP
!
! V4.5-000
!	Y. Kozono	28-Nov-1986
!	LN03 support
--*/

#include stsdef
#include descrip
#include "jsy$devil:devildef.h"

globaldef struct DEVIL$MOD devil$mod;

static int (*func_buff_char)();
static int (*func_buff_string)();
static int (*func_buff_output)();

static unsigned char *internal_buff_ptr;
static unsigned char *internal_buff_end;

devil$initialize(item_list)
struct DEVIL$ITEM_LIST item_list[];
{
    internal_buff_ptr = 0;
    internal_buff_end = 0;

    devil$mod.v_output		    = 1;
    devil$mod.v_font_pitch          = 0;
    devil$mod.v_font_size           = 0;
    devil$mod.v_font_type           = 0;
    devil$mod.v_full_paint          = 0;
    devil$mod.v_ascii               = 0;
    devil$mod.v_jis                 = 0;
    devil$mod.v_letter              = 0;
    devil$mod.v_high_speed          = 0;
    devil$mod.v_keisen              = 0;
    devil$mod.v_slant               = 0;
    devil$mod.v_image_draft         = 0;
    devil$mod.v_image_frame         = 0;
    devil$mod.v_image_mesh          = 0;
    devil$mod.v_landscape           = 0;
    devil$mod.v_overlay_form        = 0;
    devil$mod.v_reset_overlay_form  = 0;
    devil$mod.v_register_form       = 0;
    devil$mod.l_keisen_line         = 0;
    devil$mod.l_keisen_width        = 0;
    devil$mod.l_overlay_form        = 0;
    devil$mod.l_register_form       = 0;
    devil$mod.l_aspect_ratio_h      = 0;
    devil$mod.l_aspect_ratio_v      = 0;
    devil$mod.l_char_pitch          = 0;
    devil$mod.l_line_pitch          = 0;
    devil$mod.l_size_type           = 0;
    devil$mod.l_chars_per_inch      = 0;
    devil$mod.l_lines_per_inch      = 0;
    devil$mod.l_image_per_inch      = 0;

    item_list_setup(item_list);

    devil$text_initialize();
    devil$sixel_initialize();
}

devil$setup(item_list)
struct DEVIL$ITEM_LIST item_list[];
{
    item_list_setup(item_list);

    devil$text_setup();
    devil$sixel_setup();
}

devil$setup_text(item_list)
struct DEVIL$ITEM_LIST item_list[];
{
    item_list_setup(item_list);
    devil$text_setup();
}

devil$setup_sixel(item_list)
struct DEVIL$ITEM_LIST item_list[];
{
    item_list_setup(item_list);
    devil$sixel_setup();
}

static item_list_setup(item_list)
struct DEVIL$ITEM_LIST item_list[];
{
    int i;

    devil$mod.al_chars_per_inch     = 0;
    devil$mod.al_lines_per_inch     = 0;
    devil$mod.al_image_per_inch     = 0;

    for (i=0; item_list[i].item_code != 0; i ++) {
	switch (item_list[i].item_code) {
	case DEVIL$_DEVICE_TYPE:
	    devil$mod.l_device_type = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_ASCII:
	    devil$mod.v_ascii = 1;
	    break;
	case DEVIL$_ASPECT_RATIO_H:
	    devil$mod.l_aspect_ratio_h = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_ASPECT_RATIO_V:
	    devil$mod.l_aspect_ratio_v = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_JIS:
	    devil$mod.v_jis = 1;
	    break;
	case DEVIL$_LETTER:
	    devil$mod.v_letter = 1;
	    break;
	case DEVIL$_HIGH_SPEED:
	    devil$mod.v_high_speed = 1;
	    break;
	case DEVIL$_SLANT:
	    devil$mod.v_slant = 1;
	    break;
	case DEVIL$_VERTICAL:
	    devil$mod.v_vertical = 1;
	    break;
	case DEVIL$_IMAGE_DRAFT:
	    devil$mod.v_image_draft = 1;
	    break;
	case DEVIL$_IMAGE_FRAME:
	    devil$mod.v_image_frame = 1;
	    break;
	case DEVIL$_IMAGE_MESH:
	    devil$mod.v_image_mesh  = 1;
	    break;
	case DEVIL$_LANDSCAPE:
	    devil$mod.v_landscape = 1;
	    break;
	case DEVIL$_FULL_PAINT:
	    devil$mod.v_full_paint= 1;
	    break;
	case DEVIL$_FONT_PITCH:
	    devil$mod.v_font_pitch = 1;
	    devil$mod.l_font_pitch = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_FONT_SIZE:
	    devil$mod.v_font_size = 1;
	    devil$mod.l_font_size = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_FONT_TYPE:
	    devil$mod.v_font_type = 1;
	    devil$mod.l_font_type = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_KEISEN_LINE:
	    devil$mod.v_keisen = 1;
	    devil$mod.l_keisen_line = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_KEISEN_WIDTH:
	    devil$mod.v_keisen = 1;
	    devil$mod.l_keisen_width = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_OVERLAY_FORM:
	    devil$mod.v_overlay_form = 1;
	    devil$mod.l_overlay_form = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_RESET_OVERLAY_FORM:
	    devil$mod.v_reset_overlay_form = 1;
	    break;
	case DEVIL$_REGISTER_FORM:
	    devil$mod.v_register_form = 1;
	    devil$mod.l_register_form = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_CHAR_PITCH:
	    devil$mod.l_char_pitch = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_LINE_PITCH:
	    devil$mod.l_line_pitch = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_CHARS_PER_INCH:
	    devil$mod.l_chars_per_inch = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_LINES_PER_INCH:
	    devil$mod.l_lines_per_inch = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_IMAGE_PER_INCH:
	    devil$mod.l_image_per_inch = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_SIZE_TYPE:
	    devil$mod.l_size_type = *((int *)item_list[i].buff_addr);
	    break;
	case DEVIL$_RES_CHARS_PER_INCH:
	    devil$mod.al_chars_per_inch = item_list[i].buff_addr;
	    break;
	case DEVIL$_RES_LINES_PER_INCH:
	    devil$mod.al_lines_per_inch = item_list[i].buff_addr;
	    break;
	case DEVIL$_RES_IMAGE_PER_INCH:
	    devil$mod.al_image_per_inch = item_list[i].buff_addr;
	    break;
	case DEVIL$_BUFF_CHAR:
	    func_buff_char = item_list[i].buff_addr;
	    break;
	case DEVIL$_BUFF_STRING:
	    func_buff_string = item_list[i].buff_addr;
	    break;
	case DEVIL$_BUFF_OUTPUT:
	    func_buff_output = item_list[i].buff_addr;
	    break;

	/*
	 * V5.2-000: Disable 2ku->10ku kanji code conversion
	 */
	case DEVIL$_PLAIN_LBP:
	    devil$mod.v_plain_lbp = 1;
	    break;
	}
    }
}

devil$convert_text_dx(dst, src)
struct dsc$descriptor *dst;
struct dsc$descriptor *src;
{
    unsigned char buff[2048];
    unsigned char *ptr;
    int            len;
    int		   stat;

    internal_buff_ptr = buff;
    internal_buff_end = buff + 2048;

    if (!((stat = lib$analyze_sdesc(src, &len, &ptr)) & STS$M_SUCCESS))
	return stat;

    devil$text_put_string(0, ptr, len);
    stat = ots$scopy_r_dx(internal_buff_ptr - buff, buff, dst);

    internal_buff_ptr = 0;
    internal_buff_end = 0;

    return stat;
}

devil$buff_set_output(output)
int output;
{
    devil$mod.v_output = output;
}

devil$buff_char(chr)
unsigned chr;
{
    if (!devil$mod.v_output) return;

    if (internal_buff_ptr == 0)
	(*func_buff_char)(chr);
    else
	jsy$ch_pnext(chr, &internal_buff_ptr, internal_buff_end);
}

devil$buff_string(str, len)
unsigned char *str;
int            len;
{
    if (!devil$mod.v_output) return;

    if (internal_buff_ptr == 0)
	(*func_buff_string)(str, len);
    else
    if (internal_buff_ptr + len <= internal_buff_end) {
	jsy$ch_move(len, str, internal_buff_ptr);
	internal_buff_ptr += len;
    }
}

devil$buff_output()
{
    if (!devil$mod.v_output) return;

    return (*func_buff_output)();
}
