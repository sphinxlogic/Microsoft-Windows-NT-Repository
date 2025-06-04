#module DEVIL$SIXEL "V4.5-000"
#define __MODULE__ "DEVIL$SIXEL"
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
! AUTHOR: Fujiura, Ichiri       CREATION DATE: 27-May-1985
!
! MODIFIED BY:
!
! V4.5-000
!	Y. Kozono	28-Nov-1986
!	LN03 support
--*/

#include descrip
#include ssdef
#include stsdef
#include "jsy$devil:devildef.h"

#define YES		1
#define NO		0

#define TAB		'\t'
#define CR		'\r'
#define LF		'\n'
#define FORM_FEED	'\f'
#define ESC		'\033'
#define DCS		144
#define ST		156


static $DESCRIPTOR(sd_fao_hex,   "!2UX");	/* Ctrl string of fao */
static $DESCRIPTOR(sd_fao_lbp,   "\233!UL;!UL;!UL/r!!!UL(2XL)");
						/* Ctrl string of lbp seq */
static $DESCRIPTOR(sd_fao_rep,   "!!!UL");	/* Sixel repeat Pattern */
static $DESCRIPTOR(sd_fao_lbp_ini,	 "\2332 I\233!UL G");
						/* LBP8 initialize sequecne */

#define STR_LA84_IMAGE_INIT	"\033\\\033P10q"  /* LA84 initialize sequecne */
#define STR_LA84_IMAGE_RESET	"\033\\"	  /* LA84 reset sequecne      */
#define STR_LBP_IMAGE_RESET	"\233120 G"	  /* LBP8 reset sequecne      */

#define FORM_STR(str)		devil$buff_string(str, (sizeof str) - 1)
#define MAXBITBUFF	    3000
#define MAXEXPBITBUFF	    4000    /* >= MAXBITBUFF * 1.25 */
#define CUTOFF		    15

static unsigned char bitmap[MAXBITBUFF][6];
static unsigned char write_buff[MAXBITBUFF + 1];
static struct dsc$descriptor_s wbuff_dsc = {
		sizeof write_buff,
                DSC$K_DTYPE_T, DSC$K_CLASS_S,
                write_buff};

static struct FUNC {
	int (*setup)();
	int (*convert_sixel)();
	int (*put_converted)();
	int (*init_printer)();
	int (*reset_printer)();
	int (*start_frame)();
	int (*end_frame)();
    } func;

static unsigned char *buff_ptr;
static int            buff_len; /* buffer length of read operation */
static unsigned char *wbuff_ptr;
static int         image_lines;
static int         total_image_lines;
static int         left_space;
static int         prev_bitmap_siz;	/* Size of extendet bitmap */
static int         bitmap_siz;		/* Size of extendet bitmap */
static int         max_bitmap_siz;	/* Size of extendet bitmap */
static int         rep_cont;
static int         odd;
static int         la84_mod;
static unsigned    repeat;
static unsigned    escape;
static unsigned    overlay;

/*
**
**	Functions for LA84
**
*/
static sixel_setup_la84()
{
    if (devil$mod.l_image_per_inch <= 144) devil$mod.l_image_per_inch = 144; else
				           devil$mod.l_image_per_inch = 180;
}

static sixel_init_la84()
{
    la84_mod = 0;
    if (devil$mod.l_image_per_inch == 180)
	la84_mod = 2;
    if (devil$mod.l_aspect_ratio_v == 2)
	++la84_mod;
    devil$text_put_space(left_space);
    FORM_STR(STR_LA84_IMAGE_INIT);
}

static sixel_reset_la84()
{
    static int status;

    if ((la84_mod == 1 && odd % 2 == 1) ||(la84_mod == 0 && odd % 4 != 0)) {
	bitmap_siz = 0;
	if (!((status = la84_expand()) & STS$M_SUCCESS ))
	    return status;
    }
    FORM_STR(STR_LA84_IMAGE_RESET);
}

