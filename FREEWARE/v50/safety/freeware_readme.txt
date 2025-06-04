SAFETY, UTILITIES, Comprehensive Data Safety

See SafetySPD.Txt for additional details.

Documentation is included in the VMSINSTAL kit.

	-=-

"Safety V1.5
Comprehensive Data Safety for your VMS systems.

from General Cybernetic Engineering

Executive Summary:

There are many perils your data faces, and loss of data can cost
time, money, and jobs. Intruders, disgruntled insiders, or
hidden flaws in installed software can destroy records. What is
more, mistaken losses occur constantly.

Safety protects your system and your critical data in three
ways:

1. A comprehensive security system adds extra checks for access
to VMS files so that access by intruders or by people in
non-job-required ways can be regulated or prevented. This allows
your business - critical data to finally be protected against
misuse, tampering, or abuse. Access from programs doing
background dirty work (viruses, Trojans, worms, and the like, or
even programs with security holes which can be exploited
remotely (like Java browsers)) can also be blocked without
damaging normal use. This active protection works three ways: by
checking integrity    of your files against tampering, by
preventing of  untrusted images from  gaining privilege, and by
regulating what other parts of the system an image may access.

2. A deletion protection system provides a way to undelete files
which were deleted by mistake and to optionally copy deleted
files to backup facilities before removal. Unlike all other VMS
"undelete" programs on the market, this facility does not rely
on finding the disk storage that contained the file and
reclaiming it before it is overwritten. Rather, it changes the
semantics of the file system delete to use a "wastebasket"
system and captures the file intact. Thus, this system works
reliably. No others do. This facility is also useful where you
have a requirement to keep all files of a certain set of types,
since the backup function can be used to capture such files
while permitting otherwise normal system function. The shelving
or linking functions are also available for moving copies
offline if this is desired. The  Safety protection features are
fully integrated with the DPS subsystem, so that deletion
protection does not involve destroying file security.

3.  When space runs out, hasty decisions about what to keep
online often must be made, and the risk of accidentally losing
something important is high.   Safety protects you from running
out of space. Space can be monitored and older items in the
wastebasket deleted if it is becoming low, without manual
intervention. In addition, Safety  is able to "shelve" files so
that they are stored anywhere else desired on your system, and
they are brought back automatically when accessed. Thus no
manual arrangements need be made for reloading them.  Safety can
also keep the files on secondary storage, keeping a "soft link"
to the files at their original site so they will be accessed on
the secondary storage instead. Also,    Safety can store files
compressed, or can store them on secondary storage so that read
access is done on the secondary storage, but write access causes
the file to be copied back to its original site. Standard VMS
utilities are used for all file movement, and moved files are
also directly accessible in their swapped sites with standard
VMS utilities. The VMS file system remains completely valid at
all times."

...

