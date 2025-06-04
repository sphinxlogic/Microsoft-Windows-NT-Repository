	%title	"Block patching routines"
	%ident	"X1-002"

	sub lib_patch_block (string input_buffer, long modified)

!++
! Functional Description:
!	This routine will allow editing of the character buffer passed.
!	Editing is done on a byte by byte basis.
!
! Calling Sequence:
!	call lib_patch_block (...)
!
! Formal Argument(s):
!	buffer.mt.ds	The buffer to patch, should be 512 bytes long.
!	modified.wlu.r	A flag to indicate whether the block was modified
!			or not.
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

%include "$RMSDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"
%include "$SSDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"

	external long function	lib$put_output
	external long function	lib_patch_input_integer
	external long function	lib_patch_valid_byte
	external long function	sys$fao

! Type declarations for constants

	declare string constant	backpedal = "^"
	declare string constant	byte_prompt_str = "Byte? "
	declare long constant	false = 0
	declare string constant	fao_prompt =				  &
				"!3UL Old= (%x!2XL,%o!3OL) !3AS !3UL, New= "
	declare long constant	true = -1

! Type declarations for variables

	declare string	ascii_chars
	declare long	byte_l
	declare long	keep_going
	declare long	high_bit_on
	declare long	low_bits
	declare long	more_bytes
	declare string	new_prompt
	declare long	new_prompt_len
	declare long	new_value
	declare long	old_value
	declare long	result_len
	declare string	result_str
	declare long	sstatus

!+
! Mainline
!-
mainline:

	ascii_chars = space$(3)
	new_prompt = space$(80)
	result_str = space$(256)
	modified = false

	sstatus = lib_patch_input_integer (byte_prompt_str		  &
						,result_str, byte_l)

	keep_going = (lib_patch_valid_byte (byte_l, sstatus)		  &
				and (byte_l <> 0))
! note that a ^ at this stage will exit from this block . . .

	while (keep_going)
		more_bytes = true
		while (more_bytes)
			old_value   = ascii(mid(input_buffer, byte_l, 1))
			low_bits    = (old_value and 127)
			high_bit_on = ((old_value and 128) = 128)

			if (high_bit_on) then
				mid(ascii_chars, 1, 1) = "+"
			else
			mid(ascii_chars, 1, 1) = " "
			end if !(high_bit_on) then

			if (low_bits < 32) then
				mid(ascii_chars, 2, 2) = "^"		  &
							+ chr$(low_bits+64)
			else
			if (low_bits = 127) then
				mid(ascii_chars, 2, 2) = "ro"
			else
			mid(ascii_chars, 2, 2) = "'" + chr$(low_bits)
			end if !(low_bits = 127) then
			end if !(low_bits < 32) then

			call sys$fao (fao_prompt by desc		  &
					,new_prompt_len by ref		  &
					,new_prompt by desc		  &
					,byte_l by value		  &
					,old_value by value		  &
					,old_value by value		  &
					,ascii_chars by desc		  &
					,old_value by value)
			sstatus = lib_patch_input_integer		  &
					(left(new_prompt, new_prompt_len) &
					,result_str, new_value)

			if (left(result_str, 1) = backpedal) then
				byte_l = byte_l - 1
				if (byte_l < 1) then
					byte_l = 512
				end if !(byte_l < 1) then
			else
			if ((sstatus = rms$_eof) or			  &
					(left(result_str, 1) = "E") or	  &
					(left(result_str, 1) = "e")) then
				more_bytes = false
			else
			if ((new_value < 0) or (new_value > 256)) then
				call lib$put_output			  &
				     ("%PATCH-E-INVBYTE, invalid byte value")
			else
			if (new_value > 0) then
				if (new_value = 256) then
					new_value = 0
				end if !(new_value = 256) then
				if (new_value <> old_value) then
				      	modified = true
				end if !(new_value <> old_value) then
				mid(input_buffer, byte_l, 1) = chr$(new_value)
			end if !(new_value > 0) then
			byte_l = byte_l + 1
			if (byte_l > 512) then
				byte_l = 1
			end if !(byte_l > 512) then
			end if !((new_value < 0) or (new_value < 256)) then
			end if !((sstatus = rms$_eof) or
			end if !(left(result_str, 1) = backpedal) then
		next

		sstatus = lib_patch_input_integer (byte_prompt_str	  &
						,result_str, byte_l)
		keep_going = (lib_patch_valid_byte (byte_l, sstatus)	  &
					and (byte_l <> 0))
 	next

	end sub

	function long lib_patch_valid_byte (long byte_l, long sstatus)

!++
! Functional Description:
!	To indicate whether the byte value and status (as returned by the
!	input routine) mean the byte value is ok.
!
! Calling Sequence:
!	status = lib_patch_valid_byte (...)
!
! Formal Argument(s):
!	byte.rl.r	The value as entered by the user.
!	status.rl.r	The status as returned by the input routine.
!
! Implicit Inputs:
!	None
!
! Implicit Outputs:
!	None
!
! Completion Codes:
!	ss$_normal or as returned by the input routine.
!
! Side Effects:
!	None
!--

	option	type = explicit,					  &
		constant type = integer,				  &
		size = (integer long, real double)

! External references (and includes)

%include "$OTSDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"
%include "$RMSDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"
%include "$SSDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"

	external long function	lib$put_output

!+
! Mainline
!-
mainline:

	lib_patch_valid_byte = ss$_normal

	if ((sstatus and 1) <> 1) then
		lib_patch_valid_byte = sstatus
	else
	if ((byte_l < 0) or (byte_l > 512)) then
		lib_patch_valid_byte = ss$_badparam
		call lib$put_output					  &
			('%PATCH-E-BADBYTE, byte value out of bounds')
	end if
	end if

	end function

	function long lib_patch_input_integer (string prompt_str	  &
						,string result_str	  &
						,long result_val)

!++
! Functional Description:
!	Handle the prompting and input of integer data.  Allows for the
!	use of radix operators in the input string.
!
! Calling Sequence:
!	status = lib_patch_input_integer (...)
!
! Formal Argument(s):
!	prompt.rt.ds	String to use for the prompt.
!	result_str.wt.ds The result as entered by the user.
!	result_val.wl.r	The integer value of the entered string.
!
! Implicit Inputs:
!	None
!
! Implicit Outputs:
!	None
!
! Completion Codes:
!	As returned by lib_cvt_t_l.
!
! Side Effects:
!	None
!--

	option	type = explicit,					  &
		constant type = integer,				  &
		size = (integer long, real double)

! External references (and includes)

%include "$SSDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"

	external long function	lib_get_input
	external long function	lib_cvt_t_l
	external long function	str_collapse
	external long function	str_len

! Type declarations for constants

	declare string constant	backpedal = "^"
	declare string constant single_quote = "'"

! Type declarations for variables

	declare long	cvt_status
	declare long	result_len

!+
! Mainline
!-
mainline:

	result_val = 0

	cvt_status = lib_get_input (result_str, prompt_str, result_len)
	call str_collapse (result_str, result_str)

	result_len = str_len (result_str)

	if (result_len <> 0) then
		if (left(result_str, 1) = single_quote) then
			if (result_len > 1) then
				result_val = ascii(mid(result_str, 2, 1))
			end if
		else
		if (left(result_str, 1) = backpedal) then
			result_val = 0
		else
		if (cvt_status) then
			cvt_status = lib_cvt_t_l (left(result_str,result_len) &
							 ,result_val)
		end if
		end if
		end if
	end if

	lib_patch_input_integer = cvt_status

	end function
