	%title	"Format output in hexadecimal bytes"
	%ident	"V1-002"

	sub lib_output_seg_zb (string input_buffer			  &
				,long alleged_buffer_size		  &
				,long requested_segment_size)

!+
! Version:	V1-002
!
! Facility:	Library routines.
!
! Abstract:	To take a buffer and generate a formatted display of the
!		buffer as hex byte values.
!
! Environment:	User mode.
!
! History:
!
!	02-Apr-1990, DBS, Version V1-001
! 001 -	Original version.
!	18-Dec-1995, DBS; Version V1-002
! 002 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	Take the buffer as byte values and format them into hex.  All done
!	with SYS$FAO and output directed to sys$output.  Each segment, of
!	the specified size, contains a byte offset from the start of the
!	buffer.
!
! Calling Sequence:
!	call lib_output_seg_zb (buffer, buffer_size, segment_size)
!		-or-
!	pushal	segment_size
!	pushal	buffer_size
!	pushaq	buffer
!	calls	#3, g^lib_output_seg_zb
!
! Formal Argument(s):
!	input_buffer.rt.ds  Address of a string descriptor that points to the
!			buffer to be displayed.
!	alleged_buffer_size.rl.r  Address of a longword containing the number
!			of characters to display.
!	requested_segment_size.rl.r  Address of a longword containing the
!			number of bytes in each segment.
!
! Implicit Inputs:
!	None
!
! Implicit Outputs:
!	Output is directed to SYS$OUTPUT.
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

	external long function	lib$put_output
	external long function	sys$faol

! Type declarations for constants

	declare long constant	default_segment_size = 20
	declare long constant	max_segment_size = 40
	declare long constant	max_array_size = 43	! 40 + 3
	declare string constant	fao_data = "!6UL-[!XB!#(3XB)]"

! Type declarations for variables

	declare long	b_byte(1 to max_array_size)
	declare long	buffer_size
	declare long	bytes_this_segment
	declare long	counter
	declare long	fao_len
	declare long	first
	declare long	last
	declare string	fao_line
	declare long	loop
	declare long	number_of_segments
	declare long	segment_size

!+
! Mainline
!-
mainline:

	fao_line = space$(256)

	buffer_size = len(input_buffer)
	if ((alleged_buffer_size >= 1)					  &
			and (alleged_buffer_size <= buffer_size)) then
		buffer_size = alleged_buffer_size
	end if

	segment_size = requested_segment_size
	if (segment_size < 1) then
		segment_size = default_segment_size
	end if
	if (segment_size > max_segment_size) then
		segment_size = max_segment_size
	end if

	number_of_segments = (buffer_size - 1 + segment_size)/segment_size
	loop = 0

	while (loop < number_of_segments)
		first = loop*segment_size + 1
		last  = first + segment_size - 1
		if (last > buffer_size) then
			last = buffer_size
		end if
		bytes_this_segment = last - first + 1

		b_byte(1) = first - 1
		b_byte(2) = ascii(mid(input_buffer, first, 1))
		b_byte(3) = bytes_this_segment - 1

		for counter = 2 to bytes_this_segment
		    b_byte(counter+2)					  &
			= ascii(mid(input_buffer,first+counter-1,1))
		next counter

		call sys$faol (fao_data by desc				  &
				,fao_len by ref				  &
				,fao_line by desc			  &
				,b_byte() by ref)

		call lib$put_output (left(fao_line, fao_len))

		loop = loop + 1
	next

	end sub
