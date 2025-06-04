.TITLE          TESTDEV IO Access Characterization Utility Messages
.IDENT          'Version 2.2'
.FACILITY       TSTDVC,24/PREFIX=TDV_

.SEVERITY       FATAL
DEVBUG  <unable to retrieve device !AS information>/FAO=1
DISMNT  <unable to process - device !AS not currently mounted>/FAO=1
NEGSEEK <seek range must be positive integer or null for entire volume>
NILBLCK <IO block size must be positive integer no more than VMS limit>
NILIO   <IO collection size must be positive integer no more than !UL>/FAO=1
NILSTRM <IO stream count must be positive integer no more than !UL>/FAO=1
NODISK  <unsupported device type - !AS is not a disk>/FAO=1
NOLINK  <unable to open channel to device !AS>/FAO=1
NOPRIV  <insufficient IO privileges for operation - check documentation>
REVLBN  <illogical seek range - low/high LBN boundaries are reversed>
SEEKBLK <seek range insufficient to accomodate IO block size>

.SEVERITY       ERROR
IOBUG   <stream !UL unable to start upon status !UL at LBN !UL>/FAO=3
        
.SEVERITY       WARNING
COLLSIZ <ratio of collection size to seek range is relatively small>
DISTMAX <cannot use all distribution elements - will maximize>
DISTPOS <distribution elements must be positive - using default>
IOLSTRM <too few IO's to operate all streams - reducing streams>
MEG2LBN <impossibly large megabyte ranging - maximizing upper boundary>
STRMDIS <stream !UL discarded upon status !UL at LBNs !UL:!UL>/FAO=4
XCRANGE <seek upper LBN boundary exceeds device size - will maximize>
ZEROLOW <seek lower LBN boundary cannot be negative - will be zeroed>
        
.SEVERITY       INFORMATION
START   <Started testing device !AS (label !AS) at !%D>/FAO=3
STOP    <TestDev !AS on !AS completed testing at !%D>/FAO=3
DEVID   <Device Information!/  Type:!#* !AS!8* (serial !UL)!/  Host:!#* !AS!/  Capacity:!19UL block!%S (!UL megabyte!%S)>/FAO=7
TESTID  <Test Conditions!/  Coverage:!#* !AS!/  Threading:!18UL stream!%S!/  IO size:!20UL block!%S>/FAO=4
TESTID2 <  Seek range:!17UL block!%S (of !UL specified maximum)!/  Operations:!17UL read!%S  (of !UL specified : !UL dropped)>/FAO=5
PERFID  <Performance Information!/  Elapsed time:    !11%T!/  Throughput:!17UL byte/sec!/  IO rate:!18UL.!UL IO/sec>/FAO=4
PERFID2 <  IO duration:!14UL.!UL msec   (average)!/  IO spread:!16UL.!UL msec   (standard deviation)>/FAO=4
DISTID  <IO Time Distribution!/  _____ msec _____       Count>
DISTID2 " !4UL.!UL <--> !6<!UL.!UL!>!11UL (!UL.!UL%)"/FAO=7

.END
