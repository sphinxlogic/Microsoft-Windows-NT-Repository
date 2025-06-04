	.TITLE	MGBOOK_MSG
	.IDENT	/V1.0/
!++
! FACILITY:	MGBOOK
!
! MODULE DESCRIPTION:
!
!	This module contains condition code definitions for codes specific
!	to MGBOOK.
!
! AUTHOR:		Darrell Burkhead
!			Copyright © 1995, MadGoat Software.
!			ALL RIGHTS RESERVED.
!
! CREATION DATE:	February 9, 1995
!
! MODIFICATION HISTORY:
!
!	V1.0		Darrell Burkhead	 9-FEB-1995 16:14
!		Original version.
!--
	.FACILITY	MGBOOK,1/PREFIX=MGBOOK__

	.SEVERITY	INFORMATIONAL
SHELFOPEN	<Opened private shelf !AS for writing>/FAO_COUNT=1
SHELFAPP	<Appending to private shelf !AS>/FAO_COUNT=1
SHELFCLOSE	<Closed private shelf file>/FAO_COUNT=1
ADDENTRY	<Added a !AS entry called !AS to the private shelf>/FAO_COUNT=2
RESTRICTON	<Width-change restrictions are now on>
RESTRICTOFF	<Width-change restrictions are now off>

	.SEVERITY	WARNING
WRITEERR	<Error writing bookshelf entry information>
EMPTYSHELF	<Shelf file contains no BOOK or SHELF entries>

FONTERR		<Error parsing font information for font !AS>/FAO_COUNT=1

DBGERR		<Error writing debugging information>

HELPERR		<Error displaying help text>

BRDERR		<Error displaying broadcast message text>

STSERR		<Error displaying status text>

	.SEVERITY	ERROR
OPENIN		<Error opening !AS>/FAO=1

BADENTRY	<Error parsing bookshelf entry !AS>/FAO_COUNT=1
SHELFERR	<Error saving bookshelf entry information>
MENUERR		<Error handling bookshelf menu>
READERR		<Error reading bookshelf entry information>
UNKTYPE		<Unknown bookshelf entry type>

BOOKERR		<Error opening book !AS>/FAO_COUNT=1
BADRECTYP	<Found record type !UL when expecting !UL>/FAO_COUNT=2
BADSIZE		<Expected record to have size !UL, actual size is !UL>/FAO_COUNT=2
FIRSTREC	<Error reading the header record>
LASTREC		<Error reading the page table record>
BADPART		<Part !UL, RFA !XL,!XW has invalid size !UL>/FAO_COUNT=4
UNKRECTYP	<Read record with an unknown type of !UL>/FAO_COUNT=1
PARTERR		<Error reading book part !UL>/FAO_COUNT=1
SUBMENERR	<Error handling the !AS submenu>/FAO_COUNT=1
HSERR		<Error saving hotspot information>
MAINMENERR	<Error handling the main menu for book !AS>/FAO_COUNT=1
SECTERR		<Error displaying section !UL>/FAO_COUNT=1
SHOWERR		<Error displaying part !UL>/FAO_COUNT=1

SPAWNERR	<Error spawning a subprocess>

	.SEVERITY   FATAL
UNDEFTERM	<Unknown or invalid terminal type>
SHELFACC	<Invalid shelf file access; internal consistency error>
UNKDISP		<Unknown display; internal consistency error>
	.END	!MGBOOK_MSG