static sixel_convert_la84()
{
   static int    status;

   status = bitmap_expand();
   return status;
}

static sixel_put_for_la84()
{
int	status;

    switch (la84_mod) {
	case 0:
	case 1:
	    status = la84_expand();
	    break;
	case 2:
	    status = la84_nonexpand(0, 1);
	    break;
	case 3:
	    if (!((status = la84_nonexpand(0, 2)) & STS$M_SUCCESS))
		break;
	    status = la84_nonexpand(3, 2);
    }
    bitmap_siz = 0;
    return status;
}

static la84_nonexpand(start_pos, fact)
int start_pos, fact;
{
    static int i, status, rep_pat, sixel_leng;
    static char sixel_code, prev_sixel;

    wbuff_ptr = write_buff;
    rep_pat   = 0;
    for (i = 0; i < bitmap_siz; i++) {
	sixel_code = (bitmap[i][start_pos + 5 / fact] << 5 |
		      bitmap[i][start_pos + 4 / fact] << 4 |
		      bitmap[i][start_pos + 3 / fact] << 3 |
		      bitmap[i][start_pos + 2 / fact] << 2 |
		      bitmap[i][start_pos + 1 / fact] << 1 |
		      bitmap[i][start_pos + 0 / fact]	    ) + 077;
	if (prev_sixel == sixel_code)
	    rep_pat ++;
	else {
	    append_sixel(rep_pat, prev_sixel);
	    rep_pat = 1;
	    prev_sixel = sixel_code;
	}
    }
    append_sixel(rep_pat, prev_sixel);
    *wbuff_ptr ++ = '-';
    sixel_leng = wbuff_ptr - write_buff;
    image_lines += 6;

    wbuff_ptr = write_buff;
    while (sixel_leng > 128) {
	devil$buff_string(wbuff_ptr, 128);
	status = devil$buff_output();
	wbuff_ptr  += 128;
	sixel_leng -= 128;
    }
    devil$buff_string(wbuff_ptr, sixel_leng);
    status = devil$buff_output();
    return status;
}

