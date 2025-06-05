RMS_LOCKS, Miscellaneous, Shows all and/or blocked locks for RMS files and records

VMS version:  VAX: 7.2  Alpha: 7.2-1
Language   :  VAX/DEC Basic

This program will scan the DLM database capturing all locks for RMS files
and records.  It will then traverse the data displaying ALL locks, or only
BLOCKed locks.

NOTE: The program requires CMEXEC privs!  Maybe others.

Development of the program derived from another utility, and identification
of RMS file and record locks was done purely on a 'discovery' basis, not
referencing the VMS documentation.  Lock values could change in future
versions of VMS and the program would be rendered unusable.

The Alpha version of the program has one 'hack' (means I don't really
understand why there is a difference) to address differences in device
naming on Alpha.  I'm pretty sure neither version would work in a cluster
environment, I don't have a cluster to perform any testing.

Some selections to limit the volumn of data displayed would be useful
enhancements.  I don't plan any since I don't have much use for the
utility.

Also included is DLM_LOCKS.BAS, a tool used in the research for
implementing RMS_LOCKS.
