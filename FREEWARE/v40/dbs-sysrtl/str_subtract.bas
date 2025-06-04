	%title	"Subtract a substring from a string"
	%ident	"V1-002"

	sub str_subtract (string result, string from_this, string take_this)

!+
! Version:	V1-002
!
! Facility:	Library routines.
!
! Abstract:	Subtract a substring from a target string.
!
! Environment:	User mode.
!
! History:
!
!	02-Mar-1988, DBS; Version V1-001
! 001 -	Original version.
!	18-Dec-1995, DBS; Version V1-002
! 002 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	This routine will find the first occurrence of a substring in the
!	target string and remove it.
!
! Calling Sequence:
!
!	call str_subtract (result, from_this, take_this)
!
! Formal Argument(s):
!	result.wt.ds	The destination string.
!	from_this.rt.ds	The source string.
!	take_this.rt.ds	The substring to be removed.
!
! Implicit Inputs:
!	None
!
! Implicit Outputs:
!	None
!
! Completion Codes:
!	None
!
! Side Effects:
!	None
!--


	option	type = explicit,					  &
		constant type = integer,				  &
		size = (integer long, real double)

! External references (and includes)

	external long function	str_len

! Type declarations for variables

	declare long	take_this_len
	declare long	take_this_loc

!+
! Mainline
!-
mainline:

	take_this_len = str_len(take_this)
	take_this_loc = pos(from_this, take_this, 1)

	if (take_this_loc = 0) then
		result = from_this
	else
	result = left(from_this, take_this_loc-1)			  &
		 + right(from_this, take_this_loc+take_this_len)
	end if

	end sub
