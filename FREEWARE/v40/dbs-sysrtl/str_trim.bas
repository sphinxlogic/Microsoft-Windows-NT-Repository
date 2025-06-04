	%title	"Trim leading junk from a string"
	%ident	"V1-002"

	sub str_trim (string dst_str, string src_str)

!+
! Version:	V1-002
!
! Facility:	Library routines.
!
! Abstract:	Trim leading nulls, spaces and tabs from a string.
!
! Environment:	User mode.
!
! History:
!
!	04-Oct-1989, DBS; Version V1-001
! 001 -	Original version.
!	18-Dec-1995, DBS; Version V1-002
! 002 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	This routine will search from the front of the source string until it
!	finds a character not in the things it should skip list and return the
!	remainder.
!
! Calling Sequence:
!
!	call str_trim (dst_str, src_str)
!
! Formal Argument(s):
!	dst_str.wt.ds	The destination string.
!	src_str.rt.ds	The source string.
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

mainline:

	dst_str = edit$(src_str, 4 + 128)

	end sub
