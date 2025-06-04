	%title	"Extract an element from a string"
	%ident	"V1-002"

	sub str_element (string destination				  &
			,long element					  &
			,string delimiter				  &
			,string source)

!+
! Version:	V1-002
!
! Facility:	Library routines.
!
! Abstract:	Extract an element from a string.
!
! Environment:	User mode.
!
! History:
!
!	06-Oct-1989, DBS; Version V1-001
! 001 -	Original version.
!	18-Dec-1995, DBS; Version V1-002
! 002 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	This routine just calls the rtl routine but exists to complete the
!	str_ routines.
!
! Calling Sequence:
!
!	call str_element (destination, element, delimiter, source)
!
! Formal Argument(s):
!	destination.wt.ds  Somewhere to put the result.
!	element.rl.r  The element number that you want.
!	delimiter.rt.ds  The delimiter, MUST be only one character.
!	source.rt.ds  The source string.
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

	external long function	str$element

!+
! Mainline
!-
mainline:

	call str$element (destination, element, delimiter, source)

	end sub
