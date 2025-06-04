!=======================================================================
! SHOW_LOAD_AVERAGE version 1.0, Curtis Rempel
! 
! File: SHOW_LOAD_AVERAGE.MSG
!
! This file contains the text of the error messages for SHOW_LOAD_AVERAGE.
!=======================================================================

.title	SHOW_LOAD_AVERAGE error messages
.ident	'Version 1.0'

.facility show, 1 /prefix=SHOW_

.severity INFORMATIONAL
!-----------------------------------------------------------------------

.severity WARNING
!-----------------------------------------------------------------------

.severity ERROR
!-----------------------------------------------------------------------
NOCLUSTER	</CLUSTER is not a supported qualifier.>/FAO=0
NONODE		</NODE is not a supported qualifier.>/FAO=0
NOTAVAIL	<Load average information not available.>/FAO=0
NOTAUTH		<You are not authorized to use /HOGS.>/FAO=0

.severity FATAL
!-----------------------------------------------------------------------
NODRIVER	<LAVDRIVER is not loaded>/FAO=0

.end
