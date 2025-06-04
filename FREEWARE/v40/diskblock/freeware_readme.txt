DISKBLOCK, UTILITIES, Editor and performance tester for ODS2 ODS5 and Foreign disks

  DISKBLOCK is a low level disk block editing tool.
  It can be used to read, modify and write LBNs on a disk or VBN's in a file.
  It can also carry out read, write and seek performance measurements.

  Diskblock ingores all locking activity on the selected Disk or File, it
  can access and modify ODS2 and ODS5 data structures such as file headers 
  and it can read, write and test individual shadow set members independently!

  There is a buffer which holds a copy of the block which you read, 
  you can modify the contents of this buffer using EXAMINE and DEPOSIT
  commands and then REWRITE it to the same block, WRITE it to a different
  block or SELECT a new disk or file and WRITE the buffer to any block.

  When you SELECT a disk, diskblock will attempt to map the Index File
  If it is successful then any command which requires an LBN (READ, WRITE,
  COPY etc...) will also accept a FID and operate on the appropriate file
  header.  This will work even if the disk is mounted /FOREIGN.

  There are commands to SAVE and RESTORE the buffer so that you can keep 
  a copy of the original block and undo changes if they do not work as you
  expect.

  There is a CHECKSUM command to enable you to recalculate the checksums
  for ODS2 and ODS5 file headers and home blocks.

  There is a DIRECTORY command which will find the header for a 
  file (even if the disk is mounted /foreign)

  There is a COPY command which will enable you to treat any block on a 
  disk as a file header and copy the LBNs described by its mapping pointers 
  to a new file (even if the disk is mounted /foreign).
  
  There is a SEARCH command which will search the entire disk/file 
  (or any range of blocks) for a particular string or integer or file
  header.

  There is a TEST command which will carry out performance testing.  You
  may specify queue lengths, I/O sizes, read/write ratios and seek types 
  and Diskblock will report throughput and bandwidth to the disk.

  Because of its potential to corrupt disks you should only
  use Diskblock in extreme desperation or on a disk which you have
  backed up.  
  
  You need LOG_IO privilege to run DISKBLOCK.

  Diskblock has an extensive help file with further information.
