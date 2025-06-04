	%title	"Wildcard string matching"
	%ident	"V1-002"

 	function long str_match (string candidate, string pattern)

!+
! Version:	V1-002
!
! Facility:	Library routines.
!
! Abstract:	Compare two strings for a wildcard or generic match.
!
! Environment:	User mode.
!
! History:
!
!	11-Nov-1988, DBS; Version V1-001
! 001 -	Original version.
!	18-Dec-1995, DBS; Version V1-002
! 002 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	This routine will attempt to match two strings using the system
!	routine str$match_wild, and then trying to do a generic match.
!
! Calling Sequence:
!	status = str_match (candidate, pattern) or
!	if (str_match (candidate, pattern)) then...
!
! Formal Argument(s):
!	candidate.rt.ds	This is the string that you hope matches the
!			pattern string.
!	pattern.rt.ds	This is the valid pattern, eg. a wildcard.
!
! Implicit Inputs:
!	None
!
! Implicit Outputs:
!	None
!
! Completion Codes:
!	The same as returned by str$match_wild or'd with the equality (or
!	otherwise) of candidate and pattern (see below...)
!
! Side Effects:
!	None
!--


	option	type = explicit,					  &
		constant type = integer,				  &
		size = (integer long, real double)

! External references (and includes)

	external long function	str$match_wild
	external long function	str_len

! Type declarations for variables

	declare long	candidate_len
	declare long	pattern_len

!+
! Mainline
!-
mainline:

	candidate_len = str_len(candidate)
	pattern_len   = str_len(pattern)

	str_match = (str$match_wild (left(candidate, candidate_len)	  &
	      	 			,left(pattern, pattern_len))	  &
			or (left(candidate, pattern_len) = pattern))

	end function
