.TITLE		MMK_MSG
.FACILITY	MMK,1044/PREFIX=MMK__
.IDENT		"V1.6"
!++
!
! DESCRIPTION:
!
!   These are the message codes used by the MMS utility.
!
! AUTHOR:   	    M. Madison
!   	    	    COPYRIGHT © 1992-1994, MADGOAT SOFTWARE.
!   	    	    ALL RIGHTS RESERVED.
!
! CREATION DATE: 20-AUG-1992
!
! MODIFICATION HISTORY:
!
!   20-AUG-1992	V1.0	Madison	    Initial coding.
!   29-SEP-1992	V1.1	Madison	    Add IDENT, COPYRIGHT, OPENOUT.
!   04-JUN-1993	V1.2	Madison	    Add include-file messages.
!   17-OCT-1993	V1.2-1	Madison	    Change IDENT message.
!   12-DEC-1993	V1.3	Madison	    Add AMBDEPEND.
!   12-JUL-1994	V1.4	Madison	    Add CONDSKIP, ACTNOUPD, CIRCDEP.
!   02-DEC-1994	V1.5	Madison	    Add ERRLOC.
!   28-DEC-1994	V1.6	Madison	    Add MULACTION.
!--

.SEVERITY   	FATAL
NOOPNDSC    	<could not open description file !AZ>/FAO=1
PARSERR	    	<error parsing description line !AD>/FAO=2
PRSTBLERR   	<fatal error in parse table>
NOALLOC	    	<could not allocate memory for structure type !AZ>/FAO=1
NOTARGETS   	<no target found>
NOSUCHTRG   	<no such target !AZ>/FAO=1
ERRUPD	    	<error status %X!XL occurred when updating target !AZ>/FAO=2
SRCERR	    	<error locating source !AZ>/FAO=1
NOACTION    	<actions to build !AZ unknown>/FAO=1
OPENOUT	    	<error opening output file !AZ>/FAO=1
CANTUPD	    	<cannot update target !AD - sources unknown>/FAO=2
SDCMIX	    	<single/double-colon dependency mix found for target !AZ>/FAO=1
MULACTION   	<multiple dependencies with action lists found for target !AD>/FAO=2

.SEVERITY   	ERROR
ELSENOIF    	<.ELSE directive encountered without corresponding .IFDEF>
ENDIFNOIF   	<.ENDIF directive encountered without corresponding .IFDEF>

.SEVERITY   	WARNING
NOOPNRUL    	<could not open rules file !AZ>/FAO=1
NOOPNINCL   	<could not open .INCLUDE file !AZ>/FAO=1
AMBDEPEND   	<ambiguous multiple dependencies for target !AZ>/FAO=1
CONDSKIP    	<description line skipped due to conditional>
CIRCDEP	    	<circular dependency found for target !AZ>/FAO=1
CONNEXION   	<target !AZ found in circular dependency>/FAO=1

.SEVERITY   	INFORMATION
OPENDESC    	<reading description file !AZ>/FAO=1
OPENRULE    	<reading rules file !AZ>/FAO=1
OPENINCL    	<including file !AZ>/FAO=1
CHECKUPD    	<checking need to update target !AZ>/FAO=1
TRGLKUF   	<lookup for target !AZ failed - update required>/FAO=1
TRGNUPD	    	<target !AZ is out of date - update required>/FAO=1
TRGRDT	    	<target !AZ revision date is !%D>/FAO=2
SRCRDT	    	<source !AZ revision date is !%D>/FAO=2
PERFUPD	    	<performing actions to update !AZ>/FAO=1
IDENT	    	<this is Matt's Make Utility !AZ>/FAO=1
COPYRIGHT   	<!AZ>/FAO=1
ACTNOUPD    	<action did not update target !AZ>/FAO=1
ERRLOC	    	<at line number !UL in file !AD>/FAO=3

.SEVERITY   	SUCCESS
NOUPDATE    	<all targets are up-to-date>
.END