static la84_expand()
{
    static unsigned char bitmap_exp[MAXEXPBITBUFF][30];
/*
	    8,2 6,4 4,6 2,8
	    / \ / \ / \ / \
	   o   o   o   o   o
*/
    static int    exp_h_fact[5][2] = { 8, 0, 2, 6, 4, 4, 6, 2, 8, 0 };
/*
      o
    4/
    4-o
    2\
      o
    2/
    4-o
    4\
      o
*/
    static int   exp_v_fact[5][2] = { 4, 0, 4, 0, 2, 2, 4, 0, 4, 0 };
    static int	 i, orig_vpos, orig_bit, exp_bit, exp_vpos, even_siz, status;
    static int   bitmap_length,adjust;

/*
**   Make 4bit bound
*/
    if (bitmap_siz % 4 != 0 )
	for (; bitmap_siz % 4 != 0; bitmap_siz ++)
	    for (i = 0;  i <= 5;  i ++)
		bitmap[bitmap_siz][i] = 0;
    for (i = 0;  i <= 5;  i ++)
	bitmap[bitmap_siz][i] = 0;

    if (odd % 2 == 0)
	exp_vpos = 0;
/*
**  Expand x 1.25 (H) x2.5 (V)
*/
    for (orig_vpos = 0; orig_vpos <= 5 && exp_vpos != 14 && exp_vpos != 29;
			 exp_vpos ++) {
	for (exp_bit = 0, orig_bit = 0;  orig_bit < bitmap_siz; exp_bit ++) {
	    bitmap_exp[exp_bit][exp_vpos] =
		       (bitmap[orig_bit][orig_vpos] *
			exp_h_fact[exp_bit  % 4][0] *
			exp_v_fact[exp_vpos % 5][0]) +
		       (bitmap[orig_bit + 1][orig_vpos] *
			exp_h_fact[exp_bit  % 4][1] *
			exp_v_fact[exp_vpos % 5][0]) +
		       (bitmap[orig_bit][orig_vpos +1 ] *
			exp_h_fact[exp_bit  % 4][0] *
			exp_v_fact[exp_vpos % 5][1]) +
		       (bitmap[orig_bit + 1][orig_vpos + 1] *
			exp_h_fact[exp_bit  % 4][1] *
			exp_v_fact[exp_vpos % 5][1]);
	   if (exp_bit % 4 != 0)
		orig_bit ++;
        }
	if (exp_vpos % 5 == 2 || exp_vpos % 5 == 4)
	    orig_vpos ++;
    }

    for (exp_bit = 0, orig_bit = 0; orig_bit < bitmap_siz; exp_bit ++) {
	bitmap_exp[exp_bit][exp_vpos] =
		       (bitmap[orig_bit][orig_vpos] *
			exp_h_fact[exp_bit  % 4][0] *
			exp_v_fact[exp_vpos % 5][0]) +
		       (bitmap[orig_bit + 1][orig_vpos] *
			exp_h_fact[exp_bit  % 4][1] *
			exp_v_fact[exp_vpos % 5][0]);
	if ( exp_bit%4 != 0 )
	    orig_bit++;
    }
    exp_vpos++;
/*
**	Generate SIXEL
*/
    switch (odd % (2 * (2 - la84_mod))) {
	case 0:
	    bitmap_length = even_siz = exp_bit-1;
	    if (!((status = gen_sixel(bitmap_length, &bitmap_exp,0)) & STS$M_SUCCESS ))
		break;
	    if (la84_mod == 1)
		status = gen_sixel(bitmap_length, &bitmap_exp,6);
	    break;
	case 1:
	    if (exp_bit-1<even_siz) {
		bitmap_length=even_siz;
		for (adjust=exp_bit-1;adjust<=even_siz;adjust++)
		    if (adjust>=0)
			bitmap_exp[adjust][15] =
		            bitmap_exp[adjust][16] =
		            bitmap_exp[adjust][17] =
		            bitmap_exp[adjust][18] =
		            bitmap_exp[adjust][19] =
		            bitmap_exp[adjust][20] =
		            bitmap_exp[adjust][21] =
		            bitmap_exp[adjust][22] =
		            bitmap_exp[adjust][23] = 0;
	    }
	    else {
		bitmap_length=exp_bit-1;
		for (adjust=even_siz;adjust<=exp_bit-1;adjust++)
		    if (adjust>=0)
		      bitmap_exp[adjust][12] =
			bitmap_exp[adjust][13] =
			bitmap_exp[adjust][14] = 0;
	    }
	    if (!((status = gen_sixel(bitmap_length, &bitmap_exp,12)) & STS$M_SUCCESS ))
		break;
	    bitmap_length = exp_bit-1;
	    if (la84_mod == 1) {
		if (!((status = gen_sixel(bitmap_length, &bitmap_exp,18)) & STS$M_SUCCESS ))
		    break;
		status = gen_sixel(bitmap_length, &bitmap_exp,24);
	    }
	    break;
	case 2:
	    if (exp_bit-1<even_siz) {
		bitmap_length=even_siz;
		for (adjust=exp_bit-1;adjust<=even_siz;adjust++)
		    if (adjust>=0)
			bitmap_exp[adjust][0] =
		            bitmap_exp[adjust][1] =
		            bitmap_exp[adjust][2] =
		            bitmap_exp[adjust][3] =
		            bitmap_exp[adjust][4] =
		            bitmap_exp[adjust][5] = 0;
	    }
	    else {
		bitmap_length=exp_bit-1;
		for (adjust=even_siz;adjust<=exp_bit-1;adjust++)
		    if (adjust>=0)
		      bitmap_exp[adjust][24] =
		        bitmap_exp[adjust][25] =
		        bitmap_exp[adjust][26] =
			bitmap_exp[adjust][27] =
			bitmap_exp[adjust][28] =
			bitmap_exp[adjust][29] = 0;
	    }

	    status = gen_sixel(bitmap_length, &bitmap_exp,24);
	    break;
	case 3:
	    if (exp_bit-1<even_siz) {
		bitmap_length=even_siz;
		for (adjust=exp_bit-1;adjust<=even_siz;adjust++)
		    if (adjust>=0)
		      bitmap_exp[adjust][15] =
		        bitmap_exp[adjust][16] =
		        bitmap_exp[adjust][17] = 0;
	    }
	    else {
		bitmap_length=exp_bit-1;
		for (adjust=even_siz;adjust<=exp_bit-1;adjust++)
		    if (adjust>=0)
		      bitmap_exp[adjust][6] =
		        bitmap_exp[adjust][7] =
		        bitmap_exp[adjust][8] =
		        bitmap_exp[adjust][9] =
			bitmap_exp[adjust][10] =
			bitmap_exp[adjust][11] = 
			bitmap_exp[adjust][12] =
			bitmap_exp[adjust][13] =
			bitmap_exp[adjust][14] = 0;
	    }
	    if (!((status = gen_sixel(bitmap_length, &bitmap_exp,6)) & STS$M_SUCCESS ))
		break;
	    bitmap_length = exp_bit-1;
	    status = gen_sixel(bitmap_length, &bitmap_exp,18);
    }
    even_siz = exp_bit-1;
    odd ++;
    return status;
}

