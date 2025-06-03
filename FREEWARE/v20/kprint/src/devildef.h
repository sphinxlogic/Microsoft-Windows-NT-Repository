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
! V5.2-000
!	antonietta	Jun 16 1989
!	Dissabele kanji code conversion (2 ku -> 10 ku) on Kprint/LBP
!
! V4.5-000
!	Y. Kozono	28-Nov-1986
!	LN03 support
--*/

#define DEVIL$_UNKNOWN			 0
#define DEVIL$_LA80			 1
#define DEVIL$_LA84			 2
#define DEVIL$_NEC			 3
#define DEVIL$_LN80			 4
#define DEVIL$_BROTHER			 5
#define DEVIL$_LP			 6
#define DEVIL$_TEXT			 7
#define DEVIL$_VT100			 8
#define DEVIL$_LN03			 9
#define DEVIL$_LA86			10

#define DEVIL$_ASCII			 1
#define DEVIL$_JIS			 2
#define DEVIL$_LETTER			 3
#define DEVIL$_HIGH_SPEED		 4
#define DEVIL$_SLANT			 5
#define DEVIL$_VERTICAL			 6
#define DEVIL$_LANDSCAPE		 7
#define DEVIL$_OVERLAY_FORM		 8
#define DEVIL$_REGISTER_FORM		 9
#define DEVIL$_CHAR_PITCH		10
#define DEVIL$_CHARS_PER_INCH		11
#define DEVIL$_LINE_PITCH		12
#define DEVIL$_LINES_PER_INCH		13
#define DEVIL$_IMAGE_PITCH		14
#define DEVIL$_IMAGE_PER_INCH		15
#define DEVIL$_BUFF_CHAR		16
#define DEVIL$_BUFF_STRING		17
#define DEVIL$_BUFF_OUTPUT		18
#define DEVIL$_DEVICE_TYPE		19
#define DEVIL$_IMAGE_DRAFT		20
#define DEVIL$_RES_CHARS_PER_INCH	21
#define DEVIL$_RES_LINES_PER_INCH	22
#define DEVIL$_RES_IMAGE_PER_INCH	23
#define DEVIL$_ASPECT_RATIO_H		24
#define DEVIL$_ASPECT_RATIO_V		25
#define DEVIL$_KEISEN_LINE		26
#define DEVIL$_KEISEN_WIDTH		27
#define DEVIL$_IMAGE_FRAME		28
#define DEVIL$_IMAGE_MESH		29
#define DEVIL$_RESET_OVERLAY_FORM	30
#define DEVIL$_FONT_PITCH		31
#define DEVIL$_FONT_SIZE		32
#define DEVIL$_FONT_TYPE		33
#define DEVIL$_FULL_PAINT		34
#define DEVIL$_SIZE_TYPE		35
#define DEVIL$_PLAIN_LBP		36

#define DEVIL$V_BOLD			 0
#define DEVIL$V_DOUBLEHIGHT		 1
#define DEVIL$V_DOUBLEUNDERLINE		 2
#define DEVIL$V_DOUBLEWIDTH		 3
#define DEVIL$V_ITALIC			 4
#define DEVIL$V_LIGHT			 5
#define DEVIL$V_MESH			 6
#define DEVIL$V_OVERSTRIKE		 7
#define DEVIL$V_REVERSE			 8
#define DEVIL$V_UNDERLINE		 9
#define DEVIL$V_SUBSCRIPT		10
#define DEVIL$V_SUPERSCRIPT		11
#define DEVIL$V_VERTICAL		12
#define DEVIL$V_WHITE			13
#define DEVIL$V_EIGHTBIT		14

#define DEVIL$M_BOLD			(1 << DEVIL$V_BOLD)
#define DEVIL$M_DOUBLEHIGHT		(1 << DEVIL$V_DOUBLEHIGHT)
#define DEVIL$M_DOUBLEUNDERLINE		(1 << DEVIL$V_DOUBLEUNDERLINE)
#define DEVIL$M_DOUBLEWIDTH		(1 << DEVIL$V_DOUBLEWIDTH)
#define DEVIL$M_ITALIC			(1 << DEVIL$V_ITALIC)
#define DEVIL$M_LIGHT			(1 << DEVIL$V_LIGHT)
#define DEVIL$M_MESH			(1 << DEVIL$V_MESH)
#define DEVIL$M_OVERSTRIKE		(1 << DEVIL$V_OVERSTRIKE)
#define DEVIL$M_REVERSE			(1 << DEVIL$V_REVERSE)
#define DEVIL$M_UNDERLINE		(1 << DEVIL$V_UNDERLINE)
#define DEVIL$M_SUBSCRIPT		(1 << DEVIL$V_SUBSCRIPT)
#define DEVIL$M_SUPERSCRIPT		(1 << DEVIL$V_SUPERSCRIPT)
#define DEVIL$M_VERTICAL		(1 << DEVIL$V_VERTICAL)
#define DEVIL$M_WHITE			(1 << DEVIL$V_WHITE)
#define DEVIL$M_EIGHTBIT		(1 << DEVIL$V_EIGHTBIT)

#define DEVIL$V_SIZE_A4			0
#define DEVIL$V_SIZE_LETTER		1
#define DEVIL$V_SIZE_B5			2
#define DEVIL$V_SIZE_REGAL		3
#define DEVIL$V_SIZE_FROM		DEVIL$V_SIZE_A4
#define DEVIL$V_SIZE_TO			DEVIL$V_SIZE_REGAL

struct DEVIL$ITEM_LIST {
      short buff_leng;
      short item_code;
      char *buff_addr;
      char *leng_addr;
   };

struct DEVIL$MOD {
	unsigned    v_font_pitch	    : 1;
	unsigned    v_font_size		    : 1;
	unsigned    v_font_type		    : 1;
	unsigned    v_full_paint	    : 1;
	unsigned    v_ascii		    : 1;
	unsigned    v_jis		    : 1;
	unsigned    v_letter		    : 1;
	unsigned    v_high_speed	    : 1;
	unsigned    v_keisen		    : 1;
	unsigned    v_image_draft	    : 1;
	unsigned    v_image_frame	    : 1;
	unsigned    v_image_mesh	    : 1;
	unsigned    v_landscape		    : 1;
	unsigned    v_output		    : 1;
	unsigned    v_overlay_form	    : 1;
	unsigned    v_register_form	    : 1;
	unsigned    v_reset_overlay_form    : 1;
	unsigned    v_slant		    : 1;
	unsigned    v_vertical		    : 1;
	unsigned    v_plain_lbp	: 1;		/* true on Kprint/Lbp	*/
	int	    l_device_type;
	int	    l_aspect_ratio_h;
	int	    l_aspect_ratio_v;
	int	    l_font_pitch;
	int	    l_font_size;
	int	    l_font_type;
	int	    l_keisen_line;
	int	    l_keisen_width;
	int	    l_overlay_form;
	int	    l_register_form;
	int	    l_char_pitch;
	int	    l_line_pitch;
	int	    l_chars_per_inch;
	int	    l_lines_per_inch;
	int	    l_image_per_inch;
	int	    l_pitch_per_line;
	int	    l_size_type;
	double	    *al_chars_per_inch;
	int	    *al_lines_per_inch;
	int	    *al_image_per_inch;
    };

globalref struct DEVIL$MOD devil$mod;
