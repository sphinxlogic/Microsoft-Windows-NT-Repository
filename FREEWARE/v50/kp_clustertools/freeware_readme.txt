KP_CLUSTERTOOLS, UTILITIES, VMS and Cluster system management tools

CONTIG.COM		Creates or extends a system file in as contiguous a
			manner as possible.
COMBINE_FEEDBACK.COM	Logically combines feedback data from multiple files.
DECNET_LATENCY.COM	Measure round-trip network latency over DECnet
FIXQUE.COM		Creates DCL code to re-create queues, jobs, forms, and
			characteristics, based on output from $SHOW QUEUE.
LANTRAF.COM		Displays relative amount of SCS traffic on LAN adapters
NODE$TRACK.COM		Track cluster membership and notify someone of changes
REXMT.COM		Shows packet retransmit counters to detect LAN problems
SHAD.COM		Shows shadowset members which are in a merge or copy
SHAD_TRACK.COM		Shows shadowset merge/copy progress over time
SIFT_LAVC.COM		Creates time-sorted, cluster-wide list of messages
			from SYS$EXAMPLES:LAVC$FAILURE_ANALYSIS.MAR program.
SHADOW$CONFIG.COM	Gathers shadow-set state info into DCL symbols or
			logical names, for use by other procedures, like these:
SHADOW$DISPLAY.COM	Uses ASCII art to display color-coded diagrams showing
			shadowset state in multi-site clusters.
SHAD$DISP.COM		Version with abbreviated, more-compact output, and
			optional support for disaster-tolerant clusters.
SHADOW$TRACK.COM	Track shadowset membership and notify someone of changes
SHOW_PREF_PATH.COM	Show preferred LAN path being used to each cluster node


More-detailed descriptions:

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

FIXQUE.COM

Creates DCL code to re-create queues, jobs, forms, and
characteristics, based on output listing from $SHOW QUEUE.
Was developed to solve queue file corruption; still useful
to help in moving queues and jobs between systems or
setting up one cluster to have the same queue environment
as another.  Is also supplied in a "squeezed" version
(FIXQUE_SQZ.COM) for faster execution.

LANTRAF.COM

Display the LAN adapters enabled for SCS traffic and the relative amounts of
traffic SCS has sent to each 

NODE$TRACK.COM

Track cluster membership and notify someone by e-mail when things change, like
when a node leaves the cluster or joins the cluster.

REXMT.COM

Examines PEDRIVER's retransmitted-packet counters under SDA, to measure the
severity of LAN problems in an LAVc environment. ReXmt counter shows
re-transmitted packets (packet was lost or its Ack was lost).  ReRcv counter
shows duplicate packets received (Ack was lost on its way back to sender). 
Both indicate packet-loss problems in the LAN. 

SHAD.COM

Displays shadowset members which are in a shadow merge or shadow copy state, if
any. 

SHAD_TRACK.COM

Periodically displays shadowset members which are in a shadow merge or shadow
copy state, and degree of completion, to allow tracking copy or merge progress
over time.

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

SHOW_PREF_PATH.COM

Show the preferred paths being used by PEDRIVER to each remote node in the
cluster.

------------------------------------------------------------------------------
Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
Feel free to direct questions or comments to me, or send me improved versions.

Look for latest updated versions or fixes at:
  http://encompasserve.org/~parris/ or http://www.geocities.com/keithparris/ 
