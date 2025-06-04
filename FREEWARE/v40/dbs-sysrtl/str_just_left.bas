	%title	"Left justify a string"
	%ident	"V1-003"

	sub str_just_left (string target, string source, string pad_char)

!+
! Version:	V1-003
!
! Facility:	Library routines.
!
! Abstract:	Left justify and pad a string.
!
! Environment:	User mode.
!
! History:
!
!	07-Dec-1988, DBS; Version V1-001
! 001 -	Original version.
!	29-Dec-1988, DBS; Version V1-002
! 002 -	Changed the order of the parameters to reflect the standard
!	18-Dec-1995, DBS; Version V1-003
! 003 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	This routine leftt justifies a string and right fills the target
!	with the pad character supplied.
!
! Calling Sequence:
!
!	call str_just_left (target, source, pad_char)
!
! Formal Argument(s):
!	target.wt.ds	Somewhere to put the result.
!	source.rt.ds	The source string.
!	pad_char.rt.ds	The character with which to pad out the string.
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

	declare long	loop
	declare long	source_len
	declare long	target_len

!+
! Mainline
!-
mainline:

	source_len = str_len (source)
	target_len = len(target)

	lset target = left(source, source_len)

	loop = source_len + 1

	while (loop <= target_len)
		mid(target, loop, 1) = left(pad_char, 1)
		loop = loop + 1
	next

	end sub
