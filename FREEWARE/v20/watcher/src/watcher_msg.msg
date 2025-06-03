.TITLE	    WATCHER_MSG
.FACILITY   WATCHER,1075/PREFIX=WATCHER__
.IDENT	    "V1.3"
!+
! DESCRIPTION:
!
!   This module contains message codes used by WATCHER.
!
! AUTHOR:
!
!   M. Madison
!   COPYRIGHT © 1993, 1994  MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
!
! UPDATE HISTORY:
!
!   19-MAR-1990	V1.0	Madison	    Initial coding.
!   02-NOV-1990	V1.1	Madison	    Change LOGOUT, DISCONNECT (Ward@Harris)
!   22-MAR-1991	V1.2	Madison	    Add multiple-warnings support. (Kimura@HAC)
!   29-MAR-1991	V1.2-1	Madison	    Fix DISCWARN's FAO_COUNT. (Kimura@HAC)
!   03-FEB-1993	V1.3	Madison	    Add FORCEX-related stuff.
!-
.SEVERITY   	FATAL
NOCFG	    	<no configuration file found>

.SEVERITY   	WARNING
INACTIVE    	<this terminal has been inactive since !8%T (!8%T)>/FAO=2

.SEVERITY   	INFORMATION
DISCWARN    	<disconnection will occur in !8%T at !8%T>/FAO=2
LOGWARN	    	<logout will occur in !8%T at !8%T>/FAO=2
LOGOUT	    	<logging out process due to inactivity at !8%T>/FAO=1
DISCONNECT  	<disconnecting terminal due to inactivity at !8%T>/FAO=1
LOGGEDOUT   	<WATCHER logged out user !AD, pid !XL, term !AD (!AD), last change !%D>/fao=8
DISCONNECTED	<WATCHER disconnected user !AD, pid !XL, term !AD (!AD), last change !%D>/fao=8
FORCEXWARN  	<forced image exit will occur in !8%T at !8%T>/FAO=2
FORCEX	    	<forcing image exit due to inactivity at !8%T>/FAO=1
FORCEDEXIT    	<WATCHER forced image exit of user !AD, pid !XL, term !AD (!AD), last change !%D>/fao=8
