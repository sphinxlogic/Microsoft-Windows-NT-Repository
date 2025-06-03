.FACILITY	NSQUERY,1049 /PREFIX=NSQ__
.IDENT	    	"V1.2-1"
!++
! NSQ_MSG.MSG
!
! Message codes for NSQUERY program.
!
! AUTHOR:   M. Madison
!   	    COPYRIGHT © 1992, MATTHEW D. MADISON.
!   	    ALL RIGHTS RESERVED.
!
! MODIFICATION HISTORY:
!
!   09-JAN-1989	V1.0-001    Madison 	Initial coding.
!   30-JUN-1989	V1.1-002    Madison	Add buffer allocation messages.
!   04-JUN-1990	V1.1-003    Madison	Make HDRERR severity E.
!   22-NOV-1991	V1.1-004    Madison	add NORECURSION
!   28-SEP-1992	V1.2	    Madison 	Add SRVNAMERR.
!   11-OCT-1993	V1.2-1	    Madison 	Change TRYNS.
!--
.SEVERITY   	INFORMATION
TRYNS	    	<trying address !AS...>/FAO=1
IDENTMISMATCH	<sent message id !UW does not match returned id !UW>/FAO=2
MSGTRUNC    	<message was truncated>
AUTHRESP    	<authoritative response>
QUERYOK	    	<query operation completed successfully>
FMTERR	    	<format error in query>
SRVFAIL	    	<server returned failure status>
RCVQUERY    	<received a query as a response???>
NAMERR	    	<name error>
NOTIMPL	    	<query not implemented by server>
REFUSED	    	<operation refused by server>
UNKRC	    	<unknown query return code: !UL>/FAO=1
SMALLBYTLM  	<BYTLM of !UL is small - !UL would work better>/FAO=2
SMALLMAXBUF 	<your system's MAXBUF parameter is small (!UL) - !UL would work better>/FAO=2
NORECURSION 	<server does not support recursive queries>
SRVNAMERR   	<could not get address list for server !AZ>/FAO=1

.SEVERITY   	ERROR
NSOPNERR    	<error opening connection to name server>
NOCONTACT   	<name server could not be contacted>
NOALLOC	    	<LIB$GET_VM failed allocating a !UL byte buffer>/FAO=1
HDRERR	    	<error in response message header>

.END
