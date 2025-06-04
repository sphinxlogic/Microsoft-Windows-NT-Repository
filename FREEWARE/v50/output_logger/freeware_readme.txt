OUTPUT_LOGGER, UTILITIES, Capture output from multiple sources to a single file

   Applications that generate a number of small log files (detached or batch
   server processes, for example) may end up creating a large number of files
   that can fill disks and directories and make management of the information
   difficult.  These many files, in turn, are difficult to search and
   maintain and are a minimum of the disk cluster factor in size.

   By sending the contents of all of the log files to a single, common log,
   the number of files and the amount of I/O used to write and manage the logs
   can be significantly reduced.  By using a single writer to the log file
   I/O can be effectively reduced (in some cases from hundreds of I/O per
   second to less than 10 per minute).

   This module contains the routines to open and write records to a common log
   file.  This program is written in BLISS32 and works on OpenVMS VAX or
   Alpha.  Get the BLISS compiler from the OpenVMS freeware CD if you have not
   already installed it.

   This program creates a system-wide permanent mailbox called
   LOG_OUTPUT_MAILBOX.  All write I/O to that mailbox will be captured,
   stamped with the current time, the writer's PID and process name, and the
   data itself.  Every minute, if there has been any write activity to the log
   file, the file is $FLUSHed to make sure that all the data made it to disk.

   All RMS I/O is done asynch with deferred writes to the sequential file.
   Multiple writers (in a cluster for example) are allowed.

   The mailbox created by this program is allowed records up to 2048 bytes
   long.  The mailbox is created to be large enough to hold 250 records at
   maximum size.

   All I/O is done at AST level.  This program hibernates most of the time and
   just responds to write requests to the mailbox and then reads those records
   and writes them to the log file.

   If a message in the mailbox is read with the 20 byte string
   "<<>>REOPENLOGFILE!<>", the log file will be closed and a new one created.

   Privs required to run this program:	PRMMBX, SYSNAM

   Example commands to run this program:

	    $DEFINE/SYSTEM/EXEC LOG_OUTPUT_LOGFILE SYS$MANAGER:OUTPUT_LOGGER.TXT
	    $RUN SYS$SYSTEM:OUTPUT_LOGGER -
		    /DETACHED -
		    /AST_LIMIT=100 -
		    /BUFFER_LIMIT=100000 -
		    /ENQUEUE_LIMIT=1000 -
		    /ERROR=SYS$MANAGER:OUTPUT_LOGGER.LOG -
		    /EXTENT=1000 -
		    /FILE_LIMIT=100 -
		    /IO_BUFFERED=100 -
		    /IO_DIRECT=100 -
		    /MAXIMUM_WORKING_SET=1000 -
		    /OUTPUT=SYS$MANAGER:OUTPUT_LOGGER.LOG -
		    /PAGE_FILE=50000 -
		    /PRIORITY=10 -
		    /PRIVILEGES=(PRMMBX, SYSNAM) -
		    /PROCESS_NAME="OutputLogger" -
		    /UIC=[1,4] -
		    /WORKING_SET=1000

   Compile and Link instructions for VAX or Alpha:

	    $ BLISS/LIST/OPTIMIZE OUTPUT_LOGGER
	    $ LINK/MAP/FULL/CROSS OUTPUT_LOGGER
	    $ COPY OUTPUT_LOGGER.EXE SYS$COMMON:[SYSEXE]

   Example of how to re-open the log file while the output logger is running:

	    $ OPEN/WRITE/SHARE OLMBX LOG_OUTPUT_MAILBOX:
	    $ WRITE OLMBX "<<>>REOPENLOGFILE!<>"
	    $ CLOSE OLMBX

   Consider this module a template; feel free to modify as needed for your own
   uses.  No support is, however, expressed or implied.

  AUTHOR:   Norman Lastovica / Oracle Rdb Engineering
	    Oracle Corporation / Colorado Springs, Colorado
	    nlastovi@us.oracle.com
