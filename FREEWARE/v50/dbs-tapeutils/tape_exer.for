	program tape_exer

C+
C Version:	X1-003
C
C Facility:	Diagnostic Utilities.
C
C Abstract:	This program will exercise a tape by writing all over it then
C		reading it back and verifying the contents.
C
C Environment:	VMS V5.2 and later.
C
C		Link with option "PSECT_ATTR=BUFFER_PSECT,PAGE"
C
C History:
C
C	02-May-1990, DBS; Version X1-001
C 001 -	Original version.
C	08-May-1990, DBS; Version X1-002
C 002 -	Big cleanup to make it look a bit nicer.
C	05-Dec-1991, DBS; Version X1-003
C 003 -	Include error count in commencing and complete messages.
C-

 	implicit none

C External references (and includes)

	include		'($DCDEF)/nolist'
	include		'($DVIDEF)/nolist'
	include		'($IODEF)/nolist'
	include		'($RMSDEF)/nolist'
	include		'($SSDEF)/nolist'

	external	compare_buffers
	integer*4	lib$get_foreign
	external	lib$get_foreign
	integer*4	lib$put_output
	external	lib$put_output
	integer*4	lib$stop
	external	lib$stop
	external	report_error
	integer*4	sys$assign
	external	sys$assign
	integer*4	sys$fao
	external	sys$fao
	integer*4	sys$getdviw
	external	sys$getdviw
	integer*4	sys$qio
	external	sys$qio
	integer*4	sys$qiow
	external	sys$qiow
	integer*4	sys$waitfr
	external	sys$waitfr

C Structures

	structure /iosb/
		integer*2	sts		/0/
		integer*4	count		/0/
		integer*2	%fill
	end structure
	volatile /iosb/

	structure /item_list_3/
		integer*2	buffer_length		/0/
		integer*2	item_code		/0/
		integer*4	buffer_address		/0/
		integer*4	return_length_address	/0/
	end structure
	volatile /item_list_3/

