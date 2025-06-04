!*****************************************************************************
!*                                                                           *
!*  COPYRIGHT (c) 1990  BY                                                   *
!*  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.                   *
!*  ALL RIGHTS RESERVED.                                                     *
!*                                                                           *
!*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED    *
!*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE    *
!*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER    *
!*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY    *
!*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY    *
!*  TRANSFERRED.                                                             * 
!*                                                                           *
!*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE    *
!*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT    *
!*  CORPORATION.                                                             *
!*                                                                           *
!*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS    *
!*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.                  *
!*                                                                           *
!*****************************************************************************

TYPE ImageRec: RECORD
	BaseAddress: INTEGER,	! 0 if not known.
	UserSpecified: BOOLEAN,	! Was BaseAddress specified by the user?

	Globals: POINTER TO TREE(STRING) OF INTEGER,
				! An 'array' indexed by global symbol, whose
				! value is the relative address of the index
				! (global symbol) within this image.

	Name: POINTER TO STRING,! Name of the image.
END RECORD;
