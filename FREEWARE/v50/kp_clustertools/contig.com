$!
$! CONTIG.COM
$!
$! Command procedure to create or extend a page/swap/dump file so that it
$! is as-contiguous-as-possible despite severe fragmentation of the disk.
$!
$! A regular SYSGEN> CREATE command uses a contiguous-best-try allocation
$! algorithm, which finds the three largest contiguous block ranges on the
$! disk, but then gives up and allocates willy-nilly from there.  If the
$! free space required for a file is more than the three largest free
$! extents on the disk, the CREATE command may fail to allocate the desired
$! amount of free space and still stay within a single file header.  (A
$! file with an extension header cannot be used by primitive boot code,
$! which would prevent use of the file as a page, swap, or dump file.)
$!
$! This procedure takes advantage of this knowledge to create and/or
$! extend a file one extent at a time, so as to maximize the number of
$! blocks per extent and minimize the number of extents in the file
$! (and thus the amount of space taken up in the file header).
$!
$! Each time we extend the file, we extend by just the amount of space
$! in the single largest extent on the disk.  To find out how large
$! the biggest extent on the disk is, we use SYSGEN> CREATE/CONTIGUOUS
$! to create a temporary file, using this in an binary-search fashion
$! until we find the largest number of blocks at which the CREATE succeeds.
$!
$! For best results, run this with minimal or no other file activity on
$! the disk, so that the free-space extent during each loop found is not
$! snarfed up by another user before we can grab it again for the file
$! extension.  (Note that even if that does happen, it probably won't be
$! disastrous, since we'll use the (up to three) next-largest extents
$! before allocating hog-wild, and that will usually add up to at least
$! as much space as the former largest extent occupied.)
$!
$! If the procedure is aborted, the file is left at whatever size we've
$! been able to extend it to so far.
$!
$!      P1:     File specification
$!              If the file already exists, it is extended.
$!
$!      P2:     Desired number of blocks
$!                                              Keith B. Parris, 11/91, 2/96
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$!   http://encompasserve.org/~parris/ or http://www.geocities.com/keithparris/ 
$!
$       old_msg = f$environment("MESSAGE")      !Remember message settings
$       on control_y then goto CLEANUP
$       if p1 .eqs. "" then inquire p1 "File specification"
$       if p2 .eqs. "" then inquire p2 "Desired number of blocks"
$       if f$integer(p2) .le. 0
$       then
$           write sys$output "Error in P2 parameter (number of blocks): ",p2
$           exit
$       endif
$! Figure out where this file is (to be) located
$       device_name = f$parse(p1,,,"device")
$       directory_name = f$parse(p1,,,"directory")
$       devdir = device_name + directory_name
$!!! show symbol devdir         !!!
$	pid = f$getjpi("","PID")
$! Ensure that the area is clean (in case we got interrupted before)
$       if f$search("''devdir'temporary_contig_file.temporary_''pid'") .nes. "" then-
            delete 'devdir'temporary_contig_file.temporary_'pid';*
$       total_size = 0  !Start from scratch, or...
$       if f$search(p1) .nes. ""        !...if the file already exists, ...
$       then                            !...start with its current size
$           total_size = f$file_attributes(p1,"EOF")
$           if total_size .ge. p2
$           then
$               write sys$output -
        "File ''p1' is already at ''total_size' blocks,"
$               write sys$output -
        "so extending it to ''p2' blocks doesn't seem to make sense."
$               exit
$           endif
$       endif
$ LOOP:
$! Check that there's enough free space on the disk
$       space_needed = p2 - total_size
$       if f$getdvi(device_name,"freeblocks") .lt. space_needed
$       then
$           write sys$output "Device ",f$getdvi(device_name,"devnam"),-
        " doesn't have enough free space.  ''space_needed' blocks are needed."
$           goto CLEANUP        !exit
$!!! else       !!!
$!!! write sys$output "Disk ''device_name' appears to have enough free space..."        !!!
$       endif
$! Find the largest free extent on the disk
$       call find_largest_extent 'space_needed'
$       if .not. $status then goto CLEANUP      !Abort occurred --> quit
$       total_size = total_size + largest_extent
$! Don't need more than they asked for
$       if total_size .gt. p2 then total_size = p2
$! Delete the temporary file, to put its space into the extent cache
$       delete 'devdir'temporary_contig_file.temporary_'pid';*        !!/log
$! And we hope no one snarfs up the contiguous blocks before the next command...
$! Create or extend the file, using only as much space as the largest free
$! extent available on the disk.
$       mcr sysgen create 'p1'/size='total_size'
$       file_size = f$file_attributes(p1,"EOF")
$       if file_size .lt. total_size
$       then    !Some error apparently occurred during the extension
$           write sys$output "File extension to ''total_size' blocks failed."
$           write sys$output "File ''p1' is now at ''file_size' blocks."
$           exit        !Quit
$       endif
$       total_size = file_size
$       write sys$output "File ''p1' is now at ''total_size' blocks..."
$       write sys$output "File extent information:"
$       dump/header/block=count=0/output=temporary_headers.temporary_'pid' 'p1'
$       search temporary_headers.temporary_'pid' "LBN:","COUNT:"/match=and
$       delete temporary_headers.temporary_'pid';*
$! If more is needed, go get the next largest free extent
$       if total_size .lt. p2 then goto LOOP
$       write sys$output "File extension work is all done."
$ CLEANUP:
$       write sys$output "Cleaning up..."
$       set message'old_msg'
$       if f$search("''devdir'temporary_contig_file.temporary_''pid'") .nes. "" then-
                delete 'devdir'temporary_contig_file.temporary_'pid';*
