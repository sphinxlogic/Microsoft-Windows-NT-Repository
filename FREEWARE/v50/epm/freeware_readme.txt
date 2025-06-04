EPM, UTILITIES, Collect and display disk I/O statistics

			EPM V2.0 Release Notes
                             March 26, 2000

Written by:
	Ed Heinrich
	The LOKI Group, Inc.
	ed@loki.com

Ported to Alpha, cleaned up, and a VAX bug fixed by:

	Hunter Goatley
	goathunter@PROCESS.COM


OVERVIEW:

The EPM package provides a means to collect statistics on disk I/O operations 
that VMS does not, by default, provide.  EPM is capable of providing a synopsis 
of the type of disk I/O's, the size of each I/O, and tracking which files are
accessed the most both for read/write operations and as a result of file
fragmentation.


DISCLAIMER:

EPM HAS NEITHER BEEN TOTALLY DEBUGGED NOR TESTED IN EVERY POSSIBLE
CONFIGURATION.

EPM is NOT a commercial product but, rather, is provided free of charge on an 
"as is" basis.  As such, neither Edward A. Heinrich nor The LOKI Group, Inc. 
make any guarantees about the usefulness of any part of this package nor take 
any responsibility for any damage, loss, or harm that may arise out of its 
execution.


PACKAGE CONTENTS:

EPM consists of two parts:

EPMDRIVER is a standard VAX/VMS device driver, that contains code to intercept 
disk I/O request packets, (IRPs), and collect statistics based upon the 
content of the IRP.  EPMDRIVER, which communicates to the display program
via the QIO interface, intercepts the STARTIO address of the disk(s) to be 
monitored.

EPM is the user-interface to EPMDRIVER. EPM.C is a command line driven
program that communicates with EPMDRIVER, via the QIO interface, to
establish the monitoring interval, number of files to collect statistics
for, and the disk devices to intercept.  EPM also provides a means to
display the collected statistics to the current terminal.


The EPM command accepts the following qualifiers:

	/INTERVAL
		time, in seconds, between updates.
		default=60

	/DEVICES
		one or more disk devices to be monitored.  

	/FILE_COUNT
		the number of files, with both the most accesses and the ones
		that incurred the most split I/O operations within the interval
		time period.
		default=10

Sample command:
	$ epm/dev=(dka300,dka100)/file=25/inter=120

	collects statistics for DKA300 and DKA100, 25 file entries for both
	most accessed and highest split I/O counts, updates display once every
	two minutes.


BUILD INSTRUCTIONS:

To create the .EXE files, simply execute MMK or MMS in the EPM directory.

To load EPMDRIVER into memory:

VAX:

	$! LOAD_EPM.COM 	Loads EPMDRIVER
	$! substitute dev:[directory]
	$MC SYSGEN
	LOAD dev:[directory]EPMDRIVER
	CONN EPMA0/NOADAPTER/DRIVER=EPMDRIVER
	$

ALPHA:

	$ MCR SYSMAN IO CONNECT EPMA0:/NOADAPTER/DRIVER=EPMDRIVER

To execute EPM, add the following command to your LOGIN.COM file or execute it 
at DCL:

$ EPM :== $DEV:[DIRECTORY]EPM.EXE

EPM is now available as a command.

Sample output using command "epm/dev=(dka300,dka100)/file=25/inter=120"


Disk DKA300 Statistics as of 15:47:06.17

 Writes:	Reads:  	Swapper: 	Paging I/O	Split I/O 
      4  	    169  	      0  	    112  	      5
 1 Block	4 Blocks	16 Blocks	32 Blocks	64 Blocks
     76  	     52  	     38  	      7  	      0

   DKA300 Split I/O Statistics
Count   File Name
      3	DISK$LOKI_0:[SYSCOMMON.SYSLIB]DECW$DWTLIBSHR.EXE;1
      2	DISK$LOKI_0:[SYSCOMMON.SYSEXE]SYSGEN.EXE;2

   DKA300 Most Accessed File Statistics
