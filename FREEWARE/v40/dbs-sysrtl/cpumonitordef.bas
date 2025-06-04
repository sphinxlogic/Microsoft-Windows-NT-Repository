!	%title	"CPUMONITORDEF CPU Monitor Include File"
!	%ident	"X1-001"

!+
! Version:	X1-001
!
! Facility:	CPU Monitor
!
! Abstract:	Common definitions for CPU Monitor routines.
!
! Environment:	User mode.
!
! History:
!
!	26-May-1995, DBS; Version X1-001
! 001 -	Original version.
!-

! External references (and includes)

%include "$JPIDEF"	%from %library "SYS$LIBRARY:BASIC$STARLET.TLB"
%include "$SSDEF"	%from %library "SYS$LIBRARY:BASIC$STARLET.TLB"

	external long		cpumonitor_ast
	external long function	lib$ediv
	external long function	lib$put_output
	external long function	lib$sub_times
	external long function	sys$bintim
	external long function	sys$fao
	external long function	sys$getjpiw
	external long function	sys$gettim
	external long function	sys$setimr

	external sub cpumonitor_getjpi
	external sub cpumonitor_initialize (long, string, long, long,	  &
						long, long, long, long)

! Structures

record iosb_2
	long	retsts
	long	reserved_iosb_2
end record iosb_2

record item_list_3
	word	buflen
	word	itmcod
	long	bufadr
	long	retlen
end record item_list_3

record monitor_items
	quadword clocktime
	long	clockticks
	long	cputim
	long	bufio
	long	dirio
	long	pageflts
	long	cpu_excessive
end record monitor_items

record quadword
	long	long0
	long	long1
end record quadword

! Type declarations for constants

	declare long constant	cpumon_k_default_count = 2
	declare long constant	cpumon_k_min_count = 0
	declare long constant	cpumon_k_max_count = 10
	declare long constant	cpumon_k_default_percentage = 80
	declare long constant	cpumon_k_max_percentage = 99
	declare long constant	cpumon_k_min_percentage = 1
	declare long constant	cpumon_k_structure_version = 1
	declare string constant	cpumon_t_delta_time = "0 00:05:00.00"

! Type declarations for variables

	declare iosb_2		jpi_iosb
	declare item_list_3	jpi_itmlst(0 to 4)
	declare string		local_delta_time
	declare long		remainder
	declare long		sys_status
	declare quadword	system_time

! Commons and MAPS

	map (cpumonitor)						  &
		long		cpumon_structure_version,		  &
		long		cpumon_flag_1,				  &
		long		cpumon_flag_2,				  &
		long		cpumon_control_flags,			  &
		long		cpumon_cpu_percentage,			  &
		long		cpumon_cpu_threshold,			  &
		long		cpumon_busy_threshold,			  &
		long		cpumon_busy_counter,			  &
		quadword	cpumon_snooze_time,			  &
		quadword	cpumon_logintim,			  &
		monitor_items	cpumon_current,				  &
		monitor_items	cpumon_previous,			  &
		monitor_items	cpumon_difference,			  &
		monitor_items	cpumon_total
