	.title LD Error messages

	.facility LD,1/prefix=ld_

	.severity informational

CREATED		<File !AZ created>/fao_count=1
CONNECTED	<Connected !AZ to !AZ>/fao_count=2
NOWDISCONN	<Device !AZ is now disconnected>/fao_count=1
STATUS		<Device !AZ has a tracebuffer of !UL entries (!UL valid record!%S)>/fao_count=3
UNIT		<Allocated device is !AZ>/fao_count=1
TRACESTOPPED	<Trace stopped by other process>

	.severity warning

NOTCONNECTED	<Device !AZ is not connected>/fao_count=1
REMOTEALLOC	<Device !AZ is allocated on a remote node>/fao_count=1

	.severity fatal

CREATERR	<Unable to create file !AZ>/fao_count=1
OPENERR		<Unable to open file !AZ>/fao_count=1
CLOSERR		<Unable to close file !AZ>/fao_count=1
INFILERR	<Unable to open input file !AZ>/fao_count=1
OUTFILERR	<Unable to open output file !AZ>/fao_count=1
FILREADERR	<Unable to read input file !AZ>/fao_count=1
FILWRTERR	<Unable to write output file !AZ>/fao_count=1
CANTREADOLDFMT	<Can't read old format datafile (Version !UL)>/fao_count=1
DEVASSIGN	<Device has channels assigned>
ALRDYCONN	<Device already connected>
NOTCONN		<Device not connected>
FILEONVOLSET	<File watchpoint on volumeset not allowed>
FILEINUSE	<File incompatible connected to other LD disk in cluster>
DEVICEINUSE	<Device incompatible connected to other LD disk in cluster>
CONTTRACEACT	<Continuous tracing is already active for device !AZ>/fao_count=1
CONTTRACENOTACT	<Continuous tracing is not active for device !AZ>/fao_count=1
NOTRCDATA	<No trace data available>
TRCDISABL	<Tracing is disabled>
TRCENABL	<Tracing is already enabled>
BADENTPARAM	<Bad /ENTRIES parameter(s)>
PASTDATA	<Requested entry(s) past end of data>
CONFQUAL	<Conflicting qualifiers>
DUPUNIT		<Duplicate unitnumber>
BADUNIT		<Bad unit number !AZ>/fao_count=1
NOUNITSFOUND	<No useable logical disk units found>
BADDEVSYNTAX	<Bad device syntax (!AZ)>/fao_count=1
DETECTEDERR	<Detected fatal error>
NOWORLDPRIV	<No privilege to stop trace of device !AZ>/fao_count=1
NOGROUPPRIV	<No privilege to stop trace of device !AZ>/fao_count=1
NOWATCHDATA	<No watchpoint data available>
WPTNOTFOUND	<Watchpoint not found>
NOREADWRITE	<No read or write function specified for file mode>
VBNERROR	<Illegal virtual block number specified>
FILEONOTHER	<File not allowed to be on other device>
NOCLUSTER	<No cluster code loaded>
NOTVISIBLE	<Device is not visible on other nodes>
INVGEOMETRY	<Invalid geometry specified>
BADALLOCLASS	<Bad allocation class specified>
DEVCONNECTED	<Cannot set allocation class with active LD devices>
NOTODS2		<Container file must be on an ODS-2 volume>

	.end
