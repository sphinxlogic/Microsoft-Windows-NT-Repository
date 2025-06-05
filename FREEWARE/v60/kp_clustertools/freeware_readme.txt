KP_CLUSTERTOOLS, UTILITIES, VMS & Cluster system management tools

* indicates a new tool for the V6 release

BAD_CHARS.COM *		Take file with odd characters added by Windows and
			replace odd characters with equivalents.
CONTIG.COM		Creates or extends a system file in as contiguous a
			manner as possible.
COMBINE_FEEDBACK.COM	Logically combines feedback data from multiple files.
DECNET_LATENCY.COM	Measure round-trip network latency over DECnet
EDIT_LAVC.COM, *	Automatically build working LAVC$FAILURE_ANALYSIS.MAR
			program customized for a given cluster
FIXQUE.COM,		Creates DCL code to re-create queues, jobs, forms, and
			characteristics, based on output from $SHOW QUEUE.
LAVC$FAILURE_OFF.MAR *	Macro program to turn LAVC$FAILURE_ANALYSIS off when
                        desired
NODE$TRACK.COM		Track cluster membership and notify someone of changes
SIFT_LAVC.COM		Creates time-sorted, cluster-wide list of OPCOM messages
			from SYS$EXAMPLES:LAVC$FAILURE_ANALYSIS.MAR program.
SHADOW$CONFIG.COM	Gathers shadow-set state info into DCL symbols or
			logical names, for use by other procedures, like these:
SHADOW$DISPLAY.COM	Uses ASCII art to display color-coded diagrams showing
			shadowset state in multi-site clusters.
SHAD$DISP.COM		Version with abbreviated, more-compact output, and
			optional support for disaster-tolerant clusters.
SHADOW$TRACK.COM	Track shadowset membership and notify someone of changes
SHOW_PATHS.COM * 	Display LAN cluster interconnect configuration in the
			form of a matrix
SHOW_PATHS_ECS.COM *	As above, but include PEDRIVER Equivalent Channel Set
			information on 7.3 and above
SHOW_SEGMENTS.COM *	Show LAN cluster interconnects as a line drawing


More-detailed descriptions:

BAD_CHARS.COM

Take file with odd characters added by Windows and
replace odd characters with equivalents.  For example,
Windows uses <PU2> for apostrophe, and left and right
double-quote marks.  This DCL fixes these up by
replacing them with an apostrophe and regular
quotation marks, respectively.  Pass the procedure the
name of the file, and it creates a new version of the
file with the characters fixed.


CONTIG.COM

Creates or extends a system file in as contiguous a manner as possible.  System
files can only have one file header (no extension headers) because they are
accessed by primitive code.  In a Contiguous Best Try file extension, VMS will
find the three largest extents on the disk, but then gives up and proceeds to
allocates space arbitrarily for the rest of the file.  This means the file
header for a system file may fill up prematurely, sometimes before the desired
file size is reached, and a backup/restore of the entire disk is then required
to repack the file before it can be extended to the desired size. This DCL
procedure finds the largest free extent and enlarges the file by only that
amount each cycle, repeating this until the desired size is reached, thus
minimizing the degree of fragmentation for the resulting file, and minimizing
the number of extents used up in the file header. 


COMBINE_FEEDBACK.COM

Logically combines feedback data from multiple files.

Some systems (especially government systems) are not
allowed to run continuously for the 24 hours or more
that AUTOGEN wants for valid feedback data.  These
systems may run 8 hours per day, then be shut down and
have their disks removed and locked in a safe overnight.
This DCL procedure logically combines the data from
multiple AUTOGEN feedback files, so that if feedback
data is saved at the end of each uptime session, these
periods of data can effectively be combined together
and used as input to AUTOGEN as if they were contiguous.
The procedure basically works by adding the values from
cumulative counters together, finding the highest value
for peak values, and using the most recent data for any
data items which represent "current" data, and adding
the elapsed times together.


DECNET_LATENCY.COM

Sends 100 loopback packets using NCP, and measures the
average round-trip network latency.  Since it uses
the NCP> LOOP NODE command, it needs DECnet Phase IV.


EDIT_LAVC.COM

