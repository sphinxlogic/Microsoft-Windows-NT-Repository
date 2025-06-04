#module KPRI$KPRINT "V5.2-001"
#define __MODULE__ "KPRI$KPRINT"
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
! FACILITY: KPRI Version 5.2-001
!
! FUNCTIONAL DESCRIPTION:
!
! ENVIRONMENT: VAX/VMS
!
! AUTHOR: Inasawa, Yuichi       CREATION DATE: 28-MAY-1983
!
! MODIFIED BY: 伊藤 民哉
!
! A1.0
!	K. Hirata	Tue Oct 20 1992
!
!	Remove NEC,BROTHER,LBP,LN80 and LA80 support for Alpha.
!
! v5.5-2M
!	antonietta	Tue Oct 20 14:01:14 1992
!
!	Remove  NEC and BROTHER support.
!
! V5.2-001
!	antonietta	Oct 31 1989
!
!	BUG FIX, SPRed as #221 and #222
!
!	1) Supply default file extension in searching a file to define a JOB
!	   name.
!	2) To print FORTRAN CR Cntrol file.
!
! V5.2-000
!	antonietta	Jun 16 1989
!
!	Miscellaneous BUG FIX
!
!	1) VFC support (ONLY printing cover page correctly),alos not including
!	   PRN carrige control
!	2) Set left margin same as V4.5 on Kprint/Ln03 in source file DEVTEXT.C
!	3) Dissabele kanji code conversion (2 ku -> 10 ku) on Kprint/LBP
!	4) Remove verbos BLANK codes before escape sequence codes.
!	5) Change the size of buffer for JBS messaging to 256 bytes
!		(old size = 80 bytes)
!
! V5.0-000
!	T.Itoh
!	Temp. File をvisibleにした。
!	/COVER/NOFLAG をdefault にした。
!	job 名の設定のための修正をした。
!       V4.6 のバグ取り
!
! V4.6-000
!	A.murakami	30-SEP-1987	M002
!	/CHECK noefect
!
! V4.6-000
!	A.murakami	21-SEP-1987	M001
!	/COVER_PAGE 's bug fix
!
! V4.5-000
!	Y. Kozono	28-Nov-1986
!	LN03 support
!	/COVER_PAGE support
!	/HEADER replace print symbiont to print command
!
! V4.2-000
!	Y. Inasawa
!
! V4.0-006
!	Y. Inasawa,	 12-Feb-1985
!	LBP Kana support
!
! V4.0-005
!	Y. Inasawa,	 7-Feb-1985
!	JIS Level-II check for LBP8
!
! V4.0-004
!	Y. Inasawa,	 7-Feb-1985
!	/FORM=form_name support added
!
! V4.0-003
!	Y. Inasawa,	27-Jan-1985
!	Title and Footer support added for all printer
!
! V4.0-002
!	Y. Inasawa,	13-Jan-1985
!	/OUTPUT -- Output to file
!	/LOG	-- Control JOB .... Message
!	Replace SYS$SNDSMB to new V4 routine SYS$SNDJBC
!
! V4.0  K. Ishikawa,	30-NOV-1984
!	Support /COPIES qualifier
!
! V1.4  M. Iwamoto,	19-OCT-1984
!	xxxx_print and form_put have modified to support the fortran
!	carriage control file.	( it's a temporary repaire )
!
! V1.3	K. Ishikawa,	27-AUG-1984
!	Prier "head" qualifier was renamed to "title"
!	New qualifiers - delete, flag_page, header and notify.
!
! V1.2  K. Ishikawa,    08-AUG-1984
!       LA84 needs horizontal mode when writing a frame.
!       Change the logic for restoring the LA8x terminal characteristics.
!       Add support for ASCII, LETTER and SLANT.
!	Output file spec is replaced by /QUEUE qualifier.
!
--*/

#include jpidef
#include descrip
#include iodef
#include rms
#include ssdef
#include stsdef
#include climsgdef
#include "sjcdef.h"
#include "kpridef.h"
#include "jsy$devil:devildef.h"

#define DEF_PAGE_LENGTH		66
#define DEF_LINES_PER_INCH	6
#define DEF_LINE_PITCH		0
#define DEF_CHAR_PITCH		0
#define DEF_IMAGE_PER_INCH	0
#define DEF_TOP			0
#define DEF_BOTTOM		6
#define DEF_LEFT		0
#define DEF_RIGHT		80
#define SIZE_TYPE_BAS		100
#define SIZE_TYPE_FROM		(DEVIL$V_SIZE_FROM   + SIZE_TYPE_BAS)
#define SIZE_TYPE_TO		(DEVIL$V_SIZE_TO     + SIZE_TYPE_BAS)
#define DEF_SIZE_TYPE		(DEVIL$V_SIZE_A4     + SIZE_TYPE_BAS)
#define DEF_SIZE_V		11.0
#define DEF_SIZE_H		13.5
#define DEF_COPIES		1
#define DEF_PAGE_START		1
#define DEF_PAGE_END		999999999
#define DEF_PAGE_INITIAL	1
#define DEF_PAGE_POSITION	70
#define DEF_PAGE_TYPE		1
#define DEF_PRINT_START		1
#define DEF_PRINT_END		999999999
#define DEF_TITLE_POSITION	1
#define DEF_FOOTER_POSITION	1
#define BOOL int

unsigned jsy$chg_rom_full();
unsigned jsy$ch_gnext();
int      jsy$ch_pnext();
int      jsy$ch_wnext();
int      jsy$ch_move();
/* unsigned jsy$ch_gnext_shift(); */

static $DESCRIPTOR(sd_p1,		"p1");
static $DESCRIPTOR(sd_p2,		"p2");
static $DESCRIPTOR(sd_la84,		"la84");
#ifndef __alpha /* VMS */
static $DESCRIPTOR(sd_la80,		"la80");
static $DESCRIPTOR(sd_ln80,		"ln80");
static $DESCRIPTOR(sd_lbp,		"lbp");
#endif
#if	0
static $DESCRIPTOR(sd_nec,		"nec");
static $DESCRIPTOR(sd_brother,		"brother");
#endif	/* 0 */
static $DESCRIPTOR(sd_ln03,		"ln03");
static $DESCRIPTOR(sd_la86,		"la86");

static $DESCRIPTOR(sd_aspect_ratio,	"aspect_ratio");
static $DESCRIPTOR(sd_ascii,		"ascii");
static $DESCRIPTOR(sd_bottom,		"bottom");
static $DESCRIPTOR(sd_char_pitch,	"character_pitch");
static $DESCRIPTOR(sd_check,		"check");
static $DESCRIPTOR(sd_check_all,	"check_all");
static $DESCRIPTOR(sd_check_none,	"check_none");
static $DESCRIPTOR(sd_check_level_II,	"check_level_ii");
static $DESCRIPTOR(sd_check_expanded,	"check_expanded");
static $DESCRIPTOR(sd_check_extended,	"check_extended");
static $DESCRIPTOR(sd_check_keisen,	"check_keisen");
static $DESCRIPTOR(sd_command,		"command");
static $DESCRIPTOR(sd_copies,		"copies");
static $DESCRIPTOR(sd_delete,		"delete");
static $DESCRIPTOR(sd_even_page,	"even_page");
static $DESCRIPTOR(sd_feed,		"feed");
static $DESCRIPTOR(sd_flag_page,	"flag_page");
static $DESCRIPTOR(sd_flag_page_one,	"flag_page_one");
static $DESCRIPTOR(sd_cover_page,	"cover_page");
static $DESCRIPTOR(sd_cover_page_one,	"cover_page_one");
static $DESCRIPTOR(sd_font,		"font");
static $DESCRIPTOR(sd_font_pitch,	"font_pitch");
static $DESCRIPTOR(sd_font_size,	"font_size");
static $DESCRIPTOR(sd_font_type,	"font_type");
static $DESCRIPTOR(sd_font_type_line_printer,
					"font_type_line_printer");
static $DESCRIPTOR(sd_font_type_pica,	"font_type_pica");
static $DESCRIPTOR(sd_font_type_elite,	"font_type_elite");
static $DESCRIPTOR(sd_font_type_courier,"font_type_courier");
static $DESCRIPTOR(sd_font_type_mincho,	"font_type_mincho");
static $DESCRIPTOR(sd_font_type_user,	"font_type_user");
static $DESCRIPTOR(sd_footer,		"footer");
static $DESCRIPTOR(sd_form,		"form");
static $DESCRIPTOR(sd_full_paint,	"full_paint");
static $DESCRIPTOR(sd_header,		"header");
static $DESCRIPTOR(sd_high_speed,	"high_speed");
static $DESCRIPTOR(sd_identify,		"identify");
static $DESCRIPTOR(sd_image,		"image");
static $DESCRIPTOR(sd_image_draft,	"image_draft");
static $DESCRIPTOR(sd_image_frame,	"image_frame");
static $DESCRIPTOR(sd_image_mesh,	"image_mesh");
static $DESCRIPTOR(sd_jis,		"jis");
static $DESCRIPTOR(sd_keisen,		"keisen");
static $DESCRIPTOR(sd_landscape,	"landscape");
static $DESCRIPTOR(sd_left,		"left");
static $DESCRIPTOR(sd_length,		"length");
static $DESCRIPTOR(sd_letter,		"letter");
static $DESCRIPTOR(sd_line_pitch,	"line_pitch");
static $DESCRIPTOR(sd_log,		"log");
static $DESCRIPTOR(sd_manual,		"manual");
static $DESCRIPTOR(sd_notify,		"notify");
static $DESCRIPTOR(sd_odd_page,		"odd_page");
static $DESCRIPTOR(sd_output,		"output");
static $DESCRIPTOR(sd_overlay_form,	"overlay_form");
static $DESCRIPTOR(sd_page,		"page");
static $DESCRIPTOR(sd_page_after,	"page_after");
static $DESCRIPTOR(sd_page_before,	"page_before");
static $DESCRIPTOR(sd_page_bottom,	"page_bottom");
static $DESCRIPTOR(sd_page_end,		"page_end");
static $DESCRIPTOR(sd_page_initial,	"page_initial");
static $DESCRIPTOR(sd_page_position,	"page_position");
static $DESCRIPTOR(sd_page_start,	"page_start");
static $DESCRIPTOR(sd_page_type,	"page_type");
static $DESCRIPTOR(sd_page_top,		"page_top");
static $DESCRIPTOR(sd_passall,		"passall");
static $DESCRIPTOR(sd_pitch,		"pitch");
static $DESCRIPTOR(sd_print,		"print");
static $DESCRIPTOR(sd_print_end,	"print_end");
static $DESCRIPTOR(sd_print_start,	"print_start");
static $DESCRIPTOR(sd_portrate,		"portrate");
static $DESCRIPTOR(sd_queue,		"queue");
static $DESCRIPTOR(sd_register_form,	"register_form");
static $DESCRIPTOR(sd_resolution,	"resolution");
static $DESCRIPTOR(sd_right,		"right");
static $DESCRIPTOR(sd_setup,		"setup");
static $DESCRIPTOR(sd_sixel,		"sixel");
static $DESCRIPTOR(sd_size,		"size");
static $DESCRIPTOR(sd_slant,		"slant");
static $DESCRIPTOR(sd_table,		"table");
static $DESCRIPTOR(sd_top,		"top");
static $DESCRIPTOR(sd_title,		"title");
static $DESCRIPTOR(sd_vertical,		"vertical");
/*++*/
static $DESCRIPTOR(sd_preload,		"preload");
/*--*/

static $DESCRIPTOR(sd_null_output,	    "NL:");
static $DESCRIPTOR(sd_page_type_1_before,   "Page ");
static $DESCRIPTOR(sd_page_type_2_before,   "- ");
static $DESCRIPTOR(sd_page_type_2_after,    " -");
static $DESCRIPTOR(sd_table_line,	    "  . . . . .  ");
static $DESCRIPTOR(sd_input_default,	    ".lis");
static $DESCRIPTOR(sd_output_default,	    ".lis");
static $DESCRIPTOR(sd_comma,		    ",");
static $DESCRIPTOR(sd_output_tmp_def,	    "sys$scratch:");

#define  MAXBUF 4000
static unsigned char buff[MAXBUF];
static unsigned char form[MAXBUF];
static unsigned char *form_ptr;

globaldef unsigned char *kpri$ga_buff_ptr;
globaldef int            kpri$gl_buff_len;

static struct FAB    infab, outfab, tmpfab;
static struct RAB    inrab, outrab, tmprab;
static struct NAM    innam, outnam, tmpnam;	/* used for getting FIDs */
static struct XABFHC xabfhc;
static struct XABRDT xabrdt;
static struct XABPRO xabpro;

static struct dsc$descriptor wrkd          = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor title         = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor footer        = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor queue_name    = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor form_name     = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor out_spc       = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor out_def       = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor res_spc       = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor fil_spc       = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor def_spc       = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor rel_spc       = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor tmp_spc       = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor inres_spc     = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor outres_spc    = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor tmpres_spc    = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor page_before   = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor page_after    = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor setup_modules = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};

#define YES	1
#define NO	0

/*
 * V5.2-000:	Make patch easy to change default value for Conver_Pageing
 *			0 - No Cover_Page
 *			1 - Cover Page All
 *			2 - Cover Page One
 */
