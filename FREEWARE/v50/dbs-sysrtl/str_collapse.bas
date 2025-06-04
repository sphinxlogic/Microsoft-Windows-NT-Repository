	%title	"Remove spaces, tabs and nulls from a string"
	%ident	"V1-003"

	sub str_collapse (string dst_str, string src_str)

!+
! Version:	V1-003
!
! Facility:	Library routines.
!
! Abstract:	Scan a character string removing spaces, tabs and nulls and
!		write the result to a destination string.
!
! Environment:	User mode.
!
! History:
!
!	15-Mar-1989, DBS; Version V1-001
! 001 -	Original version.
!	21-Mar-1989, DBS; Version V1-002
! 002 -	Modified to allow the destination string to be the source string.
!	18-Dec-1995, DBS; Version V1-003
! 003 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	This routine will perform one scan of a source character string and
!	remove all occurences of tabs, spaces and nulls.
!
! Calling Sequence:
!
!	call str_collapse (destination, source)
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

	dst_str = edit$(src_str, 2 + 4)

	end sub
