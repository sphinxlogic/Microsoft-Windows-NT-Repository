	%title	"Block mode I/O - Read"
	%ident	"V1-003"

	function long bio_read (string bio_buffer			  &
				,rabdef bio_rab				  &
				,long bio_block)

!+
! Version:	V1-003
!
! Facility:	Library routines - Block mode I/O.
!
! Abstract:	Perform a block mode read on a previously opened file.
!
! Environment:	User mode.
!
! History:
!
!	09-Dec-1988, DBS; Version V1-001
! 001 -	Original version.
!	29-Dec-1988, DBS; Version V1-002
! 002 -	Changed the order of the parameters to reflect the standard.
!	27-Dec-1995, DBS; Version V1-003
! 003 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	This routine performs a block mode read on a file, therefore bypassing
!	the normal RMS file i/o processing and allowing you much more scope
!	and freedom (more rope?) to play around with files.
!
! Calling Sequence:
!	status = bio_read (buffer, %val(for$rab(lun)), block_number)
!
! Formal Argument(s):
!	bio_buffer.wt.ds  Address of a descriptor pointing to a 512 byte
!			buffer to receive the block.
!	bio_rab.rl.v	Address of the rab (from for$rab).
!	bio_block.rl.r	Address of a longword containing the number of the
!			virtual block within the file to read.
!
! Implicit Inputs:
!	None
!
! Implicit Outputs:
!	None
!
! Completion Codes:
!	The value retuned by sys$read is passed back to the user.
!
! Side Effects:
!	None
!--


	option	type = explicit,					  &
		constant type = integer,				  &
		size = (integer long, real double)

! External references (and includes)

%include "$RABDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"

	external long function	lib$analyze_sdesc
	external long function	sys$read

! Type declarations for constants

	declare word constant	bio_buffer_size = 512

! Type declarations for variables

	declare long	buffer_address
	declare word	buffer_size

!+
! Mainline
!-
mainline:

	call lib$analyze_sdesc (bio_buffer by desc			  &
				,buffer_size by ref			  &
				,buffer_address by ref)

	bio_rab::rab$l_bkt = bio_block
	bio_rab::rab$w_usz = buffer_size
	bio_rab::rab$l_ubf = buffer_address

	bio_read = sys$read (bio_rab by ref)

  	end function
