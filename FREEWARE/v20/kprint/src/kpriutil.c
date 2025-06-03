#module KPRI$UTIL "V5.0-000"
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
! FACILITY: KPRI Version 5.0-000
!
! FUNCTIONAL DESCRIPTION:
!
! ENVIRONMENT: VAX/VMS
!
! AUTHOR: Inasawa, Yuichi       CREATION DATE: 28-Apr-1985
!
! MODIFIED BY:
--*/

#include <descrip.h>
#include "kpridef.h"

kpri$check_digit(buf, len)
unsigned char *buf;
int            len;
{
   int ch;

   ch = jsy$chg_rom_half(jsy$ch_gchar(buf, buf + len));
   return (ch >= '0' && ch <= '9');
}

kpri$str_decimal(num, buf, siz)
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

kpri$dsc_decimal(num, dsc)
int num;
struct dsc$descriptor *dsc;
{
   char buf[10];
   int  len;

   len = kpri$str_decimal(num, buf, sizeof buf);
   ots$scopy_r_dx(len, buf, dsc);
   return len;
}

kpri$calc_col(col, buf, len)
int            col;
unsigned char *buf;
int            len;
{
   unsigned char *last;
   unsigned ch;
   int      kana_shift;
   unsigned kpri$ch_gnext_shift(); /* nee jsy$ch_gnext_shift(); */

   kana_shift = 0;
   last = buf + len;

   while ((ch = kpri$ch_gnext_shift(&buf, last, &kana_shift)) != 0xFFFF) {
      if (ch >= 0x20 && ch <= 0x7E || ch >= 0xA1 && ch <= 0xFE)
         col ++;
      else
      if (ch >= 256)
         col += 2;
      else
      if (ch == TAB)
         col += 8 - (col & 7);
   }
   return col;
}

static int char_check;

kpri$reset_char_check()
{
   char_check = 0;
}

kpri$error_char_check(err_code, chr, file_name)
int      err_code;
unsigned chr;
struct dec$descriptor *file_name;
{
   int msg_vec[5];
   unsigned char str[3];

   str[0] = 2;
   str[1] = chr >> 8;
   str[2] = chr & 0xFF;

   if (!char_check) {
      char_check = 1;
      msg_vec[0] = 3;
      msg_vec[1] = KPRI$_CODE_CHECK;
      msg_vec[2] = 1;
      msg_vec[3] = file_name;
      sys$putmsg(&msg_vec, 0, 0, 0);
   }
   msg_vec[0] = 4;
   msg_vec[1] = err_code;
   msg_vec[2] = 2;
   msg_vec[3] = str;
   msg_vec[4] = chr;
   sys$putmsg(&msg_vec, 0, 0, 0);
}


int put_param_error( string, value )
char *string;
int value;
{
    int msg_vec[ 5 ];
    struct dsc$descriptor param = { 0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0 };

    param . dsc$w_length = strlen( string );
    param . dsc$a_pointer = string;
    msg_vec[ 0 ] = 4;
    msg_vec[ 1 ] = KPRI$_INVALID_PARAM;
    msg_vec[ 2 ] = 2;
    msg_vec[ 3 ] = &param;
    msg_vec[ 4 ] = value;
    sys$putmsg( &msg_vec, 0, 0, 0 );
}

/*
*	char_code  = jsy$ch_gnext_shift(cur, end, shift)
*
*	char_code	character code
*
*	cur		current pointer of character string
*			passed by reference
*
*	end		end pointer of character string
*			passed by value
*
*	shift		shift status
*			0 - kanji
*			1 - kana
*			passed by reference
*
*	character string
*	+--+--+--+--+--+--+--+--+--+
*	|  |  |  |  |  |  |  |  |  |
*	+--+--+--+--+--+--+--+--+--+
*	    ^                        ^
*	    |                        |
*	    cur                      end
*/
#define jsy$c_eos	0xffff

unsigned kpri$ch_gnext_shift(arg_cur, arg_end, arg_shift)
unsigned char **arg_cur;
unsigned char  *arg_end;
int            *arg_shift;
{

unsigned char **cur=arg_cur;
unsigned char  *end=arg_end;
int            *shift=arg_shift;

   unsigned chr;

while (1) { 			/* 5-apr-1990 */
    /*
    if current pointer is greater than or equal to end pointer,
    return jsy$c_eos (end of string)
    */
    if (*cur >= end)
	return jsy$c_eos;

    chr = *(*cur)++;
    if (chr == 0x1b) {
	if (*cur >= end)
	    return chr;
	if (**cur == '|')
	    *shift = 0;
	else
	if (**cur == '}')
	    *shift = 1;
	else
	    return chr;
	(*cur)++;
/*	if (*cur >= end)	 * 5-apr-1990 */
/*	    return jsy$c_eos;	 * 5-apr-1990 */
/*	chr = *(*cur)++;	 * 5-apr-1990 */
    }
    else			/* 5-apr-1990 */
	break;			/* 5-apr-1990 */
} 				/* 5-apr-1990 */

    /*
    read first byte and increment current pointer
    if it is 1 byte character (7 bit or 8 bit control character)
    return 1 byte character code
    */
    if (*shift || chr < 0xa1 || chr == 0xff)
	return chr;

    /*
    if it is 8 bit character, but it is last byte of character string
    return 1 byte character code
    */   
    if (*cur == end)
	return chr;

    /*
    read second byte, if second byte is control character
    return first byte only
    */
    if ((**cur & 0x7f) < 0x21 || (**cur & 0x7f) == 0x7f)
	return chr;

    /*
    return first byte and second byte as 2 byte character
    and increment character pointer
    */
    return chr << 8 | *(*cur)++;
}

#define is_line_drawing(chr)	(chr >= 0xA8A1 && chr <= 0xA8C0)
#define is_line_drawing_dec(chr)	(chr >= 0xFE21 && chr <= 0xFE2B)

kpri$is_line_drawing_dec(chr)
unsigned chr;
{
   return is_line_drawing_dec(chr);
}



