Safety -

* An undelete that WORKS (changes VMS so delete means move to wastebasket)
* A super security system (hide files, softlink support, file passwords,
			SAFE net browsing, etc.) that does the kinds of things
			known in mainframes with RACF, Top Secret, etc., and
			more...
* A Free HSM
* Fragmentation avoidance (prevents ~98% of fragmentation from happening!)


SAFETY - Comprehensive undelete (changes delete to put files in a wastebasket
	and allows user undelete, and monitors space so it doesn't run out),
	very extensive security, and HSM style space protection features.

License:
  Safety is Copyright (C) 1994-1997 Glenn C. Everhart, All Rights
Reserved. It may be distributed freely provided this copyright
notice, unaltered, accompanies the distribution and provided the
complete distribution is supplied.

  Safety may be used by people running VMS for hobby or non-profit
functions, free of charge. Commercial users (classed as anyone using
VMS in a profit making activity) may use the security and storage
management (HSM) capabilities of Safety on a single disk free of
charge, and may use all capabilities for a period of sixty (60) days
from first Safety installation free of charge. Should they continue to
use Safety after that period and/or for more than one disk, they must
pay a charge of $499 per Alpha CPU, $299 per VAX CPU, to Glenn
Everhart, 18 Colburn Ln, Hollis, NH 03049. They will then be entitled
to limited email support and to receive a version which will emit fewer
messages. Full support requires separate negotiations.  Commercial
users includes government users also, at any level of government, and
it includes any nonprofit organizations (except DECUS) which employ
people full time.

 Educational institutions (elementary or high schools or colleges
or universities granting 4 year degrees) may license the product
for $199 per CPU.

  The version of Safety here is a complete and full function product
which has no expiration date, but which will send a few operator
messages now and then reminding you of its existence. It is
released as "shareware" in spite of the fact it was developed as
a for-profit activity, in the hope that it will be registered.

  I reserve the right to enforce the limitations of legal action
against commercial users who violate the above license. Nonprofit
users may freely use Safety with my blessings. For profit users
cannot however claim poverty.

If you have contributed software to the VAX SIG tapes (or VMS SIG
tapes or L&T SIG Tapes), you have special permission to use Safety
free of charge for your individual use. The same exemption is
made for those who have given sessions at a DECUS symposium. The
individual use by such people includes permission for their use in
consulting or business ventures they have personally (though it
does not cover others who may work at the same businesses...sorry,
I have to limit this somewhere.) Contributed means either something
you submitted or something you wrote has been on the sigtapes.

Payment should be made by check or money order to me at the address
above. Companies wishing site licenses should contact me.


	Glenn C. Everhart
	Everhart@Arisia.GCE.Com

This version will also support space management commands, softlinks, 
disk filling detection, undeletion, and complete system security. In short,
this is the full Safety product.

A few selected source files are provided also, as instructional material
in how the I/O intercept is accomplished.

With the functions provided here, you can see that VMS acquires
security features nothing else has. You are best advised to use the
manual to be sure you understand the cautions here. However, you will
find that many problems which were impossible before, or could be
solved in only the most arcane fashions, become simple to handle.

The storage management (including freespace handling) can save considerable
money. Now you can have shelving/unshelving and even storage in compressed
form with automatic decompression, free.

The deletion protetion here is the first undelete that really WORKS in
VMS. It does so by changing the semantics of DELETE (actually, of the
IO$_DELETE function) to mean "send the file to my delete agent" which
can then put it in a wastebasket. As long as you have space monitoring
turned on, file create or extend will not fail for lack of space, if
deleting files that have been in the wastebasket a while can make room.
This permits you to run delete protection in "set it and forget it" mode.

Enjoy.

Glenn C. Everhart
18 June 1997



DELETE-PROTECT.DOC and EACF_USER_MAN.TXT are separate and slightly older
descriptions of the security and deletion protection features.

The SPD follows.
================================================================



          Software
          Product
          Description

          Safety V1.3a
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
          all times.

          Safety gives you a full complement of tools  for dealing with
          space issues automatically according to your site policy. These
          facilities are safe and easily understood.  A comprehensive
          utility is provided by which you set your site policy to select
          which files are and are not eligible for automatic shelving.
          Also you are provided with screen oritented utilities for
          selecting files to shelve at any time. Access to the shelved
          files of course causes unshelving if the normal shelving-by-copy
          mode is used. Also, a simple set of rules permit locating
          shelved or softlink target files at any time, even without
          Safety running.   Safety at no time invalidates your file
          structures for normal VMS access...not even for an instant.

          In addition Safety contains functions to speed file access and
          inhibit disk fragmentation.

See AAAREADME.1ST for more complete information.