globaldef int	kpri$l_cover_page_default_value = 1;	/* Default to /Cover_Page */
static struct {
	int	    aspect_ratio_h;
	int	    aspect_ratio_v;
	int	    bottom;
	double	    chars_per_inch;
	int	    char_pitch;
	int	    copies;
	int	    device_type;
	int	    font_pitch;
	int	    font_size;
	int	    font_type;
	int	    footer_position;
	int	    form_number;
	int	    flag_page;
	int	    cover_page;
	int	    cover_page_com;
	int	    keisen_line;
	int	    keisen_width;
	int	    lines_per_inch;
	int	    line_pitch;
	int	    left;
	int	    page_length;
	int	    page_start;
	int	    page_end;
	int	    page_initial;
	int	    page_position;
	int	    page_type;
	int	    print_start;
	int	    print_end;
	int	    right;
	double	    size_h;
	double	    size_v;
	int	    size_type;
	int	    title_position;
	int	    top;
	int	    image_per_inch;
	int	    overlay_page;
	int	    overlay_second_page;
	int	    register_page;
	unsigned    keisen          : 1;
	unsigned    landscape       : 1;
	unsigned    full_paint	    : 1;
	unsigned    vertical        : 1;
	unsigned    title           : 1;
	unsigned    footer          : 1;
	unsigned    page            : 1;
	unsigned    page_top        : 1;
	unsigned    page_bottom     : 1;
	unsigned    page_before     : 1;
	unsigned    page_after      : 1;
	unsigned    passall	    : 1;
	unsigned    odd_page        : 1;
	unsigned    even_page       : 1;
	unsigned    feed            : 1;
	unsigned    sixel           : 1;
	unsigned    ascii           : 1;
	unsigned    jis             : 1;
	unsigned    letter          : 1;
	unsigned    high_speed      : 1;
	unsigned    slant           : 1;
	unsigned    delete          : 1;
	unsigned    header          : 1;
	unsigned    notify          : 1;
	unsigned    identify        : 1;
	unsigned    table           : 1;
	unsigned    output          : 1;
	unsigned    form            : 1;
	unsigned    is_form_number  : 1;
	unsigned    command         : 1;
	unsigned    manual          : 1;
	unsigned    check_level_II  : 1;
	unsigned    check_expanded  : 1;
	unsigned    check_keisen    : 1;
	unsigned    image_draft     : 1;
	unsigned    image_frame     : 1;
	unsigned    image_mesh      : 1;
	unsigned    setup	    : 1;
/*++*/
	unsigned    preload_com	    : 1;
	unsigned    preload_pos	    : 1;
/*--*/
	/* V5.2-000	*/
	unsigned	plain_lbp	: 1;	/* true on /Kprint/Lbp	*/
    } kpri;

static struct {
	int	    blank_count;
	double	    chars_per_inch;
	int	    font_pitch;
	int	    font_size;
	int	    font_type;
	int	    image_per_inch;
	int	    kana_shift;
	int	    left_margin;
	int	    line_idx;
	int	    lines_per_inch;
	int	    page_idx;
	int	    page_num;
	int	    page_size;
	int	    page_width;
	int	    space;
	int	    space_count;
	int	    text_attr;
	int	    new_text_attr;
	int	    tmp_text_attr;
	unsigned    file_added		: 1;
	unsigned    first_page		: 1;
	unsigned    ff_only		: 1;
	unsigned    fortran		: 1;
	unsigned    header_line		: 1;
	unsigned    output_opend	: 1;
	unsigned    reset_overlay_form	: 1;
	unsigned    sixel		: 1;
	unsigned    sixel_begin		: 1;
	unsigned    sixel_end		: 1;
	unsigned    top_of_page		: 1;
/*++*/
	unsigned    preloaded           : 1;
/*--*/
    } mode;

/*#define MAX_ITEM_LIST	30					V5.1	*/
#define MAX_ITEM_LIST	48
static int item_list_index;
static struct ITEM_LIST {
	short buff_leng;
	short item_code;
	char *buff_addr;
	char *leng_addr;
    } item_list[MAX_ITEM_LIST];

static struct IOSB {
	unsigned stat;
	unsigned null;
    } iosb;

static unsigned char	lines_ln03 [2][2][6] =
		{{{ 131,  88,  66,  44,  33,  22 },	/* portlate   A4     */
		  { 125,  85,  63,  42,  32,  21 }},	/*	      LETTER */
		 {{  87,  59,  44,  29,  22,  15 },	/* landscape  A4     */
		  {  94,  63,  47,  32,  24,  16 }}};	/*	      LETTER */

static unsigned char	lines_ln80 [2][4][6] =
		{{{ 132,  88,  66,  44,  33,  22 },	/* portlate   A4     */
		  { 124,  82,  62,  41,  31,  20 },	/*	      LETTER */
		  { 114,  76,  57,  38,  28,  19 },	/*	      B5     */
		  { 160, 106,  80,  53,  40,  26 }},	/*	      REGAL  */
		 {{  90,  60,  45,  30,  22,  15 },	/* landscape  A4     */
		  {  94,  62,  47,  31,  23,  15 },	/*	      LETTER */
		  {  78,  52,  39,  26,  19,  13 },	/*	      B5     */
		  {  94,  62,  47,  31,  23,  15 }}};	/*	      REGAL  */

static double	size_ln03 [2][2][2] = {{{ 11.00,  7.20 },
					{ 10.50,  8.00 }},
				       {{  7.34, 10.50 },
					{  7.84,  9.60 }}};

static double	size_ln80 [2][4][2] = {{{ 11.00,  7.70 },
					{ 10.34,  8.00 },
					{  9.50,  6.60 },
					{ 13.34,  8.00 }},
				       {{  7.50, 11.20 },
					{  7.84, 10.50 },
					{  6.50,  9.70 },
					{  7.84, 13.50 }}};

static unsigned char pitch_to_para[] = { 0,0,6,5,4,0,3,0,2,0,0,0,1};

static double	print_time;

BOOL is_get_value;

main()
{
    return kpri$kprint();
}

kpri$kprint()
{
    kpri$command();
    kpri$queue_create();
    kpri$rms_init();
    kpri$print_file();
    kpri$queue_close();

    return SS$_NORMAL;
}

static kpri$command()
{
    int status;

    sys$gettim(&print_time);		/* get starting date/time. */

    kpri.device_type = DEVIL$_LA84;	/* Default device	*/

#ifdef __alpha /* Alpha: LA80,LN80,BROTHER and NEC are obsoleted devices */
    if      (cli$present(&sd_la84)    & STS$M_SUCCESS)
	kpri.device_type = DEVIL$_LA84;
    else if (cli$present(&sd_la86)    & STS$M_SUCCESS)
	kpri.device_type = DEVIL$_LA86;
    else if (cli$present(&sd_ln03)    & STS$M_SUCCESS)
	kpri.device_type = DEVIL$_LN03;
#else
    if      (cli$present(&sd_la80)    & STS$M_SUCCESS)
	kpri.device_type = DEVIL$_LA80;
    else if (cli$present(&sd_la84)    & STS$M_SUCCESS)
	kpri.device_type = DEVIL$_LA84;
    else if (cli$present(&sd_la86)    & STS$M_SUCCESS)
	kpri.device_type = DEVIL$_LA86;
    else if ((cli$present(&sd_ln80)   & STS$M_SUCCESS)
	     /*
	      * V5.2-000: Disable 2ku->10ku kanji code conversion
	      */
/*    	  || (cli$present(&sd_lbp)    & STS$M_SUCCESS))
	kpri.device_type = DEVIL$_LN80;				V5.1	*/

	     )
	kpri.device_type = DEVIL$_LN80;
    else if(cli$present(&sd_lbp)    & STS$M_SUCCESS)
	kpri.device_type = DEVIL$_LN80, kpri.plain_lbp = 1;

#if	0
    else if (cli$present(&sd_nec)     & STS$M_SUCCESS)
	kpri.device_type = DEVIL$_NEC;
    else if (cli$present(&sd_brother) & STS$M_SUCCESS)
	kpri.device_type = DEVIL$_BROTHER;
#endif	/* 0 */
    else if (cli$present(&sd_ln03)    & STS$M_SUCCESS)
	kpri.device_type = DEVIL$_LN03;
#endif

    kpri.identify   =(cli$present(&sd_identify)   & STS$M_SUCCESS) &&
                     (cli$present(&sd_log)        & STS$M_SUCCESS);

    /*
    ** /FORM
    */
    if (kpri.form = cli$get_value(&sd_form, &form_name) & STS$M_SUCCESS)
	kpri.is_form_number =
	    ots$cvt_ti_l (&form_name, &kpri.form_number) & STS$M_SUCCESS;

    /*
    ** /OUTPUT, /QUEUE
    */
    if (!(kpri.output = cli$get_value (&sd_output, &out_spc) & STS$M_SUCCESS))
	cli$get_value (&sd_queue, &queue_name);

    /*
    ** /FLAG
    */

/* NOFLAG をdefault にする
    kpri.flag_page = 0;
*/
    kpri.flag_page = -1;
    status = cli$present(&sd_flag_page);
    if (status & STS$M_SUCCESS) {
	kpri.flag_page =  1;
	if (cli$present(&sd_flag_page_one) & STS$M_SUCCESS)
	    kpri.flag_page =  2;
    }
    else
    if (status == CLI$_NEGATED)
	kpri.flag_page = -1;

    /*
    ** /COVER_PAGE
    */

/* COVER をdefault にする
    kpri.cover_page_com = 0;
*/
/*    kpri.cover_page_com = 1;	V5.1	*/
    kpri.cover_page_com = kpri$l_cover_page_default_value;
    status = cli$present(&sd_cover_page);
    if (status & STS$M_SUCCESS) {
	kpri.cover_page_com =  1;
	if (cli$present(&sd_cover_page_one) & STS$M_SUCCESS)
	    kpri.cover_page_com =  2;
    }
    else
    if (status == CLI$_NEGATED)
	kpri.cover_page_com = -1;

    kpri.notify     = cli$present(&sd_notify)     & STS$M_SUCCESS;

/*++*/
    /*
    ** /PRELOAD
    */
    kpri.preload_com = cli$present(&sd_preload) & STS$M_SUCCESS;
/*--*/
}

