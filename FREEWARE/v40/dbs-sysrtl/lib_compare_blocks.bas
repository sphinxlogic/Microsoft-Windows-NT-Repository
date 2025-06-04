	%title	"Display differences in two 512 byte blocks"
	%ident	"X1-002"

	sub lib_compare_blocks (long block_number			  &
				,string buffer_1			  &
				,string buffer_2)

!+
! Version:	X1-002
!
! Facility:	Library routines
!
! Abstract:	
!
! Environment:	User mode.
!
! History:
!
!	03-Jul-1991, DBS; Version X1-001
! 001 -	Original version.
!	18-Dec-1995, DBS; Version X1-002
! 002 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!
! Calling Sequence:
!	call lib_compare_block (...)
!
! Formal Argument(s):
!	block_number.rl.r	The number of the block that is being
!			displayed.
!	buffer_1.rt.ds	The 512 characters in this block.
!	buffer_2.rt.ds	The 512 characters in that block.
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

	external long function	lib$analyze_sdesc
	external long function	lib$put_output
	external long function	sys$fao

! Type declarations for constants

	declare string constant	fao_count = " !UL difference!%S found"
	declare string constant	fao_format = "    !UL: !3UL = (!3UL,!3UL)" &
					+ "    !AF -- !AF"		  &
					+ "    [ Hex !4XL = (!2XL,!2XL) ]"
	declare string constant	fao_header = "Differences in block !UL"

! Type declarations for variables

	declare long	counter
	declare long	fao_len
	declare string	fao_line
	declare long	offset
	declare long	value_1
	declare long	value_2
	declare long	string1_address
	declare word	string1_size
	declare long	string2_address
	declare word	string2_size

!+
! Mainline
!-
mainline:

	fao_line = space$(132)

	call lib$analyze_sdesc (buffer_1 by desc			  &
				,string1_size by ref			  &
				,string1_address by ref)
	call lib$analyze_sdesc (buffer_2 by desc			  &
				,string2_size by ref			  &
				,string2_address by ref)

	call sys$fao (fao_header by desc				  &
			,fao_len by ref					  &
			,fao_line by desc				  &
			,block_number by value)
	call lib$put_output (left(fao_line, fao_len))

	counter = 0
	offset = 1

	while ((offset > 0) and (offset <= 512))
		value_1 = ascii(mid(buffer_1, offset, 1))
		value_2 = ascii(mid(buffer_2, offset, 1))

		if (value_1 <> value_2) then
		    call sys$fao (fao_format by desc			  &
				,fao_len by ref				  &
				,fao_line by desc			  &
				,block_number by value			  &
				,offset by value			  &
				,value_1 by value			  &
				,value_2 by value			  &
				,1 by value				  &
				,(string1_address + offset - 1) by value  &
				,1 by value				  &
				,(string2_address + offset - 1) by value  &
				,offset by value			  &
				,value_1 by value			  &
				,value_2 by value)
		    call lib$put_output (left(fao_line, fao_len))
		    counter = counter + 1
		end if
			
		offset = offset + 1
 	next

	call sys$fao (fao_count by desc					  &
			,fao_len by ref					  &
			,fao_line by desc				  &
			,counter by value)
	call lib$put_output (left(fao_line, fao_len))

	end sub
