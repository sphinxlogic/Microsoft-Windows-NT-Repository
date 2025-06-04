	%title	"Formatted text display"
	%ident	"V1-002"

	sub lib_output_seg_t (string input_buffer			  &
				,long alleged_buffer_size		  &
				,long requested_segment_size)

!+
! Version:	V1-002
!
! Facility:	Library routines.
!
! Abstract:	To take a buffer and generate a formatted display of the
!		printable characters (to sys$output).
!
! Environment:	User mode.
!
! History:
!
!	30-Mar-1990, DBS, Version V1-001
! 001 -	Original version.
!	18-Dec-1995, DBS; Version V1-002
! 002 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	Here we check the buffer for printable characters and convert those
!	that aren't into those that are, then we display it.  All done with
!	SYS$FAO.  Output is to sys$output and done in segments of the
!	specified size.  Each segment is preceeded by the offset in bytes
!	from the start of the buffer.
!
! Calling Sequence:
!	call lib_output_seg_t (input_buffer, buffer_size, segment_size)
!		-or-
!	pushal	segment_size
!	pushal	buffer_size
!	pushaq	input_buffer
!	calls	#3, g^lib_output_seg_t
!
! Formal Argument(s):
!	input_buffer.rt.ds  Address of a string descriptor that points to the
!			characters to be displayed.
!	alleged_buffer_size.rl.r  Address of a longword containing the number
!			of characters to be displayed.
!	requested_segment_size.rl.r  Address of a longword containing the
!			number of bytes to be displayed in each segment.
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

	external long function	lib$analyze_sdesc
	external long function	lib$put_output
	external long function	sys$fao

! Type declarations for constants

	declare long constant	default_segment_size = 64
	declare string constant	fao_data = "!6UL-[!AF]"

! Type declarations for variables

	declare long	buffer_size
	declare long	fao_len
	declare string	fao_line
	declare long	first
	declare long	last
	declare long	loop
	declare long	number_of_segments
	declare long	segment_size
	declare long	string_address
	declare word	string_size

!+
! Mainline
!-
mainline:

	fao_line = space$(256)

	call lib$analyze_sdesc (input_buffer by desc			  &
				,string_size by ref			  &
				,string_address by ref)

	buffer_size = len(input_buffer)
	if ((alleged_buffer_size >= 1)					  &
			and (alleged_buffer_size <= buffer_size))
		then
		buffer_size = alleged_buffer_size
	end if !((alleged_buffer_size >= 1)

	segment_size = requested_segment_size
	if (segment_size < 1) then
		segment_size = default_segment_size
	end if !(segment_size < 1) then

	number_of_segments = (buffer_size - 1 + segment_size)/segment_size
	loop = 0

	while (loop < number_of_segments)
		first = loop*segment_size + 1
		last  = first + segment_size - 1
		if (last > buffer_size) then
			last = buffer_size
		end if !(last > buffer_size) then

		call sys$fao (fao_data by desc				  &
				,fao_len by ref				  &
				,fao_line by desc			  &
				,(first - 1) by value			  &
				,(last - first + 1) by value		  &
				,(string_address + first - 1) by value)

		call lib$put_output (left(fao_line, fao_len))

		loop = loop + 1
	next

	end sub