static kpri$command_positional()
{
    int status;

    kpri.vertical   = cli$present(&sd_vertical)   & STS$M_SUCCESS;
    kpri.ascii      = cli$present(&sd_ascii)      & STS$M_SUCCESS;
    kpri.jis        = cli$present(&sd_jis)        & STS$M_SUCCESS;
    kpri.letter     = cli$present(&sd_letter)     & STS$M_SUCCESS;
    kpri.high_speed = cli$present(&sd_high_speed) & STS$M_SUCCESS;
    kpri.slant      = cli$present(&sd_slant)      & STS$M_SUCCESS;
    kpri.delete     = cli$present(&sd_delete)     & STS$M_SUCCESS;
    kpri.header     = cli$present(&sd_header)     & STS$M_SUCCESS;
    kpri.odd_page   = cli$present(&sd_odd_page)   & STS$M_SUCCESS;
    kpri.even_page  = cli$present(&sd_even_page)  & STS$M_SUCCESS;
    kpri.feed       = cli$present(&sd_feed)       & STS$M_SUCCESS;
    kpri.sixel      = cli$present(&sd_sixel)      & STS$M_SUCCESS;
    kpri.landscape  = cli$present(&sd_landscape)  & STS$M_SUCCESS;
    kpri.passall    = cli$present(&sd_passall)    & STS$M_SUCCESS;
    kpri.manual     = cli$present(&sd_manual)     & STS$M_SUCCESS;
    kpri.command    = kpri.manual ||
		      cli$present(&sd_command)    & STS$M_SUCCESS;

    kpri.full_paint = kpri.landscape;
#ifdef __alpha /* Alpha: doesn't support this qualifier */
    kpri.full_paint = 0;
#else
    status = cli$present(&sd_full_paint);
    if (status == CLI$_LOCPRES || status == CLI$_PRESENT)
	kpri.full_paint = 1;
    else
    if (status == CLI$_LOCNEG || status == CLI$_NEGATED)
	kpri.full_paint = 0;
#endif

    /*
    ** /FLAG
    */
    kpri.flag_page = 0;
    status = cli$present(&sd_flag_page);
    if (status == CLI$_LOCPRES)
	kpri.flag_page =  1;
    else
    if (status == CLI$_LOCNEG)
	kpri.flag_page = -1;

    /*
    ** /COVER
    */
    kpri.cover_page = 0;
    status = cli$present(&sd_cover_page);
    if (status == CLI$_LOCPRES)
	kpri.cover_page =  1;
    else
    if (status == CLI$_LOCNEG)
	kpri.cover_page = -1;

    /*
    ** /IMAGE
    */
    kpri.image_draft = kpri.image_frame = kpri.image_mesh = 0;
    if (cli$present(&sd_image) & STS$M_SUCCESS) {
	kpri.image_draft = cli$present(&sd_image_draft) & STS$M_SUCCESS;
	kpri.image_frame = cli$present(&sd_image_frame) & STS$M_SUCCESS;
	kpri.image_mesh  = cli$present(&sd_image_mesh)  & STS$M_SUCCESS;
    }

    /*
    ** /CHECK
    */
    kpri.check_level_II = kpri.check_expanded = kpri.check_keisen = 0;
    if (cli$present(&sd_check) & STS$M_SUCCESS) {
	kpri.check_level_II = cli$present(&sd_check_level_II) & STS$M_SUCCESS;
	kpri.check_expanded =(cli$present(&sd_check_expanded) & STS$M_SUCCESS) ||
			     (cli$present(&sd_check_extended) & STS$M_SUCCESS);
	kpri.check_keisen   = cli$present(&sd_check_keisen)   & STS$M_SUCCESS;
	if (!kpri.check_level_II && !kpri.check_expanded && !kpri.check_keisen)
	    kpri.check_level_II = kpri.check_expanded = 1;
	if (cli$present(&sd_check_none) & STS$M_SUCCESS)
	    kpri.check_level_II = kpri.check_expanded = kpri.check_keisen = 0;
	if (cli$present(&sd_check_all) & STS$M_SUCCESS)
	    kpri.check_level_II = kpri.check_expanded = kpri.check_keisen = 1;
    }

    get_integer_value(&sd_line_pitch, &kpri.line_pitch,     DEF_LINE_PITCH);
    get_integer_value(&sd_char_pitch, &kpri.char_pitch,     DEF_CHAR_PITCH);
    get_integer_value(&sd_top,        &kpri.top,            DEF_TOP);
    get_integer_value(&sd_bottom,     &kpri.bottom,         DEF_BOTTOM);
    get_integer_value(&sd_left,       &kpri.left,           DEF_LEFT);
    get_integer_value(&sd_right,      &kpri.right,          DEF_RIGHT);
    get_integer_value(&sd_copies,     &kpri.copies,         DEF_COPIES);
    get_integer_value(&sd_resolution, &kpri.image_per_inch, DEF_IMAGE_PER_INCH);

/* /LINE_PITCH=??? のバグ取り */
    if ( kpri . line_pitch != DEF_LINE_PITCH )
	switch ( kpri . device_type ) {
	case DEVIL$_LA84:
	case DEVIL$_LA86:
	case DEVIL$_LN03:
	case DEVIL$_LN80:
	    if ( kpri . line_pitch < 1 || 6 < kpri . line_pitch ) {
		put_param_error( "LINE_PITCH", kpri . line_pitch );
		kpri . line_pitch = 3;
	    }
	    break;
	case DEVIL$_NEC:
	case DEVIL$_BROTHER:
	    if ( kpri . line_pitch < 1 || 4 < kpri . line_pitch ) {
		put_param_error( "LINE_PITCH", kpri . line_pitch );
		kpri . line_pitch = 3;
	    }
	    break;
	default:
	    break;
	}
/* /CHARACTER_PITCH=??? のバグ取り */
    if ( kpri . char_pitch != DEF_CHAR_PITCH )
	switch ( kpri . device_type ) {
	case DEVIL$_LA80:
	    if ( kpri . char_pitch != 1 && kpri . char_pitch != 4 ) {
		put_param_error( "CHARACTER_PITCH", kpri . char_pitch );
		kpri . char_pitch = 1;
	    }
	    break;
	case DEVIL$_LA84:
	case DEVIL$_LA86:
	    if ( kpri . char_pitch < 1 || 4 < kpri . char_pitch ) {
		put_param_error( "CHARACTER_PITCH", kpri . char_pitch );
		kpri . char_pitch = 1;
	    }
	    break;
	case DEVIL$_LN03:
	case DEVIL$_LN80:
	    if ( kpri . char_pitch < 1 || 5 < kpri . char_pitch ) {
		put_param_error( "CHARACTER_PITCH", kpri . char_pitch );
		kpri . char_pitch = 2;
	    }
	    break;
	case DEVIL$_NEC:
	    if ( kpri . char_pitch < 1 || 3 < kpri . char_pitch ) {
		put_param_error( "CHARACTER_PITCH", kpri . char_pitch );
		kpri . char_pitch = 1;
	    }
	    break;
	case DEVIL$_BROTHER:
	    if ( kpri . char_pitch != 1 && kpri . char_pitch != 2 ) {
		put_param_error( "CHARACTER_PITCH", kpri . char_pitch );
		kpri . char_pitch = 1;
	    }
	    break;
	default:
	    break;
	}
/* /TOP=??? のバグ取り */
    if ( kpri . top < 0 ) {
        put_param_error( "TOP", kpri . top );
        kpri . top = DEF_TOP;
    }
/* /BOTTOM=??? のバグ取り */
    if ( kpri . bottom < 0 ) {
        put_param_error( "BOTTOM", kpri . bottom );
        kpri . bottom = DEF_BOTTOM;
    }
/* /LEFT=??? のバグ取り */
    if ( kpri . left < 0 ) {
        put_param_error( "LEFT", kpri . left );
        kpri . left = DEF_LEFT;
    }
/* /RESOLUTION=??? のバグ取り */
    if ( kpri . image_per_inch != DEF_IMAGE_PER_INCH )
	switch ( kpri . device_type ) {
	case DEVIL$_LA84:
	case DEVIL$_LA86:
	    if ( kpri . image_per_inch != 144 &&
		 kpri . image_per_inch != 180 ) {
		put_param_error( "RESOLUTION", kpri . image_per_inch );
		kpri . image_per_inch = 144;
	    }
	    break;
	case DEVIL$_LN03:
	    if ( kpri . image_per_inch != 30 &&
		 kpri . image_per_inch != 50 &&
		 kpri . image_per_inch != 60 &&
		 kpri . image_per_inch != 75 &&
		 kpri . image_per_inch != 100 &&
		 kpri . image_per_inch != 150 &&
		 kpri . image_per_inch != 300 ) {
		put_param_error( "RESOLUTION", kpri . image_per_inch );
		kpri . image_per_inch = 150;
	    }
	    break;
	case DEVIL$_LN80:
	    if ( kpri . image_per_inch != 60 &&
		 kpri . image_per_inch != 80 &&
		 kpri . image_per_inch != 120 &&
		 kpri . image_per_inch != 240 ) {
		put_param_error( "RESOLUTION", kpri . image_per_inch );
		kpri . image_per_inch = 120;
	    }
	    break;
	default:
	    break;
	}

    if (kpri.device_type == DEVIL$_LN80 || kpri.device_type == DEVIL$_LN03) {
	if (cli$get_value (&sd_size, &wrkd) & STS$M_SUCCESS) {
	    if (!(ots$cvt_ti_l (&wrkd, &kpri.size_type) & STS$M_SUCCESS))
		kpri.size_type = DEF_SIZE_TYPE;
	    else if (kpri.size_type < SIZE_TYPE_FROM || kpri.size_type > SIZE_TYPE_TO)
		kpri.size_type = DEF_SIZE_TYPE;
	    if (kpri.device_type == DEVIL$_LN03 && kpri.size_type > SIZE_TYPE_BAS + DEVIL$V_SIZE_LETTER)
		kpri.size_type = DEF_SIZE_TYPE;
	}
	else
	    kpri.size_type = DEF_SIZE_TYPE;
	kpri.size_type -= SIZE_TYPE_BAS;
	if (kpri.device_type == DEVIL$_LN03) {
	    kpri.size_v = size_ln03[kpri.landscape][kpri.size_type][0];
	    kpri.size_h = size_ln03[kpri.landscape][kpri.size_type][1];
	}
	else {
	    kpri.size_v = size_ln80[kpri.landscape][kpri.size_type][0];
	    kpri.size_h = size_ln80[kpri.landscape][kpri.size_type][1];
	}
    }
    else {
	get_float_value(&sd_size,   &kpri.size_v,       DEF_SIZE_V);
	get_float_value(&sd_size,   &kpri.size_h,       DEF_SIZE_H);
    }

    /*
    ** /FONT
    */
    kpri.font_pitch = 1500;
    kpri.font_size  = (kpri.device_type == DEVIL$_LN03) ? 100 : 96;
    kpri.font_type  = 80;
    if (cli$present(&sd_font) & STS$M_SUCCESS) {
	if (cli$present(&sd_font_pitch) & STS$M_SUCCESS) {
	    get_integer_value(&sd_font_pitch, &kpri.font_pitch, 15);
	    kpri.font_pitch *= 100;
	}
	if (cli$present(&sd_font_size) & STS$M_SUCCESS) {
	    get_integer_value(&sd_font_size,  &kpri.font_size,  10);
	    if (kpri.device_type != DEVIL$_LN03)
		kpri.font_size *= 10;
	}
	if (cli$present(&sd_font_type) & STS$M_SUCCESS) {
	    if (cli$present(&sd_font_type_line_printer) & STS$M_SUCCESS)
		kpri.font_type = 0;
	    else
	    if (cli$present(&sd_font_type_pica) & STS$M_SUCCESS)
		kpri.font_type = 1;
	    else
	    if (cli$present(&sd_font_type_elite) & STS$M_SUCCESS)
		kpri.font_type = 2;
	    else
	    if (cli$present(&sd_font_type_courier) & STS$M_SUCCESS)
		kpri.font_type = 3;
	    else
	    if (cli$present(&sd_font_type_mincho) & STS$M_SUCCESS)
		kpri.font_type = 80;
	    else
	    if (cli$present(&sd_font_type_user) & STS$M_SUCCESS)
		get_integer_value(&sd_font_type_user, &kpri.font_type, 80);
	}
    }
    /*
    ** /LENGTH
    */
    kpri.page_length = 0;
    if (cli$present(&sd_length) & STS$M_SUCCESS) {
	get_integer_value(&sd_length, &kpri.page_length, DEF_PAGE_LENGTH);
/* /LENGTH=??? のバグ取り */
        if ( kpri . page_length <= 0 ) {
            put_param_error( "LENGTH", kpri . page_length );
            kpri . page_length = DEF_PAGE_LENGTH;
        }
    }
    /*
    ** /PITCH
    */
    kpri.lines_per_inch = 0;
    if (cli$present(&sd_pitch) & STS$M_SUCCESS)
	get_integer_value(&sd_pitch, &kpri.lines_per_inch, DEF_LINES_PER_INCH);

    /*
    ** /PAGE
    */
    kpri.page_start    = DEF_PAGE_START;
    kpri.page_end      = DEF_PAGE_END;
    kpri.page_initial  = DEF_PAGE_INITIAL;
    kpri.page_position = DEF_PAGE_POSITION;
    kpri.page_top    = 0;
    kpri.page_bottom = 0;
    if (kpri.page = cli$present(&sd_page) & STS$M_SUCCESS) {
	get_integer_value(&sd_page_start,    &kpri.page_start,    DEF_PAGE_START);
	get_integer_value(&sd_page_end,      &kpri.page_end,      DEF_PAGE_END);
	get_integer_value(&sd_page_initial,  &kpri.page_initial,  DEF_PAGE_INITIAL);
	get_integer_value(&sd_page_position, &kpri.page_position, DEF_PAGE_POSITION);
	get_integer_value(&sd_page_type,     &kpri.page_type,     DEF_PAGE_TYPE);
/* /PAGE=(START=???) のバグ取り */
	if ( kpri . page_start <= 0 ) {
	    put_param_error( "PAGE=(START)", kpri . page_start );
	    kpri . page_start = DEF_PAGE_START;
	}
/* /PAGE=(END=???) のバグ取り */
	if ( kpri . page_end <= 0 || kpri . page_end < kpri . page_start ) {
	    put_param_error( "PAGE=(END)", kpri . page_end );
	    kpri . page_end = DEF_PAGE_END;
	}
/* /PAGE=(INITIAL=???) のバグ取り */
	if ( kpri . page_initial <= 0 ) {
	    put_param_error( "PAGE=(INITIAL)", kpri . page_initial );
	    kpri . page_initial = DEF_PAGE_INITIAL;
	}
/* /PAGE=(POSITION=???) のバグ取り */
	if ( kpri . page_position <= 0 ) {
	    put_param_error( "PAGE=(POSITION)", kpri . page_position );
	    kpri . page_position = DEF_PAGE_POSITION;
	}
/* /PAGE=(TYPE=???) のバグ取り */
	if ( kpri . page_type != 1 && kpri . page_type != 2 ) {
	    put_param_error( "PAGE=(TYPE)", kpri . page_type );
	    kpri . page_type = DEF_PAGE_TYPE;
	}

	kpri.page_top = 1;
/* /PAGE=(TOP,BOTTOM) のバグ取り */
	if (cli$present(&sd_page_bottom) & STS$M_SUCCESS)
            if ( !( cli$present( &sd_page_top ) & STS$M_SUCCESS ) )
	        kpri.page_top = 0;

	kpri.page_bottom = !kpri.page_top;
	if (kpri.page_before = cli$present(&sd_page_before) & STS$M_SUCCESS)
	    cli$get_value(&sd_page_before, &page_before);
	if (kpri.page_after = cli$present(&sd_page_after) & STS$M_SUCCESS)
	    cli$get_value(&sd_page_after, &page_after);
	if (!kpri.page_before && !kpri.page_after) {
	    if (kpri.page_type == 2) {
		kpri.page_before = kpri.page_after = 1;
		ots$scopy_dxdx(&sd_page_type_2_before, &page_before);
		ots$scopy_dxdx(&sd_page_type_2_after,  &page_after);
	    }
	    else {
		kpri.page_before = 1;
		kpri.page_after  = 0;
		ots$scopy_dxdx(&sd_page_type_1_before, &page_before);
	    }
	}
    }

    kpri.page_position --;
    if (kpri.page_position < 0) kpri.page_position = 0;

    /*
    ** /PRINT
    */
    kpri . print_start = DEF_PRINT_START;
    kpri . print_end = DEF_PRINT_END;
    if (cli$present(&sd_print) & STS$M_SUCCESS) {
	get_integer_value( &sd_print_start, &kpri . print_start, DEF_PRINT_START );
	get_integer_value( &sd_print_end, &kpri . print_end, DEF_PRINT_END );
/* /PRINT=??? のバグ取り */
	if ( kpri . print_start <= 0 ) {
	    put_param_error( "PRINT", kpri . print_start );
	    kpri . print_start = DEF_PRINT_START;
	}
	if ( kpri . print_end <= 0 || kpri . print_end < kpri . print_start ) {
	    put_param_error( "PRINT", kpri . print_end );
	    kpri . print_end = DEF_PRINT_END;
	}
    }

    /*
    ** /TITLE
    */
    if (kpri.title = cli$present(&sd_title) & STS$M_SUCCESS) {
	cli$get_value(&sd_title, &title);
	get_integer_value(&sd_title, &kpri.title_position, DEF_TITLE_POSITION);
/* /TITLE=??? のバグ取り */
	if ( kpri . title_position <= 0 ) {
	    put_param_error( "TITLE", kpri . title_position );
	    kpri . title_position = DEF_TITLE_POSITION;
	}
	if (kpri.title_position > 0)
	    kpri.title_position --;
	else {
	    kpri.title_position =(- kpri.title_position)
				  - kpri$calc_col(0, title.dsc$a_pointer,
                                                     title.dsc$w_length);
	    if (kpri.title_position < 0) kpri.title_position = 0;
	}
    }

    /*
    ** /FOOTER
    */
    if (kpri.footer = cli$present(&sd_footer) & STS$M_SUCCESS) {
	cli$get_value(&sd_footer, &footer);
	get_integer_value(&sd_footer, &kpri.footer_position, DEF_FOOTER_POSITION);
/* /FOOTER=??? のバグ取り */
        if ( kpri . footer_position <= 0 ) {
            put_param_error( "FOOTER", kpri . footer_position );
            kpri . footer_position = DEF_FOOTER_POSITION;
        }
	if (kpri.footer_position > 0)
	    kpri.footer_position --;
	else {
	    kpri.footer_position =(- kpri.footer_position)
                                   - kpri$calc_col(0, footer.dsc$a_pointer,
                                                      footer.dsc$w_length);
	    if (kpri.footer_position < 0) kpri.footer_position = 0;
       }
    }

    /*
    ** /ASPECT_RATIO
    */
    if (cli$present(&sd_aspect_ratio) & STS$M_SUCCESS) {
	if (get_integer_value(&sd_aspect_ratio, &kpri.aspect_ratio_v, 1))
	    get_integer_value(&sd_aspect_ratio, &kpri.aspect_ratio_h, 1);
/* /ASPECT_RATIO=??? のバグ取り */
	switch ( kpri . device_type ) {
	case DEVIL$_LA84:
	case DEVIL$_LA86:
	    if ( kpri . aspect_ratio_v != 1 && kpri . aspect_ratio_v != 2 ) {
		put_param_error( "ASPECT_RATIO=(縦の比率)", kpri . aspect_ratio_v );
		kpri . aspect_ratio_v = 1;
	    }
	    if ( kpri . aspect_ratio_h != 1 ) {
		put_param_error( "ASPECT_RATIO=(横の比率)", kpri . aspect_ratio_h );
		kpri . aspect_ratio_h = 1;
	    }
	    break;
	case DEVIL$_LN03:
	    if ( kpri . aspect_ratio_v < 1 ) {
		put_param_error( "ASPECT_RATIO=(縦の比率)", kpri . aspect_ratio_v );
		kpri . aspect_ratio_v = 1;
	    }
	    if ( kpri . aspect_ratio_h < 1 ) {
		put_param_error( "ASPECT_RATIO=(横の比率)", kpri . aspect_ratio_h );
		kpri . aspect_ratio_h = 1;
	    }
	    break;
	case DEVIL$_LN80:
	    if ( kpri . aspect_ratio_v < 1 ) {
		put_param_error( "ASPECT_RATIO=(縦の比率)", kpri . aspect_ratio_v );
		kpri . aspect_ratio_v = 1;
	    }
	    if ( kpri . aspect_ratio_h != 1 ) {
		put_param_error( "ASPECT_RATIO=(横の比率)", kpri . aspect_ratio_h );
		kpri . aspect_ratio_h = 1;
	    }
	    break;
	default:
	    break;
	}
    }
    /*
    ** /KEISEN
    */
    if (kpri.keisen = cli$present(&sd_keisen) & STS$M_SUCCESS) {
	get_integer_value(&sd_keisen, &kpri.keisen_width, 0);
#ifdef __alpha /* Alpha : Second value of /KEISEN is obsoleted */
	kpri.keisen_line = 0;
#else
	get_integer_value(&sd_keisen, &kpri.keisen_line,  0);
#endif
/* /KEISEN=??? のバグ取り */
	if ( kpri . keisen_width != 0 || kpri . keisen_line != 0 )
	    switch ( kpri . device_type ) {
	    case DEVIL$_LA80:
	    case DEVIL$_LA84:
	    case DEVIL$_LA86:
	    case DEVIL$_LN03:
		if ( kpri . keisen_width < 0 || kpri . keisen_width > 2 ) {
		    put_param_error( "KEISEN", kpri . keisen_width );
		    kpri . keisen_width = 1;
		}
		break;
	    case DEVIL$_LN80:
		if ( kpri . keisen_width < 0 || kpri . keisen_width > 9 ) {
		    put_param_error( "KEISEN", kpri . keisen_width );
		    kpri . keisen_width = 1;
		}
		if ( kpri . keisen_line != 0 && kpri . keisen_line != 1 ) {
		    put_param_error( "KEISEN", kpri . keisen_line );
		    kpri . keisen_line = 0;
		}
		break;
	    default:
		break;
	    }
    }

    /*
    ** /REGISTER_FORM
    */
    kpri.register_page = 0;
    if (cli$present(&sd_register_form) & STS$M_SUCCESS) {
#ifdef __alpha  /* Alpha : parameter for /REGISTER_FORM is obsoleted */
	kpri.register_page = 1;
#else
	get_integer_value(&sd_register_form, &kpri.register_page, 1);
#endif
/* /REGISTER_FORM=??? のバグ取り */
        if ( kpri . register_page != 1 && kpri . register_page != 2 ) {
            put_param_error( "REGISTER_FORM", kpri . register_page );
            kpri . register_page = 1;
        }
    }

    /*
    ** /OVERLAY_FORM
    */
    kpri.overlay_page = 0;
    if (cli$present(&sd_overlay_form) & STS$M_SUCCESS) {
#ifdef __alpha /* Alpha : /OVERLAY_FORM parameters are obsoleted */
	kpri.overlay_page = 1;
	kpri.overlay_second_page = 0;
#else
	get_integer_value(&sd_overlay_form, &kpri.overlay_page, 1);
	get_integer_value(&sd_overlay_form, &kpri.overlay_second_page, 0);
#endif
/* /OVERLAY_FORM=??? のバグ取り */
        if ( kpri . overlay_page != 1 && kpri . overlay_page != 2 ) {
            put_param_error( "OVERLAY_FORM", kpri . overlay_page );
            kpri . overlay_page = 1;
        }
        if ( kpri . overlay_second_page < 0 ||
             kpri . overlay_second_page > 2 ) {
            put_param_error( "OVERLAY_FORM", kpri . overlay_second_page );
            kpri . overlay_second_page = 0;
        }
    }

    /*
    ** /SETUP
    */
    if (kpri.setup = cli$present(&sd_setup) & STS$M_SUCCESS) {
	cli$get_value(&sd_setup, &setup_modules);
	while (cli$get_value(&sd_setup, &wrkd) & STS$M_SUCCESS) {
	    str$append(&setup_modules, &sd_comma);
	    str$append(&setup_modules, &wrkd);
	}
    }

    /*
    ** /TABLE
    */
    if (kpri.table = cli$present(&sd_table) & STS$M_SUCCESS) {
	kpri.output = 1;
	ots$scopy_dxdx(&sd_null_output, &out_spc);
    }

/*++*/
    /*
    ** /PRELOAD
    */
    status = cli$present(&sd_preload);
    kpri.preload_pos = (status == CLI$_LOCPRES);
/*--*/
}

