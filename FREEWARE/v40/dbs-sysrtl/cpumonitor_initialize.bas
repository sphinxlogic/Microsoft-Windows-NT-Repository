	%title	"Initialize CPU Monitor data structures"
	%ident	"X1-001"

	sub cpumonitor_initialize (long control_flags,			  &
					string delta_time,		  &
					long cpu_percentage,		  &
					long busy_threshold,		  &
					long nularg1,			  &
					long nularg2,			  &
					long nularg3,			  &
					long nularg4)

!+
! Version:	X1-001
!
! Facility:	CPU Monitor
!
! Abstract:	Initializes the data structures for the CPU monitor.
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
!	This routine initializes the data structures used by the cpu monitor
!	routines.
!
! Calling Sequence:
!	call cpumonitor_initialize (delta_time by desc,
!					cpu_percentage by ref,
!					busy_threshold by ref)
!
! Formal Argument(s):
!	control_flags.rl.r
!		Reserved for future use.
!	delta_time.rt.ds
!		A fixed length string descriptor pointing to a string
!		containing the time to wait between checks of CPU usage.
!		The time should be given as a delta time.
!	cpu_percentage.rl.r
!		The address of a longword containing the percentage of
!		cpu time above which is considered excessive.
!	busy_threshold.rl.r
!		The address of a longword containing the maximum number of
!		times that the cpu_percentage can be exceeded before the
!		AST routine forces an image exit.
!	nularg1.rl.r
!		Reserved for future use.
!	nularg2.rl.r
!		Reserved for future use.
!	nularg3.rl.r
!		Reserved for future use.
!	nularg4.rl.r
!		Reserved for future use.
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

	cpumon_control_flags = control_flags

	local_delta_time = edit$(delta_time, 4 + 8 + 16 + 128)
	if (edit$(local_delta_time, 2) =  "") then
	    local_delta_time = cpumon_t_delta_time
	end if !(edit$(local_delta_time, 2) =  "") then

	call sys$bintim (local_delta_time by desc,			  &
				cpumon_snooze_time by ref)

	if (cpumon_snooze_time::long1 >= 0) then
		call sys$bintim (cpumon_t_delta_time by desc,		  &
					cpumon_snooze_time by ref)
	end if !(cpumon_snooze_time::long1 >= 0) then

	cpumon_structure_version = cpumon_k_structure_version

	cpumon_cpu_percentage = cpu_percentage
	if ((cpumon_cpu_percentage < cpumon_k_min_percentage)		  &
		or (cpumon_cpu_percentage > cpumon_k_max_percentage)) then
	    cpumon_cpu_percentage = cpumon_k_default_percentage
	end if !((cpumon_cpu_percentage < cpumon_k_min_percentage)

	cpumon_busy_counter = 0
	cpumon_busy_threshold = busy_threshold
	if ((cpumon_busy_threshold < cpumon_k_min_count)		  &
		or (cpumon_busy_threshold > cpumon_k_max_count)) then
	    cpumon_busy_threshold = cpumon_k_default_count
	end if !((cpumon_busy_threshold < cpumon_k_min_count)

	jpi_itmlst(0)::buflen = 8		! we use this as a base for
	jpi_itmlst(0)::itmcod = jpi$_logintim	!   our time calculations
	jpi_itmlst(0)::bufadr = loc(cpumon_logintim)
	jpi_itmlst(0)::retlen = 0
	jpi_itmlst(1)::buflen = 0		! end of item list
	jpi_itmlst(1)::itmcod = 0

	call sys$getjpiw (, , , jpi_itmlst(0) by ref, jpi_iosb, , )

	cpumon_flag_1 = cpumon_logintim::long0	! used for consistency check
	cpumon_flag_2 = not cpumon_flag_1	!   by the AST routine to make
						!   sure the data structures
						!   are clean

	call cpumonitor_getjpi			! get ALL of the stuff we need

	cpumon_previous::cpu_excessive = 0	! this is the only previous
						!   field we need to clear
						!   all the others get loaded
						!   next time round

	cpumon_difference::clocktime::long0 = 0	! reset all the difference
	cpumon_difference::clocktime::long1 = 0	!   fields the first time
	cpumon_difference::clockticks = 0	!   through... since we only
	cpumon_difference::cputim = 0		!   have the first lot of
	cpumon_difference::bufio = 0		!   data; i.e. no previous
	cpumon_difference::dirio = 0		!   to speak about
	cpumon_difference::pageflts = 0

	cpumon_total::clockticks = 0		! zero all those total things
	cpumon_total::cputim = 0		!   that we are likely to use
	cpumon_total::bufio = 0			! those items not zeroed here
	cpumon_total::dirio = 0			!   will not be used later or
	cpumon_total::pageflts = 0		!   cannot be totalled

	call sys$setimr (,			! set up the AST timer	  &
			cpumon_snooze_time by ref,			  &
			loc(cpumonitor_ast) by value, , )

	end sub
