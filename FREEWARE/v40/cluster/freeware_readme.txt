CLUSTER_TOOLS, SYSTEM_MANAGEMENT, Unsupported Cluster management tools/hacks
 
BYEBYENODE.MAR
 
  This is a quick hack to force a remote system out of the cluster. It can
  be used when a remote system hangs for example at high IPL (bewteen 3
  and 6), or when the SMIserver does not respond.

  It uses an internal routine in the connection manager. There is one
  problem with this approach, and that is that this routine (CNX$DISC_REMOVE)
  is not available in any symboltable which we can include in the link.
  So what we do is use MATCHC to find the sequence of instructions around
  the place where we expect the code, and issue an error if we can't find it.
  If we got the address, we JSB to it and let that routine do the dirty work.
  The net result will be that the remote node will CLUEXIT.

  This is strictly a HACK, and as such it may fail at any time. Use at
  your own risk. (I included a check to prevent removal of the node from
  which this program runs, although it was fun to get a system in a state
  in which the only clusternode was removed from the cluster and in longbreak,
  without quorum....). It was tested on VMS V5.5 thru T6.2-FT1.
 
COPY_SHADOW_DUMP.MAR
 
  This program is created to retrieve a system dump from a Phase II
  shadowed systemdisk. The problem is that the bugcheck code writes
  to the member we booted from (normally), but as far as shadowing
  is concearned the disks are the same. A merge copy which may start
  after a crash may not be able to correct the blocks of the dumpfile
  on all the disks of the shadowset. Suppose that a dumpfile starts
  at the end of the disk, and ends at the beginning of it. When a
  system crashes we will start writing at the end of the disk, and
  it is very well possible that when we reach the part of the dumpfile
  at the beginning of the disk that that part has already been merged
  by a merge copy. The net result is inconsistent data as far as the
  dumpfile is concearned. This can of course be corrected by dismounting
  members of the shadowset except the master, and remount the members
  after which a full shadowcopy will correct things. This may have a
  rather high impact, which most customers don't want on their live
  systems. Another way to correct things is to trigger a merge copy on
  the fly. That's certainly possible but it involves kernel-mode hacking
  which is always error-prone. What we do here is look for the retrieval
  pointers of the dumpfile (they are already stored in memory by SYSINIT)
  and look which shadowset member is the master. We then copy all blocks
  mapped by the dumpfile from the master member to an output file.

  This version supports both the Alpha- and VAX architectures.
 
FORCE_MERGE.MAR
 
  This program is created as a hack to force a merge copy on a
  Phase II shadowset. It uses a routine inside the SHdriver for the
  dirty work. We call this entrypoint with the correct parameters.
  We also disable writelogging because we want a full merge copy to occur.