/*
**
**	Ganarate SIXEL
**
*/
static gen_sixel(gs_size, dot_array, start_pos)
int gs_size, start_pos;
char dot_array[][30];
{
    static int i, status, rep_pat, sixel_leng = 0;
    static char sixel_code = 0, prev_sixel = 0;

    wbuff_ptr = write_buff;
    rep_pat   = 0;
    for (i = 0; i <= gs_size; i++) {
	if (la84_mod == 0) {
	    sixel_code = ((dot_array[i][(start_pos+11)%30] > CUTOFF ||
			   dot_array[i][(start_pos+10)%30] > CUTOFF) << 5 |
			  (dot_array[i][(start_pos+ 9)%30] > CUTOFF ||
			   dot_array[i][(start_pos+ 8)%30] > CUTOFF) << 4 |
			  (dot_array[i][(start_pos+ 7)%30] > CUTOFF ||
			   dot_array[i][(start_pos+ 6)%30] > CUTOFF) << 3 |
			  (dot_array[i][(start_pos+ 5)%30] > CUTOFF ||
			   dot_array[i][(start_pos+ 4)%30] > CUTOFF) << 2 |
			  (dot_array[i][(start_pos+ 3)%30] > CUTOFF ||
			   dot_array[i][(start_pos+ 2)%30] > CUTOFF) << 1 |
			  (dot_array[i][(start_pos+ 1)%30] > CUTOFF ||
			   dot_array[i][(start_pos   )%30] > CUTOFF)) + 077;
	}
	else {
	    sixel_code = ((dot_array[i][start_pos+5] > CUTOFF) << 5 |
			  (dot_array[i][start_pos+4] > CUTOFF) << 4 |
			  (dot_array[i][start_pos+3] > CUTOFF) << 3 |
			  (dot_array[i][start_pos+2] > CUTOFF) << 2 |
			  (dot_array[i][start_pos+1] > CUTOFF) << 1 |
			  (dot_array[i][start_pos  ] > CUTOFF)) + 077;
	}
	if (prev_sixel == sixel_code)
	    rep_pat ++;
	else {
	    append_sixel(rep_pat, prev_sixel);
	    rep_pat = 1;
	    prev_sixel = sixel_code;
	}
    }
    append_sixel(rep_pat, prev_sixel);
    *wbuff_ptr ++ = '-';
    sixel_leng = wbuff_ptr - write_buff;
    image_lines += 6;

    wbuff_ptr = write_buff;
    while (sixel_leng > 128) {
	devil$buff_string(wbuff_ptr, 128);
	status = devil$buff_output();
	wbuff_ptr  += 128;
	sixel_leng -= 128;
    }
    devil$buff_string(wbuff_ptr, sixel_leng);
    status = devil$buff_output();
    return status;
}

