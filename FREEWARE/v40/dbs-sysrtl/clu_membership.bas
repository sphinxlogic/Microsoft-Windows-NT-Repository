	%title	"Get current cluster membership"
	%ident	"V1-003"

	sub clu_membership (string node_list)

!+
! Version:	V1-003
!
! Facility:	Library routines - cluster stuff.
!
! Abstract:	This routine will return a list of all nodes currently in the
!		cluster.
!
! Environment:	User mode.
!
! History:
!
!	13-Sep-1989, DBS, Version V1-001
! 001 -	Original version.
!	22-Aug-1990, DBS; Version V1-002
! 002 -	Modified to set syi_wildcard at run time.  There was a problem when
!	this routine was called from macro repeatedly.  Also clear the list
!	before we store our stuff.
!	18-Dec-1995, DBS; Version V1-003
! 003 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	This routine uses the sys$getsyiw() system service to do a wildcard
!	call and get the names of all the nodes that are currently active in
!	the cluster.  If this node is not a cluster member, the string
!	returned is empty.
!
! Calling Sequence:
!	call clu_membership (node_list)
!		-or-
!	pushaq	node_list
!	calls	#1, g^clu_membership
!
! Formal Argument(s):
!	node_list.wt.ds  Address of a string descriptor indicating where the
!			returned list can be written.
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


	option	type = explicit,					  &
		constant type = integer,				  &
		size = (integer long, real double)

! External references (and includes)

%include "$SSDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"
%include "$SYIDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"

	external long function	lib$analyze_sdesc
	external long function	str_len
	external long function	str_subtract
	external long function	sys$getsyiw


! Structures

record itmlst_3
	word	buffer_length
	word	item_code
	long	buffer_address
	long	return_length_address
end record itmlst_3

! Type declarations for constants

	declare string constant	comma = ","
	declare long constant	nodename_len = 15

! Type declarations for variables

	declare long	node_list_len
	declare long	string_address
	declare word	string_size
	declare word	syi_s_nodename
	declare string	syi_t_nodename
	declare long	syi_status
	declare long	syi_wildcard
	declare itmlst_3 syi_itmlst(1 to 2)

!+
! Mainline
!-
mainline:

	syi_t_nodename = space$(nodename_len)
	node_list = sp
	syi_wildcard = -1

	call lib$analyze_sdesc (syi_t_nodename by desc			  &
				,string_size by ref			  &
				,string_address by ref)

	syi_itmlst(1)::buffer_length		= nodename_len
	syi_itmlst(1)::item_code		= syi$_nodename
	syi_itmlst(1)::buffer_address		= string_address
	syi_itmlst(1)::return_length_address	= loc(syi_s_nodename)
	syi_itmlst(2)::buffer_length		= 0
	syi_itmlst(2)::item_code		= 0

	syi_status = sys$getsyiw (,syi_wildcard by ref			  &
					,,syi_itmlst() by ref,,,)

	while (syi_status <> ss$_nomorenode)
		node_list_len = str_len (node_list)
		node_list = left(node_list, node_list_len)		  &
				+ comma					  &
				+ left(syi_t_nodename, syi_s_nodename)

		syi_status = sys$getsyiw (,syi_wildcard by ref		  &
						,,syi_itmlst() by ref,,,)
	next

	call str_subtract (node_list, node_list, comma)

  	end sub
