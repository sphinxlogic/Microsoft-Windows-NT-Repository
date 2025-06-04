JPI, UTILITIES, Selective Listing Of Processes Across Clusters And Wan's

  JPI is an OpenVMS wide-area network tool which is used to create a one
  line per process static information display for all processes matching
  specified selection criteria, displaying the output according to display
  criteria. JPI utilizes the $process_scan and $getjpi OpenVMS System
  Services to accomplish the scanning. Scanning can be targeted to a local
  node, a remote node, a local OpenVMS Cluster or a remote OpenVMS cluster
  or any combination of these. JPI V2.1 will execute on OpenVMS VAX V5.3
  and later and OpenVMS AXP V6.1 and later.

  This utility uses the DCLTABLES CLI for a command named JPI. JPI is
  NOT a DEC registered command and therefore may, in the future,
  conflict with a DEC registered DCL command.

  The kit is in a VMSINSTAL saveset. Sources are contained within the
  kit. The kitinstal self-detects VAX or AXP and builds the appropriate
  images.

  The following is a sample display output:

  $ JPI/USER=SYSTEM

                    Job/Process Information from VAX/VMS T6.1-5Z0
             From node MYNODE (VAXstation 3100/GPX) at 22-MAR-1994 15:42:41.73

PID      Term   Username      Process Name  Image Name  PriB Work State M Node
--------------------------------------------------------------------------------
00000085         SYSTEM...... IPCACP....... IPCACP......  8  2052 HIB   O MYNODE
00000086         SYSTEM...... ERRFMT....... ERRFMT......  7   516 HIB   O MYNODE
00000087         SYSTEM...... OPCOM........ OPCOM.......  6   516 HIB   O MYNODE
00000089         SYSTEM...... JOB_CONTROL.. JBC$JOB_CONT  8  1028 HIB   O MYNODE
0000008A         SYSTEM...... QUEUE_MANAGER QMAN$QUEUE_M  8  1482 HIB   O MYNODE
0000008B         SYSTEM...... SECURITY_SERV SECURITY_SER  8  1628 HIB   O MYNODE
0000008C         SYSTEM...... TP_SERVER.... TPSERV......  8  2052 HIB   O MYNODE
00000090         SYSTEM...... REMACP....... REMACP......  8   516 HIB   O MYNODE
00000092         SYSTEM...... DQS$NOTIFIER. DQS$NOTIFIER  4   516 LEF   O MYNODE
00000094         SYSTEM...... SYMBIONT_2... DQS$SMB.....  4  1032 HIB   O MYNODE
00000095         SYSTEM...... SYMBIONT_3... DQS$SMB.....  4  1032 HIB   O MYNODE
00000096         SYSTEM...... LATACP....... LATACP...... 12   666 HIB   O MYNODE
00000098         SYSTEM...... SYMBIONT_5... DCPS$SMB....  4  1032 HIB   O MYNODE
00000099         SYSTEM...... SYMBIONT_6... DCPS$SMB....  4  1032 HIB   O MYNODE
0000009F         SYSTEM...... DNS$ADVER.... DNS$ADVER...  4   516 HIB   O MYNODE
000000A2         SYSTEM...... NSCHED....... NSCHED......  6  1716 LEF   O MYNODE
000000A5         SYSTEM...... SCHED_REMOTE. SCHED_REMOTE  3   812 LEF   O MYNODE
000000A9         SYSTEM...... SYMBIONT_7... VTXWPSSMB...  4   516 HIB   O MYNODE
000000AA         SYSTEM...... VTXSRV....... VTXSRV......  6  1112 HIB   O MYNODE
000000AB         SYSTEM...... VTX$WBSRV.... VTX$WBSRV...  6  1112 HIB   O MYNODE
000001AC         SYSTEM...... SYMBIONT_8... DCPS$SMB....  4  2682 HIB   O MYNODE
000001CF         SYSTEM...... SYMBIONT_9... PRTSMB......  4  1032 HIB   O MYNODE

Total of 22 Processes using 24598 pages (12 mb) of physical memory
 
  Please read the release notes on JPI for information on qualifiers allowed on
  the JPI command line.

  This utility was written using COBOL simply because this was the 
  developers language of choice and not necessarily the most efficient
  coding mechanism.

  Files modified or created by the supplied installation kit are as
  follows:

  	SYS$LIBRARY:DCLTABLES.EXE     		updated
  	SYS$SYSTEM:JPI.EXE			created
  	SYS$HELP:HELPLIB.HLB   	       		updated
  	JPI.COB					created (optionally)
	SYS$MANAGER:JPI012.RELEASE_NOTES	created