static get_integer_value(qualifier, val, def)
struct dsc$descriptor *qualifier;
int                   *val;
int                    def;
{
    int status;

    *val = def;
    if (status = cli$get_value (qualifier, &wrkd) & STS$M_SUCCESS)
	if (!(ots$cvt_ti_l (&wrkd, val) & STS$M_SUCCESS))
	    *val = def;
/*
    if (*val <= 0) *val = def;
*/
    return status;
}

static get_float_value(qualifier, val, def)
struct dsc$descriptor	*qualifier;
double			*val;
double			def;
{
    int status;

    *val = def;
    if (status = cli$get_value (qualifier, &wrkd) & STS$M_SUCCESS)
#if defined(__G_FLOAT) /* double as G_FLOAT (Alpha AXP) */
    if (!(ots$cvt_t_g (&wrkd, val) & STS$M_SUCCESS))
#else /* double as D_FLOAT (VAX) */
    if (!(ots$cvt_t_d (&wrkd, val) & STS$M_SUCCESS))
#endif
	    *val = def;
    return status;
}

static reset_item_list()
{
    item_list_index = 0;
}

static add_item_list(item_code, buff_leng, buff_addr, leng_addr)
short item_code;
short buff_leng;
char *buff_addr;
char *leng_addr;
{
    item_list[item_list_index].buff_leng = buff_leng;
    item_list[item_list_index].item_code = item_code;
    item_list[item_list_index].buff_addr = buff_addr;
    item_list[item_list_index].leng_addr = leng_addr;
    item_list_index ++;
}

static kpri$queue_create()
{
    int status;
    int context;
    int jobname_length;
    char *jobname_pointer;
    char *str;
    extern BOOL is_get_value;
    extern struct dsc$descriptor fil_spc;
    extern struct dsc$descriptor res_spc;

/*
! コマンド・ラインからファイル名を１つ取りだす。
! 成功ならば is_get_value はTRUE, そうでなければFALSE になる。
*/
    is_get_value = cli$get_value( &sd_p1, &fil_spc ) & STS$M_SUCCESS;

    if (kpri.output) return;

    reset_item_list();
    add_item_list(SJC$_QUEUE,
		    queue_name.dsc$w_length,
		    queue_name.dsc$a_pointer,
		    0);

/* JOB 名を与える */
    if ( is_get_value ) {
        context = 0;
        if ( lib$find_file( &fil_spc, &res_spc, &context,
/*                0, 0, 0, 0 ) & STS$M_SUCCESS ) {		V5.1 */
                &sd_input_default, 0, 0, 0 ) & STS$M_SUCCESS ) {
/* ノード名またはデバイス名をスキップする */
            for ( str = res_spc . dsc$a_pointer; *str != ':'; ++str )
                ;
            ++str;              /* ':' をスキップ */
            if ( *str == ':' ) {
/* デバイス名をスキップする */
                for ( ++str; *str != ':'; ++str )
                    ;
                ++str;          /* ':' をスキップ */
            }
/* ディレクトリィ名をスキップする */
            if ( *str == '[' ) {
                for ( ++str; *str != ']'; ++str )
                    ;
                ++str;          /* ']' をスキップ */
            }
/* ファイル名のみをとりだして，ジョブ名にする */
            jobname_pointer = str;
            jobname_length = 0;
            while ( *str++ != '.' ) ++jobname_length;
            add_item_list( SJC$_JOB_NAME, jobname_length, jobname_pointer, 0 );
        }
        lib$find_file_end( &context );
    }

    if (kpri.notify)
	add_item_list(SJC$_NOTIFY, 0, 0, 0);

    if (kpri.flag_page > 0) {
	if (kpri.flag_page == 1)
	    add_item_list(SJC$_FILE_FLAG, 0, 0, 0);
	else
	    add_item_list(SJC$_FILE_FLAG_ONE, 0, 0, 0);
    }
    else
    if (kpri.flag_page < 0)
	add_item_list(SJC$_NO_FILE_FLAG, 0, 0, 0);

    if (kpri.form)
	if (kpri.is_form_number)
	    add_item_list(SJC$_FORM_NUMBER, 4, &kpri.form_number, 0);
	else
	    add_item_list(SJC$_FORM_NAME,
			    form_name.dsc$w_length,
			    form_name.dsc$a_pointer,
			    0);

    add_item_list(0, 0, 0, 0);

    if (!((status = sys$sndjbcw(0, SJC$_CREATE_JOB, 0, item_list, &iosb, 0, 0))
                   & STS$M_SUCCESS))
	lib$stop(status);

    if (!(iosb.stat & STS$M_SUCCESS))
	lib$stop(iosb.stat);

    mode.file_added = 0;
}

static kpri$queue_add_file()
{
    int status;

    if (kpri.output) return;

    reset_item_list();
    add_item_list(SJC$_FILE_IDENTIFICATION,
		    NAM$C_DVI + 6 + 6,
		    &outnam.nam$t_dvi,
		    0);
    add_item_list(SJC$_DELETE_FILE,         0, 0, 0);
    add_item_list(SJC$_NO_PAGINATE,         0, 0, 0);
    add_item_list(SJC$_FILE_COPIES,         4, &kpri.copies, 0);
/*++	for HEADER
!    if (kpri.header)
!	add_item_list(SJC$_PAGE_HEADER, 0, 0, 0);
--*/
    if (kpri.flag_page > 0)
	add_item_list(SJC$_FILE_FLAG, 0, 0, 0);
    else
    if (kpri.flag_page < 0)
	add_item_list(SJC$_NO_FILE_FLAG, 0, 0, 0);

    if (kpri.setup)
	add_item_list(SJC$_FILE_SETUP_MODULES, setup_modules.dsc$w_length,
					       setup_modules.dsc$a_pointer, 0);

    add_item_list(0, 0, 0, 0);

    if (!((status = sys$sndjbcw(0, SJC$_ADD_FILE, 0, item_list,
                               &iosb, 0, 0)) & STS$M_SUCCESS))
	lib$stop(status);

    if (!(iosb.stat & STS$M_SUCCESS))
	lib$stop(iosb.stat);

    mode.file_added = 1;
}

static kpri$queue_passall_file()
{
    int status;

    if (kpri.output) return;

    reset_item_list();
    add_item_list(SJC$_FILE_SPECIFICATION,
		    res_spc.dsc$w_length,
		    res_spc.dsc$a_pointer,
		    0);
    if (kpri.delete)
	add_item_list(SJC$_DELETE_FILE,         0, 0, 0);
    add_item_list(SJC$_PASSALL,             0, 0, 0);
    add_item_list(SJC$_NO_PAGINATE,         0, 0, 0);
    add_item_list(SJC$_FILE_COPIES,         4, &kpri.copies, 0);
/*++	for HEADER
!    if (kpri.header)
!	add_item_list(SJC$_PAGE_HEADER, 0, 0, 0);
--*/
    if (kpri.flag_page > 0)
	add_item_list(SJC$_FILE_FLAG, 0, 0, 0);
    else
    if (kpri.flag_page < 0)
	add_item_list(SJC$_NO_FILE_FLAG, 0, 0, 0);

    if (kpri.setup)
	add_item_list(SJC$_FILE_SETUP_MODULES, setup_modules.dsc$w_length,
					       setup_modules.dsc$a_pointer, 0);

    add_item_list(0, 0, 0, 0);

    if (!((status = sys$sndjbcw(0, SJC$_ADD_FILE, 0, item_list,
                               &iosb, 0, 0)) & STS$M_SUCCESS))
	lib$stop(status);

    if (!(iosb.stat & STS$M_SUCCESS))
	lib$stop(iosb.stat);

    mode.file_added = 1;
}

static kpri$queue_close()
{
	/*
	 * V5.2-000:	80 Bytes is tooo small to print JBC messages.
	 */
/*    char status_buf [80];	V5.1 */
    char status_buf [256];
    struct dsc$descriptor status_msg = {sizeof status_buf,
                                       DSC$K_DTYPE_T, DSC$K_CLASS_S,
                                       status_buf};

    int status;

    if (kpri.output) return;

    reset_item_list();
    if (kpri.identify)
	add_item_list(SJC$_JOB_STATUS_OUTPUT,
			status_msg.dsc$w_length,
			status_msg.dsc$a_pointer,
			&status_msg.dsc$w_length);
    add_item_list(0, 0, 0, 0);

    if (!((status = sys$sndjbcw(0, SJC$_CLOSE_JOB, 0, item_list, &iosb, 0, 0))
                   & STS$M_SUCCESS))
	lib$stop (status);

    if (!(iosb.stat & STS$M_SUCCESS))
	lib$stop (iosb.stat);

    if (kpri.identify && mode.file_added) lib$put_output (&status_msg);
}

static int exit_condition_value;
static struct {
	int forward_link;
	int (*exit_handler)();
	int arg_count;
	int *condition_addr;
    } exit_control_block;

static kpri$exit_handler()
{
    int status;

    if (!kpri.output && outfab.fab$w_ifi != 0) {
	outfab.fab$l_fop |= FAB$M_DLT;
	if (!((status = sys$close(&outfab)) & STS$M_SUCCESS))
	    kpri$rms_error(KPRI$_CLOSEOUT, status, &outres_spc);
    }

    return SS$_NORMAL;
}

static kpri$rms_init()
{
    int status;

    kpri$rms_init_input(&tmpfab, &tmprab, &tmpnam);
    kpri$rms_init_output(&outfab, &outrab, &outnam);
    if (!kpri.output) {
/* 幽霊ファイルにしないため
	outfab.fab$l_fop |= FAB$M_TMP;
*/
	exit_control_block.exit_handler   = kpri$exit_handler;
	exit_control_block.arg_count      = 1;
	exit_control_block.condition_addr = &exit_condition_value;
	if (!((status = sys$dclexh(&exit_control_block)) & STS$M_SUCCESS))
	    lib$stop(status);
    }
}

#define NULL 0

