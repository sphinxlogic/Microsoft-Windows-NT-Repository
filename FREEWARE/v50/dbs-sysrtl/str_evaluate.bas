	%title	"Evaluate a string expression"
	%ident	"V1-005"

	function long str_evaluate (string expression			  &
					,long dot			  &
					,long q				  &
					,long z				  &
					,long result)

!+
! Version:	V1-005
!
! Facility:	Library routines.
!
! Abstract:	Evaluate a string expression.
!
! Environment:	User mode.
!
! History:
!
!	27-Nov-1989, DBS, Version V1-001
! 001 -	Original version.
!	01-Dec-1989, DBS; Version V1-002
! 002 -	Added call to lib$int_over () to disable integer overflows.
!	09-Apr-1990, DBS; Version V1-003
! 003 -	Check for upper and lowercase q's and z's.  (An oversight earlier.)
!	18-Dec-1995, DBS; Version V1-004
! 004 -	Converted from FORTRAN to BASIC.
!	19-Jan-1996, DBS; Version V1-005
! 005 -	Added code to optionally include lib$int_over - which is different
!	on the alpha - set /variant to the hw_model value ($getsyi).
!-

!++
! Functional Description:
!	This routine will take a string and parse and evaluate the simple
!	arithmetic expression contained within it.  The evaluation does not
!	handle any form of bracketing and the string is parsed from left to
!	right and terminates on any error or end of string.
!	Valid operators are (all operations are INTEGER):
!		+	addition
!		-	subtraction
!		*	multiplication
!		/	division
!	exclamation mark	logical OR
!		#	logical XOR
!		&	logical AND
!		_	logical NOT
!	The following symbols are accepted and will be given the values passed,
!	"." and "Q" and "Z".
!	[Because of the history of this routine and its original use, these
!	values have the following meaning:
!		.	the current location (or address)
!		Q	the last value displayed
!		Z	the base location (or address)]
!
! Calling Sequence:
!
!	call str_evaluate (expression, dot, q, z, result)
!
! Formal Argument(s):
!	expression.rt.ds The string containing the expression to be evaluated.
!	dot.rl.r	The value to use for ".".
!	q.rl.r		The value to use for "Q".
!	z.rl.r		The value to use for "Z".
!	result.wl.r	Somewhere to return the answer.
!
! Implicit Inputs:
!	None
!
! Implicit Outputs:
!	None
!
! Completion Codes:
!	Similar to those returned by the ots$cvt routines.
!
! Side Effects:
!	None
!--


	option	type = explicit,					  &
		constant type = integer,				  &
		size = (integer long, real double)

! External references (and includes)

%include "$SSDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"

%if (%variant < 1024) %then			! it's a VAX
	external long function	lib$int_over
%end %if
	external long function	lib_cvt_t_l
	external long function	str_len

! Type declarations for constants

	declare long constant	disable_ = 0
	declare long constant	false = 0
	declare string constant	operator_list = "+-*/!&#"
	declare long constant	true = -1

! Type declarations for variables

	declare long	cvtstatus
	declare long	do_compliment
	declare long	expression_len
	declare long	first
	declare long	keep_looking
	declare long	last
	declare string	operator
	declare long	total
	declare long	vvalue

	%sbttl	"Mainline"
!+
! Mainline
!-
mainline:

%if (%variant < 1024) %then			! it's a VAX
	call lib$int_over (disable_)
%end %if

	str_evaluate = ss$_normal	! be optimistic

	expression_len = str_len (expression)

	if (expression_len = 0) then
		str_evaluate = ss$_ivchar
	else
	result = 0
	total  = 0
	first  = 1
	operator = "+"
	while (first <= expression_len)
	    last = first
	    keep_looking = true

	    while (keep_looking)
		if (last >= expression_len) then
		    keep_looking = false
		else
		if (pos(operator_list,seg$(expression,last,last),1) <> 0)
		    then
		    last = last - 1
		    keep_looking = false
		else
		last = last + 1
		end if !(pos(operator_list,seg$(expression,last,last),1) <> 0)
		end if !(last >= expression_len) then
	    next

	    if (seg$(expression, first, first) = "_") then
		do_compliment = true
		first = first + 1
	    else
	    do_compliment = false
	    end if !(seg$(expression, first, first) = "_") then

	    if (seg$(expression, first, last) = ".") then
		vvalue = dot
		cvtstatus = ss$_normal
	    else
	    if ((seg$(expression, first, last) = "Q")			  &
		    or (seg$(expression, first, last) = "q")) then
		vvalue = q
		cvtstatus = ss$_normal
	    else
	    if ((seg$(expression, first, last) = "Z")			  &
		    or (seg$(expression, first, last) = "z")) then
		vvalue = z
		cvtstatus = ss$_normal
	    else
	    cvtstatus = lib_cvt_t_l (seg$(expression, first, last), vvalue)
	    end if !((seg$(expression, first, last) = "Z")		  &
	    end if !((seg$(expression, first, last) = "Q")		  &
	    end if !((seg$(expression, first, last) = ".")		  &

	    if ((cvtstatus and 1) <> 1) then
		first = expression_len + 1
		str_evaluate = cvtstatus
	    else
	    if (do_compliment) then
		vvalue = (not vvalue)
	    end if !(do_compliment) then
	    if (operator = "+") then
		total = total + vvalue
	    else
	    if (operator = "-") then
		total = total - vvalue
	    else
	    if (operator = "*") then
		total = total*vvalue
	    else
	    if (operator = "!") then
		total = (total or vvalue)
	    else
	    if (operator = "&") then
		total = (total and vvalue)
	    else
	    if (operator = "#") then
		total = (total xor vvalue)
	    else
	    if (operator = "/") then
		if (vvalue = 0) then
		    first = expression_len + 1
		    str_evaluate = ss$_intdiv
		else
		total = total/vvalue
		end if !(vvalue = 0) then
	    end if !(operator = "/") then
	    end if !(operator = "#") then
	    end if !(operator = "&") then
	    end if !(operator = " ") then
	    end if !(operator = "*") then
	    end if !(operator = "-" then
	    end if !(operator = "+") then
	    if (last < expression_len) then
		operator = seg$(expression, last+1, last+1)
	    end if !(last < expression_len) then
	    first = last + 2
	    end if !((cvtstatus and 1) <> 1) then
	next

	result = total
	end if !(expression_len = 0) then

	end function
