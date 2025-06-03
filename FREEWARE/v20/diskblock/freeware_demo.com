$ type /page sys$input
$ DECK/DOLLARS="EOF"

--------------------------------------------------------------------------------

     In this example DISKBLOCK is used to copy a file X.EXT from a
     disk with a corrupted home block that cannot be mounted.

$ MC DISKBLOCK


                           DISKBLOCK V5.4
  Copyright (c) Digital Equipment Corp.  1994.  All rights Reserved.

    This software can and WILL corrupt disks if it is mis-used,
    use it with care and back everything up before you start.

                      Type HELP for instructions


$ MOUNT /FOREIGN $1$DUA42 /NOWRITE
DISKBLOCK> SELECT $1$DUA42 /NOMAP /NOWRITE
DISKBLOCK> SEARCH /HEADER=X.EXT /FULL
%DSKB-I-SEARCHING, Searching blocks 0 - 2376152 for HEADER "X.EXT;*" at 13:23:45
LBN: 1193605, Filename: X.EXT;1
  FID: 5260, SEGNUM: 0, CREDATE: 14-MAR-1989 14:59:13.86

Dump of file HOME:[TEMP]DISKBLOCK.TMP;1 on 12-APR-1995 09:52:14.26
File ID (31659,48,0)   End of file block 1 / Allocated 33

Virtual block number 1 (00000001), 512 (0200) bytes

Header area
    Identification area offset:           40
    Map area offset:                      100
    Access control area offset:           231
    Reserved area offset:                 255
    Extension segment number:             0
    Structure level and version:          2, 1
    File identification:                  (5260,1,0)
    Extension file identification:        (0,0,0)
    VAX-11 RMS attributes
        Record type:                      Variable
        File organization:                Sequential
        Record attributes:                Non-spanned
        Record size:                      512
        Highest block:                    54
        End of file block:                44
        End of file byte:                 0
        Bucket size:                      0
        Fixed control area size:          0
        Maximum record size:              512
        Default extension size:           0
        Global buffer count:              0
        Directory version limit:          0
    File characteristics:                 <none specified>
    Map area words in use:                2
    Access mode:                          0
    File owner UIC:                       [RANCE]
    File protection:                      S:RWE, O:RWE, G:RE, W:E
    Back link file identification:        (4,4,0)
    Journal control flags:                <none specified>
    Active recovery units:                None
    Highest block written:                54

Identification area
    File name:                            X.EXT;1
    Revision number:                      6
    Creation date:                        14-MAR-1989 14:59:13.86
    Revision date:                        16-JAN-1995 15:15:24.20
    Expiration date:                      13-APR-1995 14:13:40.28
    Backup date:                          20-JUN-1989 11:15:39.27

Map area
    Retrieval pointers
        Count:        214        LBN:      113124
	Count:         24        LBN:       10456

Checksum:                                   11611
%DSKB-I-SEARCHDONE, Search completed at 13:44:10
... Check that this is the correct file header.......
DISKBLOCK> COPY 1193605 /OUTPUT=SYS$SCRATCH:X.EXT
%DSKB-I-CREATED, Created file DUA0:[RANCE]X.EXT;1
%DSKB-I-COPYING, Copying 214 blocks starting at LBN 113124
%DSKB-I-COPYING, Copying 24 blocks starting at LBN 10456
%DSKB-I-CPYATTRIB, File attributes successfully copied to DUA0:[RANCE]X.EXT;1

--------------------------------------------------------------------------------

  In this example a backup copy of the home block is used to repair a
  corrupted primary home block.

  $ RUN SYS$SYSTEM:DISKBLOCK

                            DISKBLOCK V5.4
      Copyright (c) Digital Equipment Corp.  1994.  All rights Reserved.

DISKBLOCK> MOUNT /FOREIGN $2$DUA22
DISKBLOCK> SELECT $2$DUA22               ; Select the problem drive
DISKBLOCK> READ 2                        ; Read backup copy of Homeblock
DSKB-I-BLKREAD, Block 2 (%X00000002) of _$2$DUA22 successfully read
DISKBLOCK> HELP ODS2 HOME HOMELBN
..........................               ; Find size and offset of field
DISKBLOCK> DEPOSIT 0 1                   ; New HOMELBN
DISKBLOCK> DEPOSIT/WORD 16 2             ; New HOMEVBN
DISKBLOCK> CHECKSUM /HOME /DEPOSIT       ; Modify checksums
%DSKB-I-CHKCHANGED, Checksum has been changed from %X289B to %X3F9B
%DSKB-I-CHKCHANGED, Checksum has been changed from %X5136 to %X7F36
DISKBLOCK> DUMP /HOME                    
..........................		 ; See if it looks OK
DISKBLOCK> WRITE 1                       ; Write to primary homeblock
%DSKB-I-BLKMODIF, Block 1 (%X00000001) of _$2$DUA22 successfully modified
DISKBLOCK> EXIT
$
EOF
$ exit