static append_sixel(rep_pattern, code)
int    rep_pattern;
char   code;
{
    static short  one_sixel_leng,i;

    if (rep_pattern != 0) {
	if (rep_pattern > 3) {
	    wbuff_dsc.dsc$w_length = 10;
	    wbuff_dsc.dsc$a_pointer = wbuff_ptr;
	    sys$fao(&sd_fao_rep, &one_sixel_leng, &wbuff_dsc, rep_pattern);
	    wbuff_ptr += one_sixel_leng;
	    *wbuff_ptr++ = code;
	}
	else
	    for (i = 0; i < rep_pattern; i++)
		*wbuff_ptr++ = code;
    }
}

/*
**
**	Functions for LBP8
**
*/
static sixel_setup_lbp()
{
    if (devil$mod.l_image_per_inch <=   0) devil$mod.l_image_per_inch = 120; else
    if (devil$mod.l_image_per_inch <=  60) devil$mod.l_image_per_inch =  60; else
    if (devil$mod.l_image_per_inch <=  80) devil$mod.l_image_per_inch =  80; else
    if (devil$mod.l_image_per_inch <= 120) devil$mod.l_image_per_inch = 120; else
				           devil$mod.l_image_per_inch = 240;
}

static sixel_init_lbp()
{
    static unsigned short    seq_leng;
    static unsigned char lbp_seq_buff[40];
    static struct dsc$descriptor_s lbp_seq  = {sizeof lbp_seq_buff,
						DSC$K_DTYPE_T,
						DSC$K_CLASS_S,
					 	lbp_seq_buff};

    lbp_seq.dsc$w_length = sizeof lbp_seq_buff;
    sys$fao(&sd_fao_lbp_ini, &seq_leng, &lbp_seq,
		720 / devil$mod.l_image_per_inch);
    devil$buff_string(lbp_seq_buff, seq_leng);
}

static sixel_reset_lbp()
{
    FORM_STR(STR_LBP_IMAGE_RESET);
}

static sixel_start_frame_lbp()
{
    devil$text_put_space(left_space);
    if (devil$mod.v_image_frame) FORM_STR("\2331;0;0{");
    if (devil$mod.v_image_mesh)  FORM_STR("\2330;0s");
}

static sixel_end_frame_lbp()
{
    devil$text_put_space(left_space);
    FORM_STR("\23311h\233");
    devil$buff_decimal(total_image_lines * (720 / devil$mod.l_image_per_inch));
    FORM_STR("e\233");
    devil$buff_decimal(max_bitmap_siz    * (720 / devil$mod.l_image_per_inch));
    FORM_STR("a");
    if (devil$mod.v_image_frame) FORM_STR("\2338}");
    if (devil$mod.v_image_mesh)  FORM_STR("\2338r");
    FORM_STR("\23311l");
    devil$buff_output();
}

static sixel_convert_lbp()
{
    static int    status;
    status = bitmap_expand();
    return status;
}

