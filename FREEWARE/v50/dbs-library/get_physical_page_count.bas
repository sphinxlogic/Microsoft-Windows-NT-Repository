	program	get_physical_page_count

!+
! Version:	V1-003
!
! Facility:	General Utilities.
!
! Abstract:	This program will define a logical (PHYSICAL_PAGE_COUNT) in
!		the process table which will contain the number of pages of
!		physical memory found on the system.
!
! Environment:	VMS V5.0 and later.
!
! History:
!
!	16-Aug-1990, DBS; Version V1-001
! 001 -	Original version.
!	10-Dec-1990, DBS; Version V1-002
! 002 -	Modified to use lib$set_logical.
!	05-Feb-1996, DBS; Version V1-003
! 003 -	Converted from FORTRAN to BASIC.
!-

	option	type = explicit,					  &
		constant type = integer,				  &
		size = (integer long, real double)

! External references

	external long function	lib$set_logical
	external long function	lib$signal
	external long function	sys$fao
	external long function	sys_memsize

! Type declarations for parameters and PARAMETER statements

	declare string constant	result_fao = "!UL"

! Type declarations for variables

	declare string constant	logical_name = "PHYSICAL_PAGE_COUNT"
	declare long		lnm_status
	declare long		page_count
	declare string		page_count_text
	declare long		page_count_len
	declare string constant	table_name = "LNM$PROCESS_TABLE"

!+
! Mainline
!-

	page_count_text = space$(16)
	page_count = sys_memsize

	call sys$fao (result_fao by desc				  &
			,page_count_len by ref				  &
			,page_count_text by desc			  &
			,page_count by value)

	lnm_status = lib$set_logical (logical_name by desc		  &
			,left(page_count_text,page_count_len) by desc	  &
			,table_name by desc)

	if ((lnm_status and 1) = 0) then
	    call lib$signal (lnm_status by value)
	end if

  	end
