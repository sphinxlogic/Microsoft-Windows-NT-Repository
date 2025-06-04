.TITLE          PROBE Probe Utility Messages
.IDENT          'Version 2.2'
.FACILITY       PROBE,22/PREFIX=PRB_
.SEVERITY       FATAL

ACQLINK         <unable to communicate with network listener - startup failed>
ACQPORT         <unable to set or start Ethernet port in required mode>
ACQPREP         <unable to initialize listener within prescribed time>
NOBOOST         <unable to boost priority to real-time listening level>
NOETDEV         <unable to find Ethernet device - adapter search exhausted>
NOIO            <unable to assign standard input or output channels>
NOIPC           <unable to create interprocess communication mechanism>
NOPRIV          <insufficient recording privileges - check documentation>
NOTICK          <missing schedule - must be provided when mode not interactive>
NOTIME          <unable to obtain & process system time reference>
NOWORK          <insufficient setup attributes to proceed - check documentation>

.SEVERITY       ERROR

ACQFILE         <recording file problem - logging function cancelled>
ACQOVFL		<recording buffer overflow - logging function cancelled>
DMSFILE         <statistics setup or reporting problem - function cancelled>
DMSITIME        <internal display manager problem setting refresh timer>
DMSLINK         <unable to communicate with data manager - functions cancelled>
DMSPREP         <data manager initialisation timed out - functions cancelled>
DMSTRIK         <statistician & display manager inactive - startup failed>
DSP_IO		<stop - internally triggered rundown on display I/O function>
INTERNAL	<service call # !UL returned unsuccessful status>/FAO=1
KEY_IO          <stop - internally triggered rundown on keyboard I/O function>
LOG_IO          <abnormal file I/O queueing - logging function cancelled>
NET_IO          <stop - internally triggered rundown on network I/O queueing>
QFRMINIT        <frame buffer queue failed to initialize !UL/!UL slots>/FAO=2
SYNCSLOW	<stop - unresponsive subprocess could not synchronize in time>

.SEVERITY       WARNING

ACQOTOCD	<collision report setup problem - capture is collision blind>
ADAPTER         <specified adapter not usable - searching default adapter list>
DIAGBUG		<abnormal event occured while processing diagnostics - exiting>
DUPNDEF         <ignoring node !AS at table line !UL - duplicated address>/FAO=2
DUPPDEF         <ignoring protocol !AS at table line !UL - duplicated ID>/FAO=2
DSPCHAR         <invalid display device characteristics - check documentation>
DSPCNX          <internal display connection problem>
DSPCVT          <internal output data type conversion problem>
DSPFAO          <internal output string construction problem>
DSPMODE         <ignoring display and attributes when mode not interactive>
DSPOUT          <internal display I/O delivery problem>
DSPOVF          <sysgen parameter MAXBUF too small - check documentation>
DSPSCR          <internal buffer area initialization problem>
DSPWDW          <window/page line count too small for minimum display>
FULNARR		<node array full - cannot add definition at table line !UL>/FAO=1
FULPARR		<all protocols set - cannot add definition at table line !UL>/FAO=1
INVCYC          <cycle value outside range [1..32767] - single cycle defaulted>
INVPFLT         <ignoring filter !AS outside of valid protocol range [1..15]>/FAO=1
INVSMTH         <dropping smoothing value !AS% outside of valid range [1..99]>/FAO=1  
INVTHRS         <dropping threshold value !AS% outside of valid range [1..50]>/FAO=1
INVTIMA         <activity time outside range - schedule completely cancelled>
INVTIMB         <standby time outside range - single immediate cycle defaulted>
KILLACQ        	<network listener release time exceeded - functions cancelled>
KILLDMS        	<data manager release time exceeded - functions cancelled>
MCTTSTS		<file required for multiple cycle statistics - report cancelled>
MISNFLT         <ignoring filter !AS - no such node defined in table>/FAO=1
MISPFLT         <ignoring filter !AS - no such protocol defined in table>/FAO=1
NET_FLG		<network listener reported warning during capture>
ODDADD		<ignoring multicast node definition at table line !UL>/FAO=1
TABLEVT		<unexpected event occured during table file input processing>
TBLCMD		<unknown command element or missing value at table line !UL!/!17* \!AS\>/FAO=2
TBLPAR		<required name parameter missing at table line !UL>/FAO=1
TBLQLF		<required qualifier missing or illegal value at table line !UL>/FAO=1
UNKPFLT		<protocol filter !UW undefined - state is not modifiable>/FAO=1
UNSAFE          <cycle !UW terminated abnormally - unreportable statistics>/FAO=1

.SEVERITY       INFORMATION

DEFSPEC         <using network adapter !AS found on system>/FAO=1
DMSPART         <statistician function is valid and active>
EX1MTHD         <automatic exit disabled - press space bar or ^Z to exit>
EX2MTHD         <automatic exit set at !UW cycle!%S of !UW second!%S - space bar or ^Z to override>/FAO=2
EX3AUTO		<Exit= Automatic @ (!5ZW)!5ZW+!4ZW [^Z | SpaceBar]>/FAO=3
EX3MAN		<Exit= Manual user action; either of [^Z | SpaceBar]>
FLUSH           <cycle !UW statistics completed at !%D>/FAO=2
MAXPACIN        <stop - packet counter limit reached>
MLTCSTOF        <turning multicast filtering off>
MLTCSTON        <turning multicast filtering on>
PFLTDIS         <protocol filtering disabled - prior active now standby>
PFLTENA         <protocol filtering enabled - prior standby now active>
PFLTINH		<protocol filtering inhibited - no protocols defined>
PFLTNUL		<protocol filtering inhibited - no standby protocols>
PFLTOFF         <protocol filter !UW state changed to off>/FAO=1
PFLTON1         <protocol filter !UW state changed to on/active>/FAO=1
PFLTON2		<protocol filter !UW state changed to on/standby>/FAO=1
TYPEOFF		<turning prior format filtering off - all traffic now accepted>
TYPETHR		<turning Ethernet format filtering on - rejecting IEEE>
TYPE802		<turning IEEE format filtering on - rejecting Ethernet>

.SEVERITY       SUCCESS

CYCLEND         <stop - automatic schedule terminated normally - no more cycles>
SPACEBAR        <stop - operator termination command received>

.END