static sixel_put_for_lbp()
{
    static int    i, j, k, status, kk, hex_param_list[MAXBITBUFF];
    static unsigned short    seq_leng;
    static unsigned char lbp_seq_buff[40];
    static struct dsc$descriptor_s lbp_seq  = {sizeof lbp_seq_buff,
						DSC$K_DTYPE_T,
						DSC$K_CLASS_S,
					 	lbp_seq_buff};

    if (bitmap_siz > max_bitmap_siz) max_bitmap_siz = bitmap_siz;

    if (!devil$mod.v_image_frame && !devil$mod.v_image_mesh) {
	if (bitmap_siz % 8 != 0)
	    for (; bitmap_siz % 8 != 0; bitmap_siz ++)
		for (i = 0;  i < 6;  i ++)
		    bitmap[bitmap_siz][i] = 0;

	lbp_seq.dsc$w_length = sizeof lbp_seq_buff;
	sys$fao(&sd_fao_lbp, &seq_leng, &lbp_seq,
		    bitmap_siz/4,		    /* num of image byte */
		    bitmap_siz/8,		    /* X width           */
		    devil$mod.l_image_per_inch,     /* Resolution	 */
		    bitmap_siz/8);		    /* Num of repert !UX */
	lbp_seq.dsc$w_length = seq_leng;
    }
    for (i = 0; i < 6; i ++) {
	if (!devil$mod.v_image_frame && !devil$mod.v_image_mesh) {
	    for (j=0, kk=0; j <= bitmap_siz; j += 8, kk ++) {
		for (k = j; k < j + 8; k ++)
		    hex_param_list[kk] = hex_param_list[kk] * 2 + bitmap[k][i];
	    }
	    sys$faol(&lbp_seq, &seq_leng, &wbuff_dsc, &hex_param_list);
	    devil$text_put_space(left_space);
	    devil$buff_string(write_buff, seq_leng);
	    devil$buff_output();
	}
	image_lines ++;

	for (k = 1; k < devil$mod.l_aspect_ratio_v; k ++) {
	    if (!devil$mod.v_image_frame && !devil$mod.v_image_mesh) {
		if (!devil$mod.v_image_draft) {
		    devil$text_put_space(left_space);
		    devil$buff_string(write_buff, seq_leng);
		}
		devil$buff_output();
	    }
	    image_lines ++;
	}
    }
    bitmap_siz = 0;
    return status;
}

/*
**
**	Functions for LN03
**
*/
static sixel_setup_ln03()
{
    if (devil$mod.l_image_per_inch <=   0) devil$mod.l_image_per_inch = 150; else
    if (devil$mod.l_image_per_inch <=  30) devil$mod.l_image_per_inch =  30; else
    if (devil$mod.l_image_per_inch <=  50) devil$mod.l_image_per_inch =  50; else
    if (devil$mod.l_image_per_inch <=  60) devil$mod.l_image_per_inch =  60; else
    if (devil$mod.l_image_per_inch <=  75) devil$mod.l_image_per_inch =  75; else
    if (devil$mod.l_image_per_inch <= 100) devil$mod.l_image_per_inch = 100; else
    if (devil$mod.l_image_per_inch <= 150) devil$mod.l_image_per_inch = 150; else
				           devil$mod.l_image_per_inch = 300;
}

static sixel_init_ln03()
{
    devil$text_put_space(left_space);
    FORM_STR("\2337 I\2200;0;");
    devil$buff_decimal(300 / devil$mod.l_image_per_inch);
    FORM_STR("q\"");
    devil$buff_decimal(devil$mod.l_aspect_ratio_v);
    devil$buff_char(';');
    devil$buff_decimal(devil$mod.l_aspect_ratio_h);
}

static sixel_reset_ln03()
{
    devil$buff_char(ST);
    devil$buff_output();
}

static sixel_convert_ln03()
{
    static int    status;

    status = sixel_expand();
    return status;
}

static sixel_put_for_ln03()
{
    devil$buff_output();
    return SS$_NORMAL;
}

/*
**
**	Put for line printer
**
*/
static sixel_setup_lp()
{
    devil$mod.l_image_per_inch = 6;
}

static sixel_put_for_lp()
{
    static int    i, j, status;

    for (i = 0;  i <= 5;  i ++) {
	for (j = 0;  j < bitmap_siz;  j ++)
	    if (bitmap[j][i]==1)
		write_buff[j] = '#';
	    else
		write_buff[j] = ' ';
	image_lines ++;
	devil$buff_string(write_buff, bitmap_siz);
	if (!((status = devil$buff_output()) & STS$M_SUCCESS))
	    break;
    }
    bitmap_siz = 0;
    return status;
}