kpri$print_file()
{
    int context;
    int flag;
    int status;
    int stv;
    char tmp_file_name[ 20 + 1 ];
    extern BOOL is_get_value;

    ots$scopy_dxdx(&sd_input_default, &def_spc);
    if (kpri.output)
	ots$scopy_dxdx(&sd_output_default, &out_def);
    else
	ots$scopy_dxdx(&sd_output_tmp_def, &out_def);

    context = 0;
    flag    = 2;
    mode.output_opend = 0;
    mode.reset_overlay_form = 1;

/* オリジナル
    while (cli$get_value(&sd_p1, &fil_spc) & STS$M_SUCCESS) {
*/

    while ( is_get_value ) {
/*++ ++*/
	mode.preloaded = 0;
/*-- --*/
	kpri$command_positional();
	while ((status = lib$find_file (&fil_spc, &res_spc, &context,
                          &def_spc, &rel_spc, &stv, &flag)) & STS$M_SUCCESS) {

	    if (kpri.passall) {
		kpri$queue_passall_file();
		continue;
	    }

	    xabfhc = cc$rms_xabfhc;
	    xabrdt = cc$rms_xabrdt;
	    xabpro = cc$rms_xabpro;
	    xabfhc.xab$l_nxt = &xabrdt;
	    xabrdt.xab$l_nxt = &xabpro;
	    xabpro.xab$l_nxt = 0;
	    kpri$rms_init_input(&infab, &inrab, &innam, &xabfhc);
	    if (kpri.delete)
		infab.fab$l_fop |= FAB$M_DLT;
	    if (!((status = kpri$rms_open_input(&infab, &inrab, &innam,
                                             &res_spc,
                                             &sd_input_default,
                                             &inres_spc))
                         & STS$M_SUCCESS)) {
		kpri$rms_error(KPRI$_OPENIN, status, &inres_spc);
		continue;
	    }
	    kpri$rms_set_input_rab(&inrab);
	    mode.fortran = infab.fab$b_rat & FAB$M_FTN;

	    if (!kpri.output || !mode.output_opend) {
		if (!kpri.output) {
/* 中間ファイルの名前を決める */
		    int item_code;
		    int pid;
		    int *fp, *fopen();
		    double timadr;

		    do {
			strcpy( tmp_file_name, "KPRIXXXXZZZZZZZZ.TMP" );
/* process id を求める */
			item_code = JPI$_PID;
			lib$getjpi( &item_code, 0, 0, &pid, 0, 0 );
			convert_and_copy( &pid, 2, tmp_file_name + 4 );
/* 絶対時間を求める */
			sys$gettim( &timadr );
			convert_and_copy( &timadr, 4, tmp_file_name + 8 );
/* ファイルが既にあるかチェックする */
			fp = fopen( tmp_file_name, "r" );
			if ( fp != NULL ) fclose( fp );
		    } while ( fp != NULL );
/* オリジナル
		    ots$scopy_r_dx(innam.nam$b_name + innam.nam$b_type,
				   innam.nam$l_name,
				   &out_spc);
*/
		    ots$scopy_r_dx( 20, tmp_file_name, &out_spc );
		}
		outfab.fab$b_fsz = infab.fab$b_fsz;
		outfab.fab$b_rfm = infab.fab$b_rfm;
		outrab.rab$l_ubf = outrab.rab$w_usz = 0;

		/*
		 * V5.2-000:	VFC support
		 *
		 *	The way to support VFC is basically simple coping of
		 *	fixed control area. But one exception exist. That is
		 *	the case of 2 bytes fixed length VFC ``records format''
		 *	with FAB$V_PRN ``record attributes''. This is files
		 *	produced by COBOL programs (written in COBOL) or DCL
		 *	OPEN/WRITE. In this case, fixed control area of Cover
		 *	page is filled by 0x018d (1 NewLine).
		 *
		 *	NOTE-1: Print carrige control does not supported yet
		 *	because page layout process (in kprint) does not know
		 *	this attributes.
		 *
		 *	NOTE-2: How print symbiont handles VFC are:
		 *		1) if(fab->fab$b_rfm & FAB$M_VFC)
		 *		2)	if(fab->fab$b_fsz == 2) {
		 *				;; set up record header buf
		 *				if(!fab->fab$b_rat & FAB$M_PRN)
		 *					;; assume file is a ``sequenced file''.
		 *		3)	}
		 *
		 *	A ``sequenced file'' is assumed its header contains
		 *	serial number. Kprint cannot simulates this way.
		 *	Because Kprint produces Cover & Flag page as part of
		 *	a output file, then pass it to print symbiont.
		 */
/*		if (outfab.fab$b_rfm == FAB$C_FIX)
		    outfab.fab$b_rfm = FAB$C_VAR;
		outfab.fab$b_rat = infab.fab$b_rat;	V5.1	*/

#define RMS_PRN_MAGIC	0x018d	/* 1*(<LF>+<CR>) + 7 bit ASCII <0x8d&0x1f>,
				 refer to "VMS Record Management Service
				 Reference Manual Section 5.25 for detail */

		outfab.fab$b_rat = infab.fab$b_rat;
		switch(outfab.fab$b_rfm) {
		case FAB$C_VFC:
			if(outfab.fab$b_fsz == 2)
				if(outfab.fab$b_rat & FAB$M_PRN) {
					/*
					 * store RMS_PRN_MAGIC data in rhb
					 * to print cover page correcoty.
					 */
					*((short *) outrab.rab$l_rhb) = (RMS_PRN_MAGIC >> 8) + ((RMS_PRN_MAGIC & 0xff) << 8);	/* swap byte */
					break;
				} else
					/*
					 * Throw away fixed control area data.
					 * When Cover Page wo'nt be produced,
					 * it semms needless to discard header.
					 * But a lot of escape sequences are
					 * written as part of 1st record. So,
					 * is won't work correctly.
					 */
					outrab.rab$l_rhb = 0;/* fall down next statements */
			else
				/*
				 * Head area will be discarded by print
				 * symbiont
				 */
				break;
		case FAB$C_FIX:
			outfab.fab$b_rfm = FAB$C_VAR;
		default:
			break;
		}

		if (kpri.device_type == DEVIL$_LN80 && kpri.sixel &&
			(outfab.fab$b_rat &
			    (FAB$M_FTN | FAB$M_CR | FAB$M_PRN)) == 0)
		    outfab.fab$b_rat |= FAB$M_CR;

		if (!((status = kpri$rms_open_output(&outfab, &outrab, &outnam,
                                                 &out_spc,
                                                 &out_def,
                                                 &outres_spc))
                           & STS$M_SUCCESS)) {
		    kpri$rms_error(KPRI$_OPENOUT, status, &outres_spc);
		    continue;
		}
		kpri$rms_set_output_rab(&outrab);
		mode.output_opend = 1;
/*++*/
		if (kpri.preload_com)
		    preload_wild();
/*--*/
	    }

	    status = print_common();

	    if (!(status & STS$M_SUCCESS) && status != RMS$_EOF)
		kpri$rms_error(KPRI$_READERR, status, &inres_spc);

	    if (!((status = sys$close(&infab)) & STS$M_SUCCESS))
		kpri$rms_error(KPRI$_CLOSEIN, status, &inres_spc);

	    if (!kpri.output) {
		if (!((status = sys$close(&outfab)) & STS$M_SUCCESS))
		    kpri$rms_error(KPRI$_CLOSEOUT, status, &outres_spc);
		mode.output_opend = 0;
	    }

	    kpri$queue_add_file();
	}
	if (status != RMS$_NMF)
	    kpri$rms_error(KPRI$_OPENIN, status, &res_spc);

	ots$scopy_dxdx(&fil_spc, &rel_spc);
	is_get_value = cli$get_value( &sd_p1, &fil_spc ) & STS$M_SUCCESS;
    }
    lib$find_file_end(&context);

    if (kpri.output && mode.output_opend) {
	if (!((status = sys$close(&outfab)) & STS$M_SUCCESS))
	    kpri$rms_error(KPRI$_CLOSEOUT, status, &outres_spc);
    }

    return(SS$_NORMAL);
}


convert_and_copy( get_ptr, byte, buffer )
unsigned char *get_ptr;
int byte;
char buffer[];
{
    char *put_ptr, convert_to_hex();
    int i;

    put_ptr = buffer + 2 * byte;
    for ( i = 0; i < byte; ++i ) {
	*( --put_ptr ) = convert_to_hex( *get_ptr % 16 );
	*( --put_ptr ) = convert_to_hex( *get_ptr / 16 );
	++get_ptr;
    }
}


char convert_to_hex( n )
int n;
{
    return( ( n < 10 ) ? n + '0' : n - 10 + 'A' );
}

formats_common(col, buf, len)
int            col;
unsigned char *buf;
int            len;
{
    unsigned char *last;
    unsigned ch;
    unsigned kpri$ch_gnext_shift(); /* nee jsy$ch_gnext_shift(); */

    last = buf + len;
    while ((ch = kpri$ch_gnext_shift(&buf, last, &mode.kana_shift)) != 0xFFFF) {
	if (kpri.check_level_II && is_level_II(ch))
	    kpri$error_char_check(KPRI$_LEVEL_II, ch, &inres_spc);
	else
/*++ M002
!	if (kpri.check_expanded && is_expanded(ch) && !jsy$is_line_drawing(ch))
!	    kpri$error_char_check(KPRI$_EXPANDED, ch, &inres_spc);
!	else
!	if (kpri.check_keisen && jsy$is_line_drawing(ch))
--*++*/
	if (kpri.check_expanded && is_expanded(ch) && !kpri$is_line_drawing_dec(ch))
	    kpri$error_char_check(KPRI$_EXPANDED, ch, &inres_spc);
	else
	if (kpri.check_keisen && kpri$is_line_drawing_dec(ch))
/*-- M002 --*/
	    kpri$error_char_check(KPRI$_KEISEN,   ch, &inres_spc);

	col = devil$text_put_char(col, ch);
    }
    return col;
}

kpri$put_table()
{
    int ch;
    static struct dsc$descriptor dx1 = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
    static struct dsc$descriptor dx2 = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};

    if ((mode.header_line || kpri$check_digit(kpri$ga_buff_ptr,
						kpri$gl_buff_len)) &&
	    kpri.page &&
	    kpri.page_start <= mode.page_idx &&
	    kpri.page_end   >= mode.page_idx) {
	ots$scopy_r_dx(kpri$gl_buff_len, kpri$ga_buff_ptr, &dx1);
	str$append(&dx1, &sd_table_line);
	if (kpri.page_before) str$append(&dx1, &page_before);
	kpri$dsc_decimal(mode.page_num, &dx2);
	str$append(&dx1, &dx2);
	if (kpri.page_after) str$append(&dx1, &page_after);
	lib$put_output(&dx1);
    }
}

int kpri$form_chr();
int kpri$form_str();
int kpri$form_put();