C Type declarations for parameters and PARAMETER statements

	character	device_prompt*(*)
	character	exit_msg*(*)
	character	fao_starting*(*)
	character	fao_written*(*)
	character	intro_msg*(*)
	integer*4	max_index
	integer*4	byte_count
	integer*4	pattern_1
	character	start_verify_msg*(*)

	parameter	(device_prompt = '_tape unit: ')
	parameter	(exit_msg = '!%D  Exercising of !AS complete'
	1				//', rewinding (ErrCnt=!UL)')
	parameter	(fao_starting = '!%D  Starting to write '
	1				//'!UL byte blocks (Errcnt=!UL)')
	parameter	(fao_written = '!%D  !UL blocks written to tape')
	parameter	(intro_msg = '!%D  Exercising !AS tape unit !AS')
	parameter	(max_index = 16383)
	parameter	(byte_count = max_index*4)
	parameter	(pattern_1 = 'FFAA5500'X)
	parameter	(start_verify_msg = '!%D  Starting verification pass')

C Type declarations for variables

	integer*4	blocks_read		/0/
	integer*4	blocks_written		/0/
	character	date_time_buffer*23	/' '/
	character	device_name*32		/' '/
	integer*4	device_name_len		/0/
	integer*4	dvi_devclass		/0/
	integer*4	dvi_errcnt		/0/
	character	dvi_fulldevnam*64	/' '/
	integer*4	dvi_s_fulldevnam	/0/
	character	dvi_media_name*64	/' '/
	integer*4	dvi_s_media_name	/0/
	integer*4	dvi_mnt			/0/
	integer*4	event_flag		/0/
	integer*4	index			/0/
	integer*4	iochannel		/0/
	integer*4	lbn_to_read		/0/
	character	line*132
	integer*4	line_len		/0/
	integer*4	loop
	integer*4	read_buffer(max_index)
	integer*4	status			/0/
	integer*4	write_buffer(max_index)

	record /iosb/		iostat(2)
	record /item_list_3/	itmlst(6)

C Commons

	common /buffer_psect/	read_buffer
	1			,write_buffer

C+
C Mainline - Tape_Exer
C-

	status = lib$get_foreign (device_name, device_prompt, device_name_len)
	if ((status .eq. rms$_eof) .or. (device_name_len .eq. 0))
	1	call lib$stop (%val('10000001'X))

	status = sys$assign (device_name(1:device_name_len), iochannel,,)
	if (status .ne. ss$_normal) call lib$stop (%val(status))

	itmlst(1).buffer_length		= 4
	itmlst(1).item_code		= dvi$_devclass
	itmlst(1).buffer_address	= %loc(dvi_devclass)
	itmlst(1).return_length_address	= 0
	itmlst(2).buffer_length		= 4
	itmlst(2).item_code		= dvi$_mnt
	itmlst(2).buffer_address	= %loc(dvi_mnt)
	itmlst(2).return_length_address	= 0
	itmlst(3).buffer_length		= len(dvi_fulldevnam)
	itmlst(3).item_code		= dvi$_fulldevnam
	itmlst(3).buffer_address	= %loc(dvi_fulldevnam)
	itmlst(3).return_length_address	= %loc(dvi_s_fulldevnam)
	itmlst(4).buffer_length		= len(dvi_media_name)
	itmlst(4).item_code		= dvi$_media_name
	itmlst(4).buffer_address	= %loc(dvi_media_name)
	itmlst(4).return_length_address	= %loc(dvi_s_media_name)
	itmlst(5).buffer_length		= 4
	itmlst(5).item_code		= dvi$_errcnt
	itmlst(5).buffer_address	= %loc(dvi_errcnt)
	itmlst(5).return_length_address	= 0

	status = sys$getdviw (,%val(iochannel),,%ref(itmlst),%ref(iostat),,,)
	if (status .ne. ss$_normal) call lib$stop (%val(status))
	status = iostat(1).sts
	if (status .ne. ss$_normal) call lib$stop (%val(status))
	if (dvi_devclass .ne. dc$_tape) call lib$stop (%val(ss$_ivdevnam))
	if (.not. dvi_mnt) call lib$stop (%val(ss$_devnotmount))
	
	call sys$fao (%descr(intro_msg)
	1		,%ref(line_len)
	1		,%descr(line)
	1		,%val(0)
	1		,%descr(dvi_media_name(1:dvi_s_media_name))
	1		,%descr(dvi_fulldevnam(1:dvi_s_fulldevnam)))
	call lib$put_output (line(1:line_len))

	do loop = max_index, 1, -1
		write_buffer(loop) = pattern_1
	enddo !loop = max_index, 1, -1

	blocks_written = 0

	status = sys$qiow (,%val(iochannel),%val(io$_rewind),,,,,,,,,)
	if (.not. status) call lib$stop (%val(status))

	call sys$fao (%descr(fao_starting)
	1		,%ref(line_len)
	1		,%descr(line)
	1		,%val(0)
	1		,%val(byte_count)
	1		,%val(dvi_errcnt))
	call lib$put_output (line(1:line_len))

	event_flag = 0
	status = sys$qio (%val(event_flag)
	1			,%val(iochannel)
	1			,%val(io$_writelblk)
	1			,%ref(iostat(event_flag + 1)),,
	1			,%val(%loc(write_buffer))
	1			,%val(byte_count),,,,)
	event_flag = 1

	do while (status .eq. ss$_normal)
		status = sys$qio (%val(event_flag)
	1				,%val(iochannel)
	1				,%val(io$_writelblk)
	1				,%ref(iostat(event_flag + 1)),,
	1				,%val(%loc(write_buffer))
	1				,%val(byte_count),,,,)
		blocks_written = blocks_written + 1

		event_flag = mod(event_flag + 1, 2)
		call sys$waitfr (%val(event_flag))
		status = iostat(event_flag + 1).sts
	enddo !while (status .eq. ss$_normal)

	event_flag = mod(event_flag + 1, 2)
	call sys$waitfr (%val(event_flag))

	call sys$fao (%descr(fao_written)
	1		,%ref(line_len)
	1		,%descr(line)
	1		,%val(0)
	1		,%val(blocks_written))
	call lib$put_output (line(1:line_len))

	if (status .ne. ss$_endoftape)
	1	call report_error (status, blocks_written, iochannel)

	status = sys$qiow (,%val(iochannel),%val(io$_rewind),,,,,,,,,)
	if (.not. status) call lib$stop (%val(status))

	call sys$fao (%descr(start_verify_msg)
	1		,%ref(line_len)
	1		,%descr(line)
	1		,%val(0))
	call lib$put_output (line(1:line_len))

	blocks_read = 0

	do while ((blocks_read .lt. blocks_written)
	1		.and. (status .eq. ss$_normal))
		status = sys$qiow (%val(0), %val(iochannel)
	1				,%val(io$_readlblk)
	1				,%ref(iostat(1)),,
	1				,%val(%loc(read_buffer))
	1				,%val(byte_count),,,,)
		blocks_read = blocks_read + 1
		call compare_buffers (blocks_read)
		status = iostat(1).sts
	enddo !while ((blocks_read .le. blocks_written)

	if (blocks_read .ne. blocks_written)
	1	call report_error (status, blocks_read, iochannel)

	status = sys$getdviw (,%val(iochannel),,%ref(itmlst),%ref(iostat),,,)
	call sys$fao (%descr(exit_msg)
	1		,%ref(line_len)
	1		,%descr(line)
	1		,%val(0)
	1		,%descr(dvi_fulldevnam(1:dvi_s_fulldevnam))
	1		,%val(dvi_errcnt))
	call lib$put_output (line(1:line_len))

	status = sys$qiow (,%val(iochannel),%val(io$_rewind),,,,,,,,,)
	if (.not. status) call lib$stop (%val(status))

	end

	subroutine compare_buffers (block_number)

C++
C Functional Description:
C	This routine will compare the read and write buffers to verify the
C	contents.  On any mismatch, the block number (as passed) and the
C	offending longword along with the expected value are reported.
C
C Calling Sequence:
C	call compare_buffers (...)
C
C Formal Argument(s):
C	block_number.rl.r  The number of the block we are checking (for
C			   reporting purposes only).
C
C Implicit Inputs:
C	The read and write buffers via the buffer_psect common.
C
C Implicit Outputs:
C	None
C
C Completion Codes:
C	None
C
C Side Effects:
C	None
C--

 	implicit none

C External references (and includes)

	external	lib$put_output
	external	sys$fao

C Structures

C Type declarations for parameters and PARAMETER statements

	character	fao_mismatch*(*)
	integer*4	max_index

	parameter	(fao_mismatch = 'Verify failed on block !UL(!UL)'
	1			//', expected !XL but found !XL')
	parameter	(max_index = 16383)

C Type declarations for variables

	integer*4	block_number

	logical*4	check_next		/.true./
	character	line*132		/' '/
	integer*4	line_len		/0/
	integer*4	loop			/0/
	integer*4	read_buffer(max_index)
	integer*4	write_buffer(max_index)

C Commons

	common /buffer_psect/	read_buffer
	1			,write_buffer

C+
C Mainline - compare_buffers
C-

	check_next = .true.
	loop = 1

	do while (check_next .and. (loop .le. max_index))
		if (read_buffer(loop) .ne. write_buffer(loop)) then
			call sys$fao (%descr(fao_mismatch)
	1				,%ref(line_len)
	1				,%descr(line)
	1				,%val(block_number)
	1				,%val(loop*4)
	1				,%val(write_buffer(loop))
	1				,%val(read_buffer(loop)))
			call lib$put_output (line(1:line_len))
			check_next = .false.
		endif !(read_buffer(loop) .ne. write_buffer(loop)) then
		loop = loop + 1
	enddo !while (check_next .and. (loop .le. max_index))

	return
	end

	subroutine report_error (status, block_number, iochannel)

C++
C Functional Description:
C	This routine will report the block number that generated the error,
C	issue a rewind command to the tape drive then use lib$stop to
C	report the status.
C
C Calling Sequence:
C	call report_status (...)
C
C Formal Argument(s):
C	status.rl.r	The status value to report.
C	block_number.rl.r  The number of the block we were processing when
C			   we got the error.
C	iochannel.rl.r	The channel assigned to the tape drive, so we can do
C			a rewind before we exit.
C
C Implicit Inputs:
C	None
C
C Implicit Outputs:
C	None
C
C Completion Codes:
C	None
C
C Side Effects:
C	None
C--

 	implicit none

C External references (and includes)

	include '($IODEF)/nolist'

	external	lib$put_output
	external	lib$stop
	external	sys$fao
	external	sys$qiow

C Structures

C Type declarations for parameters and PARAMETER statements

	character	error_msg*(*)
	parameter	(error_msg = '!%D  Error detected at block !UL'
	1				//', rewinding')

C Type declarations for variables

	integer*4	status
	integer*4	block_number
	integer*4	iochannel

	character	line*132		/' '/
	integer*4	line_len		/0/

C Commons

C+
C Mainline - report_error
C-

	call sys$fao (%descr(error_msg)
	1		,%ref(line_len)
	1		,%descr(line)
	1		,%val(0)
	1		,%val(block_number))
	call lib$put_output (line(1:line_len))

	call sys$qiow (,%val(iochannel),%val(io$_rewind),,,,,,,,,)

	call lib$stop (%val(status))

	return
	end