LAVC$FAILURE_ANALYSIS is a facility included in VMS that
produces OPCOM messages on LAN cluster interconect
failures.  It is documented in an appendix of the OpenVMS
Cluster Systems Manual, but was fairly troublesome to
set up.  This DCL procedure gathers all the information
necessary and even creates a customized version of the
LAVC$FAILURE_ANALYSIS.MAR program for a given cluster,
taking all the pain out of getting LAVC$FAILURE_ANALYSIS
up and running.  More info may be found in the file
EDIT_LAVC_README.TXT.

FIXQUE.COM

Creates DCL code to re-create queues, jobs, forms, and
characteristics, based on output listing from $SHOW QUEUE.
Was developed to solve queue file corruption; still useful
to help in moving queues and jobs between systems or
setting up one cluster to have the same queue environment
as another.  Is also supplied in a "squeezed" version
(FIXQUE_SQZ.COM) for faster execution.


LAVC$FAILURE_OFF.MAR

Short Macro program to turn off LAVC$FAILURE_ANALYSIS if
desired.


NODE$TRACK.COM

Track cluster membership and notify someone by e-mail when things change, like
when a node leaves the cluster or joins the cluster.


SIFT_LAVC.COM

Sifts through OPERATOR.LOG files on all nodes in a cluster
and creates a time-sorted, consolidated list of all the %LAVC
console messages from SYS$EXAMPLES:LAVC$FAILURE_ANALYSIS.MAR
in the format of:
    dd-mmm-yyyy hh:mm:ss.cc nodexx %LAVC-s-ident, text
This allows one to more easily determine the sequence of
events and timing in a failure of a cluster interconnect.


SHADOW$CONFIG.COM

Determine the existence, membership, and status of all shadowsets on the
system, and their member devices.  Stores results in either DCL symbols or
in logical names.

The following information is returned by the procedure:
SHADOW$CONFIG_set_count                     Number of shadowsets
SHADOW$CONFIG_SET_n_LABEL                   Volume label for each shadowset
SHADOW$CONFIG_SET_n_DEVICE_NAME             DSAn: device name for shadowset
SHADOW$CONFIG_SET_n_MEMBER_COUNT            shadowset size (# of members)
SHADOW$CONFIG_SET_n_MEMBER_m_DEVICE_NAME    Member device names
SHADOW$CONFIG_SET_n_MEMBER_m_STATE          Member state: "ShadowMember",
                                            "FullCopying", or "MergeCopying"
There is also a timestamp logical name defined if the results are placed
into logical names:
SHADOW$CONFIG_TIMESTAMP                     Time of data-gathering


SHADOW$DISPLAY.COM

Display, in semi-graphical format, all the host-based volume shadowing
shadow sets, highlighting any anomalies (such as lack of redundancy
or full copies in progress) with the judicious use of color.


SHAD$DISPLAY.COM

Abbreviated version of SHADOW$DISPLAY.COM, taking less on-screen real estate. 

This version optionally supports a multi-site disaster-tolerant cluster
configuration (it does assume that disk allocation classes are $1$ at one site
and $2$ at the other site so it can tell which site a given disk is located
at).  Shadowsets without cross-site shadowing protection are then flagged as
unprotected, even if they have two or more members, if all are within a single
site.


SHADOW$TRACK.COM

Track shadowset membership and notify someone by e-mail when a member is added
or lost, when a new shadowset is created, or when a shadowset goes away. 


SHOW_PATHS.COM

DCL procedure to display LAN cluster interconnect
details in the form of a connectivity matrix.
Nodes and their LAN adapters are shown along the
top and left of the matrix, with connectivity
indicated by a "+" at the conjunction of any two
adapters which can communicate with each other
using the NISCA protocol.


SHOW_PATHS_ECS.COM

DCL procedure to display LAN cluster interconnect
details in the form of a connectivity matrix, but
with the added detail of indicating Preferred Paths
in VMS versions prior to 7.3, and Equivalent
Channel Set information for VMS versions 7.3 and
above.


SHOW_SEGMENTS.COM

Shows the LAN cluster interconect configuration
in the form of an ASCII line drawing.


------------------------------------------------------------------------------
Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
                      or keith.parris@hp.com

I welcome questions, comments, corrections, suggestions, etc. via e-mail.

Because the VMS Freeware CD is updated only periodically, it is advisable to
check for later versions of any tools included here.

Look for latest updated versions or fixes at:
  http://encompasserve.org/~parris/

DECUS presentations describing some of these tools may be found at:
  http://www2.openvms.org/parris/ or http://www.geocities.com/keithparris/ 
