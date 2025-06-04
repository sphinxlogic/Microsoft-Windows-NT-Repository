/*
*****************************************************************************
*                                                                           *
*  COPYRIGHT (c) 1988, 1990  BY                                             *
*  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.                   *
*  ALL RIGHTS RESERVED.                                                     *
*                                                                           *
*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED    *
*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE    *
*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER    *
*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY    *
*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY    *
*  TRANSFERRED.                                                             * 
*                                                                           *
*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE    *
*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT    *
*  CORPORATION.                                                             *
*                                                                           *
*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS    *
*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.                  *
*                                                                           *
*****************************************************************************
*/

/*
 *   itmdef.h
 *
 *   created 15-jul-1987  by  Monty C. Brandenberg
 *
 */

#ifndef	ITM$ITEM_LIST_3
#define	ITM$ITEM_LIST_3

struct itm$item_list_3 {
    unsigned short	itm$w_bufsiz;
    unsigned short	itm$w_itmcod;
    char		*itm$l_bufadr;
    char		*itm$l_retlen;
};

#define	itm$s_item	sizeof( struct itm$item_list_3 )

#endif /* ITM$ITEM_LIST_3 */

