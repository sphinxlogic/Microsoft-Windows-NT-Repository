PDP_SIMULATORS, SOFTWARE, PDP-8 and PDP-11 simulators

These PDP-8 and PDP-11 simulators are not product quality. They are freeware
versions made available for your hacking and experimentation pleasure.

Sketchy documentation on the simulators is in file SIM.DOC.

1. The simulators work on both VAX/VMS and on Alpha/OSF1.

2. There's a VMS build file for each simulator:

	pdp8_build	builds PDP8.EXE
	pdp11_build	builds PDP11.EXE

   The build files include the debugger, remove that if you want.

   For OSF/1, the build command lines are:

	cc pdp8*.c scp*.c -lm -o pdp8
	cc pdp11*.c scp*.c -o pdp11

3. The simulators are only partially tested.
