	%title	"Perform string substitution"
	%ident	"V1-003"

	sub str_substitute (string dst_str, string src_str		  &
				,string old_chr, string new_chr)

!+
! Version:	V1-003
!
! Facility:	Library routines.
!
! Abstract:	Scan a character string replacing occurences of one string
!		with another and write the result to a destination string.
!
! Environment:	User mode.
!
! History:
!
!	08-Nov-1988, DBS; Version V1-001
! 001 -	Original version.
!	29-Dec-1988, DBS; Version V1-002
! 002 -	Changed the order of the parameters to reflect the standard.
!	18-Dec-1995, DBS; Version V1-003
! 003 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	This routine will perform one scan of a source character string and
!	replace all occurences of the "old_chr" with "new_chr".  The result
!	will be written to the character string dst_str.  For single chara-
!	cter substitutions, or substitutions of equal length, the source
!	and destination can be the same, but for substitutions where the
!	old and new strings are different sizes it is best to use different
!	strings for the source and destination.
!
! Calling Sequence:
!
!	call str_substitute (dst_str, src_str, old_chr, new_chr)
!
! Formal Argument(s):
!	dst_str.wt.ds	The destination string.
!	src_str.rt.ds	The source string.
!	old_chr.rt.ds	The character(s) that should be replaced.
!	new_chr.rt.ds	The character(s) to substitute for the old.
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
!
!--


	option	type = explicit,					  &
		constant type = integer,				  &
		size = (integer long, real double)

! Type declarations for variables

	declare long	dst_inc
	declare long	dst_loop
	declare long	new_len
	declare long	old_len
	declare long	src_inc
	declare long	src_len
	declare long	src_loop

!+
! Mainline
!-
mainline:

	src_len	 = len(src_str)
	old_len	 = len(old_chr)
	new_len	 = len(new_chr)
	src_loop = 1
	dst_loop = 1

	while (src_loop <= src_len)
		if (mid(src_str, src_loop, old_len) = old_chr)
			then
			mid(dst_str, dst_loop, new_len) = new_chr
			src_inc	= old_len
			dst_inc	= new_len
		else
		mid(dst_str, dst_loop, 1) = mid(src_str, src_loop, 1)
		src_inc	= 1
		dst_inc = 1
		end if
		src_loop = src_loop + src_inc
		dst_loop = dst_loop + dst_inc
	next

	end sub