static sixel_init_lp()
{
    return SS$_NORMAL;
}

static sixel_reset_lp()
{
    return SS$_NORMAL;
}

/*
**
**	Expand Sixel to Bitmap image
**
*/
static bitmap_expand()
{
    int status, aspect = NO;

    status = SS$_NORMAL;

    while (buff_len > 0) {
	/*
	** Sixel or ESC character
	*/
	if (*buff_ptr == ESC || *buff_ptr == DCS || *buff_ptr == ST ||
		(*buff_ptr >= '!' && *buff_ptr <= '~' && escape == NO && aspect == NO))
	    switch (*buff_ptr) {
		case ESC:
		case DCS:
		    escape = YES;
		    break;
		case ST:
		    break;
		case '$':
		    if (overlay) {
			if (bitmap_siz > prev_bitmap_siz)
			    prev_bitmap_siz = bitmap_siz;
		    }
		    else
			prev_bitmap_siz = bitmap_siz;
		    overlay = YES;
		    bitmap_siz = 0;
		    break;
		case '-':
		    if (overlay && bitmap_siz < prev_bitmap_siz)
			bitmap_siz = prev_bitmap_siz;
		    overlay = NO;
		    status = put_converted();
		    break;
		case '!':
		    rep_cont = 0;
		    repeat   = YES;
		    break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		    if (repeat)
			rep_cont = rep_cont * 10 + (*buff_ptr - '0');
		    break;
		default: {
		    static unsigned char    sixel;
		    static int row;

		    if (!repeat) rep_cont = 1;
		    while (rep_cont -- > 0 && bitmap_siz < MAXBITBUFF) {
			sixel = *buff_ptr - 077;
			if (overlay && bitmap_siz < prev_bitmap_siz)
			    for (row = 0;  row < 6; row ++) {
				bitmap[bitmap_siz][row] |= sixel & 0x01;
				sixel >>= 1;
			    }
			else
			    for (row = 0;  row < 6; row ++) {
				bitmap[bitmap_siz][row] = sixel & 0x01;
				sixel >>= 1;
			    }
			bitmap_siz ++;
		    }
		    repeat = NO;
		}
		break;
	    }
	    if (aspect == YES)
		if ((*buff_ptr < '0' || *buff_ptr > '9') && *buff_ptr != ';') {
		    aspect = NO;
		    continue;
		}
	    if (*buff_ptr == '\\')
		escape = NO;
	    if (*buff_ptr == 'q') {
		escape = NO;
		if (buff_len > 0 && buff_ptr[1] == '\"') {
		    aspect = YES;
		    buff_ptr ++;
		    buff_len --;
		}
	    }
	    buff_ptr ++;
	    buff_len --;
    }
    return status;
}

/*
**
**	Expand Sixel to Sixel Data
**
*/
static sixel_expand()
{
    int output = NO, aspect = NO;

    while (buff_len > 0) {
	/*
	** Sixel or ESC character
	*/
	if (*buff_ptr == ESC || *buff_ptr == DCS || *buff_ptr == ST ||
		(*buff_ptr >= '!' && *buff_ptr <= '~' && escape == NO && aspect == NO))
	    switch (*buff_ptr) {
		case ESC:
		case DCS:
		    escape = YES;
		    break;
		case ST:
		    break;
		default:
		    devil$buff_char(*buff_ptr);
		    output = YES;
	    }
	    if (aspect == YES)
		if ((*buff_ptr < '0' || *buff_ptr > '9') && *buff_ptr != ';') {
		    aspect = NO;
		    continue;
		}
	    if (*buff_ptr == '\\')
		escape = NO;
	    if (*buff_ptr == 'q') {
		escape = NO;
		if (buff_len > 0 && buff_ptr[1] == '\"') {
		    aspect = YES;
		    buff_ptr ++;
		    buff_len --;
		}
	    }
	    buff_ptr ++;
	    buff_len --;
    }
    if (output == YES)
	return put_converted();
    return SS$_NORMAL;
}

