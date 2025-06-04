	%title	"Convert a character string to a longword"
	%ident	"V1-002"

	function long lib_cvt_t_l (string source, long result)

!+
! Version:	V1-002
!
! Facility:	Library routines.
!
! Abstract:	Convert a character string to a longword integer in the
!		specified radix.
!
! Environment:	User mode.
!
! History:
!
!	22-Dec-1988, DBS, Version V1-001
! 001 -	Original version.
!	18-Dec-1995, DBS; Version V1-002
! 002 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	The source string is converted to a longword value by calling the
!	appropriate ots$ routine.  All conversions are unsigned.
!	The string can contain the following formats:
!		digits		use decimal radix
!		%Bdigits	use binary radix
!		%Ddigits	use decimal radix
!		%Odigits	use octal radix
!		%Xdigits	use hexadecimal radix
!
! Calling Sequence:
!	status = lib_cvt_t_l (string, value)
!		-or-
!	pushal	value
!	pushaq	string
!	calls	#2, g^lib_cvt_t_l
!
! Formal Argument(s):
!	source.rt.ds	Address of a string descriptor pointing to the string
!			to be converted.
!	result.wl.r	Address of a longword into which the result can be
!			placed.
!
! Implicit Inputs:
!	None
!
! Implicit Outputs:
!	None
!
! Completion Codes:
!	ss$_ivchar	Returned for an invalid radix.
!	As returned by the ots$cvt routines.
!
! Side Effects:
!	None
!--


	option	type = explicit,					  &
		constant type = integer,				  &
		size = (integer long, real double)

! External references (and includes)

%include "$OTSDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"
%include "$SSDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"

	external long function	ots$cvt_tb_l
	external long function	ots$cvt_to_l
	external long function	ots$cvt_tu_l
	external long function	ots$cvt_tz_l
	external long function	str$upcase
	external long function	str_len

! Type declarations for variables

	declare long	digit_count
	declare string	digits
	declare long	first
	declare long	radix
	declare string	radix_op

!+
! Mainline
!-
mainline:

	call str$upcase (digits, source)

	lib_cvt_t_l = ss$_normal
	digit_count = str_len (digits)
	first	    = 1
	radix	    = 10

	if (mid(digits, 1, 1) = "%") then
		first    = 3
		radix_op = mid(digits, 2, 1)
		if (radix_op = "B") then
		    	radix = 2
		else
		if (radix_op = "O") then
		    	radix = 8
		else
		if (radix_op = "D") then
		    	radix = 10
		else
		if (radix_op = "X") then
		    	radix = 16
		else
		radix = -1
		end if !(radix_op = "X") then
		end if !(radix_op = "D") then
		end if !(radix_op = "O") then
		end if !(radix_op = "B") then
	end if !(mid(digits, 1, 1) = "%") then

	if (radix = 2) then
		lib_cvt_t_l = ots$cvt_tb_l				  &
				(seg$(digits, first, digit_count)	  &
				,result by ref, 4 by value, 0 by value)
	else
	if (radix = 8) then
		lib_cvt_t_l = ots$cvt_to_l				  &
				(seg$(digits, first, digit_count)	  &
				,result by ref, 4 by value, 0 by value)
	else
	if (radix = 10) then
		lib_cvt_t_l = ots$cvt_tu_l				  &
				(seg$(digits, first, digit_count)	  &
				,result by ref, 4 by value, 0 by value)
	else
	if (radix = 16) then
		lib_cvt_t_l = ots$cvt_tz_l				  &
				(seg$(digits, first, digit_count)	  &
				,result by ref, 4 by value, 0 by value)
	else
    	lib_cvt_t_l = ss$_ivchar
	end if !(radix = 16) then
	end if !(radix = 10) then
	end if !(radix = 8) then
	end if !(radix = 2) then

  	end function
