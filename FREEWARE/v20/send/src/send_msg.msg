	.TITLE	SEND_MSG
	.IDENT	/V1.0/
!++
!
!  File:	SEND_MSG.MSG
!  Author:	Darrell Burkhead
!		COPYRIGHT © 1994, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
!  Date:	February 28, 1994
!
!  Description:
!
!	Message file for account creation programs.
!
!  Modification history:
!
!	V1.0		Darrell Burkhead	28-FEB-1994 12:18
!		Creation.
!--
	.FACILITY	SEND,1/PREFIX=SEND_

	.SEVERITY	SUCCESS
SENT		<!AS !UW terminal!%S received, !UW timed out, !UW not receiving>-
		/FAO_COUNT=4
SENTTRM		<Terminal !AS received the message>

	.SEVERITY	INFORMATIONAL
NEWPROMPT	<First destination deleted; generate a new prompt>
NODESTS		<Out of destinations>

	.SEVERITY	WARNING
INVWAIT		<Invalid value on /WAIT -- value set to 5>

	.SEVERITY	ERROR
MSGERR		<Error sending a message to !AS>/FAO_COUNT=1
CONNERR		<Error connecting to node !AS>/FAO_COUNT=1
NODERR		<Error sending a message to node !AS>/FAO_COUNT=1
AMBIGTERM	<Terminal !AS appears on multiple remote nodes>/FAO_COUNT=1
NOTRCVING	<!AS is not receiving messages>/FAO_COUNT=1
TIMEOUT		<Timed out sending a message to !AS>/FAO_COUNT=1
NOTLOGIN	<User !AS is not logged in>/FAO_COUNT=1

	.SEVERITY	FATAL
NOBITNET	<BITNET addresses are not currently handled>
TOOMANY		<Too many destinations>
NOBRDCLASS	<Broadcast class not defined; contact your system manager>