static put_converted()
{
    return (*func.put_converted)();
}

static dev_noop()
{
    return SS$_NORMAL;
}

devil$sixel_initialize()
{
    switch (devil$mod.l_device_type) {
    case DEVIL$_LA80:
    case DEVIL$_LA84:
    case DEVIL$_LA86:
	func.setup          = sixel_setup_la84;
	func.convert_sixel  = sixel_convert_la84;
	func.put_converted  = sixel_put_for_la84;
	func.init_printer   = sixel_init_la84;
	func.reset_printer  = sixel_reset_la84;
	func.start_frame    = dev_noop;
	func.end_frame      = dev_noop;
	break;
    case DEVIL$_LN80:
	func.setup          = sixel_setup_lbp;
	func.convert_sixel  = sixel_convert_lbp;
	func.put_converted  = sixel_put_for_lbp;
	func.init_printer   = sixel_init_lbp;
	func.reset_printer  = sixel_reset_lbp;
	func.start_frame    = sixel_start_frame_lbp;
	func.end_frame      = sixel_end_frame_lbp;
	break;
    case DEVIL$_LN03:
	func.setup          = sixel_setup_ln03;
	func.convert_sixel  = sixel_convert_ln03;
	func.put_converted  = sixel_put_for_ln03;
	func.init_printer   = sixel_init_ln03;
	func.reset_printer  = sixel_reset_ln03;
	func.start_frame    = dev_noop;
	func.end_frame      = dev_noop;
	break;
    case DEVIL$_LP:
	func.setup          = sixel_setup_lp;
	func.convert_sixel  = sixel_convert_lbp;
	func.put_converted  = sixel_put_for_lp;
	func.init_printer   = sixel_init_lp;
	func.reset_printer  = sixel_reset_lp;
	func.start_frame    = dev_noop;
	func.end_frame      = dev_noop;
	break;
    default:
	func.setup          = dev_noop;
	func.convert_sixel  = dev_noop;
	func.put_converted  = dev_noop;
	func.init_printer   = dev_noop;
	func.reset_printer  = dev_noop;
	func.start_frame    = dev_noop;
	func.end_frame      = dev_noop;
	break;
    }

    devil$sixel_setup();
}

devil$sixel_setup()
{
    (*func.setup)();

    if (devil$mod.al_image_per_inch != 0)
	*devil$mod.al_image_per_inch = devil$mod.l_image_per_inch;

    if (devil$mod.l_aspect_ratio_v == 0)
	devil$mod.l_aspect_ratio_v = 1;
    if (devil$mod.l_aspect_ratio_h == 0)
	devil$mod.l_aspect_ratio_h = 1;
}

devil$sixel_data(buf, len)
unsigned char *buf;
int            len;
{
    buff_ptr = buf;
    buff_len = len;
    image_lines = 0;
    (*func.convert_sixel)();
    total_image_lines += image_lines;
    return image_lines;
}

devil$sixel_init(space)
int space;
{
    left_space = space;

    prev_bitmap_siz = 0;
    max_bitmap_siz = 0;
    total_image_lines = 0;
    bitmap_siz = 0;
    rep_cont = 0;
    odd      = 0;
    repeat   = NO;
    escape   = NO;
    overlay  = NO;

    (*func.init_printer)();

    if (devil$mod.v_image_frame || devil$mod.v_image_mesh)
	(*func.start_frame)();

    return SS$_NORMAL;
}

devil$sixel_reset()
{
    if (devil$mod.v_image_frame || devil$mod.v_image_mesh)
	(*func.end_frame)();

    (*func.reset_printer)();

    return SS$_NORMAL;
}