print_common()
{
    int status, exit = 0;
    int print_this_page = 0;
    int image_lines = 0;
    char *save_ptr;
    extern int put_param_error();

    int	cover_page_flag = 0, line_pitch = 0, char_pitch = 0, lines_per_inch = 0;
    int size_h = 0, size_v = 0;
    double chars_per_inch = 0;
    char save_char;

    kpri$reset_char_check();
    mode.left_margin = 0;
    mode.blank_count = 0;
    mode.space = mode.space_count = 1;

/*++*/
    if (kpri.preload_pos)
	preload_wild();
/*--*/
    if (kpri.device_type == DEVIL$_LN03 || kpri.device_type == DEVIL$_LN80
     || kpri.device_type == DEVIL$_LA80 || kpri.device_type == DEVIL$_LA84
     || kpri.device_type == DEVIL$_LA86) {
	if (kpri.cover_page_com > 0) {
	    if (kpri.cover_page_com == 2)
		kpri.cover_page_com = 0;
	    if (kpri.cover_page < 0)
/* /nocover の有効範囲を縮めるため
		kpri.cover_page_com = 0;
*/
		;
	    else
		cover_page_flag = 1;
	}
	else if (kpri.cover_page > 0) {
/* /cover の有効範囲を縮めるため
	    kpri.cover_page_com = 1;
*/
	    cover_page_flag = 1;
	}
    }
/*++*/
    if (mode.preloaded && cover_page_flag  &&
	(kpri.device_type == DEVIL$_LA80 || kpri.device_type == DEVIL$_LA84 ||
	 kpri.device_type == DEVIL$_LA86))
	kpri$form_put_page();
/*--*/
    if (cover_page_flag == 1)
    {
	reset_item_list();
	add_item_list(DEVIL$_DEVICE_TYPE,        0, &kpri.device_type, 0);
	add_item_list(DEVIL$_BUFF_CHAR,          0, kpri$form_chr,     0);
	add_item_list(DEVIL$_BUFF_STRING,        0, kpri$form_str,     0);
	add_item_list(DEVIL$_BUFF_OUTPUT,        0, kpri$form_put,     0);
	add_item_list(DEVIL$_CHAR_PITCH,         0, &char_pitch,       0);
	add_item_list(DEVIL$_LINE_PITCH,         0, &line_pitch,       0);
	add_item_list(DEVIL$_RES_LINES_PER_INCH, 0, &lines_per_inch,   0);
	add_item_list(DEVIL$_RES_CHARS_PER_INCH, 0, &chars_per_inch,   0);
	if ((kpri.device_type == DEVIL$_LN03 || kpri.device_type == DEVIL$_LN80)
		 && (kpri.vertical ^ kpri.landscape))
	    add_item_list(DEVIL$_LANDSCAPE,   0, 0, 0);
	add_item_list(DEVIL$_SIZE_TYPE,          0, &kpri.size_type, 0);
	/*
	 * V5.2-000: Disable 2ku->10ku kanji code conversion
	 */
	if(kpri.plain_lbp)
		add_item_list(DEVIL$_PLAIN_LBP,          0, 0, 0);
	add_item_list(0, 0, 0, 0);

	devil$initialize(item_list);
	kpri$form_clr();
	/*
	 * V5.2-000:	Remove verbos BLANK codes before escape sequence codes.
	 *
	 *	Kpint /Land [/Cover] writes <Page Format (to Landscape)> with
	 *	leading a BLANK code!! This causes waste of papers on LN80.
	 */
	if (!mode.fortran) {
		form[0] = '\0';
		form_ptr = form;
	}
	devil$text_put_init();
	switch (kpri.device_type) {
	    case DEVIL$_LN03:
		size_v = size_ln03[kpri.vertical^kpri.landscape][kpri.size_type][0];
		size_h = size_ln03[kpri.vertical^kpri.landscape][kpri.size_type][1];
		mode.page_size = lines_ln03[kpri.vertical^kpri.landscape][kpri.size_type][2] - 8;
		mode.page_width = chars_per_inch * size_h;
		break;
	    case DEVIL$_LN80:
		size_v = size_ln80[kpri.vertical^kpri.landscape][kpri.size_type][0];
		size_h = size_ln80[kpri.vertical^kpri.landscape][kpri.size_type][1];
		mode.page_size = lines_ln80[kpri.vertical^kpri.landscape][kpri.size_type][2] - 8;
		mode.page_width = chars_per_inch * size_h;
		break;
	    default:
		mode.page_size = lines_per_inch * DEF_SIZE_V - 8;
		mode.page_width = 110;
	}
	mode.tmp_text_attr = mode.new_text_attr = mode.text_attr = 0;

	print_cover_page();
    }

    reset_item_list();
    add_item_list(DEVIL$_DEVICE_TYPE,        0, &kpri.device_type,    0);
    add_item_list(DEVIL$_BUFF_CHAR,          0, kpri$form_chr,        0);
    add_item_list(DEVIL$_BUFF_STRING,        0, kpri$form_str,        0);
    add_item_list(DEVIL$_BUFF_OUTPUT,        0, kpri$form_put,        0);
    add_item_list(DEVIL$_FONT_PITCH,         0, &kpri.font_pitch,     0);
    add_item_list(DEVIL$_FONT_SIZE,          0, &kpri.font_size,      0);
    add_item_list(DEVIL$_FONT_TYPE,          0, &kpri.font_type,      0);
    add_item_list(DEVIL$_CHAR_PITCH,         0, &kpri.char_pitch,     0);
    if (kpri.lines_per_inch > 0) {
	add_item_list(DEVIL$_LINES_PER_INCH,     0, &kpri.lines_per_inch, 0);
	kpri.line_pitch = 0;
	if (kpri.lines_per_inch <=12)
	   kpri.line_pitch  = pitch_to_para[kpri.lines_per_inch];
    }
    else
	add_item_list(DEVIL$_LINE_PITCH,         0, &kpri.line_pitch,     0);
    add_item_list(DEVIL$_IMAGE_PER_INCH,     0, &kpri.image_per_inch, 0);
    add_item_list(DEVIL$_ASPECT_RATIO_V,     0, &kpri.aspect_ratio_v, 0);
    add_item_list(DEVIL$_ASPECT_RATIO_H,     0, &kpri.aspect_ratio_h, 0);
    add_item_list(DEVIL$_RES_LINES_PER_INCH, 0, &kpri.lines_per_inch, 0);
    add_item_list(DEVIL$_RES_CHARS_PER_INCH, 0, &mode.chars_per_inch, 0);
    add_item_list(DEVIL$_RES_IMAGE_PER_INCH, 0, &mode.image_per_inch, 0);
    if (kpri.ascii)       add_item_list(DEVIL$_ASCII,       0, 0, 0);
    if (kpri.jis)         add_item_list(DEVIL$_JIS,         0, 0, 0);
    if (kpri.high_speed)  add_item_list(DEVIL$_HIGH_SPEED,  0, 0, 0);
    if (kpri.high_speed || kpri.image_draft)
			  add_item_list(DEVIL$_IMAGE_DRAFT, 0, 0, 0);
    if (kpri.image_frame) add_item_list(DEVIL$_IMAGE_FRAME, 0, 0, 0);
    if (kpri.image_mesh)  add_item_list(DEVIL$_IMAGE_MESH,  0, 0, 0);
    if (kpri.landscape)   add_item_list(DEVIL$_LANDSCAPE,   0, 0, 0);
    if (kpri.full_paint)  add_item_list(DEVIL$_FULL_PAINT,  0, 0, 0);
    if (kpri.letter)      add_item_list(DEVIL$_LETTER,      0, 0, 0);
    if (kpri.slant)       add_item_list(DEVIL$_SLANT,       0, 0, 0);
    if (kpri.vertical)    add_item_list(DEVIL$_VERTICAL,    0, 0, 0);
    add_item_list(DEVIL$_SIZE_TYPE,          0, &kpri.size_type, 0);
    if (kpri.keisen) {
	add_item_list(DEVIL$_KEISEN_WIDTH, 0, &kpri.keisen_width, 0);
	add_item_list(DEVIL$_KEISEN_LINE,  0, &kpri.keisen_line,  0);
    }
    if (kpri.register_page > 0) {
	if (mode.reset_overlay_form)
	    add_item_list(DEVIL$_RESET_OVERLAY_FORM, 0, 0, 0);
	mode.reset_overlay_form = 0;
	add_item_list(DEVIL$_REGISTER_FORM,
			0,
			&kpri.register_page,
			0);
    }
    if (kpri.overlay_page > 0)
	add_item_list(DEVIL$_OVERLAY_FORM,
			0,
			&kpri.overlay_page,
			0);

    /*
     * V5.2-000: Disable 2ku->10ku kanji code conversion
     */
    if(kpri.plain_lbp)
	    add_item_list(DEVIL$_PLAIN_LBP,          0, 0, 0);
    add_item_list(0, 0, 0, 0);

    devil$initialize(item_list);
    kpri$form_clr();
    if (kpri.device_type == DEVIL$_LN03) {
	devil$buff_form_feed();
	if (form_ptr > form + 1) {
	    kpri$form_put();
	    kpri$form_set_page();
	}
    }
    devil$text_put_init();

/* This must be defered until page TOP (header and top margin) are prcessed. */
/* TH, 11-SEP-1990
    if (kpri.sixel)
      	devil$sixel_init(kpri.left + mode.left_margin);
*/
    mode.lines_per_inch = kpri.lines_per_inch;
    if (kpri.page_length > 0)
	mode.page_size = kpri.page_length;
    else
/*++ ++*/
	switch (kpri.device_type) {
	    case DEVIL$_LN03:
		if (kpri.line_pitch)
		    mode.page_size = lines_ln03[kpri.landscape][kpri.size_type][kpri.line_pitch-1];
		else
		    mode.page_size = mode.lines_per_inch * kpri.size_v;
		break;
	    case DEVIL$_LN80:
		if (kpri.line_pitch)
		    mode.page_size = lines_ln80[kpri.landscape][kpri.size_type][kpri.line_pitch-1];
		else
		    mode.page_size = mode.lines_per_inch * kpri.size_v;
		break;
	    default:
		mode.page_size = mode.lines_per_inch * kpri.size_v;
	}
/*-- --*/
    mode.page_width = mode.chars_per_inch * kpri.size_h;

    mode.page_size -= ( kpri.top + kpri.bottom );
    if (kpri.header)			 mode.page_size -= 2;
    if (kpri.title  || kpri.page_top   ) mode.page_size -= 2;
    if (kpri.footer || kpri.page_bottom) mode.page_size -= 2;

    mode.kana_shift = 0; /* Kana Shift Flag -- Default is Kanji Mode */

    mode.sixel_begin = 0;
    mode.sixel_end   = 0;
    mode.sixel       = 0;

    mode.line_idx = 0;
    mode.page_idx = 0;
    mode.page_num = kpri.page_initial;
    print_this_page = 0;
    mode.text_attr = 0;
    if (kpri.slant)    mode.text_attr |= DEVIL$M_ITALIC;
    if (kpri.vertical) mode.text_attr |= DEVIL$M_VERTICAL;
    mode.new_text_attr = mode.text_attr;
    mode.tmp_text_attr = 0;

    if (kpri.header)
	init_header();

    for (;;) {

	status = get_line();
	exit = !(status & STS$M_SUCCESS);

	if (mode.sixel_begin) {
	    mode.sixel_begin = 0;
	    image_lines = 0;
	    devil$sixel_init(kpri.left + mode.left_margin);
	}

	if (mode.sixel_end) {
	    mode.sixel_end = 0;
	    if (kpri.device_type == DEVIL$_LN03 && mode.top_of_page)
	    {
		mode.top_of_page = 0;
		save_char = form[0];
		form[0] = ' ';	/* Fortan CR/LF */
		devil$sixel_reset();
		kpri$form_put();
		mode.top_of_page = 1;
		form[0] = save_char;
	    }
	    else
		devil$sixel_reset();
	    devil$text_put_init_text();
	    mode.line_idx +=
		(mode.lines_per_inch * image_lines) / mode.image_per_inch;
	}
	save_ptr = form_ptr;

	if ((mode.top_of_page |= (kpri.feed & mode.line_idx >= mode.page_size))
		|| exit) {
	    exit |= (mode.page_idx >= kpri.print_end);
	    kpri$form_reset_page();
	    if (print_this_page) {
		if (mode.page_idx > 0 && (kpri.footer || kpri.page_bottom)) {
		    kpri$form_skip_line(mode.page_size - mode.line_idx + 1);
		    print_title(kpri.footer, kpri.footer_position, &footer,
				kpri.page_bottom);
		    if (exit) break;
		}
	    }
	    if (exit) break;

	    if (mode.page_idx >= kpri.page_start) mode.page_num ++;
	    mode.line_idx = 0;
	    mode.page_idx ++;
	    print_this_page = mode.page_idx >= kpri.print_start;
	    if (kpri.odd_page)  print_this_page &=   mode.page_idx & 1;
	    if (kpri.even_page) print_this_page &= !(mode.page_idx & 1);

	    if (kpri.device_type == DEVIL$_LN03)
		devil$buff_form_feed();
	    if (form_ptr > save_ptr) {
		kpri$form_put();
	    }

	    if (kpri.sixel) devil$sixel_reset();
	    if (kpri.device_type != DEVIL$_LN03 || kpri.register_page == 0){
		kpri$form_set_page();
	    }
	    if (print_this_page) {

		/* Need to reset SIXEL to make TOP margin effective */
		if (!mode.fortran && (kpri.device_type != DEVIL$_LN03 || kpri.register_page == 0)){
		     kpri$form_put_page();
	        }

		if (mode.page_idx == 2 && kpri.overlay_second_page > 0)
		    devil$change_overlay_page(kpri.overlay_second_page);
		if (kpri.header) {
		    print_header();
		    kpri$form_skip_line(2);
		}
		kpri$form_skip_line(kpri.top);
		if (kpri.title || kpri.page_top) {
		    print_title(kpri.title, kpri.title_position, &title,
				kpri.page_top);
		    kpri$form_skip_line(2);
		}
		/* Moved from the top of this part.  This part must be
		   processed for each (top of the) page. TH, 11-SEP-1990 */
		if (kpri.sixel)
		    devil$sixel_init(kpri.left + mode.left_margin);

	    }
	}
	if (exit) break;
	if (print_this_page) {
	    if (kpri$gl_buff_len > 0) {
		if (kpri.sixel || mode.sixel) {
		    image_lines += devil$sixel_data(kpri$ga_buff_ptr,
						    kpri$gl_buff_len);
		}
		else {
		    devil$text_put_space(kpri.left + mode.left_margin);

		    if (kpri.manual &&
			    (mode.header_line ||
				 kpri$check_digit(kpri$ga_buff_ptr,
						  kpri$gl_buff_len)))
			mode.tmp_text_attr = DEVIL$M_DOUBLEHIGHT;

		    if (mode.text_attr != mode.new_text_attr ||
				mode.tmp_text_attr != 0)
			devil$text_set_char_attr(0,
					mode.text_attr,
					mode.new_text_attr | mode.tmp_text_attr);
		    mode.text_attr = mode.new_text_attr;

		    formats_common(0, kpri$ga_buff_ptr, kpri$gl_buff_len);

		    if (mode.tmp_text_attr != 0)
			devil$text_set_char_attr(0,
					mode.text_attr | mode.tmp_text_attr,
					mode.text_attr);
		    mode.tmp_text_attr = 0;

		    if (kpri.table) kpri$put_table();
		}
	    }
	}
	if (!mode.ff_only) {
	    if (print_this_page && !kpri.sixel && !mode.sixel){
		kpri$form_put();
	}
	    if (!kpri.sixel && !mode.sixel)
	    mode.line_idx ++;
	}
    }

    if (kpri.sixel) devil$sixel_reset();
    devil$text_put_reset();

    kpri$form_put();

    return status;
}

print_title(title, title_pos, title_str, page)
int title;
int title_pos;
struct dsc$descriptor *title_str;
int page;
{
    int col;

    devil$text_put_space(kpri.left);
    col = 0;

    if (title && title_pos <= kpri.page_position ) {
	devil$text_put_space(title_pos - col);
	col = title_pos;
	col = formats_common(col, title_str -> dsc$a_pointer,
				  title_str -> dsc$w_length);
    }
    if (page && kpri.page_start <= mode.page_idx &&
		kpri.page_end   >= mode.page_idx) {
	devil$text_put_space(kpri.page_position - col);
	col = kpri.page_position;
	if (kpri.page_before)
	    col = formats_common(col, page_before.dsc$a_pointer,
				      page_before.dsc$w_length);
	col += kpri$form_decimal(mode.page_num);
	if (kpri.page_after)
	    col = formats_common(col, page_after.dsc$a_pointer,
				      page_after.dsc$w_length);
    }
    if (title && title_pos > kpri.page_position ) {
	devil$text_put_space(title_pos - col);
	col = title_pos;
	col = formats_common(col, title_str -> dsc$a_pointer,
				  title_str -> dsc$w_length);
    }
}

#include syidef
#include jpidef

#define COVER_LEFT_MARGIN	 8