$       exit
$!
$! Subroutine to find the largest existing extent of free space on the
$! disk, up to a desired maximum.
$!
$! Once found, allocate and keep around a temporary file of that size
$!
$!      P1:     Maximum file size
$!
$ FIND_LARGEST_EXTENT: subroutine
$       on control_y then goto ABORT
$ RESTART:      !Come here if the final re-create fails due to other
$               !activity on the disk
$! Do a binary search for the highest number of free blocks we can find
$       high = p1 + 1   !Upper limit (1 more than highest possible)
$       low = 1         !Lower limit
$       highest = 1     !Biggest contiguous extent we've found so far
$       mid = p1        !Be optimistic: try for full size 1st time
$ F_L_E_LOOP:
$! Don't confuse the user with error messages from SYSGEN for these
$! trial allocations
$       set message/noidentification/noseverity/nofacility/notext
$       mcr sysgen -
create/contiguous/size='mid' 'devdir'temporary_contig_file.temporary_'pid'
$       set message'old_msg'    !Re-enable error messages
$! In 5.5-2, at least, SYSGEN may just give the following warning:
$!   %SYSGEN-W-INSFCONTIG, insufficient contiguous space; file allocated
$!      non-contiguously
$!   %SYSGEN-I-PRTEXT, file only partly extended. Volume may be too fragmented
$! despite the inclusion of the /CONTIGUOUS qualifier on the CREATE command.
$! In this case, we'll have to detect the problem by noticing that the file is
$! not of the size desired, or of the size desired but not actually contiguous
$!
$       temp = f$search("''devdir'temporary_contig_file.temporary_''pid'")
$       if temp .nes. ""
$       then    !Creation was successful, but check that file out...
$           tempsize = -        !Check the file size
f$file_attributes("''devdir'temporary_contig_file.temporary_''pid'","ALQ")
$!!! show symbol tempsize       !!!
            tempctg = -         !Check if it's contiguous
f$file_attributes("''devdir'temporary_contig_file.temporary_''pid'","CTG")
$!!! show symbol tempctg        !!!
$           if (tempsize .ge. mid) .and. tempctg
$           then        !We got a contiguous file of at least as big as we wanted
$!!! write sys$output "Temp file of ''tempsize' contiguous blocks created" !!!
$               highest = mid   !Remember this as the highest so far
$! Get rid of the just-created file to make room for a possibly-larger one...
$               low = mid       !Try higher next time
$               if high .gt. low + 1    !If we're not yet at the end...
$               then                    !...delete the temporary file.
$                   delete 'devdir'temporary_contig_file.temporary_'pid';*    !!/log
$               endif
$           else      !Temp file created, but not big enough or not contiguous
$!!!  if mid .ne. tempsize then -                                               !!!
$!!!!write sys$output "Temp file was ''tempsize' blocks; we wanted ''mid' blocks" !!!
$!!!  if .not. tempctg then -                                           !!!
$!!!!write sys$output "Temp file was not contiguous"                    !!!
$               high = mid      !Try a smaller size next time
$               delete 'devdir'temporary_contig_file.temporary_'pid';*        !!/log
$           endif
$       else
$!!!  write sys$output "Failed to create temp file of ''mid' blocks"    !!!
$           high = mid  !Didn't make it; try lower next time
$       endif
$       if high .gt. low + 1
$       then
$           mid = low + (high-low)/2    !Try half-way in between next time,
$           goto F_L_E_LOOP             !since this is a binary search
$       endif
$! If there isn't a temporary file hanging around, try to re-create one with
$! as many blocks as the largest one we were able to make
$       if f$search("''devdir'temporary_contig_file.temporary_''pid'") .eqs. ""
$       then
$! Re-create the temporary file with the largest size we were successful at
$           set message/noident/noseverity/nofacility/notext    !Stealth mode
$           mcr sysgen-
 create/contiguous/size='highest' 'devdir'temporary_contig_file.temporary_'pid'
$           set message'old_msg'        !Re-enable error messages
$           temp = f$search("''devdir'temporary_contig_file.temporary_''pid'")
$! If we weren't able to recreate the temporary file, someone must have grabbed
$! the blocks.  If that happens, we shrug it off and go try again. 
$           if temp .eqs. "" then goto RESTART
$       endif
$ if highest .eq. p1                                            !!
$ then                                                          !!
$  write sys$output "Found ''highest' contiguous blocks"        !!
$ else                                                          !!
$  write sys$output "Largest free extent found was ",highest    !!
$ endif                                                         !!
$       largest_extent == highest
$       exit 1  !Exit with normal status
$ ABORT:
$       write sys$output "Aborting..."
$! Keep $EXIT 0 from producing a noname/nomesage error message
$       set message/noidentification/noseverity/nofacility/notext
$       exit 0  !Exit and signal that we were aborted
$       endsubroutine
