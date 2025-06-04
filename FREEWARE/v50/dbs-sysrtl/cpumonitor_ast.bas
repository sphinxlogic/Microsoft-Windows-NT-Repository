	%title	"CPU Monitor AST routine"
	%ident	"X1-001"

	sub cpumonitor_ast (long param, long r0, long r1, long pc, long psl)

!+
! Version:	X1-001
!
! Facility:	CPU Monitor
!
! Abstract:	Calculate CPU and/or other statistics and possibly exit the
!		current image if we have exceeded the predefined limits.
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
!	This routine determines if the current process is using "excessive"
!	CPU time, and if so, does the honourable thing...
!	'tis a far better thing I do . . .
!
! Calling Sequence:
!	Called via an AST.
!
! Formal Argument(s):
!	As defined above - don't try to use them.
!
! Implicit Inputs:
!	CPUMONITOR map.
!
! Implicit Outputs:
!	CPUMONITOR map.
!
! Completion Codes:
!	Exits with SS$_EXCPUTIM otherwise returns from whence it came.
!
! Side Effects:
!	The current image could be exited.
!--

	option	type = explicit,					  &
		constant type = integer,				  &
		size = (integer long, real double)

%include "CPUMONITORDEF" %from %library "OLGA$TEXT"

	declare string	ast_fao
	declare word	buflen
	declare string	faobuf

mainline:

	if (cpumon_structure_version <> cpumon_k_structure_version) then
	    call lib$put_output						  &
		("%CPUMONITOR-W-BADVER, incorrect structure version" by desc)
	    gosub initialize_structures
	else
	if (cpumon_flag_1 <> (not cpumon_flag_2)) then
	    call lib$put_output						  &
		("%CPUMONITOR-W-NOTINIT, not initialized correctly" by desc)
	    gosub initialize_structures
	else
	call sys$setimr (, cpumon_snooze_time by ref,			  &
				loc(cpumonitor_ast) by value, , )

	call cpumonitor_getjpi

	if (cpumon_current::cpu_excessive) then
	    if (cpumon_previous::cpu_excessive) then
		cpumon_busy_counter = cpumon_busy_counter + 1
		if (cpumon_busy_counter >= cpumon_busy_threshold) then
		    ast_fao = "!/%CPUMONITOR-I-PARAMS, "		  &
			+ "PC=!XL, PSL=!XL, R0=!XL, R1=!XL"		  &
			    + "!/!22* Parameter = !XL (!-!UL)(!-!SL)"
		    faobuf = string$(256, 32)
		    call sys$fao (ast_fao by desc,			  &
					buflen by ref,			  &
					faobuf by desc,			  &
					loc(pc) by value,		  &
					loc(psl) by value,		  &
					loc(r0) by value,		  &
					loc(r1) by value,		  &
					loc(param) by value)
		    call lib$put_output (left(faobuf, buflen) by desc)
		    ast_fao = "%CPUMONITOR-I-EXITING, exiting image..." &
			+ "!/  Total CPU ticks = !UL"			  &
			+ "!/       clockticks = !UL"			  &
			+ "!/  CPU usage of !UL% exceeded on "		  &
				+ "!UL consecutive occasions"
		    faobuf = string$(256, 32)
		    call sys$fao (ast_fao by desc,			  &
					buflen by ref,			  &
					faobuf by desc,			  &
					cpumon_total::cputim by value,	  &
					cpumon_total::clockticks by value,&
					cpumon_cpu_percentage by value,	  &
					cpumon_busy_threshold by value)
		    call lib$put_output (left(faobuf, buflen) by desc)
		    call sys$exit (ss$_excputim by value)
		end if !(cpumon_busy_counter > cpumon_busy_threshold) then
	    else
	    cpumon_busy_counter = 0
	    end if !(cpumon_previous::cpu_excessive) then
	end if !(cpumon_current::cpu_excessive) then

	end if !(cpumon_flag_1 <> (not cpumon_flag_2)) then
	end if !(cpumon_structure_version <> cpumon_k_structure_version) then

	exit sub

initialize_structures:

	call lib$put_output						  &
	    ("-CPUMONITOR-I-CALLINIT, calling initialization routine" by desc)
	call cpumonitor_initialize (0 by ref, "" by desc,		  &
					0 by ref, 0 by ref, 0 by ref,	  &
					0 by ref, 0 by ref, 0 by ref)

	return

	end sub