static print_cover_page()
{
static struct dsc$descriptor file_owner = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static char	version[9],  nodename[16], username[12], owner[20];
static int	version_len, nodename_len, username_len;
short		buf_len;
int		sts, col, lin, mxc = mode.page_width - 4, nml;
char		*nmp;

    reset_item_list();
    add_item_list(SYI$_VERSION,   8, &version,  &version_len);
    add_item_list(0, 0, 0, 0);
    sts = sys$getsyi(0, 0, 0, &item_list, 0, 0, 0);
    if (!(sts & SS$_NORMAL))	lib$stop(sts);
    reset_item_list();
    add_item_list(SYI$_NODENAME, 16, &nodename, &nodename_len);
    add_item_list(0, 0, 0, 0);
    sts = sys$getsyi(0, 0, 0, &item_list, 0, 0, 0);
    if (!(sts & SS$_NORMAL))	nodename_len = 0;

    reset_item_list();
    add_item_list(JPI$_USERNAME, 12, &username, &username_len);
    add_item_list(0, 0, 0, 0);
    sts = sys$getjpi(0, 0, 0, &item_list, 0, 0, 0);
    if (!(sts & SS$_NORMAL))	lib$stop(sts);

    file_owner.dsc$a_pointer = owner;
    file_owner.dsc$w_length  = buf_len = 20;
    sts = sys$idtoasc(xabpro.xab$l_uic, &buf_len, &file_owner, 0, 0, 0);
/*++ V4.6-000 A.murakami 21-SEP-1987
!    if (!(sts & SS$_NORMAL))	lib$stop(sts);
--*++*/
    if (sts==SS$_NOSUCHID) {
    static $DESCRIPTOR(uic_ctrstr,"!%U");
    	sys$fao(&uic_ctrstr,&buf_len,&file_owner,xabpro.xab$l_uic);
        file_owner.dsc$w_length  = buf_len;
    }
    else if (!(sts & SS$_NORMAL))	lib$stop(sts);
/*-- V4.6-000 A.murakami 21-SEP-1987 --*/

    kpri$form_skip_line(lin = mode.page_size / 4);
    devil$text_put_space(col = 10);
    devil$text_set_char_attr(0, 0, DEVIL$M_DOUBLEWIDTH | DEVIL$M_DOUBLEHIGHT);
    if (nodename_len) {
	col  = formats_double(col, nodename, nodename_len);
	col += formats_double(col, "::", 2);
    }
    col += formats_double(col, username, username_len);
    kpri$form_skip_line(1);
    devil$text_set_char_attr(0, DEVIL$M_DOUBLEWIDTH | DEVIL$M_DOUBLEHIGHT, 0);
    kpri$form_skip_line(2);
    devil$text_put_space(col = 4);
    devil$text_set_char_attr(0, 0, DEVIL$M_REVERSE);
    for (sts=0; sts+2<mxc; sts+=2)
	col += formats_common(col, "　", 2);
    devil$text_set_char_attr(0, DEVIL$M_REVERSE, 0);
    kpri$form_skip_line(3);
/* KPRINT/COVER/LAND の不都合のため */
    devil$buff_string( "\033[m", ( sizeof( "\033[m" ) ) - 1 );
/*++
!    if ((col = (mxc - (innam.nam$b_name + innam.nam$b_type + innam.nam$b_ver) * 4 - 8)) < 0)
!	col = 4;
!    devil$text_put_space(col);
!    devil$text_set_char_attr(0, 0, DEVIL$M_DOUBLEWIDTH | DEVIL$M_DOUBLEHIGHT);
!    col  = formats_double(col, innam.nam$l_name, innam.nam$b_name + innam.nam$b_type + innam.nam$b_ver);
!    kpri$form_skip_line(1);
!    devil$text_set_char_attr(0, DEVIL$M_DOUBLEWIDTH | DEVIL$M_DOUBLEHIGHT, 0);
!    if (kpri.device_type == DEVIL$_LN03 || kpri.device_type == DEVIL$_LN80)
!	lin += 7;
!    else
!	lin += 9;
!    kpri$form_skip_line(mode.page_size - (lin + 10));
++*/
    nmp = inres_spc.dsc$a_pointer +
		innam.nam$b_node + innam.nam$b_dev + innam.nam$b_dir;
    nml = innam.nam$b_name + innam.nam$b_type + innam.nam$b_ver;
    devil$text_set_char_attr(0, 0, DEVIL$M_DOUBLEWIDTH | DEVIL$M_DOUBLEHIGHT);
    if (nml > (mxc - COVER_LEFT_MARGIN) / 4) {
	while (nml > (mxc - COVER_LEFT_MARGIN) / 4) {
	   devil$text_put_space(COVER_LEFT_MARGIN);
	   /*
	    * V5.4-000:
	    *
	    *	If filename is too long (That is kprint executes following
	    *	while loop), then last char of line is re-printed as 1st char
	    *	of next line
	    * formats_double(COVER_LEFT_MARGIN, nmp, (mxc -4 ) / 4);
	    *						   ^^ right margin?
	    */
	   formats_double(COVER_LEFT_MARGIN, nmp, (mxc - COVER_LEFT_MARGIN ) / 4);
	   nml -= (mxc - COVER_LEFT_MARGIN) / 4;
	   nmp += (mxc - COVER_LEFT_MARGIN) / 4;
	   if (kpri.device_type == DEVIL$_LN03 || kpri.device_type == DEVIL$_LN80) {
		kpri$form_skip_line(2);
		lin += 2;
	    }
	    else {
		kpri$form_skip_line(1);
		++ lin;
	    }
	}
	devil$text_put_space(COVER_LEFT_MARGIN);
	formats_double(COVER_LEFT_MARGIN, nmp, nml);
    }
    else {
	col = mxc - 8 - nml * 4;
	if (kpri.device_type == DEVIL$_LN03 || kpri.device_type == DEVIL$_LN80)
	    col /= 2;
	devil$text_put_space(col);
	col  = formats_double(col, nmp, nml);
    }
    if (kpri.device_type == DEVIL$_LN03 || kpri.device_type == DEVIL$_LN80)
	kpri$form_skip_line(1);
    lin += 9;
    devil$text_set_char_attr(0, DEVIL$M_DOUBLEWIDTH | DEVIL$M_DOUBLEHIGHT, 0);
    kpri$form_skip_line(mode.page_size - (lin + 13));
/*-- --*/
    devil$text_put_space(col = COVER_LEFT_MARGIN);
    col  = formats_common(col, "ファイル名：    ", 16);
/*++
!    if (innam.nam$b_rsl * 2 > mxc - 24) {
!	col += formats_full(col, innam.nam$l_node, innam.nam$b_node + innam.nam$b_dev + innam.nam$b_dir);
!	kpri$form_skip_line(1);
!	devil$text_put_space(col = 24);
!	col += formats_full(col, innam.nam$l_name, innam.nam$b_name + innam.nam$b_type + innam.nam$b_ver);
!    }
!    else
!	col += formats_full(col, innam.nam$l_rsa, innam.nam$b_rsl);
++*/
    nmp = inres_spc.dsc$a_pointer;
    nml = innam.nam$b_rsl;
    while (nml > (mxc - COVER_LEFT_MARGIN - 16) / 2) {
	formats_full(COVER_LEFT_MARGIN + 16, nmp, (mxc - COVER_LEFT_MARGIN - 16) / 2);
	nml -= (mxc - COVER_LEFT_MARGIN - 16) / 2;
	nmp += (mxc - COVER_LEFT_MARGIN - 16) / 2;
	kpri$form_skip_line(1);
	devil$text_put_space(col = COVER_LEFT_MARGIN + 16);
    }
    formats_full(COVER_LEFT_MARGIN + 16, nmp, nml);
/*-- --*/
    kpri$form_skip_line(1);
    devil$text_put_space(col = COVER_LEFT_MARGIN);
    col  = formats_common(col, "最終更新日時：  ", 16);
    formats_date(col, &xabrdt.xab$q_rdt);
    kpri$form_skip_line(1);
    devil$text_put_space(col = COVER_LEFT_MARGIN);
    col  = formats_common(col, "ファイル所有者：", 16);
    col += formats_full(col, file_owner.dsc$a_pointer, file_owner.dsc$w_length);
    kpri$form_skip_line(2);
    devil$text_put_space(col = COVER_LEFT_MARGIN);
    col  = formats_common(col, "ファイルサイズ：", 16);
    col += formats_decimal_full(col, xabfhc.xab$l_ebk);
    col += formats_common(col, " ブロック", 9);
    kpri$form_skip_line(1);
    devil$text_put_space(col = COVER_LEFT_MARGIN);
    col  = formats_common(col, "最長レコード長：", 16);
    col += formats_decimal_full(col, xabfhc.xab$w_lrl);
    col += formats_common(col, " バイト", 7);
    kpri$form_skip_line(2);
    devil$text_put_space(col = COVER_LEFT_MARGIN);
    col  = formats_common(col, "プリントキュー：", 16);
    if (!kpri.output)
	col += formats_full(col, queue_name.dsc$a_pointer, queue_name.dsc$w_length);
    kpri$form_skip_line(1);
    devil$text_put_space(col = COVER_LEFT_MARGIN);
    col  = formats_common(col, "印刷開始日時：  ", 16);
    formats_date(col, &print_time);
    kpri$form_skip_line(1);
    devil$text_put_space(col = COVER_LEFT_MARGIN);
#ifdef __alpha
    col  = formats_common(col, "システム：      ＯｐｅｎＶＭＳ　Ｖｅｒｓｉｏｎ　", 48);
#else /* VAX */
    col  = formats_common(col, "システム：      ＶＡＸ／ＶＭＳ　Ｖｅｒｓｉｏｎ　", 48);
#endif /* __alpha */
    col += formats_full(col, version, version_len);
    kpri$form_skip_line(1);
}

static unsigned short	cover_buf[128];

static formats_double(col, str, len)
int	col, len;
char	str[];
{
int		i, j;

    for (i=0,j=0; i<len; i++) {
	jsy$ch_wchar(jsy$chg_rom_full(str[i]), &cover_buf[j++]);
	if (kpri.device_type == DEVIL$_LN03)
	    jsy$ch_wchar(0xa1a1, &cover_buf[j++]);
    }
    return (formats_common(col, cover_buf, j * 2));
}

static formats_full(col, str, len)
int	col, len;
char	str[];
{
    jsy$tra_rom_full(str, len, cover_buf, 256, &len);
    return (formats_common(col, cover_buf, len));
}

static formats_decimal_full(col, num)
int	col, num;
{
char	buf[10];
int	len;

    len = kpri$str_decimal(num, buf, sizeof buf);
    return (formats_full(col, buf, len));
}

static formats_date(col, dat)
int	col;
double	*dat;
{
static struct dsc$descriptor time_desc = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
char	flag = 1;

    jlb$date_time(&time_desc, &flag, &time_desc.dsc$w_length, dat);
    return (formats_full(col, time_desc.dsc$a_pointer, time_desc.dsc$w_length));
}

#define NAME_SPACE_SIZE		 5
#define DATE_STR_SIZE		17
#define DATE_FIELD_SIZE		20
#define PAGE_PROM_SIZE		 5
#define PAGE_FIELD_SIZE		15

static char	header_buf[256];
static int	header_len;

static init_header()
{
static struct dsc$descriptor file_time = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
char		timebuf[23], *src;
short		buf_len;
unsigned short	slen, olen;
int		sts;

    header_len = mode.page_width - kpri.left - DATE_FIELD_SIZE - PAGE_FIELD_SIZE;
    file_time.dsc$a_pointer = timebuf;
    file_time.dsc$w_length  = buf_len = 23;
    sts = sys$asctim(&buf_len, &file_time, &xabrdt.xab$q_rdt, 0);
    if (!(sts & SS$_NORMAL))	lib$stop(sts);
    slen = innam.nam$b_name;
    src  = inres_spc.dsc$a_pointer +
		innam.nam$b_node + innam.nam$b_dev + innam.nam$b_dir;
    if (slen + innam.nam$b_type + NAME_SPACE_SIZE < header_len) {
	slen += innam.nam$b_type;
	if (slen + innam.nam$b_ver + NAME_SPACE_SIZE < header_len) {
	    slen += innam.nam$b_ver;
	    if (slen + innam.nam$b_dir + NAME_SPACE_SIZE < header_len) {
		slen += innam.nam$b_dir;
		src  -= innam.nam$b_dir;
		if (slen + innam.nam$b_dev + NAME_SPACE_SIZE < header_len) {
		    slen += innam.nam$b_dev;
		    src  -= innam.nam$b_dev;
		    if (slen + innam.nam$b_node + NAME_SPACE_SIZE < header_len) {
			slen += innam.nam$b_node;
			src  -= innam.nam$b_node;
		    }
		}
	    }
	}
    }
    olen = header_len;
    lib$movc5(&slen, src,     " ", &olen, &header_buf);
    slen = DATE_STR_SIZE;
    olen = DATE_FIELD_SIZE;
    lib$movc5(&slen, timebuf, " ", &olen, &header_buf[header_len]);
    header_len += olen;
    slen = olen = PAGE_PROM_SIZE;
    lib$movc5(&slen, "Page ", " ", &olen, &header_buf[header_len]);
    header_len += olen;
}

static print_header()
{
int	col;

    devil$text_put_space(kpri.left);
    col = 0;
    col  = formats_common(col, header_buf, header_len);
    col += kpri$form_decimal(mode.page_num);
}

static get_line()
{
    int status;

    mode.top_of_page = mode.first_page;
    mode.first_page = 0;
    mode.header_line = 0;

    if (kpri.command) {
	if (--mode.blank_count >= 0) {
	    kpri$gl_buff_len = 0;
	    return SS$_NORMAL;
	}
	if (--mode.space_count > 0) {
	    kpri$gl_buff_len = 0;
	    return SS$_NORMAL;
	}
	else
	    mode.space_count = mode.space;
    }

    for (;;) {
	while (((status = kpri$form_read_line()) & STS$M_SUCCESS) &&
		kpri.command &&
		!mode.sixel &&
		kpri$gl_buff_len >= 2 &&
		*kpri$ga_buff_ptr == '.') {
	    if (*(kpri$ga_buff_ptr + 1) == '.') {
		kpri$ga_buff_ptr++;
		kpri$gl_buff_len--;
		return status;
	    }
	    parse_line(kpri$ga_buff_ptr, kpri$gl_buff_len);
	    if (--mode.blank_count >= 0) {
		mode.space_count = 1;
		kpri$gl_buff_len = 0;
		return SS$_NORMAL;
	    }
	}
	if (!(status & STS$M_SUCCESS)) {
	    if (mode.sixel) {
		if (status != RMS$_EOF)
		kpri$rms_error(KPRI$_READERR, status, &tmpres_spc);
		if (!((status = sys$close(&tmpfab)) & STS$M_SUCCESS))
		    kpri$rms_error(KPRI$_CLOSEIN, status, &tmpres_spc);
		kpri$rms_old_input_rab();
		mode.sixel_end = 1;
		mode.sixel     = 0;
	    }
	    else
		break;
	}
	else
	    break;
    }

    return status;
}

