	program	get_cluster_membership

!+
! Version:	V1-003
!
! Facility:	General Utilities.
!
! Abstract:	This program will define a logical (CLUSTER_MEMBERSHIP) in the
!		process table which will contain a list of the nodes currently
!		in the cluster.  If this node is not in a cluster then the
!		logical will contain text describing this situation.
!
! Environment:	VMS V5.0 and later.
!
! History:
!
!	13-Sep-1989, DBS; Version V1-001
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

	external long function	clu_membership
	external long function	lib$set_logical
	external long function	lib$signal
	external long function	str_len
	external long function	str_subtract
	external long function	sys$getsyiw

! Type declarations for variables

	declare string constant	comma = ","
	declare string constant	logical_name = "CLUSTER_MEMBERSHIP"
	declare long		lnm_status
	declare string		node_list
	declare long		node_list_len
	declare string constant	table_name = "LNM$PROCESS_TABLE"

!+
! Mainline
!-

	node_list = space$(256)

	call clu_membership (node_list)

	node_list_len = str_len (node_list)

	if (node_list_len = 0) then
	    node_list = '%this node is not in a cluster'
	    node_list_len = str_len (node_list)
	end if

	lnm_status = lib$set_logical (logical_name by desc		  &
				,left(node_list,node_list_len) by desc	  &
				,table_name by desc)

	if ((lnm_status and 1) = 0) then
	    call lib$signal (lnm_status by value)
	end if

  	end program
