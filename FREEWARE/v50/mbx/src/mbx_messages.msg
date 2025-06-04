!
! Message and status codes used in the MBX utility
!
! © Marc Van Dyck, 01-JUN-1999
!
	.TITLE		MBX_MESSAGES MBX Run-time messages
	.IDENT		"MBX V1.2"
	.FACILITY	MBX,	1001	/PREFIX = MBX_
!
! Success messages, displayed only if explicitely requested by the /LOG qualifier.
! Exit status in DCL is SS$_NORMAL.
!
	.SEVERITY	SUCCESS
	.BASE		10
	MBXCRE		<Mailbox !AS created with room for !UL message!%S of !UL byte!%S> /FAO = 3
	MBXDEL		<Mailbox !AS deleted> /FAO = 1
	MBXATT		<Channel !UL attached to mailbox !AS> /FAO = 2
	MBXDET		<Channel !UL detached from mailbox !AS> /FAO = 2
	MSGSNT		<!UL byte!%S sent to !AS> /FAO = 2
	MSGRCV		<!UL byte!%S received from !AS> /FAO = 2
	EOFSNT		<EOF marker sent to !AS> /FAO = 1
!
! Information messages, displayed only if explicitely requested by the /LOG qualifier.
! They are informational because they inform the user about an 'alternate' situation.
! Exit status in DCL is still SS$_NORMAL. Note that the two first message are only intermediate.
!
	.SEVERITY	INFORMATIONAL
	.BASE		20
	MSGRDWAIT	<Waiting for read operation on mailbox !AS> /FAO = 1
	MSGWRWAIT	<Waiting for write operation on mailbox !AS> /FAO = 1
	MBXALRATT	<Mailbox !AS already attached with channel !UL> /FAO = 2
	MBXMKDEL	<Mailbox !AS marked for deletion> /FAO = 1
!
! The operation requested has been done, but an unxepected result occured.
! Exit status from DCL is SS$_ENDOFFILE.
!
	.SEVERITY	WARNING
	.BASE		30
	EOFRCV		<EOF marker received from !AS> /FAO = 1
!
! The operation requested has been done, but did not conclude correctly
! Exit severity from DCL is ERROR (2)
!
	.SEVERITY	ERROR
	.BASE		40
	TRCMSGSNT	<Message truncated, !UL byte!%S out of !UL sent to !AS> /FAO = 3
	TRCMSGRCV	<Message truncated, !UL byte!%S out of !UL received from !AS> /FAO = 3
	TIMMSGSNT	<Time-out expired while waiting for a reader on mailbox !AS> /FAO = 1
	TIMMSGRCV	<Time-out expired while waiting for a message on mailbox !AS> /FAO = 1
!
! All those situations cause the requested opertion to fail miserably.
! Exit severity from DCL is FATAL (4)
!
	.SEVERITY	FATAL
	.BASE		50
	MBXINTERR	<Internal error detected - please submit an SPR>
	DEVNOTMBX	<Device specified is not a mailbox>
	MBXINVPRT	<Invalid mailbox protection specification :!/ \!AS\>
	MBXINVTIM	<Invalid mailbox time-out specification :!/ \!AS\>
	MSGINVCNT	<Invalid message count:!/ \!AS\> /FAO = 1
	MSGINVSIZ	<Invalid message size :!/ \!AS\> /FAO = 1
	MBXTOOBIG	<Resultant mailbox size too large : !UL (!UL * !UL)> /FAO = 3
	NAMALRXTS	<Logical name already exists>
	NOCHANATT	<Mailbox has no channel attached>
!
! Those six messages are normally followed by a more detailed explanation
! Exit severity from DCL is FATAL (4)
!
	MBXNOTCRE	<Cannot create mailbox !AS> /FAO = 1
	MBXNOTDEL	<Cannot delete mailbox !AS> /FAO = 1
	MBXNOTATT	<Cannot attach to mailbox !AS> /FAO = 1
	MBXNOTDET	<Cannot detach from mailbox !AS> /FAO = 1
	MSGNOTSNT	<Cannot send message to mailbox !AS> /FAO = 1
	MSGNOTRCV	<Cannot receive message from mailbox !AS> /FAO = 1

	.END
