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
 * xliddy protocol blocks
 *
 *  Author:	Monty C. Brandenberg	    One Evening in August
 *
 *
 *  the xliddy data stream consists of
 *
 *  client_id	    longword identifying client
 *  timestamp       quadword VMS timestamp
 *  xl_data_type    longword identifying type of data that follows
 *  data_size	    longword count of bytes to follow
 *  data	    data_size bytes of data
 *
 */

#ifndef	XL_DEFINED
#define	XL_DEFINED

/* Size and layout of the logfile record prefix */

#define     XL_HEADER_LEN       5
#define     XL_HEADER_BYTE_LEN  20	/* XL_HEADER_LEN * 4 bytes per int */
#define     XL_SESSION          0
#define     XL_TIMESTAMP        1       /* quadword */
#define     XL_MSGTYPE          3
#define     XL_BODYSIZE         4

/* XL_MSGTYPE values */

#define	    XL_LINK_CONNECT     1
#define	    XL_LINK_ABORT       2
#define	    XL_SERVER_DATA      3
#define	    XL_CLIENT_DATA      4

#endif /* XL_DEFINED */
