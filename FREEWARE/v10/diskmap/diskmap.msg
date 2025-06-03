     	.title diskmsg
	.ident "V2.2"

!++
! Copyright (C) 1986, 1988 by
! Digital Equipment Corporation, Maynard, Mass.
!
! ***DEC INTERNAL USE ONLY***
!
! Facility:	BITMAP - disk bitmap display tool
!
! Abstract:	Messages
!
! Environment:	VAX/VMS, UIS, user mode
!
! Author:	Dave Porter, 22-Nov-1986
!
! V2.0	18-Nov-1988
!	DECwindows version
!
! V2.1	30-Nov-1988
!	Change name to "DISKMAP" to avoid conflict
!	with another "BITMAP" program
!
! V2.2	19-Jun-1989
!	Add CACHE message.
!
!--

	.facility diskmap,2047/prefix=dm$_	! private facility

	.severity info
SIZE	<!AS !SL block!%S, !SL free, cluster !SL>		/fao=4
SHAPE	<display is !SL bit!%S wide by !SL bit!%S high>		/fao=2
COLOUR	<white bits are allocated, black bits are free>		/fao=0

	.severity warning
GOTLOST	<event parameters are not as expected>			/fao=0
CACHE	<extent cache not flushed (no write access to bitmap)>	/fao=0

	.severity error
OPEN	<error opening bitmap file for !AS>			/fao=1
PARSE	<error parsing device name !AS>				/fao=1

	.severity fatal
MAP	<error mapping bitmap for !AS >	 			/fao=1
OFLO	<bitmap seems to overflow bitmap file>			/fao=0
XOPEN	<unable to open display>                                /fao=0
BIGEND	<no support for big-endian servers>			/fao=0
NALIGN	<bitmap alignment assumptions don't hold>		/fao=0
GACK	<can't fit on !SLx!SL screen [cheap programming]>	/fao=2

 	.end
