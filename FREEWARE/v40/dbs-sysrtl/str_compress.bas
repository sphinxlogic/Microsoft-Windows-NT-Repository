	%title	"Trim leading and trailing stuff and compress"
	%ident	"V1-002"

	sub str_compress (string dst_str, string src_str)

!+
! Version:	V1-002
!
! Facility:	Library routines.
!
! Abstract:	Trim leading nulls, spaces and tabs from a string, compress
!		multiple spaces and tabs to a single space.
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
!	This routine will scan a string and remove leading spaces, tabs and
!	nulls and then replace multiple spaces and tabs with single spaces.
!
! Calling Sequence:
!
!	call str_compress (destination, source)
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

	dst_str = edit$(src_str, 4 + 8 + 16 + 128)

	end sub
