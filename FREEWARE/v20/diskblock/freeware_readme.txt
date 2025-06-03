DISKBLOCK, UTILITIES, Editor for raw LBNs and VBNs on ODS2 and Foreign disks

    DISKBLOCK is a low level disk block editing tool which can be used to 
    read, modify and write LBNs on a disk or VBN's in a file.

    Diskblock is able to read and write Foreign and ODS2 mounted disks,
    ignoring all locking requirements.  It can be used to repair disks 
    which have been made unreadable by hardware and/or software errors.  
    It can also copy files from disks that can only be mounted /FOREIGN.

    There are commands to SAVE and RESTORE blocks so that you can keep
    a copy of parts of a disk and undo changes if they do not work as you
    expect.

    There is a CHECKSUM command to enable you to recalculate the checksums
    for ODS2 file headers and home blocks.

    There is a DIRECTORY command which will find the header for a
    file (even if the disk is mounted /foreign)

    There is a COPY command which will enable you to treat any block on a
    disk as a file header and copy the LBNs described by its mapping pointers
    to a new file (even if the disk is mounted /foreign).
   
    There is a SEARCH command which will search the entire disk/file
    (or any range of blocks) for a particular string or integer or file
    header.  This search usually runs at the spiral transfer rate of the disk.

    Because of its potential to corrupt disks you should only
    use Diskblock in extreme desperation or on a disk which you have
    backed up.
   
    You need LOG_IO privilege to run DISKBLOCK.

    Type HELP OVERVIEW or HELP EXAMPLES at the DISKBLOCK> prompt for 
    more information.
