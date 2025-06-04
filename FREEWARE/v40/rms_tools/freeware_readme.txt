RMS_STATS, UTILITIES, Display RMS file access statistics and more RMS tools

	The main tool in this collection is RMS_STATS.
	This tool provides an alternative to MONITOR RMS for files
	that are marked for RMS statistics ($SET FILE/STAT ... )
	It displays raw numbers versus Monitor's rates/second.
	As such it can be a more accurate tool for transient usage.
	The information presented is comparable with the ANALYZE/SYSTEM 
	output from SHOW PROC/RMS=FSB (VMS V6.1). This program 
	requires CMEXEC and target file access privileges to work. 

	Options are a repeat time, absolute or incremental numbers,
	list of files, and selected output formatting. The 'summary'
	output format line shows the number of IOs per Operation which
	is believed to be a handy indicator of file tuning effects.

	Other tools in the collection:

	    SIDR.C
		Program to print indexed file alternate key statistics.
		Mainly intended as a tool to understand excessive $PUT 
		times due to long 'DUPLICATE CHAINS'

	    COPYFILE.C		
		Example program from RMS reference manual

	    FDL$GENERATE.C	
		Example usage for FDL$GENERATE function for 
		ANAL/RMS/FDL like output but allowing FDL file 
		generation for currently accessed file.

	    GBLPAGFIL.COM
		Command file to create/compile/link C program to dynamically
		change then static SYSGEN param GBLPAGFIL. Occasionally 
		needed when dramatically increasing RMS Global buffer usage.
		Use with caution.

	    GLOBAL_BUFFER_DISPLAY.COM
		Command file to create/compile/link a BASIC program
		to dynamically display some RMS global buffer stats
		even if FILE STAT are not chosen.  VAX ONLY (V5.5-2 
		when last tested, NOT easily ported to ALPHA)

	    GLOBAL_BUFFER_HELPER.COM
		Command file to add up RMS global buffer 
		resource requirements for a wildcarded filespec.

	    INDEXED_FILE_COUNT.BAS
		Program to quickly (relative to ANALYZE/RMS) count valid,
		deleted and re-vectored record in an RMS indexed file.

	    INDEXED_FILE_PATCH.BAS;1
		Program to quickly (relative to ANALYZE/RMS) validate the
		structure for an RMS indexed file which is suspected 
		to have multiple corrupted areas due to hardware failure.
		It will generate an optional PATCH command file to try
		to patch 'around' broken bucket hopefully allowing CONVERT
		to save as much as possible from such file.

	    DEBUG.COM
		Silly command flip debug bit in image header (used for ZAP)

	    LINK.COM
		Silly LINK command file to pull in VAXCRTL.

	    MAKE_DIR.C
		Alternative to LIB$CREATE_DIR which allows
		for pre-allocation of a directory. 

	    RMS_CALL_TRACE.COM
		Command file to create tool to re-vector direct 
		calls to RMS routines (SYS$OPEN et al) through
		a shareable which traces the calls. VAX ONLY

	    SET_DIR.C
		Complement to SET FILE/NODIR. (Patch to hole in foot :-)

	    SHOW_ROOTS.C
		Example program from RMS reference manual
		which will show the index root levels for files.

	    ZAP.MAR
		Simple tool to use the VMS Debugger to PATCH buckets
		in an RMS indexed file (or any file really). Proven 
		useful trying to recover remote files over slow links.

	Have fun!	
	Regards,
		Hein van den Heuvel

