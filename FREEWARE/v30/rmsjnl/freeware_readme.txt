RMS_JRNL_SNAP, UTILITIES, RMS After-Image Journal Snapshot Utility

The RMS_JOURNAL_SNAP utility allows RMS after-image journal files to
be copied and truncated in place with concurrent online access. This
utility allows non-stop application operation where it might otherwise
be unavailable due to having to create new RMS after-image journal
files on a periodic basis.

When using RMS after-image journaling, it is necessary to disconnect
all accessories from any journaled file in order to create a new
journal stream file. This operation may be periodically required as
the after-image journal file grows. For some operations, the downtime
required to create a new after-image journal file prevents non-stop
(24x7) operations that might otherwise be desired.

The RMS_JOURNAL_SNAP utility can be used to snapshot (copy and rewind;
spool) an RMS after-image journal file of a single file.  Snapshots of
a journal file that is used to journal changes in multiple files is
not supported by the RMS_JOURNAL_SNAP utility.  The journal file can be
copied and truncated while it is open and being actively accessed. 
The RMS_JOURNAL_SNAP utility is intended to allow nonstop access to
RMS files that have after-image journaling enabled.
