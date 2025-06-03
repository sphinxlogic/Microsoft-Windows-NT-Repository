	.TITLE	UAF_MESSAGES - Messages used by the DECwindows authorize utility
	.IDENT	'X-1'

!****************************************************************************
!*									    *
!*  COPYRIGHT (c) 1986 BY						    *
!*  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.		    *
!*  ALL RIGHTS RESERVED.						    *
!*									    *
!*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED   *
!*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE   *
!*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER   *
!*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY   *
!*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY   *
!*  TRANSFERRED.							    *
!*									    *
!*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE   *
!*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT   *
!*  CORPORATION.							    *
!*									    *
!*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS   *
!*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.		    *
!*									    *
!*									    *
!****************************************************************************

!++
!
! FACILITY:	Authorization record maintenance utility
!
! ABSTRACT:
!
!	This module defines the messages used by the DECwindows authorize
!	utility.
!
! AUTHOR:	L. Mark Pilant		CREATION DATE:  17-Jun-1993
!
! MODIFICATION HISTORY:
!
!--

	.FACILITY	UAF,1009/system

! To avoid a conflict with the existing AUTHORIZE utility, set the base
! message code.

	.BASE	4000

	.SEVERITY	SUCCESS

	.SEVERITY	INFORMATION

	.SEVERITY	WARNING
	NOUAIPRIVS	"$GETUAI/$SETUAI privileges (SYSPRV or BYPASS) missing"

	.SEVERITY	ERROR
	NOWILDCARDS	"wildcard characters illegal"
	UAFOPENERR	"error opening authorization file"
	UAFREADERR	"error reading authorization file"
	UAFWRITERR	"error writing authorization file"
	NOSUCHUSER	"specified user not found"
	USEREXISTS	"specified user already exists"
	TIMECVTERR	"error converting time string"

	.SEVERITY	SEVERE
	NOPROCMEM	"process virtual memory exhausted"
	INITDISPLAY	"error intializing display"
	MODTEMPLATE	"error modifying template"
	INTERNAL1	"byte count in item table is incorrect"
	INTERNAL2	"more list entries than available storage"

	.END