Count  File Name
     13	DISK$LOKI_0:[SYSCOMMON.SYSEXE]LOGINOUT.EXE;3
      8	DISK$LOKI_0:[SYSCOMMON.SYSEXE]DIRECTORY.EXE;2
      1	DISK$LOKI_0:[000000]EAH.DIR;1
     12	DISK$LOKI_0:[SYSCOMMON.SYSEXE]TYPE.EXE;2
      6	DISK$LOKI_0:[EAH]FONT.C;1
     12	DISK$LOKI_0:[SYSCOMMON.SYSEXE]SYSUAF.DAT;7
     16	DISK$LOKI_0:[SYSCOMMON.SYSEXE]RIGHTSLIST.DAT;1
      2	DISK$LOKI_0:[000000]INDEXF.SYS;1
      2	DISK$LOKI_0:[SYSCOMMON.SYSMGR]SYLOGIN.COM;2
      8	DISK$LOKI_0:[SYSCOMMON.SYSEXE]SET.EXE;2
      4	DISK$LOKI_0:[SYSCOMMON.SYSLIB]UVMTHRTL.EXE;2
     19	DISK$LOKI_0:[SYSCOMMON.SYSLIB]VAXCRTL.EXE;2
      6	DISK$LOKI_0:[SYSCOMMON.SYSEXE]DECW$TERMINAL_CREATE.EXE;1
      9	DISK$LOKI_0:[SYSCOMMON.SYSLIB]DECW$DWTLIBSHR.EXE;1
      1	DISK$LOKI_0:[SYSCOMMON.SYSLIB]DECW$TRANSPORT_COMMON.EXE;1
      3	DISK$LOKI_0:[SYSCOMMON.SYSLIB]DECW$XLIBSHR.EXE;1
      7	DISK$LOKI_0:[SYSCOMMON.SYSLIB]DECW$TERMINALSHR.EXE;1
     19	DISK$LOKI_0:[SYSCOMMON.SYSEXE]SYSGEN.EXE;2
      1	DISK$LOKI_0:[SYSCOMMON.SYSLIB]SCRSHR.EXE;1
     15	DISK$LOKI_0:[SYS0.SYSEXE]VAXVMSSYS.PAR;1
      1	DISK$LOKI_0:[SYSCOMMON.SYSEXE]TPU.EXE;2
      3	DISK$LOKI_0:[SYSCOMMON.SYSLIB]TPUSHR.EXE;2
      3	DISK$LOKI_0:[SYSCOMMON.SYSLIB]TPU$CCTSHR.EXE;2

Disk DKA100 Statistics as of 15:47:06.17

 Writes:	Reads:  	Swapper: 	Paging I/O	Split I/O 
     34  	     73  	      4  	     59  	     41
 1 Block	4 Blocks	16 Blocks	32 Blocks	64 Blocks
     35  	     21  	     12  	     39  	      0

   DKA100 Split I/O Statistics
Count   File Name
     35	DISK$LOKI_1:[EAH]EVE$SECTION.TPU;1
      3	DISK$LOKI_1:[SYSPAGE]PAGEFILE.SYS;1
      3	DISK$LOKI_1:[EAH]RFC854.TXT;2

   DKA100 Most Accessed File Statistics
Count   File Name
      2	DISK$LOKI_1:[EAH]LOGIN.COM;4
      8	DISK$LOKI_1:[EAH]VERIFY.EXE;3
      1	DISK$LOKI_1:[000000]EAH.DIR;1
     53	DISK$LOKI_1:[EAH]EVE$SECTION.TPU;1
     20	DISK$LOKI_1:[000000]INDEXF.SYS;1
      5	DISK$LOKI_1:[EAH]RFC854.TXT;1
      4	DISK$LOKI_1:[SYSPAGE]PAGEFILE.SYS;1
      8	DISK$LOKI_1:[EAH]RFC854.TXT;2


KNOWN PROBLEMS:

Currently there are no known bugs in EPM V2.0


RESTRICTIONS:

EPM has never been tested on a shadow set disk and, therefore, contains code 
to prevent monitoring of a disk that is a shadow set member.  Any site that 
cares to test EPM in that environment can remove the checks in the EPM_COLLECT
routine in EPMDRIVER.MAR.  Multiple copies of EPM can be run simultaneously
but a disk can only be monitored by one copy of EPM at any time.

