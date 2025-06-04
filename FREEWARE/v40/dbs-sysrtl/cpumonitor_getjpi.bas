	%title	"CPU Monitor call to $GETJPIW"
	%ident	"X1-001"

	sub cpumonitor_getjpi

!+
! Version:	X1-001
!
! Facility:	CPU Monitor
!
! Abstract:	Call $GETJPIW to get the relevant details.
!
! Environment:	User mode.
!
! History:
!
!	26-May-1995, DBS; Version X1-001
! 001 -	Original version.
!-

!++
! Functional Description:
!	This routine should only be called from other CPUMONITOR routines.
!	It is NOT a general purpose routine.
!	This routine calls $GETJPIW to get the information we need.  First,
!	the "current" details are moved to "previous",  then the "difference"
!	is calculated.  It's all done here so that we have a consistent set
!	of figures regardless of where we get called from.
!
! Calling Sequence:
!	call cpumonitor_getjpi
!
! Formal Argument(s):
!	None
!
! Implicit Inputs:
!	CPUMONITOR map.
!
! Implicit Outputs:
!	CPUMONITOR map.
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

%include "CPUMONITORDEF" %from %library "OLGA$TEXT"

mainline:

! save last lot of details

	cpumon_previous = cpumon_current

! get the current details

	call sys$gettim (cpumon_current::clocktime by ref)
	call lib$sub_times (cpumon_current::clocktime by ref,		  &
				cpumon_logintim by ref,			  &
				cpumon_current::clocktime by ref)

	jpi_itmlst(0)::buflen = 4
	jpi_itmlst(0)::itmcod = jpi$_cputim
	jpi_itmlst(0)::bufadr = loc(cpumon_current::cputim)
	jpi_itmlst(0)::retlen = 0
	jpi_itmlst(1)::buflen = 4
	jpi_itmlst(1)::itmcod = jpi$_bufio
	jpi_itmlst(1)::bufadr = loc(cpumon_current::bufio)
	jpi_itmlst(1)::retlen = 0
	jpi_itmlst(2)::buflen = 4
	jpi_itmlst(2)::itmcod = jpi$_dirio
	jpi_itmlst(2)::bufadr = loc(cpumon_current::dirio)
	jpi_itmlst(2)::retlen = 0
	jpi_itmlst(3)::buflen = 4
	jpi_itmlst(3)::itmcod = jpi$_pageflts
	jpi_itmlst(3)::bufadr = loc(cpumon_current::pageflts)
	jpi_itmlst(3)::retlen = 0
	jpi_itmlst(4)::buflen = 0	! end of list
	jpi_itmlst(4)::itmcod = 0

	call sys$getjpiw (, , , jpi_itmlst(0) by ref, jpi_iosb, , )

	call lib$ediv (100000 by ref,					  &
			cpumon_current::clocktime by ref,		  &
			cpumon_current::clockticks by ref,		  &
			remainder by ref)
	cpumon_current::clockticks = abs%(cpumon_current::clockticks)

! calculate the differences

	call lib$sub_times (cpumon_current::clocktime by ref,		  &
				cpumon_previous::clocktime by ref,	  &
				cpumon_difference::clocktime by ref)

	cpumon_difference::clockticks = cpumon_current::clockticks	  &
					- cpumon_previous::clockticks
	cpumon_difference::cputim = cpumon_current::cputim		  &
					- cpumon_previous::cputim
	cpumon_difference::bufio = cpumon_current::bufio		  &
					- cpumon_previous::bufio
	cpumon_difference::dirio = cpumon_current::dirio		  &
					- cpumon_previous::dirio
	cpumon_difference::pageflts = cpumon_current::pageflts		  &
					- cpumon_previous::pageflts

	cpumon_cpu_threshold = cpumon_difference::clockticks		  &
					/100 * cpumon_cpu_percentage

! see if cpu usage is excessive

	if (cpumon_difference::cputim >= cpumon_cpu_threshold) then
		cpumon_current::cpu_excessive = -1
	else
	cpumon_current::cpu_excessive = 0
	end if !(cpumon_difference::cputim >= cpumon_cpu_threshold) then

! now update the totals

	cpumon_total::clockticks = cpumon_total::clockticks		  &
				+ cpumon_difference::clockticks
	cpumon_total::cputim = cpumon_total::cputim			  &
				+ cpumon_difference::cputim
	cpumon_total::bufio = cpumon_total::bufio			  &
				+ cpumon_difference::bufio
	cpumon_total::dirio = cpumon_total::dirio			  &
				+ cpumon_difference::dirio
	cpumon_total::pageflts = cpumon_total::pageflts			  &
				+ cpumon_difference::pageflts

	end sub