static parse_line(buf, len)
unsigned char *buf;
int            len;
{
    int cmd;
    int sign;
    int num;
    int status;
    int	omitted;

    kpri$parse(buf, len, &cmd, &sign, &num, &tmp_spc, &omitted);
    switch (cmd) {
    case CMD_BLANK:
	mode.blank_count = num;
	break;
    case CMD_FONT_PITCH:
	if (!omitted)
	    mode.font_pitch = num * 100;
	else
	    mode.font_pitch = kpri.font_pitch;
	devil$text_set_font(mode.font_pitch, -1, -1);
	break;
    case CMD_FONT_SIZE:
	if (!omitted)
	    mode.font_size = num * 10;
	else
	    mode.font_size = kpri.font_size;
	devil$text_set_font(-1, mode.font_size, -1);
	break;
    case CMD_FONT_TYPE:
	if (!omitted)
	    mode.font_type = num;
	else
	    mode.font_type = kpri.font_type;
	devil$text_set_font(-1, -1, mode.font_type);
	break;
    case CMD_HEADER:
	mode.header_line = 1;
	break;
    case CMD_HORIZONTAL:
	mode.new_text_attr &= ~DEVIL$M_VERTICAL;
	break;
    case CMD_ITALIC:
	if ( kpri . device_type == DEVIL$_LA80 ||
	     kpri . device_type == DEVIL$_LA84 ||
	     kpri . device_type == DEVIL$_LA86  )
	    mode.new_text_attr |= DEVIL$M_ITALIC;
	break;
    case CMD_LEFT_MARGIN:
	switch (sign) {
	    case -1: mode.left_margin -= num; break;
	    case  0: mode.left_margin  = num; break;
	    case  1: mode.left_margin += num; break;
	}
	break;
    case CMD_PITCH:
	if (!omitted)
	    mode.lines_per_inch = num;
	else
	    mode.lines_per_inch = kpri.lines_per_inch;
	devil$text_set_line_pitch(mode.lines_per_inch,
				  &mode.lines_per_inch);
	break;
    case CMD_SIXEL:
	if ( kpri . device_type == DEVIL$_LA84 ||
	     kpri . device_type == DEVIL$_LA86 ||
	     kpri . device_type == DEVIL$_LN03 ||
	     kpri . device_type == DEVIL$_LN80  )

		/*
		 * V5.3-001: tmp[frn]a[bm] were used for a preload file and
		 *	     a sixel file (specified by .SIXEL directive).
		 *	     xabxxx are used for a infab and tmpfab.
		 *	     But these are used exclusively (not explicitly),
		 *	     because they are used in different phases.
		 *	     If sequences are changed, this may cause problems.
		 */
	{
	    xabfhc = cc$rms_xabfhc;
	    xabrdt = cc$rms_xabrdt;
	    xabpro = cc$rms_xabpro;
	    xabfhc.xab$l_nxt = &xabrdt;
	    xabrdt.xab$l_nxt = &xabpro;
	    xabpro.xab$l_nxt = 0;
	    kpri$rms_init_input(&tmpfab, &tmprab, &tmpnam, &xabfhc);

	    if ((status = kpri$rms_open_input(&tmpfab, &tmprab, &tmpnam,
					&tmp_spc,
					&sd_input_default,
					&tmpres_spc))
			& STS$M_SUCCESS) {
		mode.sixel_begin = 1;
		mode.sixel       = 1;
		kpri$rms_set_input_rab(&tmprab);
	    } else
		kpri$rms_error(KPRI$_OPENIN, status, &tmpres_spc);

	}

	break;
    case CMD_SKIP:
	mode.blank_count = num * mode.space;
	break;
    case CMD_SPACING:
	mode.space_count = mode.space = num;
	break;
    case CMD_UPRIGHT:
	mode.new_text_attr &= ~DEVIL$M_ITALIC;
	break;
    case CMD_VERTICAL:
	if ( kpri . device_type == DEVIL$_LA84 ||
	     kpri . device_type == DEVIL$_LA86 ||
	     kpri . device_type == DEVIL$_LN03 ||
	     kpri . device_type == DEVIL$_LN80  )
	    mode.new_text_attr |= DEVIL$M_VERTICAL;
	break;
    }
}


/* 行の途中のFF を有効にするように修正 */
kpri$form_read_line()
{
    static int status;
    static int buffer_length = 0;
    static unsigned char *line_buffer_ptr;
    static unsigned char line_buffer[ MAXBUF ];

/*    if ( !buffer_length ) {
	status = kpri$rms_default_get( line_buffer, MAXBUF, &buffer_length );
	if ( !( status & STS$M_SUCCESS ) ) return( status );
	line_buffer_ptr = line_buffer;
    }
    kpri$gl_buff_len = 0;					V5.1 */
    /*
     *	Above codes does not handle  correctlly
     *	when file is a FORTRAN carrige control. 
     */
    kpri$gl_buff_len = 0;
    if ( !buffer_length ) {
	status = kpri$rms_default_get( line_buffer, MAXBUF, &buffer_length );
	if ( !( status & STS$M_SUCCESS ) ) return( status );
	line_buffer_ptr = line_buffer;
    } else
	    if(mode.fortran)
		    buff[ kpri$gl_buff_len++ ] = ' ';

    /*
     * Following code doesn't work when records are looks like:
     *
     *	xxxxxxxxxxxxxxxxxxxxxxxxxxx
     *	xxxxxxxxxxxxxxxxxxxxxxxxxxx<FF>
     *	xxxxxxxxxxxxxxxxxxxxxxxxxxx
     *	xxxxxxxxxxxxxxxxxxxxxxxxxxx
     *
     * Then Kprint produces output like:
     *
     *	xxxxxxxxxxxxxxxxxxxxxxxxxxx
     *	xxxxxxxxxxxxxxxxxxxxxxxxxxx
     *	<FF>
     *	xxxxxxxxxxxxxxxxxxxxxxxxxxx
     *	<EmptyLine>
     *	xxxxxxxxxxxxxxxxxxxxxxxxxxx
     *	<EmptyLine>
     *
     */
/*
    do {
	if ( line_buffer_ptr >= line_buffer + buffer_length ) {
	    buffer_length = 0;
	    break;
	}
	buff[ kpri$gl_buff_len++ ] = *line_buffer_ptr++;
    } while ( *line_buffer_ptr != FORM_FEED );		V5.4	*/

    if ( line_buffer_ptr < line_buffer + buffer_length )
	buff[ kpri$gl_buff_len++ ] = *line_buffer_ptr++;
    do {
	if ( line_buffer_ptr >= line_buffer + buffer_length ) {
	    buffer_length = 0;
	    break;
	}
	if( *line_buffer_ptr == FORM_FEED )
		break;
	buff[ kpri$gl_buff_len++ ] = *line_buffer_ptr++;
    } while (1);

    mode.ff_only = 0;
    kpri$ga_buff_ptr = buff;
    if (kpri$gl_buff_len > 0)
	if (mode.fortran) {
/*
	    mode.ff_only = (buff[0] == '1' && kpri$gl_buff_len == 1);
*/
	    mode.top_of_page |= buff[0] == '1';
	    kpri$ga_buff_ptr ++;
	    kpri$gl_buff_len --;
	    form[0] = buff[0];
	}
	else
	if (buff[0] == FORM_FEED) {
	    mode.ff_only = kpri$gl_buff_len == 1;
	    mode.top_of_page = 1;
	    kpri$ga_buff_ptr ++;
	    kpri$gl_buff_len --;
	    form[0] = FORM_FEED;
	}

    return status;
}

kpri$form_skip_line(n)
int n;
{
    for (; n > 0; n --) kpri$form_put();
}

kpri$form_set_page()
{
    mode.top_of_page = 1;
    if (mode.fortran) form[0] = '1';
                 else form[0] = FORM_FEED;
}

kpri$form_reset_page()
{
    mode.top_of_page = 0;
    if (mode.fortran) form[0] = ' ';
}

kpri$form_clr()
{
    mode.top_of_page = mode.first_page = 1;
    form_ptr = form + 1;
    form[0] = ' ';	/* Fortan CR/LF */
}

kpri$form_dec(num)
int num;
{
    char buf[10];
    int  len;

    len = kpri$str_decimal(num, buf, sizeof buf);
    kpri$form_str(buf, len);

    return len;
}

kpri$form_decimal(num)
int num;
{
    char buf[10];
    int  len;

    len = kpri$str_decimal(num, buf, sizeof buf);
    formats_common(0, buf, len);

    return len;
}

kpri$form_chr(ch)
unsigned ch;
{
    if (!jsy$ch_pnext(ch, &form_ptr, form + MAXBUF)) {
	kpri$form_put();
	jsy$ch_wnext(ch, &form_ptr);
    }
}

kpri$form_str(buf, len)
unsigned char *buf;
int            len;
{
    int l;

    while (form_ptr + len > form + MAXBUF) {
	l = form + MAXBUF - form_ptr;
	jsy$ch_move(l, buf, form_ptr);
	kpri$form_put();
	len -= l;
	buf += l;
    }
    jsy$ch_move(len, buf, form_ptr);
    form_ptr += len;
}

kpri$form_put_page()
{
    int status;

    if (mode.fortran) form[0] = '1';
                 else form[0] = FORM_FEED;

    if (!((status = kpri$rms_default_put(form, 1)) & STS$M_SUCCESS))
	lib$stop(status);

    mode.top_of_page = 0;
    form[0] = ' ';	/* Fortan CR/LF */
}

kpri$form_put()
{
    int   status;
    char *ptr;
    int   len;

    ptr = form;
    len = form_ptr - form;

    if (!mode.fortran && !mode.top_of_page) {
	ptr ++;
	len --;
    }

    devil$buff_convert(ptr, &len);
    if (!((status = kpri$rms_default_put(ptr, len)) & STS$M_SUCCESS))
	lib$stop(status);

    mode.top_of_page = 0;
    form_ptr = form + 1;
    form[0] = ' ';	/* Fortan CR/LF */
    return status;	/* %%% Alpha AXP %%% */
}

/*++ /PRELOAD ++*/
static $DESCRIPTOR(sd_font40_default,	    ".knj$fnt40");
static $DESCRIPTOR(sd_font24_default,	    ".knj$fnt24");
static $DESCRIPTOR(sd_font32_default,	    ".knj$fnt32");
static struct dsc$descriptor font_name     = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor fntrel_spc    = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
static struct dsc$descriptor fnt_def_spc   = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};

preload_wild()
{
    int context;
    int flag;
    int status;
    int stv;
    int	more = YES;

    switch (kpri.device_type) {
	case DEVIL$_LN03:
	    ots$scopy_dxdx(&sd_font40_default, &fnt_def_spc);
	    break;
	case DEVIL$_LA80:
	case DEVIL$_LA84:
	case DEVIL$_LA86:
	    ots$scopy_dxdx(&sd_font24_default, &fnt_def_spc);
	    break;
	default:
	    return;
    }
    context = 0;
    flag    = 2;
    while ( 1 ) {
	if (!(cli$get_value(&sd_preload, &font_name) & STS$M_SUCCESS)) {
	    if (kpri.preload_pos && more) {
		ots$scopy_r_dx(innam.nam$b_node + innam.nam$b_dev + innam.nam$b_dir + innam.nam$b_name,
			inres_spc.dsc$a_pointer, &font_name);
		more = NO;
	    }
	    else
		break;
	}
	while ((status = lib$find_file (&font_name, &tmp_spc, &context,
                          &fnt_def_spc, &fntrel_spc, &stv, &flag)) & STS$M_SUCCESS) {
	    xabfhc = cc$rms_xabfhc;
	    xabrdt = cc$rms_xabrdt;
	    xabpro = cc$rms_xabpro;
	    xabfhc.xab$l_nxt = &xabrdt;
	    xabrdt.xab$l_nxt = &xabpro;
	    xabpro.xab$l_nxt = 0;
	    kpri$rms_init_input(&tmpfab, &tmprab, &tmpnam, &xabfhc);
	    if (!((status = kpri$rms_open_input(&tmpfab, &tmprab, &tmpnam,
						&tmp_spc,
						&fnt_def_spc,
						&tmpres_spc))
						& STS$M_SUCCESS)) {
		kpri$rms_error(KPRI$_OPENIN, status, &tmpres_spc);
		break;
	    }
	    kpri$rms_set_input_rab(&tmprab);
	    preload_common();

	    if (!((status = sys$close(&tmpfab)) & STS$M_SUCCESS))
		kpri$rms_error(KPRI$_CLOSEIN, status, &tmpres_spc);
	    kpri$rms_old_input_rab();
	    ots$scopy_dxdx(&font_name, &fntrel_spc);
	}
	lib$find_file_end(&context);
	if (status != RMS$_NMF)
	    kpri$rms_error(KPRI$_OPENIN, status, &tmp_spc);
    }
}

/*	Convert Flag Table without Border handling.

	    (to)  24   32   40
	(from)	+--------------+
	    24	| -1 |  0 |  4 |
		|----+----+----|
	    32	|  2 | -1 |  8 |
		|----+----+----|
	    40	|  6 | 10 | -1 |
		+--------------+
*/
static	char	flg_tbl[3][3] = {{ -1,  0,  4 },
				 {  2, -1,  8 },
				 {  6, 10, -1 }};

static struct font_data {
    unsigned short	code;
    unsigned char	map[250];
    char		size[4];
} inbuff;
unsigned char	wrkbuff[293];

static struct dsc$descriptor srcdsc =
		{ sizeof(inbuff),DSC$K_DTYPE_T,DSC$K_CLASS_S,&inbuff};
static struct dsc$descriptor dstdsc =
		{ sizeof(wrkbuff),DSC$K_DTYPE_T,DSC$K_CLASS_S,&wrkbuff};

static	preload_common()
{
unsigned int	status, inlen, outlen, insiz, outsiz, flag, chrflg;

    switch (kpri.device_type) {
	case DEVIL$_LN03:
	    outsiz = 40;
	    break;
	case DEVIL$_LA80:
	case DEVIL$_LA84:
	case DEVIL$_LA86:
	    outsiz = 24;
	    break;
    }
    if ((status = kpri$rms_get(&tmprab, &inbuff, 256, &inlen)) & STS$M_SUCCESS) {
	if (inlen == 128)
	    insiz = 24;
	else
	    insiz = (inbuff.size[0]-'0')*10 + inbuff.size[1]-'0';
    }
    flag = flg_tbl[insiz/8-3][outsiz/8-3];
    while (status & STS$M_SUCCESS) {

	if (flag != -1)
	    if (!((status = jlb$fex_convert(&srcdsc, &srcdsc, &outlen, flag)) & STS$M_SUCCESS))
		lib$stop(status);
	jlb$preload(&srcdsc, inlen, &dstdsc, &outlen);

	if (!((status = kpri$rms_default_put(wrkbuff, outlen)) & STS$M_SUCCESS))
	    lib$stop(status);
	mode.preloaded = 1;
	status = kpri$rms_get(&tmprab, &inbuff, 256, &inlen);
    }
}
/*-- --*/
