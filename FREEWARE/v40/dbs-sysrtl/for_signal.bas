	%title	"FOR_SIGNAL Dummy routine"
	%ident	"V1-002"

	sub for_signal

!+
! Version:	V1-002
!
! Facility:	Library routines.
!
! Abstract:	Check for any fortran run-time library errors and signal them.
!
! Environment:	User mode.
!
! History:
!
!	11-Apr-1990, DBS, Version V1-001
! 001 -	Original version.
!	28-Dec-1995, DBS; Version V1-002
! 002 -	Converted from FORTRAN to BASIC and made into a dummy routine.
!-

!++
! Functional Description:
!	This routine uses the ERRSNS routine of Fortran to check for any errors
!	and if one is found, a vms error is signalled.  Continuation of the
!	calling routine depends on the severity of the error.
!
! Calling Sequence:
!		call for_signal
!
! Formal Argument(s):
!	None
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

	end sub
