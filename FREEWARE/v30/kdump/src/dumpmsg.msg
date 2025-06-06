!	DUMPMSG.MSG - DUMP Messages
!
! Version:	'V04-000'
!
!****************************************************************************
!*									    *
!*  COPYRIGHT (c) 1978, 1980, 1982, 1984, 1986 BY			    *
!*  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.		    *
!*  ALL RIGHTS RESERVED.						    *
!* 									    *
!*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED   *
!*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE   *
!*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER   *
!*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY   *
!*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY   *
!*  TRANSFERRED.							    *
!* 									    *
!*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE   *
!*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT   *
!*  CORPORATION.							    *
!* 									    *
!*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS   *
!*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.		    *
!* 									    *
!*									    *
!****************************************************************************
!

!++
!
! FACILITY:  File dump utility
!
! ABSTRACT:
!	This module contains the messages for DUMP.
!
! ENVIRONMENT:
!
!	VAX native, user mode.
!
!--
!
!
! AUTHOR:  Benn Schreiber, Stephen Zalewski	 CREATION DATE:  22-Jun-1981
!
! MODIFIED BY:
!
!	V03-001	LMP0030		L. Mark Pilant,		16-Jun-1982  10:00
!		Add a fatal error message for when the user tries to dump
!		a device starting beyond the maximum block number
!
!	V02-004	SHZ0001		Stephen H. Zalewski,	28-Sep-1981  15:27
!		Change amount of space used to print the total number of
!		bytes in a BLOCK/RECORD from longword to word.
!
!	V02-003	MLJ0045		Martin L. Jack,	12-Sep-1981  17:24
!		Correct wording of DEVQUALS message.
!
!	V02-002	MLJ0038		Martin L. Jack,	2-Sep-1981  17:05
!		Adjust facility code.
!
!	V02-001	MLJ0033		Martin L. Jack,	23-Aug-1981  9:49
!		Extensive rewriting to finish implementation.
!--

.facility DUMP,205/system
!
! The informational messages are not signalled.  They are only used as the
! object of $GETMSG as text for the DUMP output.
!
.severity informational
BN		<Block number !UL (!-!XL), !UL (!-!XW) byte!%S>/FAO=2
DUMPODEV	<Dump of device !AS on !%D>/FAO=2
DUMPOFIL	<Dump of file !AS on !%D>/FAO=2
ENDOFFILE	<	***  End of file  ***>
FILDNT		<File ID (!UL,!UL,!UL)   End of file block !UL / Allocated !UL>/FAO=5
HEADER		<!29* File Header>
LBN		<Logical block number !UL (!-!XL), !UL (!-!XW) byte!%S>/FAO=2
RECNO		<Record number !UL (!-!XL), !UL (!-!XW) byte!%S>/FAO=2
VBN		<Virtual block number !UL (!-!XL), !UL (!-!XW) byte!%S>/FAO=2


.severity error
READHEADER	<error reading file header for !AS>/FAO=1
.severity severe
BADRANGE	<inconsistent START and END specifications>
CONFQUAL	<conflicting qualifiers>
DEVQUALS	<invalid qualifier for device type>
DEVSPEC		<input specification for non-file-structured dump must contain only device>
GETCHN		<error getting device characteristics>
NOVIRMEM	<error allocating virtual memory>
BADSTART	<starting block beyond maximum legal block !UL>/FAO=1
.end
