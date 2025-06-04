	%title	"Block mode I/O - Close"
	%ident	"V1-002"

	function long bio_close (long bio_lun)

!+
! Version:	V1-002
!
! Facility:	Library routines - Block mode I/O.
!
! Abstract:	Close a file opened for block mode i/o.
!
! Environment:	User mode.
!
! History:
!
!	09-Dec-1988, DBS, Version V1-001
! 001 -	Original version.
!	27-Dec-1995, DBS; Version V1-002
! 002 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	This routine simply closes the file opened by BIO_OPEN and is
!	probably unnecessary but by having it there is some sort of
!	consistency about the code. (?)
!
! Calling Sequence:
!	status = bio_close (lun)
!		-or-
!	pushal	lun
!	calls	#1, g^bio_close
!
! Formal Argument(s):
!	bio_lun.rl.r	Address of a longword containing the logical unit
!			number of the file.
!
! Implicit Inputs:
!	None
!
! Implicit Outputs:
!	None
!
! Completion Codes:
!	Always true.
!
! Side Effects:
!	None
!--

	option	type = explicit,					  &
		constant type = integer,				  &
		size = (integer long, real double)

! External references (and includes)

%include "$SSDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"

!+
! Mainline
!-
mainline:

	close #bio_lun

	bio_close = ss$_normal

  	end function
