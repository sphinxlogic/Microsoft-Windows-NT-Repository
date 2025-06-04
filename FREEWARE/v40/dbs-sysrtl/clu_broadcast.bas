	%title "Broadcast a message to selected terminals on a cluster"
	%ident	"V1-002"

	sub clu_broadcast (string identification			  &
				,string text				  &
				,string destination)

!+
! Version:	V1-002
!
! Facility:	General system routines.
!
! Abstract:	Broadcast a formatted message to selected terminals on
!		a cluster.
!
! Environment:	As required for SYS$BRKTHRU.
!
! History:
!
!	28-Sep-1990, DBS; Version V1-001
! 001 -	Original version.
!	18-Dec-1995, DBS; Version V1-002
! 002 -	Converted from FORTRAN to BASIC.
!-

!++
! Functional Description:
!	The information supplied is formatted as shown below then broadcast
!	the the terminal(s) listed.  Note that multi-line text messages must
!	be formatted by the caller.
!	
!	##########  <ident>  <current_date_time>  #########
!	<text>
!
! Calling Sequence:
!	call sys_broadcast (prog_id, message, term_list)
!		-or-
!	pushaq	term_list
!	pushaq	message
!	pushaq	prog_id
!	calls	#3, g^sys_broadcast
!
! Formal Argument(s):
!	identification.rt.ds  Address of a string descriptor pointing to
!			the text to be used in the identification line of
!			for formatted message.
!	text.rt.ds	Address of a string descriptor pointing to the actual
!			text of the message.  This must be formatted by the
!			caller.
!	destination.rt.ds  Address of a descriptor pointing to string
!			containing the terminal (or list of terminals) to
!			receive the broadcast.
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

%include "$BRKDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"
%include "$STRDEF" %from %library "SYS$LIBRARY:BASIC$STARLET.TLB"

	external long function	lib$date_time
	external long function	str_len
	external long function	str$element
	external long function	sys$brkthru

! Type declarations for constants

	declare string constant	default_destination = "OPA0:"
	declare string constant	default_identification = "Unidentified Message"
	declare string constant	delimiter = ","

! Type declarations for variables

	map (SYSBRDCSTSTUFF)	string	broadcast_dest = 256		  &
				,string	broadcast_ident = 32		  &
				,string	date_time = 23			  &
				,string	message_text = 2048		  &
				,string	tterminal = 32

	declare long	destination_len
	declare long	element
	declare long	ident_len
	declare long	message_len
	declare long	tterminal_len
	declare long	text_len
	declare long	sstatus

	%sbttl	"Mainline"
!+
! Mainline
!-
mainline:

	call lib$date_time (date_time)

	ident_len = str_len (identification)

	if (ident_len = 0) then
		broadcast_ident = default_identification
	else
	broadcast_ident = identification
	end if

	destination_len = str_len (destination)

	if (destination_len = 0) then
		broadcast_dest = default_destination
	else
	broadcast_dest = destination
	end if

	ident_len = str_len (broadcast_ident)
	text_len = str_len (text)
	destination_len = str_len (broadcast_dest)

	message_text = cr + lf + "##########  "				  &
			+ left(broadcast_ident, ident_len)		  &
			+ "  " + date_time + "  ##########"		  &
			+ cr + lf + left(text, text_len) + cr + lf
	message_len = str_len (message_text)

	element = 0

	sstatus = str$element(tterminal, element, delimiter, broadcast_dest)

	while (sstatus <> str$_noelem)
		tterminal_len = str_len (tterminal)

		call sys$brkthru (,left(message_text, message_len)	  &
				,left(tterminal, tterminal_len)		  &
				,brk$c_device by value,,		  &
				,brk$m_cluster by value,,,,,)

		element = element + 1
		sstatus  = str$element(tterminal, element		  &
					,delimiter, broadcast_dest)
	next

	end sub
